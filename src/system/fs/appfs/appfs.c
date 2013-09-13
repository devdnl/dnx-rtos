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
#include "user/regprg.h"

/*==============================================================================
  Local symbolic constants/macros
==============================================================================*/

/*==============================================================================
  Local types, enums definitions
==============================================================================*/

/*==============================================================================
  Local function prototypes
==============================================================================*/
static dirent_t appfs_readrootdir   (void *fs_handle, dir_t *dir);
static stdret_t appfs_closedir      (void *fs_handle, dir_t *dir);

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
 * @param[out] **fs_handle      pointer to allocated memory by file system
 * @param[in]  *src_path        file source path
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
stdret_t appfs_init(void **fs_handle, const char *src_path)
{
        UNUSED_ARG(fs_handle);
        UNUSED_ARG(src_path);

        return STD_RET_OK;
}

//==============================================================================
/**
 * @brief Function release file system
 *
 * @param[in] *fs_handle            FS handle
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
stdret_t appfs_release(void *fs_handle)
{
        UNUSED_ARG(fs_handle);

        return STD_RET_OK;
}

//==============================================================================
/**
 * @brief Function open selected file
 *
 * @param[in]  *fs_handle       FS handle
 * @param[out] **extra          file extra data (useful in FS wrappers)
 * @param[out] *fd              file descriptor
 * @param[out] *lseek           file position
 * @param[in]  *path            file path
 * @param[in]   flags           file open flags
 *
 * @retval STD_RET_OK           file opened/created
 * @retval STD_RET_ERROR        file not opened/created
 */
