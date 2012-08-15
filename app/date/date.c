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
#include "utils.h"
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
      const ch_t *weekDayNames[] = {"SUN", "MON", "TUE", "WED", "THU", "FRI", "SAT"};
      const ch_t *monthsNames[]  = {"Jan", "Feb", "Mar", "Apr", "May", "Jun",
                                    "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

      bcdTime_t  time;
      bcdDate_t  date;

      if ( (ParseArg(argv, "help", PARSE_AS_EXIST, NULL) == STD_RET_OK)
         ||(ParseArg(argv, "h",    PARSE_AS_EXIST, NULL) == STD_RET_OK) )
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
      else if ( (ParseArg(argv, "set", PARSE_AS_EXIST, NULL) == STD_RET_OK)
              ||(ParseArg(argv, "S",   PARSE_AS_EXIST, NULL) == STD_RET_OK) )
      {
            i32_t ahours   = 0;
            i32_t aminutes = 0;
            i32_t aseconds = 0;
            i32_t ayear    = 0;
            i32_t amonth   = 1;
            i32_t adate    = 1;

            ParseArg(argv, "H", PARSE_AS_DEC, &ahours);
            ParseArg(argv, "m", PARSE_AS_DEC, &aminutes);
            ParseArg(argv, "s", PARSE_AS_DEC, &aseconds);

            ParseArg(argv, "Y", PARSE_AS_DEC, &ayear);
            ParseArg(argv, "M", PARSE_AS_DEC, &amonth);
            ParseArg(argv, "D", PARSE_AS_DEC, &adate);

            /* check time range */
            if (ahours > 23)
                  ahours = 23;

            if (aminutes > 59)
                  aminutes = 59;

            if (aseconds > 59)
                  aseconds = 59;

            /* check date range */
            if (ayear > 99)
                  ayear = 99;

            if (amonth > 12)
                  amonth = 12;
            else if (amonth < 1)
                  amonth = 1;

            if (adate > 31)
                  adate = 31;
            else if (adate < 1)
                  adate = 1;

            /* convert values to BCD */
            time.hours   = UTL_Byte2BCD(ahours);
            time.minutes = UTL_Byte2BCD(aminutes);
            time.seconds = UTL_Byte2BCD(aseconds);
            date.year    = UTL_Byte2BCD(ayear);
            date.month   = UTL_Byte2BCD(amonth);
            date.day     = UTL_Byte2BCD(adate);

            if ( (DS1307_SetTime(time) != STD_RET_OK)
               ||(DS1307_SetDate(date) != STD_RET_OK) )
                  print("ERROR: unable to set RTC\n");
            else
                  print("Done.\n");
      }
      else
      {
            /* show time */
            time = DS1307_GetTime();
            date = DS1307_GetDate();

            u8_t month   = UTL_BCD2Byte(date.month);
            u8_t weekday = UTL_BCD2Byte(date.weekday);

            if (date.day == 0)
                  date.day++;

            if (date.month == 0)
                  date.month++;

            print("%s, %x2 %s 20%x2, %x2:%x2:%x2\n",
                  weekDayNames[weekday - 1], date.day, monthsNames[month - 1], date.year,
                  time.hours, time.minutes, time.seconds);
      }

      /* show stack free space if requested */
      if (ParseArg(argv, "stack", PARSE_AS_EXIST, NULL) == STD_RET_OK)
            print("Free stack: %d\n", SystemGetStackFreeSpace());

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
