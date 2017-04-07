/*=========================================================================*//**
@file    ext2fs.c

@author  Daniel Zorychta

@brief   EXT2 File System by using lwext4 library (kostka.grzegorz@gmail.com)

@note    Copyright (C) 2015 Daniel Zorychta <daniel.zorychta@gmail.com>

         This program is free software; you can redistribute it and/or modify
         it under the terms of the GNU General Public License as published by
         the Free Software Foundation and modified by the dnx RTOS exception.

         NOTE: The modification  to the GPL is  included to allow you to
               distribute a combined work that includes FreeRTOS without
               being obliged to provide the source  code for proprietary
               components outside of the dnx RTOS.

         The dnx RTOS  is  distributed  in the hope  that  it will be useful,
         but WITHOUT  ANY  WARRANTY;  without  even  the implied  warranty of
         MERCHANTABILITY  or  FITNESS  FOR  A  PARTICULAR  PURPOSE.  See  the
         GNU General Public License for more details.

         Full license text is available on the following file: doc/license.txt.


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
 * @param[in ]           *opts                  file system options (can be NULL)
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
API_FS_INIT(ext2fs, void **fs_handle, const char *src_path, const char *opts)
{
        UNUSED_ARG1(opts);

        int err = sys_zalloc(sizeof(ext2fs_t), fs_handle);
        if (err == ESUCC) {
                ext2fs_t *hdl = *fs_handle;

                // open file system source file
                err = sys_fopen(src_path, "r+", &hdl->srcfile);
                if (err != ESUCC)
                        goto finish;

                // read number of file blocks
                struct stat stat;
                err = sys_fstat(hdl->srcfile, &stat);
                if (err != ESUCC)
                        goto finish;

                u64_t block_count = stat.st_size / BLOCK_SIZE;

                err = sys_mutex_create(MUTEX_TYPE_RECURSIVE, &hdl->mtx);
                if (err != ESUCC)
                        goto finish;

                err = ext4_mount(&osif, hdl, BLOCK_SIZE, block_count, &hdl->fsctx);
                if (err == ESUCC) {
                        ext4_cache_write_back(hdl->fsctx, true);
                }

                finish:
                if (err != ESUCC) {
                        if (hdl->srcfile)
                                sys_fclose(hdl->srcfile);

                        if (hdl->mtx)
                                sys_mutex_destroy(hdl->mtx);

                        sys_free(fs_handle);
                }

        }

        return err;
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
                sys_mutex_destroy(hdl->mtx);
                sys_fclose(hdl->srcfile);
                sys_free(fs_handle);
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
 * @param[out]          *fhdl                   file handle
 * @param[out]          *fpos                   file position
 * @param[in]           *path                   file path
 * @param[in]            flags                  file open flags
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
API_FS_OPEN(ext2fs, void *fs_handle, void **fhdl, fpos_t *fpos, const char *path, u32_t flags)
{
        ext2fs_t *hdl = fs_handle;

        ext4_file *file;
        int err = sys_malloc(sizeof(ext4_file), cast(void**, &file));
        if (err == ESUCC) {
                err = ext4_fopen(hdl->fsctx, file, path, flags);
                if (err == ESUCC) {
                        *fpos  = ext4_ftell(hdl->fsctx, file);
                        *fhdl = file;
                        increase_openfiles(hdl);
                } else {
                        sys_free(cast(void**, &file));
                }
        }

        return err;
}

//==============================================================================
/**
 * @brief Close file
 *
 * @param[in ]          *fs_handle              file system allocated memory
 * @param[in ]          *fhdl                   file handle
 * @param[in ]           force                  force close
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
API_FS_CLOSE(ext2fs, void *fs_handle, void *fhdl, bool force)
{
        UNUSED_ARG1(force);

        ext2fs_t *hdl = fs_handle;

        int err = ext4_fclose(hdl->fsctx, fhdl);
        if (err == ESUCC) {
                sys_free(&fhdl);
                decrease_openfiles(hdl);
        }

        return err;
}

//==============================================================================
/**
 * @brief Write data to the file
 *
 * @param[in ]          *fs_handle              file system allocated memory
 * @param[in ]          *fhdl                   file handle
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
             void            *fhdl,
             const u8_t      *src,
             size_t           count,
             fpos_t          *fpos,
             size_t          *wrcnt,
             struct vfs_fattr fattr)
{
        UNUSED_ARG1(fattr);

        ext2fs_t *hdl = fs_handle;

        int err = ext4_fseek(hdl->fsctx, fhdl, *fpos, SEEK_SET);
        if (err == ESUCC) {
                uint32_t wrc = 0;
                err = ext4_fwrite(hdl->fsctx, fhdl, src, count, &wrc);
                if (err == ESUCC) {
                        *wrcnt = wrc;
                }
        }

        return err;
}

//==============================================================================
/**
 * @brief Read data from file
 *
 * @param[in ]          *fs_handle              file system allocated memory
 * @param[in ]          *fhdl                   file handle
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
            void            *fhdl,
            u8_t            *dst,
            size_t           count,
            fpos_t          *fpos,
            size_t          *rdcnt,
            struct vfs_fattr fattr)
{
        UNUSED_ARG1(fattr);

        ext2fs_t *hdl = fs_handle;

        int err = ext4_fseek(hdl->fsctx, fhdl, *fpos, SEEK_SET);
        if (err == ESUCC) {
                u32_t rdc = 0;
                err = ext4_fread(hdl->fsctx, fhdl, dst, count, &rdc);
                if (err == ESUCC) {
                        *rdcnt = rdc;
                }
        }

        return err;
}

//==============================================================================
/**
 * @brief IO operations on files
 *
 * @param[in ]          *fs_handle              file system allocated memory
 * @param[in ]          *fhdl                   file handle
 * @param[in ]           request                request
 * @param[in ][out]     *arg                    request's argument
 *
 * @return Value depends on driver implementation (int)
 */
