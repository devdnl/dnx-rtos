/*==============================================================================
File    romfs.c

Author  Daniel Zorychta

Brief   ROM File System

        Copyright (C) 2018 Daniel Zorychta <daniel.zorychta@gmail.com>

        This program is free software; you can redistribute it and/or modify
        it under the terms of the GNU General Public License as published by
        the Free Software Foundation and modified by the dnx RTOS exception.

        NOTE: The modification  to the GPL is  included to allow you to
              distribute a combined work that includes dnx RTOS without
              being obliged to provide the source  code for proprietary
              components outside of the dnx RTOS.

        The dnx RTOS  is  distributed  in the hope  that  it will be useful,
        but WITHOUT  ANY  WARRANTY;  without  even  the implied  warranty of
        MERCHANTABILITY  or  FITNESS  FOR  A  PARTICULAR  PURPOSE.  See  the
        GNU General Public License for more details.

        Full license text is available on the following file: doc/license.txt.

==============================================================================*/

/*==============================================================================
  Include files
==============================================================================*/
#include "fs/fs.h"
#include "romfs_types.h"

/*==============================================================================
  Local macros
==============================================================================*/

/*==============================================================================
  Local object types
==============================================================================*/
typedef struct {
        romfs_dir_t *root;
} romfs_t;

/*==============================================================================
  Local function prototypes
==============================================================================*/

/*==============================================================================
  Local objects
==============================================================================*/

/*==============================================================================
  Exported objects
==============================================================================*/
extern const romfs_dir_t dir_root;

/*==============================================================================
  External objects
==============================================================================*/

/*==============================================================================
  Function definitions
==============================================================================*/

//==============================================================================
/**
 * @brief Initialize file system.
 *
 * @param[out]          **fs_handle             file system allocated memory
 * @param[in ]           *src_path              file source path
 * @param[in ]           *opts                  file system options (can be NULL)
 *
 * @return One of errno value (errno.h).
 */
//==============================================================================
API_FS_INIT(romfs, void **fs_handle, const char *src_path, const char *opts)
{
        int err = sys_zalloc(sizeof(romfs_t), fs_handle);
        if (!err) {
                romfs_t *fs = *fs_handle;
                fs->root = &dir_root;
        }

        return err;
}

//==============================================================================
/**
 * @brief Release file system.
 *
 * @param[in ]          *fs_handle              file system allocated memory
 *
 * @return One of errno value (errno.h).
 */
//==============================================================================
API_FS_RELEASE(romfs, void *fs_handle)
{
        romfs_t *hdl = fs_handle;

        // ...

        sys_free(fs_handle);

        return ESUCC;
}

//==============================================================================
/**
 * @brief Open file.
 *
 * @param[in ]          *fs_handle              file system allocated memory
 * @param[out]          *fhdl                   file extra data
 * @param[out]          *fpos                   file position
 * @param[in]           *path                   file path
 * @param[in]            flags                  file open flags
 *
 * @return One of errno value (errno.h).
 */
//==============================================================================
API_FS_OPEN(romfs, void *fs_handle, void **fhdl, fpos_t *fpos, const char *path, u32_t flags)
{
        romfs_t *hdl = fs_handle;

        int err = ESUCC;

        return err;
}

//==============================================================================
/**
 * @brief Close file.
 *
 * @param[in ]          *fs_handle              file system allocated memory
 * @param[in ]          *fhdl                   file handle
 * @param[in ]           force                  force close
 *
 * @return One of errno value (errno.h).
 */
//==============================================================================
API_FS_CLOSE(romfs, void *fs_handle, void *fhdl, bool force)
{
        romfs_t *hdl = fs_handle;

        int err = ESUCC;

        return err;
}

//==============================================================================
/**
 * @brief Write data to the file.
 *
 * @param[in ]          *fs_handle              file system allocated memory
 * @param[in ]          *fhdl                   file handle
 * @param[in ]          *src                    data source
 * @param[in ]           count                  number of bytes to write
 * @param[in ]          *fpos                   position in file
 * @param[out]          *wrcnt                  number of written bytes
 * @param[in ]           fattr                  file attributes
 *
 * @return One of errno value (errno.h).
 */
//==============================================================================
API_FS_WRITE(romfs,
             void            *fs_handle,
             void            *fhdl,
             const u8_t      *src,
             size_t           count,
             fpos_t          *fpos,
             size_t          *wrcnt,
             struct vfs_fattr fattr)
{
        romfs_t *hdl = fs_handle;

