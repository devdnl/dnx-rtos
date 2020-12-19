/*=========================================================================*//**
@file    storinit.c

@author  Daniel Zorychta

@brief   Storage initialization (SD cards, HDD, etc)

@note    Copyright (C) 2015 Daniel Zorychta <daniel.zorychta@gmail.com>

         This program is free software; you can redistribute it and/or modify
         it under the terms of the GNU General Public License as published by
         the Free Software Foundation and modified by the dnx RTOS exception.

         NOTE: The modification  to the GPL is  included to allow you to
               distribute a combined work that includes dnx RTOS without
               being obliged to provide the source  code for proprietary
               components outside of the dnx RTOS.

         The dnx RTOS  is  distributed  in the hope  that  it will be useful,
         but WITHOUT  ANY  WARRANTY;  without  even  the implied  warranty of
         MERCHANTABILITY  or  FITNESS  FOR  A  PARTICULAR  PURPOSE.  See  the
         GNU General Public License for more details.

         Full license text is available on the following file: doc/license.txt.


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
PROGRAM_PARAMS(storinit, STACK_DEPTH_LOW);

/*==============================================================================
  Function definitions
==============================================================================*/
//==============================================================================
/**
 * @brief Main function
 */
//==============================================================================
int main(int argc, char *argv[])
{
        int status = EXIT_FAILURE;

        if (argc == 1) {
                printf("Usage: %s [file]\n", argv[0]);
                return EXIT_FAILURE;
        }

        errno = 0;
        FILE *storage = fopen(argv[1], "r");
        if (storage) {
                if (ioctl(fileno(storage), IOCTL_STORAGE__INITIALIZE) == 0) {
                        if (ioctl(fileno(storage), IOCTL_STORAGE__READ_MBR) != 0) {
                                perror(argv[1]);
                        }

                } else {
                        perror(argv[1]);
                }

                fclose(storage);
        } else {
                perror(argv[1]);
        }

        return status;
}

/*==============================================================================
  End of file
==============================================================================*/
