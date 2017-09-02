# Makefile for GNU make
HDRLOC_ARCH += drivers/dci

ifeq ($(__ENABLE_DCI__), _YES_)
   ifeq ($(TARGET), stm32f4)
      CSRC_ARCH   += drivers/dci/$(TARGET)/dci.c
      CXXSRC_ARCH +=
   endif
endif
