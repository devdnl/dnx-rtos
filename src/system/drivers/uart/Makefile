# Makefile for GNU make
HDRLOC_ARCH += drivers/uart

ifeq ($(__ENABLE_UART__), _YES_)
   ifeq ($(TARGET), stm32f1)
      CSRC_ARCH   += drivers/uart/$(TARGET)/uart.c
      CXXSRC_ARCH += 
   endif
   ifeq ($(TARGET), stm32f4)
      CSRC_ARCH   += drivers/uart/$(TARGET)/uart.c
      CXXSRC_ARCH += 
   endif
endif
