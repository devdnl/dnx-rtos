#ifndef SYSTYPES_H_
#define SYSTYPES_H_
/*=========================================================================*//**
@file    systypes.h

@author  Daniel Zorychta

@brief   This file contains all system types

@note    Copyright (C) 2012 Daniel Zorychta <daniel.zorychta@gmail.com>

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
#include <stddef.h>
#include <string.h>
#include "core/basic_types.h"
#include "kernel/ktypes.h"

/*==============================================================================
  Exported symbolic constants/macros
==============================================================================*/
/* disable conflicting types from standard libraries */
#undef stdin
#undef stdout
#undef malloc
#undef free

/** string usable macros */
#define FIRST_CHARACTER(char__pstr)             char__pstr[0]
#define LAST_CHARACTER(char__pstr)              char__pstr[strlen(char__pstr) - 1]

/*==============================================================================
  Exported symbolic constants/macros
==============================================================================*/

/*==============================================================================
  Exported types, enums definitions
==============================================================================*/
/** universal status type */
typedef enum stdret_enum
{
        STD_RET_OK      = 0,
        STD_RET_ERROR   = 1,
} stdret_t;

/** directory object */
typedef struct vfs_dir DIR;

/*==============================================================================
  Exported object declarations
==============================================================================*/

/*==============================================================================
  Exported function prototypes
==============================================================================*/

#ifdef __cplusplus
}
#endif

#endif /* SYSTYPES_H_ */
/*==============================================================================
  End of file
==============================================================================*/
