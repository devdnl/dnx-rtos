/*==============================================================================
File    stm32f7xx_hal_sdmmc.h

Author  Daniel Zorychta

Brief   ST HAL SDMMC definitions.

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
@defgroup SRC_SYSTEM_DRIVERS_SDMMC_STM32H7_STM32H7XX_HAL_SDMMC_H_ SRC_SYSTEM_DRIVERS_SDMMC_STM32H7_STM32H7XX_HAL_SDMMC_H_

Detailed Doxygen description.
*/
/**@{*/

#pragma once

/*==============================================================================
  Include files
==============================================================================*/
#include "drivers/driver.h"
#include "stm32h7/stm32h7xx.h"
#include "stm32h7/lib/stm32h7xx_ll_rcc.h"
#include "stm32h7/lib/misc.h"

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Exported macros
==============================================================================*/
#define RCC_PERIPHCLK_SDMMC             LL_RCC_SDMMC_CLKSOURCE

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

/**
  * @brief  Return the peripheral clock frequency for a given peripheral(SAI..)
  * @note   Return 0 if peripheral clock identifier not managed by this API
  * @param  PeriphClk: Peripheral clock identifier
  *         This parameter can be one of the following values:
  *            @arg RCC_PERIPHCLK_SAI1  : SAI1 peripheral clock
  *            @arg RCC_PERIPHCLK_SAI23 : SAI2/3  peripheral clock (*)
  *            @arg RCC_PERIPHCLK_SAI2A : SAI2A peripheral clock (*)
  *            @arg RCC_PERIPHCLK_SAI2B : SAI2B peripheral clock (*)
  *            @arg RCC_PERIPHCLK_SAI4A : SAI4A peripheral clock (*)
  *            @arg RCC_PERIPHCLK_SAI4B : SAI4B peripheral clock (*)
  *            @arg RCC_PERIPHCLK_SPI123: SPI1/2/3 peripheral clock
  *            @arg RCC_PERIPHCLK_ADC   : ADC peripheral clock
  *            @arg RCC_PERIPHCLK_SDMMC : SDMMC peripheral clock
  *            @arg RCC_PERIPHCLK_SPI6  : SPI6 peripheral clock
  * @retval Frequency in KHz
  *
  *  (*) : Available on some STM32H7 lines only.
  */
static inline uint32_t HAL_RCCEx_GetPeriphCLKFreq(uint32_t PeriphClk)
{
        return LL_RCC_GetSDMMCClockFreq(PeriphClk);
}

#ifdef __cplusplus
}
#endif

/**@}*/
/*==============================================================================
  End of file
==============================================================================*/
