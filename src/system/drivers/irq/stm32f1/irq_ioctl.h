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
        int  timeout;
        u8_t IRQ_number;
} IRQ_number_t;

typedef struct {
        u8_t IRQ_number;
        bool enabled:1;
        bool falling_edge:1;
        bool rising_edge:1;
        bool event:1;
} IRQ_config_t;

/*==============================================================================
  Exported macros
==============================================================================*/
/** @brief  Wait for slected interrupt number
 *  @param  IRQ_number_t *      pointer to interrupt number and timeout value
 *  @return Returns  0 if timeout occurred.
 *          Returns  1 if interrupt occurred.
 *          Returns -1 if illegal interrupt number or request.
 */
#define IOCTL_IRQ__CATCH                _IOW(_IO_GROUP_IRQ, 0, const IRQ_number_t*)

/** @brief  Software interrupt trigger
 *  @param  IRQ_number_t *      pointer to interrupt number and timeout value
 *  @return On success 0, on error -1
 */
#define IOCTL_GPIO__TRIGGER             _IOW(_IO_GROUP_IRQ, 1, const IRQ_number_t*)

/** @brief  Set IRQ configuration
 *  @param  IRQ_config_t *
 *  @return On success 0, on error -1
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
