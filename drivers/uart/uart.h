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
#define UART_PART_NONE        0


/** port names */
enum UART_DEV_NUMBER_ENUM
{
      #if defined(RCC_APB2ENR_USART1EN) && (UART_1_ENABLE > 0)
            UART_DEV_1,
      #endif

      #if defined(RCC_APB1ENR_USART2EN) && (UART_2_ENABLE > 0)
            UART_DEV_2,
      #endif

      #if defined(RCC_APB1ENR_USART3EN) && (UART_3_ENABLE > 0)
            UART_DEV_3,
      #endif

      #if defined(RCC_APB1ENR_UART4EN) && (UART_4_ENABLE > 0)
            UART_DEV_4,
      #endif

      #if defined(RCC_APB1ENR_UART5EN) && (UART_5_ENABLE > 0)
            UART_DEV_5,
      #endif

      UART_DEV_LAST
};


/*==================================================================================================
                                  Exported types, enums definitions
==================================================================================================*/


/*==================================================================================================
                                     Exported function prototypes
==================================================================================================*/
extern stdRet_t UART_Init   (devx_t dev, fd_t part);
extern stdRet_t UART_Open   (devx_t dev, fd_t part);
extern stdRet_t UART_Close  (devx_t dev, fd_t part);
extern size_t   UART_Write  (devx_t dev, fd_t part, void *src, size_t size, size_t nitems, size_t seek);
extern size_t   UART_Read   (devx_t dev, fd_t part, void *dst, size_t size, size_t nitems, size_t seek);
extern stdRet_t UART_IOCtl  (devx_t dev, fd_t part, IORq_t ioRQ, void *data);
extern stdRet_t UART_Release(devx_t dev, fd_t part);

#ifdef __cplusplus
}
#endif

#endif /* UART_H_ */
/*==================================================================================================
                                            End of file
==================================================================================================*/
