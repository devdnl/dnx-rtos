/*=============================================================================================*//**
@file    initd.c

@author  Daniel Zorychta

@brief   This file contain initialize and runtime daemon

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

#ifdef __cplusplus
extern "C" {
#endif

/*==================================================================================================
                                            Include files
==================================================================================================*/
#include "initd.h"
#include "regdrv.h"
#include "tty.h"
#include <string.h>

#include "netconf.h"
#include "MPL115A2.h"

#include "lwiptest.h"
#include "httpde.h"


/*==================================================================================================
                                  Local symbolic constants/macros
==================================================================================================*/


/*==================================================================================================
                                   Local types, enums definitions
==================================================================================================*/


/*==================================================================================================
                                      Local function prototypes
==================================================================================================*/


/*==================================================================================================
                                      Local object definitions
==================================================================================================*/


/*==================================================================================================
                                     Exported object definitions
==================================================================================================*/


/*==================================================================================================
                                        Function definitions
==================================================================================================*/

void test1(void *arg)
{
      (void) arg;

      for (;;)
      {
            TTY_AddMsg(0, "Test TTY0\r\n");
            Sleep(1000);
      }
}



void test2(void *arg)
{
      (void) arg;

      for (;;)
      {
            TTY_AddMsg(1, "-=Test TTY1=-\r\n");
            Sleep(2000);
      }
}


//================================================================================================//
/**
 * @brief Task which initialise high-level devices/applications etc
 * Task is responsible for low-level application runtime environment (stdio). Task connect
 * applications' stdios with hardware layer.
 */
