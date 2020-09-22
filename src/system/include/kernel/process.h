/*=========================================================================*//**
@file    process.h

@author  Daniel Zorychta

@brief   This file support processes

@note    Copyright (C) 2015 Daniel Zorychta <daniel.zorychta@gmail.com>

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
#if __OS_TASK_KWORKER_MODE__ == 2
#define _FS_STACK __OS_IO_STACK_DEPTH__
#else
#define _FS_STACK 0
#endif

#define STACK_DEPTH_MINIMAL             ((1   * (__OS_TASK_MIN_STACK_DEPTH__)) + (__OS_IRQ_STACK_DEPTH__) + (_FS_STACK))
#define STACK_DEPTH_VERY_LOW            ((2   * (__OS_TASK_MIN_STACK_DEPTH__)) + (__OS_IRQ_STACK_DEPTH__) + (_FS_STACK))
#define STACK_DEPTH_LOW                 ((4   * (__OS_TASK_MIN_STACK_DEPTH__)) + (__OS_IRQ_STACK_DEPTH__) + (_FS_STACK))
#define STACK_DEPTH_MEDIUM              ((8   * (__OS_TASK_MIN_STACK_DEPTH__)) + (__OS_IRQ_STACK_DEPTH__) + (_FS_STACK))
#define STACK_DEPTH_LARGE               ((16  * (__OS_TASK_MIN_STACK_DEPTH__)) + (__OS_IRQ_STACK_DEPTH__) + (_FS_STACK))
#define STACK_DEPTH_VERY_LARGE          ((32  * (__OS_TASK_MIN_STACK_DEPTH__)) + (__OS_IRQ_STACK_DEPTH__) + (_FS_STACK))
#define STACK_DEPTH_HUGE                ((64  * (__OS_TASK_MIN_STACK_DEPTH__)) + (__OS_IRQ_STACK_DEPTH__) + (_FS_STACK))
#define STACK_DEPTH_VERY_HUGE           ((128 * (__OS_TASK_MIN_STACK_DEPTH__)) + (__OS_IRQ_STACK_DEPTH__) + (_FS_STACK))
#define STACK_DEPTH_CUSTOM(depth)       ((depth) + (__OS_IRQ_STACK_DEPTH__))

#define _PROCESS_SYSCALL_FLAG(tid)      (1 << (tid))
#define _PROCESS_EXIT_FLAG(tid)         (1 << ((tid) + 12))

#define _GVAR_STRUCT_NAME               global_variables
#define GLOBAL_VARIABLES_SECTION        struct _GVAR_STRUCT_NAME
#define GLOBAL_VARIABLES_SECTION_BEGIN  struct _GVAR_STRUCT_NAME {
#define GLOBAL_VARIABLES_SECTION_END    };

#ifdef __cplusplus
#       include <stdlib.h>
#       include <stddef.h>
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

#define PROGRAM_PARAMS(_name_, stack_depth)\
        static int main(int argc, char *argv[]);\
        _PROGMAN_CXX const size_t __builtin_app_##_name_##_gs__ = sizeof(struct _GVAR_STRUCT_NAME);\
        _PROGMAN_CXX const size_t __builtin_app_##_name_##_ss__ = stack_depth;\
        _PROGMAN_CXX int __builtin_app_##_name_##_main(int argc, char *argv[]) {return main(argc, argv);}

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
/**KERNELSPACE: process (program) function type */
typedef int (*process_func_t)(int, char**);

/** USERSPACE: thread function */
typedef void (*thread_func_t)(void *arg);

/** KERNELSPACE: process descriptor */
typedef struct _process _process_t;

/** KERNELSPACE: thread descriptor */
typedef struct _thread _thread_t;

/** KERNELSPACE: program attributes. Doxygen documentation in fs.h. */
struct _prog_data {
        const char     *name;           //!< program name
        const size_t   *globals_size;   //!< size of program global variables
        const size_t   *stack_depth;    //!< stack depth
        process_func_t  main;           //!< program main function
};

/** USERSPACE: process attributes */
typedef struct {
        FILE       *f_stdin;            //!< stdin  file object pointer (major)
        FILE       *f_stdout;           //!< stdout file object pointer (major)
        FILE       *f_stderr;           //!< stderr file object pointer (major)
        const char *p_stdin;            //!< stdin  file path (minor)
        const char *p_stdout;           //!< stdout file path (minor)
        const char *p_stderr;           //!< stderr file path (minor)
        const char *cwd;                //!< working directory path
        i16_t       priority;           //!< process priority
        bool        detached;           //!< independent process (no parent)
} process_attr_t;

