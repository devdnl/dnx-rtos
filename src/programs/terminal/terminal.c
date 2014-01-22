/*=========================================================================*//**
@file    terminal.c

@author  Daniel Zorychta

@brief

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
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <dirent.h>
#include <dnx/os.h>
#include <dnx/misc.h>

/*==============================================================================
  Local symbolic constants/macros
==============================================================================*/
#define PROMPT_LINE_LEN                 100
#define CWD_PATH_LEN                    128

#define set_cwd(const_char__pstr)       const char *__real_cwd = _task_get_data()->f_cwd;\
                                        _task_get_data()->f_cwd = const_char__pstr

#define restore_original_cwd()          _task_get_data()->f_cwd = __real_cwd

/*==============================================================================
  Local types, enums definitions
==============================================================================*/
enum cmd_status {
        CMD_STATUS_EXECUTED,
        CMD_STATUS_NOT_EXIST,
        CMD_STATUS_DO_EXIT
};

struct cmd_entry {
        const char *name;
        enum cmd_status (*const cmd)(char *arg);
};

/*==============================================================================
  Local function prototypes
==============================================================================*/
static void            print_prompt             (void);
static enum cmd_status find_internal_command    (const char *cmd);
static enum cmd_status find_external_command    (const char *cmd);
static enum cmd_status cmd_cd                   (char *arg);
static enum cmd_status cmd_help                 (char *arg);

/*==============================================================================
  Local object definitions
==============================================================================*/
GLOBAL_VARIABLES_SECTION_BEGIN

char line[PROMPT_LINE_LEN];
char history[PROMPT_LINE_LEN];
char cwd[CWD_PATH_LEN];

GLOBAL_VARIABLES_SECTION_END

static const struct cmd_entry commands[] = {
        {"cd"  , cmd_cd  },
        {"help", cmd_help},
};

/*==============================================================================
  Exported object definitions
==============================================================================*/

/*==============================================================================
  Function definitions
==============================================================================*/

//==============================================================================
/**
 * @brief Terminal main function
 */
//==============================================================================
PROGRAM_MAIN(terminal, int argc, char *argv[])
{
        (void) argc;
        (void) argv;

        bool prompt_enable = true;

        getcwd(global->cwd, CWD_PATH_LEN);

        for (;;) {
                /* clear input line and print prompt */
                memset(global->line, '\0', PROMPT_LINE_LEN);

                if (prompt_enable)
                        print_prompt();

                /* waiting for command */
                if (!fgets(global->line, PROMPT_LINE_LEN, stdin))
                        continue;

                LAST_CHARACTER(global->line) = '\0';
                if (strcmp(global->line, "\e^[A") == 0 || strcmp(global->line, "\e^[B") == 0) {
                        if (strlen(global->history)) {
                                ioctl(stdin, TTY_IORQ_SET_EDITLINE, global->history);
                        }

                        prompt_enable = false;
                        continue;
                } else {
                        prompt_enable = true;

                        if (strlen(global->line)) {
                                strcpy(global->history, global->line);
                        }
                }

                /* finds all spaces before command */
                char *cmd  = global->line;
                cmd += strspn(global->line, " ");

                if (cmd[0] == '\0') {
                        continue;
                }

                enum cmd_status cmd_status = find_external_command(cmd);

                if (cmd_status == CMD_STATUS_NOT_EXIST) {
                        cmd_status = find_internal_command(cmd);
                }

                switch (cmd_status) {
                case CMD_STATUS_EXECUTED:
                        continue;
                case CMD_STATUS_NOT_EXIST:
                        printf("\'%s\' is unknown command.\n", cmd);
                        break;
                case CMD_STATUS_DO_EXIT:
                        return 0;
                }
        }

        return 0;
}

//==============================================================================
/**
 * @brief Function print line prompt
 */
//==============================================================================
static void print_prompt(void)
{
        printf(FONT_COLOR_GREEN"root@%s:%s"RESET_ATTRIBUTES"\n", get_host_name(), global->cwd);
        printf(FONT_COLOR_GREEN"$ "RESET_ATTRIBUTES);
}

