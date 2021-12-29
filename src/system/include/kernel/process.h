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
#include "kernel/ktypes.h"

/*==============================================================================
  Exported symbolic constants/macros
==============================================================================*/
/** STANDARD STACK SIZES */
#define _FS_STACK __OS_IO_STACK_DEPTH__

#define _STACK_DEPTH_MINIMAL            ((1   * (__OS_TASK_MIN_STACK_DEPTH__)) + (__OS_IRQ_STACK_DEPTH__) + (_FS_STACK))
#define _STACK_DEPTH_VERY_LOW           ((2   * (__OS_TASK_MIN_STACK_DEPTH__)) + (__OS_IRQ_STACK_DEPTH__) + (_FS_STACK))
#define _STACK_DEPTH_LOW                ((4   * (__OS_TASK_MIN_STACK_DEPTH__)) + (__OS_IRQ_STACK_DEPTH__) + (_FS_STACK))
#define _STACK_DEPTH_MEDIUM             ((8   * (__OS_TASK_MIN_STACK_DEPTH__)) + (__OS_IRQ_STACK_DEPTH__) + (_FS_STACK))
#define _STACK_DEPTH_LARGE              ((16  * (__OS_TASK_MIN_STACK_DEPTH__)) + (__OS_IRQ_STACK_DEPTH__) + (_FS_STACK))
#define _STACK_DEPTH_VERY_LARGE         ((32  * (__OS_TASK_MIN_STACK_DEPTH__)) + (__OS_IRQ_STACK_DEPTH__) + (_FS_STACK))
#define _STACK_DEPTH_HUGE               ((64  * (__OS_TASK_MIN_STACK_DEPTH__)) + (__OS_IRQ_STACK_DEPTH__) + (_FS_STACK))
#define _STACK_DEPTH_VERY_HUGE          ((128 * (__OS_TASK_MIN_STACK_DEPTH__)) + (__OS_IRQ_STACK_DEPTH__) + (_FS_STACK))
#define _STACK_DEPTH_CUSTOM(depth)      ((depth) + (__OS_IRQ_STACK_DEPTH__))

#define _PROCESS_EXIT_FLAG(tid)         (1 << (tid))

/*==============================================================================
  Exported types, enums definitions
==============================================================================*/
/** KERNELSPACE: process descriptor */
typedef struct _process _process_t;

/** KERNELSPACE: thread descriptor */
typedef struct _thread _thread_t;

typedef struct {
        const char     *name;           //!< program name
        const size_t   *globals_size;   //!< size of program global variables
        const size_t   *stack_depth;    //!< stack depth
        process_func_t  main;           //!< program main function
} _program_entry_t;

typedef struct {
        uint32_t magic;
        uint32_t number_of_programs;
        const _program_entry_t *const program_entry;
} _program_table_desc_t;

/*==============================================================================
  Exported object declarations
==============================================================================*/
extern void *_stdin;
extern void *_stdout;
extern void *_stderr;
extern void *_global;
extern int   _errno;

/*==============================================================================
  Exported function prototypes
==============================================================================*/
extern void        _process_clean_up_killed_processes   (void);
extern int         _process_create                      (const char*, const _process_attr_t*, pid_t*);
extern int         _process_kill                        (pid_t);
extern void        _process_remove_zombie               (_process_t*, int*);
extern void        _process_exit                        (_process_t*, int);
extern void        _process_abort                       (_process_t*);
extern const char *_process_get_CWD                     (_process_t*);
extern int         _process_set_CWD                     (_process_t*, const char*);
extern int         _process_register_resource           (_process_t*, res_header_t*);
extern int         _process_release_resource            (_process_t*, res_header_t*, res_type_t);
extern int         _process_descriptor_free             (_process_t *process, int desc, res_type_t rtype);
extern int         _process_descriptor_allocate         (_process_t *process, int *desc, res_header_t *res);
extern int         _process_descriptor_get_resource     (_process_t *process, int desc, res_header_t **res);
extern kfile_t    *_process_get_stderr                  (_process_t*);
extern const char *_process_get_name                    (_process_t*);
extern size_t      _process_get_count                   (void);
extern _process_t *_process_get_active                  (void);
extern int         _process_get_pid                     (_process_t*, pid_t*);
extern int         _process_get_event_flags             (_process_t*, kflag_t**);
extern int         _process_get_priority                (pid_t, int*);
extern int         _process_get_container               (pid_t, _process_t**);
extern int         _process_get_stat_seek               (size_t, _process_stat_t*);
extern int         _process_get_stat_pid                (pid_t, _process_stat_t*);
extern tid_t       _process_get_active_thread           (_process_t*);
extern pid_t       _process_get_active_process_pid      (void);
extern u8_t        _process_get_max_threads             (_process_t*);
extern int         _process_thread_create               (_process_t*, thread_func_t, const _thread_attr_t*, void*, tid_t*);
extern int         _process_thread_kill                 (_process_t*, tid_t);
extern task_t     *_process_thread_get_task             (_process_t*, tid_t);
extern int         _process_thread_get_stat             (pid_t, tid_t, _thread_stat_t*);
extern void        _process_thread_exit                 (int);
extern int         _process_thread_get_status           (_process_t*, tid_t, int*);
extern void        _process_syscall_stat_inc            (_process_t*, _process_t*);
extern bool        _process_is_consistent               (void);
extern void        _process_enter_kernelspace           (_process_t*, u8_t syscall);
extern void        _process_exit_kernelspace            (_process_t*);
extern bool        _process_is_kernelspace              (_process_t*, tid_t);
extern u8_t        _process_get_curr_syscall            (_process_t*, tid_t);
extern void        _task_switched_in                    (task_t*, void*, void*);
extern void        _task_switched_out                   (task_t*, void*, void*);
extern void        _calculate_CPU_load                  (void);
extern int         _get_average_CPU_load                (_avg_CPU_load_t*);
extern void        _task_get_process_container          (task_t*, _process_t**, tid_t*);

extern const _program_table_desc_t *_get_programs_table(void);

/*==============================================================================
  Exported inline functions
==============================================================================*/

#ifdef __cplusplus
}
#endif

#endif /* _PROCESS_H_ */
/*==============================================================================
  End of file
==============================================================================*/
