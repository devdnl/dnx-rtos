# Makefile for GNU make
HDRLOC_ARCH += drivers/gpio

ifeq ($(__ENABLE_GPIO__), _YES_)
   ifeq ($(TARGET), stm32f1)
      CSRC_ARCH   += drivers/gpio/$(TARGET)/gpio.c
      CXXSRC_ARCH += 
   endif
endif
