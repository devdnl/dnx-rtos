/*=============================================================================================*//**
@file    procfs.c

@author  Daniel Zorychta

@brief   This file support process file system (procfs)

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
#include "procfs.h"
#include "sysdrv.h"
#include "print.h"
#include <string.h>


/*==================================================================================================
                                  Local symbolic constants/macros
==================================================================================================*/
/* task name length (8B name + \0). Name is 32b hex number converted to string */
#define TASK_NAME_LEN               9


/*==================================================================================================
                                   Local types, enums definitions
==================================================================================================*/


/*==================================================================================================
                                      Local function prototypes
==================================================================================================*/
static stdRet_t procfs_closedir(devx_t dev, DIR_t *dir);
static stdRet_t procfs_closedir_noop(devx_t dev, DIR_t *dir);
static dirent_t procfs_readdir_root(devx_t dev, DIR_t *dir);
static dirent_t procfs_readdir_taskname(devx_t dev, DIR_t *dir);
static dirent_t procfs_readdir_taskid(devx_t dev, DIR_t *dir);
static dirent_t procfs_readdir_taskid_n(devx_t dev, DIR_t *dir);


/*==================================================================================================
                                      Local object definitions
==================================================================================================*/


/*==================================================================================================
                                     Exported object definitions
==================================================================================================*/


/*==================================================================================================
                                        Function definitions
==================================================================================================*/

//================================================================================================//
/**
 * @brief Function initialize appfs
 *
 * @param dev           device number
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//================================================================================================//
stdRet_t procfs_init(devx_t dev)
{
      (void)dev;

      return STD_RET_OK;
}


//================================================================================================//
/**
 * @brief Function open selected file
 *
 * @param  dev          device number
 * @param *fd           file descriptor
 * @param *seek         file position
 * @param *path         file name
 * @param *mode         file mode
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//================================================================================================//
stdRet_t procfs_open(devx_t dev, fd_t *fd, size_t *seek, const ch_t *path, const ch_t *mode)
{
      (void)dev;
      (void)fd;
      (void)seek;
      (void)path;
      (void)mode;

      return STD_RET_ERROR;
}


//================================================================================================//
/**
 * @brief Close file
 *
 * @param  dev          device number
 * @param *fd           file descriptor
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//================================================================================================//
stdRet_t procfs_close(devx_t dev, fd_t fd)
{
      (void)dev;
      (void)fd;

      return STD_RET_ERROR;
}


//================================================================================================//
/**
 * @brief Write data file
 *
 * @param  dev          device number
 * @param *fd           file descriptor
 * @param *src          data source
 * @param  size         item size
 * @param  nitems       item count
 * @param  seek         file position
 *
 * @return written nitems
 */
//================================================================================================//
size_t procfs_write(devx_t dev, fd_t fd, void *src, size_t size, size_t nitems, size_t seek)
{
      (void)dev;
      (void)fd;
      (void)src;
      (void)size;
      (void)nitems;
      (void)seek;

      return STD_RET_ERROR;
}


//================================================================================================//
/**
 * @brief Read data files
 *
 * @param  dev          device number
 * @param *fd           file descriptor
 * @param *src          data source
 * @param  size         item size
 * @param  nitems       item count
 * @param  seek         file position
 *
 * @retval read nitems
 */
//================================================================================================//
size_t procfs_read(devx_t dev, fd_t fd, void *dst, size_t size, size_t nitems, size_t seek)
{
      (void)dev;
      (void)fd;
      (void)dst;
      (void)size;
      (void)nitems;
      (void)seek;

      return STD_RET_ERROR;
}


