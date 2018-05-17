/*==============================================================================
File    tcl.c

Author  Daniel Zorychta

Brief   TCL-like language interpreter.

        Copyright (C) 2017 Daniel Zorychta <daniel.zorychta@gmail.com>

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

==============================================================================*/

/*==============================================================================
  Include files
==============================================================================*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <dnx/vt100.h>
#include <dnx/misc.h>
#include <sys/ioctl.h>
#include <utcl.h>

/*==============================================================================
  Local symbolic constants/macros
==============================================================================*/
#define HISTORY_NEXT_KEY                "\033^[A\n"
#define HISTORY_PREV_KEY                "\033^[B\n"

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
        struct tcl tcl;
        char       cmd[128];
        char       history[128];
        bool       run;
        char      *args;
};

/*==============================================================================
  Exported object definitions
==============================================================================*/

/*==============================================================================
  Function definitions
==============================================================================*/
//==============================================================================
/**
 * @brief Command sleep selected amount of time.
 *
 * @param tcl   context container
 * @param args  argument list
 * @param arg   user argument
 *
 * @return One of F* status.
 */
//==============================================================================
static int tcl_cmd_sleep(struct tcl *tcl, tcl_value_t *args, void *arg)
{
        (void)tcl;
        (void)arg;

        tcl_value_t *time = tcl_list_at(args, 1);
        msleep((u32_t)(tcl_float(time) * 1000));
        tcl_free(time);
        return FNORMAL;
}

//==============================================================================
/**
 * @brief Function handle history. Check if command line contain AUP, ADN keys
 *        and set prompt to historical value
 *
 * @return true if history got, false is new command inserted
 */
//==============================================================================
static bool is_history_request()
{
        if (  strcmp(global->cmd, HISTORY_NEXT_KEY) == 0
           || strcmp(global->cmd, HISTORY_PREV_KEY) == 0) {

                if (strlch(global->history) == '\n') {
                        strlch(global->history) = '\0';
                }

                if (strlen(global->history)) {
                        ioctl(fileno(stdin), IOCTL_TTY__SET_EDITLINE, global->history);
                }

                return true;
        } else {
                if (strlen(global->cmd) > 1) {
                        if (strfch(global->cmd) != '\033') {
                                strcpy(global->history, global->cmd);
                        }
                }

                return false;
        }
}

//==============================================================================
/**
 * @brief TCL main function
 */
//==============================================================================
int_main(tcl, STACK_DEPTH_LARGE, int argc, char *argv[])
{
        tcl_init(&global->tcl);
        tcl_register_const(&global->tcl, "sleep", tcl_cmd_sleep, 2, NULL);

        if (argc > 1) {
                if (isstreq(argv[1], "-c")) {

                        if (argv[2]) {

                                strncpy(global->cmd, argv[2], sizeof(global->cmd));
                                strncat(global->cmd, "\n", sizeof(global->cmd));

                                if (tcl_eval(&global->tcl, global->cmd, strlen(global->cmd)) != FNORMAL) {

                                        if (!global->tcl.exit) {
                                                printf("Error: %s", global->cmd);
                                        }
                                }
                        } else {
                                puts("Error: command required");
                        }

                } else {

                        size_t arglen = 0;

                        for (int i = 2; (i < argc) && (argv[i] != NULL); i++) {
                                arglen += strlen(argv[i]) + 3;
                        }

                        if (arglen > 0) {
                                global->args = calloc(arglen + 1, sizeof(char));
                                if (!global->args) {
                                        goto exit;
                                }

                                for (int i = 2; (i < argc) && (argv[i] != NULL); i++) {
                                        strcat(global->args, "{");
                                        strcat(global->args, argv[i]);
                                        strcat(global->args, "} ");
                                }

                                tcl_var(&global->tcl, "args", tcl_alloc(global->args, strlen(global->args)));

                                free(global->args);
                                global->args = NULL;

                        } else {
                                tcl_var(&global->tcl, "args", tcl_alloc(" ", 1));
                        }

                        tcl_loadfile(&global->tcl, argv[1]);
                }
        } else {
                do {
                        printf("TCL> ");
                        fflush(stdout);

                        do {
                                fgets(global->cmd, sizeof(global->cmd), stdin);

                                if (is_history_request()) {
                                        continue;
                                }

                        } while (global->cmd[0] == '\033');

                        if (  tcl_eval(&global->tcl, global->cmd, strlen(global->cmd)) != FNORMAL
                           || tcl_eval(&global->tcl, "\n", strlen("\n")) != FNORMAL) {

                                if (!global->tcl.exit) {
                                        printf("Error: %s", global->cmd);
                                }
                        }
                } while (!global->tcl.exit);
        }

        exit:
        tcl_destroy(&global->tcl);

        return EXIT_SUCCESS;
}

/*==============================================================================
  End of file
==============================================================================*/
