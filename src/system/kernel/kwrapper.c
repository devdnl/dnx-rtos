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
task_t *new_task(void (*func)(void*), const char *name, uint stack_depth, void *argv)
{
        struct task_data *data = sysm_kcalloc(1, sizeof(struct task_data));
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
void delete_task(task_t *taskHdl)
{
        if (sysm_is_task_exist(taskHdl)) {
                (void)sysm_stop_task_monitoring(taskHdl);

                taskENTER_CRITICAL();
                struct task_data *data;
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
void task_exit(void)
{
        do {
                /* request to delete task */
                delete_task(get_task_handle());

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
void suspend_task(task_t *taskhdl)
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
void resume_task(task_t *taskhdl)
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
int resume_task_from_ISR(task_t *taskhdl)
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
char *get_task_name(task_t *taskhdl)
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
int get_task_priority(task_t *taskhdl)
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
void set_task_priority(task_t *taskhdl, const int priority)
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
int get_task_free_stack(task_t *taskhdl)
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
 * @return binary semaphore object
 */
//==============================================================================
sem_t *new_semaphore(void)
{
        sem_t *sem = NULL;
        vSemaphoreCreateBinary(sem);
        return sem;
}

//==============================================================================
/**
 * @brief Function delete semaphore
 *
 * @param[in] *sem      semaphore object
 */
//==============================================================================
void delete_semaphore(sem_t *sem)
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
 * @retval true         semaphore taken (SEMAPHORE_TAKEN)
 * @retval false        semaphore not taken (SEMAPHORE_NOT_TAKEN)
 */
//==============================================================================
bool take_semaphore(sem_t *sem, const uint blocktime_ms)
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
 * @retval true         semaphore taken (SEMAPHORE_GIVEN)
 * @retval false        semaphore not taken (SEMAPHORE_NOT_GIVEN)
 */
//==============================================================================
bool give_semaphore(sem_t *sem)
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
 * @retval true         semaphore taken (SEMAPHORE_TAKEN)
 * @retval false        semaphore not taken (SEMAPHORE_NOT_TAKEN)
 */
//==============================================================================
bool take_semaphore_from_ISR(sem_t *sem, bool *task_woken)
{
        if (sem) {
                return xSemaphoreTakeFromISR(sem, (signed portBASE_TYPE *)task_woken);
        } else {
                return false;
        }
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
bool give_semaphore_from_ISR(sem_t *sem, int *task_woken)
{
        if (sem) {
                return xSemaphoreGiveFromISR(sem, (signed portBASE_TYPE *)task_woken);
        } else {
                return false;
        }
}

//==============================================================================
/**
 * @brief Function delete semaphore
 *
 * @param[in] *sem      semaphore object
 */
//==============================================================================
void delete_counting_semaphore(sem_t *sem)
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
 * @retval true         semaphore taken (SEMAPHORE_TAKEN)
 * @retval false        semaphore not taken (SEMAPHORE_NOT_TAKEN)
 */
//==============================================================================
bool take_counting_semaphore(sem_t *sem, const uint blocktime_ms)
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
 * @retval true         semaphore given (SEMAPHORE_GIVEN)
 * @retval false        semaphore not given (SEMAPHORE_NOT_GIVEN)
 */
//==============================================================================
bool give_counting_semaphore(sem_t *sem)
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
 * @retval true         semaphore taken (SEMAPHORE_TAKEN)
 * @retval false        semaphore not taken (SEMAPHORE_NOT_TAKEN)
 */
//==============================================================================
bool take_counting_semaphore_from_ISR(sem_t *sem, int *task_woken)
{
        if (sem) {
                return xSemaphoreTakeFromISR(sem, (signed portBASE_TYPE *)task_woken);
        } else {
                return false;
        }
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
bool give_counting_semaphore_from_ISR(sem_t *sem, int *task_woken)
{
        if (sem) {
                return xSemaphoreGiveFromISR(sem, (signed portBASE_TYPE *)task_woken);
        } else {
                return false;
        }
}

//==============================================================================
/**
 * @brief Function delete mutex
 *
 * @param[in] *mutex    mutex object
 */
//==============================================================================
void delete_mutex(mutex_t *mutex)
{
        if (mutex) {
                return vSemaphoreDelete(mutex);
        }
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
bool lock_mutex(mutex_t *mutex, const uint blocktime_ms)
{
        if (mutex) {
                return xSemaphoreTake(mutex, MS2TICK((portTickType)blocktime_ms));
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
bool unlock_mutex(mutex_t *mutex)
{
        if (mutex) {
                return xSemaphoreGive(mutex);
        } else {
                return false;
        }
}

//==============================================================================
/**
 * @brief Function delete mutex
 *
 * @param[in] *mutex    mutex object
 */
//==============================================================================
void delete_recursive_mutex(mutex_t *mutex)
{
        if (mutex) {
                return vSemaphoreDelete(mutex);
        }
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
bool lock_recursive_mutex(mutex_t *mutex, const uint blocktime_ms)
{
        if (mutex) {
                return xSemaphoreTakeRecursive(mutex, MS2TICK((portTickType)blocktime_ms));
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
bool unlock_recursive_mutex(mutex_t *mutex)
{
        if (mutex) {
                return xSemaphoreGiveRecursive(mutex);
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
void delete_queue(queue_t *queue)
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
void reset_queue(queue_t *queue)
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
 * @retval true         item posted (QUEUE_ITEM_POSTED)
 * @retval false        item not posted (QUEUE_ITEM_NOT_POSTED, QUEUE_FULL)
 */
//==============================================================================
bool send_queue(queue_t *queue, const void *item, const uint waittime_ms)
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
 * @retval true         item posted (QUEUE_ITEM_POSTED)
 * @retval false        item not posted (QUEUE_ITEM_NOT_POSTED, QUEUE_FULL)
 */
//==============================================================================
bool send_queue_from_ISR(queue_t *queue, const void *item, int *task_woken)
{
        if (queue && item) {
                return xQueueSendFromISR(queue, item, (signed portBASE_TYPE *)task_woken);
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
 * @retval true         item received (QUEUE_ITEM_RECEIVED)
 * @retval false        item not recieved (QUEUE_ITEM_NOT_RECEIVED)
 */
//==============================================================================
bool receive_queue(queue_t *queue, void *item, const uint waittime_ms)
{
        if (queue && item) {
                return xQueueReceive(queue, item, MS2TICK((portTickType)waittime_ms));
        } else {
                return false;
        }
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
bool receive_queue_from_ISR(queue_t *queue, void *item, int *task_woken)
{
        if (queue && item) {
                return xQueueReceiveFromISR(queue, item, (signed portBASE_TYPE *)task_woken);
        } else {
                return false;
        }
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
bool receive_peek_queue(queue_t *queue, void *item, const uint waittime_ms)
{
        if (queue && item) {
                return xQueuePeek(queue, item, MS2TICK((portTickType)waittime_ms));
        } else {
                return false;
        }
}

//==============================================================================
/**
 * @brief Function x queue
 *
 * @param[in] *queue            queue object
 *
 * @return a number of items in queue, -1 if error
 */
//==============================================================================
int get_number_of_items_in_queue(queue_t *queue)
{
        if (queue) {
                return uxQueueMessagesWaiting(queue);
        } else {
                return -1;
        }
}

//==============================================================================
/**
 * @brief Function x queue
 *
 * @param[in] *queue            queue object
 *
 * @return a number of items in queue, -1 if error
 */
//==============================================================================
int get_number_of_items_in_queue_from_ISR(queue_t *queue)
{
        if (queue) {
                return uxQueueMessagesWaitingFromISR(queue);
        } else {
                return -1;
        }
}

#ifdef __cplusplus
}
#endif

/*==============================================================================
  End of file
==============================================================================*/
