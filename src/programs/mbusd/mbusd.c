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
static void print_help(const char *name)
{
        printf("Usage: %d [options]\n", name);
        puts(  "  -h, --help    this help");
        puts(  "  -l            signal list");
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
        mbus_t *mbus = mbus_new();
        if (mbus) {
                printf(FONT_BOLD"NAME, TYPE, SIZE, PERMISSIONS"RESET_ATTRIBUTES"\n");
                int n = mbus_get_number_of_signals(mbus);
                for (int i = 0; i < n; i++) {
                        mbus_sig_info_t info;
                        if (mbus_get_signal_info(mbus, i, &info)) {
                                const char *type_str;
                                switch (info.type) {
                                case MBUS_SIG_TYPE__MBOX : type_str = "MBOX";    break;
                                case MBUS_SIG_TYPE__VALUE: type_str = "Value";   break;
                                default                  : type_str = "Invalid"; break;
                                }

                                const char *perm_str;
                                switch (info.permissions) {
                                case MBUS_SIG_PERM__PRIVATE   : perm_str = "Private";    break;
                                case MBUS_SIG_PERM__READ      : perm_str = "Read";       break;
                                case MBUS_SIG_PERM__READ_WRITE: perm_str = "Read-Write"; break;
                                default                       : perm_str = "Invalid";    break;
                                }

                                printf("%s, %s, %dB, %s\n", info.name, type_str, info.size, perm_str);
                        } else {
                                break;
                        }
                }

                mbus_delete(mbus);
        } else {
                perror("Unable to create mbus connection!");
                exit(EXIT_FAILURE);
        }
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

                // TEST
                if (strcmp(argv[i], "1") == 0) {
                        mbus_t *mbus = mbus_new();
                        if (mbus) {
                                mbus_signal_create(mbus, "Test1", sizeof(int), MBUS_SIG_TYPE__MBOX, MBUS_SIG_PERM__READ_WRITE);
                                printf("Status: %d\n", mbus_get_errno(mbus));

                                mbus_signal_create(mbus, "network", sizeof(int), MBUS_SIG_TYPE__MBOX, MBUS_SIG_PERM__READ);
                                printf("Status: %d\n", mbus_get_errno(mbus));

                                mbus_signal_create(mbus, "val", sizeof(int), MBUS_SIG_TYPE__VALUE, MBUS_SIG_PERM__PRIVATE);
                                printf("Status: %d\n", mbus_get_errno(mbus));

                                mbus_signal_create(mbus, "env", sizeof(int), MBUS_SIG_TYPE__VALUE, MBUS_SIG_PERM__PRIVATE);
                                printf("Status: %d\n", mbus_get_errno(mbus));

                                mbus_delete(mbus);
                        }
                }
        }

        // analyze parameters
        if (help) {
                print_help(argv[0]);
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
