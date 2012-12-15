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

#define MTX_BLOCK_TIME              0

#define RELEASE_BLOCK_TIME          1000

/** convert BCD to byte */
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
enum ds1307_reg_enum {
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
static struct rtc_struct {
      mutex_t   mtx;
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
 * @param part    device part
 *
 * @retval STD_RET_OK                     operation success
 * @retval STD_RET_ERROR                  error
 */
//================================================================================================//
stdRet_t DS1307_Init(devx_t dev, fd_t part)
{
      (void)dev;
      (void)part;

      stdRet_t status = STD_RET_ERROR;

      if (rtc == NULL) {
            rtc = calloc(1, sizeof(struct rtc_struct));

            if (rtc) {
                  CreateMutex(rtc->mtx);

                  if (rtc->mtx) {
                        FILE_t *fi2c;

                        if ((fi2c = fopen(I2CFILE, "r+")) != NULL) {

                              u8_t tmp = DS1307_ADDRESS;

                              if (ioctl(fi2c, I2C_IORQ_SETSLAVEADDR, &tmp) != STD_RET_OK) {
                                    goto DS1307_Init_CloseFile;
                              }

                              /* read second register */
                              fseek(fi2c, REG_SECONDS, SEEK_SET);

                              if (fread(&tmp, sizeof(u8_t), 1, fi2c) != 1) {
                                    goto DS1307_Init_CloseFile;
                              }

                              /* enable oscillator if disabled */
                              if (tmp & DS1307_REG_SECONDS_CH_BM) {
                                    tmp = 0x00;

                                    fseek(fi2c, REG_SECONDS, SEEK_SET);

                                    if (fwrite(&tmp, sizeof(u8_t), 1, fi2c) != 1) {
                                          goto DS1307_Init_CloseFile;
                                    }
                              }

                              status = STD_RET_OK;

                              DS1307_Init_CloseFile:
                              fclose(fi2c);
                        }
                  }

                  if (status == STD_RET_ERROR) {
                        if (rtc->mtx)
                              DeleteMutex(rtc->mtx);

                        free(rtc);
                        rtc = NULL;
                  }
            }
      } else {
            status = STD_RET_OK;
      }

      return status;
}


//================================================================================================//
/**
 * @brief Release RTC devices
 *
 * @param dev     device number
 * @param part    device part
 *
 * @retval STD_RET_OK
 */
//================================================================================================//
stdRet_t DS1307_Release(devx_t dev, fd_t part)
{
      (void)dev;
      (void)part;

      stdRet_t status = STD_RET_ERROR;

      if (TakeMutex(rtc->mtx, RELEASE_BLOCK_TIME) == OS_OK) {
            GiveMutex(rtc->mtx);
            DeleteMutex(rtc->mtx);
            free(rtc);
            rtc = NULL;

            status = STD_RET_OK;
      }

      return status;
}


//================================================================================================//
/**
 * @brief Opens specified port and initialize default settings
 *
 * @param[in]  dev                        dev name (number)
 * @param[in]  part                       device part
 *
 * @retval STD_RET_OK                     operation success
 * @retval STD_RET_ERROR                  error
 */
//================================================================================================//
stdRet_t DS1307_Open(devx_t dev, fd_t part)
{
      (void)dev;
      (void)part;

      stdRet_t status = STD_RET_ERROR;

      if (rtc) {
            status = STD_RET_OK;
      }

      return status;
}


//================================================================================================//
/**
 * @brief Function close opened port
 *
 * @param[in]  dev                        dev name (number)
 * @param[in]  part                       device part
 *
 * @retval STD_RET_OK                     operation success
 * @retval STD_RET_ERROR                  error
 */
//================================================================================================//
stdRet_t DS1307_Close(devx_t dev, fd_t part)
{
      (void)dev;
      (void)part;

      return STD_RET_OK;
}


//================================================================================================//
/**
 * @brief Write data to UART (ISR or DMA)
 *
 * @param[in]  dev                        dev name (number)
 * @param[in]  part                       device part
 * @param[in]  *src                       source buffer
 * @param[in]  size                       item size
 * @param[in]  nitems                     number of items
 * @param[in]  seek                       seek
 *
 * @retval number of written nitems
 */
//================================================================================================//
size_t DS1307_Write(devx_t dev, fd_t part, void *src, size_t size, size_t nitems, size_t seek)
{
      (void)part;

      size_t  n = 0;
      u8_t    tmp;
      FILE_t *fi2c;

      if (dev == DS1307_DEV_NVM) {
            if ((fi2c = fopen(I2CFILE, "w")) != NULL) {
                  tmp = DS1307_ADDRESS;

                  if (ioctl(fi2c, I2C_IORQ_SETSLAVEADDR, &tmp) == STD_RET_OK) {

                        fseek(fi2c, REG_RAM_BEGIN + seek, SEEK_SET);
                        n = fwrite(src, size, nitems, fi2c);
                  }

                  fclose(fi2c);
            }
      }

      return n;
}


//================================================================================================//
/**
 * @brief Read data from UART Rx buffer
 *
 * @param[in]  dev                        RTC name (number)
 * @param[in]  part                       device part
 * @param[out] *dst                       destination buffer
 * @param[in]  size                       buffer size
 * @param[in]  seek                       seek
 *
 * @retval number of read nitems
 */
//================================================================================================//
size_t DS1307_Read(devx_t dev, fd_t part, void *dst, size_t size, size_t nitems, size_t seek)
{
      (void)part;

      size_t  n = 0;
      u8_t    tmp;
      FILE_t *fi2c;

      if (dev == DS1307_DEV_NVM) {
            if ((fi2c = fopen(I2CFILE, "r")) != NULL) {
                  tmp = DS1307_ADDRESS;

                  if (ioctl(fi2c, I2C_IORQ_SETSLAVEADDR, &tmp) == STD_RET_OK) {
                        fseek(fi2c, REG_RAM_BEGIN + seek, SEEK_SET);
                        n = fread(dst, size, nitems, fi2c);
                  }

                  fclose(fi2c);
            }
      }

      return n;
}


//================================================================================================//
/**
 * @brief Direct IO control
 *
 * @param[in]     dev                     RTC name (number)
 * @param[in]     part                    device part
 * @param[in,out] ioRQ                    IO request
 * @param[in,out] *data                   IO data (arguments, results, etc)
 *
 * @retval STD_RET_OK                     operation success
 * @retval STD_RET_ERROR                  error
 */
//================================================================================================//
stdRet_t DS1307_IOCtl(devx_t dev, fd_t part, IORq_t ioRQ, void *data)
{
      (void)part;

      stdRet_t status = STD_RET_ERROR;

      if (dev == DS1307_DEV_RTC) {

            switch (ioRQ) {
            case RTC_IORQ_GETTIME:
                  if (data) {
                        bcdTime_t *time = data;
                        *time  = GetTime();
                        status = STD_RET_OK;
                  }
                  break;

            case RTC_IORQ_SETTIME:
                  if (data) {
                        status = SetTime(data);
                  }
                  break;

            case RTC_IORQ_GETDATE:
                  if (data) {
                        bcdDate_t *date = (bcdDate_t*)data;
                        *date = GetDate();
                        status = STD_RET_OK;
                  }
                  break;

            case RTC_IORQ_SETDATE:
                  if (data) {
                        status = SetDate(data);
                  }
                  break;

            default:
                  break;
            }
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
      u8_t    tmp[3];
      FILE_t *fi2c;

      if (TakeMutex(rtc->mtx, MTX_BLOCK_TIME) == OS_OK) {
            if ((fi2c = fopen(I2CFILE, "r")) != NULL) {
                  tmp[0] = DS1307_ADDRESS;

                  if (ioctl(fi2c, I2C_IORQ_SETSLAVEADDR, &tmp[0]) == STD_RET_OK) {
                        fseek(fi2c, REG_SECONDS, SEEK_SET);

                        if (fread(&tmp, sizeof(u8_t), ARRAY_SIZE(tmp), fi2c) == ARRAY_SIZE(tmp)) {
                              rtc->time.hours   = tmp[2];
                              rtc->time.minutes = tmp[1];
                              rtc->time.seconds = tmp[0];
                        }
                  }

                  fclose(fi2c);
            }

            GiveMutex(rtc->mtx);
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
      stdRet_t  status = STD_RET_ERROR;
      u8_t      tmp[3];
      FILE_t   *fi2c;

      if (TakeMutex(rtc->mtx, MTX_BLOCK_TIME) == OS_OK) {
            if ((fi2c = fopen(I2CFILE, "w")) != NULL) {
                  tmp[0] = DS1307_ADDRESS;

                  if (ioctl(fi2c, I2C_IORQ_SETSLAVEADDR, &tmp[0]) == STD_RET_OK) {

                        tmp[0] = time->seconds;
                        tmp[1] = time->minutes;
                        tmp[2] = time->hours;

                        fseek(fi2c, REG_SECONDS, SEEK_SET);

                        if (fwrite(tmp, sizeof(u8_t), ARRAY_SIZE(tmp), fi2c) == ARRAY_SIZE(tmp)) {
                              rtc->time = *time;
                              status    = STD_RET_OK;
                        }
                  }

                  fclose(fi2c);
            }

            GiveMutex(rtc->mtx);
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
      u8_t    tmp[4];
      FILE_t *fi2c;

      if (TakeMutex(rtc->mtx, MTX_BLOCK_TIME) == OS_OK) {
            if ((fi2c = fopen(I2CFILE, "r")) != NULL) {
                  tmp[0] = DS1307_ADDRESS;

                  if (ioctl(fi2c, I2C_IORQ_SETSLAVEADDR, &tmp[0]) == STD_RET_OK) {
                        fseek(fi2c, REG_DAY, SEEK_SET);

                        if (fread(&tmp, sizeof(u8_t), ARRAY_SIZE(tmp), fi2c) == ARRAY_SIZE(tmp)) {
                              rtc->date.weekday = tmp[0];
                              rtc->date.day     = tmp[1];
                              rtc->date.month   = tmp[2];
                              rtc->date.year    = tmp[3];
                        }
                  }

                  fclose(fi2c);
            }

            GiveMutex(rtc->mtx);
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
      stdRet_t  status = STD_RET_ERROR;
      u8_t      tmp[4];
      FILE_t   *fi2c;

      if (TakeMutex(rtc->mtx, MTX_BLOCK_TIME) == OS_OK) {
            if ((fi2c = fopen(I2CFILE, "w")) != NULL) {
                  tmp[0] = DS1307_ADDRESS;

                  if (ioctl(fi2c, I2C_IORQ_SETSLAVEADDR, &tmp[0]) == STD_RET_OK) {
                        tmp[0] = WeekDay(BCD2BYTE(date->year ) + 2000,
                                         BCD2BYTE(date->month),
                                         BCD2BYTE(date->day  ) );
                        tmp[1] = date->day;
                        tmp[2] = date->month;
                        tmp[3] = date->year;

                        fseek(fi2c, REG_DAY, SEEK_SET);

                        if (fwrite(&tmp, sizeof(u8_t), ARRAY_SIZE(tmp), fi2c) == ARRAY_SIZE(tmp)) {
                              rtc->date = *date;
                              status    = STD_RET_OK;
                        }
                  }

                  fclose(fi2c);
            }

            GiveMutex(rtc->mtx);
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
