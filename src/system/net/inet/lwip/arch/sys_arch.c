/*=========================================================================*//**
@file    sys_arch.c

@author  Daniel Zorychta

@brief

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

/*==============================================================================
  Include files
==============================================================================*/
#include <stdlib.h>
#include "kernel/sysfunc.h"
#include "sys_arch.h"
#include "lwip/err.h"
#include "lwip/sys.h"

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
 * @brief  Initialize system calls
 * @param  None
 * @return None
 */
//==============================================================================
void sys_init()
{
}

//==============================================================================
/**
 * @brief Creates a new thread
 *
 * @param name human-readable name for the thread (used for debugging purposes)
 * @param thread        thread-function
 * @param arg           parameter passed to 'thread'
 * @param stacksize     stack size in bytes for the new thread (may be ignored by ports)
 * @param prio          priority of the new thread (may be ignored by ports)
 *
 * @return thread object
 */
//==============================================================================
sys_thread_t sys_thread_new(const char *name, lwip_thread_fn thread, void *arg, int stacksize, int prio)
{
        LWIP_ASSERT("sys_arch.c: wrong task name!", (name != NULL));
        LWIP_ASSERT("sys_arch.c: wrong thread pointer!", (thread != NULL));
        LWIP_ASSERT("sys_arch.c: wrong task stack depth!", (stacksize > 0));

        UNUSED_ARG1(name);

        thread_attr_t attr = {
            .priority    = prio,
            .stack_depth = stacksize,
            .detached    = true
        };

        sys_thread_t thr;

        if (sys_thread_create(thread, &attr, arg, &thr) != ESUCC) {
                thr = -1;
        }

        return thr;
}

//==============================================================================
/**
 * @brief Protect thread
 *
 * This optional function does a "fast" critical region protection and returns
 * the previous protection level. This function is only called during very short
 * critical regions. An embedded system which supports ISR-based drivers might
 * want to implement this function by disabling interrupts. Task-based systems
 * might want to implement this by using a mutex or disabling tasking. This
 * function should support recursive calls from the same task or interrupt. In
 * other words, sys_arch_protect() could be called while already protected. In
 * that case the return value indicates that it is already protected.
 *
 * @param  None
 *
 * @return Protect level (not used)
 */
//==============================================================================
sys_prot_t sys_arch_protect()
{
        sys_critical_section_begin();
        return 1;
}

//==============================================================================
/**
 * @brief Unprotect thread
 *
 * This optional function does a "fast" set of critical region protection to the
 * value specified by val. See the documentation for sys_arch_protect() for
 * more information. This function is only required if your port is supporting
 * an operating system.
 *
 * @param  lev          protection level
 *
 * @return None
 */
//==============================================================================
void sys_arch_unprotect(sys_prot_t lev)
{
        (void) lev;
        sys_critical_section_end();
}

//==============================================================================
/**
 * @brief Return time in milliseconds
 *
 * This optional function returns the current time in milliseconds (don't care
 * for wraparound, this is only used for time diffs).
 * Not implementing this function means you cannot use some modules (e.g. TCP
 * timestamps, internal timeouts for NO_SYS==1).
 *
 * @param  None
 *
 * @return time in milliseconds
 */
//==============================================================================
u32_t sys_now()
{
        return sys_get_uptime_ms();
}

#if !LWIP_COMPAT_MUTEX
//==============================================================================
/**
 * @ingroup sys_mutex
 * Create a new mutex.
 * Note that mutexes are expected to not be taken recursively by the lwIP code,
 * so both implementation types (recursive or non-recursive) should work.
 * The mutex is allocated to the memory that 'mutex'
 * points to (which can be both a pointer or the actual OS structure).
 * If the mutex has been created, ERR_OK should be returned. Returning any
 * other error will provide a hint what went wrong, but except for assertions,
 * no real error handling is implemented.
 *
 * @param mutex pointer to the mutex to create
 * @return ERR_OK if successful, another err_t otherwise
 */
//==============================================================================
err_t sys_mtx_new(sys_mtx_t *mutex)
{
        LWIP_ASSERT("sys_arch.c: wrong mutex object!", (mutex != NULL));

        if (mutex) {
                if (sys_mutex_create(MUTEX_TYPE_NORMAL, &(*mutex)) == ESUCC) {
                        return ERR_OK;
                } else {
                        return ERR_MEM;
                }
        }

        return ERR_ARG;
}

