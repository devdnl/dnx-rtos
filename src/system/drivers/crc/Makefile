# Makefile for GNU make
HDRLOC_ARCH += drivers/crc

ifeq ($(__ENABLE_CRC__), _YES_)
	ifeq ($(TARGET), stm32f1)
      CSRC_ARCH   += drivers/crc/stm32fx/crc.c
      CXXSRC_ARCH +=
   endif
	ifeq ($(TARGET), stm32f4)
      CSRC_ARCH   += drivers/crc/stm32fx/crc.c
      CXXSRC_ARCH +=
   endif
endif
