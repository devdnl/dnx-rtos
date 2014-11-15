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
#include <unistd.h>
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
 * @brief  Print help message
 * @param  name         program name
 * @return EXIT_SUCCESS
 */
//==============================================================================
static int print_help(const char *name)
{
        printf("Usage: %s [options]\n", name);
        puts(  "  -h, --help    this help");
        puts(  "  -l            signal list");
        puts(  "  -a, --add     add signal (-a [name] [mbox|val] [size] [r|w|rw|prv])");
        puts(  "  -d, --del     delete signal (-d [name])");

        return EXIT_SUCCESS;
}

//==============================================================================
/**
 * @brief  Print signal list
 * @param  None
 * @return EXIT_SUCCESS or EXIT_FAILURE
 */
//==============================================================================
static int print_signal_list()
{
        mbus_t *mbus = mbus_new();
        if (mbus) {
                int n = mbus_get_number_of_signals(mbus);

                printf("Number of signals: %d\n", n);

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
                                case MBUS_SIG_PERM__WRITE     : perm_str = "Write";      break;
                                case MBUS_SIG_PERM__READ_WRITE: perm_str = "Read-Write"; break;
                                default                       : perm_str = "Invalid";    break;
                                }

                                printf("  %s "FONT_COLOR_GRAY"[type: %s, size: %dB, mode: %s]"RESET_ATTRIBUTES"\n", info.name, type_str, info.size, perm_str);
                        } else {
                                break;
                        }
                }

                mbus_delete(mbus, false);
                return EXIT_SUCCESS;
        } else {
                fputs("Unable to create mbus connection!\n", stderr);
                return EXIT_FAILURE;
        }
}

//==============================================================================
/**
 * @brief  Add signal to base
 * @param  n            first argument where flag was found
 * @param  argv         argument table
 * @return EXIT_SUCCESS or EXIT_FAILURE
 */
//==============================================================================
static int add_signal(int n, char *argv[])
{
        char           *signal_name = NULL;
        int             signal_size = 0;
        mbus_sig_type_t signal_type = MBUS_SIG_TYPE__INVALID;
        mbus_sig_perm_t signal_perm = MBUS_SIG_PERM__INVALID;
        int             status      = EXIT_FAILURE;

        mbus_t *mbus = mbus_new();
        if (mbus) {
                if (argv[++n]) {
                        signal_name = argv[n];
                } else {
                        fputs("Invalid signal name\n", stderr);
                        goto exit;
                }

                if (argv[++n]) {
                        if (strcmp(argv[n], "mbox") == 0) {
                                signal_type = MBUS_SIG_TYPE__MBOX;
                        } else if (strcmp(argv[n], "val") == 0) {
                                signal_type = MBUS_SIG_TYPE__VALUE;
                        } else {
                                goto invalid_signal_type;
                        }
                } else {
                        invalid_signal_type:
                        fputs("Invalid signal type\n", stderr);
                        goto exit;
                }

                if (argv[++n]) {
                        sscanf(argv[n], "%d", &signal_size);
                        if (signal_size <= 0) {
                                goto invalid_signal_size;
                        }
                } else {
                        invalid_signal_size:
                        fputs("Invalid signal size\n", stderr);
                        goto exit;
                }

                if (argv[++n]) {
                        if (strcmp(argv[n], "r") == 0) {
                                signal_perm = MBUS_SIG_PERM__READ;
                        } else if (strcmp(argv[n], "w") == 0) {
                                signal_perm = MBUS_SIG_PERM__WRITE;
                        } else if (strcmp(argv[n], "rw") == 0) {
                               signal_perm = MBUS_SIG_PERM__READ_WRITE;
                        } else if (strcmp(argv[n], "prv") == 0) {
                                signal_perm = MBUS_SIG_PERM__PRIVATE;
                        } else {
                                goto invalid_signal_permissions;
                        }
                } else {
                        invalid_signal_permissions:
                        fputs("Invaid signal permissions\n", stderr);
                        goto exit;
                }

                if (mbus_signal_create(mbus, signal_name, signal_size, signal_type, signal_perm)) {
                        status = EXIT_SUCCESS;
                } else {
                        fprintf(stderr, "Signal not created (%d)\n", mbus_get_errno(mbus));
                }

                exit:
                mbus_delete(mbus, false);
        } else {
                fputs("Unable to create mbus connection!\n", stderr);
        }

        return status;
}

