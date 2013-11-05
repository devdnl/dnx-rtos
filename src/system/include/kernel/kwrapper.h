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
#include "kernel/ktypes.h"
#include "core/systypes.h"
#include "core/vfs.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "config.h"

/*==============================================================================
  Exported symbolic constants/macros
==============================================================================*/
/** error handling */
#if (pdTRUE != true)
#error "pdTRUE != true"
#endif

#if (pdFALSE != false)
#error "pdFALSE != false"
#endif

/** UNDEFINE MEMORY MANAGEMENT DEFINITIONS LOCALIZED IN FreeRTOS.h file (IMPORTANT!) */
#undef free
#undef malloc

/** STANDARD STACK SIZES */
#define STACK_DEPTH_MINIMAL             ((1  * (CONFIG_RTOS_TASK_MIN_STACK_DEPTH)) + (CONFIG_RTOS_FILE_SYSTEM_STACK_DEPTH) + (CONFIG_RTOS_IRQ_STACK_DEPTH))
#define STACK_DEPTH_VERY_LOW            ((2  * (CONFIG_RTOS_TASK_MIN_STACK_DEPTH)) + (CONFIG_RTOS_FILE_SYSTEM_STACK_DEPTH) + (CONFIG_RTOS_IRQ_STACK_DEPTH))
#define STACK_DEPTH_LOW                 ((4  * (CONFIG_RTOS_TASK_MIN_STACK_DEPTH)) + (CONFIG_RTOS_FILE_SYSTEM_STACK_DEPTH) + (CONFIG_RTOS_IRQ_STACK_DEPTH))
#define STACK_DEPTH_MEDIUM              ((6  * (CONFIG_RTOS_TASK_MIN_STACK_DEPTH)) + (CONFIG_RTOS_FILE_SYSTEM_STACK_DEPTH) + (CONFIG_RTOS_IRQ_STACK_DEPTH))
#define STACK_DEPTH_LARGE               ((8  * (CONFIG_RTOS_TASK_MIN_STACK_DEPTH)) + (CONFIG_RTOS_FILE_SYSTEM_STACK_DEPTH) + (CONFIG_RTOS_IRQ_STACK_DEPTH))
#define STACK_DEPTH_VERY_LARGE          ((10 * (CONFIG_RTOS_TASK_MIN_STACK_DEPTH)) + (CONFIG_RTOS_FILE_SYSTEM_STACK_DEPTH) + (CONFIG_RTOS_IRQ_STACK_DEPTH))
#define STACK_DEPTH_HUGE                ((12 * (CONFIG_RTOS_TASK_MIN_STACK_DEPTH)) + (CONFIG_RTOS_FILE_SYSTEM_STACK_DEPTH) + (CONFIG_RTOS_IRQ_STACK_DEPTH))
#define STACK_DEPTH_VERY_HUGE           ((14 * (CONFIG_RTOS_TASK_MIN_STACK_DEPTH)) + (CONFIG_RTOS_FILE_SYSTEM_STACK_DEPTH) + (CONFIG_RTOS_IRQ_STACK_DEPTH))
#define STACK_DEPTH_USER(depth)         (depth)

/** OS BASIC DEFINITIONS */
#define THIS_TASK                       NULL
#define MAX_DELAY                       (portMAX_DELAY / 1000)

/** CALCULATIONS */
#define PRIORITY(prio)                  (prio + (configMAX_PRIORITIES / 2))
#define LOWEST_PRIORITY                 (-(int)(configMAX_PRIORITIES / 2))
#define HIGHEST_PRIORITY                (configMAX_PRIORITIES / 2)
#define _CEILING(x,y)                   (((x) + (y) - 1) / (y))
#define MS2TICK(ms)                     (ms <= (1000/(configTICK_RATE_HZ)) ? 1 : _CEILING(ms,(1000/(configTICK_RATE_HZ))))

/** TASK LEVEL DEFINITIONS */
#define prepare_sleep_until()           unsigned long int __last_wake_time__ = get_tick_counter();
#define sleep_until(uint__seconds)      vTaskDelayUntil(&__last_wake_time__, MS2TICK((uint__seconds) * 1000UL))
#define sleep_ms_until(uint__msdelay)   vTaskDelayUntil(&__last_wake_time__, MS2TICK(uint__msdelay))

/** SEMAPHORE */
#define SEMAPHORE_TAKEN                 true
#define SEMAPHORE_NOT_TAKEN             false
#define SEMAPHORE_GIVEN                 true
#define SEMAPHORE_NOT_GIVEN             false

/** MUTEX */
#define MUTEX_LOCKED                    true
#define MUTEX_NOT_LOCKED                false

/** QUEUE */
#define QUEUE_ITEM_POSTED               true
#define QUEUE_ITEM_NOT_POSTED           false
#define QUEUE_FULL                      false
#define QUEUE_ITEM_RECIVED              true
#define QUEUE_ITEM_NOT_RECEIVED         false

