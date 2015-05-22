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
#include <kernel/kwrapper.h>
#include <kernel/process.h>

/*==============================================================================
  Exported macros
==============================================================================*/

/*==============================================================================
  Exported object types
==============================================================================*/

/*==============================================================================
  Exported objects
==============================================================================*/
extern int _errno;

/*==============================================================================
  Exported functions
==============================================================================*/

/*==============================================================================
  Exported inline functions
==============================================================================*/

//==============================================================================
/**
 * @brief pid_t process_new(const char *cmd, const process_attr_t *attr)
 * The function <b>process_new</b>() create new process according to command
 * pointed by <i>cmd</i> and attributes pointed by <i>attr</i>. Attributes
 * can be NULL what means that default setting will be applied. Command field
 * is mandatory.
 *
 * @param cmd           program name and argument list
 * @param attr          process attributes
 *
 * @errors ENOMEM, EINVAL, ENOENT
 *
 * @return On success, return process ID (PID), otherwise 0.
 *
 * @example
 * #include <dnx/thread.h>
 *
 * // ...
 *
 * errno = 0;
 *
 * static const process_attr_t attr = {
 *         .f_stdin   = stdin,
 *         .f_stdout  = stdout,
 *         .f_stderr  = stderr,
 *         .p_stdin   = NULL,
 *         .p_stdout  = NULL,
 *         .p_stderr  = NULL,
 *         .no_parent = false
 * }
 *
 * pid_t pid = process_create("ls /", &attr);
 * if (pid) {
 *         process_wait(pid, MAX_DELAY_MS);
 *
 *         int exit_code = 0;
 *         process_delete(pid, &exit_code);
 * } else {
 *         perror("Program not started");
 *
 *         // ...
 * }
 *
 * // ...
 */
//==============================================================================
static inline pid_t process_create(const char *cmd, const process_attr_t *attr)
{
        pid_t pid = 0;
        syscall(SYSCALL_PROCESSCREATE, &pid, cmd, attr);
        return pid;
}

//==============================================================================
/**
 * @brief int process_kill(pid_t pid, int *status)
 * The function <b>process_kill</b>() delete running or closed process.
 * Function return process exit code pointed by <i>status</i>.
 *
 * @param pid                   process ID
 * @param status                child process exit status (it can be NULL)
 *
 * @errors EINVAL, EAGAIN
 *
 * @return Return 0 on success. On error, -1 is returned, and
 * <b>errno</b> is set appropriately.
 *
 * @example
 * #include <dnx/thread.h>
 *
 * // ...
 *
 * errno = 0;
 *
 * static const process_attr_t attr = {
 *         .f_stdin   = stdin,
 *         .f_stdout  = stdout,
 *         .f_stderr  = stderr,
 *         .p_stdin   = NULL,
 *         .p_stdout  = NULL,
 *         .p_stderr  = NULL,
 *         .parent    = true
 * }
 *
 * int   status = -1;
 * pid_t pid    = process_create("ls /", &attr);
 * if (pid) {
 *         sleep(1); // child execute time
 *         process_kill(pid, &status);
 * } else {
 *         perror("Program not started");
 *
 *         // ...
 * }
 *
 * // ...
 */
//==============================================================================
static inline int process_kill(pid_t pid, int *status)
{
        int r = -1;
        syscall(SYSCALL_PROCESSDESTROY, &r, &pid, status);
        return r;
}

//==============================================================================
/**
 * @brief int process_wait(pid_t pid, int *status, const uint timeout)
 * The function <b>process_wait</b>() wait for program close. Function destroy
 * child process when finish successfully in the selected timeout. In case of
 * timeout then process is not destroyed.
 *
 * @param pid                   process ID
 * @param status                child process exit status (it can be NULL)
 * @param timeout               wait timeout in ms
 *
 * @errors EINVAL, ETIME, ...
 *
 * @return Return 0 on success. On error -1 is returned, and
 * <b>errno</b> is set appropriately.
 *
 * @example
 * #include <dnx/os.h>
 *
 * // ...
 *
 * errno = 0;
 *
 * static const process_attr_t attr = {
 *         .f_stdin   = stdin,
 *         .f_stdout  = stdout,
 *         .f_stderr  = stderr,
 *         .p_stdin   = NULL,
 *         .p_stdout  = NULL,
 *         .p_stderr  = NULL,
 *         .no_parent = false
 * }
 *
 * int   status = -1;
 * pid_t pid    = process_create("ls /", &attr);
 * if (pid) {
 *         process_wait(pid, &status, MAX_DELAY_MS);
 * } else {
 *         perror("Program not started");
 *
 *         // ...
 * }
 *
 * // ...
 */
//==============================================================================
static inline int process_wait(pid_t pid, int *status, const uint timeout)
{
        int r      = -1;
        sem_t *sem = NULL;
        syscall(SYSCALL_PROCESSGETEXITSEM, &r, &pid, &sem);
        if (sem && r == 0) {
                _errno = _builtinfunc(semaphore_wait, sem, timeout);
                if (_errno == 0) {
                        syscall(SYSCALL_PROCESSDESTROY, &r, &pid, status);
                }
                r = _errno ? -1 : 0;
        }

        return r;
}

