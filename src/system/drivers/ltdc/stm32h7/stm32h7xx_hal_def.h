/*==============================================================================
File    stm32h7xx_hal_def.h

Author  Daniel Zorychta

Brief   LTDC HAL definitions.

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

#pragma once

/*==============================================================================
  Include files
==============================================================================*/
#include <stddef.h>
#include "drivers/driver.h"

#if defined(ARCH_stm32h7)
#include "stm32h7xx.h"
#include "stm32h7/lib/stm32h7xx_ll_rcc.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if defined(ARCH_stm32h7)

/*==============================================================================
  Exported macros
==============================================================================*/
#define assert_param(...)
#define HAL_GetTick()                   sys_get_uptime_ms()
#define HAL_Delay(_ms)                  sys_sleep_ms(_ms)
#define HAL_MAX_DELAY                   _MAX_DELAY_MS
#define __HAL_UNLOCK(heth)              (heth->Lock = HAL_UNLOCKED)
#define __HAL_LOCK(heth)                (heth->Lock = HAL_LOCKED)
#define __weak                          __attribute__ ((weak))

#ifndef UNUSED
#define UNUSED(_x)                      UNUSED_ARG1(_x)
#endif

#define CM7_CPUID                       ((uint32_t)0x00000003)
#define CM4_CPUID                       ((uint32_t)0x00000001)

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
static inline uint32_t HAL_RCC_GetHCLKFreq(void)
{
        LL_RCC_ClocksTypeDef freq;
        LL_RCC_GetSystemClocksFreq(&freq);
        return freq.HCLK_Frequency;
}

#endif

#ifdef __cplusplus
}
#endif

/*==============================================================================
  End of file
==============================================================================*/
