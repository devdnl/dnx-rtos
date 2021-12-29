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
struct _lib_mutex {
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
                _libc_syscall(_LIBC_SYS_MUTEXCLOSE, &r, &mutex->fd);
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

/*==============================================================================
  End of file
==============================================================================*/
