/*=============================================================================================*//**
 @file    ds1307rtc.c

 @author  Daniel Zorychta

 @brief   This file support DS1307 RTC

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
#include "ds1307rtc.h"
#include "vfs.h"
#include "i2c_def.h"


/*==================================================================================================
 Local symbolic constants/macros
==================================================================================================*/
#define BCD2BYTE(bcd)               (((bcd >> 4) * 10) + (bcd & 0x0F))


/*==================================================================================================
 Local types, enums definitions
==================================================================================================*/


/*==================================================================================================
 Local function prototypes
==================================================================================================*/
static u8_t WeekDay(u16_t year, u8_t month, u8_t day);


/*==================================================================================================
 Local object definitions
==================================================================================================*/
/** current time */
static bcdTime_t currentTime;

/** current date */
static bcdDate_t currentDate;


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
stdRet_t DS1307RTC_Init(dev_t dev)
{
      (void)dev;

      stdRet_t status = STD_RET_ERROR;
      u8_t     tmp;
      FILE_t   *i2c;

      kprint("Initializing RTC...");

      /* try to open port */
      if ((i2c = fopen("/dev/i2c", NULL)) != NULL)
      {
            /* set DS1307 address */
            tmp = DS1307_ADDRESS;
            if (ioctl(i2c, I2C_IORQ_SETSLAVEADDR, &tmp) != STD_RET_OK)
                  goto DS1307_Init_Error;

            /* read second register */
            fseek(i2c, REG_SECONDS, 0);
            if (fread(&tmp, sizeof(tmp), 1, i2c) != STD_RET_OK)
                  goto DS1307_Init_Error;

            /* enable oscillator if disabled */
            if (tmp & DS1307_REG_SECONDS_CH_BM)
            {
                  tmp = 0x00;
                  if ((status = I2C_Write(I2C_NUMBER, &tmp, 1, REG_SECONDS)) != STD_RET_OK)
                        goto DS1307_Init_Error;
            }

            /* operation success */
            kprintOK();
            goto DS1307_Init_ClosePort;

            /* error occur */
            DS1307_Init_Error:
            kprintErrorNo(status);

            /* close port */
            DS1307_Init_ClosePort:
            I2C_Close(I2C_NUMBER);
      }
      else
      {
            kprintFail();
      }

      return status;
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

      return STD_RET_OK;
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
      (void)src;
      (void)size;
      (void)seek;

      stdRet_t status = STD_RET_ERROR;

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
      (void)dst;
      (void)size;
      (void)seek;

      stdRet_t status = STD_RET_ERROR;

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
      (void) dev;

      stdRet_t status = STD_RET_ERROR;

      switch (ioRQ)
      {

      }

      return status;
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





////================================================================================================//
///**
// * @brief Return current time
// *
// * If driver is free function read data directly from RTC, otherwise sends last read value.
// *
// * @return current time
// **/
////================================================================================================//
//bcdTime_t DS1307_GetTime(void)
//{
//      u8_t tmp[3];
//
//      /* try to open port */
//      if (I2C_Open(I2C_NUMBER) == STD_RET_OK)
//      {
//            /* set DS1307 address */
//            tmp[0] = DS1307_ADDRESS;
//            if (I2C_IOCtl(I2C_NUMBER, I2C_IORQ_SETSLAVEADDR, &tmp[0]) != STD_RET_OK)
//                  goto DS1307_GetTime_ClosePort;
//
//            /* load time */
//            if (I2C_Read(I2C_NUMBER, &tmp, sizeof(tmp), REG_SECONDS) != STD_RET_OK)
//                  goto DS1307_GetTime_ClosePort;
//
//            currentTime.hours   = tmp[2];
//            currentTime.minutes = tmp[1];
//            currentTime.seconds = tmp[0];
//
//            /* close port */
//            DS1307_GetTime_ClosePort:
//            I2C_Close(I2C_NUMBER);
//      }
//
//      return currentTime;
//}
//
//
////================================================================================================//
///**
// * @brief Write new time to RTC
// *
// * @param time  new time
// *
// * @retval STD_RET_OK         success
// * @retval STD_RET_ERROR      failure
// **/
////================================================================================================//
//stdRet_t DS1307_SetTime(bcdTime_t time)
//{
//      stdRet_t status = STD_RET_ERROR;
//      u8_t     tmp[3];
//
//      /* try open port */
//      if (I2C_Open(I2C_NUMBER) == STD_RET_OK)
//      {
//            /* set DS1307 address */
//            tmp[0] = DS1307_ADDRESS;
//            if (I2C_IOCtl(I2C_NUMBER, I2C_IORQ_SETSLAVEADDR, &tmp[0]) != STD_RET_OK)
//                  goto DS1307_SetTime_ClosePort;
//
//            /* set new time */
//            tmp[0] = time.seconds;
//            tmp[1] = time.minutes;
//            tmp[2] = time.hours;
//
//            if (I2C_Write(I2C_NUMBER, &tmp, sizeof(tmp), REG_SECONDS) != STD_RET_OK)
//                  goto DS1307_SetTime_ClosePort;
//
//            currentTime = time;
//
//            status = STD_RET_OK;
//
//            /* close port */
//            DS1307_SetTime_ClosePort:
//            I2C_Close(I2C_NUMBER);
//      }
//
//      return status;
//}
//
//
////================================================================================================//
///**
// * @brief Return current date
// *
// * If driver is free function read data directly from RTC, otherwise sends last read value.
// *
// * @return current time
// **/
////================================================================================================//
//bcdDate_t DS1307_GetDate(void)
//{
//      u8_t tmp[4];
//
//      /* try to open port */
//      if (I2C_Open(I2C_NUMBER) == STD_RET_OK)
//      {
//            /* set DS1307 address */
//            tmp[0] = DS1307_ADDRESS;
//            if (I2C_IOCtl(I2C_NUMBER, I2C_IORQ_SETSLAVEADDR, &tmp[0]) != STD_RET_OK)
//                  goto DS1307_GetDate_ClosePort;
//
//            /* load date */
//            if (I2C_Read(I2C_NUMBER, &tmp, sizeof(tmp), REG_DAY) != STD_RET_OK)
//                  goto DS1307_GetDate_ClosePort;
//
//            currentDate.weekday = tmp[0];
//            currentDate.day     = tmp[1];
//            currentDate.month   = tmp[2];
//            currentDate.year    = tmp[3];
//
//            /* close port */
//            DS1307_GetDate_ClosePort:
//            I2C_Close(I2C_NUMBER);
//      }
//
//      return currentDate;
//}
//
//
////================================================================================================//
///**
// * @brief Write new date to RTC
// *
// * @param date  new date
// *
// * @retval STD_RET_OK         success
// * @retval STD_RET_ERROR      failure
// **/
////================================================================================================//
//stdRet_t DS1307_SetDate(bcdDate_t date)
//{
//      stdRet_t status = STD_RET_ERROR;
//      u8_t     tmp[4];
//
//      /* try open port */
//      if (I2C_Open(I2C_NUMBER) == STD_RET_OK)
//      {
//            /* set DS1307 address */
//            tmp[0] = DS1307_ADDRESS;
//            if (I2C_IOCtl(I2C_NUMBER, I2C_IORQ_SETSLAVEADDR, &tmp[0]) != STD_RET_OK)
//                  goto DS1307_SetDate_ClosePort;
//
//            /* set new date */
//            tmp[0] = WeekDay(BCD2BYTE(date.year ) + 2000,
//                             BCD2BYTE(date.month),
//                             BCD2BYTE(date.day  ) );
//            tmp[1] = date.day;
//            tmp[2] = date.month;
//            tmp[3] = date.year;
//
//            if (I2C_Write(I2C_NUMBER, &tmp, sizeof(tmp), REG_DAY) != STD_RET_OK)
//                  goto DS1307_SetDate_ClosePort;
//
//            currentDate = date;
//
//            status = STD_RET_OK;
//
//            /* close port */
//            DS1307_SetDate_ClosePort:
//            I2C_Close(I2C_NUMBER);
//      }
//
//      return status;
//}


//================================================================================================//
/**
 * @brief Function set day of week based on year, month and day
 *
 * @param year
 * @param month
 * @param day
 *
 * @return week day
 */
//================================================================================================//
static u8_t WeekDay(u16_t year, u8_t month, u8_t day)
{
      u16_t z, c;

      if (month < 3)
      {
            z = year--;
            c = 0;
      }
      else
      {
            z = year;
            c = 2;
      }

      /* compute week day */
      u16_t w = ((23 * month) / 9) + day + 4 + year + (z / 4) - (z / 100) + (z / 400) - c;

      return (w % 7) + 1;
}


#ifdef __cplusplus
}
#endif

/*==================================================================================================
 End of file
 ==================================================================================================*/
