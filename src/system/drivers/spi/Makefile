# Makefile for GNU make
HDRLOC_ARCH += drivers/spi

ifeq ($(__ENABLE_SPI__), _YES_)
    CSRC_ARCH   += drivers/spi/spi.c
    CSRC_ARCH   += drivers/spi/$(TARGET)/spi_lld.c
    CXXSRC_ARCH += 
endif
