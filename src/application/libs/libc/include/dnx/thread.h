/*=========================================================================*//**
@file    thread.h

@author  Daniel Zorychta

@brief   Library with functions which operate on threads.

@note    Copyright (C) 2013 Daniel Zorychta <daniel.zorychta@gmail.com>

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

/**
\defgroup dnx-thread-h <dnx/thread.h>

The library is used to control thread functionality. There are functions that
creates mutex, semaphore, queue, and other objects that are related with
threads.

*/
/**@{*/

#ifndef _LIBC_DNX_THREAD_H_
#define _LIBC_DNX_THREAD_H_

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Include files
==============================================================================*/
#include <libc/source/syscall.h>
#include <libc/include/sys/types.h>
#include <stdio.h>
#include <errno.h>
#include <stdbool.h>

/*==============================================================================
  Exported macros
==============================================================================*/
#ifdef DOXYGEN
/**
 * @brief Minimal stack size
 *
 * This size of stack allows to start very minimalistic application. This stack
 * size is a reference of bigger stack sizes and is configured in system
 * configuration.
 */
#define STACK_DEPTH_MINIMAL

/**
 * @brief Very low stack size
 */
#define STACK_DEPTH_VERY_LOW

/**
 * @brief Low stack size
 */
#define STACK_DEPTH_LOW

/**
 * @brief Medium stack size
 */
#define STACK_DEPTH_MEDIUM

/**
 * @brief Large stack size
 */
#define STACK_DEPTH_LARGE

/**
 * @brief Very large stack size
 */
#define STACK_DEPTH_VERY_LARGE

/**
 * @brief Huge stack size
 */
#define STACK_DEPTH_HUGE

/**
 * @brief Very huge stack size
 */
#define STACK_DEPTH_VERY_HUGE

/**
 * @brief User defined stack size
 *
 * By using this macro, user can determine the size of stack. This macro gives
 * user full control of stack size.
 *
 * @param depth         stack depth in levels
 */
#define STACK_DEPTH_CUSTOM(depth)
#endif

/*==============================================================================
  Exported object types
==============================================================================*/
/**
 * @brief Semaphore object
 *
 * The type represent semaphore object. Fields are private.
 */
typedef void sem_t;

/**
 * @brief Mutex object
 *
 * The type represent mutex object. Fields are private.
 */
typedef struct _lib_mutex mutex_t;

/**
 * @brief Mutex type
 *
 * The enumerator represent type of mutex object.
 */
enum mutex_type {
        MUTEX_TYPE_RECURSIVE,
        MUTEX_TYPE_NORMAL
};

/**
 * @brief Queue object
 *
 * The type represent queue object. Fields are private.
 */
typedef void queue_t;

/**
 * @brief Thread function pointer
 */
typedef int (*thread_func_t)(void *arg);

/**
 * @brief Process attributes
 *
 * The type is used to configure process settings.
 */
typedef struct {
        void       *f_stdin;            //!< stdin  file object pointer (major)
        void       *f_stdout;           //!< stdout file object pointer (major)
        void       *f_stderr;           //!< stderr file object pointer (major)
        const char *p_stdin;            //!< stdin  file path (minor)
        const char *p_stdout;           //!< stdout file path (minor)
        const char *p_stderr;           //!< stderr file path (minor)
        const char *cwd;                //!< working directory path
        i16_t       priority;           //!< process priority
        bool        detached;           //!< independent process (no parent)
} process_attr_t;

/**
 * @brief Process statistics container.
 *
 * The type represent process statistics.
 */
typedef struct {
        const char *name;               //!< process name
        pid_t       pid;                //!< process ID
        size_t      memory_usage;       //!< memory usage (allocated by process)
        u16_t       memory_block_count; //!< number of used memory blocks
        u16_t       files_count;        //!< number of opened files
        u16_t       dir_count;          //!< number of opened directories
        u16_t       mutexes_count;      //!< number of used mutexes
        u16_t       semaphores_count;   //!< number of used semaphores
        u16_t       queue_count;        //!< number of used queues
        u16_t       socket_count;       //!< number of used sockets
        u16_t       threads_count;      //!< number of threads
        u16_t       CPU_load;           //!< CPU load (1% = 10)
        u16_t       stack_size;         //!< stack size
        u16_t       stack_max_usage;    //!< max stack usage
        i16_t       priority;           //!< priority
        u16_t       syscalls_per_sec;   //!< syscalls per second
} process_stat_t;

/**
 * @brief Thread statistics type
 *
 * The type represent thread attributes that configures thread settings.
 */
typedef struct {
        tid_t       tid;                //!< thread ID
        u16_t       CPU_load;           //!< CPU load (1% = 10)
        u16_t       stack_size;         //!< stack size
        u16_t       stack_max_usage;    //!< max stack usage
        i16_t       priority;           //!< priority
        u16_t       syscalls_per_sec;   //!< syscalls per second
} thread_stat_t;

/**
 * @brief Thread attributes type
 *
 * The type represent thread attributes that configures thread settings.
 */
typedef struct {
        size_t stack_depth;             //!< stack depth
        i16_t  priority;                //!< thread priority
        bool   detached;                //!< independent thread (without join possibility)
} thread_attr_t;

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
 * @brief Function creates new process.
 *
 * The function process_create() create new process according to command
 * pointed by <i>cmd</i> and attributes pointed by <i>attr</i>. Attributes
 * can be NULL what means that default setting will be applied. Command field
 * is mandatory.
 *
 * @param cmd           program name and argument list
 * @param attr          process attributes
 *
 * @exception | @ref ENOMEM
 * @exception | @ref EINVAL
 * @exception | @ref ENOENT
 *
 * @return On success return process ID (PID), otherwise 0.
 *
 * @b Example
 * @code
        #include <dnx/thread.h>
        #include <errno.h>

        // ...

        errno = 0;

        static const process_attr_t attr = {
                .f_stdin   = stdin,
                .f_stdout  = stdout,
                .f_stderr  = stderr,
                .p_stdin   = NULL,
                .p_stdout  = NULL,
                .p_stderr  = NULL,
                .detached  = false
        }

        pid_t pid = process_create("ls /", &attr);
        if (pid) {
                int exit_code = 0;
                process_wait(pid, &exit_code, MAX_DELAY_MS);
        } else {
                perror("Program not started");

                // ...
        }

        // ...

   @endcode
 *
 * @see process_kill()
 */
