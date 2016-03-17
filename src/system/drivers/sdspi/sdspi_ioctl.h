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

/**
 * @defgroup drv-sdspi SDSPI Driver
 *
 * \section drv-sdspi-desc Description
 * Driver handles SD Card by using SPI peripheral (indirect).
 *
 * \section drv-sdspi-sup-arch Supported architectures
 * \li STM32F10x
 *
 * @{
 */

#ifndef _SDSPI_IOCTL_H_
#define _SDSPI_IOCTL_H_

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
 *  @brief  Initialize SD card (OS storage request).
 *  @return On success (card initialized) 0 is returned.
 *          On error (card not initialized) -1 is returned and @ref errno is set.
 */
#define IOCTL_SDSPI__INITIALIZE_CARD    IOCTL_STORAGE__INITIALIZE

/**
 *  @brief  Read card's MBR sector and detect partitions (OS storage request).
 *  @return On success (MBR detected) 0 is returned.
 *          On error -1 (MBR not exist or IO error) is returned and @ref errno is set.
 */
#define IOCTL_SDSPI__READ_MBR           IOCTL_STORAGE__READ_MBR

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
/**@}*/
/*==============================================================================
  End of file
==============================================================================*/