//==============================================================================
API_FS_IOCTL(ext2fs, void *fs_handle, void *fhdl, int request, void *arg)
{
        UNUSED_ARG1(fs_handle);
        UNUSED_ARG1(fhdl);
        UNUSED_ARG1(request);
        UNUSED_ARG1(arg);

        return ENOTSUP;
}

//==============================================================================
/**
 * @brief Flush file data
 *
 * @param[in ]          *fs_handle              file system allocated memory
 * @param[in ]          *fhdl                   file handle
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
API_FS_FLUSH(ext2fs, void *fs_handle, void *fhdl)
{
        UNUSED_ARG1(fs_handle);
        UNUSED_ARG1(fhdl);

        return ESUCC;
}

//==============================================================================
/**
 * @brief Return file status
 *
 * @param[in ]          *fs_handle              file system allocated memory
 * @param[in ]          *fhdl                   file handle
 * @param[out]          *stat                   file status
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
API_FS_FSTAT(ext2fs, void *fs_handle, void *fhdl, struct stat *stat)
{
        ext2fs_t *hdl = fs_handle;

        struct ext4_filestat filestat;
        int err = ext4_fstat(hdl->fsctx, fhdl, &filestat);
        if (err == ESUCC) {
                stat->st_dev   = filestat.st_dev;
                stat->st_gid   = filestat.st_gid;
                stat->st_uid   = filestat.st_uid;
                stat->st_mode  = filestat.st_mode;
                stat->st_mtime = filestat.st_mtime;
                stat->st_size  = filestat.st_size;
        }

        return err;
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

        int err = ext4_dir_mk(hdl->fsctx, path);
        if (err == ESUCC) {
                ext4_chmod(hdl->fsctx, path, mode);
        }

        return err;
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
        int err = sys_malloc(sizeof(ext4_dir), cast(void**, &ext4dir));
        if (err == ESUCC) {
                err = ext4_dir_open(hdl->fsctx, ext4dir, path);
                if (err == ESUCC) {
                        dir->d_hdl   = ext4dir;
                        dir->d_seek  = 0;
                        dir->d_items = 0;

                        increase_openfiles(hdl);

                } else {
                        sys_free(cast(void**, ext4dir));
                }
        }

        return err;
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
API_FS_CLOSEDIR(ext2fs, void *fs_handle, DIR *dir)
{
        ext2fs_t *hdl = fs_handle;

        int err = ext4_dir_close(hdl->fsctx, dir->d_hdl);
        if (err == ESUCC) {
                sys_free(cast(void**, dir->d_hdl));
                decrease_openfiles(hdl);
        }

        return err;
}

//==============================================================================
/**
 * @brief Read directory
 *
 * @param[in ]          *fs_handle              file system allocated memory
 * @param[in,out]       *dir                    directory object
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
API_FS_READDIR(ext2fs, void *fs_handle, DIR *dir)
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

        ext4_direntry *ext4_dirent = ext4_dir_entry_get(hdl->fsctx, dir->d_hdl, dir->d_seek++);
        if (ext4_dirent) {
                int nlen = ext4_dirent->name_length == 255 ? 254 : ext4_dirent->name_length;
                ext4_dirent->name[nlen] = '\0';

                dir->dirent.dev      = 0;
                dir->dirent.filetype = vfsft[ext4_dirent->inode_type];
                dir->dirent.name     = cast(char*, ext4_dirent->name);
                dir->dirent.size     = ext4_dirent->size;

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
        int err = ext4_fremove(hdl->fsctx, path);
        if (err != ESUCC) {
                // try remove dir
                err = ext4_dir_rm(hdl->fsctx, path);
        }

        return err;
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
        int err = ext4_stat(hdl->fsctx, path, &filestat);
        if (err == ESUCC) {
                stat->st_dev   = filestat.st_dev;
                stat->st_gid   = filestat.st_gid;
                stat->st_uid   = filestat.st_uid;
                stat->st_mode  = filestat.st_mode;
                stat->st_mtime = filestat.st_mtime;
                stat->st_size  = filestat.st_size;
        }

        return err;
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
        int err = ext4_statfs(hdl->fsctx, &stat);
        if (err == ESUCC) {
                statfs->f_bfree  = stat.free_blocks_count;
                statfs->f_blocks = stat.blocks_count;
                statfs->f_bsize  = stat.block_size;
                statfs->f_ffree  = stat.free_inodes_count;
                statfs->f_files  = stat.inodes_count;
                statfs->f_type   = SYS_FS_TYPE__SOLID;
                statfs->f_fsname = "ext2fs";
        }

        return err;
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

        int err = ext4_cache_write_back(hdl->fsctx, false);
        if (err == ESUCC) {
                err = ext4_cache_write_back(hdl->fsctx, true);
        }

        return err;
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
        sys_mutex_lock(hdl->mtx, MAX_DELAY_MS);
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
        sys_mutex_unlock(hdl->mtx);
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

        int err = sys_fseek(hdl->srcfile, blk_id * cast(u64_t, BLOCK_SIZE), SEEK_SET);
        if (err == ESUCC) {
                size_t rdcnt;
                err = sys_fread(buf, BLOCK_SIZE * blk_cnt, &rdcnt, hdl->srcfile);
        }

        return err;
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

        int err = sys_fseek(hdl->srcfile, blk_id * cast(u64_t, BLOCK_SIZE), SEEK_SET);
        if (err == ESUCC) {
                size_t wrcnt;
                err = sys_fwrite(buf, BLOCK_SIZE * blk_cnt, &wrcnt, hdl->srcfile);
        }

        return err;
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
        sys_critical_section_begin();
        hdl->openfiles++;
        sys_critical_section_end();
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
        sys_critical_section_begin();
        hdl->openfiles--;
        sys_critical_section_end();
}

/*==============================================================================
  End of file
==============================================================================*/
