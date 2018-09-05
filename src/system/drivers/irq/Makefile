# Makefile for GNU make
HDRLOC_ARCH += drivers/irq

ifeq ($(__ENABLE_IRQ__), _YES_)
   ifeq ($(TARGET), stm32f1)
      CSRC_ARCH   += drivers/irq/$(TARGET)/irq.c
      CXXSRC_ARCH += 
   endif
   ifeq ($(TARGET), stm32f4)
      CSRC_ARCH   += drivers/irq/$(TARGET)/irq.c
      CXXSRC_ARCH += 
   endif
endif
