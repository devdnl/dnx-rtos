/*=========================================================================*//**
File     system.c

Author   Daniel Zorychta

Brief    system() function implementation.

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
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <dnx/thread.h>

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
 * @brief Function executes specified command.
 *
 * The system() executes a command specified in <i>command</i> and returns after
 * the command has been completed.
 *
 * @param command       command to execute
 */
//==============================================================================
int system(const char *command)
{
        int status = -1;

        size_t len = strlen(__OS_SYSTEM_PROG__) + strlen(command) + 4;

        static const int CWDLEN = 256;
        char *cwd = malloc(CWDLEN);
        char *cmd = calloc(1, len);
        if (cmd && cwd) {
                strcpy(cmd, __OS_SYSTEM_PROG__);
                strcat(cmd, " '");
                strcat(cmd, command);
                strcat(cmd, "'");

                getcwd(cwd, CWDLEN);

                process_attr_t attr = {
                      .f_stdin  = stdin,
                      .f_stdout = stdout,
                      .f_stderr = stderr,
                      .p_stdin  = NULL,
                      .p_stdout = NULL,
                      .p_stderr = NULL,
                      .cwd      = cwd,
                      .priority = PRIORITY_NORMAL,
                      .detached = false
                };

                process_wait(process_create(command, &attr), &status, MAX_DELAY_MS);

                free(cmd);
                free(cwd);
        } else {
                if (cwd) {
                        free(cwd);
                }

                if (cmd) {
                        free(cmd);
                }
        }

        return status;
}

/*==============================================================================
  End of file
==============================================================================*/
