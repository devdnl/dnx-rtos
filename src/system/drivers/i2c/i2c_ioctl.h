/*=========================================================================*//**
@file    i2c_ioctl.h

@author  Daniel Zorychta

@brief   This driver support I2C peripherals.

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

/**
@defgroup drv-i2c I2C Driver

\section drv-i2c-desc Description
Driver handles I2C peripheral.

\section drv-i2c-sup-arch Supported architectures
\li stm32f1
\li stm32f4

\section drv-i2c-ddesc Details
\subsection drv-i2c-ddesc-num Meaning of major and minor numbers
Some manufactures enumerate devices starting from 1 instead of 0 (e.g. ST).
In this case major number starts from 0 and is connected to the first device
e.g. I2C1.
\arg major number selects I2C peripheral e.g.:@n
     major = 0 -> I2C0 (or I2C1 if numbered from 1)
\arg minor number selects particular device e.g.:@n
     major = 0 and minor = 0 -> device 0 on I2C0,@n
     major = 0 and minor = 1 -> device 1 on I2C0,@n
     major = 0 and minor = n -> device n on I2C0,@n
     major = 1 and minor = 0 -> device 0 on I2C1,@n
     major = 1 and minor = 1 -> device 1 on I2C1,@n
     major = 1 and minor = n -> device n on I2C1

\subsubsection drv-i2c-ddesc-numres Numeration restrictions
Number of peripherals determines how big the major number can be. If there is
only one I2C peripheral then the major number is always 0.
Number of devices (minor number) can be theoretically up to 256 or to limits
of memory capacity. Minor number concerns communication that dnx RTOS is a master.
When dnx RTOS is a slave then only one minor device should be used.

\subsection drv-i2c-ddesc-init Driver initialization
To initialize driver the following code can be used:

@code
driver_init("I2C", 0, 0, "/dev/I2C0-0");  // device path can be more descriptive
@endcode
@code
driver_init("I2C", 0, 1, "/dev/I2C0-1");  // next I2C device on I2C0 bus
@endcode

\subsection drv-i2c-ddesc-release Driver release
To release driver the following code can be used:
@code
driver_release("I2C", 0, 0);
@endcode
@code
driver_release("I2C", 0, 1);
@endcode

\subsection drv-i2c-ddesc-cfg Driver configuration
Driver configuration should be done before usage and after initialization.
The best place to do this is user application or initialization daemon.

\subsubsection drv-i2c-ddesc-cfg-master Master mode
To configure particular I2C device as master the ioctl() function shall be used
as follow (EEPROM example):
@code
#include <stdio.h>
#include <stdbool.h>
#include <sys/ioctl.h>

static const I2C_config_t cfg = {
      .address       = 0xA0,                          // EEPROM address
      .sub_addr_mode = I2C_SUB_ADDR_MODE__2_BYTES,    // EEPROM up to 64KiB
      .addr_10bit    = false,                         // 7-bit address
      .slave_mode    = false                          // master mode
};

static const char *dev_path = "/dev/I2C0-0";

FILE *dev = fopen(dev_path, "r+");
if (dev) {
      if (ioctl(fileno(dev), IOCTL_I2C__CONFIGURE, &cfg) != 0) {
            perror(dev_path);
      }

      fclose(dev);
} else {
      perror(dev_path);
}

...
@endcode

\subsubsection drv-i2c-ddesc-cfg-slave Slave mode
To configure particular I2C device as slave the ioctl() function shall be used
as follow:
@code
#include <stdio.h>
#include <stdbool.h>
#include <sys/ioctl.h>

static const I2C_config_t cfg = {
      .address       = 0xA0,                          // microcontroller address
      .sub_addr_mode = I2C_SUB_ADDR_MODE__DISABLED,   // don't care in slave mode
      .addr_10bit    = false,                         // 7-bit address
      .slave_mode    = true                           // slave mode
};

static const char *dev_path = "/dev/I2C0-0";

FILE *dev = fopen(dev_path, "r+");
if (dev) {
      if (ioctl(fileno(dev), IOCTL_I2C__CONFIGURE, &cfg) != 0) {
            perror(dev_path);
      }

      fclose(dev);
} else {
      perror(dev_path);
}

...
@endcode

\subsubsection drv-i2c-ddesc-cfg-suba Meaning of Sub-Address Mode
Some I2C devices need to set internal address (called pointer) in purpose of
selection of register or memory address. Small devices e.g. RTC or sensors
uses mostly 1-byte sub-address to select particular register to write or
read. EEPROM devices uses also 1-byte sub-address but there is many devices
that uses 2-byte sub-address mode. Some devices do not use any sub-addressing
mode. Sub-address mode can be selected in configuration by using particular
enumeration value. Sub-address value is controlled by fseek() function.
The sub-address functionality works only in master mode.

\paragraph drv-i2c-ddesc-cfg-subadis Sub-addressing disabled
When this selection is used then I2C driver does not send any sub-address
bytes. Write and read operations are presented below:

<tt>Write: [S][ADDR+W][DATA0][DATA1][DATAn]...[P]</tt>

<tt>Read:  [S][ADDR+R][DATA0][DATA1][DATAn]...[P]</tt>

\paragraph drv-i2c-ddesc-cfg-suba1b 1-byte sub-addressing
When this selection is used then I2C driver send 1 byte of sub-address.
In case of read sequence, the I2C driver first send @b POINTER according to
fseek() value and next read data sequence.

<tt>Write: [S][ADDR+W][POINTER][DATA1][DATAn]...[P]</tt>

<tt>Read:  [S][ADDR+W][POINTER][Sr][ADDR+R][DATA1][DATAn]...[P]</tt>

\paragraph drv-i2c-ddesc-cfg-suba2b 2-byte sub-addressing
When this selection is used then I2C driver send 2 bytes of sub-address.
In case of read sequence, the I2C driver first send @b POINTER according to
fseek() value and next read data sequence.

<tt>Write: [S][ADDR+W][POINTER:1][POINTER:0][DATA1][DATAn]...[P]</tt>

<tt>Read:  [S][ADDR+W][POINTER:1][POINTER:0][Sr][ADDR+R][DATA1][DATAn]...[P]</tt>

\paragraph drv-i2c-ddesc-cfg-suba3b 3-byte sub-addressing
When this selection is used then I2C driver send 3 bytes of sub-address.
In case of read sequence, the I2C driver first send @b POINTER according to
fseek() value and next read data sequence.

<tt>Write: [S][ADDR+W][POINTER:2][POINTER:1][POINTER:0][DATA1][DATAn]...[P]</tt>

<tt>Read:  [S][ADDR+W][POINTER:2][POINTER:1][POINTER:0][Sr][ADDR+R][DATA1][DATAn]...[P]</tt>

\subsection drv-i2c-ddesc-write Data write
\subsubsection drv-i2c-ddesc-write-master Master mode
Data to the I2C device can be written as regular file. Example assumptions:
- EEPROM device is AT24C32 (4KiB EEPROM)
- EEPROM address is 0xA0
- EEPROM is visible in system as "/dev/ee"

@code
#include <stdio.h>
#include <stdbool.h>
#include <dnx/misc.h>
#include <sys/ioctl.h>

static const char *dev_path = "/dev/ee";
static const char *blk1     = "Data block 1 at 0x0";
static const char *blk2     = "Data block 2 at 0x100";

static const I2C_config_t cfg = {
      .address       = 0xA0,                          // EEPROM address
      .sub_addr_mode = I2C_SUB_ADDR_MODE__2_BYTES,    // EEPROM up to 64KiB
      .addr_10bit    = false,                         // 7-bit address
      .slave_mode    = false                          // master mode
};

int_main(ee_ex, STACK_DEPTH_MEDIUM, int argc, char *argv[])
{
      FILE *dev = fopen(dev_path, "r+");

      if (dev) {
            // set I2C device configuration
            ioctl(fileno(dev), IOCTL_I2C__CONFIGURE, &cfg);

            // write blk1 at address 0x0
            fseek(dev, 0x0, SEEK_SET);
            fwrite(blk1, sizeof(char), strsize(blk1), dev);

            // write blk2 at adress 0x100
            fseek(dev, 0x100, SEEK_SET);
            fwrite(blk2, sizeof(char), strsize(blk2), dev);

            // close I2C device
            fclose(dev);

      } else {
            perror(dev_path);
      }

      return 0;
}
@endcode

\subsubsection drv-i2c-ddesc-write-slave Slave mode
On slave mode, data can be written to the peripheral only when ADDR+RD request
is received from master device. For master is a read operation but for slave
device write. In this case the ioctl() function should be used to obtain the
data direction and selection event.

@code
#include <stdio.h>
#include <stdbool.h>
#include <dnx/misc.h>
#include <sys/ioctl.h>

static const char *dev_path = "/dev/I2C0";

static const I2C_config_t cfg = {
      .address       = 0xA0,                          // microcontroller address
      .sub_addr_mode = I2C_SUB_ADDR_MODE__DISABLED,   // don't care in slave mode
      .addr_10bit    = false,                         // 7-bit address
      .slave_mode    = true                           // slave mode
};

int_main(slave, STACK_DEPTH_MEDIUM, int argc, char *argv[])
{
      FILE *dev = fopen(dev_path, "r+");

      if (dev) {
            // set I2C device configuration
            ioctl(fileno(dev), IOCTL_I2C__CONFIGURE, &cfg);

            while (true) {
                    I2C_selection_t sel = {.timeout_ms = MAX_DELAY_MS};

                    if (ioctl(fileno(dev), IOCTL_I2C__SLAVE_WAIT_FOR_SELECTION, &sel) == 0) {

                        u8_t buf[64];
                        memset(buf, 0, sizeof(buf));

                        if (sel.RD_addr) {
                                // slave write data to master device (master read
                                // request).

                                strncpy(buf, "FROM-SLAVE", sizeof(buf)-1);

                                size_t n = fwrite(buf, 1, strlen(buf), i2c);

                                printf("Written: %d bytes\n", n);

                        } else {
                                // slave device read data from peripheral sent
                                // by master device.

                                size_t n = fread(buf, 1, sizeof(buf) - 1, i2c);

                                printf("Received: %d bytes\n", n);

                                // print received data
                                for (uint i = 0; i < n; i++) {
                                        printf("%02X ", buf[i]);
                                }
                                puts("");
                        }

                    } else {
                            perror(dev_path);
                    }
            }

            // close I2C device
            fclose(dev);

      } else {
            perror(dev_path);
      }

      return 0;
}
@endcode

\subsection drv-i2c-ddesc-read Data read
\subsubsection drv-i2c-ddesc-read-master Master mode
Data to the I2C device can be read as regular file. Example assumptions:
- EEPROM device is AT24C32 (4KiB EEPROM)
- EEPROM address is 0xA0
- EEPROM is visible in system as "/dev/ee"

@code
#include <stdio.h>
#include <stdbool.h>
#include <dnx/misc.h>
#include <sys/ioctl.h>

static const char *dev_path = "/dev/ee";

static const I2C_config_t cfg = {
      .address       = 0xA0,                          // EEPROM address
      .sub_addr_mode = I2C_SUB_ADDR_MODE__2_BYTES,    // EEPROM up to 64KiB
      .addr_10bit    = false                          // 7-bit address
};

GLOBAL_VARIABLES_SECTION {
      char blk1[100];
      char blk2[100];
};

int_main(ee_ex, STACK_DEPTH_MEDIUM, int argc, char *argv[])
{
      FILE *dev = fopen(dev_path, "r+");

      if (dev) {
            // set I2C device configuration
            ioctl(fileno(dev), IOCTL_I2C__CONFIGURE, &cfg);

            // read blk1 at address 0x0
            fseek(dev, 0x0, SEEK_SET);
            fread(global->blk1, ARRAY_ITEM_SIZE(global->blk1), ARRAY_SIZE(global->blk1), dev);

            // write blk2 at adress 0x100
            fseek(f, 0x100, SEEK_SET);
            fread(global->blk2, ARRAY_ITEM_SIZE(global->blk2), ARRAY_SIZE(global->blk2), dev);

            // close I2C device
            fclose(dev);

      } else {
            perror(dev_path);
      }

      return 0;
}
@endcode

\subsubsection drv-i2c-ddesc-read-slave Slave mode
On slave mode, data can be read from the peripheral only when ADDR+WR request
is received from master device. For master is a write operation but for slave
device read. In this case the ioctl() function should be used to obtain the
data direction and selection event.

@code
#include <stdio.h>
#include <stdbool.h>
#include <dnx/misc.h>
#include <sys/ioctl.h>

static const char *dev_path = "/dev/I2C0";

static const I2C_config_t cfg = {
      .address       = 0xA0,                          // microcontroller address
      .sub_addr_mode = I2C_SUB_ADDR_MODE__DISABLED,   // don't care in slave mode
      .addr_10bit    = false,                         // 7-bit address
      .slave_mode    = true                           // slave mode
};

int_main(slave, STACK_DEPTH_MEDIUM, int argc, char *argv[])
{
      FILE *dev = fopen(dev_path, "r+");

      if (dev) {
            // set I2C device configuration
            ioctl(fileno(dev), IOCTL_I2C__CONFIGURE, &cfg);

            while (true) {
                    I2C_selection_t sel = {.timeout_ms = MAX_DELAY_MS};

                    if (ioctl(fileno(dev), IOCTL_I2C__SLAVE_WAIT_FOR_SELECTION, &sel) == 0) {

                        u8_t buf[64];
                        memset(buf, 0, sizeof(buf));

                        if (sel.RD_addr) {
                                // slave write data to master device (master read
                                // request).

                                strncpy(buf, "FROM-SLAVE", sizeof(buf)-1);

                                size_t n = fwrite(buf, 1, strlen(buf), i2c);

                                printf("Written: %d bytes\n", n);

                        } else {
                                // slave device read data from peripheral sent
                                // by master device.

                                size_t n = fread(buf, 1, sizeof(buf) - 1, i2c);

                                printf("Received: %d bytes\n", n);

                                // print received data
                                for (uint i = 0; i < n; i++) {
                                        printf("%02X ", buf[i]);
                                }
                                puts("");
                        }

                    } else {
                            perror(dev_path);
                    }
            }

            // close I2C device
            fclose(dev);

      } else {
            perror(dev_path);
      }

      return 0;
}
@endcode

@{
*/

