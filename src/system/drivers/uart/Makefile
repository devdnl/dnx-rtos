# Makefile for GNU make
HDRLOC_ARCH += drivers/uart

ifeq ($(__ENABLE_UART__), _YES_)
   CSRC_ARCH   += drivers/uart/uart.c
   ifeq ($(TARGET), efr32)
      CSRC_ARCH   += drivers/uart/efr32/uart_lld.c
      CXXSRC_ARCH += 
   endif
   ifeq ($(TARGET), stm32f1)
      CSRC_ARCH   += drivers/uart/stm32fx/uart_lld.c
      CXXSRC_ARCH +=
   endif
   ifeq ($(TARGET), stm32f4)
      CSRC_ARCH   += drivers/uart/stm32fx/uart_lld.c
      CXXSRC_ARCH +=
   endif
   ifeq ($(TARGET), stm32f7)
      CSRC_ARCH   += drivers/uart/stm32fx/uart_lld.c
      CXXSRC_ARCH +=
   endif
endif
