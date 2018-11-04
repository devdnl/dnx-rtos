# Makefile for GNU make
HDRLOC_ARCH += drivers/pwm

ifeq ($(__ENABLE_PWM__), _YES_)
   ifeq ($(TARGET), stm32f4)
      CSRC_ARCH   += drivers/pwm/$(TARGET)/pwm.c
      CXXSRC_ARCH +=
   endif
endif
