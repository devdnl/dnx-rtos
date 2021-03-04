# Makefile for GNU make
HDRLOC_ARCH += drivers/sdio

ifeq ($(__ENABLE_SDIO__), _YES_)
   ifeq ($(TARGET), stm32f4)
      CSRC_ARCH   += drivers/sdio/$(TARGET)/sdio.c
      CXXSRC_ARCH +=
   endif
   ifeq ($(TARGET), stm32f7)
      CSRC_ARCH   += drivers/sdio/$(TARGET)/sdio.c
      CSRC_ARCH   += drivers/sdio/$(TARGET)/stm32f7xx_hal_sd.c
      CSRC_ARCH   += drivers/sdio/$(TARGET)/stm32f7xx_ll_sdmmc.c
      CXXSRC_ARCH +=
   endif
endif
