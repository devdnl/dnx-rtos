/*==============================================================================
File    stm32f4xx_hal_def.h

Author  Daniel Zorychta

Brief   .

        Copyright (C) 2020 Daniel Zorychta <daniel.zorychta@gmail.com>

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
@defgroup ETH_STM32FX_STM32F4XX_HAL_DEF_H_ ETH_STM32FX_STM32F4XX_HAL_DEF_H_

Detailed Doxygen description.
*/
/**@{*/

#pragma once

/*==============================================================================
  Include files
==============================================================================*/
#include <stddef.h>
#include "drivers/driver.h"
#include "stm32f4/lib/stm32f4xx_rcc.h"

#if defined(ARCH_stm32f1)
#include "stm32f10x.h"
#elif defined(ARCH_stm32f4)
#include "stm32f4xx.h"
#elif defined(ARCH_stm32f7)
#include "stm32f7xx.h"
// TODO cache clear/invalidate
#endif

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Exported macros
==============================================================================*/
#define assert_param(...)
#define HAL_GetTick()                   sys_get_uptime_ms()
#define HAL_Delay(_ms)                  sys_sleep_ms(_ms)
#define __HAL_UNLOCK(heth)              (heth->Lock = HAL_UNLOCKED)
#define __HAL_LOCK(heth)                (heth->Lock = HAL_LOCKED)
#define __weak                          __attribute__ ((weak))
#define UNUSED(_x)                      UNUSED_ARG1(_x)

/* Definition of the Ethernet driver buffers size and count */
#define ETH_RX_BUF_SIZE                 ETH_MAX_PACKET_SIZE /* buffer size for receive               */
#define ETH_TX_BUF_SIZE                 ETH_MAX_PACKET_SIZE /* buffer size for transmit              */

/* Section 2: PHY configuration section */
/* LAN8742A PHY Address*/
#define PHY_RESET_DELAY                 __ETH_PHY_RESET_DELAY__
/* PHY Configuration delay */
#define PHY_CONFIG_DELAY                __ETH_PHY_CONFIG_DELAY__

#define PHY_READ_TO                     1000
#define PHY_WRITE_TO                    1000

/* Section 3: Common PHY Registers */
#define PHY_BCR                         ((uint16_t)0x00)    /*!< Transceiver Basic Control Register   */
#define PHY_BSR                         ((uint16_t)0x01)    /*!< Transceiver Basic Status Register    */

#define PHY_RESET                       ((uint16_t)0x8000)  /*!< PHY Reset */
#define PHY_LOOPBACK                    ((uint16_t)0x4000)  /*!< Select loop-back mode */
#define PHY_FULLDUPLEX_100M             ((uint16_t)0x2100)  /*!< Set the full-duplex mode at 100 Mb/s */
#define PHY_HALFDUPLEX_100M             ((uint16_t)0x2000)  /*!< Set the half-duplex mode at 100 Mb/s */
#define PHY_FULLDUPLEX_10M              ((uint16_t)0x0100)  /*!< Set the full-duplex mode at 10 Mb/s  */
#define PHY_HALFDUPLEX_10M              ((uint16_t)0x0000)  /*!< Set the half-duplex mode at 10 Mb/s  */
#define PHY_AUTONEGOTIATION             ((uint16_t)0x1000)  /*!< Enable auto-negotiation function     */
#define PHY_RESTART_AUTONEGOTIATION     ((uint16_t)0x0200)  /*!< Restart auto-negotiation function    */
#define PHY_POWERDOWN                   ((uint16_t)0x0800)  /*!< Select the power down mode           */
#define PHY_ISOLATE                     ((uint16_t)0x0400)  /*!< Isolate PHY from MII                 */

#define PHY_AUTONEGO_COMPLETE           ((uint16_t)0x0020)  /*!< Auto-Negotiation process completed   */
#define PHY_LINKED_STATUS               ((uint16_t)0x0004)  /*!< Valid link established               */
#define PHY_JABBER_DETECTION            ((uint16_t)0x0002)  /*!< Jabber condition detected            */

/* Section 4: Extended PHY Registers */
#define PHY_SR                          ((uint16_t)__ETH_PHY_SR__)    /*!< PHY special control/ status register Offset     */
#define PHY_SPEED_STATUS                ((uint16_t)__ETH_PHY_SPEED_STATUS_BM__)  /*!< PHY Speed mask                                  */
#define PHY_DUPLEX_STATUS               ((uint16_t)__ETH_PHY_DUPLEX_STATUS_BM__)  /*!< PHY Duplex mask                                 */

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

#ifdef __cplusplus
}
#endif

/**@}*/
/*==============================================================================
  End of file
==============================================================================*/
