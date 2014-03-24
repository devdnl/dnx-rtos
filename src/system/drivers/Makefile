# Makefile for GNU make
include $(DRV_LOC)/gpio/Makefile

ifeq ($(ENABLE_CRC), YES)
include $(DRV_LOC)/crc/Makefile
endif

ifeq ($(ENABLE_ETHMAC), YES)
include $(DRV_LOC)/ethmac/Makefile
endif

ifeq ($(ENABLE_PLL), YES)
include $(DRV_LOC)/pll/Makefile
endif

ifeq ($(ENABLE_SDSPI), YES)
include $(DRV_LOC)/sdspi/Makefile
endif

ifeq ($(ENABLE_SPI), YES)
include $(DRV_LOC)/spi/Makefile
endif

ifeq ($(ENABLE_TTY), YES)
include $(DRV_LOC)/tty/Makefile
endif

ifeq ($(ENABLE_UART), YES)
include $(DRV_LOC)/uart/Makefile
endif

ifeq ($(ENABLE_WDG), YES)
include $(DRV_LOC)/wdg/Makefile
endif