//==============================================================================
stdret_t appfs_open(void *fs_handle, void **extra, fd_t *fd, u64_t *lseek, const char *path, int flags)
{
        UNUSED_ARG(fs_handle);
        UNUSED_ARG(extra);
        UNUSED_ARG(fd);
        UNUSED_ARG(lseek);
        UNUSED_ARG(path);
        UNUSED_ARG(flags);

        return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief Function close file in LFS
 *
 * @param[in] *fs_handle        FS handle
 * @param[in] *extra            file extra data (useful in FS wrappers)
 * @param[in]  fd               file descriptor
 * @param[in]  forced           force close
 * @param[in] *task             task which opened file
 *
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
stdret_t appfs_close(void *fs_handle, void *extra, fd_t fd, bool forced, task_t *task)
{
        UNUSED_ARG(fs_handle);
        UNUSED_ARG(extra);
        UNUSED_ARG(fd);
        UNUSED_ARG(forced);
        UNUSED_ARG(task);

        return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief Function write data to the file
 *
 * @param[in] *fs_handle        FS handle
 * @param[in] *extra            file extra data (useful in FS wrappers)
 * @param[in]  fd               file descriptor
 * @param[in] *src              data source
 * @param[in]  size             item size
 * @param[in]  nitems           number of items
 * @param[in]  lseek            position in file
 *
 * @return number of written items
 */
//==============================================================================
size_t appfs_write(void *fs_handle, void *extra, fd_t fd, const void *src, size_t size, size_t nitems, u64_t lseek)
{
        UNUSED_ARG(fs_handle);
        UNUSED_ARG(extra);
        UNUSED_ARG(fd);
        UNUSED_ARG(src);
        UNUSED_ARG(size);
        UNUSED_ARG(nitems);
        UNUSED_ARG(lseek);

        return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief Function read from file data
 *
 * @param[in]  *fs_handle       FS handle
 * @param[in]  *extra           file extra data (useful in FS wrappers)
 * @param[in]   fd              file descriptor
 * @param[out] *dst             data destination
 * @param[in]   size            item size
 * @param[in]   nitems          number of items
 * @param[in]   lseek           position in file
 *
 * @return number of read items
 */
//==============================================================================
size_t appfs_read(void *fs_handle, void *extra, fd_t fd, void *dst, size_t size, size_t nitems, u64_t lseek)
{
        UNUSED_ARG(fs_handle);
        UNUSED_ARG(extra);
        UNUSED_ARG(fd);
        UNUSED_ARG(dst);
        UNUSED_ARG(size);
        UNUSED_ARG(nitems);
        UNUSED_ARG(lseek);

        return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief IO operations on files
 *
 * @param[in]     *fs_handle    FS handle
 * @param[in]     *extra        file extra data (useful in FS wrappers)
 * @param[in]      fd           file descriptor
 * @param[in]      iorq         request
 * @param[in,out]  args         additional arguments
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
stdret_t appfs_ioctl(void *fs_handle, void *extra, fd_t fd, int iorq, va_list args)
{
        UNUSED_ARG(fs_handle);
        UNUSED_ARG(extra);
        UNUSED_ARG(fd);
        UNUSED_ARG(iorq);
        UNUSED_ARG(args);

        return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief Function flush file data
 *
 * @param[in]     *fs_handle    FS handle
 * @param[in]     *extra        file extra data (useful in FS wrappers)`
 * @param[in]      fd           file descriptor
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
stdret_t appfs_flush(void *fs_handle, void *extra, fd_t fd)
{
        UNUSED_ARG(fs_handle);
        UNUSED_ARG(extra);
        UNUSED_ARG(fd);

        return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief Function returns file status
 *
 * @param[in]  *fs_handle            FS handle
 * @param[in]  *extra                file extra data (useful in FS wrappers)
 * @param[in]   fd                   file descriptor
 * @param[out] *stat                 pointer to status structure
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
stdret_t appfs_fstat(void *fs_handle, void *extra, fd_t fd, struct vfs_stat *stat)
{
        UNUSED_ARG(fs_handle);
        UNUSED_ARG(extra);
        UNUSED_ARG(fd);
        UNUSED_ARG(stat);

        return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief Create directory
 *
 * @param[in] *fs_handle        FS handle
 * @param[in] *path             path to new directory
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
stdret_t appfs_mkdir(void *fs_handle, const char *path)
{
        UNUSED_ARG(fs_handle);
        UNUSED_ARG(path);

        return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief Function create node for driver file
 *
 * @param[in] *fs_handle        FS handle
 * @param[in] *path             path when driver-file shall be created
 * @param[in] *drv_if           pointer to driver interface
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
stdret_t appfs_mknod(void *fs_handle, const char *path, struct vfs_drv_interface *drv_if)
{
        UNUSED_ARG(fs_handle);
        UNUSED_ARG(path);
        UNUSED_ARG(drv_if);

        return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief Function open directory
 *
 * @param[in]  *fs_handle       FS handle
 * @param[in]  *path            directory path
 * @param[out] *dir             directory info
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
stdret_t appfs_opendir(void *fs_handle, const char *path, dir_t *dir)
{
        UNUSED_ARG(fs_handle);

        STOP_IF(!path);
        STOP_IF(!dir);

        if (path[0] == '/' && strlen(path) == 1) {
                dir->f_dd       = 0;
                dir->f_items    = regprg_get_program_count();
                dir->f_readdir  = appfs_readrootdir;
                dir->f_closedir = appfs_closedir;
                dir->f_seek     = 0;

                return STD_RET_OK;
        }

        return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief Function close dir
 *
 * @param[in] *fs_handle        FS handle
 * @param[in] *dir              directory info
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
static stdret_t appfs_closedir(void *fs_handle, dir_t *dir)
{
        UNUSED_ARG(fs_handle);
        UNUSED_ARG(dir);

        return STD_RET_OK;
}

//==============================================================================
/**
 * @brief Remove file
 *
 * @param[in] *fs_handle        FS handle
 * @param[in] *patch            localization of file/directory
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
stdret_t appfs_remove(void *fs_handle, const char *path)
{
        UNUSED_ARG(fs_handle);
        UNUSED_ARG(path);

        return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief Rename file name
 *
 * @param[in] *fs_handle            FS handle
 * @param[in] *oldName              old file name
 * @param[in] *newName              new file name
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
stdret_t appfs_rename(void *fs_handle, const char *old_name, const char *new_name)
{
        UNUSED_ARG(fs_handle);
        UNUSED_ARG(old_name);
        UNUSED_ARG(new_name);

        return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief Function change file mode
 *
 * @param[in] *fs_handle            FS handle
 * @param[in] *path                 path
 * @param[in]  mode                 file mode
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
stdret_t appfs_chmod(void *fs_handle, const char *path, int mode)
{
        UNUSED_ARG(fs_handle);
        UNUSED_ARG(path);
        UNUSED_ARG(mode);

        return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief Function change file owner and group
 *
 * @param[in] *fs_handle            FS handle
 * @param[in] *path                 path
 * @param[in]  owner                file owner
 * @param[in]  group                file group
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
stdret_t appfs_chown(void *fs_handle, const char *path, int owner, int group)
{
        UNUSED_ARG(fs_handle);
        UNUSED_ARG(path);
        UNUSED_ARG(owner);
        UNUSED_ARG(group);

        return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief Function returns file/dir status
 *
 * @param[in]  *fs_handle            FS handle
 * @param[in]  *path                 file/dir path
 * @param[out] *stat                 pointer to stat structure
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
stdret_t appfs_stat(void *fs_handle, const char *path, struct vfs_stat *stat)
{
        UNUSED_ARG(fs_handle);
        UNUSED_ARG(path);

        STOP_IF(!stat);

        stat->st_dev   = 0;
        stat->st_gid   = 0;
        stat->st_mode  = OWNER_MODE(MODE_R) | GROUP_MODE(MODE_R) | OTHER_MODE(MODE_R);
        stat->st_mtime = 0;
        stat->st_size  = 0;
        stat->st_uid   = 0;

        return STD_RET_OK;
}

//==============================================================================
/**
 * @brief Function returns FS status
 *
 * @param[in]  *fs_handle           FS handle
 * @param[out] *statfs              pointer to status structure
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
stdret_t appfs_statfs(void *fs_handle, struct vfs_statfs *statfs)
{
        UNUSED_ARG(fs_handle);

        STOP_IF(!statfs);

        statfs->f_bfree  = 0;
        statfs->f_blocks = 0;
        statfs->f_ffree  = 0;
        statfs->f_files  = 0;
        statfs->f_type   = 1;
        statfs->fsname   = "appfs";

        return STD_RET_OK;
}

//==============================================================================
/**
 * @brief Read item from opened directory
 *
 * @param[in]  *fs_handle      FS handle
 * @param[out] *dir            directory object
 *
 * @return directory entry
 */
//==============================================================================
static dirent_t appfs_readrootdir(void *fs_handle, dir_t *dir)
{
        UNUSED_ARG(fs_handle);

        STOP_IF(!dir);

        dirent_t dirent;
        dirent.name = NULL;
        dirent.size = 0;

        if (dir->f_seek < (size_t)regprg_get_program_count()) {
                dirent.filetype = FILE_TYPE_REGULAR;
                dirent.name     = (char*)regprg_get_pointer_to_program_list()[dir->f_seek].program_name;
                dirent.size     = 0;
                dir->f_seek++;
        }

        return dirent;
}

#ifdef __cplusplus
}
#endif

/*==============================================================================
  End of file
==============================================================================*/
