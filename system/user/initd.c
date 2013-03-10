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
 * Task is responsible for low-level program runtime environment (stdio).
 * Task connect programs' stdio with hardware layer.
 */
//==============================================================================
void task_initd(void *arg)
{
      (void) arg;

      set_priority(INITD_PRIORITY);

      /* mount main file system */
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
      enable_printk("/dev/tty0");

      /* something about board and system */
      printk(FONT_COLOR_GREEN FONT_BOLD "%s/%s" FONT_NORMAL " by "
             FONT_COLOR_CYAN "Daniel Zorychta "
             FONT_COLOR_YELLOW "<daniel.zorychta@gmail.com>" RESET_ATTRIBUTES "\n\n",
             get_OS_name(), get_kernel_name());

      /* driver initialization */
      init_driver("tty1", "/dev/tty1");
      init_driver("tty2", "/dev/tty2");
      init_driver("tty3", "/dev/tty3");

      /* initd info about stack usage */
      printk("[%d] initd: free stack: %d levels\n\n", get_tick_counter(), get_free_stack());

      /* change TTY for printk to last TTY */
      enable_printk("/dev/tty3");

      /*------------------------------------------------------------------------
       * main loop which read stdio from programs
       *----------------------------------------------------------------------*/
      FILE_t *ttyx[TTY_LAST] = {NULL};

      while ((ttyx[0] = fopen("/dev/tty0", "r+")) == NULL) {
            milisleep(200);
      }

      ttyx[1] = fopen("/dev/tty1", "r+");
      new_program("top", "", "/", ttyx[1], ttyx[1], NULL, NULL);

//      new_program("term", "", "/", ttyx[0], ttyx[0], NULL, NULL);

      for (;;) {
              task_t *p1 = new_program("helloworld", "", "/", ttyx[0], ttyx[0], NULL, NULL);

              sleep(1);
      }

      /* this should never happen */
      task_exit();
}

#ifdef __cplusplus
}
#endif

/*==============================================================================
  End of file
==============================================================================*/
