/*=============================================================================================*//**
@file    pll.c

@author  Daniel Zorychta

@brief   File support PLL

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
#include "pll.h"


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
 * @brief Initialize clocks
 *
 * @param dev     device number
 * @param part    device part
 *
 * @retval STD_STATUS_OK
 *
 * NOTE: PLL2 and PLL3 not used
 */
//================================================================================================//
stdRet_t PLL_Init(devx_t dev, fd_t part)
{
      (void) dev;
      (void)part;

      return STD_RET_OK;
}


//================================================================================================//
/**
 * @brief Open device
 *
 * @param dev     device number
 * @param part    device part
 *
 * @retval STD_STATUS_OK
 */
//================================================================================================//
stdRet_t PLL_Open(devx_t dev, fd_t part)
{
      (void)dev;
      (void)part;

      return STD_RET_OK;
}


//================================================================================================//
/**
 * @brief Close device
 *
 * @param dev     device number
 * @param part    device part
 *
 * @retval STD_STATUS_OK
 */
//================================================================================================//
stdRet_t PLL_Close(devx_t dev, fd_t part)
{
      (void)dev;
      (void)part;

      return STD_RET_OK;
}


//================================================================================================//
/**
 * @brief Write to the device
 *
 * @param dev     device number
 * @param part    device part
 * @param *src    source
 * @param size    size
 * @param seek    seek
 *
 * @retval number of written nitems
 */
//================================================================================================//
size_t PLL_Write(devx_t dev, fd_t part, void *src, size_t size, size_t nitems, size_t seek)
{
      (void)dev;
      (void)part;
      (void)src;
      (void)size;
      (void)seek;
      (void)nitems;

      return STD_RET_OK;
}


//================================================================================================//
/**
 * @brief Read from device
 *
 * @param dev     device number
 * @param part    device part
 * @param *dst    destination
 * @param size    size
 * @param seek    seek
 *
 * @retval number of read nitems
 */
//================================================================================================//
size_t PLL_Read(devx_t dev, fd_t part, void *dst, size_t size, size_t nitems, size_t seek)
{
      (void)dev;
      (void)part;
      (void)dst;
      (void)size;
      (void)seek;
      (void)nitems;

      return STD_RET_OK;
}


//================================================================================================//
/**
 * @brief IO control
 *
 * @param[in]     dev     device number
 * @param part    device part
 * @param[in]     ioRq    IO reqest
 * @param[in,out] data    data pointer
 *
 * @retval STD_STATUS_OK
 */
//================================================================================================//
stdRet_t PLL_IOCtl(devx_t dev, fd_t part, IORq_t ioRq, void *data)
{
      (void)dev;
      (void)part;
      (void)ioRq;
      (void)data;

      return STD_RET_OK;
}


//================================================================================================//
/**
 * @brief Release PLL devices. Here is removed driver node and reseted device
 *
 * @param dev           device number
 * @param part          device part
 *
 * @retval STD_RET_OK
 */
//================================================================================================//
stdRet_t PLL_Release(devx_t dev, fd_t part)
{
      (void)dev;
      (void)part;

      return STD_RET_OK;
}


#ifdef __cplusplus
}
#endif

/*==================================================================================================
                                            End of file
==================================================================================================*/
