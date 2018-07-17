# Makefile for GNU make
HDRLOC_ARCH += drivers/nvm

ifeq ($(__ENABLE_NVM__), _YES_)
   ifeq ($(TARGET), stm32f4)
      CSRC_ARCH   += drivers/nvm/$(TARGET)/nvm.c
      CXXSRC_ARCH +=
   endif
endif
