# Makefile for GNU make

####################################################################################################
# INSERT HERE SOURCES NOT ARCHITECTURE DEPENDED
####################################################################################################
CSRC_noarch += $(sort \
   drivers/tty/tty.c \
)

CXXSRC_noarch += $(sort \
)

HDRLOC_noarch += $(sort \
   drivers/tty \
) 

####################################################################################################
# INSERT HERE SOURCES stm32f1 ARCHITECTURE DEPENDED
####################################################################################################
CSRC_stm32f1 += $(sort \
   drivers/gpio/stm32f1/gpio.c \
   drivers/uart/stm32f1/uart.c \
   drivers/pll/stm32f1/pll.c \
   portable/stm32f1/cpuctl.c \
   portable/stm32f1/cpuhooks.c \
   portable/stm32f1/stm32f10x_vectors.c \
   portable/stm32f1/lib/misc.c \
   kernel/FreeRTOS/Source/portable/GCC/ARM_CM3/port.c \
)

CXXSRC_stm32f1 += $(sort \
)

ASRC_stm32f1 += $(sort \
   portable/stm32f1/cmx_startup.s \
)

HDRLOC_stm32f1 += $(sort \
   drivers/gpio \
   drivers/gpio/stm32f1 \
   drivers/pll \
   drivers/uart \
   portable/lib/CMSIS \
   portable/stm32f1 \
)

####################################################################################################
# INSERT HERE SOURCES POSIX ARCHITECTURE DEPENDED
####################################################################################################
CSRC_posix += $(sort \
   drivers/uart/posix/uart.c \
   drivers/pll/posix/pll.c \
   portable/posix/cpuctl.c \
   kernel/FreeRTOS/Source/portable/GCC/Posix/port.c \
)

CXXSRC_posix += $(sort \
)

ASRC_posix += $(sort \
)

HDRLOC_posix += $(sort \
   $(dir $(CSRC_posix)) \
   $(dir $(CXXSRC_posix)) \
   drivers/uart \
   drivers/pll \
)

####################################################################################################
# CORE SOURCES
####################################################################################################
CSRC_CORE += $(sort \
   core/dnx.c \
   core/sysmoni.c \
   core/progman.c \
   core/list.c \
   core/main.c \
   core/memman.c \
   core/io.c \
   core/vfs.c \
   fs/procfs/procfs.c \
   fs/appfs/appfs.c \
   fs/lfs/lfs.c \
   user/initd.c \
   user/regdrv.c \
   user/regfs.c \
   user/regprg.c \
   kernel/khooks.c \
   kernel/kwrapper.c \
   kernel/FreeRTOS/Source/croutine.c \
   kernel/FreeRTOS/Source/list.c \
   kernel/FreeRTOS/Source/queue.c \
   kernel/FreeRTOS/Source/tasks.c \
   kernel/FreeRTOS/Source/timers.c \
   kernel/FreeRTOS/Source/portable/MemMang/heap_3.c \
)

HDRLOC_CORE += $(sort \
   kernel/FreeRTOS/Source/include \
   kernel \
   config \
   portable \
   include \
   include/stdlib \
)