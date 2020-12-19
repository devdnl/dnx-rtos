/*==============================================================================
File    ext4fs.c

Author  Daniel Zorychta

Brief   EXT4 file system

        Copyright (C) 2019 Daniel Zorychta <daniel.zorychta@gmail.com>

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
#include "ext4.h"

/*==============================================================================
  Local macros
==============================================================================*/
#define SECTOR_SIZE     512
#define LOCK_TIMEOUT    MAX_DELAY_MS

/*==============================================================================
  Local object types
==============================================================================*/
typedef struct {
        struct ext4_mountpoint    *mp;
        struct ext4_blockdev_iface bdif;
        struct ext4_blockdev       bd;
        FILE                      *dev;
        mutex_t                   *fs_mutex;
        u8_t                       buf[SECTOR_SIZE];
        u32_t                      open_files;
} ext4fs_t;

/*==============================================================================
  Local function prototypes
==============================================================================*/
static void file_opened(ext4fs_t *hdl);
static void file_closed(ext4fs_t *hdl);
static int bopen(struct ext4_blockdev *bdev);
static int bclose(struct ext4_blockdev *bdev);
static int bread(struct ext4_blockdev *bdev, void *buf, uint64_t blk_id, uint32_t blk_cnt);
static int bwrite(struct ext4_blockdev *bdev, const void *buf, uint64_t blk_id, uint32_t blk_cnt);
static int bdev_lock(struct ext4_blockdev *bdev);
static int bdev_unlock(struct ext4_blockdev *bdev);
static void mp_lock(void *p_user);
static void mp_unlock(void *p_user);
static mode_t ext4ftype2vfs(u32_t mode);

