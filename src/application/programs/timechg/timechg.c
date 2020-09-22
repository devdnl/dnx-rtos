/*=========================================================================*//**
@file    timechg.c

@author  Daniel Zorychta

@brief   Set CET or CEST.

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
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <errno.h>
#include <unistd.h>
#include <time.h>
#include <dnx/misc.h>

/*==============================================================================
  Local symbolic constants/macros
==============================================================================*/

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
        time_t    current_time_unix;
        int       current_year;
        time_t    beginning_of_summer_time;
        time_t    end_of_summer_time;
};

/*==============================================================================
  Exported object definitions
==============================================================================*/
PROGRAM_PARAMS(timechg, STACK_DEPTH_LOW);

/*==============================================================================
  Function definitions
==============================================================================*/
//==============================================================================
/**
 * @brief  The function find the first time of summer time.
 */
//==============================================================================
static void calculate_beginning_of_daylight_saving_time(void)
{
        struct tm tm;
        tm.tm_hour = 1;
        tm.tm_min  = 0;
        tm.tm_sec  = 0;
        tm.tm_mday = 31;
        tm.tm_mon  = 2;
        tm.tm_year = global->current_year;

        time_t time = mktime(&tm);
        gmtime_r(&time, &tm);

        tm.tm_mday -= tm.tm_wday;

        global->beginning_of_summer_time = mktime(&tm);
}

//==============================================================================
/**
 * @brief  Function find the last time of summer time.
 */
//==============================================================================
static void calculate_end_of_daylight_saving_time(void)
{
        struct tm tm;
        tm.tm_hour = 0;
        tm.tm_min  = 59;
        tm.tm_sec  = 59;
        tm.tm_mday = 31;
        tm.tm_mon  = 9;
        tm.tm_year = global->current_year;

        time_t time = mktime(&tm);
        gmtime_r(&time, &tm);

        tm.tm_mday -= tm.tm_wday;

        global->end_of_summer_time = mktime(&tm);
}

//==============================================================================
/**
 * @brief  Function check that now is summer time or not.
 *
 * @return Return true if summer time is now, otherwise false.
 */
//==============================================================================
static bool is_summer_time(void)
{
        return (  global->current_time_unix >= global->beginning_of_summer_time
               && global->current_time_unix <= global->end_of_summer_time);
}

//==============================================================================
/**
 * @brief Program main function
 */
//==============================================================================
int main(int argc, char *argv[])
{
        if (argc != 2) {
                printf("Usage: %s <interval-sec>\n", argv[0]);
                return EXIT_FAILURE;

        } else {

                int interval = 0;
                sscanf(argv[1], "%d", &interval);

                do {
                        global->current_time_unix = time(NULL);

                        struct tm tm;
                        gmtime_r(&global->current_time_unix, &tm);
                        global->current_year = tm.tm_year;

                        calculate_beginning_of_daylight_saving_time();
                        calculate_end_of_daylight_saving_time();

                        if (is_summer_time()) {
                                tzset(TIMEZONE_CEST);
                        } else {
                                tzset(TIMEZONE_CET);
                        }

                        sleep(interval);

                } while (interval);

                return EXIT_SUCCESS;
        }
}

/*==============================================================================
  End of file
==============================================================================*/
