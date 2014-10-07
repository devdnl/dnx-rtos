/*=========================================================================*//**
@file    card_init.c

@author  Daniel Zorychta

@brief   Initialize card

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
#include <sys/ioctl.h>
#include <errno.h>

/*==============================================================================
  Local symbolic constants/macros
==============================================================================*/
#define INIT_REQUEST                    IOCTL_SDSPI__INITIALIZE_CARD
#define MBR_READ_REQUEST                IOCTL_SDSPI__READ_MBR

/*==============================================================================
  Local types, enums definitions
==============================================================================*/

/*==============================================================================
  Local function prototypes
==============================================================================*/

/*==============================================================================
  Local object definitions
==============================================================================*/
GLOBAL_VARIABLES {
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
int_main(card_init, STACK_DEPTH_LOW, int argc, char *argv[])
{
        int status = EXIT_FAILURE;

        if (argc == 1) {
                printf("Usage: %s [file]\n", argv[0]);
                return EXIT_FAILURE;
        }

        errno = 0;
        FILE *sd = fopen(argv[1], "r");
        if (sd) {
                switch (ioctl(sd, INIT_REQUEST)) {
                case 1:
                        switch (ioctl(sd, MBR_READ_REQUEST)) {
                        case 1:
                                puts("Card initialized.");
                                status = EXIT_SUCCESS;
                                break;

                        case 0:
                                puts("Card initialized. MBR not exist.");
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

                fclose(sd);
        } else {
                perror(argv[1]);
        }

        return status;
}

/*==============================================================================
  End of file
==============================================================================*/
