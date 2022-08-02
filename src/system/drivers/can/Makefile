# Makefile for GNU make
HDRLOC_ARCH += drivers/can

ifeq ($(__ENABLE_CAN__), _YES_)
 	ifeq ($(TARGET), stm32f1)
		CSRC_ARCH   += drivers/can/stm32fx/can.c
		CXXSRC_ARCH +=
	endif
	ifeq ($(TARGET), stm32f3)
		CSRC_ARCH   += drivers/can/stm32fx/can.c
		CXXSRC_ARCH +=
	endif
	ifeq ($(TARGET), stm32f4)
		CSRC_ARCH   += drivers/can/stm32fx/can.c
		CXXSRC_ARCH +=
	endif
	ifeq ($(TARGET), stm32f7)
		CSRC_ARCH   += drivers/can/stm32fx/can.c
		CXXSRC_ARCH +=
	endif
	ifeq ($(TARGET), stm32h7)
		CSRC_ARCH   += drivers/can/stm32h7/can.c
		CSRC_ARCH   += drivers/can/stm32h7/stm32h7xx_hal_fdcan.c
		CXXSRC_ARCH +=
	endif
endif
 
