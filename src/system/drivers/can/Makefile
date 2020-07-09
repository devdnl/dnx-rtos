# Makefile for GNU make
HDRLOC_ARCH += drivers/can

ifeq ($(__ENABLE_CAN__), _YES_)
	ifeq ($(TARGET), stm32f1)
      CSRC_ARCH   += drivers/can/stm32fx/can.c
      CXXSRC_ARCH +=
   endif
	ifeq ($(TARGET), stm32f4)
      CSRC_ARCH   += drivers/can/stm32fx/can.c
      CXXSRC_ARCH +=
   endif
	ifeq ($(TARGET), stm32f7)
      CSRC_ARCH   += drivers/can/stm32fx/can.c
      CXXSRC_ARCH +=
   endif
endif
