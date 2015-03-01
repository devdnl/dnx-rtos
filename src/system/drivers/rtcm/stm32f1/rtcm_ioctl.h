/*=========================================================================*//**
@file    rtcm_ioctl.h

@author  Daniel Zorychta

@brief   Real Time Clock Module

@note    Copyright (C) 2015 Daniel Zorychta <daniel.zorychta@gmail.com>

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

#ifndef _RTCM_IOCTL_H_
#define _RTCM_IOCTL_H_

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
/**
 * @brief  ...
 * @param  ...
 * @return ...
 */
#define IOCTL_RTCM__EX1           _IO(RTCM, 0x00)

/**
 * @brief  ...
 * @param  ...
 * @return ...
 */
#define IOCTL_RTCM__EX2           _IOR(RTCM, 0x01, int*)

/**
 * @brief  ...
 * @param  ...
 * @return ...
 */
#define IOCTL_RTCM__EX3           _IOW(RTCM, 0x02, int*)

/**
 * @brief  ...
 * @param  ...
 * @return ...
 */
#define IOCTL_RTCM__EX4           _IOWR(RTCM, 0x03, int*)


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

#endif /* _RTCM_IOCTL_H_ */
/*==============================================================================
  End of file
==============================================================================*/
