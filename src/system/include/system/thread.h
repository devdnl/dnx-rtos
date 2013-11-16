/*=========================================================================*//**
@file    thread.h

@author  Daniel Zorychta

@brief   Library with functions which operate on threads.

@note    Copyright (C) 2013 Daniel Zorychta <daniel.zorychta@gmail.com>

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

#ifndef _THREAD_H_
#define _THREAD_H_

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Include files
==============================================================================*/
#include "kernel/kwrapper.h"
#include "core/progman.h"

/*==============================================================================
  Exported macros
==============================================================================*/

/*==============================================================================
  Exported object types
==============================================================================*/

/*==============================================================================
  Exported objects
==============================================================================*/

/*==============================================================================
  Exported functions
==============================================================================*/

/*==============================================================================
  Exported inline functions
==============================================================================*/
//==============================================================================
/**
 * @brief Function create new task
 *
 * @param[in ] func            task code
 * @param[in ] name            task name
 * @param[in ] stack_depth     stack deep
 * @param[in ] arg             argument pointer
 *
 * @return task object pointer or NULL if error
 */
//==============================================================================
static inline task_t *task_new(void (*func)(void*), const char *name, const uint stack_depth, void *arg)
{
        return _task_new(func, name, stack_depth, arg);
}

//==============================================================================
/**
 * @brief Function delete task
 *
 * @param taskHdl        task handle
 */
//==============================================================================
static inline void task_delete(task_t *taskhdl)
{
        return _task_delete(taskhdl);
}

//==============================================================================
/**
 * @brief Exit from task code and remote task
 */
//==============================================================================
static inline void task_exit(void)
{
        _task_exit();
}

//==============================================================================
/**
 * @brief Function suspend selected task
 *
 * @param[in]  taskhdl          task handle
 */
//==============================================================================
static inline void task_suspend(task_t *taskhdl)
{
        _task_suspend(taskhdl);
}

//==============================================================================
/**
 * @brief Function suspend current task
 */
//==============================================================================
static inline void task_suspend_now(void)
{
        _task_suspend_now();
}

//==============================================================================
/**
 * @brief Function resume selected task
 *
 * @param[in]  taskhdl          task to resume
 */
//==============================================================================
static inline void task_resume(task_t *taskhdl)
{
        _task_resume(taskhdl);
}

//==============================================================================
/**
 * @brief Function resume selected task from ISR
 *
 * @param[in]  taskhdl          task to resume
 *
 * @retval true                 if yield required
 * @retval false                if yield not required
 */
//==============================================================================
static inline int task_resume_from_ISR(task_t *taskhdl)
{
        return _task_resume_from_ISR(taskhdl);
}

//==============================================================================
/**
 * @brief Function return name of current task
 *
 * @return name of current task
 */
//==============================================================================
static inline char *task_get_name(void)
{
        return _task_get_name();
}

//==============================================================================
/**
 * @brief Function return name of selected task
 *
 * @param[in]  taskhdl          task handle
 *
 * @return name of selected task or NULL if error
 */
//==============================================================================
static inline char *task_get_name_of(task_t *taskhdl)
{
        return _task_get_name_of(taskhdl);
}

//==============================================================================
/**
 * @brief Function return priority of current task
 *
 * @return current task priority
 */
//==============================================================================
static inline int task_get_priority(void)
{
        return _task_get_priority();
}

//==============================================================================
/**
 * @brief Function return task priority
 *
 * @param[in]  taskhdl          task handle
 *
 * @return priority of selected task
 */
//==============================================================================
static inline int task_get_priority_of(task_t *taskhdl)
{
        return _task_get_priority_of(taskhdl);
}

//==============================================================================
/**
 * @brief Function set priority of current task
 *
 * @param[in]  priority         priority
 */
//==============================================================================
static inline void task_set_priority(const int priority)
{
        _task_set_priority(priority);
}

//==============================================================================
/**
 * @brief Function set selected task priority
 *
 * @param[in]  taskhdl          task handle
 * @param[in]  priority         priority
 */
//==============================================================================
static inline void task_set_priority_of(task_t *taskhdl, const int priority)
{
        _task_set_priority_of(taskhdl, priority);
}

//==============================================================================
/**
 * @brief Function return a free stack level of current task
 *
 * @return free stack level
 */
//==============================================================================
static inline int task_get_free_stack(void)
{
        return _task_get_free_stack();
}

//==============================================================================
/**
 * @brief Function return a free stack level of selected task
 *
 * @param[in]  taskhdl          task handle
 *
 * @return free stack level or -1 if error
 */
//==============================================================================
static inline int task_get_free_stack_of(task_t *taskhdl)
{
        return _task_get_free_stack_of(taskhdl);
}

//==============================================================================
/**
 * @brief Function return current task handle object address
 *
 * @return current task handle
 */
//==============================================================================
static inline task_t *task_get_handle(void)
{
        return _task_get_handle();
}

//==============================================================================
/**
 * @brief Function return parent task handle
 *
 * @return parent task handle
 */
