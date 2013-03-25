/*=========================================================================*//**
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


*//*==========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Include files
==============================================================================*/
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

MODULE_NAME(UART);

/*==============================================================================
  Local symbolic constants/macros
==============================================================================*/

/*==============================================================================
  Local types, enums definitions
==============================================================================*/

/*==============================================================================
  Local function prototypes
==============================================================================*/

/*==============================================================================
  Local object definitions
==============================================================================*/

/*==============================================================================
  Function definitions
==============================================================================*/

//==============================================================================
/**
 * @brief Initialize USART devices
 *
 * @param[out] **drvhdl         driver's memory handler
 * @param[in]  dev              device number
 * @param[in]  part             device part
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
stdret_t UART_init(void **drvhdl, uint dev, uint part)
{
        (void)drvhdl;
        (void)dev;
        (void)part;

        return STD_RET_OK;
}

//==============================================================================
/**
 * @brief Release USART devices
 *
 * @param[in] *drvhdl           driver's memory handler
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
stdret_t UART_release(void *drvhdl)
{
        (void)drvhdl;

        return STD_RET_OK;
}

//==============================================================================
/**
 * @brief Opens specified port
 *
 * @param[in] *drvhdl           driver's memory handler
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
stdret_t UART_open(void *drvhdl)
{
        (void)drvhdl;

        return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief Function close opened port
 *
 * @param[in] *drvhdl           driver's memory handler
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
stdret_t UART_close(void *drvhdl)
{
        (void)drvhdl;

        return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief Write data to UART (ISR or DMA)
 *
 * @param[in] *drvhdl           driver's memory handle
 * @param[in] *src              source
 * @param[in] size              size
 * @param[in] seek              seek
 *
 * @retval number of written nitems
 */
//==============================================================================
size_t UART_write(void *drvhdl, void *src, size_t size, size_t nitems, size_t seek)
{
        (void)drvhdl;
        (void)seek;

        char *buf = calloc(256, sizeof(char));

        size_t datasize = 0;

        if (size * nitems < 256) {
                datasize = size * nitems;
        } else {
                datasize = sizeof(buf);
        }

        snprintf(buf, datasize, "%s", (char*) src);

        printf("%s", buf);

        return datasize;
}

//==============================================================================
/**
 * @brief Read data from UART Rx buffer
 *
 * @param[in]  *drvhdl          driver's memory handle
 * @param[out] *dst             destination
 * @param[in]  size             size
 * @param[in]  seek             seek
 *
 * @retval number of read nitems
 */
//==============================================================================
size_t UART_read(void *drvhdl, void *dst, size_t size, size_t nitems, size_t seek)
{
        (void) drvhdl;
        (void) dst;
        (void) size;
        (void) nitems;
        (void) seek;

        if (dst) {
                char *data = dst;

                for (uint i = 0; i < (size * nitems); i++) {
                        *(data++) = getc(stdin);
                }
        }

        return nitems;
}

//==============================================================================
/**
 * @brief Direct IO control
 *
 * @param[in]     *drvhdl       driver's memory handle
 * @param[in]     ioRq          IO reqest
 * @param[in,out] data          data pointer
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
stdret_t UART_ioctl(void *drvhdl, iorq_t ioRq, void *data)
{
        (void)drvhdl;
        (void)ioRq;
        (void)data;

        return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief Function flush device
 *
 * @param[in] *drvhdl           driver's memory handle
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
stdret_t UART_flush(void *drvhdl)
{
        (void)drvhdl;

        return STD_RET_OK;
}

#ifdef __cplusplus
}
#endif

/*==============================================================================
  End of file
==============================================================================*/