//==============================================================================
static inline pid_t process_create(const char *cmd, const process_attr_t *attr)
{
        pid_t pid = 0;
        _libc_syscall(_LIBC_SYS_PROCESSCREATE, &pid, cmd, attr);
        return pid;
}

//==============================================================================
/**
 * @brief Function kill selected process.
 *
 * The function process_kill() delete running process. Process status is not
 * read because is not set.
 *
 * @param pid                   process ID
 *
 * @exception | @ref EINVAL
 * @exception | @ref EAGAIN
 *
 * @return Return 0 on success. On error, -1 is returned, and
 * <b>errno</b> is set appropriately.
 *
 * @b Example
 * @code
        #include <dnx/thread.h>

        // ...

        errno = 0;

        static const process_attr_t attr = {
                .f_stdin   = stdin,
                .f_stdout  = stdout,
                .f_stderr  = stderr,
                .p_stdin   = NULL,
                .p_stdout  = NULL,
                .p_stderr  = NULL,
                .detached  = true
        }

        pid_t pid = process_create("cat", &attr);
        if (pid) {
                sleep(1); // child execute time
                process_kill(pid);
        } else {
                perror("Program not started");

                // ...
        }

        // ...

   @endcode
 *
 * @see process_create()
 */
//==============================================================================
static inline int process_kill(pid_t pid)
{
        int r = -1;
        _libc_syscall(_LIBC_SYS_PROCESSKILL, &r, &pid);
        return r;
}

//==============================================================================
/**
 * @brief Function wait for selected process close.
 *
 * The function process_wait() wait for process close. Function destroy
 * child process when finish successfully at the selected timeout. In case of
 * timeout the process is not destroyed.
 *
 * @param pid                   process ID
 * @param status                child process exit status (it can be NULL)
 * @param timeout               wait timeout in ms
 *
 * @exception | @ref EINVAL
 * @exception | @ref ETIME
 * @exception | ...
 *
 * @return Return 0 on success. On error -1 is returned, and
 * <b>errno</b> is set appropriately.
 *
 * @b Example
 * @code
        #include <dnx/os.h>

        // ...

        errno = 0;

        static const process_attr_t attr = {
                .f_stdin   = stdin,
                .f_stdout  = stdout,
                .f_stderr  = stderr,
                .p_stdin   = NULL,
                .p_stdout  = NULL,
                .p_stderr  = NULL,
                .detached  = false
        }

        int   status = -1;
        pid_t pid    = process_create("ls /", &attr);
        if (pid) {
                process_wait(pid, &status, MAX_DELAY_MS);
        } else {
                perror("Program not started");

                // ...
        }

        // ...

   @endcode
 *
 * @see wait()
 */
//==============================================================================
static inline int process_wait(pid_t pid, int *status, const u32_t timeout)
{
        int r = -1;
        _libc_syscall(_LIBC_SYS_PROCESSWAIT, &r, &pid, status, &timeout);
        return r;
}

//==============================================================================
/**
 * @brief Function wait for selected process close.
 *
 * The function wait() wait for process close.
 *
 * @param pid                   process ID
 *
 * @exception | @ref EINVAL
 * @exception | @ref ETIME
 * @exception | ...
 *
 * @return Return process status or -1 on error.
 *
 * @b Example
 * @code
        #include <dnx/os.h>

        // ...

        errno = 0;

        static const process_attr_t attr = {
                .f_stdin   = stdin,
                .f_stdout  = stdout,
                .f_stderr  = stderr,
                .p_stdin   = NULL,
                .p_stdout  = NULL,
                .p_stderr  = NULL,
                .detached  = false
        }

        int   status = -1;
        pid_t pid    = process_create("ls /", &attr);
        if (pid) {
                status = wait(pid);
        } else {
                perror("Program not started");

                // ...
        }

        // ...

   @endcode
 *
 * @see process_wait()
 */
//==============================================================================
static inline int wait(pid_t pid)
{
        int status = -1;
        process_wait(pid, &status, MAX_DELAY_MS);
        return status;
}

//==============================================================================
/**
 * @brief Function returns statistics of selected process.
 *
 * The function process_stat_seek() return statistics of process selected
 * by <i>seek</i>.
 *
 * @param seek      process index
 * @param stat      statistics
 *
 * @exception | @ref EINVAL
 * @exception | @ref ENOENT
 *
 * @return Return 0 on success. On error, -1 is returned, and
 * <b>errno</b> is set appropriately.
 *
 * @b Example
 * @code
        #include <dnx/thread.h>

        // ...

        process_stat_t stat;
        size_t         seek = 0;
        while (process_stat_seek(seek++, &stat) == 0) {
                printf("Memory usage: %d\n", stat.memory_usage);
        }

        // ...

   @endcode
 *
 * @see process_stat()
 */
//==============================================================================
static inline int process_stat_seek(size_t seek, process_stat_t *stat)
{
        int r = -1;
        _libc_syscall(_LIBC_SYS_PROCESSSTATSEEK, &r, &seek, stat);
        return r;
}

//==============================================================================
/**
 * @brief Function returns statistics of selected process.
 *
 * The function process_stat() return statistics of selected process
 * by <i>pid</i>.
 *
 * @param pid       PID
 * @param stat      statistics
 *
 * @exception | @ref EINVAL
 * @exception | @ref ENOENT
 *
 * @return Return 0 on success. On error, -1 is returned.
 *
 * @b Example
 * @code
        #include <dnx/thread.h>
        #include <unistd.h>

        // ...

        process_stat_t stat;
        pid_t          pid = getpid(); // or process_getpid()
        process_stat(pid, &stat);
        printf("Memory usage of this process: %d\n", stat.memory_usage);

        // ...

   @endcode
 *
 * @see process_stat_seek()
 */
