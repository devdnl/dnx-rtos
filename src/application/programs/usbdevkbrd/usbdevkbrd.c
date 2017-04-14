/*=========================================================================*//**
@file    usbdevkbrd.c

@author  Daniel Zorychta

@brief   USB device keyboard example

@note    Copyright (C) 2015 Daniel Zorychta <daniel.zorychta@gmail.com>

         This program is free software; you can redistribute it and/or modify
         it under the terms of the GNU General Public License as published by
         the Free Software Foundation and modified by the dnx RTOS exception.

         NOTE: The modification  to the GPL is  included to allow you to
               distribute a combined work that includes dnx RTOS without
               being obliged to provide the source  code for proprietary
               components outside of the dnx RTOS.

         The dnx RTOS  is  distributed  in the hope  that  it will be useful,
         but WITHOUT  ANY  WARRANTY;  without  even  the implied  warranty of
         MERCHANTABILITY  or  FITNESS  FOR  A  PARTICULAR  PURPOSE.  See  the
         GNU General Public License for more details.

         Full license text is available on the following file: doc/license.txt.


*//*==========================================================================*/

/*==============================================================================
  Include files
==============================================================================*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <dnx/misc.h>
#include <unistd.h>

/*==============================================================================
  Local symbolic constants/macros
==============================================================================*/
#define tostring(a)                     #a
#define USB_mA(mA)                      (mA / 2)
#define USB_KEY_CHARACTER_CODE(c)       (4  + ((c) - 'A'))
#define USB_KEY_NUMBER_CODE(n)          (30 + ((n) - '0'))
#define USB_KEY_FX_CODE(f)              (58 + ((f) - '1'))

/*==============================================================================
  Local types, enums definitions
==============================================================================*/

/*==============================================================================
  Local function prototypes
==============================================================================*/

/*==============================================================================
  Local object definitions
==============================================================================*/
GLOBAL_VARIABLES_SECTION {
};

static const USBD_ep_config_t ep_cfg = {
        .ep[USB_EP_NUM__ENDP0] = USBD_EP_CONFIG_IN_OUT(USB_TRANSFER__CONTROL, USBD_EP0_SIZE, USBD_EP0_SIZE),
        .ep[USB_EP_NUM__ENDP1] = USBD_EP_CONFIG_IN(USB_TRANSFER__INTERRUPT, 8),
        .ep[USB_EP_NUM__ENDP2] = USBD_EP_CONFIG_DISABLED(),
        .ep[USB_EP_NUM__ENDP3] = USBD_EP_CONFIG_DISABLED(),
        .ep[USB_EP_NUM__ENDP4] = USBD_EP_CONFIG_DISABLED(),
        .ep[USB_EP_NUM__ENDP5] = USBD_EP_CONFIG_DISABLED(),
        .ep[USB_EP_NUM__ENDP6] = USBD_EP_CONFIG_DISABLED(),
        .ep[USB_EP_NUM__ENDP7] = USBD_EP_CONFIG_DISABLED()
};

static const usb_device_descriptor_t device_descriptor = {
        .bLength            = sizeof(usb_device_descriptor_t),
        .bDescriptorType    = DEVICE_DESCRIPTOR,
        .bcdUSB             = HTOUSBS(0x0200),
        .bDeviceClass       = USB_CLASS__SPECIFIED_AT_INTERFACE_LEVEL,
        .bDeviceSubClass    = USB_SUBCLASS__SPECIFIED_AT_INTERFACE_LEVEL,
        .bDeviceProtocol    = USB_PROTOCOL__SPECIFIED_AT_INTERFACE_LEVEL,
        .bMaxPacketSize0    = USBD_EP0_SIZE,
        .idVendor           = HTOUSBS(0x0483),
        .idProduct          = HTOUSBS(0x5752),
        .bcdDevice          = HTOUSBS(0x0001),
        .iManufacturer      = 1,
        .iProduct           = 2,
        .iSerialNumber      = 3,
        .bNumConfigurations = 1
};

