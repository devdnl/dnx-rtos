/*=========================================================================*//**
@file    mm.c

@author  Daniel Zorychta

@brief   Memory management (memory distribution)

@note    Copyright (C) 2015 Daniel Zorychta <daniel.zorychta@gmail.com>

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

/*==============================================================================
  Include files
==============================================================================*/
#include "mm/mm.h"
#include "mm/heap.h"
#include "sys/types.h"
#include "dnx/misc.h"

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
static i32_t kernel_memory_usage;
static i32_t system_memory_usage;
static i32_t network_memory_usage;
static i32_t module_memory_usage[];

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
 * @brief  ?
 * @param  ?
 * @return ?
 */
//==============================================================================
static void modify_RAM_usage(void *usage, i32_t size)
{
        *reinterpret_cast(i32_t*, usage) += size;
}

//==============================================================================
/**
 * @brief
 */
//==============================================================================
void *_kcalloc(size_t num, size_t size)
{
        return _memman_calloc(num, size, modify_RAM_usage, &kernel_memory_usage);
}

void *_kmalloc(size_t size)
{
        return _memman_malloc(size, modify_RAM_usage, &kernel_memory_usage);
}

void  _kfree(void *mem)
{
        return _memman_free(mem, modify_RAM_usage, &kernel_memory_usage);
}

void *_syscalloc(size_t, size_t);
void *_sysmalloc(size_t);
void  _sysfree(size_t);

void *_modcalloc(size_t, size_t);
void *_modmalloc(size_t);
void  _modfree(size_t);

void *_netcalloc(size_t, size_t);
void *_netmalloc(size_t);
void  _netfree(size_t);

/*==============================================================================
  End of file
==============================================================================*/
