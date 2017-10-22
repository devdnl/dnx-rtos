/*==============================================================================
File    ioctl.h

Author  Daniel Zorychta

Brief   IOCTL requests for device class.

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
@defgroup DRIVER_CLASS_DEVICE_IOCTL_H_ DRIVER_CLASS_DEVICE_IOCTL_H_

Detailed Doxygen description.
*/
/**@{*/

#ifndef _DRIVER_CLASS_DEVICE_IOCTL_H_
#define _DRIVER_CLASS_DEVICE_IOCTL_H_

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
/* predefined requests*/
/**
 *  @brief  Configure device by using string options.
 *  @param  [WR] const char*            driver configuration string.
 *  @return On success (device initialized) 0 is returned.
 *          On error (device not initialized) -1 is returned and errno is set.
 */
#define IOCTL_DEVICE__CONFIGURE_STR     _IOW(DEVICE, 0x00, const char*)

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

#endif /* _DRIVER_CLASS_DEVICE_IOCTL_H_ */

/**@}*/
/*==============================================================================
  End of file
==============================================================================*/
