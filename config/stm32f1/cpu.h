/*==============================================================================
  Header configured by tools
==============================================================================*/
#ifndef _CPU_H_
#define _CPU_H_

/* configuration to modify */
#define __CPU_NAME__ STM32F107RCxx

/* fixed configuration */
#define __CPU_START_FREQUENCY__ (8000000UL)
#define __CPU_HEAP_ALIGN__      (4)
#define ARCH_stm32f1

#define STM32F100C4xx 1
#define STM32F100C6xx 2
#define STM32F100C8xx 3
#define STM32F100R6xx 4
#define STM32F100RBxx 5
#define STM32F100RExx 6
#define STM32F100VBxx 7
#define STM32F100ZCxx 8
#define STM32F101C6xx 9
#define STM32F101C8xx 10
#define STM32F101CBxx 11
#define STM32F101R6xx 12
#define STM32F101R8xx 13
#define STM32F101RBxx 14
#define STM32F101V8xx 15
#define STM32F101VBxx 16
#define STM32F103C6xx 17
#define STM32F103C8xx 18
#define STM32F103CBxx 19
#define STM32F103R4xx 20
#define STM32F103R6xx 21
#define STM32F103R8xx 22
#define STM32F103RBxx 23
#define STM32F103RCxx 24
#define STM32F103RDxx 25
#define STM32F103RExx 26
#define STM32F103RGxx 27
#define STM32F103T8xx 28
#define STM32F103V8xx 29
#define STM32F103VBxx 30
#define STM32F103VCxx 31
#define STM32F103VDxx 32
#define STM32F103VExx 33
#define STM32F103VGxx 34
#define STM32F103ZCxx 35
#define STM32F103ZExx 36
#define STM32F103ZGxx 37
#define STM32F105RBxx 38
#define STM32F105RCxx 39
#define STM32F105VBxx 40
#define STM32F105VCxx 41
#define STM32F107RCxx 42
#define STM32F107VBxx 43
#define STM32F107VCxx 44

