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
        uint       openfiles; // TODO
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
//                                ext4_cache_write_back(hdl->fsctx, true); TEST
                                *fs_handle = hdl;
                                return STD_RET_OK;
                        } else {
                                errno = ENOMEM;
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

//        ext4_cache_write_back(hdl->fsctx, false); TEST
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
        UNUSED_ARG(fd);

        ext2fs_t *hdl = fs_handle;

        ext4_file *file = malloc(sizeof(ext4_file));
        if (file) {
                int r = ext4_fopen(hdl->fsctx, file, path, flags);
                if (r == EOK) {
                        *fpos  = ext4_ftell(hdl->fsctx, file);
                        *extra = file;
                        hdl->openfiles++;
                        return STD_RET_OK;
                } else {
                        errno = r;
                }

                free(file);
        }

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
        UNUSED_ARG(fd);
        UNUSED_ARG(force);

        ext2fs_t *hdl = fs_handle;

        int r = ext4_fclose(hdl->fsctx, extra);
        if (r == EOK) {
                free(extra);
                return STD_RET_OK;
        } else {
                errno = r;
        }

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
        UNUSED_ARG(fd);
        UNUSED_ARG(fattr);

        ext2fs_t *hdl = fs_handle;

        int r = ext4_fseek(hdl->fsctx, extra, *fpos, SEEK_SET);
        if (r != EOK) {
                errno = r;
                return -1;
        }

        u32_t wrcnt = 0;
        r = ext4_fwrite(hdl->fsctx, extra, src, count, &wrcnt);
        if (r == EOK) {
                return wrcnt;
        } else {
                errno = r;
                return -1;
        }
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
        UNUSED_ARG(fd);
        UNUSED_ARG(fattr);

        ext2fs_t *hdl = fs_handle;

        int r = ext4_fseek(hdl->fsctx, extra, *fpos, SEEK_SET);
        if (r != EOK) {
                errno = r;
                return -1;
        }

        u32_t rdcnt = 0;
        r = ext4_fread(hdl->fsctx, extra, dst, count, &rdcnt);
        if (r == EOK) {
                return rdcnt;
        } else {
                errno = r;
                return -1;
        }
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

        errno = EPERM;
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

        return STD_RET_OK;
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
        UNUSED_ARG(fd);

        ext2fs_t *hdl = fs_handle;

        struct ext4_filestat filestat; // FIXME allocate to use less stack
        int r = ext4_fstat(hdl->fsctx, extra, &filestat);
        if (r == EOK) {

                stat->st_dev   = filestat.st_dev;
                stat->st_gid   = filestat.st_gid;
                stat->st_uid   = filestat.st_uid;
                stat->st_mode  = filestat.st_mode;
                stat->st_mtime = filestat.st_mtime;
                stat->st_size  = filestat.st_size;

                return STD_RET_OK;
        } else {
                errno = r;
                return STD_RET_ERROR;
        }
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
        ext2fs_t *hdl = fs_handle;

        int r = ext4_dir_mk(hdl->fsctx, path);
        if (r == EOK) {
                ext4_chmod(hdl->fsctx, path, mode);
                return STD_RET_OK;
        } else {
                errno = r;
                return STD_RET_ERROR;
        }
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

        errno = EPERM;
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

        errno = EPERM;
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
                int r = ext4_dir_open(hdl->fsctx, ext4dir, path);
                if (r == EOK) {
                        dir->f_handle   = hdl;
                        dir->f_closedir = closedir;
                        dir->f_readdir  = readdir;
                        dir->f_dd       = ext4dir;
                        dir->f_seek     = 0;
                        dir->f_items    = 0;
                        return STD_RET_OK;
                } else {
                        errno = r;
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

        int r = ext4_dir_close(hdl->fsctx, dir->f_dd);
        if (r == EOK) {
                free(dir->f_dd);
                return STD_RET_OK;
        } else {
                errno = r;
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
                dir->dirent.size     = ((ext4_dir*)(dir->f_dd))->f.fsize; // FIXME

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
        ext2fs_t *hdl = fs_handle;

        int r = ext4_fremove(hdl->fsctx, path);
        if (r == EOK) {
                return STD_RET_OK;
        }

        r = ext4_dir_rm(hdl->fsctx, path);
        if (r == EOK) {
                return STD_RET_OK;
        }

        errno = r;
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
        ext2fs_t *hdl = fs_handle;

        // TODO ?
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
        ext2fs_t *hdl = fs_handle;

        int r = ext4_chmod(hdl->fsctx, path, mode);
        if (r == EOK) {
                return STD_RET_OK;
        } else {
                errno = r;
                return STD_RET_ERROR;
        }
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
        ext2fs_t *hdl = fs_handle;

        int r = ext4_chown(hdl->fsctx, path, owner, group);
        if (r == EOK) {
                return STD_RET_OK;
        } else {
                errno = r;
                return STD_RET_ERROR;
        }
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
        ext2fs_t *hdl = fs_handle;

        struct ext4_filestat filestat; // FIXME allocate to use less stack
        int r = ext4_stat(hdl->fsctx, path, &filestat);
        if (r == EOK) {

                stat->st_dev   = filestat.st_dev;
                stat->st_gid   = filestat.st_gid;
                stat->st_uid   = filestat.st_uid;
                stat->st_mode  = filestat.st_mode;
                stat->st_mtime = filestat.st_mtime;
                stat->st_size  = filestat.st_size;

                return STD_RET_OK;
        } else {
                errno = r;
                return STD_RET_ERROR;
        }
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
        ext2fs_t *hdl = fs_handle;

        struct ext4_fs_stats stat; // FIXME allocate to use less stack
        int r = ext4_statfs(hdl->fsctx, &stat);
        if (r == EOK) {

                statfs->f_bfree  = stat.free_blocks_count;
                statfs->f_blocks = stat.blocks_count;
                statfs->f_bsize  = stat.block_size;
                statfs->f_ffree  = stat.free_inodes_count;
                statfs->f_files  = stat.inodes_count;
                statfs->f_type   = 2;
                statfs->f_fsname = "ext2fs";

                return STD_RET_OK;
        } else {
                errno = r;
                return STD_RET_ERROR;
        }
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
//        ext2fs_t *hdl = fs_handle;
//
//        ext4_cache_write_back(hdl->fsctx, false); TEST
//        ext4_cache_write_back(hdl->fsctx, true);
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
