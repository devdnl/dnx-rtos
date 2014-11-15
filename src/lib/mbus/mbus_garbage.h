/*=========================================================================*//**
@file    mbus_garbage.h

@author  Daniel Zorychta

@brief   Message Bus Library. Garbage class.

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

#ifndef _MBUS_GARBAGE_H_
#define _MBUS_GARBAGE_H_

/*==============================================================================
  Include files
==============================================================================*/
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Exported macros
==============================================================================*/

/*==============================================================================
  Exported object types
==============================================================================*/
typedef struct _mbus_garbage _mbus_garbage_t;

/*==============================================================================
  Exported objects
==============================================================================*/

/*==============================================================================
  Exported functions
==============================================================================*/
extern _mbus_garbage_t *_mbus_garbage_new(void*);
extern void             _mbus_garbage_delete(_mbus_garbage_t*);
extern bool             _mbus_garbage_is_time_expired(_mbus_garbage_t*);

/*==============================================================================
  Exported inline functions
==============================================================================*/

#ifdef __cplusplus
}
#endif

#endif /* _MBUS_GARBAGE_H_ */
/*==============================================================================
  End of file
==============================================================================*/
