# Makefile for GNU make
CSRC_CORE   += kernel/syscall.c
CSRC_CORE   += kernel/sysfunc.c
CSRC_CORE   += kernel/process.c
CSRC_CORE   += kernel/time.c
CSRC_CORE   += kernel/khooks.c
CSRC_CORE   += kernel/kwrapper.c
CSRC_CORE   += kernel/kpanic.c
CSRC_CORE   += kernel/printk.c
CSRC_CORE   += kernel/FreeRTOS/Source/croutine.c
CSRC_CORE   += kernel/FreeRTOS/Source/event_groups.c
CSRC_CORE   += kernel/FreeRTOS/Source/list.c
CSRC_CORE   += kernel/FreeRTOS/Source/queue.c
CSRC_CORE   += kernel/FreeRTOS/Source/tasks.c
CSRC_CORE   += kernel/FreeRTOS/Source/timers.c
CSRC_CORE   += kernel/FreeRTOS/Source/portable/MemMang/heap_3.c
HDRLOC_CORE += kernel/FreeRTOS/Source/include
HDRLOC_CORE += kernel/FreeRTOS
HDRLOC_CORE += kernel

ifeq ($(TARGET), stm32f1)
CSRC_ARCH   += kernel/FreeRTOS/Source/portable/GCC/ARM_CM3/port.c
HDRLOC_CORE += kernel/FreeRTOS/Source/portable/GCC/ARM_CM3
endif

ifeq ($(TARGET), stm32f4)
CSRC_ARCH   += kernel/FreeRTOS/Source/portable/GCC/ARM_CM4F/port.c
HDRLOC_CORE += kernel/FreeRTOS/Source/portable/GCC/ARM_CM4F
endif

ifeq ($(TARGET), stm32f7)
CSRC_ARCH   += kernel/FreeRTOS/Source/portable/GCC/ARM_CM4F/port.c
HDRLOC_CORE += kernel/FreeRTOS/Source/portable/GCC/ARM_CM4F
endif

ifeq ($(TARGET), efr32)
CSRC_ARCH   += kernel/FreeRTOS/Source/portable/GCC/ARM_CM7/r0p1/port.c
HDRLOC_CORE += kernel/FreeRTOS/Source/portable/GCC/ARM_CM7/r0p1/
endif
