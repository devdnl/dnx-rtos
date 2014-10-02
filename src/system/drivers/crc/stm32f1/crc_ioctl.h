/*=========================================================================*//**
@file    crc_ioctl.h

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

#ifndef _CRC_IOCTL_H_
#define _CRC_IOCTL_H_

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
/** @brief  Set CRC input mode
 *  @param  enum CRC_input_mode *
 *  @return STD_RET_OK, STD_RET_ERROR
 */
#define IOCTL_CRC__SET_INPUT_MODE       _IOW(CRC, 0x00, enum CRC_input_mode*)

/** @brief  Get CRC input mode
 *  @param  enum CRC_input_mode *
 *  @return STD_RET_OK, STD_RET_ERROR
 */
#define IOCTL_CRC__GET_INPUT_MODE       _IOR(CRC, 0x01, enum CRC_input_mode*)

/*==============================================================================
  Exported object types
==============================================================================*/
/* NOTE: at all input modes result CRC value is ALWAYS an CRC32 */
enum CRC_input_mode {
        CRC_INPUT_MODE_BYTE,            /* base word is u8_t  *///!< CRC_INPUT_MODE_BYTE
        CRC_INPUT_MODE_HALF_WORD,       /* base word is u16_t *///!< CRC_INPUT_MODE_HALF_WORD
        CRC_INPUT_MODE_WORD             /* base word is u32_t *///!< CRC_INPUT_MODE_WORD
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

#endif /* _CRC_IOCTL_H_ */
/*==============================================================================
  End of file
==============================================================================*/
