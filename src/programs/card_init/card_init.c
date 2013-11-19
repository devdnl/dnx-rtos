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

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Include files
==============================================================================*/
#include <stdio.h>
#include <stdlib.h>
#include "system/ioctl.h"

/*==============================================================================
  Local symbolic constants/macros
==============================================================================*/
#define INIT_REQUEST                    SDSPI_IORQ_INITIALIZE_CARD

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
 * @brief Cat main function
 */
//==============================================================================
PROGRAM_MAIN(card_init, int argc, char *argv[])
{
        if (argc == 1) {
                printf("Usage: %s [file]\n", argv[0]);
                return EXIT_FAILURE;
        }

        FILE *sd = fopen(argv[1], "r");
        if (sd) {
                bool status = false;
                if (ioctl(sd, INIT_REQUEST, &status) != 0) {
                        perror(argv[1]);
                        return EXIT_FAILURE;
                }

                if (status == true) {
                        puts("Card initialized.");
                        return EXIT_SUCCESS;
                } else {
                        puts("Card not detected.");
                        return EXIT_FAILURE;
                }
        } else {
                perror(argv[1]);
        }

        return EXIT_FAILURE;
}

#ifdef __cplusplus
}
#endif

/*==============================================================================
  End of file
==============================================================================*/
