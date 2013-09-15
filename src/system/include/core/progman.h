#ifndef _PROGMAN_H_
#define _PROGMAN_H_
/*=========================================================================*//**
@file    progman.h

@author  Daniel Zorychta

@brief   This file support programs layer

@note    Copyright (C) 2012, 2013 Daniel Zorychta <daniel.zorychta@gmail.com>

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
#include "config.h"
#include "core/vfs.h"

/*==============================================================================
  Exported symbolic constants/macros
==============================================================================*/
#define GLOBAL_VARIABLES_SECTION_BEGIN  struct __global_vars__ {
#define GLOBAL_VARIABLES_SECTION_END    };

#define PROGRAM_MAIN(name, argc, argv) \
        const int __prog_##name##_gs__ = sizeof(struct __global_vars__);\
        int program_##name##_main(argc, argv)

#define _IMPORT_PROGRAM(name)\
        extern const int __prog_##name##_gs__;\
        extern int program_##name##_main(int, char**)

#define _PROGRAM_CONFIG(name, stack_size) \
        {.program_name  = #name,\
         .main_function = program_##name##_main,\
         .globals_size  = &__prog_##name##_gs__,\
         .stack_depth   = stack_size}

#define stdin \
        _get_this_task_data()->f_stdin

#define stdout \
        _get_this_task_data()->f_stdout

#define stderr \
        _get_this_task_data()->f_stderr

#define global \
        ((struct __global_vars__*)_get_this_task_data()->f_global_vars)

#define create_fast_global(name) \
        struct __global_vars__*name = global

/*==============================================================================
  Exported types, enums definitions
==============================================================================*/
struct _prog_data {
        char      *program_name;
        int      (*main_function)(int, char**);
        const int *globals_size;
        int        stack_depth;
};

/*==============================================================================
  Exported object declarations
==============================================================================*/
enum prog_state {
        PROGRAM_UNKNOWN_STATE,
        PROGRAM_RUNNING,
        PROGRAM_ENDED,
        PROGRAM_NOT_ENOUGH_FREE_MEMORY,
        PROGRAM_ARGUMENTS_PARSE_ERROR,
        PROGRAM_DOES_NOT_EXIST,
        PROGRAM_HANDLE_ERROR,
};

/*==============================================================================
  Exported function prototypes
==============================================================================*/
extern task_t   *new_program       (const char*, const char*, FILE*, FILE*, enum prog_state*, int*);
extern void      delete_program    (task_t*, int);
extern void      exit              (int);
extern void      abort             (void);
extern int       system            (const char*);

#ifdef __cplusplus
}
#endif

#endif /* _PROGMAN_H_ */
/*==============================================================================
  End of file
==============================================================================*/
