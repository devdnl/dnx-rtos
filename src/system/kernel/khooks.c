/*=========================================================================*//**
@file    khooks.c

@author  Daniel Zorychta

@brief   This file support all kernel hooks

@note    Copyright (C) 2012, 2013 Daniel Zorychta <daniel.zorychta@gmail.com>

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

/*==============================================================================
  Include files
==============================================================================*/
#include "config.h"
#include "kernel/khooks.h"
#include "kernel/kpanic.h"
#include "kernel/process.h"
#include "kernel/printk.h"
#include "dnx/misc.h"
#include "lib/unarg.h"
#include "cpu/cpuctl.h"

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
u32_t        _uptime_counter_sec = 0;
u64_t        _tick_counter = 0;
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
        /*
         * Set priority of idle task to lowest possible.
         * The _kernel_release_resources() function set idle task with the highest
         * priority to release finished tasks. This function restore original
         * idle task priority.
         */
        vTaskPrioritySet(xTaskGetIdleTaskHandle(), 0);

        /*
         * Sleep CPU for single tick to save energy.
         */
        #if (__OS_SLEEP_ON_IDLE__ > 0)
        _cpuctl_sleep();
        #endif
}

//==============================================================================
/**
 * @brief Stack overflow hook
 */
//==============================================================================
void vApplicationStackOverflowHook(TaskHandle_t taskHdl, char *taskName)
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
        _tick_counter++;

#if (__OS_MONITOR_CPU_LOAD__ > 0)
        _CPU_total_time += _cpuctl_get_CPU_load_counter_delta();
#endif

        if (++sec_divider >= configTICK_RATE_HZ) {
                sec_divider = 0;
                _uptime_counter_sec++;
                _calculate_CPU_load();
        }
}

//==============================================================================
/**
 * @brief Memory for idle task.
 */
//==============================================================================
void vApplicationGetIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer,
                                   StackType_t **ppxIdleTaskStackBuffer,
                                   uint32_t *pulIdleTaskStackSize )
{
        /* If the buffers to be provided to the Idle task are declared inside this
        function then they must be declared static - otherwise they will be allocated on
        the stack and so not exists after this function exits. */
        static StaticTask_t xIdleTaskTCB;
        static StackType_t  uxIdleTaskStack[STACK_DEPTH_MINIMAL];

        /* Pass out a pointer to the StaticTask_t structure in which the Idle task's
        state will be stored. */
        *ppxIdleTaskTCBBuffer = &xIdleTaskTCB;

        /* Pass out the array that will be used as the Idle task's stack. */
        *ppxIdleTaskStackBuffer = uxIdleTaskStack;

        /* Pass out the size of the array pointed to by *ppxIdleTaskStackBuffer.
        Note that, as the array is necessarily of type StackType_t,
        configMINIMAL_STACK_SIZE is specified in words, not bytes. */
        *pulIdleTaskStackSize = ARRAY_SIZE(uxIdleTaskStack);
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

#if __OS_ENABLE_SYS_ASSERT__ > 0
//==============================================================================
/**
 * @brief  Function is called when assertion is not meet.
 *
 * @param  assert       if true then no action
 * @param  msg          message
 */
//==============================================================================
void _assert_hook(bool assert, const char *msg)
{
        if (!assert) {
                if (msg) {
                        _printk("System assert: %s", msg);
                } else {
                        _printk("System assert occurred!");
                }
        }
}
#endif

/*==============================================================================
  End of file
==============================================================================*/
