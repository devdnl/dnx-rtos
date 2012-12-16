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
#include "lfs.h"
#include "regapp.h"
#include "tty_def.h"
#include "appmoni.h"
#include <string.h>


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

      /* early initialization */
      vfs_init();
      moni_Init();
      moni_AddTask(TaskGetCurrentTaskHandle());

      struct vfs_fscfg *fscfg = malloc(sizeof(struct vfs_fscfg));

      /* early initialization - mounting FS */
      if (fscfg) {
            fscfg->dev       = 0;
            fscfg->f_init    = lfs_init;
            fscfg->f_chmod   = lfs_chmod;
            fscfg->f_chown   = lfs_chown;
            fscfg->f_close   = lfs_close;
            fscfg->f_ioctl   = lfs_ioctl;
            fscfg->f_mkdir   = lfs_mkdir;
            fscfg->f_mknod   = lfs_mknod;
            fscfg->f_open    = lfs_open;
            fscfg->f_opendir = lfs_opendir;
            fscfg->f_read    = lfs_read;
            fscfg->f_release = lfs_release;
            fscfg->f_remove  = lfs_remove;
            fscfg->f_rename  = lfs_rename;
            fscfg->f_stat    = lfs_stat;
            fscfg->f_fstat   = lfs_fstat;
            fscfg->f_statfs  = lfs_statfs;
            fscfg->f_write   = lfs_write;
            mount("/", fscfg);

            /* create basic directories */
            mkdir("/bin");
            mkdir("/dev");
            mkdir("/etc");
            mkdir("/fbin");
            mkdir("/home");
            mkdir("/mnt");
            mkdir("/proc");
            mkdir("/srv");
            mkdir("/srv/www");
            mkdir("/tmp");

            fscfg->dev       = 0;
            fscfg->f_init    = appfs_init;
            fscfg->f_chmod   = appfs_chmod;
            fscfg->f_chown   = appfs_chown;
            fscfg->f_close   = appfs_close;
            fscfg->f_ioctl   = appfs_ioctl;
            fscfg->f_mkdir   = appfs_mkdir;
            fscfg->f_mknod   = appfs_mknod;
            fscfg->f_open    = appfs_open;
            fscfg->f_opendir = appfs_opendir;
            fscfg->f_read    = appfs_read;
            fscfg->f_release = appfs_release;
            fscfg->f_remove  = appfs_remove;
            fscfg->f_rename  = appfs_rename;
            fscfg->f_stat    = appfs_stat;
            fscfg->f_fstat   = appfs_fstat;
            fscfg->f_statfs  = appfs_statfs;
            fscfg->f_write   = appfs_write;
            mount("/fbin", fscfg);

            free(fscfg);
      } else {
            TaskTerminate();
      }

      /* early initialization - basic drivers start */
      if (InitDrv("pll", "/dev/pll") != STD_RET_OK)
            while (TRUE);

      InitDrv("gpio", "/dev/gpio");

      /* early initialization - terminal support */
      InitDrv("uart1", "/dev/ttyS0");
      InitDrv("tty0", "/dev/tty0");
      kprintEnableOn("/dev/tty0");

      /* something about board and system */
      kprint("\x1B[32m\x1B[1m");
      kprint(".--------. .--. .---. .--. .--. ,--. ,--,\n");
      kprint("|__    __| |  | |    \\|  | |  | \\   V  /\n");
      kprint("   |  |    |  | |  |\\    | |  |  \\    /\n");
      kprint("   |  |    |  | |  | \\   | |  |  /    \\\n");
      kprint("   |  |    |  | |  |  \\  | |  | /  /\\  \\\n");
      kprint("   `--'    `--' `--'   `-' `--' `-'  `-'  RTOS 0.5.0\x1B[0m\n\n");

      kprint("powered by \x1B[32mFreeRTOS\x1B[0m\n");
      kprint("by \x1B[36mDaniel Zorychta \x1B[33m<daniel.zorychta@gmail.com>\x1B[0m\n\n");

      /* driver initialization */
      InitDrv("tty1", "/dev/tty1");
      InitDrv("tty2", "/dev/tty2");
      InitDrv("tty3", "/dev/tty3");
      InitDrv("i2c1", "/dev/i2c");
      InitDrv("ds1307rtc", "/dev/rtc");
      InitDrv("ds1307nvm", "/dev/nvm");
      InitDrv("eth0", "/dev/eth0");
      InitDrv("mpl115a2", "/dev/sensor");


      if (LwIP_Init() == STD_RET_OK) {
            StartDaemon("measd", NULL);
            StartDaemon("httpd", NULL);
      }

      /* initd info about stack usage */
      kprint("[%d] initd: free stack: %d levels\n\n", TaskGetTickCount(), TaskGetStackFreeSpace(THIS_TASK));

      /* change TTY for kprint to last TTY */
      kprintEnableOn("/dev/tty3");

      /*--------------------------------------------------------------------------------------------
       * main loop which read stdios from applications
       *------------------------------------------------------------------------------------------*/
      u8_t    ctty = -1;
      app_t  *apphdl[TTY_LAST] = {NULL};
      FILE_t *ttyx[TTY_LAST] = {NULL};

      while ((ttyx[0] = fopen("/dev/tty0", "r+")) == NULL)
      {
            Sleep(200);
      }

      for (;;)
      {
            /* load application if new TTY was created */
            ioctl(ttyx[0], TTY_IORQ_GETCURRENTTTY, &ctty);

            if (ctty < TTY_LAST - 1)
            {
                  if (apphdl[ctty] == NULL)
                  {
                        if (ttyx[ctty] == NULL)
                        {
                              ch_t path[16];
                              snprintf(path, sizeof(path), "/dev/tty%c", '0' + ctty);
                              ttyx[ctty] = fopen(path, "r+");
                        }

                        kprint("Starting application on new terminal: TTY%d\n", ctty);

                        TaskSuspendAll();
                        apphdl[ctty] = Exec("term", NULL);

                        if (apphdl[ctty] == NULL)
                        {
                              TaskResumeAll();
                              kprint("Not enough free memory to start application\n");
                        }
                        else
                        {
                              apphdl[ctty]->stdin  = ttyx[ctty];
                              apphdl[ctty]->stdout = ttyx[ctty];

                              TaskResumeAll();
                              kprint("Application started on TTY%d\n", ctty);
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
                              ioctl(ttyx[0], TTY_IORQ_SETACTIVETTY, &ctty);
                        }
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