        int err = ESUCC;

        return err;
}

//==============================================================================
/**
 * @brief Read data from file.
 *
 * @param[in ]          *fs_handle              file system allocated memory
 * @param[in ]          *fhdl                   file handle
 * @param[out]          *dst                    data destination
 * @param[in ]           count                  number of bytes to read
 * @param[in ]          *fpos                   position in file
 * @param[out]          *rdcnt                  number of read bytes
 * @param[in ]           fattr                  file attributes
 *
 * @return One of errno value (errno.h).
 */
//==============================================================================
API_FS_READ(romfs,
            void            *fs_handle,
            void            *fhdl,
            u8_t            *dst,
            size_t           count,
            fpos_t          *fpos,
            size_t          *rdcnt,
            struct vfs_fattr fattr)
{
        romfs_t *hdl = fs_handle;

        int err = ESUCC;

        return err;
}

//==============================================================================
/**
 * @brief IO operations on files.
 *
 * @param[in ]          *fs_handle              file system allocated memory
 * @param[in ]          *fhdl                   file handle
 * @param[in ]           request                request
 * @param[in ][out]     *arg                    request's argument
 *
 * @return One of errno value (errno.h).
 */
//==============================================================================
API_FS_IOCTL(romfs, void *fs_handle, void *fhdl, int request, void *arg)
{
        romfs_t *hdl = fs_handle;

        int err = ESUCC;

        return err;
}

//==============================================================================
/**
 * @brief Flush file data.
 *
 * @param[in ]          *fs_handle              file system allocated memory
 * @param[in ]          *fhdl                   file handle
 *
 * @return One of errno value (errno.h).
 */
//==============================================================================
API_FS_FLUSH(romfs, void *fs_handle, void *fhdl)
{
        romfs_t *hdl = fs_handle;

        int err = ESUCC;

        return err;
}

//==============================================================================
/**
 * @brief Return file status.
 *
 * @param[in ]          *fs_handle              file system allocated memory
 * @param[in ]          *fhdl                   file handle
 * @param[out]          *stat                   file status
 *
 * @return One of errno value (errno.h).
 */
//==============================================================================
API_FS_FSTAT(romfs, void *fs_handle, void *fhdl, struct stat *stat)
{
        romfs_t *hdl = fs_handle;

        int err = ESUCC;

        return err;
}

//==============================================================================
/**
 * @brief Return file/dir status.
 *
 * @param[in ]          *fs_handle              file system allocated memory
 * @param[in ]          *path                   file path
 * @param[out]          *stat                   file status
 *
 * @return One of errno value (errno.h).
 */
//==============================================================================
API_FS_STAT(romfs, void *fs_handle, const char *path, struct stat *stat)
{
        romfs_t *hdl = fs_handle;

        int err = ESUCC;

        return err;
}

//==============================================================================
/**
 * @brief Return file system status.
 *
 * @param[in ]          *fs_handle              file system allocated memory
 * @param[out]          *statfs                 file system status
 *
 * @return One of errno value (errno.h).
 */
//==============================================================================
API_FS_STATFS(romfs, void *fs_handle, struct statfs *statfs)
{
        romfs_t *hdl = fs_handle;

        statfs->f_bsize  = 0;
        statfs->f_blocks = 0;
        statfs->f_bfree  = 0;
        statfs->f_ffree  = 0;
        statfs->f_files  = 0;
        statfs->f_type   = SYS_FS_TYPE__SOLID;
        statfs->f_fsname = "romfs";

        return ESUCC;
}

//==============================================================================
/**
 * @brief Create directory.
 *
 * @param[in ]          *fs_handle              file system allocated memory
 * @param[in ]          *path                   name of created directory
 * @param[in ]           mode                   dir mode
 *
 * @return One of errno value (errno.h).
 */
//==============================================================================
API_FS_MKDIR(romfs, void *fs_handle, const char *path, mode_t mode)
{
        romfs_t *hdl = fs_handle;

        int err = ESUCC;

        return err;
}

//==============================================================================
/**
 * @brief Create pipe.
 *
 * @param[in ]          *fs_handle              file system allocated memory
 * @param[in ]          *path                   name of created pipe
 * @param[in ]           mode                   pipe mode
 *
 * @return One of errno value (errno.h).
 */
