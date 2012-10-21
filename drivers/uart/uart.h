#ifndef UART_H_
#define UART_H_
/*=============================================================================================*//**
@file    usart.h

@author  Daniel Zorychta

@brief   This file support USART peripherals

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


*//*==============================================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*==================================================================================================
                                            Include files
==================================================================================================*/
#include "uart_cfg.h"
#include "uart_def.h"
#include "stm32f10x.h"
#include "system.h"


/*==================================================================================================
                                 Exported symbolic constants/macros
==================================================================================================*/
/** port names */
enum UART_DEV_NUMBER_ENUM
{
      #ifdef RCC_APB2ENR_USART1EN
      #if (UART_1_ENABLE > 0)
            UART_DEV_1,
      #endif
      #endif

      #ifdef RCC_APB1ENR_USART2EN
      #if (UART_2_ENABLE > 0)
            UART_DEV_2,
      #endif
      #endif

      #ifdef RCC_APB1ENR_USART3EN
      #if (UART_3_ENABLE > 0)
            UART_DEV_3,
      #endif
      #endif

      #ifdef RCC_APB1ENR_UART4EN
      #if (UART_4_ENABLE > 0)
            UART_DEV_4,
      #endif
      #endif

      #ifdef RCC_APB1ENR_UART5EN
      #if (UART_5_ENABLE > 0)
            UART_DEV_5,
      #endif
      #endif

      UART_DEV_LAST
};


/*==================================================================================================
                                  Exported types, enums definitions
==================================================================================================*/


/*==================================================================================================
                                     Exported function prototypes
==================================================================================================*/
extern stdRet_t UART_Init(nod_t dev);
extern stdRet_t UART_Open(nod_t dev);
extern stdRet_t UART_Close(nod_t dev);
extern size_t   UART_Write(nod_t dev, void *src, size_t size, size_t nitems, size_t seek);
extern size_t   UART_Read(nod_t dev, void *dst, size_t size, size_t nitems, size_t seek);
extern stdRet_t UART_IOCtl(nod_t dev, IORq_t ioRQ, void *data);
extern stdRet_t UART_Release(nod_t dev);

#ifdef __cplusplus
}
#endif

#endif /* UART_H_ */
/*==================================================================================================
                                            End of file
==================================================================================================*/
