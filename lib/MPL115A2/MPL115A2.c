/*=============================================================================================*//**
@file    MPL115A2.c

@author  Daniel Zorychta

@brief   This file support temperature and pressure sensor - MPL115A2

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

/* DNLFIXME fix: this module must be defined as driver */

/*==================================================================================================
                                            Include files
==================================================================================================*/
#include "MPL115A2.h"
#include "i2c.h"


/*==================================================================================================
                                  Local symbolic constants/macros
==================================================================================================*/
#define MPL115A2_ADDRESS            0x60
#define I2CFILE                     "/dev/i2c"

/** multiplier used to remove fraction */
#define MULTIPLIER                  100

/** ADC temperature value @ 25C multiplied by 100 (605.75 * 100) */
#define ADC25C                      60575

/** degree per ADC count multiplied by 100 (-5.35 * 100) */
#define TEMP_A_FACTOR               (i16_t)-535


/*==================================================================================================
                                   Local types, enums definitions
==================================================================================================*/
/** enum with registers names */
enum registers_enum
{
      REG_PADC_MSB = 0x00,
      REG_PADC_LSB = 0x01,
      REG_TADC_MSB = 0x02,
      REG_TADC_LSB = 0x03,
      REG_A0_MSB   = 0x04,
      REG_A0_LSB   = 0x05,
      REG_B1_MSB   = 0x06,
      REG_B1_LSB   = 0x07,
      REG_B2_MSB   = 0x08,
      REG_B2_LSB   = 0x09,
      REG_C12_MSB  = 0x0A,
      REG_C12_LSB  = 0x0B,
      REG_C11_MSB  = 0x0C,
      REG_C11_LSB  = 0x0D,
      REG_C22_MSB  = 0x0E,
      REG_C22_LSB  = 0x0F,
      REG_CONVERT  = 0x12,
};


/*==================================================================================================
                                      Local function prototypes
==================================================================================================*/


/*==================================================================================================
                                      Local object definitions
==================================================================================================*/
/* coefficient values received from device */
i16_t a0;
i16_t b1;
i16_t b2;
i16_t c12;


/*==================================================================================================
                                     Exported object definitions
==================================================================================================*/


/*==================================================================================================
                                        Function definitions
==================================================================================================*/

//================================================================================================//
/**
 * @brief Initialize temperature and pressure sensor
 */
//================================================================================================//
stdRet_t MPL115A2_Init(void)
{
      stdRet_t status = STD_RET_ERROR;
      u8_t     tmp[8];
      FILE_t   *i2c;

      kprint("Initializing MPL115A2...");

      /* try to open port */
      if ((i2c = fopen(I2CFILE, NULL)) != NULL)
      {
            /* set MPL115A2 address */
            tmp[0] = MPL115A2_ADDRESS;
            if ((status = ioctl(i2c, I2C_IORQ_SETSLAVEADDR, &tmp[0])) != STD_RET_OK)
                  goto MPL115A2_Init_Error;

            /* read coefficient values */
            fseek(i2c, REG_A0_MSB, 0);
            if (fread(&tmp, sizeof(u8_t), ARRAY_SIZE(tmp), i2c) != ARRAY_SIZE(tmp))
                  goto MPL115A2_Init_Error;

            /* parse received data */
            a0  = (tmp[0] << 8) | tmp[1];
            b1  = (tmp[2] << 8) | tmp[3];
            b2  = (tmp[4] << 8) | tmp[5];
            c12 = (tmp[6] << 8) | tmp[7];

            /* operation success */
            kprintOK();
            goto MPL115A2_Init_ClosePort;

            /* error occur */
            MPL115A2_Init_Error:
            kprintErrorNo(status);

            /* close port */
            MPL115A2_Init_ClosePort:
            fclose(i2c);
      }
      else
      {
            kprintErrorNo(status);
      }

      return status;
}


//================================================================================================//
/**
 * @brief Get temperature
 *
 * @param[out] *temperature   range: -128..+127^C; resolution: 1^C
 *
 * @retval STD_RET_OK         read success
 * @retval STD_RET_ERROR      error occur
 */
