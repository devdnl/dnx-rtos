/*=============================================================================================*//**
@file    usart.c

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
#include "uart.h"
#include <stdlib.h>

#undef fclose
#undef fopen
#undef fread
#undef fseek
#undef ftell
#undef fwrite
#undef remove
#undef rename

#include <stdio.h>
#include <string.h>


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
                                        Function definitions
==================================================================================================*/

//================================================================================================//
/**
 * @brief Initialize USART devices
 *
 * @param[in] dev           UART device
 * @param[in] part          device part
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//================================================================================================//
stdRet_t UART_Init(devx_t dev, fd_t part)
{
      (void)dev;
      (void)part;

      return STD_RET_OK;
}


//================================================================================================//
/**
 * @brief Release USART devices
 *
 * @param[in] dev           I2C device
 * @param[in] part          device part
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//================================================================================================//
stdRet_t UART_Release(devx_t dev, fd_t part)
{
      (void)dev;
      (void)part;

      return STD_RET_OK;
}


//================================================================================================//
/**
 * @brief Opens specified port and initialize default settings
 *
 * @param[in]  dev                        USART name (number)
 * @param[in]  part                       device part
 *
 * @retval STD_STATUS_OK                  operation success
 * @retval UART_STATUS_PORTLOCKED         port locked for other task
 * @retval UART_STATUS_PORTNOTEXIST       port number does not exist
 * @retval UART_STATUS_NOFREEMEM          no enough free memory to allocate RxBuffer
 */
//================================================================================================//
stdRet_t UART_Open(devx_t dev, fd_t part)
{
      (void)dev;
      (void)part;

      return STD_RET_OK;
}


//================================================================================================//
/**
 * @brief Function close opened port
 *
 * @param[in]  dev                        USART name (number)
 * @param[in]  part                       device part
 *
 * @retval STD_STATUS_OK                  operation success
 * @retval UART_STATUS_PORTLOCKED         port locked for other task
 * @retval UART_STATUS_PORTNOTEXIST       port number does not exist
 */
//================================================================================================//
stdRet_t UART_Close(devx_t dev, fd_t part)
{
      (void)dev;
      (void)part;

      return STD_RET_OK;
}


//================================================================================================//
/**
 * @brief Write data to UART (ISR or DMA)
 *
 * @param[in]  dev                        dev number
 * @param[in]  part                       device part
 * @param[in]  *src                       source buffer
 * @param[in]  size                       item size
 * @param[in]  nitems                     number of items
 * @param[in]  seek                       seek
 *
 * @return number of transmitted nitems
 */
//================================================================================================//
size_t UART_Write(devx_t dev, fd_t part, void *src, size_t size, size_t nitems, size_t seek)
{
      (void)dev;
      (void)part;
      (void)seek;

      ch_t *buf = calloc(256, sizeof(ch_t));

      size_t datasize = 0;

      if (size * nitems < 256) {
            datasize = size * nitems;
      } else {
            datasize = sizeof(buf);
      }

      snprintf(buf, datasize, "%s", (ch_t*)src);

      printf("%s", buf);

      return datasize;
}


//================================================================================================//
/**
 * @brief Read data from UART Rx buffer
 *
 * @param[in]  dev                        dev number
 * @param[in]  part                       device part
 * @param[out] *dst                       destination buffer
 * @param[in]  size                       item size
 * @param[in]  nitems                     number of items
 * @param[in]  seek                       seek
 *
 * @return number of received nitems
 */
//================================================================================================//
size_t UART_Read(devx_t dev, fd_t part, void *dst, size_t size, size_t nitems, size_t seek)
{
      (void)dev;
      (void)part;
      (void)dst;
      (void)size;
      (void)nitems;
      (void)seek;

      if (dst) {
            ch_t *data = dst;

            for (uint_t i = 0; i < (size * nitems); i++) {
                  *(data++) = getc(stdin);
            }
      }

      return nitems;
}


//================================================================================================//
/**
 * @brief Direct IO control
 *
 * @param[in]     dev                     USART name (number)
 * @param[in]     part                    device part
 * @param[in,out] ioRQ                    IO request
 * @param[in,out] *data                   IO data (arguments, results, etc)
 *
 * @retval STD_STATUS_OK                  operation success
 * @retval UART_STATUS_PORTLOCKED         port locked for other task
 * @retval UART_STATUS_PORTNOTEXIST       port number does not exist
 * @retval UART_STATUS_BUFFEREMPTY        rx buffer empty
 * @retval UART_STATUS_BADRQ              bad request
 */
//================================================================================================//
stdRet_t UART_IOCtl(devx_t dev, fd_t part, iorq_t ioRQ, void *data)
{
      (void)dev;
      (void)part;
      (void)ioRQ;
      (void)data;

      return STD_RET_OK;
}


#ifdef __cplusplus
}
#endif

/*==================================================================================================
                                            End of file
==================================================================================================*/
