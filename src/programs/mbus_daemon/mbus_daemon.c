/*=========================================================================*//**
@file    lsmod.c

@author  Daniel Zorychta

@brief   Module listing

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
static const size_t buf_len = 128;

GLOBAL_VARIABLES_SECTION_BEGIN
GLOBAL_VARIABLES_SECTION_END

/*==============================================================================
  Exported object definitions
==============================================================================*/

/*==============================================================================
  Function definitions
==============================================================================*/

//==============================================================================
/**
 * @brief Program main function
 */
//==============================================================================
PROGRAM_MAIN(mbus_daemon, int argc, char *argv[])
{
        errno = 0;

        if (argc == 2) {
                if (strcmp(argv[1], "-l") == 0) {
                        mbus_t *bus = mbus_bus_new();
                        if (bus) {
                                char *name = calloc(1, buf_len);
                                if (name) {
                                        uint slots = 0;
                                        mbus_bus_get_number_of_slots(bus, &slots);

                                        printf("Registered slots (%d):\n", slots);

                                        for (uint i = 0; i < slots; i++) {
                                                if (mbus_bus_get_slot_name(bus, i, name, buf_len) == MBUS_STATUS_SUCCESS) {
                                                        printf("  %d: %s\n", i + 1, name);
                                                }
                                        }

                                        free(name);
                                }

                                mbus_bus_delete(bus);
                        }
                } else  {
                        printf("Usage: %s [-l|-h|--help]\n", argv[0]);
                }

                return EXIT_SUCCESS;
        }


        mbus_status_t status = mbus_daemon();

        FILE *log = fopen("/tmp/mbus.log", "a+");
        if (log) {
                if (status == MBUS_STATUS_DAEMON_IS_RUNNING) {
                        fprintf(log, "%d: Daemon is already started.\n", get_time_ms());
                } else {
                        fprintf(log, "%d: Daemon start error: %s\n", get_time_ms(), strerror(errno));
                }

                fclose(log);
        }

        return EXIT_FAILURE;
}

/*==============================================================================
  End of file
==============================================================================*/
