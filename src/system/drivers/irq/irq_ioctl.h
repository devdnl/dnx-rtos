/*=========================================================================*//**
@file    irq_ioctl.h

@author  Daniel Zorychta

@brief   This driver support external interrupts.

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

#ifndef _IRQ_IOCTL_H_
#define _IRQ_IOCTL_H_

/*==============================================================================
  Include files
==============================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Exported object types
==============================================================================*/
typedef enum {
        IRQ_CONFIG__IRQ_DISABLED,
        IRQ_CONFIG__TRIGGER_ON_FALLING_EDGE,
        IRQ_CONFIG__TRIGGER_ON_RISING_EDGE,
        IRQ_CONFIG__TRIGGER_ON_BOTH_EDGES,
        IRQ_CONFIG__TRIGGER_ON_LOW_LEVEL,
        IRQ_CONFIG__TRIGGER_ON_HIGH_LEVEL,
} IRQ_config_t;

/*==============================================================================
  Exported macros
==============================================================================*/
/**
 * @brief  Wait for selected interrupt number
 * @param  u32_t*       timeout in milliseconds
 * @return Returns  0 on success.
 *         Returns -1 on error and errno value is set.
 */
#define IOCTL_IRQ__CATCH                _IOW(IRQ, 0, const u32_t*)

/**
 * @brief  Software interrupt trigger
 * @param  None
 * @return Returns  0 on success.
 *         Returns -1 on error and errno value is set.
 */
#define IOCTL_IRQ__TRIGGER              _IO(IRQ, 1)

/**
 * @brief  Set IRQ configuration
 * @param  IRQ_config_t *
 * @return Returns  0 on success.
 *         Returns -1 on error and errno value is set.
 */
#define IOCTL_IRQ__CONFIGURE            _IOW(IRQ, 2, const IRQ_config_t*)

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

#endif /* _IRQ_IOCTL_H_ */
/*==============================================================================
  End of file
==============================================================================*/
