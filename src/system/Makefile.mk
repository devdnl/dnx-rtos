# Makefile for GNU make

####################################################################################################
# INSERT HERE SOURCES NOT ARCHITECTURE DEPENDED
####################################################################################################
CSRC_noarch += drivers/tty/tty.c
  
CXXSRC_noarch += 

HDRLOC_noarch += drivers/tty

####################################################################################################
# INSERT HERE SOURCES stm32f1 ARCHITECTURE DEPENDED
####################################################################################################
CSRC_stm32f1 += drivers/gpio/stm32f1/gpio.c
CSRC_stm32f1 += drivers/uart/stm32f1/uart.c
CSRC_stm32f1 += drivers/pll/stm32f1/pll.c
CSRC_stm32f1 += drivers/sdspi/stm32f1/sdspi.c
CSRC_stm32f1 += portable/stm32f1/cpuctl.c
CSRC_stm32f1 += portable/stm32f1/cpuhooks.c
CSRC_stm32f1 += portable/stm32f1/stm32f10x_vectors.c
CSRC_stm32f1 += portable/stm32f1/lib/misc.c
CSRC_stm32f1 += kernel/FreeRTOS/Source/portable/GCC/ARM_CM3/port.c
  
CXXSRC_stm32f1 += 

ASRC_stm32f1 += portable/stm32f1/cmx_startup.s
  
HDRLOC_stm32f1 += drivers/gpio
HDRLOC_stm32f1 += drivers/gpio/stm32f1
HDRLOC_stm32f1 += drivers/pll
HDRLOC_stm32f1 += drivers/uart
HDRLOC_stm32f1 += drivers/sdspi
HDRLOC_stm32f1 += portable/lib/CMSIS
HDRLOC_stm32f1 += portable/stm32f1

####################################################################################################
# INSERT HERE SOURCES POSIX ARCHITECTURE DEPENDED
####################################################################################################
CSRC_posix += drivers/uart/posix/uart.c
CSRC_posix += drivers/pll/posix/pll.c
CSRC_posix += portable/posix/cpuctl.c
CSRC_posix += kernel/FreeRTOS/Source/portable/GCC/Posix/port.c

CXXSRC_posix += 

ASRC_posix += 

HDRLOC_posix += $(dir $(CSRC_posix))
HDRLOC_posix += $(dir $(CXXSRC_posix))
HDRLOC_posix += drivers/uart
HDRLOC_posix += drivers/pll

####################################################################################################
# CORE SOURCES
####################################################################################################
CSRC_CORE += core/dnx.c
CSRC_CORE += core/sysmoni.c
CSRC_CORE += core/progman.c
CSRC_CORE += core/list.c
CSRC_CORE += core/main.c
CSRC_CORE += core/memman.c
CSRC_CORE += core/io.c
CSRC_CORE += core/vfs.c
CSRC_CORE += fs/procfs/procfs.c
CSRC_CORE += fs/appfs/appfs.c
CSRC_CORE += fs/lfs/lfs.c
CSRC_CORE += fs/fatfs/fatfs.c
CSRC_CORE += fs/fatfs/libfat/libfat_user.c
CSRC_CORE += fs/fatfs/libfat/libfat.c
CSRC_CORE += fs/fatfs/libfat/libfat_unicode.c
CSRC_CORE += user/initd.c
CSRC_CORE += user/regdrv.c
CSRC_CORE += user/regfs.c
CSRC_CORE += user/regprg.c
CSRC_CORE += kernel/khooks.c
CSRC_CORE += kernel/kwrapper.c
CSRC_CORE += kernel/FreeRTOS/Source/croutine.c
CSRC_CORE += kernel/FreeRTOS/Source/list.c
CSRC_CORE += kernel/FreeRTOS/Source/queue.c
CSRC_CORE += kernel/FreeRTOS/Source/tasks.c
CSRC_CORE += kernel/FreeRTOS/Source/timers.c
CSRC_CORE += kernel/FreeRTOS/Source/portable/MemMang/heap_3.c

HDRLOC_CORE += kernel/FreeRTOS/Source/include
HDRLOC_CORE += kernel
HDRLOC_CORE += config
HDRLOC_CORE += portable
HDRLOC_CORE += include
HDRLOC_CORE += include/stdlib

