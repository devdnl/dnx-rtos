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
#include "i2c.h"
#include "utils.h"


/*==================================================================================================
 Local symbolic constants/macros
 ==================================================================================================*/
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
 * @brief Initialize DS1307
 *
 * If bit 7 (CH) is set necessary is clear CH bit to enable oscillator
 **/
//================================================================================================//
stdRet_t DS1307_Init(void)
{
      u8_t     tmp;
      stdRet_t status;

      kprint("Initializing RTC... ");

      /* try to open port */
      if ((status = I2C_Open(I2C_NUMBER)) == STD_RET_OK)
      {
            /* set DS1307 address */
            tmp = DS1307_ADDRESS;
            if ((status = I2C_IOCtl(I2C_NUMBER, I2C_IORQ_SETSLAVEADDR, &tmp)) != STD_RET_OK)
                  goto DS1307_Init_Error;

            /* read second register */
            if ((status = I2C_Read(I2C_NUMBER, &tmp, 1, REG_SECONDS)) != STD_RET_OK)
                  goto DS1307_Init_Error;

            /* enable oscillator if disabled */
            if (tmp & DS1307_REG_SECONDS_CH_BM)
            {
                  tmp = 0x00;
                  if ((status = I2C_Write(I2C_NUMBER, &tmp, 1, REG_SECONDS)) != STD_RET_OK)
                        goto DS1307_Init_Error;
            }

            /* operation success */
            fontGreen(k);
            kprint("SUCCESS\n");
            goto DS1307_Init_ClosePort;

            /* error occur */
            DS1307_Init_Error:
            fontRed(k);
            kprint("FAILURE (%d)\n", status);

            /* close port */
            DS1307_Init_ClosePort:
            I2C_Close(I2C_NUMBER);
      }
      else
      {
            fontRed(k);
            kprint("FAILURE (%d)\n", status);
      }

      resetAttr(k);

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
bcdTime_t DS1307_GetTime(void)
{
      u8_t tmp[3];

      /* try to open port */
      if (I2C_Open(I2C_NUMBER) == STD_RET_OK)
      {
            /* set DS1307 address */
            tmp[0] = DS1307_ADDRESS;
            if (I2C_IOCtl(I2C_NUMBER, I2C_IORQ_SETSLAVEADDR, &tmp[0]) != STD_RET_OK)
                  goto DS1307_GetTime_ClosePort;

            /* load time */
            if (I2C_Read(I2C_NUMBER, &tmp, sizeof(tmp), REG_SECONDS) != STD_RET_OK)
                  goto DS1307_GetTime_ClosePort;

            currentTime.hours   = tmp[2];
            currentTime.minutes = tmp[1];
            currentTime.seconds = tmp[0];

            /* close port */
            DS1307_GetTime_ClosePort:
            I2C_Close(I2C_NUMBER);
      }

      return currentTime;
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
stdRet_t DS1307_SetTime(bcdTime_t time)
{
      stdRet_t status = STD_RET_ERROR;
      u8_t     tmp[3];

      /* try open port */
      if (I2C_Open(I2C_NUMBER) == STD_RET_OK)
      {
            /* set DS1307 address */
            tmp[0] = DS1307_ADDRESS;
            if (I2C_IOCtl(I2C_NUMBER, I2C_IORQ_SETSLAVEADDR, &tmp[0]) != STD_RET_OK)
                  goto DS1307_SetTime_ClosePort;

            /* set new time */
            tmp[0] = time.seconds;
            tmp[1] = time.minutes;
            tmp[2] = time.hours;

            if (I2C_Write(I2C_NUMBER, &tmp, sizeof(tmp), REG_SECONDS) != STD_RET_OK)
                  goto DS1307_SetTime_ClosePort;

            currentTime = time;

            status = STD_RET_OK;

            /* close port */
            DS1307_SetTime_ClosePort:
            I2C_Close(I2C_NUMBER);
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
bcdDate_t DS1307_GetDate(void)
{
      u8_t tmp[4];

      /* try to open port */
      if (I2C_Open(I2C_NUMBER) == STD_RET_OK)
      {
            /* set DS1307 address */
            tmp[0] = DS1307_ADDRESS;
            if (I2C_IOCtl(I2C_NUMBER, I2C_IORQ_SETSLAVEADDR, &tmp[0]) != STD_RET_OK)
                  goto DS1307_GetDate_ClosePort;

            /* load date */
            if (I2C_Read(I2C_NUMBER, &tmp, sizeof(tmp), REG_DAY) != STD_RET_OK)
                  goto DS1307_GetDate_ClosePort;

            currentDate.weekday = tmp[0];
            currentDate.day     = tmp[1];
            currentDate.month   = tmp[2];
            currentDate.year    = tmp[3];

            /* close port */
            DS1307_GetDate_ClosePort:
            I2C_Close(I2C_NUMBER);
      }

      return currentDate;
}


//================================================================================================//
/**
 * @brief Write new date to RTC
 *
 * @param date  new date
 *
 * @retval STD_RET_OK         success
 * @retval STD_RET_ERROR      failure
 **/
//================================================================================================//
stdRet_t DS1307_SetDate(bcdDate_t date)
{
      stdRet_t status = STD_RET_ERROR;
      u8_t     tmp[4];

      /* try open port */
      if (I2C_Open(I2C_NUMBER) == STD_RET_OK)
      {
            /* set DS1307 address */
            tmp[0] = DS1307_ADDRESS;
            if (I2C_IOCtl(I2C_NUMBER, I2C_IORQ_SETSLAVEADDR, &tmp[0]) != STD_RET_OK)
                  goto DS1307_SetDate_ClosePort;

            /* set new date */
            tmp[0] = WeekDay(UTL_BCD2Byte(date.year ) + 2000,
                             UTL_BCD2Byte(date.month),
                             UTL_BCD2Byte(date.day  ) );
            tmp[1] = date.day;
            tmp[2] = date.month;
            tmp[3] = date.year;

            if (I2C_Write(I2C_NUMBER, &tmp, sizeof(tmp), REG_DAY) != STD_RET_OK)
                  goto DS1307_SetDate_ClosePort;

            currentDate = date;

            status = STD_RET_OK;

            /* close port */
            DS1307_SetDate_ClosePort:
            I2C_Close(I2C_NUMBER);
      }

      return status;
}


//================================================================================================//
/**
 * @brief Read data from DS1307's RAM
 *
 * @param *dst          data destination address
 * @param size          size of data
 * @param seek          seek
 *
 * @retval STD_RET_OK         success
 * @retval STD_RET_ERROR      failure
 **/
//================================================================================================//
stdRet_t DS1307_ReadRAM(u8_t *dst, u8_t size, u8_t seek)
{
      stdRet_t status = STD_RET_ERROR;
      u8_t     tmp;

      if (dst && size)
      {
            /* try open port */
            if (I2C_Open(I2C_NUMBER) == STD_RET_OK)
            {
                  /* set DS1307 address */
                  tmp = DS1307_ADDRESS;
                  if (I2C_IOCtl(I2C_NUMBER, I2C_IORQ_SETSLAVEADDR, &tmp) != STD_RET_OK)
                        goto DS1307_ReadRAM_ClosePort;

                  /* read data from RAM */
                  if (I2C_Read(I2C_NUMBER, dst, size, REG_RAM_BEGIN + seek) != STD_RET_OK)
                        goto DS1307_ReadRAM_ClosePort;

                  status = STD_RET_OK;

                  /* close port */
                  DS1307_ReadRAM_ClosePort:
                  I2C_Close(I2C_NUMBER);
            }
      }

      return status;
}

//================================================================================================//
/**
 * @brief Write data into DS1307's RAM
 *
 * @param *src          data source address
 * @param size          size of data
 * @param seek          seek
 *
 * @retval STD_RET_OK         success
 * @retval STD_RET_ERROR      failure
 **/
//================================================================================================//
stdRet_t DS1307_WriteRAM(u8_t *src, u8_t size, u8_t seek)
{
      stdRet_t status = STD_RET_ERROR;
      u8_t     tmp;

      if (src && size)
      {
            /* try open port */
            if (I2C_Open(I2C_NUMBER) == STD_RET_OK)
            {
                  /* set DS1307 address */
                  tmp = DS1307_ADDRESS;
                  if (I2C_IOCtl(I2C_NUMBER, I2C_IORQ_SETSLAVEADDR, &tmp) != STD_RET_OK)
                        goto DS1307_WriteRAM_ClosePort;

                  /* write data into RAM */
                  if (I2C_Write(I2C_NUMBER, src, size, REG_RAM_BEGIN + seek) != STD_RET_OK)
                        goto DS1307_WriteRAM_ClosePort;

                  status = STD_RET_OK;

                  /* close port */
                  DS1307_WriteRAM_ClosePort:
                  I2C_Close(I2C_NUMBER);
            }
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
