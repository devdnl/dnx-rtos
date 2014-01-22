/*=========================================================================*//**
@file    sh.c

@author  Daniel Zorychta

@brief   Shell interpreter

@note    Copyright (C) 2014 Daniel Zorychta <daniel.zorychta@gmail.com>

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
#include <dirent.h>
#include <stdbool.h>
#include <sys/ioctl.h>
#include <dnx/os.h>
#include <dnx/thread.h>
#include <dnx/misc.h>
#include <sys/stat.h>

/*==============================================================================
  Local symbolic constants/macros
==============================================================================*/
#define PROMPT_LINE_LEN                 100
#define CWD_PATH_LEN                    128
#define HISTORY_NEXT_KEY                "\e^[A"
#define HISTORY_PREV_KEY                "\e^[B"
#define ETX                             0x03

/*==============================================================================
  Local types, enums definitions
==============================================================================*/

/*==============================================================================
  Local function prototypes
==============================================================================*/
static void             clear_prompt            ();
static void             print_prompt            ();
static const char      *get_user_name           ();
static bool             read_input              ();
static bool             history_request         ();
static char            *find_cmd_begin          ();
static bool             is_cd_cmd               (const char *cmd);
static bool             is_exit_cmd             (const char *cmd);
static char            *find_arg                (char *cmd);
static void             change_directory        (char *str);
static void             print_fail_message      (char *cmd);
static bool             start_program           (const char *master, const char *slave, const char *file);
static bool             analyze_line            (char *cmd);

/*==============================================================================
  Local object definitions
==============================================================================*/
GLOBAL_VARIABLES_SECTION_BEGIN

char  line[PROMPT_LINE_LEN];
char  history[PROMPT_LINE_LEN];
char  cwd[CWD_PATH_LEN];
bool  prompt_enable;
FILE *input;

GLOBAL_VARIABLES_SECTION_END

/*==============================================================================
  Exported object definitions
==============================================================================*/

/*==============================================================================
  Function definitions
==============================================================================*/

//==============================================================================
/**
 * @brief Clear prompt line
 */
//==============================================================================
static void clear_prompt()
{
        memset(global->line, '\0', PROMPT_LINE_LEN);
}

//==============================================================================
/**
 * @brief Function print line prompt
 */
//==============================================================================
static void print_prompt(void)
{
        if (global->prompt_enable) {
                printf(FONT_COLOR_GREEN"%s@%s:%s"RESET_ATTRIBUTES"\n", get_user_name(), get_host_name(), global->cwd);
                printf(FONT_COLOR_GREEN"$ "RESET_ATTRIBUTES);
        }
}

//==============================================================================
/**
 * @brief Return user name
 *
 * @return user name string
 */
//==============================================================================
static const char *get_user_name()
{
        return "root";
}

//==============================================================================
/**
 * @brief Read command from selected file
 *
 * @return true if string read, false if not
 */
//==============================================================================
static bool read_input()
{
        if (fgets(global->line, PROMPT_LINE_LEN, global->input)) {
                /* remove LF at the on of line */
                LAST_CHARACTER(global->line) = '\0';

                return true;
        } else {
                return false;
        }
}

//==============================================================================
/**
 * @brief Function handle history. Check if command line contain AUP, ADN keys
 *        and set prompt to historical value
 *
 * return true if history got, false is new command inserted
 */
//==============================================================================
static bool history_request()
{
        if (strcmp(global->line, HISTORY_NEXT_KEY) == 0 || strcmp(global->line, HISTORY_PREV_KEY) == 0) {
                if (strlen(global->history)) {
                        ioctl(global->input, TTY_IORQ_SET_EDITLINE, global->history);
                }

                global->prompt_enable = false;

                return true;
        } else {
                global->prompt_enable = true;

                if (strlen(global->line)) {
                        strcpy(global->history, global->line);
                }

                return false;
        }
}

