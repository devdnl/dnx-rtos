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


/** statuses */
enum UART_STATUS_enum
{
      UART_STATUS_PORTNOTEXIST          = -1,
      UART_STATUS_PORTLOCKED            = -2,
      UART_STATUS_INCORRECTSIZE         = -3,
      UART_STATUS_NOFREEMEM             = -4,
      UART_STATUS_BADRQ                 = -5,
      UART_STATUS_BUFFEREMPTY           = -6,
};


/** IO request for UART driver */
enum UART_IORq_enum
{
      UART_IORQ_ENABLE_WAKEUP_IDLE,                   /* no arg       */
      UART_IORQ_ENABLE_WAKEUP_ADDRESS_MARK,           /* no arg       */
      UART_IORQ_ENABLE_PARITY_CHECK,                  /* no arg       */
      UART_IORQ_DISABLE_PARITY_CHECK,                 /* no arg       */
      UART_IORQ_SET_ODD_PARITY,                       /* no arg       */
      UART_IORQ_SET_EVEN_PARITY,                      /* no arg       */
      UART_IORQ_ENABLE_RECEIVER_WAKEUP_MUTE,          /* no arg       */
      UART_IORQ_DISABLE_RECEIVER_WAKEUP_MUTE,         /* no arg       */
      UART_IORQ_ENABLE_LIN_MODE,                      /* no arg       */
      UART_IORQ_DISABLE_LIN_MODE,                     /* no arg       */
      UART_IORQ_SET_1_STOP_BIT,                       /* no arg       */
      UART_IORQ_SET_2_STOP_BITS,                      /* no arg       */
      UART_IORQ_SET_LIN_BRK_DETECTOR_11_BITS,         /* no arg       */
      UART_IORQ_SET_LIN_BRK_DETECTOR_10_BITS,         /* no arg       */
      UART_IORQ_SET_ADDRESS_NODE,                     /* in u8_t arg  */
      UART_IORQ_ENABLE_CTS,                           /* no arg       */
      UART_IORQ_DISABLE_CTS,                          /* no arg       */
      UART_IORQ_ENABLE_RTS,                           /* no arg       */
      UART_IORQ_DISABLE_RTS,                          /* no arg       */
      UART_IORQ_GET_BYTE,                             /* out u8_t arg */
      UART_IORQ_SEND_BYTE,                            /* in 8_t arg   */
      UART_IORQ_SET_BAUDRATE,                         /* in u32_t arg */
};


/*==================================================================================================
                                  Exported types, enums definitions
==================================================================================================*/


/*==================================================================================================
                                     Exported function prototypes
==================================================================================================*/
extern stdRet_t UART_Init(void);


extern stdRet_t UART_Open(dev_t usartName);


extern stdRet_t UART_Close(dev_t usartName);


extern stdRet_t UART_Write(dev_t usartName, void *src, size_t size, size_t seek);


extern stdRet_t UART_Read(dev_t usartName, void *dst, size_t size, size_t seek);


extern stdRet_t UART_IOCtl(dev_t usartName, IORq_t ioRQ, void *data);


#ifdef __cplusplus
   }
#endif

#endif /* UART_H_ */
/*==================================================================================================
                                            End of file
==================================================================================================*/