/*==============================================================================
  Local objects
==============================================================================*/
static const struct ext4_lock EXT4_LOCK = {
        .lock   = mp_lock,
        .unlock = mp_unlock,
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
 * @brief Initialize file system.
 *
 * @param[out]          **fs_handle             file system allocated memory
 * @param[in ]           *src_path              file source path
 * @param[in ]           *opts                  file system options (can be NULL)
 *
 * @return One of errno value (errno.h).
 */
//==============================================================================
API_FS_INIT(ext4fs, void **fs_handle, const char *src_path, const char *opts)
{
        int err = sys_zalloc(sizeof(ext4fs_t), fs_handle);
        if (!err) {
                ext4fs_t *hdl = *fs_handle;

                bool read_only = sys_stropt_is_flag(opts, "ro");

                err = sys_fopen(src_path, read_only ? "r" : "r+", cast(FILE**, &hdl->dev));
                if (err) goto finish;

                struct stat st;
                err = sys_fstat(hdl->dev, &st);
                if (err) goto finish;

                err = sys_mutex_create(MUTEX_TYPE_RECURSIVE, cast(mutex_t**, &hdl->fs_mutex));
                if (err) goto finish;

                hdl->bdif.open     = bopen;
                hdl->bdif.bread    = bread;
                hdl->bdif.bwrite   = bwrite;
                hdl->bdif.close    = bclose;
                hdl->bdif.lock     = bdev_lock;
                hdl->bdif.unlock   = bdev_unlock;
                hdl->bdif.ph_bsize = SECTOR_SIZE;
                hdl->bdif.ph_bbuf  = hdl->buf;
                hdl->bdif.ph_bcnt  = st.st_size / SECTOR_SIZE;
                hdl->bdif.p_user   = hdl;

                hdl->bd.part_size   = st.st_size;
                hdl->bd.bdif        = &hdl->bdif;

                err = ext4_mount(&hdl->bd, &hdl->mp, read_only, hdl);
                if (!err) {
                        ext4_mount_setup_locks(hdl->mp, &EXT4_LOCK);

                        ext4_recover(hdl->mp);
                        ext4_journal_start(hdl->mp);

                        ext4_cache_write_back(__EXT4FS_CFG_WR_BUF_STRATEGY__, hdl->mp);

                        if (read_only) {
                                printk("EXTFS: read only file system");
                        }
                }

                finish:
                if (err) {
                        if (hdl->fs_mutex) {
                                sys_mutex_destroy(hdl->fs_mutex);
                        }

                        if (hdl->dev) {
                                sys_fclose(cast(FILE*, hdl->dev));
                        }

                        sys_free(fs_handle);
                }
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
API_FS_RELEASE(ext4fs, void *fs_handle)
{
        ext4fs_t *hdl = fs_handle;
        int       err = EBUSY;

        if (hdl->open_files == 0) {
                err = ext4_cache_write_back(false, hdl->mp);
                if (err) goto finish;

                ext4_journal_stop(hdl->mp);

                err = ext4_umount(hdl->mp);
                if (err) goto finish;

                sys_mutex_destroy(hdl->fs_mutex);
                sys_fclose(cast(FILE*, hdl->dev));
                sys_free(&fs_handle);
        }

        finish:
        return err;
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
API_FS_OPEN(ext4fs, void *fs_handle, void **fhdl, fpos_t *fpos, const char *path, u32_t flags)
{
        ext4fs_t  *hdl = fs_handle;
        ext4_file *file;

        file_opened(hdl);

        int err = sys_zalloc(sizeof(ext4_file), cast(void**, &file));
        if (!err) {
                err = ext4_fopen2(file, path, flags, hdl->mp);
                if (!err) {
                        if (flags & O_CREAT) {
                                time_t time = 0;
                                if (sys_gettime(&time) == ESUCC) {

                                        ext4_ctime_set(path, time, hdl->mp);
                                        ext4_mtime_set(path, file, time, hdl->mp);
                                        ext4_atime_set(path, time, hdl->mp);
                                }
                        }

                        *fpos = ext4_ftell(file);
                        *fhdl = file;
                } else {
                        sys_free(cast(void**, &file));
                        file_closed(hdl);
                }
        }

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
API_FS_CLOSE(ext4fs, void *fs_handle, void *fhdl, bool force)
{
        UNUSED_ARG1(force);

        ext4fs_t  *hdl  = fs_handle;
        ext4_file *file = fhdl;

        int err = ext4_fclose(file);
        if (!err) {
                sys_free(cast(void**, &file));
                file_closed(hdl);
        }

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
API_FS_WRITE(ext4fs,
             void            *fs_handle,
             void            *fhdl,
             const u8_t      *src,
             size_t           count,
             fpos_t          *fpos,
             size_t          *wrcnt,
             struct vfs_fattr fattr)
{
        UNUSED_ARG1(fattr);

        ext4fs_t *hdl = fs_handle;

        int err = ext4_fseek(fhdl, *fpos, SEEK_SET);
        if (!err) {
                err = ext4_fwrite(fhdl, src, count, wrcnt);

                time_t mtime = 0;
                if (sys_gettime(&mtime) == ESUCC) {
                        ext4_mtime_set(NULL, fhdl, mtime, hdl->mp);
                }
        }

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
API_FS_READ(ext4fs,
            void            *fs_handle,
            void            *fhdl,
            u8_t            *dst,
            size_t           count,
            fpos_t          *fpos,
            size_t          *rdcnt,
            struct vfs_fattr fattr)
{
        UNUSED_ARG2(fs_handle, fattr);

        int err = ext4_fseek(fhdl, *fpos, SEEK_SET);
        if (!err) {
                err = ext4_fread(fhdl, dst, count, rdcnt);
        }

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
API_FS_IOCTL(ext4fs, void *fs_handle, void *fhdl, int request, void *arg)
{
        UNUSED_ARG4(fs_handle, fhdl, request, arg);

        return ENOTSUP;
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
API_FS_FLUSH(ext4fs, void *fs_handle, void *fhdl)
{
        UNUSED_ARG2(fs_handle, fhdl);

        return ESUCC;
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
API_FS_FSTAT(ext4fs, void *fs_handle, void *fhdl, struct stat *stat)
{
        ext4fs_t  *hdl  = fs_handle;
        ext4_file *file = fhdl;

        u32_t ctime = 0;
        int err = ext4_ctime_get(NULL, file, &ctime, hdl->mp);
        if (err) goto finish;

        u32_t mtime = 0;
        err = ext4_mtime_get(NULL, file, &mtime, hdl->mp);
        if (err) goto finish;

        u32_t uid, gid;
        err = ext4_owner_get(NULL, file, &uid, &gid, hdl->mp);
        if (err) goto finish;

        u32_t mode;
        err = ext4_mode_get(NULL, file, &mode, hdl->mp);
        if (err) goto finish;

        if (!err) {
                stat->st_ctime = ctime;
                stat->st_mtime = mtime;
                stat->st_dev   = 0;
                stat->st_uid   = uid;
                stat->st_gid   = gid;
                stat->st_mode  = (mode & ~EXT4_INODE_MODE_TYPE_MASK) | ext4ftype2vfs(mode);
                stat->st_size  = ext4_fsize(file);
        }

        finish:
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
API_FS_STAT(ext4fs, void *fs_handle, const char *path, struct stat *stat)
{
        ext4fs_t *hdl = fs_handle;

        u32_t ctime = 0;
        int err = ext4_ctime_get(path, NULL, &ctime, hdl->mp);
        if (err) goto finish;

        u32_t mtime = 0;
        err = ext4_mtime_get(path, NULL, &mtime, hdl->mp);
        if (err) goto finish;

        u32_t uid, gid;
        err = ext4_owner_get(path, NULL, &uid, &gid, hdl->mp);
        if (err) goto finish;

        u32_t mode;
        err = ext4_mode_get(path, NULL, &mode, hdl->mp);
        if (err) goto finish;

        u64_t     size = 0;
        ext4_file file;
        memset(&file, 0, sizeof(file));
        if (ext4_fopen2(&file, path, O_RDONLY, hdl->mp) == ESUCC) {
                size = ext4_fsize(&file);
                ext4_fclose(&file);
        }

        if (!err) {
                stat->st_ctime = ctime;
                stat->st_mtime = mtime;
                stat->st_dev   = 0;
                stat->st_uid   = uid;
                stat->st_gid   = gid;
                stat->st_mode  = (mode & ~EXT4_INODE_MODE_TYPE_MASK) | ext4ftype2vfs(mode);
                stat->st_size  = size;
        }

        finish:
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
API_FS_STATFS(ext4fs, void *fs_handle, struct statfs *statfs)
{
        ext4fs_t *hdl = fs_handle;

        statfs->f_type   = SYS_FS_TYPE__SOLID;
        statfs->f_fsname = "ext4fs";

        struct ext4_mount_stats stfs;
        int err = ext4_mount_point_stats(hdl->mp, &stfs);
        if (!err) {
                statfs->f_bsize  = stfs.block_size;
                statfs->f_blocks = stfs.blocks_count;
                statfs->f_bfree  = stfs.free_blocks_count;
                statfs->f_ffree  = stfs.free_inodes_count;
                statfs->f_files  = stfs.inodes_count;
        }

        return err;
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
API_FS_MKDIR(ext4fs, void *fs_handle, const char *path, mode_t mode)
{
        ext4fs_t *hdl = fs_handle;

        int err = ext4_dir_mk(path, hdl->mp);
        if (!err) {
                err = ext4_mode_set(path, mode, hdl->mp);

                time_t ctime = 0;
                if (sys_gettime(&ctime) == ESUCC) {
                        ext4_mtime_set(path, NULL, ctime, hdl->mp);
                }
        }

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
API_FS_MKFIFO(ext4fs, void *fs_handle, const char *path, mode_t mode)
{
        UNUSED_ARG3(fs_handle, path, mode);

        return ENOTSUP;
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
API_FS_MKNOD(ext4fs, void *fs_handle, const char *path, const dev_t dev)
{
        UNUSED_ARG3(fs_handle, path, dev);

        return ENOTSUP;
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
API_FS_OPENDIR(ext4fs, void *fs_handle, const char *path, DIR *dir)
{
        ext4fs_t *hdl = fs_handle;

        file_opened(hdl);

        int err = sys_zalloc(sizeof(ext4_dir), &dir->d_hdl);
        if (!err) {
                err = ext4_dir_open(dir->d_hdl, path, hdl->mp);
                if (!err) {
                        memset(&dir->dirent, 0, sizeof(dir->dirent));
                        dir->d_items = 0;
                        dir->d_seek  = 0;
                } else {
                        file_closed(hdl);
                        sys_free(&dir->d_hdl);
                }
        }

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
API_FS_CLOSEDIR(ext4fs, void *fs_handle, DIR *dir)
{
        ext4fs_t *hdl = fs_handle;

        int err = ext4_dir_close(dir->d_hdl);
        if (!err) {
                file_closed(hdl);
                sys_free(&dir->d_hdl);
        }

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
API_FS_READDIR(ext4fs, void *fs_handle, DIR *dir)
{
        ext4fs_t *hdl = fs_handle;
        ext4_dir *d   = dir->d_hdl;

        if (dir->d_seek == 0) {
                ext4_dir_entry_rewind(d);
        }

        int err = ENOENT;

        ext4_direntry *de = const_cast(ext4_direntry*, ext4_dir_entry_next(d));
        if (de) {
                ext4_file f;
                f.flags = 0;
                f.fpos  = 0;
                f.fsize = 0;
                f.inode = de->inode;
                f.mp    = hdl->mp;

                u32_t mode = 0;
                err = ext4_mode_get(NULL, &f, &mode, hdl->mp);
                if (!err) {
                        size_t len = de->name_length >= 255 ? 254 : de->name_length;
                        de->name[len] = '\0';

                        dir->dirent.dev    = 0;
                        dir->dirent.mode   = (mode & ~EXT4_INODE_MODE_TYPE_MASK) | ext4ftype2vfs(mode);
                        dir->dirent.d_name = cast(const char*, de->name);
                        dir->dirent.size   = ext4_fsize(&f);
                        dir->d_seek        = d->next_off;
                }
        }

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
API_FS_REMOVE(ext4fs, void *fs_handle, const char *path)
{
        ext4fs_t *hdl = fs_handle;

        u32_t mode;
        int err = ext4_mode_get(path, NULL, &mode, hdl->mp);
        if (!err) {
                if ((mode &= EXT4_INODE_MODE_TYPE_MASK) == EXT4_INODE_MODE_DIRECTORY) {
                        err = ext4_dir_rm(path, hdl->mp);
                } else {
                        err = ext4_fremove(path, hdl->mp);
                }
        }

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
API_FS_RENAME(ext4fs, void *fs_handle, const char *old_name, const char *new_name)
{
        ext4fs_t *hdl = fs_handle;
        return ext4_frename(old_name, new_name, hdl->mp);
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
API_FS_CHMOD(ext4fs, void *fs_handle, const char *path, mode_t mode)
{
        ext4fs_t *hdl = fs_handle;
        return ext4_mode_set(path, mode, hdl->mp);
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
API_FS_CHOWN(ext4fs, void *fs_handle, const char *path, uid_t owner, gid_t group)
{
        ext4fs_t *hdl = fs_handle;
        return ext4_owner_set(path, owner, group, hdl->mp);
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
API_FS_SYNC(ext4fs, void *fs_handle)
{
        ext4fs_t *hdl = fs_handle;

        int err = ext4_cache_write_back(false, hdl->mp);
        if (!err) {
                err = ext4_cache_flush(hdl->mp);
                ext4_cache_write_back(__EXT4FS_CFG_WR_BUF_STRATEGY__, hdl->mp);
        }

        return err;
}

//==============================================================================
/**
 * @brief  Function increase open files counter.
 *
 * @param  hdl          file system handle.
 */
//==============================================================================
static void file_opened(ext4fs_t *hdl)
{
        if (sys_mutex_lock(hdl->fs_mutex, LOCK_TIMEOUT) == ESUCC) {
                hdl->open_files++;
                sys_mutex_unlock(hdl->fs_mutex);
        }
}

//==============================================================================
/**
 * @brief  Function decrease open files counter.
 *
 * @param  hdl          file system handle.
 */
//==============================================================================
static void file_closed(ext4fs_t *hdl)
{
        if (sys_mutex_lock(hdl->fs_mutex, LOCK_TIMEOUT) == ESUCC) {
                hdl->open_files--;
                sys_mutex_unlock(hdl->fs_mutex);
        }
}

//==============================================================================
/**
 * @brief  Function open block device. Not used, block opened at FS init.
 *
 * @param  bdev         block device.
 *
 * @return One of errno value (errno.h).
 */
//==============================================================================
static int bopen(struct ext4_blockdev *bdev)
{
        UNUSED_ARG1(bdev);
        return ESUCC;
}

//==============================================================================
/**
 * @brief  Function close block device. Not used, block closed at FS release.
 *
 * @param  bdev         block device.
 *
 * @return One of errno value (errno.h).
 */
//==============================================================================
static int bclose(struct ext4_blockdev *bdev)
{
        UNUSED_ARG1(bdev);
        return ESUCC;
}

//==============================================================================
/**
 * @brief  Function read data from selected block (by cache).
 *
 * @param  bdev         block device.
 * @param  buf          buffer to read.
 * @param  blk_id       block ID (start block).
 * @param  blk_cnt      number of blocks to read.
 *
 * @return One of errno value (errno.h).
 */
//==============================================================================
static int bread(struct ext4_blockdev *bdev, void *buf, uint64_t blk_id, uint32_t blk_cnt)
{
        ext4fs_t *hdl = bdev->bdif->p_user;

        size_t rdcnt = 0;
        sys_fseek(hdl->dev, blk_id * bdev->bdif->ph_bsize, SEEK_SET);
        return sys_fread(buf, bdev->bdif->ph_bsize * blk_cnt, &rdcnt, hdl->dev);
}

//==============================================================================
/**
 * @brief  Function write data to selected block (by cache).
 *
 * @param  bdev         block device.
 * @param  buf          buffer to write.
 * @param  blk_id       block ID (start block).
 * @param  blk_cnt      number of blocks to read.
 *
 * @return One of errno value (errno.h).
 */
//==============================================================================
static int bwrite(struct ext4_blockdev *bdev, const void *buf, uint64_t blk_id, uint32_t blk_cnt)
{
        ext4fs_t *hdl = bdev->bdif->p_user;

        size_t wrcnt = 0;
        sys_fseek(hdl->dev, blk_id * bdev->bdif->ph_bsize, SEEK_SET);
        return sys_fwrite(buf, bdev->bdif->ph_bsize * blk_cnt, &wrcnt, hdl->dev);
}

//==============================================================================
/**
 * @brief  Function lock access to disc.
 *
 * @param  bdev         block device.
 *
 * @return One of errno value (errno.h).
 */
//==============================================================================
static int bdev_lock(struct ext4_blockdev *bdev)
{
        ext4fs_t *hdl = bdev->bdif->p_user;
        return sys_mutex_lock(hdl->fs_mutex, LOCK_TIMEOUT);
}

//==============================================================================
/**
 * @brief  Function unlock access to disc.
 *
 * @param  bdev         block device.
 *
 * @return One of errno value (errno.h).
 */
//==============================================================================
static int bdev_unlock(struct ext4_blockdev *bdev)
{
        ext4fs_t *hdl = bdev->bdif->p_user;
        return sys_mutex_unlock(hdl->fs_mutex);
}

//==============================================================================
/**
 * @brief  Function lock access to mount point.
 *
 * @param  p_user       user object pointer
 */
//==============================================================================
static void mp_lock(void *p_user)
{
        ext4fs_t *hdl = p_user;
        sys_mutex_lock(hdl->fs_mutex, LOCK_TIMEOUT);
}

//==============================================================================
/**
 * @brief  Function unlock access to mount point.
 *
 * @param  p_user       user object pointer
 */
//==============================================================================
static void mp_unlock(void *p_user)
{
        ext4fs_t *hdl = p_user;
        sys_mutex_unlock(hdl->fs_mutex);
}

//==============================================================================
/**
 * @brief  Function convert EXT4 file type to VFS.
 *
 * @param  mode         file type coded in mode.
 *
 * @return VFS file type.
 */
//==============================================================================
static mode_t ext4ftype2vfs(u32_t mode)
{
        switch (mode & EXT4_INODE_MODE_TYPE_MASK) {
        case EXT4_INODE_MODE_FIFO:      return S_IFIFO;
        case EXT4_INODE_MODE_CHARDEV:   return S_IFDEV;
        case EXT4_INODE_MODE_DIRECTORY: return S_IFDIR;
        case EXT4_INODE_MODE_BLOCKDEV:  return S_IFDEV;
        case EXT4_INODE_MODE_FILE:      return S_IFREG;
        case EXT4_INODE_MODE_SOFTLINK:  return S_IFLNK;
        case EXT4_INODE_MODE_SOCKET:    return S_IFIFO;
        default:                        return mode;
        }
}

//==============================================================================
/**
 * @brief  Function allocate memory for EXT4 library.
 *
 * @param  size         size to allocate.
 *
 * @return Pointer to allocated memory or NULL if error.
 */
//==============================================================================
void *ext4_user_malloc(size_t size)
{
        void *mem = NULL;
        return sys_malloc(size, &mem) == ESUCC ? mem : NULL;
}

//==============================================================================
/**
 * @brief  Function allocate memory for EXT4 library.
 *
 * @param  num          number of elements to allocate.
 * @param  size         element size.
 *
 * @return Pointer to allocated memory or NULL if error.
 */
//==============================================================================
void *ext4_user_calloc(size_t num, size_t size)
{
        void *mem = NULL;
        return sys_zalloc(num * size, &mem) == ESUCC ? mem : NULL;
}

//==============================================================================
/**
 * @brief  Function free allocated memory.
 *
 * @param  mem          memory to free.
 */
//==============================================================================
void ext4_user_free(void *mem)
{
        sys_free(&mem);
}

/*==============================================================================
  End of file
==============================================================================*/
