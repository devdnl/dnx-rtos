/*=============================================================================================*//**
 @file    ds1307.c

 @author  Daniel Zorychta

 @brief   This file support DS1307

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
#include "i2c_def.h"


/*==================================================================================================
                                  Exported symbolic constants/macros
==================================================================================================*/
#define I2CFILE                     "/dev/i2c"
#define I2CFILEMODE                 "r+"
#define BCD2BYTE(bcd)               (((bcd >> 4) * 10) + (bcd & 0x0F))

/** DS1307 write address */
#define DS1307_ADDRESS              0x68

/** DS1307 I2C interface number */
#define I2C_NUMBER                  I2C_DEV_1

/** define CH bit */
#define DS1307_REG_SECONDS_CH_BP    7
#define DS1307_REG_SECONDS_CH_BM    (1 << DS1307_REG_SECONDS_CH_BP)

/** define PM bit */
#define DS1307_REG_HOURS_PM_BP      5
#define DS1307_REG_HOURS_PM_BM      (1 << DS1307_REG_HOURS_PM_BP)

/** define RS0 bit */
#define DS1307_REG_CTRL_RS0_BP      0
#define DS1307_REG_CTRL_RS0_BM      (1 << DS1307_REG_CTRL_RS0_BP)

/** define RS1 bit */
#define DS1307_REG_CTRL_RS1_BP      1
#define DS1307_REG_CTRL_RS1_BM      (1 << DS1307_REG_CTRL_RS1_BP)

/** define SQWE bit */
#define DS1307_REG_CTRL_SQWE_BP     4
#define DS1307_REG_CTRL_SQWE_BM     (1 << DS1307_REG_CTRL_SQWE_BP)

/** define OUT bit */
#define DS1307_REG_CTRL_OUT_BP      7
#define DS1307_REG_CTRL_OUT_BM      (1 << DS1307_REG_CTRL_OUT_BP)