//==============================================================================
/**
 * @brief int process_stat_seek(size_t seek, process_stat_t *stat)
 * The function <b>process_stat_seek</b>() return statistics of next process selected
 * by <i>seek</i>.
 * <pre>
 * typedef struct {
 *         const char *name;
 *         pid_t       pid;
 *         size_t      files_count;
 *         size_t      dir_count;
 *         size_t      mutexes_count;
 *         size_t      semaphores_count;
 *         size_t      queue_count;
 *         size_t      threads_count;
 *         size_t      memory_block_count;
 *         size_t      memory_usage;
 *         size_t      cpu_load_cnt;
 *         size_t      stack_size;
 *         size_t      stack_free;
 * } process_stat_t;
 * </pre>
 *
 * @param seek      process index
 * @param stat      statistics
 *
 * @errors EINVAL, ENOENT, ESRCH
 *
 * @return Return 0 on success. On error, -1 is returned.
 *
 * @example
 * #include <dnx/thread.h>
 *
 * // ...
 *
 * process_stat_t stat;
 * size_t         seek = 0;
 * while (process_stat_seek(seek++, &stat) == 0) {
 *         printf("Memory usage: %d\n", stat.memory_usage);
 * }
 *
 * // ...
 */
//==============================================================================
static inline int process_stat_seek(size_t seek, process_stat_t *stat)
{
        int r = -1;
        syscall(SYSCALL_PROCESSSTATSEEK, &r, &seek, stat);
        return r;
}

//==============================================================================
/**
 * @brief int process_stat(pid_t pid, process_stat_t *stat)
 * The function <b>process_stat</b>() return statistics of selected process
 * by <i>pid</i>.
 * <pre>
 * typedef struct {
 *         const char *name;
 *         pid_t       pid;
 *         size_t      files_count;
 *         size_t      dir_count;
 *         size_t      mutexes_count;
 *         size_t      semaphores_count;
 *         size_t      queue_count;
 *         size_t      threads_count;
 *         size_t      memory_block_count;
 *         size_t      memory_usage;
 *         size_t      cpu_load_cnt;
 *         size_t      stack_size;
 *         size_t      stack_free;
 * } process_stat_t;
 * </pre>
 *
 * @param pid       PID
 * @param stat      statistics
 *
 * @errors EINVAL, ENOENT, ESRCH
 *
 * @return Return 0 on success. On error, -1 is returned.
 *
 * @example
 * #include <dnx/thread.h>
 * #include <unistd.h>
 *
 * // ...
 *
 * process_stat_t stat;
 * pid_t          pid = getpid(); // or process_getpid()
 * process_stat(pid, &stat);
 * printf("Memory usage of this process: %d\n", stat.memory_usage);
 *
 * // ...
 */
//==============================================================================
static inline int process_stat(pid_t pid, process_stat_t *stat)
{
        int r = -1;
        syscall(SYSCALL_PROCESSSTATPID, &r, &pid, stat);
        return r;
}

//==============================================================================
/**
 * @brief pid_t process_getpid(void)
 * The function <b>process_getpid</b>() return PID of current process (caller).
 *
 * @param None
 *
 * @errors EINVAL, ENOENT, ESRCH
 *
 * @return Return PID on success. On error, 0 is returned.
 *
 * @example
 * #include <dnx/thread.h>
 *
 * // ...
 *
 * pid_t pid = process_getpid();
 * printf("PID of this process is: %d\n, pid);
 *
 * // ...
 */
//==============================================================================
static inline pid_t process_getpid(void)
{
        pid_t pid = 0;
        syscall(SYSCALL_PROCESSGETPID, &pid);
        return pid;
}

//==============================================================================
/**
 * @brief int task_get_priority(void)
 * The function <b>task_get_priority</b>() returns priority value of task which
 * calls function.
 *
 * @param None
 *
 * @errors None
 *
 * @return Priority value.
 *
 * @example
 * #include <dnx/thread.h>
 * #include <stdbool.h>
 *
 * task_t *task;
 *
 * void my_task(void *arg)
 * {
 *         if (task_get_priority() < 0) {
 *                 task_set_priority(0);
 *         }
 *
 *         while (true) {
 *                 // task do something
 *         }
 * }
 */
//==============================================================================
static inline int process_get_priority(pid_t pid)
{
        int prio = 0;
        syscall(SYSCALL_PROCESSGETPRIO, &prio, &pid);
        return prio;
}

//==============================================================================
/**
 * @brief thread_t *thread_new(void (*func)(void*), const int stack_depth, void *arg)
 * The function <b>thread_new</b>() creates new thread using function pointed
 * by <i>func</i> with stack depth <i>stack_depth</i>. To thread can be passed
 * additional argument pointed by <i>arg</i>. If thread was created then
 * pointer to object is returned, otherwise <b>NULL</b> is returned, and
 * <b>errno</b> set appropriately. Threads are functions which are called as
 * new task and have own stack, but global variables are shared with parent
 * thread.
 *
 * @param func          thread function
 * @param stack_depth   stack depth
 * @param arg           thread argument
 *
 * @errors EINVAL, ENOMEM,
 *
 * @return If thread was created then pointer to object is returned, otherwise
 * <b>NULL</b> is returned, and <b>errno</b> set appropriately.
 *
 * @example
 * #include <dnx/thread.h>
 * #include <unistd.h>
 *
 * // ...
 *
 * void thread(void *arg)
 * {
 *         // ...
 *
 *         // thread function exit without any function,
 *         // or just by return
 * }
 *
 * void some_function()
 * {
 *         errno = 0;
 *         thread_t *thread = thread_new(thread, STACK_DEPTH_LOW, NULL);
 *         if (thread) {
 *                 // some code ...
 *
 *                 while (!thread_is_finished(thread)) {
 *                         sleep_ms(1);
 *                 }
 *
 *                 thread_delete(thread);
 *         } else {
 *                 perror("Thread error");
 *         }
 * }
 *
 * // ...
 */
