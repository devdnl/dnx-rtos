# Makefile for GNU make
HDRLOC_ARCH += drivers/nvm

ifeq ($(__ENABLE_NVM__), _YES_)
   ifeq ($(TARGET), stm32f1)
      CSRC_ARCH   += drivers/nvm/stm32f1/nvm.c
      CXXSRC_ARCH +=
   endif
   ifeq ($(TARGET), stm32f4)
      CSRC_ARCH   += drivers/nvm/stm32f4/nvm.c
      CXXSRC_ARCH +=
   endif
endif