#if   (__CPU_NAME__ == STM32F100C4xx)
#define __CPU_RAM_MEM_SIZE__
#define STM32F10X_LD_VL
#elif (__CPU_NAME__ == STM32F100C6xx)
#define __CPU_RAM_MEM_SIZE__
#define STM32F10X_LD_VL
#elif (__CPU_NAME__ == STM32F100C8xx)
#define __CPU_RAM_MEM_SIZE__
#define STM32F10X_MD_VL
#elif (__CPU_NAME__ == STM32F100R6xx)
#define __CPU_RAM_MEM_SIZE__
#define STM32F10X_LD_VL
#elif (__CPU_NAME__ == STM32F100RBxx)
#define __CPU_RAM_MEM_SIZE__
#define STM32F10X_MD_VL
#elif (__CPU_NAME__ == STM32F100RExx)
#define __CPU_RAM_MEM_SIZE__
#define STM32F10X_HD_VL
#elif (__CPU_NAME__ == STM32F100VBxx)
#define __CPU_RAM_MEM_SIZE__
#define STM32F10X_MD_VL
#elif (__CPU_NAME__ == STM32F100ZCxx)
#define __CPU_RAM_MEM_SIZE__
#define STM32F10X_HD_VL
#elif (__CPU_NAME__ == STM32F101C6xx)
#define __CPU_RAM_MEM_SIZE__
#define STM32F10X_LD
#elif (__CPU_NAME__ == STM32F101C8xx)
#define __CPU_RAM_MEM_SIZE__
#define STM32F10X_MD
#elif (__CPU_NAME__ == STM32F101CBxx)
#define __CPU_RAM_MEM_SIZE__
#define STM32F10X_MD
#elif (__CPU_NAME__ == STM32F101R6xx)
#define __CPU_RAM_MEM_SIZE__
#define STM32F10X_LD
#elif (__CPU_NAME__ == STM32F101R8xx)
#define __CPU_RAM_MEM_SIZE__
#define STM32F10X_MD
#elif (__CPU_NAME__ == STM32F101RBxx)
#define __CPU_RAM_MEM_SIZE__
#define STM32F10X_MD
#elif (__CPU_NAME__ == STM32F101V8xx)
#define __CPU_RAM_MEM_SIZE__
#define STM32F10X_MD
#elif (__CPU_NAME__ == STM32F101VBxx)
#define __CPU_RAM_MEM_SIZE__
#define STM32F10X_MD
#elif (__CPU_NAME__ == STM32F103C6xx)
#define __CPU_RAM_MEM_SIZE__
#define STM32F10X_LD
#elif (__CPU_NAME__ == STM32F103C8xx)
#define __CPU_RAM_MEM_SIZE__
#define STM32F10X_MD
#elif (__CPU_NAME__ == STM32F103CBxx)
#define __CPU_RAM_MEM_SIZE__
#define STM32F10X_MD
#elif (__CPU_NAME__ == STM32F103R4xx)
#define __CPU_RAM_MEM_SIZE__
#define STM32F10X_LD
#elif (__CPU_NAME__ == STM32F103R6xx)
#define __CPU_RAM_MEM_SIZE__
#define STM32F10X_LD
#elif (__CPU_NAME__ == STM32F103R8xx)
#define __CPU_RAM_MEM_SIZE__
#define STM32F10X_MD
#elif (__CPU_NAME__ == STM32F103RBxx)
#define __CPU_RAM_MEM_SIZE__
#define STM32F10X_MD
#elif (__CPU_NAME__ == STM32F103RCxx)
#define __CPU_RAM_MEM_SIZE__
#define STM32F10X_HD
#elif (__CPU_NAME__ == STM32F103RDxx)
#define __CPU_RAM_MEM_SIZE__
#define STM32F10X_HD
#elif (__CPU_NAME__ == STM32F103RExx)
#define __CPU_RAM_MEM_SIZE__
#define STM32F10X_HD
#elif (__CPU_NAME__ == STM32F103RGxx)
#define __CPU_RAM_MEM_SIZE__
#define STM32F10X_XL
#elif (__CPU_NAME__ == STM32F103T8xx)
#define __CPU_RAM_MEM_SIZE__
#define STM32F10X_MD
#elif (__CPU_NAME__ == STM32F103V8xx)
#define __CPU_RAM_MEM_SIZE__
#define STM32F10X_MD
#elif (__CPU_NAME__ == STM32F103VBxx)
#define __CPU_RAM_MEM_SIZE__
#define STM32F10X_MD
#elif (__CPU_NAME__ == STM32F103VCxx)
#define __CPU_RAM_MEM_SIZE__
#define STM32F10X_HD
#elif (__CPU_NAME__ == STM32F103VDxx)
#define __CPU_RAM_MEM_SIZE__
#define STM32F10X_HD
#elif (__CPU_NAME__ == STM32F103VExx)
#define __CPU_RAM_MEM_SIZE__
#define STM32F10X_HD
#elif (__CPU_NAME__ == STM32F103VGxx)
#define __CPU_RAM_MEM_SIZE__
#define STM32F10X_XL
#elif (__CPU_NAME__ == STM32F103ZCxx)
#define __CPU_RAM_MEM_SIZE__
#define STM32F10X_HD
#elif (__CPU_NAME__ == STM32F103ZExx)
#define __CPU_RAM_MEM_SIZE__
#define STM32F10X_HD
#elif (__CPU_NAME__ == STM32F103ZGxx)
#define __CPU_RAM_MEM_SIZE__
#define STM32F10X_XL
#elif (__CPU_NAME__ == STM32F105RBxx)
#define __CPU_RAM_MEM_SIZE__
#define STM32F10X_CL
#elif (__CPU_NAME__ == STM32F105RCxx)
#define __CPU_RAM_MEM_SIZE__
#define STM32F10X_CL
#elif (__CPU_NAME__ == STM32F105VBxx)
#define __CPU_RAM_MEM_SIZE__
#define STM32F10X_CL
#elif (__CPU_NAME__ == STM32F105VCxx)
#define __CPU_RAM_MEM_SIZE__
#define STM32F10X_CL
#elif (__CPU_NAME__ == STM32F107RCxx)
#define __CPU_RAM_MEM_SIZE__ 65536
#define STM32F10X_CL
#elif (__CPU_NAME__ == STM32F107VBxx)
#define __CPU_RAM_MEM_SIZE__
#define STM32F10X_CL
#elif (__CPU_NAME__ == STM32F107VCxx)
#define __CPU_RAM_MEM_SIZE__
#define STM32F10X_CL
#else
#error Select supported microcontroller!
#endif

#endif /* _CONFIG_H_ */
/*==============================================================================
  End of file
==============================================================================*/