//================================================================================================//
/**
 * @brief Control file
 *
 * @param  dev          device number
 * @param  fd           file descriptor
 * @param  iorq         request
 * @param *data         data
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//================================================================================================//
stdRet_t procfs_ioctl(devx_t dev, fd_t fd, IORq_t iorq, void *data)
{
      (void)dev;
      (void)fd;
      (void)iorq;
      (void)data;

      return STD_RET_ERROR;
}


//================================================================================================//
/**
 * @brief Statistics of opened file
 *
 * @param  dev          device number
 * @param *fd           file descriptor
 * @param *stat         output statistics
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//================================================================================================//
stdRet_t procfs_fstat(devx_t dev, fd_t fd, struct vfs_stat *stat)
{
      (void)dev;
      (void)fd;
      (void)stat;

      return STD_RET_ERROR;
}


//================================================================================================//
/**
 * @brief Create directory
 *
 * @param  dev          device number
 * @param *path         directory path
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//================================================================================================//
stdRet_t procfs_mkdir(devx_t dev, const ch_t *path)
{
      (void)dev;
      (void)path;

      return STD_RET_ERROR;
}


//================================================================================================//
/**
 * @brief Create device node
 *
 * @param  dev          device number
 * @param *path         node path
 * @param *dcfg         device configuration
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//================================================================================================//
stdRet_t procfs_mknod(devx_t dev, const ch_t *path, struct vfs_drvcfg *dcfg)
{
      (void)dev;
      (void)path;
      (void)dcfg;

      return STD_RET_ERROR;
}


//================================================================================================//
/**
 * @brief Opens directory
 *
 * @param  dev          device number
 * @param *path         directory path
 * @param *dir          directory object to fill
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//================================================================================================//
stdRet_t procfs_opendir(devx_t dev, const ch_t *path, DIR_t *dir)
{
      (void)dev;

      stdRet_t status = STD_RET_ERROR;

      if (path && dir) {
            status    = STD_RET_OK;
            dir->seek = 0;

            if (strcmp(path, "/") == 0) {
                  dir->dd    = NULL;
                  dir->items = 2;
                  dir->rddir = procfs_readdir_root;
                  dir->cldir = procfs_closedir_noop;
            } else if (strcmp(path, "/taskname/") == 0) {
                  dir->dd    = NULL;
                  dir->items = moni_GetTaskCount();
                  dir->rddir = procfs_readdir_taskname;
                  dir->cldir = procfs_closedir_noop;
            } else if (strcmp(path, "/taskid/") == 0) {
                  dir->dd    = calloc(TASK_NAME_LEN, sizeof(ch_t));
                  dir->items = moni_GetTaskCount();
                  dir->rddir = procfs_readdir_taskid;
                  dir->cldir = procfs_closedir;
            } else if (strncmp(path, "/taskid/", 7) == 0) {
                  path = strchr(path + 1, '/') + 1;

                  if (path) {
                        u32_t taskHdl = 0;
                        struct taskstat taskdata;

                        atoi((ch_t*)path, 16, (i32_t*)&taskHdl);

                        if ((status = moni_GetTaskHdlStat((task_t)taskHdl, &taskdata)) == STD_RET_OK) {
                              dir->dd    = (void*)taskHdl;
                              dir->items = 6;
                              dir->rddir = procfs_readdir_taskid_n;
                              dir->cldir = procfs_closedir_noop;
                        }
                  }
            } else {
                  status = STD_RET_ERROR;
            }
      }

      return status;
}


//================================================================================================//
/**
 * @brief Function close opened dir (is used when dd contains pointer to allocated block)
 *
 * @param  dev          device number
 * @param *dir          directory object
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//================================================================================================//
static stdRet_t procfs_closedir(devx_t dev, DIR_t *dir)
{
      (void)dev;

      stdRet_t status = STD_RET_ERROR;

      if (dir) {
            if (dir->dd) {
                  free(dir->dd);
                  dir->dd = NULL;

                  status = STD_RET_OK;
            }
      }

      return status;
}


//================================================================================================//
/**
 * @brief Function close opened dir (is used when dd contains data which cannot be freed)
 *
 * @param  dev          device number
 * @param *dir          directory object
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//================================================================================================//
static stdRet_t procfs_closedir_noop(devx_t dev, DIR_t *dir)
{
      (void)dev;
      (void)dir;

      return STD_RET_OK;
}


//================================================================================================//
/**
 * @brief Remove file
 *
 * @param  dev          device number
 * @param *path         file path
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//================================================================================================//
stdRet_t procfs_remove(devx_t dev, const ch_t *path)
{
      (void)dev;
      (void)path;

      return STD_RET_ERROR;
}


//================================================================================================//
/**
 * @brief Rename file
 *
 * @param  dev          device number
 * @param *oldName      old file name
 * @param *newName      new file name
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//================================================================================================//
stdRet_t procfs_rename(devx_t dev, const ch_t *oldName, const ch_t *newName)
{
      (void)dev;
      (void)oldName;
      (void)newName;

      return STD_RET_ERROR;
}


//================================================================================================//
/**
 * @brief Change file mode
 *
 * @param  dev          device number
 * @param *path         file path
 * @param  mode         new mode
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//================================================================================================//
stdRet_t procfs_chmod(devx_t dev, const ch_t *path, u32_t mode)
{
      (void)dev;
      (void)path;
      (void)mode;

      return STD_RET_ERROR;
}


//================================================================================================//
/**
 * @brief Change file owner and group
 *
 * @param  dev          device number
 * @param *path         file path
 * @param  owner        owner
 * @param  group        group
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//================================================================================================//
stdRet_t procfs_chown(devx_t dev, const ch_t *path, u16_t owner, u16_t group)
{
      (void)dev;
      (void)path;
      (void)owner;
      (void)group;

      return STD_RET_ERROR;
}


//================================================================================================//
/**
 * @brief File statistics
 *
 * @param  dev          device number
 * @param *path         file path
 * @param *stat         file statistics
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//================================================================================================//
stdRet_t procfs_stat(devx_t dev, const ch_t *path, struct vfs_stat *stat)
{
      (void)dev;

      stdRet_t status = STD_RET_ERROR;

      if (path && stat) {
            stat->st_dev   = 0;
            stat->st_gid   = 0;
            stat->st_mode  = 0444;
            stat->st_mtime = 0;
            stat->st_rdev  = 0;
            stat->st_size  = 0;
            stat->st_uid   = 0;

            status = STD_RET_OK;
      }

      return status;
}


//================================================================================================//
/**
 * @brief File system statistics
 *
 * @param  dev          device number
 * @param *statfs       FS statistics
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//================================================================================================//
stdRet_t procfs_statfs(devx_t dev, struct vfs_statfs *statfs)
{
      (void)dev;

      stdRet_t status = STD_RET_ERROR;

      if (statfs) {
            statfs->f_bfree  = 0;
            statfs->f_blocks = 0;
            statfs->f_ffree  = 0;
            statfs->f_files  = 0;
            statfs->f_type   = 1;
            statfs->fsname   = "procfs";

            status = STD_RET_OK;
      }

      return status;
}


//================================================================================================//
/**
 * @brief Release file system
 *
 * @param dev           device number
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//================================================================================================//
stdRet_t procfs_release(devx_t dev)
{
      (void)dev;

      return STD_RET_OK;
}


//================================================================================================//
/**
 * @brief Read item from opened directory
 *
 * @param *dir          directory object
 *
 * @return directory entry
 */
