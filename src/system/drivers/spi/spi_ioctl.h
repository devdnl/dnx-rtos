/*=========================================================================*//**
@file    spi_ioctl.h

@author  Daniel Zorychta

@brief   SPI module ioctl request codes.

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

#ifndef _SPI_IOCTL_H_
#define _SPI_IOCTL_H_

/*==============================================================================
  Include files
==============================================================================*/
#include "drivers/ioctl_macros.h"

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Exported macros
==============================================================================*/
/**
 *  @brief  Set SPI configuration
 *  @param  struct SPI_config *
 *  @return On success 0 is returned, otherwise -1
 */
#define IOCTL_SPI__SET_CONFIGURATION    _IOW(SPI, 0x00, struct SPI_config*)

/**
 *  @brief  Gets SPI configuration
 *  @param  struct SPI_config *
 *  @return On success 0 is returned, otherwise -1
 */
#define IOCTL_SPI__GET_CONFIGURATION    _IOR(SPI, 0x01, struct SPI_config*)

/**
 *  @brief  Select specified slave (CS = 0) [RAW mode]
 *  @param  None
 *  @return On success 0 is returned, otherwise -1
 */
#define IOCTL_SPI__SELECT               _IO(SPI, 0x02)

/**
 *  @brief  Deselect specified slave (CS = 1) [RAW mode]
 *  @param  None
 *  @return On success 0 is returned, otherwise -1
 */
#define IOCTL_SPI__DESELECT             _IO(SPI, 0x03)

/**
 *  @brief  Transmit and receive specified buffer
 *  @param  struct SPI_transive *
 *  @return On success 0 is returned, otherwise -1
 */
#define IOCTL_SPI__TRANSCEIVE           _IOWR(SPI, 0x04, struct SPI_transive*)

/**
 *  @brief  Transmit without selection
 *  @param  int         byte to transfer
 *  @return On success 0 is returned, otherwise -1
 */
#define IOCTL_SPI__TRANSMIT_NO_SELECT   _IOW(SPI, 0x05, int)

/*==============================================================================
  Exported object types
==============================================================================*/
/* SPI clock divider */
enum SPI_clk_divider {
        SPI_CLK_DIV_2,
        SPI_CLK_DIV_4,
        SPI_CLK_DIV_8,
        SPI_CLK_DIV_16,
        SPI_CLK_DIV_32,
        SPI_CLK_DIV_64,
        SPI_CLK_DIV_128,
        SPI_CLK_DIV_256
};

/* SPI modes */
enum SPI_mode {
        SPI_MODE_0,     /* CPOL = 0; CPHA = 0 (SCK 0 at idle, capture on rising edge)  */
        SPI_MODE_1,     /* CPOL = 0; CPHA = 1 (SCK 0 at idle, capture on falling edge) */
        SPI_MODE_2,     /* CPOL = 1; CPHA = 0 (SCK 1 at idle, capture on falling edge) */
        SPI_MODE_3      /* CPOL = 1; CPHA = 1 (SCK 1 at idle, capture on rising edge)  */
};

/* SPI configuration type */
struct SPI_config {
        u8_t                    dummy_byte  : 8;
        enum SPI_clk_divider    clk_divider : 3;
        enum SPI_mode           mode        : 2;
        bool                    msb_first   : 1;
        u8_t                    CS_port_idx;
        u8_t                    CS_pin_idx;
};

/* SPI transmit and receive type */
struct SPI_transceive {
        const u8_t             *tx_buffer;      /* TX buffer pointer  */
        u8_t                   *rx_buffer;      /* RX buffer pointer  */
        size_t                  count;          /* RX/TX buffer size  */
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

#endif /* _SPI_IOCTL_H_ */
/*==============================================================================
  End of file
==============================================================================*/
