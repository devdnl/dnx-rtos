/*=========================================================================*//**
@file    uname.c

@author  Daniel Zorychta

@brief   Show system information

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
#include <dnx/os.h>

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
PROGRAM_MAIN(uname, STACK_DEPTH_MINIMAL, int argc, char *argv[])
{
        (void) argc;
        (void) argv;

        printf("%s %s, %s %s, %s\n",
               get_OS_name(), get_OS_version(),
               get_kernel_name(), get_kernel_version(),
               get_platform_name());

        return EXIT_SUCCESS;
}

/*==============================================================================
  End of file
==============================================================================*/
