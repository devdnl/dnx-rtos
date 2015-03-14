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
#include "kernel/ktypes.h"
#include "core/vfs.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "config.h"

/*==============================================================================
  Exported symbolic constants/macros
==============================================================================*/
/** UNDEFINE MEMORY MANAGEMENT DEFINITIONS LOCALIZED IN FreeRTOS.h file (IMPORTANT!) */
#undef free
#undef malloc

/** STANDARD STACK SIZES */
#define STACK_DEPTH_MINIMAL             ((1   * (CONFIG_RTOS_TASK_MIN_STACK_DEPTH)) + (CONFIG_RTOS_FILE_SYSTEM_STACK_DEPTH) + (CONFIG_RTOS_IRQ_STACK_DEPTH))
#define STACK_DEPTH_VERY_LOW            ((2   * (CONFIG_RTOS_TASK_MIN_STACK_DEPTH)) + (CONFIG_RTOS_FILE_SYSTEM_STACK_DEPTH) + (CONFIG_RTOS_IRQ_STACK_DEPTH))
#define STACK_DEPTH_LOW                 ((4   * (CONFIG_RTOS_TASK_MIN_STACK_DEPTH)) + (CONFIG_RTOS_FILE_SYSTEM_STACK_DEPTH) + (CONFIG_RTOS_IRQ_STACK_DEPTH))
#define STACK_DEPTH_MEDIUM              ((8   * (CONFIG_RTOS_TASK_MIN_STACK_DEPTH)) + (CONFIG_RTOS_FILE_SYSTEM_STACK_DEPTH) + (CONFIG_RTOS_IRQ_STACK_DEPTH))
#define STACK_DEPTH_LARGE               ((16  * (CONFIG_RTOS_TASK_MIN_STACK_DEPTH)) + (CONFIG_RTOS_FILE_SYSTEM_STACK_DEPTH) + (CONFIG_RTOS_IRQ_STACK_DEPTH))
#define STACK_DEPTH_VERY_LARGE          ((32  * (CONFIG_RTOS_TASK_MIN_STACK_DEPTH)) + (CONFIG_RTOS_FILE_SYSTEM_STACK_DEPTH) + (CONFIG_RTOS_IRQ_STACK_DEPTH))
#define STACK_DEPTH_HUGE                ((64  * (CONFIG_RTOS_TASK_MIN_STACK_DEPTH)) + (CONFIG_RTOS_FILE_SYSTEM_STACK_DEPTH) + (CONFIG_RTOS_IRQ_STACK_DEPTH))
#define STACK_DEPTH_VERY_HUGE           ((128 * (CONFIG_RTOS_TASK_MIN_STACK_DEPTH)) + (CONFIG_RTOS_FILE_SYSTEM_STACK_DEPTH) + (CONFIG_RTOS_IRQ_STACK_DEPTH))
#define STACK_DEPTH_USER(depth)         (depth)

/** OS BASIC DEFINITIONS */
#define THIS_TASK                       NULL
#define MAX_DELAY_MS                    ((portMAX_DELAY) - 1000)
#define MAX_DELAY_S                     (MAX_DELAY_MS / 1000)

/** CALCULATIONS */
#define PRIORITY(prio)                  (prio + (configMAX_PRIORITIES / 2))
#define NORMAL_PRIORITY                 0
#define LOWEST_PRIORITY                 (-(int)(configMAX_PRIORITIES / 2))
#define HIGHEST_PRIORITY                ((int)(configMAX_PRIORITIES / 2))
#define LOW_PRIORITY                    LOWEST_PRIORITY
#define MAX_PRIORITY                    HIGHEST_PRIORITY
#define _CEILING(x,y)                   (((x) + (y) - 1) / (y))
#define MS2TICK(ms)                     (ms <= (1000/(configTICK_RATE_HZ)) ? 1 : _CEILING(ms,(1000/(configTICK_RATE_HZ))))

