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
#include "lib/mbus/mbus.h"

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
        (void)argc;
        (void)argv;

        errno = 0;

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
