#ifndef _GPIO_MACROS_H_
#define _GPIO_MACROS_H_
/*=========================================================================*//**
@file    gpio_macros.h

@author  Daniel Zorychta

@brief   This driver support GPIO. Configuration and pin control macros.

@note    Copyright (C) 2013 Daniel Zorychta <daniel.zorychta@gmail.com>

         This program is free software; you can redistribute it and/or modify
         it under the terms of the GNU General Public License as published by
         the  Free Software  Foundation;  either version 2 of the License, or
         any later version.

         This  program  is  distributed  in the hope that  it will be useful,
         but  WITHOUT  ANY  WARRANTY;  without  even  the implied warranty of
         MERCHANTABILITY  or  FITNESS  FOR  A  PARTICULAR  PURPOSE.  See  the
         GNU General Public License for more details.

         You  should  have received a copy  of the GNU General Public License
         along  with  this  program;  if not,  write  to  the  Free  Software
         Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.


*//*==========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Include files
==============================================================================*/


/*==============================================================================
  Exported macros
==============================================================================*/
/** define GPIO pin outputs types and speeds (do not edit) */
#define _GPIO_OUT_PUSH_PULL_10MHZ               0x01
#define _GPIO_OUT_PUSH_PULL_2MHZ                0x02
#define _GPIO_OUT_PUSH_PULL_50MHZ               0x03
#define _GPIO_OUT_OPEN_DRAIN_10MHZ              0x05
#define _GPIO_OUT_OPEN_DRAIN_2MHZ               0x06
#define _GPIO_OUT_OPEN_DRAIN_50MHZ              0x07
#define _GPIO_ALT_OUT_PUSH_PULL_10MHZ           0x09
#define _GPIO_ALT_OUT_PUSH_PULL_2MHZ            0x0A
#define _GPIO_ALT_OUT_PUSH_PULL_50MHZ           0x0B
#define _GPIO_ALT_OUT_OPEN_DRAIN_10MHZ          0x0D
#define _GPIO_ALT_OUT_OPEN_DRAIN_2MHZ           0x0E
#define _GPIO_ALT_OUT_OPEN_DRAIN_50MHZ          0x0F

/** define GPIO pin inputs (do not edit) */
#define _GPIO_IN_ANALOG                         0x00
#define _GPIO_IN_FLOAT                          0x04
#define _GPIO_IN_PULLED                         0x08

/** define pin state (do not edit) */
#define _FLOAT                                  0U
#define _DOWN                                   0U
#define _LOW                                    0U
#define _UP                                     1U
#define _HIGH                                   1U

/** create enumerator with pin configuration */
#define _PIN_CONFIGURATION(port, number, pin_name, mode, state)      enum port##_##pin_name##_CFG {pin_name##_PORT = port##_BASE, pin_name##_BP = (number), pin_name##_BM = (1 << (number)), port##_PIN_##number##_MODE = (mode), port##_PIN_##number##_STATE = (state)}

/** PIN control macros */
#define GPIO_SET_PIN(name)                      ((GPIO_t*)name##_PORT)->BSRR |= (1 << name##_BP)
#define GPIO_CLEAR_PIN(name)                    ((GPIO_t*)name##_PORT)->BRR  |= (1 << name##_BP)
#define GPIO_TEST_PIN(name)                     (((GPIO_t*)name##_PORT)->IDR  & (1 << name##_BP))

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