/*==============================================================================
  Exported types, enums definitions
==============================================================================*/
typedef enum _task_type {
        TASK_TYPE_RAW,
        TASK_TYPE_PROCESS,
        TASK_TYPE_THREAD
} task_type_t;

typedef struct _task_data {
        FILE            *f_stdin;               /* stdin file                         */
        FILE            *f_stdout;              /* stdout file                        */
        FILE            *f_stderr;              /* stderr file                        */
        const char      *f_cwd;                 /* current working path               */
        void            *f_mem;                 /* address to global variables        */
        void            *f_monitor;             /* pointer to task monitor data       */
        task_t          *f_parent_task;         /* program's parent task              */
        void            *f_task_object;         /* thread object                      */
        u32_t            f_cpu_usage;           /* counter used to calculate CPU load */
        int              f_errno;               /* program error number               */
        task_type_t      f_task_type:2;         /* task type                          */
        bool             f_task_kill:1;         /* task is already killing            */
        uint             f_mutex_section:24;    /* mutex section counter              */
} _task_data_t;

enum mutex_type {
        MUTEX_RECURSIVE,
        MUTEX_NORMAL
};

/*==============================================================================
  Exported object declarations
==============================================================================*/

/*==============================================================================
  Exported function prototypes
==============================================================================*/
extern task_t       *_task_new                          (void (*)(void*), const char*, const uint, void*);
extern void          _task_delete                       (task_t*);
extern void          _task_exit                         (void);
extern void          _task_suspend                      (task_t*);
extern void          _task_resume                       (task_t*);
extern bool          _task_resume_from_ISR              (task_t*);
extern char         *_task_get_name_of                  (task_t*);
extern int           _task_get_priority_of              (task_t*);
extern void          _task_set_priority_of              (task_t*, const int);
extern int           _task_get_free_stack_of            (task_t*);
extern _task_data_t *_task_get_data                     (void);
extern sem_t        *_semaphore_new                     (const uint, const uint);
extern void          _semaphore_delete                  (sem_t*);
extern bool          _semaphore_wait                    (sem_t*, const uint);
extern bool          _semaphore_signal                  (sem_t*);
extern bool          _semaphore_wait_from_ISR           (sem_t*, bool*);
extern bool          _semaphore_signal_from_ISR         (sem_t*, bool*);
extern mutex_t      *_mutex_new                         (enum mutex_type);
extern void          _mutex_delete                      (mutex_t*);
extern bool          _mutex_lock                        (mutex_t*, const uint);
extern bool          _mutex_unlock                      (mutex_t*);
extern queue_t      *_queue_new                         (const uint, const uint);
extern void          _queue_delete                      (queue_t*);
extern void          _queue_reset                       (queue_t*);
extern bool          _queue_send                        (queue_t*, const void*, const uint);
extern bool          _queue_send_from_ISR               (queue_t*, const void*, bool*);
extern bool          _queue_receive                     (queue_t*, void*, const uint);
extern bool          _queue_receive_from_ISR            (queue_t*, void*, bool*);
extern bool          _queue_receive_peek                (queue_t*, void*, const uint);
extern int           _queue_get_number_of_items         (queue_t*);
extern int           _queue_get_number_of_items_from_ISR(queue_t*);
extern int           _queue_get_space_available         (queue_t *queue);

/*==============================================================================
  Exported inline functions
==============================================================================*/
//==============================================================================
/**
 * @brief Function start kernel scheduler
 */
//==============================================================================
static inline void _kernel_start(void)
{
        vTaskStartScheduler();
}

//==============================================================================
/**
 * @brief Function return OS time in milliseconds
 *
 * @return a OS time in milliseconds
 */
//==============================================================================
static inline uint _kernel_get_time_ms(void)
{
        return (xTaskGetTickCount() * ((1000/(configTICK_RATE_HZ))));
}

