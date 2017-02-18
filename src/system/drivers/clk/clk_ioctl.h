/*=========================================================================*//**
@file    clk_ioctl.h

@author  Daniel Zorychta

@brief   CLK ioctl request codes.

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

/**
 * @defgroup drv-clk System clock Driver
 *
 * \section drv-clk-desc Description
 * Driver handles system clock peripheral (PLL).
 *
 * \section drv-clk-sup-arch Supported architectures
 * \li STM32F10x
 *
 * @todo Details
 *
 *
 * @{
 */

#ifndef _CLK_IOCTL_H_
#define _CLK_IOCTL_H_

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
 *  @brief  Get frequency and name of selected clock [Hz].
 *  @param  [WR,RD] @ref CLK_info_t * clock information.
 *  @return On success 0 is returned, otherwise -1.
 *
 *  @b Example
 *  @code
    #include <sys/ioctl.h>

    //...

    CLK_info_t clk;
    clk.iterator = 0;

    int stat;

    do {
            stat = ioctl(IOCTL_CLK__GET_CLK_INFO, &clk);

            printf("%s: %d Hz\n", clk.name, clk.freq_Hz);

    while (stat == 0 && clk.name);

    //...
    @endcode
 */
#define IOCTL_CLK__GET_CLK_INFO   _IOWR(CLK, 0x00, CLK_info_t*)

/*==============================================================================
  Exported object types
==============================================================================*/
/**
 * Type represent clock info object (iterator).
 */
typedef struct {
        u8_t        iterator;           //!< [IN]  Clock iterator (starts from 0, auto incremented).
        u32_t       freq_Hz;            //!< [OUT] Clock frequency in Hz.
        const char *name;               //!< [OUT] Clock name (NULL if clock does not exist, end of iteration).
} CLK_info_t;

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

#endif /* _CLK_IOCTL_H_ */
/**@}*/
/*==============================================================================
  End of file
==============================================================================*/
