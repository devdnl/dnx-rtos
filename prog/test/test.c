/*=========================================================================*//**
@file    test.c

@author  Daniel Zorychta

@brief

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
#include "test.h"

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
GLOBAL_VARIABLES {
        int test;
};

/*==============================================================================
  Exported object definitions
==============================================================================*/
const uint prog_test_gs = sizeof(struct __global_vars__);

/*==============================================================================
  Function definitions
==============================================================================*/

//=============================================================================
/**
 * @brief
 */
//=============================================================================
int prog_test_main(ch_t *argv[], int argc)
{
        printf("test\n");

        global->test = 0;

        return 0;
}


#ifdef __cplusplus
}
#endif

/*==============================================================================
  End of file
==============================================================================*/
