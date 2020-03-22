/*==============================================================================
File    irq_cfg.h

Author  Daniel Zorychta

Brief   This driver support external interrupts (EXTI).

        Copyright (C) 2018 Daniel Zorychta <daniel.zorychta@gmail.com>

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

#ifndef _IRQ_CFG_H_
#define _IRQ_CFG_H_

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Include files
==============================================================================*/
#include "stm32f4/stm32f4xx.h"

/*==============================================================================
  Exported symbolic constants/macros
==============================================================================*/
/*
 * Interrupts Default Configuration
 * 0: IRQ DISABLED
 * 1: TRIGGER ON FALLING EDGE
 * 2: TRIGGER ON RISING EDGE
 * 3: TRIGGER ON FALLING AND RISING EDGE
 */
enum _IRQ_MODE {
        _IRQ_MODE_DISABLED                = 0,
        _IRQ_MODE_FALLING_EDGE            = 1,
        _IRQ_MODE_RISING_EDGE             = 2,
        _IRQ_MODE_FALLING_AND_RISING_EDGE = 3
};

#define _IRQ_LINE_0_MODE                __IRQ_LINE_0_MODE__
#define _IRQ_LINE_1_MODE                __IRQ_LINE_1_MODE__
#define _IRQ_LINE_2_MODE                __IRQ_LINE_2_MODE__
#define _IRQ_LINE_3_MODE                __IRQ_LINE_3_MODE__
#define _IRQ_LINE_4_MODE                __IRQ_LINE_4_MODE__
#define _IRQ_LINE_5_MODE                __IRQ_LINE_5_MODE__
#define _IRQ_LINE_6_MODE                __IRQ_LINE_6_MODE__
#define _IRQ_LINE_7_MODE                __IRQ_LINE_7_MODE__
#define _IRQ_LINE_8_MODE                __IRQ_LINE_8_MODE__
#define _IRQ_LINE_9_MODE                __IRQ_LINE_9_MODE__
#define _IRQ_LINE_10_MODE               __IRQ_LINE_10_MODE__
#define _IRQ_LINE_11_MODE               __IRQ_LINE_11_MODE__
#define _IRQ_LINE_12_MODE               __IRQ_LINE_12_MODE__
#define _IRQ_LINE_13_MODE               __IRQ_LINE_13_MODE__
#define _IRQ_LINE_14_MODE               __IRQ_LINE_14_MODE__
#define _IRQ_LINE_15_MODE               __IRQ_LINE_15_MODE__

/*==============================================================================
  Exported types, enums definitions
==============================================================================*/

/*==============================================================================
  Exported object declarations
==============================================================================*/

/*==============================================================================
  Exported function prototypes
==============================================================================*/

#ifdef __cplusplus
}
#endif

#endif /* _IRQ_CFG_H_ */
/*==============================================================================
  End of file
==============================================================================*/
