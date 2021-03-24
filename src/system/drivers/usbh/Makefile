# Makefile for GNU make
HDRLOC_ARCH += drivers/usbh

ifeq ($(__ENABLE_USBH__), _YES_)
   ifeq ($(TARGET), stm32f4)
   	HDRLOC_ARCH += drivers/usbh/$(TARGET)
   	#HDRLOC_ARCH += drivers/usbh/$(TARGET)/STM32_USB_OTG_Driver/inc
   	HDRLOC_ARCH += drivers/usbh/$(TARGET)/STM32_USB_Host_Library/Core/Inc
   	HDRLOC_ARCH += drivers/usbh/$(TARGET)/STM32_USB_Host_Library/Class/MSC/Inc
   	HDRLOC_ARCH += drivers/usbh/$(TARGET)/STM32_USB_HCD
   
      CSRC_ARCH   += drivers/usbh/$(TARGET)/STM32_USB_HCD/stm32f4xx_hal_hcd.c
      CSRC_ARCH   += drivers/usbh/$(TARGET)/STM32_USB_HCD/stm32f4xx_ll_usb.c

      CSRC_ARCH   += drivers/usbh/$(TARGET)/usbh.c
      CSRC_ARCH   += drivers/usbh/$(TARGET)/STM32_USB_Host_Library/Core/Src/usbh_core.c
      CSRC_ARCH   += drivers/usbh/$(TARGET)/STM32_USB_Host_Library/Core/Src/usbh_ctlreq.c
      CSRC_ARCH   += drivers/usbh/$(TARGET)/STM32_USB_Host_Library/Core/Src/usbh_ioreq.c
      CSRC_ARCH   += drivers/usbh/$(TARGET)/STM32_USB_Host_Library/Core/Src/usbh_pipes.c
      CSRC_ARCH   += drivers/usbh/$(TARGET)/STM32_USB_Host_Library/Class/MSC/Src/usbh_msc.c
      CSRC_ARCH   += drivers/usbh/$(TARGET)/STM32_USB_Host_Library/Class/MSC/Src/usbh_msc_bot.c
      CSRC_ARCH   += drivers/usbh/$(TARGET)/STM32_USB_Host_Library/Class/MSC/Src/usbh_msc_scsi.c
      
      CXXSRC_ARCH +=
   endif
endif