//==============================================================================
static inline task_t *task_get_parent_handle(void)
{
        return _task_get_parent_handle();
}

//==============================================================================
/**
 * @brief Function return a number of tasks
 *
 * @return a number of tasks
 */
//==============================================================================
static inline int task_get_number_of_tasks(void)
{
        return _kernel_get_number_of_tasks();
}

/* TODO task_kill() -- general routine to kill task of any kind */

//==============================================================================
/**
 * @brief Function set stdin file
 *
 * @param[in] stream
 */
//==============================================================================
static inline void task_set_stdin(FILE *stream)
{
        _task_set_stdin(stream);
}

//==============================================================================
/**
 * @brief Function set stdout file
 *
 * @param[in] stream
 */
//==============================================================================
static inline void task_set_stdout(FILE *stream)
{
        _task_set_stdout(stream);
}

//==============================================================================
/**
 * @brief Function set stderr file
 *
 * @param[in] stream
 */
//==============================================================================
static inline void task_set_stderr(FILE *stream)
{
        _task_set_stderr(stream);
}

//==============================================================================
/**
 * @brief Function set cwd path
 *
 * @param str           cwd string
 */
//==============================================================================
static inline void task_set_cwd(const char *str)
{
        _task_set_cwd(str);
}

//==============================================================================
/**
 * @brief Function yield task
 */
//==============================================================================
static inline void task_yield(void)
{
        _task_yield();
}

//==============================================================================
/**
 * @brief Create new thread of configured task (program or RAW task)
 *
 * @param func          thread function
 * @param stack_depth   stack depth
 * @param arg           thread argument
 *
 * @return thread object if success, otherwise NULL
 */
//==============================================================================
static inline thread_t *thread_new(void (*func)(void*), const int stack_depth, void *arg)
{
        return _thread_new(func, stack_depth, arg);
}

//==============================================================================
/**
 * @brief Function wait for thread exit
 *
 * @param thread        thread object
 *
 * @return 0 on success, otherwise -EINVAL
 */
//==============================================================================
static inline int thread_join(thread_t *thread)
{
        return _thread_join(thread);
}

//==============================================================================
/**
 * @brief Cancel current working thread
 *
 * @return 0 on success, otherwise other
 */
//==============================================================================
static inline int thread_cancel(thread_t *thread)
{
        return _thread_cancel(thread);
}

//==============================================================================
/**
 * @brief Check if thread is finished
 *
 * @param thread        thread object
 *
 * @return true if finished, otherwise false
 */
//==============================================================================
static inline bool thread_is_finished(thread_t *thread)
{
        return _thread_is_finished(thread);
}

//==============================================================================
/**
 * @brief Delete thread object
 *
 * @param thread        thread object
 *
 * @return 0 on success
 * @return -EAGAIN if thread is running, try later
 * @return -EINVAL if argument is invalid
 */
//==============================================================================
static inline int thread_delete(thread_t *thread)
{
        return _thread_delete(thread);
}

//==============================================================================
/**
 * @brief Function create semaphore
 *
 * @param cnt_max       max count value (1 for binary)
 * @param cnt_init      initial value (0 or 1 for binary)
 *
 * @return semaphore object pointer, otherwise NULL
 */
//==============================================================================
static inline sem_t *semaphore_new(const uint cnt_max, const uint cnt_init)
{
        return _semaphore_new(cnt_max, cnt_init);
}

//==============================================================================
/**
 * @brief Function delete semaphore
 *
 * @param[in] sem       semaphore object
 */
//==============================================================================
static inline void semaphore_delete(sem_t *sem)
{
        _semaphore_delete(sem);
}

//==============================================================================
/**
 * @brief Function wait for semaphore
 *
 * @param[in] sem               semaphore object
 * @param[in] timeout           semaphore polling time
 *
 * @retval true                 semaphore taken
 * @retval false                semaphore not taken
 */
//==============================================================================
static inline bool semaphore_wait(sem_t *sem, const uint timeout)
{
        return _semaphore_take(sem, timeout);
}

//==============================================================================
/**
 * @brief Function signal semaphore
 *
 * @param[in] sem       semaphore object
 *
 * @retval true         semaphore given
 * @retval false        semaphore not given
 */
//==============================================================================
static inline bool semaphore_signal(sem_t *sem)
{
        return _semaphore_give(sem);
}

//==============================================================================
/**
 * @brief Function wait for semaphore from ISR
 *
 * @param[in]  sem              semaphore object
 * @param[out] task_woken       true if higher priority task woke, otherwise false
 *
 * @retval true                 semaphore taken
 * @retval false                semaphore not taken
 */
//==============================================================================
static inline bool semaphore_wait_from_ISR(sem_t *sem, bool *task_woken)
{
        return _semaphore_take_from_ISR(sem, task_woken);
}

//==============================================================================
/**
 * @brief Function signal semaphore from ISR
 *
 * @param[in]  sem              semaphore object
 * @param[out] task_woken       true if higher priority task woke, otherwise false
 *
 * @retval true                 semaphore taken
 * @retval false                semaphore not taken
 */
