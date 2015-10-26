# Makefile for GNU make
HDRLOC_ARCH += drivers/pll

ifeq ($(__ENABLE_PLL__), _YES_)
   ifeq ($(TARGET), stm32f1)
      CSRC_ARCH   += drivers/pll/$(TARGET)/pll.c
      CXXSRC_ARCH +=
   endif 
endif
