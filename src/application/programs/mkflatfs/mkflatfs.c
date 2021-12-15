/*==============================================================================
File    mkflatfs.c

Author  Daniel Zorychta

Brief   Make flatfs in selected file/device

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
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <dnx/misc.h>

/*==============================================================================
  Local macros
==============================================================================*/
#define MAGIC_BLOCK_FS          0x54414C46
#define BLOCK_SIZE              512
#define CRC32_INIT              0xFFFFFFFF
#define MAX_FILE_SIZE_KiB       64
#define BYTES_IN_KiB            1024

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

/*==============================================================================
  Local function prototypes
==============================================================================*/
static u32_t calc_crc32(const void *buf, size_t buflen, u32_t init);
static u32_t calc_blk_crc(const void *blk);
static int block_write(u32_t address, const void *src, size_t blocks);
static int format(u64_t disc_size, u32_t max_file_size, bool fast_format);

/*==============================================================================
  Local objects
==============================================================================*/
GLOBAL_VARIABLES_SECTION {
        FILE *disc;
        const char *path;

        union {
                blk_fs_t blk;
                u8_t buf[BLOCK_SIZE];
        };
};

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
PROGRAM_PARAMS(mkflatfs, STACK_DEPTH_LOW);

/*==============================================================================
  External objects
==============================================================================*/

/*==============================================================================
  Function definitions
==============================================================================*/

//==============================================================================
/**
 * Main program function.
 *
 * Note: Please adjust stack size according to programs needs.
 *
 * @param argc      argument count
 * @param argv      arguments
 *
 * @return Exit status.
 */
//==============================================================================
int main(int argc, char *argv[])
{
        int exit = EXIT_FAILURE;

        if (argc < 3) {
                printf("Usage: %s <device> <max-file-size-KiB> [fast]\n", argv[0]);

        } else {
                global->path = argv[1];
                i32_t max_file_size = atoi(argv[2]);
                bool  fast_format = isstreq(argv[3], "fast");
                u64_t disc_size;

                global->disc = fopen(global->path, "r+");
                if (global->disc) {

                        struct stat bufstat;
                        if (fstat(global->disc, &bufstat) == 0) {

                                bool args_correct = true;

                                disc_size = bufstat.st_size;

                                if (disc_size % BLOCK_SIZE != 0) {
                                        fprintf(stderr, "Incorrect disc geometry!\n");
                                        args_correct = false;
                                }

                                if (max_file_size < MAX_FILE_SIZE_KiB) {
                                        fprintf(stderr, "Too small max file size! Min is %lu KiB.\n", MAX_FILE_SIZE_KiB);
                                        args_correct = false;
                                }

                                if ((max_file_size * BYTES_IN_KiB) >= ((i64_t)disc_size - (4 * (i64_t)BLOCK_SIZE))) {
                                        fprintf(stderr, "Too big max file size!\n");
                                        args_correct = false;
                                }

                                if (args_correct) {
                                        exit = format(disc_size / BLOCK_SIZE,
                                                      max_file_size * BYTES_IN_KiB / BLOCK_SIZE,
                                                      fast_format);

                                } else {
                                        fprintf(stderr, "Disc not formatted.\n");
                                }
                        }

                        fclose(global->disc);
                } else {
                        perror(global->path);
                }
        }

        return exit;
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
 * @brief  Function write selected amount of blocks to disc.
 *
 * @param  hdl          file system handle
 * @param  address      block address
 * @param  dst          destination buffer
 * @param  blocks       number of blocks to read
 *
 * @return One of errno value (errno.h).
 */
//==============================================================================
static int block_write(u32_t address, const void *src, size_t blocks)
{
        int err = fseek(global->disc, address * BLOCK_SIZE, SEEK_SET);
        if (!err) {
                errno = 0;
                size_t n = fwrite(src, BLOCK_SIZE, blocks, global->disc);
                err = (n == blocks) ? ESUCC : errno;
        }

        return err;
}

//==============================================================================
/**
 * @brief  Format disc.
 *
 * @param  disc_size            disc size in blocks
 * @param  max_file_size        max file size in blocks
 * @param  fast_format          fast format
 *
 * @return On success 0 is returned.
 */
//==============================================================================
static int format(u64_t disc_size, u32_t max_file_size, bool fast_format)
{
        max_file_size += 1;

        u32_t address_of_fs_block = 0;
        u32_t address_of_primary_bmp = 1;
        u32_t address_of_secondary_bmp = disc_size - 1;
        u32_t blocks_for_files = disc_size - 3;

        if (CEILING(blocks_for_files, max_file_size) > (BLOCK_SIZE * 8)) {
                max_file_size = blocks_for_files / (BLOCK_SIZE * 8);
                printf("Corrected maximal file size.\n");
        }

        u32_t total_files = CEILING(blocks_for_files, max_file_size);
        u32_t blocks_per_last_file = blocks_for_files - ((total_files - 1) * max_file_size);

        printf("Volume size      : %llu B\n", disc_size * BLOCK_SIZE);
        printf("Maximal file size: %llu B\n", cast(u64_t, max_file_size - 1) * BLOCK_SIZE);
        printf("Last file size   : %llu B\n", cast(u64_t, blocks_per_last_file - 1) * BLOCK_SIZE);
        printf("Maximal files    : %u\n", total_files);
        printf("Formatting...\n");

        memset(global->buf, 0, sizeof(global->buf));
        int err = block_write(address_of_secondary_bmp, global->buf, 1);
        if (!err) {

                int err = block_write(address_of_primary_bmp, global->buf, 1);
                if (!err) {

                        // full format
                        if (not fast_format) {
                                u32_t address = address_of_primary_bmp + 1;
                                for (u32_t blknum = 0; blknum < total_files; blknum++) {
                                        block_write(address, global->buf, 1);
                                        address += max_file_size;
                                }
                        }

                        // save file system descriptor
                        blk_fs_t *blk = &global->blk;
                        memset(blk, 0, sizeof(*blk));

                        blk->magic = MAGIC_BLOCK_FS;
                        blk->total_volume_blocks = disc_size;
                        blk->file_node_blocks = max_file_size;
                        blk->last_file_node_blocks = blocks_per_last_file;
                        blk->bitmap_address_primary = address_of_primary_bmp;
                        blk->bitmap_address_secondary = address_of_secondary_bmp;
                        blk->first_file_node_address = address_of_primary_bmp + 1;
                        blk->total_files = total_files;
                        blk->crc = calc_blk_crc(blk);

                        err = block_write(address_of_fs_block, blk, 1);
                }
        }

        if (!err) {
                printf("Done.\n");
        } else {
                perror(global->path);
        }

        return err;
}

/*==============================================================================
  End of file
==============================================================================*/
