/*=========================================================================*//**
@file    static.c

@author  Daniel Zorychta

@brief   Static variables definition.

@note    Copyright (C) 2021 Daniel Zorychta <daniel.zorychta@gmail.com>

         This program is free software; you can redistribute it and/or modify
         it under the terms of the GNU General Public License as published by
         the Free Software Foundation and modified by the dnx RTOS exception.

         NOTE: The modification  to the GPL is  included to allow you to
               distribute a combined work that includes dnx RTOS without
               being obliged to provide the source  code for proprietary
               components outside of the dnx RTOS.

         The dnx RTOS  is  distributed  in the hope  that  it will be useful,
         but WITHOUT  ANY  WARRANTY;  without  even  the implied  warranty of
         MERCHANTABILITY  or  FITNESS  FOR  A  PARTICULAR  PURPOSE.  See  the
         GNU General Public License for more details.

         Full license text is available on the following file: doc/license.txt.


*//*==========================================================================*/

/*==============================================================================
  Include files
==============================================================================*/
#include <sys/types.h>
#include <libc/source/syscall.h>
#include "common.h"

/*==============================================================================
  Local macros
==============================================================================*/

/*==============================================================================
  Local object types
==============================================================================*/

/*==============================================================================
  Local function prototypes
==============================================================================*/

/*==============================================================================
  Local objects
==============================================================================*/

/*==============================================================================
  Exported objects
==============================================================================*/
/** buffer used to store temporary time structure */
struct tm _libc_tmbuf = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

/** buffer used to store converted time to string */
char _libc_timestr[32];

unsigned int _libc_seed = 123456789;

/*
 * Those variables are switched by kernel according to current process/thread.
 */
static struct _libc_file libc_stdin = {
        .fd = 0,
};

static struct _libc_file libc_stdout = {
        .fd = 1,
};

static struct _libc_file libc_stderr = {
        .fd = 2,
};

struct _libc_file *_libc_stdin  = &libc_stdin;
struct _libc_file *_libc_stdout = &libc_stdout;
struct _libc_file *_libc_stderr = &libc_stderr;

int   *_libc_errno;
void **_libc_global;

/*==============================================================================
  Function definitions
==============================================================================*/

/*==============================================================================
  End of file
==============================================================================*/
