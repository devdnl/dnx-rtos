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
#include "system/thread.h"
#include <errno.h>
#include <string.h>

/*==============================================================================
  Local symbolic constants/macros
==============================================================================*/
/* wait time for operation on VFS */
#define MTX_BLOCK_TIME                          10

#define FILE_VALIDATION_NUMBER                  (u32_t)0x495D47CB
#define DIR_VALIDATION_NUMBER                   (u32_t)0x297E823D

/*==============================================================================
  Local types, enums definitions
==============================================================================*/
/** file type */
struct vfs_file
{
        void      *FS_hdl;
        stdret_t (*f_close)(void *FS_hdl, void *extra_data, fd_t fd, bool force, const task_t *opened_by_task);
        ssize_t  (*f_write)(void *FS_hdl, void *extra_data, fd_t fd, const u8_t *src, size_t count, u64_t *fpos);
        ssize_t  (*f_read )(void *FS_hdl, void *extra_data, fd_t fd, u8_t *dst, size_t count, u64_t *fpos);
        stdret_t (*f_ioctl)(void *FS_hdl, void *extra_data, fd_t fd, int iorq, void *args);
        stdret_t (*f_stat )(void *FS_hdl, void *extra_data, fd_t fd, struct stat *stat);
        stdret_t (*f_flush)(void *FS_hdl, void *extra_data, fd_t fd);
        void      *f_extra_data;
        fd_t       fd;
        u64_t      f_lseek;
        int        f_errflag;
        u32_t      validation;
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
static int              fclose                  (FILE *file, bool force, task_t *opened_by_task);
static int              increase_task_priority  (void);
static inline void      restore_priority        (int priority);
static inline void      mutex_force_lock        (mutex_t *mtx);
static int              file_mode_str_to_flags  (const char *str);
static struct FS_data  *find_mounted_FS         (const char *path, u16_t len, u32_t *itemid);
static struct FS_data  *find_base_FS            (const char *path, char **extPath);
static char            *new_corrected_path      (const char *path, enum path_correction corr);

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
        vfs_resource_mtx = mutex_new(MUTEX_RECURSIVE);

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
                errno = EINVAL;
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

        mutex_force_lock(vfs_resource_mtx);
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
                  && FIRST_CHARACTER(cwd_mount_point) == '/' ) {

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
                                mutex_unlock(vfs_resource_mtx);
                                sysm_sysfree(cwd_src_path);
                                return STD_RET_OK;
                        } else {
                                errno = ENOMEM;
                        }
                }

                sysm_sysfree(new_fs);
                sysm_sysfree(cwd_mount_point);
                sysm_sysfree(cwd_src_path);
        } else {
                errno = ENOENT;
        }

        mutex_unlock(vfs_resource_mtx);
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
                errno = EINVAL;
                return STD_RET_ERROR;
        }

        char *cwd_path = new_corrected_path(path, ADD_SLASH);
        if (cwd_path == NULL) {
                return STD_RET_ERROR;
        }

        mutex_force_lock(vfs_resource_mtx);
        u32_t           item_id;
        struct FS_data *mount_fs = find_mounted_FS(cwd_path, -1, &item_id);
        sysm_sysfree(cwd_path);

        if (mount_fs == NULL) {
                errno = ENOENT;
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
                        mutex_unlock(vfs_resource_mtx);
                        return STD_RET_OK;
                }
        } else {
                errno = EBUSY;
        }

vfs_umount_error:
        mutex_unlock(vfs_resource_mtx);
        return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief Function gets mount point for n item
 *
 * @param[in]   item            mount point number
 * @param[out] *mntent          mount entry data
 *
 * @retval STD_RET_OK           success
 * @retval STD_RET_ERROR        error
 */
