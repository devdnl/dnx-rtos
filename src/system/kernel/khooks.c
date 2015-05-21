/*=========================================================================*//**
@file    khooks.c

@author  Daniel Zorychta

@brief   This file support all kernel hooks

@note    Copyright (C) 2012, 2013 Daniel Zorychta <daniel.zorychta@gmail.com>

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

/*==============================================================================
  Include files
==============================================================================*/
#include "config.h"
#include "kernel/khooks.h"
#include "kernel/kpanic.h"
#include "kernel/process.h"
#include "lib/unarg.h"
#include "portable/cpuctl.h"

/*==============================================================================
  Local symbolic constants/macros
==============================================================================*/

/*==============================================================================
  Local types, enums definitions
==============================================================================*/

/*==============================================================================
  Local function prototypes
==============================================================================*/

/*==============================================================================
  Local object definitions
==============================================================================*/
static u32_t sec_divider;

/*==============================================================================
  Exported object definitions
==============================================================================*/
u32_t        _uptime_counter_sec;
extern u32_t _CPU_total_time;

/*==============================================================================
  Function definitions
==============================================================================*/

//==============================================================================
/**
 * @brief Application Idle hook (idle task - don't use loop)
 */
//==============================================================================
void vApplicationIdleHook(void)
{
#if (CONFIG_RTOS_SLEEP_ON_IDLE > 0)
        _cpuctl_sleep();
#endif
}

//==============================================================================
/**
 * @brief Stack overflow hook
 */
//==============================================================================
void vApplicationStackOverflowHook(task_t *taskHdl, char *taskName)
{
        UNUSED_ARG2(taskHdl, taskName);
        _kernel_panic_report(_KERNEL_PANIC_DESC_CAUSE_STACKOVF);
}

//==============================================================================
/**
 * @brief Hook when system tick was increased
 */
//==============================================================================
void vApplicationTickHook(void)
{
        _CPU_total_time += _cpuctl_get_CPU_load_counter_delta();

        if (++sec_divider >= configTICK_RATE_HZ) {
                sec_divider = 0;
                _uptime_counter_sec++;
                _calculate_CPU_load();
        }
}

//==============================================================================
/**
 * @brief Hook when task is switched in
 */
//==============================================================================
void vApplicationSwitchedIn(void)
{
        _task_switched_in();
}

//==============================================================================
/**
 * @brief Hook when task is switched out
 */
//==============================================================================
void vApplicationSwitchedOut(void)
{
        _task_switched_out();
}

//==============================================================================
/**
 * @brief Function return uptime counter in seconds
 *
 * @return uptime counter
 */
//==============================================================================
u32_t _get_uptime_counter(void)
{
        return _uptime_counter_sec;
}

/*==============================================================================
  End of file
==============================================================================*/
