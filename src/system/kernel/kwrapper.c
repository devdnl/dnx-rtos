/*=========================================================================*//**
@file    kwrapper.c

@author  Daniel Zorychta

@brief   Kernel wrapper

@note    Copyright (C) 2012, 2013 Daniel Zorychta <daniel.zorychta@gmail.com>

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

/*==============================================================================
  Include files
==============================================================================*/
#include "kernel/kwrapper.h"
#include "kernel/ktypes.h"
#include "kernel/errno.h"
#include "kernel/sysfunc.h"
#include "lib/cast.h"
#include "event_groups.h"

/*==============================================================================
  Local symbolic constants/macros
==============================================================================*/
/** CALCULATIONS */
#define _CEILING(x,y)   (((x) + (y) - 1) / (y))
#define MS2TICK(ms)     ((ms <= (1000/(configTICK_RATE_HZ)) ? 1 : _CEILING(ms,(1000/(configTICK_RATE_HZ)))) + 1)

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
 * @brief Function check that semaphore is a valid object
 * @param sem           semaphore object to examine
 * @return If object is valid then true is returned, false otherwise.
 */
//==============================================================================
static bool is_semaphore_valid(sem_t *sem)
{
        return _mm_is_object_in_heap(sem) && sem->header.type == RES_TYPE_SEMAPHORE && sem->object;
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
        return _mm_is_object_in_heap(mtx) && mtx->header.type == RES_TYPE_MUTEX && mtx->object;
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
        return _mm_is_object_in_heap(queue) && queue->header.type == RES_TYPE_QUEUE && queue->object;
}

//==============================================================================
/**
 * @brief Function check that flag is a valid object
 * @param flag         flag object to examine
 * @return If object is valid then true is returned, false otherwise.
 */
//==============================================================================
static bool is_flag_valid(flag_t *flag)
{
        return _mm_is_object_in_heap(flag) && flag->header.type == RES_TYPE_FLAG && flag->object;
}

//==============================================================================
/**
 * @brief Function start kernel scheduler
 */
//==============================================================================
void _kernel_start(void)
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
u32_t _kernel_get_time_ms(void)
{
        return (xTaskGetTickCount() * (1000/(configTICK_RATE_HZ)));
}

//==============================================================================
/**
 * @brief Function return tick counter
 *
 * @return a tick counter value
 */
//==============================================================================
u32_t _kernel_get_tick_counter(void)
{
        return (u32_t)xTaskGetTickCount();
}

//==============================================================================
/**
 * @brief Function return a number of task
 *
 * @return a number of tasks
 */
//==============================================================================
int _kernel_get_number_of_tasks(void)
{
        return uxTaskGetNumberOfTasks();
}

//==============================================================================
/**
 * @brief  Function lock context switch (caller task is running only).
 *         All IRQs works normally.
 */
//==============================================================================
void _kernel_scheduler_lock(void)
{
        vTaskSuspendAll();
}

//==============================================================================
/**
 * @brief  Function unlock context switch
 */
//==============================================================================
void _kernel_scheduler_unlock(void)
{
        if (xTaskResumeAll() == pdTRUE) {
                taskYIELD();
        }
}

//==============================================================================
/**
 * @brief  Function release kernel resources: finished tasks and not used memory.
 */
//==============================================================================
void _kernel_release_resources(void)
{
        /*
         * Force context switch to Idle task to delete finished tasks. Task
         * priority is restored in the vApplicationIdleHook().
         */
        vTaskPrioritySet(xTaskGetIdleTaskHandle(), configMAX_PRIORITIES - 1);
        taskYIELD();
}

//==============================================================================
/**
 * @brief  Function return handle of idle task.
 *
 * @return Idle task handle.
 */
