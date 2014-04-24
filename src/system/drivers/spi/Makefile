# Makefile for GNU make
HDRLOC_ARCH += drivers/spi

ifeq ($(ENABLE_SPI), __YES__)
   ifeq ($(TARGET), stm32f1)
      CSRC_ARCH   += drivers/spi/$(TARGET)/spi.c
      CXXSRC_ARCH += 
   endif
endif
