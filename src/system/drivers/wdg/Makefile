# Makefile for GNU make
HDRLOC_ARCH += drivers/wdg

ifeq ($(__ENABLE_WDG__), _YES_)
   ifeq ($(TARGET), stm32f1)
      CSRC_ARCH   += drivers/wdg/stm32fx/wdg.c
      CXXSRC_ARCH +=
   endif
   ifeq ($(TARGET), stm32f4)
      CSRC_ARCH   += drivers/wdg/stm32fx/wdg.c
      CXXSRC_ARCH +=
	endif
   ifeq ($(TARGET), stm32f7)
      CSRC_ARCH   += drivers/wdg/stm32fx/wdg.c
      CXXSRC_ARCH +=
	endif
endif
