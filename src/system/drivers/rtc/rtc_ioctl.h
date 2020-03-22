/*=========================================================================*//**
@file    rtc_ioctl.h

@author  Daniel Zorychta

@brief   Real-Time Clock Module

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

/**
@defgroup drv-rtc RTC Driver

\section drv-rtc-desc Description
Driver handles real time clock peripheral.

\section drv-rtc-sup-arch Supported architectures
\li stm32f1
\li stm32f4

\section drv-rtc-ddesc Details
\subsection drv-rtc-ddesc-num Meaning of major and minor numbers
The major number value depends on number RTC peripherals in microcontroller. The
minor number should be set to 0.

\subsubsection drv-rtc-ddesc-numres Numeration restrictions
Major number architecture limitations:
\li stm32f1: 0

\subsection drv-rtc-ddesc-init Driver initialization
To initialize driver the following code can be used:

@code
driver_init("RTC", 0, 0, "/dev/rtc");
@endcode

Driver path is used by system to read time. The path of RTC device can be
configured by Configtool.

\subsection drv-rtc-ddesc-release Driver release
To release driver the following code can be used:
@code
driver_release("RTC", 0, 0);
@endcode

\subsection drv-rtc-ddesc-cfg Driver configuration
Driver can be configured by using ioctl() function. The RTC alarm only can be
configured. Example:
\code
#include <stdio.h>
#include <sys/ioctl.h>
#include <time.h>
#include <unistd.h>
#include <dnx/os.h>

// ...

FILE *rtc = fopen("/dev/rtc", "r+");
if (rtc) {
        // wake up in 5 seconds
        time_t t = time(NULL) + 5000;

        if (ioctl(fileno(rtc), IOCTL_RTC__SET_ALARM, &t) == 0) {
                sync();
                system_shutdown();
        } else {
                perror("ioctl()");
        }

        fclose(rtc);
} else {
        perror("/dev/rtc");
}

// ...

\endcode

\subsection drv-rtc-ddesc-write Data write
Only sizeof(time_t) bytes can be written to the device at position 0. More bytes
is trimmed. When write position is not 0 then error is returned.

\note To setup time please use stime() function instead of direct device write.

\subsection drv-rtc-ddesc-read Data read
Only sizeof(time_t) bytes can be read from device at position 0. Reading other
file positions is not allowed and error is generated.

\note To read time please use time() function instead of direct device read.

@{
*/

#ifndef _RTC_IOCTL_H_
#define _RTC_IOCTL_H_

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
 *  @brief  Set RTC alarm. This function can be used to wake up CPU from sleep.
 *  @param  [WR] @ref time_t *  UNIX timestamp.
 *  @return On success 0 is returned, otherwise -1.
 */
#define IOCTL_RTC__SET_ALARM    _IOW(RTC, 0x00, const time_t*)

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

#endif /* _RTC_IOCTL_H_ */
/**@}*/
/*==============================================================================
  End of file
==============================================================================*/