//================================================================================================//
stdRet_t MPL115A2_GetTemperature(i8_t *temperature)
{
      stdRet_t status = STD_RET_ERROR;
      u8_t     tmp[2];
      u16_t    temp;
      FILE_t   *i2c;

      /* try to open port */
      if ((i2c = fopen(I2CFILE, NULL)) != NULL)
      {
            /* set MPL115A2 address */
            tmp[0] = MPL115A2_ADDRESS;
            if ((status = ioctl(i2c, I2C_IORQ_SETSLAVEADDR, &tmp[0])) != STD_RET_OK)
                  goto MPL115A2_GetTemperature_ClosePort;

            /* start new conversion */
            tmp[0] = 0x01;
            fseek(i2c, REG_CONVERT, 0);
            if (fwrite(&tmp, sizeof(u8_t), 1, i2c) != 1)
                  goto MPL115A2_GetTemperature_ClosePort;

            Sleep(5);

            /* load temperature */
            fseek(i2c, REG_TADC_MSB, 0);
            if (fread(&tmp, sizeof(u8_t), ARRAY_SIZE(tmp), i2c) != ARRAY_SIZE(tmp))
                  goto MPL115A2_GetTemperature_ClosePort;

            /* binds temperature */
            temp = (((tmp[0] << 8) | tmp[1]) >> 6);
            *temperature = (i8_t)(((i16_t)(temp * MULTIPLIER - ADC25C)) / TEMP_A_FACTOR);

            /* close port */
            MPL115A2_GetTemperature_ClosePort:
            fclose(i2c);
            status = STD_RET_OK;
      }

      return status;
}


//================================================================================================//
/**
 * @brief Get Pressure
 *
 * @param[out] *pressure      range: 500..1150hPa
 *
 * @retval STD_RET_OK         read success
 * @retval STD_RET_ERROR      error occur
 */
//================================================================================================//
stdRet_t MPL115A2_GetPressure(u16_t *pressure)
{
      stdRet_t status = STD_RET_ERROR;
      u8_t     tmp[4];
      FILE_t   *i2c;

      /* try to open port */
      if ((i2c = fopen(I2CFILE, NULL)) != NULL)
      {
            /* set MPL115A2 address */
            tmp[0] = MPL115A2_ADDRESS;
            if (ioctl(i2c, I2C_IORQ_SETSLAVEADDR, &tmp[0]) != STD_RET_OK)
                  goto MPL115A2_GetPressure_ClosePort;

            /* start new conversion */
            tmp[0] = 0x01;
            fseek(i2c, REG_CONVERT, 0);
            if (fwrite(&tmp, sizeof(u8_t), 1, i2c) != 1)
                  goto MPL115A2_GetPressure_ClosePort;

            Sleep(5);

            /* load temperature */
            fseek(i2c, REG_PADC_MSB, 0);
            if (fread(&tmp, sizeof(u8_t), ARRAY_SIZE(tmp), i2c) != ARRAY_SIZE(tmp))
                  goto MPL115A2_GetPressure_ClosePort;

            /* binds pressure */
            u16_t Padc = ((tmp[0] << 8) | tmp[1]) >> 6;

            /* binds temperature */
            u16_t Tadc = ((tmp[2] << 8) | tmp[3]) >> 6;

            /* compute pressure */
            i32_t si_c12x2 = c12 * Tadc;
            i32_t si_a1    = (((b1 << 11) + si_c12x2) >> 11);
            i32_t si_a2    = b2 >> 1;
            i32_t si_a1x1  = si_a1 * Padc;
            i32_t si_y1    = ((a0 << 10) + si_a1x1) >> 10;
            i32_t si_a2x2  = si_a2 * Tadc;
            i32_t siPcomp  = ((si_y1 << 10) + si_a2x2) >> 13;

            *pressure = (u16_t)((650 * siPcomp) / 1023) + 500;

            /* close port */
            MPL115A2_GetPressure_ClosePort:
            fclose(i2c);
            status = STD_RET_OK;
      }

      return status;
}

#ifdef __cplusplus
}
#endif

/*==================================================================================================
                                            End of file
==================================================================================================*/
