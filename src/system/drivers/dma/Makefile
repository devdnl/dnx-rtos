# Makefile for GNU make
HDRLOC_ARCH += drivers/dma

ifeq ($(__ENABLE_DMA__), _YES_)
    ifeq ($(TARGET), stm32f1)
        CSRC_ARCH   += drivers/dma/drv/STM32F1F3/dma.c
        CXXSRC_ARCH += 
    endif
    ifeq ($(TARGET), stm32f3)
        CSRC_ARCH   += drivers/dma/drv/STM32F1F3/dma.c
        CXXSRC_ARCH += 
    endif
    ifeq ($(TARGET), stm32f4)
        CSRC_ARCH   += drivers/dma/drv/STM32F4F7/dma.c
        CXXSRC_ARCH += 
    endif
    ifeq ($(TARGET), stm32f7)
        CSRC_ARCH   += drivers/dma/drv/STM32F4F7/dma.c
        CXXSRC_ARCH += 
    endif
endif
