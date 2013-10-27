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

#ifdef __cplusplus
extern "C" {
#endif

/**=== FREQ CONFIGURATION ====================================================*/
/**
 * CPU frequency [Hz]
 */
#define CONFIG_CPU_TARGET_FREQ                          (72000000UL)


/**=== MEMORY CONFIGURATION ==================================================*/
/**
 * Size of RAM memory [bytes]
 */
#define CONFIG_RAM_SIZE                                 (64 * 1024)

/**
 * Size of dynamic allocated memory (HEAP) [bytes]
 */
#define CONFIG_HEAP_SIZE                                (62 * 1024)

/**
 * Heap memory alignment
 */
#define CONFIG_HEAP_ALIGN                               (4)

/**
 * Heap block size [bytes]
 */
#define CONFIG_HEAP_BLOCK_SIZE                          (16)


/**=== RTOS CONFIGURATION ====================================================*/
/**
 * Task minimal stack depth [levels]
 */
#define CONFIG_RTOS_TASK_MIN_STACK_DEPTH                (48)

/**
 * File system stack depth [levels]
 */
#define CONFIG_RTOS_FILE_SYSTEM_STACK_DEPTH             (32)

/**
 * Interrupt stack depth [levels]
 */
#define CONFIG_RTOS_IRQ_STACK_DEPTH                     (16)

/**
 * Task priorities
 */
#define CONFIG_RTOS_TASK_MAX_PRIORITIES                 (7)

/**
 * Task name length [bytes]
 */
#define CONFIG_RTOS_TASK_NAME_LEN                       (16)

/**
 * Task schedule frequency [Hz]
 */
#define CONFIG_RTOS_TASK_SCHED_FREQ                     (1000)

/**
 * Kernel interrupt priority (CPU depend)
 */
#define CONFIG_RTOS_KERNEL_IRQ_PRIORITY                 (0xFF) /* priority 15 (lowest) */

/**
 * System call interrupt priority (CPU depend)
 */
#define CONFIG_RTOS_SYSCALL_IRQ_PRIORITY                (0xEF) /* priority 14 */

/**
 * User programs interrupt priority (CPU depend)
 */
#define CONFIG_USER_IRQ_PRIORITY                        (0xDF) /* priority 13 */

/**
 * Enable (1) or disable (0) CPU low power mode in Idle task
 * Note: if enabled, in some systems debugging and programming could be hindered
 */
#define CONFIG_RTOS_SLEEP_ON_IDLE                       (0)


/**=== PRINT CONFIGURATION ===================================================*/
/**
 * Enable (1) or disable (0) printf() family functions
 */
#define CONFIG_PRINTF_ENABLE                            (1)

/**
 * Enable (1) or disable (0) scanf() family functions
 */
#define CONFIG_SCANF_ENABLE                             (1)

/**
 * Enable (1) or disable (0) system messages (printk())
 */
#define CONFIG_SYSTEM_MSG_ENABLE                        (1)

/**
 * Enable (1) or disable (0) color definitions
 */
#define CONFIG_COLOR_TERMINAL_ENABLE                    (1)

/**
 * Maximum stream buffer size
 */
#define CONFIG_FSCANF_STREAM_BUFFER_SIZE                (100)


/**=== SYSTEM MONITOR CONFIGURATION ==========================================*/
/**
 * Enable (1) or disable (0) task memory usage monitoring
 */
#define CONFIG_MONITOR_TASK_MEMORY_USAGE                (1)

/**
 * Enable (1) or disable (0) file usage monitoring
 */
#define CONFIG_MONITOR_TASK_FILE_USAGE                  (1)

/**
 * Enable (1) or disable (0) kernel memory usage monitoring
 */
#define CONFIG_MONITOR_KERNEL_MEMORY_USAGE              (1)

/**
 * Enable (1) or disable (0) modules (drivers) memory usage monitoring
 */
#define CONFIG_MONITOR_MODULE_MEMORY_USAGE              (1)

/**
 * Enable (1) or disable (0) system memory usage monitoring
 */
#define CONFIG_MONITOR_SYSTEM_MEMORY_USAGE              (1)

/**
 * Enable (1) or disable (0) CPU load monitoring
 */
#define CONFIG_MONITOR_CPU_LOAD                         (1)


/**=== SYSTEM ENVIRONMENT CONFIGURATION ======================================*/
/**
 * Define the host name
 */
#define CONFIG_HOSTNAME                                 "localhost"


/**=== NETWORK CONFIGURATION =================================================*/
/**
 * Enable (1) or disable (0) network interface
 */
#define CONFIG_NETWORK_ENABLE                           (1)


/**=== DEVELOPMENT OPTIONS ===================================================*/
/**
 * Enable (1) or disable (0) system assert macro (stop system if condition met)
 */
#define CONFIG_SYSTEM_STOP_MACRO                        (1)


#ifdef __cplusplus
}
#endif

#endif /* _CONFIG_H_ */
/*==============================================================================
  End of file
==============================================================================*/