//==============================================================================
/**
 * @brief Function find external commands (registered applications)
 *
 * @param *cmd          command
 * @param *arg          argument list
 *
 * @return operation status
 */
//==============================================================================
static enum cmd_status find_external_command(const char *cmd)
{
        errno = 0;
        prog_t *prog = program_new(cmd, global->cwd, stdin, stdout, stderr);
        if (!prog) {
                if (errno == ENOENT) {
                        return CMD_STATUS_NOT_EXIST;
                } else {
                        perror(cmd);
                        return CMD_STATUS_EXECUTED;
                }
        }

        while (program_wait_for_close(prog, MAX_DELAY) != 0);

        program_delete(prog);

        ioctl(stdout, TTY_IORQ_ECHO_ON);

        return CMD_STATUS_EXECUTED;
}

//==============================================================================
/**
 * @brief Function find internal terminal commands
 *
 * @param *cmd          command
 * @param *arg          argument list
 *
 * @return operation status
 */
//==============================================================================
static enum cmd_status find_internal_command(const char *cmd)
{
        /* finds first space after command */
        char *arg;
        if ((arg = strchr(cmd, ' ')) != NULL) {
                *(arg++) = '\0';
                arg += strspn(arg, " ");
        } else {
                arg = strchr(cmd, '\0');
        }

        /* terminal exit */
        if (strcmp("exit", cmd) == 0) {
                return CMD_STATUS_DO_EXIT;
        }

        enum cmd_status status = CMD_STATUS_NOT_EXIST;

        for (uint i = 0; i < ARRAY_SIZE(commands); i++) {
                if (strcmp(cmd, commands[i].name) == 0) {

                        errno = 0;
                        set_cwd(global->cwd);
                        status = commands[i].cmd(arg);
                        restore_original_cwd();
                }
        }

        return status;
}

//==============================================================================
/**
 * @brief Function change current working path
 *
 * @param *arg          arguments
 */
//==============================================================================
static enum cmd_status cmd_cd(char *arg)
{
        char  *newpath   = NULL;
        bool   free_path = false;

        if (strcmp(arg, "..") == 0) {
                char *lastslash = strrchr(global->cwd, '/');
                if (lastslash) {
                        if (lastslash != global->cwd) {
                                *lastslash = '\0';
                         } else {
                                 *(lastslash + 1) = '\0';
                         }
                }
        } else if (strcmp(arg, ".") == 0) {
                /* do nothing */
        } else if (FIRST_CHARACTER(arg) != '/') {
                newpath = calloc(strlen(arg) + strlen(global->cwd) + 2, ARRAY_ITEM_SIZE(global->cwd));
                if (newpath) {
                        strcpy(newpath, global->cwd);

                        if (newpath[strlen(newpath) - 1] != '/') {
                                newpath[strlen(newpath)] = '/';
                        }

                        strcat(newpath, arg);

                        free_path = true;
                } else {
                        perror(NULL);
                }
        } else if (FIRST_CHARACTER(arg) == '/') {
                newpath = arg;
        } else {
                puts(strerror(ENOENT));
        }

        if (newpath) {
                DIR *dir = opendir(newpath);
                if (dir) {
                        closedir(dir);
                        strncpy(global->cwd, newpath, CWD_PATH_LEN);
                } else {
                        perror(arg);
                }

                if (free_path) {
                        free(newpath);
                }
        }

        return CMD_STATUS_EXECUTED;
}

//==============================================================================
/**
 * @brief Function listing all internal supported commands
 *
 * @param *arg          arguments
 */
//==============================================================================
static enum cmd_status cmd_help(char *arg)
{
        (void) arg;

        for (uint cmd = 0; cmd < ARRAY_SIZE(commands); cmd++) {
                printf("%s\n", commands[cmd].name);
        }

        return CMD_STATUS_EXECUTED;
}

/*==============================================================================
  End of file
==============================================================================*/
