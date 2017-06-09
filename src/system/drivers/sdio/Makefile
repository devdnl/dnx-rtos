# Makefile for GNU make
HDRLOC_ARCH += drivers/sdio

ifeq ($(__ENABLE_SDIO__), _YES_)
   ifeq ($(TARGET), stm32f4)
      CSRC_ARCH   += drivers/sdio/$(TARGET)/sdio.c
      CXXSRC_ARCH +=
   endif
endif
