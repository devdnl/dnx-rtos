# Makefile for GNU make
HDRLOC_ARCH += drivers/i2c

ifeq ($(__ENABLE_I2C__), _YES_)
    CSRC_ARCH   += drivers/i2c/i2c.c
    ifeq ($(TARGET), stm32f1)
        CSRC_ARCH   += drivers/i2c/drv/STM32F1F4/i2c_lld.c
        CXXSRC_ARCH += 
    endif
    ifeq ($(TARGET), stm32f3)
        CSRC_ARCH   += drivers/i2c/drv/STM32F3F7/i2c_lld.c
        CXXSRC_ARCH += 
    endif
    ifeq ($(TARGET), stm32f4)
        CSRC_ARCH   += drivers/i2c/drv/STM32F1F4/i2c_lld.c
        CXXSRC_ARCH += 
    endif
    ifeq ($(TARGET), stm32f7)
        CSRC_ARCH   += drivers/i2c/drv/STM32F3F7H7/i2c_lld.c
        CXXSRC_ARCH += 
    endif
    ifeq ($(TARGET), stm32h7)
        CSRC_ARCH   += drivers/i2c/drv/STM32F3F7H7/i2c_lld.c
        CXXSRC_ARCH += 
    endif
endif