//================================================================================================//
static dirent_t procfs_readdir_root(devx_t dev, DIR_t *dir)
{
      (void)dev;

      dirent_t dirent;
      dirent.name = NULL;
      dirent.size = 0;

      if (dir) {
            dirent.filetype = FILE_TYPE_DIR;
            dirent.size     = 0;

            switch (dir->seek++) {
            case 0: dirent.name = "taskid";   break;
            case 1: dirent.name = "taskname"; break;
            default: break;
            }
      }

      return dirent;
}

//================================================================================================//
/**
 * @brief Read item from opened directory
 *
 * @param *dir          directory object
 *
 * @return directory entry
 */
//================================================================================================//
static dirent_t procfs_readdir_taskname(devx_t dev, DIR_t *dir)
{
      (void)dev;

      dirent_t dirent;
      dirent.name = NULL;
      dirent.size = 0;

      struct taskstat taskdata;

      if (dir) {
            if (moni_GetTaskStat(dir->seek, &taskdata) == STD_RET_OK) {
                  dirent.filetype = FILE_TYPE_REGULAR;
                  dirent.name     = taskdata.taskName;
                  dirent.size     = 0;

                  dir->seek++;
            }
      }

      return dirent;
}


//================================================================================================//
/**
 * @brief Read item from opened directory
 *
 * @param *dir          directory object
 *
 * @return directory entry
 */
//================================================================================================//
static dirent_t procfs_readdir_taskid(devx_t dev, DIR_t *dir)
{
      (void)dev;

      dirent_t dirent;
      dirent.name = NULL;
      dirent.size = 0;

      struct taskstat taskdata;

      if (dir) {
            if (dir->dd && dir->seek < moni_GetTaskCount()) {
                  if (moni_GetTaskStat(dir->seek, &taskdata) == STD_RET_OK) {
                        snprintf(dir->dd, TASK_NAME_LEN, "%x", (u32_t)taskdata.taskHdl);

                        dirent.filetype = FILE_TYPE_DIR;
                        dirent.name     = dir->dd;
                        dirent.size     = 0;

                        dir->seek++;
                  }
            }
      }

      return dirent;
}


//================================================================================================//
/**
 * @brief Read item from opened directory
 *
 * @param *dir          directory object
 *
 * @return directory entry
 */
//================================================================================================//
static dirent_t procfs_readdir_taskid_n(devx_t dev, DIR_t *dir)
{
      (void)dev;

      dirent_t dirent;
      dirent.name = NULL;
      dirent.size = 0;

      if (dir) {
            struct taskstat taskdata;

            if (dir->seek < 6 && moni_GetTaskHdlStat((task_t)dir->dd, &taskdata) == STD_RET_OK) {
                  dirent.filetype = FILE_TYPE_REGULAR;
                  dirent.size = sizeof(u32_t);

                  switch (dir->seek) {
                  case 0: dirent.name = "name"; dirent.size = strlen(taskdata.taskName); break;
                  case 1: dirent.name = "priority";  break;
                  case 2: dirent.name = "freestack"; break;
                  case 3: dirent.name = "usedmem";   break;
                  case 4: dirent.name = "openfiles"; break;
                  case 5: dirent.name = "cpuload";   break;
                  default: break;
                  }

                  dir->seek++;
            }
      }

      return dirent;
}


#ifdef __cplusplus
}
#endif

/*==================================================================================================
                                            End of file
==================================================================================================*/
