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
        u8_t                           syscall;         /* syscall            */
};

/*==============================================================================
  Local function prototypes
==============================================================================*/
static void catch_process(enum _kernel_panic_desc_cause suggested_cause);

/*==============================================================================
  Local objects
==============================================================================*/
static struct kernel_panic_desc kpanic_desc __attribute__ ((section (".noinit")));
static u32_t kpanic_ctr;

#if ((__OS_SYSTEM_MSG_ENABLE__ > 0) && (__OS_PRINTF_ENABLE__ > 0))
static const char *CAUSE[] = {
       "SEGFAULT",
       "STACKOVF",
       "CPUFAULT",
       "INTERNAL1",
       "INTERNAL2",
       "INTERNAL3",
       "INTERNAL4",
       "PANICLOOP",
       "UNKNOWN"
};
#endif

/*==============================================================================
  Exported objects
==============================================================================*/
_process_t *_kernel_panic_trap_proc;

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
 * @param  info         kernel panic info
 *
 * @return If kernel panic occured in the last session then true is returned,
 *         otherwise false.
 */
//==============================================================================
bool _kernel_panic_info(struct _kernel_panic_info *info)
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

                printk(VT100_FONT_COLOR_RED"%s: %s: %d:%d,%s,%s,%u"VT100_RESET_ATTRIBUTES,
                       panic_type, kpanic_desc.name, kpanic_desc.pid, kpanic_desc.tid,
                       CAUSE[kpanic_desc.cause], kpanic_desc.kernelspace ? "KS" : "US",
                       kpanic_desc.syscall);

                _cpuctl_print_exception();
            #endif

                if (info) {
                        info->cause = (kpanic_desc.cause > _KERNEL_PANIC_DESC_CAUSE_UNKNOWN)
                                    ? _KERNEL_PANIC_DESC_CAUSE_UNKNOWN : kpanic_desc.cause;
                        info->cause_str = CAUSE[info->cause];
                        info->name = (kpanic_desc.name == NULL) ? "<unknown>" : kpanic_desc.name;
                        info->kernelspace = kpanic_desc.kernelspace;
                        info->pid = kpanic_desc.pid;
                        info->tid = kpanic_desc.tid;
                        info->sycall = kpanic_desc.syscall;
                }

                kpanic_desc.valid1 = 0;
                kpanic_desc.valid2 = 0;
        }

        return occurred;
}

//==============================================================================
/**
 * @brief  Function report kernel panic
 *
 * @param  suggest_cause        suggested cause
 */
//==============================================================================
void _kernel_panic_report(enum _kernel_panic_desc_cause suggested_cause)
{
        _ISR_disable();
        catch_process(suggested_cause);
        _ISR_enable();
        _task_yield();
}

//==============================================================================
/**
 * @brief  Function report kernel panic
 *
 * @param  suggest_cause        suggested cause
 */
//==============================================================================
void _kernel_panic_report_from_ISR(enum _kernel_panic_desc_cause suggested_cause)
{
        catch_process(suggested_cause);
        _task_yield_from_ISR(true);
}

//==============================================================================
/**
 * @brief  Function catch current process.
 *
 * @param  suggest_cause        suggested cause
 */
//==============================================================================
static void catch_process(enum _kernel_panic_desc_cause suggested_cause)
{
        kpanic_ctr++;

        if (!_kernel_panic_trap_proc) {
                kpanic_desc.cause = suggested_cause;
                _kernel_panic_trap_proc = _process_get_active();
        }
        _assert_hook_suspend(true);
        _task_set_priority(_kernel_get_idle_task_handle(), PRIORITY_HIGHEST);
        _assert_hook_suspend(false);

        if (kpanic_ctr > 1000000) {
                _kernel_panic_trap_proc = NULL;
                kpanic_desc.cause = _KERNEL_PANIC_DESC_CAUSE_PANICLOOP;
                _kernel_panic_handle(false);
        }
}

//==============================================================================
/**
 * @brief  Function handle kernel panic. Function is executed in IDLE task.
 *
 * @param  system_consistent    system is consistent
 */
//==============================================================================
void _kernel_panic_handle(bool system_consistent)
{
        if (_kernel_panic_trap_proc or (kpanic_desc.cause == _KERNEL_PANIC_DESC_CAUSE_PANICLOOP)) {
                _process_t *proc = _kernel_panic_trap_proc;

                if (proc) {
                        kpanic_desc.name = _process_get_name(proc);
                        kpanic_desc.tid  = _process_get_active_thread(proc);
                        _process_get_pid(proc, &kpanic_desc.pid);
                        kpanic_desc.kernelspace = _process_is_kernelspace(proc, kpanic_desc.tid);
                        kpanic_desc.syscall = _process_get_curr_syscall(proc, kpanic_desc.tid);

                } else {
                        kpanic_desc.name = "<NULL>";
                        kpanic_desc.pid  = 0;
                        kpanic_desc.tid  = 0;
                        kpanic_desc.kernelspace = true;
                        kpanic_desc.syscall = 0xFF;
                        system_consistent = false;
                }

                if (system_consistent && (kpanic_desc.pid > 1)) {

                        _printk("APP CRASH <%s> %d:%d,%s,%s,%u\n",
                                kpanic_desc.name,
                                kpanic_desc.pid, kpanic_desc.tid,
                                CAUSE[kpanic_desc.cause],
                                kpanic_desc.kernelspace ? "KRN" : "USR",
                                kpanic_desc.syscall);

                        _process_kill(kpanic_desc.pid);

                } else {
                        kpanic_desc.valid1 = _KERNEL_PANIC_DESC_VALID1;
                        kpanic_desc.valid2 = _KERNEL_PANIC_DESC_VALID2;
                        _cpuctl_clean_dcache();
                        _cpuctl_restart_system();
                }

                kpanic_ctr = 0;
                _kernel_panic_trap_proc = NULL;
        }
}

/*==============================================================================
  End of file
==============================================================================*/