//==============================================================================
static inline tid_t thread_create(thread_func_t func, thread_attr_t *attr, void *arg)
{
        tid_t tid = 0;
        syscall(SYSCALL_THREADCREATE, &tid, func, attr, arg);
        return tid;
}

//==============================================================================
/**
 * @brief int thread_cancel(thread_t *thread)
 * The function <b>thread_cancel</b>() kills running thread <i>tid</i>.
 *
 * @param thread        thread object
 *
 * @errors EINVAL
 *
 * @return On success, 0 is returned. On error, 1 is returned, and <b>errno</b>
 * is set appropriately.
 *
 * @example
 * #include <dnx/thread.h>
 * #include <unistd.h>
 *
 * // ...
 *
 * void thread(void *arg)
 * {
 *         // ...
 *
 *         // thread function exit without any function,
 *         // or just by return
 * }
 *
 * void some_function()
 * {
 *         errno = 0;
 *         thread_t *thread = thread_new(thread, STACK_DEPTH_LOW, NULL);
 *
 *         if (thread) {
 *                 // some code ...
 *
 *                 while (thread_cancel(thread) != true) {
 *                         sleep_ms(10);
 *                 }
 *
 *                 thread_delete(thread);
 *         } else {
 *                 perror("Thread error");
 *         }
 * }
 *
 * // ...
 */
//==============================================================================
static inline int thread_cancel(tid_t tid)
{
        int r = -1;
        syscall(SYSCALL_THREADDESTROY, &r, &tid);
        return r;
}

//==============================================================================
/**
 * @brief int thread_join(thread_t *thread)
 * The function <b>thread_join</b>() joins selected thread pointed by
 * <i>thread</i> to parent program. Function wait until thread was closed.
 *
 *
 * @param thread        thread object
 *
 * @errors ETIME, EINVAL
 *
 * @return On success, 0 is returned. On error, 1 is returned, and <b>errno</b>
 * is set appropriately.
 *
 * @example
 * #include <dnx/thread.h>
 * #include <unistd.h>
 *
 * // ...
 *
 * void thread1(void *arg)
 * {
 *         // ...
 *
 *         // thread function exit without any function,
 *         // or just by return
 * }
 *
 * void thread2(void *arg)
 * {
 *         // ...
 *
 *         // thread function exit without any function,
 *         // or just by return
 * }
 *
 * void some_function()
 * {
 *         errno = 0;
 *         thread_t *thread1 = thread_new(thread1, STACK_DEPTH_LOW, NULL);
 *         thread_t *thread2 = thread_new(thread2, STACK_DEPTH_LOW, NULL);
 *
 *         if (thread1 && thread2) {
 *                 // some code ...
 *
 *                 thread_join(thread1);
 *                 thread_join(thread2);
 *
 *                 thread_delete(thread1);
 *                 thread_delete(thread2);
 *         } else {
 *                 perror("Thread error");
 *
 *                 if (thread1) {
 *                         thread_cancel(thread1);
 *                         thread_delete(thread1);
 *                 }
 *
 *                 if (thread2) {
 *                         thread_cancel(thread2);
 *                         thread_delete(thread2);
 *                 }
 *         }
 * }
 *
 * // ...
 */
//==============================================================================
static inline int thread_join(tid_t tid)
{
        int r      = -1;
        sem_t *sem = NULL;

        syscall(SYSCALL_THREADGETEXITSEM, &r, &tid, &sem);

        if (sem && r == 0) {
                _builtinfunc(semaphore_wait, sem, MAX_DELAY_MS);
                syscall(SYSCALL_THREADDESTROY, &r, &tid);
        }

        return r;
}

//==============================================================================
/**
 * @brief sem_t *semaphore_new(const uint cnt_max, const uint cnt_init)
 * The function <b>semaphore_new</b>() creates new semaphore object. The
 * semaphore can be counting or binary. If counting then <i>cnt_max</i>
 * is bigger that 2. <i>cnt_init</i> is an initial value of semaphore.
 * Semaphore can be used for task synchronization.
 *
 * @param cnt_max       max count value (1 for binary)
 * @param cnt_init      initial value (0 or 1 for binary)
 *
 * @errors None
 *
 * @return On success, semaphore object is returned. On error, <b>NULL</b> is
 * returned.
 *
 * @example
 * #include <dnx/thread.h>
 * #include <stdbool.h>
 *
 * // ...
 *
 * sem_t *sem = semaphore_new(1, 0);
 *
 * // ...
 *
 * void thread2(void *arg)
 * {
 *         while (true) {
 *                 // this task will wait for semaphore signal
 *                 semaphore_wait(sem, MAX_DELAY_MS);
 *
 *                 // ...
 *         }
 * }
 *
 * void thread1(void *arg)
 * {
 *         while (true) {
 *                // ...
 *
 *                // this task signal to thread2 that can execute part of code
 *                semaphore_signal(sem);
 *         }
 * }
 *
 * // ...
 */
