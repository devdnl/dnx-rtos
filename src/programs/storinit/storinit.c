/*=========================================================================*//**
@file    storinit.c

@author  Daniel Zorychta

@brief   Storage initialization (SD cards, HDD, etc)

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
#include <sys/ioctl.h>
#include <errno.h>

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
 * @brief Main function
 */
//==============================================================================
int_main(storinit, STACK_DEPTH_LOW, int argc, char *argv[])
{
        int status = EXIT_FAILURE;

        if (argc == 1) {
                printf("Usage: %s [file]\n", argv[0]);
                return EXIT_FAILURE;
        }

        errno = 0;
        FILE *stor = fopen(argv[1], "r");
        if (stor) {
                switch (ioctl(stor, IOCTL_STORAGE__INITIALIZE)) {
                case 1:
                        switch (ioctl(stor, IOCTL_STORAGE__READ_MBR)) {
                        case 1:
                                puts("Storage initialized.");
                                status = EXIT_SUCCESS;
                                break;

                        case 0:
                                puts("Storage initialized. MBR not exist.");
                                status = EXIT_SUCCESS;
                                break;

                        case -1:
                                perror(argv[1]);
                                break;
                        }
                        break;

                default:
                        perror(argv[1]);
                        break;
                }

                fclose(stor);
        } else {
                perror(argv[1]);
        }

        return status;
}

/*==============================================================================
  End of file
==============================================================================*/
