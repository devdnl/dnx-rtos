/*=========================================================================*//**
@file    top.c

@author  Daniel Zorychta

@brief   Application show CPU load

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
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <sys/ioctl.h>
#include <dnx/os.h>
#include <dnx/thread.h>
#include <dnx/vt100.h>
#include <dnx/misc.h>

/*==============================================================================
  Local symbolic constants/macros
==============================================================================*/
#define KEY_READ_INTERVAL_SEC   (CLOCKS_PER_SEC * 0.01)
#define REFRESH_INTERVAL_SEC    (CLOCKS_PER_SEC * 1)

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
        int            term_row;
        memstat_t      mem;
        process_stat_t pstat;
};

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
int_main(top, STACK_DEPTH_VERY_LOW, int argc, char *argv[])
{
        UNUSED_ARG2(argc, argv);

        ioctl(stdin, IOCTL_TTY__ECHO_OFF);
        ioctl(stdout, IOCTL_TTY__CLEAR_SCR);

        int     key   = ' ';
        int     shift = 0;
        clock_t timer = clock() + REFRESH_INTERVAL_SEC;

        while (key != 'q' && key != '\0') {
                ioctl(stdin, IOCTL_VFS__NON_BLOCKING_RD_MODE);
                key = getchar();
                ioctl(stdin, IOCTL_VFS__DEFAULT_RD_MODE);

                if (!strchr("k,.", key)) {
                        if (difftime(clock(), timer) < REFRESH_INTERVAL_SEC) {
                                sleep_ms(KEY_READ_INTERVAL_SEC);
                                continue;
                        } else {
                                timer = clock();
                        }
                }

                global->term_row = 24;
                ioctl(stdout, IOCTL_TTY__GET_ROW, &global->term_row);
                global->term_row--;

                u32_t uptime = get_uptime();
                u32_t udays  = (uptime / (3600 * 24));
                u32_t uhrs   = (uptime / 3600) % 24;
                u32_t umins  = (uptime / 60) % 60;

                println(VT100_CLEAR_SCREEN);

                println("%s - %dd %d:%02d up\n", argv[0], udays, uhrs, umins);

                println("Bytes Mem: %d total, %d used, %d free\n",
                        get_memory_size(), get_used_memory(), get_free_memory());


                get_memory_usage_details(&global->mem);
                println("Bytes Mem: %d static, %d kernel, %d filesys\n",
                        global->mem.static_memory_usage,
                        global->mem.kernel_memory_usage,
                        global->mem.filesystems_memory_usage);

                println("Bytes Mem: %d network, %d modules, %d programs\n",
                        global->mem.network_memory_usage,
                        global->mem.modules_memory_usage,
                        global->mem.programs_memory_usage);

                println("\n");

                println(VT100_FONT_COLOR_BLACK VT100_BACK_COLOR_WHITE
                        "  PID PR     MEM  STU %%STU  %%CPU TH RES CMD"
                        VT100_RESET_ATTRIBUTES "\n");
//                       65535 -7 6553600 1200 99.9 99 999 initd

                size_t seek = 0;
                while (process_stat_seek(seek++, &global->pstat) == 0) {
                        char cpu_load_str[6];
                        if (global->pstat.zombie) {
                                snprintf(cpu_load_str, 6, "zombi");
                        } else {
                                snprintf(cpu_load_str, 6, " %2d.%d",
                                         global->pstat.cpu_load_cnt * 100 / global->pstat.cpu_load_total_cnt,
                                         (global->pstat.cpu_load_cnt * 100 / global->pstat.cpu_load_total_cnt) % 10);
                        }

                        println("%5d %2d %7d %4d %4d %s %2d %3d %s\n",
                                global->pstat.pid,
                                global->pstat.priority,
                                global->pstat.memory_usage,
                                global->pstat.stack_max_usage,
                                global->pstat.stack_max_usage * 100 / global->pstat.stack_size,
                                cpu_load_str,
                                global->pstat.threads_count,
                                global->pstat.dir_count + global->pstat.files_count +
                                global->pstat.mutexes_count + global->pstat.queue_count
                                + global->pstat.semaphores_count,
                                global->pstat.name);
                }

                enable_CPU_load_measurement();

                printf(VT100_CURSOR_HOME VT100_CURSOR_DOWN(2));


//                int task_number = get_number_of_monitored_tasks();
//
//                u32_t uptime = get_uptime();
//                u32_t udays  = (uptime / (3600 * 24));
//                u32_t uhrs   = (uptime / 3600) % 24;
//                u32_t umins  = (uptime / 60) % 60;
//
//                memstat_t mem;
//                get_detailed_memory_usage(&mem);
//
//                int mem_total = get_memory_size();
//                int mem_used  = get_used_memory();
//                int mem_free  = get_free_memory();
//
//                println(CLEAR_SCREEN"Press "FONT_BOLD"q"RESET_ATTRIBUTES" to quit or "FONT_BOLD"k"RESET_ATTRIBUTES" to kill program\n");
//                println("Total tasks: %u\tUp time: %ud %2u:%2u\n", task_number, udays, uhrs, umins);
//                println("Memory:\t%u total,\t%u used,\t%u free\n", mem_total, mem_used, mem_free);
//                println("Kernel  : %d\n", mem.used_kernel_memory);
//                println("System  : %d\n", mem.used_system_memory);
//                println("Modules : %d\n", mem.used_modules_memory);
//                println("Network : %d\n", mem.used_network_memory);
//                println("Programs: %d\n", mem.used_programs_memory);
//                println("\x1B[30;47mTSKHDL    PRI   FRSTK   ST+MEM  OPFI    %%CPU    NAME \x1B[0m\n");
//
//                if (key == '.') {
//                        if (shift + global->term_row < task_number) {
//                                shift++;
//                        }
//                } else if (key == ',') {
//                        if (shift) {
//                                shift--;
//                        }
//                }
//
//                taskstat_t *taskstat = calloc(task_number, sizeof(taskstat_t));
//                if (taskstat) {
//                        int      status         = 0;
//                        u32_t    total_cpu_load = get_total_CPU_usage();
//                        disable_CPU_load_measurement();
//                        for (int i = 0; i < task_number && status == 0; i++) {
//                                status = get_task_stat(i, &taskstat[i]);
//                        }
//                        enable_CPU_load_measurement();
//
//                        for (int i = shift; i < task_number && global->term_row > 0; i++) {
//                                taskstat_t taskinfo = taskstat[i];
//
//                                println("%x  %d\t%u\t%u\t%u\t%u.%u%%\t%s\n",
//                                       taskinfo.task_handle,
//                                       taskinfo.priority,
//                                       taskinfo.free_stack,
//                                       taskinfo.memory_usage,
//                                       taskinfo.opened_files,
//                                       ( taskinfo.cpu_usage * 100)  / total_cpu_load,
//                                       ((taskinfo.cpu_usage * 1000) / total_cpu_load) % 10,
//                                       taskinfo.task_name);
//                        }
//
//                        free(taskstat);
//                }

//                if (key == 'k') {
//                        ioctl(stdin, IOCTL_TTY__ECHO_ON);
//
//                        int task_handle = 0;
//
//                        printf("Enter task handle: 0x");
//                        fflush(stdout);
//                        scanf("%8X", &task_handle);
//
//                        task_t *task = (task_t *)task_handle;
//                        if (task == task_get_parent_handle()) {
//                                puts("Parent task cannot be killed!");
//                                sleep(2);
//                        } else if (task != task_get_handle()) {
//                                errno = 0;
//                                if (task_is_exist(task)) {
//                                        task_delete(task);
//                                } else {
//                                        perror(NULL);
//                                        timer = timer_reset();
//                                }
//                        } else {
//                                puts(strerror(EPERM));
//                                sleep(2);
//                        }
//
//                        ioctl(stdin, IOCTL_TTY__ECHO_OFF);
//                }
        }

        puts("Quit");

        ioctl(stdin, IOCTL_TTY__ECHO_ON);

        return 0;
}

/*==============================================================================
  End of file
==============================================================================*/
