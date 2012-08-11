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
 */
//================================================================================================//
cmdStatus_t FindInternalCmd(ch_t *line)
{
      /* exit command --------------------------------------------------------------------------- */
      if (strcmp("exit", line) == 0)
      {
            Free(line);
            print("Exit\n");
            Exit(STD_RET_OK);
      }

      /* echo ----------------------------------------------------------------------------------- */
      if (strncmp("echo ", line, 5) == 0)
      {
            print("%s\n", (line + 5));
            return CMD_EXECUTED;
      }

      /* stack mesurement ----------------------------------------------------------------------- */
      if (strcmp("stack", line) == 0)
      {
            print("Free stack: %d\n", SystemGetStackFreeSpace());
            return CMD_EXECUTED;
      }

      return CMD_NOT_EXIST;
}


//================================================================================================//
/**
 * @brief Function find external commands (registered applications)
 */
//================================================================================================//
cmdStatus_t FindExternalCmd(ch_t *line)
{
      stdRet_t  appHdlStatus;
      appArgs_t *appHdl;
      appArgs_t *appHdlCpy;

      /* waiting for empty stdout */
      Sleep(10);

      appHdl = Exec(line, NULL, &appHdlStatus);

      if (appHdlStatus == STD_RET_OK)
      {
            appHdlCpy      = appHdl;
            appHdl->stdin  = stdin;
            appHdl->stdout = stdout;

            while (appHdl->exitCode == STD_RET_UNKNOWN)
            {
                  Sleep(100);
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

      /* allocate memory for input line */
      line = (ch_t *)Malloc(PROMPT_LINE_SIZE * sizeof(ch_t));

      if (!line)
      {
            print("No enough free memory\n");
            return STD_RET_ERROR;
      }
      else
      {
            print("Welcome to board - kernel FreeRTOS (tty1)\n");
      }

      /* main loop ------------------------------------------------------------------------------ */
      for (;;)
      {
            /* clear input line and print prompt */
            memset(line, ASCII_NULL, PROMPT_LINE_SIZE);
            PrintPrompt();

            /* waiting for command */
            scan("%s3", line);

            /* check internal commands */
            if ((cmdStatus = FindInternalCmd(line)) == CMD_EXECUTED)
                  continue;

            /* check external commands */
            if ((cmdStatus = FindExternalCmd(line)) == CMD_EXECUTED)
                  continue;

            /* check status */
            if (cmdStatus == CMD_ALLOC_ERROR)
            {
                  print("Not enough free memory to run application.\n");
            }
            else if (cmdStatus == CMD_NOT_EXIST)
            {
                  if (strlen(line) != 0)
                  {
                        print("%s is unknown command.\n", line);
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