//==============================================================================
task_t *_kernel_get_idle_task_handle(void)
{
        return xTaskGetIdleTaskHandle();
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
int _task_create(task_func_t func, const char *name, const size_t stack_depth, void *argv, void *tag, task_t **task)
{
        int err = EINVAL;

        if (func && name && stack_depth) {
                int         scheduler_status = xTaskGetSchedulerState();
                UBaseType_t parent_priority  = PRIORITY(0);

                if (scheduler_status != taskSCHEDULER_NOT_STARTED) {
                        parent_priority = uxTaskPriorityGet(_THIS_TASK);

                        // set this task priority to highest possible to prevent
                        // against context switch to new task
                        vTaskPrioritySet(_THIS_TASK, configMAX_PRIORITIES - 1);
                }

                UBaseType_t child_priority = parent_priority == (configMAX_PRIORITIES - 1) ?
                                             parent_priority - 1 : parent_priority;

                task_t *tsk = NULL;
                if (xTaskCreate(func, name, stack_depth, argv,
                                child_priority, &tsk) == pdPASS) {

                        vTaskSetApplicationTaskTag(tsk, (void *)tag);

                        if (task) *task = tsk;

                        err = ESUCC;
                } else {
                        err = ENOMEM;
                }

                if (scheduler_status != taskSCHEDULER_NOT_STARTED) {
                        // restore this task priority
                        vTaskPrioritySet(_THIS_TASK, parent_priority);
                }

                if (!err && child_priority != parent_priority) {
                        vTaskPrioritySet(tsk, parent_priority);
                }
        }

        return err;
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
void _task_destroy(task_t *taskHdl)
{
        vTaskDelete(taskHdl);
}

//==============================================================================
/**
 * @brief Function wait for task exit
 */
//==============================================================================
void _task_exit(void)
{
        /* request to delete task */
        _task_set_priority(_THIS_TASK, PRIORITY_NORMAL);
        _task_destroy(_THIS_TASK);

        /* wait for exit */
        for (;;) {}
}

//==============================================================================
/**
 * @brief Function suspend selected task
 *
 * @param[in] *taskhdl          task handle (NULL for current task)
 *
 * @return None
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
 * @param[in] *taskhdl          task handle (NULL for current task)
 *
 * @return name of selected task or NULL if error
 */
//==============================================================================
char *_task_get_name(task_t *taskhdl)
{
        return pcTaskGetName(taskhdl);
}

//==============================================================================
/**
 * @brief Function return task priority
 *
 * @param[in] *taskhdl          task handle (NULL for current task)
 *
 * @return priority of selected task
 */
//==============================================================================
int _task_get_priority(task_t *taskhdl)
{
        return (int)(uxTaskPriorityGet(taskhdl) - (__OS_TASK_MAX_PRIORITIES__ / 2));
}

//==============================================================================
/**
 * @brief Function set selected task priority
 *
 * @param[in] *taskhdl          task handle (NULL for current task)
 * @param[in]  priority         priority
 *
 * @return None
 */
//==============================================================================
void _task_set_priority(task_t *taskhdl, const int priority)
{
        vTaskPrioritySet(taskhdl, PRIORITY(priority));
}

//==============================================================================
/**
 * @brief Function return a free stack level of selected task
 *
 * @param[in] *taskhdl          task handle (NULL for current task)
 *
 * @return free stack level or -1 if error
 */
//==============================================================================
int _task_get_free_stack(task_t *taskhdl)
{
        return uxTaskGetStackHighWaterMark(taskhdl);
}

//==============================================================================
/**
 * @brief Function yield task
 *
 * @param None
 *
 * @return None
 */
//==============================================================================
void _task_yield(void)
{
        taskYIELD();
}

//==============================================================================
/**
 * @brief Function yield task from ISR
 *
 * @param None
 *
 * @return None
 */
//==============================================================================
void _task_yield_from_ISR(bool yield)
{
        portYIELD_FROM_ISR(yield);
}

//==============================================================================
/**
 * @brief Function return current task handle object address
 *
 * @param None
 *
 * @return current task handle
 */
//==============================================================================
task_t *_task_get_handle(void)
{
        return xTaskGetCurrentTaskHandle();
}

//==============================================================================
/**
 * @brief Function set task tag
 *
 * @param[in] taskhdl           task handle (NULL for current task)
 * @param[in] tag               task tag
 *
 * @return None
 */
//==============================================================================
void _task_set_tag(task_t *taskhdl, void *tag)
{
        vTaskSetApplicationTaskTag(taskhdl, (TaskHookFunction_t)tag);
}

//==============================================================================
/**
 * @brief Function return task tag
 *
 * @param[in] *taskhdl          task handle (NULL for current task)
 *
 * @return task tag
 */
//==============================================================================
void *_task_get_tag(task_t *taskhdl)
{
        return (void*)xTaskGetApplicationTaskTag(taskhdl);
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
int _semaphore_create(size_t cnt_max, size_t cnt_init, sem_t **sem)
{
        int err = EINVAL;

        if (cnt_max > 0 && sem) {
                err = _kzalloc(_MM_KRN, sizeof(sem_t), cast(void**, sem));
                if (err == ESUCC) {

                        if (cnt_max == 1) {

                                (*sem)->object = xSemaphoreCreateBinaryStatic(&(*sem)->buffer);
                                if ((*sem)->object) {
                                        if (cnt_init) {
                                                xSemaphoreGive((*sem)->object);
                                        } else {
                                                xSemaphoreTake((*sem)->object, 0);
                                        }
                                }
                        } else {
                                (*sem)->object = xSemaphoreCreateCountingStatic(
                                                        cnt_max,
                                                        cnt_init,
                                                        &(*sem)->buffer);
                        }

                        if ((*sem)->object) {
                                (*sem)->header.type = RES_TYPE_SEMAPHORE;
                        } else {
                                _kfree(_MM_KRN, cast(void**, sem));
                                err = ENOMEM;
                        }
                }
        }

        return err;
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
                sem->header.type = RES_TYPE_UNKNOWN;
                vSemaphoreDelete(sem->object);
                sem->object = NULL;
                return _kfree(_MM_KRN, cast(void**, &sem));
        } else {
                printk("Invalid semaphore object @ %p", sem);
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
int _semaphore_wait(sem_t *sem, const u32_t blocktime_ms)
{
        if (is_semaphore_valid(sem)) {
                bool r = xSemaphoreTake(sem->object, MS2TICK((TickType_t)blocktime_ms));
                return r ? ESUCC : ETIME;
        } else {
                printk("Invalid semaphore object @ %p", sem);
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
                printk("Invalid semaphore object @ %p", sem);
                return EINVAL;
        }
}

//==============================================================================
/**
 * @brief Function get semaphore count
 *
 * @param[in]  *sem     semaphore object
 * @param[out] *value   semaphore value (counter)
 *
 * @return One of errno values.
 */
//==============================================================================
int _semaphore_get_value(sem_t *sem, size_t *value)
{
        if (is_semaphore_valid(sem) && value) {
                *value = (size_t)uxSemaphoreGetCount(sem->object);
                return ESUCC;
        } else {
                printk("Invalid semaphore object @ %p", sem);
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
        int err = EINVAL;

        if (type <= MUTEX_TYPE_NORMAL && mtx) {
                err = _kzalloc(_MM_KRN, sizeof(mutex_t), cast(void**, mtx));
                if (err == ESUCC) {
                        if (type == MUTEX_TYPE_RECURSIVE) {
                                (*mtx)->object    = xSemaphoreCreateRecursiveMutexStatic(&(*mtx)->buffer);
                                (*mtx)->recursive = true;
                        } else {
                                (*mtx)->object    = xSemaphoreCreateMutexStatic(&(*mtx)->buffer);
                                (*mtx)->recursive = false;
                        }

                        if ((*mtx)->object) {
                                (*mtx)->header.type = RES_TYPE_MUTEX;
                        } else {
                                _kfree(_MM_KRN, cast(void**, mtx));
                                err = ENOMEM;
                        }
                }
        }

        return err;
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
                mutex->header.type = RES_TYPE_UNKNOWN;
                vSemaphoreDelete(mutex->object);
                mutex->object = NULL;
                return _kfree(_MM_KRN, cast(void**, &mutex));
        } else {
                printk("Invalid mutex object @ %p", mutex);
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
int _mutex_lock(mutex_t *mutex, const u32_t blocktime_ms)
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
                printk("Invalid mutex object @ %p", mutex);
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
                printk("Invalid mutex object @ %p", mutex);
                return EINVAL;
        }
}


//==============================================================================
/**
 * @brief Function create new flag (event).
 *
 * @param flag          created flag handle
 *
 * @return One of errno values.
 */
//==============================================================================
int _flag_create(flag_t **flag)
{
        int err = EINVAL;

        if (flag) {
                err = _kzalloc(_MM_KRN, sizeof(flag_t), cast(void**, flag));
                if (err == ESUCC) {
                        (*flag)->object = xEventGroupCreateStatic(&(*flag)->buffer);

                        if ((*flag)->object) {
                                (*flag)->header.type = RES_TYPE_FLAG;
                        } else {
                                _kfree(_MM_KRN, cast(void**, flag));
                                err = ENOMEM;
                        }
                }
        }

        return err;
}

//==============================================================================
/**
 * @brief Function destroy flag.
 *
 * @param flag          flag object
 *
 * @return One of errno values.
 */
//==============================================================================
int _flag_destroy(flag_t *flag)
{
        if (is_flag_valid(flag)) {
                flag->header.type = RES_TYPE_UNKNOWN;
                vEventGroupDelete(flag->object);
                flag->object = NULL;
                return _kfree(_MM_KRN, cast(void**, &flag));
        } else {
                printk("Invalid flag object @ %p", flag);
                return EINVAL;
        }
}

//==============================================================================
/**
 * @brief Function wait for selected flags.
 *
 * @param flag          flag object
 * @param bits          bits to wait for (logical AND)
 * @param blocktime_ms  timeout value
 *
 * @return One of errno values.
 */
//==============================================================================
int _flag_wait(flag_t *flag, u32_t bits, const u32_t blocktime_ms)
{
        if (is_flag_valid(flag)) {
                if (xEventGroupWaitBits(flag->object, bits, true, true, blocktime_ms)) {
                        return ESUCC;
                } else {
                        return ETIME;
                }

        } else {
                printk("Invalid flag object @ %p", flag);
                return EINVAL;
        }
}

//==============================================================================
/**
 * @brief Function set selected bits.
 *
 * @param flag          flag object
 * @param bits          bits to set (OR)
 *
 * @return One of errno values.
 */
//==============================================================================
int _flag_set(flag_t *flag, u32_t bits)
{
        if (is_flag_valid(flag)) {
                xEventGroupSetBits(flag->object, bits);
                return ESUCC;
        } else {
                printk("Invalid flag object @ %p", flag);
                return EINVAL;
        }
}

//==============================================================================
/**
 * @brief Function clear selected bits.
 *
 * @param flag          flag object
 * @param bits          bits to clear (OR)
 *
 * @return One of errno values.
 */
//==============================================================================
int _flag_clear(flag_t *flag, u32_t bits)
{
        if (is_flag_valid(flag)) {
                xEventGroupClearBits(flag->object, bits);
                return ESUCC;
        } else {
                printk("Invalid flag object @ %p", flag);
                return EINVAL;
        }
}

//==============================================================================
/**
 * @brief Function get bits.
 *
 * @param flag          flag object
 *
 * @return Flags.
 */
//==============================================================================
u32_t _flag_get(flag_t *flag)
{
        if (is_flag_valid(flag)) {
                return (u32_t)xEventGroupGetBits(flag->object);
        } else {
                printk("Invalid flag object @ %p", flag);
                return 0;
        }
}

//==============================================================================
/**
 * @brief Function get bits from interrupt.
 *
 * @param flag          flag object
 *
 * @return Flags.
 */
//==============================================================================
u32_t _flag_get_from_ISR(flag_t *flag)
{
        if (is_flag_valid(flag)) {
                return (u32_t)xEventGroupGetBitsFromISR(flag->object);
        } else {
                return 0;
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
int _queue_create(size_t length, size_t item_size, queue_t **queue)
{
        int err = EINVAL;

        if (length && item_size && queue) {
                err = _kzalloc(_MM_KRN, sizeof(queue_t) + (length * item_size), cast(void**, queue));
                if (err == ESUCC) {
                        (*queue)->object = xQueueCreateStatic(length,
                                                              item_size,
                                                              cast(uint8_t*, &(*queue)[1]),
                                                              &(*queue)->buffer);
                        if ((*queue)->object) {
                                (*queue)->header.type = RES_TYPE_QUEUE;
                        } else {
                                _kfree(_MM_KRN, cast(void**, &queue));
                                err = ENOMEM;
                        }
                }
        }

        return err;
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
                queue->header.type = RES_TYPE_UNKNOWN;
                vQueueDelete(queue->object);
                queue->object = NULL;
                _kfree(_MM_KRN, cast(void**, &queue));
                return ESUCC;
        } else {
                printk("Invalid queue object @ %p", queue);
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
                printk("Invalid queue object @ %p", queue);
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
int _queue_send(queue_t *queue, const void *item, const u32_t waittime_ms)
{
        if (is_queue_valid(queue) && item) {
                BaseType_t r = xQueueSend(queue->object, item, MS2TICK((TickType_t)waittime_ms));
                return r == pdTRUE ? ESUCC : ENOSPC;
        } else {
                printk("Invalid queue object @ %p", queue);
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
int _queue_receive(queue_t *queue, void *item, const u32_t waittime_ms)
{
        if (is_queue_valid(queue) && item) {
                BaseType_t r = xQueueReceive(queue->object, item, MS2TICK((TickType_t)waittime_ms));
                return r == pdTRUE ? ESUCC : ETIME;
        } else {
                printk("Invalid queue object @ %p", queue);
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
int _queue_receive_peek(queue_t *queue, void *item, const u32_t waittime_ms)
{
        if (is_queue_valid(queue) && item) {
                BaseType_t r = xQueuePeek(queue->object, item, MS2TICK((TickType_t)waittime_ms));
                return r == pdTRUE ? ESUCC : EAGAIN;
        } else {
                printk("Invalid queue object @ %p", queue);
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
                printk("Invalid queue object @ %p", queue);
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
                UBaseType_t nomsg = uxQueueSpacesAvailable(queue->object);
                *items = nomsg;
                return ESUCC;
        } else {
                printk("Invalid queue object @ %p", queue);
                return EINVAL;
        }
}

//==============================================================================
/**
 * @brief Function enter to critical section
 *
 * @param None
 *
 * @return None
 */
//==============================================================================
void _critical_section_begin(void)
{
        taskENTER_CRITICAL();
}

//==============================================================================
/**
 * @brief Function exit from critical section
 *
 * @param None
 *
 * @return None
 */
//==============================================================================
void _critical_section_end(void)
{
        taskEXIT_CRITICAL();
}

//==============================================================================
/**
 * @brief Function disable interrupts
 *
 * @param None
 *
 * @return None
 */
//==============================================================================
void _ISR_disable(void)
{
        taskDISABLE_INTERRUPTS();
}

//==============================================================================
/**
 * @brief Function enable interrupts
 *
 * @param None
 *
 * @return None
 */
//==============================================================================
void _ISR_enable(void)
{
        taskENABLE_INTERRUPTS();
}

//==============================================================================
/**
 * @brief Function put to sleep task in milliseconds
 *
 * @param[in] milliseconds
 *
 * @return None
 */
//==============================================================================
void _sleep_ms(const u32_t milliseconds)
{
        vTaskDelay(MS2TICK(milliseconds));
}

//==============================================================================
/**
 * @brief Function put to sleep task in seconds
 *
 * @param[in] seconds
 *
 * @return None
 */
//==============================================================================
void _sleep(const u32_t seconds)
{
        vTaskDelay(MS2TICK(seconds * 1000UL));
}

//==============================================================================
/**
 * @brief Function sleep task in regular periods (reference argument)
 *
 * @param milliseconds          milliseconds
 * @param ref_time_ticks        reference time in OS ticks
 *
 * @return None
 */
//==============================================================================
void _sleep_until_ms(const u32_t milliseconds, u32_t *ref_time_ticks)
{
        vTaskDelayUntil((TickType_t *)ref_time_ticks, MS2TICK(milliseconds));
}

//==============================================================================
/**
 * @brief Function sleep task in regular periods (reference argument)
 *
 * @param seconds       seconds
 * @param ref_time_ticks        reference time in OS ticks
 *
 * @return None
 */
//==============================================================================
void _sleep_until(const u32_t seconds, u32_t *ref_time_ticks)
{
        vTaskDelayUntil((TickType_t *)ref_time_ticks, MS2TICK(seconds * 1000UL));
}

/*==============================================================================
  End of file
==============================================================================*/
