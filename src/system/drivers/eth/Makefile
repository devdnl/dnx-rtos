# Makefile for GNU make
HDRLOC_ARCH += drivers/eth

ifeq ($(__ENABLE_ETH__), _YES_)
	ifeq ($(TARGET), stm32f1)
      CSRC_ARCH   += drivers/eth/stm32fx/eth.c
      CSRC_ARCH   += drivers/eth/stm32fx/stm32f4xx_hal_eth.c
      CXXSRC_ARCH += 
   endif
	ifeq ($(TARGET), stm32f4)
      CSRC_ARCH   += drivers/eth/stm32fx/eth.c
      CSRC_ARCH   += drivers/eth/stm32fx/stm32f4xx_hal_eth.c
      CXXSRC_ARCH += 
   endif
	ifeq ($(TARGET), stm32f7)
      CSRC_ARCH   += drivers/eth/stm32fx/eth.c
      CSRC_ARCH   += drivers/eth/stm32fx/stm32f4xx_hal_eth.c
      CXXSRC_ARCH += 
   endif
endif
