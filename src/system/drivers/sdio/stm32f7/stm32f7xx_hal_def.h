/*==============================================================================
File    stm32f7xx_hal_def.h

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
@defgroup SRC_SYSTEM_DRIVERS_SDIO_STM32F7_STM32F7XX_HAL_DEF_H_ SRC_SYSTEM_DRIVERS_SDIO_STM32F7_STM32F7XX_HAL_DEF_H_

Detailed Doxygen description.
*/
/**@{*/

#pragma once

/*==============================================================================
  Include files
==============================================================================*/
#include "drivers/driver.h"
#include "stm32f7/stm32f7xx.h"

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Exported macros
==============================================================================*/
//#define SDMMC1
#define HAL_SD_MODULE_ENABLED

#define assert_param(...)
#define HAL_GetTick()                   sys_get_uptime_ms()
#define HAL_Delay(_ms)                  sys_sleep_ms(_ms)
#define __weak                          __attribute__ ((weak))
#define UNUSED(_x)                      UNUSED_ARG1(_x)

/*==============================================================================
  Exported object types
==============================================================================*/
/**
  * @brief  HAL Status structures definition
  */
typedef enum
{
        SD_HAL_OK = ESUCC,
        SD_HAL_TIMEOUT = ETIME,
        SD_HAL_BUSY = EBUSY,
        SD_HAL_ERROR = EIO,
} HAL_StatusTypeDef;

/**
  * @brief  HAL Lock structures definition
  */
typedef enum
{
        SD_HAL_UNLOCKED = 0x00U,
        SD_HAL_LOCKED   = 0x01U
} HAL_LockTypeDef;


/**
  * @brief  DMA handle Structure definition
  */
typedef struct __DMA_HandleTypeDef
{
  DMA_Stream_TypeDef         *Instance;                                                    /*!< Register base address                  */
// FIXME
//  DMA_InitTypeDef            Init;                                                         /*!< DMA communication parameters           */

  HAL_LockTypeDef            Lock;                                                         /*!< DMA locking object                     */
// FIXME
//  __IO HAL_DMA_StateTypeDef  State;                                                        /*!< DMA transfer state                     */

  void                       *Parent;                                                      /*!< Parent object state                    */

  void                       (* XferCpltCallback)( struct __DMA_HandleTypeDef * hdma);     /*!< DMA transfer complete callback         */

  void                       (* XferHalfCpltCallback)( struct __DMA_HandleTypeDef * hdma); /*!< DMA Half transfer complete callback    */

  void                       (* XferM1CpltCallback)( struct __DMA_HandleTypeDef * hdma);   /*!< DMA transfer complete Memory1 callback */

  void                       (* XferM1HalfCpltCallback)( struct __DMA_HandleTypeDef * hdma);   /*!< DMA transfer Half complete Memory1 callback */

  void                       (* XferErrorCallback)( struct __DMA_HandleTypeDef * hdma);    /*!< DMA transfer error callback            */

  void                       (* XferAbortCallback)( struct __DMA_HandleTypeDef * hdma);    /*!< DMA transfer Abort callback            */

 __IO uint32_t               ErrorCode;                                                    /*!< DMA Error code                          */

 uint32_t                    StreamBaseAddress;                                            /*!< DMA Stream Base Address                */

 uint32_t                    StreamIndex;                                                  /*!< DMA Stream Index                       */

}DMA_HandleTypeDef;

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

/**@}*/
/*==============================================================================
  End of file
==============================================================================*/
