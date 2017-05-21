# Makefile for GNU make
HDRLOC_ARCH += drivers/i2c

ifeq ($(__ENABLE_I2C__), _YES_)
    CSRC_ARCH   += drivers/i2c/i2c.c
    CSRC_ARCH   += drivers/i2c/$(TARGET)/i2c_lld.c
    CXXSRC_ARCH += 
endif
