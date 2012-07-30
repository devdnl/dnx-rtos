/*=============================================================================================*//**
@file    eth.c

@author  Daniel Zorychta

@brief   This file support ethernet peripheral

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


*//*==============================================================================================*/

#ifdef __cplusplus
      extern "C" {
#endif

/*==================================================================================================
                                            Include files
==================================================================================================*/
#include "eth.h"
#include "stm32_eth.h"


/*==================================================================================================
                                  Local symbolic constants/macros
==================================================================================================*/


/*==================================================================================================
                                   Local types, enums definitions
==================================================================================================*/


/*==================================================================================================
                                      Local function prototypes
==================================================================================================*/


/*==================================================================================================
                                      Local object definitions
==================================================================================================*/


/*==================================================================================================
                                     Exported object definitions
==================================================================================================*/


/*==================================================================================================
                                        Function definitions
==================================================================================================*/

//================================================================================================//
/**
 * @brief Initialise unit
 */
//================================================================================================//
stdStatus_t ETHER_Init(void)
{
      return STD_STATUS_OK;
}


//================================================================================================//
/**
 * @brief Open device
 *
 * @param dev     device number
 *
 * @retval STD_STATUS_OK
 */
//================================================================================================//
stdStatus_t ETHER_Open(dev_t dev)
{
      (void)dev;

      return STD_STATUS_OK;
}


//================================================================================================//
/**
 * @brief Close device
 *
 * @param dev     device number
 *
 * @retval STD_STATUS_OK
 */
//================================================================================================//
stdStatus_t ETHER_Close(dev_t dev)
{
      (void)dev;

      return STD_STATUS_OK;
}


//================================================================================================//
/**
 * @brief Write to the device
 *
 * @param dev     device number
 * @param *src    source
 * @param size    size
 * @param seek    seek
 *
 * @retval STD_STATUS_OK
 */
//================================================================================================//
stdStatus_t ETHER_Write(dev_t dev, void *src, size_t size, size_t seek)
{
      (void)dev;
      (void)src;
      (void)size;
      (void)seek;

      return STD_STATUS_OK;
}


//================================================================================================//
/**
 * @brief Read from device
 *
 * @param dev     device number
 * @param *dst    destination
 * @param size    size
 * @param seek    seek
 *
 * @retval STD_STATUS_OK
 */
//================================================================================================//
stdStatus_t ETHER_Read(dev_t dev, void *dst, size_t size, size_t seek)
{
      (void)dev;
      (void)dst;
      (void)size;
      (void)seek;

      return STD_STATUS_OK;
}


//================================================================================================//
/**
 * @brief IO control
 *
 * @param[in]     dev     device number
 * @param[in]     ioRq    IO reqest
 * @param[in,out] data    data pointer
 *
 * @retval STD_STATUS_OK
 */
//================================================================================================//
stdStatus_t ETHER_IOCtl(dev_t dev, IORq_t ioRq, void *data)
{
      (void)dev;
      (void)ioRq;
      (void)data;

      return STD_STATUS_OK;
}

#ifdef __cplusplus
      }
#endif

/*==================================================================================================
                                            End of file
==================================================================================================*/
