/*=============================================================================================*//**
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


*//*==============================================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*==================================================================================================
                                            Include files
==================================================================================================*/
#include "vfs.h"
#include "system.h"
#include <string.h>


/*==================================================================================================
                                  Local symbolic constants/macros
==================================================================================================*/
/* delete definition of illegal macros in this file */
#undef ioctl
#undef opendir
#undef remove
#undef rename

/* wait time for operation on VFS */
#define MTX_BLOCK_TIME        100


/*==================================================================================================
                                   Local types, enums definitions
==================================================================================================*/
struct fsinfo {
      ch_t *path;
      struct vfs_fscfg fs;
      u8_t mntFSCnt;
};


struct vfshdl {
      list_t  *mntList;
      mutex_t mtx;
};


/*==================================================================================================
                                      Local function prototypes
==================================================================================================*/
static ch_t          *GetWordFromPath(ch_t *str, ch_t **word, size_t *length);
static i32_t          GetPathDeep(const ch_t *path);
static struct fsinfo *FindMountedFS(const ch_t *path, u16_t len, u32_t *itemid);
static struct fsinfo *FindBaseFS(const ch_t *path, u16_t len, ch_t **extPath);

/*==================================================================================================
                                      Local object definitions
==================================================================================================*/
static struct vfshdl *vfs;


/*==================================================================================================
                                        Function definitions
==================================================================================================*/

//================================================================================================//
/**
 * @brief Initialize VFS module
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//================================================================================================//
stdRet_t vfs_init(void)
{
      stdRet_t ret = STD_RET_OK;

      if (TakeMutex(fs->mtx, MTX_BLOCK_TIME)) {
            if (vfs == NULL) {
                  vfs = calloc(1, sizeof(struct vfshdl));

                  if (vfs) {
                        CreateMutex(vfs->mtx);
                        vfs->mntList = ListCreate();

                        if (!vfs->mtx || !vfs->mntList) {
                              if (vfs->mtx)
                                    DeleteMutex(vfs->mtx);

                              if (vfs->mntList)
                                    ListDestroy(vfs->mntList);

                              free(vfs);
                              vfs = NULL;
                        }
                  }

                  if (vfs == NULL)
                        ret = STD_RET_ERROR;
            }

            GiveMutex(fs->mtx);
      }

      return ret;
}


//================================================================================================//
/**
 * @brief Function mount file system in VFS
 * External file system is mounted to empty directory. If directory is not empty mounting is not
 * possible.
 *
 * @param *path               path when dir shall be mounted
 * @param *mountcfg           pointer to description of mount FS
 *
 * @retval STD_RET_OK         mount success
 * @retval STD_RET_ERROR      mount error
 */
//================================================================================================//
stdRet_t vfs_mount(const ch_t *path, struct vfs_fscfg *mountcfg)
{
      stdRet_t status = STD_RET_ERROR;

      if (TakeMutex(fs->mtx, MTX_BLOCK_TIME)) {
            if (path && mountcfg && vfs) {
                  struct fsinfo *mountfs = NULL;
                  struct fsinfo *basefs  = NULL;
                  ch_t          *extPath = NULL;
                  DIR_t         *dir     = NULL;
                  struct fsinfo *newfs   = NULL;

                  mountfs = FindMountedFS(path, strlen(path), NULL);
                  basefs  = FindBaseFS(path, strlen(path), &extPath);

                  /* create new FS in existing dir and FS, otherwise create new FS if first mount */
                  if (basefs) {
                        if (basefs->fs.opendir && extPath) {
                              dir = basefs->fs.opendir(basefs->fs.dev, extPath);

                              if (mountfs == NULL && dir) {
                                    vfs_closedir(dir);

                                    newfs = calloc(1, sizeof(struct fsinfo));

                                    basefs->mntFSCnt++;
                              }
                        }
                  } else if (  ListGetItemCount(vfs->mntList) == 0
                            && strlen(path) == 1 && path[0] == '/') {

                        newfs = calloc(1, sizeof(struct fsinfo));
                  }

                  /* mount FS if created */
                  if (newfs) {
                        newfs->path     = calloc(strlen(path) + 1, sizeof(ch_t));
                        newfs->fs       = *mountcfg;
                        newfs->mntFSCnt = 0;

                        if (newfs->path) {
                              strcpy(newfs->path, path);

                              if (ListAddItem(vfs->mntList, newfs) >= 0) {
                                    status = STD_RET_OK;
                              }
                        } else {
                              free(newfs);
                        }
                  }
            }

            GiveMutex(fs->mtx);
      }

      return status;
}


