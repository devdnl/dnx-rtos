/*==============================================================================
File    flatfs.c

Author  daniel.zorychta@gmail.com

Brief   Daniel Zorychta

        Copyright (C) 2021 Daniel Zorychta <daniel.zorychta@gmail.com>

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
#include <assert.h>

/*==============================================================================
  Local macros
==============================================================================*/
#if __FLATFS_LOG_ENABLE__ > 0
#define DBG(...)                MSG(__VA_ARGS__)
#else
#define DBG(...)
#endif

#define MSG(...)                printk("FLATFS: "__VA_ARGS__)

#define MTX_TIMEOUT             2000

#define MAGIC_BLOCK_FS          0x54414C46
#define MAGIC_BLOCK_FILE        0x454C4946

#define BLOCK_SIZE              512
#define CRC32_INIT              0xFFFFFFFF

#define MAX_FILE_NAME_LEN       (BLOCK_SIZE - offsetof(blk_file_t, name))

#define foreach_file_node(_hdl) u32_t _max_files = _hdl->total_files;\
        for (size_t i = 0; i < ARRAY_SIZE(_hdl->bitmap.word); i++)\
        for (size_t bit = 0; (bit < ARRAY_ITEM_SIZE(_hdl->bitmap.word) * 8) and (_max_files > 0); bit++, _max_files--)\
        for (u32_t node = ((ARRAY_ITEM_SIZE(_hdl->bitmap.word) * 8) * i) + bit; node < sizeof(_hdl->bitmap.word) * 8; node = UINT32_MAX)

#define foreach_used_file_node(hdl) foreach_file_node(hdl) if (hdl->bitmap.word[i] & (1 << bit))

#define FIND_ANY_NODE           UINT32_MAX
#define MAX_FILE_NAME_LEN       (BLOCK_SIZE - offsetof(blk_file_t, name))

/*==============================================================================
  Local object types
==============================================================================*/
/*
 * File system descriptor block.
 */
typedef union {
        struct {
                u32_t magic;                    // Block magic number: MAGIC_BLOCK_FS
                u32_t crc;                      // CRC32 calculated starting from field below up to block end
                u32_t total_volume_blocks;      // Total number of block with FS and bitmaps
                u32_t file_node_blocks;         // Number of blocks per file
                u32_t last_file_node_blocks;    // Number of blocks of last file
                u32_t bitmap_address_primary;   // Address of primary bitmap
                u32_t bitmap_address_secondary; // Address of secondary bitmap
                u32_t first_file_node_address;  // Address of first file entry block
                u32_t total_files;              // Total number of files
        };

        u8_t block[BLOCK_SIZE];
} blk_fs_t;

/*
 * File descriptor block.
 */
typedef union {
        struct {
                u32_t  magic;                   // Block magic number: MAGIC_BLOCK_FILE
                u32_t  crc;                     // CRC32 calculated starting from field below up to block end
                u64_t  ctime;                   // time of creation
                u64_t  mtime;                   // time of last modification
                mode_t mode;                    // file mode
                uid_t  uid;                     // user ID of owner
                gid_t  gid;                     // group ID of owner
                u32_t  size;                    // File size in bytes
                char   name[320];               // file name
        };

        u8_t block[BLOCK_SIZE];
} blk_file_t;

static_assert(sizeof(blk_file_t) == BLOCK_SIZE, "blk_file_t is bigger than BLOCK_SIZE!");

/*
 * Bitmap block.
 */
typedef struct {
        u32_t word[BLOCK_SIZE / sizeof(u32_t)];
} blk_bitmap_t;

/*
 * File handle.
 */
typedef struct {
        u32_t node_num;
} file_t;

/*
 * Type contains block cache buffer.
 */
typedef struct {
        u32_t node;
        i32_t hit;
        blk_file_t blk;
} path_index_t;

/*
 * File system handle.
 */
typedef struct {
        kmtx_t      *mtx;
        kfile_t     *disc;
        llist_t     *open_files;
        bool         read_only;
        bool         force_check;
        u32_t        total_files;
        u32_t        total_volume_blocks;
        u32_t        file_node_blocks;
        u32_t        last_file_node_blocks;
        u32_t        bitmap_address_primary;
        u32_t        bitmap_address_secondary;
        u32_t        first_file_node_address;
        blk_bitmap_t bitmap;
        char         tmp_path[MAX_FILE_NAME_LEN];
        u8_t         block[2][BLOCK_SIZE];

#if __FLATFS_INDEX_SIZE__ > 0
        path_index_t path_index[__FLATFS_INDEX_SIZE__];
#endif
} flatfs_t;

/*
 * Readdir specified dir entry
 */
typedef struct {
        u64_t  size;            //!< File size in bytes
        mode_t mode;            //!< File mode (protection, file type)
        dev_t  dev;             //!< Device address (if file type is driver)
        char   name[];          //!< File name placeholder
} readdir_dirent_t;

/*==============================================================================
  Local function prototypes
==============================================================================*/
static int readdir_dirent_llist_cmp(const void *a, const void *b);
static u32_t calc_crc32(const void *buf, size_t buflen, u32_t init);
static int block_read(flatfs_t *hdl, u32_t address, void *dst, size_t blocks);
static int block_write(flatfs_t *hdl, u32_t address, const void *src, size_t blocks);
static int mount(flatfs_t *hdl);
static int check_first_sector_coherency(flatfs_t *hdl);
static int check_bitmaps_coherency_and_repair(flatfs_t *hdl);
static int check_file_block_coherency(flatfs_t *hdl, const blk_file_t *blk);
static u32_t node_get_blkaddr(flatfs_t *hdl, u32_t node_num);
static int node_find(flatfs_t *hdl, const char *name, size_t name_len, mode_t mode_mask, blk_file_t *blk, u32_t *node_num);
static int node_create(flatfs_t *hdl, const char *name, mode_t mode, blk_file_t *blk, u32_t *node_num);
static int node_remove(flatfs_t *hdl, u32_t node_num);
static void set_bitmap_bit(flatfs_t *hdl, u32_t node_num, bool set);
static u32_t calc_blk_crc(const void *blk);
static int file_block_read(flatfs_t *hdl, u32_t node_num, blk_file_t *blk);
static int file_block_write(flatfs_t *hdl, u32_t node_num, blk_file_t *blk);
static void index_remove(flatfs_t *hdl, u32_t node_num);
static bool index_find(flatfs_t *hdl, const char *name, size_t name_len, mode_t mode, blk_file_t *blk, u32_t *node_num);
static bool index_get(flatfs_t *hdl, u32_t node_num, blk_file_t *blk);
static void index_add(flatfs_t *hdl, u32_t node_num, const blk_file_t *blk);
static void index_update(flatfs_t *hdl, u32_t node_num, const blk_file_t *blk);

/*==============================================================================
  Local objects
==============================================================================*/
/*
 * Table for polynomial: 0x04c11db7
 */
