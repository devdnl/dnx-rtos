/*=========================================================================*//**
@file    khooks.h

@author  Daniel Zorychta

@brief   This file support all kernel hooks

@note    Copyright (C) 2012 Daniel Zorychta <daniel.zorychta@gmail.com>

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

#ifndef _KHOOKS_H_
#define _KHOOKS_H_

#ifdef __cplusplus
   extern "C" {
#endif

/*==============================================================================
  Include files
==============================================================================*/
#include "kernel/kwrapper.h"

/*==============================================================================
  Exported symbolic constants/macros
==============================================================================*/
#if __OS_ENABLE_SYS_ASSERT__ > 0
#define _assert(x) _assert_hook(x)
#else
#define _assert(x) (void)(x)
#endif

/*==============================================================================
  Exported types, enums definitions
==============================================================================*/

/*==============================================================================
  Exported object declarations
==============================================================================*/

/*==============================================================================
  Exported function prototypes
==============================================================================*/
extern void  vApplicationStackOverflowHook(TaskHandle_t pxTask, char *pcTaskName);
extern void  vApplicationTickHook(void);
extern u32_t _get_uptime_counter(void);

#if __OS_ENABLE_SYS_ASSERT__ > 0
extern void _assert_hook(bool assert);
#endif

#ifdef __cplusplus
   }
#endif

#endif /* _KHOOKS_H_ */
/*==============================================================================
  End of file
==============================================================================*/