//================================================================================================//
void Initd(void *arg)
{
      (void) arg;

      u8_t currtty = -1;
      ch_t *string[TTY_COUNT];
      appArgs_t *apphdl[TTY_COUNT];

      /* user initialization */
      InitDrv("i2c1", "i2c");
      InitDrv("ds1307rtc", "rtc");
      InitDrv("ds1307nvm", "nvm");

      if (InitDrv("eth0", "eth0") != STD_RET_OK)
            goto initd_net_end;

      if (LwIP_Init() != STD_RET_OK)
            goto initd_net_end;

      kprint("Starting httpde...");
      if (TaskCreate(httpd_init, "httpde", HTTPDE_STACK_SIZE, NULL, 2, NULL) == pdPASS)
      {
            kprintOK();
      }
      else
      {
            kprintFail();
      }

      initd_net_end:

      MPL115A2_Init();

      /*--------------------------------------------------------------------------------------------
       * starting terminal
       *------------------------------------------------------------------------------------------*/
//      kprint("[%d] initd: starting interactive console... ", TaskGetTickCount());

      /* try to start terminal */
//      appArgs_t *appHdl = Exec("terminal", NULL);
//
//      if (appHdl == NULL)
//      {
//            kprintFail();
//            kprint("Probably no enough free space. Restarting board...");
//            TaskResumeAll();
//            TaskDelay(5000);
//            SystemReboot();
//      }
//      else
//      {
//            kprintOK();
//      }

      /* initd info about stack usage */
      kprint("[%d] initd: free stack: %d levels\n\n", TaskGetTickCount(), TaskGetStackFreeSpace(THIS_TASK));


//      TaskCreate(test1, "testTTY0", MINIMAL_STACK_SIZE, NULL, 3, NULL);
//      TaskCreate(test2, "testTTY1", MINIMAL_STACK_SIZE, NULL, 3, NULL);


      /*--------------------------------------------------------------------------------------------
       * main loop which read stdios from applications
       *------------------------------------------------------------------------------------------*/
//      u8_t   currentTTY  = 0;
//      ch_t   character;
//      bool_t stdoutEmpty = FALSE;
//      bool_t RxFIFOEmpty = FALSE;

      /* clear main variables */
      memset(string, 0x00, sizeof(string));
      memset(apphdl, 0x00, sizeof(apphdl));

      for (;;)
      {
            /* load application in created TTY */
            u8_t lasttty = currtty;
            if (lasttty != (currtty = TTY_GetCurrTTY()))
            {
                  if (string[currtty] == NULL)
                  {
                        string[currtty] = Calloc(100, sizeof(ch_t));
                  }

                  if (string[currtty])
                  {
                        if (apphdl[currtty] == NULL)
                        {
                              apphdl[currtty] = Exec("terminal", NULL);
                        }

                        if (!apphdl[currtty])
                        {
                              Free(string[currtty]);
                              kprint("Not enough free memory to start application\n");
                        }
                  }
                  else
                  {
                        kprint("Not enough free memory to start application\n");
                  }
            }

            /* STDOUT support ------------------------------------------------------------------- */
            for (u8_t i = 0; i < TTY_COUNT; i++)
            {
                  if (apphdl[i])
                  {
                        ch_t chr;

                        if ((chr = ufgetChar(apphdl[i]->stdout)) != ASCII_CANCEL)
                        {
                              *(string[i] + 0) = chr;
                              *(string[i] + 1) = 0;
                              TTY_AddMsg(i, string[i]);
                        }
                  }
            }

            /* STDIN support -------------------------------------------------------------------- */
            ch_t chr = TTY_GetChr(currtty);

            if (chr)
            {
                  fputChar(apphdl[currtty]->stdin, chr);
            }

            Sleep(1);
//
//            if (TTY_CheckNewMsg(currentTTY))
//            {
//                  ch_t *msg = TTY_GetMsg(currentTTY, TTY_LAST_MSG);
//
//                  if (msg)
//                  {
//                        UART_Write(UART_DEV_1, msg, strlen(msg), 0);
//                  }
//            }
//
//            /* STDIN support -------------------------------------------------------------------- */
//            if (UART_IOCtl(UART_DEV_1, UART_IORQ_GET_BYTE, &character) == STD_RET_OK)
//            {
//                  i8_t keyFn = decodeFn(character);
//
//                  if (character >= '0' && character <= '4')
//                  {
//                        if (currentTTY != (character - '0'))
//                        {
//                              currentTTY = character - '0';
//
//                              ch_t *clrscr = "\x1B[2J";
//                              UART_Write(UART_DEV_1, clrscr, strlen(clrscr), 0);
//
//                              for (u8_t i = 0; i < TTY_MSGS; i++)
//                              {
//                                    ch_t *msg = TTY_GetMsg(currentTTY, i);
//
//                                    if (msg)
//                                    {
//                                          UART_Write(UART_DEV_1, msg, strlen(msg), 0);
//                                    }
//                              }
//                        }
//                  }

//                  if (keyFn == -1)
//                  {
////                        ufputChar(appHdl->stdin, character);
//                        RxFIFOEmpty = FALSE;
//                  }
//                  else if (keyFn > 0)
//                  {
//                        currentTTY = keyFn - 1;
//                  }
//            }
//            else
//            {
//                  RxFIFOEmpty = TRUE;
//            }

            /* application monitoring ----------------------------------------------------------- */
//            if (appHdl->exitCode != STD_RET_UNKNOWN)
//            {
//                  if (appHdl->exitCode == STD_RET_OK)
//                        kprint("\n[%d] initd: terminal was terminated.\n", TaskGetTickCount());
//                  else
//                        kprint("\n[%d] initd: terminal was terminated with error.\n", TaskGetTickCount());
//
//                  FreeStdio(appHdl);
//
//                  kprint("[%d] initd: disable FreeRTOS scheduler. Bye.\n", TaskGetTickCount());
//
//                  vTaskEndScheduler();
//
//                  while (TRUE)
//                        TaskDelay(1000);
//            }

            /* wait state */
//            if (stdoutEmpty && RxFIFOEmpty)
//                  TaskDelay(1);
      }

      /* this should never happen */
//      TaskTerminate();
}



#ifdef __cplusplus
}
#endif

/*==================================================================================================
                                            End of file
==================================================================================================*/
