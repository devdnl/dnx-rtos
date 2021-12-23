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
        void *mem = NULL;
        _libc_syscall(_LIBC_SYS_MALLOC, &mem, &size);
        return mem;
}

/*==============================================================================
  End of file
==============================================================================*/
