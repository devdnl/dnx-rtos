# Makefile for GNU make
HDRLOC_ARCH += drivers/rtcm

ifeq ($(__ENABLE_RTCM__), _YES_)
   ifeq ($(TARGET), stm32f1)
      CSRC_ARCH   += drivers/rtcm/$(TARGET)/rtcm.c
   	CXXSRC_ARCH += 
   endif
endif