//================================================================================================//
/**
 * @brief Function umount dir from file system
 *
 * @param *path               dir path
 *
 * @retval STD_RET_OK         mount success
 * @retval STD_RET_ERROR      mount error
 */
//================================================================================================//
stdRet_t vfs_umount(const ch_t *path)
{
      stdRet_t status = STD_RET_ERROR;

      if (TakeMutex(fs->mtx, MTX_BLOCK_TIME)) {
            if (path && vfs) {
                  if (path[0] == '/') {
                        u32_t itemid;
                        struct fsinfo *mountfs = FindMountedFS(path, strlen(path), &itemid);
                        struct fsinfo *basefs  = FindBaseFS(path, strrchr(path, '/') - path, NULL);

                        if (mountfs && basefs) {
                              if (mountfs->mntFSCnt == 0) {
                                    if (basefs->mntFSCnt)
                                          basefs->mntFSCnt--;

                                    if (mountfs->path)
                                          free(mountfs->path);

                                    if (ListRmItemByID(vfs->mntList, itemid) == 0)
                                          status = STD_RET_OK;
                              }
                        }
                  }
            }

            GiveMutex(fs->mtx);
      }

      return status;
}


//================================================================================================//
/**
 * @brief Function create node for driver file
 *
 * @param *path               path when driver-file shall be created
 * @param *drvcfg             pointer to description of driver
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//================================================================================================//
stdRet_t vfs_mknod(const ch_t *path, struct vfs_drvcfg *drvcfg)
{
      stdRet_t status = STD_RET_ERROR;

      if (TakeMutex(fs->mtx, MTX_BLOCK_TIME)) {
            if (path && drvcfg && vfs) {

            }

            GiveMutex(fs->mtx);
      }

      return status;
}


//================================================================================================//
/**
 * @brief Create directory
 *
 * @param *path   path to new directory
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//================================================================================================//
stdRet_t vfs_mkdir(const ch_t *path)
{
      stdRet_t status = STD_RET_ERROR;

      if (TakeMutex(fs->mtx, MTX_BLOCK_TIME)) {
            if (path && vfs) {
                  ch_t *extPath     = NULL;
                  struct fsinfo *fs = FindBaseFS(path, strlen(path), &extPath);

                  if (fs) {
                        if (fs->fs.mkdir) {
                              status = fs->fs.mkdir(fs->fs.dev, extPath);
                        }
                  }
            }

            GiveMutex(fs->mtx);
      }

      return status;
}


//================================================================================================//
/**
 * @brief Function open directory
 *
 * @param *path         directory path
 *
 * @return directory object
 */
//================================================================================================//
DIR_t *vfs_opendir(const ch_t *path)
{
      DIR_t *dir = NULL;

      if (TakeMutex(fs->mtx, MTX_BLOCK_TIME)) {
            if (path && vfs) {

            }

            GiveMutex(fs->mtx);
      }

      return dir;
}


//================================================================================================//
/**
 * @brief Function close opened directory
 *
 * @param *dir          directory object
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//================================================================================================//
stdRet_t vfs_closedir(DIR_t *dir)
{
      stdRet_t status = STD_RET_ERROR;

      if (dir)
      {
            free(dir);
            status = STD_RET_OK;
      }

      return status;
}


//================================================================================================//
/**
 * @brief Function read next item of opened directory
 *
 * @param *dir          directory object
 *
 * @return element attributes
 */
//================================================================================================//
dirent_t vfs_readdir(DIR_t *dir)
{
      dirent_t direntry;
      direntry.name = NULL;
      direntry.size = 0;

      if (dir->rddir) {
            direntry = dir->rddir(dir);
      }

      return direntry;
}


