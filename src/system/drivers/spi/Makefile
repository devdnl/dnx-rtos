# Makefile for GNU make
HDRLOC_ARCH += drivers/spi

ifeq ($(__ENABLE_SPI__), _YES_)
   ifeq ($(TARGET), stm32f1)
      CSRC_ARCH   += drivers/spi/$(TARGET)/spi.c
      CXXSRC_ARCH += 
   endif
endif
