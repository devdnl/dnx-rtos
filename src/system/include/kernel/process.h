/*=========================================================================*//**
@file    process.h

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

#ifndef _PROCESS_H_
#define _PROCESS_H_

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Include files
==============================================================================*/
#include "config.h"
#include "fs/vfs.h"

/*==============================================================================
  Exported symbolic constants/macros
==============================================================================*/
/** STANDARD STACK SIZES */
#define STACK_DEPTH_MINIMAL             ((1   * (CONFIG_RTOS_TASK_MIN_STACK_DEPTH)) + (CONFIG_RTOS_IRQ_STACK_DEPTH))
#define STACK_DEPTH_VERY_LOW            ((2   * (CONFIG_RTOS_TASK_MIN_STACK_DEPTH)) + (CONFIG_RTOS_IRQ_STACK_DEPTH))
#define STACK_DEPTH_LOW                 ((4   * (CONFIG_RTOS_TASK_MIN_STACK_DEPTH)) + (CONFIG_RTOS_IRQ_STACK_DEPTH))
#define STACK_DEPTH_MEDIUM              ((8   * (CONFIG_RTOS_TASK_MIN_STACK_DEPTH)) + (CONFIG_RTOS_IRQ_STACK_DEPTH))
#define STACK_DEPTH_LARGE               ((16  * (CONFIG_RTOS_TASK_MIN_STACK_DEPTH)) + (CONFIG_RTOS_IRQ_STACK_DEPTH))
#define STACK_DEPTH_VERY_LARGE          ((32  * (CONFIG_RTOS_TASK_MIN_STACK_DEPTH)) + (CONFIG_RTOS_IRQ_STACK_DEPTH))
#define STACK_DEPTH_HUGE                ((64  * (CONFIG_RTOS_TASK_MIN_STACK_DEPTH)) + (CONFIG_RTOS_IRQ_STACK_DEPTH))
#define STACK_DEPTH_VERY_HUGE           ((128 * (CONFIG_RTOS_TASK_MIN_STACK_DEPTH)) + (CONFIG_RTOS_IRQ_STACK_DEPTH))
#define STACK_DEPTH_USER(depth)         (depth)

#define _GVAR_STRUCT_NAME               global_variables
#define GLOBAL_VARIABLES_SECTION        struct _GVAR_STRUCT_NAME
#define GLOBAL_VARIABLES_SECTION_BEGIN  struct _GVAR_STRUCT_NAME {
#define GLOBAL_VARIABLES_SECTION_END    };

#ifdef __cplusplus
        inline void* operator new     (size_t size) {return malloc(size);}
        inline void* operator new[]   (size_t size) {return malloc(size);}
        inline void  operator delete  (void* ptr  ) {free(ptr);}
        inline void  operator delete[](void* ptr  ) {free(ptr);}
#       define _PROGMAN_CXX extern "C"
#       define _PROGMAN_EXTERN_C extern "C"
#else
#       define _PROGMAN_CXX
#       define _PROGMAN_EXTERN_C extern
#endif

#define _IMPORT_PROGRAM(_name_)\
        _PROGMAN_EXTERN_C const size_t __builtin_app_##_name_##_gs__;\
        _PROGMAN_EXTERN_C const size_t __builtin_app_##_name_##_ss__;\
        _PROGMAN_EXTERN_C int __builtin_app_##_name_##_main(int, char**)

#define int_main(_name_, stack_depth, argc, argv)\
        _PROGMAN_CXX const size_t __builtin_app_##_name_##_gs__ = sizeof(struct _GVAR_STRUCT_NAME);\
        _PROGMAN_CXX const size_t __builtin_app_##_name_##_ss__ = stack_depth;\
        _PROGMAN_CXX int __builtin_app_##_name_##_main(argc, argv)

#define _PROGRAM_CONFIG(_name_) \
        {.name          = #_name_,\
         .main          = __builtin_app_##_name_##_main,\
         .globals_size  = &__builtin_app_##_name_##_gs__,\
         .stack_depth   = &__builtin_app_##_name_##_ss__}

/*==============================================================================
  Exported types, enums definitions
==============================================================================*/
/***/
typedef int (*process_func_t)(int, char**);

/** KERNELSPACE: program attributes */
struct _prog_data {
        const char     *name;
        const size_t   *globals_size;
        const size_t   *stack_depth;
        process_func_t  main;
};

/** USERSPACE: Process attributes */
typedef struct {
        FILE       *f_stdin;            //!< stdin  file object pointer (major)
        FILE       *f_stdout;           //!< stdout file object pointer (major)
        FILE       *f_stderr;           //!< stderr file object pointer (major)
        const char *p_stdin;            //!< stdin  file path (minor)
        const char *p_stdout;           //!< stdout file path (minor)
        const char *p_stderr;           //!< stderr file path (minor)
        const char *cwd;                //!< working directory path
} process_attr_t;

/** USERSPACE: thread attributes */
typedef struct {
        pid_t  pid;
        size_t files_count;
        size_t dir_count;
        size_t mutexes_count;
        size_t semaphores_count;
        size_t queue_count;
        size_t threads_count;
        size_t memory_block_count;
        size_t memory_usage;
        size_t cpu_load_cnt;
        size_t stack_size;
        size_t stack_free;
} process_stat_t;

/** USERSPACE: thread attributes */
typedef struct {
        size_t stack_depth;
} thread_attr_t;

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
extern int         _process_create                              (const char*, const process_attr_t*, pid_t*);
extern int         _process_destroy                             (pid_t, int*);
extern int         _process_exit                                (task_t*, int);
extern int         _process_abort                               (task_t*);
extern const char *_process_get_CWD                             (task_t*);
extern int         _process_register_resource                   (task_t*, res_header_t*);
extern int         _process_release_resource                    (task_t*, res_header_t*, res_type_t);
extern int         _process_get_statistics                      (size_t, process_stat_t*);
extern FILE       *_process_get_stderr                          (task_t*);
extern int         _process_get_pid                             (task_t*, pid_t*);

extern int         _process_memalloc                            (task_t*, size_t, void**, bool);
extern int         _process_memfree                             (task_t*, void*);
extern void        _copy_task_context_to_standard_variables     (void);
extern void        _copy_standard_variables_to_task_context     (void);

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

#endif /* _PROCESS_H_ */
/*==============================================================================
  End of file
==============================================================================*/