//==============================================================================
static inline int process_stat(pid_t pid, process_stat_t *stat)
{
        int r = -1;
        _libc_syscall(_LIBC_SYS_PROCESSSTATPID, &r, &pid, stat);
        return r;
}

//==============================================================================
/**
 * @brief Function returns PID of current process.
 *
 * The function process_getpid() return PID of current process (caller).
 *
 * @exception | @ref EINVAL
 * @exception | @ref ENOENT
 *
 * @return Return PID on success. On error, 0 is returned.
 *
 * @b Example
 * @code
        #include <dnx/thread.h>

        // ...

        pid_t pid = process_getpid();
        printf("PID of this process is: %d\n, (uint)pid);

        // ...

   @endcode
 *
 * @see getpid()
 */
//==============================================================================
static inline pid_t process_getpid(void)
{
        pid_t pid = 0;
        _libc_syscall(_LIBC_SYS_PROCESSGETPID, &pid);
        return pid;
}

//==============================================================================
/**
 * @brief Function returns priority of current process.
 *
 * The function process_get_priority() returns priority value of selected
 * process.
 *
 * @param pid           process ID
 *
 * @return Priority value.
 *
 * @b Example
 * @code
        #include <dnx/thread.h>
        #include <stdbool.h>

        // ...

        print("Process priority is: %d\n, (int)process_get_priority(getpid()));

        //...

   @endcode
 */
//==============================================================================
static inline int process_get_priority(pid_t pid)
{
        int prio = 0;
        _libc_syscall(_LIBC_SYS_PROCESSGETPRIO, &prio, &pid);
        return prio;
}

//==============================================================================
/**
 * @brief Function creates new thread of current process.
 *
 * The function thread_create() creates new thread using function pointed
 * by <i>func</i> with attributes pointed by <i>attr</i> and argument pointed
 * by <i>arg</i>. Threads are functions which are called as
 * new task and have own stack, but global variables are shared with main
 * thread.
 *
 * @param func          thread function
 * @param attr          thread attributes (can be NULL)
 * @param arg           thread argument (can be NULL)
 *
 * @exception | @ref EINVAL
 * @exception | @ref ENOMEM
 *
 * @return On success return thread ID (TID), otherwise 0,
 * and <b>errno</b> is set appropriately.
 *
 * @b Example
 * @code
        #include <dnx/thread.h>
        #include <unistd.h>

        // ...

        int thread(void *arg)
        {
                // ...

                // thread function exit without any function,
                // or just by return
                return 0;
        }

        void some_function()
        {
                errno = 0;

                const thread_attr_t attr = {
                        .stack_depth = STACK_DEPTH_LOW,
                        .priority    = PRIORITY_NORMAL,
                        .detached    = false
                }

                tid_t tid = thread_create(thread, &attr, NULL);
                if (tid) {
                        printf("Thread %d created\n", (int)tid);
                } else {
                        perror("Thread not created");
                }

                // ...
        }

        // ...

   @endcode
 *
 * @see thread_cancel(), thread_join()
 */
//==============================================================================
static inline tid_t thread_create(thread_func_t func, const thread_attr_t *attr, void *arg)
{
        tid_t tid = 0;
        _libc_syscall(_LIBC_SYS_THREADCREATE, &tid, func, attr, arg);
        return tid;
}

//==============================================================================
/**
 * @brief Function cancel selected thread.
 *
 * The function thread_cancel() kills running thread <i>tid</i>.
 *
 * @param tid           thread ID
 *
 * @exception | @ref EINVAL
 *
 * @return On success, 0 is returned. On error, -1 is returned, and <b>errno</b>
 * is set appropriately.
 *
 * @b Example
 * @code
        #include <dnx/thread.h>
        #include <unistd.h>

        // ...

        int thread(void *arg)
        {
                // ...

                // thread function exit without any function,
                // or just by return
                return 0;
        }

        void some_function()
        {
                errno = 0;

                tid_t tid = thread_create(thread, NULL, (void*)0);
                if (tid) {
                        printf("Thread %d created\n", (int)tid);
                        sleep(1);

                        if (thread_cancel(tid) == 0) {
                                puts("Thread killed");
                        } else {
                                perror("Thread cancel error");
                        }

                } else {
                        perror("Thread not created");
                }
        }

        // ...

   @endcode
 *
 * @see thread_create(), thread_join(), thread_exit()
 */
//==============================================================================
static inline int thread_cancel(tid_t tid)
{
        int r = -1;
        _libc_syscall(_LIBC_SYS_THREADKILL, &r, &tid);
        return r;
}

//==============================================================================
/**
 * @brief Function return current thread id.
 *
 * @exception | @ref EINVAL
 *
 * @return On success, thread ID is returned. On error, -1 is returned, and <b>errno</b>
 * is set appropriately.
 *
 * @b Example
 * @code
        #include <dnx/thread.h>
        #include <unistd.h>

        // ...

        int thread(void *arg)
        {
                // ...

                printf("Thread: %d\n", thread_current());

                // thread function exit without any function,
                // or just by return
                return 0;
        }

        void some_function()
        {
                errno = 0;

                tid_t tid = thread_create(thread, NULL, (void*)0);
                if (tid) {
                        printf("Thread %d created\n", (int)tid);
                        sleep(1);

                        if (thread_cancel(tid) == 0) {
                                puts("Thread killed");
                        } else {
                                perror("Thread cancel error");
                        }

                } else {
                        perror("Thread not created");
                }
        }

        // ...

   @endcode
 *
 * @see thread_create(), thread_join(), thread_exit()
 */