/*==============================================================================
  Exported types, enums definitions
==============================================================================*/
struct task_data {
        FILE            *f_stdin;        /* stdin file                         */
        FILE            *f_stdout;       /* stdout file                        */
        FILE            *f_stderr;       /* stderr file                        */
        const char      *f_cwd;          /* current working path               */
        void            *f_global_vars;  /* address to global variables        */
        void            *f_user;         /* pointer to user data               */
        void            *f_monitor;      /* pointer to task monitor data       */
        task_t          *f_parent_task;  /* program's parent task              */
        u32_t            f_cpu_usage;    /* counter used to calculate CPU load */
        bool             f_program;      /* true if task is complex program    */
};

/*==============================================================================
  Exported object declarations
==============================================================================*/

/*==============================================================================
  Exported inline functions
==============================================================================*/
//==============================================================================
/**
 * @brief Function start kernel scheduler
 */
//==============================================================================
static inline void start_task_scheduler(void)
{
        vTaskStartScheduler();
}

//==============================================================================
/**
 * @brief Function put to sleep task in milliseconds
 *
 * @param[in] milliseconds
 */
//==============================================================================
static inline void sleep_ms(const uint milliseconds)
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
static inline void sleep(const uint seconds)
{
        vTaskDelay(MS2TICK(seconds * 1000UL));
}

//==============================================================================
/**
 * @brief Function suspend current task
 */
//==============================================================================
static inline void suspend_this_task(void)
{
        vTaskSuspend(THIS_TASK);
}

//==============================================================================
/**
 * @brief Function yield task
 */
//==============================================================================
static inline void yield_task(void)
{
        taskYIELD();
}

//==============================================================================
/**
 * @brief Function enter to critical section
 */
//==============================================================================
static inline void enter_critical_section(void)
{
        taskENTER_CRITICAL();
}

//==============================================================================
/**
 * @brief Function exit from critical section
 */
//==============================================================================
static inline void exit_critical_section(void)
{
        taskEXIT_CRITICAL();
}

//==============================================================================
/**
 * @brief Function disable interrupts
 */
//==============================================================================
static inline void disable_ISR(void)
{
        taskDISABLE_INTERRUPTS();
}

//==============================================================================
/**
 * @brief Function enable interrupts
 */
//==============================================================================
static inline void enable_ISR(void)
{
        taskENABLE_INTERRUPTS();
}

//==============================================================================
/**
 * @brief Function return tick counter
 *
 * @return a tick counter value
 */
//==============================================================================
static inline int get_tick_counter(void)
{
        return xTaskGetTickCount();
}

//==============================================================================
/**
 * @brief Function return OS time in milliseconds
 *
 * @return a OS time in milliseconds
 */
//==============================================================================
static inline int get_OS_time_ms(void)
{
        return (xTaskGetTickCount() * ((1000/(configTICK_RATE_HZ))));
}

//==============================================================================
/**
 * @brief Function return name of current task
 *
 * @return name of current task
 */
//==============================================================================
static inline char *get_this_task_name(void)
{
        return (char *)pcTaskGetTaskName(THIS_TASK);
}

//==============================================================================
/**
 * @brief Function return current task handle object address
 *
 * @return current task handle
 */
//==============================================================================
static inline task_t *get_task_handle(void)
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
static inline void set_priority(const int priority)
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
static inline int get_priority(void)
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
static inline int get_free_stack(void)
{
        return uxTaskGetStackHighWaterMark(THIS_TASK);
}

//==============================================================================
/**
 * @brief Function return a number of task
 *
 * @return a number of tasks
 */
//==============================================================================
static inline int get_number_of_tasks(void)
{
        return uxTaskGetNumberOfTasks();
}

//==============================================================================
/**
 * @brief Function set task tag
 *
 * @param[in] *taskhdl          task handle
 */
