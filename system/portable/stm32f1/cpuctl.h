#ifndef CPUCTL_H_
#define CPUCTL_H_
/*=========================================================================*//**
@file    cpuctl.h

@author  Daniel Zorychta

@brief   This file support CPU control

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
#include "core/basic_types.h"

/*==============================================================================
  Exported symbolic constants/macros
==============================================================================*/
/* interrupt rename */
#define xPortPendSVHandler                      PendSV_Handler
#define xPortSysTickHandler                     SysTick_Handler
#define vPortSVCHandler                         SVC_Handler

/* CPU/platform name */
#define CPUCTL_PLATFORM_NAME                    "ARM Cortex-M3 STM32F1xx"

/*==============================================================================
  Exported types, enums definitions
==============================================================================*/

/*==============================================================================
  Exported function prototypes
==============================================================================*/
extern void  cpuctl_init(void);
extern void  cpuctl_restart_system(void);
extern void  cpuctl_init_CPU_load_timer(void);
extern u32_t cpuctl_get_CPU_load_timer(void);
extern void  cpuctl_clear_CPU_load_timer(void);
extern u32_t cpuctl_get_CPU_total_time(void);
extern void  cpuctl_clear_CPU_total_time(void);
#ifdef __cplusplus
}
#endif

#endif /* CPUCTL_H_ */
/*==============================================================================
  End of file
==============================================================================*/
