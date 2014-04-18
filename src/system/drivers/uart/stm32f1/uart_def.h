/*=========================================================================*//**
@file    usart_def.h

@author  Daniel Zorychta

@brief   This file support statuses and request of USART peripherals

@note    Copyright (C) 2012 Daniel Zorychta <daniel.zorychta@gmail.com>

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

#ifndef _UART_DEF_H_
#define _UART_DEF_H_

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Include files
==============================================================================*/
#include "stm32f1/uart_ioctl.h"
#include "stm32f1/uart_cfg.h"

/*==============================================================================
  Exported symbolic constants/macros
==============================================================================*/
/* UART minor number */
#define _UART_MINOR_NUMBER      0

/* port names */
enum
{
        #if defined(RCC_APB2ENR_USART1EN) && (_UART1_ENABLE > 0)
        _UART1,
        #endif
        #if defined(RCC_APB1ENR_USART2EN) && (_UART2_ENABLE > 0)
        _UART2,
        #endif
        #if defined(RCC_APB1ENR_USART3EN) && (_UART3_ENABLE > 0)
        _UART3,
        #endif
        #if defined(RCC_APB1ENR_UART4EN) && (_UART4_ENABLE > 0)
        _UART4,
        #endif
        #if defined(RCC_APB1ENR_UART5EN) && (_UART5_ENABLE > 0)
        _UART5,
        #endif
        _UART_NUMBER
};

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

#endif /* _UART_DEF_H_ */
/*==============================================================================
  End of file
==============================================================================*/
