# Makefile for GNU make
CSRC_CORE   += kernel/khooks.c
CSRC_CORE   += kernel/kwrapper.c
CSRC_CORE   += kernel/FreeRTOS/Source/croutine.c
CSRC_CORE   += kernel/FreeRTOS/Source/event_groups.c
CSRC_CORE   += kernel/FreeRTOS/Source/list.c
CSRC_CORE   += kernel/FreeRTOS/Source/queue.c
CSRC_CORE   += kernel/FreeRTOS/Source/tasks.c
CSRC_CORE   += kernel/FreeRTOS/Source/timers.c
CSRC_CORE   += kernel/FreeRTOS/Source/portable/MemMang/heap_3.c
HDRLOC_CORE += kernel/FreeRTOS/Source/include
HDRLOC_CORE += kernel

ifeq ($(TARGET), stm32f1)
CSRC_ARCH   += kernel/FreeRTOS/Source/portable/GCC/ARM_CM3/port.c
endif
