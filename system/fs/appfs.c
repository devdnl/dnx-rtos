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
#include "fs/appfs.h"
#include "regprg.h"

/*==============================================================================
  Local symbolic constants/macros
==============================================================================*/

/*==============================================================================
  Local types, enums definitions
==============================================================================*/

/*==============================================================================
  Local function prototypes
==============================================================================*/
static dirent_t appfs_readrootdir(void *fshdl, dir_t *dir);
static stdret_t appfs_closedir(void *fshdl, dir_t *dir);

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
 * @brief Initialize file system
 *
 * @param[out] **fshdl          pointer to allocated memory by file system
 * @param[in]  *src_path        file source path
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
stdret_t appfs_init(void **fshdl, const char *src_path)
{
        (void)fshdl;
        (void)src_path;

        return STD_RET_OK;
}

//==============================================================================
/**
 * @brief Function release file system
 *
 * @param[in] *fshdl            FS handle
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
stdret_t appfs_release(void *fshdl)
{
        (void)fshdl;

        return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief Function open selected file
 *
 * @param[in]  *fshdl           FS handle
 * @param[out] *fd              file descriptor
 * @param[out] *seek            file position
 * @param[in]  *path            file path
 * @param[in]  *mode            file mode
 *
 * @retval STD_RET_OK           file opened/created
 * @retval STD_RET_ERROR        file not opened/created
 */
//==============================================================================
stdret_t appfs_open(void *fshdl, fd_t *fd, size_t *seek, const char *path, const char *mode)
{
        (void)fshdl;
        (void)fd;
        (void)seek;
        (void)path;
        (void)mode;

        return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief Function close file in LFS
 *
 * @param[in] *fshdl            FS handle
 * @param[in] fd                file descriptor
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
stdret_t appfs_close(void *fshdl, fd_t fd)
{
        (void)fshdl;
        (void)fd;

        return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief Function write data to the file
 *
 * @param[in] *fshdl            FS handle
 * @param[in]  fd               file descriptor
 * @param[in] *src              data source
 * @param[in]  size             item size
 * @param[in]  nitems           number of items
 * @param[in]  seek             position in file
 *
 * @return number of written items
 */
//==============================================================================
size_t appfs_write(void *fshdl, fd_t fd, void *src, size_t size, size_t nitems, size_t seek)
{
        (void)fshdl;
        (void)fd;
        (void)src;
        (void)size;
        (void)nitems;
        (void)seek;

        return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief Function read from file data
 *
 * @param[in]  *fshdl           FS handle
 * @param[in]   fd              file descriptor
 * @param[out] *dst             data destination
 * @param[in]   size            item size
 * @param[in]   nitems          number of items
 * @param[in]   seek            position in file
 *
 * @return number of read items
 */
//==============================================================================
size_t appfs_read(void *fshdl, fd_t fd, void *dst, size_t size, size_t nitems, size_t seek)
{
        (void)fshdl;
        (void)fd;
        (void)dst;
        (void)size;
        (void)nitems;
        (void)seek;

        return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief IO operations on files
 *
 * @param[in]     *fshdl        FS handle
 * @param[in]      fd           file descriptor
 * @param[in]      iorq         request
 * @param[in,out] *data         data pointer
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
stdret_t appfs_ioctl(void *fshdl, fd_t fd, iorq_t iorq, void *data)
{
        (void)fshdl;
        (void)fd;
        (void)iorq;
        (void)data;

        return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief Function flush file data
 *
 * @param[in]     *fshdl        FS handle
 * @param[in]      fd           file descriptor
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
stdret_t appfs_flush(void *fshdl, fd_t fd)
{
        (void)fshdl;
        (void)fd;

        return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief Function returns file status
 *
 * @param[in]  *fshdl                FS handle
 * @param[in]  fd                    file descriptor
 * @param[out] *stat                 pointer to status structure
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
stdret_t appfs_fstat(void *fshdl, fd_t fd, struct vfs_statf *stat)
{
        (void)fshdl;
        (void)fd;
        (void)stat;

        return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief Create directory
 *
 * @param[in] *fshdl            FS handle
 * @param[in] *path             path to new directory
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
stdret_t appfs_mkdir(void *fshdl, const char *path)
{
        (void)fshdl;
        (void)path;

        return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief Function create node for driver file
 *
 * @param[in] *fshdl            FS handle
 * @param[in] *path             path when driver-file shall be created
 * @param[in] *drv_if           pointer to driver interface
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
stdret_t appfs_mknod(void *fshdl, const char *path, struct vfs_drv_interface *drv_if)
{
        (void)fshdl;
        (void)path;
        (void)drv_if;

        return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief Function open directory
 *
 * @param[in]  *fshdl           FS handle
 * @param[in]  *path            directory path
 * @param[out] *dir             directory info
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
stdret_t appfs_opendir(void *fshdl, const char *path, dir_t *dir)
{
        (void)fshdl;

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
 * @brief Function close dir
 *
 * @param[in] *fshdl            FS handle
 * @param[in] *dir              directory info
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
static stdret_t appfs_closedir(void *fshdl, dir_t *dir)
{
        (void)fshdl;
        (void)dir;

        return STD_RET_OK;
}

//==============================================================================
/**
 * @brief Remove file
 *
 * @param[in] *fshdl            FS handle
 * @param[in] *patch            localization of file/directory
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
stdret_t appfs_remove(void *fshdl, const char *path)
{
        (void)fshdl;
        (void)path;

        return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief Rename file name
 *
 * @param[in] *fshdl                FS handle
 * @param[in] *oldName              old file name
 * @param[in] *newName              new file name
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
stdret_t appfs_rename(void *fshdl, const char *old_name, const char *new_name)
{
        (void)fshdl;
        (void)old_name;
        (void)new_name;

        return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief Function change file mode
 *
 * @param[in] *fshdl                FS handle
 * @param[in] *path                 path
 * @param[in]  mode                 file mode
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
stdret_t appfs_chmod(void *fshdl, const char *path, u32_t mode)
{
        (void)fshdl;
        (void)path;
        (void)mode;

        return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief Function change file owner and group
 *
 * @param[in] *fshdl                FS handle
 * @param[in] *path                 path
 * @param[in]  owner                file owner
 * @param[in]  group                file group
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
stdret_t appfs_chown(void *fshdl, const char *path, u16_t owner, u16_t group)
{
        (void)fshdl;
        (void)path;
        (void)owner;
        (void)group;

        return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief Function returns file/dir status
 *
 * @param[in]  *fshdl                FS handle
 * @param[in]  *path                 file/dir path
 * @param[out] *stat                 pointer to stat structure
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
stdret_t appfs_stat(void *fshdl, const char *path, struct vfs_statf *stat)
{
        (void)fshdl;

        if (path && stat) {
                stat->st_dev   = 0;
                stat->st_gid   = 0;
                stat->st_mode  = 0444;
                stat->st_mtime = 0;
                stat->st_size  = 0;
                stat->st_uid   = 0;

                return STD_RET_OK;
        }

        return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief Function returns FS status
 *
 * @param[in]  *fshdl               FS handle
 * @param[out] *statfs              pointer to status structure
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
stdret_t appfs_statfs(void *fshdl, struct vfs_statfs *statfs)
{
        (void)fshdl;

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
 * @brief Read item from opened directory
 *
 * @param fsd             file system descriptor
 * @param *dir            directory object
 *
 * @return directory entry
 */
//==============================================================================
static dirent_t appfs_readrootdir(void *fshdl, dir_t *dir)
{
        (void)fshdl;

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
