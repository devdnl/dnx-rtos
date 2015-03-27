/*=========================================================================*//**
@file    config.h

@author  Daniel Zorychta

@brief   Global system configuration

@note    Copyright (C) 2013 Daniel Zorychta <daniel.zorychta@gmail.com>

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

#ifndef _CONFIG_H_
#define _CONFIG_H_

#include "../../../config/project/flags.h"


/**=== FREQ CONFIGURATION ====================================================*/
/**
 * CPU start frequency [Hz] (frequency after power up, before clock configuration)
 */
#define CONFIG_CPU_START_FREQ                           (_CPU_START_FREQ_)

/**
 * CPU base frequency [Hz] (internal RC, oscillator or crystal frequency connected to PLL)
 */
#define CONFIG_CPU_OSC_FREQ                             (__CPU_OSC_FREQ__)


/**=== DYNAMIC MEMORY CONFIGURATION ==========================================*/
/**
 * Heap memory alignment
 */
#define CONFIG_HEAP_ALIGN                               (_HEAP_ALIGN_)

/**
 * Heap block size [bytes]
 */
#define CONFIG_HEAP_BLOCK_SIZE                          (__HEAP_BLOCK_SIZE__)


/**=== RTOS CONFIGURATION ====================================================*/
/**
 * Task minimal stack depth [levels]
 */
#define CONFIG_RTOS_TASK_MIN_STACK_DEPTH                (__OS_TASK_MIN_STACK_DEPTH__)

/**
 * File system stack depth [levels] // TEST file system stack not necessary
 */
#define CONFIG_RTOS_FILE_SYSTEM_STACK_DEPTH             (__OS_FILE_SYSTEM_STACK_DEPTH__)

/**
 * System call thread stack depht [levels]
 */
#define CONFIG_RTOS_SYSCALL_STACK_DEPTH                 (__OS_SYSCALL_STACK_DEPTH__)

/**
 * Interrupt stack depth [levels]
 */
#define CONFIG_RTOS_IRQ_STACK_DEPTH                     (__OS_IRQ_STACK_DEPTH__)

/**
 * Task priorities
 */
#define CONFIG_RTOS_TASK_MAX_PRIORITIES                 (__OS_TASK_MAX_PRIORITIES__)

/**
 * Task name length [bytes]
 */
#define CONFIG_RTOS_TASK_NAME_LEN                       (__OS_TASK_NAME_LEN__)

/**
 * Task schedule frequency [Hz]
 */
#define CONFIG_RTOS_TASK_SCHED_FREQ                     (__OS_TASK_SCHED_FREQ__)

/**
 * Kernel interrupt priority (CPU depend)
 */
#define CONFIG_RTOS_KERNEL_IRQ_PRIORITY                 (_IRQ_RTOS_KERNEL_PRIORITY_)

/**
 * System call interrupt priority (CPU depend)
 */
#define CONFIG_RTOS_SYSCALL_IRQ_PRIORITY                (_IRQ_RTOS_SYSCALL_PRIORITY_)

/**
 * User programs interrupt priority (CPU depend)
 */
#define CONFIG_USER_IRQ_PRIORITY                        (__IRQ_USER_PRIORITY__)

/**
 * Enable (1) or disable (0) CPU low power mode in Idle task
 * Note: if enabled, in some systems debugging and programming could be hindered
 */
#define CONFIG_RTOS_SLEEP_ON_IDLE                       (__OS_SLEEP_ON_IDLE__)


/**=== PRINT CONFIGURATION ===================================================*/
/**
 * Enable (1) or disable (0) printf() family functions
 */
#define CONFIG_PRINTF_ENABLE                            (__OS_PRINTF_ENABLE__)

/**
 * Enable (1) or disable (0) scanf() family functions
 */
#define CONFIG_SCANF_ENABLE                             (__OS_SCANF_ENABLE__)

/**
 * Enable (1) or disable (0) system messages (printk())
 */
#define CONFIG_SYSTEM_MSG_ENABLE                        (__OS_SYSTEM_MSG_ENABLE__)

/**
 * Enable (1) or disable (0) color definitions
 */
#define CONFIG_COLOR_TERMINAL_ENABLE                    (__OS_COLOR_TERMINAL_ENABLE__)

/**
 * Maximum stream buffer size [bytes]
 */
#define CONFIG_STREAM_BUFFER_LENGTH                     (__OS_STREAM_BUFFER_LENGTH__)

/**
 * Maximum pipe length [bytes]
 */
#define CONFIG_PIPE_LENGTH                              (__OS_PIPE_LENGTH__)

/**
 * ERRNO string support (strerror(), perror(), etc)
 * 0: disabled
 * 1: only numbers
 * 2: short names
 * 3: full names
 */
#define CONFIG_ERRNO_STRING_LEN                         (__OS_ERRNO_STRING_LEN__)


/**=== NETWORK CONFIGURATION =================================================*/
/**
 * Enable (1) or disable (0) network interface
 */
#define CONFIG_NETWORK_ENABLE                           (__NETWORK_ENABLE__)


/**=== SYSTEM MONITOR CONFIGURATION ==========================================*/
/**
 * Enable (1) or disable (0) task memory usage monitoring
 */
#define CONFIG_MONITOR_TASK_MEMORY_USAGE                (__OS_MONITOR_TASK_MEMORY_USAGE__)

/**
 * Enable (1) or disable (0) file usage monitoring
 */
#define CONFIG_MONITOR_TASK_FILE_USAGE                  (__OS_MONITOR_TASK_FILE_USAGE__)

/**
 * Enable (1) or disable (0) kernel memory usage monitoring
 */
#define CONFIG_MONITOR_KERNEL_MEMORY_USAGE              (__OS_MONITOR_KERNEL_MEMORY_USAGE__)

/**
 * Enable (1) or disable (0) modules (drivers) memory usage monitoring
 */
#define CONFIG_MONITOR_MODULE_MEMORY_USAGE              (__OS_MONITOR_MODULE_MEMORY_USAGE__)

/**
 * Enable (1) or disable (0) system memory usage monitoring
 */
#define CONFIG_MONITOR_SYSTEM_MEMORY_USAGE              (__OS_MONITOR_SYSTEM_MEMORY_USAGE__)

/**
 * Enable (1) or disable (0) CPU load monitoring
 */
#define CONFIG_MONITOR_CPU_LOAD                         (__OS_MONITOR_CPU_LOAD__)

/**
 * Enable (1) or disable (0) network memory monitoring
 */
#define CONFIG_MONITOR_NETWORK_MEMORY_USAGE             (__OS_MONITOR_NETWORK_MEMORY_USAGE__)

/**
 * Maximum amount of memory that can be used by network [bytes]. Set to 0 to disable limit.
 */
#define CONFIG_MONITOR_NETWORK_MEMORY_USAGE_LIMIT       (__OS_MONITOR_NETWORK_MEMORY_USAGE_LIMIT__)


/**=== SYSTEM ENVIRONMENT CONFIGURATION ======================================*/
/**
 * Define the host name
 */
#define CONFIG_HOSTNAME                                 __OS_HOSTNAME__


/**
 * RTC file
 */
#define CONFIG_RTC_FILE_PATH                            __OS_RTC_FILE_PATH__

/**=== DEVELOPMENT OPTIONS ===================================================*/
/**
 * Enable (1) or disable (0) system assert macro (stops system if condition met)
 */
#define CONFIG_SYSTEM_STOP_MACRO                        (__OS_SYSTEM_STOP_MACRO__)

#endif /* _CONFIG_H_ */
/*==============================================================================
  End of file
==============================================================================*/
