# Makefile for GNU make
HDRLOC_ARCH += drivers/gpio

ifeq ($(ENABLE_GPIO), __YES__)
   ifeq ($(TARGET), stm32f1)
      CSRC_ARCH   += drivers/gpio/$(TARGET)/gpio.c
      CXXSRC_ARCH += 
   endif
endif
