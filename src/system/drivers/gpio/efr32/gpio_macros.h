/*=========================================================================*//**
@file    gpio_macros.h

@author  Daniel Zorychta

@brief   This driver support GPIO. Configuration and pin control macros.

@note    Copyright (C) 2017 Daniel Zorychta <daniel.zorychta@gmail.com>

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


*//*==========================================================================*/

#ifndef _GPIO_MACROS_H_
#define _GPIO_MACROS_H_

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Include files
==============================================================================*/

/*==============================================================================
  Exported macros
==============================================================================*/
/** concat macro */
#define _CONCAT(x, y) x##y

/** define GPIO pin outputs types and speeds (do not edit) */
#define _GPIO_MODE_DISABLED                     0x00
#define _GPIO_MODE_INPUT                        0x01
#define _GPIO_MODE_INPUTPULL                    0x02
#define _GPIO_MODE_INPUTPULLFILTER              0x03
#define _GPIO_MODE_PUSHPULL                     0x04
#define _GPIO_MODE_PUSHPULLALT                  0x05
#define _GPIO_MODE_WIREDOR                      0x06
#define _GPIO_MODE_WIREDORPULLDOWN              0x07
#define _GPIO_MODE_WIREDAND                     0x08
#define _GPIO_MODE_WIREDANDFILTER               0x09
#define _GPIO_MODE_WIREDANDPULLUP               0x0A
#define _GPIO_MODE_WIREDANDPULLUPFILTER         0x0B
#define _GPIO_MODE_WIREDANDALT                  0x0C
#define _GPIO_MODE_WIREDANDALTFILTER            0x0D
#define _GPIO_MODE_WIREDANDALTPULLUP            0x0E
#define _GPIO_MODE_WIREDANDALTPULLUPFILTER      0x0F

/** define GPIO pin drive strength */
#define _GPIO_DRIVE_STRONG                      0U
#define _GPIO_DRIVE_WEAK                        1U

/** define pin state (do not edit) */
#define _LOW                                    0U
#define _HIGH                                   1U

/*==============================================================================
  Exported object types
==============================================================================*/

/*==============================================================================
  Exported objects
==============================================================================*/

/*==============================================================================
  Exported functions
==============================================================================*/

#ifdef __cplusplus
}
#endif

#endif /* _GPIO_MACROS_H_ */
/*==============================================================================
  End of file
==============================================================================*/
