# Makefile for GNU make
HDRLOC_ARCH += drivers/uart

ifeq ($(__ENABLE_UART__), _YES_)
    CSRC_ARCH   += drivers/uart/uart.c
    CSRC_ARCH   += drivers/uart/$(TARGET)/uart_lld.c
    CXXSRC_ARCH += 
endif