//==============================================================================
/**
 * @ingroup sys_mutex
 * Blocks the thread until the mutex can be grabbed.
 * @param mutex the mutex to lock
 */
//==============================================================================
void sys_mtx_lock(sys_mtx_t *mutex)
{
        LWIP_ASSERT("sys_arch.c: wrong mutex object!", (mutex != NULL));

        if (mutex && *mutex) {
                sys_mutex_lock(*mutex, MAX_DELAY_MS);
        }
}

//==============================================================================
/**
 * @ingroup sys_mutex
 * Releases the mutex previously locked through 'sys_mtx_lock()'.
 * @param mutex the mutex to unlock
 */
//==============================================================================
void sys_mtx_unlock(sys_mtx_t *mutex)
{
        LWIP_ASSERT("sys_arch.c: wrong mutex object!", (mutex != NULL));

        if (mutex && *mutex) {
                sys_mutex_unlock(*mutex);
        }
}

//==============================================================================
/**
 * @ingroup sys_mutex
 * Deallocates a mutex.
 * @param mutex the mutex to delete
 */
//==============================================================================
void sys_mtx_free(sys_mtx_t *mutex)
{
        LWIP_ASSERT("sys_arch.c: wrong mutex object!", (mutex != NULL));

        if (mutex && *mutex) {
                sys_mutex_destroy(*mutex);
        }
}

#ifndef sys_mtx_valid
//==============================================================================
/**
 * @ingroup sys_mutex
 * Returns 1 if the mutes is valid, 0 if it is not valid.
 * When using pointers, a simple way is to check the pointer for != NULL.
 * When directly using OS structures, implementing this may be more complex.
 * This may also be a define, in which case the function is not prototyped.
 */
//==============================================================================
int sys_mtx_valid(sys_mtx_t *mutex)
{
        LWIP_ASSERT("sys_arch.c: wrong mutex object!", (mutex != NULL));

        if (mutex && *mutex) {
                return 1;
        } else {
                return 0;
        }
}
#endif

#ifndef sys_mtx_set_invalid
//==============================================================================
/**
 * @ingroup sys_mutex
 * Invalidate a mutex so that sys_mtx_valid() returns 0.
 * ATTENTION: This does NOT mean that the mutex shall be deallocated:
 * sys_mtx_free() is always called before calling this function!
 * This may also be a define, in which case the function is not prototyped.
 */
//==============================================================================
void sys_mtx_set_invalid(sys_mtx_t *mutex)
{
        LWIP_ASSERT("sys_arch.c: wrong mutex object!", (mutex != NULL));

        if (mutex) {
                *mutex = NULL;
        }
}
#endif
#endif

//==============================================================================
/**
 * @brief Create a new semaphore
 *
 * @param sem           pointer to the semaphore to create
 * @param count         initial count of the semaphore
 *
 * @return ERR_OK if successful, another err_t otherwise
 */
//==============================================================================
err_t sys_sem_new(sys_sem_t *sem, u8_t count)
{
        LWIP_ASSERT("sys_arch.c: wrong semaphore object!", (sem != NULL));

        if (sem) {
                if (sys_semaphore_create(1, count, &(*sem)) == ESUCC) {
                        return ERR_OK;
                } else {
                        return ERR_MEM;
                }
        }

        return ERR_ARG;
}

//==============================================================================
/**
 * @brief Delete a semaphore
 *
 * @param sem semaphore to delete
 *
 * @return None
 */
//==============================================================================
void sys_sem_free(sys_sem_t *sem)
{
        LWIP_ASSERT("sys_arch.c: wrong semaphore object!", (sem != NULL));

        if (sem && *sem) {
                sys_semaphore_destroy(*sem);
        }
}

//==============================================================================
/**
 * @brief Signals a semaphore
 *
 * @param sem the semaphore to signal
 *
 * @return None
 */
//==============================================================================
void sys_sem_signal(sys_sem_t *sem)
{
        LWIP_ASSERT("sys_arch.c: wrong semaphore object!", (sem != NULL));

        if (sem && *sem) {
                sys_semaphore_signal(*sem);
        }
}

