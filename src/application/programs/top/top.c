/*=========================================================================*//**
@file    top.c

@author  Daniel Zorychta

@brief   Application show CPU load

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
#define KEY_READ_INTERVAL_SEC   (CLOCKS_PER_SEC / 100)
#define REFRESH_INTERVAL_SEC    (CLOCKS_PER_SEC * 1)
#define MSG_LINE_POS            VT100_CURSOR_HOME VT100_CURSOR_DOWN(5) VT100_ERASE_LINE_FROM_CUR

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
 * @brief Main function
 */
//==============================================================================
int_main(top, STACK_DEPTH_LOW, int argc, char *argv[])
{
        UNUSED_ARG2(argc, argv);

        ioctl(fileno(stdin), IOCTL_TTY__ECHO_OFF);
        ioctl(fileno(stdout), IOCTL_TTY__CLEAR_SCR);

        int     key   = ' ';
        clock_t timer = clock() + REFRESH_INTERVAL_SEC;

        while (key != 'q' && key != '\0') {
                ioctl(fileno(stdin), IOCTL_VFS__NON_BLOCKING_RD_MODE);
                clearerr(stdin);
                key = getchar();
                ioctl(fileno(stdin), IOCTL_VFS__DEFAULT_RD_MODE);

                if (!strchr("qk,.", key) and key != ETX) {
                        if ((clock() - timer) < REFRESH_INTERVAL_SEC) {
                                msleep(KEY_READ_INTERVAL_SEC);
                                continue;
                        } else {
                                timer = clock();
                        }
                }

                u32_t uptime = get_uptime();
                u32_t udays  = (uptime / (3600 * 24));
                u32_t uhrs   = (uptime / 3600) % 24;
                u32_t umins  = (uptime / 60) % 60;

                printf(VT100_CLEAR_SCREEN);

                avg_CPU_load_t avg = {0, 0, 0, 0};
                get_average_CPU_load(&avg);

                printf("%s - %ud %u:%0u up, avg. load %%: %d.%d, %d.%d, %d.%d\n",
                        argv[0], udays, uhrs, umins,
                        avg.avg1min  / 10, avg.avg1min  % 10,
                        avg.avg5min  / 10, avg.avg5min  % 10,
                        avg.avg15min / 10, avg.avg15min % 10);

                printf("B Mem: %d total, %d used, %d free\n",
                        get_memory_size(), get_used_memory(), get_free_memory());

                get_memory_usage_details(&global->mem);
                printf("%d static, %d kernel, %d filesystems\n",
                        global->mem.static_memory_usage,
                        global->mem.kernel_memory_usage,
                        global->mem.filesystems_memory_usage);

                printf("%d shared, %d cached, %d modules\n",
                        global->mem.shared_memory_usage,
                        global->mem.cached_memory_usage,
                        global->mem.modules_memory_usage);

                printf("%d network, %d programs\n",
                        global->mem.network_memory_usage,
                        global->mem.programs_memory_usage);

                printf("\n");

                printf(VT100_FONT_COLOR_BLACK VT100_BACK_COLOR_WHITE
                       "PID PR     MEM  STU %%STU  %%CPU SCPS TH RES CMD"
                       VT100_RESET_ATTRIBUTES "\n");

                size_t seek = 0;
                while (process_stat_seek(seek++, &global->pstat) == 0) {
                        char cpu_load_str[7];
                        if (global->pstat.threads_count == 0) {
                                snprintf(cpu_load_str, sizeof(cpu_load_str), "zombie");
                        } else {
                                snprintf(cpu_load_str, 7, " %2d.%d",
                                         global->pstat.CPU_load / 10,
                                         global->pstat.CPU_load % 10);
                        }

                        printf("%3d %2d %7u %4d %4d %s %4u %2d %3d %s\n",
                                global->pstat.pid,
                                global->pstat.priority,
                                (uint)global->pstat.memory_usage,
                                global->pstat.stack_max_usage,
                                global->pstat.stack_max_usage * 100 / global->pstat.stack_size,
                                cpu_load_str,
                                global->pstat.syscalls,
                                global->pstat.threads_count,
                                global->pstat.dir_count
                                + global->pstat.files_count
                                + global->pstat.mutexes_count
                                + global->pstat.queue_count
                                + global->pstat.semaphores_count
                                + global->pstat.socket_count,
                                global->pstat.name);
                }

                if (key == 'k') {
                        printf(MSG_LINE_POS);
                        printf("Kill PID: ");
                        fflush(stdout);

                        ioctl(fileno(stdin), IOCTL_TTY__ECHO_ON);

                        int pid = 0;
                        scanf("%d", &pid);

                        if (process_kill(pid) != 0) {
                                int e = errno;
                                printf(MSG_LINE_POS);
                                puts(strerror(e));
                                errno = 0;
                                sleep(2);
                        }

                        ioctl(fileno(stdin), IOCTL_TTY__ECHO_OFF);

                } else if (key == 'q' or key == ETX) {
                        break;
                }
        }

        ioctl(fileno(stdin), IOCTL_TTY__ECHO_ON);

        return 0;
}

/*==============================================================================
  End of file
==============================================================================*/
