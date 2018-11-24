/*==============================================================================
File    hexdump.c

Author  Daniel Zorychta

Brief   Hex presentation program.

        Copyright (C) 2017 Daniel Zorychta <daniel.zorychta@gmail.com>

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
#include <ctype.h>
#include <dnx/misc.h>

/*==============================================================================
  Local macros
==============================================================================*/
#define printline(buf, ...) snprintf(buf, sizeof(global->line) - (ptr - global->line), __VA_ARGS__)

/*==============================================================================
  Local object types
==============================================================================*/

/*==============================================================================
  Local function prototypes
==============================================================================*/

/*==============================================================================
  Local objects
==============================================================================*/
GLOBAL_VARIABLES_SECTION {
        u8_t buffer[16];
        char line[100];
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
 * Main program function.
 *
 * Note: Please adjust stack size according to programs needs.
 *
 * @param argc      argument count
 * @param argv      arguments
 */
//==============================================================================
int_main(hexdump, STACK_DEPTH_LOW, int argc, char *argv[])
{
        if (argc < 2) {
                printf("Usage: %s <file> [seek] [bytes]\n", argv[0]);
                return EXIT_FAILURE;
        }

        u32_t seek  = 0;
        u32_t bytes = UINT32_MAX;

        if (argc >= 3) {
                seek = strtol(argv[2], NULL, 0);
        }

        if (argc >= 4) {
                bytes = strtol(argv[3], NULL, 0);
        }

        FILE *f = fopen(argv[1], "r");
        if (f) {
                fseek(f, seek, SEEK_SET);

                do {
                        u32_t seek = ftell(f);
                        u32_t len  = min(sizeof(global->buffer), bytes);

                        size_t n = fread(global->buffer, 1, len, f);

                        char *ptr = global->line;

                        // file offset
                        ptr += printline(ptr, "%08x  ", seek);

                        // 0-7 byte
                        for (size_t i = 0; (i < 8) && (i < n); i++) {
                                ptr += printline(ptr, "%02x ", global->buffer[i]);
                        }

                        // delimiter
                        ptr += printline(ptr, " ");

                        // 8-16 byte
                        for (size_t i = 8; i < n; i++) {
                                ptr += printline(ptr, "%02x ", global->buffer[i]);
                        }

                        // print empty space if less than 16 bytes
                        int s = 16 - n;

                        while (s > 0) {
                                ptr += printline(ptr, "   ");
                                s--;
                        }

                        // characters
                        ptr += printline(ptr, " |");
                        for (size_t i = 0; i < n; i++) {
                                char c = global->buffer[i];
                                ptr += printline(ptr, "%c", isprint(c) ? c : ' ');
                        }

                        // end
                        ptr += printline(ptr, "|");

                        puts(global->line);

                        bytes -= n;

                } while ((bytes > 0) && !feof(f));

                fclose(f);

                return EXIT_SUCCESS;

        } else {
                perror(argv[1]);
                return EXIT_FAILURE;
        }
}

/*==============================================================================
  End of file
==============================================================================*/

