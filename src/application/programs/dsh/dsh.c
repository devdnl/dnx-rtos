/*=========================================================================*//**
@file    dsh.c

@author  Daniel Zorychta

@brief   dnx RTOS Shell interpreter

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
#include <errno.h>
#include <unistd.h>
#include <ctype.h>
#include <dirent.h>
#include <stdbool.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <dnx/os.h>
#include <dnx/thread.h>
#include <dnx/misc.h>
#include <dnx/vt100.h>

/*==============================================================================
  Local symbolic constants/macros
==============================================================================*/
#define PROMPT_LINE_LEN                 100
#define CWD_PATH_LEN                    128
#define HISTORY_NEXT_KEY                "\033^[A"
#define HISTORY_PREV_KEY                "\033^[B"
#define COMMAND_HINT_KEY                "\033^[T"

/*==============================================================================
  Local types, enums definitions
==============================================================================*/

/*==============================================================================
  Local function prototypes
==============================================================================*/
static void  clear_prompt         ();
static void  print_prompt         ();
static bool  read_input           ();
static bool  history_request      ();
static bool  command_hint         ();
static bool  is_cd_cmd            (const char *cmd);
static bool  is_exit_cmd          (const char *cmd);
static bool  is_clear_cmd         (const char *cmd);
static bool  is_detached_cmd      (char *cmd);
static char *find_arg             (char *cmd);
static void  change_directory     (char *str);
static char *trim_string          (char *str);
static void  print_fail_message   (char *cmd);
static bool  start_program        (char *master, char *slave, char *std_in, char *file, char *fmode, bool detached);
static bool  analyze_line         (char *cmd);

/*==============================================================================
  Local object definitions
==============================================================================*/
GLOBAL_VARIABLES_SECTION {
        char            line[PROMPT_LINE_LEN];
        char            history[PROMPT_LINE_LEN];
        char            cwd[CWD_PATH_LEN];
        bool            prompt_enable;
        FILE           *input;
        bool            stream_closed;
        process_attr_t  pidmaster_attr;
        process_attr_t  pidslave_attr;
        const char     *pipe_file;
};

