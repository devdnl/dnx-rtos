/*==============================================================================
File    dd.c

Author  Daniel Zorychta

Brief   Disc destroyer

        Copyright (C) 3DGence 2021 Daniel Zorychta <daniel.zorychta@gmail.com>

==============================================================================*/

/*==============================================================================
  Include files
==============================================================================*/
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <dnx/misc.h>
#include <dnx/os.h>

/*==============================================================================
  Local macros
==============================================================================*/

/*==============================================================================
  Local object types
==============================================================================*/

/*==============================================================================
  Local function prototypes
==============================================================================*/
static void help(const char *name);

/*==============================================================================
  Local objects
==============================================================================*/
GLOBAL_VARIABLES_SECTION {
        const char *input_file;
        const char *output_file;
        size_t seek;
        size_t skip;
        size_t block_size;
        size_t block_count;
};

/*==============================================================================
  Exported objects
==============================================================================*/
PROGRAM_PARAMS(dd, STACK_DEPTH_LOW);

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
 */
//==============================================================================
int main(int argc, char *argv[])
{
        global->block_size = 512;

        if (argc == 1) {
                help(argv[0]);
                return EXIT_FAILURE;
        }

        for (int i = 1; i < argc; i++) {
                if (isstreq(argv[i], "--help") or (argc == 1)) {
                        help(argv[0]);
                        return EXIT_FAILURE;

                } else if (isstreqn(argv[i], "if=", 3)) {
                        global->input_file = argv[i] + 3;

                } else if (isstreqn(argv[i], "of=", 3)) {
                        global->output_file = argv[i] + 3;

                } else if (isstreqn(argv[i], "bs=", 3)) {
                        global->block_size = atoi(argv[i] + 3);

                } else if (isstreqn(argv[i], "count=", 6)) {
                        global->block_count = atoi(argv[i] + 6);

                } else if (isstreqn(argv[i], "seek=", 5)) {
                        global->seek = atoi(argv[i] + 5);

                } else if (isstreqn(argv[i], "skip=", 5)) {
                        global->skip = atoi(argv[i] + 5);

                } else {
                        printf("Unknown argument: '%s'\n", argv[i]);
                        return EXIT_FAILURE;
                }
        }

        u8_t *blk = malloc(global->block_size);
        if (blk) {
                FILE *in  = fopen(global->input_file, "r");
                if (!in) {
                        perror(global->input_file);
                }

                FILE *out = fopen(global->output_file, "r+");
                if (!out) {
                        out = fopen(global->output_file, "w");
                        if (!out) {
                                perror(global->output_file);
                        }
                }

                if (in and out) {
                        fseek(in, global->skip, SEEK_SET);
                        fseek(out, global->seek, SEEK_SET);

                        u64_t start_time = get_uptime_ms();
                        size_t s = 0;

                        while (global->block_count > 0) {
                                size_t n = fread(blk, 1, global->block_size, in);
                                if (ferror(in)) {
                                        perror(global->input_file);
                                        break;
                                }

                                if (n) {
                                        n = fwrite(blk, 1, n, out);
                                        if (ferror(out)) {
                                                perror(global->output_file);
                                                break;
                                        }
                                } else {
                                        break;
                                }

                                s += n;

                                global->block_count--;
                        }

                        u64_t stop_time = get_uptime_ms();

                        float time = (float)(stop_time - start_time) / 1000.0f;
                              time = max(time, 0.0001f);

                        printf("%u bytes copied, %0.3f s, %u KiB/s\n", s, time,
                               (uint)(((float)s / time) / 1024));
                }

                if (in) {
                        fclose(in);
                }

                if (out) {
                        fclose(out);
                }

                free(blk);
        } else {
                perror("dd");
        }

        return EXIT_SUCCESS;
}

//==============================================================================
/**
 * @brief  Help message.
 *
 * @param  name         program name
 */
//==============================================================================
static void help(const char *name)
{
        printf("Usage: %s [ARGUMENTS]...\n", name);
        printf("  if=FILE       input file\n");
        printf("  of=FILE       output file\n");
        printf("  bs=BYTES      block size\n");
        printf("  count=N       number of blocks\n");
        printf("  seek=N        output file seek blocks\n");
        printf("  skip=N        input file skip blocks\n");
}

/*==============================================================================
  End of file
==============================================================================*/

