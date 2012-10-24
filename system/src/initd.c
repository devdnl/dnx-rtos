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

      /* driver initialization */
      InitDrv("uart1", "ttyS0");
      InitDrv("tty0", "tty0");
      kprintEnableOn("/dev/tty0");
      InitDrv("tty1", "tty1");
      InitDrv("tty2", "tty2");
      InitDrv("tty3", "tty3");
      InitDrv("i2c1", "i2c");
      InitDrv("ds1307rtc", "rtc");
      InitDrv("ds1307nvm", "nvm");

      /* something about board and system */
      kprint("\n\x1B[32m");
      kprint(".--------. .--. .---. .--. .--. ,--. ,--,\n");
      kprint("|__    __| |  | |    \\|  | |  | \\   V  /\n");
      kprint("   |  |    |  | |  |\\    | |  |  \\    /\n");
      kprint("   |  |    |  | |  | \\   | |  |  /    \\\n");
      kprint("   |  |    |  | |  |  \\  | |  | /  /\\  \\\n");
      kprint("   `--'    `--' `--'   `-' `--' `-'  `-'\n\n\x1B[0m");

      kprint("powered by \x1B[32mFreeRTOS\x1B[0m\n");
      kprint("by \x1B[36mDaniel Zorychta \x1B[33m<daniel.zorychta@gmail.com>\x1B[0m\n\n");

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
      kprintEnableOn("/dev/tty3");
      kprint("kprint() on TTY3\n");

      /*--------------------------------------------------------------------------------------------
       * main loop which read stdios from applications
       *------------------------------------------------------------------------------------------*/
      u8_t ctty = -1;

      appArgs_t *apphdl[TTY_LAST];
      memset(apphdl, 0x00, sizeof(apphdl));

      FILE_t *tty;
      FILE_t *ttyx[TTY_LAST];
      memset(ttyx, 0x00, sizeof(ttyx));

      tty     = fopen("/dev/tty0", "r+");
      ttyx[0] = tty;

      for (;;)
      {
            /* load application if new TTY was created */
            ioctl(tty, TTY_IORQ_GETCURRENTTTY, &ctty);

            if (ctty < TTY_LAST)
            {
                  if (apphdl[ctty] == NULL)
                  {
                        if (ttyx[ctty] == NULL)
                        {
                              ch_t path[16];
                              snprint(path, sizeof(path), "/dev/tty%c", '0' + ctty);
                              ttyx[ctty] = fopen(path, "r+");
                        }

                        kprint("Starting application on new terminal: TTY%d\n", ctty);

                        apphdl[ctty] = Exec("term", NULL);

                        if (apphdl[ctty] == NULL)
                        {
                              kprint("Not enough free memory to start application\n");
                        }
                        else
                        {
                              kprint("Application started on TTY%d\n", ctty);
                              apphdl[ctty]->stdin  = ttyx[ctty];
                              apphdl[ctty]->stdout = ttyx[ctty];
                        }
                  }
            }

            /* application monitoring */
            for (u8_t i = 0; i < TTY_LAST; i++)
            {
                  if (apphdl[i])
                  {
                        if (apphdl[i]->exitCode != STD_RET_UNKNOWN)
                        {
                              kprint("Application closed on TTY%d\n", ctty);

                              FreeApphdl(apphdl[i]);
                              apphdl[i] = NULL;

                              ioctl(ttyx[i], TTY_IORQ_CLEARTTY, NULL);
                              fclose(ttyx[i]);
                              ttyx[i] = NULL;

                              ctty = 0;
                              ioctl(tty, TTY_IORQ_SETACTIVETTY, &ctty);
                        }
                  }
            }

            TaskDelay(1000);
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
