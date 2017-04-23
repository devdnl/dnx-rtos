# Makefile for GNU make
HDRLOC_ARCH += drivers/crc

ifeq ($(__ENABLE_CRC__), _YES_)
	ifeq ($(TARGET), stm32f1)
      CSRC_ARCH   += drivers/crc/$(TARGET)/crc.c
      CXXSRC_ARCH +=
   endif 
endif
