/*=========================================================================*//**
@file    kwrapper.c

@author  Daniel Zorychta

@brief   Kernel wrapper

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

/*==============================================================================
  Include files
==============================================================================*/
#include "kernel/kwrapper.h"
#include "kernel/ktypes.h"
#include "lib/cast.h"
#include "errno.h"

/*==============================================================================
  Local symbolic constants/macros
==============================================================================*/

/*==============================================================================
  Local types, enums definitions
==============================================================================*/
struct mutex {
        void         *object;
        struct mutex *self;
        bool          recursive;
};

struct queue {
        void         *object;
        struct queue *self;
};

struct sem {
        void       *object;
        struct sem *self;
};

/*==============================================================================
  Local function prototypes
==============================================================================*/

/*==============================================================================
  Local object definitions
==============================================================================*/

/*==============================================================================
  Exported object definitions
==============================================================================*/

/*==============================================================================
  Function definitions
==============================================================================*/
//==============================================================================
/**
 * @brief Function check that semaphore is a valid object
 * @param sem           semaphore object to examine
 * @return If object is valid then true is returned, false otherwise.
 */
//==============================================================================
static bool is_semaphore_valid(sem_t *sem)
{
        return sem && sem->self == sem && sem->object;
}

//==============================================================================
/**
 * @brief Function check that mutex is a valid object
 * @param mtx           mutex object to examine
 * @return If object is valid then true is returned, false otherwise.
 */
//==============================================================================
static bool is_mutex_valid(mutex_t *mtx)
{
        return mtx && mtx->self == mtx && mtx->object;
}

//==============================================================================
/**
 * @brief Function check that queue is a valid object
 * @param queue         queue object to examine
 * @return If object is valid then true is returned, false otherwise.
 */
//==============================================================================
static bool is_queue_valid(queue_t *queue)
{
        return queue && queue->self == queue && queue->object;
}

//==============================================================================
/**
 * @brief Function create new task and if enabled add to monitor list
 *
 * Function by default allocate memory for task data (localized in task tag)
 * which is used to cpu load calculation and standard IO and etc.
 *
 * @param[in ] func             task code
 * @param[in ] name             task name
 * @param[in ] stack_depth      stack deep
 * @param[in ] argv             argument pointer (can be NULL)
 * @param[in ] tag              user's tag (can be NULL)
 * @param[out] task             task handle (can be NULL)
 *
 * @return On of errno value.
 */
//==============================================================================
int _task_create(void (*func)(void*), const char *name, const uint stack_depth, void *argv, void *tag, task_t **task)
{
        int result = EINVAL;

        if (func && name && stack_depth) {
                int scheduler_status = xTaskGetSchedulerState();
                uint child_priority  = PRIORITY(0);

                if (scheduler_status != taskSCHEDULER_NOT_STARTED) {
                        child_priority = uxTaskPriorityGet(THIS_TASK);
                }

                taskENTER_CRITICAL();

                if (xTaskCreate(func, name, stack_depth, argv, child_priority, task) == pdPASS) {

                        if (scheduler_status != taskSCHEDULER_NOT_STARTED) {
                                vTaskSuspend(task);
                        }

                        vTaskSetApplicationTaskTag(task, (void *)tag);

                        result = ESUCC;
                } else {
                        result = ENOMEM;
                }

                taskEXIT_CRITICAL();
        }

        return result;
}

//==============================================================================
/**
 * @brief Function delete task
 * Function remove task from monitoring list, and next delete the task from OS
 * list. Function resume the parent task before delete.
 *
 * @param *taskHdl       task handle
 *
 * @return One of errno value.
 */
//==============================================================================
int _task_destroy(task_t *taskHdl)
{
        if (taskHdl) {
                vTaskDelete(taskHdl);
                return ESUCC;
        } else {
                return EINVAL;
        }
}

//==============================================================================
/**
 * @brief Function wait for task exit
 */
//==============================================================================
void _task_exit(void)
{
        /* request to delete task */
        _task_destroy(_task_get_handle());

        /* wait for exit */
        for (;;) {}
}

