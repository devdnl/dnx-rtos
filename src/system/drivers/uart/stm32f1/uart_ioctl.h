/*=========================================================================*//**
@file    uart_ioctl.h

@author  Daniel Zorychta

@brief   This file support UART ioctl request codes.

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

#ifndef _UART_IOCTL_H_
#define _UART_IOCTL_H_

/*==============================================================================
  Include files
==============================================================================*/
#include "core/ioctl_macros.h"

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Exported macros
==============================================================================*/
/**
 *  @brief  Set UART configuration
 *  @param  struct UART_config *
 *  @return On success 0 is returned, otherwise -1
 */
#define IOCTL_UART__SET_CONFIGURATION           _IOW(UART, 0x00, struct UART_config*)

/**
 *  @brief  Gets UART configuration
 *  @param  struct UART_config *
 *  @return On success 0 is returned, otherwise -1
 */
#define IOCTL_UART__GET_CONFIGURATION           _IOR(UART, 0x01, struct UART_config*)

/**
 *  @brief  Gets character from UART in non-blocking mode
 *  @param  char *
 *  @return On success 0 is returned, otherwise -1
 */
#define IOCTL_UART__GET_CHAR_UNBLOCKING         _IOR(UART, 0x02, char*)

/*==============================================================================
  Exported object types
==============================================================================*/
enum UART_parity {
        UART_PARITY_OFF,
        UART_PARITY_ODD,
        UART_PARITY_EVEN
};

enum UART_stop_bits {
        UART_STOP_BIT_1,
        UART_STOP_BIT_2
};

enum UART_LIN_break_length {
        UART_LIN_BREAK_10_BITS,
        UART_LIN_BREAK_11_BITS
};

struct UART_config {
        enum UART_parity                parity                  : 2;
        enum UART_stop_bits             stop_bits               : 1;
        enum UART_LIN_break_length      LIN_break_length        : 1;
        bool                            tx_enable               : 1;
        bool                            rx_enable               : 1;
        bool                            LIN_mode_enable         : 1;
        bool                            hardware_flow_ctrl      : 1;
        bool                            single_wire_mode        : 1;
        u32_t                           baud                    : 23;
};

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

#endif /* _UART_IOCTL_H_ */
/*==============================================================================
  End of file
==============================================================================*/
