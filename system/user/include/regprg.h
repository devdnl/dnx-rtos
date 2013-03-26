#ifndef REGPRG_H_
#define REGPRG_H_
/*=========================================================================*//**
@file    regprg.h

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
#include "core/systypes.h"

/*==============================================================================
  Exported symbolic constants/macros
==============================================================================*/

/*==============================================================================
  Exported types, enums definitions
==============================================================================*/
struct regprg_pdata {
        char  *program_name;
        int  (*main_function)(int, char**);
        const uint *globals_size;
        const uint *stack_depth;
};

/*==============================================================================
  Exported object declarations
==============================================================================*/

/*==============================================================================
  Exported function prototypes
==============================================================================*/
extern stdret_t regprg_get_program_data(char*, struct regprg_pdata*);
extern int regprg_get_program_count(void);
extern struct regprg_pdata *regprg_get_pointer_to_program_list(void);

#ifdef __cplusplus
}
#endif

#endif /* REGPRG_H_ */
/*==============================================================================
  End of file
==============================================================================*/
