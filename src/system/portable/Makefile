# Makefile for GNU make
HDRLOC_CORE += portable

ifeq ($(TARGET), stm32f1)
ASRC_ARCH   += portable/$(TARGET)/cmx_startup.s
CSRC_ARCH   += portable/stm32f1/cpuctl.c
CSRC_ARCH   += portable/stm32f1/cpuhooks.c
CSRC_ARCH   += portable/stm32f1/stm32f10x_vectors.c
CSRC_ARCH   += portable/stm32f1/lib/misc.c
CSRC_ARCH   += portable/stm32f1/lib/stm32f10x_rcc.c
HDRLOC_ARCH += portable/lib/CMSIS
HDRLOC_ARCH += portable/stm32f1
endif
