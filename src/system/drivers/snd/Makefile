# Makefile for GNU make
HDRLOC_ARCH += drivers/snd

ifeq ($(__ENABLE_SND__), _YES_)
   ifeq ($(TARGET), stm32f4)
      CSRC_ARCH   += drivers/snd/$(TARGET)/snd.c
      CXXSRC_ARCH +=
   endif
endif
