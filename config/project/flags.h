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
#ifndef _FLAGS_H_
#define _FLAGS_H_

/* definition of answers for questions */
#ifndef __NO__
#define __NO__ 0
#endif

#ifndef __YES__
#define __YES__ 1
#endif

/* fixed configuration */
#define stm32f0 0
#define stm32f1 1
#define stm32f2 2
#define stm32f3 3
#define stm32f4 4

/* current CPU architecture */
#define __CPU_ARCH__ stm32f1

/* include specific CPU architecture files */
#if   (__CPU_ARCH__ == stm32f0)
#include "stm32f0/cpu.h"
#elif (__CPU_ARCH__ == stm32f1)
#include "../stm32f1/cpu.h"
#include "../stm32f1/crc_flags.h"
#include "../stm32f1/gpio_flags.h
#include "../stm32f1/ethmac_flags.h"
#elif (__CPU_ARCH__ == stm32f2)
#include "stm32f2/cpu.h"
#elif (__CPU_ARCH__ == stm32f3)
#include "stm32f3/cpu.h"
#elif (__CPU_ARCH__ == stm32f4)
#include "stm32f4/cpu.h"
#endif

/* CPU configuration */
#define __CPU_START_FREQ__ __CPU_START_FREQUENCY__
#define __CPU_OSC_FREQ__ 8000000
#define __CPU_RAM_SIZE__ __CPU_RAM_MEM_SIZE__
#define __IRQ_RTOS_KERNEL_PRIORITY__ 0xFF
#define __IRQ_RTOS_SYSCALL_PRIORITY__ 0xEF
#define __IRQ_USER_PRIORITY__ 0xDF

/* dynamic memory configuration */
#define __HEAP_ALIGN__ __CPU_HEAP_ALIGN__
#define __HEAP_SIZE__ 64332
#define __HEAP_BLOCK_SIZE__ 4

/* os configuration */
#define __OS_TASK_MIN_STACK_DEPTH__ 48
#define __OS_FILE_SYSTEM_STACK_DEPTH__ 48
#define __OS_IRQ_STACK_DEPTH__ 16
#define __OS_TASK_MAX_PRIORITIES__ 7
#define __OS_TASK_NAME_LEN__ 16
#define __OS_TASK_SCHED_FREQ__ 1000
#define __OS_SLEEP_ON_IDLE__ __YES__
#define __OS_PRINTF_ENABLE__ __YES__
#define __OS_SCANF_ENABLE__ __YES__
#define __OS_SYSTEM_MSG_ENABLE__ __YES__
#define __OS_COLOR_TERMINAL_ENABLE__ __YES__
#define __OS_STREAM_BUFFER_LENGTH__ 100
#define __OS_PIPE_LENGTH__ 128
#define __OS_ERRNO_STRING_LEN__ 3
#define __OS_MONITOR_TASK_MEMORY_USAGE__ __YES__
#define __OS_MONITOR_TASK_FILE_USAGE__ __YES__
#define __OS_MONITOR_KERNEL_MEMORY_USAGE__ __YES__
#define __OS_MONITOR_MODULE_MEMORY_USAGE__ __YES__
#define __OS_MONITOR_SYSTEM_MEMORY_USAGE__ __YES__
#define __OS_MONITOR_CPU_LOAD__ __YES__
#define __OS_MONITOR_NETWORK_MEMORY_USAGE__ __YES__
#define __OS_MONITOR_NETWORK_MEMORY_USAGE_LIMIT__ 0
#define __OS_HOSTNAME__ "localhost"
#define __OS_SYSTEM_STOP_MACRO__ __YES__

/* network configuration */
#define __NETWORK_ENABLE__ __YES__
#define __NETWORK_MAC_ADDR_0__ 0x50
#define __NETWORK_MAC_ADDR_1__ 0xE5
#define __NETWORK_MAC_ADDR_2__ 0x49
#define __NETWORK_MAC_ADDR_3__ 0x37
#define __NETWORK_MAC_ADDR_4__ 0xB5
#define __NETWORK_MAC_ADDR_5__ 0xBD
#define __NETWORK_ETHIF_FILE__ "/dev/eth0"

/* file systems */
#define __ENABLE_DEVFS__ __YES__
#define __ENABLE_LFS__ __YES__
#define __ENABLE_FATFS__ __YES__
#define __ENABLE_PROCFS__ __YES__

/* modules */
#define __ENABLE_GPIO__ __NO__
#define __ENABLE_CRC__ __NO__
#define __ENABLE_ETH__ __NO__
#define __ENABLE_PLL__ __NO__
#define __ENABLE_SDSPI__ __NO__
#define __ENABLE_SPI__ __NO__
#define __ENABLE_TTY__ __NO__
#define __ENABLE_UART__ __NO__
#define __ENABLE_WDG__ __NO__
#define __ENABLE_I2S__ __NO__
#define __ENABLE_USB__ __NO__
#define __ENABLE_USBOTG__ __NO__
#define __ENABLE_I2C__ __NO__
#define __ENABLE_ADC__ __NO__
#define __ENABLE_DAC__ __NO__
#define __ENABLE_SDIO__ __NO__
#define __ENABLE_FSMC__ __NO__
#define __ENABLE_FDMC__ __NO__
#define __ENABLE_HDMICEC__ __NO__

#endif /* _FLAGS_H_ */
/*==============================================================================
  End of file
==============================================================================*/
