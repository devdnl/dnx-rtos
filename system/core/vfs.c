/*=========================================================================*//**
@file    vfs.c

@author  Daniel Zorychta

@brief   This file support virtual file system

@note    Copyright (C) 2012 Daniel Zorychta <daniel.zorychta@gmail.com>

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

/*==============================================================================
  Local symbolic constants/macros
==============================================================================*/
/* wait time for operation on VFS */
#define MTX_BLOCK_TIME                        10
#define force_lock_mutex(mtx, blocktime)      while (lock_mutex(mtx, blocktime) != MUTEX_LOCKED)

#define calloc(nmemb, msize)                  sysm_syscalloc(nmemb, msize)
#define malloc(size)                          sysm_sysmalloc(size)
#define free(mem)                             sysm_sysfree(mem)

/*==============================================================================
  Local types, enums definitions
==============================================================================*/
/** file type */
struct vfs_file
{
        void     *FS_hdl;
        stdret_t (*f_close)(void *FS_hdl, fd_t fd);
        size_t   (*f_write)(void *FS_hdl, fd_t fd, void *src, size_t size, size_t nitems, size_t seek);
        size_t   (*f_read )(void *FS_hdl, fd_t fd, void *dst, size_t size, size_t nitmes, size_t seek);
        stdret_t (*f_ioctl)(void *FS_hdl, fd_t fd, iorq_t iorq, va_list);
        stdret_t (*f_stat )(void *FS_hdl, fd_t fd, struct vfs_statf *stat);
        stdret_t (*f_flush)(void *FS_hdl, fd_t fd);
        fd_t     fd;
        size_t   f_seek;
};

struct FS_data {
          char                          *mount_point;
          struct FS_data                *base_FS;
          void                          *handle;
          struct vfs_FS_interface       interface;
          u8_t                          mounted_FS_counter;
};

enum path_correction {
          ADD_SLASH,
          SUB_SLASH,
};

/*==============================================================================
  Local function prototypes
==============================================================================*/
static struct FS_data *find_mounted_FS(const char *path, u16_t len, u32_t *itemid);
static struct FS_data *find_base_FS(const char *path, char **extPath);
static char           *new_corrected_path(const char *path, enum path_correction corr);

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
        vfs_mnt_list     = new_list();
        vfs_resource_mtx = new_mutex();

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
        struct FS_data *mount_fs;
        struct FS_data *base_fs;
        struct FS_data *new_fs   = NULL;
        char           *new_path = NULL;
        char           *external_path;

        if (!mount_point || !fs_interface) {
                return STD_RET_ERROR;
        }

        if (!(new_path = new_corrected_path(mount_point, ADD_SLASH))) {
                return STD_RET_ERROR;
        }

        force_lock_mutex(vfs_resource_mtx, MTX_BLOCK_TIME);
        mount_fs = find_mounted_FS(new_path, -1, NULL);
        base_fs  = find_base_FS(new_path, &external_path);

        /*
         * create new FS in existing DIR and FS, otherwise create new FS if
         * first mount
         */
        if (base_fs && mount_fs == NULL) {
                if (base_fs->interface.fs_opendir && external_path) {

                        if (base_fs->interface.fs_opendir(base_fs->handle,
                                                         external_path,
                                                         NULL) == STD_RET_OK) {

                                new_fs = calloc(1, sizeof(struct FS_data));
                                base_fs->mounted_FS_counter++;
                        }
                }
        } else if (  list_get_item_count(vfs_mnt_list) == 0
                  && strlen(new_path) == 1
                  && new_path[0] == '/' ) {

                new_fs = calloc(1, sizeof(struct FS_data));
        }

        /*
         * mount FS if created
         */
        if (new_fs && fs_interface->fs_init) {
                new_fs->interface = *fs_interface;

                if (fs_interface->fs_init(&new_fs->handle, src_path) == STD_RET_OK) {
                        new_fs->mount_point = new_path;
                        new_fs->base_FS     = base_fs;
                        new_fs->mounted_FS_counter = 0;

                        if (list_add_item(vfs_mnt_list, vfs_id_counter++, new_fs) >= 0) {
                                unlock_mutex(vfs_resource_mtx);
                                return STD_RET_OK;
                        }
                }

                free(new_fs);
                free(new_path);
        }

        unlock_mutex(vfs_resource_mtx);
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
        u32_t          item_id;
        char           *new_path;
        struct FS_data *mount_fs;

        if (!path) {
                return STD_RET_ERROR;
        }

        if (path[0] != '/') {
                return STD_RET_ERROR;
        }

        new_path = new_corrected_path(path, ADD_SLASH);
        if (new_path == NULL) {
                return STD_RET_ERROR;
        }

        force_lock_mutex(vfs_resource_mtx, MTX_BLOCK_TIME);
        mount_fs = find_mounted_FS(new_path, -1, &item_id);
        free(new_path);

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
                        free(mount_fs->mount_point);
                }

                if (list_rm_iditem(vfs_mnt_list, item_id) == STD_RET_OK) {
                        unlock_mutex(vfs_resource_mtx);
                        return STD_RET_OK;
                }
        }

        vfs_umount_error:
        unlock_mutex(vfs_resource_mtx);
        return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief Function gets mount point for n item
 *
 * @param[in]   item        mount point number
 * @param[out] *mntent      mount entry data
 */
