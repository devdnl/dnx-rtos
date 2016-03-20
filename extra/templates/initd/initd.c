/*=========================================================================*//**
@file    initd.c

@author  Daniel Zorychta

@brief   This file contain initialize and runtime daemon

@note    Copyright (C) <!year!> Daniel Zorychta <daniel.zorychta@gmail.com>

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

/*==============================================================================
  Include files
==============================================================================*/
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/mount.h>
#include <dnx/os.h>
#include <dnx/net.h>
#include <dnx/thread.h>
#include <dnx/misc.h>
#include "user/initd.h"

/*==============================================================================
  Local symbolic constants/macros
==============================================================================*/

/*==============================================================================
  Local types, enums definitions
==============================================================================*/

/*==============================================================================
  Local function prototypes
==============================================================================*/
static int run_level_boot(void);
static int run_level_0(void);
static int run_level_1(void);
static int run_level_2(void);
static int run_level_exit(void);

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
 * @brief Initialize devices and programs
 * User program which provide basic system functionality e.g. STDIO handle
 * (joining TTY driver with program), basic program starting and etc. This task
 * is an example to show how this can be implemented.
 */
//==============================================================================
void initd(void *arg)
{
        (void)arg;

        task_set_priority(INITD_PRIORITY);

        if (run_level_boot() != STD_RET_OK)
                goto start_failure;

        if (run_level_0() != STD_RET_OK)
                goto start_failure;

        if (run_level_1() != STD_RET_OK)
                goto start_failure;

        if (run_level_2() != STD_RET_OK)
                goto start_failure;

start_failure:
        run_level_exit();

        task_exit();
}

//==============================================================================
/**
 * @brief Run level at boot time
 *
 * @retval STD_RET_OK           run level finished successfully
 * @retval STD_RET_ERROR        run level error
 */
//==============================================================================
static int run_level_boot(void)
{

        return STD_RET_OK;
}

//==============================================================================
/**
 * @brief Run level 0
 *
 * @retval STD_RET_OK           run level finished successfully
 * @retval STD_RET_ERROR        run level error
 */
//==============================================================================
static int run_level_0(void)
{

        return STD_RET_OK;
}

//==============================================================================
/**
 * @brief Run level 1
 *
 * @retval STD_RET_OK           run level finished successfully
 * @retval STD_RET_ERROR        run level error
 */
//==============================================================================
static int run_level_1(void)
{

        return STD_RET_OK;
}

//==============================================================================
/**
 * @brief Run level 2
 *
 * @retval STD_RET_OK           run level finished successfully
 * @retval STD_RET_ERROR        run level error
 */
//==============================================================================
static int run_level_2(void)
{

        return STD_RET_OK;
}

//==============================================================================
/**
 * @brief Run level exit
 *
 * @retval STD_RET_OK           run level finished successfully
 * @retval STD_RET_ERROR        run level error
 */
//==============================================================================
static int run_level_exit(void)
{
        sync();

        critical_section_begin();
        ISR_disable();

        while (true) {
                msleep(MAX_DELAY_MS);
        }

        return STD_RET_OK;
}

/*==============================================================================
  End of file
==============================================================================*/