//==============================================================================
static inline sem_t *semaphore_new(const size_t cnt_max, const size_t cnt_init)
{
        sem_t *sem = NULL;
        syscall(SYSCALL_SEMAPHORECREATE, &sem, &cnt_max, &cnt_init);
        return sem;
}

//==============================================================================
/**
 * @brief void semaphore_delete(sem_t *sem)
 * The function <b>semaphore_delete</b>() removes created semaphore pointed by
 * <i>sem</i>. Be aware that if semaphore was removed when tasks use it, then
 * process starvation can occur on tasks which wait for semaphore signal.
 *
 * @param sem       semaphore object pointer
 *
 * @errors None
 *
 * @return
 *
 * @example
 * #include <dnx/thread.h>
 *
 * // ...
 *
 * sem_t *sem = semaphore_new(1, 0);
 *
 * // operation on semaphore
 *
 * semaphore_delete(sem);
 *
 * // ...
 */
//==============================================================================
static inline void semaphore_delete(sem_t *sem)
{
        syscall(SYSCALL_SEMAPHOREDESTROY, NULL, sem);
}

//==============================================================================
/**
 * @brief bool semaphore_wait(sem_t *sem, const uint timeout)
 * The function <b>semaphore_wait</b>() waits for semaphore signal pointed by
 * <i>sem</i> by <i>timeout</i> milliseconds. If semaphore was signaled then
 * <b>true</b> is returned, otherwise (timeout) <b>false</b>. When <i>timeout</i>
 * value is set to 0 then semaphore is polling without timeout.
 *
 * @param sem       semaphore object pointer
 *
 * @errors None
 *
 * @return On success, <b>true</b> is returned. On timeout or if semaphore is
 * not signaled <b>false</b> is returned.
 *
 * @example
 * #include <dnx/thread.h>
 * #include <stdbool.h>
 *
 * // ...
 *
 * sem_t *sem = semaphore_new(1, 0);
 *
 * // ...
 *
 * void thread2(void *arg)
 * {
 *         while (true) {
 *                 // this task will wait for semaphore signal
 *                 semaphore_wait(sem, MAX_DELAY_MS);
 *
 *                 // ...
 *         }
 * }
 *
 * void thread1(void *arg)
 * {
 *         while (true) {
 *                // ...
 *
 *                // this task signal to thread2 that can execute part of code
 *                semaphore_signal(sem);
 *         }
 * }
 *
 * // ...
 */
//==============================================================================
static inline bool semaphore_wait(sem_t *sem, const uint timeout)
{
        _errno = _builtinfunc(semaphore_wait, sem, timeout);
        return !_errno;
}

//==============================================================================
/**
 * @brief bool semaphore_signal(sem_t *sem)
 * The function <b>semaphore_signal</b>() signals semaphore pointed by <i>sem</i>.
 *
 * @param sem       semaphore object pointer
 *
 * @errors None
 *
 * @return On corrected signaling, <b>true</b> is returned. If semaphore cannot
 * be signaled or object is invalid then <b>false</b> is returned.
 *
 * @example
 * #include <dnx/thread.h>
 * #include <stdbool.h>
 *
 * // ...
 *
 * sem_t *sem = semaphore_new(1, 0);
 *
 * // ...
 *
 * void thread2(void *arg)
 * {
 *         while (true) {
 *                 // this task will wait for semaphore signal
 *                 semaphore_wait(sem, MAX_DELAY_MS);
 *
 *                 // ...
 *         }
 * }
 *
 * void thread1(void *arg)
 * {
 *         while (true) {
 *                // ...
 *
 *                // this task signal to thread2 that can execute part of code
 *                semaphore_signal(sem);
 *         }
 * }
 *
 * // ...
 */
//==============================================================================
static inline bool semaphore_signal(sem_t *sem)
{
        _errno = _builtinfunc(semaphore_signal, sem);
        return !_errno;
}

//==============================================================================
/**
 * @brief mutex_t *mutex_new(enum mutex_type type)
 * The function <b>mutex_new</b>() creates new mutex of type <i>type</i>.
 * Two types of mutex can be created: <b>MUTEX_RECURSIVE</b> and <b>MUTEX_NORMAL</b>.
 *
 * @param type          mutex type (MUTEX_RECURSIVE or MUTEX_NORMAL)
 *
 * @errors None
 *
 * @return On success, pointer to the mutex object is returned. On error,
 * <b>NULL</b> is returned.
 *
 * @example
 * #include <dnx/thread.h>
 *
 * int resource;
 *
 * void thread1(void *arg)
 * {
 *         mutex_t *mtx = arg;
 *
 *         // protected access to resource
 *         if (mutex_lock(mtx, MAX_DELAY_MS)) {
 *                 // write to buffer is allowed
 *                 resource = ...;
 *
 *                 // ...
 *
 *                 mutex_unlock(mtx);
 *         }
 * }
 *
 * void thread2(void *arg)
 * {
 *         mutex_t *mtx = arg;
 *
 *         // protected access to resource
 *         if (mutex_lock(mtx, MAX_DELAY_MS)) {
 *                 // write to buffer is allowed
 *                 resource = ...;
 *
 *                 // ...
 *
 *                 mutex_unlock(mtx);
 *         }
 * }
 *
 * int main()
 * {
 *         // ...
 *
 *         mutex_t *mtx = mutex_new(MUTEX_NORMAL);
 *
 *         thread_new(thread1, STACK_DEPTH_LOW, mtx);
 *         thread_new(thread2, STACK_DEPTH_LOW, mtx);
 *
 *         thread_join(thread1);
 *         thread_join(thread2);
 *
 *         mutex_delete(mtx);
 *
 *         // ...
 * }
 */
