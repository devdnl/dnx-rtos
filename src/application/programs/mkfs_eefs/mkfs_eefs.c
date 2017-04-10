/*=========================================================================*//**
File     mkfs_eefs.c

Author   Daniel Zorychta

Brief    Format selected file by EEFS.

         Copyright (C) 2016 Daniel Zorychta <danzor87@gmail.com>

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
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>
#include <time.h>
#include <dnx/misc.h>
#include <sys/stat.h>

/*==============================================================================
  Local macros
==============================================================================*/
#define BLOCK_SIZE              128
#define BLOCKS_IN_BYTE          8
#define ENTRY_TYPE_NONE         0xFF

#define BLOCK_MAIN_MAGIC        0x53464545
#define BLOCK_DIR_MAGIC         0x30524944

/*==============================================================================
  Local object types
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
 * Main block definition (block 0).
 */
typedef struct PACKED {
        uint32_t magic;         //!< FS identifier ('EEFS')
        uint16_t blocks;        //!< number of memory blocks
        uint8_t  bitmap_blocks; //!< number of next bitmap blocks
        uint8_t  bitmap[121];   //!< empty block bitmap
} block_main_t;

/**
 * Bitmap block definition.
 */
typedef struct PACKED {
        uint8_t  map[128];      //!< bitmap
} block_bitmap_t;

/**
 * Single directory entry.
 */
typedef struct PACKED {
        uint16_t block_addr;    //!< block address with object
        uint8_t  type;          //!< entry type
        char     name[21];      //!< entry name
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
        uint16_t    entry_next; //!< next directory entries
        uint16_t    all_items;  //!< number of items in all chains
        uint8_t     _[8];       //!< reserved
        dir_entry_t entry[4];   //!< directory entry
} block_dir_t;

/*==============================================================================
  Local function prototypes
==============================================================================*/

/*==============================================================================
  Local objects
==============================================================================*/
GLOBAL_VARIABLES_SECTION {
        FILE  *file;
        size_t mem_size;

        union {
                block_main_t   main;
                block_bitmap_t bitmap;
                block_dir_t    dir;
        } block;
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
 * Show help message.
 *
 * @param  pname        program name.
 */
//==============================================================================
static void show_help(char *pname)
{
        printf("Usage: %s <file> [mem-size]\n\n", pname);
        puts("  file            file to format (device)");
        puts("  mem-size        memory size in bytes (none for automatic size)");
}

//==============================================================================
/**
 * Function write block by filling pages.
 *
 * @param block_pos     block position in memory.
 *
 * @return EXIT_SUCCESS or EXIT_FAILURE.
 */
//==============================================================================
static int write_block(uint16_t block_pos)
{
        fseek(global->file, block_pos * cast(fpos_t, BLOCK_SIZE), SEEK_SET);

        if (fwrite(&global->block, 1, BLOCK_SIZE, global->file) != BLOCK_SIZE) {

                perror("Block write");
                return EXIT_FAILURE;
        } else {
                return EXIT_SUCCESS;
        }
}

//==============================================================================
/**
 * Function allocate selected block in bitmap.
 *
 * @param  bitmap       bitmap array.
 * @param  block        block to allocate.
 */
//==============================================================================
static void alloc_block(uint8_t bitmap[], uint16_t block)
{
        bitmap[block / 8] &= ~(1 << (block % 8));
}

//==============================================================================
/**
 * Function format selected file according to given parameters.
 *
 * @return EXIT_SUCCESS or EXIT_FAILURE.
 */
//==============================================================================
static int mkfs(void)
{
        u16_t block_pos = 0;

        // calculate file system parameters
        uint32_t blocks = (uint32_t)global->mem_size / BLOCK_SIZE;

        int32_t bmpblks = blocks - (sizeof(global->block.main.bitmap) * BLOCKS_IN_BYTE);
        bmpblks         = bmpblks < 0 ? 0 : CEILING(bmpblks, BLOCK_SIZE * BLOCKS_IN_BYTE);

        // create main block
        memset(&global->block.main, 0xFF, sizeof(global->block));

        global->block.main.magic          = BLOCK_MAIN_MAGIC;
        global->block.main.blocks         = blocks;
        global->block.main.bitmap_blocks  = bmpblks;

        u16_t used_blocks = bmpblks + 2; // bitmap blocks + 1 main block + 1 root dir

        u8_t main_bitmap_blocks = min(sizeof(global->block.main.bitmap) * BLOCKS_IN_BYTE, used_blocks);

        used_blocks -= main_bitmap_blocks;

        for (u16_t block = 0; block < main_bitmap_blocks; block++) {
                alloc_block(global->block.main.bitmap, block);
        }

        if (write_block(block_pos++) != EXIT_SUCCESS) {
                return EXIT_FAILURE;
        }

        // create bitmap blocks (additional bitmaps, memory size > 120832B)
        memset(&global->block.bitmap, 0xFF, sizeof(global->block.bitmap));
        u16_t block_cnt = 0;

        while (used_blocks) {
                alloc_block(global->block.main.bitmap, block_cnt);
                block_cnt++;

                if (block_cnt > sizeof(global->block.bitmap) * BLOCKS_IN_BYTE) {
                        block_cnt = 0;

                        if (write_block(block_pos++) != EXIT_SUCCESS) {
                                return EXIT_FAILURE;
                        }

                        memset(&global->block.bitmap, 0xFF, sizeof(global->block.bitmap));
                }

                used_blocks--;
        }

        // create root
        time_t t = time(NULL);

        memset(&global->block.dir, 0xFF, sizeof(global->block.dir));
        global->block.dir.magic      = BLOCK_DIR_MAGIC;
        global->block.dir.ctime      = t;
        global->block.dir.mtime      = t;
        global->block.dir.gid        = getgid();
        global->block.dir.uid        = getuid();
        global->block.dir.mode       = 0777;
        global->block.dir.parent     = 0;
        global->block.dir.all_items  = 0;
        global->block.dir.entry_next = 0;

        return write_block(block_pos++);
}

//==============================================================================
/**
 * Main program function.
 *
 * @param argc      argument count
 * @param argv      arguments
 */
//==============================================================================
int_main(mkfs_eefs, STACK_DEPTH_LOW, int argc, char *argv[])
{
        if (argc < 2) {
                show_help(argv[0]);
        } else {
                if (  (argv[2] != NULL)
                   && (  (sscanf(argv[2], "%u", &global->mem_size) != 1)
                      || (global->mem_size == 0) ) ) {

                        fprintf(stderr, "Wrong memory size: %u\n", global->mem_size);
                        return EXIT_FAILURE;
                }

                global->file = fopen(argv[1], "r+");
                if (!global->file) {
                        perror(argv[1]);
                        return EXIT_FAILURE;

                } else {
                        struct stat buf;
                        memset(&buf, 0, sizeof(struct stat));
                        fstat(global->file, &buf);
                        global->mem_size = buf.st_size;

                        printf("Memory size set automatically to %d bytes.\n",
                               cast(uint, global->mem_size)
                        );
                }

                int err = mkfs();

                fclose(global->file);

                return err;
        }

        return EXIT_FAILURE;
}

/*==============================================================================
  End of file
==============================================================================*/
