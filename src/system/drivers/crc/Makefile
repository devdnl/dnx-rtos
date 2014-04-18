# Makefile for GNU make
HDRLOC_ARCH += drivers/crc

ifeq ($(ENABLE_CRC), __YES__)
   ifeq ($(TARGET), stm32f1)
      CSRC_ARCH   += drivers/crc/$(TARGET)/crc.c
      CXXSRC_ARCH += 
   endif
endif
