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

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Include files
==============================================================================*/
#include "terminal.h"
#include "tty_def.h"
#include <string.h>

/*==============================================================================
  Local symbolic constants/macros
==============================================================================*/
#define PROMPT_LINE_LEN                 100
#define CWD_PATH_LEN                    128

/*==============================================================================
  Local types, enums definitions
==============================================================================*/
enum cmd_status {
        CMD_EXECUTED,
        CMD_NOT_EXIST,
        CMD_NOT_ENOUGH_FREE_MEMORY
};

struct cmd_entry {
        const char *name;
        enum cmd_status (*const cmd)(char *arg);
};

/*==============================================================================
  Local function prototypes
==============================================================================*/
static void print_prompt(void);
static enum cmd_status find_internal_command(char *cmd, char *arg);
static enum cmd_status find_external_command(ch_t *cmd, ch_t *arg);
static enum cmd_status cmd_uptime(char *arg);
static enum cmd_status cmd_reboot(char *arg);

/*==============================================================================
  Local object definitions
==============================================================================*/
GLOBAL_VARIABLES {
        char line[PROMPT_LINE_LEN];
        char cwd[CWD_PATH_LEN];
};

/*==============================================================================
  Exported object definitions
==============================================================================*/
PROGRAM_PARAMS(terminal, STACK_DEPTH_MEDIUM);

/*==============================================================================
  Function definitions
==============================================================================*/

//==============================================================================
/**
 * @brief Terminal main function
 */
//==============================================================================
int PROGRAM_MAIN(terminal, int argc, char *argv[])
{
        char *cmd;
        char *arg;
        u8_t  tty = 0;
        enum cmd_status cmd_status;

        strcpy(global->cwd, "/");

        ioctl(stdin, TTY_IORQ_GETCURRENTTTY, &tty);

        printf("Welcome to %s/%s (tty%u)\n", get_OS_name(), get_kernel_name(), tty);

        for (;;) {
              /* clear input line and print prompt */
              memset(global->line, '\0', PROMPT_LINE_LEN);
              print_prompt();

              /* waiting for command */
              scanf("%100s", global->line);

              /* finds all spaces before command */
              cmd  = global->line;
              cmd += strspn(global->line, " ");

              /* finds first space after command */
              if ((arg = strchr(cmd, ' ')) != NULL) {
                    *(arg++) = '\0';
                    arg += strspn(arg, " ");
              } else {
                    arg = strchr(cmd, '\0');
              }

              /* terminal exit */
              if (strcmp("exit", cmd) == 0) {
                      break;
              }

              if (strcmp("", cmd) == 0) {
                      continue;
              }

              /* identify program localization */
              cmd_status = find_internal_command(cmd, arg);

              if (cmd_status == CMD_NOT_EXIST) {
                      cmd_status = find_external_command(cmd, arg);
              }

              switch (cmd_status) {
              case CMD_EXECUTED:
                      continue;

              case CMD_NOT_EXIST:
                      printf("\"%s\" is unknown command.\n", cmd);
                      break;

              case CMD_NOT_ENOUGH_FREE_MEMORY:
                      printf("Not enough free memory.\n");
                      break;
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
 * @brief Function find internal terminal commands
 *
 * @param *cmd          command
 * @param *arg          argument list
 *
 * @return operation status
 */
//==============================================================================
static enum cmd_status find_internal_command(char *cmd, char *arg)
{
      const struct cmd_entry commands[] = {
//            {"echo"  , cmdECHO  },
//            {"stack" , cmdSTACK },
//            {"cd"    , cmdCD    },
//            {"ls"    , cmdLS    },
//            {"mkdir" , cmdMKDIR },
//            {"touch" , cmdTOUCH },
//            {"rm"    , cmdRM    },
//            {"free"  , cmdFREE  },
            {"uptime", cmd_uptime},
//            {"clear" , cmdCLEAR },
            {"reboot", cmd_reboot},
//            {"df"    , cmdDF    },
//            {"mount" , cmdMOUNT },
//            {"umount", cmdUMOUNT},
      };

      for (uint i = 0; i < ARRAY_SIZE(commands); i++) {
            if (strcmp(cmd, commands[i].name) == 0) {
                  return commands[i].cmd(arg);
            }
      }

      return CMD_NOT_EXIST;
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
static enum cmd_status find_external_command(ch_t *cmd, ch_t *arg)
{
//      cmdStatus_t status = CMD_NOT_EXIST;
//
//      suspend_all_tasks();
//
//      prog_t *appHdl = exec(cmd, arg);
//
//      if (appHdl)
//      {
//            appHdl->stdin  = stdin;
//            appHdl->stdout = stdout;
//            appHdl->cwd    = cdpath;
//            resume_all_tasks();
//
//            /* waiting for application exit */
//            while (appHdl->exitCode == STD_RET_UNKNOWN)
//            {
//                  /* check if application exist */
//                  if (get_task_name(appHdl->taskHandle) == NULL) {
//                        break;
//                  }
//
//                  sleep(1);
//            }
//
//            delete_program(appHdl);
//
//            status = CMD_EXECUTED;
//      } else {
//            resume_all_tasks();
//      }
//
//      return status;
        return CMD_NOT_EXIST;
}

//==============================================================================
/**
 * @brief Function show uptime
 *
 * @param *arg          arguments
 */
//==============================================================================
static enum cmd_status cmd_uptime(char *arg)
{
      (void) arg;

      u32_t uptime = get_uptime();
      u32_t udays  = (uptime / (3600 * 24));
      u32_t uhrs   = (uptime / 3600) % 24;
      u32_t umins  = (uptime / 60) % 60;

      printf("up %ud %u2:%u2\n", udays, uhrs, umins);

      return CMD_EXECUTED;
}

//==============================================================================
/**
 * @brief Function reboot system
 *
 * @param *arg          arguments
 */
//==============================================================================
static enum cmd_status cmd_reboot(char *arg)
{
      (void) arg;

      printf("Rebooting...\n");
      milisleep(500);
      reboot();

      return CMD_EXECUTED;
}

#ifdef __cplusplus
}
#endif

/*==============================================================================
  End of file
==============================================================================*/