#ifndef _I2C_IOCTL_H_
#define _I2C_IOCTL_H_

/*==============================================================================
  Include files
==============================================================================*/
#include "drivers/class/device/ioctl.h"

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Exported object types
==============================================================================*/
/**
 * Type defines possible modes of sub-addressing sequence (used e.g. in EEPROM).
 */
typedef enum {
        I2C_SUB_ADDR_MODE__DISABLED = 0,        //!< Sub-addressing disabled.
        I2C_SUB_ADDR_MODE__1_BYTE   = 1,        //!< Sub-address is 1 byte long.
        I2C_SUB_ADDR_MODE__2_BYTES  = 2,        //!< Sub-address is 2 byte long.
        I2C_SUB_ADDR_MODE__3_BYTES  = 3         //!< Sub-address is 3 byte long.
} I2C_sub_addr_mode_t;

/**
 * Type represents I2C peripheral configuration.
 */
typedef struct {
        u16_t               address;            /*!< Device address 8 or 10 bit.*/
        I2C_sub_addr_mode_t sub_addr_mode;      /*!< Number of bytes of sub-address (EEPROM, RTC).*/
        bool                addr_10bit;         /*!< @b true: 10 bit addressing enabled.*/
        bool                slave_mode;         /*!< @b true: slave moce enabled.*/
} I2C_config_t;