//==============================================================================
/**
 * @brief Finds command start (ignore first spaces if exist)
 *
 * @return pointer to commnad in command line
 */
//==============================================================================
static char *find_cmd_begin()
{
        char *cmd  = global->line;
        cmd       += strspn(global->line, " ");

        return cmd;
}

//==============================================================================
/**
 * @brief Check if command is CD
 *
 * @param cmd
 *
 * @return true if CD command, otherwise false
 */
//==============================================================================
static bool is_cd_cmd(const char *cmd)
{
        return strncmp(cmd, "cd ", 3) == 0 || strcmp(cmd, "cd") == 0;
}

//==============================================================================
/**
 * @brief Check if command is EXIT
 *
 * @param cmd
 *
 * @return true if EXIT command, otherwise false
 */
//==============================================================================
static bool is_exit_cmd(const char *cmd)
{
        return strncmp(cmd, "exit ", 5) == 0 || strcmp(cmd, "exit") == 0;
}

//==============================================================================
/**
 * @brief Finds arguments after command
 *
 * @param cmd           command line begin
 *
 * @return pointer to first argument
 */
//==============================================================================
static char *find_arg(char *cmd)
{
        char *arg;
        if ((arg = strchr(cmd, ' ')) != NULL) {
                *(arg++) = '\0';
                arg += strspn(arg, " ");
        } else {
                arg = strchr(cmd, '\0');
        }

        return arg;
}

//==============================================================================
/**
 * @brief Function change current working path
 *
 * @param str          command name with arguments
 */
//==============================================================================
static void change_directory(char *str)
{
        char  *newpath   = NULL;
        bool   free_path = false;

        char *arg = find_arg(str);

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
}

//==============================================================================
/**
 * @brief Function print command find failure
 *
 * @param cmd           cmd string
 */
//==============================================================================
static void print_fail_message(char *cmd)
{
        if (strchr(cmd, ' '))
                *strchr(cmd, ' ') = '\0';

        printf("\'%s\' is unknown command.\n", cmd);
}

//==============================================================================
/**
 * @brief Function start defined program group
 *
 * @param cmd           command line
 *
 * @return true if command executed, false if error
 */
//==============================================================================
static bool start_program(const char *master, const char *slave, const char *file)
{
        errno     = 0;
        FILE *in  = stdin;
        FILE *out = stdout;

        if (file) {
                printf("File  : %s\n", file);
        }

        if (master && slave) {
                if (mkfifo("/tmp/sh", 0666) != 0) {
                        perror("sh");
                        return false;
                }

                in = fopen("/tmp/sh", "r+");
                if (!in) {
                        remove("/tmp/sh");
                        perror("sh");
                        return false;
                }

                prog_t *pm = program_new(master, global->cwd, stdin, in, in);
                prog_t *ps = program_new(slave, global->cwd, in, stdout, stdout);


                while (program_wait_for_close(pm, 1000));
                u8_t etx = ETX;
                fwrite(&etx, 1, 1, in);

                while (program_wait_for_close(ps, 1000));

                fclose(in);

                if (remove("/tmp/sh")) {
                        /* if error retry again */
                        remove("/tmp/sh");
                }

                return true;
        }

        if (master && slave && file) {

        }

        if (master && file) {

        }

        return false;
}

//==============================================================================
/**
 * @brief Function analyze line
 *
 * @param cmd           command line
 *
 * @return true if command executed, false if error
 */
