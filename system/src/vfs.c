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
/* delete definition of illegal macros in this file DNLFIXME */
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
      struct fsinfo    *basefs;
      struct vfs_fscfg  fs;
      u8_t mntFSCnt;
};


struct vfshdl {
      list_t  *mntList;
      mutex_t mtx;
};


/*==================================================================================================
                                      Local function prototypes
==================================================================================================*/
static struct fsinfo *FindMountedFS(const ch_t *path, u16_t len, u32_t *itemid);
static struct fsinfo *FindBaseFS(const ch_t *path, u16_t len, ch_t **extPath);
static ch_t          *AddEndSlash(const ch_t *path);
static ch_t          *SubEndSlash(const ch_t *path);

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

      if (path && mountcfg && vfs) {
            struct fsinfo *mountfs = NULL;
            struct fsinfo *basefs  = NULL;
            ch_t          *extPath = NULL;
            struct fsinfo *newfs   = NULL;
            ch_t          *newpath = AddEndSlash(path);

            if (newpath) {
                  if (TakeMutex(vfs->mtx, MTX_BLOCK_TIME)) {
                        /* find if file system is already mounted and find base file system ----- */
                        mountfs = FindMountedFS(newpath, -1, NULL);
                        basefs  = FindBaseFS(newpath, strlen(newpath), &extPath);

                        /* create new FS in existing dir and FS, otherwise create new FS if first mount */
                        if (basefs && mountfs == NULL) {
                              if (basefs->fs.opendir && extPath) {

                                    if (basefs->fs.opendir(basefs->fs.dev,
                                                           extPath, NULL) == STD_RET_OK) {

                                          newfs = calloc(1, sizeof(struct fsinfo));
                                          basefs->mntFSCnt++;
                                    }
                              }
                        } else if (  ListGetItemCount(vfs->mntList) == 0
                                  && strlen(newpath) == 1 && newpath[0] == '/') {

                              newfs = calloc(1, sizeof(struct fsinfo));
                        }

                        /* mount FS if created -------------------------------------------------- */
                        if (newfs) {
                              if (mountcfg->init) {
                                    if (mountcfg->init(mountcfg->dev) == STD_RET_OK) {
                                          newfs->path     = newpath;
                                          newfs->basefs   = basefs;
                                          newfs->fs       = *mountcfg;
                                          newfs->mntFSCnt = 0;

                                          if (ListAddItem(vfs->mntList, newfs) >= 0) {
                                                status = STD_RET_OK;
                                          }
                                    }
                              }
                        }

                        /* free allocated memory if error --------------------------------------- */
                        if (status != STD_RET_OK) {
                              if (newfs)
                                    free(newfs);

                              free(newpath);
                        }

                        GiveMutex(fs->mtx);
                  }
            }
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

      if (path && vfs) {
            if (path[0] == '/') {
                  if (TakeMutex(vfs->mtx, MTX_BLOCK_TIME)) {
                        u32_t itemid;
                        ch_t *newpath = AddEndSlash(path);

                        if (newpath) {
                              struct fsinfo *mountfs = FindMountedFS(newpath, -1, &itemid);

                              if (mountfs) {
                                    if (mountfs->mntFSCnt == 0) {
                                          if (mountfs->basefs)
                                                if (mountfs->basefs->mntFSCnt)
                                                      mountfs->basefs->mntFSCnt--;

                                          if (mountfs->path)
                                                free(mountfs->path);

                                          if (ListRmItemByID(vfs->mntList, itemid) == 0)
                                                status = STD_RET_OK;
                                    }
                              }

                              free(newpath);
                        }

                        GiveMutex(fs->mtx);
                  }
            }
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

      if (path && drvcfg && vfs) {
            if (TakeMutex(vfs->mtx, MTX_BLOCK_TIME)) {
                  ch_t *extPath     = NULL;
                  struct fsinfo *fs = FindBaseFS(path, strlen(path), &extPath);

                  if (fs) {
                        if (fs->fs.mknod) {
                              status = fs->fs.mknod(fs->fs.dev, extPath, drvcfg);
                        }
                  }

                  GiveMutex(fs->mtx);
            }
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

      if (path && vfs) {
            if (TakeMutex(vfs->mtx, MTX_BLOCK_TIME)) {
                  ch_t *newpath = SubEndSlash(path);

                  if (newpath) {
                        ch_t *extPath     = NULL;
                        struct fsinfo *fs = FindBaseFS(newpath, strlen(newpath), &extPath);

                        if (fs) {
                              if (fs->fs.mkdir) {
                                    status = fs->fs.mkdir(fs->fs.dev, extPath);
                              }
                        }

                        free(newpath);
                  }

                  GiveMutex(fs->mtx);
            }
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
      DIR_t *dir = malloc(sizeof(DIR_t));

      if (path && vfs && dir) {
            if (TakeMutex(vfs->mtx, MTX_BLOCK_TIME)) {
                  ch_t *newpath = AddEndSlash(path);

                  if (newpath) {
                        ch_t     *extPath = NULL;
                        struct fsinfo *fs = FindBaseFS(newpath, strlen(newpath), &extPath);

                        if (fs) {
                              if (fs->fs.opendir) {
                                    if (fs->fs.opendir(fs->fs.dev, extPath, dir) != STD_RET_OK) {
                                          free(dir);
                                          dir = NULL;
                                    }
                              }
                        }
                  }

                  GiveMutex(fs->mtx);
            }
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
 * Removes file or directory. Removes directory if is not a mount point.
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

      if (path) {
            if (TakeMutex(vfs->mtx, MTX_BLOCK_TIME)) {
                  ch_t *slashpath = AddEndSlash(path);

                  if (slashpath) {
                        struct fsinfo *mfs = FindMountedFS(slashpath, -1, NULL);

                        if (mfs == NULL) {
                              ch_t *extPath = NULL;
                              slashpath[strlen(slashpath) - 1] = '\0';
                              struct fsinfo *bfs = FindBaseFS(slashpath, strlen(slashpath) - 1, &extPath);

                              if (bfs) {
                                    if (bfs->fs.remove) {
                                          status = bfs->fs.remove(bfs->fs.dev, extPath);
                                    }
                              }
                        }

                        free(slashpath);
                  }
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

      if (TakeMutex(vfs->mtx, MTX_BLOCK_TIME)) {
            if (oldName && newName) {
                  ch_t     *extPathOld = NULL;
                  ch_t     *extPathNew = NULL;
                  struct fsinfo *fsOld = FindBaseFS(oldName, strlen(oldName), &extPathOld);
                  struct fsinfo *fsNew = FindBaseFS(newName, strlen(newName), &extPathNew);

                  if (fsOld && fsNew && fsOld == fsNew) {
                        if (fsOld->fs.rename) {
                              status = fsOld->fs.rename(fsOld->fs.dev, extPathOld, extPathNew);
                        }
                  }
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

      if (TakeMutex(vfs->mtx, MTX_BLOCK_TIME)) {
            if (path && stat) {
                  ch_t     *extPath = NULL;
                  struct fsinfo *fs = FindBaseFS(path, strlen(path), &extPath);

                  if (fs) {
                        if (fs->fs.stat) {
                              status = fs->fs.stat(fs->fs.dev, extPath, stat);
                        }
                  }
            }

            GiveMutex(fs->mtx);
      }

      return status;
}


//================================================================================================//
/**
 * @brief Function returns file system status
 *
 * @param *path         fs path
 * @param *statfs       pointer to fs status structure
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//================================================================================================//
stdRet_t vfs_statfs(const ch_t *path, struct vfs_statfs *statfs)
{
      stdRet_t status = STD_RET_ERROR;

      if (TakeMutex(vfs->mtx, MTX_BLOCK_TIME)) {
            if (path && statfs) {
                  struct fsinfo *fs = FindMountedFS(path, -1, NULL);

                  if (fs) {
                        if (fs->fs.statfs) {
                              status = fs->fs.statfs(fs->fs.dev, statfs);
                        }
                  }
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

      if (TakeMutex(vfs->mtx, MTX_BLOCK_TIME)) {
            if (path && mode) {
                  file = calloc(1, sizeof(FILE_t));

                  if (file) {
                        /* DNLTODO vfs_fopen */

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
 * @brief Function find FS in mounted list
 *
 * @param *path         path to FS
 * @param  len          path length
 * @param *itemid       item id in mount list
 *
 * @return pointer to FS info
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
 * @brief Function find base FS of selected path
 *
 * @param *path         path to FS
 * @param  len          path length
 * @param **extPath     pointer to external part of path
 *
 * @return pointer to FS info
 */
//================================================================================================//
static struct fsinfo *FindBaseFS(const ch_t *path, u16_t len, ch_t **extPath)
{
      struct fsinfo *fsinfo = NULL;

      ch_t *pathTail = (ch_t*)path + len;

      if (*(pathTail - 1) == '/')
            pathTail--;

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
 * @brief Function corrects path -- adds additional / at the end if not exist
 *
 * @param *path         path to correct
 *
 * @return pointer to new path
 */
//================================================================================================//
static ch_t *AddEndSlash(const ch_t *path)
{
      ch_t *newpath;
      u16_t nplen = strlen(path);

      if (path[nplen - 1] != '/') {
            nplen++;
      }

      newpath = calloc(nplen + 1, sizeof(ch_t));

      if (newpath) {
            strcpy(newpath, path);

            if (nplen > strlen(path)) {
                  strcat(newpath, "/");
            }
      }

     return newpath;
}


//================================================================================================//
/**
 * @brief Function corrects path -- subs / at the end if exist
 *
 * @param *path         path to correct
 *
 * @return pointer to new path
 */
//================================================================================================//
static ch_t *SubEndSlash(const ch_t *path)
{
      ch_t *newpath;
      u16_t nplen = strlen(path);

      if (path[nplen - 1] == '/') {
            nplen--;
      }

      newpath = calloc(nplen + 1, sizeof(ch_t));

      if (newpath) {
            strncpy(newpath, path, nplen);
      }

      return newpath;
}

#ifdef __cplusplus
}
#endif

/*==================================================================================================
                                            End of file
==================================================================================================*/
