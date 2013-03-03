#ifndef PROCESS_H_
#define PROCESS_H_
/*=========================================================================*//**
@file    process.h

@author  Daniel Zorychta

@brief   This module support process

@note    Copyright (C) 2013 Daniel Zorychta <daniel.zorychta@gmail.com>

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
#include "oswrap.h"

/*==============================================================================
  Exported symbolic constants/macros
==============================================================================*/

/*==============================================================================
  Exported types, enums definitions
==============================================================================*/
typedef i32_t pid_t;

/*==============================================================================
  Exported object declarations
==============================================================================*/

/*==============================================================================
  Exported function prototypes
==============================================================================*/
extern stdRet_t  proc_init(void);
extern pid_t     proc_new_process(void(*)(void*), char*, uint, void*);
extern void      proc_kill_process(pid_t pid);
extern task_t   *proc_get_process_task_handle(pid_t pid);
extern pid_t     proc_get_pid(void);
extern void      proc_process_terminate(void);
extern task_t   *proc_get_process_task_handle(pid_t pid);

#ifdef __cplusplus
}
#endif

#endif /* PROCESS_H_ */
/*==============================================================================
  End of file
==============================================================================*/
