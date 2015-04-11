/*=========================================================================*//**
@file    kpanic.c

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

/*==============================================================================
  Include files
==============================================================================*/
#include <string.h>
#include "kernel/kpanic.h"
#include "kernel/printk.h"
#include "kernel/kwrapper.h"
#include "lib/vt100.h"
#include "lib/cast.h"
#include "mm/mm.h"
#include "config.h"

/*==============================================================================
  Local macros
==============================================================================*/
#define _KERNEL_PANIC_DESC_VALID1               0x7C63EE8E
#define _KERNEL_PANIC_DESC_VALID2               0xCF4C5484

/*==============================================================================
  Local object types
==============================================================================*/
struct kernel_panic_desc {
        uint32_t                       valid1;
        enum _kernel_panic_desc_cause  cause;
        char                           task_name[CONFIG_RTOS_TASK_NAME_LEN];
        uint32_t                       valid2;
};

/*==============================================================================
  Local function prototypes
==============================================================================*/

/*==============================================================================
  Local objects
==============================================================================*/
static struct kernel_panic_desc *kernel_panic_descriptor;

/*==============================================================================
  Exported objects
==============================================================================*/

/*==============================================================================
  External objects
==============================================================================*/

/*==============================================================================
  Function definitions
==============================================================================*/

//==============================================================================
/**
 * @brief  Initialize kernel panic detector
 * @param  None
 * @return One of errno value.
 */
//==============================================================================
int _kernel_panic_init()
{
        return _kmalloc(_MM_KRN,
                        sizeof(struct kernel_panic_desc),
                        reinterpret_cast(void**, &kernel_panic_descriptor));
}

//==============================================================================
/**
 * @brief  Function check if the kernel panic occurred in the last session
 * @param  show_msg     true: show error message
 * @return If kernel panic occured in the last session then true is returned,
 *         otherwise false.
 */
//==============================================================================
bool _kernel_panic_detect(bool show_msg)
{
#if ((CONFIG_SYSTEM_MSG_ENABLE > 0) && (CONFIG_PRINTF_ENABLE > 0))
        static const char *cause[] = {
               "SEGFAULT",
               "STACKOVF",
               "CPUFAULT",
               "UNKNOWN"
        };
#endif

        bool occurred = (  kernel_panic_descriptor->valid1 == _KERNEL_PANIC_DESC_VALID1
                        && kernel_panic_descriptor->valid2 == _KERNEL_PANIC_DESC_VALID2 );

        if (occurred) {
                if (show_msg) {
                        if (kernel_panic_descriptor->cause > _KERNEL_PANIC_DESC_CAUSE_UNKNOWN) {
                                kernel_panic_descriptor->cause = _KERNEL_PANIC_DESC_CAUSE_UNKNOWN;
                        }

                        kernel_panic_descriptor->task_name[CONFIG_RTOS_TASK_NAME_LEN - 1] = '\0';
                        if (strlen(kernel_panic_descriptor->task_name) == 0) {
                                strncpy(kernel_panic_descriptor->task_name, "<defected>", CONFIG_RTOS_TASK_NAME_LEN);
                        }

#if ((CONFIG_SYSTEM_MSG_ENABLE > 0) && (CONFIG_PRINTF_ENABLE > 0))
                        _printk(VT100_FONT_COLOR_RED"*** KERNEL PANIC ***"VT100_RESET_ATTRIBUTES"\n");
                        _printk("Cause: %s\n", cause[kernel_panic_descriptor->cause]);
                        _printk("Task : %s\n\n", kernel_panic_descriptor->task_name);
#endif
                        _sleep(2);
                }

                kernel_panic_descriptor->valid1 = 0;
                kernel_panic_descriptor->valid2 = 0;
        }

        return occurred;
}

//==============================================================================
/**
 * @brief  Function report kernel panic
 * @param  suggest_cause        suggested cause (STACKOVF has priority)
 * @return None
 */
//==============================================================================
void _kernel_panic_report(const char *task_name, enum _kernel_panic_desc_cause suggest_cause)
{
        strncpy(kernel_panic_descriptor->task_name, task_name, CONFIG_RTOS_TASK_NAME_LEN);

        if (suggest_cause == _KERNEL_PANIC_DESC_CAUSE_STACKOVF || _task_get_free_stack() == 0) {
                kernel_panic_descriptor->cause = _KERNEL_PANIC_DESC_CAUSE_STACKOVF;
        } else {
                kernel_panic_descriptor->cause = suggest_cause;
        }

        kernel_panic_descriptor->valid1 = _KERNEL_PANIC_DESC_VALID1;
        kernel_panic_descriptor->valid2 = _KERNEL_PANIC_DESC_VALID2;

        _cpuctl_restart_system();
}

/*==============================================================================
  End of file
==============================================================================*/
