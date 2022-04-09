# Makefile for GNU make
HDRLOC_ARCH += drivers/emi

ifeq ($(__ENABLE_EMI__), _YES_)
   ifeq ($(TARGET), stm32f4)
      CSRC_ARCH   += drivers/emi/stm32fx/emi.c
      CXXSRC_ARCH +=
   endif
   ifeq ($(TARGET), stm32f7)
      CSRC_ARCH   += drivers/emi/stm32fx/emi.c
      CXXSRC_ARCH +=
   endif
   ifeq ($(TARGET), stm32h7)
      CSRC_ARCH   += drivers/emi/stm32fx/emi.c
      CXXSRC_ARCH +=
   endif
endif
