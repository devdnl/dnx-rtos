/*=============================================================================================*//**
@file    terminal.c

@author  Daniel Zorychta

@brief

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

/*==================================================================================================
                                            Include files
==================================================================================================*/
#include "terminal.h"
#include "uart.h"
#include <string.h>
#include <stdarg.h>


/*==================================================================================================
                                  Local symbolic constants/macros
==================================================================================================*/


/*==================================================================================================
                                   Local types, enums definitions
==================================================================================================*/


/*==================================================================================================
                                      Local function prototypes
==================================================================================================*/


/*==================================================================================================
                                      Local object definitions
==================================================================================================*/


/*==================================================================================================
                                     Exported object definitions
==================================================================================================*/


/*==================================================================================================
                                        Function definitions
==================================================================================================*/

//================================================================================================//
/**
 * @brief
 */
//================================================================================================//
APPLICATION(terminal)
{
      ch_t buffer[20];

      const ch_t *text = "Wather station supported by FreeRTOS. Welcome.\r\n";

      if (UART_Open(UART_DEV_1) != STD_STATUS_OK)
            TaskTerminate();

      UART_Write(UART_DEV_1, (ch_t*)text, strlen(text), 0);

      for (;;)
      {
            ch_t key;

            if (UART_IOCtl(UART_DEV_1, UART_IORQ_GET_BYTE, &key) == STD_STATUS_OK)
                  UART_IOCtl(UART_DEV_1, UART_IORQ_SEND_BYTE, &key);

            if (key == 'r')
            {
                  memset(buffer, 0, sizeof(buffer));
                  bprint(buffer, sizeof(buffer), "\r\nTask ID: %x\r\n", TaskGetPID());
                  UART_Write(UART_DEV_1, buffer, strlen(buffer), 0);
                  key = 0;
            }
      }

      UART_Close(UART_DEV_1);

      TaskTerminate();
}


/*==================================================================================================
                                            End of file
==================================================================================================*/