//==============================================================================
static inline int thread_current(void)
{
        int r = -1;
        _libc_syscall(_LIBC_SYS_GETACTIVETHREAD, &r);
        return r;
}

//==============================================================================
/**
 * @brief  The thread_exit function terminates execution of the calling thread.
 * Function do not release resources allocated by thread.
 *
 * @param status        thread exit status
 *
 * @b Example
 * @code
        #include <dnx/thread.h>
        #include <unistd.h>

        // ...

        int thread(void *arg)
        {
                // ...

                // thread function exit without any function,
                // or just by return

                if (fail) {
                        thread_exit(-1);
                        // exit here
                }

                // ...
                return 0;
        }

        void some_function()
        {
                errno = 0;

                tid_t tid = thread_create(thread, NULL, (void*)0);
                if (tid) {
                        printf("Thread %d created\n", (int)tid);
                } else {
                        perror("Thread not created");
                }
        }

        // ...

   @endcode
 *
 * @see thread_create(), thread_join(), thread_cancel()
 */
//==============================================================================
static inline void thread_exit(int status)
{
        _libc_syscall(_LIBC_SYS_THREADEXIT, NULL, &status);
}

//==============================================================================
/**
 * @brief Function join thread to main thread (wait for close by selected time).
 *
 * The function thread_join2() joins selected thread <i>tid</i> to parent
 * program. Function wait until thread was closed or timeout occurred.
 *
 * @param tid           thread ID
 * @param status        thread exit status
 * @param timeout_ms    timeout in milliseconds
 *
 * @exception | @ref EINVAL
 * @exception | @ref ETIME
 *
 * @return On success, 0 is returned. On error, -1 is returned, and <b>errno</b>
 * is set appropriately.
 *
 * @b Example
 * @code
        #include <dnx/thread.h>
        #include <unistd.h>

        // ...

        int thread(void *arg)
        {
                // ...

                // thread function exit without any function,
                // or just by return
                return 0;
        }

        void some_function()
        {
                errno = 0;

                tid_t tid = thread_create(thread, NULL, (void*)0);
                if (tid) {
                        printf("Thread %d created\n", (int)tid);
                        sleep(1);

                        int status;
                        if (thread_join2(tid, &status, MAX_DELAY_MS) == 0) {
                                puts("Joinded with thread");
                        } else {
                                perror("Thread join error");
                        }

                } else {
                        perror("Thread not created");
                }
        }

        // ...

   @endcode
 *
 * @see thread_create(), thread_cancel()
 */
//==============================================================================
static inline int thread_join2(tid_t tid, int *status, uint32_t timeout_ms)
{
        int r = -1;
        _libc_syscall(_LIBC_SYS_THREADJOIN, &r, &tid, status, &timeout_ms);
        return r;
}

//==============================================================================
/**
 * @brief Function join thread to main thread (wait for close).
 *
 * The function thread_join() joins selected thread <i>tid</i> to parent
 * program. Function wait until thread was closed.
 *
 * @param tid           thread ID
 * @param status        thread exit status
 *
 * @exception | @ref EINVAL
 * @exception | @ref ETIME
 *
 * @return On success, 0 is returned. On error, -1 is returned, and <b>errno</b>
 * is set appropriately.
 *
 * @b Example
 * @code
        #include <dnx/thread.h>
        #include <unistd.h>

        // ...

        int thread(void *arg)
        {
                // ...

                // thread function exit without any function,
                // or just by return
                return 0;
        }

        void some_function()
        {
                errno = 0;

                tid_t tid = thread_create(thread, NULL, (void*)0);
                if (tid) {
                        printf("Thread %d created\n", (int)tid);
                        sleep(1);

                        int status;
                        if (thread_join(tid, &status) == 0) {
                                puts("Joinded with thread");
                        } else {
                                perror("Thread join error");
                        }

                } else {
                        perror("Thread not created");
                }
        }

        // ...

   @endcode
 *
 * @see thread_create(), thread_cancel()
 */
//==============================================================================
static inline int thread_join(tid_t tid, int *status)
{
        return thread_join2(tid, status, MAX_DELAY_MS);
}

//==============================================================================
/**
 * @brief Function returns statistics of selected thread.
 *
 * The function thread_stat() return statistics of thread selected by <i>tid</i>.
 *
 * @param pid       process ID
 * @param tid       thread ID
 * @param stat      statistics
 *
 * @exception | @ref EINVAL
 * @exception | @ref ENOENT
 *
 * @return Return 0 on success. On error, -1 is returned, and
 * <b>errno</b> is set appropriately.
 *
 * @b Example
 * @code
        #include <dnx/thread.h>

        // ...

        thread_stat_t stat;

        if (thread_stat(getpid(), 0, &stat) == 0) {
                printf("CPU load: %d\n", stat.CPU_load);
        }

        // ...

   @endcode
 *
 * @see process_stat()
 */
//==============================================================================
static inline int thread_stat(pid_t pid, tid_t tid, thread_stat_t *stat)
{
        int r = -1;
        _libc_syscall(_LIBC_SYS_THREADSTAT, &r, &pid, &tid, stat);
        return r;
}

//==============================================================================
/**
 * @brief Function create new semaphore.
 *
 * The function semaphore_new() creates new semaphore object. The
 * semaphore can be counting or binary. If counting then <i>cnt_max</i>
 * is bigger that 2. The <i>cnt_init</i> is an initial value of semaphore.
 * Semaphore can be used for task synchronization.
 *
 * @param cnt_max       max count value (1 for binary)
 * @param cnt_init      initial value (0 or 1 for binary)
 *
 * @exception | @ref EINVAL
 * @exception | @ref ENOMEM
 * @exception | @ref ESRCH
 *
 * @return On success pointer to semaphore object is returned.
 * On error, <b>NULL</b> pointer is returned, and <b>errno</b>
 * is set appropriately.
 *
 * @b Example
 * @code
        #include <dnx/thread.h>
        #include <stdbool.h>
        #include <errno.h>
        #include <stdlib.h>

        // ...

        errno = 0;
        sem_t *sem = semaphore_new(1, 0); // binary semaphore
        if (sem == NULL) {
                perror("Semaphore error");
                abort();
        }

        // ...

        void thread2(void *arg)
        {
                while (true) {
                        // this task will wait for semaphore signal
                        semaphore_wait(sem, MAX_DELAY_MS);

                        // ...
                }
        }

        void thread1(void *arg)
        {
                while (true) {
                       // ...

                       // this task signal to thread2 that can execute part of code
                       semaphore_signal(sem);
                }
        }

        // ...

   @endcode
 *
 * @see semaphore_delete()
 */
