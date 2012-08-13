/*=============================================================================================*//**
@file    date.c

@author  Daniel Zorychta

@brief

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

/*==================================================================================================
                                            Include files
==================================================================================================*/
#include "date.h"
#include "i2c.h"
#include <string.h>

/* Begin of application section declaration */
APPLICATION(date)
APP_SEC_BEGIN

/*==================================================================================================
                                  Local symbolic constants/macros
==================================================================================================*/


/*==================================================================================================
                                   Local types, enums definitions
==================================================================================================*/


/*==================================================================================================
                                      Local object definitions
==================================================================================================*/


/*==================================================================================================
                                        Function definitions
==================================================================================================*/

//================================================================================================//
/**
 * @brief clear main function
 */
//================================================================================================//
stdRet_t appmain(ch_t *argv)
{
      (void) argv;

      stdRet_t status;
      u8_t  date[7];
      u8_t  tries = 5;
      u8_t  sa = 0x68;

      print("Test %d5, %d, 0x%x4\n", -10, 10, 0xDA);

      for (;;)
      {
            /* try to open port */
            print("[0] Opening port: ");

            if ((status = I2C_Open(I2C_DEV_1)) != STD_RET_OK)
            {
                  print("Failed (%d)\n", status);

                  Sleep(2000);

                  if (tries-- == 0)
                        return STD_RET_ERROR;

                  continue;
            }
            else
            {
                  print("Success\n");
            }

            break;
      }

      /* set slave address */
      print("[1] Setting slave address: ");
      if ((status = I2C_IOCtl(I2C_DEV_1, I2C_IORQ_SETSLAVEADDR, &sa)) != STD_RET_OK)
      {
            print("Failed (%d)\n", status);
            goto ClosePort;
      }
      else
      {
            print("Success\n");
      }

      /* read register to check that oscillator is enabled */
      print("[2] Check oscillator settings: ");
      if ((status = I2C_Read(I2C_DEV_1, date, 1, 0)) != STD_RET_OK)
      {
            print("Failed (%d)\n", status);
            goto ClosePort;
      }
      else
      {
            print("Success\n");
      }

      if (date[0] & (1<<7))
      {
            print("    Initializing RTC: ");

            memset(date, 0x00, sizeof(date));

            if ((status = I2C_Write(I2C_DEV_1, date, sizeof(date), 0)) != STD_RET_OK)
            {
                  print("Failed (%d)\n", status);
                  goto ClosePort;
            }
            else
            {
                  print("Success\n");
            }
      }

      /* read time */
      print("[3] Read date: ");
      if ((status = I2C_Read(I2C_DEV_1, date, sizeof(date), 0)) != STD_RET_OK)
      {
            print("Failed (%d)\n", status);
            goto ClosePort;
      }
      else
      {
            print("Success\n");
      }

      /* show time */
      print("%x2:%x2:%x2 day: %x %x2-%x2-20%x2\n", date[2], date[1], date[0],
            date[3], date[4], date[5], date[6]);

      ClosePort:
      I2C_Close(I2C_DEV_1);

      return STD_RET_OK;
}

/* End of application section declaration */
APP_SEC_END

/*==================================================================================================
                                            End of file
==================================================================================================*/
