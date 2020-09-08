/*=========================================================================*//**
@file    kpanic.c

@author  Daniel Zorychta

@brief   Kernel panic handling

@note    Copyright (C) 2020 Daniel Zorychta <daniel.zorychta@gmail.com>

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
#include <string.h>
#include "kernel/kpanic.h"
#include "kernel/printk.h"
#include "kernel/kwrapper.h"
#include "kernel/process.h"
#include "kernel/sysfunc.h"
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
        uint32_t                       valid1;          /* validation value 1 */
        enum _kernel_panic_desc_cause  cause;           /* kernel panic cause */
        const char                    *name;            /* process name       */
        pid_t                          pid;             /* process ID         */
        tid_t                          tid;             /* thread ID          */
        uint32_t                       valid2;          /* validation value 2 */
};

/*==============================================================================
  Local function prototypes
==============================================================================*/

/*==============================================================================
  Local objects
==============================================================================*/
static struct kernel_panic_desc kernel_panic_descriptor __attribute__ ((section (".noinit")));

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
int _kernel_panic_init(void)
{
        return 0;
}

//==============================================================================
/**
 * @brief  Function check if the kernel panic occurred in the last session
 *
 * @param  path         file path to write panic message
 *
 * @return If kernel panic occured in the last session then true is returned,
 *         otherwise false.
 */
//==============================================================================
bool _kernel_panic_detect(FILE *file)
{
#if ((__OS_SYSTEM_MSG_ENABLE__ > 0) && (__OS_PRINTF_ENABLE__ > 0))
        static const char *cause[] = {
               "SEGFAULT",
               "STACKOVF",
               "CPUFAULT",
               "INTERNAL1",
               "INTERNAL2",
               "INTERNAL3",
               "INTERNAL4",
               "UNKNOWN"
        };
#endif

        bool occurred = (  kernel_panic_descriptor.valid1 == _KERNEL_PANIC_DESC_VALID1
                        && kernel_panic_descriptor.valid2 == _KERNEL_PANIC_DESC_VALID2 );

        if (occurred) {
              #if ((__OS_SYSTEM_MSG_ENABLE__ > 0) && (__OS_PRINTF_ENABLE__ > 0))
                const char *panic_type;

                if (  (strncmp(kernel_panic_descriptor.name, "kworker",  128) == 0)
                   || (kernel_panic_descriptor.cause >= _KERNEL_PANIC_DESC_CAUSE_CPUFAULT) ) {
                        panic_type = "KERNEL PANIC";
                } else {
                        panic_type = "APP CRASH";
                }

                printk("%s in %s: %d:%d:%s", panic_type, kernel_panic_descriptor.name,
                       kernel_panic_descriptor.pid, kernel_panic_descriptor.tid,
                       cause[kernel_panic_descriptor.cause]);
              #endif

                if (file) {
                        if (kernel_panic_descriptor.cause > _KERNEL_PANIC_DESC_CAUSE_UNKNOWN) {
                                kernel_panic_descriptor.cause = _KERNEL_PANIC_DESC_CAUSE_UNKNOWN;
                        }

                        if (kernel_panic_descriptor.name == NULL) {
                                kernel_panic_descriptor.name = "<unknown>";
                        }

                      #if ((__OS_SYSTEM_MSG_ENABLE__ > 0) && (__OS_PRINTF_ENABLE__ > 0))
                        sys_fprintf(file, VT100_FONT_COLOR_RED"*** %s ***"VT100_RESET_ATTRIBUTES"\n", panic_type);
                        sys_fprintf(file, "Cause: %s\n", cause[kernel_panic_descriptor.cause]);
                        sys_fprintf(file, "PID  : %d (%.*s)\n", kernel_panic_descriptor.pid, 256, kernel_panic_descriptor.name);
                        sys_fprintf(file, "TID  : %d\n", kernel_panic_descriptor.tid);
                        _cpuctl_print_exception(file);
                      #endif
                }

                kernel_panic_descriptor.valid1 = 0;
                kernel_panic_descriptor.valid2 = 0;
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
void _kernel_panic_report(enum _kernel_panic_desc_cause suggested_cause)
{
        _ISR_disable();

        _process_t *proc = _process_get_active();

        if (proc) {
                kernel_panic_descriptor.name = _process_get_name(proc);
                kernel_panic_descriptor.tid  = _process_get_active_thread(proc);
                _process_get_pid(proc, &kernel_panic_descriptor.pid);

        } else {
                if (_task_get_handle() == _kernel_get_idle_task_handle()) {
                        kernel_panic_descriptor.name = "IDLE";
                        kernel_panic_descriptor.pid  = 0;
                        kernel_panic_descriptor.tid  = 0;
                } else {
                        kernel_panic_descriptor.name = NULL;
                        kernel_panic_descriptor.pid  = -1;
                        kernel_panic_descriptor.tid  = -1;
                }
        }

        if (suggested_cause == _KERNEL_PANIC_DESC_CAUSE_STACKOVF || _task_get_free_stack(_THIS_TASK) == 0) {
                kernel_panic_descriptor.cause = _KERNEL_PANIC_DESC_CAUSE_STACKOVF;
        } else {
                kernel_panic_descriptor.cause = suggested_cause;
        }

        kernel_panic_descriptor.valid1 = _KERNEL_PANIC_DESC_VALID1;
        kernel_panic_descriptor.valid2 = _KERNEL_PANIC_DESC_VALID2;

        _cpuctl_restart_system();
}

/*==============================================================================
  End of file
==============================================================================*/
