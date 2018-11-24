/*=========================================================================*//**
@file    date.c

@author  Daniel Zorychta

@brief   Program set and read date (time)

@note    Copyright (C) 2015 Daniel Zorychta <daniel.zorychta@gmail.com>

         This program is free software; you can redistribute it and/or modify
         it under the terms of the GNU General Public License as published by
         the Free Software Foundation and modified by the dnx RTOS exception.

         NOTE: The modification  to the GPL is  included to allow you to
               distribute a combined work that includes dnx RTOS without
               being obliged to provide the source  code for proprietary
               components outside of the dnx RTOS.

         The dnx RTOS  is  distributed  in the hope  that  it will be useful,
         but WITHOUT  ANY  WARRANTY;  without  even  the implied  warranty of
         MERCHANTABILITY  or  FITNESS  FOR  A  PARTICULAR  PURPOSE.  See  the
         GNU General Public License for more details.

         Full license text is available on the following file: doc/license.txt.


*//*==========================================================================*/

/*==============================================================================
  Include files
==============================================================================*/
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

/*==============================================================================
  Local symbolic constants/macros
==============================================================================*/
#define TIME_FORMAT_BUF_LEN     100

/*==============================================================================
  Local types, enums definitions
==============================================================================*/

/*==============================================================================
  Local function prototypes
==============================================================================*/

/*==============================================================================
  Local object definitions
==============================================================================*/
GLOBAL_VARIABLES_SECTION {
        char buf[TIME_FORMAT_BUF_LEN];
};

static const char *wrong_time_fm = "Wrong time format!";

/*==============================================================================
  Exported object definitions
==============================================================================*/

/*==============================================================================
  Function definitions
==============================================================================*/
//==============================================================================
/**
 * @brief Program main function
 */
//==============================================================================
int_main(date, STACK_DEPTH_LOW, int argc, char *argv[])
{
        bool UTC       = false;
        bool show_date = true;

        for (int i = 1; i < argc; i++) {
                if (strcmp(argv[i], "-u") == 0) {
                        UTC = true;
                        continue;
                }

                if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0) {
                        printf("%s <options|+format>\n", argv[0]);
                        puts("Options:");
                        puts("  --help                      this help");
                        puts("  --set=DD/MM/YYYY,HH:MM:SS   set date and time (UTC)");
                        puts("  --set-tz=TIMEOFFSET         set timezone offset e.g. +0400, -0100\n");
                        puts("  -u                          UTC time");
                        puts("Formats start with % character (e.g. date \"+Time: %X\"):");
                        puts("  %nHIJMSAaBbhCYydpjmXxFDz");
                        show_date = false;
                        break;
                }

                if (strncmp("--set=", argv[i], 6) == 0) {
                        int day, month, year, hour, minute, second;
                        day = month = year = hour = minute = second = 0xFFFF;

                        sscanf(&argv[i][6], "%d/%d/%d,%d:%d:%d",
                               &day, &month, &year, &hour, &minute, &second);

                        if (  day  > 31 || month  > 12 || month  < 1  ||year < 1970
                           || hour > 23 || minute > 59 || second > 59) {

                                puts(wrong_time_fm);
                                return EXIT_FAILURE;
                        }

                        struct tm t;
                        t.tm_hour = hour;
                        t.tm_min  = minute;
                        t.tm_sec  = second;
                        t.tm_mday = day;
                        t.tm_mon  = month - 1;
                        t.tm_year = year - 1900;

                        time_t tv = mktime(&t);

                        errno = 0;
                        if (stime(&tv) != 0) {
                                perror(argv[0]);
                                return EXIT_FAILURE;
                        }

                        show_date = false;
                        break;
                }

                if (strncmp("--set-tz=", argv[i], 9) == 0) {
                        if (strlen(&argv[i][9]) != 5) {
                                puts(wrong_time_fm);
                                return EXIT_FAILURE;
                        }

                        uint sign, hour, minute;
                        sign = hour = minute = 0xFF;

                        sscanf(&argv[i][9], "%c%2u%2u", &sign, &hour, &minute);

                        if (!(sign == '-' || sign == '+') || hour > 23 || minute > 59) {
                                puts(wrong_time_fm);
                                return EXIT_FAILURE;
                        }

                        int diff = cast(int, (hour * 3600 + minute * 60))
                                 * (sign == '-' ? -1 : 1);
                        tzset(diff);

                        show_date = false;
                        break;
                }

                if (argv[i][0] == '+') {
                        time_t t = time(NULL);

                        struct tm tm;
                        if (UTC) {
                                gmtime_r(&t, &tm);
                        } else {
                                localtime_r(&t, &tm);
                        }

                        strftime(global->buf, TIME_FORMAT_BUF_LEN, &argv[i][1], &tm);
                        puts(global->buf);

                        free(global->buf);

                        show_date = false;
                        break;
                }
        }

        if (show_date) {
                time_t t = time(NULL);

                struct tm tm;
                if (UTC) {
                        gmtime_r(&t, &tm);
                } else {
                        localtime_r(&t, &tm);
                }

                strftime(global->buf, TIME_FORMAT_BUF_LEN, "%a %B %d %X %Y\n", &tm);

                printf(global->buf);
        }

        return EXIT_SUCCESS;
}

/*==============================================================================
  End of file
==============================================================================*/
