# Makefile for GNU make
HDRLOC_ARCH += drivers/pll

ifeq ($(ENABLE_PLL), __YES__)
   ifeq ($(TARGET), stm32f1)
      CSRC_ARCH   += drivers/pll/$(TARGET)/pll.c
      CXXSRC_ARCH += 
   endif
endif
