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
#include "dlist.h"
#include "oswrap.h"
#include "memman.h"
#include <string.h>


/*==================================================================================================
                                  Local symbolic constants/macros
==================================================================================================*/
/* wait time for operation on VFS */
#define MTX_BLOCK_TIME                    10

#define calloc(nmemb, msize)              memman_calloc(nmemb, msize)
#define malloc(size)                      memman_malloc(size)
#define free(mem)                         memman_free(mem)


/*==================================================================================================
                                   Local types, enums definitions
==================================================================================================*/
struct fsinfo {
      ch_t             *mntpoint;
      struct fsinfo    *basefs;
      struct vfs_fscfg  fs;
      u8_t mntFSCnt;
};


struct vfshdl {
      list_t  *mntList;
      mutex_t  mtx;
      u32_t    idcnt;
};


/*==================================================================================================
                                      Local function prototypes
==================================================================================================*/
static struct fsinfo *FindMountedFS(const ch_t *path, u16_t len, u32_t *itemid);
static struct fsinfo *FindBaseFS(const ch_t *path, ch_t **extPath);
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
 * @param *srcPath            path to source file when file system load data
 * @param *mntPoint           path when dir shall be mounted
 * @param *mountcfg           pointer to description of mount FS
 *
 * @retval STD_RET_OK         mount success
 * @retval STD_RET_ERROR      mount error
 */
//================================================================================================//
stdRet_t vfs_mount(const ch_t *srcPath, const ch_t *mntPoint, struct vfs_fscfg *mountcfg)
{
      stdRet_t status = STD_RET_ERROR;

      if (mntPoint && mountcfg && vfs) {
            struct fsinfo *mountfs = NULL;
            struct fsinfo *basefs  = NULL;
            ch_t          *extPath = NULL;
            struct fsinfo *newfs   = NULL;
            ch_t          *newpath = AddEndSlash(mntPoint);

            if (newpath && vfs) {
                  while (TakeMutex(vfs->mtx, MTX_BLOCK_TIME) != OS_OK);

                  /* find if file system is already mounted and find base file system ----------- */
                  mountfs = FindMountedFS(newpath, -1, NULL);
                  basefs  = FindBaseFS(newpath, &extPath);

                  /* create new FS in existing dir and FS, otherwise create new FS if first mount */
                  if (basefs && mountfs == NULL) {
                        if (basefs->fs.f_opendir && extPath) {

                              if (basefs->fs.f_opendir(basefs->fs.f_fsd,
                                                       extPath, NULL) == STD_RET_OK) {

                                    newfs = calloc(1, sizeof(struct fsinfo));
                                    basefs->mntFSCnt++;
                              }
                        }
                  } else if (  ListGetItemCount(vfs->mntList) == 0
                            && strlen(newpath) == 1 && newpath[0] == '/') {

                        newfs = calloc(1, sizeof(struct fsinfo));
                  }

                  /* mount FS if created -------------------------------------------------------- */
                  if (newfs && mountcfg->f_init) {
                        newfs->fs = *mountcfg;

                        if (mountcfg->f_init(srcPath, &newfs->fs.f_fsd) == STD_RET_OK) {
                              newfs->mntpoint = newpath;
                              newfs->basefs   = basefs;
                              newfs->mntFSCnt = 0;

                              if (ListAddItem(vfs->mntList, vfs->idcnt++, newfs) >= 0) {
                                    status = STD_RET_OK;
                              }
                        }
                  }

                  GiveMutex(vfs->mtx);
            }

            /* free allocated memory if error */
            if (status == STD_RET_ERROR) {
                  if (newfs) {
                        free(newfs);
                  }

                  free(newpath);
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
                  while (TakeMutex(vfs->mtx, MTX_BLOCK_TIME) != OS_OK);

                  u32_t itemid;
                  ch_t *newpath = AddEndSlash(path);

                  if (newpath) {
                        struct fsinfo *mountfs = FindMountedFS(newpath, -1, &itemid);

                        if (mountfs) {
                              if (mountfs->fs.f_release && mountfs->mntFSCnt == 0) {
                                    if (mountfs->fs.f_release(mountfs->fs.f_fsd) == STD_RET_OK) {

                                          /* decrease mount points if base FS exist */
                                          if (mountfs->basefs) {
                                                if (mountfs->basefs->mntFSCnt)
                                                      mountfs->basefs->mntFSCnt--;
                                          }

                                          if (mountfs->mntpoint)
                                                free(mountfs->mntpoint);

                                          if (ListRmItemByID(vfs->mntList, itemid) == 0)
                                                status = STD_RET_OK;
                                    }
                              }
                        }

                        free(newpath);
                  }

                  GiveMutex(vfs->mtx);
            }
      }

      return status;
}


//================================================================================================//
/**
 * @brief Function gets mount point for n item
 *
 * @param  item         mount point number
 * @param *mntent       mount entry data
 */
