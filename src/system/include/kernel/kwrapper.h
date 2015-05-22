/*=========================================================================*//**
@file    kwrapper.h

@author  Daniel Zorychta

@brief   Kernel wrapper

@note    Copyright (C) 2012  Daniel Zorychta <daniel.zorychta@gmail.com>

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

#ifndef _KWRAPPER_H_
#define _KWRAPPER_H_

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Include files
==============================================================================*/
#include <stdbool.h>
#include "kernel/builtinfunc.h"
#include "kernel/ktypes.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "config.h"

/*==============================================================================
  Exported symbolic constants/macros
==============================================================================*/
/** UNDEFINE MEMORY MANAGEMENT DEFINITIONS LOCALIZED IN FreeRTOS.h file (IMPORTANT!) */
//#undef free
//#undef malloc

/** KERNEL NAME */
#define _KERNEL_NAME                    "FreeRTOS"
#define _KERNEL_VERSION                 tskKERNEL_VERSION_NUMBER

/** OS BASIC DEFINITIONS */
#define _THIS_TASK                      NULL
#define MAX_DELAY_MS                    ((portMAX_DELAY) - 1000)
#define MAX_DELAY_S                     (MAX_DELAY_MS / 1000)

/** PRIORITIES */
#define PRIORITY(prio)                  (prio + (configMAX_PRIORITIES / 2))
#define PRIORITY_LOWEST                 (-(int)(configMAX_PRIORITIES / 2))
#define PRIORITY_NORMAL                 0
#define PRIORITY_HIGHEST                ((int)(configMAX_PRIORITIES / 2))

/*==============================================================================
  Exported types, enums definitions
==============================================================================*/
/** USERSPACE/KERNELSPACE: mutex type */
enum mutex_type {
        MUTEX_TYPE_RECURSIVE,
        MUTEX_TYPE_NORMAL
};

/*==============================================================================
  Exported object declarations
==============================================================================*/

/*==============================================================================
  Exported function prototypes
==============================================================================*/
// TODO kwrapper: automatically detected ISR functions

extern void     _kernel_start                      (void);
extern uint     _kernel_get_time_ms                (void);
extern uint     _kernel_get_tick_counter           (void);
extern int      _kernel_get_number_of_tasks        (void);
extern void     _kernel_scheduler_lock             (void);
extern void     _kernel_scheduler_unlock           (void);

extern int      _task_create                       (task_func_t, const char*, const size_t, void*, void*, task_t**);
extern void     _task_destroy                      (task_t*);
extern void     _task_exit                         (void);
extern void     _task_suspend                      (task_t*);
extern void     _task_resume                       (task_t*);
extern bool     _task_resume_from_ISR              (task_t*);
extern char    *_task_get_name                     (task_t*);
extern int      _task_get_priority                 (task_t*);
extern void     _task_set_priority                 (task_t*, const int);
extern int      _task_get_free_stack               (task_t*);
extern void     _task_yield                        (void);
extern void     _task_yield_from_ISR               (void);
extern task_t  *_task_get_handle                   (void);
extern void     _task_set_tag                      (task_t*, void*);
extern void    *_task_get_tag                      (task_t*);

extern int      _semaphore_create                  (const uint, const uint, sem_t**);
extern int      _semaphore_destroy                 (sem_t*);
extern int      _semaphore_wait                    (sem_t*, const uint);
extern int      _semaphore_signal                  (sem_t*);
extern int      _semaphore_wait_from_ISR           (sem_t*, bool*);
extern int      _semaphore_signal_from_ISR         (sem_t*, bool*);

extern int      _mutex_create                      (enum mutex_type, mutex_t**);
extern int      _mutex_destroy                     (mutex_t*);
extern int      _mutex_lock                        (mutex_t*, const uint);
extern int      _mutex_unlock                      (mutex_t*);

extern int      _queue_create                      (const uint, const uint, queue_t**);
extern int      _queue_destroy                     (queue_t*);
extern int      _queue_reset                       (queue_t*);
extern int      _queue_send                        (queue_t*, const void*, const uint);
extern int      _queue_send_from_ISR               (queue_t*, const void*, bool*);
extern int      _queue_receive                     (queue_t*, void*, const uint);
extern int      _queue_receive_from_ISR            (queue_t*, void*, bool*);
extern int      _queue_receive_peek                (queue_t*, void*, const uint);
extern int      _queue_get_number_of_items         (queue_t*, size_t*);
extern int      _queue_get_number_of_items_from_ISR(queue_t*, size_t*);
extern int      _queue_get_space_available         (queue_t*, size_t*);

extern void     _critical_section_begin            (void);
extern void     _critical_section_end              (void);

extern void     _ISR_disable                       (void);
extern void     _ISR_enable                        (void);

extern void     _sleep_ms                          (const uint);
extern void     _sleep                             (const uint);
extern void     _sleep_until_ms                    (const uint, int*);
extern void     _sleep_until                       (const uint, int*);

/*==============================================================================
  Exported inline functions
==============================================================================*/

#ifdef __cplusplus
}
#endif

#endif /* _KWRAPPER_H_ */
/*==============================================================================
  End of file
==============================================================================*/