static const u8_t hid_report_descriptor[] = {
        0x05, 0x01,                    // USAGE_PAGE (Generic Desktop)
        0x09, 0x06,                    // USAGE (Keyboard)
        0xa1, 0x01,                    // COLLECTION (Application)
        0x05, 0x07,                    //   USAGE_PAGE (Keyboard)
        0x19, 0xe0,                    //   USAGE_MINIMUM (Keyboard LeftControl)
        0x29, 0xe7,                    //   USAGE_MAXIMUM (Keyboard Right GUI)
        0x15, 0x00,                    //   LOGICAL_MINIMUM (0)
        0x25, 0x01,                    //   LOGICAL_MAXIMUM (1)
        0x75, 0x01,                    //   REPORT_SIZE (1)
        0x95, 0x08,                    //   REPORT_COUNT (8)
        0x81, 0x02,                    //   INPUT (Data,Var,Abs)
        0x95, 0x01,                    //   REPORT_COUNT (1)
        0x75, 0x08,                    //   REPORT_SIZE (8)
        0x81, 0x03,                    //   INPUT (Cnst,Var,Abs)
        0x95, 0x05,                    //   REPORT_COUNT (5)
        0x75, 0x01,                    //   REPORT_SIZE (1)
        0x05, 0x08,                    //   USAGE_PAGE (LEDs)
        0x19, 0x01,                    //   USAGE_MINIMUM (Num Lock)
        0x29, 0x05,                    //   USAGE_MAXIMUM (Kana)
        0x91, 0x02,                    //   OUTPUT (Data,Var,Abs)
        0x95, 0x01,                    //   REPORT_COUNT (1)
        0x75, 0x03,                    //   REPORT_SIZE (3)
        0x91, 0x03,                    //   OUTPUT (Cnst,Var,Abs)
        0x95, 0x06,                    //   REPORT_COUNT (6)
        0x75, 0x08,                    //   REPORT_SIZE (8)
        0x15, 0x00,                    //   LOGICAL_MINIMUM (0)
        0x25, 0x65,                    //   LOGICAL_MAXIMUM (101)
        0x05, 0x07,                    //   USAGE_PAGE (Keyboard)
        0x19, 0x00,                    //   USAGE_MINIMUM (Reserved (no event indicated))
        0x29, 0x65,                    //   USAGE_MAXIMUM (Keyboard Application)
        0x81, 0x00,                    //   INPUT (Data,Ary,Abs)
        0xc0                           // END_COLLECTION
};

typedef struct config {
        usb_configuration_descriptor_t  configuration_descriptor;
        usb_interface_descriptor_t      interface_descriptor;
        usb_hid_descriptor_t            hid_descriptor;
        usb_endpoint_descriptor_t       endpoint_descriptor;
} __packed usb_hid_configuration_t;


static const usb_hid_configuration_t hid_cfg = {
        .configuration_descriptor = {
                .bLength             = sizeof(usb_configuration_descriptor_t),
                .bDescriptorType     = CONFIGURATION_DESCRIPTOR,
                .wTotalLength        = USBTOHS(sizeof(usb_hid_configuration_t)),
                .bNumInterfaces      = 1,
                .bConfigurationValue = 1,
                .iConfiguration      = 0,
                .bmAttributes        = D7_RESERVED,     /* bus powered */
                .bMaxPower           = USB_mA(100)
        },

        .interface_descriptor = {
                .bLength             = sizeof(usb_interface_descriptor_t),
                .bDescriptorType     = INTERFACE_DESCRIPTOR,
                .bInterfaceNumber    = 0,
                .bAlternateSetting   = 0,
                .bNumEndpoints       = 1,
                .bInterfaceClass     = USB_CLASS__HUMAN_INTERFACE_DEVICE,
                .bInterfaceSubClass  = USB_SUBCLASS__BOOT_INTERFACE,
                .bInterfaceProtocol  = USB_PROTOCOL__KEYBOARD,
                .iInterface          = 0
        },

        .hid_descriptor = {
                .bLength             = sizeof(usb_interface_descriptor_t),
                .bDescriptorType     = HID_MAIN_DESCRIPTOR,
                .bcdHID              = USBTOHS(0x0110),
                .bCountryCode        = 0,
                .bNumDescriptors     = 1,
                .bDescriptorType1    = HID_REPORT_DESCRIPTOR,
                .wDescriptorLength1  = sizeof(hid_report_descriptor)
        },

        .endpoint_descriptor = {
                .bLength             = sizeof(usb_endpoint_descriptor_t),
                .bDescriptorType     = ENDPOINT_DESCRIPTOR,
                .bEndpointAddress    = USB_EP_NUM__ENDP1 | USB_ENDP_IN,
                .bmAttributes        = USB_TRANSFER__INTERRUPT,
                .wMaxPacketSize      = USBTOHS(8),
                .bInterval           = 10
        }
};

