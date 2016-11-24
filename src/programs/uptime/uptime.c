/*=========================================================================*//**
@file    uptime.c

@author  Daniel Zorychta

@brief   Show system uptime

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

/*==============================================================================
  Function definitions
==============================================================================*/
//==============================================================================
/**
 * @brief Program main function
 */
//==============================================================================
int_main(uptime, STACK_DEPTH_LOW, int argc, char *argv[])
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
