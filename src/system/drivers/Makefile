# Makefile for GNU make
ifeq ($(ENABLE_GPIO), __YES__)
include $(DRV_LOC)/gpio/Makefile
endif

ifeq ($(ENABLE_AFIO), __YES__)
include $(DRV_LOC)/afio/Makefile
endif

ifeq ($(ENABLE_CRC), __YES__)
include $(DRV_LOC)/crc/Makefile
endif

ifeq ($(ENABLE_ETH), __YES__)
include $(DRV_LOC)/ethmac/Makefile
endif

ifeq ($(ENABLE_PLL), __YES__)
include $(DRV_LOC)/pll/Makefile
endif

ifeq ($(ENABLE_SDSPI), __YES__)
include $(DRV_LOC)/sdspi/Makefile
endif

ifeq ($(ENABLE_SPI), __YES__)
include $(DRV_LOC)/spi/Makefile
endif

ifeq ($(ENABLE_TTY), __YES__)
include $(DRV_LOC)/tty/Makefile
endif

ifeq ($(ENABLE_UART), __YES__)
include $(DRV_LOC)/uart/Makefile
endif

ifeq ($(ENABLE_WDG), __YES__)
include $(DRV_LOC)/wdg/Makefile
endif
