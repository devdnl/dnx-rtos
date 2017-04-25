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

/** speed (do not edit) */
#define _GPIO_SPEED_LOW                         0x00
#define _GPIO_SPEED_MEDIUM                      0x01
#define _GPIO_SPEED_HIGH                        0x02
#define _GPIO_SPEED_VERY_HIGH                   0x03

/** [mmmm tttt pppp] (do not edit) */
#define _GPIO_MODE_IN                           0x000
#define _GPIO_MODE_IN_PU                        0x001
#define _GPIO_MODE_IN_PD                        0x002
#define _GPIO_MODE_PP                           0x100
#define _GPIO_MODE_OD                           0x110
#define _GPIO_MODE_OD_PU                        0x111
#define _GPIO_MODE_OD_PD                        0x112
#define _GPIO_MODE_AF_PP                        0x200
#define _GPIO_MODE_AF_PP_PU                     0x201
#define _GPIO_MODE_AF_PP_PD                     0x202
#define _GPIO_MODE_AF_OD                        0x210
#define _GPIO_MODE_AF_OD_PU                     0x211
#define _GPIO_MODE_AF_OD_PD                     0x212
#define _GPIO_MODE_ANALOG                       0x300

/** define pin state (do not edit) */
#define _FLOAT                                  0U
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
