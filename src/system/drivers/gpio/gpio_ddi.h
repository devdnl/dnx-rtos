/*=========================================================================*//**
@file    gpio_ddi.h

@author  Daniel Zorychta

@brief   GPIO Driver Direct Interface.

@note    Copyright (C) 2016 Daniel Zorychta <daniel.zorychta@gmail.com>

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
 * @defgroup drv-gpio-ddi GPIO Driver Direct Interface.
 *
 * \section drv-gpio-ddi-desc Description
 * GPIO Driver Direct Interface. This interface can be used only from driver
 * level. Interface is created to provide common low-level functions that
 * handles basic microcontroller functionality.
 *
 * \section drv-gpio-sup-arch Supported architectures
 * \li STM32F10x
 *
 * @todo Details
 *
 *
 * @{
 */

#ifndef _GPIO_DDI_H_
#define _GPIO_DDI_H_

/*==============================================================================
  Include files
==============================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Exported object types
==============================================================================*/

/*==============================================================================
  Exported macros
==============================================================================*/

/*==============================================================================
  Exported objects
==============================================================================*/

/*==============================================================================
  Exported functions
==============================================================================*/
//==============================================================================
/**
 * @brief Function set pin to 1.
 * @param port_idx      Port index
 * @param pin_idx       Pin index
 */
//==============================================================================
extern void _GPIO_DDI_set_pin(u8_t port_idx, u8_t pin_idx);

//==============================================================================
/**
 * @brief Function clear pin to 0.
 * @param port_idx      Port index.
 * @param pin_idx       Pin index.
 */
//==============================================================================
extern void _GPIO_DDI_clear_pin(u8_t port_idx, u8_t pin_idx);

//==============================================================================
/**
 * @brief  Function get pin state.
 * @param  port_idx      Port index.
 * @param  pin_idx       Pin index.
 * @return Pin value. On error -1.
 */
//==============================================================================
extern i8_t _GPIO_DDI_get_pin(u8_t port_idx, u8_t pin_idx);

/*==============================================================================
  Exported inline functions
==============================================================================*/

#ifdef __cplusplus
}
#endif

#endif /* _GPIO_DDI_H_ */
/**@}*/
/*==============================================================================
  End of file
==============================================================================*/
