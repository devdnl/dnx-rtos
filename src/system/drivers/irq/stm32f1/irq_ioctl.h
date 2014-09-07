/*=========================================================================*//**
@file    irq_ioctl.h

@author  Daniel Zorychta

@brief   This driver support external interrupts (EXTI).

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
typedef struct {
        uint timeout;           // total wait time for interrupt trigger
        uint irq_number;        // EXTI line number (0-15)
} IRQ_catch_t;

typedef struct {
        uint irq_number:8;      // EXTI line number (0-15)
        enum IRQ_config_mode {  // EXTI line mode
                IRQ_CONFIG_MODE__IRQ_DISABLED,
                IRQ_CONFIG_MODE__TRIGGER_ON_FALLING_EDGE,
                IRQ_CONFIG_MODE__TRIGGER_ON_RISING_EDGE,
                IRQ_CONFIG_MODE__TRIGGER_ON_FALLING_AND_RISING_EDGE
        } mode:8;
} IRQ_config_t;

/*==============================================================================
  Exported macros
==============================================================================*/
/**
 * @brief  Macro can be used to initialize IRQ_catch_t type
 * @param  irq_number          interrupt number (0-15)
 * @param  timeout             wait timeout
 * @return Initialized catch structure
 */
#define IRQ_CATCH_SETUP(uint__irq_number, uint__timeout) {.irq_number = uint__irq_number, .timeout = uint__timeout}

/**
 * @brief  Macro can be used to initialize IRQ_config_t type
 * @param  irq_number          number of interrupt to configure
 * @param  mode                interrupt mode
 * @return Initialized configuration structure
 */
#define IRQ_CONFIG_SETUP(uint__irq_number, enum_IRQ_config_mode__mode) {.irq_number = uint__irq_number, .mode = enum_IRQ_config_mode__mode}


/**
 * @brief  Wait for slected interrupt number
 * @param  IRQ_catch_t *      pointer to interrupt number and timeout value
 * @return Returns  0 if timeout occurred.
 *         Returns  1 if interrupt occurred.
 *         Returns -1 if illegal interrupt number or request or configuration.
 */
#define IOCTL_IRQ__CATCH                _IOW(_IO_GROUP_IRQ, 0, const IRQ_catch_t*)

/**
 * @brief  Software interrupt trigger
 * @param  int                 interrupt number to trigger (0-15)
 * @return On success 0, on error -1
 */
#define IOCTL_IRQ__TRIGGER              _IOW(_IO_GROUP_IRQ, 1, const int)

/**
 * @brief  Set IRQ configuration
 * @param  IRQ_config_t *
 * @return On success 0, on error -1
 */
#define IOCTL_IRQ__CONFIGURE            _IOW(_IO_GROUP_IRQ, 2, const IRQ_config_t*)

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
