# Makefile for GNU make
include $(DRV_LOC)/gpio/Makefile

ifeq ($(MODCONFIG__CRC_ENABLE), ENABLE)
include $(DRV_LOC)/crc/Makefile
endif

ifeq ($(MODCONFIG__ETHMAC_ENABLE), ENABLE)
include $(DRV_LOC)/ethmac/Makefile
endif

ifeq ($(MODCONFIG__PLL_ENABLE), ENABLE)
include $(DRV_LOC)/pll/Makefile
endif

ifeq ($(MODCONFIG__SDSPI_ENABLE), ENABLE)
include $(DRV_LOC)/sdspi/Makefile
endif

ifeq ($(MODCONFIG__SPI_ENABLE), ENABLE)
include $(DRV_LOC)/spi/Makefile
endif

ifeq ($(MODCONFIG__TTY_ENABLE), ENABLE)
include $(DRV_LOC)/tty/Makefile
endif

ifeq ($(MODCONFIG__UART_ENABLE), ENABLE)
include $(DRV_LOC)/uart/Makefile
endif

ifeq ($(MODCONFIG__WDG_ENABLE), ENABLE)
include $(DRV_LOC)/wdg/Makefile
endif