//==============================================================================
static inline void _set_task_tag(task_t *taskhdl, void *tag)
{
        vTaskSetApplicationTaskTag(taskhdl, tag);
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
static inline void *_get_task_tag(task_t *taskhdl)
{
        return (void*)xTaskGetApplicationTaskTag(taskhdl);
}

//==============================================================================
/**
 * @brief Function return data of this task
 *
 * @return this task data
 */
//==============================================================================
static inline struct task_data *_get_this_task_data(void)
{
        return (struct task_data*)_get_task_tag(THIS_TASK);
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
static inline struct task_data *_get_task_data(task_t *taskhdl)
{
        return (struct task_data*)_get_task_tag(taskhdl);
}

//==============================================================================
/**
 * @brief Function return parent task handle
 *
 * @return parent task handle
 */
//==============================================================================
static inline task_t *get_parent_task_handle(void)
{
        return _get_task_data(THIS_TASK)->f_parent_task;
}

//==============================================================================
/**
 * @brief Function set global variables address
 *
 * @param[in] *mem
 */
//==============================================================================
static inline void set_address_of_global_variables(void *mem)
{
        _get_task_data(THIS_TASK)->f_global_vars = mem;
}

//==============================================================================
/**
 * @brief Function set stdin file
 *
 * @param[in] *file
 */
//==============================================================================
static inline void set_stdin(FILE *file)
{
        _get_task_data(THIS_TASK)->f_stdin = file;
}

//==============================================================================
/**
 * @brief Function set stdout file
 *
 * @param[in] *file
 */
//==============================================================================
static inline void set_stdout(FILE *file)
{
        _get_task_data(THIS_TASK)->f_stdout = file;
}

//==============================================================================
/**
 * @brief Function set stderr file
 *
 * @param[in] *file
 */
//==============================================================================
static inline void set_stderr(FILE *file)
{
        _get_task_data(THIS_TASK)->f_stderr = file;
}

//==============================================================================
/**
 * @brief Function set task user data
 *
 * @param[in] *mem
 */
//==============================================================================
static inline void set_user_data(void *mem)
{
        _get_task_data(THIS_TASK)->f_user = mem;
}

//==============================================================================
/**
 * @brief Function get user data
 *
 * @return user data pointer
 */
//==============================================================================
static inline void *get_user_data(void)
{
        return _get_task_data(THIS_TASK)->f_user;
}

//==============================================================================
/**
 * @brief Function set task monitor data
 *
 * @param[in] *taskhdl          task handle
 * @param[in] *mem              task monitor data block
 */
//==============================================================================
static inline void _set_task_monitor_data(task_t *taskhdl, void *mem)
{
        _get_task_data(taskhdl)->f_monitor = mem;
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
static inline void *_get_task_monitor_data(task_t *taskhdl)
{
        return _get_task_data(taskhdl)->f_monitor;
}

//==============================================================================
/**
 * @brief Function create new counting semaphore
 *
 * @param[in] cnt_max           max counter
 * @param[in] cnt_init          counter init value
 *
 * @param pointer to semaphore object, otherwise NULL if error
 */
//==============================================================================
static inline sem_t *new_counting_semaphore(const uint cnt_max, const uint cnt_init)
{
        return xSemaphoreCreateCounting(cnt_max, cnt_init);
}

//==============================================================================
/**
 * @brief Function create new mutex
 *
 * @return pointer to mutex object, otherwise NULL if error
 */
//==============================================================================
static inline mutex_t *new_mutex(void)
{
        return xSemaphoreCreateMutex();
}

//==============================================================================
/**
 * @brief Function create new mutex
 *
 * @return pointer to mutex object, otherwise NULL if error
 */
//==============================================================================
static inline mutex_t *new_recursive_mutex(void)
{
        return xSemaphoreCreateRecursiveMutex();
}

//==============================================================================
/**
 * @brief Function create new queue
 *
 * @param[in] length            queue length
 * @param[in] item_size         queue item size
 *
 * @return pointer to queue object, otherwise NULL if error
 */
//==============================================================================
static inline queue_t *new_queue(uint length, const uint item_size)
{
        return xQueueCreate((unsigned portBASE_TYPE)length, (unsigned portBASE_TYPE)item_size);
}

/*==============================================================================
  Exported function prototypes
==============================================================================*/
extern task_t  *new_task                                (void (*)(void*), const char*, uint, void*);
extern void     delete_task                             (task_t*);
extern void     task_exit                               (void);
extern void     suspend_task                            (task_t*);
extern void     resume_task                             (task_t*);
extern int      resume_task_from_ISR                    (task_t*);
extern char    *get_task_name                           (task_t*);
extern int      get_task_priority                       (task_t*);
extern void     set_task_priority                       (task_t*, const int);
extern int      get_task_free_stack                     (task_t*);
extern sem_t   *new_semaphore                           (void);
extern void     delete_semaphore                        (sem_t*);
extern bool     take_semaphore                          (sem_t*, const uint);
extern bool     give_semaphore                          (sem_t*);
extern bool     take_semaphore_from_ISR                 (sem_t*, bool*);
extern bool     give_semaphore_from_ISR                 (sem_t*, int*);
extern void     delete_counting_semaphore               (sem_t*);
extern bool     take_counting_semaphore                 (sem_t*, const uint);
extern bool     give_counting_semaphore                 (sem_t*);
extern bool     take_counting_semaphore_from_ISR        (sem_t*, int*);
extern bool     give_counting_semaphore_from_ISR        (sem_t*, int*);
extern void     delete_mutex                            (mutex_t*);
extern bool     lock_mutex                              (mutex_t*, const uint);
extern bool     unlock_mutex                            (mutex_t*);
extern void     delete_recursive_mutex                  (mutex_t*);
extern bool     lock_recursive_mutex                    (mutex_t*, const uint);
extern bool     unlock_recursive_mutex                  (mutex_t*);
extern void     delete_queue                            (queue_t*);
extern void     reset_queue                             (queue_t*);
extern bool     send_queue                              (queue_t*, const void*, const uint);
extern bool     send_queue_from_ISR                     (queue_t*, const void*, int*);
extern bool     receive_queue                           (queue_t*, void*, const uint);
extern bool     receive_queue_from_ISR                  (queue_t*, void*, int*);
extern bool     receive_peek_queue                      (queue_t*, void*, const uint);
extern int      get_number_of_items_in_queue            (queue_t*);
extern int      get_number_of_items_in_queue_from_ISR   (queue_t*);
#ifdef __cplusplus
}
#endif

#endif /* _KWRAPPER_H_ */
/*==============================================================================
  End of file
==============================================================================*/
