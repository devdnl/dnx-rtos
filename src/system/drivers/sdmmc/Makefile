# Makefile for GNU make
HDRLOC_ARCH += drivers/sdmmc

ifeq ($(__ENABLE_SDMMC__), _YES_)
   ifeq ($(TARGET), stm32h7)
      CSRC_ARCH   += drivers/sdmmc/$(TARGET)/sdmmc.c
      CSRC_ARCH   += drivers/sdmmc/$(TARGET)/stm32h7xx_hal_mmc_ex.c
      CSRC_ARCH   += drivers/sdmmc/$(TARGET)/stm32h7xx_hal_mmc.c
      CSRC_ARCH   += drivers/sdmmc/$(TARGET)/stm32h7xx_hal_sd_ex.c
      CSRC_ARCH   += drivers/sdmmc/$(TARGET)/stm32h7xx_hal_sd.c
      CSRC_ARCH   += drivers/sdmmc/$(TARGET)/stm32h7xx_ll_sdmmc.c
      CXXSRC_ARCH +=
   endif
endif
