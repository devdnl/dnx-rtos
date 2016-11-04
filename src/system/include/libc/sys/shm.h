/*=========================================================================*//**
File     shm.h

Author   Daniel Zorychta

Brief    Shared memory functions.

         Copyright (C) 2016 Daniel Zorychta <daniel.zorychta@gmail.com>

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

/**
@defgroup SHM_H_ SHM_H_

The library is used to control shared memory regions used in IPC.
*/
/**@{*/

#ifndef _SHM_H_
#define _SHM_H_

/*==============================================================================
  Include files
==============================================================================*/
#include <kernel/syscall.h>

#ifdef __cplusplus
extern "C" {
#endif

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
 * @brief Function create shared memory region.
 *
 * The shmget() function creates shared memory region of name <i>key</i> and
 * size <i>size</i>. If region already exists then function return error.
 *
 * @param key           region name
 * @param size          region size
 *
 * @exception | EINVAL
 * @exception | ENOMEM
 * @exception | EEXIST
 *
 * @return On success, <b>0</b> is returned. On error, <b>-1</b>
 * is returned, and <b>errno</b> is set appropriately.
 *
 * @b Example
 * @code
        #include <sys/shm.h>

        // ...

        #define SHMNAME "my_region"

        char   *shm = NULL;
        size_t  sz  = 0;

        if (shmget(SHMNAME, 100) == 0) {
                if (shmat(SHMNAME, &shm, &sz) == 0) {
                        puts("Shared memory in use");
                } else {
                        perror(SHMNAME);
                }
        } else {
                perror(SHMNAME);
        }

        // ...

   @endcode
 *
 * @see shmat(), shmdt(), shmrm()
 */
//==============================================================================
static inline int shmget(const char *key, size_t size)
{
        int r = -1;
#if __OS_ENABLE_SHARED_MEMORY__ == _YES_
        syscall(SYSCALL_SHMCREATE, &r, key, &size);
#else
        (void)key;
        (void)size;
#endif
        return r;
}

//==============================================================================
/**
 * @brief Function destroy shared memory region.
 *
 * The shmrm() function destroy shared memory region of name <i>key</i>.
 * The region should be released by processes to be possible remove block.
 *
 * @param key           region name
 *
 * @exception | EINVAL
 * @exception | ENOMEM
 * @exception | ENOENT
 * @exception | EADDRINUSE
 *
 * @return On success, <b>0</b> is returned. On error, <b>-1</b>
 * is returned, and <b>errno</b> is set appropriately.
 *
 * @b Example
 * @code
        #include <sys/shm.h>

        // ...

        #define SHMNAME "my_region"

        char   *shm = NULL;
        size_t  sz  = 0;

        if (shmget(SHMNAME, 100) == 0) {
                if (shmat(SHMNAME, &shm, &sz) == 0) {
                        puts("Shared memory in use");
                } else {
                        perror(SHMNAME);
                }
        } else {
                perror(SHMNAME);
        }

        // ...

        if (shm) {
                shmdt(SHMNAME);

                if (shmrm(SHMNAME)) {
                        perror(SHNAME);
                }
        }

        // ...

   @endcode
 *
 * @see shmat(), shmdt(), shmget()
 */
//==============================================================================
static inline int shmrm(const char *key)
{
        int r = -1;
#if __OS_ENABLE_SHARED_MEMORY__ == _YES_
        syscall(SYSCALL_SHMDESTROY, &r, key);
#else
        (void)key;
#endif
        return r;
}

//==============================================================================
/**
 * @brief Function attach shared memory region.
 *
 * The shmat() function attach shared memory region of name <i>key</i>.
 *
 * @param key           region name
 * @param mem           region address
 * @param size          region size
 *
 * @exception | EINVAL
 * @exception | ENOENT
 *
 * @return On success, <b>0</b> is returned. On error, <b>-1</b>
 * is returned, and <b>errno</b> is set appropriately.
 *
 * @b Example
 * @code
        #include <sys/shm.h>

        // ...

        #define SHMNAME "my_region"

        char   *shm = NULL;
        size_t  sz  = 0;

        if (shmget(SHMNAME, 100) == 0) {
                if (shmat(SHMNAME, &shm, &sz) == 0) {
                        puts("Shared memory in use");

                        strncpy(shm, "My region", sz);

                        // ...

                } else {
                        perror(SHMNAME);
                }
        } else {
                perror(SHMNAME);
        }

        // ...

   @endcode
 *
 * @see shmrm(), shmdt(), shmget()
 */
//==============================================================================
static inline int shmat(const char *key, void **mem, size_t *size)
{
        int r = -1;
#if __OS_ENABLE_SHARED_MEMORY__ == _YES_
        syscall(SYSCALL_SHMATTACH, &r, key, mem, &size);
#else
        (void)key;
        (void)mem;
        (void)size;
#endif
        return r;
}

//==============================================================================
/**
 * @brief Function detach shared memory region.
 *
 * The shmdt() function detach shared memory region of name <i>key</i>.
 *
 * @param key           region name
 *
 * @exception | EINVAL
 * @exception | ENOENT
 *
 * @return On success, <b>0</b> is returned. On error, <b>-1</b>
 * is returned, and <b>errno</b> is set appropriately.
 *
 * @b Example
 * @code
        #include <sys/shm.h>

        // ...

        #define SHMNAME "my_region"

        char   *shm = NULL;
        size_t  sz  = 0;

        if (shmget(SHMNAME, 100) == 0) {
                if (shmat(SHMNAME, &shm, &sz) == 0) {
                        puts("Shared memory in use");

                        strncpy(shm, "My region", sz);

                        // ...

                } else {
                        perror(SHMNAME);
                }
        } else {
                perror(SHMNAME);
        }

        // ...

        if (shm) {
                shmdt(SHMNAME);

                if (shmrm(SHMNAME)) {
                        perror(SHNAME);
                }
        }

        // ...

        // ...

   @endcode
 *
 * @see shmrm(), shmdt(), shmget()
 */
//==============================================================================
static inline int shmdt(const char *key)
{
        int r = -1;
#if __OS_ENABLE_SHARED_MEMORY__ == _YES_
        syscall(SYSCALL_SHMDETACH, &r, key);
#else
        (void)key;
#endif
        return r;
}

#ifdef __cplusplus
}
#endif

#endif /* _SHM_H_ */

/**@}*/
/*==============================================================================
  End of file
==============================================================================*/
