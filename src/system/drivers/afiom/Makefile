# Makefile for GNU make
HDRLOC_ARCH += drivers/afiom

ifeq ($(__ENABLE_AFIOM__), _YES_)
	ifeq ($(TARGET), stm32f1)
   	CSRC_ARCH   += drivers/afiom/$(TARGET)/afiom.c
   	CXXSRC_ARCH +=
   endif 
endif
