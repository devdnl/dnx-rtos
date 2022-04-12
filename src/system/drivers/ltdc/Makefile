# Makefile for GNU make
HDRLOC_ARCH += drivers/ltdc

ifeq ($(__ENABLE_LTDC__), _YES_)
   ifeq ($(TARGET), stm32h7)
      CSRC_ARCH   += drivers/ltdc/$(TARGET)/ltdc.c
      CSRC_ARCH   += drivers/ltdc/$(TARGET)/stm32h7xx_hal_ltdc.c
      CXXSRC_ARCH +=
   endif
endif
