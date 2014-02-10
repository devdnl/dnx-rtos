#!/usr/bin/env bash

ID=AE*

minicom -D /dev/serial/by-id/usb-FTDI_FT232R_USB_UART_$ID-if00-port0 -b 115200
