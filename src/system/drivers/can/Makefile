# Makefile for GNU make
HDRLOC_ARCH += drivers/can

ifeq ($(__ENABLE_CAN__), _YES_)
   ifeq ($(TARGET), stm32f1)
      CSRC_ARCH   += drivers/can/$(TARGET)/can.c
      CXXSRC_ARCH +=
   endif
endif
