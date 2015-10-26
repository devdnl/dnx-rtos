/*=========================================================================*//**
@file    <!fs_name!>.c

@author  <!author!>

@brief   <!fs_description!>

@note    Copyright (C) <!year!> <!author!> <<!email!>>

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

/*==============================================================================
  Include files
==============================================================================*/
#include "fs/fs.h"

/*==============================================================================
  Local macros
==============================================================================*/

/*==============================================================================
  Local object types
==============================================================================*/

/*==============================================================================
  Local function prototypes
==============================================================================*/
static int closedir(void *fs_handle, DIR *dir);
static int readdir (void *fs_handle, DIR *dir, dirent_t **dirent);

/*==============================================================================
  Local objects
==============================================================================*/

/*==============================================================================
  Exported objects
==============================================================================*/

/*==============================================================================
  External objects
==============================================================================*/

/*==============================================================================
  Function definitions
==============================================================================*/

//==============================================================================
/**
 * @brief Initialize file system
 *
 * @param[out]          **fs_handle             file system allocated memory
 * @param[in ]           *src_path              file source path
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
API_FS_INIT(<!fs_name!>, void **fs_handle, const char *src_path)
{
        return EINVAL;
}

//==============================================================================
/**
 * @brief Release file system
 *
 * @param[in ]          *fs_handle              file system allocated memory
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
API_FS_RELEASE(<!fs_name!>, void *fs_handle)
{
        return EINVAL;
}

//==============================================================================
/**
 * @brief Open file
 *
 * @param[in ]          *fs_handle              file system allocated memory
 * @param[out]          *extra                  file extra data
 * @param[out]          *fd                     file descriptor
 * @param[out]          *fpos                   file position
 * @param[in]           *path                   file path
 * @param[in]            flags                  file open flags (O_...)
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
API_FS_OPEN(<!fs_name!>, void *fs_handle, void **extra, fd_t *fd, fpos_t *fpos, const char *path, u32_t flags)
{
        return ENOENT;
}

//==============================================================================
/**
 * @brief Close file
 *
 * @param[in ]          *fs_handle              file system allocated memory
 * @param[in ]          *extra                  file extra data
 * @param[in ]           fd                     file descriptor
 * @param[in ]           force                  force close
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
API_FS_CLOSE(<!fs_name!>, void *fs_handle, void *extra, fd_t fd, bool force)
{
        return EINVAL;
}

//==============================================================================
/**
 * @brief Write data to the file
 *
 * @param[in ]          *fs_handle              file system allocated memory
 * @param[in ]          *extra                  file extra data
 * @param[in ]           fd                     file descriptor
 * @param[in ]          *src                    data source
 * @param[in ]           count                  number of bytes to write
 * @param[in ]          *fpos                   position in file
 * @param[out]          *wrcnt                  number of written bytes
 * @param[in ]           fattr                  file attributes
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
API_FS_WRITE(<!fs_name!>,
             void            *fs_handle,
             void            *extra,
             fd_t             fd,
             const u8_t      *src,
             size_t           count,
             fpos_t          *fpos,
             size_t          *wrcnt,
             struct vfs_fattr fattr)
{
        return EIO;
}

//==============================================================================
/**
 * @brief Read data from file
 *
 * @param[in ]          *fs_handle              file system allocated memory
 * @param[in ]          *extra                  file extra data
 * @param[in ]           fd                     file descriptor
 * @param[out]          *dst                    data destination
 * @param[in ]           count                  number of bytes to read
 * @param[in ]          *fpos                   position in file
 * @param[out]          *rdcnt                  number of read bytes
 * @param[in ]           fattr                  file attributes
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
API_FS_READ(<!fs_name!>,
            void            *fs_handle,
            void            *extra,
            fd_t             fd,
            u8_t            *dst,
            size_t           count,
            fpos_t          *fpos,
            size_t          *rdcnt,
            struct vfs_fattr fattr)
{
        return EIO;
}

//==============================================================================
/**
 * @brief IO operations on files
 *
 * @param[in ]          *fs_handle              file system allocated memory
 * @param[in ]          *extra                  file extra data
 * @param[in ]           fd                     file descriptor
 * @param[in ]           request                request
 * @param[in ][out]     *arg                    request's argument
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
API_FS_IOCTL(<!fs_name!>, void *fs_handle, void *extra, fd_t fd, int request, void *arg)
{
        return ESUCC;
}

//==============================================================================
/**
 * @brief Flush file data
 *
 * @param[in ]          *fs_handle              file system allocated memory
 * @param[in ]          *extra                  file extra data
 * @param[in ]           fd                     file descriptor
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
API_FS_FLUSH(<!fs_name!>, void *fs_handle, void *extra, fd_t fd)
{
        return EINVAL;
}

//==============================================================================
/**
 * @brief Return file status
 *
 * @param[in ]          *fs_handle              file system allocated memory
 * @param[in ]          *extra                  file extra data
 * @param[in ]           fd                     file descriptor
 * @param[out]          *stat                   file status
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
API_FS_FSTAT(<!fs_name!>, void *fs_handle, void *extra, fd_t fd, struct stat *stat)
{
        return EINVAL;
}

//==============================================================================
/**
 * @brief Create directory
 *
 * @param[in ]          *fs_handle              file system allocated memory
 * @param[in ]          *path                   name of created directory
 * @param[in ]           mode                   dir mode
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
API_FS_MKDIR(<!fs_name!>, void *fs_handle, const char *path, mode_t mode)
{
        return ENOENT;
}

//==============================================================================
/**
 * @brief Create pipe
 *
 * @param[in ]          *fs_handle              file system allocated memory
 * @param[in ]          *path                   name of created pipe
 * @param[in ]           mode                   pipe mode
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
API_FS_MKFIFO(<!fs_name!>, void *fs_handle, const char *path, mode_t mode)
{
        return ENOENT;
}

//==============================================================================
/**
 * @brief Create node for driver file
 *
 * @param[in ]          *fs_handle              file system allocated memory
 * @param[in ]          *path                   name of created node
 * @param[in ]           dev                    driver id
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
API_FS_MKNOD(<!fs_name!>, void *fs_handle, const char *path, const dev_t dev)
{
        return ENOENT;
}

//==============================================================================
/**
 * @brief Open directory
 *
 * @param[in ]          *fs_handle              file system allocated memory
 * @param[in ]          *path                   name of opened directory
 * @param[in ]          *dir                    directory object
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
API_FS_OPENDIR(<!fs_name!>, void *fs_handle, const char *path, DIR *dir)
{
        dir->f_closedir = closedir;
        dir->f_readdir  = readdir;
        // ...

        return ENOENT;
}

//==============================================================================
/**
 * @brief Close directory
 *
 * @param[in ]          *fs_handle              file system allocated memory
 * @param[in ]          *dir                    directory object
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
static int closedir(void *fs_handle, DIR *dir)
{
        return EINVAL;
}

//==============================================================================
/**
 * @brief Read directory
 *
 * @param[in ]          *fs_handle              file system allocated memory
 * @param[in ]          *dir                    directory object
 * @param[out]          **dirent                directory entry
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
static int readdir(void *fs_handle, DIR *dir, dirent_t **dirent)
{
        dir->dirent.name = "Example";
        dir->dirent.size = 10;
        dir->dirent.filetype = FILE_TYPE_REGULAR;
        *dirent = &dir->dirent;

        return ESUCC;
}

//==============================================================================
/**
 * @brief Remove file/directory
 *
 * @param[in ]          *fs_handle              file system allocated memory
 * @param[in ]          *path                   name of removed file/directory
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
API_FS_REMOVE(<!fs_name!>, void *fs_handle, const char *path)
{
        return ENOENT;
}

//==============================================================================
/**
 * @brief Rename file/directory
 *
 * @param[in ]          *fs_handle              file system allocated memory
 * @param[in ]          *old_name               old object name
 * @param[in ]          *new_name               new object name
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
API_FS_RENAME(<!fs_name!>, void *fs_handle, const char *old_name, const char *new_name)
{
        return ENOENT;
}

//==============================================================================
/**
 * @brief Change file's mode
 *
 * @param[in ]          *fs_handle              file system allocated memory
 * @param[in ]          *path                   file path
 * @param[in ]           mode                   new file mode
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
API_FS_CHMOD(<!fs_name!>, void *fs_handle, const char *path, mode_t mode)
{
        return ENOENT;
}

//==============================================================================
/**
 * @brief Change file's owner and group
 *
 * @param[in ]          *fs_handle              file system allocated memory
 * @param[in ]          *path                   file path
 * @param[in ]           owner                  new file owner
 * @param[in ]           group                  new file group
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
API_FS_CHOWN(<!fs_name!>, void *fs_handle, const char *path, uid_t owner, gid_t group)
{
        return ENOENT;
}

//==============================================================================
/**
 * @brief Return file/dir status
 *
 * @param[in ]          *fs_handle              file system allocated memory
 * @param[in ]          *path                   file path
 * @param[out]          *stat                   file status
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
API_FS_STAT(<!fs_name!>, void *fs_handle, const char *path, struct stat *stat)
{
        stat->st_dev   = 0;
        stat->st_gid   = 0;
        stat->st_mode  = S_IRUSR | S_IRGRO | S_IROTH;
        stat->st_mtime = 0;
        stat->st_size  = 0;
        stat->st_uid   = 0;

        return ENOENT;
}

//==============================================================================
/**
 * @brief Return file system status
 *
 * @param[in ]          *fs_handle              file system allocated memory
 * @param[out]          *statfs                 file system status
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
API_FS_STATFS(<!fs_name!>, void *fs_handle, struct statfs *statfs)
{
        statfs->f_bfree  = 0;
        statfs->f_blocks = 0;
        statfs->f_ffree  = 0;
        statfs->f_files  = 0;
        statfs->f_type   = 1;
        statfs->f_fsname = "<!fs_name!>";

        return EINVAL;
}

//==============================================================================
/**
 * @brief Synchronize all buffers to a medium
 *
 * @param[in ]          *fs_handle              file system allocated memory
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
API_FS_SYNC(<!fs_name!>, void *fs_handle)
{
        return ESUCC;
}

/*==============================================================================
  End of file
==============================================================================*/
