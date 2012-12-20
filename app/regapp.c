/*=============================================================================================*//**
@file    regapp.c

@author  Daniel Zorychta

@brief   This file is used to registration applications

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
#include "regapp.h"
#include <string.h>

/* include here applications headers */
#include "terminal.h"
#include "date.h"
#include "top.h"
#include "httpd.h"
#include "measd.h"
#include "cat.h"


/*==================================================================================================
                                  Local symbolic constants/macros
==================================================================================================*/


/*==================================================================================================
                                   Local types, enums definitions
==================================================================================================*/


/*==================================================================================================
                                      Local function prototypes
==================================================================================================*/
static dirent_t appfs_readrootdir(devx_t dev, DIR_t *dir);
static stdRet_t appfs_closedir(devx_t dev, DIR_t *dir);


/*==================================================================================================
                                      Local object definitions
==================================================================================================*/
static const regAppData_t appList[] =
{
      {TERMINAL_NAME, terminal, TERMINAL_STACK_SIZE},
      {DATE_NAME    , date    , DATE_STACK_SIZE    },
      {TOP_NAME     , top     , TOP_STACK_SIZE     },
      {HTTPD_NAME   , httpd   , HTTPD_STACK_SIZE   },
      {MEASD_NAME   , measd   , MEASD_STACK_SIZE   },
      {CAT_NAME     , cat     , CAT_STACK_SIZE     },
};


/*==================================================================================================
                                     Exported object definitions
==================================================================================================*/


/*==================================================================================================
                                        Function definitions
==================================================================================================*/

//================================================================================================//
/**
 * @brief Function find in the application list selected application's parameters
 *
 * @param *appName            application name
 *
 * @return application informations needed to run
 */
//================================================================================================//
regAppData_t GetAppData(const ch_t *appName)
{
      u32_t i;

      for (i = 0; i < ARRAY_SIZE(appList); i++)
      {
            if (strcmp(appList[i].appName, appName) == 0)
            {
                  return appList[i];
            }
      }

      regAppData_t appNULL = {NULL, NULL, 0};

      return appNULL;
}


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
stdRet_t appfs_init(devx_t dev)
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
stdRet_t appfs_open(devx_t dev, fd_t *fd, size_t *seek, const ch_t *path, const ch_t *mode)
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
 * @param dev           device number
 * @param *fd           file descriptor
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//================================================================================================//
stdRet_t appfs_close(devx_t dev, fd_t fd)
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
size_t appfs_write(devx_t dev, fd_t fd, void *src, size_t size, size_t nitems, size_t seek)
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
size_t appfs_read(devx_t dev, fd_t fd, void *dst, size_t size, size_t nitems, size_t seek)
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
stdRet_t appfs_ioctl(devx_t dev, fd_t fd, IORq_t iorq, void *data)
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
 * @param dev           device number
 * @param *fd           file descriptor
 * @param *stat         output statistics
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//================================================================================================//
stdRet_t appfs_fstat(devx_t dev, fd_t fd, struct vfs_stat *stat)
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
 * @param dev           device number
 * @param *path         directory path
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//================================================================================================//
stdRet_t appfs_mkdir(devx_t dev, const ch_t *path)
{
      (void)dev;
      (void)path;

      return STD_RET_ERROR;
}


//================================================================================================//
/**
 * @brief Create device node
 *
 * @param dev           device number
 * @param *path         node path
 * @param *dcfg         device configuration
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//================================================================================================//
stdRet_t appfs_mknod(devx_t dev, const ch_t *path, struct vfs_drvcfg *dcfg)
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
 * @param dev           device number
 * @param *path         directory path
 * @param *dir          directory object to fill
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//================================================================================================//
stdRet_t appfs_opendir(devx_t dev, const ch_t *path, DIR_t *dir)
{
      (void)dev;

      stdRet_t status = STD_RET_ERROR;

      if (path && dir) {
            if (path[0] == '/' && strlen(path) == 1) {
                  dir->dd    = 0;
                  dir->items = ARRAY_SIZE(appList);
                  dir->rddir = appfs_readrootdir;
                  dir->cldir = appfs_closedir;
                  dir->seek  = 0;

                  status = STD_RET_OK;
            }
      }

      return status;
}


//================================================================================================//
/**
 * @brief Function closed opened dir
 *
 * @param  dev          device number
 * @param *dir          directory object
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//================================================================================================//
static stdRet_t appfs_closedir(devx_t dev, DIR_t *dir)
{
      (void)dev;
      (void)dir;

      return STD_RET_OK;
}


//================================================================================================//
/**
 * @brief Remove file
 *
 * @param dev           device number
 * @param *path         file path
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//================================================================================================//
stdRet_t appfs_remove(devx_t dev, const ch_t *path)
{
      (void)dev;
      (void)path;

      return STD_RET_ERROR;
}


//================================================================================================//
/**
 * @brief Rename file
 *
 * @param dev           device number
 * @param *oldName      old file name
 * @param *newName      new file name
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//================================================================================================//
stdRet_t appfs_rename(devx_t dev, const ch_t *oldName, const ch_t *newName)
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
 * @param dev           device number
 * @param *path         file path
 * @param mode          new mode
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//================================================================================================//
stdRet_t appfs_chmod(devx_t dev, const ch_t *path, u32_t mode)
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
 * @param dev           device number
 * @param *path         file path
 * @param owner         owner
 * @param group         group
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//================================================================================================//
stdRet_t appfs_chown(devx_t dev, const ch_t *path, u16_t owner, u16_t group)
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
 * @param dev           device number
 * @param *path         file path
 * @param *stat         file statistics
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//================================================================================================//
stdRet_t appfs_stat(devx_t dev, const ch_t *path, struct vfs_stat *stat)
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
 * @param dev           device number
 * @param *statfs       FS statistics
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//================================================================================================//
stdRet_t appfs_statfs(devx_t dev, struct vfs_statfs *statfs)
{
      (void)dev;

      stdRet_t status = STD_RET_ERROR;

      if (statfs) {
            statfs->f_bfree  = 0;
            statfs->f_blocks = 0;
            statfs->f_ffree  = 0;
            statfs->f_files  = 0;
            statfs->f_type   = 1;
            statfs->fsname   = "appfs";

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
stdRet_t appfs_release(devx_t dev)
{
      (void)dev;

      return STD_RET_OK;
}


//================================================================================================//
/**
 * @brief Read item from opened directory
 *
 * @param dev           device number
 * @param *dir          directory object
 *
 * @return directory entry
 */
//================================================================================================//
static dirent_t appfs_readrootdir(devx_t dev, DIR_t *dir)
{
      (void)dev;

      dirent_t dirent;
      dirent.name = NULL;
      dirent.size = 0;

      if (dir) {
            if (dir->seek < ARRAY_SIZE(appList)) {
                  dirent.filetype = FILE_TYPE_REGULAR;
                  dirent.name     = (ch_t*)appList[dir->seek].appName;
                  dirent.size     = 0;
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
