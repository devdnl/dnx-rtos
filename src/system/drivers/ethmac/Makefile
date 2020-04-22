# Makefile for GNU make
HDRLOC_ARCH += drivers/ethmac

ifeq ($(__ENABLE_ETHMAC__), _YES_)
	ifeq ($(TARGET), stm32f1)
      CSRC_ARCH   += drivers/ethmac/stm32fx/ethmac.c
      CSRC_ARCH   += drivers/ethmac/stm32fx/stm32f4x7_eth.c
      CXXSRC_ARCH += 
   endif
	ifeq ($(TARGET), stm32f4)
      CSRC_ARCH   += drivers/ethmac/stm32fx/ethmac.c
      CSRC_ARCH   += drivers/ethmac/stm32fx/stm32f4x7_eth.c
      CXXSRC_ARCH += 
   endif
endif
