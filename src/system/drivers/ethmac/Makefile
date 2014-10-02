# Makefile for GNU make
HDRLOC_ARCH += drivers/ethmac

ifeq ($(ENABLE_ETH), __YES__)
   CSRC_ARCH   += drivers/ethmac/$(TARGET)/ethmac.c
   CSRC_ARCH   += drivers/ethmac/$(TARGET)/stm32_eth_driver/stm32_eth.c
   CXXSRC_ARCH += 
endif
