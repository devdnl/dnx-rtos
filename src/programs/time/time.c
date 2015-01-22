/*=========================================================================*//**
@file    time.c

@author  Daniel Zorychta

@brief   Check execution time of selected program

@note    Copyright (C) 2013 Daniel Zorychta <daniel.zorychta@gmail.com>

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
#include <errno.h>
#include <unistd.h>
#include <dnx/os.h>
#include <sys/types.h>

/*==============================================================================
  Local symbolic constants/macros
==============================================================================*/
#define CWD_LEN         80
#define CMD_LEN         100

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
char cwd[CWD_LEN];
char cmd[CMD_LEN];
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
PROGRAM_MAIN(time, STACK_DEPTH_VERY_LOW, int argc, char *argv[])
{
        if (argc == 1) {
                printf("Usage: %s [program]\n", argv[0]);
                return EXIT_SUCCESS;
        }

        getcwd(global->cwd, CWD_LEN);

        for (int i = 1; i < argc; i++) {
                if (strlen(argv[i]) + strlen(global->cmd) < CMD_LEN) {
                        strcat(global->cmd, argv[i]);
                        strcat(global->cmd, " ");
                } else {
                        break;
                }
        }

        errno = 0;

        u32_t start_time = get_time_ms();

        prog_t *prog = program_new(global->cmd, global->cwd, stdin, stdout, stderr);
        if (prog) {
                program_wait_for_close(prog, MAX_DELAY_MS);
                program_delete(prog);
        } else {
                perror(argv[1]);
        }

        u32_t stop_time = get_time_ms() - start_time;
        printf("\nreal\t%dm%d.%03ds\n", stop_time / 60000, (stop_time / 1000) % 60, stop_time % 1000);

        return EXIT_SUCCESS;
}

/*==============================================================================
  End of file
==============================================================================*/
