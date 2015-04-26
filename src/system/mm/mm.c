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
#include <stdarg.h>
#include <stdbool.h>
#include "mm/mm.h"
#include "mm/heap.h"
#include "lib/cast.h"
#include "kernel/errno.h"

/*==============================================================================
  Local macros
==============================================================================*/

/*==============================================================================
  Local object types
==============================================================================*/

/*==============================================================================
  Local function prototypes
==============================================================================*/
static void modify_RAM_usage(void *usage, i32_t size);
static int  kalloc(enum _mm_mem mpur, const size_t size, bool clear, void **mem, size_t modid);

/*==============================================================================
  Local objects
==============================================================================*/
static i32_t  memory_usage[_MM_COUNT - 1];
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
 * @brief  Initialize memory management module
 *
 * @param  None
 *
 * @return On success ESUCC is returned, otherwise other value.
 */
//==============================================================================
int _mm_init(void)
{
        return _kmalloc(_MM_KRN,
                        _drvreg_number_of_modules,
                        static_cast(void*, &module_memory_usage));
}

//==============================================================================
/**
 * @brief  Allocate memory
 *
 * @param[in]  mpur             memory purpose
 * @param[in]  size             object size
 * @param[out] mem              pointer to memory block pointer
 * @param[in]  ...              module ID if _MM_MOD selected
 *
 * @return One of errno values.
 */
//==============================================================================
int _kzalloc(enum _mm_mem mpur, const size_t size, void **mem, ...)
{
        va_list arg;
        va_start(arg, mem);
        size_t modid = va_arg(arg, int);
        va_end(arg);

        return kalloc(mpur, size, true, mem, modid);
}

//==============================================================================
/**
 * @brief  Allocate memory
 *
 * @param[in]  mpur             memory purpose
 * @param[in]  size             object size
 * @param[out] mem              pointer to memory block pointer
 * @param[in]  ...              module ID if _MM_MOD selected
 *
 * @return One of errno values.
 */
//==============================================================================
int _kmalloc(enum _mm_mem mpur, const size_t size, void **mem, ...)
{
        va_list arg;
        va_start(arg, mem);
        size_t modid = va_arg(arg, int);
        va_end(arg);

        return kalloc(mpur, size, false, mem, modid);
}

//==============================================================================
/**
 * @brief  Free allocated memory. Set selected buffer pointer to NULL.
 *
 * @param[in]     mpur          memory purpose
 * @param[in,out] mem           pointer to memory block to free
 * @param[in]  ...              module ID if _MM_MOD selected
 *
 * @return One of errno values.
 */
//==============================================================================
int _kfree(enum _mm_mem mpur, void **mem, ...)
{
        int result = EINVAL;

        if (mpur < _MM_COUNT && mem && *mem) {
                i32_t *usage = NULL;

                if (mpur == _MM_MOD) {
                        va_list arg;
                        va_start(arg, mem);
                        size_t modid = va_arg(arg, int);
                        va_end(arg);

                        if (modid < _drvreg_number_of_modules) {
                                usage = &module_memory_usage[modid];
                        }
                } else {
                        usage = &memory_usage[mpur];
                }

                if (usage) {
                        _heap_free(*mem, modify_RAM_usage, usage);
                        *mem   = NULL;
                        result = ESUCC;
                }
        }

        return result;
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
                mem_usage->static_memory_usage      = _HEAP_RAM_SIZE - _HEAP_SIZE;
                mem_usage->kernel_memory_usage      = memory_usage[_MM_KRN];
                mem_usage->filesystems_memory_usage = memory_usage[_MM_FS];
                mem_usage->network_memory_usage     = memory_usage[_MM_NET];
                mem_usage->programs_memory_usage    = memory_usage[_MM_PROG];
                mem_usage->modules_memory_usage     = 0;

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
 * @brief  Allocate memory
 *
 * @param[in]  mpur             memory purpose
 * @param[in]  size             object size
 * @param[in]  clear            clear allocated block
 * @param[out] mem              pointer to memory block pointer
 * @param[in]  modid            module ID if _MM_MOD selected
 *
 * @return One of errno values.
 */
//==============================================================================
static int kalloc(enum _mm_mem mpur, const size_t size, bool clear, void **mem, size_t modid)
{
        int result = EINVAL;

        if (mpur < _MM_COUNT && size && mem) {
                i32_t *usage = NULL;

                if (mpur == _MM_MOD) {
                        if (modid < _drvreg_number_of_modules) {
                                usage = &module_memory_usage[modid];
                        }
                } else {
                        usage = &memory_usage[mpur];
                }

                if (usage) {
                        if (clear) {
                                *mem = _heap_zalloc(size, modify_RAM_usage, usage);
                        } else {
                                *mem = _heap_malloc(size, modify_RAM_usage, usage);
                        }

                        result = *mem ? ESUCC : ENOMEM;
                }
        }

        return result;
}

/*==============================================================================
  End of file
==============================================================================*/
