# Makefile for GNU make
HDRLOC_ARCH += drivers/wdg

ifeq ($(ENABLE_WDG), __YES__)
   ifeq ($(TARGET), stm32f1)
	   CSRC_ARCH   += drivers/wdg/$(TARGET)/wdg.c
   	CXXSRC_ARCH +=
	endif 
endif
