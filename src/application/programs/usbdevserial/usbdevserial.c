/*=========================================================================*//**
@file    usbdevserial.c

@author  Daniel Zorychta

@brief   USB Virtual Serial Port example

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
#include <dnx/thread.h>
#include <dnx/os.h>
#include <unistd.h>

/*==============================================================================
  Local symbolic constants/macros
==============================================================================*/
#define tostring(a)             #a
#define USB_mA(mA)              (mA / 2)

#define INTERRUPT_BUF_SIZE      16
#define BULK_BUF_SIZE           64

/*==============================================================================
  Local types, enums definitions
==============================================================================*/
typedef struct {
        usb_configuration_descriptor_t          configuration_descriptor;
        usb_interface_descriptor_t              interface_descriptor_0;
        usb_cdc_header_descriptor_t             cdc_header_descriptor;
        usb_cdc_call_management_descriptor_t    cdc_call_manag_descriptor;
        usb_cdc_acm_descriptor_t                cdc_acm_descriptor;
        usb_cdc_union_descriptor_t              cdc_union_descriptor;
        usb_endpoint_descriptor_t               ep2in_descriptor;
        usb_interface_descriptor_t              interface_descriptor_1;
        usb_endpoint_descriptor_t               ep1out_descriptor;
        usb_endpoint_descriptor_t               ep1in_descriptor;
} __packed usb_com_configuration_t;

typedef struct {
        usb_notification_packet_t               notification;
        u16_t                                   state;
} __packed usb_serial_state_notification_t;

/*==============================================================================
  Local function prototypes
==============================================================================*/

/*==============================================================================
  Local object definitions
==============================================================================*/
GLOBAL_VARIABLES_SECTION {
        usb_cdc_line_coding_t           line_coding;
        u8_t                            buffer[BULK_BUF_SIZE];
        bool                            configured;
};

static const USBD_ep_config_t ep_cfg = {
        .ep[USB_EP_NUM__ENDP0] = USBD_EP_CONFIG_IN_OUT(USB_TRANSFER__CONTROL, USBD_EP0_SIZE, USBD_EP0_SIZE),
        .ep[USB_EP_NUM__ENDP1] = USBD_EP_CONFIG_IN_OUT(USB_TRANSFER__BULK, BULK_BUF_SIZE, BULK_BUF_SIZE),
        .ep[USB_EP_NUM__ENDP2] = USBD_EP_CONFIG_IN(USB_TRANSFER__INTERRUPT, INTERRUPT_BUF_SIZE),
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
        .bDeviceClass       = USB_CLASS__COMMUNICATION_DEVICE,
        .bDeviceSubClass    = USB_SUBCLASS__SPECIFIED_AT_INTERFACE_LEVEL,
        .bDeviceProtocol    = USB_PROTOCOL__SPECIFIED_AT_INTERFACE_LEVEL,
        .bMaxPacketSize0    = USBD_EP0_SIZE,
        .idVendor           = HTOUSBS(0x0483),
        .idProduct          = HTOUSBS(0x5754),
        .bcdDevice          = HTOUSBS(0x0100),
        .iManufacturer      = 1,
        .iProduct           = 2,
        .iSerialNumber      = 3,
        .bNumConfigurations = 1
};

