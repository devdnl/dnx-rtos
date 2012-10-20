/*=============================================================================================*//**
@file    terminal.c

@author  Daniel Zorychta

@brief

@note    Copyright (C) 2012 Daniel Zorychta <daniel.zorychta@gmail.com>

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


*//*==============================================================================================*/

/*==================================================================================================
                                            Include files
==================================================================================================*/
#include "terminal.h"
#include <string.h>

/* Begin of application section declaration */
APPLICATION(terminal)
APP_SEC_BEGIN

/*==================================================================================================
                                  Local symbolic constants/macros
==================================================================================================*/
/** user prompt line size [B] */
#define PROMPT_LINE_SIZE            100


/*==================================================================================================
                                   Local types, enums definitions
==================================================================================================*/
typedef enum
{
      CMD_EXECUTED,
      CMD_NOT_EXIST,
      CMD_ALLOC_ERROR,
      CMD_EXIT,
} cmdStatus_t;


/*==================================================================================================
                                      Local object definitions
==================================================================================================*/


/*==================================================================================================
                                        Function definitions
==================================================================================================*/

//================================================================================================//
/**
 * @brief Function print prompt
 */
//================================================================================================//
void PrintPrompt(void)
{
      fontGreen();
      printf("root@%s: ", SystemGetHostname());
      resetAttr();
}


//================================================================================================//
/**
 * @brief Function find internal terminal commands
 *
 * @param *cmd          command
 * @param *arg          argument list
 *
 * @return operation status
 */
//================================================================================================//
cmdStatus_t FindInternalCmd(ch_t *cmd, ch_t *arg)
{
      /* exit command --------------------------------------------------------------------------- */
      if (strcmp("exit", cmd) == 0)
      {
            printf("Exit\n");
            return CMD_EXIT;
      }

      /* echo ----------------------------------------------------------------------------------- */
      if (strcmp("echo", cmd) == 0)
      {
            if (arg)
                  printf("%s\n", arg);
            else
                  printf("\n");
            return CMD_EXECUTED;
      }

      /* stack measurement ---------------------------------------------------------------------- */
      if (strcmp("stack", cmd) == 0)
      {
            printf("Free stack: %d\n", SystemGetStackFreeSpace());
            return CMD_EXECUTED;
      }

      /* system reboot -------------------------------------------------------------------------- */
      if (strcmp("reboot", cmd) == 0)
      {
            printf("Reboting...\n");
            Sleep(500);
            SystemReboot();
            return CMD_EXECUTED;
      }

      return CMD_NOT_EXIST;
}


//================================================================================================//
/**
 * @brief Function find external commands (registered applications)
 *
 * @param *cmd          command
 * @param *arg          argument list
 *
 * @return operation status
 */
//================================================================================================//
cmdStatus_t FindExternalCmd(ch_t *cmd, ch_t *arg)
{
      appArgs_t *appHdl;

      appHdl = Exec(cmd, arg);

      if (appHdl)
      {
            appHdl->tty = tty;

            while (appHdl->exitCode == STD_RET_UNKNOWN)
            {
                  Sleep(10);
            }

            FreeApphdl(appHdl);

            return CMD_EXECUTED;
      }
      else
      {
            return CMD_NOT_EXIST;
      }
}



//================================================================================================//
/**
 * @brief terminal main function
 */
//================================================================================================//
stdRet_t appmain(ch_t *argv)
{
      stdRet_t    termStatus = STD_RET_OK;
      cmdStatus_t cmdStatus;
      ch_t        *line;
      ch_t        *history;
      ch_t        *cmd;
      ch_t        *arg;

      /* allocate memory for input line */
      line    = Malloc(PROMPT_LINE_SIZE * sizeof(ch_t));
      history = Malloc(PROMPT_LINE_SIZE * sizeof(ch_t));

      if (!line || !history)
      {
            if (line)
            {
                  Free(line);
            }

            if (history)
            {
                  Free(history);
            }

            printf("No enough free memory\n");
            termStatus = STD_RET_ERROR;
            goto Terminal_Exit;
      }

      printf("Welcome to %s - kernel FreeRTOS (tty%d)\n", SystemGetHostname(), tty + 1);

      memset(history, ASCII_NULL, PROMPT_LINE_SIZE);

      /* main loop ------------------------------------------------------------------------------ */
      for (;;)
      {
            /* clear input line and print prompt */
            memset(line, ASCII_NULL, PROMPT_LINE_SIZE);
            PrintPrompt();

            /* waiting for command */
            scanf("%s3", line);

            /* check that history was call */
            if (strcmp(line, "\x1B[A") == 0)
            {
                  ch_t character;

                  strcpy(line, history);
                  printf("%s", history);

                  do
                  {
                        character = getChar();
                  }
                  while (!(character == ASCII_LF || character == ASCII_CR));

                  printf("\n");
            }
            else
            {
                  memcpy(history, line, PROMPT_LINE_SIZE);
            }

            /* finds all spaces before command */
            cmd = line;
            cmd += strspn(line, " ");

            /* finds first space after command */
            if ((arg = strchr(cmd, ' ')) != NULL)
            {
                  *(arg++) = ASCII_NULL;
                  arg += strspn(arg, " ");
            }
            else
            {
                  arg = strchr(cmd, ASCII_NULL);
            }

            /* check internal commands */
            if ((cmdStatus = FindInternalCmd(cmd, arg)) == CMD_EXECUTED)
            {
                  continue;
            }
            else if (cmdStatus == CMD_EXIT)
            {
                  break;
            }

            /* check external commands */
            if ((cmdStatus = FindExternalCmd(cmd, arg)) == CMD_EXECUTED)
            {
                  continue;
            }

            /* check status */
            if (cmdStatus == CMD_ALLOC_ERROR)
            {
                  printf("Not enough free memory to run application.\n");
            }
            else if (cmdStatus == CMD_NOT_EXIST)
            {
                  if (strlen(cmd) != 0)
                  {
                        printf("\"%s\" is unknown command.\n", cmd);
                  }
            }
      }

      /* free used memory */
      Free(line);
      Free(history);

      /* if stack size is debugging */
      if (ParseArg(argv, "stack", PARSE_AS_EXIST, NULL) == STD_RET_OK)
      {
            printf("Free stack: %d levels\n", SystemGetStackFreeSpace());
      }

      Terminal_Exit:
      return termStatus;
}

/* End of application section declaration */
APP_SEC_END


/*==================================================================================================
                                            End of file
==================================================================================================*/
