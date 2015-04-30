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
#include "fs/fs.h"
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
        uint       openfiles;
} ext2fs_t;

/*==============================================================================
  Local function prototypes
==============================================================================*/
static int  closedir(void *fs_handle, DIR *dir);
static int  readdir(void *fs_handle, DIR *dir, dirent_t **dirent);
static void ext4_lock(void *obj);
static void ext4_unlock(void *obj);
static int  ext4_bread(struct ext4_blockdev *bdev, void *buf, uint64_t blk_id, uint32_t blk_cnt);
static int  ext4_bwrite(struct ext4_blockdev *bdev, const void *buf, uint64_t blk_id, uint32_t blk_cnt);
static void increase_openfiles(ext2fs_t *hdl);
static void decrease_openfiles(ext2fs_t *hdl);

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
 * @return One of errno value (errno.h)
 */
//==============================================================================
API_FS_INIT(ext2fs, void **fs_handle, const char *src_path)
{
        int result = _sys_zalloc(sizeof(ext2fs_t), fs_handle);
        if (result == ESUCC) {
                ext2fs_t *hdl = *fs_handle;

                // open file system source file
                result = _sys_fopen(src_path, "r+", &hdl->srcfile);
                if (result != ESUCC)
                        goto finish;

                // read number of file blocks
                struct stat stat;
                result = _sys_fstat(hdl->srcfile, &stat);
                if (result != ESUCC)
                        goto finish;

                u64_t block_count = stat.st_size / BLOCK_SIZE;

                result = _sys_mutex_create(MUTEX_TYPE_RECURSIVE, &hdl->mtx);
                if (result != ESUCC)
                        goto finish;

                result = ext4_mount(&osif, hdl, BLOCK_SIZE, block_count, &hdl->fsctx);
                if (result == ESUCC) {
                        ext4_cache_write_back(hdl->fsctx, true);
                }

                finish:
                if (result != ESUCC) {
                        if (hdl->srcfile)
                                _sys_fclose(hdl->srcfile);

                        if (hdl->mtx)
                                _sys_mutex_destroy(hdl->mtx);

                        _sys_free(fs_handle);
                }

        }

        return result;
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
API_FS_RELEASE(ext2fs, void *fs_handle)
{
        ext2fs_t *hdl = fs_handle;

        if (hdl->openfiles == 0) {
                ext4_cache_write_back(hdl->fsctx, false);
                ext4_umount(hdl->fsctx);
                _sys_mutex_destroy(hdl->mtx);
                _sys_fclose(hdl->srcfile);
                _sys_free(fs_handle);
                return ESUCC;
        } else {
                return EBUSY;
        }
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
 * @return One of errno value (errno.h)
 */
//==============================================================================
API_FS_OPEN(ext2fs, void *fs_handle, void **extra, fd_t *fd, fpos_t *fpos, const char *path, u32_t flags)
{
        UNUSED_ARG1(fd);

        ext2fs_t *hdl = fs_handle;

        ext4_file *file;
        int result = _sys_malloc(sizeof(ext4_file), static_cast(void**, &file));
        if (result == ESUCC) {
                result = ext4_fopen(hdl->fsctx, file, path, flags);
                if (result == ESUCC) {
                        *fpos  = ext4_ftell(hdl->fsctx, file);
                        *extra = file;
                        increase_openfiles(hdl);
                } else {
                        _sys_free(static_cast(void**, &file));
                }
        }

        return result;
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
API_FS_CLOSE(ext2fs, void *fs_handle, void *extra, fd_t fd, bool force)
{
        UNUSED_ARG1(fd);
        UNUSED_ARG1(force);

        ext2fs_t *hdl = fs_handle;

        int status = ext4_fclose(hdl->fsctx, extra);
        if (status == ESUCC) {
                _sys_free(&extra);
                decrease_openfiles(hdl);
        }

        return status;
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
API_FS_WRITE(ext2fs,
             void            *fs_handle,
             void            *extra,
             fd_t             fd,
             const u8_t      *src,
             size_t           count,
             fpos_t          *fpos,
             size_t          *wrcnt,
             struct vfs_fattr fattr)
{
        UNUSED_ARG1(fd);
        UNUSED_ARG1(fattr);

        ext2fs_t *hdl = fs_handle;

        int status = ext4_fseek(hdl->fsctx, extra, *fpos, SEEK_SET);
        if (status == ESUCC) {
                uint32_t wrc = 0;
                status = ext4_fwrite(hdl->fsctx, extra, src, count, &wrc);
                if (status == ESUCC) {
                        *wrcnt = wrc;
                }
        }

        return status;
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
API_FS_READ(ext2fs,
            void            *fs_handle,
            void            *extra,
            fd_t             fd,
            u8_t            *dst,
            size_t           count,
            fpos_t          *fpos,
            size_t          *rdcnt,
            struct vfs_fattr fattr)
{
        UNUSED_ARG1(fd);
        UNUSED_ARG1(fattr);

        ext2fs_t *hdl = fs_handle;

        int status = ext4_fseek(hdl->fsctx, extra, *fpos, SEEK_SET);
        if (status == ESUCC) {
                u32_t rdc = 0;
                status = ext4_fread(hdl->fsctx, extra, dst, count, &rdc);
                if (status == ESUCC) {
                        *rdcnt = rdc;
                }
        }

        return status;
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
        UNUSED_ARG1(fs_handle);
        UNUSED_ARG1(extra);
        UNUSED_ARG1(fd);
        UNUSED_ARG1(request);
        UNUSED_ARG1(arg);

        return ENOTSUP;
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
API_FS_FLUSH(ext2fs, void *fs_handle, void *extra, fd_t fd)
{
        UNUSED_ARG1(fs_handle);
        UNUSED_ARG1(extra);
        UNUSED_ARG1(fd);

        return ESUCC;
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
API_FS_FSTAT(ext2fs, void *fs_handle, void *extra, fd_t fd, struct stat *stat)
{
        UNUSED_ARG1(fd);

        ext2fs_t *hdl = fs_handle;

        struct ext4_filestat filestat;
        int status = ext4_fstat(hdl->fsctx, extra, &filestat);
        if (status == ESUCC) {
                stat->st_dev   = filestat.st_dev;
                stat->st_gid   = filestat.st_gid;
                stat->st_uid   = filestat.st_uid;
                stat->st_mode  = filestat.st_mode;
                stat->st_mtime = filestat.st_mtime;
                stat->st_size  = filestat.st_size;
        }

        return status;
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
API_FS_MKDIR(ext2fs, void *fs_handle, const char *path, mode_t mode)
{
        ext2fs_t *hdl = fs_handle;

        int status = ext4_dir_mk(hdl->fsctx, path);
        if (status == ESUCC) {
                ext4_chmod(hdl->fsctx, path, mode);
        }

        return status;
}

//==============================================================================
/**
 * @brief Create pipe
 *
 * @param[in ]          *fs_handle              file system allocated memory
 * @param[in ]          *path                   name of created pipe
 * @param[in ]           mode                   pipe mode
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
API_FS_MKFIFO(ext2fs, void *fs_handle, const char *path, mode_t mode)
{
        UNUSED_ARG1(fs_handle);
        UNUSED_ARG1(path);
        UNUSED_ARG1(mode);

        return ENOTSUP;
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
API_FS_MKNOD(ext2fs, void *fs_handle, const char *path, const dev_t dev)
{
        UNUSED_ARG1(fs_handle);
        UNUSED_ARG1(path);
        UNUSED_ARG1(dev);

        return ENOTSUP;
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
API_FS_OPENDIR(ext2fs, void *fs_handle, const char *path, DIR *dir)
{
        ext2fs_t *hdl = fs_handle;

        ext4_dir *ext4dir;
        int result = _sys_malloc(sizeof(ext4_dir), static_cast(void**, &ext4dir));
        if (result == ESUCC) {
                result = ext4_dir_open(hdl->fsctx, ext4dir, path);
                if (result == ESUCC) {
                        dir->f_handle   = hdl;
                        dir->f_closedir = closedir;
                        dir->f_readdir  = readdir;
                        dir->f_dd       = ext4dir;
                        dir->f_seek     = 0;
                        dir->f_items    = 0;

                        increase_openfiles(hdl);

                } else {
                        _sys_free(static_cast(void**, ext4dir));
                }
        }

        return result;
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
        ext2fs_t *hdl = fs_handle;

        int result = ext4_dir_close(hdl->fsctx, dir->f_dd);
        if (result == ESUCC) {
                _sys_free(static_cast(void**, dir->f_dd));
                decrease_openfiles(hdl);
        }

        return result;
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
                dir->dirent.size     = ext4_dirent->size;

                *dirent = &dir->dirent;

                return ESUCC;
        } else {
                return ENOENT;
        }
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
API_FS_REMOVE(ext2fs, void *fs_handle, const char *path)
{
        ext2fs_t *hdl = fs_handle;

        // try remove file
        int status = ext4_fremove(hdl->fsctx, path);
        if (status != ESUCC) {
                // try remove dir
                status = ext4_dir_rm(hdl->fsctx, path);
        }

        return status;
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
API_FS_RENAME(ext2fs, void *fs_handle, const char *old_name, const char *new_name)
{

        ext2fs_t *hdl = fs_handle;
        UNUSED_ARG1(hdl);
        UNUSED_ARG1(old_name);
        UNUSED_ARG1(new_name);

        return ENOTSUP;

        // FIXME ex2fs::rename(): this function does not work correctly. Do not use!
//        int r = ext4_rename(hdl->fsctx, old_name, new_name);
//        if (r != EOK) {
//                errno = r;
//                return STD_RET_ERROR;
//        } else {
//                return STD_RET_OK;
//        }
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
API_FS_CHMOD(ext2fs, void *fs_handle, const char *path, mode_t mode)
{
        ext2fs_t *hdl = fs_handle;
        return ext4_chmod(hdl->fsctx, path, mode);
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
API_FS_CHOWN(ext2fs, void *fs_handle, const char *path, uid_t owner, gid_t group)
{
        ext2fs_t *hdl = fs_handle;
        return ext4_chown(hdl->fsctx, path, owner, group);
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
API_FS_STAT(ext2fs, void *fs_handle, const char *path, struct stat *stat)
{
        ext2fs_t *hdl = fs_handle;

        struct ext4_filestat filestat;
        int status = ext4_stat(hdl->fsctx, path, &filestat);
        if (status == ESUCC) {
                stat->st_dev   = filestat.st_dev;
                stat->st_gid   = filestat.st_gid;
                stat->st_uid   = filestat.st_uid;
                stat->st_mode  = filestat.st_mode;
                stat->st_mtime = filestat.st_mtime;
                stat->st_size  = filestat.st_size;
        }

        return status;
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
API_FS_STATFS(ext2fs, void *fs_handle, struct statfs *statfs)
{
        ext2fs_t *hdl = fs_handle;

        struct ext4_fs_stats stat;
        int status = ext4_statfs(hdl->fsctx, &stat);
        if (status == ESUCC) {
                statfs->f_bfree  = stat.free_blocks_count;
                statfs->f_blocks = stat.blocks_count;
                statfs->f_bsize  = stat.block_size;
                statfs->f_ffree  = stat.free_inodes_count;
                statfs->f_files  = stat.inodes_count;
                statfs->f_type   = 2;
                statfs->f_fsname = "ext2fs";
        }

        return status;
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
        ext2fs_t *hdl = fs_handle;

        int status = ext4_cache_write_back(hdl->fsctx, false);
        if (status == ESUCC) {
                status = ext4_cache_write_back(hdl->fsctx, true);
        }

        return status;
}

//==============================================================================
/**
 * @brief  Protect file system against concurrent
 *
 * @param  ctx          user's context (passed by ext4_mount() function)
 *
 * @return None
 */
//==============================================================================
static void ext4_lock(void *ctx)
{
        ext2fs_t *hdl = ctx;
        _sys_mutex_lock(hdl->mtx, MAX_DELAY_MS);
}

//==============================================================================
/**
 * @brief  Disable orotect file system against concurrent
 *
 * @param  ctx          user's context (passed by ext4_mount() function)
 *
 * @return None
 */
//==============================================================================
static void ext4_unlock(void *ctx)
{
        ext2fs_t *hdl = ctx;
        _sys_mutex_unlock(hdl->mtx);
}

//==============================================================================
/**
 * @brief  Read block from device
 *
 * @param  bdev         block device descriptor
 * @param  buf          data destination buffer
 * @param  blk_id       block ID to read (sector number)
 * @param  blk_cnt      number of blocks to read (sectors)
 *
 * @return Standard error value (one of errno.h)
 */
//==============================================================================
static int ext4_bread(struct ext4_blockdev *bdev, void *buf, uint64_t blk_id, uint32_t blk_cnt)
{
        ext2fs_t *hdl = bdev->usr_ctx;

        int result = _sys_fseek(hdl->srcfile, blk_id * static_cast(u64_t, BLOCK_SIZE), SEEK_SET);
        if (result == ESUCC) {
                size_t rdcnt;
                result = _sys_fread(buf, BLOCK_SIZE * blk_cnt, &rdcnt, hdl->srcfile);
        }

        return result;
}

//==============================================================================
/**
 * @brief  Write block to device
 *
 * @param  bdev         block device descriptor
 * @param  buf          data source buffer
 * @param  blk_id       block ID to read (sector number)
 * @param  blk_cnt      number of blocks to read (sectors)
 *
 * @return Standard error value (one of errno.h)
 */
//==============================================================================
static int ext4_bwrite(struct ext4_blockdev *bdev, const void *buf, uint64_t blk_id, uint32_t blk_cnt)
{
        ext2fs_t *hdl = bdev->usr_ctx;

        int result = _sys_fseek(hdl->srcfile, blk_id * static_cast(u64_t, BLOCK_SIZE), SEEK_SET);
        if (result == ESUCC) {
                size_t wrcnt;
                result = _sys_fwrite(buf, BLOCK_SIZE * blk_cnt, &wrcnt, hdl->srcfile);
        }

        return result;
}

//==============================================================================
/**
 * @brief  Increase number of open files
 *
 * @param  hdl          this file system handle
 *
 * @return None
 */
//==============================================================================
static void increase_openfiles(ext2fs_t *hdl)
{
        _sys_critical_section_begin();
        hdl->openfiles++;
        _sys_critical_section_end();
}

//==============================================================================
/**
 * @brief  Decrease number of open files
 *
 * @param  hdl          this file system handle
 *
 * @return None
 */
//==============================================================================
static void decrease_openfiles(ext2fs_t *hdl)
{
        _sys_critical_section_begin();
        hdl->openfiles--;
        _sys_critical_section_end();
}

/*==============================================================================
  End of file
==============================================================================*/
