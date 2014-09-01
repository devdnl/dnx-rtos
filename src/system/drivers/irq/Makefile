# Makefile for GNU make
HDRLOC_ARCH += drivers/irq

ifeq ($(ENABLE_IRQ), __YES__)
   ifeq ($(TARGET), stm32f1)
      CSRC_ARCH   += drivers/irq/$(TARGET)/irq.c
      CXXSRC_ARCH += 
   endif
endif
