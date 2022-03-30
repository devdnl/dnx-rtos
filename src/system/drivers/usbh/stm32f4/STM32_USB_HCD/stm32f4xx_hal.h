/*==============================================================================
File    stm32f4xx_hal.h

Author  Daniel Zorychta

Brief   .

        Copyright (C) 2021 Daniel Zorychta <daniel.zorychta@gmail.com>

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
@defgroup _USB_HCD_STM32F4XX_HAL_H_ _USB_HCD_STM32F4XX_HAL_H_

Detailed Doxygen description.
*/
/**@{*/

#pragma once

/*==============================================================================
  Include files
==============================================================================*/
#include "stm32f4xx.h"
#include "drivers/driver.h"

#ifdef __cplusplus
extern "C" {
#endif

#if defined(ARCH_stm32f4)

/*==============================================================================
  Exported macros
==============================================================================*/
#if  defined ( __GNUC__ )
#ifndef __weak
#define __weak   __attribute__((weak))
#endif /* __weak */
#ifndef __packed
#define __packed __attribute__((__packed__))
#endif /* __packed */
#endif /* __GNUC__ */

#define assert_param(...)
#define __HAL_LOCK(_hhcd)       _hhcd->Lock = HAL_LOCKED
#define __HAL_UNLOCK(_hhcd)     _hhcd->Lock = HAL_UNLOCKED
#define UNUSED(_u)              UNUSED_ARG1(_u)
#define HAL_Delay(_ms)          sys_sleep_ms(_ms)

/*==============================================================================
  Exported object types
==============================================================================*/
typedef enum {
        HAL_OK = ESUCC,
        HAL_TIMEOUT = ETIME,
        HAL_BUSY = EBUSY,
        HAL_ERROR = EIO,
} HAL_StatusTypeDef;

typedef enum {
        HAL_UNLOCKED,
        HAL_LOCKED
} HAL_LockTypeDef;

/*==============================================================================
  Exported objects
==============================================================================*/

/*==============================================================================
  Exported functions
==============================================================================*/

/*==============================================================================
  Exported inline functions
==============================================================================*/

#endif

#ifdef __cplusplus
}
#endif

/**@}*/
/*==============================================================================
  End of file
==============================================================================*/