static const usb_com_configuration_t com_configuration = {
        .configuration_descriptor = {
                .bLength                = sizeof(usb_configuration_descriptor_t),
                .bDescriptorType        = CONFIGURATION_DESCRIPTOR,
                .wTotalLength           = HTOUSBS(sizeof(usb_com_configuration_t)),
                .bNumInterfaces         = 2,
                .bConfigurationValue    = 1,
                .iConfiguration         = 0,
                .bmAttributes           = D7_RESERVED,  /* bus powered */
                .bMaxPower              = USB_mA(100)
        },

        .interface_descriptor_0 = {
                .bLength                = sizeof(usb_interface_descriptor_t),
                .bDescriptorType        = INTERFACE_DESCRIPTOR,
                .bInterfaceNumber       = 0,
                .bAlternateSetting      = 0,
                .bNumEndpoints          = 1,
                .bInterfaceClass        = USB_CLASS__COMMUNICATION_INTERFACE,
                .bInterfaceSubClass     = USB_SUBCLASS__ABSTRACT_CONTROL_MODEL,
                .bInterfaceProtocol     = 0,
                .iInterface             = 0
        },

        .cdc_header_descriptor = {
                .bFunctionLength        = sizeof(usb_cdc_header_descriptor_t),
                .bDescriptorType        = CS_INTERFACE_DESCRIPTOR,
                .bDescriptorSubtype     = CDC_HEADER_DESCRIPTOR,
                .bcdCDC                 = HTOUSBS(0x120)
        },

        .cdc_call_manag_descriptor = {
                .bFunctionLength        = sizeof(usb_cdc_call_management_descriptor_t),
                .bDescriptorType        = CS_INTERFACE_DESCRIPTOR,
                .bDescriptorSubtype     = CDC_CALL_MANAGEMENT_DESCRIPTOR,
                .bmCapabilities         = 0x3,
                .bDataInterface         = 1
        },

        .cdc_acm_descriptor = {
                .bFunctionLength        = sizeof(usb_cdc_acm_descriptor_t),
                .bDescriptorType        = CS_INTERFACE_DESCRIPTOR,
                .bDescriptorSubtype     = CDC_ACM_DESCRIPTOR,
                .bmCapabilities         = 0x2
        },

        .cdc_union_descriptor = {
                .bFunctionLength        = sizeof(usb_cdc_union_descriptor_t),
                .bDescriptorType        = CS_INTERFACE_DESCRIPTOR,
                .bDescriptorSubtype     = CDC_UNION_DESCRIPTOR,
                .bControlInterface      = 0,
                .bSubordinateInterface0 = 1
        },

        .ep2in_descriptor = {
                .bLength                = sizeof(usb_endpoint_descriptor_t),
                .bDescriptorType        = ENDPOINT_DESCRIPTOR,
                .bEndpointAddress       = USB_EP_NUM__ENDP2 | USB_ENDP_IN,
                .bmAttributes           = USB_TRANSFER__INTERRUPT,
                .wMaxPacketSize         = HTOUSBS(INTERRUPT_BUF_SIZE),
                .bInterval              = 10
        },

        .interface_descriptor_1 = {
                .bLength                = sizeof(usb_interface_descriptor_t),
                .bDescriptorType        = INTERFACE_DESCRIPTOR,
                .bInterfaceNumber       = 1,
                .bAlternateSetting      = 0,
                .bNumEndpoints          = 2,
                .bInterfaceClass        = USB_CLASS__DATA_INTERFACE,
                .bInterfaceSubClass     = 0,
                .bInterfaceProtocol     = 0,
                .iInterface             = 0
        },

        .ep1out_descriptor = {
                .bLength                = sizeof(usb_endpoint_descriptor_t),
                .bDescriptorType        = ENDPOINT_DESCRIPTOR,
                .bEndpointAddress       = USB_EP_NUM__ENDP1 | USB_ENDP_OUT,
                .bmAttributes           = USB_TRANSFER__BULK,
                .wMaxPacketSize         = HTOUSBS(BULK_BUF_SIZE),
                .bInterval              = 0
        },

        .ep1in_descriptor = {
                .bLength                = sizeof(usb_endpoint_descriptor_t),
                .bDescriptorType        = ENDPOINT_DESCRIPTOR,
                .bEndpointAddress       = USB_EP_NUM__ENDP1 | USB_ENDP_IN,
                .bmAttributes           = USB_TRANSFER__BULK,
                .wMaxPacketSize         = HTOUSBS(BULK_BUF_SIZE),
                .bInterval              = 0
        }
};

