# Makefile for GNU make
HDRLOC_ARCH += drivers/timenc

ifeq ($(__ENABLE_TIMENC__), _YES_)
   ifeq ($(TARGET), stm32f7)
      CSRC_ARCH   += drivers/timenc/$(TARGET)/timenc.c
      CXXSRC_ARCH +=
   endif
endif
