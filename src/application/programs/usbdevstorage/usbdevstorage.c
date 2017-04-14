/*=========================================================================*//**
@file    usbdevstorage.c

@author  Daniel Zorychta

@brief   USB storage example

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
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <dnx/misc.h>
#include <dnx/thread.h>
#include <dnx/os.h>
#include <unistd.h>
#include "scsi.h"

/*==============================================================================
  Local symbolic constants/macros
==============================================================================*/
#define tostring(a)             #a
#define USB_mA(mA)              (mA / 2)

#define REVERSE_UINT32(x)       ( (((x) << 24) & 0xFF000000U) \
                                | (((x) <<  8) & 0x00FF0000U) \
                                | (((x) >>  8) & 0x0000FF00U) \
                                | (((x) >> 24) & 0x000000FFU) )

#define REVERSE_UINT16(x)       ( (((x) << 8) & 0xFF00U) | (((x) >> 8) & 0x00FFU) )

#define BULK_BUF_SIZE           64

#define LB_SIZE                 512
#define BUFFER_SIZE             (LB_SIZE * 4)

/*==============================================================================
  Local types, enums definitions
==============================================================================*/
typedef struct {
        usb_configuration_descriptor_t          configuration_descriptor;
        usb_interface_descriptor_t              interface_descriptor;
        usb_endpoint_descriptor_t               ep1in_descriptor;
        usb_endpoint_descriptor_t               ep1out_descriptor;
} __packed usb_msc_configuration_t;

/*==============================================================================
  Local function prototypes
==============================================================================*/

/*==============================================================================
  Local object definitions
==============================================================================*/
GLOBAL_VARIABLES_SECTION {
        union {
                u8_t              buffer[BUFFER_SIZE];
                usb_msc_bot_cbw_t CBW;
                usb_msc_bot_csw_t CSW;
        } msc;

        bool  configured;
        u32_t LB_COUNT;
};

/* USB constants */
static const USBD_ep_config_t ep_cfg = {
        .ep[USB_EP_NUM__ENDP0] = USBD_EP_CONFIG_IN_OUT(USB_TRANSFER__CONTROL, USBD_EP0_SIZE, USBD_EP0_SIZE),
        .ep[USB_EP_NUM__ENDP1] = USBD_EP_CONFIG_IN_OUT(USB_TRANSFER__BULK, BULK_BUF_SIZE, BULK_BUF_SIZE),
        .ep[USB_EP_NUM__ENDP2] = USBD_EP_CONFIG_DISABLED(),
        .ep[USB_EP_NUM__ENDP3] = USBD_EP_CONFIG_DISABLED(),
        .ep[USB_EP_NUM__ENDP4] = USBD_EP_CONFIG_DISABLED(),
        .ep[USB_EP_NUM__ENDP5] = USBD_EP_CONFIG_DISABLED(),
        .ep[USB_EP_NUM__ENDP6] = USBD_EP_CONFIG_DISABLED(),
        .ep[USB_EP_NUM__ENDP7] = USBD_EP_CONFIG_DISABLED()
};