//==============================================================================
/**
 * @brief  Delete signal from base
 * @param  signal_name  name of signal to delete
 * @return EXIT_SUCCESS or EXIT_FAILURE
 */
//==============================================================================
static int delete_signal(char *signal_name)
{
        int status = EXIT_FAILURE;

        mbus_t *mbus = mbus_new();
        if (mbus) {
                if (signal_name) {
                        if (mbus_signal_force_delete(mbus, signal_name)) {
                                status = EXIT_SUCCESS;
                        } else {
                                fprintf(stderr, "Signal not deleted (%d)\n", mbus_get_errno(mbus));
                        }
                } else {
                        fputs("Invalid signal name\n", stderr);
                }

                mbus_delete(mbus, false);
        } else {
                fputs("Unable to create mbus connection!\n", stderr);
        }

        return status;
}

//==============================================================================
/**
 * @brief Program main function
 */
//==============================================================================
int_main(mbusd, STACK_DEPTH_LOW, int argc, char *argv[])
{
        // search parameters
        for (int i = 1; i < argc; i++) {
                if (strcmp(argv[i], "-l") == 0) {
                        return print_signal_list();
                }

                if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
                        return print_help(argv[0]);
                }

                if (strcmp(argv[i], "-a") == 0 || strcmp(argv[i], "--add") == 0) {
                        return add_signal(i, argv);
                }

                if (strcmp(argv[i], "-d") == 0 || strcmp(argv[i], "--del") == 0) {
                        return delete_signal(argv[i + 1]);
                }


                // TEST
                if (strcmp(argv[i], "a") == 0) {
                        mbus_t *mbus = mbus_new();
                        if (mbus) {
                                mbus_signal_create(mbus, "mbusd.Test1", sizeof(int), MBUS_SIG_TYPE__MBOX, MBUS_SIG_PERM__WRITE);
                                printf("A Status: %d\n", mbus_get_errno(mbus));

                                mbus_signal_create(mbus, "mbusd.network", sizeof(int), MBUS_SIG_TYPE__MBOX, MBUS_SIG_PERM__READ);
                                printf("A Status: %d\n", mbus_get_errno(mbus));

                                mbus_signal_create(mbus, "mbusd.val", sizeof(int), MBUS_SIG_TYPE__VALUE, MBUS_SIG_PERM__PRIVATE);
                                printf("A Status: %d\n", mbus_get_errno(mbus));

                                mbus_signal_create(mbus, "mbusd.env", sizeof(int), MBUS_SIG_TYPE__VALUE, MBUS_SIG_PERM__PRIVATE);
                                printf("A Status: %d\n", mbus_get_errno(mbus));

                                mbus_signal_create(mbus, "mbusd.var", sizeof(int), MBUS_SIG_TYPE__MBOX, MBUS_SIG_PERM__READ_WRITE);
                                printf("A Status: %d\n", mbus_get_errno(mbus));

                                mbus_delete(mbus, false);
                                printf("A Status: %d\n", mbus_get_errno(mbus));
                        }
                }

                if (strcmp(argv[i], "d") == 0) {
                        mbus_t *mbus = mbus_new();
                        if (mbus) {
                                mbus_signal_delete(mbus, "mbusd.val");
                                printf("D Status: %d\n", mbus_get_errno(mbus));

                                mbus_signal_delete(mbus, "mbusd.val2");
                                printf("D Status: %d\n", mbus_get_errno(mbus));

                                mbus_signal_delete(mbus, "mbusd.network");
                                printf("D Status: %d\n", mbus_get_errno(mbus));

                                mbus_delete(mbus, false);
                        }
                }

                if (strcmp(argv[i], "wr") == 0) {
                        mbus_t *mbus = mbus_new();
                        if (mbus) {
                                int data = get_time_ms();
                                mbus_signal_set(mbus, "mbusd.Test1", &data);
                                printf("Wr status: %d; %d\n", mbus_get_errno(mbus), data);

                                data = -get_time_ms();
                                mbus_signal_set(mbus, "mbusd.env", &data);
                                printf("Wr status: %d; %d\n", mbus_get_errno(mbus), data);

                                int e = mbus_signal_is_exist(mbus, "mbusd.Test1");
                                printf("Ex status: %d; %d\n", e, mbus_get_errno(mbus));

                                e = mbus_signal_is_exist(mbus, "mbusd.Test1fdsf");
                                printf("Ex status: %d; %d\n", e, mbus_get_errno(mbus));

                                mbus_delete(mbus, false);
                        }
                }

                if (strcmp(argv[i], "rd") == 0) {
                        mbus_t *mbus = mbus_new();
                        if (mbus) {
                                int data = -1;
                                mbus_signal_get(mbus, "mbusd.Test1", &data);
                                printf("Rd status: %d; %d\n", mbus_get_errno(mbus), data);

                                data = -1;
                                mbus_signal_get(mbus, "mbusd.env", &data);
                                printf("Rd status: %d; %d\n", mbus_get_errno(mbus), data);

                                mbus_delete(mbus, false);
                        }
                }

                if (strcmp(argv[i], "e") == 0) {
                        mbus_t *mbus = mbus_new();
                        if (mbus) {
                                bool status;
                                status = mbus_signal_create(mbus, "1", sizeof(bool), MBUS_SIG_TYPE__VALUE, MBUS_SIG_PERM__PRIVATE);
                                printf("e %d; %d\n", status, mbus_get_errno(mbus));

                                status = mbus_signal_create(mbus, "2", sizeof(bool), MBUS_SIG_TYPE__VALUE, MBUS_SIG_PERM__PRIVATE);
                                printf("e %d; %d\n", status, mbus_get_errno(mbus));

                                status = mbus_signal_create(mbus, "3", sizeof(mbus_errno_t), MBUS_SIG_TYPE__VALUE, MBUS_SIG_PERM__PRIVATE);
                                printf("e %d; %d\n", status, mbus_get_errno(mbus));

                                print_signal_list();

                                mbus_delete(mbus, true);

                                print_signal_list();
                        }
                }

                if (strcmp(argv[i], "lo") == 0) {
                        mbus_t *mbus = mbus_new();
                        if (mbus) {
                                while (true) {
                                        int var = -1;
                                        if (mbus_signal_get(mbus, "mbusd.var", &var)) {
                                                printf("Var: %d\n", var);
                                        } else {
                                                sleep_ms(500);
                                        }
                                }

                                mbus_delete(mbus, false);
                        }
                }

                if (strcmp(argv[i], "+") == 0) {
                        mbus_t *mbus = mbus_new();
                        if (mbus) {
                                int var = get_time_ms();
                                if (mbus_signal_set(mbus, "mbusd.var", &var)) {
                                        printf("Var: %d\n", var);
                                }
                                mbus_delete(mbus, false);
                        }
                }

                if (strcmp(argv[i], "f") == 0) {
                        mbus_t *mbus = mbus_new();
                        if (mbus) {
                                mbus_signal_force_delete(mbus, "mbusd.var");
                                printf("F Status: %d\n", mbus_get_errno(mbus));

                                mbus_signal_force_delete(mbus, "mbusd.env");
                                printf("F Status: %d\n", mbus_get_errno(mbus));

                                mbus_signal_force_delete(mbus, "mbusd.val");
                                printf("F Status: %d\n", mbus_get_errno(mbus));

                                mbus_signal_force_delete(mbus, "mbusd.network");
                                printf("F Status: %d\n", mbus_get_errno(mbus));

                                mbus_signal_force_delete(mbus, "mbusd.Test1");
                                printf("F Status: %d\n", mbus_get_errno(mbus));

                                mbus_delete(mbus, false);
                        }
                }
        }

        // print help if argument is give but is illegal
        if (argc > 1) {
                print_help(argv[0]);
                return EXIT_FAILURE;
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
