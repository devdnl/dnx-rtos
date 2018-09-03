# Makefile for GNU make
HDRLOC_ARCH += drivers/rtc

ifeq ($(__ENABLE_RTC__), _YES_)
   ifeq ($(TARGET), stm32f1)
      CSRC_ARCH   += drivers/rtc/$(TARGET)/rtc.c
   	CXXSRC_ARCH += 
   endif
   ifeq ($(TARGET), stm32f4)
      CSRC_ARCH   += drivers/rtc/$(TARGET)/rtc.c
   	CXXSRC_ARCH += 
   endif
endif
