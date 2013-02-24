/*=========================================================================*//**
@file    regprg.c

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
#include "regprg.h"
#include <string.h>

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
static const struct regprg_pdata pdata[] = {
        {.name          = "test",
         .main_function = prog_test_main,
         .globals_size  = prog_test_gs,
         .stack_deep    = PROG_TEST_STACK_DEEP},
};

/*==============================================================================
  Exported object definitions
==============================================================================*/

/*==============================================================================
  Function definitions
==============================================================================*/

//==============================================================================
/**
 * @brief Function returns pointer to all program data necessary to start program
 *
 * @param [in]  *name           program name
 * @param [out] *pdata          program data
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
stdRet_t regprg_get_program_data(ch_t *name, const struct regprg_pdata *prg_data)
{
        if (!prg_data || !name) {
                return STD_RET_ERROR;
        }

        for (uint i = 0; i < ARRAY_SIZE(pdata); i++) {
                if (strcmp(name, pdata[i].name) == 0) {
                        prg_data = &pdata[i];
                        return STD_RET_OK;
                }
        }

        return STD_RET_ERROR;
}

#ifdef __cplusplus
}
#endif

/*==============================================================================
  End of file
==============================================================================*/
