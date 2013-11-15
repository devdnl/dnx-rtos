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

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Include files
==============================================================================*/
#include "kernel/kwrapper.h"
#include "core/sysmoni.h"

/*==============================================================================
  Local symbolic constants/macros
==============================================================================*/

/*==============================================================================
  Local types, enums definitions
==============================================================================*/

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
 * @brief Function create new task and if enabled add to monitor list
 *
 * Function by default allocate memory for task data (localized in task tag)
 * which is used to cpu load calculation and standard IO and etc.
 *
 * @param[in ] *func            task code
 * @param[in ] *name            task name
 * @param[in ]  stack_depth     stack deep
 * @param[in ] *argv            argument pointer
 *
 * @return task object pointer or NULL if error
 */
//==============================================================================
task_t *_task_new(void (*func)(void*), const char *name, const uint stack_depth, void *argv)
{
        struct _task_data *data = sysm_kcalloc(1, sizeof(struct _task_data));
        if (!data) {
                return NULL;
        }

        data->f_parent_task  = xTaskGetCurrentTaskHandle();
        int scheduler_status = xTaskGetSchedulerState();
        uint child_priority  = PRIORITY(0);

        if (scheduler_status != taskSCHEDULER_NOT_STARTED) {
                child_priority = uxTaskPriorityGet(THIS_TASK);
        }

        taskENTER_CRITICAL();
        task_t *task = NULL;
        if (xTaskCreate(func, (signed char *)name, stack_depth, argv, child_priority, &task)) {

                if (scheduler_status != taskSCHEDULER_NOT_STARTED) {
                        vTaskSuspend(task);
                }

                taskEXIT_CRITICAL();
                vTaskSetApplicationTaskTag(task, (void *)data);

                if (sysm_start_task_monitoring(task) == STD_RET_OK) {
                        vTaskResume(task);
                } else {
                        vTaskDelete(task);
                        sysm_kfree(data);
                        task = NULL;
                }
        } else {
                taskEXIT_CRITICAL();
                sysm_kfree(data);
        }

        return task;
}

//==============================================================================
/**
 * @brief Function delete task
 * Function remove task from monitoring list, and next delete the task from OS
 * list. Function resume the parent task before delete.
 *
 * @param *taskHdl       task handle
 */
//==============================================================================
void _task_delete(task_t *taskHdl)
{
        if (sysm_is_task_exist(taskHdl)) {
                (void)sysm_stop_task_monitoring(taskHdl);

                taskENTER_CRITICAL();
                struct _task_data *data;
                if ((data = (void *)xTaskGetApplicationTaskTag(taskHdl))) {

                        vTaskSetApplicationTaskTag(taskHdl, NULL);

                        taskYIELD();
                        if (data->f_parent_task) {
                                vTaskResume(data->f_parent_task);
                        }

                        sysm_kfree(data);
                }

                vTaskDelete(taskHdl);
                taskEXIT_CRITICAL();
        }
}

//==============================================================================
/**
 * @brief Function wait for task exit
 */
