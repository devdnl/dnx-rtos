/*=============================================================================================*//**
@file    i2c.c

@author  Daniel Zorychta

@brief   This driver support I2C

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
#include "i2c.h"


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
                                        Function definitions
==================================================================================================*/

//================================================================================================//
/**
 * @brief Initialize I2C
 *
 * @param[in] dev             I2C device
 * @param[in] part            device part
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//================================================================================================//
stdRet_t I2C_Init(devx_t dev, fd_t part)
{
      (void)dev;
      (void)part;

      return STD_RET_OK;
}


//================================================================================================//
/**
 * @brief Release I2C peripheral
 *
 * @param[in] dev           I2C device
 * @param[in] part          device part
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//================================================================================================//
stdRet_t I2C_Release(devx_t dev, fd_t part)
{
      (void)dev;
      (void)part;

      return STD_RET_OK;
}


//================================================================================================//
/**
 * @brief Open selected I2C port
 *
 * @param[in] dev           I2C device
 * @param[in] part          device part
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//================================================================================================//
stdRet_t I2C_Open(devx_t dev, fd_t part)
{
      (void)dev;
      (void)part;

      return STD_RET_OK;
}


//================================================================================================//
/**
 * @brief Close port
 *
 * @param[in] dev           I2C device
 * @param[in] part          device part
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//================================================================================================//
stdRet_t I2C_Close(devx_t dev, fd_t part)
{
      (void)dev;
      (void)part;

      return STD_RET_OK;
}


//================================================================================================//
/**
 * @brief Function write data to the I2C device
 *
 * @param[in] dev           I2C device
 * @param[in] part          device part
 * @param[in] *src          source data
 * @param[in] size          item size
 * @param[in] nitems        number of items
 * @param[in] seek          register address
 *
 * @retval number of written nitems
 */
//================================================================================================//
size_t I2C_Write(devx_t dev, fd_t part, void *src, size_t size, size_t nitems, size_t seek)
{
      (void)dev;
      (void)part;
      (void)src;
      (void)size;
      (void)nitems;
      (void)seek;

      return 0;
}


//================================================================================================//
/**
 * @brief Read data wrom I2C device
 *
 * @param[in ] dev           I2C device
 * @param[in ] part          device part
 * @param[out] *dst          destination data
 * @param[in ] size          data size
 * @param[in ] seek          register address
 *
 * @retval number of written nitems
 */
//================================================================================================//
size_t I2C_Read(devx_t dev, fd_t part, void *dst, size_t size, size_t nitems, size_t seek)
{
      (void)dev;
      (void)part;
      (void)dst;
      (void)size;
      (void)nitems;
      (void)seek;


      return 0;
}


//================================================================================================//
/**
 * @brief Specific settings of I2C port
 *
 * @param[in    ] dev           I2C device
 * @param[in    ] part          device part
 * @param[in    ] ioRQ          input/output reqest
 * @param[in,out] *data         input/output data
 *
 * @retval STD_RET_OK                     operation success
 * @retval I2C_STATUS_PORTNOTEXIST        port not exist
 * @retval I2C_STATUS_PORTLOCKED          port locked
 * @retval I2C_STATUS_OVERRUN             overrun
 * @retval I2C_STATUS_ACK_FAILURE         ack failure
 * @retval I2C_STATUS_ARB_LOST            arbitration lost error
 * @retval I2C_STATUS_BUS_ERROR           bus error
 * @retval I2C_STATUS_ERROR               more than 1 error
 */
//================================================================================================//
stdRet_t I2C_IOCtl(devx_t dev, fd_t part, IORq_t ioRQ, void *data)
{
      (void)dev;
      (void)ioRQ;
      (void)data;
      (void)part;

      return STD_RET_OK;
}


#ifdef __cplusplus
}
#endif

/*==================================================================================================
                                            End of file
==================================================================================================*/
