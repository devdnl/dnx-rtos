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
#include "kernel/khooks.h"
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
        bool                           kernelspace;     /* kernel space       */
};

/*==============================================================================
  Local function prototypes
==============================================================================*/

/*==============================================================================
  Local objects
==============================================================================*/
static struct kernel_panic_desc kpanic_desc __attribute__ ((section (".noinit")));

#if ((__OS_SYSTEM_MSG_ENABLE__ > 0) && (__OS_PRINTF_ENABLE__ > 0))
static const char *CAUSE[] = {
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
        bool occurred = (  kpanic_desc.valid1 == _KERNEL_PANIC_DESC_VALID1
                        && kpanic_desc.valid2 == _KERNEL_PANIC_DESC_VALID2 );

        if (occurred) {
              #if ((__OS_SYSTEM_MSG_ENABLE__ > 0) && (__OS_PRINTF_ENABLE__ > 0))
                const char *panic_type;

                if (  (strncmp(kpanic_desc.name, "kworker",  128) == 0)
                   || (kpanic_desc.cause >= _KERNEL_PANIC_DESC_CAUSE_CPUFAULT) ) {
                        panic_type = "KERNEL PANIC";
                } else {
                        panic_type = "APP CRASH";
                }

                printk("%s: %s: %d:%d:%s:%s", panic_type, kpanic_desc.name,
                       kpanic_desc.pid, kpanic_desc.tid,
                       CAUSE[kpanic_desc.cause], kpanic_desc.kernelspace ? "KS" : "US");
              #endif

                if (file) {
                        if (kpanic_desc.cause > _KERNEL_PANIC_DESC_CAUSE_UNKNOWN) {
                                kpanic_desc.cause = _KERNEL_PANIC_DESC_CAUSE_UNKNOWN;
                        }

                        if (kpanic_desc.name == NULL) {
                                kpanic_desc.name = "<unknown>";
                        }

                      #if ((__OS_SYSTEM_MSG_ENABLE__ > 0) && (__OS_PRINTF_ENABLE__ > 0))
                        sys_fprintf(file, VT100_FONT_COLOR_RED"*** %s ***"VT100_RESET_ATTRIBUTES"\n", panic_type);
                        sys_fprintf(file, "Cause: %s\n", CAUSE[kpanic_desc.cause]);
                        sys_fprintf(file, "PID  : %d (%.*s)\n", kpanic_desc.pid, 256, kpanic_desc.name);
                        sys_fprintf(file, "TID  : %d\n", kpanic_desc.tid);
                        sys_fprintf(file, "SPACE: %s\n", kpanic_desc.kernelspace ? "kernel" : "user");
                        _cpuctl_print_exception(file);
                      #endif
                }

                kpanic_desc.valid1 = 0;
                kpanic_desc.valid2 = 0;
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
        _assert_hook_suspend(true);
        _ISR_disable();

        _process_t *proc = _process_get_active();

        if (proc) {
                kpanic_desc.name = _process_get_name(proc);
                kpanic_desc.tid  = _process_get_active_thread(proc);
                _process_get_pid(proc, &kpanic_desc.pid);
                kpanic_desc.kernelspace = _process_is_kernelspace(proc, kpanic_desc.tid);

        } else {
                if (_task_get_handle() == _kernel_get_idle_task_handle()) {
                        kpanic_desc.name = "IDLE";
                        kpanic_desc.pid  = 0;
                        kpanic_desc.tid  = 0;
                } else {
                        kpanic_desc.name = NULL;
                        kpanic_desc.pid  = -1;
                        kpanic_desc.tid  = -1;
                }

                kpanic_desc.kernelspace = true;
        }

        if (suggested_cause == _KERNEL_PANIC_DESC_CAUSE_STACKOVF || _task_get_free_stack(_THIS_TASK) == 0) {
                kpanic_desc.cause = _KERNEL_PANIC_DESC_CAUSE_STACKOVF;
        } else {
                kpanic_desc.cause = suggested_cause;
        }

        // consistency check - if everything looks good then kill program
        if (kpanic_desc.pid > 1) {

                if (_mm_check_consistency() && _process_is_consistent(true)) {
                        _process_kill(kpanic_desc.pid);
                        printk("APP CRASH: %s: %d:%d:%s:%s", kpanic_desc.name,
                               kpanic_desc.pid, kpanic_desc.tid,
                               CAUSE[kpanic_desc.cause], kpanic_desc.kernelspace ? "KRN" : "USR");
                        _ISR_enable();
                        _assert_hook_suspend(false);
                        return;
                }
        }

        kpanic_desc.valid1 = _KERNEL_PANIC_DESC_VALID1;
        kpanic_desc.valid2 = _KERNEL_PANIC_DESC_VALID2;

        _cpuctl_restart_system();
}

/*==============================================================================
  End of file
==============================================================================*/