//==============================================================================
/**
 * @brief Wait for a semaphore for the specified timeout
 *
 * @param sem           the semaphore to wait for
 * @param timeout       timeout in milliseconds to wait (0 = wait forever)
 *
 * @return time (in milliseconds) waited for the semaphore or SYS_ARCH_TIMEOUT on timeout
 */
//==============================================================================
u32_t sys_arch_sem_wait(sys_sem_t *sem, u32_t timeout)
{
        LWIP_ASSERT("sys_arch.c: wrong semaphore object!", (sem != NULL));

        if (sem && *sem) {
                u32_t start_time = sys_get_uptime_ms();
                int   sem_status = 0;

                if (timeout) {
                        sem_status = sys_semaphore_wait(*sem, timeout);
                } else {
                        sem_status = sys_semaphore_wait(*sem, MAX_DELAY_MS);
                }

                if (sem_status == ESUCC) {
                        return sys_get_uptime_ms() - start_time;
                }
        }

        return SYS_ARCH_TIMEOUT;
}

//==============================================================================
/**
 * @brief Check if a semaphore is valid/allocated
 *
 * @param sem           semaphore
 *
 * @return 1 for valid, 0 for invalid
 */
//==============================================================================
int sys_sem_valid(sys_sem_t *sem)
{
        LWIP_ASSERT("sys_arch.c: wrong semaphore object!", (sem != NULL));

        if (sem && *sem) {
                return 1;
        } else {
                return 0;
        }
}

//==============================================================================
/**
 * @brief Set a semaphore invalid so that sys_sem_valid returns 0
 *
 * @param  sem          semaphore
 *
 * @return None
 */
//==============================================================================
void sys_sem_set_invalid(sys_sem_t *sem)
{
        LWIP_ASSERT("sys_arch.c: wrong semaphore object!", (sem != NULL));

        if (sem) {
                *sem = NULL;
        }
}

//==============================================================================
/**
 * @brief Create a new mbox of specified size
 *
 * @param mbox          pointer to the mbox to create
 * @param size          (minimum) number of messages in this mbox
 *
 * @return ERR_OK if successful, another err_t otherwise
 */
//==============================================================================
err_t sys_mbox_new(sys_mbox_t *mbox, int size)
{
        LWIP_ASSERT("sys_arch.c: wrong mbox object!", (mbox != NULL));

        if (mbox && size) {
                if (sys_queue_create(size, sizeof(void*), &(*mbox)) == ESUCC) {
                        return ERR_OK;
                } else {
                        return ERR_MEM;
                }
        }

        return ERR_ARG;
}

//==============================================================================
/**
 * @brief Delete an mbox
 *
 * @param mbox          mbox to delete
 *
 * @return None
 */
//==============================================================================
void sys_mbox_free(sys_mbox_t *mbox)
{
        LWIP_ASSERT("sys_arch.c: wrong mbox object!", (mbox != NULL));

        if (mbox && *mbox) {
                sys_queue_destroy(*mbox);
        }
}

//==============================================================================
/**
 * @brief Post a message to an mbox - may not fail
 * - blocks if full, only used from tasks not from ISR
 *
 * @param mbox          mbox to posts the message
 * @param msg           message to post (ATTENTION: can be NULL)
 *
 * @return None
 */
//==============================================================================
void sys_mbox_post(sys_mbox_t *mbox, void *msg)
{
        LWIP_ASSERT("sys_arch.c: wrong mbox object!", (mbox != NULL));

        if (mbox && *mbox) {
                sys_queue_send(*mbox, &msg, MAX_DELAY_MS);
        }
}

//==============================================================================
/**
 * @brief Try to post a message to an mbox - may fail if full or ISR
 *
 * @param mbox          mbox to posts the message
 * @param msg           message to post (ATTENTION: can be NULL)
 *
 * @return ERR_MEM if this one is full, else, ERR_OK if the "msg" is posted
 */
//==============================================================================
err_t sys_mbox_trypost(sys_mbox_t *mbox, void *msg)
{
        LWIP_ASSERT("sys_arch.c: wrong mbox object!", (mbox != NULL));

        if (mbox && *mbox) {
                if (sys_queue_send(*mbox, &msg, 0) == ESUCC) {
                        return ERR_OK;
                } else {
                        return ERR_MEM;
                }
        }

        return ERR_ARG;
}

