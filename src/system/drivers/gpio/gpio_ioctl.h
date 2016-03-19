/*=========================================================================*//**
@file    gpio_ioctl.h

@author  Daniel Zorychta

@brief   This driver support GPIO ioctl request codes.

@note    Copyright (C) 2014 Daniel Zorychta <daniel.zorychta@gmail.com>

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

/**
 * @defgroup drv-gpio GPIO Driver
 *
 * \section drv-gpio-desc Description
 * Driver handles GPIO peripheral.
 *
 * \section drv-gpio-sup-arch Supported architectures
 * \li STM32F10x
 *
 * @todo Details
 *
 *
 * @{
 */

#ifndef _GPIO_IOCTL_H_
#define _GPIO_IOCTL_H_

/*==============================================================================
  Include files
==============================================================================*/
#include "drivers/ioctl_macros.h"

// PIN names definitions (IOCTL_GPIO_PIN__name and IOCTL_GPIO_PIN_MASK__name)
#ifdef ARCH_stm32f1
        #include "stm32f1/gpio_cfg.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Exported object types
==============================================================================*/
/**
 * Type represent pin number (natural number).
 */
typedef uint GPIO_pin_t;

/**
 * Type used to read pin state
 */
typedef struct {
        GPIO_pin_t pin;         //!< Pin number.
        int        state;       //!< Pin state.
} GPIO_pin_state_t;

/*==============================================================================
  Exported macros
==============================================================================*/
/**
 *  @brief  Set selected pin (set pin to Hi state).
 *  @param  [WR] @ref GPIO_pin_t*                 pin number
 *  @return On success 0 is returned, otherwise -1 and @ref errno code is set
 */
#define IOCTL_GPIO__SET_PIN             _IOW(GPIO, 0, const GPIO_pin_t*)

/**
 *  @brief  Clear selected pin (set pin to Low state).
 *  @param  [WR] @ref GPIO_pin_t*                 pin number
 *  @return On success 0 is returned, otherwise -1 and @ref errno code is set
 */
#define IOCTL_GPIO__CLEAR_PIN           _IOW(GPIO, 1, const GPIO_pin_t*)

/**
 *  @brief  Toggle pin state.
 *  @param  [WR] @ref GPIO_pin_t*                 pin number
 *  @return On success 0 is returned, otherwise -1 and @ref errno code is set
 */
#define IOCTL_GPIO__TOGGLE_PIN          _IOW(GPIO, 2, const GPIO_pin_t*)

/**
 *  @brief  Set pin state.
 *  @param  [WR] @ref GPIO_pin_state_t*              pin number and state to set
 *  @return On success 0 is returned, otherwise -1 and @ref errno code is set
 */
#define IOCTL_GPIO__SET_PIN_STATE       _IOW(GPIO, 3, const GPIO_pin_state_t*)

/**
 *  @brief  Gets pin state.
 *  @param  [RD] @ref GPIO_pin_state_t*           pin number and state
 *  @return On success 0 is returned, otherwise -1 and @ref errno code is set
 */
#define IOCTL_GPIO__GET_PIN_STATE       _IOR(GPIO, 4, GPIO_pin_state_t*)

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

#endif /* _GPIO_IOCTL_H_ */
/**@}*/
/*==============================================================================
  End of file
==============================================================================*/
