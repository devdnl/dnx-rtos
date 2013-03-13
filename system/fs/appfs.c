/*=========================================================================*//**
@file    appfs.c

@author  Daniel Zorychta

@brief   This file implement application file system

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
#include "appfs.h"
#include "regprg.h"
#include <string.h>

/*==============================================================================
  Local symbolic constants/macros
==============================================================================*/

/*==============================================================================
  Local types, enums definitions
==============================================================================*/

/*==============================================================================
  Local function prototypes
==============================================================================*/
static dirent_t appfs_readrootdir(fsd_t fsd, DIR_t *dir);
static stdRet_t appfs_closedir(fsd_t fsd, DIR_t *dir);

/*==============================================================================
  Local object definitions
==============================================================================*/

/*==============================================================================
  Exported object definitions
==============================================================================*/

/*==============================================================================
  Function definitions
==============================================================================*/

//==============================================================================
/**
 * @brief Function initialize appfs
 *
 * @param[in]  *srcPath         source path
 * @param[out] *fsd             file system descriptor
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
stdRet_t appfs_init(const char *srcPath, fsd_t *fsd)
{
        (void)fsd;
        (void)srcPath;

        return STD_RET_OK;
}

//==============================================================================
/**
 * @brief Function open selected file
 *
 * @param  fsd            file system descriptor
 * @param *fd             file descriptor
 * @param *seek           file position
 * @param *path           file name
 * @param *mode           file mode
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
stdRet_t appfs_open(fsd_t fsd, fd_t *fd, size_t *seek, const char *path, const char *mode)
{
        (void)fsd;
        (void)fd;
        (void)seek;
        (void)path;
        (void)mode;

        return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief Close file
 *
 * @param fsd             file system descriptor
 * @param *fd             file descriptor
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
stdRet_t appfs_close(fsd_t fsd, fd_t fd)
{
        (void)fsd;
        (void)fd;

        return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief Write data file
 *
 * @param  fsd            file system descriptor
 * @param *fd             file descriptor
 * @param *src            data source
 * @param  size           item size
 * @param  nitems         item count
 * @param  seek           file position
 *
 * @return written nitems
 */
