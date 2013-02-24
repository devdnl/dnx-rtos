#ifndef RUNTIME_H_
#define RUNTIME_H_
/*=========================================================================*//**
@file    runtime.h

@author  Daniel Zorychta

@brief   This file support runtime environment for applications

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
#include "systypes.h"
#include "oswrap.h"

/*==============================================================================
  Exported symbolic constants/macros
==============================================================================*/
#define GLOBAL_VARIABLES                struct __global_vars__
#define stdin                           get_program_stdin()
#define stdout                          get_program_stdout()
#define global                          ((struct __global_vars__ *)get_program_globals())
#define create_fast_global(name)        struct __global_vars__ *name = get_program_globals()

/*==============================================================================
  Exported types, enums definitions
==============================================================================*/

/*==============================================================================
  Exported object declarations
==============================================================================*/
enum prg_status {
        PROGRAM_INITING,
        PROGRAM_RUNNING,
        PROGRAM_ENDED,
        PROGRAM_NEVER_EXISTED,
        PROGRAM_NOT_ENOUGH_FREE_MEMORY,
        PROGRAM_ARGUMENTS_PARSE_ERROR,
};

/*==============================================================================
  Exported function prototypes
==============================================================================*/
extern task_t          run_program(ch_t *name, ch_t *args, FILE_t *fstdin, FILE_t *fstdout, ch_t *cwd);
extern stdRet_t        kill_program(task_t taskhdl);
extern enum prg_status get_program_status(task_t taskhdl);
extern FILE_t         *get_program_stdin(void);
extern FILE_t         *get_program_stdout(void);
extern void           *get_program_globals(void);
extern ch_t           *get_program_cwd(void);

#ifdef __cplusplus
}
#endif

#endif /* RUNTIME_H_ */
/*==============================================================================
  End of file
==============================================================================*/