//==============================================================================
static inline mutex_t *mutex_new(enum mutex_type type)
{
        mutex_t *mtx = NULL;
        syscall(SYSCALL_MUTEXCREATE, &mtx, &type);
        return mtx;
}

//==============================================================================
/**
 * @brief void mutex_delete(mutex_t *mutex)
 * The function <b>mutex_delete</b>() delete created mutex pointed by <i>mutex</i>.
 *
 * @param mutex     mutex
 *
 * @errors None
 *
 * @return None
 *
 * @example
 * #include <dnx/thread.h>
 *
 * int resource;
 *
 * void thread1(void *arg)
 * {
 *         mutex_t *mtx = arg;
 *
 *         // protected access to resource
 *         if (mutex_lock(mtx, MAX_DELAY_MS)) {
 *                 // write to buffer is allowed
 *                 resource = ...;
 *
 *                 // ...
 *
 *                 mutex_unlock(mtx);
 *         }
 * }
 *
 * void thread2(void *arg)
 * {
 *         mutex_t *mtx = arg;
 *
 *         // protected access to resource
 *         if (mutex_lock(mtx, MAX_DELAY_MS)) {
 *                 // write to buffer is allowed
 *                 resource = ...;
 *
 *                 // ...
 *
 *                 mutex_unlock(mtx);
 *         }
 * }
 *
 * int main()
 * {
 *         // ...
 *
 *         mutex_t *mtx = mutex_new(MUTEX_NORMAL);
 *
 *         thread_new(thread1, STACK_DEPTH_LOW, mtx);
 *         thread_new(thread2, STACK_DEPTH_LOW, mtx);
 *
 *         thread_join(thread1);
 *         thread_join(thread2);
 *
 *         mutex_delete(mtx);
 *
 *         // ...
 * }
 */
//==============================================================================
static inline void mutex_delete(mutex_t *mutex)
{
        syscall(SYSCALL_MUTEXDESTROY, NULL, mutex);
}

//==============================================================================
/**
 * @brief bool mutex_lock(mutex_t *mutex, const uint timeout)
 * The function <b>mutex_lock</b>() lock mutex pointed by <i>mutex</i>. If
 * mutex is locked by other thread then system try to lock mutex by <i>timeout</i>
 * milliseconds. If mutex is recursive then task can lock mutex recursively, and
 * the same times shall be unlocked. If normal mutex is used then task can lock
 * mutex only one time (not recursively).
 *
 * @param mutex     mutex
 * @param timeout   timeout
 *
 * @errors None
 *
 * @return If mutex is locked then <b>true</b> is returned. If mutex is used or
 * timeout occur or object is incorrect, then <b>false</b> is returned.
 *
 * @example
 * #include <dnx/thread.h>
 *
 * int resource;
 *
 * void thread1(void *arg)
 * {
 *         mutex_t *mtx = arg;
 *
 *         // protected access to resource
 *         if (mutex_lock(mtx, MAX_DELAY_MS)) {
 *                 // write to buffer is allowed
 *                 resource = ...;
 *
 *                 // ...
 *
 *                 mutex_unlock(mtx);
 *         }
 * }
 *
 * void thread2(void *arg)
 * {
 *         mutex_t *mtx = arg;
 *
 *         // protected access to resource
 *         if (mutex_lock(mtx, MAX_DELAY_MS)) {
 *                 // write to buffer is allowed
 *                 resource = ...;
 *
 *                 // ...
 *
 *                 mutex_unlock(mtx);
 *         }
 * }
 *
 * int main()
 * {
 *         // ...
 *
 *         mutex_t *mtx = mutex_new(MUTEX_NORMAL);
 *
 *         thread_new(thread1, STACK_DEPTH_LOW, mtx);
 *         thread_new(thread2, STACK_DEPTH_LOW, mtx);
 *
 *         thread_join(thread1);
 *         thread_join(thread2);
 *
 *         mutex_delete(mtx);
 *
 *         // ...
 * }
 */
//==============================================================================
static inline bool mutex_lock(mutex_t *mutex, const uint timeout)
{
        _errno = _builtinfunc(mutex_lock, mutex, timeout);
        return !_errno;
}