/** USERSPACE: process statistics */
typedef struct {
        const char *name;               //!< process name
        pid_t       pid;                //!< process ID
        size_t      memory_usage;       //!< memory usage (allocated by process)
        u16_t       memory_block_count; //!< number of used memory blocks
        u16_t       files_count;        //!< number of opened files
        u16_t       dir_count;          //!< number of opened directories
        u16_t       mutexes_count;      //!< number of used mutexes
        u16_t       semaphores_count;   //!< number of used semaphores
        u16_t       queue_count;        //!< number of used queues
        u16_t       socket_count;       //!< number of used sockets
        u16_t       threads_count;      //!< number of threads
        u16_t       CPU_load;           //!< CPU load (1% = 10)
        u16_t       stack_size;         //!< stack size
        u16_t       stack_max_usage;    //!< max stack usage
        i16_t       priority;           //!< priority
        u16_t       syscalls;           //!< syscalls per second
} process_stat_t;

/** USERSPACE: thread statistics */
typedef struct {
        tid_t       tid;                //!< thread ID
        u16_t       CPU_load;           //!< CPU load (1% = 10)
        u16_t       stack_size;         //!< stack size
        u16_t       stack_max_usage;    //!< max stack usage
        i16_t       priority;           //!< priority
        u16_t       syscalls;           //!< syscalls per second
} thread_stat_t;

/** USERSPACE: thread attributes */
typedef struct {
        size_t stack_depth;             //!< stack depth
        i16_t  priority;                //!< thread priority
        bool   detached;                //!< independent thread (without join possibility)
} thread_attr_t;

/** USERSPACE: average CPU load */
typedef struct {
        u16_t avg1sec;                  //!< average CPU laod within 1 second (1% = 10)
        u16_t avg1min;                  //!< average CPU load within 1 minute (1% = 10)
        u16_t avg5min;                  //!< average CPU load within 5 minutes (1% = 10)
        u16_t avg15min;                 //!< average CPU load within 15 minutes (1% = 10)
} avg_CPU_load_t;

/*==============================================================================
  Exported object declarations
==============================================================================*/
extern FILE                     *stdin;
extern FILE                     *stdout;
extern FILE                     *stderr;
extern struct _GVAR_STRUCT_NAME *global;
extern int                      _errno;
extern const struct _prog_data  _prog_table[];
extern const int                _prog_table_size;

/*==============================================================================
  Exported function prototypes
==============================================================================*/
extern void        _process_clean_up_killed_processes   (void);
extern int         _process_create                      (const char*, const process_attr_t*, pid_t*);
extern int         _process_kill                        (pid_t);
extern void        _process_remove_zombie               (_process_t*, int*);
extern int         _process_kill                        (pid_t);
extern void        _process_exit                        (_process_t*, int);
extern void        _process_abort                       (_process_t*);
extern const char *_process_get_CWD                     (_process_t*);
extern int         _process_set_CWD                     (_process_t*, const char*);
extern int         _process_register_resource           (_process_t*, res_header_t*);
extern int         _process_release_resource            (_process_t*, res_header_t*, res_type_t);
extern FILE       *_process_get_stderr                  (_process_t*);
extern const char *_process_get_name                    (_process_t*);
extern size_t      _process_get_count                   (void);
extern _process_t *_process_get_active                  (void);
extern int         _process_get_pid                     (_process_t*, pid_t*);
extern int         _process_get_event_flags             (_process_t*, flag_t**);
extern int         _process_get_priority                (pid_t, int*);
extern int         _process_get_container               (pid_t, _process_t**);
extern int         _process_get_stat_seek               (size_t, process_stat_t*);
extern int         _process_get_stat_pid                (pid_t, process_stat_t*);
extern tid_t       _process_get_active_thread           (_process_t *process);
extern pid_t       _process_get_active_process_pid      (void);
extern u8_t        _process_get_max_threads             (_process_t*);
extern int         _process_thread_create               (_process_t*, thread_func_t, const thread_attr_t*, void*, tid_t*);
extern int         _process_thread_kill                 (_process_t*, tid_t);
extern task_t     *_process_thread_get_task             (_process_t *proc, tid_t tid);
extern int         _process_thread_get_stat             (pid_t, tid_t tid, thread_stat_t*);
extern void        _process_syscall_stat_inc            (_process_t *proc, _process_t *kworker);
extern void        _task_switched_in                    (task_t *task, void *task_tag);
extern void        _task_switched_out                   (task_t *task, void *task_tag);
extern void        _calculate_CPU_load                  (void);
extern int         _get_average_CPU_load                (avg_CPU_load_t*);
extern void        _task_get_process_container          (task_t*, _process_t**, tid_t*);

/*==============================================================================
  Exported inline functions
==============================================================================*/
static inline const struct _prog_data *_get_programs_table(void)
{
        return _prog_table;
}

static inline int _get_programs_table_size(void)
{
        return _prog_table_size;
}

#ifdef __cplusplus
}
#endif

#endif /* _PROCESS_H_ */
/*==============================================================================
  End of file
==============================================================================*/
