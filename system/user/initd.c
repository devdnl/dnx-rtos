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
#include "tty_def.h"
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

      mount("lfs", NULL, "/");

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

      mount("appfs", NULL, "/fbin");
      mount("procfs", NULL, "/proc");

      /* early initialization - basic drivers start */
      if (InitDrv("pll", "/dev/pll") != STD_RET_OK) {
            while (TRUE);
      }

      InitDrv("gpio", "/dev/gpio");

      /* early initialization - terminal support */
      InitDrv("uart1", "/dev/ttyS0");
      InitDrv("tty0", "/dev/tty0");
      kprintEnable("/dev/tty0");

#if defined(ARCH_posix) /* DNLTEST posix bug: kprint works only on /dev/ttyS0 */
      kprintEnable("/dev/ttyS0");
#endif

      /* something about board and system */
      kprint(FONT_COLOR_GREEN FONT_BOLD "%s/%s" FONT_NORMAL " by " FONT_COLOR_CYAN "Daniel Zorychta "
             FONT_COLOR_YELLOW "<daniel.zorychta@gmail.com>" RESET_ATTRIBUTES "\n\n",
             SystemGetOSName(), SystemGetKernelName());

      /* driver initialization */
      InitDrv("tty1", "/dev/tty1");
      InitDrv("tty2", "/dev/tty2");
#if !defined(ARCH_posix) /* DNLTEST posix bug: kprint works only on /dev/ttyS0 */
      InitDrv("tty3", "/dev/tty3");
#endif
      InitDrv("i2c1", "/dev/i2c");
      InitDrv("ds1307rtc", "/dev/rtc");
      InitDrv("ds1307nvm", "/dev/nvm");
      InitDrv("eth0", "/dev/eth0");
      InitDrv("mpl115a2", "/dev/sensor");


#if !defined(ARCH_posix)
      if (StartDaemon("lwipd", NULL) == STD_RET_OK) {
            StartDaemon("measd", NULL);
            StartDaemon("httpd", NULL);
      }
#endif

      /* initd info about stack usage */
      kprint("[%d] initd: free stack: %d levels\n\n", TaskGetTickCount(), TaskGetStackFreeSpace(THIS_TASK));

      /* change TTY for kprint to last TTY */
#if !defined(ARCH_posix) /* DNLTEST posix bug: kprint works only on /dev/ttyS0 */
      kprintEnable("/dev/tty3");
#endif

      /*--------------------------------------------------------------------------------------------
       * main loop which read stdios from applications
       *------------------------------------------------------------------------------------------*/
#if !defined(ARCH_posix) /* DNLTEST posix bug: kprint works only on /dev/ttyS0 */
      u8_t    ctty = -1; /* DNLTEST */
#else
      u8_t    ctty = 0;
#endif
      app_t  *apphdl[TTY_LAST] = {NULL};
      FILE_t *ttyx[TTY_LAST]   = {NULL};

#if !defined(ARCH_posix) /* DNLTEST posix bug: kprint works only on /dev/ttyS0 */
      while ((ttyx[0] = fopen("/dev/tty0", "r+")) == NULL) {
#else
      while ((ttyx[0] = fopen("/dev/ttyS0", "r+")) == NULL) {
#endif
            Sleep(200);
      }

      for (;;) {
            /* load application if new TTY was created */
            ioctl(ttyx[0], TTY_IORQ_GETCURRENTTTY, &ctty);

            if (ctty < TTY_LAST - 1) {
                  if (apphdl[ctty] == NULL) {
                        if (ttyx[ctty] == NULL) {
                              ch_t path[16];
                              snprintf(path, sizeof(path), "/dev/tty%c", '0' + ctty);
                              ttyx[ctty] = fopen(path, "r+");
                        }

                        kprint("Starting application on new terminal: TTY%d\n", ctty);

                        TaskSuspendAll();
                        apphdl[ctty] = Exec("term", NULL);

                        if (apphdl[ctty] == NULL) {
                              TaskResumeAll();
                              kprint("Not enough free memory to start application\n");
                        } else {
                              apphdl[ctty]->stdin  = ttyx[ctty];
                              apphdl[ctty]->stdout = ttyx[ctty];

                              TaskResumeAll();
                              kprint("Application started on TTY%d\n", ctty);
                        }
                  }
            }

            /* application monitoring */
            for (u8_t i = 0; i < TTY_LAST - 1; i++) {
                  if (apphdl[i]) {
                        if (apphdl[i]->exitCode != STD_RET_UNKNOWN) {
                              kprint("Application closed on TTY%d\n", ctty);

                              KillApp(apphdl[i]);
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
