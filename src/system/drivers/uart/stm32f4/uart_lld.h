/*=========================================================================*//**
@file    uart_lld.h

@author  Daniel Zorychta

@brief   This file support UART peripheral.

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

#if !defined(_UART_LLD_H_) && defined(ARCH_stm32f4)
#define _UART_LLD_H_

/*==============================================================================
  Include files
==============================================================================*/
#include "../uart.h"
#include "stm32f4/stm32f4xx.h"

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Exported macros
==============================================================================*/

/*==============================================================================
  Exported object types
==============================================================================*/
//* UARTs */
enum {
        #if defined(RCC_APB2ENR_USART1EN)
        _UART1,
        #endif
        #if defined(RCC_APB1ENR_USART2EN)
        _UART2,
        #endif
        #if defined(RCC_APB1ENR_USART3EN)
        _UART3,
        #endif
        #if defined(RCC_APB1ENR_UART4EN)
        _UART4,
        #endif
        #if defined(RCC_APB1ENR_UART5EN)
        _UART5,
        #endif
        #if defined(RCC_APB2ENR_USART6EN)
        _UART6,
        #endif
        #if defined(RCC_APB1ENR_UART7EN)
        _UART7,
        #endif
        #if defined(RCC_APB1ENR_UART8EN)
        _UART8,
        #endif
        #if defined(RCC_APB2ENR_UART9EN)
        _UART9,
        #endif
        #if defined(RCC_APB2ENR_UART10EN)
        _UART10,
        #endif
        _UART_COUNT
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

#endif /* _UART_LLD_H_ */
/*==============================================================================
  End of file
==============================================================================*/
