#ifndef DS1307_H_
#define DS1307_H_
/*=============================================================================================*//**
@file    ds1307.h

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
extern "C" {
#endif

/*==================================================================================================
                                            Include files
==================================================================================================*/


/*==================================================================================================
                                  Exported symbolic constants/macros
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
                                  Exported types, enums definitions
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
                                     Exported object declarations
==================================================================================================*/


/*==================================================================================================
                                     Exported function prototypes
==================================================================================================*/


#ifdef __cplusplus
}
#endif

#endif /* DS1307_H_ */
/*==================================================================================================
                                             End of file
==================================================================================================*/
