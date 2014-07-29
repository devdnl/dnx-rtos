/*=========================================================================*//**
@file    cpu_flags.h

@author  Daniel Zorychta

@brief   CPU module configuration.

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
#ifndef _CPU_H_
#define _CPU_H_

/* configuration to modify */
#define __CPU_NAME__ STM32F107RCxx
#define __CPU_FAMILY__ __STM32F10X_CL__

/* fixed configuration */
#define __CPU_START_FREQUENCY__           (8000000UL)
#define __CPU_HEAP_ALIGN__                (4)
#define __CPU_IRQ_RTOS_KERNEL_PRIORITY__  (0xFF)
#define __CPU_IRQ_RTOS_SYSCALL_PRIORITY__ (0xEF)
#define ARCH_stm32f1

/* current CPU family definitions */
#define __STM32F10X_LD_VL__ 0
#define __STM32F10X_MD_VL__ 1
#define __STM32F10X_HD_VL__ 2
#define __STM32F10X_LD__    3
#define __STM32F10X_MD__    4
#define __STM32F10X_HD__    5
#define __STM32F10X_XL__    6
#define __STM32F10X_CL__    7

#if   (__CPU_FAMILY__ == __STM32F10X_LD_VL__)
#define STM32F10X_LD_VL
#elif (__CPU_FAMILY__ == __STM32F10X_MD_VL__)
#define STM32F10X_MD_VL
#elif (__CPU_FAMILY__ == __STM32F10X_HD_VL__)
#define STM32F10X_HD_VL
#elif (__CPU_FAMILY__ == __STM32F10X_LD__)
#define STM32F10X_LD
#elif (__CPU_FAMILY__ == __STM32F10X_MD__)
#define STM32F10X_MD
#elif (__CPU_FAMILY__ == __STM32F10X_HD__)
#define STM32F10X_HD
#elif (__CPU_FAMILY__ == __STM32F10X_XL__)
#define STM32F10X_XL
#elif (__CPU_FAMILY__ == __STM32F10X_CL__)
#define STM32F10X_CL
#else
#error Wrong CPU family
#endif

#endif /* _CONFIG_H_ */
/*==============================================================================
  End of file
==============================================================================*/