//================================================================================================//
/**
 * @brief Remove file
 *
 * @param *patch        localization of file/directory
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//================================================================================================//
stdRet_t vfs_remove(const ch_t *path)
{
      stdRet_t status = STD_RET_ERROR;

      if (TakeMutex(fs->mtx, MTX_BLOCK_TIME)) {
            if (path) {

            }

            GiveMutex(fs->mtx);
      }

      return status;
}


//================================================================================================//
/**
 * @brief Rename file name
 * The implementation of rename can move files only if external FS provide functionality. Local
 * VFS can not do this.
 *
 * @param *oldName            old file name
 * @param *newName            new file name
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//================================================================================================//
stdRet_t vfs_rename(const ch_t *oldName, const ch_t *newName)
{
      stdRet_t status = STD_RET_ERROR;

      if (TakeMutex(fs->mtx, MTX_BLOCK_TIME)) {
            if (oldName && newName) {

            }

            GiveMutex(fs->mtx);
      }

      return status;
}


//================================================================================================//
/**
 * @brief Function returns file/dir status
 *
 * @param *path         file/dir path
 * @param *stat         pointer to stat structure
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//================================================================================================//
stdRet_t vfs_stat(const ch_t *path, struct vfs_stat *stat)
{
      stdRet_t status = STD_RET_ERROR;

      if (TakeMutex(fs->mtx, MTX_BLOCK_TIME)) {
            if (path && stat) {
            }

            GiveMutex(fs->mtx);
      }

      return status;
}


//================================================================================================//
/**
 * @brief Function open selected file
 *
 * @param *name         file path
 * @param *mode         file mode
 *
 * @retval NULL if file can't be created
 */
//================================================================================================//
FILE_t *vfs_fopen(const ch_t *path, const ch_t *mode)
{
      FILE_t *file = NULL;

      if (TakeMutex(fs->mtx, MTX_BLOCK_TIME)) {
            if (path && mode) {
                  file = calloc(1, sizeof(FILE_t));

                  if (file) {


                        /* check file mode */
                        if (file) {
                              /* open for reading */
                              if (strncmp("r", mode, 2) == 0)
                              {
                                    file->write = NULL;
                              }
                              /* open for writing (file need not exist) */
                              else if (strncmp("w", mode, 2) == 0)
                              {
                                    file->read = NULL;
                              }
                              /* open for appending (file need not exist) */
                              else if (strncmp("a", mode, 2) == 0)
                              {
                                    file->read = NULL;
                              }
                              /* open for reading and writing, start at beginning */
                              else if (strncmp("r+", mode, 2) == 0)
                              {
                                    /* nothing to change */
                              }
                              /* open for reading and writing (overwrite file) */
                              else if (strncmp("w+", mode, 2) == 0)
                              {
                                    /* nothing to change */
                              }
                              /* open for reading and writing (append if file exists) */
                              else if (strncmp("a+", mode, 2) == 0)
                              {
                                    /* nothing to change */
                              }
                              /* invalid mode */
                              else
                              {
                                    vfs_fclose(file);
                                    file = NULL;
                              }
                        }
                  }
            }

            GiveMutex(fs->mtx);
      }

      return file;
}


//================================================================================================//
/**
 * @brief Function close opened file
 *
 * @param *file               pinter to file
 *
 * @retval STD_RET_OK         file closed successfully
 * @retval STD_RET_ERROR      file not closed
 */
//================================================================================================//
stdRet_t vfs_fclose(FILE_t *file)
{
      stdRet_t status = STD_RET_ERROR;

      if (file) {
            if (file->close) {
                  if (file->close(file->dev, file->fd) == STD_RET_OK) {
                        free(file);
                        status = STD_RET_OK;
                  }
            }
      }

      return status;
}


//================================================================================================//
/**
 * @brief Function write data to file
 *
 * @param *ptr                address to data (src)
 * @param size                item size
 * @param nitems              number of items
 * @param *file               pointer to file object
 *
 * @return STD_RET_OK or 0 if write finished successfully, otherwise > 0
 */
//================================================================================================//
size_t vfs_fwrite(void *ptr, size_t size, size_t nitems, FILE_t *file)
{
      size_t n = 0;

      if (ptr && size && nitems && file) {
            if (file->write) {
                  n = file->write(file->dev, file->fd, ptr, size, nitems, file->seek);
                  file->seek += n * size;
            }
      }

      return n;
}