static const usb_device_qualifier_descriptor_t device_qualifier = {
        .bLength             = sizeof(usb_device_qualifier_descriptor_t),
        .bDescriptorType     = DEVICE_QUALIFIER_DESCRIPTOR,
        .bcdUSB              = HTOUSBS(0x0200),
        .bDeviceClass        = USB_CLASS__SPECIFIED_AT_INTERFACE_LEVEL,
        .bDeviceSubClass     = USB_SUBCLASS__SPECIFIED_AT_INTERFACE_LEVEL,
        .bDeviceProtocol     = USB_PROTOCOL__SPECIFIED_AT_INTERFACE_LEVEL,
        .bMaxPacketSize0     = 0,
        .bNumConfigurations  = 1,
        .bReserved           = 0
};

static const usb_string_descriptor_t(1) string_lang = {
        .bLength         = sizeof(usb_string_descriptor_t(1)),
        .bDescriptorType = STRING_DESCRIPTOR,
        .bString         = {HTOUSBS(USB_LANG_US_ENGLISH)}
};

static const usb_string_descriptor_t(7) string_manufacturer = {
        .bLength         = sizeof(usb_string_descriptor_t(7)),
        .bDescriptorType = STRING_DESCRIPTOR,
        .bString         = {HTOUSBS('d'), HTOUSBS('n'), HTOUSBS('x'), HTOUSBS('R'), HTOUSBS('T'), HTOUSBS('O'), HTOUSBS('S')}
};

static const usb_string_descriptor_t(8) string_product = {
        .bLength         = sizeof(usb_string_descriptor_t(8)),
        .bDescriptorType = STRING_DESCRIPTOR,
        .bString         = {HTOUSBS('K'), HTOUSBS('e'), HTOUSBS('y'), HTOUSBS('b'), HTOUSBS('o'), HTOUSBS('a'), HTOUSBS('r'), HTOUSBS('d')}
};

static const usb_string_descriptor_t(5) string_serial = {
        .bLength         = sizeof(usb_string_descriptor_t(5)),
        .bDescriptorType = STRING_DESCRIPTOR,
        .bString         = {HTOUSBS('0'), HTOUSBS('.'), HTOUSBS('0'), HTOUSBS('.'), HTOUSBS('1')}
};

static const usb_max_string_descriptor_t *string[] = {
        cast(usb_max_string_descriptor_t*, &string_lang),
        cast(usb_max_string_descriptor_t*, &string_manufacturer),
        cast(usb_max_string_descriptor_t*, &string_product),
        cast(usb_max_string_descriptor_t*, &string_serial)
};

static const u8_t  GPIO_LED_WHITE = IOCTL_GPIO_PIN_IDX__NULL;
static const u8_t  GPIO_LED_RED   = IOCTL_GPIO_PIN_IDX__NULL;
static const u8_t  GPIO_LED_GREEN = IOCTL_GPIO_PIN_IDX__NULL;
static const char *GPIO_PORT_PATH = "/dev/GPIOA";

/*==============================================================================
  Exported object definitions
==============================================================================*/

/*==============================================================================
  Function definitions
==============================================================================*/

//==============================================================================
/**
 * @brief Prints message when request is not recognized
 */
//==============================================================================
static void print_setup(usb_setup_packet_t *setup)
{
        printf(" bmRequestType: 0x%x [dir: %d; type: %d; recipient: %d]\n",
               setup->bmRequestType,
               (setup->bmRequestType & REQUEST_DIRECTION_MASK) >> 7,
               (setup->bmRequestType & REQUEST_TYPE_MASK) >> 5,
               (setup->bmRequestType & REQUEST_RECIPIENT_MASK));

        printf(" bRequest     : 0x%x\n", setup->bRequest);

        printf(" wValue       : 0x%x [type: %d; index: %d]\n",
               setup->wValue, setup->wValue >> 8, setup->wValue & 0xFF);

        printf(" wIndex       : 0x%x\n", setup->wIndex);
        printf(" wLength      : 0x%x\n", setup->wLength);
}

//==============================================================================
/**
 * @brief Keyboard main function
 */
