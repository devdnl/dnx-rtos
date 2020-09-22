/*=========================================================================*//**
@file    uptime.c

@author  Daniel Zorychta

@brief   Show system uptime

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
#include <dnx/os.h>

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
};

/*==============================================================================
  Exported object definitions
==============================================================================*/
PROGRAM_PARAMS(uptime, STACK_DEPTH_LOW);

/*==============================================================================
  Function definitions
==============================================================================*/
//==============================================================================
/**
 * @brief Program main function
 */
//==============================================================================
int main(int argc, char *argv[])
{
        (void) argc;
        (void) argv;

        u32_t uptime = get_uptime();
        u32_t udays  = (uptime / (3600 * 24));
        u32_t uhrs   = (uptime / 3600) % 24;
        u32_t umins  = (uptime / 60) % 60;

        avg_CPU_load_t avg;
        get_average_CPU_load(&avg);

        printf("up %ud %u:%02u, average load: %d.%d, %d.%d, %d.%d\n",
               udays, uhrs, umins,
               avg.avg1min  / 10, avg.avg1min  % 10,
               avg.avg5min  / 10, avg.avg5min  % 10,
               avg.avg15min / 10, avg.avg15min % 10);

        return EXIT_SUCCESS;
}

/*==============================================================================
  End of file
==============================================================================*/
