#ifndef DS1307RTC_DEF_H_
#define DS1307RTC_DEF_H_
/*=============================================================================================*//**
@file    ds1307rtc_def.h

@author  Daniel Zorychta

@brief   This file support DS1307 especially RTC

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
#include "basic_types.h"


/*==================================================================================================
                                  Exported symbolic constants/macros
==================================================================================================*/


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


/** RTC requests */
enum RTC_IORQ_enum
{
      RTC_IORQ_GETTIME,                   /* out bcdTime_t */
      RTC_IORQ_SETTIME,                   /* in  bcdTime_t */
      RTC_IORQ_GETDATE,                   /* out bcdDate_t */
      RTC_IORQ_SETDATE,                   /* in  bcdDate_t */
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

#endif /* DS1307RTC_DEF_H_ */
/*==================================================================================================
                                             End of file
==================================================================================================*/
