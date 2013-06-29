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
#include <stdio.h>
#include <stdlib.h>
#include "top.h"


/*==============================================================================
  Local symbolic constants/macros
==============================================================================*/

/*==============================================================================
  Local types, enums definitions
==============================================================================*/
struct thread_data {
        FILE *input;
        void *main_mem;
};

/*==============================================================================
  Local function prototypes
==============================================================================*/
static void read_keyboard_task(void *arg);

/*==============================================================================
  Local object definitions
==============================================================================*/
GLOBAL_VARIABLES {
        int key;
};

/*==============================================================================
  Exported object definitions
==============================================================================*/
PROGRAM_PARAMS(top, STACK_DEPTH_VERY_LOW);

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
        (void) argc;
        (void) argv;

        struct thread_data main_data = {stdin, global};
        task_t *kbrd_task = new_task(read_keyboard_task, "top:1", STACK_DEPTH_USER(80), &main_data);
        if (!kbrd_task) {
                printf("Cannot create child task\n");
                return EXIT_FAILURE;
        }

        while (global->key != 'q') {
                u8_t n = get_number_of_monitored_tasks();

                printf(CLEAR_SCREEN"Press q to quit\n");

                printf("Total tasks: %u\n", n);

                u32_t uptime = get_uptime();
                u32_t udays  = (uptime / (3600 * 24));
                u32_t uhrs   = (uptime / 3600) % 24;
                u32_t umins  = (uptime / 60) % 60;

                printf("Up time: %ud %2u:%2u\n", udays, uhrs, umins);

                struct sysmoni_used_memory mem;
                get_detailed_memory_usage(&mem);

                int mem_total = get_memory_size();
                int mem_used  = get_used_memory();
                int mem_free  = get_free_memory();

                printf("Memory:\t%u total,\t%u used,\t%u free\n",
                       mem_total,
                       mem_used,
                       mem_free);

                printf("Kernel  : %d\nSystem  : %d\nModules : %d\nPrograms: %d\n\n",
                       mem.used_kernel_memory,
                       mem.used_system_memory,
                       mem.used_modules_memory,
                       mem.used_programs_memory);

                printf("\x1B[30;47m TSKHDL   PRI   FRSTK   MEM     OPFI    %%CPU    NAME \x1B[0m\n");

                for (int i = 0; i < n; i++) {
                        struct sysmoni_taskstat taskinfo;
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

                sleep_ms(2000);
        }

        return EXIT_SUCCESS;
}

//==============================================================================
/**
 * @brief Task used to read keyboard in background to main program
 *
 * @param[in] *arg              pointer to main task memory
 */
//==============================================================================
static void read_keyboard_task(void *arg)
{
        /* connecting main thread with children */
        struct thread_data *main_data = arg;
        set_stdin(main_data->input);
        set_global_variables(main_data->main_mem);

        for (;;) {
                global->key = getchar();
                if (global->key == 'q')
                        break;
        }

        suspend_task(get_parent_handle());
        resume_task(get_parent_handle());

        task_exit();
}

#ifdef __cplusplus
}
#endif

/*==============================================================================
  End of file
==============================================================================*/