//==============================================================================
API_FS_MKFIFO(romfs, void *fs_handle, const char *path, mode_t mode)
{
        romfs_t *hdl = fs_handle;

        int err = ESUCC;

        return err;
}

//==============================================================================
/**
 * @brief Create node for driver file.
 *
 * @param[in ]          *fs_handle              file system allocated memory
 * @param[in ]          *path                   name of created node
 * @param[in ]           dev                    driver number
 *
 * @return One of errno value (errno.h).
 */
//==============================================================================
API_FS_MKNOD(romfs, void *fs_handle, const char *path, const dev_t dev)
{
        romfs_t *hdl = fs_handle;

        int err = ESUCC;

        return err;
}

//==============================================================================
/**
 * @brief Open directory.
 *
 * @param[in ]          *fs_handle              file system allocated memory
 * @param[in ]          *path                   name of opened directory
 * @param[in ]          *dir                    directory object
 *
 * @return One of errno value (errno.h).
 */
//==============================================================================
API_FS_OPENDIR(romfs, void *fs_handle, const char *path, DIR *dir)
{
        romfs_t *hdl = fs_handle;

        int err = ESUCC;

        return err;
}

//==============================================================================
/**
 * @brief Close directory.
 *
 * @param[in ]          *fs_handle              file system allocated memory
 * @param[in ]          *dir                    directory object
 *
 * @return One of errno value (errno.h).
 */
//==============================================================================
API_FS_CLOSEDIR(romfs, void *fs_handle, DIR *dir)
{
        romfs_t *hdl = fs_handle;

        int err = ESUCC;

        return err;
}

//==============================================================================
/**
 * @brief Read directory.
 *
 * @param[in ]          *fs_handle              file system allocated memory
 * @param[in,out]       *dir                    directory object
 *
 * @return One of errno value (errno.h).
 */
//==============================================================================
API_FS_READDIR(romfs, void *fs_handle, DIR *dir)
{
        romfs_t *hdl = fs_handle;

        int err = ESUCC;

        return err;
}

//==============================================================================
/**
 * @brief Remove file/directory.
 *
 * @param[in ]          *fs_handle              file system allocated memory
 * @param[in ]          *path                   name of removed file/directory
 *
 * @return One of errno value (errno.h).
 */
//==============================================================================
API_FS_REMOVE(romfs, void *fs_handle, const char *path)
{
        romfs_t *hdl = fs_handle;

        int err = ESUCC;

        return err;
}

//==============================================================================
/**
 * @brief Rename file/directory.
 *
 * @param[in ]          *fs_handle              file system allocated memory
 * @param[in ]          *old_name               old object name
 * @param[in ]          *new_name               new object name
 *
 * @return One of errno value (errno.h).
 */
//==============================================================================
API_FS_RENAME(romfs, void *fs_handle, const char *old_name, const char *new_name)
{
        romfs_t *hdl = fs_handle;

        int err = ESUCC;

        return err;
}

//==============================================================================
/**
 * @brief Change file's mode.
 *
 * @param[in ]          *fs_handle              file system allocated memory
 * @param[in ]          *path                   file path
 * @param[in ]           mode                   new file mode
 *
 * @return One of errno value (errno.h).
 */
//==============================================================================
API_FS_CHMOD(romfs, void *fs_handle, const char *path, mode_t mode)
{
        romfs_t *hdl = fs_handle;

        int err = ESUCC;

        return err;
}

//==============================================================================
/**
 * @brief Change file's owner and group.
 *
 * @param[in ]          *fs_handle              file system allocated memory
 * @param[in ]          *path                   file path
 * @param[in ]           owner                  new file owner
 * @param[in ]           group                  new file group
 *
 * @return One of errno value (errno.h).
 */
//==============================================================================
API_FS_CHOWN(romfs, void *fs_handle, const char *path, uid_t owner, gid_t group)
{
        romfs_t *hdl = fs_handle;

        int err = ESUCC;

        return err;
}

//==============================================================================
/**
 * @brief Synchronize all buffers to a medium.
 *
 * @param[in ]          *fs_handle              file system allocated memory
 *
 * @return One of errno value (errno.h).
 */
//==============================================================================
API_FS_SYNC(romfs, void *fs_handle)
{
        romfs_t *hdl = fs_handle;

        int err = ESUCC;

        return err;
}

/*==============================================================================
  End of file
==============================================================================*/
