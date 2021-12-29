/*==============================================================================
@file    concurrent.c

@author  Daniel Zorychta

@brief   Concurrency functions (mutex, semaphore, queue).

@note    Copyright (C) 2021 Daniel Zorychta <daniel.zorychta@gmail.com>

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
#include <dnx/thread.h>
#include <stdlib.h>
#include "libc/source/syscall.h"

/*==============================================================================
  Local macros
==============================================================================*/

/*==============================================================================
  Local object types
==============================================================================*/
struct _libc_mutex {
        int fd;
};

struct _libc_sem {
        int fd;
};

struct _libc_queue {
        int fd;
};

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
 */
//==============================================================================
mutex_t *mutex_new(enum mutex_type type)
{
        mutex_t *mutex = malloc(sizeof(*mutex));
        if (mutex) {
                mutex->fd = -1;
                _libc_syscall(_LIBC_SYS_MUTEXOPEN, &mutex->fd, &type);

                if (mutex->fd >= 0) {
                        return mutex;
                }

                free(mutex);
        }

        return NULL;
}

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
 */
//==============================================================================
void mutex_delete(mutex_t *mutex)
{
        if (mutex && (mutex->fd >= 0)) {
                int r = -1;
                _libc_syscall(_LIBC_SYS_CLOSE, &r, &mutex->fd);
                free(mutex);
        }
}

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
 */
//==============================================================================
bool mutex_lock(mutex_t *mutex, const u32_t timeout)
{
        bool locked = false;

        if (mutex && (mutex->fd >= 0)) {
                int r = -1;
                _libc_syscall(_LIBC_SYS_MUTEXLOCK, &r, &mutex->fd, &timeout);
                locked = (r == 0);
        }

        return locked;
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
 */
//==============================================================================
bool mutex_unlock(mutex_t *mutex)
{
        bool unlocked = false;

        if (mutex && (mutex->fd >= 0)) {
                int r = -1;
                _libc_syscall(_LIBC_SYS_MUTEXUNLOCK, &r, &mutex->fd);
                unlocked = (r == 0);
        }

        return unlocked;
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
 */
//==============================================================================
sem_t *semaphore_new(const size_t cnt_max, const size_t cnt_init)
{
        sem_t *sem = malloc(sizeof(*sem));
        if (sem) {
                sem->fd = -1;
                _libc_syscall(_LIBC_SYS_SEMAPHOREOPEN, &sem->fd, &cnt_max, &cnt_init);

                if (sem->fd >= 0) {
                        return sem;
                }

                free(sem);
        }

        return NULL;
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
 */
//==============================================================================
void semaphore_delete(sem_t *sem)
{
        if (sem && (sem->fd >= 0)) {
                int r = -1;
                _libc_syscall(_LIBC_SYS_CLOSE, &r, &sem->fd);
                free(sem);
        }
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
bool semaphore_wait(sem_t *sem, const u32_t timeout)
{
        bool ready = false;

        if (sem && (sem->fd >= 0)) {
                int r = -1;
                _libc_syscall(_LIBC_SYS_SEMAPHOREWAIT, &r, &sem->fd, &timeout);
                ready = (r == 0);
        }

        return ready;
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
bool semaphore_signal(sem_t *sem)
{
        bool signalled = false;

        if (sem && (sem->fd >= 0)) {
                int r = -1;
                _libc_syscall(_LIBC_SYS_SEMAPHORESIGNAL, &r, &sem->fd);
                signalled = (r == 0);
        }

        return signalled;
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
int semaphore_get_value(sem_t *sem)
{
        int value = -1;

        if (sem && (sem->fd >= 0)) {
                int r = -1;
                size_t val = 0;
                _libc_syscall(_LIBC_SYS_SEMAPHOREGETVALUE, &r, &sem->fd, &val);
                if (r == 0) {
                        value = val;
                }
        }

        return value;
}

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
 */
//==============================================================================
queue_t *queue_new(const size_t length, const size_t item_size)
{
        queue_t *queue = malloc(sizeof(*queue));
        if (queue) {
                queue->fd = -1;
                _libc_syscall(_LIBC_SYS_QUEUEOPEN, &queue->fd, &length, &item_size);

                if (queue->fd >= 0) {
                        return queue;
                }

                free(queue);
        }

        return NULL;
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
 */
//==============================================================================
void queue_delete(queue_t *queue)
{
        if (queue && (queue->fd >= 0)) {
                int r = -1;
                _libc_syscall(_LIBC_SYS_CLOSE, &r, &queue->fd);
                free(queue);
        }
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
 */
//==============================================================================
bool queue_reset(queue_t *queue)
{
        bool done = false;

        if (queue && (queue->fd >= 0)) {
                int r = -1;
                _libc_syscall(_LIBC_SYS_QUEUERESET, &r, &queue->fd);
                done = (r == 0);
        }

        return done;
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
 */
//==============================================================================
bool queue_send(queue_t *queue, const void *item, const u32_t timeout)
{
        bool done = false;

        if (queue && (queue->fd >= 0)) {
                int r = -1;
                _libc_syscall(_LIBC_SYS_QUEUESEND, &r, &queue->fd, item, &timeout);
                done = (r == 0);
        }

        return done;
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
 */
//==============================================================================
bool queue_receive(queue_t *queue, void *item, const u32_t timeout)
{
        bool done = false;

        if (queue && (queue->fd >= 0)) {
                int r = -1;
                _libc_syscall(_LIBC_SYS_QUEUERECEIVE, &r, &queue->fd, item, &timeout);
                done = (r == 0);
        }

        return done;
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
 */
//==============================================================================
bool queue_receive_peek(queue_t *queue, void *item, const u32_t timeout)
{
        bool done = false;

        if (queue && (queue->fd >= 0)) {
                int r = -1;
                _libc_syscall(_LIBC_SYS_QUEUERECEIVEPEEK, &r, &queue->fd, item, &timeout);
                done = (r == 0);
        }

        return done;
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
 */
//==============================================================================
int queue_get_number_of_items(queue_t *queue)
{
        int items = -1;

        if (queue && (queue->fd >= 0)) {
                int r = -1;
                size_t count = 0;
                _libc_syscall(_LIBC_SYS_QUEUEITEMSCOUNT, &r, &queue->fd, &count);
                if (r == 0) items = count;
        }

        return items;
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
 */
//==============================================================================
int queue_get_space_available(queue_t *queue)
{
        int items = -1;

        if (queue && (queue->fd >= 0)) {
                int r = -1;
                size_t count = 0;
                _libc_syscall(_LIBC_SYS_QUEUEFREESPACE, &r, &queue->fd, &count);
                if (r == 0) items = count;
        }

        return items;
}

/*==============================================================================
  End of file
==============================================================================*/
