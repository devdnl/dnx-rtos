/*=========================================================================*//**
@file    crcm_ioctl.h

@author  Daniel Zorychta

@brief   CRC driver ioctl request codes.

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

#ifndef _CRCM_IOCTL_H_
#define _CRCM_IOCTL_H_

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
/** @brief  Set CRC input mode
 *  @param  enum CRC_input_mode *
 *  @return On success 0 is returned, otherwise -1 and errno code is set
 */
#define IOCTL_CRCM__SET_INPUT_MODE       _IOW(CRCM, 0x00, enum CRC_input_mode*)

/** @brief  Get CRC input mode
 *  @param  enum CRC_input_mode *
 *  @return On success 0 is returned, otherwise -1 and errno code is set
 */
#define IOCTL_CRCM__GET_INPUT_MODE       _IOR(CRCM, 0x01, enum CRC_input_mode*)

/*==============================================================================
  Exported object types
==============================================================================*/
/* NOTE: at all input modes result CRC value is ALWAYS an CRC32 */
enum CRCM_input_mode {
        CRCM_INPUT_MODE_8BIT,            /* base word is u8_t  */
        CRCM_INPUT_MODE_16BIT,           /* base word is u16_t */
        CRCM_INPUT_MODE_32BIT            /* base word is u32_t */
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

#endif /* _CRCM_IOCTL_H_ */
/*==============================================================================
  End of file
==============================================================================*/
