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
#include "core/ioctl_macros.h"

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Exported macros
==============================================================================*/
/** @brief  Gets SYSCLK frequency [Hz]
 *  @param  u32_t *
 *  @return STD_RET_OK, STD_RET_ERROR
 */
#define IOCTL_PLL__GET_SYSCLK_FREQ      _IOR(_IO_GROUP_PLL, 0x00, u32_t*)

/** @brief  Gets HCLK frequency [Hz]
 *  @param  u32_t *
 *  @return STD_RET_OK, STD_RET_ERROR
 */
#define IOCTL_PLL__GET_HCLK_FREQ        _IOR(_IO_GROUP_PLL, 0x01, u32_t*)

/** @brief  Gets PCLK1 frequency [Hz]
 *  @param  u32_t *
 *  @return STD_RET_OK, STD_RET_ERROR
 */
#define IOCTL_PLL__GET_PCLK1_FREQ       _IOR(_IO_GROUP_PLL, 0x02, u32_t*)

/** @brief  Gets PCLK2 frequency [Hz]
 *  @param  u32_t *
 *  @return STD_RET_OK, STD_RET_ERROR
 */
#define IOCTL_PLL__GET_PCLK2_FREQ       _IOR(_IO_GROUP_PLL, 0x03, u32_t*)

/** @brief  Gets ADCCLK frequency [Hz]
 *  @param  u32_t *
 *  @return STD_RET_OK, STD_RET_ERROR
 */
#define IOCTL_PLL__GET_ADCCLK_FREQ      _IOR(_IO_GROUP_PLL, 0x04, u32_t*)

/** @brief  Gets PCLK1 frequency [Hz]
 *  @param  u32_t *
 *  @return STD_RET_OK, STD_RET_ERROR
 */
#define IOCTL_PLL__GET_PCLK1_TIM_FREQ   _IOR(_IO_GROUP_PLL, 0x05, u32_t*)

/** @brief  Gets PCLK2 frequency [Hz]
 *  @param  u32_t *
 *  @return STD_RET_OK, STD_RET_ERROR
 */
#define IOCTL_PLL__GET_PCLK2_TIM_FREQ   _IOR(_IO_GROUP_PLL, 0x06, u32_t*)

/*==============================================================================
  Exported object types
==============================================================================*/

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
