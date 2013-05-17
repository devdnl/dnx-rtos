#ifndef INITD_H_
#define INITD_H_
/*=========================================================================*//**
@file    initd.h

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
#include "system/dnx.h"

/*==============================================================================
  Exported symbolic constants/macros
==============================================================================*/
/** initd stack size */
#define INITD_STACK_DEPTH               STACK_DEPTH_VERY_LOW

/** initd name */
#define INITD_NAME                      "initd"

/** priority */
#define INITD_PRIORITY                  0

/** arguments */
#define INITD_ARGS                      NULL

/*==============================================================================
  Exported types, enums definitions
==============================================================================*/

/*==============================================================================
  Exported object declarations
==============================================================================*/

/*==============================================================================
  Exported function prototypes
==============================================================================*/
extern void task_initd(void *arg);

#ifdef __cplusplus
}
#endif

#endif /* INITD_H_ */
/*==============================================================================
  End of file
==============================================================================*/
