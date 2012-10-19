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
#include "i2c_def.h"


/*==================================================================================================
 Local symbolic constants/macros
==================================================================================================*/
#define I2CFILE                     "/dev/i2c"
#define BCD2BYTE(bcd)               (((bcd >> 4) * 10) + (bcd & 0x0F))


/*==================================================================================================
 Local types, enums definitions
==================================================================================================*/


/*==================================================================================================
 Local function prototypes
==================================================================================================*/
static u8_t      WeekDay(u16_t year, u8_t month, u8_t day);
static bcdTime_t GetTime(void);
static stdRet_t  SetTime(bcdTime_t *time);
static bcdDate_t GetDate(void);
static stdRet_t  SetDate(bcdDate_t *date);


/*==================================================================================================
 Local object definitions
==================================================================================================*/
struct rtc_struct
{
      bcdTime_t time;
      bcdDate_t date;
} *rtc;


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

      rtc = Calloc(1, sizeof(struct rtc_struct));

      if (rtc)
      {
            /* try to open port */
            if ((i2c = fopen(I2CFILE, NULL)) != NULL)
            {
                  /* set DS1307 address */
                  tmp = DS1307_ADDRESS;
                  if (ioctl(i2c, I2C_IORQ_SETSLAVEADDR, &tmp) != STD_RET_OK)
                        goto DS1307_Init_Error;

                  /* read second register */
                  fseek(i2c, REG_SECONDS, 0);
                  if (fread(&tmp, sizeof(tmp), 1, i2c) != 1)
                        goto DS1307_Init_Error;

                  /* enable oscillator if disabled */
                  if (tmp & DS1307_REG_SECONDS_CH_BM)
                  {
                        tmp = 0x00;
                        fseek(i2c, REG_SECONDS, 0);
                        if (fwrite(&tmp, sizeof(tmp), 1, i2c) != 1)
                              goto DS1307_Init_Error;
                  }

                  /* operation success */
                  kprintOK();
                  status = STD_RET_OK;
                  goto DS1307_Init_ClosePort;

                  /* error occur */
                  DS1307_Init_Error:
                  kprintFail();

                  /* close port */
                  DS1307_Init_ClosePort:
                  fclose(i2c);
            }
            else
            {
                  Free(rtc);
                  kprintFail();
            }
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
 * @retval number of written nitems
 */
//================================================================================================//
size_t DS1307RTC_Write(dev_t dev, void *src, size_t size, size_t nitems, size_t seek)
{
      (void)dev;
      (void)src;
      (void)size;
      (void)seek;
      (void)nitems;

      return 0;
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
 * @retval number of written nitems
 */
//================================================================================================//
size_t DS1307RTC_Read(dev_t dev, void *dst, size_t size, size_t nitems, size_t seek)
{
      (void)dev;
      (void)dst;
      (void)size;
      (void)seek;
      (void)nitems;

      return 0;
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

      if (rtc != NULL)
      {
            switch (ioRQ)
            {
                  case RTC_IORQ_GETTIME:
                  {
                        bcdTime_t *time = (bcdTime_t*)data;
                        *time  = GetTime();
                        status = STD_RET_OK;
                        break;
                  }

                  case RTC_IORQ_SETTIME:
                  {
                        status = SetTime(data);
                        break;
                  }

                  case RTC_IORQ_GETDATE:
                  {
                        bcdDate_t *date = (bcdDate_t*)data;
                        *date = GetDate();
                        status = STD_RET_OK;
                        break;
                  }

                  case RTC_IORQ_SETDATE:
                  {
                        status = SetDate(data);
                        break;
                  }

                  default:
                        break;
            }
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

      if (rtc != NULL)
      {
            Free(rtc);
      }

      return STD_RET_OK;
}


//================================================================================================//
/**
 * @brief Return current time
 *
 * If driver is free function read data directly from RTC, otherwise sends last read value.
 *
 * @return current time
 **/
//================================================================================================//
static bcdTime_t GetTime(void)
{
      u8_t tmp[3];
      FILE_t *i2c;

      /* try to open port */
      if ((i2c = fopen(I2CFILE, NULL)) != NULL)
      {
            /* set DS1307 address */
            tmp[0] = DS1307_ADDRESS;
            if (ioctl(i2c, I2C_IORQ_SETSLAVEADDR, &tmp[0]) != STD_RET_OK)
                  goto GetTime_ClosePort;

            /* load time */
            fseek(i2c, REG_SECONDS, 0);

            if (fread(&tmp, sizeof(u8_t), ARRAY_SIZE(tmp), i2c) == ARRAY_SIZE(tmp))
            {
                  rtc->time.hours   = tmp[2];
                  rtc->time.minutes = tmp[1];
                  rtc->time.seconds = tmp[0];
            }

            /* close port */
            GetTime_ClosePort:
            fclose(i2c);
      }

      return rtc->time;
}


//================================================================================================//
/**
 * @brief Write new time to RTC
 *
 * @param time  new time
 *
 * @retval STD_RET_OK         success
 * @retval STD_RET_ERROR      failure
 **/
//================================================================================================//
static stdRet_t SetTime(bcdTime_t *time)
{
      stdRet_t status = STD_RET_ERROR;
      u8_t     tmp[3];
      FILE_t   *i2c;

      /* try open port */
      if ((i2c = fopen(I2CFILE, NULL)) != NULL)
      {
            /* set DS1307 address */
            tmp[0] = DS1307_ADDRESS;
            if (ioctl(i2c, I2C_IORQ_SETSLAVEADDR, &tmp[0]) != STD_RET_OK)
                  goto SetTime_ClosePort;

            /* set new time */
            tmp[0] = time->seconds;
            tmp[1] = time->minutes;
            tmp[2] = time->hours;

            fseek(i2c, REG_SECONDS, 0);

            if (fwrite(tmp, sizeof(u8_t), ARRAY_SIZE(tmp), i2c) == ARRAY_SIZE(tmp))
            {
                  rtc->time = *time;

                  status = STD_RET_OK;
            }

            /* close port */
            SetTime_ClosePort:
            fclose(i2c);
      }

      return status;
}


//================================================================================================//
/**
 * @brief Return current date
 *
 * If driver is free function read data directly from RTC, otherwise sends last read value.
 *
 * @return current time
 **/
//================================================================================================//
static bcdDate_t GetDate(void)
{
      u8_t tmp[4];
      FILE_t *i2c;

      /* try to open port */
      if ((i2c = fopen(I2CFILE, NULL)) != NULL)
      {
            /* set DS1307 address */
            tmp[0] = DS1307_ADDRESS;
            if (ioctl(i2c, I2C_IORQ_SETSLAVEADDR, &tmp[0]) != STD_RET_OK)
                  goto GetDate_ClosePort;

            /* load date */
            fseek(i2c, REG_DAY, 0);

            if (fread(&tmp, sizeof(u8_t), ARRAY_SIZE(tmp), i2c) == ARRAY_SIZE(tmp))
            {
                  rtc->date.weekday = tmp[0];
                  rtc->date.day     = tmp[1];
                  rtc->date.month   = tmp[2];
                  rtc->date.year    = tmp[3];
            }

            /* close port */
            GetDate_ClosePort:
            fclose(i2c);
      }

      return rtc->date;
}


//================================================================================================//
/**
 * @brief Write new date to RTC
 *
 * @param *date  new date
 *
 * @retval STD_RET_OK         success
 * @retval STD_RET_ERROR      failure
 **/
//================================================================================================//
static stdRet_t SetDate(bcdDate_t *date)
{
      stdRet_t status = STD_RET_ERROR;
      u8_t     tmp[4];
      FILE_t   *i2c;

      /* try open port */
      if ((i2c = fopen(I2CFILE, NULL)) != NULL)
      {
            /* set DS1307 address */
            tmp[0] = DS1307_ADDRESS;
            if (ioctl(i2c, I2C_IORQ_SETSLAVEADDR, &tmp[0]) != STD_RET_OK)
                  goto SetDate_ClosePort;

            /* set new date */
            tmp[0] = WeekDay(BCD2BYTE(date->year ) + 2000,
                             BCD2BYTE(date->month),
                             BCD2BYTE(date->day  ) );
            tmp[1] = date->day;
            tmp[2] = date->month;
            tmp[3] = date->year;

            fseek(i2c, REG_DAY, 0);
            if (fwrite(&tmp, sizeof(u8_t), ARRAY_SIZE(tmp), i2c) == ARRAY_SIZE(tmp))
            {
                  rtc->date = *date;

                  status = STD_RET_OK;
            }

            /* close port */
            SetDate_ClosePort:
            fclose(i2c);
      }

      return status;
}


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
