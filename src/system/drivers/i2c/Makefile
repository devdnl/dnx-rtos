# Makefile for GNU make
HDRLOC_ARCH += drivers/i2c

ifeq ($(ENABLE_I2C), __YES__)
   ifeq ($(TARGET), stm32f1)
      CSRC_ARCH   += drivers/i2c/$(TARGET)/i2c.c
      CXXSRC_ARCH += 
   endif
endif
