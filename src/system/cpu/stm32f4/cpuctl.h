/*=========================================================================*//**
@file    cpuctl.h

@author  Daniel Zorychta

@brief   This file support CPU control

@note    Copyright (C) 2017 Daniel Zorychta <daniel.zorychta@gmail.com>

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

#ifndef _CPUCTL_H_
#define _CPUCTL_H_

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Include files
==============================================================================*/
#include <sys/types.h>
#include "config.h"

/*==============================================================================
  Exported symbolic constants/macros
==============================================================================*/
/* CPU/platform name */
#define _CPUCTL_PLATFORM_NAME                   "ARM Cortex-M4F STM32F4xx"
#define _CPUCTL_VENDOR_NAME                     "STMicroelectronics"
#define _CPUCTL_BYTE_ORDER                      _BYTE_ORDER_LITTLE_ENDIAN

/* renames of interrupts vectors for kernel purposes */
#define xPortPendSVHandler                      PendSV_Handler
#define xPortSysTickHandler                     SysTick_Handler
#define vPortSVCHandler                         SVC_Handler

/*==============================================================================
  Exported types, enums definitions
==============================================================================*/

/*==============================================================================
  Exported function prototypes
==============================================================================*/
extern void  _cpuctl_init                       (void);
extern void  _cpuctl_restart_system             (void);
extern void  _cpuctl_shutdown_system            (void);
extern void  _cpuctl_sleep                      (void);
extern void  _cpuctl_update_system_clocks       (void);
extern void  _cpuctl_delay_us                   (u16_t);
extern void  _cpuctl_print_exception            (void *file);

#if (__OS_MONITOR_CPU_LOAD__ > 0)
extern void  _cpuctl_init_CPU_load_counter      (void);
extern u32_t _cpuctl_get_CPU_load_counter_delta (void);
#endif

#ifdef __cplusplus
}
#endif

#endif /* _CPUCTL_H_ */
/*==============================================================================
  End of file
==============================================================================*/