//==============================================================================
/**
 * @brief Function return tick counter
 *
 * @return a tick counter value
 */
//==============================================================================
static inline uint _kernel_get_tick_counter(void)
{
        return (uint)xTaskGetTickCount();
}

//==============================================================================
/**
 * @brief Function return a number of task
 *
 * @return a number of tasks
 */
//==============================================================================
static inline int _kernel_get_number_of_tasks(void)
{
        return uxTaskGetNumberOfTasks();
}

//==============================================================================
/**
 * @brief Function suspend current task
 */
//==============================================================================
static inline void _task_suspend_now(void)
{
        vTaskSuspend(THIS_TASK);
}

//==============================================================================
/**
 * @brief Function yield task
 */
//==============================================================================
static inline void _task_yield(void)
{
        taskYIELD();
}

//==============================================================================
/**
 * @brief Function yield task from ISR
 */
//==============================================================================
static inline void _task_yield_from_ISR(void)
{
#ifdef portYIELD_FROM_ISR
        portYIELD_FROM_ISR(true);
#else
        taskYIELD();
#endif
}

//==============================================================================
/**
 * @brief Function return name of current task
 *
 * @return name of current task
 */
//==============================================================================
static inline char *_task_get_name(void)
{
        return pcTaskGetTaskName(THIS_TASK);
}

//==============================================================================
/**
 * @brief Function return current task handle object address
 *
 * @return current task handle
 */
//==============================================================================
static inline task_t *_task_get_handle(void)
{
        return xTaskGetCurrentTaskHandle();
}

//==============================================================================
/**
 * @brief Function set priority of current task
 *
 * @param[in]  priority         priority
 */
//==============================================================================
static inline void _task_set_priority(const int priority)
{
        vTaskPrioritySet(THIS_TASK, PRIORITY(priority));
}

//==============================================================================
/**
 * @brief Function return priority of current task
 *
 * @return current task priority
 */
//==============================================================================
static inline int _task_get_priority(void)
{
        return (int)(uxTaskPriorityGet(THIS_TASK) - (CONFIG_RTOS_TASK_MAX_PRIORITIES / 2));
}

//==============================================================================
/**
 * @brief Function return a free stack level of current task
 *
 * @return free stack level
 */
//==============================================================================
static inline int _task_get_free_stack(void)
{
        return uxTaskGetStackHighWaterMark(THIS_TASK);
}

//==============================================================================
/**
 * @brief Function set task tag
 *
 * @param[in] taskhdl           task handle
 * @param[in] tag               task tag
 */
//==============================================================================
static inline void _task_set_tag(task_t *taskhdl, void *tag)
{
        vTaskSetApplicationTaskTag(taskhdl, (TaskHookFunction_t)tag);
}

//==============================================================================
/**
 * @brief Function return task tag
 *
 * @param[in] *taskhdl          task handle
 *
 * @return task tag
 */
//==============================================================================
static inline void *_task_get_tag(task_t *taskhdl)
{
        return (void*)xTaskGetApplicationTaskTag(taskhdl);
}

//==============================================================================
/**
 * @brief Function return data of selected task
 *
 * @param[in] *taskhdl          task handle
 *
 * @return task data
 */
//==============================================================================
static inline struct _task_data *_task_get_data_of(task_t *taskhdl)
{
        return (struct _task_data *)_task_get_tag(taskhdl);
}

//==============================================================================
/**
 * @brief Function set task monitor data
 *
 * @param[in] *taskhdl          task handle
 * @param[in] *mem              task monitor data block
 */
//==============================================================================
static inline void _task_set_monitor_data(task_t *taskhdl, void *mem)
{
        _task_get_data_of(taskhdl)->f_monitor = mem;
}

//==============================================================================
/**
 * @brief Function get task monitor data
 *
 * @param[in] *taskhdl          task handle
 *
 * @return task monitor data
 */
