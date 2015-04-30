/*=========================================================================*//**
@file    unarg.h

@author  Daniel Zorychta

@brief

@note    Copyright (C) 2015 Daniel Zorychta <daniel.zorychta@gmail.com>

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

#ifndef _LIB_UNARG_H_
#define _LIB_UNARG_H_

/*==============================================================================
  Include files
==============================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Exported macros
==============================================================================*/
#define UNUSED_ARG1(_arg1)                                      ((void)_arg1)
#define UNUSED_ARG2(_arg1, _arg2)                               ((void)_arg1); ((void)_arg2)
#define UNUSED_ARG3(_arg1, _arg2, _arg3)                        ((void)_arg1); ((void)_arg2); ((void)_arg3)
#define UNUSED_ARG4(_arg1, _arg2, _arg3, _arg4)                 ((void)_arg1); ((void)_arg2); ((void)_arg3); ((void)_arg4)
#define UNUSED_ARG5(_arg1, _arg2, _arg3, _arg4, _arg5)          ((void)_arg1); ((void)_arg2); ((void)_arg3); ((void)_arg4); ((void)_arg5)
#define UNUSED_ARG6(_arg1, _arg2, _arg3, _arg4, _arg5, _arg6)   ((void)_arg1); ((void)_arg2); ((void)_arg3); ((void)_arg4); ((void)_arg5); ((void)_arg6)

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

#endif /* _LIB_UNARG_H_ */
/*==============================================================================
  End of file
==============================================================================*/