//==============================================================================
stdret_t vfs_getmntentry(size_t item, struct vfs_mntent *mntent)
{
        if (!mntent) {
                errno = EINVAL;
                return STD_RET_ERROR;
        }

        mutex_force_lock(vfs_resource_mtx);
        struct FS_data *fs = list_get_nitem_data(vfs_mnt_list, item);
        mutex_unlock(vfs_resource_mtx);

        if (fs) {
                struct vfs_statfs stat_fs = {.f_fsname = NULL};

                if (fs->interface.fs_statfs) {
                        int priority = increase_task_priority();
                        fs->interface.fs_statfs(fs->handle, &stat_fs);
                        restore_priority(priority);
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

        errno = ENXIO;

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
                errno = EINVAL;
                return -1;
        }

        char *cwd_path = new_corrected_path(path, NO_SLASH_ACTION);
        if (!cwd_path) {
                return -1;
        }

        mutex_force_lock(vfs_resource_mtx);
        char *external_path = NULL;
        struct FS_data *fs  = find_base_FS(cwd_path, &external_path);
        mutex_unlock(vfs_resource_mtx);

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
 * @param[in] path              path to new directory
 * @param[in] mode              directory mode
 *
 * @return 0 on success. On error, -1 is returned
 */
//==============================================================================
int vfs_mkdir(const char *path, mode_t mode)
{
        if (!path) {
                errno = EINVAL;
                return -1;
        }

        char *cwd_path = new_corrected_path(path, SUB_SLASH);
        if (!cwd_path) {
               return -1;
        }

        mutex_force_lock(vfs_resource_mtx);
        char *external_path = NULL;
        struct FS_data *fs  = find_base_FS(cwd_path, &external_path);
        mutex_unlock(vfs_resource_mtx);

        int status = -1;
        if (fs) {
                if (fs->interface.fs_mkdir) {
                        int priority = increase_task_priority();
                        status = fs->interface.fs_mkdir(fs->handle, external_path, mode) == STD_RET_OK ? 0 : -1;
                        restore_priority(priority);
                }
        }

        sysm_sysfree(cwd_path);

        return status;
}

//==============================================================================
/**
 * @brief Create pipe
 *
 * @param[in] path              path to pipe
 * @param[in] mode              directory mode
 *
 * @return 0 on success. On error, -1 is returned
 */
//==============================================================================
int vfs_mkfifo(const char *path, mode_t mode)
{
        if (!path) {
                errno = EINVAL;
                return -1;
        }

        char *cwd_path = new_corrected_path(path, NO_SLASH_ACTION);
        if (!cwd_path) {
                return -1;
        }

        mutex_force_lock(vfs_resource_mtx);
        char *external_path = NULL;
        struct FS_data *fs  = find_base_FS(cwd_path, &external_path);
        mutex_unlock(vfs_resource_mtx);

        int status = -1;
        if (fs) {
                if (fs->interface.fs_mkfifo) {
                        int priority = increase_task_priority();
                        status = fs->interface.fs_mkfifo(fs->handle, external_path, mode) == STD_RET_OK ? 0 : -1;
                        restore_priority(priority);
                }
        }

        sysm_sysfree(cwd_path);

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
                errno = EINVAL;
                return NULL;
        }

        DIR *dir = sysm_sysmalloc(sizeof(DIR));
        if (dir) {
                stdret_t status = STD_RET_ERROR;

                char *cwd_path = new_corrected_path(path, ADD_SLASH);
                if (cwd_path) {
                        mutex_force_lock(vfs_resource_mtx);
                        char *external_path = NULL;
                        struct FS_data *fs  = find_base_FS(cwd_path, &external_path);
                        mutex_unlock(vfs_resource_mtx);

                        if (fs) {
                                dir->f_handle = fs->handle;

                                if (fs->interface.fs_opendir) {
                                        int priority = increase_task_priority();
                                        status = fs->interface.fs_opendir(fs->handle, external_path, dir);
                                        restore_priority(priority);
                                }
                        }

                        sysm_sysfree(cwd_path);
                }

                if (status == STD_RET_ERROR) {
                        sysm_sysfree(dir);
                        dir = NULL;
                } else {
                        dir->validation = DIR_VALIDATION_NUMBER;
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
                if (dir->f_closedir && dir->validation == DIR_VALIDATION_NUMBER) {
                        if (dir->f_closedir(dir->f_handle, dir) == STD_RET_OK) {
                                dir->validation = 0;
                                sysm_sysfree(dir);
                                return 0;
                        }
                }
        }

        errno = EINVAL;
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

        if (dir) {
                if (dir->f_readdir && dir->validation == DIR_VALIDATION_NUMBER) {
                        int priority = increase_task_priority();
                        direntry = dir->f_readdir(dir->f_handle, dir);
                        restore_priority(priority);
                        return direntry;
                }
        }

        errno = EINVAL;
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
                errno = EINVAL;
                return -1;
        }

        char *cwd_path = new_corrected_path(path, ADD_SLASH);
        if (!cwd_path) {
                return -1;
        }

        mutex_force_lock(vfs_resource_mtx);
        char *external_path      = NULL;
        struct FS_data *mount_fs = find_mounted_FS(cwd_path, -1, NULL);
        LAST_CHARACTER(cwd_path) = '\0';
        struct FS_data *base_fs  = find_base_FS(cwd_path, &external_path);
        mutex_unlock(vfs_resource_mtx);

        int status = -1;
        if (base_fs && !mount_fs) {
                if (base_fs->interface.fs_remove) {
                        int priority = increase_task_priority();
                        status = base_fs->interface.fs_remove(base_fs->handle,
                                                              external_path) == STD_RET_OK ? 0 : -1;
                        restore_priority(priority);
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
                errno = EINVAL;
                return -1;
        }

        int   status       = -1;
        char *cwd_old_name = new_corrected_path(old_name, NO_SLASH_ACTION);
        char *cwd_new_name = new_corrected_path(new_name, NO_SLASH_ACTION);
        if (!cwd_old_name || !cwd_new_name) {
                goto exit;
        }

        mutex_force_lock(vfs_resource_mtx);
        char *old_extern_path  = NULL;
        char *new_extern_path  = NULL;
        struct FS_data *old_fs = find_base_FS(cwd_old_name, &old_extern_path);
        struct FS_data *new_fs = find_base_FS(cwd_new_name, &new_extern_path);
        mutex_unlock(vfs_resource_mtx);

        if (!old_fs || !new_fs) {
                goto exit;
        }

        if (old_fs != new_fs) {
                errno = EPERM;
                goto exit;
        }

        if (!old_fs->interface.fs_rename) {
                goto exit;
        }

        int priority = increase_task_priority();
        status = old_fs->interface.fs_rename(old_fs->handle, old_extern_path,
                                             new_extern_path) == STD_RET_OK ? 0 : -1;
        restore_priority(priority);

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
                errno = EINVAL;
                return -1;
        }

        char *cwd_path = new_corrected_path(path, NO_SLASH_ACTION);
        if (!cwd_path) {
                return -1;
        }

        mutex_force_lock(vfs_resource_mtx);
        char *external_path = NULL;
        struct FS_data *fs  = find_base_FS(cwd_path, &external_path);
        mutex_unlock(vfs_resource_mtx);

        int status = -1;
        if (fs) {
                if (fs->interface.fs_chmod) {
                        int priority = increase_task_priority();
                        status = fs->interface.fs_chmod(fs->handle, external_path, mode) == STD_RET_OK ? 0 : -1;
                        restore_priority(priority);
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
                errno = EINVAL;
                return -1;
        }

        char *cwd_path = new_corrected_path(path, NO_SLASH_ACTION);
        if (!cwd_path) {
                return -1;
        }

        mutex_force_lock(vfs_resource_mtx);
        char *external_path = NULL;
        struct FS_data *fs  = find_base_FS(path, &external_path);
        mutex_unlock(vfs_resource_mtx);

        int status = -1;
        if (fs) {
                if (fs->interface.fs_chown) {
                        int priority = increase_task_priority();
                        status = fs->interface.fs_chown(fs->handle, external_path, owner, group) == STD_RET_OK ? 0 : -1;
                        restore_priority(priority);
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
int vfs_stat(const char *path, struct stat *stat)
{
        if (!path || !stat) {
                errno = EINVAL;
                return -1;
        }

        char *cwd_path = new_corrected_path(path, NO_SLASH_ACTION);
        if (!cwd_path) {
                return -1;
        }

        mutex_force_lock(vfs_resource_mtx);
        char *external_path = NULL;
        struct FS_data *fs  = find_base_FS(path, &external_path);
        mutex_unlock(vfs_resource_mtx);

        int status = -1;
        if (fs) {
                if (fs->interface.fs_stat){
                        int priority = increase_task_priority();
                        status = fs->interface.fs_stat(fs->handle, external_path, stat) == STD_RET_OK ? 0 : -1;
                        restore_priority(priority);
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
                errno = EINVAL;
                return -1;
        }

        char *cwd_path = new_corrected_path(path, ADD_SLASH);
        if (!cwd_path) {
                return -1;
        }

        mutex_force_lock(vfs_resource_mtx);
        struct FS_data *fs = find_mounted_FS(cwd_path, -1, NULL);
        mutex_unlock(vfs_resource_mtx);
        sysm_sysfree(cwd_path);

        int status = -1;
        if (fs) {
                if (fs->interface.fs_statfs) {
                        int priority = increase_task_priority();
                        status = fs->interface.fs_statfs(fs->handle, statfs) == STD_RET_OK ? 0 : -1;
                        restore_priority(priority);
                }
        }

        return status;
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
                errno = EINVAL;
                return NULL;
        }

        if (LAST_CHARACTER(path) == '/') { /* path is a directory */
                errno = EISDIR;
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
                mutex_force_lock(vfs_resource_mtx);
                char *external_path = NULL;
                struct FS_data *fs  = find_base_FS(cwd_path, &external_path);
                mutex_unlock(vfs_resource_mtx);

                if (fs == NULL) {
                        goto vfs_open_error;
                }

                if (fs->interface.fs_open == NULL) {
                        goto vfs_open_error;
                }

                int priority = increase_task_priority();

                if (fs->interface.fs_open(fs->handle, &file->f_extra_data,
                                          &file->fd,  &file->f_lseek,
                                          external_path, flags) == STD_RET_OK) {

                        restore_priority(priority);

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

                        file->validation = FILE_VALIDATION_NUMBER;
                        sysm_sysfree(cwd_path);
                        return file;
                }

                restore_priority(priority);

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
        } else {
                errno = EINVAL;
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
        return fclose(file, false, NULL);
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
        return fclose(file, true, opened_by_task);
}

//==============================================================================
/**
 * @brief Function write data to file
 *
 * @param[in] *ptr              address to data (src)
 * @param[in]  size             item size
 * @param[in]  count            number of items
 * @param[in] *file             pointer to file object
 *
 * @return the number of items successfully written. If an error occurs, or the
 *         end-of-file is reached, the return value is a short item count (or 0).
 */
//==============================================================================
size_t vfs_fwrite(const void *ptr, size_t size, size_t count, FILE *file)
{
        ssize_t n = 0;

        if (ptr && size && count && file) {
                if (file->f_write && file->validation == FILE_VALIDATION_NUMBER) {
                        n = file->f_write(file->FS_hdl, file->f_extra_data, file->fd,
                                          ptr, size * count, &file->f_lseek);

                        if (n < 0) {
                                file->f_errflag |= VFS_EFLAG_ERR;
                                n = 0;
                        } else if (n < (ssize_t)(size * count)) {
                                file->f_errflag |= VFS_EFLAG_EOF;
                        }

                        if (n >= 0) {
                                file->f_lseek += (u64_t)n;
                                n /= size;
                        }
                } else {
                        errno = ENOENT;
                }
        } else {
                errno = EINVAL;
        }

        return n;
}

//==============================================================================
/**
 * @brief Function read data from file
 *
 * @param[out] *ptr             address to data (dst)
 * @param[in]   size            item size
 * @param[in]   count           number of items
 * @param[in]  *file            pointer to file object
 *
 * @return the number of items successfully read. If an error occurs, or the
 *         end-of-file is reached, the return value is a short item count (or 0).
 */
//==============================================================================
size_t vfs_fread(void *ptr, size_t size, size_t count, FILE *file)
{
        ssize_t n = 0;

        if (ptr && size && count && file) {
                if (file->f_read && file->validation == FILE_VALIDATION_NUMBER) {
                        n = file->f_read(file->FS_hdl, file->f_extra_data, file->fd,
                                         ptr, size * count, &file->f_lseek);

                        if (n < 0) {
                                file->f_errflag |= VFS_EFLAG_ERR;
                                n = 0;
                        } else if (n < (ssize_t)(size * count)) {
                                file->f_errflag |= VFS_EFLAG_EOF;
                        }

                        if (n >= 0) {
                                file->f_lseek += (u64_t)n;
                                n /= size;
                        }
                } else {
                        errno = ENOENT;
                }
        } else {
                errno = EINVAL;
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
        struct stat stat;

        if (!file || mode > VFS_SEEK_END) {
                errno = EINVAL;
                return -1;
        }

        if (file->validation != FILE_VALIDATION_NUMBER) {
                errno = ENOENT;
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
        if (file) {
                return file->f_lseek;
        } else {
                errno = EINVAL;
                return -1;
        }
}

//==============================================================================
/**
 * @brief Function support not standard operations on devices
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
        va_list arg;
        va_start(arg, rq);
        int status = vfs_vioctl(file, rq, arg);
        va_end(arg);
        return status;
}

//==============================================================================
/**
 * @brief Function support not standard operations on devices
 *
 * @param[in]     *file         file
 * @param[in]      rq           request
 * @param[in,out]  arg          arguments
 *
 * @return 0 on success. On error, different from 0 is returned
 */
//==============================================================================
int vfs_vioctl(FILE *file, int rq, va_list arg)
{
        if (!file) {
                errno = EINVAL;
                return -1;
        }

        if (!file->f_ioctl && file->validation != FILE_VALIDATION_NUMBER) {
                errno = ENOENT;
                return -1;
        }

        return file->f_ioctl(file->FS_hdl, file->f_extra_data, file->fd, rq, va_arg(arg, void*));
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
int vfs_fstat(FILE *file, struct stat *stat)
{
        if (!file || !stat) {
                errno = EINVAL;
                return -1;
        }

        if (!file->f_stat && file->validation != FILE_VALIDATION_NUMBER) {
                errno = ENOENT;
                return -1;
        }

        int priority = increase_task_priority();
        int status = file->f_stat(file->FS_hdl, file->f_extra_data, file->fd, stat) == STD_RET_OK ? 0 : -1;
        restore_priority(priority);

        return status;
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
                errno = EINVAL;
                return -1;
        }

        if (!file->f_flush && file->validation != FILE_VALIDATION_NUMBER) {
                errno = ENOENT;
                return -1;
        }

        int priority = increase_task_priority();
        int status = file->f_flush(file->FS_hdl, file->f_extra_data, file->fd) == STD_RET_OK ? 0 : -1;
        restore_priority(priority);

        return status;
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
                if (file->validation == FILE_VALIDATION_NUMBER) {
                        return file->f_errflag & VFS_EFLAG_EOF ? 1 : 0;
                } else {
                        errno = ENOENT;
                }
        } else {
                errno = EINVAL;
        }

        return 1;
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
                if (file->validation == FILE_VALIDATION_NUMBER) {
                        file->f_errflag = 0;
                } else {
                        errno = ENOENT;
                }
        } else {
                errno = EINVAL;
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
                if (file->validation == FILE_VALIDATION_NUMBER) {
                        return file->f_errflag & VFS_EFLAG_ERR ? 1 : 0;
                } else {
                        errno = ENOENT;
                }
        } else {
                errno = EINVAL;
        }

        return 1;
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
 * @brief Generic file close
 *
 * @param[in] file              pinter to file
 * @param[in] force             force close
 * @param[in] opened_by_task    task which opened file
 *
 * @return 0 on success. On error, -1 is returned
 */
//==============================================================================
static int fclose(FILE *file, bool force, task_t *opened_by_task)
{
        if (file) {
                if (file->f_close && file->validation == FILE_VALIDATION_NUMBER) {
                        if (file->f_close(file->FS_hdl, file->f_extra_data, file->fd, force, opened_by_task) == STD_RET_OK) {
                                file->validation = 0;
                                sysm_sysfree(file);
                                return 0;
                        }
                } else {
                        errno = ENOENT;
                }
        } else {
                errno = EINVAL;
        }

        return -1;
}

//==============================================================================
/**
 * @brief Function increase task priority and return original priority value
 *
 * @return original task priority
 */
//==============================================================================
static int increase_task_priority(void)
{
        int priority = task_get_priority();

        if (priority < HIGHEST_PRIORITY) {
                task_set_priority(priority + 1);
        }

        return priority;
}

//==============================================================================
/**
 * @brief Function restore original priority
 *
 * @param priority
 */
//==============================================================================
static inline void restore_priority(int priority)
{
        task_set_priority(priority);
}

//==============================================================================
/**
 * @brief Force lock mutex
 *
 * @param mtx           mutex to lock
 */
//==============================================================================
static inline void mutex_force_lock(mutex_t *mtx)
{
        while (mutex_lock(mtx, MTX_BLOCK_TIME) != true);
}

//==============================================================================
/**
 * @brief Function convert file open mode string to flags
 *        Function set errno: EINVAL
 *
 * @param[in] *str      file open mode string
 *
 * @return file open flags, -1 if error
 */
//==============================================================================
static int file_mode_str_to_flags(const char *str)
{
        if (strcmp("r", str) == 0) {
                return (O_RDONLY);
        }

        if (strcmp("r+", str) == 0) {
                return (O_RDWR);
        }

        if (strcmp("w", str) == 0) {
                return (O_WRONLY | O_CREAT);
        }

        if (strcmp("w+", str) == 0) {
                return (O_RDWR | O_CREAT);
        }

        if (strcmp("a", str) == 0) {
                return (O_WRONLY | O_CREAT | O_APPEND);
        }

        if (strcmp("a+", str) == 0) {
                return (O_RDWR | O_CREAT | O_APPEND);
        }

        errno = EINVAL;

        return -1;
}

//==============================================================================
/**
 * @brief Function find FS in mounted list
 *        Function set errno: ENXIO
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

        if (fs_info) {
                errno = ENXIO;
        }

        return fs_info;
}

//==============================================================================
/**
 * @brief Function find base FS of selected path
 *        Function set errno: ENOENT
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

        if (!fs_info) {
                errno = ENOENT;
        }

        return fs_info;
}

//==============================================================================
/**
 * @brief Function create new path with slash and cwd correction
 *        Function set errno: ENOMEM
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
        if (corr == SUB_SLASH && LAST_CHARACTER(path) == '/') {
                new_path_len--;
        } else if (corr == ADD_SLASH && LAST_CHARACTER(path) != '/') {
                new_path_len++;
        }

        /* correct cwd */
        if (FIRST_CHARACTER(path) != '/') {
                cwd = _task_get_data()->f_cwd;
                if (cwd) {
                        cwd_len       = strlen(cwd);
                        new_path_len += cwd_len;

                        if (LAST_CHARACTER(cwd) != '/' && cwd_len) {
                                new_path_len++;
                                cwd_len++;
                        }
                }
        }

        new_path = sysm_syscalloc(new_path_len + 1, sizeof(char));
        if (new_path) {
                if (cwd_len && cwd) {
                        strcpy(new_path, cwd);

                        if (LAST_CHARACTER(cwd) != '/') {
                                strcat(new_path, "/");
                        }
                }

                if (corr == SUB_SLASH) {
                        strncat(new_path, path, new_path_len - cwd_len);
                } else if (corr == ADD_SLASH) {
                        strcat(new_path, path);

                        if (LAST_CHARACTER(new_path) != '/') {
                                strcat(new_path, "/");
                        }
                } else {
                        strcat(new_path, path);
                }
        } else {
                errno = ENOMEM;
        }

        return new_path;
}

#ifdef __cplusplus
}
#endif

/*==============================================================================
  End of file
==============================================================================*/
