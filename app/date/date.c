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
#include "utils.h"
#include <string.h>
#include "ds1307_def.h"

/* Begin of application section declaration */
PROGRAM(date, 4)
PROG_SEC_BEGIN

/*==================================================================================================
                                  Local symbolic constants/macros
==================================================================================================*/
#define ParseArg(a, b, c, d)    STD_RET_ERROR /* DNLFIXME new argument parse needed */

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
stdRet_t appmain(ch_t *argv[], int argc)
{
      const ch_t *weekDayNames[] = {"SUN", "MON", "TUE", "WED", "THU", "FRI", "SAT"};
      const ch_t *monthsNames[]  = {"Jan", "Feb", "Mar", "Apr", "May", "Jun",
                                    "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

      stdRet_t  status = STD_RET_OK;
      bcdTime_t time   = {0x00, 0x00, 0x00};
      bcdDate_t date   = {0, 0, 0, 0};
      FILE_t    *rtc;

      if ( (ParseArg(argv, "help", PARSE_AS_EXIST, NULL) == STD_RET_OK)
         ||(ParseArg(argv, "h",    PARSE_AS_EXIST, NULL) == STD_RET_OK) ) {
            printf("Syntax: date [OPTION]...\n");
            printf("Print actual time and date.\n");
            printf("  -S,  --set    set RTC time and date\n");
            printf("  -H,           hours\n");
            printf("  -m,           minutes\n");
            printf("  -s,           seconds\n");
            printf("  -Y,           year\n");
            printf("  -M,           month\n");
            printf("  -D,           day\n");
            printf("       --stack  print free stack\n");
      } else {
            rtc = fopen("/dev/rtc", "r+");

            if (rtc)
            {
                  if ( (ParseArg(argv, "set", PARSE_AS_EXIST, NULL) == STD_RET_OK)
                     ||(ParseArg(argv, "S",   PARSE_AS_EXIST, NULL) == STD_RET_OK) ) {
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

                        if ( (ioctl(rtc, RTC_IORQ_SETTIME, &time) != STD_RET_OK)
                           ||(ioctl(rtc, RTC_IORQ_SETDATE, &date) != STD_RET_OK) )
                        {
                              printf("ERROR: unable to set RTC\n");
                              status = STD_RET_ERROR;
                        }
                  }
                  else
                  {
                        /* show time */
                        ioctl(rtc, RTC_IORQ_GETTIME, &time);
                        ioctl(rtc, RTC_IORQ_GETDATE, &date);

                        u8_t month   = UTL_BCD2Byte(date.month);
                        u8_t weekday = UTL_BCD2Byte(date.weekday);

                        if (weekday == 0)
                              weekday++;

                        if (month == 0)
                              month++;

                        if (date.day == 0)
                              date.day++;

                        if (date.month == 0)
                              date.month++;

                        printf("%s, %x2 %s 20%x2, %x2:%x2:%x2\n",
                              weekDayNames[weekday - 1], date.day, monthsNames[month - 1], date.year,
                              time.hours, time.minutes, time.seconds);
                  }

                  fclose(rtc);
            }
            else
            {
                  printf("Unable to open RTC file\n");
            }
      }

      /* show stack free space if requested */
      if (ParseArg(argv, "stack", PARSE_AS_EXIST, NULL) == STD_RET_OK)
      {
            printf("Free stack: %d levels\n", get_free_stack());
      }

      return status;
}

/* End of application section declaration */
PROG_SEC_END

#ifdef __cplusplus
}
#endif

/*==================================================================================================
                                            End of file
==================================================================================================*/