//==============================================================================
int_main(usbdevkbrd, STACK_DEPTH_LOW, int argc, char *argv[])
{
        (void)argc;
        (void)argv;

        FILE *ep0  = fopen("/dev/usbd-ep0", "r+");
        FILE *ep1  = fopen("/dev/usbd-ep1", "r+");
        FILE *gpio = fopen(GPIO_PORT_PATH, "r+");

        if (ep0 && ep1) {
                USBD_setup_container_t setup     = {.timeout = 25};
                bool                   configured = false;
                int                    operation  = -1;

                ioctl(stdin, IOCTL_VFS__NON_BLOCKING_RD_MODE);
                ioctl(ep0, IOCTL_USBD__START);

                while (true) {
                        int ch = getchar();
                        if (ch == 'q')
                                break;

                        if (configured) {
                                usb_hid_keyboard_boot_report_t report;
                                memset(&report, 0, sizeof(report));

                                if (ch != EOF) {
                                        switch (ch) {
                                        case 'f':
                                                report.key[0] = 69;
                                                break;

                                        case 'x':
                                                report.modifiers = USB_KEYBOARD_LEFT_ALT;
                                                report.key[0] = 59;
                                                operation     = 0;
                                                break;

                                        case 'w':
                                                report.modifiers = USB_KEYBOARD_LEFT_GUI;
                                                break;

                                        case 'c':
                                        case 'C':
                                                report.key[0] = USB_KEYBOARD_CAPS_LOCK_CODE;
                                                break;

                                        case 'n':
                                                report.key[0] = USB_KEYBOARD_NUM_LOCK_CODE;
                                                break;

                                        case 'r':
                                                report.modifiers = USB_KEYBOARD_LEFT_GUI;
                                                report.key[0]    = USB_KEY_CHARACTER_CODE('R');
                                                operation        = 0;
                                                break;

                                        }
                                } else {
                                        switch (operation) {
                                        case  0: operation++; break;
                                        case  1: operation++; break;
                                        case  2: report.key[0] = USB_KEY_CHARACTER_CODE('F'); operation++; break;
                                        case  3: report.key[0] = USB_KEY_CHARACTER_CODE('O'); operation++; break;
                                        case  4: report.key[0] = USB_KEY_CHARACTER_CODE('R'); operation++; break;
                                        case  5: report.key[0] = USB_KEY_CHARACTER_CODE('M'); operation++; break;
                                        case  6: report.key[0] = USB_KEY_CHARACTER_CODE('A'); operation++; break;
                                        case  7: report.key[0] = USB_KEY_CHARACTER_CODE('T'); operation++; break;
                                        case  8: report.key[0] = 44; operation++; break;
                                        case  9: report.key[0] = USB_KEY_CHARACTER_CODE('C'); operation++; break;
                                        case 10: report.key[0] = 51; report.modifiers = USB_KEYBOARD_LEFT_SHIFT; operation++; break;
                                        case 11: operation = -1; break;
                                        default: operation = -1; break;
                                        }
                                }

                                printf("REPORT (%d/%d)\n",
                                       fwrite(&report, 1, sizeof(report), ep1),
                                       sizeof(report));
                        }

                        if (ioctl(ep0, IOCTL_USBD__GET_SETUP_PACKET, &setup) == 0) {
                                printf("SETUP: ");
                        } else {
                                continue;
                        }

                        /* clears USB reset indicator */
                        bool was_reset = false;
                        ioctl(ep0, IOCTL_USBD__WAS_RESET, &was_reset);

                        if (setup.packet.wLength == 0) {
                                int operation = -1;

                                switch (setup.packet.bmRequestType & REQUEST_RECIPIENT_MASK) {
                                case DEVICE_RECIPIENT:
                                        switch (setup.packet.bRequest) {
                                        case SET_ADDRESS:
                                                printf(tostring(SET_ADDRESS)" (%d):", setup.packet.wValue);
                                                if (ioctl(ep0, IOCTL_USBD__SEND_ZLP) == 0) {
                                                        ioctl(ep0, IOCTL_USBD__SET_ADDRESS, &setup.packet.wValue);
                                                        puts(" OK");
                                                } else {
                                                        puts(" ERROR");
                                                }
                                                continue;

                                        case SET_CONFIGURATION:
                                                printf(tostring(SET_CONFIGURATION)" (%d):", setup.packet.wValue);
                                                operation = ioctl(ep0, IOCTL_USBD__CONFIGURE_EP_1_7, &ep_cfg);
                                                break;
                                        }
                                        break;

                                case INTERFACE_RECIPIENT:
                                        switch (setup.packet.bRequest) {
                                        case SET_IDLE:
                                                printf(tostring(SET_IDLE)":");
                                                operation = 0;
                                                break;
                                        }
                                        break;
                                }

                                if (operation == 0) {
                                        if (ioctl(ep0, IOCTL_USBD__SEND_ZLP) != 0) {
                                                puts(" ERROR");
                                        } else {
                                                puts(" OK");
                                        }
                                } else if (operation == 1) {
                                        puts(" ERROR");
                                        ioctl(ep0, IOCTL_USBD__SET_ERROR_STATUS);
                                } else {
                                        puts("UNKNOWN REQUEST");
                                        print_setup(&setup.packet);
                                        ioctl(ep0, IOCTL_USBD__SET_ERROR_STATUS);
                                }

                        } else if ((setup.packet.bmRequestType & REQUEST_DIRECTION_MASK) == DEVICE_TO_HOST) {

                                size_t       size = 0;
                                const void  *data = NULL;

                                switch (setup.packet.bRequest) {
                                case GET_DESCRIPTOR:
                                        printf("GET ");
                                        switch (setup.packet.wValue >> 8) {
                                        case DEVICE_DESCRIPTOR:
                                                printf(tostring(DEVICE_DESCRIPTOR)":");
                                                size = min(setup.packet.wLength, sizeof(device_descriptor));
                                                data = &device_descriptor;
                                                break;

                                        case CONFIGURATION_DESCRIPTOR:
                                                printf(tostring(CONFIGURATION_DESCRIPTOR)":");
                                                size = min(setup.packet.wLength, sizeof(hid_cfg));
                                                data = &hid_cfg;
                                                break;

                                        case STRING_DESCRIPTOR:
                                                printf(tostring(STRING_DESCRIPTOR)":");
                                                int idx = setup.packet.wValue & 0xFF;
                                                if (idx <= 3) {
                                                        size = min(setup.packet.wLength, string[idx]->bLength);
                                                        data = string[idx];
                                                }
                                                break;

                                        case DEVICE_QUALIFIER_DESCRIPTOR:
                                                printf(tostring(DEVICE_QUALIFIER_DESCRIPTOR)":");
                                                size = min(setup.packet.wLength, sizeof(usb_device_qualifier_descriptor_t));
                                                data = &device_qualifier;
                                                break;

                                        case HID_REPORT_DESCRIPTOR:
                                                printf(tostring(HID_REPORT_DESCRIPTOR)":");
                                                size       = min(setup.packet.wLength, sizeof(hid_report_descriptor));
                                                data       = &hid_report_descriptor;
                                                configured = true;
                                                break;
                                        }
                                        break;
                                }

                                if (size && data) {
                                        printf(" (%d/%d)\n", fwrite(data, 1, size, ep0), cast(int, size));
                                } else {
                                        puts(" UNKNOWN REQUEST [IN]");
                                        print_setup(&setup.packet);
                                        ioctl(ep0, IOCTL_USBD__SET_ERROR_STATUS);
                                }

                        } else {
                                switch (setup.packet.bRequest) {
                                case SET_REPORT:
                                        printf(tostring(SET_REPORT)":");
                                        uint8_t report = 0;
                                        printf("(%d/1)\n", fread(&report, 1, 1, ep0));

                                        if (report & USB_KEYBOARD_CAPS_LOCK_LED) {
                                                ioctl(gpio, IOCTL_GPIO__SET_PIN, &GPIO_LED_WHITE);
                                        } else {
                                                ioctl(gpio, IOCTL_GPIO__CLEAR_PIN, &GPIO_LED_WHITE);
                                        }

                                        if (report & USB_KEYBOARD_NUM_LOCK_LED) {
                                                ioctl(gpio, IOCTL_GPIO__SET_PIN, &GPIO_LED_GREEN);
                                        } else {
                                                ioctl(gpio, IOCTL_GPIO__CLEAR_PIN, &GPIO_LED_GREEN);
                                        }

                                        if (report & USB_KEYBOARD_SCROLL_LOCK_LED) {
                                                ioctl(gpio, IOCTL_GPIO__SET_PIN, &GPIO_LED_RED);
                                        } else {
                                                ioctl(gpio, IOCTL_GPIO__CLEAR_PIN, &GPIO_LED_RED);
                                        }
                                        break;

                                default:
                                        puts("UNKNOWN REQUEST [OUT]");
                                        print_setup(&setup.packet);
                                        ioctl(ep0, IOCTL_USBD__SET_ERROR_STATUS);
                                }
                        }
                }

                ioctl(ep0, IOCTL_USBD__STOP);
        }

        if (ep0)
                fclose(ep0);

        if (ep1)
                fclose(ep1);

        if (gpio)
                fclose(gpio);

        puts("Exit.");

        return 0;
}

/*==============================================================================
  End of file
==============================================================================*/
