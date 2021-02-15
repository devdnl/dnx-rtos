/*==============================================================================
File    threads.c

Author  Daniel Zorychta

Brief   C11 threads library implementation.

        Copyright (C) 2021 Daniel Zorychta <daniel.zorychta@gmail.com>

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

==============================================================================*/

/*==============================================================================
  Include files
==============================================================================*/
#include <threads.h>
#include <dnx/thread.h>

/*==============================================================================
  Local macros
==============================================================================*/

/*==============================================================================
  Local object types
==============================================================================*/

/*==============================================================================
  Local function prototypes
==============================================================================*/

/*==============================================================================
  Local objects
==============================================================================*/

/*==============================================================================
  Exported objects
==============================================================================*/

/*==============================================================================
  External objects
==============================================================================*/

/*==============================================================================
  Function definitions
==============================================================================*/

//==============================================================================
/**
 * @brief  Creates a new thread executing the function func. The function is invoked as func(arg).
 * If successful, the object pointed to by thr is set to the identifier of the new thread.
 * The completion of this function synchronizes-with the beginning of the thread.
 *
 * @param  thr          pointer to memory location to put the identifier of the new thread
 * @param  func         function to execute
 * @param  arg          argument to pass to the function
 *
 * @return thrd_success if the creation of the new thread was successful.
 *         Otherwise returns thrd_nomem if there was insufficient amount of
 *         memory or thrd_error if another error occurred.
 */
//==============================================================================
int thrd_create(thrd_t *thr, thrd_start_t func, void *arg)
{
        if (thr && func) {
                thread_attr_t ATTR = {
                        .stack_depth = STACK_DEPTH_LOW,
                        .priority    = _task_get_priority(_THIS_TASK),
                        .detached    = false,
                };

                tid_t tid = thread_create(func, &ATTR, arg);
                if (tid > 0) {
                        *thr = tid;
                        return thrd_success;
                }
        }

        return thrd_error;
}

//==============================================================================
/**
 * @brief  Checks whether lhs and rhs refer to the same thread.
 *
 * @param  lhs          thread to compare
 * @param  rhs          thread to compare
 *
 * @return Non-zero value if lhs and rhs refer to the same value, ​0​ otherwise.
 */
//==============================================================================
int thrd_equal(thrd_t lhs, thrd_t rhs)
{
        return lhs == rhs;
}

//==============================================================================
/**
 * @brief  Returns the identifier of the calling thread.
 *
 * @return The identifier of the calling thread.
 */
//==============================================================================
thrd_t thrd_current(void)
{
        return _process_get_active_thread(NULL);
}

//==============================================================================
/**
 * @brief Blocks the execution of the current thread for at least until the
 * TIME_UTC based duration pointed to by duration has elapsed. The sleep may
 * resume earlier if a signal that is not ignored is received. In such case,
 * if remaining is not NULL, the remaining time duration is stored into the
 * object pointed to by remaining.
 *
 * @param  duration     pointer to the duration to sleep for
 * @param  remaining    pointer to the object to put the remaining time on
 *                      interruption. May be NULL, in which case it is ignored
 *
 * @return 0​ on successful sleep, -1 if a signal occurred, other negative value if an error occurred.
 */
//==============================================================================
int thrd_sleep(const struct timespec *duration, struct timespec *remaining)
{
        u32_t sleep = (duration->tv_sec * 1000) + (duration->tv_nsec / 1000000);
        _builtinfunc(sleep_ms, sleep);

        if (remaining) {
                remaining->tv_nsec = 0;
                remaining->tv_sec  = 0;
        }

        return 0;
}

//==============================================================================
/**
 * @brief Provides a hint to the implementation to reschedule the execution of
 * threads, allowing other threads to run.
 */
//==============================================================================
void thrd_yield(void)
{
        _builtinfunc(sleep_ms, 0);
}

//==============================================================================
/**
 * @brief
 *
 * @param  ?
 *
 * @return ?
 */
//==============================================================================
void thrd_exit(int exit)
{
        UNUSED_ARG1(exit);
        thread_cancel(thrd_current());
        while (true);
}

//==============================================================================
/**
 * @brief
 *
 * @param  ?
 *
 * @return ?
 */
