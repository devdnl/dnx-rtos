/*=========================================================================*//**
@file    alloc.c

@author  Daniel Zorychta

@brief   Memory allocate functions.

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


*//*==========================================================================*/

/*==============================================================================
  Include files
==============================================================================*/
#include <stdio.h>
#include "common.h"

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
 * @brief Function allocates memory block.
 *
 * The malloc() function allocates <i>size</i> bytes and returns a pointer to the
 * allocated memory. The memory is not initialized.  If size is 0, then malloc()
 * returns either @ref NULL.
 *
 * @param size      size bytes to allocate
 *
 * @exception | @ref ENOMEM
 *
 * @return Returns a pointer to the allocated memory, which is suitably aligned
 * for any built-in type.  On error, these functions return @ref NULL.  The
 * @ref NULL pointer may also be returned by a successful call to function with
 * a <i>size</i> of zero.
 */
//==============================================================================
void *_libc_malloc(size_t size)
{
        void *mem;
        int err = _libc_syscall(_LIBC_SYS_MALLOC, &size, &mem);
        return err ? NULL : mem;
}

//==============================================================================
/**
 * @brief Function allocates memory block.
 *
 * The calloc() function allocates memory for an array of <i>n</i> elements
 * of <i>size</i> bytes each and returns a pointer to the allocated memory.
 * The memory is set to zero.
 *
 * @param n         number of elements
 * @param size      size of elements
 *
 * @exception | @ref ENOMEM
 *
 * @return Returns a pointer to the allocated memory, which is suitably aligned
 * for any built-in type.  On error, these functions return @ref NULL. The
 * @ref NULL pointer may also be returned by a successful call to function with
 * a <i>n</i> or <i>size</i> of zero.
 */
//==============================================================================
void *_libc_zalloc(size_t size)
{
        void *mem;
        int err = _libc_syscall(_LIBC_SYS_ZALLOC, &size, &mem);
        return err ? NULL : mem;
}

//==============================================================================
/**
 * @brief Function frees allocated memory block.
 *
 * The free() function frees the memory space pointed to by <i>ptr</i>, which
 * must have been returned by a previous call to malloc(), calloc(),
 * realloc(). If <i>ptr</i> is @ref NULL, no operation is performed.
 *
 * @param ptr       pointer to memory space to be freed
 *
 * @exception | @ref ESRCH
 * @exception | @ref ENOENT
 * @exception | @ref EFAULT
 * @exception | @ref EINVAL
 */
//==============================================================================
void _libc_free(void *ptr)
{
        if (ptr) {
                _libc_syscall(_LIBC_SYS_FREE, ptr);
        } else {
                errno = EINVAL;
        }
}

/*==============================================================================
  End of file
==============================================================================*/
