# Makefile for GNU make
HDRLOC_CORE += cpu

ifeq ($(TARGET), stm32f1)
ASRC_ARCH   += cpu/stm32f1/cmx_startup.s
CSRC_ARCH   += cpu/stm32f1/cpuctl.c
CSRC_ARCH   += cpu/stm32f1/stm32f10x_vectors.c
CSRC_ARCH   += cpu/stm32f1/lib/misc.c
CSRC_ARCH   += cpu/stm32f1/lib/stm32f10x_rcc.c
HDRLOC_ARCH += cpu/stm32f1
HDRLOC_ARCH += cpu/lib/CMSIS
endif

ifeq ($(TARGET), stm32f4)
ASRC_ARCH   += cpu/stm32f4/cmx_startup.s
CSRC_ARCH   += cpu/stm32f4/cpuctl.c
CSRC_ARCH   += cpu/stm32f4/stm32f4xx_vectors.c
CSRC_ARCH   += cpu/stm32f4/lib/misc.c
CSRC_ARCH   += cpu/stm32f4/lib/stm32f4xx_rcc.c
HDRLOC_ARCH += cpu/stm32f4/stm32f4xx
HDRLOC_ARCH += cpu/stm32f4
HDRLOC_ARCH += cpu/lib/CMSIS
endif

ifeq ($(TARGET), stm32f7)
ASRC_ARCH   += cpu/stm32f7/cmx_startup.s
CSRC_ARCH   += cpu/stm32f7/cpuctl.c
CSRC_ARCH   += cpu/stm32f7/stm32f7xx_vectors.c
CSRC_ARCH   += cpu/stm32f7/lib/misc.c
CSRC_ARCH   += cpu/stm32f7/lib/stm32f7xx_ll_rcc.c
HDRLOC_ARCH += cpu/stm32f7/stm32f7xx
HDRLOC_ARCH += cpu/stm32f7
HDRLOC_ARCH += cpu/lib/CMSIS
endif

ifeq ($(TARGET), efr32)
ASRC_ARCH   += cpu/efr32/cmx_startup.s
CSRC_ARCH   += cpu/efr32/cpuctl.c
CSRC_ARCH   += cpu/efr32/efr32xx_vectors.c
CSRC_ARCH   += cpu/efr32/lib/em_cmu.c
CSRC_ARCH   += cpu/efr32/lib/em_assert.c
CSRC_ARCH   += cpu/efr32/lib/em_system.c
CSRC_ARCH   += cpu/efr32/lib/system_efr32mg.c
HDRLOC_ARCH += cpu/efr32/efr32xx
HDRLOC_ARCH += cpu/efr32
HDRLOC_ARCH += cpu/efr32/lib
HDRLOC_ARCH += cpu/lib/CMSIS
endif
