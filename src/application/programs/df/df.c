/*=========================================================================*//**
@file    df.c

@author  Daniel Zorychta

@brief   List mounted file systems

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
#include <mntent.h>
#include <dnx/misc.h>
#include <dnx/vt100.h>
#include <dnx/thread.h>
#include <unistd.h>

/*==============================================================================
  Local symbolic constants/macros
==============================================================================*/
#define KiB                             (u32_t)(1024)
#define MiB                             (u32_t)(1024*1024)
#define GiB                             (u64_t)(1024*1024*1024)
#define CONVERT_TO_KiB(_val)            (_val >> 10)
#define CONVERT_TO_MiB(_val)            (_val >> 20)
#define CONVERT_TO_GiB(_val)            (_val >> 30)

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
PROGRAM_PARAMS(df, STACK_DEPTH_LOW);

/*==============================================================================
  Function definitions
==============================================================================*/
static int thread(void *arg)
{
        tid_t id = thread_current();
        pid_t pid = getpid();

        for (size_t i = 0 ; i < 5; i++) {
                msleep(1000);
                printf("Jestem wątek nr %u z %u\n", id, pid);
                thread_exit(10 + id);
        }

        return id;
}

//==============================================================================
/**
 * @brief Cat main function
 */
//==============================================================================
int main(int argc, char *argv[])
{
        (void) argc;
        (void) argv;

        pid_t pid = getpid();
        printf("=====Jestem programem %u=====\n", pid);

        if (argc == 1) {
                pid = process_create("df --no-child", &(process_attr_t) {
                        .f_stdin  = stdin,
                        .f_stdout = stdout,
                        .f_stderr = stderr,
                        .p_stdin  = NULL,
                        .p_stdout = NULL,
                        .p_stderr = NULL,
                        .cwd      = "/",
                        .priority = PRIORITY_NORMAL,
                        .detached = false
                });
        } else {
//                sleep(7);
        }

        printf(VT100_FONT_BOLD"File system"VT100_CURSOR_FORWARD(6)"Total"VT100_CURSOR_FORWARD(6)
               "Free"VT100_CURSOR_FORWARD(7)"%%Used  Mount point"VT100_RESET_ATTRIBUTES"\n");

        struct mntent mnt;
        int           i = 0;
        while (getmntentry(i++, &mnt) == 0) {
                u32_t dtotal;
                u32_t dfree;
                const char *unit;

                if (mnt.mnt_total > 1*GiB) {
                        dtotal = CONVERT_TO_MiB(mnt.mnt_total);
                        dfree  = CONVERT_TO_MiB(mnt.mnt_free);
                        unit   = "MiB";
                } else if (mnt.mnt_total > 1*MiB) {
                        dtotal = CONVERT_TO_KiB(mnt.mnt_total);
                        dfree  = CONVERT_TO_KiB(mnt.mnt_free);
                        unit   = "KiB";
                } else {
                        dtotal = mnt.mnt_total;
                        dfree  = mnt.mnt_free;
                        unit   = "B";
                }

                u32_t percent = ((dtotal - dfree) * 1000) / dtotal;

                printf("%s"  VT100_CURSOR_BACKWARD(90)VT100_CURSOR_FORWARD(17)
                       "%u%s"VT100_CURSOR_BACKWARD(90)VT100_CURSOR_FORWARD(28)
                       "%u%s"VT100_CURSOR_BACKWARD(90)VT100_CURSOR_FORWARD(39)
                       "%u.%u%%"VT100_CURSOR_BACKWARD(90)VT100_CURSOR_FORWARD(46)
                       "%s\n",
                       mnt.mnt_fsname, dtotal, unit, dfree, unit,
                       percent / 10, percent % 10, mnt.mnt_dir);
        }

        for (size_t i = 0; i < 5; i++) {
                thread_create(thread, &(const thread_attr_t) {
                        .priority = PRIORITY_NORMAL,
                        .stack_depth = STACK_DEPTH_LOW,
                        .detached = false
                }, NULL);
        }

        sleep(1);

        for (size_t i = 0; i < 5; i++) {
                int status = -1;
                thread_join(i + 1, &status);
                printf("Status z wątku %d: %d\n", i+1, status);
        }

        if (argc == 1) {
                int status = -1;
                process_wait(pid, &status, MAX_DELAY_MS);
                printf("Child pid return status: %d\n", status);
        } else {
                return pid;
        }

        return EXIT_SUCCESS;
}

/*==============================================================================
  End of file
==============================================================================*/
