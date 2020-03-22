/*=========================================================================*//**
File     eefs.c

Author   Daniel Zorychta

Brief    EEPROM File System. File system for small memories 1-64KiB EEPROM.

         Copyright (C) 2016 Daniel Zorychta <daniel.zorychta@gmail.com>

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


*//*==========================================================================*/

/*==============================================================================
  Include files
==============================================================================*/
#include "fs/fs.h"

/*==============================================================================
  Local symbolic constants/macros
==============================================================================*/
#if __EEFS_LOG_ENABLE__ > 0
#define DBG(...) printk("EEFS: "__VA_ARGS__)
#else
#define DBG(...)
#endif

#define BUSY_TIMEOUT                    2000

#define BLOCK_SIZE                      128
#define BLOCKS_IN_BYTE                  8

#define MAIN_BLOCK_ADDR                 0

#define ENTRY_TYPE_DIR                  0xFE
#define ENTRY_TYPE_FILE                 0xFD
#define ENTRY_TYPE_NODE                 0xFC

#define BLOCK_MAGIC_MAIN                0x53464545
#define BLOCK_MAGIC_BITMAP              0x20504D42
#define BLOCK_MAGIC_DIR                 0x30524944
#define BLOCK_MAGIC_DIR_ENTRY           0x45524944
#define BLOCK_MAGIC_FILE                0x454C4946
#define BLOCK_MAGIC_FILE_DATA           0x41544144
#define BLOCK_MAGIC_NODE                0x30564544

#define DIR_DESC_MAGIC                  0x24526701
#define FILE_DESC_MAGIC                 0xA2751FF7

#define NAME_LEN                        21      // note: modify with care

#define cache_get_block(sys_cache)      cast(block_cached_t*, &sys_cache[1])->block
#define cache_get_block_num(sys_cache)  cast(block_cached_t*, &sys_cache[1])->block_num

#define FLAG_SYNC                       (1<<0)
#define FLAG_RDONLY                     (1<<1)

#define block_is_dir(block_buf)         (block_buf.buf.dir.magic  == BLOCK_MAGIC_DIR)
#define block_is_dir_entry(block_buf)   (block_buf.buf.dir_entry.magic  == BLOCK_MAGIC_DIR_ENTRY)
#define block_is_file(block_buf)        (block_buf.buf.file.magic == BLOCK_MAGIC_FILE)
#define block_is_file_data(block_buf)   (block_buf.buf.file.magic == BLOCK_MAGIC_FILE_DATA)
#define block_is_node(block_buf)        (block_buf.buf.node.magic == BLOCK_MAGIC_NODE)

#define DEFAULT_MODE                    (S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH)

/*==============================================================================
  Local types, enums definitions
==============================================================================*/
/**
 * FS structure:
 * Block        Structure
 * 0            block_main (defines number of additional bitmap blocks)
 * 1            block_bitmap 1 (if needed)
 * 2            block_bitmap 2 (if needed)
 * n            block_bitmap n (if needed)
 * n+1          dir_entry root (position calculated by using block_main definitions)
 *
 * Next blocks according to block position address defined in root dir entry.
 */

/**
 * Generic checksum block (calculation purposes).
 */
typedef struct PACKED {
        uint8_t  buf[126];
        uint16_t checksum;
} block_chsum_t;

/**
 * Main block definition (block 0).
 */
typedef struct PACKED {
        uint32_t magic;         //!< FS identifier ('EEFS')
        uint16_t blocks;        //!< number of memory 128B blocks
        uint8_t  bitmap_blocks; //!< number of next bitmap blocks
        uint8_t  bitmap[119];   //!< empty block bitmap
        uint16_t checksum;      //!< checksum
} block_main_t;

/**
 * Bitmap block definition.
 */
typedef struct PACKED {
        uint32_t magic;         //!< block magic number
        uint8_t  map[122];      //!< bitmap
        uint16_t checksum;      //!< checksum
} block_bitmap_t;

/**
 * Single directory entry.
 */
typedef struct PACKED {
        uint16_t block_addr;    //!< block address with object
        uint8_t  type;          //!< entry type
        char     name[NAME_LEN];//!< entry name
} dir_entry_t;

/**
 * Directory block.
 */
typedef struct PACKED{
        uint32_t    magic;      //!< block magic number
        uint32_t    ctime;      //!< create timestamp
        uint32_t    mtime;      //!< modification timestamp
        uint16_t    gid;        //!< group ID
        uint16_t    uid;        //!< user ID
        uint16_t    mode;       //!< mode
        uint16_t    parent;     //!< parent entry block number
        uint16_t    next;       //!< next directory entries
        uint16_t    all_items;  //!< number of items in all chains
        uint8_t     _[6];       //!< reserved
        dir_entry_t entry[4];   //!< directory entry
        uint16_t    checksum;   //!< checksum
} block_dir_t;

/**
 * Directory entry block.
 */
typedef struct PACKED {
        uint32_t    magic;      //!< block magic number
        dir_entry_t entry[5];   //!< directory entries
        uint16_t    next;       //!< next directory entries
        uint16_t    checksum;   //!< checksum
} block_dir_entry_t;

/**
 * File block.
 */
typedef struct PACKED {
        uint32_t magic;         //!< block magic number
        uint32_t ctime;         //!< create timestamp
        uint32_t mtime;         //!< modification timestamp
        uint16_t gid;           //!< group ID
        uint16_t uid;           //!< user ID
        uint16_t mode;          //!< mode
        uint16_t size;          //!< file size in bytes
        uint8_t  data[102];     //!< file data
        uint16_t data_next;     //!< next file data
        uint16_t checksum;      //!< checksum
} block_file_t;

/**
 * File data block.
 */
typedef struct PACKED {
        uint32_t magic;         //!< block magic number
        uint8_t  data[120];     //!< data
        uint16_t data_next;     //!< next data block
        uint16_t checksum;      //!< checksum
} block_file_data_t;

/**
 * Device file block.
 */
typedef struct PACKED {
        uint32_t magic;         //!< block magic number
        uint32_t ctime;         //!< create timestamp
        uint32_t mtime;         //!< modification timestamp
        uint32_t dev;           //!< device number
        uint16_t gid;           //!< group ID
        uint16_t uid;           //!< user ID
        uint16_t mode;          //!< mode
        uint8_t  _[104];        //!< reserved
        uint16_t checksum;      //!< checksum
} block_node_t;

/**
 * Generic block.
 */
typedef union {
        block_main_t      main;
        block_bitmap_t    bitmap;
        block_dir_t       dir;
        block_dir_entry_t dir_entry;
        block_file_t      file;
        block_file_data_t file_data;
        block_node_t      node;
        block_chsum_t     chsum;
} block_t;

/**
 * Dir descriptor.
 */
typedef struct dir_desc {
        struct dir_desc *next;
        uint32_t         magic;
        uint16_t         block_num;
        char             name[NAME_LEN];
} dir_desc_t;

/**
 * File descriptor.
 */
typedef struct file_desc {
        struct file_desc *next;
        uint32_t          magic;
        uint16_t          block_num;
        uint8_t           flags;
} file_desc_t;

/**
 * Type contains block buffer and position on disc.
 */
typedef struct {
        u16_t   num;
        block_t buf;
} block_buf_t;

/**
 * File system handle.
 */
typedef struct {
        FILE        *srcdev;
        mutex_t     *lock_mtx;
        dir_desc_t  *open_dirs;
        file_desc_t *open_files;
        uint16_t     root_dir_block;
        block_buf_t  block;
        block_buf_t  tmpblock;
        u8_t         flag;
} EEFS_t;

/*==============================================================================
  Local function prototypes
==============================================================================*/
static uint16_t fletcher16(uint8_t const *data, size_t bytes);
static int block_read(EEFS_t *hdl, block_buf_t *blk);
static int block_write(EEFS_t *hdl, block_buf_t *blk);
static bool is_entry_item_used(dir_entry_t *entry);
static int block_load(EEFS_t *hdl, const char *path);
static int block_load_by_type(EEFS_t *hdl, const char *path, uint32_t type);
static int block_get_file_stat(EEFS_t *hdl, struct stat *stat);
static int bmp_block_find_empty(EEFS_t *hdl, uint16_t *blknum);
static int bmp_block_alloc_ctrl(EEFS_t *hdl, uint16_t blknum, bool allocate);
static int bmp_block_alloc(EEFS_t *hdl, uint16_t blknum);
static int bmp_block_free(EEFS_t *hdl, uint16_t blknum);
static int bmp_get_used_blocks(EEFS_t *hdl, uint16_t *blkused);
static const char *path_get_next_item(const char *path, char **name, size_t *len, bool *last);
static const char *path_get_last_slash(const char *path);
static int path_alloc_dirname(const char *path, char **path_base);
static const char *path_get_basename(const char *path, size_t *len);
static int dir_get_entry_params(EEFS_t *hdl, dir_entry_t **dirent, u8_t *items, u16_t *blknext);
static int dir_add_chain(EEFS_t *hdl);
static int dir_add_item(EEFS_t *hdl, dir_entry_t *dirent, const char *name, u16_t blkparent, u32_t type);
static int dir_add_entry(EEFS_t *hdl, const char *path, uint32_t type);
static int dir_rm_entry(EEFS_t *hdl, const char *path);
static int dir_get_size(EEFS_t *hdl, u16_t *dirsize);
static int dir_read_entry(EEFS_t *hdl, dir_desc_t *dd, dir_entry_t *eefs_entry, dirent_t *dirent);
static int file_truncate(EEFS_t *hdl);
static int file_add_chain(EEFS_t *hdl);
static int file_write(EEFS_t *hdl, const u8_t *src, size_t count, fpos_t *fpos, size_t *wrcnt);
static int file_read(EEFS_t *hdl, u8_t *dst, size_t count, fpos_t *fpos, size_t *rdcnt);

