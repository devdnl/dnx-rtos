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
#include <string.h>
#include "config.h"
#include "mm/mm.h"
#include "mm/heap.h"
#include "lib/cast.h"
#include "kernel/errno.h"
#include "kernel/ktypes.h"
#include "kernel/kwrapper.h"

/*==============================================================================
  Local macros
==============================================================================*/
/**
 * RAM size defined in linker script
 */
#define RAM_SIZE                        ((size_t)&__ram_size)

/**
 * MEM_ALIGNMENT: should be set to the alignment of the CPU for which
 * program is compiled. 4 byte alignment -> define MEM_ALIGNMENT to 4, 2
 * byte alignment -> define MEM_ALIGNMENT to 2.
 */
#define MEM_ALIGNMENT                   CONFIG_HEAP_ALIGN

/**
 * Calculate memory size for an aligned buffer - returns the next highest
 * multiple of MEM_ALIGNMENT (e.g. MEM_ALIGN_SIZE(3) and
 * MEM_ALIGN_SIZE(4) will both yield 4 for MEM_ALIGNMENT == 4).
 */
#define MEM_ALIGN_SIZE(size)            (((size) + MEM_ALIGNMENT - 1) & ~(MEM_ALIGNMENT-1))

/*==============================================================================
  Local object types
==============================================================================*/

/*==============================================================================
  Local function prototypes
==============================================================================*/
static int kalloc(enum _mm_mem mpur, size_t size, bool clear, void **mem, void *arg);

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
extern void      *__ram_size;

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
        return _kzalloc(_MM_KRN,
                        _drvreg_number_of_modules * sizeof(i32_t),
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
        va_list vaarg;
        va_start(vaarg, mem);
        void *arg = va_arg(vaarg, void*);
        va_end(vaarg);

        return kalloc(mpur, size, true, mem, arg);
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
        va_list vaarg;
        va_start(vaarg, mem);
        void *arg = va_arg(vaarg, void*);
        va_end(vaarg);

        return kalloc(mpur, size, false, mem, arg);
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
                va_list vaarg;
                va_start(vaarg, mem);
                void *arg = va_arg(vaarg, void*);
                va_end(vaarg);

                i32_t *usage = NULL;

                switch (mpur) {
                case _MM_MOD: {
                        size_t modid = reinterpret_cast(size_t, arg);
                        if (modid < _drvreg_number_of_modules) {
                                usage  = &module_memory_usage[modid];
                                result = ESUCC;
                        }
                        break;
                }

                case _MM_PROG:
                        if ((*reinterpret_cast(res_header_t**, mem))->type == RES_TYPE_MEMORY) {
                                usage  = arg;
                                result = ESUCC;
                                (*reinterpret_cast(res_header_t**, mem))->next = NULL;
                                (*reinterpret_cast(res_header_t**, mem))->type = RES_TYPE_UNKNOWN;
                        } else {
                                result = EFAULT;
                                break;
                        }
                        // go through
                case _MM_KRN:
                case _MM_FS:
                case _MM_NET:
                        usage  = &memory_usage[mpur];
                        result = ESUCC;
                        break;

                default:
                        break;
                }

                if (result == ESUCC) {
                        size_t blksize = 0;
                        _heap_free(*mem, &blksize);

                        _critical_section_begin();
                        *usage -= blksize;
                        _critical_section_end();

                        *mem = NULL;
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
int _mm_get_mem_usage_details(_mm_mem_usage_t *mem_usage)
{
        if (mem_usage) {
                mem_usage->static_memory_usage      = RAM_SIZE - _heap_get_size();
                mem_usage->kernel_memory_usage      = memory_usage[_MM_KRN];
                mem_usage->filesystems_memory_usage = memory_usage[_MM_FS];
                mem_usage->network_memory_usage     = memory_usage[_MM_NET];
                mem_usage->programs_memory_usage    = memory_usage[_MM_PROG];
                mem_usage->modules_memory_usage     = 0;

                for (size_t i = 0; i < _drvreg_number_of_modules; i++) {
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
 * @brief  Return size of selected memory block
 *
 * @param  mem      block to check
 *
 * @return Size of selected memory block, 0 on error
 */
//==============================================================================
size_t _mm_get_block_size(void *mem)
{
        return _heap_get_block_size(mem);
}

//==============================================================================
/**
 * @brief  Return free memory
 *
 * @param  None
 *
 * @return Free memory
 */
//==============================================================================
size_t _mm_get_mem_free()
{
        return _heap_get_free();
}

//==============================================================================
/**
 * @brief  Return total usage of RAM memory
 *
 * @param  None
 *
 * @return Total RAM usage
 */
//==============================================================================
size_t _mm_get_mem_usage()
{
        return (RAM_SIZE - _heap_get_size()) + _heap_get_used();
}

//==============================================================================
/**
 * @brief  Return memory size (RAM)
 *
 * @param  None
 *
 * @return Total RAM memory size
 */
//==============================================================================
size_t _mm_get_mem_size()
{
        return RAM_SIZE;
}

//==============================================================================
/**
 * @brief  Allocate memory
 *
 * _MM_PROG:
 *      Function allocate extra size (res_header_t) when block is created for
 *      application purposes. Extra size is used to create chain of resources.
 *
 * _MM_MOD
 *      Function use arg argument to pass module number. Argument is directly
 *      interpreted as int type.
 *
 * @param[in]      mpur             memory purpose
 * @param[in]      size             object size
 * @param[in]      clear            clear allocated block
 * @param[out]     mem              pointer to memory block pointer
 * @param[out,in]  arg              argument depending on selected memory region
 *
 * @return One of errno values.
 */
//==============================================================================
static int kalloc(enum _mm_mem mpur, size_t size, bool clear, void **mem, void *arg)
{
        int result = EINVAL;

        if (mpur < _MM_COUNT && size && mem) {
                i32_t *usage = NULL;

                switch (mpur) {
                case _MM_MOD: {
                        size_t modid = reinterpret_cast(size_t, arg);
                        if (modid < _drvreg_number_of_modules) {
                                usage = &module_memory_usage[modid];
                        }
                        break;
                }

                case _MM_PROG:
                        size += sizeof(res_header_t);
                        // go thorough

                case _MM_KRN:
                case _MM_FS:
                case _MM_NET:
                        usage = &memory_usage[mpur];
                        break;

                default:
                        break;
                }

                if (usage) {
                        size = MEM_ALIGN_SIZE(size);

                        size_t allocated = 0;
                        void *blk = _heap_alloc(size, &allocated);

                        if (blk) {
                                _kernel_scheduler_lock();
                                *usage += allocated;
                                _kernel_scheduler_unlock();

                                if (clear) {
                                        memset(blk, 0, size);
                                }

                                if (mpur == _MM_PROG) {
                                         reinterpret_cast(res_header_t*, blk)->next = NULL;
                                         reinterpret_cast(res_header_t*, blk)->type = RES_TYPE_MEMORY;
                                }

                                *mem = blk;

                                result = ESUCC;
                        } else {
                                result = ENOMEM;
                        }
                }
        }

        return result;
}

/*==============================================================================
  End of file
==============================================================================*/
