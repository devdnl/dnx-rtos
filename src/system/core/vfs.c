/*=========================================================================*//**
@file    vfs.c

@author  Daniel Zorychta

@brief   This file support virtual file system

@note    Copyright (C) 2012, 2013 Daniel Zorychta <daniel.zorychta@gmail.com>

           This program is free software; you can redistribute it and/or modify
           it under the terms of the GNU General Public License as published by
           the  Free Software  Foundation;  either version 2 of the License, or
           any later version.

           This  program  is  distributed  in the hope that  it will be useful,
           but  WITHOUT  ANY  WARRANTY;  without  even  the implied warranty of
           MERCHANTABILITY  or  FITNESS  FOR  A  PARTICULAR  PURPOSE.  See  the
           GNU General Public License for more details.

           You  should  have received a copy  of the GNU General Public License
           along  with  this  program;  if not,  write  to  the  Free  Software
           Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.


*//*==========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Include files
==============================================================================*/
#include "core/vfs.h"
#include "core/list.h"
#include "core/sysmoni.h"
#include "kernel/kwrapper.h"
#include <errno.h>

/*==============================================================================
  Local symbolic constants/macros
==============================================================================*/
/* wait time for operation on VFS */
#define MTX_BLOCK_TIME                          10
#define force_lock_recursive_mutex(mtx)         while (recursive_mutex_lock(mtx, MTX_BLOCK_TIME) != MUTEX_LOCKED)

#define first_character(str)                    str[0]
#define last_character(str)                     str[strlen(str) - 1]

/*==============================================================================
  Local types, enums definitions
==============================================================================*/
/** file type */
struct vfs_file
{
        void      *FS_hdl;
        stdret_t (*f_close)(void *FS_hdl, void *extra_data, fd_t fd, bool force, const task_t *opened_by_task);
        size_t   (*f_write)(void *FS_hdl, void *extra_data, fd_t fd, const u8_t *src, size_t count, u64_t *fpos);
        size_t   (*f_read )(void *FS_hdl, void *extra_data, fd_t fd, u8_t *dst, size_t count, u64_t *fpos);
        stdret_t (*f_ioctl)(void *FS_hdl, void *extra_data, fd_t fd, int iorq, void *args);
        stdret_t (*f_stat )(void *FS_hdl, void *extra_data, fd_t fd, struct vfs_stat *stat);
        stdret_t (*f_flush)(void *FS_hdl, void *extra_data, fd_t fd);
        void      *f_extra_data;
        fd_t       fd;
        u64_t      f_lseek;
        int        f_errflag;
};

struct FS_data {
        char                          *mount_point;
        struct FS_data                *base_FS;
        void                          *handle;
        struct vfs_FS_interface        interface;
        u8_t                           mounted_FS_counter;
};

enum path_correction {
        ADD_SLASH,
        SUB_SLASH,
        NO_SLASH_ACTION,
};

/*==============================================================================
  Local function prototypes
==============================================================================*/
static int             file_mode_str_to_flags   (const char *str);
static struct FS_data *find_mounted_FS          (const char *path, u16_t len, u32_t *itemid);
static struct FS_data *find_base_FS             (const char *path, char **extPath);
static char           *new_corrected_path       (const char *path, enum path_correction corr);

/*==============================================================================
  Local object definitions
==============================================================================*/
static list_t  *vfs_mnt_list;
static mutex_t *vfs_resource_mtx;
static u32_t    vfs_id_counter;

/*==============================================================================
  Function definitions
==============================================================================*/

//==============================================================================
/**
 * @brief Initialize VFS module
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
stdret_t vfs_init(void)
{
        vfs_mnt_list     = list_new();
        vfs_resource_mtx = recursive_mutex_new();

        if (!vfs_mnt_list || !vfs_resource_mtx)
                return STD_RET_ERROR;
        else
                return STD_RET_OK;
}

//==============================================================================
/**
 * @brief Function mount file system in VFS
 *
 * @param[in] *src_path         path to source file when file system load data
 * @param[in] *mount_point      path when dir shall be mounted
 * @param[in] *fs_interface     pointer to description of mount interface
 *
 * @retval STD_RET_OK           mount success
 * @retval STD_RET_ERROR        mount error
 */
