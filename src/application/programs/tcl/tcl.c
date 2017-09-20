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
        char  cmd[128];
        bool  run;
        char *args;
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
 * @brief TCL main function
 */
//==============================================================================
int_main(tcl, STACK_DEPTH_MEDIUM, int argc, char *argv[])
{
        struct tcl tcl;

        tcl_init(&tcl);
        tcl_register_const(&tcl, "sleep", tcl_cmd_sleep, 2, NULL);

        if (argc > 1) {
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

                        tcl_var(&tcl, "args", tcl_alloc(global->args, strlen(global->args)));

                        free(global->args);
                        global->args = NULL;

                } else {
                        tcl_var(&tcl, "args", tcl_alloc(" ", 1));
                }

                tcl_loadfile(&tcl, argv[1]);
        } else {
                do {
                        printf("TCL> ");
                        fflush(stdout);

                        do {
                                fgets(global->cmd, sizeof(global->cmd), stdin);
                        } while (global->cmd[0] == '\033');

                        if (  tcl_eval(&tcl, global->cmd, strlen(global->cmd)) != FNORMAL
                           || tcl_eval(&tcl, "\n", strlen("\n")) != FNORMAL) {

                                if (!tcl.exit) {
                                        printf("Error: %s", global->cmd);
                                }
                        }
                } while (!tcl.exit);
        }

        exit:
        tcl_destroy(&tcl);

        return EXIT_SUCCESS;
}

/*==============================================================================
  End of file
==============================================================================*/