//==============================================================================
void _task_exit(void)
{
        do {
                /* request to delete task */
                _task_delete(_task_get_handle());

                /* wait for exit */
                for (;;) {}
        } while(0);
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
int _task_resume_from_ISR(task_t *taskhdl)
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
                return (char *)pcTaskGetTaskName(taskhdl);
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
 * @param cnt_max       max count value (1 for binary)
 * @param cnt_init      initial value (0 or 1 for binary)
 *
 * @return binary semaphore object
 */
//==============================================================================
sem_t *_semaphore_new(const uint cnt_max, const uint cnt_init)
{
        if (cnt_max == 0)
                return NULL;

        sem_t *sem = NULL;

        if (cnt_max == 1) {
                vSemaphoreCreateBinary(sem);
                if (sem && cnt_init == 1) {
                        xSemaphoreTake(sem, 0);
                }
        } else {
                sem = xSemaphoreCreateCounting(cnt_max, cnt_init);
        }

        return sem;
}

//==============================================================================
/**
 * @brief Function delete semaphore
 *
 * @param[in] *sem      semaphore object
 */
//==============================================================================
void _semaphore_delete(sem_t *sem)
{
        if (sem) {
                vSemaphoreDelete(sem);
        }
}

//==============================================================================
/**
 * @brief Function take semaphore
 *
 * @param[in] *sem              semaphore object
 * @param[in]  blocktime_ms     semaphore polling time
 *
 * @retval true         semaphore taken
 * @retval false        semaphore not taken
 */
//==============================================================================
bool _semaphore_take(sem_t *sem, const uint blocktime_ms)
{
        if (sem) {
                return xSemaphoreTake(sem, MS2TICK((portTickType)blocktime_ms));
        } else {
                return false;
        }
}

//==============================================================================
/**
 * @brief Function give semaphore
 *
 * @param[in] *sem      semaphore object
 *
 * @retval true         semaphore given
 * @retval false        semaphore not given
 */
//==============================================================================
bool _semaphore_give(sem_t *sem)
{
        if (sem) {
                return xSemaphoreGive(sem);
        } else {
                return false;
        }
}

//==============================================================================
/**
 * @brief Function take semaphore from ISR
 *
 * @param[in]  *sem              semaphore object
 * @param[out] *task_woken       true if higher priority task woken, otherwise false
 *
 * @retval true         semaphore taken
 * @retval false        semaphore not taken
 */
//==============================================================================
bool _semaphore_take_from_ISR(sem_t *sem, bool *task_woken)
{
        if (sem && task_woken) {
                signed portBASE_TYPE woken = 0;
                int ret = xSemaphoreTakeFromISR(sem, &woken);
                *task_woken = (bool)woken;
                return ret;
        } else {
                return false;
        }
}

//==============================================================================
/**
 * @brief Function give semaphore from ISR
 *
 * @param[in]  *sem              semaphore object
 * @param[out] *task_woken       true if higher priority task woken, otherwise false
 *
 * @retval true         semaphore taken
 * @retval false        semaphore not taken
 */
//==============================================================================
bool _semaphore_give_from_ISR(sem_t *sem, bool *task_woken)
{
        if (sem && task_woken) {
                signed portBASE_TYPE woken = 0;
                int ret = xSemaphoreGiveFromISR(sem, &woken);
                *task_woken = (bool)woken;
                return ret;
        } else {
                return false;
        }
}

//==============================================================================
/**
 * @brief Function create new mutex
 *
 * @param type          mutex type
 *
 * @return pointer to mutex object, otherwise NULL if error
 */
//==============================================================================
mutex_t *_mutex_new(enum mutex_type type)
{
        mutex_t *mtx = sysm_kmalloc(sizeof(mutex_t));
        if (mtx) {
                if (type == MUTEX_RECURSIVE) {
                        mtx->mutex     = xSemaphoreCreateRecursiveMutex();
                        mtx->recursive = true;
                } else {
                        mtx->mutex     = xSemaphoreCreateMutex();
                        mtx->recursive = false;
                }
        }

        return mtx;
}

//==============================================================================
/**
 * @brief Function delete mutex
 *
 * @param[in] *mutex    mutex object
 */
//==============================================================================
void _mutex_delete(mutex_t *mutex)
{
        if (mutex) {
                return vSemaphoreDelete(mutex);
        }
}

//==============================================================================
/**
 * @brief Function lock mutex
 *
 * @param[in] mutex             mutex object
 * @param[in] blocktime_ms      polling time
 *
 * @retval true                 mutex locked
 * @retval false                mutex not locked
 */
//==============================================================================
bool _mutex_lock(mutex_t *mutex, const uint blocktime_ms)
{
        if (mutex) {
                if (mutex->recursive) {
                        return xSemaphoreTakeRecursive(mutex->mutex, MS2TICK((portTickType)blocktime_ms));
                } else {
                        return xSemaphoreTake(mutex->mutex, MS2TICK((portTickType)blocktime_ms));
                }
        } else {
                return false;
        }
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
bool _mutex_unlock(mutex_t *mutex)
{
        if (mutex) {
                if (mutex->recursive) {
                        return xSemaphoreGiveRecursive(mutex->mutex);
                } else {
                        return xSemaphoreGive(mutex->mutex);
                }
        } else {
                return false;
        }
}

//==============================================================================
/**
 * @brief Function delete queue
 *
 * @param[in] *queue            queue object
 */
//==============================================================================
void _queue_delete(queue_t *queue)
{
        if (queue) {
                vQueueDelete(queue);
        }
}

//==============================================================================
/**
 * @brief Function reset queue
 *
 * @param[in] *queue            queue object
 */
//==============================================================================
void _queue_reset(queue_t *queue)
{
        if (queue) {
                xQueueReset(queue);
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
 * @retval true         item posted
 * @retval false        item not posted
 */
//==============================================================================
bool _queue_send(queue_t *queue, const void *item, const uint waittime_ms)
{
        if (queue && item) {
                return xQueueSend(queue, item, MS2TICK((portTickType)waittime_ms));
        } else {
                return false;
        }
}

//==============================================================================
/**
 * @brief Function send queue
 *
 * @param[in]  *queue            queue object
 * @param[in]  *item             item
 * @param[out] *task_woken       1 if higher priority task woken, otherwise 0
 *
 * @retval true         item posted
 * @retval false        item not posted
 */
//==============================================================================
bool _queue_send_from_ISR(queue_t *queue, const void *item, bool *task_woken)
{
        if (queue && item && task_woken) {
                signed portBASE_TYPE woken = 0;
                int ret = xQueueSendFromISR(queue, item, &woken);
                *task_woken = (bool)woken;
                return ret;
        } else {
                return false;
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
 * @retval true         item received
 * @retval false        item not received
 */
//==============================================================================
bool _queue_receive(queue_t *queue, void *item, const uint waittime_ms)
{
        if (queue && item) {
                return xQueueReceive(queue, item, MS2TICK((portTickType)waittime_ms));
        } else {
                return false;
        }
}

//==============================================================================
/**
 * @brief Function receive queue from ISR
 *
 * @param[in]  queue            queue object
 * @param[out] item             item
 * @param[out] task_woken       true if higher priority task woke, otherwise false
 *
 * @retval true                 item received
 * @retval false                item not received
 */
//==============================================================================
bool _queue_receive_from_ISR(queue_t *queue, void *item, bool *task_woken)
{
        if (queue && item && task_woken) {
                signed portBASE_TYPE woken = 0;
                int ret = xQueueReceiveFromISR(queue, item, &woken);
                *task_woken = (bool)woken;
                return ret;
        } else {
                return false;
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
 * @retval true         item received
 * @retval false        item not received
 */
//==============================================================================
bool _queue_receive_peek(queue_t *queue, void *item, const uint waittime_ms)
{
        if (queue && item) {
                return xQueuePeek(queue, item, MS2TICK((portTickType)waittime_ms));
        } else {
                return false;
        }
}

//==============================================================================
/**
 * @brief Function gets number of items in queue
 *
 * @param[in] *queue            queue object
 *
 * @return a number of items in queue, -1 if error
 */
//==============================================================================
int _queue_get_number_of_items(queue_t *queue)
{
        if (queue) {
                return uxQueueMessagesWaiting(queue);
        } else {
                return -1;
        }
}

//==============================================================================
/**
 * @brief Function gets number of items in queue from ISR
 *
 * @param[in] *queue            queue object
 *
 * @return a number of items in queue, -1 if error
 */
//==============================================================================
int _queue_get_number_of_items_from_ISR(queue_t *queue)
{
        if (queue) {
                return uxQueueMessagesWaitingFromISR(queue);
        } else {
                return -1;
        }
}

//==============================================================================
/**
 * @brief Function return data of this task
 *
 * @return this task data
 */
//==============================================================================
_task_data_t *_task_get_data(void)
{
        return (struct _task_data*)_task_get_tag(THIS_TASK);
}

#ifdef __cplusplus
}
#endif

/*==============================================================================
  End of file
==============================================================================*/
