# Makefile for GNU make
HDRLOC_ARCH += drivers/usbd

ifeq ($(__ENABLE_USBD__), _YES_)
   ifeq ($(TARGET), stm32f1)
      CSRC_ARCH   += drivers/usbd/$(TARGET)/usbd.c
      CXXSRC_ARCH += 
   endif
endif
