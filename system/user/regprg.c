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

#include "helloworld.h"
#include "top.h"
#include "terminal.h"
#include "cat.h"

/*==============================================================================
  Local symbolic constants/macros
==============================================================================*/
#define IMPORT_PROGRAM(name) \
        {.program_name  = #name,\
         .main_function = program_##name##_main,\
         .globals_size  = &prog_##name##_gs,\
         .stack_depth   = &prog_##name##_stack}

/*==============================================================================
  Local types, enums definitions
==============================================================================*/

/*==============================================================================
  Local function prototypes
==============================================================================*/

/*==============================================================================
  Local object definitions
==============================================================================*/
static const struct regprg_pdata prog_table[] = {
        IMPORT_PROGRAM(helloworld),
        IMPORT_PROGRAM(top),
        IMPORT_PROGRAM(terminal),
        IMPORT_PROGRAM(cat),
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
stdret_t regprg_get_program_data(char *name, struct regprg_pdata *prg_data)
{
        if (!prg_data || !name) {
                return STD_RET_ERROR;
        }

        for (uint i = 0; i < ARRAY_SIZE(prog_table); i++) {
                if (strcmp(name, prog_table[i].program_name) == 0) {
                        *prg_data = prog_table[i];
                        return STD_RET_OK;
                }
        }

        return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief Function returns number of programs
 */
//==============================================================================
int regprg_get_program_count(void)
{
        return ARRAY_SIZE(prog_table);
}

//==============================================================================
/**
 * @brief Function returns pointer to the program table
 */
//==============================================================================
struct regprg_pdata *regprg_get_pointer_to_program_list(void)
{
        return (struct regprg_pdata*)&prog_table;
}

#ifdef __cplusplus
}
#endif

/*==============================================================================
  End of file
==============================================================================*/
