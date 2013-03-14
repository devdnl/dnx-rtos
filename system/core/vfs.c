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
#include "memman.h"
#include <string.h>

/*==============================================================================
  Local symbolic constants/macros
==============================================================================*/
/* wait time for operation on VFS */
#define MTX_BLOCK_TIME                        10
#define force_lock_mutex(mtx, blocktime)      while (lock_mutex(mtx, blocktime) != MUTEX_LOCKED)

#define calloc(nmemb, msize)                  memman_calloc(nmemb, msize)
#define malloc(size)                          memman_malloc(size)
#define free(mem)                             memman_free(mem)

/*==============================================================================
  Local types, enums definitions
==============================================================================*/
/** file type */
struct vfs_file
{
        uint     dev;   /* device number or file system description */
        uint     fd;    /* device part number or file description   */
        stdret_t (*f_close)(uint dev, uint fd);
        size_t   (*f_write)(uint dev, uint fd, void *src, size_t size, size_t nitems, size_t seek);
        size_t   (*f_read )(uint dev, uint fd, void *dst, size_t size, size_t nitmes, size_t seek);
        stdret_t (*f_ioctl)(uint dev, uint fd, iorq_t iorq, void *data);
        stdret_t (*f_stat )(uint dev, uint fd, void *stat);
        size_t   f_seek;
};

