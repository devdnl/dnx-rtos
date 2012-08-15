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

/*==================================================================================================
                                            Include files
==================================================================================================*/
#include "MPL115A2.h"
#include "i2c.h"


/*==================================================================================================
                                  Local symbolic constants/macros
==================================================================================================*/
#define MPL115A2_ADDRESS            (0xC0 >> 1)
#define I2C_NUMBER                  I2C_DEV_1


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
      REG_CONVERT  = 0x12,
};


/*==================================================================================================
                                      Local function prototypes
==================================================================================================*/


/*==================================================================================================
                                      Local object definitions
==================================================================================================*/
/* coefficient values readed from device */
u16_t a0;
u16_t b1;
u16_t b2;
u16_t c12;


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
      stdRet_t status;
      u8_t     tmp[8];

      kprint("Initializing MPL115A2... ");

      /* try to open port */
      if ((status = I2C_Open(I2C_NUMBER)) == STD_RET_OK)
      {
            /* set DS1307 address */
            tmp[0] = MPL115A2_ADDRESS;
            if ((status = I2C_IOCtl(I2C_NUMBER, I2C_IORQ_SETSLAVEADDR, &tmp[0])) != STD_RET_OK)
                  goto MPL115A2_Init_Error;

            /* read coefficient values */
            if ((status = I2C_Read(I2C_NUMBER, &tmp, 8, REG_A0_MSB)) != STD_RET_OK)
                  goto MPL115A2_Init_Error;

            /* operation success */
            fontGreen(k);
            kprint("SUCCESS\n");
            goto MPL115A2_Init_ClosePort;

            /* error occur */
            MPL115A2_Init_Error:
            fontRed(k);
            kprint("FAILURE (%d)\n", status);

            /* close port */
            MPL115A2_Init_ClosePort:
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
 * @brief Get temperature
 *
 * @param[out] *temperature
 *
 * @retval STD_RET_OK         read success
 * @retval STD_RET_ERROR      error occur
 */
//================================================================================================//
stdRet_t MPL115A2_GetTemperature(u16_t *temperature)
{
      return STD_RET_ERROR;
}


//================================================================================================//
/**
 * @brief
 */
//================================================================================================//
stdRet_t MPL115A2_GetPressure(u16_t *pressure)
{
      return STD_RET_ERROR;
}

#ifdef __cplusplus
}
#endif
#ifdef __cplusplus
}
#endif

/*==================================================================================================
                                            End of file
==================================================================================================*/
