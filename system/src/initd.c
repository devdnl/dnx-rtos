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

      u8_t      currtty = -1;
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

      /* initd info about stack usage */
      kprint("[%d] initd: free stack: %d levels\n\n", TaskGetTickCount(), TaskGetStackFreeSpace(THIS_TASK));

      /* change TTY for kprint to last TTY */
      ChangekprintTTY(TTY_COUNT - 1);

      kprint("kprint() on TTY4\n");

      /*--------------------------------------------------------------------------------------------
       * main loop which read stdios from applications
       *------------------------------------------------------------------------------------------*/
      memset(apphdl, 0x00, sizeof(apphdl));

      for (;;)
      {
            /* load application if new TTY was created */
            currtty = TTY_GetCurrTTY();

            if (currtty < TTY_COUNT - 1)
            {
                  if (apphdl[currtty] == NULL)
                  {
                        kprint("Starting application on new terminal: TTY%d\n", currtty + 1);

                        apphdl[currtty] = Exec("term", NULL);

                        if (apphdl[currtty] == NULL)
                        {
                              kprint("Not enough free memory to start application\n");
                        }
                        else
                        {
                              kprint("Application started on TTY%d\n", currtty + 1);
                              apphdl[currtty]->tty = currtty;
                        }
                  }
            }

            /* application monitoring */
            for (u8_t i = 0; i < TTY_COUNT - 1; i++)
            {
                  if (apphdl[i])
                  {
                        if (apphdl[i]->exitCode != STD_RET_UNKNOWN)
                        {
                              kprint("Application closed on TTY%d\n", currtty + 1);

                              FreeApphdl(apphdl[i]);
                              apphdl[i] = NULL;

                              TTY_ChangeTTY(0);
                        }
                  }
                  else
                  {
                        TTY_Clear(i);
                  }
            }

            TaskDelay(500);
      }

      /* this should never happen */
      TaskTerminate();
}



#ifdef __cplusplus
}
#endif

/*==================================================================================================
                                            End of file
==================================================================================================*/
