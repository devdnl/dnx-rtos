# Makefile for GNU make
HDRLOC_ARCH += drivers/genericmod

ifeq ($(ENABLE_GENERICMOD), __YES__)
   ifeq ($(TARGET), stm32f1)
      CSRC_ARCH   += drivers/genericmod/$(TARGET)/genericmod.c
	   CXXSRC_ARCH += 
   endif
endif