//==============================================================================
static inline sem_t *semaphore_new(const size_t cnt_max, const size_t cnt_init)
{
        sem_t *sem = NULL;
        _libc_syscall(_LIBC_SYS_SEMAPHORECREATE, &sem, &cnt_max, &cnt_init);
        return sem;
}

//==============================================================================
/**
 * @brief Function delete created semaphore.
 *
 * The function semaphore_delete() removes created semaphore pointed by
 * <i>sem</i>. Be aware that if semaphore was removed when tasks use it, then
 * process starvation can occur on tasks which wait for semaphore signal.
 *
 * @param sem           semaphore object pointer
 *
 * @exception | @ref ESRCH
 * @exception | @ref ENOENT
 * @exception | @ref EFAULT
 *
 * @b Example
 * @code
        #include <dnx/thread.h>

        // ...

        sem_t *sem = semaphore_new(1, 0);

        // ...
        // operations on semaphore
        // ...

        semaphore_delete(sem);

        // ...

   @endcode
 *
 * @see semaphore_new()
 */
//==============================================================================
static inline void semaphore_delete(sem_t *sem)
{
        _libc_syscall(_LIBC_SYS_SEMAPHOREDESTROY, NULL, sem);
}

//==============================================================================
/**
 * @brief Function wait for semaphore.
 *
 * The function semaphore_wait() waits for semaphore signal pointed by
 * <i>sem</i> by <i>timeout</i> milliseconds. If semaphore was signaled then
 * <b>true</b> is returned, otherwise (timeout) <b>false</b>. When <i>timeout</i>
 * value is set to 0 then semaphore is polling without timeout.
 *
 * @param sem           semaphore object pointer
 * @param timeout       timeout value in milliseconds
 *
 * @exception | @ref EINVAL
 * @exception | @ref ETIME
 *
 * @return On success, <b>true</b> is returned. On timeout or if semaphore is
 * not signaled or object is invalid <b>false</b> is returned.
 *
 * @b Example
 * @code
        #include <dnx/thread.h>
        #include <stdbool.h>
        #include <errno.h>
        #include <stdlib.h>

        // ...

        errno = 0;
        sem_t *sem = semaphore_new(1, 0); // binary semaphore
        if (sem == NULL) {
                perror("Semaphore error");
                abort();
        }

        // ...

        void thread2(void *arg)
        {
                while (true) {
                        // this task will wait for semaphore signal
                        semaphore_wait(sem, MAX_DELAY_MS);

                        // ...
                }
        }

        void thread1(void *arg)
        {
                while (true) {
                       // ...

                       // this task signal to thread2 that can execute part of code
                       semaphore_signal(sem);
                }
        }

        // ...

   @endcode
 *
 * @see semaphore_signal()
 */
//==============================================================================
static inline bool semaphore_wait(sem_t *sem, const u32_t timeout)
{
        bool r = false;
        _libc_syscall(_LIBC_SYS_SEMAPHOREWAIT, &r, sem, &timeout);
        return r;
}

//==============================================================================
/**
 * @brief Function signal semaphore.
 *
 * The function semaphore_signal() signals semaphore pointed by <i>sem</i>.
 *
 * @param sem           semaphore object pointer
 *
 * @exception | @ref EBUSY
 * @exception | @ref EINVAL
 *
 * @return On corrected signaling, <b>true</b> is returned. If semaphore cannot
 * be signaled or object is invalid then <b>false</b> is returned.
 *
 * @b Example
 * @code
        #include <dnx/thread.h>
        #include <stdbool.h>

        // ...

        sem_t *sem = semaphore_new(1, 0);

        // ...

        void thread2(void *arg)
        {
                while (true) {
                        // this task will wait for semaphore signal
                        semaphore_wait(sem, MAX_DELAY_MS);

                        // ...
                }
        }

        void thread1(void *arg)
        {
                while (true) {
                       // ...

                       // this task signal to thread2 that can execute part of code
                       semaphore_signal(sem);
                }
        }

        // ...

   @endcode
 *
 * @see semaphore_wait()
 */
//==============================================================================
static inline bool semaphore_signal(sem_t *sem)
{
        bool r = false;
        _libc_syscall(_LIBC_SYS_SEMAPHORESIGNAL, &r, sem);
        return r;
}

//==============================================================================
/**
 * @brief Function get counter value of semaphore.
 *
 * The function get value of semaphore pointed by <i>sem</i>. The counter value
 * is modified by semaphore_wait() and semaphore_signal() family functions.
 *
 * @param sem           semaphore object pointer
 *
 * @exception | @ref EINVAL
 *
 * @return On success return counter value, on error -1 is returned.
 *
 * @b Example
 * @code
        // ...

        int value = semaphore_get_value(sem);
        if (value > 0) {
                // ...
        }

        // ...

   @endcode
 *
 * @see semaphore_signal(), semaphore_wait()
 */
//==============================================================================
static inline int semaphore_get_value(sem_t *sem)
{
        int r = -1;
        _libc_syscall(_LIBC_SYS_SEMAPHOREGETVALUE, &r, sem);
        return r;
}

