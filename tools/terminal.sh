#!/usr/bin/env bash

if [ "$1" == "" ]; then
    dev=/dev/serial/by-id/usb-FTDI_FT232R_USB_UART_AE*-if00-port0
else
    dev=$1
fi

minicom -C /tmp/minicom.log -D "$dev"  -b 115200
