/*=========================================================================*//**
@file    mian.c

@author  Daniel Zorychta

@brief   This file provide system initialisation and RTOS start.

@note    Copyright (C) 2012, 2013  Daniel Zorychta <daniel.zorychta@gmail.com>

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
#include "portable/cpuctl.h"
#include "mm/heap.h"
#include "fs/vfs.h"
#include "lib/unarg.h"
#include "kernel/syscall.h"
#include "kernel/kpanic.h"
#include "kernel/kwrapper.h"

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
 * @brief  Task used to initialize dnx RTOS system. This task decrease stack
 *         usage in startup phase. Task after system initialization is deleted.
 */
//==============================================================================
void dnxinit(void *arg)
{
        UNUSED_ARG1(arg);

        _vfs_init();
        _syscall_init();
        _task_exit();
}

//==============================================================================
/**
 * @brief Main function
 */
//==============================================================================
int main(void)
{
        _cpuctl_init();
        _heap_init();
        _mm_init();
        _kernel_panic_init();
        _task_create(dnxinit, "", (1024 / sizeof(StackType_t)), NULL, NULL, NULL);
        _kernel_start();
        return -1;
}

/*==============================================================================
  End of file
==============================================================================*/