//================================================================================================//
/**
 * @brief Function read data from file
 *
 * @param *ptr                address to data (dst)
 * @param size                item size
 * @param nitems              number of items
 * @param *file               pointer to file object
 *
 * @return number of read items
 */
//================================================================================================//
size_t vfs_fread(void *ptr, size_t size, size_t nitems, FILE_t *file)
{
      size_t n = 0;

      if (ptr && size && nitems && file) {
            if (file->read) {
                  n = file->read(file->dev, file->fd, ptr, size, nitems, file->seek);
                  file->seek += n * size;
            }
      }

      return n;
}


//================================================================================================//
/**
 * @brief Function set seek value
 *
 * @param *file               file object
 * @param offset              seek value
 * @param mode                seek mode DNLFIXME implement: seek mode
 *
 * @retval STD_RET_OK         seek moved successfully
 * @retval STD_RET_ERROR      error occurred
 */
//================================================================================================//
stdRet_t vfs_fseek(FILE_t *file, i32_t offset, i32_t mode)
{
      (void)mode;

      stdRet_t status = STD_RET_ERROR;

      if (file) {
            file->seek = offset;
            status     = STD_RET_OK;
      }

      return status;
}


//================================================================================================//
/**
 * @brief Function support not standard operations
 *
 * @param *file               file
 * @param rq                  request
 * @param *data               pointer to data
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//================================================================================================//
stdRet_t vfs_ioctl(FILE_t *file, IORq_t rq, void *data)
{
      stdRet_t status = STD_RET_ERROR;

      if (file) {
            if (file->ioctl) {
                  status = file->ioctl(file->dev, file->fd, rq, data);
            }
      }

      return status;
}


//================================================================================================//
/**
 * @brief
 */
//================================================================================================//
static struct fsinfo *FindMountedFS(const ch_t *path, u16_t len, u32_t *itemid)
{
      struct fsinfo *fsinfo = NULL;

      i32_t icount = ListGetItemCount(vfs->mntList);

      for (i32_t i = 0; i < icount; i++) {

            struct fsinfo *data = ListGetItemDataByNo(vfs->mntList, i);

            if (strncmp(path, data->path, len) == 0) {
                  fsinfo = data;

                  if (itemid)
                        *itemid = ListGetItemID(vfs->mntList, i);

                  break;
            }
      }

      return fsinfo;
}


//================================================================================================//
/**
 * @brief
 */
//================================================================================================//
static struct fsinfo *FindBaseFS(const ch_t *path, u16_t len, ch_t **extPath)
{
      struct fsinfo *fsinfo = NULL;

      ch_t *pathTail = (ch_t*)path + len;

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











//================================================================================================//
/**
 * @brief Function return pointer to word
 *
 * @param[in]  *str          string
 * @param[out] **word        pointer to word beginning
 * @param[out] *length       pointer to word length
 *
 * @return pointer to next word, otherwise NULL
 */
//================================================================================================//
static ch_t *GetWordFromPath(ch_t *str, ch_t **word, size_t *length)
{
      ch_t *bwd = NULL;
      ch_t *ewd = NULL;
      ch_t *nwd = NULL;

      if (str) {
            bwd = strchr(str, '/');

            if (bwd) {
                  ewd = strchr(bwd + 1, '/');

                  if (ewd == NULL) {
                        ewd = strchr(bwd + 1, '\0');
                        nwd = NULL;
                  } else {
                        nwd = ewd;
                  }

                  bwd++;
            }
      }

      if (word)
            *word   = bwd;

      if (length)
            *length = ewd - bwd;

      return nwd;
}


//================================================================================================//
/**
 * @brief Check path deep
 *
 * @param *path         path
 *
 * @return path deep
 */
//================================================================================================//
static i32_t GetPathDeep(const ch_t *path)
{
      u32_t deep = 0;
      const ch_t *lastpath = NULL;

      if (path[0] == '/') {
            lastpath = path++;

            while ((path = strchr(path, '/'))) {
                  lastpath = path;
                  path++;
                  deep++;
            }

            if (lastpath[1] != '\0')
                  deep++;
      }

      return deep;
}


#ifdef __cplusplus
}
#endif

/*==================================================================================================
                                            End of file
==================================================================================================*/
