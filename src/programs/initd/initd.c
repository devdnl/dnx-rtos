/*=========================================================================*//**
@file    initd.c

@author  Daniel Zorychta

@brief   Initialization daemon

@note    Copyright (C) 2015 Daniel Zorychta <daniel.zorychta@gmail.com>

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
#include <sys/mount.h>
#include <sys/stat.h>
#include <dnx/misc.h>
#include <dnx/os.h>
#include <unistd.h>

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
        const char *str;
};

/*==============================================================================
  Exported object definitions
==============================================================================*/

/*==============================================================================
  Function definitions
==============================================================================*/

//==============================================================================
/**
 * @brief Program main function
 *
 * @param  argc         count of arguments
 * @param *argv[]       argument table
 *
 * @return program status
 */
//==============================================================================
int_main(initd, STACK_DEPTH_MEDIUM, int argc, char *argv[])
{
        UNUSED_ARG2(argc, argv);

        int result = 0;

        if (argc == 2 && strcmp(argv[1], "--child") == 0) {
                puts("Hello! I'm child of initd parent!");

                global->str = "Works!";

                int i = 0;
                while (true) {
                        printf("Sec: %d\n", i++);
                        sleep(1);
                }

        } else {
                result = mount("lfs", "", "/");
                result = mkdir("/dev", 0777);
                result = driver_init("gpio", "/dev/gpio");
                result = driver_init("afiom", NULL);
                result = driver_init("uart2", "/dev/ttyS0");
                result = driver_init("tty0", "/dev/tty0");

                result = syslog_enable("/dev/tty0");

                detect_kernel_panic(true);

                result = driver_init("tty1", "/dev/tty1");

                stdout = fopen("/dev/tty0", "w");

                printf("Hello world! I'm using syscalls!\n");


                puts("Starting child...");

                pid_t pid = 0;
                const process_attr_t attr = {
                       .cwd      = "/",
                       .p_stdin  = "/dev/tty1",
                       .p_stdout = "/dev/tty1",
                       .p_stderr = "/dev/tty1"
                };

                result = _process_create(&pid, &attr, "initd --child");

                printf("Result: %d\n", result);
        }

        return result;
}

/*==============================================================================
  End of file
==============================================================================*/
