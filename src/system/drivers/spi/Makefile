# Makefile for GNU make
HDRLOC_ARCH += drivers/spi

ifeq ($(__ENABLE_SPI__), _YES_)
	CSRC_ARCH   += drivers/spi/spi.c
  	ifeq ($(TARGET), stm32f1)
   	CSRC_ARCH   += drivers/spi/stm32fx/spi_lld.c
    	CXXSRC_ARCH += 
  	endif
  	ifeq ($(TARGET), stm32f4)
    	CSRC_ARCH   += drivers/spi/stm32fx/spi_lld.c
    	CXXSRC_ARCH += 
  endif
endif
