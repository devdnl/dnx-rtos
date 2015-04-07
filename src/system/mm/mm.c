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
#include "dnx/misc.h"
#include "errno.h"

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
static i32_t  kernel_memory_usage;
static i32_t  filesystems_memory_usage;
static i32_t  network_memory_usage;
static i32_t  applications_memory_usage;
static i32_t *module_memory_usage;

/*==============================================================================
  Exported objects
==============================================================================*/

/*==============================================================================
  External objects
==============================================================================*/
extern const uint _drvreg_number_of_modules;

/*==============================================================================
  Function definitions
==============================================================================*/

//==============================================================================
/**
 * @brief  Function modify selected variable of memory usage
 *
 * @param  usage        pointer to memory usage variable
 * @param  size         allocated/freed memory
 *
 * @return None
 */
//==============================================================================
static void modify_RAM_usage(void *usage, i32_t size)
{
        *reinterpret_cast(i32_t*, usage) += size;
}

//==============================================================================
/**
 * @brief  ?
 * @param  ?
 * @return ?
 */
//==============================================================================
void _mm_init(void)
{
        module_memory_usage = _kmalloc(_drvreg_number_of_modules);
}

//==============================================================================
/**
 * @brief  Allocate memory
 *
 * @param  num          number of object of size 'size' to allocate
 * @param  size         object size
 *
 * @return Pointer to memory region or NULL if no memory
 */
//==============================================================================
void *_kcalloc(size_t num, size_t size)
{
        return _heap_calloc(num, size, modify_RAM_usage, &kernel_memory_usage);
}

//==============================================================================
/**
 * @brief  Allocate memory
 *
 * @param  size         object size
 *
 * @return Pointer to memory region or NULL if no memory
 */
//==============================================================================
void *_kmalloc(size_t size)
{
        return _heap_malloc(size, modify_RAM_usage, &kernel_memory_usage);
}

//==============================================================================
/**
 * @brief  Free allocated memory
 *
 * @param  num          number of object of size 'size' to allocate
 * @param  size         object size
 *
 * @return Pointer to memory region or NULL if no memory
 */
//==============================================================================
void _kfree(void *mem)
{
        return _heap_free(mem, modify_RAM_usage, &kernel_memory_usage);
}

//==============================================================================
/**
 * @brief  Allocate memory
 *
 * @param  num          number of object of size 'size' to allocate
 * @param  size         object size
 *
 * @return Pointer to memory region or NULL if no memory
 */
//==============================================================================
void *_fscalloc(size_t num, size_t size)
{
        return _heap_calloc(num, size, modify_RAM_usage, &filesystems_memory_usage);
}

//==============================================================================
/**
 * @brief  Allocate memory
 *
 * @param  size         object size
 *
 * @return Pointer to memory region or NULL if no memory
 */
//==============================================================================
void *_fsmalloc(size_t size)
{
        return _heap_malloc(size, modify_RAM_usage, &filesystems_memory_usage);
}

//==============================================================================
/**
 * @brief  Free allocated memory
 *
 * @param  num          number of object of size 'size' to allocate
 * @param  size         object size
 *
 * @return Pointer to memory region or NULL if no memory
 */
//==============================================================================
void _fsfree(void *mem)
{
        return _heap_free(mem, modify_RAM_usage, &filesystems_memory_usage);
}

//==============================================================================
/**
 * @brief  Allocate memory
 *
 * @param  num          number of object of size 'size' to allocate
 * @param  size         object size
 *
 * @return Pointer to memory region or NULL if no memory
 */
//==============================================================================
void *_netcalloc(size_t num, size_t size)
{
        return _heap_calloc(num, size, modify_RAM_usage, &network_memory_usage);
}

//==============================================================================
/**
 * @brief  Allocate memory
 *
 * @param  size         object size
 *
 * @return Pointer to memory region or NULL if no memory
 */
//==============================================================================
void *_netmalloc(size_t size)
{
        return _heap_malloc(size, modify_RAM_usage, &network_memory_usage);
}

//==============================================================================
/**
 * @brief  Free allocated memory
 *
 * @param  num          number of object of size 'size' to allocate
 * @param  size         object size
 *
 * @return Pointer to memory region or NULL if no memory
 */