/*==============================================================================
  Exported object definitions
==============================================================================*/
PROGRAM_PARAMS(dsh, STACK_DEPTH_LOW);

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
        if (global->prompt_enable && global->input == stdin) {
                printf(VT100_FONT_COLOR_GREEN"%s@%s:%s"VT100_RESET_ATTRIBUTES"\n",
                       get_user_name(), get_host_name(), global->cwd);

                printf(VT100_FONT_COLOR_GREEN"$ "VT100_RESET_ATTRIBUTES);

                fflush(stdout);
        }
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
        if (global->stream_closed) {
                strcpy(global->line, "exit");
                return true;
        }

        if (fgets(global->line, PROMPT_LINE_LEN, global->input)) {

                if (feof(global->input)) {
                        global->stream_closed = true;
                }

                /* remove LF at the end of line */
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
 * @return true if history got, false is new command inserted
 */
//==============================================================================
static bool history_request()
{
        if (strcmp(global->line, HISTORY_NEXT_KEY) == 0 || strcmp(global->line, HISTORY_PREV_KEY) == 0) {
                if (strlen(global->history)) {
                        ioctl(fileno(global->input), IOCTL_TTY__SET_EDITLINE, global->history);
                }

                global->prompt_enable = false;

                return true;
        } else {
                global->prompt_enable = true;

                if (strlen(global->line)) {
                        if (global->line[0] != '\033') {
                                strcpy(global->history, global->line);
                        }
                }

                return false;
        }
}

//==============================================================================
/**
 * @brief Finds hint for typed command
 * @param None
 * @return true if hint key was recognized, otherwise false
 */
//==============================================================================
static bool command_hint()
{
        char *tabstart = strchr(global->line, '\033');
        if (tabstart) {
                if (strcmp(tabstart, COMMAND_HINT_KEY) == 0) {
                        *tabstart = '\0';

                        if (strlen(global->line) != 0) {
                                dirent_t *dirent;
                                int cnt  = 0;

                                DIR *dir = opendir("/proc/bin");
                                if (dir) {
                                        while ((dirent = readdir(dir))) {
                                                if (strncmp(dirent->d_name, global->line, strlen(global->line)) == 0) {
                                                        cnt++;
                                                }
                                        }

                                        closedir(dir);
                                }

                                dir = opendir("/proc/bin");
                                if (dir) {
                                        if (cnt > 1) {
                                                puts("");
                                        }

                                        while ((dirent = readdir(dir))) {

                                                if (strncmp(dirent->d_name, global->line, strlen(global->line)) == 0) {
                                                        if (cnt > 1) {
                                                                printf("%s ", dirent->d_name);
                                                        } else  {
                                                                ioctl(fileno(global->input), IOCTL_TTY__SET_EDITLINE, dirent->d_name);
                                                                break;
                                                        }
                                                }
                                        }

                                        closedir(dir);

                                        if (cnt > 1) {
                                                puts(" ");
                                                print_prompt();
                                                ioctl(fileno(global->input), IOCTL_TTY__REFRESH_LAST_LINE);
                                        }
                                }
                        }

                        global->prompt_enable = false;
                        return true;
                }
        }

        return false;
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
 * @brief Check if command is CLEAR
 *
 * @param  cmd
 *
 * @return true if EXIT command, otherwise false
 */
//==============================================================================
static bool is_clear_cmd(const char *cmd)
{
        return strncmp(cmd, "clear ", 6) == 0 || strcmp(cmd, "clear") == 0;
}

//==============================================================================
/**
 * @brief Check if line is commented.
 *
 * @param  cmd
 *
 * @return True when commented, otherwise false.
 */
//==============================================================================
static bool is_comment(const char *cmd)
{
        return cmd[0] == '#';
}

//==============================================================================
/**
 * @brief  Find ampersand at end of string and replace it by null.
 *
 * @param  cmd  Command string. String modified when ampersand is found.
 *
 * @return True when ampersand found, otherwise false.
 */
//==============================================================================
static bool is_detached_cmd(char *cmd)
{
        size_t len  = strlen(cmd);
        char  *back = &cmd[len - 1];
        while (len-- && isspace(cast(int, *back)) && --back);

        if (*back == '&') {
                *back = '\0';
                return true;
        } else {
                return false;
        }
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
        char *arg = find_arg(str);

        memset(global->cwd, 0, sizeof(global->cwd));

        if (arg[0] == '/') {
                strlcpy(global->cwd, arg, sizeof(global->cwd));
        } else {
                getcwd(global->cwd, sizeof(global->cwd));
                strlcat(global->cwd, "/", sizeof(global->cwd));
                strlcat(global->cwd, arg, sizeof(global->cwd));
                strlcat(global->cwd, "/", sizeof(global->cwd));
        }

        DIR *dir = opendir(global->cwd);
        if (dir) {
                closedir(dir);
                chdir(global->cwd);
        } else {
                perror(arg);
        }

        getcwd(global->cwd, sizeof(global->cwd));
}

//==============================================================================
/**
 * @brief Remove leading and trailing spaces.
 *
 * @param str           string to trim
 *
 * @return New string pointer (basing on the same buffer).
 */
//==============================================================================
static char *trim_string(char *str)
{
        str = str + strspn(str, " ");

        for (int i = strlen(str) - 1; i > 0; i--) {
                if (isspace(str[i])) {
                        str[i] = '\0';
                } else {
                        break;
                }
        }

        return str;
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
        cmd = trim_string(cmd);

        if (strchr(cmd, ' '))
                *strchr(cmd, ' ') = '\0';

        if (errno == ENOMEM || errno == EACCES) {
                perror(cmd);
        } else {
                printf("\'%s\' is unknown command.\n", cmd);
        }
}

//==============================================================================
/**
 * @brief Function start defined program group
 *
 * @param master        master program name
 * @param slave         slave program name
 * @param file          output file
 * @param fmode         file mode
 * @param detached      detached operation
 *
 * @return true if command executed, false if error
 */
//==============================================================================
static bool start_program(char *master, char *slave, char *std_in,
                          char *file,   char *fmode, bool detached)
{
        errno              = 0;
        FILE    *pipe      = NULL;
        FILE    *fout      = NULL;
        char    *pipe_name = NULL;
        bool     status    = true;
        pid_t    pidmaster = 0;
        pid_t    pidslave  = 0;

        if (master) {
                master = trim_string(master);
        }

        if (slave) {
                slave = trim_string(slave);
        }

        if (file) {
                file = trim_string(file);
        }

        if (std_in) {
                std_in = trim_string(std_in);
        }

        if (std_in == NULL && file) {
                fout = fopen(file, fmode);
                if (!fout) {
                        perror(file);
                        goto free_resources;
                }
        }

        if (master && slave) {
                pipe_name = calloc(sizeof(char), strlen(global->pipe_file) + 7);
                if (pipe_name) {
                        u64_t uptime = get_time_ms();
                        snprintf(pipe_name, strlen(global->pipe_file) + 7, "%s%lx", global->pipe_file, uptime);

                        if (mkfifo(pipe_name, 0666)) {
                                perror("sh");
                                goto free_resources;
                        }

                        pipe = fopen(pipe_name, "r+");
                        if (!pipe) {
                                perror("sh");
                                goto free_resources;
                        }
                } else {
                        perror("sh");
                        goto free_resources;
                }
        }

        memset(&global->pidmaster_attr, 0, sizeof(process_attr_t));

        if (master && slave && file) {
                global->pidmaster_attr.f_stdin    = stdin;
                global->pidmaster_attr.f_stdout   = pipe;
                global->pidmaster_attr.f_stderr   = stderr;
                global->pidmaster_attr.cwd        = global->cwd;
                global->pidmaster_attr.detached   = false;
                global->pidmaster_attr.priority   = PRIORITY_NORMAL;
                pidmaster = process_create(master, &global->pidmaster_attr);
                if (pidmaster == 0) {
                        print_fail_message(master);
                        goto free_resources;
                }

                global->pidslave_attr.f_stdin    = pipe;
                global->pidslave_attr.f_stdout   = fout;
                global->pidslave_attr.f_stderr   = stderr;
                global->pidslave_attr.cwd        = global->cwd;
                global->pidslave_attr.detached   = false;
                global->pidslave_attr.priority   = PRIORITY_NORMAL;
                pidslave = process_create(slave, &global->pidslave_attr);
                if (pidslave == 0) {
                        process_kill(pidmaster);
                        print_fail_message(slave);
                        goto free_resources;
                }

                process_wait(pidmaster, NULL, MAX_DELAY_MS);
                ioctl(fileno(pipe), IOCTL_PIPE__CLOSE);
                process_wait(pidslave, NULL, MAX_DELAY_MS);

        } else if (master && slave) {
                global->pidmaster_attr.f_stdin    = stdin;
                global->pidmaster_attr.f_stdout   = pipe;
                global->pidmaster_attr.f_stderr   = stderr;
                global->pidmaster_attr.cwd        = global->cwd;
                global->pidmaster_attr.detached   = false;
                global->pidmaster_attr.priority   = PRIORITY_NORMAL;
                pidmaster = process_create(master, &global->pidmaster_attr);
                if (pidmaster == 0) {
                        print_fail_message(master);
                        goto free_resources;
                }

                global->pidslave_attr.f_stdin    = pipe;
                global->pidslave_attr.f_stdout   = stdout;
                global->pidslave_attr.f_stderr   = stderr;
                global->pidslave_attr.cwd        = global->cwd;
                global->pidslave_attr.detached   = false;
                global->pidslave_attr.priority   = PRIORITY_NORMAL;
                pidslave = process_create(slave, &global->pidslave_attr);
                if (pidslave == 0) {
                        process_kill(pidmaster);
                        print_fail_message(slave);
                        goto free_resources;
                }

                process_wait(pidmaster, NULL, MAX_DELAY_MS);
                ioctl(fileno(pipe), IOCTL_PIPE__CLOSE);
                process_wait(pidslave, NULL, MAX_DELAY_MS);

        } else if (master && file && std_in) {
                global->pidmaster_attr.p_stdin    = std_in;
                global->pidmaster_attr.p_stdout   = file;
                global->pidmaster_attr.p_stderr   = file;
                global->pidmaster_attr.cwd        = global->cwd;
                global->pidmaster_attr.detached   = detached;
                global->pidmaster_attr.priority   = PRIORITY_NORMAL;
                pidmaster = process_create(master, &global->pidmaster_attr);
                if (pidmaster == 0) {
                        print_fail_message(master);
                        goto free_resources;
                }

                if (not detached) {
                        process_wait(pidmaster, NULL, MAX_DELAY_MS);
                }

        } else if (master && std_in) {
                global->pidmaster_attr.p_stdin    = std_in;
                global->pidmaster_attr.f_stdout   = stdout;
                global->pidmaster_attr.f_stderr   = stderr;
                global->pidmaster_attr.cwd        = global->cwd;
                global->pidmaster_attr.detached   = detached;
                global->pidmaster_attr.priority   = PRIORITY_NORMAL;
                pidmaster = process_create(master, &global->pidmaster_attr);
                if (pidmaster == 0) {
                        print_fail_message(master);
                        goto free_resources;
                }

                if (not detached) {
                        process_wait(pidmaster, NULL, MAX_DELAY_MS);
                }

        } else if (master && file) {
                global->pidmaster_attr.f_stdin    = stdin;
                global->pidmaster_attr.f_stdout   = fout;
                global->pidmaster_attr.f_stderr   = stderr;
                global->pidmaster_attr.cwd        = global->cwd;
                global->pidmaster_attr.detached   = false;
                global->pidmaster_attr.priority   = PRIORITY_NORMAL;
                pidmaster = process_create(master, &global->pidmaster_attr);
                if (pidmaster == 0) {
                        print_fail_message(master);
                        goto free_resources;
                }

                process_wait(pidmaster, NULL, MAX_DELAY_MS);

        } else if (master) {
                global->pidmaster_attr.f_stdin    = detached ? NULL : stdin;
                global->pidmaster_attr.f_stdout   = detached ? NULL : stdout;
                global->pidmaster_attr.f_stderr   = detached ? NULL : stderr;
                global->pidmaster_attr.cwd        = global->cwd;
                global->pidmaster_attr.detached   = detached;
                global->pidmaster_attr.priority   = PRIORITY_NORMAL;
                pidmaster = process_create(master, &global->pidmaster_attr);
                if (pidmaster == 0) {
                        print_fail_message(master);
                        goto free_resources;
                }

                if (not detached) {
                        process_wait(pidmaster, NULL, MAX_DELAY_MS);
                }

        } else {
                status = false;
        }

free_resources:
        if (fout) {
                fclose(fout);
        }

        if (pipe) {
                fclose(pipe);
                remove(pipe_name);
        }

        if (pipe_name) {
                free(pipe_name);
        }

        return status;
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
        int   pipe_number = 0;
        int   out_number  = 0;
        int   in_number   = 0;
        bool  detached    = is_detached_cmd(cmd);
        char *fmode       = "";

        size_t n = strlen(cmd);
        for (size_t i = 0; i < n; i++) {
                if (cmd[i] == '|') {
                        pipe_number++;

                } else if (cmd[i + 0] == '>' && cmd[i + 1] == '>') {
                        out_number++;
                        fmode = "a";
                        i++;

                } else if (cmd[i] == '>') {
                        out_number++;
                        fmode = "w";

                } else if (cmd[i] == '<') {
                        in_number++;
                }
        }

        if (pipe_number > 1 || out_number > 1 || in_number > 1) {
                puts("sh: syntax error");
                return true;
        }

        if (in_number) {
                if (pipe_number) {
                        puts("sh: syntax error");
                        return true;

                } else if (out_number) {
                        char *master = cmd;

                        char *std_in = strchr(master, '<');
                        char *file = strchr(master, '>');
                        *std_in++ = '\0';
                        *file++ = '\0';

                        return start_program(master, NULL, std_in, file, fmode, detached);
                } else {
                        char *master = cmd;

                        char *std_in = strchr(master, '<');
                        *std_in++ = '\0';

                        return start_program(master, NULL, std_in, NULL, fmode, detached);
                }
        }

        if (out_number && pipe_number) {
                if ((strchr(cmd, '|') > strchr(cmd, '>')) || strchr(cmd, '|') == cmd) {
                        puts("sh: syntax error");
                        return true;
                } else {
                        char *master = cmd;

                        char *slave = strchr(master, '|');
                        *slave++ = '\0';

                        char *file = strchr(slave, '>');
                        *file++ = '\0';

                        return start_program(master, slave, NULL, file, fmode, detached);
                }
        }

        if (out_number) {
                if (strchr(cmd, '>') == cmd) {
                        puts("sh: syntax error");
                        return true;
                } else {
                        char *master = cmd;

                        char *file = strchr(master, '>');
                        *file++ = '\0';

                        if (*file == '>') {
                                *file++ = '\0';
                        }

                        return start_program(master, NULL, NULL, file, fmode, detached);
                }
        }

        if (pipe_number) {
                if (strchr(cmd, '|') == cmd) {
                        puts("sh: syntax error");
                        return true;
                } else {
                        char *master = cmd;

                        char *slave = strchr(master, '|');
                        *slave++ = '\0';

                        return start_program(master, slave, NULL, NULL, fmode, detached);
                }
        }

        if (strlen(cmd)) {
                return start_program(cmd, NULL, NULL, NULL, fmode, detached);
        } else {
                return false;
        }
}

//==============================================================================
/**
 * @brief Terminal main function
 */
//==============================================================================
int main(int argc, char *argv[])
{
        global->prompt_enable = true;
        global->input         = stdin;
        global->pipe_file     = "/run/dsh-";

        if (argc >= 2) {
                for (int i = 1; i < argc; i++) {
                        if (isstreq(argv[i], "-e")) {

                                memset(global->line, 0, sizeof(global->line));

                                for (int n = i + 1; n < argc; n++) {
                                        strlcat(global->line, argv[n], sizeof(global->line));
                                        strlcat(global->line, " ", sizeof(global->line));
                                }

                                char *cmd = trim_string(global->line);

                                if (!analyze_line(cmd)) {
                                        print_fail_message(argv[i+1]);
                                }

                                exit(0);
                        }
                }

                global->input = fopen(argv[1], "r");
                if (!global->input) {
                        perror(argv[1]);
                        return EXIT_FAILURE;
                }
        }

        mkdir("/run", 0666);

        getcwd(global->cwd, CWD_PATH_LEN);
        chdir(global->cwd);

        for (;;) {
                clear_prompt();

                print_prompt();

                if (!read_input())
                        break;

                if (history_request())
                        continue;

                if (command_hint())
                        continue;

                char *cmd = trim_string(global->line);

                if (strlen(cmd) == 0)
                        continue;

                if (is_comment(cmd)) {
                        continue;
                }

                if (is_cd_cmd(cmd)) {
                        change_directory(cmd);
                        continue;
                }

                if (is_exit_cmd(cmd)) {
                        break;
                }

                if (is_clear_cmd(cmd)) {
                        ioctl(fileno(stdout), IOCTL_TTY__CLEAR_SCR);
                        continue;
                }

                if (analyze_line(cmd)) {
                        continue;
                }

                print_fail_message(cmd);
        }

        if (global->input != stdin) {
                fclose(global->input);
        }

        return 0;
}

/*==============================================================================
  End of file
==============================================================================*/
