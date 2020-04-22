# Makefile for GNU make
HDRLOC_ARCH += drivers/pwm

ifeq ($(__ENABLE_PWM__), _YES_)
   ifeq ($(TARGET), stm32f1)
      CSRC_ARCH   += drivers/pwm/stm32fx/pwm.c
      CXXSRC_ARCH +=
   endif
   ifeq ($(TARGET), stm32f4)
      CSRC_ARCH   += drivers/pwm/stm32fx/pwm.c
      CXXSRC_ARCH +=
   endif
endif
