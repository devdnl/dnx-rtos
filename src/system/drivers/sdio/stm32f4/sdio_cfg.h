/*==============================================================================
File    sdio_cfg.h

Author  Daniel Zorychta

Brief   SD Card Interface Driver

        Copyright (C) 2017 Daniel Zorychta <daniel.zorychta@gmail.com>

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

#ifndef _SDIO_CFG_H_
#define _SDIO_CFG_H_

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Include files
==============================================================================*/
#include "config.h"

/*==============================================================================
  Exported macros
==============================================================================*/
#define _SDIO_USE_DMA_NEVER             0
#define _SDIO_USE_DMA_IFAVAILABLE       1
#define _SDIO_USE_DMA_ALWAYS            2

#define _SDIO_CFG_NEGEDGE               __SDIO_CFG_NEGEDGE__
#define _SDIO_CFG_BUS_WIDE              __SDIO_CFG_BUS_WIDE__
#define _SDIO_CFG_PWRSAVE               __SDIO_CFG_PWRSAVE__
#define _SDIO_CFG_USEDMA                __SDIO_CFG_USEDMA__
#define _SDIO_CFG_CARD_TIMEOUT          __SDIO_CFG_CARD_TIMEOUT__
#define _SDIO_CFG_CLKDIV                (((__SDIO_CFG_CKDIV__) < 2) ? 0 : ((__SDIO_CFG_CKDIV__) - 2))
#define _SDIO_CFG_BYPASS                (((__SDIO_CFG_CKDIV__) < 2) ? SDIO_CLKCR_BYPASS : 0)
#define _SDIO_CFG_ACMD6_BUS_WIDE        ((2 * ((_SDIO_CFG_BUS_WIDE) >> SDIO_CLKCR_WIDBUS_Pos)) & 0x3)
#define _SDIO_CFG_INIT_CLKDIV           158
#define _SDIO_CFG_IRQ_PRIORITY          __SDIO_IRQ_PRIORITY__

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

#endif /* _SDIO_CFG_H_ */
/*==============================================================================
  End of file
==============================================================================*/