/**
 * Type used to check I2C slave mode event.
 */
typedef struct {
        u32_t               timeout_ms;         /*!< Timeout of waiting for event.*/
        bool                RD_addr;            /*!< ADDR+RD received.*/
} I2C_selection_t;

/**
 * Configuration of recovery
 */
typedef struct {
        bool enable;

        struct {
                u8_t port_idx;
                u8_t pin_idx;
        } SCL;

        struct {
                u8_t port_idx;
                u8_t pin_idx;
        } SDA;
} I2C_recovery_t;

/*==============================================================================
  Exported macros
==============================================================================*/
/**
 * @brief  Configure device
 * @param  [WR] @ref I2C_config_t*        device configuration
 * @return On success 0 is returned, otherwise -1 and @ref errno code is set.
 */
#define IOCTL_I2C__CONFIGURE                    _IOW(I2C, 0, const I2C_config_t*)

/**
 * @brief  Wait for I2C address event.
 * @param  [WR] @ref I2C_selection_t*     I2C address event (device selection)
 * @return On success 0 is returned, otherwise -1 and @ref errno code is set.
 */
#define IOCTL_I2C__SLAVE_WAIT_FOR_SELECTION     _IOWR(I2C, 1, I2C_selection_t*)

/**
 * @brief  Configure device by using string
 * @param  [WR] const char*             configuration string
 * @return On success 0 is returned, otherwise -1 and @ref errno code is set.
 */
#define IOCTL_I2C__CONFIGURE_STR                IOCTL_DEVICE__CONFIGURE_STR

/**
 * @brief  Configure recovery pin
 * @param  [WR] const I2C_recovery_t*   recovery configuration
 * @return On success 0 is returned, otherwise -1 and @ref errno code is set.
 */
#define IOCTL_I2C__CONFIGURE_RECOVERY           _IOW(I2C, 2, I2C_selection_t*)

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

#endif /* _I2C_IOCTL_H_ */
/**@}*/
/*==============================================================================
  End of file
==============================================================================*/
