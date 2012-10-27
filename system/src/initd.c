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

#include "lwiptest.h"


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

      /* early initialization - terminal support */
      InitDrv("uart1", "ttyS0");
      InitDrv("tty0", "tty0");
      kprintEnableOn("/dev/tty0");

      /* something about board and system */
      kprint("\x1B[32m\x1B[1m");
      kprint(".--------. .--. .---. .--. .--. ,--. ,--,\n");
      kprint("|__    __| |  | |    \\|  | |  | \\   V  /\n");
      kprint("   |  |    |  | |  |\\    | |  |  \\    /\n");
      kprint("   |  |    |  | |  | \\   | |  |  /    \\\n");
      kprint("   |  |    |  | |  |  \\  | |  | /  /\\  \\\n");
      kprint("   `--'    `--' `--'   `-' `--' `-'  `-'\x1B[0m\n\n");

      kprint("powered by \x1B[32mFreeRTOS\x1B[0m\n");
      kprint("by \x1B[36mDaniel Zorychta \x1B[33m<daniel.zorychta@gmail.com>\x1B[0m\n\n");

      /* driver initialization */
      InitDrv("tty1", "tty1");
      InitDrv("tty2", "tty2");
      InitDrv("tty3", "tty3");
      InitDrv("i2c1", "i2c");
      InitDrv("ds1307rtc", "rtc");
      InitDrv("ds1307nvm", "nvm");
      InitDrv("eth0", "eth0");
      InitDrv("mpl115a2", "sensor");

      /* library initializations */
      if (LwIP_Init() != STD_RET_OK) /* FIXME this shall looks better */
            goto initd_net_end;

      kprint("Starting httpde..."); /* FIXME create httpd as really deamon application */
      Execd("httpd", NULL);
//      if (TaskCreate(httpd_init, "httpde", HTTPDE_STACK_SIZE, NULL, 2, NULL) == pdPASS)
//      {
//            kprintOK();
//      }
//      else
//      {
//            kprintFail();
//      }

      initd_net_end:

      /* initd info about stack usage */
      kprint("[%d] initd: free stack: %d levels\n\n", TaskGetTickCount(), TaskGetStackFreeSpace(THIS_TASK));

      /* change TTY for kprint to last TTY */
      kprintEnableOn("/dev/tty3");
      kprint("kprint() on TTY3\n");

      /*--------------------------------------------------------------------------------------------
       * main loop which read stdios from applications
       *------------------------------------------------------------------------------------------*/
      u8_t ctty = -1;

      app_t *apphdl[TTY_LAST];
      memset(apphdl, 0x00, sizeof(apphdl));

      FILE_t *tty;
      FILE_t *ttyx[TTY_LAST];
      memset(ttyx, 0x00, sizeof(ttyx));

      while ((tty = fopen("/dev/tty0", "r+")) == NULL)
      {
            Sleep(200);
      }

      ttyx[0] = tty;

      for (;;)
      {
            /* load application if new TTY was created */
            ioctl(tty, TTY_IORQ_GETCURRENTTTY, &ctty);

            if (ctty < TTY_LAST - 1)
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
            for (u8_t i = 0; i < TTY_LAST - 1; i++)
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
