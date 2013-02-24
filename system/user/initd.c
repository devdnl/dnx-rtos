/*=========================================================================*//**
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


*//*==========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Include files
==============================================================================*/
#include "initd.h"
#include "tty_def.h"
#include <string.h>

/*==============================================================================
  Local symbolic constants/macros
==============================================================================*/

/*==============================================================================
  Local types, enums definitions
==============================================================================*/

/*==============================================================================
  Local function prototypes
==============================================================================*/

/*==============================================================================
  Local object definitions
==============================================================================*/

/*==============================================================================
  Exported object definitions
==============================================================================*/

/*==============================================================================
  Function definitions
==============================================================================*/

//==============================================================================
/**
 * @brief Task which initialise high-level devices/applications etc
 * Task is responsible for low-level application runtime environment (stdio).
 * Task connect applications' stdios with hardware layer.
 */
//==============================================================================
void task_initd(void *arg)
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

      mount("procfs", NULL, "/proc");

      /* early initialization - basic drivers start */
      if (init_driver("pll", NULL) != STD_RET_OK) {
            while (TRUE);
      }

      init_driver("gpio", NULL);

      /* early initialization - terminal support */
      init_driver("uart1", "/dev/ttyS0");
      init_driver("tty0", "/dev/tty0");
      kprintEnable("/dev/tty0");

      /* something about board and system */
      kprint(FONT_COLOR_GREEN FONT_BOLD "%s/%s" FONT_NORMAL " by "
             FONT_COLOR_CYAN "Daniel Zorychta "
             FONT_COLOR_YELLOW "<daniel.zorychta@gmail.com>" RESET_ATTRIBUTES "\n\n",
             get_OS_name(), get_kernel_name());

      /* driver initialization */
      init_driver("tty1", "/dev/tty1");
      init_driver("tty2", "/dev/tty2");
      init_driver("tty3", "/dev/tty3");
      init_driver("i2c1", "/dev/i2c");
      init_driver("ds1307rtc", "/dev/rtc");
      init_driver("ds1307nvm", "/dev/nvm");
      init_driver("eth0", "/dev/eth0");
      init_driver("mpl115a2", "/dev/sensor");

      /* initd info about stack usage */
      kprint("[%d] initd: free stack: %d levels\n\n", get_tick_counter(), get_free_stack());

      /* change TTY for kprint to last TTY */
      kprintEnable("/dev/tty3");

      /*------------------------------------------------------------------------
       * main loop which read stdios from applications
       *----------------------------------------------------------------------*/
      FILE_t *ttyx[TTY_LAST] = {NULL};

      while ((ttyx[0] = fopen("/dev/tty0", "r+")) == NULL) {
            milisleep(200);
      }

      for (;;) {
              task_t p1 = run_program("test", "jeden dwa trzy", ttyx[0], ttyx[0], "/");

              for (int i = 0; i < 10; i++) {
                      enum prg_status ps = get_program_status(p1);

                      switch (ps) {
                      case PROGRAM_INITING: kprint("initing...\n"); break;
                      case PROGRAM_RUNNING: kprint("running...\n"); break;
                      case PROGRAM_ENDED:   kprint("ended...\n"); break;
                      case PROGRAM_NEVER_EXISTED:  kprint("doesn't exist...\n"); break;
                      case PROGRAM_ARGUMENTS_PARSE_ERROR:  kprint("parse error...\n"); break;
                      case PROGRAM_NOT_ENOUGH_FREE_MEMORY: kprint("EFM...\n"); break;
                      }

                      milisleep(250);
              }

              sleep(5);
      }

      /* this should never happen */
      terminate_task();
}

#ifdef __cplusplus
}
#endif

/*==============================================================================
  End of file
==============================================================================*/
