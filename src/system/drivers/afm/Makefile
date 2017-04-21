# Makefile for GNU make
HDRLOC_ARCH += drivers/afm

ifeq ($(__ENABLE_AFM__), _YES_)
	ifeq ($(TARGET), stm32f1)
   	CSRC_ARCH   += drivers/afm/$(TARGET)/afm.c
   	CXXSRC_ARCH +=
   endif 
endif
