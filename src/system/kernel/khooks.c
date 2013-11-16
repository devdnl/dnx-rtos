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
#include "core/printx.h"
#include "core/sysmoni.h"
#include "core/progman.h"
#include "portable/cpuctl.h"
#include "system/thread.h"

#ifdef __cplusplus
extern "C" {
#endif

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
/** uptime counter */
u32_t uptime_counter_sec;
u32_t uptime_divider;

/*==============================================================================
  Exported object definitions
==============================================================================*/

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
void vApplicationStackOverflowHook(task_t *taskHdl, signed char *taskName)
{
        task_delete(taskHdl);
        printk(FONT_COLOR_RED"%s: stack overflow!"RESET_ATTRIBUTES"\n", taskName);
}

//==============================================================================
/**
 * @brief Hook when system tick was increased
 */
//==============================================================================
void vApplicationTickHook(void)
{
        if (++uptime_divider >= (configTICK_RATE_HZ)) {
                uptime_divider = 0;
                uptime_counter_sec++;
        }
}

//==============================================================================
/**
 * @brief Hook when task is switched in
 */
//==============================================================================
void vApplicationSwitchedIn(void)
{
        _copy_task_context_to_standard_variables();
        sysm_task_switched_in();
}

//==============================================================================
/**
 * @brief Hook when task is switched out
 */
//==============================================================================
void vApplicationSwitchedOut(void)
{
        _copy_standard_variables_to_task_context();
        sysm_task_switched_out();
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
        return uptime_counter_sec;
}

#ifdef __cplusplus
}
#endif

/*==============================================================================
  End of file
==============================================================================*/
