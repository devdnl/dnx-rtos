#ifndef _KWRAPPER_H_
#define _KWRAPPER_H_
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

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Include files
==============================================================================*/
#include "kernel/ktypes.h"
#include "core/systypes.h"
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

/** STANDART STACK SIZES */
#define STACK_DEPTH_MINIMAL             (1  * (CONFIG_RTOS_TASK_MIN_STACK_DEPTH))
#define STACK_DEPTH_VERY_LOW            (2  * (CONFIG_RTOS_TASK_MIN_STACK_DEPTH))
#define STACK_DEPTH_LOW                 (4  * (CONFIG_RTOS_TASK_MIN_STACK_DEPTH))
#define STACK_DEPTH_MEDIUM              (6  * (CONFIG_RTOS_TASK_MIN_STACK_DEPTH))
#define STACK_DEPTH_LARGE               (8  * (CONFIG_RTOS_TASK_MIN_STACK_DEPTH))
#define STACK_DEPTH_VERY_LARGE          (10 * (CONFIG_RTOS_TASK_MIN_STACK_DEPTH))
#define STACK_DEPTH_HUGE                (12 * (CONFIG_RTOS_TASK_MIN_STACK_DEPTH))
#define STACK_DEPTH_VERY_HUGE           (14 * (CONFIG_RTOS_TASK_MIN_STACK_DEPTH))
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
        struct vfs_file *f_stdin;        /* stdin file                         */
        struct vfs_file *f_stdout;       /* stdout file                        */
        struct vfs_file *f_stderr;       /* stderr file                        */
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
static inline void sleep_ms(uint milliseconds)
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
static inline void sleep(uint seconds)
{
        vTaskDelay(MS2TICK(seconds * 1000UL));
}

//==============================================================================
/**
 * @brief Function suspend selected task
 *
 * @param[in] *taskhdl          task handle
 */
