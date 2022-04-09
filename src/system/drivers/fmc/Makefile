# Makefile for GNU make
HDRLOC_ARCH += drivers/fmc

ifeq ($(__ENABLE_FMC__), _YES_)
   ifeq ($(TARGET), stm32f4)
      CSRC_ARCH   += drivers/fmc/stm32fx/fmc.c
      CXXSRC_ARCH +=
   endif
   ifeq ($(TARGET), stm32f7)
      CSRC_ARCH   += drivers/fmc/stm32fx/fmc.c
      CXXSRC_ARCH +=
   endif
   ifeq ($(TARGET), stm32h7)
      CSRC_ARCH   += drivers/fmc/stm32fx/fmc.c
      CXXSRC_ARCH +=
   endif
endif