/*==================================================================================================
                                  Local types, enums definitions
==================================================================================================*/
/** define registers addresses in DS1307 */
enum ds1307_reg_enum
{
      REG_SECONDS   = 0x00,
      REG_MINUTES   = 0x01,
      REG_HOURS     = 0x02,
      REG_DAY       = 0x03,
      REG_DATE      = 0x04,
      REG_MONTH     = 0x05,
      REG_YEAR      = 0x06,
      REG_CTRL      = 0x07,
      REG_RAM_BEGIN = 0x08,
      REG_RAM_END   = 0x40
};


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
/* structure used to control RTC and NVM */
static struct rtc_struct
{
      bcdTime_t time;
      bcdDate_t date;
      u32_t     lock;
      bool_t    nvmInit;
      bool_t    rtcInit;
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
stdRet_t DS1307_Init(nod_t dev)
{
      stdRet_t status = STD_RET_ERROR;

      u8_t   tmp;
      FILE_t *i2c;

      if (rtc == NULL)
      {
            rtc = calloc(1, sizeof(struct rtc_struct));
      }

      if (rtc)
      {
            if (dev == DS1307_DEV_RTC)
            {
                  /* try to open port */
                  if ((i2c = fopen(I2CFILE, I2CFILEMODE)) != NULL)
                  {
                        /* set DS1307 address */
                        tmp = DS1307_ADDRESS;
                        if (ioctl(i2c, I2C_IORQ_SETSLAVEADDR, &tmp) != STD_RET_OK)
                        {
                              goto DS1307_Init_CloseFile;
                        }

                        /* read second register */
                        fseek(i2c, REG_SECONDS, 0);

                        if (fread(&tmp, sizeof(u8_t), 1, i2c) != 1)
                        {
                              goto DS1307_Init_CloseFile;
                        }

                        /* enable oscillator if disabled */
                        if (tmp & DS1307_REG_SECONDS_CH_BM)
                        {
                              tmp = 0x00;
                              fseek(i2c, REG_SECONDS, 0);

                              if (fwrite(&tmp, sizeof(u8_t), 1, i2c) != 1)
                              {
                                    goto DS1307_Init_CloseFile;
                              }
                        }

                        /* operation success */
                        status = STD_RET_OK;
                        rtc->rtcInit = TRUE;

                        /* close port */
                        DS1307_Init_CloseFile:
                        fclose(i2c);
                  }
            }
            else if (dev == DS1307_DEV_NVM)
            {
                  rtc->nvmInit = TRUE;
                  rtc->lock = EMPTY_TASK;
                  status = STD_RET_OK;
            }
      }

      return status;
}


//================================================================================================//
/**
 * @brief Opens specified port and initialize default settings
 *
 * @param[in]  dev                        dev name (number)
 *
 * @retval STD_RET_OK                     operation success
 * @retval STD_RET_ERROR                  error
 */
//================================================================================================//
stdRet_t DS1307_Open(nod_t dev)
{
      stdRet_t status = STD_RET_ERROR;

      if (dev == DS1307_DEV_NVM)
      {
            TaskSuspendAll();

            if (rtc->lock == EMPTY_TASK)
            {
                  rtc->lock = TaskGetPID();
                  status = STD_RET_OK;
            }

            TaskResumeAll();
      }
      else if (dev == DS1307_DEV_RTC)
      {
            status = STD_RET_OK;
      }

      return status;
}


//================================================================================================//
/**
 * @brief Function close opened port
 *
 * @param[in]  dev                        dev name (number)
 *
 * @retval STD_RET_OK                     operation success
 * @retval STD_RET_ERROR                  error
 */
//================================================================================================//
stdRet_t DS1307_Close(nod_t dev)
{
      stdRet_t status = STD_RET_ERROR;

      if (dev == DS1307_DEV_NVM)
      {
            if (rtc->lock == (u32_t)TaskGetPID())
            {
                  rtc->lock = EMPTY_TASK;
                  status = STD_RET_OK;
            }
      }
      else if (dev == DS1307_DEV_RTC)
      {
            status = STD_RET_OK;
      }

      return status;
}


//================================================================================================//
/**
 * @brief Write data to UART (ISR or DMA)
 *
 * @param[in]  dev                        dev name (number)
 * @param[in]  *src                       source buffer
 * @param[in]  size                       item size
 * @param[in]  nitems                     number of items
 * @param[in]  seek                       seek
 *
 * @retval number of written nitems
 */
//================================================================================================//
size_t DS1307_Write(nod_t dev, void *src, size_t size, size_t nitems, size_t seek)
{
      size_t n = 0;
      u8_t   tmp;
      FILE_t *i2c;

      if (dev == DS1307_DEV_NVM)
      {
            if (rtc->lock == (u32_t)TaskGetPID())
            {
                  if (src && size && nitems)
                  {
                        /* try open port */
                        if ((i2c = fopen(I2CFILE, I2CFILEMODE)) != NULL)
                        {
                              /* set DS1307 address */
                              tmp = DS1307_ADDRESS;
                              if (ioctl(i2c, I2C_IORQ_SETSLAVEADDR, &tmp) != STD_RET_OK)
                                    goto WriteRAM_ClosePort;

                              /* write data into RAM */
                              fseek(i2c, REG_RAM_BEGIN + seek, 0);
                              n = fwrite(src, size, nitems, i2c);

                              /* close port */
                              WriteRAM_ClosePort:
                              fclose(i2c);
                        }
                  }
            }
      }

      return n;
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
 * @retval number of read nitems
 */
//================================================================================================//
size_t DS1307_Read(nod_t dev, void *dst, size_t size, size_t nitems, size_t seek)
{
      size_t n = 0;
      u8_t   tmp;
      FILE_t *i2c;

      if (dev == DS1307_DEV_NVM)
      {
            if (rtc->lock == (u32_t)TaskGetPID())
            {
                  if (dst && size && nitems)
                  {
                        /* try open port */
                        if ((i2c = fopen(I2CFILE, I2CFILEMODE)) != NULL)
                        {
                              /* set DS1307 address */
                              tmp = DS1307_ADDRESS;
                              if (ioctl(i2c, I2C_IORQ_SETSLAVEADDR, &tmp) != STD_RET_OK)
                                    goto ReadRAM_ClosePort;

                              /* read data from RAM */
                              fseek(i2c, REG_RAM_BEGIN + seek, 0);
                              n = fread(dst, size, nitems, i2c);

                              /* close port */
                              ReadRAM_ClosePort:
                              fclose(i2c);
                        }
                  }
            }
      }

      return n;
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
stdRet_t DS1307_IOCtl(nod_t dev, IORq_t ioRQ, void *data)
{
      stdRet_t status = STD_RET_ERROR;

      if (dev == DS1307_DEV_RTC)
      {
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
stdRet_t DS1307_Release(nod_t dev)
{
      stdRet_t status = STD_RET_ERROR;

      if (dev == DS1307_DEV_RTC)
      {
            rtc->nvmInit = FALSE;
            status = STD_RET_OK;
      }
      else if (dev == DS1307_DEV_NVM)
      {
            rtc->rtcInit = FALSE;
            status = STD_RET_OK;
      }

      if (!rtc->rtcInit && !rtc->nvmInit)
      {
            free(rtc);
            rtc = NULL;
      }

      return status;
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
      if ((i2c = fopen(I2CFILE, I2CFILEMODE)) != NULL)
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
      if ((i2c = fopen(I2CFILE, I2CFILEMODE)) != NULL)
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
      if ((i2c = fopen(I2CFILE, I2CFILEMODE)) != NULL)
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
      if ((i2c = fopen(I2CFILE, I2CFILEMODE)) != NULL)
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