//==============================================================================
static inline void *_task_get_monitor_data(task_t *taskhdl)
{
        return _task_get_data_of(taskhdl)->f_monitor;
}

//==============================================================================
/**
 * @brief Function return parent task handle
 *
 * @return parent task handle
 */
//==============================================================================
static inline task_t *_task_get_parent_handle(void)
{
        return _task_get_data_of(THIS_TASK)->f_parent_task;
}

//==============================================================================
/**
 * @brief Function set global variables address
 *
 * @param[in] *mem
 */
//==============================================================================
static inline void _task_set_memory_address(void *mem)
{
        _task_get_data_of(THIS_TASK)->f_mem = mem;
}

//==============================================================================
/**
 * @brief Function set stdin file
 *
 * @param[in] *file
 */
//==============================================================================
static inline void _task_set_stdin(FILE *file)
{
        _task_get_data_of(THIS_TASK)->f_stdin = file;
}

//==============================================================================
/**
 * @brief Function set stdout file
 *
 * @param[in] *file
 */
//==============================================================================
static inline void _task_set_stdout(FILE *file)
{
        _task_get_data_of(THIS_TASK)->f_stdout = file;
}

//==============================================================================
/**
 * @brief Function set stderr file
 *
 * @param[in] *file
 */
//==============================================================================
static inline void _task_set_stderr(FILE *file)
{
        _task_get_data_of(THIS_TASK)->f_stderr = file;
}

//==============================================================================
/**
 * @brief Function set cwd path
 *
 * @param str           cwd string
 */
//==============================================================================
static inline void _task_set_cwd(const char *str)
{
        _task_get_data()->f_cwd = str;
}

//==============================================================================
/**
 * @brief Function enter to critical section
 */
//==============================================================================
static inline void _critical_section_begin(void)
{
        taskENTER_CRITICAL();
}

//==============================================================================
/**
 * @brief Function exit from critical section
 */
//==============================================================================
static inline void _critical_section_end(void)
{
        taskEXIT_CRITICAL();
}

//==============================================================================
/**
 * @brief Function disable interrupts
 */
//==============================================================================
static inline void _ISR_disable(void)
{
        taskDISABLE_INTERRUPTS();
}

//==============================================================================
/**
 * @brief Function enable interrupts
 */
//==============================================================================
static inline void _ISR_enable(void)
{
        taskENABLE_INTERRUPTS();
}

//==============================================================================
/**
 * @brief Function put to sleep task in milliseconds
 *
 * @param[in] milliseconds
 */
//==============================================================================
static inline void _sleep_ms(const uint milliseconds)
{
        vTaskDelay(MS2TICK(milliseconds));
}

//==============================================================================
/**
 * @brief Function put to sleep task in seconds
 *
 * @param[in] seconds
 */
//==============================================================================
static inline void _sleep(const uint seconds)
{
        vTaskDelay(MS2TICK(seconds * 1000UL));
}

//==============================================================================
/**
 * @brief Function sleep task in regular periods (reference argument)
 *
 * @param milliseconds          milliseconds
 * @param ref_time_ticks        reference time in OS ticks
 */
//==============================================================================
static inline void _sleep_until_ms(const uint milliseconds, int *ref_time_ticks)
{
        vTaskDelayUntil((TickType_t *)ref_time_ticks, MS2TICK(milliseconds));
}

//==============================================================================
/**
 * @brief Function sleep task in regular periods (reference argument)
 *
 * @param seconds       seconds
 * @param ref_time_ticks        reference time in OS ticks
 */
//==============================================================================
static inline void _sleep_until(const uint seconds, int *ref_time_ticks)
{
        vTaskDelayUntil((TickType_t *)ref_time_ticks, MS2TICK(seconds * 1000UL));
}

#ifdef __cplusplus
}
#endif

#endif /* _KWRAPPER_H_ */
/*==============================================================================
  End of file
==============================================================================*/
