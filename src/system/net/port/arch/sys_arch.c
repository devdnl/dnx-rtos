/*=========================================================================*//**
@file    sys_arch.c

@author  Daniel Zorychta

@brief

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

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Include files
==============================================================================*/
#include "sys_arch.h"
#include "system/dnx.h"
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
 * @brief Initialize system calls
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
        (void)prio;

        return new_task(thread, name, stacksize, arg);
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
 */
//==============================================================================
sys_prot_t sys_arch_protect()
{
        enter_critical_section();
        return 1;
}

//==============================================================================
/**
 * @brief Unprotect thread
 *
 * This optional function does a "fast" set of critical region protection to the
 * value specified by pval. See the documentation for sys_arch_protect() for
 * more information. This function is only required if your port is supporting
 * an operating system.
 */
//==============================================================================
void sys_arch_unprotect(sys_prot_t lev)
{
        (void) lev;
        exit_critical_section();
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
 * @return time in milliseconds
 */
//==============================================================================
u32_t sys_now()
{
        return get_OS_time_ms();
}

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
        _stop_if(!sem);

        if (sem) {
                *sem = new_semaphore();
                if (*sem) {
                        if (count == 0) {
                                take_semaphore(*sem, 0);
                        }

                        return ERR_OK;
                }

                return ERR_MEM;
        }

        return ERR_ARG;
}

//==============================================================================
/**
 * @brief Delete a semaphore
 *
 * @param sem semaphore to delete
 */
//==============================================================================
void sys_sem_free(sys_sem_t *sem)
{
        _stop_if(!sem);

        if (sem) {
                delete_semaphore(*sem);
        }
}

//==============================================================================
/**
 * @brief Signals a semaphore
 *
 * @param sem the semaphore to signal
 */
//==============================================================================
void sys_sem_signal(sys_sem_t *sem)
{
        _stop_if(!sem);

        if (sem) {
                give_semaphore(*sem);
        }
}

//==============================================================================
/**
 * @brief Wait for a semaphore for the specified timeout
 *
 * @param sem the semaphore to wait for
 * @param timeout timeout in milliseconds to wait (0 = wait forever)
 *
 * @return time (in milliseconds) waited for the semaphore or SYS_ARCH_TIMEOUT on timeout
 */
//==============================================================================
u32_t sys_arch_sem_wait(sys_sem_t *sem, u32_t timeout)
{
        _stop_if(!sem);

        if (sem) {
                u32_t start_time = get_OS_time_ms();
                bool  sem_status = false;

                if (timeout) {
                        sem_status = take_semaphore(*sem, timeout);
                } else {
                        sem_status = take_semaphore(*sem, MAX_DELAY);
                }

                if (sem_status == SEMAPHORE_TAKEN) {
                        return (u32_t)get_OS_time_ms() - start_time;
                }
        }

        return SYS_ARCH_TIMEOUT;
}

//==============================================================================
/**
 * @brief Check if a semaphore is valid/allocated
 *
 * @return 1 for valid, 0 for invalid
 */
//==============================================================================
int sys_sem_valid(sys_sem_t *sem)
{
        _stop_if(!sem);

        if (sem) {
                if (*sem) {
                        return 1;
                }
        }

        return 0;
}

//==============================================================================
/**
 * @brief Set a semaphore invalid so that sys_sem_valid returns 0
 */
//==============================================================================
void sys_sem_set_invalid(sys_sem_t *sem)
{
        _stop_if(!sem);

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
        _stop_if(!mbox);

        if (mbox && size) {
                *mbox = new_queue(size, sizeof(void*));
                if (*mbox) {
                        return ERR_OK;
                }

                return ERR_MEM;
        }

        return ERR_ARG;
}

//==============================================================================
/**
 * @brief Delete an mbox
 *
 * @param mbox          mbox to delete
 */
//==============================================================================
void sys_mbox_free(sys_mbox_t *mbox)
{
        _stop_if(!mbox);

        if (mbox) {
                _stop_if(get_number_of_items_in_queue(*mbox));
                delete_queue(*mbox);
        }
}

//==============================================================================
/**
 * @brief Post a message to an mbox - may not fail
 * - blocks if full, only used from tasks not from ISR
 *
 * @param mbox          mbox to posts the message
 * @param msg           message to post (ATTENTION: can be NULL)
 */
//==============================================================================
void sys_mbox_post(sys_mbox_t *mbox, void *msg)
{
        _stop_if(!mbox || !msg);

        if (mbox) {
                send_queue(*mbox, &msg, MAX_DELAY);
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
        _stop_if(!mbox || !msg);

        if (mbox) {
                if (send_queue(*mbox, &msg, 0)) {
                        return ERR_OK;
                }

                return ERR_MEM;
        }

        return ERR_ARG;
}

//==============================================================================
/**
 * @brief Wait for a new message to arrive in the mbox
 *
 * @param mbox          mbox to get a message from
 * @param msg           pointer where the message is stored
 * @param timeout       maximum time (in milliseconds) to wait for a message
 *
 * @return time (in milliseconds) waited for a message, may be 0 if not waited
 *         or SYS_ARCH_TIMEOUT on timeout
 *         The returned time has to be accurate to prevent timer jitter!
 */
//==============================================================================
u32_t sys_arch_mbox_fetch(sys_mbox_t *mbox, void **msg, u32_t timeout)
{
        _stop_if(!mbox || !msg);

        if (mbox) {
                u32_t start_time = get_OS_time_ms();
                bool  received   = false;

                if (timeout) {
                        received = receive_queue(*mbox, &(*msg), timeout);
                } else {
                        received = receive_queue(*mbox, &(*msg), MAX_DELAY);
                }

                if (received) {
                        return (u32_t)get_OS_time_ms() - start_time;
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
 * @param timeout       maximum time (in milliseconds) to wait for a message
 *
 * @return 0 (milliseconds) if a message has been received
 *         or SYS_MBOX_EMPTY if the mailbox is empty
 */
//==============================================================================
u32_t sys_arch_mbox_tryfetch(sys_mbox_t *mbox, void **msg)
{
        _stop_if(!mbox || !msg);

        if (mbox) {
                if (receive_queue(*mbox, &(*msg), 0)) {
                        return 0;
                }
        }

        return SYS_MBOX_EMPTY;
}

//==============================================================================
/**
 * @brief Check if an mbox is valid/allocated
 *
 * @return 1 for valid, 0 for invalid
 */
//==============================================================================
int sys_mbox_valid(sys_mbox_t *mbox)
{
        _stop_if(!mbox);

        if (mbox) {
                if (*mbox) {
                        return 1;
                }
        }

        return 0;
}

//==============================================================================
/**
 * @brief Set an mbox invalid so that sys_mbox_valid returns 0
 */
//==============================================================================
void sys_mbox_set_invalid(sys_mbox_t *mbox)
{
        _stop_if(!mbox);

        if (mbox) {
                *mbox = NULL;
        }
}

#ifdef __cplusplus
}
#endif

/*==============================================================================
  End of file
==============================================================================*/
