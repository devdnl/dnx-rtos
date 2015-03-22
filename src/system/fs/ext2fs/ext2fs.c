/*=========================================================================*//**
@file    ext2fs.c

@author  Daniel Zorychta

@brief   EXT2 File System by using lwext4 library (kostka.grzegorz@gmail.com)

@note    Copyright (C) 2015 Daniel Zorychta <daniel.zorychta@gmail.com>

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
#include "core/fs.h"
#include "lwext4/ext4.h"

/*==============================================================================
  Local macros
==============================================================================*/
#define BLOCK_SIZE      512

/*==============================================================================
  Local object types
==============================================================================*/
typedef struct {
        ext4_fs_t *fsctx;
        mutex_t   *mtx;
        FILE      *srcfile;
} ext2fs_t;

/*==============================================================================
  Local function prototypes
==============================================================================*/
static stdret_t closedir(void *fs_handle, DIR *dir);
static dirent_t *readdir (void *fs_handle, DIR *dir);
static void ext4_lock(void *obj);
static void ext4_unlock(void *obj);
static int ext4_bread(struct ext4_blockdev *bdev, void *buf, uint64_t blk_id, uint32_t blk_cnt);
static int ext4_bwrite(struct ext4_blockdev *bdev, const void *buf, uint64_t blk_id, uint32_t blk_cnt);

