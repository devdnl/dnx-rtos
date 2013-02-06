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

#define calloc(nmemb, msize)                  memman_calloc(nmemb, msize)
#define malloc(size)                          memman_malloc(size)
#define free(mem)                             memman_free(mem)

/*==============================================================================
  Local types, enums definitions
==============================================================================*/
struct fsinfo {
          ch_t             *mntpoint;
          struct fsinfo    *basefs;
          struct vfs_fscfg  fs;
          u8_t              mntFSCnt;
};

struct vfshdl {
          list_t  *mntList;
          mutex_t  mtx;
          u32_t    idcnt;
};

enum pathCorrection {
          ADD_SLASH,
          SUB_SLASH,
};

/*==============================================================================
  Local function prototypes
==============================================================================*/
static struct fsinfo *FindMountedFS(const ch_t *path, u16_t len, u32_t *itemid);
static struct fsinfo *FindBaseFS(const ch_t *path, ch_t **extPath);
static ch_t          *NewCorrectedPath(const ch_t *path, enum pathCorrection corr);

/*==============================================================================
  Local object definitions
==============================================================================*/
static struct vfshdl *vfs;

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
stdRet_t vfs_init(void)
{
        if (vfs != NULL) {
                return STD_RET_OK;
        }

        vfs = calloc(1, sizeof(struct vfshdl));

        if (vfs) {
                vfs->mtx     = CreateMutex();
                vfs->mntList = ListCreate();

                if (!vfs->mtx || !vfs->mntList) {
                        if (vfs->mtx) {
                                DeleteMutex(vfs->mtx);
                        }

                        if (vfs->mntList) {
                                ListDelete(vfs->mntList);
                        }

                        free(vfs);
                        vfs = NULL;
                        return STD_RET_ERROR;
                }

                return STD_RET_OK;
        }

        return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief Function mount file system in VFS
 * External file system is mounted to empty directory. If directory is not empty
 * mounting is not possible.
 *
 * @param *srcPath              path to source file when file system load data
 * @param *mntPoint             path when dir shall be mounted
 * @param *mountcfg             pointer to description of mount FS
 *
 * @retval STD_RET_OK           mount success
 * @retval STD_RET_ERROR        mount error
 */
//==============================================================================
stdRet_t vfs_mount(const ch_t *srcPath, const ch_t *mntPoint, struct vfs_fscfg *mountcfg)
{
        struct fsinfo *mountfs;
        struct fsinfo *basefs;
        struct fsinfo *newfs   = NULL;
        ch_t          *newpath = NULL;
        ch_t          *extPath;

        if (!mntPoint || !mountcfg || !vfs) {
                return STD_RET_ERROR;
        }

        newpath = NewCorrectedPath(mntPoint, ADD_SLASH);
        if (newpath == NULL) {
                return STD_RET_ERROR;
        }

        while (TakeMutex(vfs->mtx, MTX_BLOCK_TIME) != OS_OK);
        mountfs = FindMountedFS(newpath, -1, NULL);
        basefs  = FindBaseFS(newpath, &extPath);

        /*
         * create new FS in existing DIR and FS, otherwise create new FS if
         * first mount
         */
        if (basefs && mountfs == NULL) {
                if (basefs->fs.f_opendir && extPath) {

                        if (basefs->fs.f_opendir(basefs->fs.f_fsd,
                                                 extPath, NULL) == STD_RET_OK) {

                                newfs = calloc(1, sizeof(struct fsinfo));
                                basefs->mntFSCnt++;
                        }
                }
        } else if (  ListGetItemCount(vfs->mntList) == 0
                  && strlen(newpath) == 1
                  && newpath[0] == '/' ) {

                newfs = calloc(1, sizeof(struct fsinfo));
        }

        /*
         * mount FS if created
         */
        if (newfs && mountcfg->f_init) {
                newfs->fs = *mountcfg;

                if (mountcfg->f_init(srcPath, &newfs->fs.f_fsd) == STD_RET_OK) {
                        newfs->mntpoint = newpath;
                        newfs->basefs   = basefs;
                        newfs->mntFSCnt = 0;

                        if (ListAddItem(vfs->mntList, vfs->idcnt++, newfs) >= 0) {
                                GiveMutex(vfs->mtx);
                                return STD_RET_OK;
                        }
                }

                free(newfs);
                free(newpath);
        }

        GiveMutex(vfs->mtx);
        return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief Function unmount dir from file system
 *
 * @param *path                 dir path
 *
 * @retval STD_RET_OK           mount success
 * @retval STD_RET_ERROR        mount error
 */
//==============================================================================
stdRet_t vfs_umount(const ch_t *path)
{
        u32_t          itemid;
        ch_t          *newpath;
        struct fsinfo *mountfs;

        if (!path || !vfs) {
                return STD_RET_ERROR;
        }

        if (path[0] != '/') {
                return STD_RET_ERROR;
        }

        newpath = NewCorrectedPath(path, ADD_SLASH);
        if (newpath == NULL) {
                return STD_RET_ERROR;
        }

        while (TakeMutex(vfs->mtx, MTX_BLOCK_TIME) != OS_OK);
        mountfs = FindMountedFS(newpath, -1, &itemid);
        free(newpath);

        if (mountfs) {
                if (mountfs->fs.f_release && mountfs->mntFSCnt == 0) {
                        if (mountfs->fs.f_release(mountfs->fs.f_fsd) == STD_RET_OK) {

                                /* decrease mount points if base FS exist */
                                if (mountfs->basefs) {
                                        if (mountfs->basefs->mntFSCnt) {
                                                mountfs->basefs->mntFSCnt--;
                                        }
                                }

                                if (mountfs->mntpoint) {
                                        free(mountfs->mntpoint);
                                }

                                if (ListRmItemByID(vfs->mntList, itemid) == 0) {
                                        GiveMutex(vfs->mtx);
                                        return STD_RET_OK;
                                }
                        }
                }
        }

        GiveMutex(vfs->mtx);
        return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief Function gets mount point for n item
 *
 * @param  item           mount point number
 * @param *mntent         mount entry data
 */
//==============================================================================
stdRet_t vfs_getmntentry(size_t item, struct vfs_mntent *mntent)
{
        if (mntent) {
                struct fsinfo *fs = NULL;

                while (TakeMutex(vfs->mtx, MTX_BLOCK_TIME) != OS_OK);
                fs = ListGetItemDataByNo(vfs->mntList, item);
                GiveMutex(vfs->mtx);

                if (fs) {
                        struct vfs_statfs statfs;
                        statfs.fsname = NULL;

                        if (fs->fs.f_statfs) {
                                fs->fs.f_statfs(fs->fs.f_fsd, &statfs);
                        }

                        if (statfs.fsname) {
                                if (strlen(fs->mntpoint) > 1) {
                                        strncpy(mntent->mnt_dir, fs->mntpoint,
                                                strlen(fs->mntpoint) - 1);
                                } else {
                                        strcpy(mntent->mnt_dir, fs->mntpoint);
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
 * @param *path                   path when driver-file shall be created
 * @param *drvcfg                 pointer to description of driver
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
stdRet_t vfs_mknod(const ch_t *path, struct vfs_drvcfg *drvcfg)
{
        if (path && drvcfg && vfs) {
                ch_t *extPath = NULL;

                while (TakeMutex(vfs->mtx, MTX_BLOCK_TIME) != OS_OK);
                struct fsinfo *fs = FindBaseFS(path, &extPath);
                GiveMutex(vfs->mtx);

                if (fs) {
                        if (fs->fs.f_mknod) {
                                return fs->fs.f_mknod(fs->fs.f_fsd, extPath,
                                                      drvcfg);
                        }
                }
        }

        return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief Create directory
 *
 * @param *path   path to new directory
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
stdRet_t vfs_mkdir(const ch_t *path)
{
        stdRet_t status = STD_RET_ERROR;

        if (path && vfs) {
                ch_t *newpath = NewCorrectedPath(path, SUB_SLASH);
                if (newpath == NULL) {
                       return STD_RET_ERROR;
                }

                ch_t *extPath = NULL;

                while (TakeMutex(vfs->mtx, MTX_BLOCK_TIME) != OS_OK);
                struct fsinfo *fs = FindBaseFS(newpath, &extPath);
                GiveMutex(vfs->mtx);

                if (fs) {
                        if (fs->fs.f_mkdir) {
                                status = fs->fs.f_mkdir(fs->fs.f_fsd, extPath);
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
 * @param *path           directory path
 *
 * @return directory object
 */
//==============================================================================
DIR_t *vfs_opendir(const ch_t *path)
{
        stdRet_t status = STD_RET_ERROR;
        DIR_t *dir      = NULL;

        if (!path || !vfs) {
                return NULL;
        }

        dir = malloc(sizeof(DIR_t));
        if (dir) {
                ch_t *newpath = NewCorrectedPath(path, ADD_SLASH);

                if (newpath) {
                        ch_t *extPath = NULL;

                        while (TakeMutex(vfs->mtx, MTX_BLOCK_TIME) != OS_OK);
                        struct fsinfo *fs = FindBaseFS(newpath, &extPath);
                        GiveMutex(vfs->mtx);

                        if (fs) {
                                dir->fsd = fs->fs.f_fsd;

                                if (fs->fs.f_opendir) {
                                        status = fs->fs.f_opendir(fs->fs.f_fsd,
                                                                  extPath,
                                                                  dir);
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
 * @param *dir            directory object
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
stdRet_t vfs_closedir(DIR_t *dir)
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
 * @param *dir            directory object
 *
 * @return element attributes
 */
//==============================================================================
dirent_t vfs_readdir(DIR_t *dir)
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
 * @param *patch          localization of file/directory
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
stdRet_t vfs_remove(const ch_t *path)
{
        stdRet_t status = STD_RET_ERROR;

        if (path && vfs) {
                ch_t *newpath = NewCorrectedPath(path, ADD_SLASH);
                if (newpath) {
                        ch_t *extPath = NULL;

                        while (TakeMutex(vfs->mtx, MTX_BLOCK_TIME) != OS_OK);
                        struct fsinfo *mntfs  = FindMountedFS(newpath, -1, NULL);
                        struct fsinfo *basefs = FindBaseFS(path, &extPath);
                        GiveMutex(vfs->mtx);

                        if (basefs && mntfs == NULL) {
                                if (basefs->fs.f_remove) {
                                        status = basefs->fs.f_remove(basefs->fs.f_fsd,
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
 * functionality. Local VFS cannot do this.
 *
 * @param *oldName                old file name
 * @param *newName                new file name
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
stdRet_t vfs_rename(const ch_t *oldName, const ch_t *newName)
{
        if (oldName && newName && vfs) {
                ch_t *extPathOld = NULL;
                ch_t *extPathNew = NULL;

                while (TakeMutex(vfs->mtx, MTX_BLOCK_TIME) != OS_OK);
                struct fsinfo *fsOld = FindBaseFS(oldName, &extPathOld);
                struct fsinfo *fsNew = FindBaseFS(newName, &extPathNew);
                GiveMutex(vfs->mtx);

                if (fsOld && fsNew && fsOld == fsNew) {
                        if (fsOld->fs.f_rename) {
                                return fsOld->fs.f_rename(fsOld->fs.f_fsd,
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
 * @param *path   file path
 * @param  mode   file mode
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
stdRet_t vfs_chmod(const ch_t *path, u16_t mode)
{
        if (path && vfs) {
                ch_t *extPath = NULL;

                while (TakeMutex(vfs->mtx, MTX_BLOCK_TIME) != OS_OK);
                struct fsinfo *fs = FindBaseFS(path, &extPath);
                GiveMutex(vfs->mtx);

                if (fs) {
                        if (fs->fs.f_chmod) {
                                return fs->fs.f_chmod(fs->fs.f_fsd,
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
 * @param *path   file path
 * @param  owner  file owner
 * @param  group  file group
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
stdRet_t vfs_chown(const ch_t *path, u16_t owner, u16_t group)
{
        if (path && vfs) {
                ch_t *extPath = NULL;

                while (TakeMutex(vfs->mtx, MTX_BLOCK_TIME) != OS_OK);
                struct fsinfo *fs = FindBaseFS(path, &extPath);
                GiveMutex(vfs->mtx);

                if (fs) {
                        if (fs->fs.f_chown) {
                                return fs->fs.f_chown(fs->fs.f_fsd, extPath,
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
 * @param *path           file/dir path
 * @param *stat           pointer to stat structure
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
stdRet_t vfs_stat(const ch_t *path, struct vfs_stat *stat)
{
        if (path && stat && vfs) {
                ch_t *extPath = NULL;

                while (TakeMutex(vfs->mtx, MTX_BLOCK_TIME) != OS_OK);
                struct fsinfo *fs = FindBaseFS(path, &extPath);
                GiveMutex(vfs->mtx);

                if (fs) {
                        if (fs->fs.f_stat) {
                                return fs->fs.f_stat(fs->fs.f_fsd, extPath,
                                                     stat);
                        }
                }
        }

        return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief Function returns file system status
 *
 * @param *path           fs path
 * @param *statfs         pointer to fs status structure
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
stdRet_t vfs_statfs(const ch_t *path, struct vfs_statfs *statfs)
{
        if (path && statfs && vfs) {
                ch_t *newpath = NewCorrectedPath(path, ADD_SLASH);
                if (newpath) {
                        while (TakeMutex(vfs->mtx, MTX_BLOCK_TIME) != OS_OK);
                        struct fsinfo *fs = FindMountedFS(newpath, -1, NULL);
                        GiveMutex(vfs->mtx);
                        free(newpath);

                        if (fs) {
                                if (fs->fs.f_statfs) {
                                        return fs->fs.f_statfs(fs->fs.f_fsd,
                                                               statfs);
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
 * @param *name           file path
 * @param *mode           file mode
 *
 * @retval NULL if file can't be created
 */
//==============================================================================
FILE_t *vfs_fopen(const ch_t *path, const ch_t *mode)
{
        if (!path || !mode || !vfs) {
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

        FILE_t *file = calloc(1, sizeof(FILE_t));
        if (file) {
                ch_t *extPath = NULL;

                while (TakeMutex(vfs->mtx, MTX_BLOCK_TIME) != OS_OK);
                struct fsinfo *fs = FindBaseFS(path, &extPath);
                GiveMutex(vfs->mtx);

                if (fs == NULL) {
                        goto vfs_open_error;
                }

                if (fs->fs.f_open == NULL) {
                        goto vfs_open_error;
                }

                if (fs->fs.f_open(fs->fs.f_fsd, &file->fd, &file->f_seek,
                                  extPath, mode) == STD_RET_OK) {

                        file->dev     = fs->fs.f_fsd;
                        file->f_close = fs->fs.f_close;
                        file->f_ioctl = fs->fs.f_ioctl;
                        file->f_stat  = (void*)fs->fs.f_fstat;

                        if (strncmp("r", mode, 2) != 0) {
                                file->f_write = fs->fs.f_write;
                        }

                        if (  strncmp("w", mode, 2) != 0
                           && strncmp("a", mode, 2) != 0) {
                                file->f_read  = fs->fs.f_read;
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
 * @param *file                 pinter to file
 *
 * @retval STD_RET_OK           file closed successfully
 * @retval STD_RET_ERROR        file not closed
 */
//==============================================================================
stdRet_t vfs_fclose(FILE_t *file)
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
 * @param *ptr                    address to data (src)
 * @param size                    item size
 * @param nitems                  number of items
 * @param *file                   pointer to file object
 *
 * @return STD_RET_OK or 0 if write finished successfully, otherwise > 0
 */
//==============================================================================
size_t vfs_fwrite(void *ptr, size_t size, size_t nitems, FILE_t *file)
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
 * @param *ptr                    address to data (dst)
 * @param size                    item size
 * @param nitems                  number of items
 * @param *file                   pointer to file object
 *
 * @return number of read items
 */
//==============================================================================
size_t vfs_fread(void *ptr, size_t size, size_t nitems, FILE_t *file)
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
 * @param *file                 file object
 * @param offset                seek value
 * @param mode                  seek mode
 *
 * @retval STD_RET_OK           seek moved successfully
 * @retval STD_RET_ERROR        error occurred
 */
//==============================================================================
stdRet_t vfs_fseek(FILE_t *file, i32_t offset, i32_t mode)
{
        if (file) {
                struct vfs_stat stat;

                if (mode == VFS_SEEK_END) {
                        stat.st_size = 0;
                        vfs_fstat(file, &stat);
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
 * @param *file                   file object
 *
 * @return -1 if error, otherwise correct value
 */
//==============================================================================
i32_t vfs_ftell(FILE_t *file)
{
        i32_t seek = -1;

        if (file) {
                seek = file->f_seek;
        }

        return seek;
}

//==============================================================================
/**
 * @brief Function support not standard operations
 *
 * @param *file                   file
 * @param rq                      request
 * @param *data                   pointer to data
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
stdRet_t vfs_ioctl(FILE_t *file, IORq_t rq, void *data)
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
 * @param *path           file/dir path
 * @param *stat           pointer to stat structure
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
stdRet_t vfs_fstat(FILE_t *file, struct vfs_stat *stat)
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
 * @param *path           path to FS
 * @param  len            path length
 * @param *itemid         item id in mount list
 *
 * @return pointer to FS info
 */
//==============================================================================
static struct fsinfo *FindMountedFS(const ch_t *path, u16_t len, u32_t *itemid)
{
        struct fsinfo *fsinfo = NULL;

        int_t icount = ListGetItemCount(vfs->mntList);

        for (int_t i = 0; i < icount; i++) {

                struct fsinfo *data = ListGetItemDataByNo(vfs->mntList, i);

                if (strncmp(path, data->mntpoint, len) == 0) {
                        fsinfo = data;

                        if (itemid) {
                                if (ListGetItemID(vfs->mntList, i, itemid) != 0) {
                                        fsinfo = NULL;
                                }
                        }

                        break;
                }
        }

        return fsinfo;
}

//==============================================================================
/**
 * @brief Function find base FS of selected path
 *
 * @param *path         path to FS
 * @param **extPath     pointer to external part of path
 *
 * @return pointer to FS info
 */
//==============================================================================
static struct fsinfo *FindBaseFS(const ch_t *path, ch_t **extPath)
{
        struct fsinfo *fsinfo = NULL;

        ch_t *pathTail = (ch_t*)path + strlen(path);

        if (*(pathTail - 1) == '/') {
                pathTail--;
        }

        while (pathTail >= path) {
                struct fsinfo *fs = FindMountedFS(path, pathTail - path + 1, NULL);

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
 * @param *path           path to correct
 * @param  corr           path correction kind
 *
 * @return pointer to new path
 */
//==============================================================================
static ch_t *NewCorrectedPath(const ch_t *path, enum pathCorrection corr)
{
        ch_t   *new_path;
        uint_t  new_path_len = strlen(path);

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

        new_path = calloc(new_path_len + 1, sizeof(ch_t));
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