//==============================================================================
static inline bool semaphore_signal_from_ISR(sem_t *sem, bool *task_woken)
{
        return _semaphore_give_from_ISR(sem, task_woken);
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
static inline mutex_t *mutex_new(enum mutex_type type)
{
        return _mutex_new(type);
}

//==============================================================================
/**
 * @brief Function delete mutex
 *
 * @param[in] mutex     mutex object
 */
//==============================================================================
static inline void mutex_delete(mutex_t *mutex)
{
        _mutex_delete(mutex);
}

//==============================================================================
/**
 * @brief Function lock mutex
 *
 * @param[in] mutex            mutex object
 * @param[in] timeout          polling time
 *
 * @retval true                 mutex locked
 * @retval false                mutex not locked
 */
//==============================================================================
static inline bool mutex_lock(mutex_t *mutex, const uint timeout)
{
        return _mutex_lock(mutex, timeout);
}

//==============================================================================
/**
 * @brief Function unlock mutex
 *
 * @param[in] mutex             mutex object
 *
 * @retval true                 mutex unlocked
 * @retval false                mutex still locked
 */
//==============================================================================
static inline bool mutex_unlock(mutex_t *mutex)
{
        return _mutex_unlock(mutex);
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
static inline queue_t *queue_new(const uint length, const uint item_size)
{
        return _queue_new(length, item_size);
}

//==============================================================================
/**
 * @brief Function delete queue
 *
 * @param[in] queue             queue object
 */
//==============================================================================
static inline void queue_delete(queue_t *queue)
{
        _queue_delete(queue);
}

//==============================================================================
/**
 * @brief Function reset queue
 *
 * @param[in] queue             queue object
 */
//==============================================================================
static inline void queue_reset(queue_t *queue)
{
        _queue_reset(queue);
}

//==============================================================================
/**
 * @brief Function send queue
 *
 * @param[in] queue             queue object
 * @param[in] item              item
 * @param[in] timeout           timeout
 *
 * @retval true                 item posted
 * @retval false                item not posted
 */
//==============================================================================
static inline bool queue_send(queue_t *queue, const void *item, const uint timeout)
{
        return _queue_send(queue, item, timeout);
}

//==============================================================================
/**
 * @brief Function send queue
 *
 * @param[in]  queue            queue object
 * @param[in]  item             item
 * @param[out] task_woken       true if higher priority task woke, otherwise false
 *
 * @retval true                 item posted
 * @retval false                item not posted
 */
//==============================================================================
static inline bool queue_send_from_ISR(queue_t *queue, const void *item, bool *task_woken)
{
        return _queue_send_from_ISR(queue, item, task_woken);
}

//==============================================================================
/**
 * @brief Function send queue
 *
 * @param[in]  queue            queue object
 * @param[out] item             item
 * @param[in]  timeout          timeout
 *
 * @retval true                 item received
 * @retval false                item not received
 */
//==============================================================================
static inline bool queue_receive(queue_t *queue, void *item, const uint timeout)
{
        return _queue_receive(queue, item, timeout);
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
static inline bool queue_recieve_from_ISR(queue_t *queue, void *item, bool *task_woken)
{
        return _queue_receive_from_ISR(queue, item, task_woken);
}

//==============================================================================
/**
 * @brief Function peek queue
 *
 * @param[in]  queue            queue object
 * @param[out] item             item
 * @param[in]  waittime_ms      wait time
 *
 * @retval true                 item received
 * @retval false                item not received
 */
//==============================================================================
static inline bool queue_receive_peek(queue_t *queue, void *item, const uint timeout)
{
        return _queue_receive_peek(queue, item, timeout);
}

//==============================================================================
/**
 * @brief Function gets number of items in queue
 *
 * @param[in] queue             queue object
 *
 * @return a number of items in queue, -1 if error
 */
//==============================================================================
static inline int queue_get_number_of_items(queue_t *queue)
{
        return _queue_get_number_of_items(queue);
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
static inline int queue_get_number_of_items_from_ISR(queue_t *queue)
{
        return _queue_get_number_of_items_from_ISR(queue);
}

//==============================================================================
/**
 * @brief Function enter to critical section
 */
//==============================================================================
static inline void critical_section_begin(void)
{
        _critical_section_begin();
}

//==============================================================================
/**
 * @brief Function exit from critical section
 */
//==============================================================================
static inline void critical_section_end(void)
{
        _critical_section_end();
}

//==============================================================================
/**
 * @brief Function disable interrupts
 */
//==============================================================================
static inline void ISR_disable(void)
{
        _ISR_disable();
}

//==============================================================================
/**
 * @brief Function enable interrupts
 */
//==============================================================================
static inline void ISR_enable(void)
{
        _ISR_enable();
}

#ifdef __cplusplus
}
#endif

#endif /* _THREAD_H_ */
/*==============================================================================
  End of file
==============================================================================*/