//==============================================================================
/**
 * @brief Function creates new mutex object.
 *
 * The function mutex_new() creates new mutex of type <i>type</i>.
 * Two types of mutex can be created: @ref MUTEX_TYPE_RECURSIVE and
 * @ref MUTEX_TYPE_NORMAL.
 *
 * @param type          mutex type
 *
 * @exception | @ref EINVAL
 * @exception | @ref ENOMEM
 * @exception | @ref ESRCH
 *
 * @return On success, pointer to the mutex object is returned. On error,
 * <b>NULL</b> pointer is returned.
 *
 * @b Example
 * @code
        #include <dnx/thread.h>

        int resource;

        mutex_t *mtx = mutex_new(MUTEX_TYPE_NORMAL);

        void thread1(void *arg)
        {
                // protected access to resource
                if (mutex_lock(mtx, MAX_DELAY_MS)) {
                        // write to buffer is allowed
                        resource = ...;

                        // ...

                        mutex_unlock(mtx);
                }
        }

        void thread2(void *arg)
        {
                // protected access to resource
                if (mutex_lock(mtx, MAX_DELAY_MS)) {
                        // write to buffer is allowed
                        resource = ...;

                        // ...

                        mutex_unlock(mtx);
                }
        }

        // ...

   @endcode
 *
 * @see mutex_delete()
 */
//==============================================================================
extern mutex_t *mutex_new(enum mutex_type type);

//==============================================================================
/**
 * @brief Function delete mutex object.
 *
 * The function mutex_delete() delete created mutex pointed by <i>mutex</i>.
 *
 * @param mutex         mutex
 *
 * @exception | @ref ESRCH
 * @exception | @ref ENOENT
 * @exception | @ref EFAULT
 *
 * @b Example
 * @code
        #include <dnx/thread.h>

        int resource;

        mutex_t *mtx = mutex_new(MUTEX_TYPE_NORMAL);

        void thread1(void *arg)
        {
                // protected access to resource
                if (mutex_lock(mtx, MAX_DELAY_MS)) {
                        // write to buffer is allowed
                        resource = ...;

                        // ...

                        mutex_unlock(mtx);
                }
        }

        void thread2(void *arg)
        {
                // protected access to resource
                if (mutex_lock(mtx, MAX_DELAY_MS)) {
                        // write to buffer is allowed
                        resource = ...;

                        // ...

                        mutex_unlock(mtx);
                }
        }

        // ...
        // join threads...
        // ...

        mutex_delete(mtx);

   @endcode
 *
 * @see mutex_new()
 */
//==============================================================================
extern void mutex_delete(mutex_t *mutex);

//==============================================================================
/**
 * @brief Function lock mutex object for current thread.
 *
 * The function mutex_lock() lock mutex pointed by <i>mutex</i>. If
 * mutex is locked by other thread then system try to lock mutex by <i>timeout</i>
 * milliseconds. If mutex is recursive then task can lock mutex recursively, and
 * the same times shall be unlocked. If normal mutex is used then task can lock
 * mutex only one time (not recursively).
 *
 * @param mutex     mutex
 * @param timeout   timeout
 *
 * @exception | @ref EINVAL
 * @exception | @ref ETIME
 *
 * @return If mutex is locked then <b>true</b> is returned. If mutex is used or
 * timeout occur or object is incorrect, then <b>false</b> is returned.
 *
 * @b Example
 * @code
        #include <dnx/thread.h>

        int resource;

        mutex_t *mtx = mutex_new(MUTEX_TYPE_NORMAL);

        void thread1(void *arg)
        {
                // protected access to resource
                if (mutex_lock(mtx, MAX_DELAY_MS)) {
                        // write to buffer is allowed
                        resource = ...;

                        // ...

                        mutex_unlock(mtx);
                }
        }

        void thread2(void *arg)
        {
                // protected access to resource
                if (mutex_lock(mtx, MAX_DELAY_MS)) {
                        // write to buffer is allowed
                        resource = ...;

                        // ...

                        mutex_unlock(mtx);
                }
        }

        // ...

   @endcode
 *
 * @see mutex_unlock(), mutex_trylock()
 */
//==============================================================================
extern bool mutex_lock(mutex_t *mutex, const u32_t timeout);

//==============================================================================
/**
 * @brief Function try lock mutex object for current thread.
 *
 * The function mutex_trylock() locks mutex pointed by <i>mutex</i>.
 * If mutex is recursive then task can lock mutex recursively, and
 * the same times shall be unlocked. If normal mutex is used then task can lock
 * mutex only one time (not recursively). Function is equivalent to
 * mutex_lock(mtx, 0) call.
 *
 * @param mutex         mutex
 *
 * @exception | @ref EINVAL
 * @exception | @ref ETIME
 *
 * @return If mutex is locked then <b>true</b> is returned. If mutex is used
 * or object is incorrect, then <b>false</b> is returned.
 *
 * @b Example
 * @code
        #include <dnx/thread.h>
        #include <stdbool.h>

        int resource;

        mutex_t *mtx = mutex_new(MUTEX_TYPE_NORMAL);

        void thread1(void *arg)
        {
                // protected access to resource
                while (mutex_trylock(mtx) != true) {
                        // write to buffer is allowed
                        resource = ...;

                        // ...

                        mutex_unlock(mtx);
                        break;
                }
        }

        void thread2(void *arg)
        {
                // protected access to resource
                while (mutex_trylock(mtx) != true) {
                        // write to buffer is allowed
                        resource = ...;

                        // ...

                        mutex_unlock(mtx);
                        break;
                }
        }

        // ...

   @endcode
 *
 * @see mutex_unlock(), mutex_lock()
 */
//==============================================================================
static inline bool mutex_trylock(mutex_t *mutex)
{
        return mutex_lock(mutex, 0);
}

