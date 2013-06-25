#ifndef _ASSERT_H_
#define _ASSERT_H_
/*=========================================================================*//**
@file    assert.h

@author  Daniel Zorychta

@brief   This file provide assert macro.

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
#include "config.h"

/*==============================================================================
  Exported macros
==============================================================================*/
#if CONFIG_ASSERT_USAGE > 0
#       define assert(ignore) do{if ((ignore)) break; printf("Assertion failed: %s, file %s, line %d\n", #ignore, __FILE__, __LINE__); abort();}while(0)
#else
#       define assert(ignore)
#endif

/*==============================================================================
  Exported object types
==============================================================================*/

/*==============================================================================
  Exported objects
==============================================================================*/

/*==============================================================================
  Exported functions
==============================================================================*/


#ifdef __cplusplus
}
#endif

#endif /* _ASSERT_H_ */
/*==============================================================================
  End of file
==============================================================================*/