//==============================================================================
stdret_t vfs_getmntentry(size_t item, struct vfs_mntent *mntent)
{
        struct FS_data    *fs = NULL;
        struct vfs_statfs stat_fs;

        if (mntent) {
                force_lock_mutex(vfs_resource_mtx, MTX_BLOCK_TIME);
                fs = list_get_nitem_data(vfs_mnt_list, item);
                unlock_mutex(vfs_resource_mtx);

                if (fs) {
                        stat_fs.fsname = NULL;

                        if (fs->interface.fs_statfs) {
                                fs->interface.fs_statfs(fs->handle, &stat_fs);
                        }

                        if (stat_fs.fsname) {
                                if (strlen(fs->mount_point) > 1) {
                                        strncpy(mntent->mnt_dir, fs->mount_point,
                                                strlen(fs->mount_point) - 1);
                                } else {
                                        strcpy(mntent->mnt_dir, fs->mount_point);
                                }

                                strcpy(mntent->mnt_fsname, stat_fs.fsname);
                                mntent->free  = stat_fs.f_bfree;
                                mntent->total = stat_fs.f_blocks;

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
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
stdret_t vfs_mknod(const char *path, struct vfs_drv_interface *drv_interface)
{
        char           *external_path = NULL;
        struct FS_data *fs;

        if (path && drv_interface) {
                force_lock_mutex(vfs_resource_mtx, MTX_BLOCK_TIME);
                fs = find_base_FS(path, &external_path);
                unlock_mutex(vfs_resource_mtx);

                if (fs) {
                        if (fs->interface.fs_mknod) {
                                return fs->interface.fs_mknod(fs->handle,
                                                              external_path,
                                                              drv_interface);
                        }
                }
        }

        return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief Create directory
 *
 * @param[in] *path                 path to new directory
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
stdret_t vfs_mkdir(const char *path)
{
        stdret_t       status = STD_RET_ERROR;
        char           *new_path;
        char           *external_path = NULL;
        struct FS_data *fs;

        if (path) {
                if (!(new_path = new_corrected_path(path, SUB_SLASH))) {
                       return STD_RET_ERROR;
                }

                force_lock_mutex(vfs_resource_mtx, MTX_BLOCK_TIME);
                fs = find_base_FS(new_path, &external_path);
                unlock_mutex(vfs_resource_mtx);

                if (fs) {
                        if (fs->interface.fs_mkdir) {
                                status = fs->interface.fs_mkdir(fs->handle, external_path);
                        }
                }

                free(new_path);
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
dir_t *vfs_opendir(const char *path)
{
        stdret_t       status         = STD_RET_ERROR;
        dir_t          *dir           = NULL;
        char           *external_path = NULL;
        char           *new_path;
        struct FS_data *fs;

        if (!path) {
                return NULL;
        }

        if ((dir = malloc(sizeof(dir_t)))) {
                if ((new_path = new_corrected_path(path, ADD_SLASH))) {
                        force_lock_mutex(vfs_resource_mtx, MTX_BLOCK_TIME);
                        fs = find_base_FS(new_path, &external_path);
                        unlock_mutex(vfs_resource_mtx);

                        if (fs) {
                                dir->handle = fs->handle;

                                if (fs->interface.fs_opendir) {
                                        status = fs->interface.fs_opendir(fs->handle, external_path, dir);
                                }
                        }

                        free(new_path);
                }

                if (status == STD_RET_ERROR) {
                        free(dir);
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
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
stdret_t vfs_closedir(dir_t *dir)
{
        if (dir) {
                if (dir->cldir) {
                        if (dir->cldir(dir->handle, dir) == STD_RET_OK) {
                                free(dir);
                                return STD_RET_OK;
                        }
                }
        }

        return STD_RET_ERROR;
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
dirent_t vfs_readdir(dir_t *dir)
{
        dirent_t direntry;
        direntry.name = NULL;
        direntry.size = 0;

        if (dir->rddir) {
                direntry = dir->rddir(dir->handle, dir);
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
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
stdret_t vfs_remove(const char *path)
{
        stdret_t       status         = STD_RET_ERROR;
        char           *external_path = NULL;
        char           *new_path;
        struct FS_data *mount_fs;
        struct FS_data *base_fs;

        if (path) {
                if ((new_path = new_corrected_path(path, ADD_SLASH))) {
                        force_lock_mutex(vfs_resource_mtx, MTX_BLOCK_TIME);
                        mount_fs = find_mounted_FS(new_path, -1, NULL);
                        base_fs  = find_base_FS(path, &external_path);
                        unlock_mutex(vfs_resource_mtx);

                        if (base_fs && !mount_fs) {
                                if (base_fs->interface.fs_remove) {
                                        status = base_fs->interface.fs_remove(base_fs->handle,
                                                                              external_path);
                                }
                        }

                        free(new_path);
                }
        }

        return status;
}

//==============================================================================
/**
 * @brief Rename file name
 * The implementation of rename can move files only if external FS provide
 * functionality. Local VFS cannot do this. Cross FS move is also not possible.
 *
 * @param[in] *oldName                  old file name
 * @param[in] *newName                  new file name
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
stdret_t vfs_rename(const char *oldName, const char *newName)
{
        char *old_extern_path = NULL;
        char *new_extern_path = NULL;
        struct FS_data *old_fs;
        struct FS_data *new_fs;

        if (oldName && newName) {
                force_lock_mutex(vfs_resource_mtx, MTX_BLOCK_TIME);
                old_fs = find_base_FS(oldName, &old_extern_path);
                new_fs = find_base_FS(newName, &new_extern_path);
                unlock_mutex(vfs_resource_mtx);

                if (old_fs && new_fs && old_fs == new_fs) {
                        if (old_fs->interface.fs_rename) {
                                return old_fs->interface.fs_rename(old_fs->handle,
                                                                   old_extern_path,
                                                                   new_extern_path);
                        }
                }
        }

        return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief Function change file mode
 *
 * @param[in] *path         file path
 * @param[in]  mode         file mode
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
stdret_t vfs_chmod(const char *path, u16_t mode)
{
        char *external_path = NULL;
        struct FS_data *fs;

        if (path) {
                force_lock_mutex(vfs_resource_mtx, MTX_BLOCK_TIME);
                fs = find_base_FS(path, &external_path);
                unlock_mutex(vfs_resource_mtx);

                if (fs) {
                        if (fs->interface.fs_chmod) {
                                return fs->interface.fs_chmod(fs->handle, external_path, mode);
                        }
                }
        }

        return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief Function change file owner and group
 *
 * @param[in] *path         file path
 * @param[in]  owner        file owner
 * @param[in]  group        file group
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
stdret_t vfs_chown(const char *path, u16_t owner, u16_t group)
{
        char *external_path = NULL;
        struct FS_data *fs;

        if (path) {
                force_lock_mutex(vfs_resource_mtx, MTX_BLOCK_TIME);
                fs = find_base_FS(path, &external_path);
                unlock_mutex(vfs_resource_mtx);

                if (fs) {
                        if (fs->interface.fs_chown) {
                                return fs->interface.fs_chown(fs->handle, external_path, owner, group);
                        }
                }
        }

        return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief Function returns file/dir status
 *
 * @param[in]  *path            file/dir path
 * @param[out] *stat            pointer to structure
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
stdret_t vfs_stat(const char *path, struct vfs_statf *stat)
{
        char *external_path = NULL;
        struct FS_data *fs;

        if (path && stat) {
                force_lock_mutex(vfs_resource_mtx, MTX_BLOCK_TIME);
                fs = find_base_FS(path, &external_path);
                unlock_mutex(vfs_resource_mtx);

                if (fs) {
                        if (fs->interface.fs_stat) {
                                return fs->interface.fs_stat(fs->handle, external_path, stat);
                        }
                }
        }

        return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief Function returns file system status
 *
 * @param[in]  *path            fs path
 * @param[out] *statfs          pointer to FS status structure
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
stdret_t vfs_statfs(const char *path, struct vfs_statfs *statfs)
{
        char *new_path;
        struct FS_data *fs;

        if (path && statfs) {
                if ((new_path = new_corrected_path(path, ADD_SLASH))) {
                        force_lock_mutex(vfs_resource_mtx, MTX_BLOCK_TIME);
                        fs = find_mounted_FS(new_path, -1, NULL);
                        unlock_mutex(vfs_resource_mtx);
                        free(new_path);

                        if (fs) {
                                if (fs->interface.fs_statfs) {
                                        return fs->interface.fs_statfs(fs->handle, statfs);
                                }
                        }
                }
        }

        return STD_RET_ERROR;
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
file_t *vfs_fopen(const char *path, const char *mode)
{
        file_t *file;
        char   *external_path = NULL;
        struct FS_data *fs;

        if (!path || !mode) {
                return NULL;
        }

        if (path[strlen(path) - 1] == '/') { /* path is directory */
                return NULL;
        }

        if (  strncmp("r", mode, 2) != 0 && strncmp("r+", mode, 2) != 0
           && strncmp("w", mode, 2) != 0 && strncmp("w+", mode, 2) != 0
           && strncmp("a", mode, 2) != 0 && strncmp("a+", mode, 2) != 0) {

                return NULL;
        }

        if ((file = calloc(1, sizeof(file_t)))) {
                force_lock_mutex(vfs_resource_mtx, MTX_BLOCK_TIME);
                fs = find_base_FS(path, &external_path);
                unlock_mutex(vfs_resource_mtx);

                if (fs == NULL) {
                        goto vfs_open_error;
                }

                if (fs->interface.fs_open == NULL) {
                        goto vfs_open_error;
                }

                if (fs->interface.fs_open(fs->handle, &file->fd, &file->f_seek,
                                          external_path, mode) == STD_RET_OK) {

                        file->FS_hdl   = fs->handle;
                        file->f_close = fs->interface.fs_close;
                        file->f_ioctl = fs->interface.fs_ioctl;
                        file->f_stat  = fs->interface.fs_fstat;

                        if (strncmp("r", mode, 2) != 0) {
                                file->f_write = fs->interface.fs_write;
                        }

                        if (  strncmp("w", mode, 2) != 0
                           && strncmp("a", mode, 2) != 0) {
                                file->f_read  = fs->interface.fs_read;
                        }

                        return file;
                }

                vfs_open_error:
                free(file);
        }

        return NULL;
}

//==============================================================================
/**
 * @brief Function close opened file
 *
 * @param[in] *file             pinter to file
 *
 * @retval STD_RET_OK           file closed successfully
 * @retval STD_RET_ERROR        file not closed
 */
//==============================================================================
stdret_t vfs_fclose(file_t *file)
{
        if (file) {
                if (file->f_close) {
                        if (file->f_close(file->FS_hdl, file->fd) == STD_RET_OK) {
                                free(file);
                                return STD_RET_OK;
                        }
                }
        }

        return STD_RET_ERROR;
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
 * @return STD_RET_OK or 0 if write finished successfully, otherwise > 0
 */
//==============================================================================
size_t vfs_fwrite(void *ptr, size_t size, size_t nitems, file_t *file)
{
        size_t n = 0;

        if (ptr && size && nitems && file) {
                if (file->f_write) {
                        n = file->f_write(file->FS_hdl, file->fd, ptr, size,
                                          nitems, file->f_seek);
                        file->f_seek += n * size;
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
 * @return number of read items
 */
//==============================================================================
size_t vfs_fread(void *ptr, size_t size, size_t nitems, file_t *file)
{
        size_t n = 0;

        if (ptr && size && nitems && file) {
                if (file->f_read) {
                        n = file->f_read(file->FS_hdl, file->fd, ptr, size,
                                         nitems, file->f_seek);
                        file->f_seek += n * size;
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
 * @retval STD_RET_OK           seek moved successfully
 * @retval STD_RET_ERROR        error occurred
 */
//==============================================================================
stdret_t vfs_fseek(file_t *file, i32_t offset, int mode)
{
        struct vfs_statf stat;

        if (file) {
                if (mode == VFS_SEEK_END) {
                        stat.st_size = 0;

                        if (vfs_fstat(file, &stat) != STD_RET_OK) {
                                return STD_RET_ERROR;
                        }
                }

                switch (mode) {
                case VFS_SEEK_SET: file->f_seek  = offset; break;
                case VFS_SEEK_CUR: file->f_seek += offset; break;
                case VFS_SEEK_END: file->f_seek  = stat.st_size + offset; break;
                default: return STD_RET_ERROR;
                }

                return STD_RET_OK;
        }

        return STD_RET_ERROR;
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
i32_t vfs_ftell(file_t *file)
{
        if (file)
                return file->f_seek;
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
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
stdret_t vfs_ioctl(file_t *file, iorq_t rq, ...)
{
        va_list  args;
        stdret_t status = STD_RET_ERROR;

        if (file) {
                if (file->f_ioctl) {
                        va_start(args, rq);
                        status = file->f_ioctl(file->FS_hdl, file->fd, rq, args);
                        va_end(args);
                }
        }

        return status;
}

//==============================================================================
/**
 * @brief Function returns file/dir status
 *
 * @param[in]  *path            file/dir path
 * @param[out] *stat            pointer to stat structure
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
stdret_t vfs_fstat(file_t *file, struct vfs_statf *stat)
{
        if (file && stat) {
                if (file->f_stat) {
                        return file->f_stat(file->FS_hdl, file->fd, stat);
                }
        }

        return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief Function flush file data
 *
 * @param[in] *file     file to flush
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
stdret_t vfs_fflush(file_t *file)
{
        if (file) {
                if (file->f_flush) {
                        return file->f_flush(file->FS_hdl, file->fd);
                }
        }

        return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief Function check end of file
 *
 * @param[in] *file     file
 *
 * @return 0 if there is not a file end, otherwise greather than 0
 */
//==============================================================================
int vfs_feof(file_t *file)
{
        if (file) {
                u32_t seek  = vfs_ftell(file);
                u32_t fsize = vfs_fseek(file, 0, SEEK_END);
                vfs_fseek(file, seek, SEEK_SET);

                if (seek >= fsize) {
                        return 1;
                }
        }

        return 0;
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
 * @brief Function create new path with slash correction
 *
 * @param[in] *path             path to correct
 * @param[in]  corr             path correction kind
 *
 * @return pointer to new path
 */
//==============================================================================
static char *new_corrected_path(const char *path, enum path_correction corr)
{
        char *new_path;
        uint  new_path_len = strlen(path);

        if (corr == SUB_SLASH) {
                if (path[new_path_len - 1] == '/') {
                        new_path_len--;
                }
        } else if (corr == ADD_SLASH) {
                if (path[new_path_len - 1] != '/') {
                        new_path_len++;
                }
        } else {
                return NULL;
        }

        new_path = calloc(new_path_len + 1, sizeof(char));
        if (new_path) {
                if (corr == SUB_SLASH) {
                        strncpy(new_path, path, new_path_len);
                } else if (corr == ADD_SLASH) {
                        strcpy(new_path, path);

                        if (new_path_len > strlen(path)) {
                                strcat(new_path, "/");
                        }
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
