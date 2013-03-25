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
#include "vfs.h"
#include "dlist.h"
#include "oswrap.h"
#include "sysmoni.h"
#include <string.h>

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
        void     *fshdl;
        stdret_t (*f_close)(void *fshdl, fd_t fd);
        size_t   (*f_write)(void *fshdl, fd_t fd, void *src, size_t size, size_t nitems, size_t seek);
        size_t   (*f_read )(void *fshdl, fd_t fd, void *dst, size_t size, size_t nitmes, size_t seek);
        stdret_t (*f_ioctl)(void *fshdl, fd_t fd, iorq_t iorq, void *data);
        stdret_t (*f_stat )(void *fshdl, fd_t fd, struct vfs_statf *stat);
        stdret_t (*f_flush)(void *fshdl, fd_t fd);
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
        struct FS_data *mountfs;
        struct FS_data *basefs;
        struct FS_data *newfs   = NULL;
        char           *newpath = NULL;
        char           *extPath;

        if (!mount_point || !fs_interface) {
                return STD_RET_ERROR;
        }

        if (!(newpath = new_corrected_path(mount_point, ADD_SLASH))) {
                return STD_RET_ERROR;
        }

        force_lock_mutex(vfs_resource_mtx, MTX_BLOCK_TIME);
        mountfs = find_mounted_FS(newpath, -1, NULL);
        basefs  = find_base_FS(newpath, &extPath);

        /*
         * create new FS in existing DIR and FS, otherwise create new FS if
         * first mount
         */
        if (basefs && mountfs == NULL) {
                if (basefs->interface.fs_opendir && extPath) {

                        if (basefs->interface.fs_opendir(basefs->handle,
                                                         extPath,
                                                         NULL) == STD_RET_OK) {

                                newfs = calloc(1, sizeof(struct FS_data));
                                basefs->mounted_FS_counter++;
                        }
                }
        } else if (  list_get_item_count(vfs_mnt_list) == 0
                  && strlen(newpath) == 1
                  && newpath[0] == '/' ) {

                newfs = calloc(1, sizeof(struct FS_data));
        }

        /*
         * mount FS if created
         */
        if (newfs && fs_interface->fs_init) {
                newfs->interface = *fs_interface;

                if (fs_interface->fs_init(&newfs->handle, src_path) == STD_RET_OK) {
                        newfs->mount_point = newpath;
                        newfs->base_FS     = basefs;
                        newfs->mounted_FS_counter = 0;

                        if (list_add_item(vfs_mnt_list, vfs_id_counter++, newfs) >= 0) {
                                unlock_mutex(vfs_resource_mtx);
                                return STD_RET_OK;
                        }
                }

                free(newfs);
                free(newpath);
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
        u32_t          itemid;
        char           *newpath;
        struct FS_data *mountfs;

        if (!path) {
                return STD_RET_ERROR;
        }

        if (path[0] != '/') {
                return STD_RET_ERROR;
        }

        newpath = new_corrected_path(path, ADD_SLASH);
        if (newpath == NULL) {
                return STD_RET_ERROR;
        }

        force_lock_mutex(vfs_resource_mtx, MTX_BLOCK_TIME);
        mountfs = find_mounted_FS(newpath, -1, &itemid);
        free(newpath);

        if (mountfs == NULL) {
                goto vfs_umount_error;
        }

        if (mountfs->interface.fs_release && mountfs->mounted_FS_counter == 0) {
                if (mountfs->interface.fs_release(mountfs->handle) != STD_RET_OK) {
                        goto vfs_umount_error;
                }

                mountfs->handle = NULL;

                if (mountfs->base_FS) {
                        if (mountfs->base_FS->mounted_FS_counter) {
                                mountfs->base_FS->mounted_FS_counter--;
                        }
                }

                if (mountfs->mount_point) {
                        free(mountfs->mount_point);
                }

                if (list_rm_iditem(vfs_mnt_list, itemid) == STD_RET_OK) {
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
        struct vfs_statfs statfs;

        if (mntent) {
                force_lock_mutex(vfs_resource_mtx, MTX_BLOCK_TIME);
                fs = list_get_nitem_data(vfs_mnt_list, item);
                unlock_mutex(vfs_resource_mtx);

                if (fs) {
                        statfs.fsname = NULL;

                        if (fs->interface.fs_statfs) {
                                fs->interface.fs_statfs(fs->handle, &statfs);
                        }

                        if (statfs.fsname) {
                                if (strlen(fs->mount_point) > 1) {
                                        strncpy(mntent->mnt_dir, fs->mount_point,
                                                strlen(fs->mount_point) - 1);
                                } else {
                                        strcpy(mntent->mnt_dir, fs->mount_point);
                                }

                                strcpy(mntent->mnt_fsname, statfs.fsname);
                                mntent->free  = statfs.f_bfree;
                                mntent->total = statfs.f_blocks;

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
        char           *extPath = NULL;
        struct FS_data *fs;

        if (path && drv_interface) {
                force_lock_mutex(vfs_resource_mtx, MTX_BLOCK_TIME);
                fs = find_base_FS(path, &extPath);
                unlock_mutex(vfs_resource_mtx);

                if (fs) {
                        if (fs->interface.fs_mknod) {
                                return fs->interface.fs_mknod(fs->handle,
                                                             extPath,
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
        stdret_t       status   = STD_RET_ERROR;
        char           *newpath;
        char           *extPath = NULL;
        struct FS_data *fs;

        if (path) {
                if (!(newpath = new_corrected_path(path, SUB_SLASH))) {
                       return STD_RET_ERROR;
                }

                force_lock_mutex(vfs_resource_mtx, MTX_BLOCK_TIME);
                fs = find_base_FS(newpath, &extPath);
                unlock_mutex(vfs_resource_mtx);

                if (fs) {
                        if (fs->interface.fs_mkdir) {
                                status = fs->interface.fs_mkdir(fs->handle, extPath);
                        }
                }

                free(newpath);
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
        stdret_t       status   = STD_RET_ERROR;
        dir_t          *dir     = NULL;
        char           *extPath = NULL;
        char           *newpath;
        struct FS_data *fs;

        if (!path) {
                return NULL;
        }

        if ((dir = malloc(sizeof(dir_t)))) {
                if ((newpath = new_corrected_path(path, ADD_SLASH))) {
                        force_lock_mutex(vfs_resource_mtx, MTX_BLOCK_TIME);
                        fs = find_base_FS(newpath, &extPath);
                        unlock_mutex(vfs_resource_mtx);

                        if (fs) {
                                dir->handle = fs->handle;

                                if (fs->interface.fs_opendir) {
                                        status = fs->interface.fs_opendir(fs->handle, extPath, dir);
                                }
                        }

                        free(newpath);
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
        stdret_t       status   = STD_RET_ERROR;
        char           *extPath = NULL;
        char           *newpath;
        struct FS_data *mntfs;
        struct FS_data *basefs;

        if (path) {
                if ((newpath = new_corrected_path(path, ADD_SLASH))) {
                        force_lock_mutex(vfs_resource_mtx, MTX_BLOCK_TIME);
                        mntfs  = find_mounted_FS(newpath, -1, NULL);
                        basefs = find_base_FS(path, &extPath);
                        unlock_mutex(vfs_resource_mtx);

                        if (basefs && !mntfs) {
                                if (basefs->interface.fs_remove) {
                                        status = basefs->interface.fs_remove(basefs->handle, extPath);
                                }
                        }

                        free(newpath);
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
        char *extPathOld = NULL;
        char *extPathNew = NULL;
        struct FS_data *fsOld;
        struct FS_data *fsNew;

        if (oldName && newName) {
                force_lock_mutex(vfs_resource_mtx, MTX_BLOCK_TIME);
                fsOld = find_base_FS(oldName, &extPathOld);
                fsNew = find_base_FS(newName, &extPathNew);
                unlock_mutex(vfs_resource_mtx);

                if (fsOld && fsNew && fsOld == fsNew) {
                        if (fsOld->interface.fs_rename) {
                                return fsOld->interface.fs_rename(fsOld->handle,
                                                                 extPathOld,
                                                                 extPathNew);
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
        char *extPath = NULL;
        struct FS_data *fs;

        if (path) {
                force_lock_mutex(vfs_resource_mtx, MTX_BLOCK_TIME);
                fs = find_base_FS(path, &extPath);
                unlock_mutex(vfs_resource_mtx);

                if (fs) {
                        if (fs->interface.fs_chmod) {
                                return fs->interface.fs_chmod(fs->handle, extPath, mode);
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
        char *extPath = NULL;
        struct FS_data *fs;

        if (path) {
                force_lock_mutex(vfs_resource_mtx, MTX_BLOCK_TIME);
                fs = find_base_FS(path, &extPath);
                unlock_mutex(vfs_resource_mtx);

                if (fs) {
                        if (fs->interface.fs_chown) {
                                return fs->interface.fs_chown(fs->handle, extPath, owner, group);
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
        char *extPath = NULL;
        struct FS_data *fs;

        if (path && stat) {
                force_lock_mutex(vfs_resource_mtx, MTX_BLOCK_TIME);
                fs = find_base_FS(path, &extPath);
                unlock_mutex(vfs_resource_mtx);

                if (fs) {
                        if (fs->interface.fs_stat) {
                                return fs->interface.fs_stat(fs->handle, extPath, stat);
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
        char *newpath;
        struct FS_data *fs;

        if (path && statfs) {
                if ((newpath = new_corrected_path(path, ADD_SLASH))) {
                        force_lock_mutex(vfs_resource_mtx, MTX_BLOCK_TIME);
                        fs = find_mounted_FS(newpath, -1, NULL);
                        unlock_mutex(vfs_resource_mtx);
                        free(newpath);

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
        char   *extPath = NULL;
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
                fs = find_base_FS(path, &extPath);
                unlock_mutex(vfs_resource_mtx);

                if (fs == NULL) {
                        goto vfs_open_error;
                }

                if (fs->interface.fs_open == NULL) {
                        goto vfs_open_error;
                }

                if (fs->interface.fs_open(fs->handle, &file->fd, &file->f_seek,
                                          extPath, mode) == STD_RET_OK) {

                        file->fshdl   = fs->handle;
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
                        if (file->f_close(file->fshdl, file->fd) == STD_RET_OK) {
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
                        n = file->f_write(file->fshdl, file->fd, ptr, size,
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
                        n = file->f_read(file->fshdl, file->fd, ptr, size,
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
 * @param[in,out] *data         pointer to data
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
stdret_t vfs_ioctl(file_t *file, iorq_t rq, void *data)
{
        if (file) {
                if (file->f_ioctl) {
                        return file->f_ioctl(file->fshdl, file->fd, rq, data);
                }
        }

        return STD_RET_ERROR;
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
                        return file->f_stat(file->fshdl, file->fd, stat);
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
                        return file->f_flush(file->fshdl, file->fd);
                }
        }

        return STD_RET_ERROR;
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
        struct FS_data *fsinfo = NULL;

        int icount = list_get_item_count(vfs_mnt_list);

        for (int i = 0; i < icount; i++) {

                struct FS_data *data = list_get_nitem_data(vfs_mnt_list, i);

                if (strncmp(path, data->mount_point, len) != 0) {
                        continue;
                }

                fsinfo = data;

                if (itemid) {
                        if (list_get_nitem_ID(vfs_mnt_list, i, itemid) != STD_RET_OK) {
                                fsinfo = NULL;
                        }
                }

                break;
        }

        return fsinfo;
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
        struct FS_data *fsinfo = NULL;

        char *pathTail = (char*)path + strlen(path);

        if (*(pathTail - 1) == '/') {
                pathTail--;
        }

        while (pathTail >= path) {
                struct FS_data *fs = find_mounted_FS(path, pathTail - path + 1, NULL);

                if (fs) {
                        fsinfo = fs;
                        break;
                } else {
                        while (*(--pathTail) != '/' && pathTail >= path);
                }
        }

        if (fsinfo && extPath) {
                *extPath = pathTail;
        }

        return fsinfo;
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
