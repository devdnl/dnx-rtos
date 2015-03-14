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

#ifndef _PROGMAN_H_
#define _PROGMAN_H_

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
#define _GVAR_STRUCT_NAME               global_variables
#define GLOBAL_VARIABLES                struct _GVAR_STRUCT_NAME  // compability with older versions
#define GLOBAL_VARIABLES_SECTION        GLOBAL_VARIABLES
#define GLOBAL_VARIABLES_SECTION_BEGIN  GLOBAL_VARIABLES {
#define GLOBAL_VARIABLES_SECTION_END    };

#ifdef __cplusplus
        inline void* operator new     (size_t size) {return _sysm_tskmalloc(size);}
        inline void* operator new[]   (size_t size) {return _sysm_tskmalloc(size);}
        inline void  operator delete  (void* ptr  ) {_sysm_tskfree(ptr);}
        inline void  operator delete[](void* ptr  ) {_sysm_tskfree(ptr);}
#       define _PROGMAN_CXX extern "C"
#       define _PROGMAN_EXTERN_C extern "C"
#else
#       define _PROGMAN_CXX
#       define _PROGMAN_EXTERN_C extern
#endif

#define PROGRAM_MAIN(name, stack_depth, argc, argv) \
        _PROGMAN_CXX const int __built_in_app_##name##_gs__ = sizeof(struct _GVAR_STRUCT_NAME);\
        _PROGMAN_CXX const int __built_in_app_##name##_ss__ = stack_depth;\
        _PROGMAN_CXX int __built_in_app_##name##_main(argc, argv)

#define _IMPORT_PROGRAM(name)\
        _PROGMAN_EXTERN_C const int __built_in_app_##name##_gs__;\
        _PROGMAN_EXTERN_C const int __built_in_app_##name##_ss__;\
        _PROGMAN_EXTERN_C int __built_in_app_##name##_main(int, char**)

#define int_main(name, stack_depth, argc, argv)\
        PROGRAM_MAIN(name, stack_depth, argc, argv)

#define _PROGRAM_CONFIG(name) \
        {.program_name  = #name,\
         .main_function = __built_in_app_##name##_main,\
         .globals_size  = &__built_in_app_##name##_gs__,\
         .stack_depth   = &__built_in_app_##name##_ss__}

/*==============================================================================
  Exported types, enums definitions
==============================================================================*/
struct _prog_data {
        char      *program_name;
        int      (*main_function)(int, char**);
        const int *globals_size;
        const int *stack_depth;
};

typedef struct thread thread_t;

typedef struct prog prog_t;

/*==============================================================================
  Exported object declarations
==============================================================================*/
extern FILE                     *stdin;
extern FILE                     *stdout;
extern FILE                     *stderr;
extern struct _GVAR_STRUCT_NAME *global;
extern int                      _errno;

/*==============================================================================
  Exported function prototypes
==============================================================================*/
extern void      _exit                                          (int);
extern void      _abort                                         (void);
extern int       _system                                        (const char*);
extern prog_t   *_program_new                                   (const char*, const char*, FILE*, FILE*, FILE*);
extern int       _program_kill                                  (prog_t*);
extern int       _program_delete                                (prog_t*);
extern int       _program_wait_for_close                        (prog_t*, const uint);
extern int       _program_get_exit_code                         (prog_t*);
extern bool      _program_is_closed                             (prog_t*);
extern void      _task_kill                                     (task_t*);
extern thread_t *_thread_new                                    (void (*)(void*), const int, void*);
extern int       _thread_join                                   (thread_t*);
extern bool      _thread_is_finished                            (thread_t*);
extern int       _thread_delete                                 (thread_t*);
extern int       _thread_cancel                                 (thread_t*);
extern void      _copy_task_context_to_standard_variables       (void);
extern void      _copy_standard_variables_to_task_context       (void);

/*==============================================================================
  Exported inline functions
==============================================================================*/
static inline const struct _prog_data *_get_programs_table(void)
{
        extern const struct _prog_data _prog_table[];
        return _prog_table;
}

static inline int _get_programs_table_size(void)
{
        extern const int _prog_table_size;
        return _prog_table_size;
}

#ifdef __cplusplus
}
#endif

#endif /* _PROGMAN_H_ */
/*==============================================================================
  End of file
==============================================================================*/
