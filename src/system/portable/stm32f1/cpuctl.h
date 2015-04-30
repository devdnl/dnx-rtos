/*=========================================================================*//**
@file    cpuctl.h

@author  Daniel Zorychta

@brief   This file support CPU control

@note    Copyright (C) 2012-2014 Daniel Zorychta <daniel.zorychta@gmail.com>

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
#define _CPUCTL_PLATFORM_NAME                   "ARM Cortex-M3 STM32F1xx"
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
extern void  _cpuctl_sleep                      (void);
extern void  _cpuctl_update_system_clocks       (void);

#if (CONFIG_MONITOR_CPU_LOAD > 0)
extern void  _cpuctl_init_CPU_load_counter      (void);
extern void  _cpuctl_reset_CPU_load_counter     (void);
extern u32_t _cpuctl_get_CPU_load_counter_value (void);
#endif

#ifdef __cplusplus
}
#endif

#endif /* _CPUCTL_H_ */
/*==============================================================================
  End of file
==============================================================================*/