//==============================================================================
/**
 * @brief Function unlocks earlier locked object.
 *
 * The function mutex_unlock() unlock mutex pointed by <i>mutex</i>.
 *
 * @param mutex         mutex
 *
 * @exception | @ref EBUSY
 * @exception | @ref EINVAL
 *
 * @return If mutex is unlocked then <b>true</b> is returned. If mutex is not
 * unlocked or object is incorrect then <b>false</b> is returned.
 *
 * @b Example
 * @code
        #include <dnx/thread.h>

        int resource;

        mutex_t *mtx = mutex_new(MUTEX_TYPE_NORMAL);

        void thread1(void *arg)
        {
                // protected access to resource
                if (mutex_lock(mtx, MAX_DELAY_MS)) {
                        // write to buffer is allowed
                        resource = ...;

                        // ...

                        mutex_unlock(mtx);
                }
        }

        void thread2(void *arg)
        {
                // protected access to resource
                if (mutex_lock(mtx, MAX_DELAY_MS)) {
                        // write to buffer is allowed
                        resource = ...;

                        // ...

                        mutex_unlock(mtx);
                }
        }

        // ...

   @endcode
 *
 * @see mutex_lock(), mutex_trylock()
 */
//==============================================================================
extern bool mutex_unlock(mutex_t *mutex);

//==============================================================================
/**
 * @brief Function creates new queue object.
 *
 * The function queue_new() create new queue with length <i>length</i>
 * of item size <i>item_size</i>. Returns pointer to the created object or
 * <b>NULL</b> on error. Both, <i>length</i> and <i>item_size</i> cannot be zero.
 *
 * @param length        queue length
 * @param item_size     size of item
 *
 * @exception | @ref EINVAL
 * @exception | @ref ENOMEM
 * @exception | @ref ESRCH
 *
 * @return On success returns pointer to the created object or <b>NULL</b> on
 * error.
 *
 * @b Example
 * @code
        #include <dnx/thread.h>

        // ...

        queue_t *queue = queue_new(10, sizeof(char));

        // ...

        void thread1(void *arg)
        {
                char c = '1';
                queue_send(queue, &c, MAX_DELAY_MS);

                // additional operations
        }

        void thread2(void *arg)
        {
                char c;
                queue_receive(queue, &c, MAX_DELAY_MS);

                if (c == '1') {
                        // ...
                } else {
                        // ...
                }

                // additional operations
        }

        // ...
        // join threads...
        // ...

        queue_delete(queue);

        //...

   @endcode
 *
 * @see queue_delete()
 */
//==============================================================================
static inline queue_t *queue_new(const size_t length, const size_t item_size)
{
        queue_t *queue = NULL;
        _libc_syscall(_LIBC_SYS_QUEUECREATE, &queue, &length, &item_size);
        return queue;
}

//==============================================================================
/**
 * @brief Function deletes queue object.
 *
 * The function queue_delete() deletes the created queue pointed by
 * <i>queue</i>. Make sure that neither task use queue before delete.
 *
 * @param queue         queue object
 *
 * @exception | @ref ESRCH
 * @exception | @ref ENOENT
 * @exception | @ref EFAULT
 *
 * @b Example
 * @code
        #include <dnx/thread.h>

        // ...

        queue_t *queue = queue_new(10, sizeof(char));

        // ...

        void thread1(void *arg)
        {
                char c = '1';
                queue_send(queue, &c, MAX_DELAY_MS);

                // additional operations
        }

        void thread2(void *arg)
        {
                char c;
                queue_receive(queue, &c, MAX_DELAY_MS);

                if (c == '1') {
                        // ...
                } else {
                        // ...
                }

                // additional operations
        }

        // ...
        // join threads...
        // ...

        queue_delete(queue);

        //...

   @endcode
 *
 * @see queue_new()
 */
//==============================================================================
static inline void queue_delete(queue_t *queue)
{
        _libc_syscall(_LIBC_SYS_QUEUEDESTROY, NULL, queue);
}

//==============================================================================
/**
 * @brief Function removes all items from queue.
 *
 * The function queue_reset() reset the selected queue pointed by
 * <i>queue</i>.
 *
 * @param queue         queue object
 *
 * @exception | @ref EBUSY
 * @exception | @ref EINVAL
 *
 * @return On success true is returned, otherwise false.
 *
 * @b Example
 * @code
        #include <dnx/thread.h>

        // ...

        queue_t *queue = queue_new(10, sizeof(char));

        // ...

        void thread1(void *arg)
        {
                queue_reset(queue);

                char c = '1';
                queue_send(queue, &c, MAX_DELAY_MS);

                // additional operations
        }

        void thread2(void *arg)
        {
                char c;
                queue_receive(queue, &c, MAX_DELAY_MS);

                if (c == '1') {
                        // ...
                } else {
                        // ...
                }

                // additional operations
        }

        // ...
        // join threads...
        // ...

        queue_delete(queue);

        //...

   @endcode
 */
//==============================================================================
static inline bool queue_reset(queue_t *queue)
{
        bool r = false;
        _libc_syscall(_LIBC_SYS_QUEUERESET, &r, queue);
        return r;
}

//==============================================================================
/**
 * @brief Function writes value to queue.
 *
 * The function queue_send() send specified item pointed by <i>item</i>
 * to queue pointed by <i>queue</i>. If queue is full then system try to send
 * item for <i>timeout</i> milliseconds. If <i>timeout</i> is set to zero then
 * sending is aborted immediately if queue is full, and <b>false</b> value is
 * returned.
 *
 * @param queue     queue object
 * @param item      item to send
 * @param timeout   send timeout (0 for polling)
 *
 * @exception | @ref EINVAL
 * @exception | @ref ENOSPC
 *
 * @return On success, <b>true</b> is returned. On error, <b>false</b> is returned.
 *
 * @b Example
 * @code
        #include <dnx/thread.h>

        // ...

        queue_t *queue = queue_new(10, sizeof(char));

        // ...

        void thread1(void *arg)
        {
                char c = '1';
                queue_send(queue, &c, MAX_DELAY_MS);

                // additional operations
        }

        void thread2(void *arg)
        {
                char c;
                queue_receive(queue, &c, MAX_DELAY_MS);

                if (c == '1') {
                        // ...
                } else {
                        // ...
                }

                // additional operations
        }

        // ...
        // join threads...
        // ...

        queue_delete(queue);

        //...

   @endcode
 *
 * @see queue_receive(), queue_receive_peek()
 */