/*==============================================================================
  Local objects
==============================================================================*/
static const struct ext4_os_if osif = {
        .lock   = ext4_lock,
        .unlock = ext4_unlock,
        .bread  = ext4_bread,
        .bwrite = ext4_bwrite
};

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
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
API_FS_INIT(ext2fs, void **fs_handle, const char *src_path)
{
        // open file system source file
        FILE *srcfile = _sys_fopen(src_path, "r+");
        if (!srcfile) {
                return STD_RET_ERROR;
        }

        // read number of file blocks
        struct stat stat;
        if (_sys_fstat(srcfile, &stat) != 0) {
                _sys_fclose(srcfile);
                return STD_RET_ERROR;
        }

        u64_t block_count = stat.st_size / BLOCK_SIZE;

        // create FS context
        ext2fs_t *hdl = malloc(sizeof(ext2fs_t));
        if (hdl) {
                hdl->mtx = _sys_mutex_new(MUTEX_RECURSIVE);
                if (hdl->mtx) {
                        hdl->srcfile = srcfile;
                        hdl->fsctx   = ext4_mount(&osif, hdl, BLOCK_SIZE, block_count);
                        if (hdl->fsctx) {
                                *fs_handle = hdl;
                                return STD_RET_OK;
                        }

                        _sys_mutex_delete(hdl->mtx);
                }

                free(hdl);
        }

        _sys_fclose(srcfile);

        return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief Release file system
 *
 * @param[in ]          *fs_handle              file system allocated memory
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
API_FS_RELEASE(ext2fs, void *fs_handle)
{
        ext2fs_t *hdl = fs_handle;

        ext4_umount(hdl->fsctx);
        _sys_mutex_delete(hdl->mtx);
        _sys_fclose(hdl->srcfile);
        free(hdl);

        return STD_RET_OK;
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
 * @param[in]            flags                  file open flags
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
API_FS_OPEN(ext2fs, void *fs_handle, void **extra, fd_t *fd, fpos_t *fpos, const char *path, u32_t flags)
{
        UNUSED_ARG(fs_handle);
        UNUSED_ARG(extra);
        UNUSED_ARG(fd);
        UNUSED_ARG(fpos);
        UNUSED_ARG(path);
        UNUSED_ARG(flags);
        return STD_RET_ERROR;
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
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
API_FS_CLOSE(ext2fs, void *fs_handle, void *extra, fd_t fd, bool force)
{
        UNUSED_ARG(fs_handle);
        UNUSED_ARG(extra);
        UNUSED_ARG(fd);
        UNUSED_ARG(force);
        return STD_RET_ERROR;
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
 * @param[in ]           fattr                  file attributes
 *
 * @return number of written bytes, -1 if error
 */
//==============================================================================
API_FS_WRITE(ext2fs, void *fs_handle,void *extra, fd_t fd, const u8_t *src, size_t count, fpos_t *fpos, struct vfs_fattr fattr)
{
        UNUSED_ARG(fs_handle);
        UNUSED_ARG(extra);
        UNUSED_ARG(fd);
        UNUSED_ARG(fpos);
        UNUSED_ARG(fattr);
        UNUSED_ARG(count);
        UNUSED_ARG(src);
        return 0;
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
 * @param[in ]           fattr                  file attributes
 *
 * @return number of read bytes, -1 if error
 */
//==============================================================================
API_FS_READ(ext2fs, void *fs_handle, void *extra, fd_t fd, u8_t *dst, size_t count, fpos_t *fpos, struct vfs_fattr fattr)
{
        UNUSED_ARG(fs_handle);
        UNUSED_ARG(extra);
        UNUSED_ARG(fd);
        UNUSED_ARG(fpos);
        UNUSED_ARG(fattr);
        UNUSED_ARG(count);
        UNUSED_ARG(dst);
        return 0;
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
 * @return Value depends on driver implementation (int)
 */
//==============================================================================
API_FS_IOCTL(ext2fs, void *fs_handle, void *extra, fd_t fd, int request, void *arg)
{
        UNUSED_ARG(fs_handle);
        UNUSED_ARG(extra);
        UNUSED_ARG(fd);
        UNUSED_ARG(request);
        UNUSED_ARG(arg);
        return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief Flush file data
 *
 * @param[in ]          *fs_handle              file system allocated memory
 * @param[in ]          *extra                  file extra data
 * @param[in ]           fd                     file descriptor
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
API_FS_FLUSH(ext2fs, void *fs_handle, void *extra, fd_t fd)
{
        UNUSED_ARG(fs_handle);
        UNUSED_ARG(extra);
        UNUSED_ARG(fd);
        return STD_RET_ERROR;
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
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
API_FS_FSTAT(ext2fs, void *fs_handle, void *extra, fd_t fd, struct stat *stat)
{
        UNUSED_ARG(fs_handle);
        UNUSED_ARG(extra);
        UNUSED_ARG(fd);
        UNUSED_ARG(stat);
        return STD_RET_OK;
}

//==============================================================================
/**
 * @brief Create directory
 *
 * @param[in ]          *fs_handle              file system allocated memory
 * @param[in ]          *path                   name of created directory
 * @param[in ]           mode                   dir mode
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
API_FS_MKDIR(ext2fs, void *fs_handle, const char *path, mode_t mode)
{
        UNUSED_ARG(fs_handle);
        UNUSED_ARG(path);
        UNUSED_ARG(mode);
        return STD_RET_ERROR;
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
API_FS_MKFIFO(ext2fs, void *fs_handle, const char *path, mode_t mode)
{
        UNUSED_ARG(fs_handle);
        UNUSED_ARG(path);
        UNUSED_ARG(mode);
        return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief Create node for driver file
 *
 * @param[in ]          *fs_handle              file system allocated memory
 * @param[in ]          *path                   name of created node
 * @param[in ]           dev                    driver id
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
API_FS_MKNOD(ext2fs, void *fs_handle, const char *path, const dev_t dev)
{
        UNUSED_ARG(fs_handle);
        UNUSED_ARG(path);
        UNUSED_ARG(dev);
        return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief Open directory
 *
 * @param[in ]          *fs_handle              file system allocated memory
 * @param[in ]          *path                   name of opened directory
 * @param[in ]          *dir                    directory object
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
API_FS_OPENDIR(ext2fs, void *fs_handle, const char *path, DIR *dir)
{
        ext2fs_t *hdl = fs_handle;

        ext4_dir *ext4dir = malloc(sizeof(ext4_dir));
        if (ext4dir) {
                if (ext4_dir_open(hdl->fsctx, ext4dir, path) == EOK) {
                        dir->f_handle   = hdl;
                        dir->f_closedir = closedir;
                        dir->f_readdir  = readdir;
                        dir->f_dd       = ext4dir;
                        dir->f_seek     = 0;
                        dir->f_items    = 0;
                        return STD_RET_OK;
                }

                free(ext4dir);
        }

        return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief Close directory
 *
 * @param[in ]          *fs_handle              file system allocated memory
 * @param[in ]          *dir                    directory object
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
static stdret_t closedir(void *fs_handle, DIR *dir)
{
        ext2fs_t *hdl = fs_handle;

        if (ext4_dir_close(hdl->fsctx, dir->f_dd) == EOK) {
                free(dir->f_dd);
                return STD_RET_OK;
        } else {
                return STD_RET_ERROR;
        }
}

//==============================================================================
/**
 * @brief Read directory
 *
 * @param[in ]          *fs_handle              file system allocated memory
 * @param[in ]          *dir                    directory object
 *
 * @return On success pointer to directory entry description object. On error or
 *         when there is not more objects return NULL. When no object then
 *         errno is set to 0, otherwise to specific value.
 */
//==============================================================================
static dirent_t *readdir(void *fs_handle, DIR *dir)
{
        static const uint8_t vfsft[] = {
                [EXT4_DIRENTRY_UNKNOWN ] = FILE_TYPE_UNKNOWN,
                [EXT4_DIRENTRY_REG_FILE] = FILE_TYPE_REGULAR,
                [EXT4_DIRENTRY_DIR     ] = FILE_TYPE_DIR,
                [EXT4_DIRENTRY_CHRDEV  ] = FILE_TYPE_DRV,
                [EXT4_DIRENTRY_BLKDEV  ] = FILE_TYPE_DRV,
                [EXT4_DIRENTRY_FIFO    ] = FILE_TYPE_PIPE,
                [EXT4_DIRENTRY_SOCK    ] = FILE_TYPE_PIPE,
                [EXT4_DIRENTRY_SYMLINK ] = FILE_TYPE_LINK,
        };

        ext2fs_t *hdl = fs_handle;

        ext4_direntry *ext4_dirent = ext4_dir_entry_get(hdl->fsctx, dir->f_dd, dir->f_seek++);
        if (ext4_dirent) {
                int nlen = ext4_dirent->name_length == 255 ? 254 : ext4_dirent->name_length;
                ext4_dirent->name[nlen] = '\0';

                dir->dirent.dev      = 0;
                dir->dirent.filetype = vfsft[ext4_dirent->inode_type];
                dir->dirent.name     = static_cast(char*, ext4_dirent->name);
                dir->dirent.size     = 0; // FIXME

                return &dir->dirent;
        } else {
                return NULL;
        }
}

//==============================================================================
/**
 * @brief Remove file/directory
 *
 * @param[in ]          *fs_handle              file system allocated memory
 * @param[in ]          *path                   name of removed file/directory
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
API_FS_REMOVE(ext2fs, void *fs_handle, const char *path)
{
        UNUSED_ARG(fs_handle);
        UNUSED_ARG(path);
        return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief Rename file/directory
 *
 * @param[in ]          *fs_handle              file system allocated memory
 * @param[in ]          *old_name               old object name
 * @param[in ]          *new_name               new object name
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
API_FS_RENAME(ext2fs, void *fs_handle, const char *old_name, const char *new_name)
{
        UNUSED_ARG(fs_handle);
        UNUSED_ARG(old_name);
        UNUSED_ARG(new_name);
        return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief Change file's mode
 *
 * @param[in ]          *fs_handle              file system allocated memory
 * @param[in ]          *path                   file path
 * @param[in ]           mode                   new file mode
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
API_FS_CHMOD(ext2fs, void *fs_handle, const char *path, mode_t mode)
{
        UNUSED_ARG(fs_handle);
        UNUSED_ARG(path);
        UNUSED_ARG(mode);
        return STD_RET_ERROR;
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
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
API_FS_CHOWN(ext2fs, void *fs_handle, const char *path, uid_t owner, gid_t group)
{
        UNUSED_ARG(fs_handle);
        UNUSED_ARG(path);
        UNUSED_ARG(owner);
        UNUSED_ARG(group);
        return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief Return file/dir status
 *
 * @param[in ]          *fs_handle              file system allocated memory
 * @param[in ]          *path                   file path
 * @param[out]          *stat                   file status
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
API_FS_STAT(ext2fs, void *fs_handle, const char *path, struct stat *stat)
{
        UNUSED_ARG(fs_handle);
        UNUSED_ARG(path);

        stat->st_dev   = 0;
        stat->st_gid   = 0;
        stat->st_mode  = S_IRUSR | S_IRGRO | S_IROTH;
        stat->st_mtime = 0;
        stat->st_size  = 0;
        stat->st_uid   = 0;

        return STD_RET_OK;
}

//==============================================================================
/**
 * @brief Return file system status
 *
 * @param[in ]          *fs_handle              file system allocated memory
 * @param[out]          *statfs                 file system status
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
API_FS_STATFS(ext2fs, void *fs_handle, struct statfs *statfs)
{
        UNUSED_ARG(fs_handle);
        UNUSED_ARG(statfs);

        statfs->f_bfree  = 0;
        statfs->f_blocks = 0;
        statfs->f_ffree  = 0;
        statfs->f_files  = 0;
        statfs->f_type   = 1;
        statfs->f_fsname = "ext4fs";

        return STD_RET_OK;
}

//==============================================================================
/**
 * @brief Synchronize all buffers to a medium
 *
 * @param[in ]          *fs_handle              file system allocated memory
 *
 * @return None
 */
//==============================================================================
API_FS_SYNC(ext2fs, void *fs_handle)
{
        UNUSED_ARG(fs_handle);
}

//==============================================================================
/**
 * @brief  ?
 * @param  ?
 * @return ?
 */
//==============================================================================
static void ext4_lock(void *ctx)
{
        ext2fs_t *hdl = ctx;
        _sys_mutex_lock(hdl->mtx, MAX_DELAY_MS);
}

//==============================================================================
/**
 * @brief  ?
 * @param  ?
 * @return ?
 */
//==============================================================================
static void ext4_unlock(void *ctx)
{
        ext2fs_t *hdl = ctx;
        _sys_mutex_unlock(hdl->mtx);
}

//==============================================================================
/**
 * @brief  ?
 * @param  ?
 * @return ?
 */
//==============================================================================
static int ext4_bread(struct ext4_blockdev *bdev, void *buf, uint64_t blk_id, uint32_t blk_cnt)
{
        ext2fs_t *hdl = bdev->usr_ctx;

        _sys_fseek(hdl->srcfile, blk_id * static_cast(u64_t, BLOCK_SIZE), SEEK_SET);
        size_t n = _sys_fread(buf, BLOCK_SIZE, blk_cnt, hdl->srcfile);
        if (n == blk_cnt)
                return EOK;
        else
                return errno;
}

//==============================================================================
/**
 * @brief  ?
 * @param  ?
 * @return ?
 */
//==============================================================================
static int ext4_bwrite(struct ext4_blockdev *bdev, const void *buf, uint64_t blk_id, uint32_t blk_cnt)
{
        ext2fs_t *hdl = bdev->usr_ctx;

        _sys_fseek(hdl->srcfile, blk_id * static_cast(u64_t, BLOCK_SIZE), SEEK_SET);
        size_t n = _sys_fwrite(buf, BLOCK_SIZE, blk_cnt, hdl->srcfile);
        if (n == blk_cnt)
                return EOK;
        else
                return errno;
}

/*==============================================================================
  End of file
==============================================================================*/
