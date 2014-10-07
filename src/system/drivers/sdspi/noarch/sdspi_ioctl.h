/*=========================================================================*//**
@file    sdspi_ioctl.h

@author  Daniel Zorychta

@brief   This file support ioctl request codes.

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

#ifndef _SDSPI_IOCTL_H_
#define _SDSPI_IOCTL_H_

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
 *  @brief  Initialize SD card
 *  @param  None
 *  @return On success (card initialized) 1 is returned.
 *          On failure (card not initialized) 0 is returned.
 *          On error -1 is returned and errno is set.
 */
#define IOCTL_SDSPI__INITIALIZE_CARD    _IO(SDSPI, 0x00)

/**
 *  @brief  Read card's MBR sector and detect partitions
 *  @param  None
 *  @return On success (MBR detected) 1 is returned.
 *          On failure (MBR not exist) 0 is returned.
 *          On error -1 is returned and errno is set.
 */
#define IOCTL_SDSPI__READ_MBR           _IO(SDSPI, 0x01)

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

#endif /* _SDSPI_IOCTL_H_ */
/*==============================================================================
  End of file
==============================================================================*/
