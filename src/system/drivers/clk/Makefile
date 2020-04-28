# Makefile for GNU make
HDRLOC_ARCH += drivers/clk

ifeq ($(__ENABLE_CLK__), _YES_)
   ifeq ($(TARGET), stm32f1)
      CSRC_ARCH   += drivers/clk/$(TARGET)/clk.c
      CXXSRC_ARCH +=
   endif 
   ifeq ($(TARGET), stm32f4)
      CSRC_ARCH   += drivers/clk/$(TARGET)/clk.c
      CXXSRC_ARCH +=
   endif 
   ifeq ($(TARGET), stm32f7)
      CSRC_ARCH   += drivers/clk/$(TARGET)/clk.c
      CXXSRC_ARCH +=
   endif 
endif
