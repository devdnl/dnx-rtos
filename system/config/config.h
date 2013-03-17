#ifndef CONFIG_H_
#define CONFIG_H_
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
#define CONFIG_HEAP_SIZE                                (63 * 1024)

/**
 * Heap memory alignment
 */
#define CONFIG_HEAP_ALIGN                               (4)

/**
 * Heap block size
 */
#define CONFIG_HEAP_BLOCK_SIZE                          (16)


/**=== RTOS CONFIGURATION ====================================================*/
/**
 * Task priorities
 */
#define CONFIG_RTOS_TASK_MAX_PRIORITIES                 (7)

/**
 * Task minimal stack depth
 */
#define CONFIG_RTOS_TASK_MIN_STACK_DEPTH                (64)

/**
 * Task name length
 */
#define CONFIG_RTOS_TASK_NAME_LEN                       (16)

/**
 * Kernel interrupt priority
 */
#define CONFIG_RTOS_KERNEL_IRQ_PRIO                     (255)

/**
 * System call interrupt priority
 */
#define CONFIG_RTOS_SYSCALL_IRQ_PRIO                    (191) /* equivalent to 0xb0, or priority 11. */

/**
 * Library kernel interrupt priority
 */
#define CONFIG_RTOS_LIB_KERNEL_IRQ_PRIO                 (15)


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
 * Enable (1) or disable (0) system messages
 */
#define CONFIG_SYSTEM_MSG_ENABLE                        (1)

/**
 * Enable (1) or disable (0) color definitions
 */
#define CONFIG_COLOR_TERM_ENABLE                        (1)

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
 * Enable (1) or disable (0) kernel memory usage monitoring
 */
#define CONFIG_MONITOR_KERNEL_MEMORY_USAGE              (1)

/**
 * Enable (1) or disable (0) system memory usage monitoring
 */
#define CONFIG_MONITOR_SYSTEM_MEMORY_USAGE              (1)

/**
 * Enable (1) or disable (0) file systems memory usage monitoring
 */
#define CONFIG_MONITOR_FILE_SYSTEM_MEMORY_USAGE         (1)

/**
 * Enable (1) or disable (0) file usage monitoring
 */
#define CONFIG_MONITOR_FILE_USAGE                       (1)

/**
 * Enable (1) or disable (0) CPU load monitoring
 */
#define CONFIG_MONITOR_CPU_LOAD                         (1)

/**=== SYSTEM ENVIRONMENT CONFIGURATION ======================================*/
/**
 * Define the host name
 */
#define CONFIG_HOSTNAME                                 "localhost"

#ifdef __cplusplus
}
#endif

#endif /* CONFIG_H_ */
/*==============================================================================
  End of file
==============================================================================*/