//==============================================================================
stdret_t vfs_mount(const char *src_path, const char *mount_point, struct vfs_FS_interface *fs_interface)
{
        if (!mount_point || !fs_interface || !src_path) {
                return STD_RET_ERROR;
        }

        char *cwd_mount_point = new_corrected_path(mount_point, ADD_SLASH);
        if (!cwd_mount_point) {
                return STD_RET_ERROR;
        }

        char *cwd_src_path = new_corrected_path(src_path, SUB_SLASH);
        if (!cwd_src_path) {
                sysm_sysfree(cwd_mount_point);
                return STD_RET_ERROR;
        }

        force_lock_recursive_mutex(vfs_resource_mtx);
        struct FS_data *mount_fs = find_mounted_FS(cwd_mount_point, -1, NULL);

        char *external_path      = NULL;
        struct FS_data *base_fs  = find_base_FS(cwd_mount_point, &external_path);

        /*
         * create new FS in existing DIR and FS, otherwise create new FS if
         * first mount
         */
        struct FS_data *new_fs = NULL;

        if (base_fs && mount_fs == NULL) {
                if (base_fs->interface.fs_opendir && external_path) {

                        DIR dir;
                        if (base_fs->interface.fs_opendir(base_fs->handle,
                                                         external_path,
                                                         &dir) == STD_RET_OK) {

                                new_fs = sysm_syscalloc(1, sizeof(struct FS_data));
                                base_fs->mounted_FS_counter++;
                                dir.f_closedir(dir.f_handle, &dir);
                        }
                }
        } else if (  list_get_item_count(vfs_mnt_list) == 0
                  && strlen(cwd_mount_point) == 1
                  && first_character(cwd_mount_point) == '/' ) {

                new_fs = sysm_syscalloc(1, sizeof(struct FS_data));
        }

        /*
         * mount FS if created
         */
        if (new_fs && fs_interface->fs_init) {
                new_fs->interface = *fs_interface;

                if (fs_interface->fs_init(&new_fs->handle, cwd_src_path) == STD_RET_OK) {
                        new_fs->mount_point = cwd_mount_point;
                        new_fs->base_FS     = base_fs;
                        new_fs->mounted_FS_counter = 0;

                        if (list_add_item(vfs_mnt_list, vfs_id_counter++, new_fs) >= 0) {
                                recursive_mutex_unlock(vfs_resource_mtx);
                                sysm_sysfree(cwd_src_path);
                                return STD_RET_OK;
                        }
                }

                sysm_sysfree(new_fs);
                sysm_sysfree(cwd_mount_point);
                sysm_sysfree(cwd_src_path);
        }

        recursive_mutex_unlock(vfs_resource_mtx);
        return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief Function unmount dir from file system
 *
 * @param[in] *path             dir path
 *
 * @retval STD_RET_OK           mount success
 * @retval STD_RET_ERROR        mount error
 */
//==============================================================================
stdret_t vfs_umount(const char *path)
{
        if (!path) {
                return STD_RET_ERROR;
        }

        char *cwd_path = new_corrected_path(path, ADD_SLASH);
        if (cwd_path == NULL) {
                return STD_RET_ERROR;
        }

        force_lock_recursive_mutex(vfs_resource_mtx);
        u32_t           item_id;
        struct FS_data *mount_fs = find_mounted_FS(cwd_path, -1, &item_id);
        sysm_sysfree(cwd_path);

        if (mount_fs == NULL) {
                goto vfs_umount_error;
        }

        if (mount_fs->interface.fs_release && mount_fs->mounted_FS_counter == 0) {
                if (mount_fs->interface.fs_release(mount_fs->handle) != STD_RET_OK) {
                        goto vfs_umount_error;
                }

                mount_fs->handle = NULL;

                if (mount_fs->base_FS) {
                        if (mount_fs->base_FS->mounted_FS_counter) {
                                mount_fs->base_FS->mounted_FS_counter--;
                        }
                }

                if (mount_fs->mount_point) {
                        sysm_sysfree(mount_fs->mount_point);
                }

                if (list_rm_iditem(vfs_mnt_list, item_id) == STD_RET_OK) {
                        recursive_mutex_unlock(vfs_resource_mtx);
                        return STD_RET_OK;
                }
        }

        vfs_umount_error:
        recursive_mutex_unlock(vfs_resource_mtx);
        return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief Function gets mount point for n item
 *
 * @param[in]   item            mount point number
 * @param[out] *mntent          mount entry data
 *
 * @retval STD_RET_OK           mount success
 * @retval STD_RET_ERROR        mount error
 */
//==============================================================================
stdret_t vfs_getmntentry(size_t item, struct vfs_mntent *mntent)
{
        if (mntent) {
                force_lock_recursive_mutex(vfs_resource_mtx);
                struct FS_data *fs = list_get_nitem_data(vfs_mnt_list, item);
                recursive_mutex_unlock(vfs_resource_mtx);

                if (fs) {
                        struct vfs_statfs stat_fs = {.f_fsname = NULL};

                        if (fs->interface.fs_statfs) {
                                fs->interface.fs_statfs(fs->handle, &stat_fs);
                        }

                        if (stat_fs.f_fsname) {
                                if (strlen(fs->mount_point) > 1) {
                                        strncpy(mntent->mnt_dir, fs->mount_point,
                                                strlen(fs->mount_point) - 1);
                                } else {
                                        strcpy(mntent->mnt_dir, fs->mount_point);
                                }

                                strcpy(mntent->mnt_fsname, stat_fs.f_fsname);
                                mntent->free  = (u64_t)stat_fs.f_bfree  * stat_fs.f_bsize;
                                mntent->total = (u64_t)stat_fs.f_blocks * stat_fs.f_bsize;

                                return STD_RET_OK;
                        }
                }
        }

        return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief Function create node for driver file
 *
 * @param[in] *path                 path when driver-file shall be created
 * @param[in] *drvcfg               pointer to description of driver
 *
 * @return zero on success. On error, -1 is returned
 */
//==============================================================================
int vfs_mknod(const char *path, struct vfs_drv_interface *drv_interface)
{
        if (!path || !drv_interface) {
                return -1;
        }

        char *cwd_path = new_corrected_path(path, NO_SLASH_ACTION);
        if (!cwd_path) {
                return -1;
        }

        force_lock_recursive_mutex(vfs_resource_mtx);
        char *external_path = NULL;
        struct FS_data *fs  = find_base_FS(cwd_path, &external_path);
        recursive_mutex_unlock(vfs_resource_mtx);

        int status = -1;
        if (fs) {
                if (fs->interface.fs_mknod) {
                        status = fs->interface.fs_mknod(fs->handle, external_path, drv_interface) == STD_RET_OK ? 0 : -1;
                }
        }

        sysm_sysfree(cwd_path);

        return status;
}

//==============================================================================
/**
 * @brief Create directory
 *
 * @param[in] *path                 path to new directory
 *
 * @return 0 on success. On error, -1 is returned
 */
//==============================================================================
int vfs_mkdir(const char *path)
{
        stdret_t status = -1;

        if (path) {
                char *cwd_path = new_corrected_path(path, SUB_SLASH);
                if (!cwd_path) {
                       return -1;
                }

                force_lock_recursive_mutex(vfs_resource_mtx);
                char *external_path = NULL;
                struct FS_data *fs  = find_base_FS(cwd_path, &external_path);
                recursive_mutex_unlock(vfs_resource_mtx);

                if (fs) {
                        if (fs->interface.fs_mkdir) {
                                status = fs->interface.fs_mkdir(fs->handle, external_path) == STD_RET_OK ? 0 : -1;
                        }
                }

                sysm_sysfree(cwd_path);
        }

        return status;
}

//==============================================================================
/**
 * @brief Function open directory
 *
 * @param[in] *path                 directory path
 *
 * @return directory object
 */
//==============================================================================
DIR *vfs_opendir(const char *path)
{
        if (!path) {
                return NULL;
        }

        DIR *dir = sysm_sysmalloc(sizeof(DIR));
        if (dir) {
                stdret_t status = STD_RET_ERROR;

                char *cwd_path = new_corrected_path(path, ADD_SLASH);
                if (cwd_path) {
                        force_lock_recursive_mutex(vfs_resource_mtx);
                        char *external_path = NULL;
                        struct FS_data *fs  = find_base_FS(cwd_path, &external_path);
                        recursive_mutex_unlock(vfs_resource_mtx);

                        if (fs) {
                                dir->f_handle = fs->handle;

                                if (fs->interface.fs_opendir) {
                                        status = fs->interface.fs_opendir(fs->handle, external_path, dir);
                                }
                        }

                        sysm_sysfree(cwd_path);
                }

                if (status == STD_RET_ERROR) {
                        sysm_sysfree(dir);
                        dir = NULL;
                }
        }

        return dir;
}

//==============================================================================
/**
 * @brief Function close opened directory
 *
 * @param[in] *dir                  directory object
 *
 * @return 0 on success. On error, -1 is returned
 */
//==============================================================================
int vfs_closedir(DIR *dir)
{
        if (dir) {
                if (dir->f_closedir) {
                        if (dir->f_closedir(dir->f_handle, dir) == STD_RET_OK) {
                                sysm_sysfree(dir);
                                return 0;
                        }
                }
        }

        return -1;
}

//==============================================================================
/**
 * @brief Function read next item of opened directory
 *
 * @param[in] *dir                  directory object
 *
 * @return element attributes
 */
//==============================================================================
dirent_t vfs_readdir(DIR *dir)
{
        dirent_t direntry;
        direntry.name = NULL;
        direntry.size = 0;

        if (dir->f_readdir) {
                direntry = dir->f_readdir(dir->f_handle, dir);
        }

        return direntry;
}

//==============================================================================
/**
 * @brief Remove file
 * Removes file or directory. Removes directory if is not a mount point.
 *
 * @param[in] *patch                localization of file/directory
 *
 * @return 0 on success. On error, -1 is returned
 */
//==============================================================================
int vfs_remove(const char *path)
{
        if (!path) {
                return -1;
        }

        char *cwd_path = new_corrected_path(path, ADD_SLASH);
        if (!cwd_path) {
                return -1;
        }

        force_lock_recursive_mutex(vfs_resource_mtx);
        char *external_path      = NULL;
        struct FS_data *mount_fs = find_mounted_FS(cwd_path, -1, NULL);
        last_character(cwd_path) = '\0';
        struct FS_data *base_fs  = find_base_FS(cwd_path, &external_path);
        recursive_mutex_unlock(vfs_resource_mtx);

        int status = -1;
        if (base_fs && !mount_fs) {
                if (base_fs->interface.fs_remove) {
                        status = base_fs->interface.fs_remove(base_fs->handle,
                                                              external_path) == STD_RET_OK ? 0 : -1;
                }
        }

        sysm_sysfree(cwd_path);

        return status;
}

//==============================================================================
/**
 * @brief Rename file name
 * The implementation of rename can move files only if external FS provide
 * functionality. Local VFS cannot do this. Cross FS move is also not possible.
 *
 * @param[in] *old_name                  old file name
 * @param[in] *new_name                  new file name
 *
 * @return 0 on success. On error, -1 is returned
 */
//==============================================================================
int vfs_rename(const char *old_name, const char *new_name)
{
        if (!old_name || !new_name) {
                return -1;
        }

        int   status       = -1;
        char *cwd_old_name = new_corrected_path(old_name, NO_SLASH_ACTION);
        char *cwd_new_name = new_corrected_path(new_name, NO_SLASH_ACTION);
        if (!cwd_old_name || !cwd_new_name) {
                goto exit;
        }

        force_lock_recursive_mutex(vfs_resource_mtx);
        char *old_extern_path  = NULL;
        char *new_extern_path  = NULL;
        struct FS_data *old_fs = find_base_FS(cwd_old_name, &old_extern_path);
        struct FS_data *new_fs = find_base_FS(cwd_new_name, &new_extern_path);
        recursive_mutex_unlock(vfs_resource_mtx);

        if (!old_fs || !new_fs || old_fs != new_fs) {
                goto exit;
        }

        if (!old_fs->interface.fs_rename) {
                goto exit;
        }

        status = old_fs->interface.fs_rename(old_fs->handle, old_extern_path,
                                             new_extern_path) == STD_RET_OK ? 0 : -1;

exit:
        if (cwd_old_name) {
                sysm_sysfree(cwd_old_name);
        }

        if (cwd_new_name) {
                sysm_sysfree(cwd_new_name);
        }

        return status;
}

//==============================================================================
/**
 * @brief Function change file mode
 *
 * @param[in] *path         file path
 * @param[in]  mode         file mode
 *
 * @return 0 on success. On error, -1 is returned
 */
//==============================================================================
int vfs_chmod(const char *path, int mode)
{
        if (!path) {
                return -1;
        }

        char *cwd_path = new_corrected_path(path, NO_SLASH_ACTION);
        if (!cwd_path) {
                return -1;
        }

        force_lock_recursive_mutex(vfs_resource_mtx);
        char *external_path = NULL;
        struct FS_data *fs  = find_base_FS(cwd_path, &external_path);
        recursive_mutex_unlock(vfs_resource_mtx);

        int status = -1;
        if (fs) {
                if (fs->interface.fs_chmod) {
                        status = fs->interface.fs_chmod(fs->handle, external_path, mode) == STD_RET_OK ? 0 : -1;
                }
        }

        sysm_sysfree(cwd_path);

        return status;
}

//==============================================================================
/**
 * @brief Function change file owner and group
 *
 * @param[in] *path         file path
 * @param[in]  owner        file owner
 * @param[in]  group        file group
 *
 * @return 0 on success. On error, -1 is returned
 */
//==============================================================================
int vfs_chown(const char *path, int owner, int group)
{
        if (!path) {
                return -1;
        }

        char *cwd_path = new_corrected_path(path, NO_SLASH_ACTION);
        if (!cwd_path) {
                return -1;
        }

        force_lock_recursive_mutex(vfs_resource_mtx);
        char *external_path = NULL;
        struct FS_data *fs  = find_base_FS(path, &external_path);
        recursive_mutex_unlock(vfs_resource_mtx);

        int status = -1;
        if (fs) {
                if (fs->interface.fs_chown){
                        status = fs->interface.fs_chown(fs->handle, external_path, owner, group) == STD_RET_OK ? 0 : -1;
                }
        }

        sysm_sysfree(cwd_path);

        return status;
}

//==============================================================================
/**
 * @brief Function returns file/dir status
 *
 * @param[in]  *path            file/dir path
 * @param[out] *stat            pointer to structure
 *
 * @return 0 on success. On error, -1 is returned
 */
//==============================================================================
int vfs_stat(const char *path, struct vfs_stat *stat)
{
        if (!path || !stat) {
                return -1;
        }

        char *cwd_path = new_corrected_path(path, NO_SLASH_ACTION);
        if (!cwd_path) {
                return -1;
        }

        force_lock_recursive_mutex(vfs_resource_mtx);
        char *external_path = NULL;
        struct FS_data *fs  = find_base_FS(path, &external_path);
        recursive_mutex_unlock(vfs_resource_mtx);

        int status = -1;
        if (fs) {
                if (fs->interface.fs_stat){
                        status = fs->interface.fs_stat(fs->handle, external_path, stat) == STD_RET_OK ? 0 : -1;
                }
        }

        sysm_sysfree(cwd_path);

        return status;
}

//==============================================================================
/**
 * @brief Function returns file system status
 *
 * @param[in]  *path            fs path
 * @param[out] *statfs          pointer to FS status structure
 *
 * @return 0 on success. On error, -1 is returned
 */
//==============================================================================
int vfs_statfs(const char *path, struct vfs_statfs *statfs)
{
        if (!path || !statfs) {
                return -1;
        }

        char *cwd_path = new_corrected_path(path, ADD_SLASH);
        if (!cwd_path) {
                return -1;
        }

        force_lock_recursive_mutex(vfs_resource_mtx);
        struct FS_data *fs = find_mounted_FS(cwd_path, -1, NULL);
        recursive_mutex_unlock(vfs_resource_mtx);
        sysm_sysfree(cwd_path);

        if (!fs) {
                return -1;
        }

        if (!fs->interface.fs_statfs) {
                return -1;
        }

        return fs->interface.fs_statfs(fs->handle, statfs) == STD_RET_OK ? 0 : -1;
}

//==============================================================================
/**
 * @brief Function open selected file
 *
 * @param[in] *name             file path
 * @param[in] *mode             file mode
 *
 * @retval NULL if file can't be created
 */
//==============================================================================
FILE *vfs_fopen(const char *path, const char *mode)
{
        if (!path || !mode) {
                return NULL;
        }

        if (last_character(path) == '/') { /* path is a directory */
                return NULL;
        }

        int flags = file_mode_str_to_flags(mode);
        if (flags == -1) {
                return NULL;
        }

        char *cwd_path = new_corrected_path(path, NO_SLASH_ACTION);
        if (!cwd_path) {
                return NULL;
        }

        FILE *file = sysm_syscalloc(1, sizeof(FILE));
        if (file) {
                force_lock_recursive_mutex(vfs_resource_mtx);
                char *external_path = NULL;
                struct FS_data *fs  = find_base_FS(cwd_path, &external_path);
                recursive_mutex_unlock(vfs_resource_mtx);

                if (fs == NULL) {
                        goto vfs_open_error;
                }

                if (fs->interface.fs_open == NULL) {
                        goto vfs_open_error;
                }

                if (fs->interface.fs_open(fs->handle, &file->f_extra_data,
                                          &file->fd,  &file->f_lseek,
                                          external_path, flags) == STD_RET_OK) {

                        file->FS_hdl  = fs->handle;
                        file->f_close = fs->interface.fs_close;
                        file->f_ioctl = fs->interface.fs_ioctl;
                        file->f_stat  = fs->interface.fs_fstat;
                        file->f_flush = fs->interface.fs_flush;

                        if (strncmp("r", mode, 2) != 0) {
                                file->f_write = fs->interface.fs_write;
                        }

                        if (  strncmp("w", mode, 2) != 0
                           && strncmp("a", mode, 2) != 0) {
                                file->f_read  = fs->interface.fs_read;
                        }

                        sysm_sysfree(cwd_path);
                        return file;
                }

                vfs_open_error:
                sysm_sysfree(file);
        }

        sysm_sysfree(cwd_path);
        return NULL;
}

//==============================================================================
/**
 * @brief Function close old stream and open new
 *
 * @param[in] *name             file path
 * @param[in] *mode             file mode
 * @param[in] *file             old stream
 *
 * @retval NULL if file can't be created
 */
//==============================================================================
FILE *vfs_freopen(const char *name, const char *mode, FILE *file)
{
        if (name || mode || file) {
                if (vfs_fclose(file) == STD_RET_OK) {
                        return vfs_fopen(name, mode);
                }
        }

        return NULL;
}

//==============================================================================
/**
 * @brief Function close opened file
 *
 * @param[in] *file             pinter to file
 *
 * @return 0 on success. On error, -1 is returned
 */
//==============================================================================
int vfs_fclose(FILE *file)
{
        if (file) {
                if (file->f_close) {
                        if (file->f_close(file->FS_hdl, file->f_extra_data, file->fd, false, task_get_handle()) == STD_RET_OK) {
                                sysm_sysfree(file);
                                return 0;
                        }
                }
        }

        return -1;
}

//==============================================================================
/**
 * @brief Function force close opened file (used by system to close all files)
 *
 * @param[in] *file             pinter to file
 * @param[in] *opened_by_task   task which opened file
 *
 * @return 0 on success. On error, -1 is returned
 */
//==============================================================================
int vfs_fclose_force(FILE *file, task_t *opened_by_task)
{
        if (file) {
                if (file->f_close) {
                        if (file->f_close(file->FS_hdl, file->f_extra_data, file->fd, true, opened_by_task) == STD_RET_OK) {
                                sysm_sysfree(file);
                                return 0;
                        }
                }
        }

        return -1;
}

//==============================================================================
/**
 * @brief Function write data to file
 *
 * @param[in] *ptr              address to data (src)
 * @param[in]  size             item size
 * @param[in]  nitems           number of items
 * @param[in] *file             pointer to file object
 *
 * @return the number of items successfully written. If an error occurs, or the
 *         end-of-file is reached, the return value is a short item count (or 0).
 */
//==============================================================================
size_t vfs_fwrite(const void *ptr, size_t size, size_t nitems, FILE *file)
{
        size_t n = 0;

        if (ptr && size && nitems && file) {
                if (file->f_write) {
                        n = file->f_write(file->FS_hdl, file->f_extra_data, file->fd,
                                          ptr, size * nitems, &file->f_lseek);
                        file->f_lseek += (u64_t)n;
                        n /= size;

                        if (n < nitems)
                                file->f_errflag |= VFS_EFLAG_EOF;
                }
        }

        return n;
}

//==============================================================================
/**
 * @brief Function read data from file
 *
 * @param[out] *ptr             address to data (dst)
 * @param[in]   size            item size
 * @param[in]   nitems          number of items
 * @param[in]  *file            pointer to file object
 *
 * @return the number of items successfully read. If an error occurs, or the
 *         end-of-file is reached, the return value is a short item count (or 0).
 */
//==============================================================================
size_t vfs_fread(void *ptr, size_t size, size_t nitems, FILE *file)
{
        size_t n = 0;

        if (ptr && size && nitems && file) {
                if (file->f_read) {
                        n = file->f_read(file->FS_hdl, file->f_extra_data, file->fd,
                                         ptr, size * nitems, &file->f_lseek);
                        file->f_lseek += (u64_t)n;
                        n /= size;

                        if (n < nitems)
                                file->f_errflag |= VFS_EFLAG_EOF;
                }
        }

        return n;
}

//==============================================================================
/**
 * @brief Function set seek value
 *
 * @param[in] *file             file object
 * @param[in]  offset           seek value
 * @param[in]  mode             seek mode
 *
 * @return 0 on success. On error, -1 is returned
 */
//==============================================================================
int vfs_fseek(FILE *file, i64_t offset, int mode)
{
        struct vfs_stat stat;

        if (!file) {
                return -1;
        }

        if (mode == VFS_SEEK_END) {
                stat.st_size = 0;
                if (vfs_fstat(file, &stat) != 0) {
                        return -1;
                }
        }

        switch (mode) {
        case VFS_SEEK_SET: file->f_lseek  = offset; break;
        case VFS_SEEK_CUR: file->f_lseek += offset; break;
        case VFS_SEEK_END: file->f_lseek  = stat.st_size + offset; break;
        default: return -1;
        }

        return 0;
}

//==============================================================================
/**
 * @brief Function returns seek value
 *
 * @param[in] *file             file object
 *
 * @return -1 if error, otherwise correct value
 */
//==============================================================================
i64_t vfs_ftell(FILE *file)
{
        if (file)
                return file->f_lseek;
        else
                return -1;
}

//==============================================================================
/**
 * @brief Function support not standard operations
 *
 * @param[in]     *file         file
 * @param[in]      rq           request
 * @param[in,out] ...           additional function arguments
 *
 * @return 0 on success. On error, different from 0 is returned
 */
//==============================================================================
int vfs_ioctl(FILE *file, int rq, ...)
{
        va_list  args;
        stdret_t status;

        if (!file) {
                return -1;
        }

        if (!file->f_ioctl) {
                return -1;
        }

        va_start(args, rq);
        status = file->f_ioctl(file->FS_hdl, file->f_extra_data, file->fd, rq, va_arg(args, void*));
        va_end(args);

        return status;
}

//==============================================================================
/**
 * @brief Function returns file/dir status
 *
 * @param[in]  *path            file/dir path
 * @param[out] *stat            pointer to stat structure
 *
 * @return 0 on success. On error, -1 is returned
 */
//==============================================================================
int vfs_fstat(FILE *file, struct vfs_stat *stat)
{
        if (!file || !stat) {
                return -1;
        }

        if (!file->f_stat) {
                return -1;
        }

        return file->f_stat(file->FS_hdl, file->f_extra_data, file->fd, stat) == STD_RET_OK ? 0 : -1;
}

//==============================================================================
/**
 * @brief Function flush file data
 *
 * @param[in] *file     file to flush
 *
 * @return 0 on success. On error, -1 is returned
 */
//==============================================================================
int vfs_fflush(FILE *file)
{
        if (!file) {
                return -1;
        }

        if (!file->f_flush) {
                return -1;
        }

        return file->f_flush(file->FS_hdl, file->f_extra_data, file->fd) == STD_RET_OK ? 0 : -1;
}

//==============================================================================
/**
 * @brief Function check end of file
 *
 * @param[in] *file     file
 *
 * @return 0 if there is not a file end, otherwise greater than 0
 */
//==============================================================================
int vfs_feof(FILE *file)
{
        if (file) {
                return file->f_errflag & VFS_EFLAG_EOF ? 1 : 0;
        }

        return 0;
}

//==============================================================================
/**
 * @brief Function clear file's error
 *
 * @param[in] *file     file
 */
//==============================================================================
void vfs_clearerr(FILE *file)
{
        if (file) {
                file->f_errflag = 0;
        }
}

//==============================================================================
/**
 * @brief Function check that file has no errors
 *
 * @param[in] *file     file
 *
 * @return nonzero value if the file stream has errors occurred, 0 otherwise
 */
//==============================================================================
int vfs_ferror(FILE *file)
{
        if (file) {
                return file->f_errflag & VFS_EFLAG_ERR ? 1 : 0;
        }

        return 0;
}

//==============================================================================
/**
 * @brief Function rewind file
 *
 * @param[in] *file     file
 *
 * @return 0 on success. On error, -1 is returned
 */
//==============================================================================
int vfs_rewind(FILE *file)
{
        return vfs_fseek(file, 0, VFS_SEEK_SET);
}

//==============================================================================
/**
 * @brief Function convert file open mode string to flags
 *
 * @param[in] *str      file open mode string
 *
 * @return file open flags, -1 if error
 */
//==============================================================================
static int file_mode_str_to_flags(const char *str)
{
        if (strncmp("r", str, 2) == 0) {
                return (O_RDONLY);
        }

        if (strncmp("r+", str, 2) == 0) {
                return (O_RDWR);
        }

        if (strncmp("w", str, 2) == 0) {
                return (O_WRONLY | O_CREAT);
        }

        if (strncmp("w+", str, 2) == 0) {
                return (O_RDWR | O_CREAT);
        }

        if (strncmp("a", str, 2) == 0) {
                return (O_WRONLY | O_CREAT | O_APPEND);
        }

        if (strncmp("a+", str, 2) == 0) {
                return (O_RDWR | O_CREAT | O_APPEND);
        }

        return -1;
}

//==============================================================================
/**
 * @brief Function find FS in mounted list
 *
 * @param[in]  *path            path to FS
 * @param[in]   len             path length
 * @param[out] *itemid          item id in mount list
 *
 * @return pointer to FS info
 */
//==============================================================================
static struct FS_data *find_mounted_FS(const char *path, u16_t len, u32_t *itemid)
{
        struct FS_data *fs_info = NULL;

        int item_count = list_get_item_count(vfs_mnt_list);

        for (int i = 0; i < item_count; i++) {

                struct FS_data *data = list_get_nitem_data(vfs_mnt_list, i);

                if (strncmp(path, data->mount_point, len) != 0) {
                        continue;
                }

                fs_info = data;

                if (itemid) {
                        if (list_get_nitem_ID(vfs_mnt_list, i, itemid) != STD_RET_OK) {
                                fs_info = NULL;
                        }
                }

                break;
        }

        return fs_info;
}

//==============================================================================
/**
 * @brief Function find base FS of selected path
 *
 * @param[in]   *path           path to FS
 * @param[out] **extPath        pointer to external part of path
 *
 * @return pointer to FS info
 */
//==============================================================================
static struct FS_data *find_base_FS(const char *path, char **extPath)
{
        struct FS_data *fs_info = NULL;

        char *path_tail = (char*)path + strlen(path);

        if (*(path_tail - 1) == '/') {
                path_tail--;
        }

        while (path_tail >= path) {
                struct FS_data *fs = find_mounted_FS(path, path_tail - path + 1, NULL);

                if (fs) {
                        fs_info = fs;
                        break;
                } else {
                        while (*(--path_tail) != '/' && path_tail >= path);
                }
        }

        if (fs_info && extPath) {
                *extPath = path_tail;
        }

        return fs_info;
}

//==============================================================================
/**
 * @brief Function create new path with slash and cwd correction
 *
 * @param[in] *path             path to correct
 * @param[in]  corr             path correction kind
 *
 * @return pointer to new path
 */
//==============================================================================
static char *new_corrected_path(const char *path, enum path_correction corr)
{
        char       *new_path;
        uint        new_path_len = strlen(path);
        const char *cwd;
        uint        cwd_len = 0;

        /* correct ending slash */
        if (corr == SUB_SLASH && last_character(path) == '/') {
            new_path_len--;
        } else if (corr == ADD_SLASH && last_character(path) != '/') {
            new_path_len++;
        }

        /* correct cwd */
        if (first_character(path) != '/') {
                cwd = _task_get_data()->f_cwd;
                if (cwd) {
                    cwd_len       = strlen(cwd);
                    new_path_len += cwd_len;

                    if (last_character(cwd) != '/' && cwd_len) {
                        new_path_len++;
                        cwd_len++;
                    }
                }
        }

        new_path = sysm_syscalloc(new_path_len + 1, sizeof(char));
        if (new_path) {
                if (cwd_len && cwd) {
                        strcpy(new_path, cwd);

                        if (last_character(cwd) != '/') {
                            strcat(new_path, "/");
                        }
                }

                if (corr == SUB_SLASH) {
                        strncat(new_path, path, new_path_len - cwd_len);
                } else if (corr == ADD_SLASH) {
                        strcat(new_path, path);

                        if (last_character(new_path) != '/') {
                            strcat(new_path, "/");
                        }
                } else {
                    strcat(new_path, path);
                }
        }

        return new_path;
}

#ifdef __cplusplus
}
#endif

/*==============================================================================
  End of file
==============================================================================*/
