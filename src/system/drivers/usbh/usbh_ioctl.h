/*==============================================================================
File    usbh_ioctl.h

Author

Brief   USB Host driver

        Copyright (C) 2019  <>

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

==============================================================================*/

/**
@defgroup drv-usbh USBH Driver

\section drv-usbh-desc Description
Driver handles ...

\section drv-usbh-sup-arch Supported architectures
\li stm32f4

\section drv-usbh-ddesc Details
\subsection drv-usbh-ddesc-num Meaning of major and minor numbers
\todo Meaning of major and minor numbers

\subsubsection drv-usbh-ddesc-numres Numeration restrictions
\todo Numeration restrictions

\subsection drv-usbh-ddesc-init Driver initialization
To initialize driver the following code can be used:

@code
driver_init("USBH", 0, 0, "/dev/USBH0-0");
@endcode
@code
driver_init("USBH", 0, 1, "/dev/USBH0-1");
@endcode

\subsection drv-usbh-ddesc-release Driver release
To release driver the following code can be used:
@code
driver_release("USBH", 0, 0);
@endcode
@code
driver_release("USBH", 0, 1);
@endcode

\subsection drv-usbh-ddesc-cfg Driver configuration
\todo Driver configuration

\subsection drv-usbh-ddesc-write Data write
\todo Data write

\subsection drv-usbh-ddesc-read Data read
\todo Data read

@{
*/

#ifndef _USBH_IOCTL_H_
#define _USBH_IOCTL_H_

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
 *  @brief  Return manufacturer string.
 *  @param  [RD] usbh_buf_t*
 *  @return On success 0 is returned, otherwise -1.
 */
#define IOCTL_USBH__GET_MANUFACTURER_STRING     _IOR(USBH, 0x00, usbh_buf_t*)

/**
 *  @brief  Return product string.
 *  @param  [RD] usbh_buf_t*
 *  @return On success 0 is returned, otherwise -1.
 */
#define IOCTL_USBH__GET_PRODUCT_STRING          _IOR(USBH, 0x01, usbh_buf_t*)

/**
 *  @brief  Return serial string.
 *  @param  [RD] usbh_buf_t*
 *  @return On success 0 is returned, otherwise -1.
 */
#define IOCTL_USBH__GET_SERIAL_STRING           _IOR(USBH, 0x02, usbh_buf_t*)

/*==============================================================================
  Exported object types
==============================================================================*/
typedef struct {
        char  *ptr;
        size_t len;
} usbh_buf_t;

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

#endif /* _USBH_IOCTL_H_ */
/**@}*/
/*==============================================================================
  End of file
==============================================================================*/