//==============================================================================
/**
 * @brief Wait for a new message to arrive in the mbox
 *
 * @param mbox          mbox to get a message from
 * @param msg           pointer where the message is stored
 * @param timeout       maximum time (in milliseconds) to wait for a message (0 = wait forever)
 *
 * @return time (in milliseconds) waited for a message, may be 0 if not waited
 *         or SYS_ARCH_TIMEOUT on timeout
 *         The returned time has to be accurate to prevent timer jitter!
 */
//==============================================================================
u32_t sys_arch_mbox_fetch(sys_mbox_t *mbox, void **msg, u32_t timeout)
{
        LWIP_ASSERT("sys_arch.c: wrong mbox object!", (mbox != NULL));
        LWIP_ASSERT("sys_arch.c: wrong mbox message destination!", (msg != NULL));

        if (mbox && *mbox) {
                u32_t start_time = sys_get_uptime_ms();

                if (sys_queue_receive(*mbox,
                                      &(*msg),
                                      timeout ? timeout : MAX_DELAY_MS) == ESUCC) {

                        return sys_get_uptime_ms() - start_time;
                }
        }

        return SYS_ARCH_TIMEOUT;
}

//==============================================================================
/**
 * @brief Wait for a new message to arrive in the mbox
 *
 * @param mbox          mbox to get a message from
 * @param msg           pointer where the message is stored
 *
 * @return 0 (milliseconds) if a message has been received
 *         or SYS_MBOX_EMPTY if the mailbox is empty
 */
//==============================================================================
u32_t sys_arch_mbox_tryfetch(sys_mbox_t *mbox, void **msg)
{
        LWIP_ASSERT("sys_arch.c: wrong mbox object!", (mbox != NULL));
        LWIP_ASSERT("sys_arch.c: wrong mbox message destination!", (msg != NULL));

        if (mbox && *mbox) {
                if (sys_queue_receive(*mbox, &(*msg), 0) == ESUCC) {
                        return 0;
                }
        }

        return SYS_MBOX_EMPTY;
}

//==============================================================================
/**
 * @brief Check if an mbox is valid/allocated
 *
 * @param mbox          mbox to check
 *
 * @return 1 for valid, 0 for invalid
 */
//==============================================================================
int sys_mbox_valid(sys_mbox_t *mbox)
{
        LWIP_ASSERT("sys_arch.c: wrong mbox object!", (mbox != NULL));

        if (mbox && *mbox) {
                return 1;
        } else {
                return 0;
        }
}

//==============================================================================
/**
 * @brief Set an mbox invalid so that sys_mbox_valid returns 0
 *
 * @param mbox          mbox to invalidate
 *
 * @return None
 */
//==============================================================================
void sys_mbox_set_invalid(sys_mbox_t *mbox)
{
        LWIP_ASSERT("sys_arch.c: wrong mbox object!", (mbox != NULL));

        if (mbox) {
                *mbox = NULL;
        }
}

//==============================================================================
/**
 * @brief  Free allocated memory block.
 *
 * @param  mem          memory block to free
 */
//==============================================================================
void sys_free(void *mem)
{
        _kfree(_MM_NET, &mem);
}

//==============================================================================
/**
 * @brief  Allocate memory block.
 *
 * @param  blksize      memory block size
 *
 * @return Memory pointer.
 */
//==============================================================================
void *sys_malloc(size_t blksz)
{
        void *mem = NULL;

        if (_kmalloc(_MM_NET, blksz, &mem) == 0) {
                return mem;
        } else {
                return NULL;
        }
}

//==============================================================================
/**
 * @brief  Allocate memory block.
 *
 * @param  n            number of blocks
 * @param  blksize      memory block size
 *
 * @return Memory pointer.
 */
//==============================================================================
void *sys_calloc(size_t n, size_t blksz)
{
        void *mem = NULL;

        if (_kzalloc(_MM_NET, blksz * n, &mem) == 0) {
                return mem;
        } else {
                return NULL;
        }
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
int sys_rand(void)
{
        return rand();
}

/*==============================================================================
  End of file
==============================================================================*/
