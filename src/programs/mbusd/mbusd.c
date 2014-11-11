/*=========================================================================*//**
@file    mbusd.c

@author  Daniel Zorychta

@brief   mbus daemon

@note    Copyright (C) 2014 Daniel Zorychta <daniel.zorychta@gmail.com>

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
#include <string.h>
#include <errno.h>
#include <dnx/os.h>
#include "mbus.h"

/*==============================================================================
  Local symbolic constants/macros
==============================================================================*/

/*==============================================================================
  Local types, enums definitions
==============================================================================*/

/*==============================================================================
  Local function prototypes
==============================================================================*/

/*==============================================================================
  Local object definitions
==============================================================================*/
GLOBAL_VARIABLES_SECTION {
};

/*==============================================================================
  Exported object definitions
==============================================================================*/

/*==============================================================================
  Function definitions
==============================================================================*/
//==============================================================================
/**
 * @brief  ?
 * @param  ?
 * @return ?
 */
//==============================================================================
static void print_help()
{

}

//==============================================================================
/**
 * @brief  ?
 * @param  ?
 * @return ?
 */
//==============================================================================
static void print_signal_list()
{

}

//==============================================================================
/**
 * @brief Program main function
 */
//==============================================================================
int_main(mbusd, STACK_DEPTH_LOW, int argc, char *argv[])
{
        bool help = false, list = false;

        // search parameters
        for (int i = 1; i < argc; i++) {
                if (strcmp(argv[i], "-l") == 0) {
                        list = true;
                        continue;
                }

                if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
                        help = true;
                        continue;
                }
        }

        // analyze parameters
        if (help) {
                print_help();
                return EXIT_SUCCESS;

        } else if (list) {
                print_signal_list();
                return EXIT_SUCCESS;
        }

        // start daemon
        if (mbus_daemon() == MBUS_ERRNO__DAEMON_IS_ALREADY_STARTED) {
                FILE *f = fopen("/var/mbus.log", "a+");
                if (f) {
                        fprintf(f, "[%d] mbus is already started\n", get_time_ms());
                        fclose(f);
                }
        }

        return EXIT_FAILURE;
}

/*==============================================================================
  End of file
==============================================================================*/