static const u32_t CRC32_TAB[] = {
        0x00000000, 0x04c11db7, 0x09823b6e, 0x0d4326d9,
        0x130476dc, 0x17c56b6b, 0x1a864db2, 0x1e475005,
        0x2608edb8, 0x22c9f00f, 0x2f8ad6d6, 0x2b4bcb61,
        0x350c9b64, 0x31cd86d3, 0x3c8ea00a, 0x384fbdbd,
        0x4c11db70, 0x48d0c6c7, 0x4593e01e, 0x4152fda9,
        0x5f15adac, 0x5bd4b01b, 0x569796c2, 0x52568b75,
        0x6a1936c8, 0x6ed82b7f, 0x639b0da6, 0x675a1011,
        0x791d4014, 0x7ddc5da3, 0x709f7b7a, 0x745e66cd,
        0x9823b6e0, 0x9ce2ab57, 0x91a18d8e, 0x95609039,
        0x8b27c03c, 0x8fe6dd8b, 0x82a5fb52, 0x8664e6e5,
        0xbe2b5b58, 0xbaea46ef, 0xb7a96036, 0xb3687d81,
        0xad2f2d84, 0xa9ee3033, 0xa4ad16ea, 0xa06c0b5d,
        0xd4326d90, 0xd0f37027, 0xddb056fe, 0xd9714b49,
        0xc7361b4c, 0xc3f706fb, 0xceb42022, 0xca753d95,
        0xf23a8028, 0xf6fb9d9f, 0xfbb8bb46, 0xff79a6f1,
        0xe13ef6f4, 0xe5ffeb43, 0xe8bccd9a, 0xec7dd02d,
        0x34867077, 0x30476dc0, 0x3d044b19, 0x39c556ae,
        0x278206ab, 0x23431b1c, 0x2e003dc5, 0x2ac12072,
        0x128e9dcf, 0x164f8078, 0x1b0ca6a1, 0x1fcdbb16,
        0x018aeb13, 0x054bf6a4, 0x0808d07d, 0x0cc9cdca,
        0x7897ab07, 0x7c56b6b0, 0x71159069, 0x75d48dde,
        0x6b93dddb, 0x6f52c06c, 0x6211e6b5, 0x66d0fb02,
        0x5e9f46bf, 0x5a5e5b08, 0x571d7dd1, 0x53dc6066,
        0x4d9b3063, 0x495a2dd4, 0x44190b0d, 0x40d816ba,
        0xaca5c697, 0xa864db20, 0xa527fdf9, 0xa1e6e04e,
        0xbfa1b04b, 0xbb60adfc, 0xb6238b25, 0xb2e29692,
        0x8aad2b2f, 0x8e6c3698, 0x832f1041, 0x87ee0df6,
        0x99a95df3, 0x9d684044, 0x902b669d, 0x94ea7b2a,
        0xe0b41de7, 0xe4750050, 0xe9362689, 0xedf73b3e,
        0xf3b06b3b, 0xf771768c, 0xfa325055, 0xfef34de2,
        0xc6bcf05f, 0xc27dede8, 0xcf3ecb31, 0xcbffd686,
        0xd5b88683, 0xd1799b34, 0xdc3abded, 0xd8fba05a,
        0x690ce0ee, 0x6dcdfd59, 0x608edb80, 0x644fc637,
        0x7a089632, 0x7ec98b85, 0x738aad5c, 0x774bb0eb,
        0x4f040d56, 0x4bc510e1, 0x46863638, 0x42472b8f,
        0x5c007b8a, 0x58c1663d, 0x558240e4, 0x51435d53,
        0x251d3b9e, 0x21dc2629, 0x2c9f00f0, 0x285e1d47,
        0x36194d42, 0x32d850f5, 0x3f9b762c, 0x3b5a6b9b,
        0x0315d626, 0x07d4cb91, 0x0a97ed48, 0x0e56f0ff,
        0x1011a0fa, 0x14d0bd4d, 0x19939b94, 0x1d528623,
        0xf12f560e, 0xf5ee4bb9, 0xf8ad6d60, 0xfc6c70d7,
        0xe22b20d2, 0xe6ea3d65, 0xeba91bbc, 0xef68060b,
        0xd727bbb6, 0xd3e6a601, 0xdea580d8, 0xda649d6f,
        0xc423cd6a, 0xc0e2d0dd, 0xcda1f604, 0xc960ebb3,
        0xbd3e8d7e, 0xb9ff90c9, 0xb4bcb610, 0xb07daba7,
        0xae3afba2, 0xaafbe615, 0xa7b8c0cc, 0xa379dd7b,
        0x9b3660c6, 0x9ff77d71, 0x92b45ba8, 0x9675461f,
        0x8832161a, 0x8cf30bad, 0x81b02d74, 0x857130c3,
        0x5d8a9099, 0x594b8d2e, 0x5408abf7, 0x50c9b640,
        0x4e8ee645, 0x4a4ffbf2, 0x470cdd2b, 0x43cdc09c,
        0x7b827d21, 0x7f436096, 0x7200464f, 0x76c15bf8,
        0x68860bfd, 0x6c47164a, 0x61043093, 0x65c52d24,
        0x119b4be9, 0x155a565e, 0x18197087, 0x1cd86d30,
        0x029f3d35, 0x065e2082, 0x0b1d065b, 0x0fdc1bec,
        0x3793a651, 0x3352bbe6, 0x3e119d3f, 0x3ad08088,
        0x2497d08d, 0x2056cd3a, 0x2d15ebe3, 0x29d4f654,
        0xc5a92679, 0xc1683bce, 0xcc2b1d17, 0xc8ea00a0,
        0xd6ad50a5, 0xd26c4d12, 0xdf2f6bcb, 0xdbee767c,
        0xe3a1cbc1, 0xe760d676, 0xea23f0af, 0xeee2ed18,
        0xf0a5bd1d, 0xf464a0aa, 0xf9278673, 0xfde69bc4,
        0x89b8fd09, 0x8d79e0be, 0x803ac667, 0x84fbdbd0,
        0x9abc8bd5, 0x9e7d9662, 0x933eb0bb, 0x97ffad0c,
        0xafb010b1, 0xab710d06, 0xa6322bdf, 0xa2f33668,
        0xbcb4666d, 0xb8757bda, 0xb5365d03, 0xb1f740b4
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
API_FS_INIT(flatfs, void **fs_handle, const char *src_path, const char *opts)
{
        int err = sys_zalloc(sizeof(flatfs_t), fs_handle);
        if (!err) {
                flatfs_t *hdl = *fs_handle;

                hdl->read_only = sys_stropt_is_flag(opts, "ro");
                hdl->force_check = sys_stropt_is_flag(opts, "check");

                if (hdl->read_only) {
                        DBG("mounting read-only");
                }

                if (hdl->force_check) {
                        DBG("enable file system force check");
                }

                if (__FLATFS_INDEX_SIZE__ > 0) {
                        MSG("indexing enabled");
                }

                err = sys_mutex_create(KMTX_TYPE_NORMAL, &hdl->mtx);
                if (!err) {

                        err = sys_llist_create(sys_llist_functor_cmp_pointers, NULL, &hdl->open_files);
                        if (!err) {

                                err = sys_fopen(src_path, hdl->read_only ? O_RDONLY : O_RDWR, 0, &hdl->disc);
                                if (!err) {

                                        err = mount(hdl);
                                        if (!err) {
                                                return err;
                                        }
                                }

                                sys_llist_destroy(hdl->open_files);
                        }

                        sys_mutex_destroy(hdl->mtx);
                }

                sys_free(fs_handle);
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
API_FS_RELEASE(flatfs, void *fs_handle)
{
        flatfs_t *hdl = fs_handle;

        kmtx_t *mtx = hdl->mtx;

        int err = sys_mutex_lock(mtx, MTX_TIMEOUT);
        if (!err) {
                if (sys_llist_size(hdl->open_files) == 0) {

                        hdl->mtx = NULL;
                        sys_llist_destroy(hdl->open_files);

                        sys_fflush(hdl->disc);
                        sys_fclose(hdl->disc);

                        sys_mutex_unlock(mtx);
                        sys_mutex_destroy(mtx);

                        sys_free(&fs_handle);
                } else {
                        err = EBUSY;
                }
        }

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
API_FS_OPEN(flatfs, void *fs_handle, void **fhdl, fpos_t *fpos, const char *path, u32_t flags)
{
        flatfs_t *hdl = fs_handle;

        file_t *file;
        int err = sys_zalloc(sizeof(file_t), cast(void**, &file));
        if (!err) {
                *fhdl = file;
                *fpos = 0;

                err = sys_mutex_lock(hdl->mtx, MTX_TIMEOUT);
                if (!err) {
                        blk_file_t *blk = cast(blk_file_t*, hdl->block[0]);

                        /*
                         * Try to open directory that should contains selected file
                         */
                        sys_strlcpy(hdl->tmp_path, path, sizeof(hdl->tmp_path));
                        char *slash = strrchr(hdl->tmp_path, '/');
                        if (slash) {
                                *slash = '\0';

                                if (not isstrempty(hdl->tmp_path)) {
                                        err = node_find(hdl, hdl->tmp_path, MAX_FILE_NAME_LEN,
                                                S_IFDIR, blk, &file->node_num);
                                        if (err) {
                                                goto finish;
                                        }
                                } else {
                                        // root dir always exists (without any node)
                                }

                        } else {
                                err = ENOENT;
                                goto finish;
                        }

                        /*
                         * Try to open path as is to check if is a directory
                         */
                        err = node_find(hdl, path, MAX_FILE_NAME_LEN,
                                        S_IFDIR, blk, &file->node_num);
                        if (not err) {
                                err = EISDIR;
                                goto finish;
                        }

                        /*
                         * Try to open/create file in selected directory
                         */
                        err = node_find(hdl, path, MAX_FILE_NAME_LEN,
                                S_IFREG, blk, &file->node_num);

                        if (err == ENOENT) {
                                if (flags & O_CREAT) {
                                        err = node_create(hdl, path, S_IFREG, blk,
                                                  &file->node_num);
                                }
                        }

                        /*
                         * Opened block should be a regular file
                         */
                        if (S_ISREG(blk->mode)) {

                                if (!err) {
                                        if (flags & O_TRUNC) {
                                                time_t mtime = 0;
                                                sys_gettime(&mtime);

                                                blk->size  = 0;
                                                blk->mtime = mtime;

                                                err = file_block_write(hdl, file->node_num, blk);
                                        }

                                        if (flags & O_APPEND) {
                                                *fpos = blk->size;
                                        }
                                }

                                if (!err) {
                                        if (sys_llist_push_back(hdl->open_files, file) == file) {
                                                err = 0;
                                        } else {
                                                err = ENOMEM;
                                        }
                                } else {
                                        sys_free(cast(void**, &file));
                                }
                        } else {
                                err = EISDIR;
                        }

                        finish:
                        sys_mutex_unlock(hdl->mtx);
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
API_FS_CLOSE(flatfs, void *fs_handle, void *fhdl, bool force)
{
        flatfs_t *hdl = fs_handle;

        int err = sys_mutex_lock(hdl->mtx, MTX_TIMEOUT);
        if (!err) {
                file_t *file = fhdl;

                int pos = sys_llist_find_begin(hdl->open_files, file);
                if (pos >= 0) {
                        sys_llist_erase(hdl->open_files, pos);
                        err = ESUCC;

                } else {
                        err = force ? ESUCC : ENOENT;
                }

                sys_mutex_unlock(hdl->mtx);
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
 * @param[out]          *wrctr                  number of written bytes
 * @param[in ]           fattr                  file attributes
 *
 * @return One of errno value (errno.h).
 */
//==============================================================================
API_FS_WRITE(flatfs,
             void            *fs_handle,
             void            *fhdl,
             const u8_t      *src,
             size_t           count,
             fpos_t          *fpos,
             size_t          *wrctr,
             struct vfs_fattr fattr)
{
        UNUSED_ARG1(fattr);

        flatfs_t *hdl  = fs_handle;
        file_t   *file = fhdl;

        if (hdl->read_only) {
                return EROFS;
        }

        int err = sys_mutex_lock(hdl->mtx, MTX_TIMEOUT);
        if (!err) {

                u32_t seek = *fpos;
                *wrctr = 0;

                blk_file_t *blk = cast(blk_file_t*, hdl->block[0]);
                err = file_block_read(hdl, file->node_num, blk);

                if (!err) {
                        // fill by zeros void between seek and file size
                        if (seek > blk->size) {
                                u32_t zeros = seek - blk->size;
                                u32_t wrpos = blk->size;
                                u32_t block_address = (node_get_blkaddr(hdl, file->node_num) + 1) + (wrpos / BLOCK_SIZE);

                                while (!err and (zeros > 0)) {
                                        u32_t bytes = min(zeros, BLOCK_SIZE - (wrpos % BLOCK_SIZE));

                                        if (wrpos % BLOCK_SIZE != 0) {
                                                err = block_read(hdl, block_address, hdl->block[1], 1);
                                        }

                                        if (!err) {
                                                memset(&hdl->block[1][(wrpos % BLOCK_SIZE)], 0, bytes);
                                                err = block_write(hdl, block_address, hdl->block[1], 1);
                                        }

                                        if (!err) {
                                                *wrctr += bytes;
                                                zeros -= bytes;
                                                wrpos += bytes;
                                                block_address++;
                                        }
                                }
                        }

                        // write data buffer
                        u32_t file_blocks    = (file->node_num == (hdl->total_files - 1)) ? hdl->last_file_node_blocks : hdl->file_node_blocks;
                        u64_t max_file_size  = cast(u64_t, file_blocks - 1) * BLOCK_SIZE;
                        u32_t bytes_to_write = min(max_file_size - seek, count);
                        u32_t block_address  = (node_get_blkaddr(hdl, file->node_num) + 1) + (seek / BLOCK_SIZE);

                        while (!err and (bytes_to_write > 0)) {

                                u32_t blocks    = 0;
                                u32_t bytes     = 0;
                                const u8_t *buf = NULL;

                                if ((seek % BLOCK_SIZE == 0) and (bytes_to_write >= BLOCK_SIZE)) {
                                        blocks = bytes_to_write / BLOCK_SIZE;
                                        bytes  = blocks * BLOCK_SIZE;
                                        buf    = src;

                                } else {
                                        blocks = 1;
                                        bytes  = BLOCK_SIZE - (seek % BLOCK_SIZE);
                                        bytes  = min(bytes, bytes_to_write);
                                        buf    = hdl->block[1];

                                        err = block_read(hdl, block_address, hdl->block[1], blocks);
                                        if (!err) {
                                                memcpy(&hdl->block[1][(seek % BLOCK_SIZE)], src, bytes);
                                        }
                                }

                                if (!err) {
                                        err = block_write(hdl, block_address, buf, blocks);
                                }

                                if (!err) {
                                        block_address  += blocks;
                                        bytes_to_write -= bytes;
                                        seek           += bytes;
                                        src            += bytes;
                                        *wrctr         += bytes;
                                }
                        }

                        if (*wrctr > 0) {
                                time_t now = 0;
                                sys_gettime(&now);

                                blk->mtime = now;
                                blk->size  = max(blk->size, seek);
                                err = file_block_write(hdl, file->node_num, blk);
                        }
                }

                sys_mutex_unlock(hdl->mtx);
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
 * @param[out]          *rdctr                  number of read bytes
 * @param[in ]           fattr                  file attributes
 *
 * @return One of errno value (errno.h).
 */
//==============================================================================
API_FS_READ(flatfs,
            void            *fs_handle,
            void            *fhdl,
            u8_t            *dst,
            size_t           count,
            fpos_t          *fpos,
            size_t          *rdctr,
            struct vfs_fattr fattr)
{
        UNUSED_ARG1(fattr);

        flatfs_t *hdl  = fs_handle;
        file_t   *file = fhdl;

        int err = sys_mutex_lock(hdl->mtx, MTX_TIMEOUT);
        if (!err) {

                u32_t seek = *fpos;
                *rdctr = 0;

                blk_file_t *blk = cast(blk_file_t*, hdl->block[0]);
                err = file_block_read(hdl, file->node_num, blk);
                if (!err and (seek < blk->size)) {

                        u32_t bytes_to_read = min(blk->size - seek, count);
                        u32_t block_address = (node_get_blkaddr(hdl, file->node_num) + 1) + (seek / BLOCK_SIZE);

                        while (!err and (bytes_to_read > 0)) {

                                u32_t blocks = 0;
                                u32_t bytes  = 0;

                                if ((seek % BLOCK_SIZE == 0) and (bytes_to_read >= BLOCK_SIZE)) {

                                        blocks = bytes_to_read / BLOCK_SIZE;
                                        bytes  = blocks * BLOCK_SIZE;

                                        err = block_read(hdl, block_address, dst, blocks);

                                } else {
                                        blocks = 1;
                                        bytes  = BLOCK_SIZE - (seek % BLOCK_SIZE);
                                        bytes  = min(bytes, bytes_to_read);

                                        err = block_read(hdl, block_address, hdl->block[1], blocks);
                                        if (!err) {
                                                memcpy(dst, &hdl->block[1][(seek % BLOCK_SIZE)], bytes);
                                        }
                                }

                                if (!err) {
                                        block_address += blocks;
                                        bytes_to_read -= bytes;
                                        seek          += bytes;
                                        dst           += bytes;
                                        *rdctr        += bytes;
                                }
                        }
                }

                sys_mutex_unlock(hdl->mtx);
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
API_FS_IOCTL(flatfs, void *fs_handle, void *fhdl, int request, void *arg)
{
        UNUSED_ARG4(fs_handle, fhdl, request, arg);
        return EBADRQC;
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
API_FS_FLUSH(flatfs, void *fs_handle, void *fhdl)
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
API_FS_FSTAT(flatfs, void *fs_handle, void *fhdl, struct stat *stat)
{
        flatfs_t *hdl = fs_handle;

        int err = sys_mutex_lock(hdl->mtx, MTX_TIMEOUT);
        if (!err) {
                file_t *file = fhdl;

                blk_file_t *blk = cast(blk_file_t*, hdl->block[0]);
                err = file_block_read(hdl, file->node_num, blk);
                if (!err) {
                        stat->st_ctime = blk->ctime;
                        stat->st_mtime = blk->mtime;
                        stat->st_dev   = 0;
                        stat->st_gid   = blk->gid;
                        stat->st_uid   = blk->uid;
                        stat->st_mode  = blk->mode;
                        stat->st_size  = blk->size;
                }

                sys_mutex_unlock(hdl->mtx);
        }

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
API_FS_STAT(flatfs, void *fs_handle, const char *path, struct stat *stat)
{
        flatfs_t *hdl = fs_handle;

        int err = sys_mutex_lock(hdl->mtx, MTX_TIMEOUT);
        if (!err) {
                blk_file_t *blk = cast(blk_file_t*, hdl->block[0]);
                u32_t node_num;
                err = node_find(hdl, path, MAX_FILE_NAME_LEN, FIND_ANY_NODE, blk, &node_num);
                if (!err) {
                        stat->st_ctime = blk->ctime;
                        stat->st_mtime = blk->mtime;
                        stat->st_dev   = 0;
                        stat->st_gid   = blk->gid;
                        stat->st_uid   = blk->uid;
                        stat->st_mode  = blk->mode;
                        stat->st_size  = blk->size;
                }

                sys_mutex_unlock(hdl->mtx);
        }

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
API_FS_STATFS(flatfs, void *fs_handle, struct statfs *statfs)
{
        flatfs_t *hdl = fs_handle;

        statfs->f_bsize  = BLOCK_SIZE;
        statfs->f_blocks = hdl->total_volume_blocks;
        statfs->f_bfree  = hdl->total_volume_blocks - hdl->total_files - 3; /* FS + 2*BMP */
        statfs->f_ffree  = hdl->total_files;
        statfs->f_files  = hdl->total_files;
        statfs->f_type   = SYS_FS_TYPE__SOLID;
        statfs->f_fsname = "flatfs";

        int err = sys_mutex_lock(hdl->mtx, MTX_TIMEOUT);
        if (!err) {

                foreach_file_node(hdl) {
                        if (hdl->bitmap.word[i] & (1 << bit)) {

                                blk_file_t *blk = cast(blk_file_t*, hdl->block[0]);
                                err = file_block_read(hdl, node, blk);
                                if (!err) {
                                        statfs->f_ffree--;
                                        statfs->f_bfree -= CEILING(blk->size, BLOCK_SIZE);

                                } else if (err == EILSEQ) {
                                        DBG("incoherent node %u in block %u",
                                            node, node_get_blkaddr(hdl, node));
                                } else {
                                        goto finish;
                                }
                        }
                }

                finish:
                sys_mutex_unlock(hdl->mtx);
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
API_FS_MKDIR(flatfs, void *fs_handle, const char *path, mode_t mode)
{
        flatfs_t *hdl = fs_handle;

        int err = sys_mutex_lock(hdl->mtx, MTX_TIMEOUT);
        if (!err) {
                blk_file_t *blk = cast(blk_file_t*, hdl->block[0]);
                u32_t node_num;

                /*
                 * Try to open directory that should contains selected dir
                 */
                sys_strlcpy(hdl->tmp_path, path, sizeof(hdl->tmp_path));
                char *slash = strrchr(hdl->tmp_path, '/');
                if (slash) {
                        *slash = '\0';

                        if (not isstrempty(hdl->tmp_path)) {
                                err = node_find(hdl, hdl->tmp_path, MAX_FILE_NAME_LEN,
                                        S_IFDIR, blk, &node_num);
                                if (err) {
                                        goto finish;
                                }
                        } else {
                                // root dir always exists (without any node)
                        }

                } else {
                        err = ENOENT;
                        goto finish;
                }

                /*
                 * Try to open path as is to check if object exists
                 */
                err = node_find(hdl, path, MAX_FILE_NAME_LEN, FIND_ANY_NODE, blk, &node_num);

                if (err == ENOENT) {
                        err = node_create(hdl, path, mode | S_IFDIR, blk, &node_num);
                }

                finish:
                sys_mutex_unlock(hdl->mtx);
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
API_FS_MKFIFO(flatfs, void *fs_handle, const char *path, mode_t mode)
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
API_FS_MKNOD(flatfs, void *fs_handle, const char *path, const dev_t dev)
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
API_FS_OPENDIR(flatfs, void *fs_handle, const char *path, kdir_t *dir)
{
        flatfs_t *hdl = fs_handle;

        int err = sys_mutex_lock(hdl->mtx, MTX_TIMEOUT);
        if (not err) {

                if (not isstreq(path, "/")) {
                        sys_strlcpy(hdl->tmp_path, path, sizeof(hdl->tmp_path));
                        LAST_CHARACTER(hdl->tmp_path) = '\0';

                        blk_file_t *blk = cast(blk_file_t*, hdl->block[0]);
                        u32_t node_num;
                        err = node_find(hdl, hdl->tmp_path, MAX_FILE_NAME_LEN, S_IFDIR, blk, &node_num);
                }

                const char *dir_name = path;
                size_t dir_name_len = strlen(path);
                llist_t *list = NULL;

                if (not err) {
                        err = sys_llist_create(readdir_dirent_llist_cmp, NULL, &list);
                }

                if (not err) {
                        dir->d_hdl = list;

                        foreach_file_node(hdl) {

                                u32_t mask = (1 << bit);
                                if ((hdl->bitmap.word[i] & mask) == 0) {
                                        continue;
                                }

                                DBG("diropen() node: %lu", node);

                                blk_file_t *blk = cast(blk_file_t*, hdl->block[0]);
                                err = file_block_read(hdl, node, blk);
                                if (err) {
                                        goto finish;
                                }

                                if (strncmp(blk->name, dir_name, dir_name_len) != 0) {
                                        continue;
                                }

                                sys_strlcpy(hdl->tmp_path, &blk->name[dir_name_len], sizeof(hdl->tmp_path));
                                char *slash = strchr(hdl->tmp_path, '/');
                                if (slash) *slash = '\0';

                                readdir_dirent_t *entry;
                                size_t name_sz  = strsize(hdl->tmp_path);
                                size_t entry_sz = sizeof(*entry) + name_sz;

                                err = sys_zalloc(entry_sz, cast(void**, &entry));
                                if (err) {
                                        goto finish;
                                }

                                entry->dev = 0;
                                entry->mode = blk->mode;
                                entry->size = blk->size;
                                sys_strlcpy(entry->name, hdl->tmp_path, name_sz);

                                err = sys_llist_push_back(list, entry) ? 0 : ENOMEM;
                                if (not err) {
                                        sys_llist_unique(list);
                                }
                        }

                        finish:
                        if (not err) {
                                dir->d_seek = 0;
                                dir->d_items = sys_llist_size(list);
                                dir->dirent.d_name = NULL;
                        } else {
                                sys_llist_destroy(dir->d_hdl);
                        }
                }

                sys_mutex_unlock(hdl->mtx);
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
API_FS_CLOSEDIR(flatfs, void *fs_handle, kdir_t *dir)
{
        UNUSED_ARG2(fs_handle, dir);

        if (dir->dirent.d_name) {
                sys_llist_destroy(dir->d_hdl);
        }

        return ESUCC;
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
API_FS_READDIR(flatfs, void *fs_handle, kdir_t *dir)
{
        flatfs_t *hdl = fs_handle;

        int err = sys_mutex_lock(hdl->mtx, MTX_TIMEOUT);
        if (!err) {
                err = ENOENT;

                readdir_dirent_t *entry = sys_llist_at(dir->d_hdl, dir->d_seek);
                if (entry) {
                        dir->dirent.d_name = entry->name;
                        dir->dirent.dev    = entry->dev;
                        dir->dirent.mode   = entry->mode;
                        dir->dirent.size   = entry->size;
                        dir->d_seek++;
                        err = 0;
                }

                sys_mutex_unlock(hdl->mtx);
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
API_FS_REMOVE(flatfs, void *fs_handle, const char *path)
{
        flatfs_t *hdl = fs_handle;

        if (hdl->read_only) {
                return EROFS;
        }

        int err = sys_mutex_lock(hdl->mtx, MTX_TIMEOUT);
        if (!err) {

                blk_file_t *file = cast(blk_file_t*, hdl->block[0]);
                u32_t df_node_num = 0;

                err = node_find(hdl, path, MAX_FILE_NAME_LEN, FIND_ANY_NODE, file, &df_node_num);
                if (not err) {
                        if (S_ISDIR(file->mode)) {

                                sys_strlcpy(hdl->tmp_path, path, sizeof(hdl->tmp_path));
                                sys_strlcat(hdl->tmp_path, "/", sizeof(hdl->tmp_path));

                                size_t dir_name_len = strlen(hdl->tmp_path);

                                foreach_used_file_node(hdl) {

                                        blk_file_t *blk = cast(blk_file_t*, hdl->block[0]);
                                        err = file_block_read(hdl, node, blk);
                                        if (!err) {
                                                if (strncmp(blk->name, hdl->tmp_path, dir_name_len) == 0) {
                                                        err = node_remove(hdl, node);
                                                }
                                        }

                                        if (err) {
                                                goto finish;
                                        }
                                }
                        }

                        if (not err) {
                                err = node_remove(hdl, df_node_num);
                        }
                }

                finish:
                sys_mutex_unlock(hdl->mtx);
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
API_FS_RENAME(flatfs, void *fs_handle, const char *old_name, const char *new_name)
{
        flatfs_t *hdl = fs_handle;

        if (hdl->read_only) {
                return EROFS;
        }

        int err = sys_mutex_lock(hdl->mtx, MTX_TIMEOUT);
        if (!err) {
                blk_file_t *file = cast(blk_file_t*, hdl->block[0]);
                u32_t node_num;
                err = node_find(hdl, new_name, MAX_FILE_NAME_LEN, FIND_ANY_NODE,
                        file, &node_num);

                if (err == ENOENT) {

                        u32_t node;
                        err = node_find(hdl, old_name, MAX_FILE_NAME_LEN,
                                FIND_ANY_NODE, file, &node);

                        if (!err) {
                                sys_strlcpy(file->name, new_name, MAX_FILE_NAME_LEN);
                                err = file_block_write(hdl, node, file);
                        }

                } else {
                        err = EEXIST;
                }

                sys_mutex_unlock(hdl->mtx);
        }

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
API_FS_CHMOD(flatfs, void *fs_handle, const char *path, mode_t mode)
{
        flatfs_t *hdl = fs_handle;

        if (hdl->read_only) {
                return EROFS;
        }

        int err = sys_mutex_lock(hdl->mtx, MTX_TIMEOUT);
        if (!err) {
                blk_file_t *file = cast(blk_file_t*, hdl->block[0]);
                u32_t node;

                err = node_find(hdl, path, MAX_FILE_NAME_LEN, FIND_ANY_NODE, file, &node);
                if (!err) {
                        file->mode = (file->mode & S_IPMT) | (mode & S_IPMT);
                        err = file_block_write(hdl, node, file);
                }

                sys_mutex_unlock(hdl->mtx);
        }

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
API_FS_CHOWN(flatfs, void *fs_handle, const char *path, uid_t owner, gid_t group)
{
        flatfs_t *hdl = fs_handle;

        if (hdl->read_only) {
                return EROFS;
        }

        int err = sys_mutex_lock(hdl->mtx, MTX_TIMEOUT);
        if (!err) {
                blk_file_t *file = cast(blk_file_t*, hdl->block[0]);
                u32_t node;

                err = node_find(hdl, path, MAX_FILE_NAME_LEN, FIND_ANY_NODE, file, &node);
                if (!err) {
                        file->uid = owner;
                        file->gid = group;
                        err = file_block_write(hdl, node, file);
                }

                sys_mutex_unlock(hdl->mtx);
        }

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
API_FS_SYNC(flatfs, void *fs_handle)
{
#if __FLATFS_INDEX_SIZE__ > 0
        /*
         * Decrease index cache usage points.
         */
        flatfs_t *hdl = fs_handle;

        int err = sys_mutex_lock(hdl->mtx, MTX_TIMEOUT);
        if (!err) {
                for (size_t i = 0; i < ARRAY_SIZE(hdl->path_index); i++) {

                        path_index_t *index = &hdl->path_index[i];

                        if (not isstrempty(index->blk.name)) {
                                if (hdl->path_index[i].hit > (INT32_MIN + __OS_SYSTEM_CACHE_SYNC_PERIOD__)) {
                                        hdl->path_index[i].hit -= __OS_SYSTEM_CACHE_SYNC_PERIOD__;
                                }
                        }
                }

                sys_mutex_unlock(hdl->mtx);
        }
#else
        UNUSED_ARG1(fs_handle);
#endif

        return ESUCC;
}

//==============================================================================
/**
 * @brief  Dir entry compare functor.
 *
 * @param  a            entry a
 * @param  b            entry b
 *
 * @return Compare result.
 */
//==============================================================================
static int readdir_dirent_llist_cmp(const void *a, const void *b)
{
        readdir_dirent_t *entry_a = const_cast(a);
        readdir_dirent_t *entry_b = const_cast(b);

        size_t n = strcmp(entry_a->name, entry_b->name);

        if (n == 0) {
                if (S_ISDIR(entry_a->mode)) {
                        entry_b->mode = entry_a->mode;
                } else if (S_ISDIR(entry_b->mode)) {
                        entry_a->mode = entry_b->mode;
                }
        }

        return n;
}

//==============================================================================
/**
 * @brief  Calculate CRC32.
 *
 * @param  buf          buffer
 * @param  buflen       buffer length
 * @param  init         initial value ()
 *
 * @return CRC32 result.
 */
//==============================================================================
static u32_t calc_crc32(const void *buf, size_t buflen, u32_t init)
{
        const u8_t *b = buf;

        u32_t crc = init;
        while (buflen--) {
                crc = (crc << 8) ^ CRC32_TAB[((crc >> 24) ^ *b) & 255];
                b++;
        }

        return crc;
}

//==============================================================================
/**
 * @brief  Function read selected amount of blocks from disc.
 *
 * @param  hdl          file system handle
 * @param  address      block start address
 * @param  dst          destination buffer
 * @param  blocks       number of blocks to read
 *
 * @return One of errno value (errno.h).
 */
//==============================================================================
static int block_read(flatfs_t *hdl, u32_t address, void *dst, size_t blocks)
{
        int err = sys_fseek(hdl->disc, address * BLOCK_SIZE, VFS_SEEK_SET);
        if (!err) {
                size_t rdctr;
                err = sys_fread(dst, BLOCK_SIZE * blocks, &rdctr, hdl->disc);
                if (err) {
                        DBG("block_read: disc read error %d", err);
                }
        } else {
                DBG("block_read: fseek error %d", err);
        }

        return err;
}

//==============================================================================
/**
 * @brief  Function write selected amount of blocks to disc.
 *
 * @param  hdl          file system handle
 * @param  address      block start address
 * @param  dst          destination buffer
 * @param  blocks       number of blocks to read
 *
 * @return One of errno value (errno.h).
 */
//==============================================================================
static int block_write(flatfs_t *hdl, u32_t address, const void *src, size_t blocks)
{
        if (hdl->read_only) {
                return EROFS;
        }

        int err = sys_fseek(hdl->disc, address * BLOCK_SIZE, VFS_SEEK_SET);
        if (!err) {
                size_t wrctr;
                err = sys_fwrite(src, BLOCK_SIZE * blocks, &wrctr, hdl->disc);
                if (err) {
                        DBG("block_write: disc write error %d", err);
                }
        } else {
                DBG("block_write: fseek error %d", err);
        }

        return err;
}

//==============================================================================
/**
 * @brief  Function mount file system.
 *
 * @param  hdl          file system handle
 *
 * @return One of errno value (errno.h).
 */
//==============================================================================
static int mount(flatfs_t *hdl)
{
        int err = check_first_sector_coherency(hdl);
        if (!err) {
                err = check_bitmaps_coherency_and_repair(hdl);
                if (err) {
                        DBG("fs bitmap not coherent");
                }
        } else {
                DBG("fs descriptor not coherent");
        }

        return err;
}

//==============================================================================
/**
 * @brief  Check coherency of first FS sector.
 *
 * @param  hdl          file system handle
 *
 * @return One of errno value (errno.h).
 */
//==============================================================================
static int check_first_sector_coherency(flatfs_t *hdl)
{
        int err = block_read(hdl, 0, hdl->block[0], 1);
        if (!err) {
                blk_fs_t *blk = cast(blk_fs_t*, hdl->block[0]);

                if (blk->magic == MAGIC_BLOCK_FS) {

                        u32_t crc = calc_crc32(&blk->total_volume_blocks,
                                               BLOCK_SIZE - offsetof(blk_fs_t, total_volume_blocks),
                                               CRC32_INIT);

                        u32_t max_files = CEILING(blk->total_volume_blocks - 3 /* FS + 2xBMP */,
                                                  blk->file_node_blocks);

                        if (   (blk->crc == crc)
                           and (blk->total_files == max_files)
                           and (blk->total_files <= (BLOCK_SIZE * 8))
                           and (blk->total_volume_blocks >= 3)
                           and (blk->file_node_blocks > 1)
                           and (blk->file_node_blocks < blk->total_volume_blocks - 3)
                           and (blk->last_file_node_blocks <= blk->file_node_blocks)
                           and (blk->bitmap_address_primary < blk->total_volume_blocks)
                           and (blk->bitmap_address_secondary < blk->total_volume_blocks)
                           and (blk->bitmap_address_primary != blk->bitmap_address_secondary)
                           and (blk->first_file_node_address < blk->total_volume_blocks)
                           and (blk->first_file_node_address != blk->bitmap_address_primary)
                           and (blk->first_file_node_address != blk->bitmap_address_secondary) ) {

                                hdl->total_files = blk->total_files;
                                hdl->total_volume_blocks = blk->total_volume_blocks;
                                hdl->file_node_blocks = blk->file_node_blocks;
                                hdl->last_file_node_blocks = blk->last_file_node_blocks;
                                hdl->first_file_node_address = blk->first_file_node_address;
                                hdl->bitmap_address_primary = blk->bitmap_address_primary;
                                hdl->bitmap_address_secondary = blk->bitmap_address_secondary;

                                err = ESUCC;

                        } else {
                                DBG("file system parameters not coherent or CRC error");
                                err = EILSEQ;
                        }

                } else {
                        DBG("unknown file system signature");
                        err = EMEDIUMTYPE;
                }
        }

        return err;
}

//==============================================================================
/**
 * @brief  Function try to repair file system. Function load bitmap to handle.
 *
 * @param  hdl          file system handle
 *
 * @return One of errno value (errno.h).
 */
//==============================================================================
static int check_bitmaps_coherency_and_repair(flatfs_t *hdl)
{
        blk_bitmap_t *bmp_pri = &hdl->bitmap;
        blk_bitmap_t *bmp_sec = cast(blk_bitmap_t*, hdl->block[0]);
        blk_file_t   *file    = cast(blk_file_t*, hdl->block[1]);

        int err = block_read(hdl, hdl->bitmap_address_primary, bmp_pri, 1);
        if (!err) {
                err = block_read(hdl, hdl->bitmap_address_secondary, bmp_sec, 1);
        }

        if (!err) {
                bool bmp_pri_dirty = false;
                bool bmp_sec_dirty = false;

                for (size_t i = 0; i < ARRAY_SIZE(bmp_pri->word); i++) {

                        if ((bmp_pri->word[i] != bmp_sec->word[i]) or hdl->force_check) {

                                u32_t mask = 1;
                                for (size_t bit = 0; bit < sizeof(bmp_pri->word) * 8; bit++) {

                                        u32_t node = ((sizeof(bmp_pri->word) * 8) * i) + bit;

                                        u32_t address = node * hdl->file_node_blocks
                                                      + hdl->first_file_node_address;

                                        if ((bmp_pri->word[i] & mask) != (bmp_sec->word[i] & mask)) {


                                                err = block_read(hdl, address, file, 1);
                                                if (err) {
                                                        goto finish;
                                                }

                                                if (check_file_block_coherency(hdl, file) == 0) {

                                                        if ((bmp_pri->word[i] & mask) == 0) {
                                                                bmp_pri->word[i] |= mask;
                                                                bmp_pri_dirty = true;
                                                        }

                                                        if ((bmp_sec->word[i] & mask) == 0) {
                                                                bmp_sec->word[i] |= mask;
                                                                bmp_sec_dirty = true;
                                                        }
                                                } else {
                                                        if (bmp_pri->word[i] & mask) {
                                                                bmp_pri->word[i] &= ~mask;
                                                                bmp_pri_dirty = true;
                                                        }

                                                        if (bmp_sec->word[i] & mask) {
                                                                bmp_sec->word[i] &= ~mask;
                                                                bmp_sec_dirty = true;
                                                        }
                                                }

                                                MSG("fixed incoherent node %u in block %u",
                                                       node, address);

                                        } else if (hdl->force_check and (bmp_pri->word[i] & mask)) {

                                                err = block_read(hdl, address, file, 1);
                                                if (err) {
                                                        goto finish;
                                                }

                                                if (check_file_block_coherency(hdl, file) != 0) {

                                                        bmp_pri->word[i] &= ~mask;
                                                        bmp_pri_dirty = true;

                                                        bmp_sec->word[i] &= ~mask;
                                                        bmp_sec_dirty = true;

                                                        MSG("removed incoherent node %u in block %u",
                                                               node, address);
                                                }
                                        }

                                        mask <<= 1;
                                }
                        }
                }

                if (not hdl->read_only) {
                        if (!err and bmp_pri_dirty) {
                                err = block_write(hdl, hdl->bitmap_address_primary, bmp_pri, 1);
                        }

                        if (!err and bmp_sec_dirty) {
                                err = block_write(hdl, hdl->bitmap_address_secondary, bmp_sec, 1);
                        }
                }
        }

        finish:
        return err;
}

//==============================================================================
/**
 * @brief  Function check if file block is consistent.
 *
 * @param  hdl          file system handle
 *
 * @return One of errno value (errno.h).
 */
//==============================================================================
static int check_file_block_coherency(flatfs_t *hdl, const blk_file_t *blk)
{
        if (   (blk->magic == MAGIC_BLOCK_FILE)
           and (blk->crc == calc_blk_crc(blk))
           and (blk->size <= ((hdl->file_node_blocks - 1) * BLOCK_SIZE)) ) {

                return ESUCC;

        } else {
                DBG("incoherent file");
                return EILSEQ;
        }
}

//==============================================================================
/**
 * @brief  Calculate block address according selected node number.
 *
 * @param  hdl          file system handle
 * @param  node_num     node number
 *
 * @return Block address.
 */
//==============================================================================
static u32_t node_get_blkaddr(flatfs_t *hdl, u32_t node_num)
{
        return (node_num * hdl->file_node_blocks) + hdl->first_file_node_address;
}

//==============================================================================
/**
 * @brief  Find node by name.
 *
 * @param  hdl          file system handle
 * @param  name         node name
 * @param  name_len     node name length
 * @param  mode         mode
 * @param  blk          block buffer
 * @param  node_num     node number
 *
 * @return One of errno value (errno.h).
 */
//==============================================================================
static int node_find(flatfs_t *hdl, const char *name, size_t name_len, mode_t mode,
        blk_file_t *blk, u32_t *node_num)
{
        /*
         * Try find node in index
         */
        bool found = index_find(hdl, name, name_len, mode, blk, node_num);
        if (found) {
                return 0;
        }

        /*
         * Try to find path in storage
         */
        foreach_file_node(hdl) {

                if (hdl->bitmap.word[i] & (1 << bit)) {

                        *node_num = node;

                        int err = block_read(hdl, node_get_blkaddr(hdl, node), blk, 1);
                        if (err) {
                                return err;
                        }

                        if (check_file_block_coherency(hdl, blk) == 0) {
                                if (    (strncmp(name, blk->name, name_len) == 0)
                                    and (  ((blk->mode & S_IFMT) == mode)
                                        or (mode == FIND_ANY_NODE)) ) {

                                        DBG("found file '%s' in node %u in block %u",
                                            name, node, node_get_blkaddr(hdl, node));

                                        index_add(hdl, node, blk);

                                        return ESUCC;
                                }

                        } else {
                                DBG("incoherent node %u in block %u",
                                    node, node_get_blkaddr(hdl, node));
                        }
                }
        }

        return ENOENT;
}

//==============================================================================
/**
 * @brief  Create new node.
 *
 * @param  hdl          file system handle
 * @param  name         node name
 * @param  mode         file mode
 * @param  blk          block buffer
 * @param  node_num     node number
 *
 * @return One of errno value (errno.h).
 */
//==============================================================================
static int node_create(flatfs_t *hdl, const char *name, mode_t mode,
        blk_file_t *blk, u32_t *node_num)
{
        if (hdl->read_only) {
                return EROFS;
        }

        foreach_file_node(hdl) {

                u32_t mask = (1 << bit);

                if ((hdl->bitmap.word[i] & mask) == 0) {

                        *node_num = node;

                        time_t now = 0;
                        sys_gettime(&now);

                        memset(blk, 0, sizeof(blk_file_t));
                        blk->magic = MAGIC_BLOCK_FILE;
                        blk->ctime = now;
                        blk->mtime = now;
                        blk->mode  = 0666 | mode;
                        blk->uid   = 0;
                        blk->gid   = 0;
                        blk->size  = 0;
                        sys_strlcpy(blk->name, name, MAX_FILE_NAME_LEN);

                        hdl->bitmap.word[i] |= mask;
                        int err = block_write(hdl, hdl->bitmap_address_primary, &hdl->bitmap, 1);
                        if (!err) {
                                err = file_block_write(hdl, node, blk);
                                if (!err) {
                                        err = block_write(hdl, hdl->bitmap_address_secondary, &hdl->bitmap, 1);
                                }
                        }

                        if (!err) {
                                DBG("created new file '%s' in node %u in block %u",
                                    name, node, node_get_blkaddr(hdl, node));
                        } else {
                                DBG("node '%s' create error %d", name, err);
                        }

                        return err;
                }
        }

        return ENOSPC;
}

//==============================================================================
/**
 * @brief  Remove selected node number.
 *
 * @param  hdl          file system handle
 * @param  blk_addr     block address
 * @param  node_num     node number
 *
 * @return One of errno value (errno.h).
 */
//==============================================================================
static int node_remove(flatfs_t *hdl, u32_t node_num)
{
        index_remove(hdl, node_num);

        memset(hdl->block[0], 0xFF, sizeof(hdl->block[0]));

        set_bitmap_bit(hdl, node_num, false);

        int err = block_write(hdl, hdl->bitmap_address_primary, &hdl->bitmap, 1);
        if (!err) {
                err = block_write(hdl, node_get_blkaddr(hdl, node_num), hdl->block[0], 1);
                if (!err) {
                        err = block_write(hdl, hdl->bitmap_address_secondary, &hdl->bitmap, 1);
                }
        }

        return err;
}

//==============================================================================
/**
 * @brief  Set selected bit in bitmap.
 *
 * @param  hdl          file system handle
 * @param  node_num     node number
 * @param  set          true: set, false: clear
 */
//==============================================================================
static void set_bitmap_bit(flatfs_t *hdl, u32_t node_num, bool set)
{
        u32_t bits = ARRAY_ITEM_SIZE(hdl->bitmap.word) * 8;
        u32_t mask = (1 << (node_num % bits));

        if (set) {
                hdl->bitmap.word[node_num / bits] |= mask;
        } else {
                hdl->bitmap.word[node_num / bits] &= ~mask;
        }
}

//==============================================================================
/**
 * @brief  Calculate CRC32 of fs and file blocks.
 *
 * @param  blk          fs or file block
 *
 * @return CRC32.
 */
//==============================================================================
static u32_t calc_blk_crc(const void *blk)
{
        const blk_fs_t *fsblk = blk;

        return calc_crc32(&fsblk->total_volume_blocks,
                          BLOCK_SIZE - offsetof(blk_fs_t, total_volume_blocks),
                          CRC32_INIT);
}

//==============================================================================
/**
 * @brief  Read file block.
 *
 * @param  hdl          file system handle
 * @param  node_num     node number
 * @param  blk          destination block buffer
 *
 * @return One of errno value (errno.h).
 */
//==============================================================================
static int file_block_read(flatfs_t *hdl, u32_t node_num, blk_file_t *blk)
{
        bool found = index_get(hdl, node_num, blk);
        if (found) {
                return 0;
        }

        int err = block_read(hdl, node_get_blkaddr(hdl, node_num), blk, 1);
        if (!err) {
                err = check_file_block_coherency(hdl, blk);
        }

        return err;
}

//==============================================================================
/**
 * @brief  Write file block.
 *
 * @param  hdl          file system handle
 * @param  node_num     node number
 * @param  blk          source block buffer
 *
 * @return One of errno value (errno.h).
 */
//==============================================================================
static int file_block_write(flatfs_t *hdl, u32_t node_num, blk_file_t *blk)
{
        index_update(hdl, node_num, blk);

        blk->crc = calc_blk_crc(blk);
        return block_write(hdl, node_get_blkaddr(hdl, node_num), blk, 1);
}

//==============================================================================
/**
 * @brief  Find selected node (file) in index cache.
 *
 * @param  hdl          file system handle
 * @param  name         node name
 * @param  name_len     node name length
 * @param  mode         mode
 * @param  blk          block buffer
 * @param  node_num     node number
 *
 * @return True if node found, otherwise false.
 */
//==============================================================================
static bool index_find(flatfs_t *hdl, const char *name, size_t name_len, mode_t mode,
        blk_file_t *blk, u32_t *node_num)
{
#if __FLATFS_INDEX_SIZE__ > 0
        for (size_t i = 0; i < ARRAY_SIZE(hdl->path_index); i++) {

                path_index_t *index = &hdl->path_index[i];

                if (not isstrempty(index->blk.name)) {

                        if (    (strncmp(name, index->blk.name, name_len) == 0)
                            and (  (S_IFMT(index->blk.mode) == mode)
                                or (mode == FIND_ANY_NODE)) ) {

                                DBG("index hit [%li]: '%s' @ %lu", index->hit,
                                    name, index->node);

                                if (hdl->path_index[i].hit < INT32_MAX) {
                                        hdl->path_index[i].hit++;
                                }

                                memcpy(blk, &index->blk, sizeof(*blk));

                                *node_num = index->node;

                                return true;
                        }
                }
        }

#else
        UNUSED_ARG6(hdl, name, name_len, mode, blk, node_num);
#endif

        return false;
}

//==============================================================================
/**
 * @brief  Get selected node (file) from index cache.
 *
 * @param  hdl          file system handle
 * @param  node_num     node number
 * @param  blk          block buffer
 *
 * @return True if node found, otherwise false.
 */
//==============================================================================
static bool index_get(flatfs_t *hdl, u32_t node_num, blk_file_t *blk)
{
#if __FLATFS_INDEX_SIZE__ > 0
        for (size_t i = 0; i < ARRAY_SIZE(hdl->path_index); i++) {

                path_index_t *index = &hdl->path_index[i];

                if (index->node == node_num) {

                        DBG("index hit [%li]: '%s' @ %lu", index->hit,
                            index->blk.name, index->node);

                        if (hdl->path_index[i].hit < INT32_MAX) {
                                hdl->path_index[i].hit++;
                        }

                        memcpy(blk, &index->blk, sizeof(*blk));

                        return true;
                }
        }

#else
        UNUSED_ARG3(hdl, blk, node_num);
#endif

        return false;
}

//==============================================================================
/**
 * @brief  Create new index.
 *
 * @param  hdl          file system handle
 * @param  node_num     node number
 * @param  blk          block data
 */
//==============================================================================
static void index_add(flatfs_t *hdl, u32_t node_num, const blk_file_t *blk)
{
#if __FLATFS_INDEX_SIZE__ > 0
        i32_t min_hit = INT32_MAX;
        bool found = false;

        for (size_t i = 0; i < ARRAY_SIZE(hdl->path_index); i++) {

                path_index_t *index = &hdl->path_index[i];

                if (isstrempty(index->blk.name)) {
                        index->hit = INT32_MIN;
                        min_hit = INT32_MIN;
                        break;
                } else {
                        min_hit = min(min_hit, index->hit);
                }
        }

        for (size_t i = 0; not found and (i < ARRAY_SIZE(hdl->path_index)); i++) {
                path_index_t *index = &hdl->path_index[i];

                if (index->hit == min_hit) {
                        index->blk = *blk;
                        index->hit = 0;
                        index->node = node_num;
                        break;
                }
        }
#else
        UNUSED_ARG3(hdl, node_num, blk);
#endif
}

//==============================================================================
/**
 * @brief  Update index.
 *
 * @param  hdl          file system handle
 * @param  node_num     node number
 * @param  blk          block data
 */
//==============================================================================
static void index_update(flatfs_t *hdl, u32_t node_num, const blk_file_t *blk)
{
#if __FLATFS_INDEX_SIZE__ > 0
        for (size_t i = 0; i < ARRAY_SIZE(hdl->path_index); i++) {

                path_index_t *index = &hdl->path_index[i];

                if (index->node == node_num) {
                        index->blk = *blk;
                        break;
                }
        }
#else
        UNUSED_ARG3(hdl,  node_num, blk);
#endif
}

//==============================================================================
/**
 * @brief  Clear address in index.
 *
 * @param  hdl          file system handle
 * @param  node_num     node number
 */
//==============================================================================
static void index_remove(flatfs_t *hdl, u32_t node_num)
{
#if __FLATFS_INDEX_SIZE__ > 0
        for (size_t i = 0; i < ARRAY_SIZE(hdl->path_index); i++) {
                path_index_t *index = &hdl->path_index[i];
                if (index->node == node_num) {
                        memset(index, 0, sizeof(*index));
                        break;
                }
        }
#else
        UNUSED_ARG2(hdl, node_num);
#endif
}

/*==============================================================================
  End of file
==============================================================================*/