struct FS_data {
          char             *mount_point;
          struct FS_data   *base_FS;
          struct vfs_fscfg  FS_config;
          u8_t              mounted_FS_counter;
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
 * @param[in] *srcPath          path to source file when file system load data
 * @param[in] *mntPoint         path when dir shall be mounted
 * @param[in] *mountcfg         pointer to description of mount FS
 *
 * @retval STD_RET_OK           mount success
 * @retval STD_RET_ERROR        mount error
 */
//==============================================================================
stdret_t vfs_mount(const char *srcPath, const char *mntPoint, struct vfs_fscfg *mountcfg)
{
        struct FS_data *mountfs;
        struct FS_data *basefs;
        struct FS_data *newfs   = NULL;
        char           *newpath = NULL;
        char           *extPath;

        if (!mntPoint || !mountcfg) {
                return STD_RET_ERROR;
        }

        if (!(newpath = new_corrected_path(mntPoint, ADD_SLASH))) {
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
                if (basefs->FS_config.f_opendir && extPath) {

                        if (basefs->FS_config.f_opendir(basefs->FS_config.f_fsd,
                                                 extPath, NULL) == STD_RET_OK) {

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
        if (newfs && mountcfg->f_init) {
                newfs->FS_config = *mountcfg;

                if (mountcfg->f_init(srcPath, &newfs->FS_config.f_fsd) == STD_RET_OK) {
                        newfs->mount_point  = newpath;
                        newfs->base_FS    = basefs;
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

        if (mountfs->FS_config.f_release && mountfs->mounted_FS_counter == 0) {
                if (mountfs->FS_config.f_release(mountfs->FS_config.f_fsd) != STD_RET_OK) {
                        goto vfs_umount_error;
                }

                /* decrease mount points if base FS exist */
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
        if (mntent) {
                struct FS_data *fs = NULL;

                force_lock_mutex(vfs_resource_mtx, MTX_BLOCK_TIME);
                fs = list_get_nitem_data(vfs_mnt_list, item);
                unlock_mutex(vfs_resource_mtx);

                if (fs) {
                        struct vfs_statfs statfs;
                        statfs.fsname = NULL;

                        if (fs->FS_config.f_statfs) {
                                fs->FS_config.f_statfs(fs->FS_config.f_fsd, &statfs);
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
stdret_t vfs_mknod(const char *path, struct vfs_drvcfg *drvcfg)
{
        if (path && drvcfg) {
                char *extPath = NULL;

                force_lock_mutex(vfs_resource_mtx, MTX_BLOCK_TIME);
                struct FS_data *fs = find_base_FS(path, &extPath);
                unlock_mutex(vfs_resource_mtx);

                if (fs) {
                        if (fs->FS_config.f_mknod) {
                                return fs->FS_config.f_mknod(fs->FS_config.f_fsd,
                                                             extPath, drvcfg);
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
        stdret_t status = STD_RET_ERROR;

        if (path) {
                char *newpath = new_corrected_path(path, SUB_SLASH);
                if (newpath == NULL) {
                       return STD_RET_ERROR;
                }

                char *extPath = NULL;

                force_lock_mutex(vfs_resource_mtx, MTX_BLOCK_TIME);
                struct FS_data *fs = find_base_FS(newpath, &extPath);
                unlock_mutex(vfs_resource_mtx);

                if (fs) {
                        if (fs->FS_config.f_mkdir) {
                                status = fs->FS_config.f_mkdir(fs->FS_config.f_fsd,
                                                               extPath);
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
        stdret_t status = STD_RET_ERROR;
        dir_t *dir      = NULL;

        if (!path) {
                return NULL;
        }

        dir = malloc(sizeof(dir_t));
        if (dir) {
                char *newpath = new_corrected_path(path, ADD_SLASH);

                if (newpath) {
                        char *extPath = NULL;

                        force_lock_mutex(vfs_resource_mtx, MTX_BLOCK_TIME);
                        struct FS_data *fs = find_base_FS(newpath, &extPath);
                        unlock_mutex(vfs_resource_mtx);

                        if (fs) {
                                dir->fsd = fs->FS_config.f_fsd;

                                if (fs->FS_config.f_opendir) {
                                        status = fs->FS_config.f_opendir(fs->FS_config.f_fsd,
                                                                         extPath, dir);
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
                        if (dir->cldir(dir->fsd, dir) == STD_RET_OK) {
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
                direntry = dir->rddir(dir->fsd, dir);
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
        stdret_t status = STD_RET_ERROR;

        if (path) {
                char *newpath = new_corrected_path(path, ADD_SLASH);
                if (newpath) {
                        char *extPath = NULL;

                        force_lock_mutex(vfs_resource_mtx, MTX_BLOCK_TIME);
                        struct FS_data *mntfs  = find_mounted_FS(newpath, -1, NULL);
                        struct FS_data *basefs = find_base_FS(path, &extPath);
                        unlock_mutex(vfs_resource_mtx);

                        if (basefs && mntfs == NULL) {
                                if (basefs->FS_config.f_remove) {
                                        status = basefs->FS_config.f_remove(basefs->FS_config.f_fsd,
                                                                            extPath);
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
        if (oldName && newName) {
                char *extPathOld = NULL;
                char *extPathNew = NULL;

                force_lock_mutex(vfs_resource_mtx, MTX_BLOCK_TIME);
                struct FS_data *fsOld = find_base_FS(oldName, &extPathOld);
                struct FS_data *fsNew = find_base_FS(newName, &extPathNew);
                unlock_mutex(vfs_resource_mtx);

                if (fsOld && fsNew && fsOld == fsNew) {
                        if (fsOld->FS_config.f_rename) {
                                return fsOld->FS_config.f_rename(fsOld->FS_config.f_fsd,
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
        if (path) {
                char *extPath = NULL;

                force_lock_mutex(vfs_resource_mtx, MTX_BLOCK_TIME);
                struct FS_data *fs = find_base_FS(path, &extPath);
                unlock_mutex(vfs_resource_mtx);

                if (fs) {
                        if (fs->FS_config.f_chmod) {
                                return fs->FS_config.f_chmod(fs->FS_config.f_fsd,
                                                             extPath, mode);
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
        if (path) {
                char *extPath = NULL;

                force_lock_mutex(vfs_resource_mtx, MTX_BLOCK_TIME);
                struct FS_data *fs = find_base_FS(path, &extPath);
                unlock_mutex(vfs_resource_mtx);

                if (fs) {
                        if (fs->FS_config.f_chown) {
                                return fs->FS_config.f_chown(fs->FS_config.f_fsd, extPath,
                                                             owner, group);
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
stdret_t vfs_stat(const char *path, struct vfs_stat *stat)
{
        if (path && stat) {
                char *extPath = NULL;

                force_lock_mutex(vfs_resource_mtx, MTX_BLOCK_TIME);
                struct FS_data *fs = find_base_FS(path, &extPath);
                unlock_mutex(vfs_resource_mtx);

                if (fs) {
                        if (fs->FS_config.f_stat) {
                                return fs->FS_config.f_stat(fs->FS_config.f_fsd, extPath, stat);
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
        if (path && statfs) {
                char *newpath = new_corrected_path(path, ADD_SLASH);
                if (newpath) {
                        force_lock_mutex(vfs_resource_mtx, MTX_BLOCK_TIME);
                        struct FS_data *fs = find_mounted_FS(newpath, -1, NULL);
                        unlock_mutex(vfs_resource_mtx);
                        free(newpath);

                        if (fs) {
                                if (fs->FS_config.f_statfs) {
                                        return fs->FS_config.f_statfs(fs->FS_config.f_fsd, statfs);
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
                char *extPath = NULL;

                force_lock_mutex(vfs_resource_mtx, MTX_BLOCK_TIME);
                struct FS_data *fs = find_base_FS(path, &extPath);
                unlock_mutex(vfs_resource_mtx);

                if (fs == NULL) {
                        goto vfs_open_error;
                }

                if (fs->FS_config.f_open == NULL) {
                        goto vfs_open_error;
                }

                if (fs->FS_config.f_open(fs->FS_config.f_fsd, &file->fd, &file->f_seek,
                                  extPath, mode) == STD_RET_OK) {

                        file->dev     = fs->FS_config.f_fsd;
                        file->f_close = fs->FS_config.f_close;
                        file->f_ioctl = fs->FS_config.f_ioctl;
                        file->f_stat  = (void*)fs->FS_config.f_fstat;

                        if (strncmp("r", mode, 2) != 0) {
                                file->f_write = fs->FS_config.f_write;
                        }

                        if (  strncmp("w", mode, 2) != 0
                           && strncmp("a", mode, 2) != 0) {
                                file->f_read  = fs->FS_config.f_read;
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
                        if (file->f_close(file->dev, file->fd) == STD_RET_OK) {
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
                        n = file->f_write(file->dev, file->fd, ptr, size,
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
                        n = file->f_read(file->dev, file->fd, ptr, size,
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
        if (file) {
                struct vfs_stat stat;

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
                        return file->f_ioctl(file->dev, file->fd, rq, data);
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
stdret_t vfs_fstat(file_t *file, struct vfs_stat *stat)
{
        if (file && stat) {
                if (file->f_stat) {
                        return file->f_stat(file->dev, file->fd, stat);
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
