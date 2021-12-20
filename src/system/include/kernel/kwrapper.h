/*=========================================================================*//**
@file    kwrapper.h

@author  Daniel Zorychta

@brief   Kernel wrapper

@note    Copyright (C) 2012  Daniel Zorychta <daniel.zorychta@gmail.com>

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

#ifndef _KWRAPPER_H_
#define _KWRAPPER_H_

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Include files
==============================================================================*/
#include <stdbool.h>
#include "lib/sys/types.h"
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

#define _MAX_DELAY_MS                   ((portMAX_DELAY) - 1000)
#define _MAX_DELAY_S                    (_MAX_DELAY_MS / 1000)

/** PRIORITIES */
#define _PRIORITY(prio)                 (prio + (configMAX_PRIORITIES / 2))
#define _PRIORITY_LOWEST                (-(int)(configMAX_PRIORITIES / 2))
#define _PRIORITY_NORMAL                0
#define _PRIORITY_HIGHEST               ((int)(configMAX_PRIORITIES / 2))

/*==============================================================================
  Exported types, enums definitions
==============================================================================*/
/** USERSPACE/KERNELSPACE: mutex type */
enum kmtx_type {
        KMTX_TYPE_RECURSIVE,
        KMTX_TYPE_NORMAL
};

/*==============================================================================
  Exported object declarations
==============================================================================*/

/*==============================================================================
  Exported function prototypes
==============================================================================*/
// TODO kwrapper: automatically detected ISR functions

extern void     _kernel_start                      (void);
extern u64_t    _kernel_get_time_ms                (void);
extern u64_t    _kernel_get_tick_counter           (void);
extern int      _kernel_get_number_of_tasks        (void);
extern void     _kernel_scheduler_lock             (void);
extern void     _kernel_scheduler_unlock           (void);
extern void     _kernel_release_resources          (void);
extern task_t  *_kernel_get_idle_task_handle       (void);

extern int      _task_create                       (task_func_t, const char*, const size_t, void*, task_t**);
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
extern void     _task_yield_from_ISR               (bool);
extern task_t  *_task_get_handle                   (void);
extern void     _task_set_storage_pointer          (task_t*, u8_t, void*);
extern void    *_task_get_storage_pointer          (task_t*, u8_t);

extern int      _semaphore_create                  (size_t, size_t, ksem_t**);
extern int      _semaphore_destroy                 (ksem_t*);
extern int      _semaphore_wait                    (ksem_t*, const u32_t);
extern int      _semaphore_signal                  (ksem_t*);
extern int      _semaphore_get_value               (ksem_t *sem, size_t *value);
extern int      _semaphore_wait_from_ISR           (ksem_t*, bool*);
extern int      _semaphore_signal_from_ISR         (ksem_t*, bool*);

extern int      _mutex_create                      (enum kmtx_type, kmtx_t**);
extern int      _mutex_destroy                     (kmtx_t*);
extern int      _mutex_lock                        (kmtx_t*, const u32_t);
extern int      _mutex_unlock                      (kmtx_t*);

extern int      _flag_create                       (kflag_t**);
extern int      _flag_destroy                      (kflag_t*);
extern int      _flag_wait                         (kflag_t*, u32_t, const u32_t);
extern int      _flag_set                          (kflag_t*, u32_t);
extern int      _flag_clear                        (kflag_t*, u32_t);
extern u32_t    _flag_get                          (kflag_t*);
extern u32_t    _flag_get_from_ISR                 (kflag_t*);

extern int      _queue_create                      (size_t, size_t, kqueue_t**);
extern int      _queue_destroy                     (kqueue_t*);
extern int      _queue_reset                       (kqueue_t*);
extern int      _queue_send                        (kqueue_t*, const void*, const u32_t);
extern int      _queue_send_from_ISR               (kqueue_t*, const void*, bool*);
extern int      _queue_receive                     (kqueue_t*, void*, const u32_t);
extern int      _queue_receive_from_ISR            (kqueue_t*, void*, bool*);
extern int      _queue_receive_peek                (kqueue_t*, void*, const u32_t);
extern int      _queue_get_number_of_items         (kqueue_t*, size_t*);
extern int      _queue_get_number_of_items_from_ISR(kqueue_t*, size_t*);
extern int      _queue_get_space_available         (kqueue_t*, size_t*);

extern void     _critical_section_begin            (void);
extern void     _critical_section_end              (void);

extern void     _ISR_disable                       (void);
extern void     _ISR_enable                        (void);

extern void     _sleep_ms                          (const u32_t);
extern void     _sleep_until_ms                    (const u32_t, u32_t*);
extern void     _sleep_until                       (const u32_t, u32_t*);

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