//==============================================================================
/**
 * @brief Function suspend selected task
 *
 * @param[in] *taskhdl          task handle
 */
//==============================================================================
void _task_suspend(task_t *taskhdl)
{
        if (taskhdl) {
                vTaskSuspend(taskhdl);
        }
}

//==============================================================================
/**
 * @brief Function resume selected task
 *
 * @param[in] *taskhdl          task handle
 */
//==============================================================================
void _task_resume(task_t *taskhdl)
{
        if (taskhdl) {
                vTaskResume(taskhdl);
        }
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
bool _task_resume_from_ISR(task_t *taskhdl)
{
        if (taskhdl)
                return xTaskResumeFromISR(taskhdl);
        else
                return false;
}

//==============================================================================
/**
 * @brief Function return name of selected task
 *
 * @param[in] *taskhdl          task handle
 *
 * @return name of selected task or NULL if error
 */
//==============================================================================
char *_task_get_name_of(task_t *taskhdl)
{
        if (taskhdl)
                return pcTaskGetTaskName(taskhdl);
        else
                return NULL;
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
int _task_get_priority_of(task_t *taskhdl)
{
        if (taskhdl)
                return (int)(uxTaskPriorityGet(taskhdl) - (CONFIG_RTOS_TASK_MAX_PRIORITIES / 2));
        else
                return 0;
}

//==============================================================================
/**
 * @brief Function set selected task priority
 *
 * @param[in] *taskhdl          task handle
 * @param[in]  priority         priority
 */
//==============================================================================
void _task_set_priority_of(task_t *taskhdl, const int priority)
{
        if (taskhdl)
                vTaskPrioritySet(taskhdl, PRIORITY(priority));
}

//==============================================================================
/**
 * @brief Function return a free stack level of selected task
 *
 * @param[in] *taskhdl          task handle
 *
 * @return free stack level or -1 if error
 */
//==============================================================================
int _task_get_free_stack_of(task_t *taskhdl)
{
        if (taskhdl)
                return uxTaskGetStackHighWaterMark(taskhdl);
        else
                return -1;
}

//==============================================================================
/**
 * @brief Function create binary semaphore
 *
 * @param[in]  cnt_max          max count value (1 for binary)
 * @param[in]  cnt_init         initial value (0 or 1 for binary)
 * @param[out] sem              created semaphore handle
 *
 * @return One of errno values.
 */
//==============================================================================
int _semaphore_create(const uint cnt_max, const uint cnt_init, sem_t **sem)
{
        int result = EINVAL;

        if (cnt_max > 0 && sem) {
                result = _kcalloc(_MM_KRN, 1, sizeof(struct sem), reinterpret_cast(void**, sem));
                if (result == ESUCC) {

                        if (cnt_max == 1) {
                                vSemaphoreCreateBinary((*sem)->object);
                                if ((*sem)->object) {
                                        if (cnt_init) {
                                                xSemaphoreGive((*sem)->object);
                                        } else {
                                                xSemaphoreTake((*sem)->object, 0);
                                        }
                                }
                        } else {
                                (*sem)->object = xSemaphoreCreateCounting(cnt_max, cnt_init);
                        }

                        if ((*sem)->object) {
                                (*sem)->self = *sem;
                        } else {
                                _kfree(_MM_KRN, reinterpret_cast(void**, sem));
                                result = ENOMEM;
                        }
                }
        }

        return result;
}

//==============================================================================
/**
 * @brief Function delete semaphore
 *
 * @param[in] *sem      semaphore object
 *
 * @return One of errno values.
 */
//==============================================================================
int _semaphore_destroy(sem_t *sem)
{
        if (is_semaphore_valid(sem)) {
                vSemaphoreDelete(sem->object);
                sem->object = NULL;
                sem->self   = NULL;
                return _kfree(_MM_KRN, reinterpret_cast(void**, &sem));
        } else {
                return EINVAL;
        }
}

//==============================================================================
/**
 * @brief Function wait for semaphore
 *
 * @param[in] *sem              semaphore object
 * @param[in]  blocktime_ms     semaphore polling time
 *
 * @return One of errno values.
 */
//==============================================================================
int _semaphore_wait(sem_t *sem, const uint blocktime_ms)
{
        if (is_semaphore_valid(sem)) {
                bool r = xSemaphoreTake(sem->object, MS2TICK((TickType_t)blocktime_ms));
                return r ? ESUCC : ETIME;
        } else {
                return EINVAL;
        }
}

//==============================================================================
/**
 * @brief Function signal semaphore
 *
 * @param[in] *sem      semaphore object
 *
 * @return One of errno values.
 */
//==============================================================================
int _semaphore_signal(sem_t *sem)
{
        if (is_semaphore_valid(sem)) {
                bool r = xSemaphoreGive(sem->object);
                return r ? ESUCC : EBUSY;
        } else {
                return EINVAL;
        }
}

//==============================================================================
/**
 * @brief Function wait for semaphore from ISR
 *
 * @param[in]  *sem              semaphore object
 * @param[out] *task_woken       true if higher priority task woken, otherwise false (can be NULL)
 *
 * @return One of errno values.
 */
//==============================================================================
int _semaphore_wait_from_ISR(sem_t *sem, bool *task_woken)
{
        if (is_semaphore_valid(sem)) {
                BaseType_t woken = 0;
                int ret = xSemaphoreTakeFromISR(sem->object, &woken);

                if (task_woken) {
                        *task_woken = (bool)woken;
                }

                return ret ? ESUCC : EBUSY;
        } else {
                return EINVAL;
        }
}

//==============================================================================
/**
 * @brief Function signal semaphore from ISR
 *
 * @param[in]  *sem              semaphore object
 * @param[out] *task_woken       true if higher priority task woken, otherwise false (can be NULL)
 *
 * @return One of errno values.
 */
//==============================================================================
int _semaphore_signal_from_ISR(sem_t *sem, bool *task_woken)
{
        if (is_semaphore_valid(sem)) {
                BaseType_t woken = 0;
                int ret = xSemaphoreGiveFromISR(sem->object, &woken);

                if (task_woken) {
                        *task_woken = (bool)woken;
                }

                return ret ? ESUCC : EBUSY;
        } else {
                return EINVAL;
        }
}

//==============================================================================
/**
 * @brief Function create new mutex
 *
 * @param[in]  type     mutex type
 * @param[out] mtx      created mutex handle
 *
 * @return One of errno values.
 */
//==============================================================================
int _mutex_create(enum mutex_type type, mutex_t **mtx)
{
        int result = EINVAL;

        if (type <= MUTEX_TYPE_NORMAL && mtx) {
                result = _kcalloc(_MM_KRN, 1, sizeof(struct mutex), reinterpret_cast(void**, mtx));
                if (result == ESUCC) {
                        if (type == MUTEX_TYPE_RECURSIVE) {
                                (*mtx)->object    = xSemaphoreCreateRecursiveMutex();
                                (*mtx)->recursive = true;
                        } else {
                                (*mtx)->object    = xSemaphoreCreateMutex();
                                (*mtx)->recursive = false;
                        }

                        if ((*mtx)->object) {
                                (*mtx)->self = *mtx;
                        } else {
                                _kfree(_MM_KRN, reinterpret_cast(void**, mtx));
                                result = ENOMEM;
                        }
                }
        }

        return result;
}

//==============================================================================
/**
 * @brief Function destroy mutex
 *
 * @param[in] *mutex    mutex object
 *
 * @return One of errno values.
 */
//==============================================================================
int _mutex_destroy(mutex_t *mutex)
{
        if (is_mutex_valid(mutex)) {
                vSemaphoreDelete(mutex->object);
                mutex->object = NULL;
                mutex->self   = NULL;
                return _kfree(_MM_KRN, reinterpret_cast(void**, &mutex));
        } else {
                return EINVAL;
        }
}

//==============================================================================
/**
 * @brief Function lock mutex
 *
 * @param[in] mutex             mutex object
 * @param[in] blocktime_ms      polling time
 *
 * @return One of errno values.
 */
//==============================================================================
int _mutex_lock(mutex_t *mutex, const uint blocktime_ms)
{
        if (is_mutex_valid(mutex)) {
                bool status;
                if (mutex->recursive) {
                        status = xSemaphoreTakeRecursive(mutex->object, MS2TICK((TickType_t)blocktime_ms));
                } else {
                        status = xSemaphoreTake(mutex->object, MS2TICK((TickType_t)blocktime_ms));
                }

                return status ? ESUCC : ETIME;
        } else {
                return EINVAL;
        }
}

//==============================================================================
/**
 * @brief Function unlock mutex
 *
 * @param[in] *mutex            mutex object
 *
 * @return One of errno values.
 */
//==============================================================================
int _mutex_unlock(mutex_t *mutex)
{
        if (is_mutex_valid(mutex)) {
                bool status;
                if (mutex->recursive) {
                        status = xSemaphoreGiveRecursive(mutex->object);
                } else {
                        status = xSemaphoreGive(mutex->object);
                }

                return status ? ESUCC : EBUSY;
        } else {
                return EINVAL;
        }
}

//==============================================================================
/**
 * @brief Function create new queue
 *
 * @param[in]  length           queue length
 * @param[in]  item_size        queue item size
 * @param[out] queue            created queue
 *
 * @return One of errno values.
 */
//==============================================================================
int _queue_create(const uint length, const uint item_size, queue_t **queue)
{
        int result = EINVAL;

        if (length && item_size && queue) {
                result = _kcalloc(_MM_KRN, 1, sizeof(struct queue), reinterpret_cast(void**, queue));
                if (result == ESUCC) {
                        (*queue)->object = xQueueCreate(length, item_size);
                        if ((*queue)->object) {
                                (*queue)->self = *queue;
                        } else {
                                _kfree(_MM_KRN, reinterpret_cast(void**, &queue));
                                result = ENOMEM;
                        }
                }
        }

        return result;
}

//==============================================================================
/**
 * @brief Function delete queue
 *
 * @param[in] *queue            queue object
 *
 * @return One of errno values.
 */
//==============================================================================
int _queue_destroy(queue_t *queue)
{
        if (is_queue_valid(queue)) {
                vQueueDelete(queue->object);
                queue->object = NULL;
                queue->self   = NULL;
                _kfree(_MM_KRN, reinterpret_cast(void**, &queue));
                return ESUCC;
        } else {
                return EINVAL;
        }
}

//==============================================================================
/**
 * @brief Function reset queue
 *
 * @param[in] *queue            queue object
 *
 * @return One of errno values.
 */
//==============================================================================
int _queue_reset(queue_t *queue)
{
        if (is_queue_valid(queue)) {
                BaseType_t r = xQueueReset(queue->object);
                return r == pdTRUE ? ESUCC : EBUSY;
        } else {
                return EINVAL;
        }
}

//==============================================================================
/**
 * @brief Function send queue
 *
 * @param[in] *queue            queue object
 * @param[in] *item             item
 * @param[in]  waittime_ms      wait time
 *
 * @return One of errno values.
 */
//==============================================================================
int _queue_send(queue_t *queue, const void *item, const uint waittime_ms)
{
        if (is_queue_valid(queue) && item) {
                BaseType_t r = xQueueSend(queue->object, item, MS2TICK((TickType_t)waittime_ms));
                return r == pdTRUE ? ESUCC : ENOSPC;
        } else {
                return EINVAL;
        }
}

//==============================================================================
/**
 * @brief Function send queue
 *
 * @param[in]  *queue            queue object
 * @param[in]  *item             item
 * @param[out] *task_woken       true if higher priority task woken, otherwise false (can be NULL)
 *
 * @return One of errno values.
 */
//==============================================================================
int _queue_send_from_ISR(queue_t *queue, const void *item, bool *task_woken)
{
        if (is_queue_valid(queue) && item) {
                BaseType_t woken = 0;
                int ret = xQueueSendFromISR(queue->object, item, &woken);

                if (task_woken) {
                        *task_woken = (bool)woken;
                }

                return ret == pdTRUE ? ESUCC : ENOSPC;
        } else {
                return EINVAL;
        }
}

//==============================================================================
/**
 * @brief Function send queue
 *
 * @param[in]  *queue            queue object
 * @param[out] *item             item
 * @param[in]   waittime_ms      wait time
 *
 * @return One of errno values.
 */
//==============================================================================
int _queue_receive(queue_t *queue, void *item, const uint waittime_ms)
{
        if (is_queue_valid(queue) && item) {
                BaseType_t r = xQueueReceive(queue->object, item, MS2TICK((TickType_t)waittime_ms));
                return r == pdTRUE ? ESUCC : EAGAIN;
        } else {
                return EINVAL;
        }
}

//==============================================================================
/**
 * @brief Function receive queue from ISR
 *
 * @param[in]  queue            queue object
 * @param[out] item             item
 * @param[out] task_woken       true if higher priority task woke, otherwise false (can be NULL)
 *
 * @return One of errno values.
 */
//==============================================================================
int _queue_receive_from_ISR(queue_t *queue, void *item, bool *task_woken)
{
        if (is_queue_valid(queue) && item) {
                BaseType_t woken = 0;
                BaseType_t ret   = xQueueReceiveFromISR(queue->object, item, &woken);

                if (task_woken) {
                        *task_woken = (bool)woken;
                }

                return ret == pdTRUE ? ESUCC : EAGAIN;
        } else {
                return EINVAL;
        }
}

//==============================================================================
/**
 * @brief Function peek queue
 *
 * @param[in]  *queue            queue object
 * @param[out] *item             item
 * @param[in]   waittime_ms      wait time
 *
 * @return One of errno values.
 */
//==============================================================================
int _queue_receive_peek(queue_t *queue, void *item, const uint waittime_ms)
{
        if (is_queue_valid(queue) && item) {
                BaseType_t r = xQueuePeek(queue->object, item, MS2TICK((TickType_t)waittime_ms));
                return r == pdTRUE ? ESUCC : EAGAIN;
        } else {
                return EINVAL;
        }
}

//==============================================================================
/**
 * @brief Function gets number of items in queue
 *
 * @param[in]  queue            queue object
 * @param[out] items            number of items in queue
 *
 * @return One of errno values.
 */
//==============================================================================
int _queue_get_number_of_items(queue_t *queue, size_t *items)
{
        if (is_queue_valid(queue) && items) {
                UBaseType_t nomsg = uxQueueMessagesWaiting(queue->object);
                *items = nomsg;
                return ESUCC;
        } else {
                return EINVAL;
        }
}

//==============================================================================
/**
 * @brief Function gets number of items in queue from ISR
 *
 * @param[in]  queue            queue object
 * @param[out] items            number of items in queue
 *
 * @return One of errno values.
 */
//==============================================================================
int _queue_get_number_of_items_from_ISR(queue_t *queue, size_t *items)
{
        if (is_queue_valid(queue) && items) {
                UBaseType_t nomsg = uxQueueMessagesWaitingFromISR(queue->object);
                *items = nomsg;
                return ESUCC;
        } else {
                return EINVAL;
        }
}

//==============================================================================
/**
 * @brief Function gets number of free items in queue
 *
 * @param[in]  queue            queue object
 * @param[out] items            number of items in queue
 *
 * @return One of errno values.
 */
//==============================================================================
int _queue_get_space_available(queue_t *queue, size_t *items)
{
        if (is_queue_valid(queue) && items) {
                UBaseType_t nomsg = uxQueueSpacesAvailable(queue);
                *items = nomsg;
                return ESUCC;
        } else {
                return EINVAL;
        }
}

/*==============================================================================
  End of file
==============================================================================*/