/*==============================================================================
  Local object definitions
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
API_FS_INIT(eefs, void **fs_handle, const char *src_path, const char *opts)
{
        int err = sys_zalloc(sizeof(EEFS_t), fs_handle);
        if (!err) {
                EEFS_t *hdl = *fs_handle;

                err = sys_fopen(src_path, "r+", &hdl->srcdev);
                if (err) {
                        goto finish;
                }

                err = sys_mutex_create(MUTEX_TYPE_NORMAL, &hdl->lock_mtx);
                if (err) {
                        goto finish;
                }

                hdl->block.num = MAIN_BLOCK_ADDR;
                err = block_read(hdl, &hdl->block);
                if (!err) {

                        if (  hdl->block.buf.main.magic         == BLOCK_MAGIC_MAIN
                           && hdl->block.buf.main.blocks        >= 8
                           && hdl->block.buf.main.bitmap_blocks <= 64 ) {

                                hdl->root_dir_block = 1 + hdl->block.buf.main.bitmap_blocks;

                                if (!isstrempty(opts)) {
                                        if (sys_stropt_is_flag(opts, "sync")) {
                                                hdl->flag |= FLAG_SYNC;
                                                DBG("enabled cache write-through");
                                        }

                                        if (sys_stropt_is_flag(opts, "ro")) {
                                                hdl->flag |= FLAG_RDONLY;
                                                DBG("readonly mount");
                                        }
                                }
                        } else {
                                err = EMEDIUMTYPE;
                        }
                }

                finish:
                if (err) {
                        DBG("init error %d", err);

                        if (hdl->srcdev) {
                                sys_fclose(hdl->srcdev);
                        }

                        if (hdl->lock_mtx) {
                                sys_mutex_destroy(hdl->lock_mtx);
                        }

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
API_FS_RELEASE(eefs, void *fs_handle)
{
        EEFS_t *hdl = fs_handle;

        int err = sys_mutex_lock(hdl->lock_mtx, BUSY_TIMEOUT);
        if (!err) {
                if ((hdl->open_files == NULL) && (hdl->open_dirs == NULL)) {

                        sys_fclose(hdl->srcdev);

                        mutex_t *mtx = hdl->lock_mtx;

                        memset(hdl, 0, sizeof(EEFS_t));

                        sys_mutex_unlock(mtx);
                        sys_mutex_destroy(mtx);

                        sys_free(cast(void*, &hdl));

                        return err;
                } else {
                        err = EBUSY;
                }

                sys_mutex_unlock(hdl->lock_mtx);
        }

        return err;
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
API_FS_MKNOD(eefs, void *fs_handle, const char *path, const dev_t dev)
{
        EEFS_t *hdl = fs_handle;

        int err = sys_mutex_lock(hdl->lock_mtx, BUSY_TIMEOUT);
        if (!err) {

                err = dir_add_entry(fs_handle, path, BLOCK_MAGIC_NODE);
                if (!err) {
                        hdl->block.buf.node.dev = cast(u32_t, dev);
                        err = block_write(hdl, &hdl->block);
                }

                sys_mutex_unlock(hdl->lock_mtx);
        }

        DBG("mknod '%s' (%d)", path, err);

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
API_FS_MKDIR(eefs, void *fs_handle, const char *path, mode_t mode)
{
        EEFS_t *hdl = fs_handle;

        int err = sys_mutex_lock(hdl->lock_mtx, BUSY_TIMEOUT);
        if (!err) {

                err = dir_add_entry(fs_handle, path, BLOCK_MAGIC_DIR);
                if (!err) {
                        hdl->block.buf.dir.mode = mode;
                        err = block_write(hdl, &hdl->block);
                }

                sys_mutex_unlock(hdl->lock_mtx);
        }

        DBG("mkdir '%s' (%d)", path, err);

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
API_FS_MKFIFO(eefs, void *fs_handle, const char *path, mode_t mode)
{
        UNUSED_ARG3(fs_handle, path, mode);

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
API_FS_OPENDIR(eefs, void *fs_handle, const char *path, DIR *dir)
{
        EEFS_t *hdl = fs_handle;

        int err = sys_mutex_lock(hdl->lock_mtx, BUSY_TIMEOUT);
        if (!err) {

                err = block_load_by_type(hdl, path, BLOCK_MAGIC_DIR);
                if (!err) {

                        err = sys_malloc(sizeof(dir_desc_t), &dir->d_hdl);
                        if (!err) {
                                dir_desc_t *dd = dir->d_hdl;

                                dd->block_num  = hdl->block.num;
                                dd->magic      = DIR_DESC_MAGIC;
                                dd->next       = hdl->open_dirs;
                                hdl->open_dirs = dd;

                                dir->d_items   = hdl->block.buf.dir.all_items;
                                dir->d_seek    = 0;
                        }
                }

                sys_mutex_unlock(hdl->lock_mtx);
        }

        DBG("opendir '%s' (%d)", path, err);

        return err;
}

//==============================================================================
/**
 * @brief Function close dir
 *
 * @param[in] *fs_handle        FS handle
 * @param[in] *dir              directory info
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
API_FS_CLOSEDIR(eefs, void *fs_handle, DIR *dir)
{
        EEFS_t     *hdl = fs_handle;
        dir_desc_t *dd  = dir->d_hdl;

        int err = sys_mutex_lock(hdl->lock_mtx, BUSY_TIMEOUT);
        if (!err) {
                if (dd->magic == DIR_DESC_MAGIC) {

                        err = EFAULT;

                        dir_desc_t *dirp = hdl->open_dirs;
                        dir_desc_t *prev = NULL;

                        while (dirp) {
                                if (dirp == dd) {
                                        if (prev) {
                                                prev->next = dirp->next;
                                        } else {
                                                hdl->open_dirs = dirp->next;
                                        }

                                        memset(dirp, 0, sizeof(dir_desc_t));
                                        sys_free(&dir->d_hdl);

                                        err = ESUCC;
                                        break;
                                }

                                prev = dirp;
                                dirp = dirp->next;
                        }
                } else {
                        err = EILSEQ;
                }

                sys_mutex_unlock(hdl->lock_mtx);
        }

        DBG("closedir (%d)", err);

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
API_FS_READDIR(eefs, void *fs_handle, DIR *dir)
{
        EEFS_t *hdl = fs_handle;

        dir_desc_t *dd = dir->d_hdl;
        if (!dd || dd->magic != DIR_DESC_MAGIC) {
                DBG("readdir, broken descriptor");
                return EINVAL;
        }

        u16_t seek     = dir->d_seek;
        hdl->block.num = dd->block_num;

        memset(dd->name, 0, sizeof(dd->name));
        dir->dirent.d_name = dd->name;
        dir->dirent.dev    = 0;
        dir->dirent.size   = 0;
        dir->dirent.mode   = 0;

        int err = sys_mutex_lock(hdl->lock_mtx, BUSY_TIMEOUT);
        if (!err) {
                do {
                        err = block_read(hdl, &hdl->block);
                        if (err) {
                                break;
                        }

                        dir_entry_t *entry   = NULL;
                        uint8_t      items   = 0;
                        uint16_t     blknext = 0;

                        err = dir_get_entry_params(hdl, &entry, &items, &blknext);

                        while (!err && items--) {
                                if (is_entry_item_used(entry)) {

                                        if (seek == 0) {
                                                err = dir_read_entry(hdl, dd, entry,
                                                                     &dir->dirent);
                                                dir->d_seek++;
                                                break;
                                        } else {
                                                seek--;
                                        }
                                }

                                entry++;
                        }

                        if (dd->name[0] == 0) {
                                hdl->block.num = blknext;
                        }

                } while (!err && (hdl->block.num > 0) && (dd->name[0] == '\0'));

                sys_mutex_unlock(hdl->lock_mtx);
        }

        if (!err && dd->name[0] == '\0') {
                err = ENOENT;
        }

        DBG("readdir (%d)", err);

        return err;
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
API_FS_REMOVE(eefs, void *fs_handle, const char *path)
{
        EEFS_t *hdl = fs_handle;

        int err = sys_mutex_lock(hdl->lock_mtx, BUSY_TIMEOUT);
        if (!err) {

                err = dir_rm_entry(hdl, path);

                sys_mutex_unlock(hdl->lock_mtx);
        }

        DBG("remove '%s' (%d)", path, err);

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
API_FS_RENAME(eefs, void *fs_handle, const char *old_name, const char *new_name)
{
        EEFS_t *hdl = fs_handle;

        int err =  LAST_CHARACTER(old_name) == '/'
                || LAST_CHARACTER(new_name) == '/'
                 ? EISDIR : ESUCC;

        if (!err) {
                err = sys_mutex_lock(hdl->lock_mtx, BUSY_TIMEOUT);
        }

        if (!err) {

                char       *dirnameold  = NULL;
                char       *dirnamenew  = NULL;
                const char *basenameold = path_get_basename(old_name, NULL);
                const char *basenamenew = path_get_basename(new_name, NULL);

                err = path_alloc_dirname(old_name, &dirnameold);
                if (err) goto finish;

                err = path_alloc_dirname(new_name, &dirnamenew);
                if (err) goto finish;

                if (!isstreq(dirnameold, dirnamenew)) {
                        err = EFAULT;
                }

                if (!err) {
                        err = block_load_by_type(hdl, dirnameold, BLOCK_MAGIC_DIR);
                }

                while (!err) {
                        dir_entry_t *dirent  = NULL;
                        u8_t         items   = 0;
                        u16_t        blknext = 0;

                        err = dir_get_entry_params(hdl, &dirent, &items, &blknext);

                        while (!err && items--) {

                                if (  is_entry_item_used(dirent)
                                   && isstreqn(basenameold, dirent->name, NAME_LEN)) {

                                        strlcpy(dirent->name, basenamenew, NAME_LEN);

                                        err = block_write(hdl, &hdl->block);

                                        goto finish;
                                }

                                dirent++;
                        }

                        if (blknext > 0) {
                                hdl->block.num = blknext;
                                err = block_read(hdl, &hdl->block);

                        } else {
                                err = ENOENT;
                        }

                }

                finish:
                if (dirnameold) {
                        sys_free(cast(void*, &dirnameold));
                }

                if (dirnamenew) {
                        sys_free(cast(void*, &dirnamenew));
                }

                sys_mutex_unlock(hdl->lock_mtx);
        }

        DBG("rename '%s' -> '%s' (%d)", old_name, new_name, err);

        return err;
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
API_FS_CHMOD(eefs, void *fs_handle, const char *path, mode_t mode)
{
        EEFS_t *hdl = fs_handle;

        int err = sys_mutex_lock(hdl->lock_mtx, BUSY_TIMEOUT);
        if (!err) {

                err = block_load(hdl, path);
                if (!err) {
                        switch (hdl->block.buf.dir.magic) {
                        case BLOCK_MAGIC_DIR:
                                hdl->block.buf.dir.mode = mode;
                                break;

                        case BLOCK_MAGIC_FILE:
                                hdl->block.buf.file.mode = mode;
                                break;

                        case BLOCK_MAGIC_NODE:
                                hdl->block.buf.node.mode = mode;
                                break;

                        default:
                                err = ENOENT;
                                break;
                        }

                        if (!err) {
                                err = block_write(hdl, &hdl->block);
                        }
                }

                sys_mutex_unlock(hdl->lock_mtx);
        }

        DBG("chmod '%s' 0%o", path, mode);

        return err;
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
API_FS_CHOWN(eefs, void *fs_handle, const char *path, uid_t owner, gid_t group)
{
        EEFS_t *hdl = fs_handle;

        int err = sys_mutex_lock(hdl->lock_mtx, BUSY_TIMEOUT);
        if (!err) {

                err = block_load(hdl, path);
                if (!err) {
                        switch (hdl->block.buf.dir.magic) {
                        case BLOCK_MAGIC_DIR:
                                hdl->block.buf.dir.uid = owner;
                                hdl->block.buf.dir.gid = group;
                                break;

                        case BLOCK_MAGIC_FILE:
                                hdl->block.buf.file.uid = owner;
                                hdl->block.buf.file.gid = group;
                                break;

                        case BLOCK_MAGIC_NODE:
                                hdl->block.buf.node.uid = owner;
                                hdl->block.buf.node.gid = group;
                                break;

                        default:
                                err = ENOENT;
                                break;
                        }

                        if (!err) {
                                err = block_write(hdl, &hdl->block);
                        }
                }

                sys_mutex_unlock(hdl->lock_mtx);
        }

        DBG("chown '%s' %d %d (%d)", path, owner, group, err);

        return err;
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
API_FS_STAT(eefs, void *fs_handle, const char *path, struct stat *stat)
{
        EEFS_t *hdl = fs_handle;

        int err = sys_mutex_lock(hdl->lock_mtx, BUSY_TIMEOUT);
        if (!err) {

                err = block_load(hdl, path);
                if (!err) {
                        err = block_get_file_stat(hdl, stat);
                }

                sys_mutex_unlock(hdl->lock_mtx);
        }

        DBG("stat '%s' (%d)", path, err);

        return err;
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
API_FS_FSTAT(eefs, void *fs_handle, void *fhdl, struct stat *stat)
{
        EEFS_t      *hdl = fs_handle;
        file_desc_t *fd  = fhdl;

        int err = EILSEQ;

        if (fd->magic == FILE_DESC_MAGIC) {

                err = sys_mutex_lock(hdl->lock_mtx, BUSY_TIMEOUT);
                if (!err) {

                        hdl->block.num = fd->block_num;
                        err = block_read(hdl, &hdl->block);

                        if (!err) {
                                err = block_get_file_stat(hdl, stat);
                        }

                        sys_mutex_unlock(hdl->lock_mtx);
                }
        }

        DBG("fstat (%d)", err);

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
API_FS_STATFS(eefs, void *fs_handle, struct statfs *statfs)
{
        EEFS_t *hdl = fs_handle;

        statfs->f_bsize  = sizeof(block_t);
        statfs->f_ffree  = 0;
        statfs->f_files  = 0;
        statfs->f_fsname = "eefs";
        statfs->f_type   = SYS_FS_TYPE__SOLID;

        int err = sys_mutex_lock(hdl->lock_mtx, BUSY_TIMEOUT);
        if (!err) {

                hdl->block.num = 0;
                err = block_read(hdl, &hdl->block);
                if (!err) {
                        statfs->f_blocks = hdl->block.buf.main.blocks;

                        u16_t blkused = 0;
                        err = bmp_get_used_blocks(hdl, &blkused);
                        if (!err) {
                                statfs->f_bfree = statfs->f_blocks - blkused;
                        }
                }

                sys_mutex_unlock(hdl->lock_mtx);
        }

        DBG("statfs (%d)", err);

        return err;
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
API_FS_OPEN(eefs, void *fs_handle, void **fhdl, fpos_t *fpos, const char *path, u32_t flags)
{
        EEFS_t *hdl = fs_handle;

        int err = sys_mutex_lock(hdl->lock_mtx, BUSY_TIMEOUT);
        if (!err) {

                file_desc_t *fd;
                err = sys_malloc(sizeof(file_desc_t), cast(void*, &fd));
                if (!err) {

                        err = block_load(hdl, path);
                        if (err == ENOENT) {
                                if (flags & O_CREAT) {
                                        err = dir_add_entry(hdl, path, BLOCK_MAGIC_FILE);
                                        if (!err) {
                                                err = block_load(hdl, path);
                                        }
                                }
                        }

                        if (!err) {
                                if (  hdl->block.buf.file.magic == BLOCK_MAGIC_FILE
                                   || hdl->block.buf.node.magic == BLOCK_MAGIC_NODE) {

                                        if (flags & O_TRUNC) {
                                                err = file_truncate(hdl);
                                        }

                                        if (!err) {
                                                if (block_is_node(hdl->block)) {
                                                        *fpos     = 0;
                                                        dev_t dev = hdl->block.buf.node.dev;
                                                        err       = sys_driver_open(dev, flags);

                                                } else if (block_is_file(hdl->block)) {
                                                        *fpos = !(flags & O_APPEND) ? 0
                                                              : hdl->block.buf.file.size;
                                                }
                                        }

                                        if (!err) {
                                                fd->block_num   = hdl->block.num;
                                                fd->magic       = FILE_DESC_MAGIC;
                                                fd->flags       = flags;
                                                fd->next        = hdl->open_files;
                                                hdl->open_files = fd;
                                                *fhdl           = fd;
                                        }
                                } else {
                                        err = EISDIR;
                                }
                        }

                        if (err) {
                                sys_free(cast(void*, &fd));
                        }
                }

                sys_mutex_unlock(hdl->lock_mtx);
        }

        DBG("file open '%s' (%d)", path, err);

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
API_FS_CLOSE(eefs, void *fs_handle, void *fhdl, bool force)
{
        EEFS_t      *hdl = fs_handle;
        file_desc_t *fd  = fhdl;

        int err = sys_mutex_lock(hdl->lock_mtx, BUSY_TIMEOUT);
        if (!err) {
                if (fd->magic == FILE_DESC_MAGIC) {

                        err = EFAULT;

                        file_desc_t *file = hdl->open_files;
                        file_desc_t *prev = NULL;

                        while (file) {
                                if (file == fd) {
                                        dev_t dev = -1;

                                        hdl->block.num = fd->block_num;
                                        err = block_read(hdl, &hdl->block);
                                        if (!err) {
                                                if (block_is_node(hdl->block)) {
                                                        dev = hdl->block.buf.node.dev;
                                                }
                                        }

                                        if (dev != -1) {
                                                err = sys_driver_close(dev, force);
                                        } else {
                                                err = ESUCC;
                                        }

                                        if (!err) {
                                                if (prev) {
                                                        prev->next = file->next;
                                                } else {
                                                        hdl->open_files = file->next;
                                                }

                                                memset(fd, 0, sizeof(file_desc_t));

                                                sys_free(&fhdl);
                                        }

                                        break;
                                }

                                prev = file;
                                file = file->next;
                        }
                } else {
                        err = EILSEQ;
                }

                sys_mutex_unlock(hdl->lock_mtx);
        }

        DBG("close file '%d' (%d)", cast(file_desc_t*, fhdl)->block_num, err);

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
API_FS_WRITE(eefs,
             void            *fs_handle,
             void            *fhdl,
             const u8_t      *src,
             size_t           count,
             fpos_t          *fpos,
             size_t          *wrcnt,
             struct vfs_fattr fattr)
{
        EEFS_t      *hdl = fs_handle;
        file_desc_t *fd  = fhdl;

        int err = EILSEQ;

        if (fd->magic != FILE_DESC_MAGIC) {
                return err;
        }

        err = sys_mutex_lock(hdl->lock_mtx, BUSY_TIMEOUT);
        if (!err) {

                hdl->block.num = fd->block_num;
                err = block_read(hdl, &hdl->block);

                if (!err) {
                        if (block_is_node(hdl->block)) {
                                dev_t dev = hdl->block.buf.node.dev;
                                sys_mutex_unlock(hdl->lock_mtx);

                                err = sys_driver_write(dev, src, count,
                                                       fpos, wrcnt, fattr);
                                goto finish;

                        } else if (block_is_file(hdl->block)) {
                                err = file_write(hdl, src, count, fpos, wrcnt);

                        } else {
                                err = EILSEQ;
                        }
                }
        }

        sys_mutex_unlock(hdl->lock_mtx);

        finish:
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
API_FS_READ(eefs,
            void            *fs_handle,
            void            *fhdl,
            u8_t            *dst,
            size_t           count,
            fpos_t          *fpos,
            size_t          *rdcnt,
            struct vfs_fattr fattr)
{
        EEFS_t      *hdl = fs_handle;
        file_desc_t *fd  = fhdl;

        int err = EILSEQ;

        if (fd->magic != FILE_DESC_MAGIC) {
                return err;
        }

        err = sys_mutex_lock(hdl->lock_mtx, BUSY_TIMEOUT);
        if (!err) {

                hdl->block.num = fd->block_num;
                err = block_read(hdl, &hdl->block);

                if (!err) {
                        if (block_is_node(hdl->block)) {
                                dev_t dev = hdl->block.buf.node.dev;
                                sys_mutex_unlock(hdl->lock_mtx);

                                err = sys_driver_read(dev, dst, count,
                                                      fpos, rdcnt, fattr);
                                goto finish;

                        } else if (block_is_file(hdl->block)) {
                                err = file_read(hdl, dst, count, fpos, rdcnt);

                        } else {
                                err = EILSEQ;
                        }
                }
        }

        sys_mutex_unlock(hdl->lock_mtx);

        finish:
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
 * @return One of errno value (errno.h)
 */
