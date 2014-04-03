/*==============================================================================
  Header configured by tools
==============================================================================*/
#ifndef _CPU_H_
#define _CPU_H_

/* configuration to modify */
#define __CPU_NAME__ STM32F100C4xx
#define __CPU_FAMILY__ __STM32F10X_LD_VL__
#define __CPU_RAM_MEM_SIZE__ 4096

/* fixed configuration */
#define __CPU_START_FREQUENCY__ (8000000UL)
#define __CPU_HEAP_ALIGN__      (4)
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