//==============================================================================
void _netfree(void *mem)
{
        return _heap_free(mem, modify_RAM_usage, &network_memory_usage);
}

//==============================================================================
/**
 * @brief  Allocate memory
 *
 * @param  num          number of object of size 'size' to allocate
 * @param  size         object size
 *
 * @return Pointer to memory region or NULL if no memory
 */
//==============================================================================
void *_appcalloc(size_t num, size_t size)
{
        return _heap_calloc(num, size, modify_RAM_usage, &network_memory_usage);
}

//==============================================================================
/**
 * @brief  Allocate memory
 *
 * @param  size         object size
 *
 * @return Pointer to memory region or NULL if no memory
 */
//==============================================================================
void *_appmalloc(size_t size)
{
        return _heap_malloc(size, modify_RAM_usage, &network_memory_usage);
}

//==============================================================================
/**
 * @brief  Free allocated memory
 *
 * @param  num          number of object of size 'size' to allocate
 * @param  size         object size
 *
 * @return Pointer to memory region or NULL if no memory
 */
//==============================================================================
void _appfree(void *mem)
{
        return _heap_free(mem, modify_RAM_usage, &network_memory_usage);
}

//==============================================================================
/**
 * @brief  Allocate memory
 *
 * @param  num          number of object of size 'size' to allocate
 * @param  size         object size
 *
 * @return Pointer to memory region or NULL if no memory
 */
//==============================================================================
void *_modcalloc(size_t num, size_t size, size_t modid)
{
        if (modid >= 0 && modid < _drvreg_number_of_modules) {
                return _heap_calloc(num, size, modify_RAM_usage, &module_memory_usage[modid]);
        } else {
                return NULL;
        }
}

//==============================================================================
/**
 * @brief  Allocate memory
 *
 * @param  size         object size
 *
 * @return Pointer to memory region or NULL if no memory
 */
//==============================================================================
void *_modmalloc(size_t size, size_t modid)
{
        if (modid >= 0 && modid < _drvreg_number_of_modules) {
                return _heap_malloc(size, modify_RAM_usage, &module_memory_usage[modid]);
        } else {
                return NULL;
        }
}

//==============================================================================
/**
 * @brief  Free allocated memory
 *
 * @param  num          number of object of size 'size' to allocate
 * @param  size         object size
 *
 * @return Pointer to memory region or NULL if no memory
 */
//==============================================================================
void _modfree(void *mem, size_t modid)
{
        if (modid >= 0 && modid < _drvreg_number_of_modules) {
                return _heap_free(mem, modify_RAM_usage, &module_memory_usage[modid]);
        }
}

//==============================================================================
/**
 * @brief  Return information of memory usage
 *
 * @param  mem_usage    memory usage information
 *
 * @return On success ESUCC (0) is returned, otherwise different than 0 is returned.
 */
//==============================================================================
int _mm_get_mem_usage(_mm_mem_usage_t *mem_usage)
{
        if (mem_usage) {
                mem_usage->kernel_memory_usage       = kernel_memory_usage;
                mem_usage->filesystems_memory_usage  = filesystems_memory_usage;
                mem_usage->network_memory_usage      = network_memory_usage;
                mem_usage->applications_memory_usage = applications_memory_usage;
                mem_usage->modules_memory_usage      = 0;

                for (size_t i = 0; _drvreg_number_of_modules; i++) {
                        mem_usage->modules_memory_usage += module_memory_usage[i];
                }

                return ESUCC;
        } else {
                return EINVAL;
        }
}

//==============================================================================
/**
 * @brief  Return used memory by selected module
 *
 * @param  module       number of module
 * @param  usage        memory usage
 *
 * @return On success 0 is returned, otherwise other value.
 */
//==============================================================================
int _mm_get_module_mem_usage(uint module, i32_t *usage)
{
        if (module < _drvreg_number_of_modules && usage) {
                *usage = module_memory_usage[module];
                return ESUCC;
        } else {
                return EINVAL;
        }
}

/*==============================================================================
  End of file
==============================================================================*/
