/*=========================================================================*//**
@file    pll_ioctl.h

@author  Daniel Zorychta

@brief   PLL ioctl request codes.

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

#ifndef _PLL_IOCTL_H_
#define _PLL_IOCTL_H_

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
 *  @brief  Get frequency and name of selected clock [Hz]
 *  @param  PLL_clk_info_t *
 *  @return On success 0 is returned, otherwise -1
 */
#define IOCTL_PLL__GET_CLK_INFO   _IOR(PLL, 0x00, PLL_clk_info_t*)

/*==============================================================================
  Exported object types
==============================================================================*/
typedef struct {
        u8_t        iterator;           //!< [IN]  clock iterator (starts from 0, auto incremented)
        u32_t       clock_Hz;           //!< [OUT] clock frequency in Hz
        const char *clock_name;         //!< [OUT] clock name
} PLL_clk_info_t;

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

#endif /* _PLL_IOCTL_H_ */
/*==============================================================================
  End of file
==============================================================================*/
