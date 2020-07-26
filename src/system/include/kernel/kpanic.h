/*=========================================================================*//**
@file    kpanic.h

@author  Daniel Zorychta

@brief   Kernel panic handling

@note    Copyright (C) 2015 Daniel Zorychta <daniel.zorychta@gmail.com>

         This program is free software; you can redistribute it and/or modify
         it under the terms of the GNU General Public License as published by
         the Free Software Foundation and modified by the dnx RTOS exception.

         NOTE: The modification  to the GPL is  included to allow you to
               distribute a combined work that includes dnx RTOS without
               being obliged to provide the source  code for proprietary
               components outside of the dnx RTOS.

         The dnx RTOS  is  distributed  in the hope  that  it will be useful,
         but WITHOUT  ANY  WARRANTY;  without  even  the implied  warranty of
         MERCHANTABILITY  or  FITNESS  FOR  A  PARTICULAR  PURPOSE.  See  the
         GNU General Public License for more details.

         Full license text is available on the following file: doc/license.txt.


*//*==========================================================================*/

#ifndef _KPANIC_H_
#define _KPANIC_H_

/*==============================================================================
  Include files
==============================================================================*/
#include <stdbool.h>
#include "fs/vfs.h"

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
        _KERNEL_PANIC_DESC_CAUSE_SEGFAULT   = 0,
        _KERNEL_PANIC_DESC_CAUSE_STACKOVF   = 1,
        _KERNEL_PANIC_DESC_CAUSE_CPUFAULT   = 2,
        _KERNEL_PANIC_DESC_CAUSE_INTERNAL_1 = 3,
        _KERNEL_PANIC_DESC_CAUSE_INTERNAL_2 = 4,
        _KERNEL_PANIC_DESC_CAUSE_INTERNAL_3 = 5,
        _KERNEL_PANIC_DESC_CAUSE_INTERNAL_4 = 6,
        _KERNEL_PANIC_DESC_CAUSE_UNKNOWN    = 7
};

/*==============================================================================
  Exported objects
==============================================================================*/

/*==============================================================================
  Exported functions
==============================================================================*/
extern int  _kernel_panic_init();
extern bool _kernel_panic_detect(FILE*);
extern void _kernel_panic_report(enum _kernel_panic_desc_cause);

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
