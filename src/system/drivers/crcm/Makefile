# Makefile for GNU make
HDRLOC_ARCH += drivers/crcm

ifeq ($(__ENABLE_CRCM__), _YES_)
	ifeq ($(TARGET), stm32f1)
      CSRC_ARCH   += drivers/crcm/$(TARGET)/crcm.c
      CXXSRC_ARCH +=
   endif 
endif
