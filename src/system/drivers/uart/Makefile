# Makefile for GNU make
HDRLOC_ARCH += drivers/uart

ifeq ($(ENABLE_UART), __YES__)
   ifeq ($(TARGET), stm32f1)
      CSRC_ARCH   += drivers/uart/$(TARGET)/uart.c
      CXXSRC_ARCH += 
   endif
endif
