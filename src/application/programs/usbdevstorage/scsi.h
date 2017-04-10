/*=========================================================================*//**
@file    scsi.h

@author  Daniel Zorychta

@brief   SCSI definitions

@note    Copyright (C) 2014 Daniel Zorychta <daniel.zorychta@gmail.com>

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

#ifndef _SCSI_H_
#define _SCSI_H_

/*==============================================================================
  Include files
==============================================================================*/
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Exported macros
==============================================================================*/
#ifndef __packed
#define __packed  __attribute__ ((__packed__))
#endif

/** SCSI sense keys */
#define SCSI_NO_SENSE         0x00
#define SCSI_RECOVERED_ERROR  0x01
#define SCSI_NOT_READY        0x02
#define SCSI_MEDIUM_ERROR     0x03
#define SCSI_HARDWARE_ERROR   0x04
#define SCSI_ILLEGAL_REQUEST  0x05
#define SCSI_UNIT_ATTENTION   0x06
#define SCSI_DATA_PROTECT     0x07
#define SCSI_BLANK_CHECK      0x08
#define SCSI_VENDOR_SPECIFIC  0x09
#define SCSI_COPY_ABORTED     0x0A
#define SCSI_ABORTED_COMMAND  0x0B
#define SCSI_VOLUME_OVERFLOW  0x0D
#define SCSI_MISCOMPARE       0x0E

/*==============================================================================
  Exported object types
==============================================================================*/
/**
 * The type contains SCSI requests
 */
typedef enum {
        SCSI_REQUEST__TEST_UNIT_READY                   = 0x00,
        SCSI_REQUEST__REQUEST_SENSE                     = 0x03,
        SCSI_REQUEST__INQUIRY                           = 0x12,
        SCSI_REQUEST__MODE_SENSE_6                      = 0x1A,
        SCSI_REQUEST__MODE_SENSE_10                     = 0x5A,
        SCSI_REQUEST__START_STOP_UNIT                   = 0x1B,
        SCSI_REQUEST__PREVENT_ALLOW_MEDIUM_REMOVAL      = 0x1E,
        SCSI_REQUEST__READ_FORMAT_CAPACITIES            = 0x23,
        SCSI_REQUEST__READ_CAPACITY_10                  = 0x25,
        SCSI_REQUEST__READ_10                           = 0x28,
        SCSI_REQUEST__WRITE_10                          = 0x2A,
        SCSI_REQUEST__VERIFY_10                         = 0x2F
} scsi_request_t;

/**
 * The type describes a detailed error code
 */
typedef struct {
        uint8_t         error_code;
        uint8_t         obsolete;
        uint8_t         key;
        uint8_t         information[4];
        uint8_t         add_length;
        uint8_t         command_specific_information[4];
        uint8_t         code;
        uint8_t         qualifier;
        uint8_t         specific[4];
} __packed scsi_request_sense6_data_t;

/**
 * The type describes general information of a device
 */
typedef struct {
        uint8_t         pq_pdt;
        uint8_t         rmb;
        uint8_t         version;
        uint8_t         format;
        uint8_t         add_length;
        uint8_t         flags[3];
        char            vendor[8];
        char            product[16];
        char            revision[4];
} __packed scsi_inquiry_data_t;

/**
 * The type describes a device serial number
 */
typedef struct {
        uint8_t         pg_pdt;
        uint8_t         page_code;
        uint16_t        page_length;
        char            serial_number[8];
} __packed scsi_unit_serial_number_data_t;

/**
 * The type describes detailed information of a device
 */
typedef struct {
        uint8_t         mode_data_length;
        uint8_t         medium_type;
        uint8_t         device_specific_parameter;
        uint8_t         block_descriptor_length;
} __packed scsi_mode_parameter_header6_t;

/**
 * The type describes detailed information of a device
 */
typedef struct {
        uint16_t        mode_data_length;
        uint8_t         medium_type;
        uint8_t         device_specific_parameter;
        uint8_t         long_lba;
        uint8_t         reserved;
        uint16_t        block_descriptor_length;
} __packed scsi_mode_parameter_header10_t;

/**
 * The type describes information about disc capacity
 */
typedef struct {
        uint8_t         reserved[3];
        uint8_t         capacity_list_length;
        uint32_t        lba_count;
        uint8_t         descriptor_code;
        uint8_t         block_length_msb;
        uint16_t        block_length;
} __packed scsi_format_capacity_data_t;

/**
 * The type that describes a device capacity
 */
typedef struct {
        uint32_t        last_lba;
        uint32_t        block_length;
} __packed scsi_capacity_data_t;

/*==============================================================================
  Exported objects
==============================================================================*/

/*==============================================================================
  Exported functions
==============================================================================*/

/*==============================================================================
  Exported inline functions
==============================================================================*/

#ifdef __cplusplus
}
#endif

#endif /* _SCSI_H_ */
/*==============================================================================
  End of file
==============================================================================*/
