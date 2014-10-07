/*=========================================================================*//**
@file    flags.h

@author  Daniel Zorychta

@brief   Global project flags which are included from compiler command line

@note    Copyright (C) 2014 Daniel Zorychta <daniel.zorychta@gmail.com>

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

/*
 * NOTE: All flags defined as: __FLAG_NAME__ (with doubled underscore as suffix
 *       and prefix) are exported to the single configuration file
 *       (by using Configtool) when entire project configuration is exported.
 *       All other flag definitions and statements are ignored.
 */


#ifndef _FLAGS_H_
#define _FLAGS_H_

/* definition of answers for questions */
#define _NO_  0
#define _YES_ 1

/* fixed configuration */
#define stm32f1 0
#define stm32f2 1
#define stm32f3 2
#define stm32f4 3

/* current CPU architecture */
#define __CPU_ARCH__ stm32f1
#define noarch __CPU_ARCH__ /* always valid */
#define ARCH_noarch

/* include modules flags divided to architecture */
#if (__CPU_ARCH__ == noarch)
#       include "../noarch/loop_flags.h"
#       include "../noarch/tty_flags.h"
#       include "../noarch/sdspi_flags.h"
#endif
#if (__CPU_ARCH__ == stm32f1)
#       include "../stm32f1/cpu.h"
#       include "../stm32f1/crc_flags.h"
#       include "../stm32f1/gpio_flags.h"
#       include "../stm32f1/afio_flags.h"
#       include "../stm32f1/eth_flags.h"
#       include "../stm32f1/pll_flags.h"
#       include "../stm32f1/spi_flags.h"
#       include "../stm32f1/wdg_flags.h"
#       include "../stm32f1/uart_flags.h"
#       include "../stm32f1/usb_flags.h"
#       include "../stm32f1/irq_flags.h"
#       include "../stm32f1/i2c_flags.h"
#elif (__CPU_ARCH__ == stm32f2)
#       include "stm32f2/cpu.h"
#elif (__CPU_ARCH__ == stm32f3)
#       include "stm32f3/cpu.h"
#elif (__CPU_ARCH__ == stm32f4)
#       include "stm32f4/cpu.h"
#endif

/* CPU configuration */
#define  _CPU_START_FREQ_ _CPU_START_FREQUENCY_
#define __CPU_OSC_FREQ__ 8000000
#define  _IRQ_RTOS_KERNEL_PRIORITY_ _CPU_IRQ_RTOS_KERNEL_PRIORITY_
#define  _IRQ_RTOS_SYSCALL_PRIORITY_  _CPU_IRQ_RTOS_SYSCALL_PRIORITY_
#define __IRQ_USER_PRIORITY__ 0xDF

/* dynamic memory configuration */
#define  _HEAP_ALIGN_ _CPU_HEAP_ALIGN_
#define __HEAP_BLOCK_SIZE__ 4

/* os configuration */
#define __OS_TASK_MIN_STACK_DEPTH__ 48
#define __OS_FILE_SYSTEM_STACK_DEPTH__ 48
#define __OS_IRQ_STACK_DEPTH__ 16
#define __OS_TASK_MAX_PRIORITIES__ 7
#define __OS_TASK_NAME_LEN__ 16
#define __OS_TASK_SCHED_FREQ__ 1000
#define __OS_SLEEP_ON_IDLE__ _NO_
#define __OS_PRINTF_ENABLE__ _YES_
#define __OS_SCANF_ENABLE__ _YES_
#define __OS_SYSTEM_MSG_ENABLE__ _YES_
#define __OS_COLOR_TERMINAL_ENABLE__ _YES_
#define __OS_STREAM_BUFFER_LENGTH__ 100
#define __OS_PIPE_LENGTH__ 128
#define __OS_ERRNO_STRING_LEN__ 3
#define __OS_MONITOR_TASK_MEMORY_USAGE__ _YES_
#define __OS_MONITOR_TASK_FILE_USAGE__ _YES_
#define __OS_MONITOR_KERNEL_MEMORY_USAGE__ _YES_
#define __OS_MONITOR_MODULE_MEMORY_USAGE__ _YES_
#define __OS_MONITOR_SYSTEM_MEMORY_USAGE__ _YES_
#define __OS_MONITOR_CPU_LOAD__ _YES_
#define __OS_MONITOR_NETWORK_MEMORY_USAGE__ _YES_
#define __OS_MONITOR_NETWORK_MEMORY_USAGE_LIMIT__ 0
#define __OS_HOSTNAME__ "localhost"
#define __OS_SYSTEM_STOP_MACRO__ _YES_

/* network configuration */
#define __NETWORK_ENABLE__ _YES_
#define __NETWORK_MAC_ADDR_0__ 0x50
#define __NETWORK_MAC_ADDR_1__ 0xE5
#define __NETWORK_MAC_ADDR_2__ 0x49
#define __NETWORK_MAC_ADDR_3__ 0x37
#define __NETWORK_MAC_ADDR_4__ 0xB5
#define __NETWORK_MAC_ADDR_5__ 0xBD
#define __NETWORK_ETHIF_FILE__ "/dev/eth0"

/* file systems */
#define __ENABLE_DEVFS__ _YES_
#define __ENABLE_LFS__ _YES_
#define __ENABLE_FATFS__ _YES_
#define __ENABLE_PROCFS__ _YES_
#define __FATFS_LFN_ENABLE__ _YES_
#define __FATFS_LFN_CODEPAGE__ 852

/* modules */
#define __ENABLE_LOOP__ _YES_
#define __ENABLE_GPIO__ _YES_
#define __ENABLE_AFIO__ _YES_
#define __ENABLE_CRC__ _YES_
#define __ENABLE_ETH__ _YES_
#define __ENABLE_PLL__ _YES_
#define __ENABLE_SDSPI__ _YES_
#define __ENABLE_SPI__ _YES_
#define __ENABLE_TTY__ _YES_
#define __ENABLE_UART__ _YES_
#define __ENABLE_WDG__ _YES_
#define __ENABLE_USB__ _NO_
#define __ENABLE_I2C__ _YES_
#define __ENABLE_IRQ__ _YES_

#endif /* _FLAGS_H_ */
/*==============================================================================
  End of file
==============================================================================*/
