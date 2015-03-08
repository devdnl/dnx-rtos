/*=========================================================================*//**
@file    date.c

@author  Daniel Zorychta

@brief   Program set and read date (time)

@note    Copyright (C) 2015 Daniel Zorychta <daniel.zorychta@gmail.com>

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
                        puts("Formats start with % character:");
                        puts("  %nHIJMSAaBbhCYydpjmXxFDz");
                        show_date = false;
                        break;
                }

                if (strncmp("--set=", argv[i], 6) == 0) {
                        uint day, month, year, hour, minute, second;
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

                        sscanf(&argv[i][9], "%c%2d%2d", &sign, &hour, &minute);

                        if (!(sign == '-' || sign == '+') || hour > 23 || minute > 59) {
                                puts(wrong_time_fm);
                                return EXIT_FAILURE;
                        }

                        int diff = (hour * 3600 + minute * 60) * (sign == '-' ? -1 : 1);
                        tzset(diff);

                        show_date = false;
                        break;
                }

                if (argv[i][0] == '+') {
                        char *buf = calloc(1, TIME_FORMAT_BUF_LEN);
                        if (buf) {
                                time_t     t  = time(NULL);

                                struct tm tm;
                                if (UTC) {
                                        gmtime_r(&t, &tm);
                                } else {
                                        localtime_r(&t, &tm);
                                }

                                strftime(buf, TIME_FORMAT_BUF_LEN, &argv[i][1], &tm);
                                puts(buf);

                                free(buf);

                                show_date = false;
                                break;
                        }
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

                printf(asctime(&tm));
        }

        return EXIT_SUCCESS;
}

/*==============================================================================
  End of file
==============================================================================*/
