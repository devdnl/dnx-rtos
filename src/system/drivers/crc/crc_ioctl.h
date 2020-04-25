/*=========================================================================*//**
@file    crc_ioctl.h

@author  Daniel Zorychta

@brief   CRC driver ioctl request codes.

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
@defgroup drv-crc CRC Driver

\section drv-crc-desc Description
Driver handles CRC peripheral installed in the microcontroller.

\section drv-crc-sup-arch Supported architectures
\li STM32f1
\li STM32f4

\section drv-crc-ddesc Details
\subsection drv-crc-ddesc-num Meaning of major and minor numbers
The major number determines selection of CRC peripheral. Minor number
has no meaning and should be set to 0.

\subsection drv-crc-ddesc-init Driver initialization
To initialize driver the following code can be used:

@code
driver_init("CRC", 0, 0, "/dev/CRC");
@endcode

\subsection drv-crc-ddesc-release Driver release
To release driver the following code can be used:
@code
driver_release("CRC", 0, 0);
@endcode

\subsection drv-crc-ddesc-cfg Driver configuration
Configuration is limited to selection of CRC polynomial and initial value.
Those options can be not supported by all architectures.

\subsection drv-crc-ddesc-write Data write
Data to the CRC device can be write as regular file.

@code
#include <stdio.h>
#include <stdbool.h>
#include <dnx/misc.h>
#include <sys/ioctl.h>

static const char *dev_path = "/dev/CRC";
static const u8_t  buf[] = {0,1,2,3,4,5,6,7,8,9};

GLOBAL_VARIABLES_SECTION {
      u32_t CRC;
};

int_main(crc_ex, STACK_DEPTH_MEDIUM, int argc, char *argv[])
{
      FILE *dev = fopen(dev_path, "r+");

      if (dev) {
            // calculate CRC
            fseek(dev, 0x0, SEEK_SET);
            fwrite(buf, sizeof(u8_t), sizeof(buf), dev);

            // read CRC
            fseek(dev, 0x0, SEEK_SET);
            fread(global->CRC, sizeof(u32_t), 1, dev);

            // close device
            fclose(dev);

      } else {
            perror(dev_path);
      }

      return 0;
}
@endcode

\subsection drv-crc-ddesc-read Data read
Data to the CRC device can be read as regular file.

@code
#include <stdio.h>
#include <stdbool.h>
#include <dnx/misc.h>
#include <sys/ioctl.h>

static const char *dev_path = "/dev/CRC";
static const u8_t  buf[] = {0,1,2,3,4,5,6,7,8,9};

GLOBAL_VARIABLES_SECTION {
      u32_t CRC;
};

int_main(crc_ex, STACK_DEPTH_MEDIUM, int argc, char *argv[])
{
      FILE *dev = fopen(dev_path, "r+");

      if (dev) {
            // calculate CRC
            fseek(dev, 0x0, SEEK_SET);
            fwrite(buf, sizeof(u8_t), sizeof(buf), dev);

            // read CRC
            fseek(dev, 0x0, SEEK_SET);
            fread(global->CRC, sizeof(u32_t), 1, dev);

            // close device
            fclose(dev);

      } else {
            perror(dev_path);
      }

      return 0;
}
@endcode

@{
*/

#ifndef _CRC_IOCTL_H_
#define _CRC_IOCTL_H_

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
 * @brief  Set CRC polynomial (if supported by CRC peripheral).
 * @param  [WR] const u32_t * polynomial value
 * @return On success 0 is returned, otherwise -1 and @ref errno code is set.
 */
#define IOCTL_CRC__SET_POLYNOMIAL       _IOW(CRC, 0x00, const u32_t*)

 /**
  * @brief  Set initial CRC value (if supported by CRC peripheral).
  * @param  [WR] const u32_t * initial value
  * @return On success 0 is returned, otherwise -1 and @ref errno code is set.
  */
#define IOCTL_CRC__SET_INITIAL_VALUE    _IOW(CRC, 0x01, const u32_t*)

/*==============================================================================
  Exported object types
==============================================================================*/

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

#endif /* _CRC_IOCTL_H_ */
/**@}*/
/*==============================================================================
  End of file
==============================================================================*/
