/*==============================================================================
File    stm32h7xx_hal_def.h

Author  Daniel Zorychta

Brief   FDCAN HAL definitions.

        Copyright (C) 2022 Daniel Zorychta <daniel.zorychta@gmail.com>

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
@defgroup FDCAN_STM32FX_STM32H7XX_HAL_DEF_H_ FDCAN_STM32FX_STM32H7XX_HAL_DEF_H_

Detailed Doxygen description.
*/
/**@{*/

#pragma once

/*==============================================================================
  Include files
==============================================================================*/
#include <stddef.h>
#include "drivers/driver.h"

#if defined(ARCH_stm32h7)
#include "stm32h7xx.h"
#include "stm32h7/lib/stm32h7xx_ll_rcc.h"
#include "stm32h7/lib/misc.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if defined(ARCH_stm32h7)

/*==============================================================================
  Exported macros
==============================================================================*/
#define HAL_MAX_DELAY                   _MAX_DELAY_MS
#define __HAL_UNLOCK(heth)              (heth->Lock = HAL_UNLOCKED)
#define __HAL_LOCK(heth)                (heth->Lock = HAL_LOCKED)

#ifndef UNUSED
#define UNUSED(_x)                      UNUSED_ARG1(_x)
#endif

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

#endif

#ifdef __cplusplus
}
#endif

/**@}*/
/*==============================================================================
  End of file
==============================================================================*/