//==============================================================================
int thrd_detach(thrd_t thrd)
{
        UNUSED_ARG1(thrd);
        return thrd_error;
}

//==============================================================================
/**
 * @brief  Blocks the current thread until the thread identified by thr finishes
 *  execution. If res is not a null pointer, the result code of the thread is put
 *  to the location pointed to by res. The termination of the thread
 *  synchronizes-with the completion of this function. The behavior is undefined
 *  if the thread was previously detached or joined by another thread.
 *
 * @param  thr          identifier of the thread to join
 * @param  res          location to put the result code to
 *
 * @return ?
 */
//==============================================================================
int thrd_join(thrd_t thr, int *result)
{
        if (result) *result = 0;
        int err = thread_join(thr);
        return !err ? thrd_success : thrd_error;
}

//==============================================================================
/**
 * @brief
 *
 * @param  ?
 *
 * @return thrd_success if successful, thrd_error otherwise.
 */
//==============================================================================
int mtx_init(mtx_t *mtx, int type)
{
        mutex_t *mutex;
        int err = _mutex_create(type == mtx_recursive
                               ? MUTEX_TYPE_RECURSIVE
                               : MUTEX_TYPE_NORMAL, &mutex);
        if (!err) {
                *mtx = mutex;
        }

        return !err ? thrd_success : thrd_error;
}

//==============================================================================
/**
 * @brief
 *
 * @param   mtx         pointer to the mutex to lock
 */
//==============================================================================
void mtx_destroy(mtx_t *mtx)
{
        _mutex_destroy(*mtx);
}

//==============================================================================
/**
 * @brief
 *
 * @param   mtx         pointer to the mutex to lock
 *
 * @return thrd_success if successful, thrd_error otherwise.
 */
//==============================================================================
int mtx_lock(mtx_t *mtx)
{
        int err = _mutex_lock(*mtx, MAX_DELAY_MS);
        return !err ? thrd_success : thrd_error;
}

//==============================================================================
/**
 * @brief
 *
 * @param   mtx         pointer to the mutex to lock
 * @param   time_point  pointer to the timeout time to wait until
 *
 * @return thrd_success if successful, thrd_error otherwise.
 */
//==============================================================================
int mtx_timedlock(mtx_t *__restrict mtx, const struct timespec *__restrict time_point)
{
        u32_t timeout = (time_point->tv_sec * 1000) + (time_point->tv_nsec / 1000000);
        int err = _mutex_lock(*mtx, timeout);
        return !err ? thrd_success : thrd_error;
}

//==============================================================================
/**
 * @brief
 *
 * @param   mtx         pointer to the mutex to lock
 *
 * @return thrd_success if successful, thrd_error otherwise.
 */
//==============================================================================
int mtx_trylock(mtx_t *mtx)
{
        int err = _mutex_lock(*mtx, 0);
        return !err ? thrd_success : thrd_error;
}

//==============================================================================
/**
 * @brief
 *
 * @param   mtx         pointer to the mutex to lock
 *
 * @return thrd_success if successful, thrd_error otherwise.
 */
//==============================================================================
int mtx_unlock(mtx_t *mtx)
{
        int err = _mutex_unlock(*mtx);
        return !err ? thrd_success : thrd_error;
}

//==============================================================================
/**
 * @brief  Initializes new condition variable. The object pointed to by cond will
 *         be set to value that identifies the condition variable.
 *
 * @param  cnd          pointer to a variable to store identifier of the condition variable to
 *
 * @return thrd_success if successful, thrd_error otherwise.
 */
//==============================================================================
int cnd_init(cnd_t *cnd)
{
        sem_t *sem;
        int err = _semaphore_create(1, 0, &sem);
        if (!err) {
                *cnd = sem;
        }
        return !err ? thrd_success : thrd_error;
}

//==============================================================================
/**
 * @brief  Destroys the condition variable pointed to by cond.
 *         If there are threads waiting on cond, the behavior is undefined.
 *
 * @param  cnd          pointer to the condition variable to destroy
 */
//==============================================================================
void cnd_destroy(cnd_t *cnd)
{
        _semaphore_destroy(*cnd);
}

/*==============================================================================
  End of file
==============================================================================*/
