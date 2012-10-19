/*=============================================================================================*//**
 @file    ds1307nvm.c

 @author  Daniel Zorychta

 @brief   This file support DS1307 NVM

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
extern "C"
{
#endif

/*==================================================================================================
 Include files
 ==================================================================================================*/
#include "ds1307.h"
#include "ds1307nvm.h"
#include "i2c_def.h"


/*==================================================================================================
 Local symbolic constants/macros
 ==================================================================================================*/
#define I2CFILE                     "/dev/i2c"


/*==================================================================================================
 Local types, enums definitions
 ==================================================================================================*/


/*==================================================================================================
 Local function prototypes
 ==================================================================================================*/


/*==================================================================================================
 Local object definitions
 ==================================================================================================*/
u32_t lock;


/*==================================================================================================
 Exported object definitions
 ==================================================================================================*/


/*==================================================================================================
 Function definitions
 ==================================================================================================*/

//================================================================================================//
/**
 * @brief Initialize RTC devices
 *
 * @param dev     device number
 *
 * @retval STD_RET_OK                     operation success
 * @retval STD_RET_ERROR                  error
 */
//================================================================================================//
stdRet_t DS1307NVM_Init(dev_t dev)
{
      (void)dev;

      lock = EMPTY_TASK;

      return STD_RET_OK;
}


//================================================================================================//
/**
 * @brief Opens specified port and initialize default settings
 *
 * @param[in]  dev                        RTC name (number)
 *
 * @retval STD_RET_OK                     operation success
 * @retval STD_RET_ERROR                  error
 */
//================================================================================================//
stdRet_t DS1307RTC_Open(dev_t dev)
{
      (void)dev;

      if (lock == EMPTY_TASK)
      {
            TaskSuspendAll();
            lock = TaskGetPID();
            TaskResumeAll();
      }

      return STD_RET_OK;
}


//================================================================================================//
/**
 * @brief Function close opened port
 *
 * @param[in]  dev                        RTC name (number)
 *
 * @retval STD_RET_OK                     operation success
 * @retval STD_RET_ERROR                  error
 */
//================================================================================================//
stdRet_t DS1307RTC_Close(dev_t dev)
{
      (void)dev;

      stdRet_t status = STD_RET_ERROR;

      if (lock == (u32_t)TaskGetPID())
      {
            lock = EMPTY_TASK;

            status = STD_RET_OK;
      }

      return status;
}


//================================================================================================//
/**
 * @brief Write data to UART (ISR or DMA)
 *
 * @param[in]  dev                        RTC name (number)
 * @param[in]  *src                       source buffer
 * @param[in]  size                       buffer size
 * @param[in]  seek                       seek
 *
 * @retval STD_RET_OK                     operation success
 * @retval STD_RET_ERROR                  error
 */
//================================================================================================//
stdRet_t DS1307RTC_Write(dev_t dev, void *src, size_t size, size_t seek)
{
      (void)dev;

      stdRet_t status = STD_RET_ERROR;
      u8_t     tmp;
      FILE_t   *i2c;

      if (lock == (u32_t)TaskGetPID())
      {
            if (src && size)
            {
                  /* try open port */
                  if ((i2c = fopen(I2CFILE, NULL)) != NULL)
                  {
                        /* set DS1307 address */
                        tmp = DS1307_ADDRESS;
                        if (ioctl(i2c, I2C_IORQ_SETSLAVEADDR, &tmp) != STD_RET_OK)
                              goto WriteRAM_ClosePort;

                        /* write data into RAM */
                        fseek(i2c, REG_RAM_BEGIN + seek, 0);
                        if (fwrite(src, sizeof(u8_t), size, i2c) != STD_RET_OK)
                              goto WriteRAM_ClosePort;

                        status = STD_RET_OK;

                        /* close port */
                        WriteRAM_ClosePort:
                        fclose(i2c);
                  }
            }
      }

      return status;
}


//================================================================================================//
/**
 * @brief Read data from UART Rx buffer
 *
 * @param[in]  dev                        RTC name (number)
 * @param[out] *dst                       destination buffer
 * @param[in]  size                       buffer size
 * @param[in]  seek                       seek
 *
 * @retval STD_RET_OK                     operation success
 * @retval STD_RET_ERROR                  error
 */
//================================================================================================//
stdRet_t DS1307RTC_Read(dev_t dev, void *dst, size_t size, size_t seek)
{
      (void)dev;

      stdRet_t status = STD_RET_ERROR;
      u8_t     tmp;
      FILE_t   *i2c;

      if (lock == (u32_t)TaskGetPID())
      {
            if (dst && size)
            {
                  /* try open port */
                  if ((i2c = fopen(I2CFILE, NULL)) != NULL)
                  {
                        /* set DS1307 address */
                        tmp = DS1307_ADDRESS;
                        if (ioctl(i2c, I2C_IORQ_SETSLAVEADDR, &tmp) != STD_RET_OK)
                              goto ReadRAM_ClosePort;

                        /* read data from RAM */
                        fseek(i2c, REG_RAM_BEGIN + seek, 0);
                        if (fread(dst, sizeof(u8_t), size, i2c) != STD_RET_OK)
                              goto ReadRAM_ClosePort;

                        status = STD_RET_OK;

                        /* close port */
                        ReadRAM_ClosePort:
                        fclose(i2c);
                  }
            }
      }

      return status;
}


//================================================================================================//
/**
 * @brief Direct IO control
 *
 * @param[in]     dev                     RTC name (number)
 * @param[in,out] ioRQ                    IO request
 * @param[in,out] *data                   IO data (arguments, results, etc)
 *
 * @retval STD_RET_OK                     operation success
 * @retval STD_RET_ERROR                  error
 */
//================================================================================================//
stdRet_t DS1307RTC_IOCtl(dev_t dev, IORq_t ioRQ, void *data)
{
      (void)dev;
      (void)ioRQ;
      (void)data;

      return STD_RET_ERROR;
}


//================================================================================================//
/**
 * @brief Release RTC devices
 *
 * @param dev     device number
 *
 * @retval STD_RET_OK
 */
//================================================================================================//
stdRet_t DS1307RTC_Release(dev_t dev)
{
      (void) dev;

      return STD_RET_OK;
}

#ifdef __cplusplus
}
#endif

/*==================================================================================================
 End of file
 ==================================================================================================*/
