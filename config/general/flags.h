/*==============================================================================
  Header configured by tools
==============================================================================*/
#ifndef _FLAGS_H_
#define _FLAGS_H_

/* definition of answers for questions */
#ifndef NO
#define NO 0
#endif

#ifndef YES
#define YES 1
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
#elif (__CPU_ARCH__ == stm32f2)
#include "stm32f2/cpu.h"
#elif (__CPU_ARCH__ == stm32f3)
#include "stm32f3/cpu.h"
#elif (__CPU_ARCH__ == stm32f4)
#include "stm32f4/cpu.h"
#endif

/* CPU configuration */
#define __CPU_START_FREQ__ __CPU_START_FREQUENCY__
#define __CPU_RAM_SIZE__ __CPU_RAM_MEM_SIZE__
#define __CPU_OSC_FREQ__ 8000000
#define __IRQ_RTOS_KERNEL_PRIORITY__ 0xFF
#define __IRQ_RTOS_SYSCALL_PRIORITY__ 0xEF
#define __IRQ_USER_PRIORITY__ 0xDF

/* dynamic memory configuration */
#define __HEAP_ALIGN __CPU_HEAP_ALIGN__
#define __HEAP_SIZE (64332)
#define __HEAP_BLOCK_SIZE (4)

/* os configuration */
#define __OS_TASK_MIN_STACK_DEPTH__ 48
#define __OS_FILE_SYSTEM_STACK_DEPTH__ 48
#define __OS_IRQ_STACK_DEPTH__ 16
#define __OS_TASK_MAX_PRIORITIES__ 7
#define __OS_TASK_NAME_LEN__ 16
#define __OS_TASK_SCHED_FREQ__ 1000
#define __OS_SLEEP_ON_IDLE__ NO
#define __OS_PRINTF_ENABLE__ YES
#define __OS_SCANF_ENABLE__ YES
#define __OS_SYSTEM_MSG_ENABLE__ YES
#define __OS_COLOR_TERMINAL_ENABLE__ YES
#define __OS_STREAM_BUFFER_LENGTH__ 100
#define __OS_PIPE_LENGTH__ 128
#define __OS_ERRNO_STRING_LEN__ 3
#define __OS_MONITOR_TASK_MEMORY_USAGE__ YES
#define __OS_MONITOR_TASK_FILE_USAGE__ YES
#define __OS_MONITOR_KERNEL_MEMORY_USAGE__ YES
#define __OS_MONITOR_MODULE_MEMORY_USAGE__ YES
#define __OS_MONITOR_SYSTEM_MEMORY_USAGE__ YES
#define __OS_MONITOR_CPU_LOAD__ YES
#define __OS_MONITOR_NETWORK_MEMORY_USAGE__ YES
#define __OS_MONITOR_NETWORK_MEMORY_USAGE_LIMIT__ 0
#define __OS_HOSTNAME__ "localhost"
#define __OS_SYSTEM_STOP_MACRO__ YES

/* network configuration */
#define __NETWORK_ENABLE__                           (1)
#define __NETWORK_MAC_ADDR_0__                        0x50
#define __NETWORK_MAC_ADDR_1__                        0xE5
#define __NETWORK_MAC_ADDR_2__                        0x49
#define __NETWORK_MAC_ADDR_3__                        0x37
#define __NETWORK_MAC_ADDR_4__                        0xB5
#define __NETWORK_MAC_ADDR_5__                        0xBD

/* file systems */
#define __ENABLE_DEVFS__ YES
#define __ENABLE_LFS__ YES
#define __ENABLE_FATFS__ YES
#define __ENABLE_PROCFS__ YES

/* modules */
#define __ENABLE_CRC__ YES
#define __ENABLE_ETHMAC__ YES
#define __ENABLE_PLL__ YES
#define __ENABLE_SDSPI__ YES
#define __ENABLE_SPI__ YES
#define __ENABLE_TTY__ YES
#define __ENABLE_UART__ YES
#define __ENABLE_WDG__ YES

#endif /* _FLAGS_H_ */
/*==============================================================================
  End of file
==============================================================================*/