//==============================================================================
static inline void suspend_task(task_t *taskhdl)
{
        vTaskSuspend(taskhdl);
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
 * @brief Function resume selected task
 *
 * @param[in] *taskhdl          task handle
 */
//==============================================================================
static inline void resume_task(task_t *taskhdl)
{
        vTaskResume(taskhdl);
}

//==============================================================================
/**
 * @brief Function resume selected task from ISR
 *
 * @param[in] *taskhdl          task handle
 *
 * @retval true                 if yield required
 * @retval false                if yield not required
 */
//==============================================================================
static inline int resume_task_from_ISR(task_t *taskhdl)
{
        return xTaskResumeFromISR(taskhdl);
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
 * @brief Function return name of selected task
 *
 * @param[in] *taskhdl          task handle
 *
 * @return name of selected task
 */
//==============================================================================
static inline char *get_task_name(task_t *taskhdl)
{
        return (char *)pcTaskGetTaskName(taskhdl);
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
 * @brief Function return task priority
 *
 * @param[in] *taskhdl          task handle
 *
 * @return priority of selected task
 */
//==============================================================================
static inline int get_task_priority(task_t *taskhdl)
{
        return (int)(uxTaskPriorityGet(taskhdl) - (CONFIG_RTOS_TASK_MAX_PRIORITIES / 2));
}

//==============================================================================
/**
 * @brief Function set selected task priority
 *
 * @param[in] *taskhdl          task handle
 * @param[in]  priority         priority
 */
//==============================================================================
static inline void set_task_priority(task_t *taskhdl, int priority)
{
        vTaskPrioritySet(taskhdl, PRIORITY(priority));
}

//==============================================================================
/**
 * @brief Function set priority of current task
 *
 * @param[in]  priority         priority
 */
//==============================================================================
static inline void set_priority(int priority)
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
 * @brief Function return a free stack level of selected task
 *
 * @param[in] *taskhdl          task handle
 *
 * @return free stack level
 */
//==============================================================================
static inline int get_task_free_stack(task_t *taskhdl)
{
        return uxTaskGetStackHighWaterMark(taskhdl);
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
static inline task_t *get_parent_handle(void)
{
        return _get_task_data(THIS_TASK)->f_parent_task;
}

//==============================================================================
/**
 * @brief Function set global variables
 *
 * @param[in] *mem
 */
//==============================================================================
static inline void set_global_variables(void *mem)
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
static inline void set_stdin(struct vfs_file *file)
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
static inline void set_stdout(struct vfs_file *file)
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
static inline void set_stderr(struct vfs_file *file)
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
 * @brief Function delete semaphore
 *
 * @param[in] *sem      semaphore object
 */
//==============================================================================
static inline void delete_semaphore(sem_t *sem)
{
        vSemaphoreDelete(sem);
}

//==============================================================================
/**
 * @brief Function take semaphore
 *
 * @param[in] *sem              semaphore object
 * @param[in]  blocktime_ms     semaphore polling time
 *
 * @retval true         semaphore taken (SEMAPHORE_TAKEN)
 * @retval false        semaphore not taken (SEMAPHORE_NOT_TAKEN)
 */
//==============================================================================
static inline bool take_semaphore(sem_t *sem, uint blocktime_ms)
{
        return xSemaphoreTake(sem, MS2TICK((portTickType)blocktime_ms));
}

//==============================================================================
/**
 * @brief Function give semaphore
 *
 * @param[in] *sem      semaphore object
 *
 * @retval true         semaphore taken (SEMAPHORE_GIVEN)
 * @retval false        semaphore not taken (SEMAPHORE_NOT_GIVEN)
 */
//==============================================================================
static inline bool give_semaphore(sem_t *sem)
{
        return xSemaphoreGive(sem);
}

//==============================================================================
/**
 * @brief Function take semaphore from ISR
 *
 * @param[in]  *sem              semaphore object
 * @param[out] *task_woken       true if higher priority task woken, otherwise false
 *
 * @retval true         semaphore taken (SEMAPHORE_TAKEN)
 * @retval false        semaphore not taken (SEMAPHORE_NOT_TAKEN)
 */
//==============================================================================
static inline bool take_semaphore_from_ISR(sem_t *sem, bool *task_woken)
{
        return xSemaphoreTakeFromISR(sem, (signed portBASE_TYPE *)task_woken);
}

//==============================================================================
/**
 * @brief Function give semaphore from ISR
 *
 * @param[in]  *sem              semaphore object
 * @param[out] *task_woken       1 if higher priority task woken, otherwise 0
 *
 * @retval true         semaphore taken (SEMAPHORE_GIVEN)
 * @retval false        semaphore not taken (SEMAPHORE_NOT_GIVEN)
 */
//==============================================================================
static inline bool give_semaphore_from_ISR(sem_t *sem, int *task_woken)
{
        return xSemaphoreGiveFromISR(sem, (signed portBASE_TYPE *)task_woken);
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
static inline sem_t *new_counting_semaphore(uint cnt_max, uint cnt_init)
{
        return xSemaphoreCreateCounting(cnt_max, cnt_init);
}

//==============================================================================
/**
 * @brief Function delete semaphore
 *
 * @param[in] *sem      semaphore object
 */
//==============================================================================
static inline void delete_counting_semaphore(sem_t *sem)
{
        vSemaphoreDelete(sem);
}

//==============================================================================
/**
 * @brief Function take semaphore
 *
 * @param[in] *sem              semaphore object
 * @param[in]  blocktime_ms     semaphore polling time
 *
 * @retval true         semaphore taken (SEMAPHORE_TAKEN)
 * @retval false        semaphore not taken (SEMAPHORE_NOT_TAKEN)
 */
//==============================================================================
static inline bool take_counting_semaphore(sem_t *sem, uint blocktime_ms)
{
        return xSemaphoreTake(sem, MS2TICK((portTickType)blocktime_ms));
}

//==============================================================================
/**
 * @brief Function give semaphore
 *
 * @param[in] *sem      semaphore object
 *
 * @retval true         semaphore given (SEMAPHORE_GIVEN)
 * @retval false        semaphore not given (SEMAPHORE_NOT_GIVEN)
 */
//==============================================================================
static inline bool give_counting_semaphore(sem_t *sem)
{
        return xSemaphoreGive(sem);
}

//==============================================================================
/**
 * @brief Function take semaphore from ISR
 *
 * @param[in]  *sem              semaphore object
 * @param[out] *task_woken       true if higher priority task woken, otherwise false
 *
 * @retval true         semaphore taken (SEMAPHORE_TAKEN)
 * @retval false        semaphore not taken (SEMAPHORE_NOT_TAKEN)
 */
//==============================================================================
static inline bool take_counting_semaphore_from_ISR(sem_t *sem, int *task_woken)
{
        return xSemaphoreTakeFromISR(sem, (signed portBASE_TYPE *)task_woken);
}

//==============================================================================
/**
 * @brief Function give semaphore from ISR
 *
 * @param[in]  *sem              semaphore object
 * @param[out] *task_woken       1 if higher priority task woken, otherwise 0
 *
 * @retval true         semaphore given (SEMAPHORE_GIVEN)
 * @retval false        semaphore not given (SEMAPHORE_NOT_GIVEN)
 */
//==============================================================================
static inline bool give_counting_semaphore_from_ISR(sem_t *sem, int *task_woken)
{
        return xSemaphoreGiveFromISR(sem, (signed portBASE_TYPE *)task_woken);
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
 * @brief Function delete mutex
 *
 * @param[in] *mutex    mutex object
 */
//==============================================================================
static inline void delete_mutex(mutex_t *mutex)
{
        return vSemaphoreDelete(mutex);
}

//==============================================================================
/**
 * @brief Function lock mutex
 *
 * @param[in] *mutex            mutex object
 * @param[in]  blocktime_ms     polling time
 *
 * @retval true         mutex locked (MUTEX_LOCKED)
 * @retval false        mutex not locked (MUTEX_NOT_LOCKED)
 */
//==============================================================================
static inline bool lock_mutex(mutex_t *mutex, uint blocktime_ms)
{
        return xSemaphoreTake(mutex, MS2TICK((portTickType)blocktime_ms));
}

//==============================================================================
/**
 * @brief Function unlock mutex
 *
 * @param[in] *mutex            mutex object
 *
 * @retval true         mutex unlocked
 * @retval false        mutex still locked
 */
//==============================================================================
static inline bool unlock_mutex(mutex_t *mutex)
{
        return xSemaphoreGive(mutex);
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
 * @brief Function delete mutex
 *
 * @param[in] *mutex    mutex object
 */
//==============================================================================
static inline void delete_recursive_mutex(mutex_t *mutex)
{
        return vSemaphoreDelete(mutex);
}

//==============================================================================
/**
 * @brief Function lock mutex
 *
 * @param[in] *mutex            mutex object
 * @param[in]  blocktime_ms     polling time
 *
 * @retval true         mutex locked (MUTEX_LOCKED)
 * @retval false        mutex not locked (MUTEX_NOT_LOCKED)
 */
//==============================================================================
static inline bool lock_recursive_mutex(mutex_t *mutex, uint blocktime_ms)
{
        return xSemaphoreTakeRecursive(mutex, MS2TICK((portTickType)blocktime_ms));
}

//==============================================================================
/**
 * @brief Function unlock mutex
 *
 * @param[in] *mutex            mutex object
 *
 * @retval true         mutex unlocked
 * @retval false        mutex still locked
 */
//==============================================================================
static inline bool unlock_recursive_mutex(mutex_t *mutex)
{
        return xSemaphoreGiveRecursive(mutex);
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
static inline queue_t *new_queue(uint length, uint item_size)
{
        return xQueueCreate((unsigned portBASE_TYPE)length, (unsigned portBASE_TYPE)item_size);
}

//==============================================================================
/**
 * @brief Function delete queue
 *
 * @param[in] *queue            queue object
 */
//==============================================================================
static inline void delete_queue(queue_t *queue)
{
        vQueueDelete(queue);
}

//==============================================================================
/**
 * @brief Function reset queue
 *
 * @param[in] *queue            queue object
 */
//==============================================================================
static inline void reset_queue(queue_t *queue)
{
        xQueueReset(queue);
}

//==============================================================================
/**
 * @brief Function send queue
 *
 * @param[in] *queue            queue object
 * @param[in] *item             item
 * @param[in]  waittime_ms      wait time
 *
 * @retval true         item posted (QUEUE_ITEM_POSTED)
 * @retval false        item not posted (QUEUE_ITEM_NOT_POSTED, QUEUE_FULL)
 */
//==============================================================================
static inline bool send_queue(queue_t *queue, void *item, uint waittime_ms)
{
        return xQueueSend(queue, item, MS2TICK((portTickType)waittime_ms));
}

//==============================================================================
/**
 * @brief Function send queue
 *
 * @param[in]  *queue            queue object
 * @param[in]  *item             item
 * @param[out] *task_woken       1 if higher priority task woken, otherwise 0
 *
 * @retval true         item posted (QUEUE_ITEM_POSTED)
 * @retval false        item not posted (QUEUE_ITEM_NOT_POSTED, QUEUE_FULL)
 */
//==============================================================================
static inline bool send_queue_from_ISR(queue_t *queue, void *item, int *task_woken)
{
        return xQueueSendFromISR(queue, item, (signed portBASE_TYPE *)task_woken);
}

//==============================================================================
/**
 * @brief Function send queue
 *
 * @param[in]  *queue            queue object
 * @param[out] *item             item
 * @param[in]   waittime_ms      wait time
 *
 * @retval true         item received (QUEUE_ITEM_RECEIVED)
 * @retval false        item not recieved (QUEUE_ITEM_NOT_RECEIVED)
 */
//==============================================================================
static inline bool receive_queue(queue_t *queue, void *item, uint waittime_ms)
{
        return xQueueReceive(queue, item, MS2TICK((portTickType)waittime_ms));
}

//==============================================================================
/**
 * @brief Function x queue
 *
 * @param[in]  *queue            queue object
 * @param[out] *item             item
 * @param[out] *task_woken       1 if higher priority task woken, otherwise 0
 *
 * @retval true         item received (QUEUE_ITEM_RECEIVED)
 * @retval false        item not recieved (QUEUE_ITEM_NOT_RECEIVED)
 */
//==============================================================================
static inline bool receive_queue_from_ISR(queue_t *queue, void *item, int *task_woken)
{
        return xQueueReceiveFromISR(queue, item, (signed portBASE_TYPE *)task_woken);
}

//==============================================================================
/**
 * @brief Function x queue
 *
 * @param[in]  *queue            queue object
 * @param[out] *item             item
 * @param[in]   waittime_ms      wait time
 *
 * @retval true         item received (QUEUE_ITEM_RECEIVED)
 * @retval false        item not recieved (QUEUE_ITEM_NOT_RECEIVED)
 */
//==============================================================================
static inline bool receive_peek_queue(queue_t *queue, void *item, uint waittime_ms)
{
        return xQueuePeek(queue, item, MS2TICK((portTickType)waittime_ms));
}

//==============================================================================
/**
 * @brief Function x queue
 *
 * @param[in] *queue            queue object
 *
 * @return a number of items in queue
 */
//==============================================================================
static inline int get_number_of_items_in_queue(queue_t *queue)
{
        return uxQueueMessagesWaiting(queue);
}

//==============================================================================
/**
 * @brief Function x queue
 *
 * @param[in] *queue            queue object
 *
 * @return a number of items in queue
 */
//==============================================================================
static inline int get_number_of_items_in_queue_from_ISR(queue_t *queue)
{
        return uxQueueMessagesWaitingFromISR(queue);
}

/*==============================================================================
  Exported function prototypes
==============================================================================*/
extern task_t *new_task         (void (*func)(void*), const char*, uint, void*);
extern void    delete_task      (task_t *taskHdl);
extern sem_t  *new_semaphore    (void);
extern void    task_exit        (void);

#ifdef __cplusplus
}
#endif

#endif /* _KWRAPPER_H_ */
/*==============================================================================
  End of file
==============================================================================*/
