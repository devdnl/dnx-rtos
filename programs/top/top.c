/*=========================================================================*//**
@file    top.c

@author  Daniel Zorychta

@brief   Application show CPU load

@note    Copyright (C) 2013 Daniel Zorychta <daniel.zorychta@gmail.com>

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

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Include files
==============================================================================*/
#include "top.h"

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
GLOBAL_VARIABLES {
};

/*==============================================================================
  Exported object definitions
==============================================================================*/
PROGRAM_PARAMS(top, STACK_DEPTH_LOW);

/*==============================================================================
  Function definitions
==============================================================================*/

//==============================================================================
/**
 * @brief Main function
 */
//==============================================================================
int PROGRAM_MAIN(top, int argc, char *argv[])
{
        (void)argc;
        (void)argv;

        u8_t divcnt = 10;

        while (TRUE) {
                char chr = EOF;
                fread(&chr, sizeof(char), 1, stdin);

                if (chr == 'q') {
                        break;
                }

                milisleep(100);

                if (divcnt < 10) {
                        divcnt++;
                        continue;
                }

                u8_t n = get_number_of_monitored_tasks();

                printf("\x1B[2J\x1B[HPress q to quit\n");

                printf("Total tasks: %u\n", n);

                u32_t uptime = get_uptime();
                u32_t udays  = (uptime / (3600 * 24));
                u32_t uhrs   = (uptime / 3600) % 24;
                u32_t umins  = (uptime / 60) % 60;

                printf("Up time: %ud %u2:%u2\n", udays, uhrs, umins);

                printf("Memory:\t%u total,\t%u used,\t%u free\n\n",
                       get_memory_size(),
                       get_used_memory(),
                       get_free_memory());

                printf("\x1B[30;47m TSKHDL   PRI   FRSTK   MEM     OPFI    %%CPU    NAME \x1B[0m\n");

                for (int i = 0; i < n; i++) {
                        struct taskstat taskinfo;
                        u32_t total_cpu_load = get_total_CPU_usage();

                        if (get_task_stat(i, &taskinfo) == STD_RET_OK) {
                                printf("%x  %d\t%u\t%u\t%u\t%u.%u%%\t%s\n",
                                taskinfo.task_handle,
                                taskinfo.priority,
                                taskinfo.free_stack,
                                taskinfo.memory_usage,
                                taskinfo.opened_files,
                                ( taskinfo.cpu_usage * 100)  / total_cpu_load,
                                ((taskinfo.cpu_usage * 1000) / total_cpu_load) % 10,
                                taskinfo.task_name);
                        } else {
                                break;
                        }
                }

                clear_total_CPU_usage();

                divcnt = 0;
        }

        return STD_RET_OK;
}

#ifdef __cplusplus
}
#endif

/*==============================================================================
  End of file
==============================================================================*/