//==============================================================================
size_t appfs_write(fsd_t fsd, fd_t fd, void *src, size_t size, size_t nitems, size_t seek)
{
        (void)fsd;
        (void)fd;
        (void)src;
        (void)size;
        (void)nitems;
        (void)seek;

        return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief Read data files
 *
 * @param  fsd            file system descriptor
 * @param *fd             file descriptor
 * @param *src            data source
 * @param  size           item size
 * @param  nitems         item count
 * @param  seek           file position
 *
 * @retval read nitems
 */
//==============================================================================
size_t appfs_read(fsd_t fsd, fd_t fd, void *dst, size_t size, size_t nitems, size_t seek)
{
        (void)fsd;
        (void)fd;
        (void)dst;
        (void)size;
        (void)nitems;
        (void)seek;

        return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief Control file
 *
 * @param  fsd            file system descriptor
 * @param  fd             file descriptor
 * @param  iorq           request
 * @param *data           data
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
stdRet_t appfs_ioctl(fsd_t fsd, fd_t fd, iorq_t iorq, void *data)
{
        (void)fsd;
        (void)fd;
        (void)iorq;
        (void)data;

        return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief Statistics of opened file
 *
 * @param fsd             file system descriptor
 * @param *fd             file descriptor
 * @param *stat           output statistics
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
stdRet_t appfs_fstat(fsd_t fsd, fd_t fd, struct vfs_stat *stat)
{
        (void)fsd;
        (void)fd;
        (void)stat;

        return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief Create directory
 *
 * @param fsd             file system descriptor
 * @param *path           directory path
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
stdRet_t appfs_mkdir(fsd_t fsd, const char *path)
{
        (void)fsd;
        (void)path;

        return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief Create device node
 *
 * @param fsd             file system descriptor
 * @param *path           node path
 * @param *dcfg           device configuration
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
stdRet_t appfs_mknod(fsd_t fsd, const char *path, struct vfs_drvcfg *dcfg)
{
        (void)fsd;
        (void)path;
        (void)dcfg;

        return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief Opens directory
 *
 * @param fsd             file system descriptor
 * @param *path           directory path
 * @param *dir            directory object to fill
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
stdRet_t appfs_opendir(fsd_t fsd, const char *path, DIR_t *dir)
{
        (void)fsd;

        if (path && dir) {
                if (path[0] == '/' && strlen(path) == 1) {
                        dir->dd    = 0;
                        dir->items = regprg_get_program_count();
                        dir->rddir = appfs_readrootdir;
                        dir->cldir = appfs_closedir;
                        dir->seek  = 0;

                        return STD_RET_OK;
                }
        }

        return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief Function closed opened dir
 *
 * @param  fsd            file system descriptor
 * @param *dir            directory object
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
static stdRet_t appfs_closedir(fsd_t fsd, DIR_t *dir)
{
        (void)fsd;
        (void)dir;

        return STD_RET_OK;
}

//==============================================================================
/**
 * @brief Remove file
 *
 * @param fsd             file system descriptor
 * @param *path           file path
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
stdRet_t appfs_remove(fsd_t fsd, const char *path)
{
        (void)fsd;
        (void)path;

        return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief Rename file
 *
 * @param fsd             file system descriptor
 * @param *oldName        old file name
 * @param *newName        new file name
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
stdRet_t appfs_rename(fsd_t fsd, const char *oldName, const char *newName)
{
        (void)fsd;
        (void)oldName;
        (void)newName;

        return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief Change file mode
 *
 * @param fsd             file system descriptor
 * @param *path           file path
 * @param mode            new mode
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
stdRet_t appfs_chmod(fsd_t fsd, const char *path, u32_t mode)
{
        (void)fsd;
        (void)path;
        (void)mode;

        return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief Change file owner and group
 *
 * @param fsd             file system descriptor
 * @param *path           file path
 * @param owner           owner
 * @param group           group
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
stdRet_t appfs_chown(fsd_t fsd, const char *path, u16_t owner, u16_t group)
{
        (void)fsd;
        (void)path;
        (void)owner;
        (void)group;

        return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief File statistics
 *
 * @param fsd             file system descriptor
 * @param *path           file path
 * @param *stat           file statistics
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
stdRet_t appfs_stat(fsd_t fsd, const char *path, struct vfs_stat *stat)
{
        (void)fsd;

        if (path && stat) {
                stat->st_dev   = 0;
                stat->st_gid   = 0;
                stat->st_mode  = 0444;
                stat->st_mtime = 0;
                stat->st_rdev  = 0;
                stat->st_size  = 0;
                stat->st_uid   = 0;

                return STD_RET_OK;
        }

        return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief File system statistics
 *
 * @param fsd             file system descriptor
 * @param *statfs         FS statistics
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
stdRet_t appfs_statfs(fsd_t fsd, struct vfs_statfs *statfs)
{
        (void)fsd;

        if (statfs) {
                statfs->f_bfree  = 0;
                statfs->f_blocks = 0;
                statfs->f_ffree  = 0;
                statfs->f_files  = 0;
                statfs->f_type   = 1;
                statfs->fsname   = "appfs";

                return STD_RET_OK;
        }

        return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief Release file system
 *
 * @param fsd             file system descriptor
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
stdRet_t appfs_release(fsd_t fsd)
{
        (void)fsd;

        return STD_RET_OK;
}

//==============================================================================
/**
 * @brief Read item from opened directory
 *
 * @param fsd             file system descriptor
 * @param *dir            directory object
 *
 * @return directory entry
 */
//==============================================================================
static dirent_t appfs_readrootdir(fsd_t fsd, DIR_t *dir)
{
        (void)fsd;

        dirent_t dirent;
        dirent.name = NULL;
        dirent.size = 0;

        if (dir) {
                if (dir->seek < (size_t)regprg_get_program_count()) {
                        dirent.filetype = FILE_TYPE_REGULAR;
                        dirent.name     = (char*)regprg_get_pointer_to_program_list()[dir->seek].program_name;
                        dirent.size     = 0;
                        dir->seek++;
                }
        }

        return dirent;
}

#ifdef __cplusplus
}
#endif

/*==============================================================================
  End of file
==============================================================================*/