static const usb_device_descriptor_t device_descriptor = {
        .bLength             = sizeof(usb_device_descriptor_t),
        .bDescriptorType     = DEVICE_DESCRIPTOR,
        .bcdUSB              = HTOUSBS(0x0200),
        .bDeviceClass        = USB_CLASS__SPECIFIED_AT_INTERFACE_LEVEL,
        .bDeviceSubClass     = USB_SUBCLASS__SPECIFIED_AT_INTERFACE_LEVEL,
        .bDeviceProtocol     = USB_PROTOCOL__SPECIFIED_AT_INTERFACE_LEVEL,
        .bMaxPacketSize0     = USBD_EP0_SIZE,
        .idVendor            = HTOUSBS(0x0483),
        .idProduct           = HTOUSBS(0x5755),
        .bcdDevice           = HTOUSBS(0x0100),
        .iManufacturer       = 1,
        .iProduct            = 2,
        .iSerialNumber       = 3,
        .bNumConfigurations  = 1
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

static const usb_msc_configuration_t msc_configuration = {
        .configuration_descriptor = {
                .bLength                = sizeof(usb_configuration_descriptor_t),
                .bDescriptorType        = CONFIGURATION_DESCRIPTOR,
                .wTotalLength           = HTOUSBS(sizeof(usb_msc_configuration_t)),
                .bNumInterfaces         = 1,
                .bConfigurationValue    = 1,
                .iConfiguration         = 0,
                .bmAttributes           = D7_RESERVED,  /* bus powered */
                .bMaxPower              = USB_mA(100)
        },

        .interface_descriptor = {
                .bLength                = sizeof(usb_interface_descriptor_t),
                .bDescriptorType        = INTERFACE_DESCRIPTOR,
                .bInterfaceNumber       = 0,
                .bAlternateSetting      = 0,
                .bNumEndpoints          = 2,
                .bInterfaceClass        = USB_CLASS__MASS_STORAGE,
                .bInterfaceSubClass     = USB_SUBCLASS__SCSI_TRANSPARENT,
                .bInterfaceProtocol     = USB_PROTOCOL__BULK_ONLY_TRANSPORT,
                .iInterface             = 0
        },

        .ep1in_descriptor = {
                .bLength                = sizeof(usb_endpoint_descriptor_t),
                .bDescriptorType        = ENDPOINT_DESCRIPTOR,
                .bEndpointAddress       = USB_EP_NUM__ENDP1 | USB_ENDP_IN,
                .bmAttributes           = USB_TRANSFER__BULK,
                .wMaxPacketSize         = HTOUSBS(BULK_BUF_SIZE),
                .bInterval              = 0
        },

        .ep1out_descriptor = {
                .bLength                = sizeof(usb_endpoint_descriptor_t),
                .bDescriptorType        = ENDPOINT_DESCRIPTOR,
                .bEndpointAddress       = USB_EP_NUM__ENDP1 | USB_ENDP_OUT,
                .bmAttributes           = USB_TRANSFER__BULK,
                .wMaxPacketSize         = HTOUSBS(BULK_BUF_SIZE),
                .bInterval              = 0
        }
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

static const usb_string_descriptor_t(7) string_product = {
        .bLength         = sizeof(usb_string_descriptor_t(7)),
        .bDescriptorType = STRING_DESCRIPTOR,
        .bString         = {HTOUSBS('S'), HTOUSBS('t'), HTOUSBS('o'), HTOUSBS('r'), HTOUSBS('a'), HTOUSBS('g'), HTOUSBS('e')}
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

/* SCSI constants */
static const u8_t LUN = 0;

static const scsi_inquiry_data_t inquiry = {
        .pq_pdt         = 0,
        .rmb            = 0x80,
        .version        = 0,
        .format         = 0x02,
        .add_length     = sizeof(scsi_inquiry_data_t) - 5,
        .flags          = {0, 0, 0},
        .vendor         = {'D', 'a', 'n', 'i', 'e', 'l', ' ', ' '},
        .product        = {'d', 'n', 'x', 'R','T', 'O', 'S', ' ','S', 'T', 'O', 'R','A', 'G', 'E', ' '},
        .revision       = {'0', '.', '0', '1'}
};

static const scsi_unit_serial_number_data_t serial_number = {
        .pg_pdt         = 0,
        .page_code      = 0x80,
        .page_length    = REVERSE_UINT16(sizeof(scsi_unit_serial_number_data_t) - 4),
        .serial_number  = {'0', '0', '0', '0', '0', '0', '0', '1'}
};

static const scsi_mode_parameter_header6_t mode_sense6 = {
        .mode_data_length               = sizeof(scsi_mode_parameter_header6_t) - 1,
        .medium_type                    = 0,
        .device_specific_parameter      = 0,
        .block_descriptor_length        = 0
};

static const scsi_mode_parameter_header10_t mode_sense10 = {
        .mode_data_length               = REVERSE_UINT16(sizeof(scsi_mode_parameter_header10_t) - 2),
        .medium_type                    = 0,
        .device_specific_parameter      = 0,
        .long_lba                       = 0,
        .reserved                       = 0,
        .block_descriptor_length        = 0
};

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
 * @brief Prints error when request is not recognized
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
 * @brief SCSI command handler
 */
//==============================================================================
static void ep1_handler(void *arg)
{
        (void)arg;

        FILE *gpio = fopen(GPIO_PORT_PATH, "r+");
        FILE *ep1  = fopen("/dev/usbd-ep1", "r+");
        FILE *sda  = fopen("/dev/sda", "r+");

        if (sda) {
                struct stat stat;
                if (fstat(sda, &stat) == 0) {
                        global->LB_COUNT = stat.st_size / LB_SIZE;
                }
        }

        while (!global->configured) {
                msleep(10);
        }

        u32_t tag;
        u32_t len;
        u32_t lba;
        u32_t lbc;
        u8_t  CSW_status;

        scsi_request_sense6_data_t request_sense6;
        request_sense6.error_code                       = 0x70;
        request_sense6.obsolete                         = 0;
        request_sense6.key                              = SCSI_NO_SENSE;
        request_sense6.information[0]                   = 0;
        request_sense6.information[1]                   = 0;
        request_sense6.information[2]                   = 0;
        request_sense6.information[3]                   = 0;
        request_sense6.add_length                       = sizeof(request_sense6) - 8;
        request_sense6.command_specific_information[0]  = 0;
        request_sense6.command_specific_information[1]  = 0;
        request_sense6.command_specific_information[2]  = 0;
        request_sense6.command_specific_information[3]  = 0;
        request_sense6.code                             = 0;
        request_sense6.qualifier                        = 0;
        request_sense6.specific[0]                      = 0;
        request_sense6.specific[1]                      = 0;
        request_sense6.specific[2]                      = 0;
        request_sense6.specific[3]                      = 0;

        while (ep1) {
                size_t n = fread(&global->msc.CBW, 1, BULK_BUF_SIZE, ep1);
                if (n == 0)
                        continue;

                tag        = global->msc.CBW.dCBWTag;
                len        = global->msc.CBW.dCBWDataTransferLength;
                CSW_status = USB_MASS_STORAGE_BOT_CSW_COMMAND_FAILED;

                if (  global->msc.CBW.dCBWSignature != USB_MASS_STORAGE_BOT_CBW_SIGNATURE
                   || global->msc.CBW.bCBWLUN       != LUN
                   || global->msc.CBW.bCBWCBLength  >  USB_MASS_STORAGE_REQUEST_CBWCB_LENGTH
                   || n                             != sizeof(usb_msc_bot_cbw_t) ) {

                        puts("Invalid SCSI command");

                        printf("global->msc.CBW.dCBWSignature: 0x%x (0x%x)\n", global->msc.CBW.dCBWSignature, USB_MASS_STORAGE_BOT_CBW_SIGNATURE);
                        printf("global->msc.CBW.bCBWLUN: %d\n", global->msc.CBW.bCBWLUN);
                        printf("global->msc.CBW.bCBWCBLength: %d (max %d)\n", global->msc.CBW.bCBWCBLength, USB_MASS_STORAGE_REQUEST_CBWCB_LENGTH);
                        printf("read: %d\n (max %d)\n", cast(int, n), cast(int, sizeof(usb_msc_bot_cbw_t)));

                        static const u16_t EP1IN  = USB_ENDP_IN  | USB_EP_NUM__ENDP1;
                        static const u16_t EP1OUT = USB_ENDP_OUT | USB_EP_NUM__ENDP1;
                        ioctl(ep1, IOCTL_USBD__SET_EP_STALL, &EP1IN);
                        ioctl(ep1, IOCTL_USBD__SET_EP_STALL, &EP1OUT);

                        global->configured = false;
                        while (!global->configured) {
                                msleep(5);
                        }
                } else {

                        switch (global->msc.CBW.CBWCB[0]) {
                        case SCSI_REQUEST__TEST_UNIT_READY:
                                puts("TEST UNIT READY");
                                CSW_status = USB_MASS_STORAGE_BOT_CSW_COMMAND_PASSED;
                                break;

                        case SCSI_REQUEST__INQUIRY: {
                                u16_t length = global->msc.CBW.CBWCB[3] << 8 | global->msc.CBW.CBWCB[4];
                                if (global->msc.CBW.CBWCB[1] == 0 || global->msc.CBW.CBWCB[2] == 0) {
                                        fwrite(&inquiry, 1, min(length, sizeof(inquiry)), ep1);
                                        CSW_status = USB_MASS_STORAGE_BOT_CSW_COMMAND_PASSED;
                                        puts("INQUIRY");
                                } else if (global->msc.CBW.CBWCB[1] == 1 || global->msc.CBW.CBWCB[2] == 0x80) {
                                        fwrite(&serial_number, 1, min(length, sizeof(serial_number)), ep1);
                                        CSW_status = USB_MASS_STORAGE_BOT_CSW_COMMAND_PASSED;
                                        puts("INQUIRY (SERIAL)");
                                } else {
                                        CSW_status = USB_MASS_STORAGE_BOT_CSW_COMMAND_FAILED;
                                        puts("INQUIRY UNKNOWN REQUEST");
                                }
                                break;
                        }

                        case SCSI_REQUEST__REQUEST_SENSE:
                                puts("REQUEST SENSE");
                                memset(&global->msc.buffer, 0, LB_SIZE);
                                memcpy(&global->msc.buffer, &request_sense6, sizeof(request_sense6));
                                fwrite(&global->msc.buffer, 1, min(len, LB_SIZE), ep1);
                                CSW_status = USB_MASS_STORAGE_BOT_CSW_COMMAND_PASSED;
                                break;

                        case SCSI_REQUEST__MODE_SENSE_6:
                                puts("MODE SENSE (6)");
                                memset(&global->msc.buffer, 0, LB_SIZE);
                                memcpy(&global->msc.buffer, &mode_sense6, sizeof(mode_sense6));
                                fwrite(&global->msc.buffer, 1, min(len, LB_SIZE), ep1);
                                CSW_status = USB_MASS_STORAGE_BOT_CSW_COMMAND_PASSED;
                                break;

                        case SCSI_REQUEST__MODE_SENSE_10:
                                puts("MODE SENSE (10)");
                                memset(&global->msc.buffer, 0, LB_SIZE);
                                memcpy(&global->msc.buffer, &mode_sense10, sizeof(mode_sense10));
                                fwrite(&global->msc.buffer, 1, min(len, LB_SIZE), ep1);
                                CSW_status = USB_MASS_STORAGE_BOT_CSW_COMMAND_PASSED;
                                break;

                        case SCSI_REQUEST__READ_CAPACITY_10:
                                puts("READ CAPACITY (10)");
                                scsi_capacity_data_t capacity = {
                                        .last_lba       = REVERSE_UINT32(global->LB_COUNT - 1),
                                        .block_length   = REVERSE_UINT32(LB_SIZE)
                                };
                                fwrite(&capacity, 1, sizeof(capacity), ep1);
                                CSW_status = USB_MASS_STORAGE_BOT_CSW_COMMAND_PASSED;
                                break;

                        case SCSI_REQUEST__READ_FORMAT_CAPACITIES:
                                puts("READ FORMAT CAPACITIES");
                                scsi_format_capacity_data_t format_capacity = {
                                        .reserved               = {0, 0, 0},
                                        .capacity_list_length   = sizeof(scsi_format_capacity_data_t) - 4,
                                        .lba_count              = REVERSE_UINT32(global->LB_COUNT),
                                        .descriptor_code        = 1,
                                        .block_length_msb       = 0,
                                        .block_length           = REVERSE_UINT16(LB_SIZE)
                                };
                                fwrite(&format_capacity, 1, sizeof(format_capacity), ep1);
                                CSW_status = USB_MASS_STORAGE_BOT_CSW_COMMAND_PASSED;
                                break;

                        case SCSI_REQUEST__START_STOP_UNIT:
                                puts("START STOP UNIT");
                                CSW_status = USB_MASS_STORAGE_BOT_CSW_COMMAND_PASSED;
                                break;

                        case SCSI_REQUEST__PREVENT_ALLOW_MEDIUM_REMOVAL:
                                puts("PREVENT ALLOW MEDIUM REMOVAL");
                                CSW_status = USB_MASS_STORAGE_BOT_CSW_COMMAND_PASSED;
                                break;

                        case SCSI_REQUEST__READ_10:
                                ioctl(gpio, IOCTL_GPIO__SET_PIN, &GPIO_LED_GREEN);

                                lba = global->msc.CBW.CBWCB[2] << 24
                                    | global->msc.CBW.CBWCB[3] << 16
                                    | global->msc.CBW.CBWCB[4] <<  8
                                    | global->msc.CBW.CBWCB[5] <<  0;

                                lbc = global->msc.CBW.CBWCB[7] << 8
                                    | global->msc.CBW.CBWCB[8] << 0;

                                printf("READ (10) [%d, %d]\n", lba, lbc);

                                if (sda) {
                                        fseek(sda, lba * LB_SIZE, SEEK_SET);

                                        while (lbc) {
                                                int count = min(lbc, BUFFER_SIZE / LB_SIZE);
                                                fread(&global->msc.buffer, LB_SIZE, count, sda);
                                                fwrite(&global->msc.buffer, LB_SIZE, count, ep1);
                                                lbc -= count;
                                        }
                                } else {
                                        memset(&global->msc.buffer, 0, LB_SIZE);

                                        while (lbc) {
                                                fwrite(&global->msc.buffer, LB_SIZE, 1, ep1);
                                                lbc--;
                                        }
                                }

                                CSW_status = USB_MASS_STORAGE_BOT_CSW_COMMAND_PASSED;
                                ioctl(gpio, IOCTL_GPIO__CLEAR_PIN, &GPIO_LED_GREEN);
                                break;

                        case SCSI_REQUEST__WRITE_10:
                                ioctl(gpio, IOCTL_GPIO__SET_PIN, &GPIO_LED_RED);

                                lba = global->msc.CBW.CBWCB[2] << 24
                                    | global->msc.CBW.CBWCB[3] << 16
                                    | global->msc.CBW.CBWCB[4] <<  8
                                    | global->msc.CBW.CBWCB[5] <<  0;

                                lbc = global->msc.CBW.CBWCB[7] << 8
                                    | global->msc.CBW.CBWCB[8] << 0;

                                printf("WRITE (10) [%d, %d]\n", lba, lbc);

                                if (sda) {
                                        fseek(sda, lba * LB_SIZE, SEEK_SET);
                                        while (lbc) {
                                                int count = min(lbc, BUFFER_SIZE / LB_SIZE);
                                                fread(&global->msc.buffer, LB_SIZE, count, ep1);
                                                fwrite(&global->msc.buffer, LB_SIZE, count, sda);
                                                lbc -= count;
                                        }
                                } else {
                                        while (lbc) {
                                                fread(&global->msc.buffer, LB_SIZE, 1, ep1);
                                                lbc--;
                                        }
                                }

                                CSW_status = USB_MASS_STORAGE_BOT_CSW_COMMAND_PASSED;
                                ioctl(gpio, IOCTL_GPIO__CLEAR_PIN, &GPIO_LED_RED);
                                break;

                        default:
                                printf(" dCBWSignature: 0x%x\n"
                                       " dCBWTag: 0x%x\n"
                                       " dCBWDataTransferLength: 0x%x\n"
                                       " bmCBWFlags: 0x%x\n"
                                       " bCBWLUN: 0x%x\n"
                                       " bCBWCBLength: 0x%x\n",
                                       global->msc.CBW.dCBWSignature,
                                       global->msc.CBW.dCBWTag,
                                       global->msc.CBW.dCBWDataTransferLength,
                                       global->msc.CBW.bmCBWFlags,
                                       global->msc.CBW.bCBWLUN,
                                       global->msc.CBW.bCBWCBLength);

                                for (int i = 0; i < 16; i++) {
                                        printf(" CBWCB[%d]: 0x%x\n", i, global->msc.CBW.CBWCB[i]);
                                }

                                request_sense6.key  = SCSI_ILLEGAL_REQUEST;
                                request_sense6.code = 0x20;
                                CSW_status = USB_MASS_STORAGE_BOT_CSW_COMMAND_FAILED;
                                break;
                        }

                        global->msc.CSW.dCSWSignature   = USB_MASS_STORAGE_BOT_CSW_SIGNATURE;
                        global->msc.CSW.dCSWTag         = tag;
                        global->msc.CSW.dCSWDataResidue = 0;
                        global->msc.CSW.dCSWStatus      = CSW_status;
                        fwrite(&global->msc.CSW, 1, sizeof(usb_msc_bot_csw_t), ep1);
                }
        }

        if (gpio)
                fclose(gpio);

        if (ep1)
                fclose(ep1);

        if (sda)
                fclose(sda);
}

//==============================================================================
/**
 * @brief Storage main function
 */
//==============================================================================
int_main(usbdevstorage, STACK_DEPTH_LOW, int argc, char *argv[])
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
                        ioctl(ep0, IOCTL_USBD__WAS_RESET, &was_reset);;

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
                                                global->configured = true;
                                                break;
                                        }
                                        break;

                                case INTERFACE_RECIPIENT:
                                        switch (setup.packet.bRequest) {
                                        case BULK_ONLY_RESET:
                                                printf(tostring(BULK_ONLY_RESET)":");
                                                operation = 0;
                                                break;
                                        }
                                        break;

                                case ENDPOINT_RECIPIENT:
                                        switch (setup.packet.bRequest) {
                                        case CLEAR_FEATURE:
                                                printf(tostring(CLEAR_FEATURE)": ");
                                                switch (setup.packet.wValue) {
                                                case ENDPOINT_HALT:
                                                        printf(tostring(ENDPOINT_HALT)" (0x%x):", setup.packet.wIndex);
                                                        ioctl(ep0, IOCTL_USBD__SET_EP_VALID, &setup.packet.wIndex);
                                                        operation = 0;
                                                        break;
                                                }
                                        }
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
                                                size = min(setup.packet.wLength, sizeof(msc_configuration));
                                                data = &msc_configuration;
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

                                        case GET_MAX_LUN:
                                                printf(tostring(GET_MAX_LUN)":");
                                                size = min(setup.packet.wLength, sizeof(LUN));
                                                data = &LUN;
                                                global->configured = true;
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
                                default:
                                        puts("UNKNOWN REQUEST [OUT]");
                                        print_setup(&setup.packet);
                                        ioctl(ep0, IOCTL_USBD__SET_ERROR_STATUS);
                                }
                        }
                }

                ioctl(ep0, IOCTL_USBD__STOP);
        }

        if (ep0) {
                fclose(ep0);
        }

        if (ep1_thread) {
                thread_cancel(ep1_thread);
        }

        puts("Exit.");

        return 0;
}

/*==============================================================================
  End of file
==============================================================================*/
