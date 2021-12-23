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
#include <stdbool.h>
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
                        .priority    = PRIORITY_NORMAL,
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
        return thread_current();
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

        _libc_syscall(_LIBC_SYS_MSLEEP, NULL, &sleep);

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
        _libc_syscall(_LIBC_SYS_MSLEEP, NULL, &(const uint32_t){0});
}

//==============================================================================
/**
 * @brief  First, for every thread-specific storage key which was created with
 * a non-null destructor and for which the associated value is non-null (see
 * tss_create), thrd_exit sets the value associated with the key to NULL and
 * then invokes the destructor with the previous value of the key. The order in
 * which the destructors are invoked is unspecified.
 *
 * If, after this, there remain keys with both non-null destructors and values
 * (e.g. if a destructor executed tss_set), the process is repeated up to
 * TSS_DTOR_ITERATIONS times.
 *
 * Finally, the thrd_exit function terminates execution of the calling thread
 * and sets its result code to res.
 *
 * If the last thread in the program is terminated with thrd_exit, the entire
 * program terminates as if by calling exit with EXIT_SUCCESS as the argument
 * (so the functions registered by atexit are executed in the context of that
 * last thread).
 *
 * @param  res          exit value
 */
//==============================================================================
void thrd_exit(int res)
{
        thread_exit(res);
        while (1) {}
}

//==============================================================================
/**
 * @brief  Detaches the thread identified by thr from the current environment.
 * The resources held by the thread will be freed automatically once the thread
 * exits.
 *
 * @param  thrd         identifier of the thread to detach
 *
 * @return thrd_success if successful, thrd_error otherwise.
 */
//==============================================================================
int thrd_detach(thrd_t thrd)
{
        UNUSED_ARG1(thrd);
        // not supported by dnx RTOS
        return thrd_error;
}

//==============================================================================
/**
 * @brief  Blocks the current thread until the thread identified by thr finishes
 * execution. If res is not a null pointer, the result code of the thread is put
 * to the location pointed to by res. The termination of the thread
 * synchronizes-with the completion of this function. The behavior is undefined
 * if the thread was previously detached or joined by another thread.
 *
 * @param  thr          identifier of the thread to join
 * @param  res          location to put the result code to
 *
 * @return thrd_success if successful, thrd_error otherwise.
 */
//==============================================================================
int thrd_join(thrd_t thr, int *res)
{
        int err = thread_join(thr, res);
        return !err ? thrd_success : thrd_error;
}

//==============================================================================
/**
 * @brief Creates a new mutex object with type. The object pointed to by mutex
 * is set to an identifier of the newly created mutex.
 *
 * type must have one of the following values:
 * - mtx_plain - a simple, non-recursive mutex is created.
 * - mtx_timed - a non-recursive mutex, that supports timeout, is created.
 * - mtx_plain | mtx_recursive - a recursive mutex is created.
 * - mtx_timed | mtx_recursive - a recursive mutex, that supports timeout, is created.
 *
 * @param  mtx          pointer to the mutex to initialize
 * @param  type         the type of the mutex
 *
 * @return thrd_success if successful, thrd_error otherwise.
 */
//==============================================================================
int mtx_init(mtx_t *mtx, int type)
{
        *mtx = mutex_new(type == mtx_recursive ? MUTEX_TYPE_RECURSIVE : MUTEX_TYPE_NORMAL);
        return *mtx ? thrd_success : thrd_error;
}

//==============================================================================
/**
 * @brief Destroys the mutex pointed to by mutex.
 * If there are threads waiting on mutex, the behavior is undefined.
 *
 * @param   mtx         pointer to the mutex to lock
 */
//==============================================================================
void mtx_destroy(mtx_t *mtx)
{
        mutex_delete(*mtx);
}

//==============================================================================
/**
 * @brief Blocks the current thread until the mutex pointed to by mutex is locked.
 * The behavior is undefined if the current thread has already locked the mutex
 * and the mutex is not recursive.
 * Prior calls to mtx_unlock on the same mutex synchronize-with this operation,
 * and all lock/unlock operations on any given mutex form a single total order
 * (similar to the modification order of an atomic).
 *
 * @param   mtx         pointer to the mutex to lock
 *
 * @return thrd_success if successful, thrd_error otherwise.
 */
//==============================================================================
int mtx_lock(mtx_t *mtx)
{
        return mutex_lock(*mtx, MAX_DELAY_MS) ? thrd_success : thrd_error;
}

