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

/*==============================================================================
  Include files
==============================================================================*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/ioctl.h>
#include <dnx/os.h>
#include <dnx/timer.h>
#include <dnx/thread.h>

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
GLOBAL_VARIABLES_SECTION_BEGIN
int term_row;
GLOBAL_VARIABLES_SECTION_END

/*==============================================================================
  Exported object definitions
==============================================================================*/

/*==============================================================================
  Function definitions
==============================================================================*/
//==============================================================================
/**
 * @brief Function print line if is enough free lines in terminal
 */
//==============================================================================
static void println(const char *fmt, ...)
{
        if (global->term_row > 0) {
                va_list args;
                va_start(args, fmt);
                vfprintf(stdout, fmt, args);
                va_end(args);
                global->term_row--;
        }
}

//==============================================================================
/**
 * @brief Main function
 */
//==============================================================================
PROGRAM_MAIN(top, int argc, char *argv[])
{
        (void) argc;
        (void) argv;

        ioctl(stdin, TTY_IORQ_ECHO_OFF);
        ioctl(stdout, TTY_IORQ_CLEAR_SCR);

        int     key   = ' ';
        int     shift = 0;
        timer_t timer = timer_set_expired();

        while (key != 'q' && key != '\0') {
                ioctl(stdin, NON_BLOCKING_RD_MODE);
                key = getchar();
                ioctl(stdin, DEFAULT_RD_MODE);

                if (!strchr("k,.", key)) {
                        if (timer_is_not_expired(timer, 1000)) {
                                sleep_ms(10);
                                continue;
                        } else {
                                timer = timer_reset();
                        }
                }

                global->term_row = 24;
                ioctl(stdout, TTY_IORQ_GET_ROW, &global->term_row);
                global->term_row--;

                int task_number = get_number_of_monitored_tasks();

                u32_t uptime = get_uptime();
                u32_t udays  = (uptime / (3600 * 24));
                u32_t uhrs   = (uptime / 3600) % 24;
                u32_t umins  = (uptime / 60) % 60;

                struct sysmoni_used_memory mem;
                get_detailed_memory_usage(&mem);

                int mem_total = get_memory_size();
                int mem_used  = get_used_memory();
                int mem_free  = get_free_memory();

                println(CLEAR_SCREEN"Press q to quit or k to kill program\n");
                println("Total tasks: %u\tUp time: %ud %2u:%2u\n", task_number, udays, uhrs, umins);
                println("Memory:\t%u total,\t%u used,\t%u free\n", mem_total, mem_used, mem_free);
                println("Kernel  : %d\n", mem.used_kernel_memory);
                println("System  : %d\n", mem.used_system_memory);
                println("Modules : %d\n", mem.used_modules_memory);
                println("Network : %d\n", mem.used_network_memory);
                println("Programs: %d\n", mem.used_programs_memory);
                println("\x1B[30;47m TSKHDL   PRI   FRSTK   MEM     OPFI    %%CPU    NAME \x1B[0m\n");

                if (key == '.') {
                        if (shift + global->term_row < task_number) {
                                shift++;
                        }
                } else if (key == ',') {
                        if (shift) {
                                shift--;
                        }
                }

                struct sysmoni_taskstat *taskstat = calloc(task_number, sizeof(struct sysmoni_taskstat));
                if (taskstat) {
                        stdret_t status         = STD_RET_OK;
                        u32_t    total_cpu_load = get_total_CPU_usage();
                        disable_CPU_load_measurement();
                        for (int i = 0; i < task_number && status == STD_RET_OK; i++) {
                                status = get_task_stat(i, &taskstat[i]);
                        }
                        enable_CPU_load_measurement();

                        for (int i = shift; i < task_number && global->term_row > 0; i++) {
                                struct sysmoni_taskstat taskinfo = taskstat[i];

                                println("%x  %d\t%u\t%u\t%u\t%u.%u%%\t%s\n",
                                       taskinfo.task_handle,
                                       taskinfo.priority,
                                       taskinfo.free_stack,
                                       taskinfo.memory_usage,
                                       taskinfo.opened_files,
                                       ( taskinfo.cpu_usage * 100)  / total_cpu_load,
                                       ((taskinfo.cpu_usage * 1000) / total_cpu_load) % 10,
                                       taskinfo.task_name);
                        }

                        free(taskstat);
                }

                if (key == 'k') {
                        ioctl(stdin, TTY_IORQ_ECHO_ON);

                        int task_handle = 0;

                        printf("Enter task handle: 0x");
                        scanf("%8X", &task_handle);

                        task_t *task = (task_t *)task_handle;
                        if (task != task_get_handle() && task != task_get_parent_handle()) {
                                errno = 0;
                                if (task_is_exist(task)) {
                                        task_delete(task);
                                } else {
                                        perror(NULL);
                                        timer = timer_reset();
                                }
                        } else {
                                puts(strerror(EPERM));
                        }

                        ioctl(stdin, TTY_IORQ_ECHO_OFF);
                }
        }

        ioctl(stdin, TTY_IORQ_ECHO_ON);

        return 0;
}

/*==============================================================================
  End of file
==============================================================================*/