//==============================================================================
/**
 * @brief bool mutex_trylock(mutex_t *mutex)
 * The function <b>mutex_trylock</b>() lock mutex pointed by <i>mutex</i>.
 * If mutex is recursive then task can lock mutex recursively, and
 * the same times shall be unlocked. If normal mutex is used then task can lock
 * mutex only one time (not recursively). Function is equivalent to
 * mutex_lock(mtx, 0) call.
 *
 * @param mutex     mutex
 *
 * @errors EINVAL, ETIME, ...
 *
 * @return If mutex is locked then <b>true</b> is returned. If mutex is used
 * or object is incorrect, then <b>false</b> is returned.
 *
 * @example
 * #include <dnx/thread.h>
 *
 * int resource;
 *
 * void thread1(void *arg)
 * {
 *         mutex_t *mtx = arg;
 *
 *         // protected access to resource
 *         if (mutex_trylock(mtx)) {
 *                 // write to buffer is allowed
 *                 resource = ...;
 *
 *                 // ...
 *
 *                 mutex_unlock(mtx);
 *         }
 * }
 *
 * void thread2(void *arg)
 * {
 *         mutex_t *mtx = arg;
 *
 *         // protected access to resource
 *         if (mutex_trylock(mtx, MAX_DELAY_MS)) {
 *                 // write to buffer is allowed
 *                 resource = ...;
 *
 *                 // ...
 *
 *                 mutex_unlock(mtx);
 *         }
 * }
 *
 * int main()
 * {
 *         // ...
 *
 *         mutex_t *mtx = mutex_new(MUTEX_NORMAL);
 *
 *         tid_t t1 = thread_create(thread1, NULL, mtx);
 *         tid_t t1 = thread_create(thread2, NULL, mtx);
 *
 *         thread_join(t1);
 *         thread_join(t2);
 *
 *         mutex_delete(mtx);
 *
 *         // ...
 * }
 */
//==============================================================================
static inline bool mutex_trylock(mutex_t *mutex)
{
        return mutex_lock(mutex, 0);
}

//==============================================================================
/**
 * @brief bool mutex_unlock(mutex_t *mutex)
 * The function <b>mutex_unlock</b>() unlock mutex pointed by <i>mutex</i>.
 *
 * @param mutex     mutex
 *
 * @errors None
 *
 * @return If mutex is unlocked then <b>true</b> is returned. If mutex is not
 * unlocked or object is incorrect then <b>false</b> is returned.
 *
 * @example
 * #include <dnx/thread.h>
 *
 * int resource;
 *
 * void thread1(void *arg)
 * {
 *         mutex_t *mtx = arg;
 *
 *         // protected access to resource
 *         if (mutex_lock(mtx, MAX_DELAY_MS)) {
 *                 // write to buffer is allowed
 *                 resource = ...;
 *
 *                 // ...
 *
 *                 mutex_unlock(mtx);
 *         }
 * }
 *
 * void thread2(void *arg)
 * {
 *         mutex_t *mtx = arg;
 *
 *         // protected access to resource
 *         if (mutex_lock(mtx, MAX_DELAY_MS)) {
 *                 // write to buffer is allowed
 *                 resource = ...;
 *
 *                 // ...
 *
 *                 mutex_unlock(mtx);
 *         }
 * }
 *
 * int main()
 * {
 *         // ...
 *
 *         mutex_t *mtx = mutex_new(MUTEX_NORMAL);
 *
 *         thread_new(thread1, STACK_DEPTH_LOW, mtx);
 *         thread_new(thread2, STACK_DEPTH_LOW, mtx);
 *
 *         thread_join(thread1);
 *         thread_join(thread2);
 *
 *         mutex_delete(mtx);
 *
 *         // ...
 * }
 */
//==============================================================================
static inline bool mutex_unlock(mutex_t *mutex)
{
        _errno = _builtinfunc(mutex_unlock, mutex);
        return !_errno;
}

//==============================================================================
/**
 * @brief queue_t *queue_new(const uint length, const uint item_size)
 * The function <b>queue_new</b>() create new queue with length <i>length</i>
 * of item size <i>item_size</i>. Returns pointer to the created object or
 * <b>NULL</b> on error. Both, <i>length</i> and <i>item_size</i> cannot be zero.
 *
 * @param length        queue length
 * @param item_size     size of item
 *
 * @errors None
 *
 * @return On success returns pointer to the created object or <b>NULL</b> on
 * error.
 *
 * @example
 * #include <dnx/thread.h>
 *
 * void thread1(void *arg)
 * {
 *         queue_t *queue = arg;
 *
 *         char c = '1';
 *         queue_send(queue, &c, MAX_DELAY_MS);
 *
 *         // some operations
 * }
 *
 * void thread2(void *arg)
 * {
 *         queue_t *queue = arg;
 *
 *         char c;
 *
 *         queue_receive(queue, &c, MAX_DELAY_MS);
 *
 *         if (c == '1') {
 *                 // ...
 *         } else {
 *                 // ...
 *         }
 *
 *         // some operations
 * }
 *
 * int main()
 * {
 *         // ...
 *
 *         queue_t *queue = queue_new(10, sizeof(char));
 *
 *         thread_new(thread1, STACK_DEPTH_LOW, queue);
 *         thread_new(thread2, STACK_DEPTH_LOW, queue);
 *
 *         thread_join(thread1);
 *         thread_join(thread2);
 *
 *         queue_delete(queue);
 *
 *         // ...
 * }
 */
//==============================================================================
static inline queue_t *queue_new(const size_t length, const size_t item_size)
{
        queue_t *queue = NULL;
        syscall(SYSCALL_QUEUECREATE, &queue, &length, &item_size);
        return queue;
}