//==============================================================================
static inline bool queue_send(queue_t *queue, const void *item, const u32_t timeout)
{
        bool r = false;
        _libc_syscall(_LIBC_SYS_QUEUESEND, &r, queue, item, &timeout);
        return r;
}

//==============================================================================
/**
 * @brief Function receives item from queue.
 *
 * The function queue_receive() receive top item from queue pointed by
 * <i>queue</i> and copy it to the item pointed by <i>item</i>. The item is
 * removed from queue. Try of receive is doing for time <i>timeout</i>. If item
 * was successfully received, then <b>true</b> is returned, otherwise <b>false</b>.
 *
 * @param queue         queue object
 * @param item          item destination
 * @param timeout       send timeout (0 for polling)
 *
 * @exception | @ref EINVAL
 * @exception | @ref EAGAIN
 *
 * @return On success, <b>true</b> is returned. On error, <b>false</b> is returned.
 *
 * @b Example
 * @code
        #include <dnx/thread.h>

        // ...

        queue_t *queue = queue_new(10, sizeof(char));

        // ...

        void thread1(void *arg)
        {
                char c = '1';
                queue_send(queue, &c, MAX_DELAY_MS);

                // additional operations
        }

        void thread2(void *arg)
        {
                char c;
                queue_receive(queue, &c, MAX_DELAY_MS);

                if (c == '1') {
                        // ...
                } else {
                        // ...
                }

                // additional operations
        }

        // ...
        // join threads...
        // ...

        queue_delete(queue);

        //...

   @endcode
 *
 * @see queue_send(), queue_receive_peek()
 */
//==============================================================================
static inline bool queue_receive(queue_t *queue, void *item, const u32_t timeout)
{
        bool r = false;
        _libc_syscall(_LIBC_SYS_QUEUERECEIVE, &r, queue, item, &timeout);
        return r;
}

//==============================================================================
/**
 * @brief Function receives item from queue without remove.
 *
 * The function queue_receive_peek() is similar to queue_receive(),
 * expect that top item is not removed from the queue.
 *
 * @param queue         queue object
 * @param item          item destination
 * @param timeout       send timeout (0 for polling)
 *
 * @exception | @ref EINVAL
 * @exception | @ref EAGAIN
 *
 * @return On success, <b>true</b> is returned. On error, <b>false</b> is returned.
 *
 * @b Example
 * @code
        #include <dnx/thread.h>

        // ...

        queue_t *queue = queue_new(10, sizeof(char));

        // ...

        void thread1(void *arg)
        {
                char c = '1';
                queue_send(queue, &c, MAX_DELAY_MS);

                // additional operations
        }

        void thread2(void *arg)
        {
                char c;
                queue_receive_peek(queue, &c, MAX_DELAY_MS);

                if (c == '1') {
                        queue_receive(queue, &c, MAX_DELAY_MS);
                        // ...
                } else {
                        // ...
                }

                // additional operations
        }

        // ...
        // join threads...
        // ...

        queue_delete(queue);

        //...

   @endcode
 *
 * @see queue_send(), queue_receive()
 */
//==============================================================================
static inline bool queue_receive_peek(queue_t *queue, void *item, const u32_t timeout)
{
        bool r = false;
        _libc_syscall(_LIBC_SYS_QUEUERECEIVEPEEK, &r, queue, item, &timeout);
        return r;
}

//==============================================================================
/**
 * @brief Function returns number of items stored in queue.
 *
 * The function queue_get_number_of_items() returns a number of items
 * stored in the queue pointed by <i>queue</i>.
 *
 * @param queue         queue object
 *
 * @exception | @ref EINVAL
 *
 * @return Number of items stored in the queue. On error, -1 is returned.
 *
 * @b Example
 * @code
        #include <dnx/thread.h>

        // ...

        queue_t *queue = queue_new(10, sizeof(char));

        // ...

        void thread1(void *arg)
        {
                char c = '1';
                queue_send(queue, &c, MAX_DELAY_MS);

                // additional operations
        }

        void thread2(void *arg)
        {
                while (queue_get_number_of_items() > 0) {

                        char c;
                        queue_receive(queue, &c, MAX_DELAY_MS);

                        if (c == '1') {
                                // ...
                        } else {
                                // ...
                        }

                        // additional operations
                }
        }

        // ...
        // join threads...
        // ...

        queue_delete(queue);

        //...

   @endcode
 *
 * @see queue_send(), queue_receive(), queue_reset()
 */
//==============================================================================
static inline int queue_get_number_of_items(queue_t *queue)
{
        int r = -1;
        _libc_syscall(_LIBC_SYS_QUEUEITEMSCOUNT, &r, queue);
        return r;
}

//==============================================================================
/**
 * @brief Function returns available space in queue.
 *
 * The function queue_get_space_available() returns a number of free
 * items available in the queue pointed by <i>queue</i>.
 *
 * @param queue         queue object
 *
 * @exception | @ref EINVAL
 *
 * @return Number of free items available in the queue. On error, -1 is returned.
 *
 * @b Example
 * @code
        #include <dnx/thread.h>

        // ...

        queue_t *queue = queue_new(10, sizeof(int));

        // ...

        if (queue_get_space_available(queue) > 0) {
                // ...
        } else {
                // ...
        }

        // ...

   @endcode
 *
 * @see queue_get_number_of_items(), queue_receive(), queue_send(), queue_reset()
 */
//==============================================================================
static inline int queue_get_space_available(queue_t *queue)
{
        int r = -1;
        _libc_syscall(_LIBC_SYS_QUEUEFREESPACE, &r, queue);
        return r;
}

#ifdef __cplusplus
}
#endif

#endif /* _THREAD_H_ */

/**@}*/
/*==============================================================================
  End of file
==============================================================================*/