//==============================================================================
/**
 * @brief Blocks the current thread until the mutex pointed to by mutex is locked
 * or until the TIME_UTC based time point pointed to by time_point has been reached.
 *
 * The behavior is undefined if the current thread has already locked the mutex
 * and the mutex is not recursive.
 *
 * The behavior is undefined if the mutex does not support timeout.
 *
 * Prior calls to mtx_unlock on the same mutex synchronize-with this operation
 * (if this operation succeeds), and all lock/unlock operations on any given
 * mutex form a single total order (similar to the modification order of an atomic).
 *
 * @param   mtx         pointer to the mutex to lock
 * @param   time_point  pointer to the timeout time to wait until
 *
 * @return thrd_success if successful, thrd_error otherwise.
 */
//==============================================================================
int mtx_timedlock(mtx_t *restrict mtx, const struct timespec *restrict time_point)
{
        u32_t timeout = (time_point->tv_sec * 1000) + (time_point->tv_nsec / 1000000);
        return mutex_lock(*mtx, timeout) ? thrd_success : thrd_error;
}

//==============================================================================
/**
 * @brief  Tries to lock the mutex pointed to by mutex without blocking. Returns
 * immediately if the mutex is already locked.
 *
 * Prior calls to mtx_unlock on the same mutex synchronize-with this operation
 * (if this operation succeeds), and all lock/unlock operations on any given
 * mutex form a single total order (similar to the modification order of an
 * atomic).
 *
 * @param   mtx         pointer to the mutex to lock
 *
 * @return thrd_success if successful, thrd_error otherwise.
 */
//==============================================================================
int mtx_trylock(mtx_t *mtx)
{
        return mutex_lock(*mtx, 0) ? thrd_success : thrd_error;
}

//==============================================================================
/**
 * @brief Unlocks the mutex pointed to by mutex.
 * The behavior is undefined if the mutex is not locked by the calling thread.
 *
 * This function synchronizes-with subsequent mtx_lock, mtx_trylock, or
 * mtx_timedlock on the same mutex. All lock/unlock operations on any given
 * mutex form a single total order (similar to the modification order of an atomic).
 *
 * @param   mtx         pointer to the mutex to lock
 *
 * @return thrd_success if successful, thrd_error otherwise.
 */
//==============================================================================
int mtx_unlock(mtx_t *mtx)
{
        return mutex_unlock(*mtx) ? thrd_success : thrd_error;
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
        *cnd = semaphore_new(1, 0);
        return *cnd ? thrd_success : thrd_error;
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
        semaphore_delete(*cnd);
}

//==============================================================================
/**
 * @brief Atomically unlocks the mutex pointed to by mutex and blocks on the
 * condition variable pointed to by cond until the thread is signalled by
 * cnd_signal or cnd_broadcast, or until a spurious wake-up occurs. The mutex
 * is locked again before the function returns.
 *
 * The behavior is undefined if the mutex is not already locked by the calling thread.
 *
 * @param  cnd          pointer to the condition variable to block on
 * @param  mtx          pointer to the mutex to unlock for the duration of the block
 *
 * @return thrd_success if successful, thrd_error otherwise.
 */
//==============================================================================
int cnd_wait(cnd_t *cnd, mtx_t *mtx)
{
        int err = thrd_error;
        if (mutex_unlock(*mtx)) {
                if (semaphore_wait(*cnd, MAX_DELAY_MS)) {
                        err = thrd_success;
                }
                mutex_lock(*mtx, MAX_DELAY_MS);
        }
        return err;
}

//==============================================================================
/**
 * @brief Atomically unlocks the mutex pointed to by mutex and blocks on the
 * condition variable pointed to by cond until the thread is signalled by
 * cnd_signal or cnd_broadcast, or until the TIME_UTC based time point pointed
 * to by time_point has been reached, or until a spurious wake-up occurs.
 * The mutex is locked again before the function returns.
 *
 * The behavior is undefined if the mutex is not already locked by the calling thread.
 *
 * @param  cnd          pinter to the condition variable to block on
 * @param  mtx          pointer to the mutex to unlock for the duration of the block
 * @param  duration     pointer to a object specifying timeout time to wait until
 *
 * @return thrd_success if successful, thrd_timedout if the timeout time has
 * been reached before the mutex is locked, or thrd_error if an error occurred.
 */
//==============================================================================
int cnd_timedwait(cnd_t *restrict cnd, mtx_t *restrict mtx, const struct timespec *restrict duration)
{
        u32_t timeout = (duration->tv_sec * 1000) + (duration->tv_nsec / 1000000);

        int err = thrd_error;
        if (mutex_unlock(*mtx)) {
                if (semaphore_wait(*cnd, timeout)) {
                        err = thrd_success;
                }
                mutex_lock(*mtx, MAX_DELAY_MS);
        }
        return err;
}

//==============================================================================
/**
 * @brief  Unblocks one thread that currently waits on condition variable pointed
 * to by cond. If no threads are blocked, does nothing and returns thrd_success.
 *
 * @param  cnd          pinter to the condition variable
 *
 * @return thrd_success if successful, thrd_error otherwise.
 */
