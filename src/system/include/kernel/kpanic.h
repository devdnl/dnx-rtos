/*=========================================================================*//**
@file    kpanic.h

@author  Daniel Zorychta

@brief   Kernel panic handling

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

#ifndef _KPANIC_H_
#define _KPANIC_H_

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
enum _kernel_panic_desc_cause {
        _KERNEL_PANIC_DESC_CAUSE_SEGFAULT = 0,
        _KERNEL_PANIC_DESC_CAUSE_STACKOVF = 1,
        _KERNEL_PANIC_DESC_CAUSE_CPUFAULT = 2,
        _KERNEL_PANIC_DESC_CAUSE_UNKNOWN  = 3
};

/*==============================================================================
  Exported objects
==============================================================================*/

/*==============================================================================
  Exported functions
==============================================================================*/
extern int  _kernel_panic_init();
extern bool _kernel_panic_detect(bool);
extern void _kernel_panic_report(const char*, enum _kernel_panic_desc_cause);

/*==============================================================================
  Exported inline functions
==============================================================================*/

#ifdef __cplusplus
}
#endif

#endif /* _KPANIC_H_ */
/*==============================================================================
  End of file
==============================================================================*/
