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

#ifdef __cplusplus
extern "C" {
#endif

/*==================================================================================================
                                            Include files
==================================================================================================*/
#include "date.h"
#include "ds1307.h"
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
      const ch_t *weekDay[] = {"MON", "TUE", "WED", "THU", "FRI", "SAT", "SUN"};
      const ch_t *months[]  = {"Jan", "Feb", "Mar", "Apr", "May", "Jun",
                               "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

      bcdTime_t  time;
      bcdDate_t  date;

      if ( (ParseArgsAs(argv, "help", PARSE_AS_EXIST, NULL) == STD_RET_OK)
         ||(ParseArgsAs(argv, "h",    PARSE_AS_EXIST, NULL) == STD_RET_OK) )
      {
            print("Syntax: %s [OPTION]...\n", DATE_NAME);
            print("Print actual time and date.\n");
            print("  -S,  --set    set RTC time and date\n");
            print("  -H,           hours\n");
            print("  -m,           minutes\n");
            print("  -s,           seconds\n");
            print("  -Y,           year\n");
            print("  -M,           month\n");
            print("  -D,           day\n");
            print("       --stack  print free stack\n");
      }
      else if ( (ParseArgsAs(argv, "set", PARSE_AS_EXIST, NULL) == STD_RET_OK)
              ||(ParseArgsAs(argv, "S",   PARSE_AS_EXIST, NULL) == STD_RET_OK) )
      {
            i32_t ahours   = 0;
            i32_t aminutes = 0;
            i32_t aseconds = 0;
            i32_t ayear    = 0;
            i32_t amonth   = 1;
            i32_t adate    = 1;

            ParseArgsAs(argv, "H", PARSE_AS_HEX, &ahours);
            ParseArgsAs(argv, "m", PARSE_AS_HEX, &aminutes);
            ParseArgsAs(argv, "s", PARSE_AS_HEX, &aseconds);

            ParseArgsAs(argv, "Y", PARSE_AS_HEX, &ayear);
            ParseArgsAs(argv, "M", PARSE_AS_HEX, &amonth);
            ParseArgsAs(argv, "D", PARSE_AS_HEX, &adate);

            print("%x2-%x2-20%x2, %x2:%x2:%x2\n", adate, amonth, ayear, ahours, aminutes, aseconds);

            time.hours   = ahours;
            time.minutes = aminutes;
            time.seconds = aseconds;
            date.year    = ayear;
            date.month   = amonth;
            date.day     = adate;

            if ( (DS1307_SetTime(time) != STD_RET_OK)
               ||(DS1307_SetDate(date) != STD_RET_OK) )
                  print("Set time error.\n");
      }
      else
      {
            time = DS1307_GetTime();
            date = DS1307_GetDate();

            if (date.day == 0)
                  date.day++;

            if (date.month == 0)
                  date.month++;

            print("%s, %x2 %s 20%x2, %x2:%x2:%x2\n",
                  weekDay[date.weekday-1], date.day, months[date.month-1], date.year,
                  time.hours, time.minutes, time.seconds);
      }

      if (ParseArgsAs(argv, "stack", PARSE_AS_EXIST, NULL) == STD_RET_OK)
      {
            print("Stack free: %d\n", SystemGetStackFreeSpace());
      }

      return STD_RET_OK;
}

/* End of application section declaration */
APP_SEC_END

#ifdef __cplusplus
}
#endif

/*==================================================================================================
                                            End of file
==================================================================================================*/
