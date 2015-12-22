# Makefile for GNU make
HDRLOC_ARCH += drivers/afio

ifeq ($(__ENABLE_AFIO__), _YES_)
	ifeq ($(TARGET), stm32f1)
   	CSRC_ARCH   += drivers/afio/$(TARGET)/afio.c
   	CXXSRC_ARCH +=
   endif 
endif