//==============================================================================
static bool analyze_line(char *cmd)
{
        int pipe_number = 0;
        int out_number  = 0;

        for (size_t i = 0; i < strlen(cmd); i++) {
                if (cmd[i] == '|')
                        pipe_number++;

                if (cmd[i] == '>')
                        out_number++;
        }

        if (pipe_number > 1 || out_number > 1) {
                puts("Syntax error");
                return true;
        }

        if (out_number && pipe_number) {
                if ((strchr(cmd, '|') > strchr(cmd, '>')) || strchr(cmd, '|') == cmd) {
                        puts("Syntax error");
                        return true;
                } else {
                        char *master = cmd;

                        char *slave = strchr(master, '|');
                        *slave++ = '\0';

                        char *file = strchr(slave, '>');
                        *file++ = '\0';

                        return start_program(master, slave, file);
                }
        }

        if (out_number) {
                if (strchr(cmd, '>') == cmd) {
                        puts("Syntax error");
                        return true;
                } else {
                        char *master = cmd;

                        char *file = strchr(master, '>');
                        *file++ = '\0';

                        return start_program(master, NULL, file);
                }
        }

        if (pipe_number) {
                if (strchr(cmd, '|') == cmd) {
                        puts("Syntax error");
                        return true;
                } else {
                        char *master = cmd;

                        char *slave = strchr(master, '|');
                        *slave++ = '\0';

                        return start_program(master, slave, NULL);
                }
        }

        if (strlen(cmd)) {
                return start_program(cmd, NULL, NULL);
        } else {
                return false;
        }
}










////==============================================================================
///**
// * @brief Function find external commands (registered applications)
// *
// * @param *cmd          command
// * @param *arg          argument list
// *
// * @return operation status
// */
////==============================================================================
//static enum cmd_status find_external_command(const char *cmd)
//{
//        errno = 0;
//        prog_t *prog = program_new(cmd, global->cwd, stdin, stdout, stderr);
//        if (!prog) {
//                if (errno == ENOENT) {
//                        return CMD_STATUS_NOT_EXIST;
//                } else {
//                        perror(cmd);
//                        return CMD_STATUS_EXECUTED;
//                }
//        }
//
//        while (program_wait_for_close(prog, MAX_DELAY) != 0);
//
//        program_delete(prog);
//
//        ioctl(stdout, TTY_IORQ_ECHO_ON);
//
//        return CMD_STATUS_EXECUTED;
//}
//
////==============================================================================
///**
// * @brief Function find internal terminal commands
// *
// * @param *cmd          command
// * @param *arg          argument list
// *
// * @return operation status
// */
////==============================================================================
//static enum cmd_status find_internal_command(const char *cmd)
//{
//        /* finds first space after command */
//        char *arg;
//        if ((arg = strchr(cmd, ' ')) != NULL) {
//                *(arg++) = '\0';
//                arg += strspn(arg, " ");
//        } else {
//                arg = strchr(cmd, '\0');
//        }
//
//        /* terminal exit */
//        if (strcmp("exit", cmd) == 0) {
//                return CMD_STATUS_DO_EXIT;
//        }
//
//        enum cmd_status status = CMD_STATUS_NOT_EXIST;
//
//        for (uint i = 0; i < ARRAY_SIZE(commands); i++) {
//                if (strcmp(cmd, commands[i].name) == 0) {
//
//                        errno = 0;
//                        set_cwd(global->cwd);
//                        status = commands[i].cmd(arg);
//                        restore_original_cwd();
//                }
//        }
//
//        return status;
//}

//==============================================================================
/**
 * @brief Terminal main function
 */
//==============================================================================
PROGRAM_MAIN(sh, int argc, char *argv[])
{
        (void) argc;
        (void) argv;

        global->prompt_enable = true;
        global->input         = stdin;

        getcwd(global->cwd, CWD_PATH_LEN);

        for (;;) {
                clear_prompt();

                print_prompt();

                if (!read_input())
                        continue;

                if (history_request())
                        continue;

                char *cmd = find_cmd_begin();

                if (strlen(cmd) == 0)
                        continue;

                if (is_cd_cmd(cmd)) {
                        change_directory(cmd);
                        continue;
                }

                if (is_exit_cmd(cmd)) {
                        return 0;
                }

                if (analyze_line(cmd)) {
                        continue;
                }

                print_fail_message(cmd);
        }

        return 0;
}

/*==============================================================================
  End of file
==============================================================================*/