//==============================================================================
/**
 * @brief void queue_delete(queue_t *queue)
 * The function <b>queue_delete</b>() deletes the created queue pointed by
 * <i>queue</i>. Make sure that neither task use queue before delete.
 *
 * @param queue     queue object
 *
 * @errors None
 *
 * @return None
 *
 * @example
 * #include <dnx/thread.h>
 *
 * void thread1(void *arg)
 * {
 *         queue_t *queue = arg;
 *
 *         char c = '1';
 *         queue_send(queue, &c, MAX_DELAY_MS);
 *
 *         // some operations
 * }
 *
 * void thread2(void *arg)
 * {
 *         queue_t *queue = arg;
 *
 *         char c;
 *
 *         queue_receive(queue, &c, MAX_DELAY_MS);
 *
 *         if (c == '1') {
 *                 // ...
 *         } else {
 *                 // ...
 *         }
 *
 *         // some operations
 * }
 *
 * int main()
 * {
 *         // ...
 *
 *         queue_t *queue = queue_new(10, sizeof(char));
 *
 *         thread_new(thread1, STACK_DEPTH_LOW, queue);
 *         thread_new(thread2, STACK_DEPTH_LOW, queue);
 *
 *         thread_join(thread1);
 *         thread_join(thread2);
 *
 *         queue_delete(queue);
 *
 *         // ...
 * }
 */
//==============================================================================
static inline void queue_delete(queue_t *queue)
{
        syscall(SYSCALL_QUEUEDESTROY, NULL, queue);
}

//==============================================================================
/**
 * @brief bool queue_reset(queue_t *queue)
 * The function <b>queue_reset</b>() reset the selected queue pointed by
 * <i>queue</i>.
 *
 * @param queue     queue object
 *
 * @errors None
 *
 * @return On success true is returned, otherwise false.
 *
 * @example
 * #include <dnx/thread.h>
 *
 * void thread1(void *arg)
 * {
 *         queue_t *queue = arg;
 *
 *         queue_reset(queue);
 *
 *         char c = '1';
 *         queue_send(queue, &c, MAX_DELAY_MS);
 *
 *         // some operations
 * }
 *
 * void thread2(void *arg)
 * {
 *         queue_t *queue = arg;
 *
 *         char c;
 *
 *         queue_receive(queue, &c, MAX_DELAY_MS);
 *
 *         if (c == '1') {
 *                 // ...
 *         } else {
 *                 // ...
 *         }
 *
 *         // some operations
 * }
 *
 * int main()
 * {
 *         // ...
 *
 *         queue_t *queue = queue_new(10, sizeof(char));
 *
 *         thread_new(thread1, STACK_DEPTH_LOW, queue);
 *         thread_new(thread2, STACK_DEPTH_LOW, queue);
 *
 *         thread_join(thread1);
 *         thread_join(thread2);
 *
 *         queue_delete(queue);
 *
 *         // ...
 * }
 */
//==============================================================================
static inline bool queue_reset(queue_t *queue)
{
        _errno = _builtinfunc(queue_reset, queue);
        return !_errno;
}

//==============================================================================
/**
 * @brief bool queue_send(queue_t *queue, const void *item, const uint timeout)
 * The function <b>queue_send</b>() send specified item pointed by <i>item</i>
 * to queue pointed by <i>queue</i>. If queue is full then system try to send
 * item for <i>timeout</i> milliseconds. If <i>timeout</i> is set to zero then
 * sending is aborted immediately if queue is full, and <b>false</b> value is
 * returned.
 *
 * @param queue     queue object
 * @param item      item to send
 * @param timeout   send timeout (0 for polling)
 *
 * @errors None
 *
 * @return On success, <b>true</b> is returned. On error, <b>false</b> is returned.
 *
 * @example
 * #include <dnx/thread.h>
 *
 * void thread1(void *arg)
 * {
 *         queue_t *queue = arg;
 *
 *         char c = '1';
 *         queue_send(queue, &c, MAX_DELAY_MS);
 *
 *         // some operations
 * }
 *
 * void thread2(void *arg)
 * {
 *         queue_t *queue = arg;
 *
 *         char c;
 *
 *         queue_receive(queue, &c, MAX_DELAY_MS);
 *
 *         if (c == '1') {
 *                 // ...
 *         } else {
 *                 // ...
 *         }
 *
 *         // some operations
 * }
 *
 * int main()
 * {
 *         // ...
 *
 *         queue_t *queue = queue_new(10, sizeof(char));
 *
 *         thread_new(thread1, STACK_DEPTH_LOW, queue);
 *         thread_new(thread2, STACK_DEPTH_LOW, queue);
 *
 *         thread_join(thread1);
 *         thread_join(thread2);
 *
 *         queue_delete(queue);
 *
 *         // ...
 * }
 */
//==============================================================================
static inline bool queue_send(queue_t *queue, const void *item, const uint timeout)
{
        _errno = _builtinfunc(queue_send, queue, item, timeout);
        return !_errno;
}

//==============================================================================
/**
 * @brief bool queue_receive(queue_t *queue, void *item, const uint timeout)
 * The function <b>queue_receive</b>() receive top item from queue pointed by
 * <i>queue</i> and copy it to the item pointed by <i>item</i>. The item is
 * removed from queue. Try of receive is doing for time <i>timeout</i>. If item
 * was successfully received, then <b>true</b> is returned, otherwise <b>false</b>.
 *
 * @param queue     queue object
 * @param item      item destination
 * @param timeout   send timeout (0 for polling)
 *
 * @errors None
 *
 * @return On success, <b>true</b> is returned. On error, <b>false</b> is returned.
 *
 * @example
 * #include <dnx/thread.h>
 *
 * void thread1(void *arg)
 * {
 *         queue_t *queue = arg;
 *
 *         char c = '1';
 *         queue_send(queue, &c, MAX_DELAY_MS);
 *
 *         // some operations
 * }
 *
 * void thread2(void *arg)
 * {
 *         queue_t *queue = arg;
 *
 *         char c;
 *
 *         queue_receive(queue, &c, MAX_DELAY_MS);
 *
 *         if (c == '1') {
 *                 // ...
 *         } else {
 *                 // ...
 *         }
 *
 *         // some operations
 * }
 *
 * int main()
 * {
 *         // ...
 *
 *         queue_t *queue = queue_new(10, sizeof(char));
 *
 *         thread_new(thread1, STACK_DEPTH_LOW, queue);
 *         thread_new(thread2, STACK_DEPTH_LOW, queue);
 *
 *         thread_join(thread1);
 *         thread_join(thread2);
 *
 *         queue_delete(queue);
 *
 *         // ...
 * }
 */
