/*=========================================================================*//**
File     dmesg.c

Author   Daniel Zorychta

Brief    System log reader

         Copyright (C) 2016 Daniel Zorychta <daniel.zorychta@gmail.com>

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

/*==============================================================================
  Local objects
==============================================================================*/
GLOBAL_VARIABLES_SECTION {
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
int_main(dmesg, STACK_DEPTH_LOW, int argc, char *argv[])
{
        bool clear = false;

        for (int i = 1; i < argc; i++) {
            if (isstreq(argv[i], "-h") || isstreq(argv[i], "--help")) {
                    printf("Usage: %s [options]\n\n", argv[0]);
                    puts("Options:");
                    puts("  -c, --clear     log clear");
                    puts("  -h, --help      this help");
                    return EXIT_FAILURE;
            }

            if (isstreq(argv[i], "-c") || isstreq(argv[i], "--help")) {
                    clear = true;
            }
        }

        if (clear) {
                syslog_clear();

        } else {
                char  str[128];
                u32_t ts = 0;
                while (syslog_read(str, sizeof(str), &ts)) {
                        printf("[%5d.%03d] %s\n", ts / 1000, ts % 1000, str);
                }
        }

        return EXIT_SUCCESS;
}

/*==============================================================================
  End of file
==============================================================================*/