//==============================================================================
int cnd_signal(cnd_t *cnd)
{
        return semaphore_signal(*cnd) ? thrd_success : thrd_error;
}

//==============================================================================
/**
 * @brief  Unblocks all thread that currently wait on condition variable pointed
 * to by cond. If no threads are blocked, does nothing and returns thrd_success.
 *
 * @param  cnd          pinter to the condition variable
 *
 * @return thrd_success if successful, thrd_error otherwise.
 */
//==============================================================================
int cnd_broadcast(cnd_t *cnd)
{
        UNUSED_ARG1(cnd);
        // not supported by dnx RTOS
        return thrd_error;
}

//==============================================================================
/**
 * @brief  Calls function func exactly once, even if invoked from several threads.
 * The completion of the function func synchronizes with all previous or
 * subsequent calls to call_once with the same flag variable.
 *
 * @param  flag         pointer to an object of type call_once that is used to ensure func is called only once
 * @param  func         the function to execute only once
 */
//==============================================================================
void call_once(once_flag *once, void (*func)(void))
{
        if (*once == ONCE_FLAG_INIT) {
                bool call = false;

                _libc_syscall(_LIBC_SYS_SCHEDULERLOCK, NULL);

                call = (*once == ONCE_FLAG_INIT);
                *once = ONCE_FLAG_INIT + 1;

                _libc_syscall(_LIBC_SYS_SCHEDULERUNLOCK, NULL);

                if (call) {
                        func();
                }
        }
}

//==============================================================================
/**
 * @brief  Creates new thread-specific storage key and stores it in the object
 * pointed to by tss_key. Although the same key value may be used by different
 * threads, the values bound to the key by tss_set are maintained on a per-thread
 * basis and persist for the life of the calling thread.
 *
 * The value NULL is associated with the newly created key in all existing
 * threads, and upon thread creation, the values associated with all TSS keys
 * is initialized to NULL.
 *
 * If destructor is not a null pointer, then also associates the destructor
 * which is called when the storage is released by thrd_exit (but not by
 * tss_delete and not at program termination by exit).
 *
 * A call to tss_create from within a thread-specific storage destructor results
 * in undefined behavior.
 *
 * @param  tss          pointer to memory location to store the new thread-specific storage key
 * @param  dtor         pointer to a function to call at thread exit
 *
 * @return thrd_success if successful, thrd_error otherwise.
 */
//==============================================================================
int tss_create(tss_t *tss, tss_dtor_t dtor)
{
        UNUSED_ARG2(tss, dtor);
        // not supported by dnx RTOS
        return thrd_error;
}

//==============================================================================
/**
 * @brief  Destroys the thread-specific storage identified by tss_id.
 *
 * The destructor, if one was registered by tss_create, is not called (they are
 * only called at thread exit, either by thrd_exit or by returning from the thread
 * function), it is the responsibility of the programmer to ensure that every
 * thread that is aware of tss_id performed all necessary cleanup, before the
 * call to tss_delete is made.
 *
 * If tss_delete is called while another thread is executing destructors for
 * tss_id, it's unspecified whether this changes the number of invocations to
 * the associated destructor.
 *
 * If tss_delete is called while the calling thread is executing destructors,
 * then the destructor associated with tss_id will not be executed again on this
 * thread.
 *
 * @param  tss  thread-specific storage key previously returned by tss_create and not yet deleted by tss_delete
 */
//==============================================================================
void tss_delete(tss_t tss)
{
        UNUSED_ARG1(tss);
        // not supported by dnx RTOS
}

//==============================================================================
/**
 * @brief  Returns the value held in thread-specific storage for the current
 * thread identified by tss_key. Different threads may get different values
 * identified by the same key.
 *
 * On thread startup (see thrd_create), the values associated with all TSS keys
 * are NULL. Different value may be placed in the thread-specific storage with
 * tss_set.
 *
 * @param  tss  thread-specific storage key previously returned by tss_create and not yet deleted by tss_delete
 *
 * @return The value on success, NULL on failure.
 */
//==============================================================================
void *tss_get(tss_t tss)
{
        UNUSED_ARG1(tss);
        // not supported by dnx RTOS
        return NULL;
}

//==============================================================================
/**
 * @brief  Sets the value of the thread-specific storage identified by tss_id
 * for the current thread to val. Different threads may set different values to
 * the same key.
 *
 * The destructor, if available, is not invoked.
 *
 * @param  tss  thread-specific storage key previously returned by tss_create and not yet deleted by tss_delete
 * @param  val  value to set thread-specific storage to
 *
 * @return thrd_success if successful, thrd_error otherwise.
 */
//==============================================================================
int tss_set(tss_t tss, void *val)
{
        UNUSED_ARG2(tss, val);
        // not supported by dnx RTOS
        return thrd_error;
}

/*==============================================================================
  End of file
==============================================================================*/