//==============================================================================
static inline bool queue_receive(queue_t *queue, void *item, const uint timeout)
{
        _errno = _builtinfunc(queue_receive, queue, item, timeout);
        return !_errno;
}

//==============================================================================
/**
 * @brief bool queue_receive_peek(queue_t *queue, void *item, const uint timeout)
 * The function <b>queue_receive_peek</b>() is similar to <b>queue_receive</b>(),
 * expect that top item is not removed from the queue.
 *
 * @param queue     queue object
 * @param item      item destination
 * @param timeout   send timeout (0 for polling)
 *
 * @errors None
 *
 * @return On success, <b>true</b> is returned. On error, <b>false</b> is returned.
 *
 * @example
 * #include <dnx/thread.h>
 *
 * void thread1(void *arg)
 * {
 *         queue_t *queue = arg;
 *
 *         char c = '1';
 *         queue_send(queue, &c, MAX_DELAY_MS);
 *
 *         // some operations
 * }
 *
 * void thread2(void *arg)
 * {
 *         queue_t *queue = arg;
 *
 *         char c;
 *
 *         queue_receive_peek(queue, &c, MAX_DELAY_MS);
 *
 *         if (c == '1') {
 *                 // ...
 *         } else {
 *                 // ...
 *         }
 *
 *         // ...
 *
 *         queue_receive(queue, &c, MAX_DELAY_MS);
 *
 *         // some operations
 * }
 *
 * int main()
 * {
 *         // ...
 *
 *         queue_t *queue = queue_new(10, sizeof(char));
 *
 *         thread_new(thread1, STACK_DEPTH_LOW, queue);
 *         thread_new(thread2, STACK_DEPTH_LOW, queue);
 *
 *         thread_join(thread1);
 *         thread_join(thread2);
 *
 *         queue_delete(queue);
 *
 *         // ...
 * }
 */
//==============================================================================
static inline bool queue_receive_peek(queue_t *queue, void *item, const uint timeout)
{
        _errno = _builtinfunc(queue_receive_peek, queue, item, timeout);
        return !_errno;
}

//==============================================================================
/**
 * @brief int queue_get_number_of_items(queue_t *queue)
 * The function <b>queue_get_number_of_items</b>() returns a number of items
 * stored in the queue pointed by <i>queue</i>.
 *
 * @param queue     queue object
 *
 * @errors None
 *
 * @return Number of items stored in the queue. On error, -1 is returned.
 *
 * @example
 * #include <dnx/thread.h>
 *
 * void thread1(void *arg)
 * {
 *         queue_t *queue = arg;
 *
 *         char c = '1';
 *         queue_send(queue, &c, MAX_DELAY_MS);
 *
 *         // some operations
 * }
 *
 * void thread2(void *arg)
 * {
 *         queue_t *queue = arg;
 *
 *         for (i = 0; i < queue_get_number_of_items(queue); i++) {
 *                 char c;
 *                 queue_receive(queue, &c, MAX_DELAY_MS);
 *
 *                 // some operations
 *         }
 *
 *         // ...
 * }
 *
 * int main()
 * {
 *         // ...
 *
 *         queue_t *queue = queue_new(10, sizeof(char));
 *
 *         thread_new(thread1, STACK_DEPTH_LOW, queue);
 *         thread_new(thread2, STACK_DEPTH_LOW, queue);
 *
 *         thread_join(thread1);
 *         thread_join(thread2);
 *
 *         queue_delete(queue);
 *
 *         // ...
 * }
 */
//==============================================================================
static inline int queue_get_number_of_items(queue_t *queue)
{
        size_t len = -1;
        _errno = _builtinfunc(queue_get_number_of_items, queue, &len);
        return len;
}

//==============================================================================
/**
 * @brief int queue_get_space_available(queue_t *queue)
 * The function <b>queue_get_space_available</b>() returns a number of free
 * items available in the queue pointed by <i>queue</i>.
 *
 * @param queue     queue object
 *
 * @errors None
 *
 * @return Number of free items available in the queue. On error, -1 is returned.
 *
 * @example
 * #include <dnx/thread.h>
 *
 * // ...
 *
 * queue_t *queue = queue_new(10, sizeof(int));
 *
 * // ...
 *
 * if (queue_get_space_available(queue) > 0) {
 *         // ...
 * } else {
 *         // ...
 * }
 *
 * // ...
 */
//==============================================================================
static inline int queue_get_space_available(queue_t *queue)
{
        size_t space = -1;
        _errno = _builtinfunc(queue_get_space_available, queue, &space);
        return space;
}

#ifdef __cplusplus
}
#endif

#endif /* _THREAD_H_ */
/*==============================================================================
  End of file
==============================================================================*/
