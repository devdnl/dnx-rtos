# Makefile for GNU make
HDRLOC_ARCH += drivers/wdg

ifeq ($(__ENABLE_WDG__), _YES_)
   ifeq ($(TARGET), stm32f1)
      CSRC_ARCH   += drivers/wdg/$(TARGET)/wdg.c
   	CXXSRC_ARCH +=
   endif
endif
