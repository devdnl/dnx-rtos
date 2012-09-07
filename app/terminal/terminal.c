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

#include "MPL115A2.h" /* DNLTEST wywalic po testach czujnika */

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
      print("root@board: ");
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
            print("Exit\n");
            return CMD_EXIT;
      }

      /* echo ----------------------------------------------------------------------------------- */
      if (strcmp("echo", cmd) == 0)
      {
            if (arg)
                  print("%s\n", arg);
            else
                  print("\n");
            return CMD_EXECUTED;
      }

      /* stack measurement ---------------------------------------------------------------------- */
      if (strcmp("stack", cmd) == 0)
      {
            print("Free stack: %d\n", SystemGetStackFreeSpace());
            return CMD_EXECUTED;
      }

      /* system reboot -------------------------------------------------------------------------- */
      if (strcmp("reboot", cmd) == 0)
      {
            print("Reboting...\n");
            Sleep(1000);
            SystemReboot();
            return CMD_EXECUTED;
      }

      /* DNLTEST temperature -------------------------------------------------------------------- */
      if (strcmp("temp", cmd) == 0)
      {
            i8_t temp = 0;

            if (MPL115A2_GetTemperature(&temp) == STD_RET_OK)
            {
                  print("Temperature: %d^C; 0x%x\n", (i32_t)temp, (u32_t)temp);
            }
            else
            {
                  print("Read failure\n");
            }

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
      stdRet_t  appHdlStatus;
      appArgs_t *appHdl;
      appArgs_t *appHdlCpy;

      /* waiting for empty stdout */
      fsflush(stdout);

      appHdl = Exec(cmd, arg, &appHdlStatus);

      if (appHdlStatus == STD_RET_OK)
      {
            appHdlCpy      = appHdl;
            appHdl->stdin  = stdin;
            appHdl->stdout = stdout;

            while (appHdl->exitCode == STD_RET_UNKNOWN)
            {
                  Sleep(10);
            }

            FreeAppStdio(appHdlCpy);

            return CMD_EXECUTED;
      }
      else if (appHdlStatus == STD_RET_ALLOCERROR)
      {
            return CMD_ALLOC_ERROR;
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
      (void) argv;

      cmdStatus_t cmdStatus;
      ch_t        *line;
      ch_t        *history;
      ch_t        *cmd;
      ch_t        *arg;

      /* allocate memory for input line */
      line    = (ch_t *)Malloc(PROMPT_LINE_SIZE * sizeof(ch_t));
      history = (ch_t *)Malloc(PROMPT_LINE_SIZE * sizeof(ch_t));

      if (!line || !history)
      {
            if (line)
                  Free(line);

            if (history)
                  Free(history);

            print("No enough free memory\n");
            return STD_RET_ERROR;
      }
      else
      {
            print("Welcome to board - kernel FreeRTOS (tty1)\n");
      }

      memset(history, ASCII_NULL, PROMPT_LINE_SIZE);

      /* main loop ------------------------------------------------------------------------------ */
      for (;;)
      {
            /* clear input line and print prompt */
            memset(line, ASCII_NULL, PROMPT_LINE_SIZE);
            PrintPrompt();

            /* waiting for command */
            scan("%s3", line);

            /* check that history was call */
            if (strcmp(line, "\x1B[A") == 0)
            {
                  ch_t character;

                  strcpy(line, history);
                  print("%s", history);

                  do
                  {
                        character = getChar();
                  }
                  while (!(character == ASCII_LF || character == ASCII_CR));

                  print("\n");
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
                  continue;
            else if (cmdStatus == CMD_EXIT)
                  break;

            /* check external commands */
            if ((cmdStatus = FindExternalCmd(cmd, arg)) == CMD_EXECUTED)
                  continue;

            /* check status */
            if (cmdStatus == CMD_ALLOC_ERROR)
            {
                  print("Not enough free memory to run application.\n");
            }
            else if (cmdStatus == CMD_NOT_EXIST)
            {
                  if (strlen(cmd) != 0)
                  {
                        print("\"%s\" is unknown command.\n", cmd);
                  }
            }
      }

      Free(line);
      return STD_RET_OK;
}

/* End of application section declaration */
APP_SEC_END


/*==================================================================================================
                                            End of file
==================================================================================================*/