static const usb_device_qualifier_descriptor_t device_qualifier = {
        .bLength                = sizeof(usb_device_qualifier_descriptor_t),
        .bDescriptorType        = DEVICE_QUALIFIER_DESCRIPTOR,
        .bcdUSB                 = HTOUSBS(0x0200),
        .bDeviceClass           = USB_CLASS__SPECIFIED_AT_INTERFACE_LEVEL,
        .bDeviceSubClass        = USB_SUBCLASS__SPECIFIED_AT_INTERFACE_LEVEL,
        .bDeviceProtocol        = USB_PROTOCOL__SPECIFIED_AT_INTERFACE_LEVEL,
        .bMaxPacketSize0        = 0,
        .bNumConfigurations     = 1,
        .bReserved              = 0
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

static const usb_string_descriptor_t(6) string_product = {
        .bLength         = sizeof(usb_string_descriptor_t(6)),
        .bDescriptorType = STRING_DESCRIPTOR,
        .bString         = {HTOUSBS('S'), HTOUSBS('e'), HTOUSBS('r'), HTOUSBS('i'), HTOUSBS('a'), HTOUSBS('l')}
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

static const usb_serial_state_notification_t serial_state = {
        .notification = {
                .bmRequestType = 0xA1,
                .bNotification = 32,
                .wValue        = 0,
                .wIndex        = 0,
                .wLength       = 2,
        },

        .state = 3
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
 * @brief Serial data transfer handler
 */
//==============================================================================
static void ep1_handler(void *arg)
{
        (void) arg;

        FILE *ep1  = fopen("/dev/usbd-ep1", "r+");
        FILE *ep2  = fopen("/dev/usbd-ep2", "r+");
        FILE *gpio = fopen(GPIO_PORT_PATH, "r+");

        while (!global->configured) {
                msleep(100);
        }

        if (ep1 && ep2) {
                fwrite("", 1, 1, ep1);
                fwrite(&serial_state, 1, sizeof(usb_serial_state_notification_t), ep2);

                while (true) {
                        rewind(ep1);
                        size_t n = fread(&global->buffer, 1, BULK_BUF_SIZE, ep1);

                        if (n > 0) {
                                global->buffer[1] = '\r';
                                global->buffer[2] = '\n';
                                rewind(ep1);
                                fwrite(&global->buffer, 1, n+2, ep1);

                                char c = global->buffer[0];
                                if (c == 'W') {
                                        ioctl(gpio, IOCTL_GPIO__SET_PIN, &GPIO_LED_WHITE);
                                } else if (c == 'w') {
                                        ioctl(gpio, IOCTL_GPIO__CLEAR_PIN, &GPIO_LED_WHITE);
                                } else if (c == 'h') {
                                        static const char *hello = "Welcome to dnxRTOS!\r\n";
                                        static const char *help  = "Press 'W' to turn on and 'w' to turn off the white LED\r\n";

                                        fwrite(hello, 1, strlen(hello), ep1);
                                        fwrite(help,  1, strlen(help),  ep1);
                                }
                        }
                }
        }
}

//==============================================================================
/**
 * @brief Serial main function
 */
//==============================================================================
int_main(usbdevserial, STACK_DEPTH_LOW, int argc, char *argv[])
{
        (void)argc;
        (void)argv;

        static const thread_attr_t attr = {
                 .priority    = PRIORITY_NORMAL,
                 .stack_depth = STACK_DEPTH_LOW
        };

        FILE *ep0        = fopen("/dev/usbd-ep0", "r+");
        tid_t ep1_thread = thread_create(ep1_handler, &attr, NULL);

        if (ep0 && ep1_thread) {
                USBD_setup_container_t setup = {.timeout = 250};

                ioctl(stdin, IOCTL_VFS__NON_BLOCKING_RD_MODE);
                ioctl(ep0, IOCTL_USBD__START);

                while (true) {
                        /* check if program shall be terminated */
                        int c = getchar();
                        if (c == 'q') {
                               break;
                        }

                        /* wait for SETUP packet */
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
                                        case SET_CONTROL_LINE_STATE:
                                                printf(tostring(SET_CONTROL_LINE_STATE));
                                                printf(" [RTX = %d; DTR = %d]:",
                                                       (setup.packet.wValue & (1 << 0)) >> 0,
                                                       (setup.packet.wValue & (1 << 1)) >> 1);

                                                FILE *gpio = fopen(GPIO_PORT_PATH, "r+");
                                                if (gpio) {

                                                        if (setup.packet.wValue & (1 << 0)) {
                                                                ioctl(gpio, IOCTL_GPIO__SET_PIN, &GPIO_LED_GREEN);
                                                        } else {
                                                                ioctl(gpio, IOCTL_GPIO__CLEAR_PIN, &GPIO_LED_GREEN);
                                                        }

                                                        if (setup.packet.wValue & (1 << 1)) {
                                                                ioctl(gpio, IOCTL_GPIO__SET_PIN, &GPIO_LED_RED);
                                                        } else {
                                                                ioctl(gpio, IOCTL_GPIO__CLEAR_PIN, &GPIO_LED_RED);
                                                        }

                                                        fclose(gpio);
                                                }

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
                                                size = min(setup.packet.wLength, sizeof(com_configuration));
                                                data = &com_configuration;
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
                                        }
                                        break;

                                case GET_LINE_CODING:
                                        printf(tostring(GET_LINE_CODING)":");
                                        size = min(setup.packet.wLength, sizeof(usb_cdc_line_coding_t));
                                        data = &global->line_coding;
                                        break;
                                }

                                if (size && data) {
                                        printf(" (%d/%d)\n", fwrite(data, 1, size, ep0), (int)size);
                                } else {
                                        puts(" UNKNOWN REQUEST [IN]");
                                        print_setup(&setup.packet);
                                        ioctl(ep0, IOCTL_USBD__SET_ERROR_STATUS);
                                }

                        } else {
                                size_t size = 0;
                                size_t n    = 0;

                                switch (setup.packet.bRequest) {
                                case SET_LINE_CODING:
                                        printf(tostring(SET_LINE_CODING)":");
                                        size = min(setup.packet.wLength, sizeof(usb_cdc_line_coding_t));
                                        n    = fread(&global->line_coding, 1, size, ep0);
                                        printf(" (%d/%d)\n", cast(int, n), cast(int, size));

                                        printf("  dwDTERate  : %d\n"
                                               "  bCharFormat: %d\n"
                                               "  bParityType: %d\n"
                                               "  bDataBits  : %d\n",
                                               global->line_coding.dwDTERate,
                                               global->line_coding.bCharFormat,
                                               global->line_coding.bParityType,
                                               global->line_coding.bDataBits);

                                        global->configured = true;

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

        if (ep1_thread) {
                thread_cancel(ep1_thread);
        }

        puts("Exit.");

        return 0;
}

/*==============================================================================
  End of file
==============================================================================*/
