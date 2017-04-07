/*=========================================================================*//**
@file    dht11_ioctl.h

@author  Daniel Zorychta

@brief   DHT11 sensor driver.

@note    Copyright (C) 2016 Daniel Zorychta <daniel.zorychta@gmail.com>

         This program is free software; you can redistribute it and/or modify
         it under the terms of the GNU General Public License as published by
         the Free Software Foundation and modified by the dnx RTOS exception.

         NOTE: The modification  to the GPL is  included to allow you to
               distribute a combined work that includes FreeRTOS without
               being obliged to provide the source  code for proprietary
               components outside of the dnx RTOS.

         The dnx RTOS  is  distributed  in the hope  that  it will be useful,
         but WITHOUT  ANY  WARRANTY;  without  even  the implied  warranty of
         MERCHANTABILITY  or  FITNESS  FOR  A  PARTICULAR  PURPOSE.  See  the
         GNU General Public License for more details.

         Full license text is available on the following file: doc/license.txt.


*//*==========================================================================*/

/**
@defgroup drv-dht11 DHT11 Driver

\section drv-dht11-desc Description
Driver handles DHT11 sensor.

\section drv-dht11-sup-arch Supported architectures
\li Any

\subsection drv-dht11-ddesc-num Meaning of major and minor numbers
Module handles only major device number. The minor number is not used. A new
instance of driver is next major number.

\subsubsection drv-dht11-ddesc-numres Numeration restrictions
Number of devices (major number) can be theoretically up to 256 or to limits
of memory capacity. Minor number is ignored (should be set to 0).

\subsection drv-dht11-ddesc-init Driver initialization
To initialize driver the following code can be used:

@code
driver_init("DHT11", 0, 0, "/dev/DHT11-0");  // device path can be more descriptive
@endcode
@code
driver_init("DHT11", 1, 0, "/dev/DHT11-1");
@endcode

\subsection drv-dht11-ddesc-release Driver release
To release driver the following code can be used:
@code
driver_release("DHT11", 0, 0);
@endcode
@code
driver_release("DHT11", 1, 0);
@endcode

\subsection drv-dht11-ddesc-cfg Driver configuration
Driver configuration should be done before usage and after initialization.
The best place to do this is user application. To configure particular DHT11
sensor the ioctl() function shall be used as follow:
@code
#include <stdio.h>
#include <stdbool.h>
#include <sys/ioctl.h>

static const DHT11_config_t cfg = {
      .port_idx = IOCTL_GPIO_PORT_IDX__DHT11,
      .pin_idx  = IOCTL_GPIO_PIN_IDX__DHT11
};

FILE *dev = fopen("/dev/DHT11-0", "r+");
if (dev) {
      if (ioctl(dev, IOCTL_DHT11__CONFIGURE, &cfg) != 0) {
            perror(NULL);
      }

      fclose(dev);
} else {
      perror(NULL);
}

...
@endcode

\subsection drv-dht11-ddesc-write Data write
There is no possibility to write any data to the device.

\subsection drv-dht11-ddesc-read Data read
Data from the DHT11 sensor can be read as regular file.

@code
#include <stdio.h>
#include <stdbool.h>
#include <dnx/misc.h>
#include <sys/ioctl.h>

GLOBAL_VARIABLES_SECTION {
};

static const DHT11_config_t cfg = {
      .port_idx = IOCTL_GPIO_PORT_IDX__DHT11,
      .pin_idx  = IOCTL_GPIO_PIN_IDX__DHT11
};

int_main(dht11, STACK_DEPTH_LOW, int argc, char *argv[])
{
      FILE *dev = fopen("/dev/DHT11-0", "r+");
      if (dev) {
            ioctl(dev, IOCTL_I2C__CONFIGURE, &cfg);

            uint8_t buf[5];
            rewind(dev);
            fread(buf, 1, ARRAY_SIZE(buf), dev);

            fclose(dev);

      } else {
            perror(NULL);
      }

      return 0;
}
@endcode

@{
*/

#ifndef _DHT11_IOCTL_H_
#define _DHT11_IOCTL_H_

/*==============================================================================
  Include files
==============================================================================*/
#include "drivers/ioctl_macros.h"

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Exported macros
==============================================================================*/
/**
 *  @brief  Join driver with pin where DHT11 sensor is connected to.
 *  @param  [WR] @ref DHT11_config_t*           Driver configuration structure.
 *  @return On success 0 is returned, otherwise -1 and @ref errno code is set.
 */
#define IOCTL_DHT11__CONFIGURE          _IOW(DHT11, 0, const DHT11_config_t*)

/*==============================================================================
  Exported object types
==============================================================================*/
/**
 * Type represent DHT11 configuration.
 */
typedef struct {
        u8_t port_idx;      /*!< Port index (use IOCTL_GPIO_PORT_IDX__xxx macro).*/
        u8_t pin_idx;       /*!< Pin index (use IOCTL_GPIO_PIN_IDX__xxx macro).*/
} DHT11_config_t;

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

#endif /* _DHT11_IOCTL_H_ */
/**@}*/
/*==============================================================================
  End of file
==============================================================================*/
