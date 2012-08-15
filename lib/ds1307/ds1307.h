#ifndef DS1307_H_
#define DS1307_H_
/*=============================================================================================*//**
@file    ds1307.h

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
extern "C" {
#endif

/*==================================================================================================
                                            Include files
==================================================================================================*/
#include "system.h"


/*==================================================================================================
                                  Exported symbolic constants/macros
==================================================================================================*/
/** define DS1307 RAM size */
#define DS1307_RAM_SIZE             (REG_RAM_END - REG_RAM_BEGIN)


/*==================================================================================================
                                  Exported types, enums definitions
==================================================================================================*/
/** time structure */
typedef struct time_struct
{
      u8_t seconds;     /**< [BCD] */
      u8_t minutes;     /**< [BCD] */
      u8_t hours;       /**< [BCD] */
} bcdTime_t;

/** date structure */
typedef struct date_struct
{
      u8_t weekday;     /**< [BCD] */
      u8_t day;         /**< [BCD] */
      u8_t month;       /**< [BCD] */
      u8_t year;        /**< [BCD] */
} bcdDate_t;


/*==================================================================================================
                                     Exported object declarations
==================================================================================================*/


/*==================================================================================================
                                     Exported function prototypes
==================================================================================================*/
extern void      DS1307_Init(void);
extern bcdTime_t DS1307_GetTime(void);
extern stdRet_t  DS1307_SetTime(bcdTime_t time);
extern bcdDate_t DS1307_GetDate(void);
extern stdRet_t  DS1307_SetDate(bcdDate_t date);
extern stdRet_t  DS1307_ReadRAM(u8_t *dst, u8_t size, u8_t seek);
extern stdRet_t  DS1307_WriteRAM(u8_t *src, u8_t size, u8_t seek);

#ifdef __cplusplus
}
#endif

#endif /* DS1307_H_ */
/*==================================================================================================
                                             End of file
==================================================================================================*/