//================================================================================================//
stdRet_t vfs_getmntentry(size_t item, struct vfs_mntent *mntent)
{
      stdRet_t status = STD_RET_ERROR;

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
                              strncpy(mntent->mnt_dir, fs->mntpoint, strlen(fs->mntpoint) - 1);
                        } else {
                              strcpy(mntent->mnt_dir, fs->mntpoint);
                        }

                        strcpy(mntent->mnt_fsname, statfs.fsname);
                        mntent->free  = statfs.f_bfree;
                        mntent->total = statfs.f_blocks;

                        status = STD_RET_OK;
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
            ch_t *extPath     = NULL;

            while (TakeMutex(vfs->mtx, MTX_BLOCK_TIME) != OS_OK);
            struct fsinfo *fs = FindBaseFS(path, &extPath);
            GiveMutex(vfs->mtx);

            if (fs) {
                  if (fs->fs.f_mknod) {
                        status = fs->fs.f_mknod(fs->fs.f_fsd, extPath, drvcfg);
                  }
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
            ch_t *newpath = SubEndSlash(path);

            if (newpath) {
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
      stdRet_t status = STD_RET_ERROR;
      DIR_t *dir      = NULL;

      if (path && vfs) {
            dir = malloc(sizeof(DIR_t));

            if (dir) {
                  ch_t *newpath = AddEndSlash(path);

                  if (newpath) {
                        ch_t *extPath = NULL;

                        while (TakeMutex(vfs->mtx, MTX_BLOCK_TIME) != OS_OK);
                        struct fsinfo *fs = FindBaseFS(newpath, &extPath);
                        GiveMutex(vfs->mtx);

                        if (fs) {
                              dir->fsd = fs->fs.f_fsd;

                              if (fs->fs.f_opendir) {
                                    status = fs->fs.f_opendir(fs->fs.f_fsd, extPath, dir);
                              }
                        }

                        free(newpath);
                  }

                  if (status == STD_RET_ERROR) {
                        free(dir);
                        dir = NULL;
                  }
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
            if (dir->cldir) {
                  status = dir->cldir(dir->fsd, dir);

                  if (status == STD_RET_OK) {
                        free(dir);
                  }
            }
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
            direntry = dir->rddir(dir->fsd, dir);
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

      if (path && vfs) {
            ch_t *slashpath = AddEndSlash(path);

            if (slashpath) {
                  ch_t *extPath = NULL;

                  while (TakeMutex(vfs->mtx, MTX_BLOCK_TIME) != OS_OK);
                  struct fsinfo *mfs = FindMountedFS(slashpath, -1, NULL);
                  struct fsinfo *bfs = FindBaseFS(path, &extPath);
                  GiveMutex(vfs->mtx);

                  if (bfs && mfs == NULL) {
                        if (bfs->fs.f_remove) {
                              status = bfs->fs.f_remove(bfs->fs.f_fsd, extPath);
                        }
                  }

                  free(slashpath);
            }
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

      if (oldName && newName && vfs) {
            ch_t *extPathOld = NULL;
            ch_t *extPathNew = NULL;

            while (TakeMutex(vfs->mtx, MTX_BLOCK_TIME) != OS_OK);
            struct fsinfo *fsOld = FindBaseFS(oldName, &extPathOld);
            struct fsinfo *fsNew = FindBaseFS(newName, &extPathNew);
            GiveMutex(vfs->mtx);

            if (fsOld && fsNew && fsOld == fsNew) {
                  if (fsOld->fs.f_rename) {
                        status = fsOld->fs.f_rename(fsOld->fs.f_fsd, extPathOld, extPathNew);
                  }
            }
      }

      return status;
}


//================================================================================================//
/**
 * @brief Function change file mode
 *
 * @param *path   file path
 * @param  mode   file mode
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//================================================================================================//
stdRet_t vfs_chmod(const ch_t *path, u16_t mode)
{
      stdRet_t status = STD_RET_ERROR;

      if (path && vfs) {
            ch_t *extPath = NULL;

            while (TakeMutex(vfs->mtx, MTX_BLOCK_TIME) != OS_OK);
            struct fsinfo *fs = FindBaseFS(path, &extPath);
            GiveMutex(vfs->mtx);

            if (fs) {
                  if (fs->fs.f_chmod) {
                        status = fs->fs.f_chmod(fs->fs.f_fsd, extPath, mode);
                  }
            }
      }

      return status;
}


//================================================================================================//
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
//================================================================================================//
stdRet_t vfs_chown(const ch_t *path, u16_t owner, u16_t group)
{
      stdRet_t status = STD_RET_ERROR;

      if (path && vfs) {
            ch_t *extPath = NULL;

            while (TakeMutex(vfs->mtx, MTX_BLOCK_TIME) != OS_OK);
            struct fsinfo *fs = FindBaseFS(path, &extPath);
            GiveMutex(vfs->mtx);

            if (fs) {
                  if (fs->fs.f_chown) {
                        status = fs->fs.f_chown(fs->fs.f_fsd, extPath, owner, group);
                  }
            }
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

      if (path && stat && vfs) {
            ch_t *extPath = NULL;

            while (TakeMutex(vfs->mtx, MTX_BLOCK_TIME) != OS_OK);
            struct fsinfo *fs = FindBaseFS(path, &extPath);
            GiveMutex(vfs->mtx);

            if (fs) {
                  if (fs->fs.f_stat) {
                        status = fs->fs.f_stat(fs->fs.f_fsd, extPath, stat);
                  }
            }
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

      if (path && statfs && vfs) {
            ch_t *slashpath = AddEndSlash(path);

            if (slashpath) {
                  while (TakeMutex(vfs->mtx, MTX_BLOCK_TIME) != OS_OK);
                  struct fsinfo *fs = FindMountedFS(slashpath, -1, NULL);
                  GiveMutex(vfs->mtx);

                  if (fs) {
                        if (fs->fs.f_statfs) {
                              status = fs->fs.f_statfs(fs->fs.f_fsd, statfs);
                        }
                  }

                  free(slashpath);
            }
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

      if (path && mode && vfs) {
            if (  path[strlen(path) - 1] != '/'
               && (  strncmp("r", mode, 2) == 0 || strncmp("r+", mode, 2) == 0
                  || strncmp("w", mode, 2) == 0 || strncmp("w+", mode, 2) == 0
                  || strncmp("a", mode, 2) == 0 || strncmp("a+", mode, 2) == 0) ) {

                  file = calloc(1, sizeof(FILE_t));

                  if (file) {
                        stdRet_t status  = STD_RET_ERROR;
                        ch_t    *extPath = NULL;

                        while (TakeMutex(vfs->mtx, MTX_BLOCK_TIME) != OS_OK);
                        struct fsinfo *fs = FindBaseFS(path, &extPath);
                        GiveMutex(vfs->mtx);

                        if (fs) {
                              if (fs->fs.f_open) {
                                    status = fs->fs.f_open(fs->fs.f_fsd,  &file->fd,
                                                           &file->f_seek, extPath, mode);
                              }
                        }

                        if (status == STD_RET_OK) {
                              file->dev     = fs->fs.f_fsd;
                              file->f_close = fs->fs.f_close;
                              file->f_ioctl = fs->fs.f_ioctl;
                              file->f_stat  = (void*)fs->fs.f_fstat;

                              if (strncmp("r", mode, 2) != 0) {
                                    file->f_write = fs->fs.f_write;
                              }

                              if (strncmp("w", mode, 2) != 0 && strncmp("a", mode, 2) != 0) {
                                    file->f_read  = fs->fs.f_read;
                              }
                        } else {
                              free(file);
                              file = NULL;
                        }
                  }
            }
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
            if (file->f_close) {
                  status = file->f_close(file->dev, file->fd);

                  if (status == STD_RET_OK) {
                        free(file);
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
            if (file->f_write) {
                  n = file->f_write(file->dev, file->fd, ptr, size, nitems, file->f_seek);
                  file->f_seek += n * size;
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
            if (file->f_read) {
                  n = file->f_read(file->dev, file->fd, ptr, size, nitems, file->f_seek);
                  file->f_seek += n * size;
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
 * @param mode                seek mode
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
            struct vfs_stat stat;

            if (mode == VFS_SEEK_END) {
                  stat.st_size = 0;
                  vfs_fstat(file, &stat);
            }

            switch (mode) {
            case VFS_SEEK_SET: file->f_seek  = offset; break;
            case VFS_SEEK_CUR: file->f_seek += offset; break;
            case VFS_SEEK_END: file->f_seek  = stat.st_size + offset; break;
            default: break;
            }

            status = STD_RET_OK;
      }

      return status;
}


//================================================================================================//
/**
 * @brief Function returns seek value
 *
 * @param *file               file object
 *
 * @return -1 if error, otherwise correct value
 */
//================================================================================================//
i32_t vfs_ftell(FILE_t *file)
{
      i32_t seek = -1;

      if (file) {
            seek = file->f_seek;
      }

      return seek;
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
            if (file->f_ioctl) {
                  status = file->f_ioctl(file->dev, file->fd, rq, data);
            }
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
stdRet_t vfs_fstat(FILE_t *file, struct vfs_stat *stat)
{
      stdRet_t status = STD_RET_ERROR;

      if (file && stat) {
            if (file->f_stat) {
                  status = file->f_stat(file->dev, file->fd, stat);
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

            if (strncmp(path, data->mntpoint, len) == 0) {
                  fsinfo = data;

                  if (itemid) {
                        if (ListGetItemID(vfs->mntList, i, itemid) != 0)
                              fsinfo = NULL;
                  }

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
 * @param **extPath     pointer to external part of path
 *
 * @return pointer to FS info
 */
//================================================================================================//
static struct fsinfo *FindBaseFS(const ch_t *path, ch_t **extPath)
{
      struct fsinfo *fsinfo = NULL;

      ch_t *pathTail = (ch_t*)path + strlen(path);

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