//==============================================================================
API_FS_IOCTL(eefs, void *fs_handle, void *fhdl, int request, void *arg)
{
        EEFS_t      *hdl = fs_handle;
        file_desc_t *fd  = fhdl;

        int err = EILSEQ;

        if (fd->magic == FILE_DESC_MAGIC) {

                err = sys_mutex_lock(hdl->lock_mtx, BUSY_TIMEOUT);
                if (!err) {
                        dev_t dev = -1;

                        hdl->block.num = fd->block_num;
                        err = block_read(hdl, &hdl->block);

                        if (!err) {
                                if (block_is_node(hdl->block)) {
                                        dev = hdl->block.buf.node.dev;
                                }
                        }

                        sys_mutex_unlock(hdl->lock_mtx);

                        if (dev != -1) {
                                err = sys_driver_ioctl(dev, request, arg);
                        }
                }
        }

        return err;
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
API_FS_FLUSH(eefs, void *fs_handle, void *fhdl)
{
        EEFS_t      *hdl = fs_handle;
        file_desc_t *fd  = fhdl;

        int err = EILSEQ;

        if (fd->magic == FILE_DESC_MAGIC) {

                err = sys_mutex_lock(hdl->lock_mtx, BUSY_TIMEOUT);
                if (!err) {
                        dev_t dev = -1;

                        hdl->block.num = fd->block_num;
                        err = block_read(hdl, &hdl->block);

                        if (!err) {
                                if (block_is_node(hdl->block)) {
                                        dev = hdl->block.buf.node.dev;
                                }
                        }

                        sys_mutex_unlock(hdl->lock_mtx);

                        if (dev != -1) {
                                err = sys_driver_flush(dev);
                        }
                }
        }

        return err;
}

//==============================================================================
/**
 * @brief Synchronize all buffers to a medium.
 *
 * @param[in ]          *fs_handle              file system allocated memory
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
API_FS_SYNC(eefs, void *fs_handle)
{
        UNUSED_ARG1(fs_handle);

        return ESUCC;
}

//==============================================================================
/**
 * @brief  Function calculate fletcher 16 checksum.
 *
 * @param  data         buffer
 * @param  bytes        buffer size
 *
 * @return Checksum.
 */
//==============================================================================
static uint16_t fletcher16(uint8_t const *data, size_t bytes)
{
        uint16_t sum1 = 0xff, sum2 = 0xff;
        size_t tlen;

        while (bytes) {
                tlen = ((bytes >= 20) ? 20 : bytes);
                bytes -= tlen;
                do {
                        sum2 += sum1 += *data++;
                        tlen--;
                } while (tlen);

                sum1 = (sum1 & 0xff) + (sum1 >> 8);
                sum2 = (sum2 & 0xff) + (sum2 >> 8);
        }

        /* Second reduction step to reduce sums to 8 bits */
        sum1 = (sum1 & 0xff) + (sum1 >> 8);
        sum2 = (sum2 & 0xff) + (sum2 >> 8);

        return (sum2 << 8) | sum1;
}

//==============================================================================
/**
 * @brief Function read block from memory. Function uses caching subsystem.
 *
 * @param  hdl          FS handle.
 * @param  blk          block to read.
 *
 * @return One of errno value.
 */
//==============================================================================
static int block_read(EEFS_t *hdl, block_buf_t *blk)
{
        memset(&blk->buf, 0, 128);

        size_t rdcnt = 0;
        sys_fseek(hdl->srcdev, blk->num * BLOCK_SIZE, SEEK_SET);
        int err = sys_fread(&blk->buf, BLOCK_SIZE, &rdcnt, hdl->srcdev);

        if (!err) {
                u16_t chsum  = fletcher16(blk->buf.chsum.buf, sizeof(blk->buf.chsum.buf));
                      chsum ^= blk->num;

                err = (chsum == blk->buf.chsum.checksum) ? ESUCC : EILSEQ;

                if (err) {
                        DBG("Block %d checksum fail", blk->num);
                }
        }

        return err;
}

//==============================================================================
/**
 * @brief Function write block to memory. Function uses caching subsystem.
 *
 * @param  hdl          FS handle.
 * @param  blk          block to write.
 *
 * @return One of errno value.
 */
//==============================================================================
static int block_write(EEFS_t *hdl, block_buf_t *blk)
{
        if (hdl->flag & FLAG_RDONLY) {
                return EROFS;

        } else {
                blk->buf.chsum.checksum = fletcher16(blk->buf.chsum.buf,
                                                     sizeof(blk->buf.chsum.buf))
                                        ^ blk->num;

                size_t wrcnt = 0;
                sys_fseek(hdl->srcdev, blk->num * BLOCK_SIZE, SEEK_SET);
                return sys_fwrite(&blk->buf, BLOCK_SIZE, &wrcnt, hdl->srcdev);
        }
}

//==============================================================================
/**
 * @brief  Function check if entry is used.
 *
 * @param  entry    directory entry
 *
 * @return If entry is empty then true is returned otherwise false.
 */
//==============================================================================
static bool is_entry_item_used(dir_entry_t *entry)
{
        return (  entry->type == ENTRY_TYPE_DIR
               || entry->type == ENTRY_TYPE_FILE
               || entry->type == ENTRY_TYPE_NODE);
}

//==============================================================================
/**
 * @brief  Function load block by using path.
 *
 * @param  hdl          EEFS handle
 * @param  path         path
 *
 * @return One of errno value.
 */
//==============================================================================
static int block_load(EEFS_t *hdl, const char *path)
{
        int      err;
        char    *name;
        size_t   namelen;
        bool     namelast;

        bool     found = false;
        hdl->block.num = hdl->root_dir_block;

        if (isstreq(path, "/")) {
                return block_read(hdl, &hdl->block);
        }

        const char *path_ref = path_get_next_item(path, &name, &namelen, &namelast);

        if (!path_ref) {
                DBG("load_block() '%s'", path);
                return ENOENT;
        }

        do {
                if (namelen >= NAME_LEN) {
                        err = ENAMETOOLONG;
                        break;
                }

                err = block_read(hdl, &hdl->block);
                if (err) {
                        break;
                }

                dir_entry_t *dirent  = NULL;
                uint8_t      items   = 0;
                uint16_t     blknext = 0;

                err = dir_get_entry_params(hdl, &dirent, &items, &blknext);

                while (!err && items--) {

                        if (is_entry_item_used(dirent)) {

                                if ( (namelen == strnlen(dirent->name, sizeof(dirent->name)))
                                   && isstreqn(name, dirent->name, namelen) ) {

                                        if (namelast) {
                                                found          = true;
                                                hdl->block.num = dirent->block_addr;
                                                err = block_read(hdl, &hdl->block);

                                        } else {
                                                if (dirent->type == ENTRY_TYPE_DIR) {
                                                        blknext  = dirent->block_addr;
                                                        path_ref = path_get_next_item(path_ref,
                                                                                      &name,
                                                                                      &namelen,
                                                                                      &namelast);
                                                } else {
                                                        err = ENOTDIR;
                                                }
                                        }

                                        break;
                                }
                        }

                        dirent++;
                }

                // load next chain
                if (not found) {
                        if (blknext && (blknext != 0xFFFF)) {
                                hdl->block.num = blknext;
                        } else {
                                err = ENOENT;
                                break;
                        }
                }

        } while (not found && not err);

        return err;
}

//==============================================================================
/**
 * @brief  Function load block by path and type.
 *
 * @param  hdl          EEFS handle
 * @param  path         path
 * @param  type         block type
 *
 * @return One of errno value.
 */
//==============================================================================
static int block_load_by_type(EEFS_t *hdl, const char *path, uint32_t type)
{
        int err = block_load(hdl, path);
        if (!err) {
                if (hdl->block.buf.dir.magic != type) {
                        DBG("Invalid block type");
                        err = ENOENT;
                }
        }

        return err;
}

//==============================================================================
/**
 * @brief  Function get file statistics from current block.
 *
 * @param  hdl          EEFS handle
 * @param  stat         file statistics
 *
 * @return One of errno value.
 */
//==============================================================================
static int block_get_file_stat(EEFS_t *hdl, struct stat *stat)
{
        int err = ESUCC;

        switch (hdl->block.buf.dir.magic) {
        case BLOCK_MAGIC_DIR: {
                u16_t size = 0;
                err = dir_get_size(hdl, &size);

                stat->st_size  = size;
                stat->st_dev   = 0;
                stat->st_mode  = S_IFDIR | hdl->block.buf.dir.mode;
                stat->st_uid   = hdl->block.buf.dir.uid;
                stat->st_gid   = hdl->block.buf.dir.gid;
                stat->st_ctime = hdl->block.buf.dir.ctime;
                stat->st_mtime = hdl->block.buf.dir.mtime;
                break;
        }

        case BLOCK_MAGIC_FILE:
                stat->st_size  = hdl->block.buf.file.size;
                stat->st_dev   = 0;
                stat->st_mode  = S_IFREG | hdl->block.buf.file.mode;
                stat->st_uid   = hdl->block.buf.file.uid;
                stat->st_gid   = hdl->block.buf.file.gid;
                stat->st_ctime = hdl->block.buf.file.ctime;
                stat->st_mtime = hdl->block.buf.file.mtime;
                break;

        case BLOCK_MAGIC_NODE: {
                struct vfs_dev_stat dev_stat;
                err = sys_driver_stat(hdl->block.buf.node.dev, &dev_stat);

                if (!err) {
                        stat->st_size = dev_stat.st_size;

                } if (err == ENODEV) {
                        stat->st_size = 0;
                        err = ESUCC;
                }

                stat->st_dev   = hdl->block.buf.node.dev;
                stat->st_mode  = S_IFDEV | hdl->block.buf.node.mode;
                stat->st_uid   = hdl->block.buf.node.uid;
                stat->st_gid   = hdl->block.buf.node.gid;
                stat->st_ctime = hdl->block.buf.node.ctime;
                stat->st_mtime = hdl->block.buf.node.mtime;
                break;
        }

        default:
                err = EILSEQ;
                break;
        }

        return err;
}

//==============================================================================
/**
 * @brief  Function find empty block by using bitmap.
 *
 * @param  hdl          EEFS handle
 * @param  blknum       found empty block
 *
 * @return One of errno value.
 */
//==============================================================================
static int bmp_block_find_empty(EEFS_t *hdl, uint16_t *blknum)
{
        hdl->tmpblock.num = MAIN_BLOCK_ADDR;
        int err = block_read(hdl, &hdl->tmpblock);
        if (!err) {

                if (hdl->tmpblock.buf.main.magic != BLOCK_MAGIC_MAIN) {
                        err = EILSEQ;
                        goto finish;
                }

                uint16_t blks    = hdl->tmpblock.buf.main.blocks;
                uint16_t bmpblks = hdl->tmpblock.buf.main.bitmap_blocks;
                uint8_t *bmp     = hdl->tmpblock.buf.main.bitmap;
                uint8_t  bsz     = sizeof(hdl->tmpblock.buf.main.bitmap);
                uint8_t  bmpnext = 1;

                *blknum = 0;

                do {
                        for (u8_t i = 0; i < bsz && blks > 0; i++, blks -= 8) {
                                if (bmp[i] == 0) {
                                        continue;
                                }

                                for (u8_t bit = 0; bit < 8; bit++) {
                                        if ((bmp[i] >> bit) & 1) {
                                                *blknum += (i * 8) + bit;
                                                goto finish;
                                        }
                                }
                        }

                        if (bmpblks && blks > 0) {
                                *blknum += (bsz * 8);

                                hdl->tmpblock.num = bmpnext;
                                err = block_read(hdl, &hdl->tmpblock);
                                if (!err) {
                                        if (hdl->block.buf.bitmap.magic == BLOCK_MAGIC_BITMAP) {
                                                bmp = hdl->block.buf.bitmap.map;
                                                bsz = sizeof(hdl->block.buf.bitmap.map);
                                                bmpnext++;
                                        } else {
                                                DBG("Invalid bitmap block");
                                                err = EILSEQ;
                                        }
                                }
                        }

                        if (*blknum == 0) {
                                err = ENOSPC;
                        }
                } while (!err && bmpblks--);
        }

        finish:
        return err;
}

//==============================================================================
/**
 * @brief  Function allocate/release block.
 *
 * @param  hdl          EEFS handle
 * @param  blknum       block number to allocate/release
 * @param  allocate     allocate block if true, otherwise release
 *
 * @return One of errno value.
 */
//==============================================================================
static int bmp_block_alloc_ctrl(EEFS_t *hdl, uint16_t blknum, bool allocate)
{
        hdl->tmpblock.num = MAIN_BLOCK_ADDR;
        int err = block_read(hdl, &hdl->tmpblock);
        if (!err) {

                if (hdl->tmpblock.buf.main.magic != BLOCK_MAGIC_MAIN) {
                        err = EILSEQ;
                        goto finish;
                }

                if (blknum >= hdl->tmpblock.buf.main.blocks) {
                        err = ENOSPC;
                        goto finish;
                }

                u16_t bmpblk = 0;
                u8_t  blkidx = (blknum / 8);
                u8_t  blkbit = (blknum % 8);
                u8_t *bmp    = hdl->tmpblock.buf.main.bitmap;

                if (blkidx >= sizeof(hdl->tmpblock.buf.main.bitmap)) {

                        blkidx -= sizeof(hdl->tmpblock.buf.main.bitmap);
                        bmpblk  = (blkidx / sizeof(hdl->tmpblock.buf.bitmap.map));
                        blkidx -= bmpblk * sizeof(hdl->tmpblock.buf.bitmap.map);
                        bmpblk += 1;

                        if (bmpblk <= hdl->tmpblock.buf.main.bitmap_blocks) {
                                hdl->tmpblock.num = bmpblk;
                                err = block_read(hdl, &hdl->tmpblock);
                                if (!err) {
                                        bmp = hdl->tmpblock.buf.bitmap.map;
                                }
                        } else {
                                err = ENOSPC;
                        }
                }

                if (!err) {
                        if (allocate && !(bmp[blkidx] & (1 << blkbit))) {
                                err = EADDRINUSE;

                        } else {
                                if (allocate) {
                                        bmp[blkidx] &= ~(1 << blkbit);
                                } else {
                                        bmp[blkidx] |= (1 << blkbit);
                                }

                                err = block_write(hdl, &hdl->tmpblock);
                        }
                }
        }

        finish:
        return err;
}

//==============================================================================
/**
 * @brief  Function allocate selected block.
 *
 * @param  hdl          EEFS handle
 * @param  blknum       block to allocate
 *
 * @return One of errno value.
 */
//==============================================================================
static int bmp_block_alloc(EEFS_t *hdl, uint16_t blknum)
{
        return bmp_block_alloc_ctrl(hdl, blknum, true);
}

//==============================================================================
/**
 * @brief  Function free selected block.
 *
 * @param  hdl          EEFS handle
 * @param  blknum       block to free
 *
 * @return One of errno value.
 */
//==============================================================================
static int bmp_block_free(EEFS_t *hdl, uint16_t blknum)
{
        return bmp_block_alloc_ctrl(hdl, blknum, false);
}

//==============================================================================
/**
 * @brief  Function get number of used blocks.
 *
 * @param  hdl          EEFS handle
 * @param  blkused      number of used blocks
 *
 * @return One of errno value.
 */
//==============================================================================
static int bmp_get_used_blocks(EEFS_t *hdl, uint16_t *blkused)
{
        hdl->tmpblock.num = MAIN_BLOCK_ADDR;
        int err = block_read(hdl, &hdl->tmpblock);
        if (!err) {

                if (hdl->tmpblock.buf.main.magic != BLOCK_MAGIC_MAIN) {
                        err = EILSEQ;
                        goto finish;
                }

                uint16_t blks    = hdl->tmpblock.buf.main.blocks;
                uint16_t bmpblks = hdl->tmpblock.buf.main.bitmap_blocks;
                uint8_t *bmp     = hdl->tmpblock.buf.main.bitmap;
                uint8_t  bsz     = sizeof(hdl->tmpblock.buf.main.bitmap);
                uint8_t  bmpnext = 1;

                do {
                        for (u8_t i = 0; i < bsz && blks > 0; i++, blks -= 8) {
                                u8_t ones;

                                if (bmp[i] == 0xFF) {
                                        ones = 8;
                                } else if (bmp[i] == 0) {
                                        ones = 0;
                                } else {
                                        ones = ((bmp[i] >> 7) & 1)
                                             + ((bmp[i] >> 6) & 1)
                                             + ((bmp[i] >> 5) & 1)
                                             + ((bmp[i] >> 4) & 1)
                                             + ((bmp[i] >> 3) & 1)
                                             + ((bmp[i] >> 2) & 1)
                                             + ((bmp[i] >> 1) & 1)
                                             + ((bmp[i] >> 0) & 1);
                                }

                                *blkused += (8 - ones);
                        }

                        if (bmpblks) {
                                hdl->tmpblock.num = bmpnext;
                                err = block_read(hdl, &hdl->tmpblock);
                                if (!err) {
                                        bmp = hdl->tmpblock.buf.bitmap.map;
                                        bsz = sizeof(hdl->tmpblock.buf.bitmap.map);
                                        bmpnext++;
                                }
                        }
                } while (!err && bmpblks--);
        }

        finish:
        return err;
}

//==============================================================================
/**
 * @brief  Function return next item from path (subdir).
 *
 * @param [in]  path        path
 * @param [out] name        name of subitem
 * @param [out] len         subitem name length
 * @param [out] last        subitem is the last one
 *
 * @return Beginning of subitem pointer (slash position).
 */
//==============================================================================
static const char *path_get_next_item(const char *path, char **name, size_t *len, bool *last)
{
        if (FIRST_CHARACTER(path) == '/') {
                path++;
        }

        if (FIRST_CHARACTER(path) == '\0') {
                return NULL;
        }

        size_t      itemlen = 0;
        const char *slash   = strchr(path, '/');

        if (slash) {
                itemlen = cast(size_t, slash - path);
                *last   = (slash[0] == '/' && slash[1] == '\0');
        } else {
                itemlen = strnlen(path, NAME_LEN);
                slash   = path + itemlen;
                *last   = true;
        }

        *len  = itemlen;
        *name = cast(char *, path);

        return slash;
}

//==============================================================================
/**
 * @brief  Function return position of last slash.
 *
 * @param  path     path
 *
 * @return Pointer to last slash.
 */
//==============================================================================
static const char *path_get_last_slash(const char *path)
{
        const char *slash = path;

        if (LAST_CHARACTER(slash) == '/') {
                slash = strrchr(slash, '/');
        }

        if (slash) {
                slash = strrchr(slash, '/');
        }

        return slash;
}

//==============================================================================
/**
 * @brief  Function allocate name for dirname. Should be freed by using free().
 *
 * @param  path         path
 * @param  path_base    output of allocated dirname
 *
 * @return One of errno value.
 */
//==============================================================================
static int path_alloc_dirname(const char *path, char **path_base)
{
        int err = ENOENT;

        if (!isstreqn(path, "/", NAME_LEN)) {
                const char *slash = path_get_last_slash(path);

                if (slash) {
                        size_t sz = slash - path + 1;

                        err = sys_zalloc(sz + 1, cast(void*, path_base));
                        if (!err) {
                                strncpy(*path_base, path, sz);
                        }
                }
        }

        return err;
}

//==============================================================================
/**
 * @brief  Function return pointer to basename and return length.
 *
 * @param  path     path
 * @param  length   basename length
 *
 * @return Pointer to basename string.
 */
//==============================================================================
static const char *path_get_basename(const char *path, size_t *len)
{
        const char *basename     = path_get_last_slash(path) + 1;
        size_t      basenamelen  = strnlen(basename, NAME_LEN);
                    basenamelen -= LAST_CHARACTER(basename) == '/' ? 1 : 0;

        if (len) {
                *len = basenamelen;
        }

        return basename;
}

//==============================================================================
/**
 * @brief  Function gets entry parameteres of dir/dir_entry blocks.
 *
 * @param  hdl          EEFS handle
 * @param  dirent       pointer to dir entry
 * @param  items        number of direntries
 * @param  blknext      next block number
 *
 * @return One of errno value.
 */
//==============================================================================
static int dir_get_entry_params(EEFS_t *hdl, dir_entry_t **dirent, u8_t *items, u16_t *blknext)
{
        int err = EILSEQ;

        if (block_is_dir(hdl->block)) {
                *dirent  = hdl->block.buf.dir.entry;
                *items   = ARRAY_SIZE(hdl->block.buf.dir.entry);
                *blknext = hdl->block.buf.dir.next;
                err      = ESUCC;

        } else if (block_is_dir_entry(hdl->block)) {
                *dirent  = hdl->block.buf.dir_entry.entry;
                *items   = ARRAY_SIZE(hdl->block.buf.dir_entry.entry);
                *blknext = hdl->block.buf.dir_entry.next;
                err      = ESUCC;
        }

        return err;
}

//==============================================================================
/**
 * @brief  Function adds dir chain.
 *
 * @param  hdl          EEFS handle
 *
 * @return One of errno value.
 */
//==============================================================================
static int dir_add_chain(EEFS_t *hdl)
{
        int err = bmp_block_find_empty(hdl, &hdl->tmpblock.num);
        if (!err) {

                memset(&hdl->tmpblock.buf, 0xFF, sizeof(hdl->tmpblock.buf));
                hdl->tmpblock.buf.dir_entry.next  = 0;
                hdl->tmpblock.buf.dir_entry.magic = BLOCK_MAGIC_DIR_ENTRY;

                err = block_write(hdl, &hdl->tmpblock);
                if (!err) {

                        if (block_is_dir(hdl->block)) {
                                hdl->block.buf.dir.next = hdl->tmpblock.num;

                        } else if (block_is_dir_entry(hdl->block)) {
                                hdl->block.buf.dir_entry.next = hdl->tmpblock.num;

                        } else {
                                err = EILSEQ;
                        }

                        err = block_write(hdl, &hdl->block);
                        if (!err) {

                                memcpy(&hdl->block, &hdl->tmpblock, sizeof(hdl->block));

                                err = bmp_block_alloc(hdl, hdl->tmpblock.num);
                        }
                }
        }

        return err;
}

//==============================================================================
/**
 * @brief  Function add item to loaded dir block.
 *
 * @param  hdl          EEFS handle
 * @param  dirent       entry where new item is added
 * @param  blkparent    parent block
 * @param  type         item type (block type)
 *
 * @return One of errno value.
 */
//==============================================================================
static int dir_add_item(EEFS_t *hdl, dir_entry_t *dirent, const char *name, u16_t blkparent, u32_t type)
{
        int err = bmp_block_find_empty(hdl, &hdl->tmpblock.num);
        if (!err) {

                // set directory entry
                dirent->type       = type;
                dirent->block_addr = hdl->tmpblock.num;
                strlcpy(dirent->name, name, NAME_LEN);

                // remove slash at the end if exist
                char *lch = &LAST_CHARACTER(dirent->name);
                if (*lch == '/') {
                        *lch = '\0';
                }

                // create new object
                memset(&hdl->tmpblock.buf, 0xFF, sizeof(hdl->tmpblock.buf));

                time_t time = 0;
                sys_gettime(&time);

                switch (type) {
                case BLOCK_MAGIC_DIR:
                        dirent->type                     = ENTRY_TYPE_DIR;
                        hdl->tmpblock.buf.dir.magic      = type;
                        hdl->tmpblock.buf.dir.ctime      = time;
                        hdl->tmpblock.buf.dir.mtime      = time;
                        hdl->tmpblock.buf.dir.mode       = DEFAULT_MODE;
                        hdl->tmpblock.buf.dir.gid        = 0;
                        hdl->tmpblock.buf.dir.uid        = 0;
                        hdl->tmpblock.buf.dir.all_items  = 0;
                        hdl->tmpblock.buf.dir.next       = 0;
                        hdl->tmpblock.buf.dir.parent     = blkparent;
                        break;

                case BLOCK_MAGIC_FILE:
                        dirent->type                     = ENTRY_TYPE_FILE;
                        hdl->tmpblock.buf.file.magic     = type;
                        hdl->tmpblock.buf.file.ctime     = time;
                        hdl->tmpblock.buf.file.mtime     = time;
                        hdl->tmpblock.buf.file.data_next = 0;
                        hdl->tmpblock.buf.file.gid       = 0;
                        hdl->tmpblock.buf.file.uid       = 0;
                        hdl->tmpblock.buf.file.mode      = DEFAULT_MODE;
                        hdl->tmpblock.buf.file.size      = 0;
                        break;

                case BLOCK_MAGIC_NODE:
                        dirent->type                     = ENTRY_TYPE_NODE;
                        hdl->tmpblock.buf.node.magic     = type;
                        hdl->tmpblock.buf.node.ctime     = time;
                        hdl->tmpblock.buf.node.mtime     = time;
                        hdl->tmpblock.buf.node.mode      = DEFAULT_MODE;
                        hdl->tmpblock.buf.node.gid       = 0;
                        hdl->tmpblock.buf.node.uid       = 0;
                        break;

                default:
                        err = EINVAL;
                        break;
                }

                if (!err) {
                        err = block_write(hdl, &hdl->tmpblock);
                }

                if (!err) {
                        err = block_write(hdl, &hdl->block);
                }

                if (!err) {
                        if (hdl->block.num != blkparent) {
                                hdl->block.num = blkparent;
                                err = block_read(hdl, &hdl->block);
                        }
                }

                if (!err) {
                        if (hdl->block.buf.dir.magic == BLOCK_MAGIC_DIR) {
                                hdl->block.buf.dir.all_items++;
                                err = block_write(hdl, &hdl->block);
                        } else {
                                err = EILSEQ;
                        }
                }

                if (!err) {
                        memcpy(&hdl->block, &hdl->tmpblock, sizeof(hdl->block));
                        err = bmp_block_alloc(hdl, hdl->tmpblock.num);
                }
        }

        return err;
}

//==============================================================================
/**
 * @brief  Function adds entry to selected dir by path.
 *
 * @param  hdl          EEFS handle
 * @param  path         item path to create
 * @param  type         item type (block type)
 *
 * @return One of errno value.
 */
//==============================================================================
static int dir_add_entry(EEFS_t *hdl, const char *path, uint32_t type)
{
        char *root_path = NULL;

        int err = block_load_by_type(hdl, path, BLOCK_MAGIC_DIR);
        if (!err) {
                err = EEXIST;
                goto finish;
        }

        err = path_alloc_dirname(path, &root_path);
        if (err) {
                goto finish;
        }

        err = block_load_by_type(hdl, root_path, BLOCK_MAGIC_DIR);
        if (err) {
                goto finish;
        }

        u16_t blkparent = hdl->block.num;

        bool created = false;
        do {
                dir_entry_t *dirent  = NULL;
                uint8_t      items   = 0;
                uint16_t     blknext = 0;

                err = dir_get_entry_params(hdl, &dirent, &items, &blknext);

                while (!err && items--) {

                        if (!is_entry_item_used(dirent)) {

                                const char *name = path_get_last_slash(path);

                                if (name) {
                                        name++;

                                        err = dir_add_item(hdl, dirent, name,
                                                           blkparent, type);

                                        created = true;

                                } else {
                                        err = EINVAL;
                                }
                                break;
                        }

                        dirent++;
                }

                if (!err && !created) {
                        if (blknext > 0) {
                                hdl->block.num = blknext;
                                err = block_read(hdl, &hdl->block);

                        } else {
                                err = dir_add_chain(hdl);
                        }
                }

        } while (!err && !created);

        finish:
        if (root_path) {
                sys_free(cast(void*, &root_path));
        }

        return err;
}

//==============================================================================
/**
 * @brief  Function remove entry pointed by path.
 *
 * @param  hdl          EEFS handle
 * @param  path         item path
 *
 * @return One of errno value.
 */
//==============================================================================
static int dir_rm_entry(EEFS_t *hdl, const char *path)
{
        char       *dirname     = NULL;
        size_t      basenamelen = 0;
        const char *basename    = path_get_basename(path, &basenamelen);

        int err = path_alloc_dirname(path, &dirname);
        if (err) {
                goto finish;
        }

        err = block_load_by_type(hdl, dirname, BLOCK_MAGIC_DIR);
        if (err) {
                goto finish;
        }

        u16_t blkdirroot = hdl->block.num;
        u16_t blkprev    = 0;

        do {
                dir_entry_t *enttorm = NULL;
                dir_entry_t *dirent  = NULL;
                u8_t         items   = 0;
                u16_t        blknext = 0;
                u8_t         used    = 0;

                err = dir_get_entry_params(hdl, &dirent, &items, &blknext);
                if (err) {
                        break;
                }

                while (items--) {

                        if (is_entry_item_used(dirent)) {

                                used++;

                                if (  basenamelen == strnlen(dirent->name, NAME_LEN)
                                   && isstreqn(basename, dirent->name, basenamelen)) {

                                        enttorm = dirent;
                                }
                        }

                        dirent++;
                }

                if (enttorm) {

                        u16_t blkfile = enttorm->block_addr;

                        // check if file/dir is already opened
                        if (enttorm->type == ENTRY_TYPE_DIR) {
                                dir_desc_t *dd = hdl->open_dirs;
                                while (dd) {
                                        if (dd->block_num == blkfile) {
                                                err = EBUSY;
                                                goto finish;
                                        }

                                        dd = dd->next;
                                }
                        } else {
                                file_desc_t *fd = hdl->open_files;
                                while (fd) {
                                        if (fd->block_num == blkfile) {
                                                err = EBUSY;
                                                goto finish;
                                        }

                                        fd = fd->next;
                                }
                        }

                        // check if item is empty directory --------------------
                        if (enttorm->type == ENTRY_TYPE_DIR) {
                                hdl->tmpblock.num = blkfile;
                                err = block_read(hdl, &hdl->tmpblock);
                                if (!err) {
                                        if (block_is_dir(hdl->tmpblock)) {
                                                if (hdl->tmpblock.buf.dir.all_items > 0) {
                                                        err = ENOTEMPTY;
                                                }
                                        } else {
                                                err = EILSEQ;
                                        }
                                }

                                if (err) {
                                        break;
                                }
                        }

                        // clear dir entry, check that dir block has not entries
                        if (used == 1 && !block_is_dir(hdl->block)) {
                                memset(&hdl->block.buf, 0xFF, sizeof(hdl->block.buf));
                                err = bmp_block_free(hdl, hdl->block.num);
                        } else {
                                memset(enttorm, 0, sizeof(dir_entry_t));
                        }

                        if (!err) {
                                err = block_write(hdl, &hdl->block);
                        }

                        if (!err && used == 1 && blkprev) {
                                hdl->block.num = blkprev;
                                err = block_read(hdl, &hdl->block);
                                if (!err) {
                                        if (block_is_dir(hdl->block)) {
                                                hdl->block.buf.dir.next = blknext;

                                        } else if (block_is_dir_entry(hdl->block)) {
                                                hdl->block.buf.dir_entry.next = blknext;

                                        } else {
                                                err = EILSEQ;
                                        }

                                        if (!err) {
                                                err = block_write(hdl, &hdl->block);
                                        }
                                }
                        }

                        // update number of files in directory -----------------
                        if (!err && hdl->block.num != blkdirroot) {
                                hdl->block.num = blkdirroot;
                                err = block_read(hdl, &hdl->block);
                        }

                        if (!err) {
                                hdl->block.buf.dir.all_items--;
                                err = block_write(hdl, &hdl->block);
                        }

                        // remove file -----------------------------------------
                        if (!err) {
                                hdl->block.num = blkfile;
                                err = block_read(hdl, &hdl->block);
                        }

                        if (!err && !block_is_dir(hdl->block)) {
                                err = file_truncate(hdl);
                        }

                        if (!err) {
                                memset(&hdl->block.buf, 0xFF, sizeof(block_t));
                                err = block_write(hdl, &hdl->block);
                        }

                        if (!err) {
                                err = bmp_block_free(hdl, blkfile);
                        }

                        break;

                } else {
                        blkprev = hdl->block.num;

                        if (blknext > 0) {
                                hdl->block.num = blknext;
                                err = block_read(hdl, &hdl->block);

                        } else {
                                err = ENOENT;
                        }
                }
        } while (!err);

        finish:
        if (dirname) {
                sys_free(cast(void*, &dirname));
        }

        return err;
}

//==============================================================================
/**
 * @brief  Function get size of dir.
 *
 * @param  hdl          EEFS handle
 * @param  dirsize      directory size
 *
 * @return One of errno value.
 */
//==============================================================================
static int dir_get_size(EEFS_t *hdl, u16_t *dirsize)
{
        int err;

        *dirsize = 0;

        do {
                err = block_read(hdl, &hdl->tmpblock);
                if (!err) {
                        *dirsize += sizeof(block_t);

                        if (block_is_dir(hdl->tmpblock)) {
                                hdl->tmpblock.num = hdl->tmpblock.buf.dir.next;

                        } else if (block_is_dir_entry(hdl->tmpblock)) {
                                hdl->tmpblock.num = hdl->tmpblock.buf.dir_entry.next;
                        }

                        if (hdl->tmpblock.num == 0) {
                                break;
                        }
                }

        } while (!err);

        return err;
}

//==============================================================================
/**
 * @brief  Function read directory entry.
 *
 * @param  hdl          EEFS handle
 * @param  dd           directory descriptor
 * @param  entry        directory entry
 * @param  dirent       VFS directory entry
 *
 * @return One of errno value.
 */
//==============================================================================
static int dir_read_entry(EEFS_t *hdl, dir_desc_t *dd, dir_entry_t *eefs_entry, dirent_t *dirent)
{
        int err = EILSEQ;

        strlcpy(dd->name, eefs_entry->name, sizeof(dd->name));

        dirent->dev       = -1;
        hdl->tmpblock.num = eefs_entry->block_addr;

        switch (eefs_entry->type) {
        case ENTRY_TYPE_DIR: {
                u16_t size = 0;
                err = dir_get_size(hdl, &size);
                dirent->size = size;
                dirent->mode = S_IFDIR | hdl->tmpblock.buf.dir.mode;
                break;
        }

        case ENTRY_TYPE_FILE:
                err = block_read(hdl, &hdl->tmpblock);
                if (!err) {
                        dirent->size = hdl->tmpblock.buf.file.size;
                        dirent->mode = S_IFREG | hdl->tmpblock.buf.file.mode;
                }
                break;

        case ENTRY_TYPE_NODE:
                err = block_read(hdl, &hdl->tmpblock);
                if (!err) {
                        dirent->dev  = hdl->tmpblock.buf.node.dev;
                        dirent->mode = S_IFDEV | hdl->tmpblock.buf.node.mode;

                        struct vfs_dev_stat dev_stat;
                        err = sys_driver_stat(hdl->tmpblock.buf.node.dev, &dev_stat);
                        if (!err) {
                                dirent->size = dev_stat.st_size;
                        } else if (err == ENODEV) {
                                dirent->size = 0;
                                err = ESUCC;
                        }
                }
                break;
        }

        return err;
}

//==============================================================================
/**
 * @brief  Function truncate file block that is already loaded into block buffer.
 *
 * @param  hdl          EEFS handle
 *
 * @return One of errno value.
 */
//==============================================================================
static int file_truncate(EEFS_t *hdl)
{
        int err = EILSEQ;

        time_t time = 0;
        sys_gettime(&time);

        if (block_is_file(hdl->block)) {

                hdl->tmpblock.num = hdl->block.buf.file.data_next;

                if (!(  hdl->block.buf.file.size      == 0
                     && hdl->block.buf.file.data_next == 0 )) {

                        hdl->block.buf.file.size      = 0;
                        hdl->block.buf.file.data_next = 0;
                        hdl->block.buf.file.mtime     = time;

                        err = block_write(hdl, &hdl->block);
                } else {
                        err = ESUCC;
                }

                while (!err && hdl->tmpblock.num > 0) {

                        err = block_read(hdl, &hdl->tmpblock);
                        if (!err) {
                                u16_t next = hdl->tmpblock.buf.file_data.data_next;

                                memset(&hdl->tmpblock.buf, 0xFF, sizeof(block_t));
                                err = block_write(hdl, &hdl->tmpblock);
                                if (!err) {
                                        err = bmp_block_free(hdl, hdl->tmpblock.num);
                                }

                                hdl->tmpblock.num = next;
                        }
                }

        } else if (block_is_node(hdl->block)) {
                err = ESUCC;

        } else {
                err = EILSEQ;
        }

        return err;
}

//==============================================================================
/**
 * @brief  Function add next chain to selected file/file_data block.
 *
 * @param  hdl          EEFS handle
 *
 * @return One of errno value.
 */
//==============================================================================
static int file_add_chain(EEFS_t *hdl)
{
        u16_t next = 0;
        int err = bmp_block_find_empty(hdl, &next);
        if (!err) {

                memset(&hdl->tmpblock, 0, sizeof(hdl->tmpblock));
                hdl->tmpblock.buf.file_data.magic = BLOCK_MAGIC_FILE_DATA;

                hdl->tmpblock.num = next;
                err = block_write(hdl, &hdl->tmpblock);

                if (!err) {
                        if (block_is_file(hdl->block)) {
                                hdl->block.buf.file.data_next = next;

                        } else if (block_is_file_data(hdl->block)) {
                                hdl->block.buf.file_data.data_next = next;

                        } else {
                                err = EILSEQ;
                        }

                        if (!err) {
                                err = block_write(hdl, &hdl->block);
                        }
                }

                if (!err) {
                        memcpy(&hdl->block, &hdl->tmpblock, sizeof(hdl->block));
                }

                if (!err) {
                        err = bmp_block_alloc(hdl, next);
                }
        }

        DBG("added file chain (%d)", err);

        return err;
}

//==============================================================================
/**
 * @brief  Function write data to file loaded in current block.
 *
 * @param  hdl          EEFS handle
 * @param  src          source buffer
 * @param  count        number of bytes to write
 * @param  fpos         position in file
 * @param  wrcnt        number of wrote bytes
 *
 * @return One of errno value.
 */
//==============================================================================
static int file_write(EEFS_t *hdl, const u8_t *src, size_t count, fpos_t *fpos, size_t *wrcnt)
{
        int err = ESUCC;

        *wrcnt         = 0;
        u16_t baseblk  = hdl->block.num;
        u16_t chainpos = 0;
        u16_t blkseek  = *fpos;
        u16_t chainsz  = sizeof(((block_file_t*)0)->data);
        u8_t *data     = hdl->block.buf.file.data;

        // calculate start chain number
        if (*fpos > chainsz) {
                chainpos = CEILING((*fpos - chainsz),
                                   sizeof(((block_file_data_t*)0)->data));

                blkseek  = (*fpos - chainsz)
                         - ((chainpos - 1) * sizeof(((block_file_data_t*)0)->data));
        }

        while (!err && count > 0) {
                if (chainpos > 0) {
                        u16_t next = 0;
                        chainsz    = sizeof(hdl->block.buf.file_data.data);
                        data       = hdl->block.buf.file_data.data;

                        if (block_is_file(hdl->block)) {
                                next = hdl->block.buf.file.data_next;

                        } else if (block_is_file_data(hdl->block)) {
                                next = hdl->block.buf.file_data.data_next;

                        } else {
                                err = EILSEQ;
                                break;
                        }

                        if (next == 0) {
                                err = file_add_chain(hdl);
                        } else {
                                hdl->block.num = next;
                                err = block_read(hdl, &hdl->block);
                        }

                        chainpos--;
                }

                if (!err && chainpos == 0) {
                        u16_t sz = min(cast(u16_t, chainsz - blkseek), count);

                        if (sz) {
                                memcpy(data + blkseek, src, sz);
                        }

                        err = block_write(hdl, &hdl->block);

                        if (!err) {
                                *wrcnt  += sz;
                                blkseek  = 0;
                                src     += sz;
                                count   -= sz;
                                chainpos = 1;
                        }
                }
        }

        if (*wrcnt) {
                hdl->block.num = baseblk;
                err = block_read(hdl, &hdl->block);
                if (!err) {
                        time_t time = 0;
                        sys_gettime(&time);
                        hdl->block.buf.file.mtime = time;

                        hdl->block.buf.file.size = max((*fpos + *wrcnt),
                                                       hdl->block.buf.file.size);
                        err = block_write(hdl, &hdl->block);
                }
        }

        return err;
}

//==============================================================================
/**
 * @brief  Function read data from selected file loaded in current block.
 *
 * @param  hdl          EEFS handle
 * @param  dst          destination buffer
 * @param  count        number of bytes to read
 * @param  fpos         file position
 * @param  rdcnt        number of read bytes
 *
 * @return One of errno value.
 */
//==============================================================================
static int file_read(EEFS_t *hdl, u8_t *dst, size_t count, fpos_t *fpos, size_t *rdcnt)
{
        int err = ESUCC;

        *rdcnt         = 0;
        u16_t chainpos = 0;
        u16_t blkseek  = *fpos;
        u16_t chainsz  = sizeof(((block_file_t*)0)->data);
        u8_t *data     = hdl->block.buf.file.data;

        if (*fpos >= hdl->block.buf.file.size) {
                return err;
        }

        if (*fpos + count > hdl->block.buf.file.size) {
                count = hdl->block.buf.file.size - *fpos;
        }

        // calculate start chain number
        if (*fpos > chainsz) {
                chainpos = CEILING((*fpos - chainsz),
                                   sizeof(((block_file_data_t*)0)->data));

                blkseek  = (*fpos - chainsz)
                         - ((chainpos - 1) * sizeof(((block_file_data_t*)0)->data));
        }

        while (!err && count > 0) {
                if (chainpos > 0) {
                        u16_t next = 0;
                        chainsz    = sizeof(hdl->block.buf.file_data.data);
                        data       = hdl->block.buf.file_data.data;

                        if (block_is_file(hdl->block)) {
                                next = hdl->block.buf.file.data_next;

                        } else if (block_is_file_data(hdl->block)) {
                                next = hdl->block.buf.file_data.data_next;

                        } else {
                                err = EILSEQ;
                                break;
                        }

                        if (next == 0) {
                                break; // EOF

                        } else {
                                hdl->block.num = next;
                                err = block_read(hdl, &hdl->block);
                        }

                        chainpos--;
                }

                if (!err && chainpos == 0) {
                        u16_t sz = min(cast(u16_t, chainsz - blkseek), count);

                        if (sz) {
                                memcpy(dst, data + blkseek, sz);
                        }

                        *rdcnt  += sz;
                        blkseek  = 0;
                        dst     += sz;
                        count   -= sz;
                        chainpos = 1;
                }
        }

        return err;
}

/*==============================================================================
  End of file
==============================================================================*/
