/*=========================================================================*//**
@file    util.h

@author  Daniel Zorychta

@brief

@note    Copyright (C) 2014 Daniel Zorychta <daniel.zorychta@gmail.com>

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

#ifndef _UTIL_H_
#define _UTIL_H_

/*==============================================================================
  Include files
==============================================================================*/
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Exported macros
==============================================================================*/
/** array element count */
#define ARRAY_SIZE(array)                       (sizeof(array)/sizeof(array[0]))
#define ARRAY_ITEM_SIZE(array)                  (sizeof(array[0]))
#define FIELD_SIZEOF(t, f)                      (sizeof(((t*)0)->f))
#define CONTAINER_OF(ptr, type, member)         ((type *)((char *)ptr - offsetof(type, member)))

/** usable macros */
#define UNUSED_ARG(argument)                    (void)argument

/** string usable macros */
#define FIRST_CHARACTER(char__pstr)             char__pstr[0]
#define LAST_CHARACTER(char__pstr)              char__pstr[strlen(char__pstr) - 1]

/*==============================================================================
  Exported object types
==============================================================================*/

/*==============================================================================
  Exported objects
==============================================================================*/

/*==============================================================================
  Exported functions
==============================================================================*/

/*==============================================================================
  Exported inline functions
==============================================================================*/

#ifdef __cplusplus
}
#endif

#endif /* _UTIL_H_ */
/*==============================================================================
  End of file
==============================================================================*/
