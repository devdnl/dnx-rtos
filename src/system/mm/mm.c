/*=========================================================================*//**
@file    mm.c

@author  Daniel Zorychta

@brief   Memory management (memory distribution)

@note    Copyright (C) 2015 Daniel Zorychta <daniel.zorychta@gmail.com>

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
#include <stdarg.h>
#include <string.h>
#include "config.h"
#include "mm/mm.h"
#include "mm/heap.h"
#include "mm/shm.h"
#include "lib/cast.h"
#include "kernel/errno.h"
#include "kernel/ktypes.h"
#include "kernel/kwrapper.h"
#include "kernel/sysfunc.h"
#include "kernel/kpanic.h"

/*==============================================================================
  Local macros
==============================================================================*/
/**
 * Heap size declared by linker script
 */
#define HEAP_SIZE                       ((size_t)&__heap_size)

/**
 * Heap start declared by linker script
 */
#define HEAP_START                      ((void *)&__heap_start)

/**
 * Stack start declared by linker script
 */
#define STACK_START                     ((void *)&__stack_start)

/**
 * RAM start declared by linker script
 */
#define RAM_START                       ((void *)&__ram_start)

/**
 * Calculate memory size for an aligned buffer - returns the next highest
 * multiple of MEM_ALIGNMENT (e.g. MEM_ALIGN_SIZE(3) and
 * MEM_ALIGN_SIZE(4) will both yield 4 for MEM_ALIGNMENT == 4).
 */
#define MEM_ALIGN_SIZE(size)            (((size) + _HEAP_ALIGN_ - 1) & ~(_HEAP_ALIGN_-1))

/**
 * Macro check if selected address (mem) is in selected heap (heap).
 */
#define IS_IN_HEAP(heap, mem)           ((mem) >= cast(void*, (heap).ram) && (mem) < (cast(void*, (heap).ram_end)))

#define TEXT_START                      ((void *)&__text_start)
#define TEXT_END                        ((void *)&__text_end)

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
#if ((__OS_SYSTEM_MSG_ENABLE__ > 0) && (__OS_PRINTF_ENABLE__ > 0))
static const char  *REGISTERED_REGION_STR  = "Registered memory region @ 0x%X of size %d bytes";
static const char  *REGISTRATION_ERROR_STR = "Memory region registration error (%d) @ 0x%X of size %d bytes";
#endif
static _mm_region_t memory_region;
static i32_t        memory_usage[_MM_COUNT - 1];
static i32_t       *module_memory_usage;

/*==============================================================================
  Exported objects
==============================================================================*/

/*==============================================================================
  External objects
==============================================================================*/
/** pointer to stack start */
extern void *__stack_start;

/** pointer to heap start */
extern void *__heap_start;

/** pointer to heap size value */
extern void *__heap_size;

/** basic RAM start */
extern void *__ram_start;

/** number of drivers */
extern const uint _drvreg_number_of_modules;

extern void *__text_start;
extern void *__text_end;

/*==============================================================================
  Function definitions
==============================================================================*/

//==============================================================================
/**
 * @brief  Initialize memory management module. Initialize first heap region.
 *
 * @return On success ESUCC is returned, otherwise other value.
 */
//==============================================================================
int _mm_init(void)
{
        int err = _heap_init(&memory_region.heap, HEAP_START, HEAP_SIZE);
        if (!err) {
                printk(REGISTERED_REGION_STR, HEAP_START, HEAP_SIZE);

                err = _kzalloc(_MM_KRN,
                               _drvreg_number_of_modules * sizeof(i32_t),
                               cast(void*, &module_memory_usage));
        }

        return err;
}

//==============================================================================
/**
 * @brief  Function register next memory region. Can be called before _mm_init().
 *
 * @param  region       region to register
 * @param  start        region start address
 * @param  size         region size
 *
 * @return One of errno values.
 */
//==============================================================================
int _mm_register_region(_mm_region_t *region, void *start, size_t size)
{
        int err = EINVAL;

        if (region && start && size) {
                // check if memory region is already used
                for (_mm_region_t *r = &memory_region; r; r = r->next) {
                        if (r->heap.ram == start) {
                                err = EADDRINUSE;
                                goto finish;
                        }
                }

                // add region to list
                for (_mm_region_t *r = &memory_region; r; r = r->next) {
                        if (r->next == NULL) {
                                region->next = NULL;
                                err = _heap_init(&region->heap, start, size);
                                if (!err) {
                                        r->next = region;
                                }
                                break;
                        }
                }

                finish:
                if (!err) {
                        printk(REGISTERED_REGION_STR, start, size);
                } else {
                        printk(REGISTRATION_ERROR_STR, err, start, size);
                }
        }

        return err;
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
        int err = EINVAL;

        if (mpur < _MM_COUNT && mem && *mem) {
                va_list vaarg;
                va_start(vaarg, mem);
                void *arg = va_arg(vaarg, void*);
                va_end(vaarg);

                i32_t *usage = NULL;

                switch (mpur) {
                case _MM_MOD: {
                        size_t modid = cast(size_t, arg);
                        if (modid < _drvreg_number_of_modules) {
                                usage = &module_memory_usage[modid];
                                err   = ESUCC;
                        }
                        break;
                }

                case _MM_PROG: {
                        if ((*cast(res_header_t**, mem))->type == RES_TYPE_MEMORY) {
                                usage = &memory_usage[mpur];
                                err   = ESUCC;
                                (*cast(res_header_t**, mem))->next = NULL;
                                (*cast(res_header_t**, mem))->type = RES_TYPE_UNKNOWN;
                        } else {
                                err = EFAULT;
                        }
                        break;
                }

                case _MM_CACHE:
                case _MM_SHM:
                case _MM_KRN:
                case _MM_FS:
                case _MM_NET:
                        usage = &memory_usage[mpur];
                        err   = ESUCC;
                        break;

                default:
                        break;
                }

                if (!err) {
                        size_t blksize = 0;

                        for (_mm_region_t *r = &memory_region; r; r = r->next) {
                                if (IS_IN_HEAP(r->heap, *mem)) {
                                        _heap_free(&r->heap, *mem, &blksize);
                                        break;
                                }
                        }

                        _kernel_scheduler_lock();
                        *usage -= blksize;
                        _kernel_scheduler_unlock();

                        *mem = NULL;
                }
        }

        return err;
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
                mem_usage->static_memory_usage      = STACK_START - RAM_START;
                mem_usage->kernel_memory_usage      = memory_usage[_MM_KRN];
                mem_usage->filesystems_memory_usage = memory_usage[_MM_FS];
                mem_usage->network_memory_usage     = memory_usage[_MM_NET];
                mem_usage->programs_memory_usage    = memory_usage[_MM_PROG];
                mem_usage->shared_memory_usage      = memory_usage[_MM_SHM];
                mem_usage->cached_memory_usage      = memory_usage[_MM_CACHE];
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
        for (_mm_region_t *r = &memory_region; r; r = r->next) {
                if (IS_IN_HEAP(r->heap, mem)) {
                        return _heap_get_block_size(&r->heap, mem);
                }
        }

        return 0;
}

//==============================================================================
/**
 * @brief  Return free memory (calculate by using all heap regions).
 *
 * @return Free memory.
 */
//==============================================================================
size_t _mm_get_mem_free(void)
{
        size_t freemem = 0;

        for (_mm_region_t *r = &memory_region; r; r = r->next) {
                freemem += _heap_get_free(&r->heap);
        }

        return freemem;
}

//==============================================================================
/**
 * @brief  Return total usage of RAM memory (calculate all heap regions).
 *
 * @return Total RAM usage.
 */
//==============================================================================
size_t _mm_get_mem_usage(void)
{
        size_t memusage = STACK_START - RAM_START;

        for (_mm_region_t *r = &memory_region; r; r = r->next) {
                memusage += _heap_get_used(&r->heap);
        }

        return memusage;
}

//==============================================================================
/**
 * @brief  Return memory size (RAM).
 *
 * @return Total RAM memory size.
 */
//==============================================================================
size_t _mm_get_mem_size(void)
{
        size_t ramsize = STACK_START - RAM_START;

        for (_mm_region_t *r = &memory_region; r; r = r->next) {
                ramsize += _heap_get_size(&r->heap);
        }

        return ramsize;
}

//==============================================================================
/**
 * @brief  Function check if object is located in heap
 *
 * @param  ptr  pointer to examine
 *
 * @return If pointer is on heap true is returned, otherwise false.
 */
//==============================================================================
bool _mm_is_object_in_heap(void *ptr)
{
        if (ptr == NULL) {
                return false;
        }

        for (_mm_region_t *r = &memory_region; r; r = r->next) {
                if (  (cast(uintptr_t, ptr) >= cast(uintptr_t, r->heap.ram))
                   && (cast(uintptr_t, ptr) <= cast(uintptr_t, r->heap.ram_end  )) ) {

                        return true;
                }
        }

        return false;
}

//==============================================================================
/**
 * @brief  Function check if address is in .text section.
 *
 * @param  ptr          address to examine
 *
 * @return If pointer is in .text section true is returned, otherwise false.
 */
//==============================================================================
bool _mm_is_rom_address(void *ptr)
{
        return (ptr != NULL) && (ptr >= TEXT_START) && (ptr <= TEXT_END);
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
        int err = EINVAL;

        if (mpur < _MM_COUNT && size && mem) {
                i32_t *usage = NULL;

                switch (mpur) {
                case _MM_MOD: {
                        size_t modid = cast(size_t, arg);
                        if (modid < _drvreg_number_of_modules) {
                                usage = &module_memory_usage[modid];
                        }
                        break;
                }

                case _MM_PROG:
                        size += sizeof(res_header_t);
                        usage = &memory_usage[mpur];
                        break;

                case _MM_NET:
                        if (__OS_MONITOR_NETWORK_MEMORY_USAGE_LIMIT__ == 0) {
                                usage = &memory_usage[mpur];
                        } else if (memory_usage[_MM_NET] >= __OS_MONITOR_NETWORK_MEMORY_USAGE_LIMIT__) {
                                err = ENOMEM;
                                goto finish;
                        } else {
                                usage = &memory_usage[mpur];
                        }
                        break;

                case _MM_CACHE:
                case _MM_SHM:
                case _MM_KRN:
                case _MM_FS:
                        usage = &memory_usage[mpur];
                        break;

                default:
                        break;
                }

                if (!usage) {
                        _kernel_panic_report(_KERNEL_PANIC_DESC_CAUSE_INTERNAL_4);
                }

                size = MEM_ALIGN_SIZE(size);

                size_t allocated = 0;
                void  *blk       = NULL;
                       err       = ENOMEM;

                for (_mm_region_t *r = &memory_region; r; r = r->next) {

                        if (_heap_get_free(&r->heap) >= size) {

                                blk = _heap_alloc(&r->heap, size, &allocated);

                                if (blk) {
                                        _kernel_scheduler_lock();
                                        *usage += allocated;
                                        _kernel_scheduler_unlock();

                                        if (clear) {
                                                memset(blk, 0, size);
                                        }

                                        if (mpur == _MM_PROG) {
                                                 cast(res_header_t*, blk)->next = NULL;
                                                 cast(res_header_t*, blk)->type = RES_TYPE_MEMORY;
                                        }

                                        *mem = blk;

                                        err = ESUCC;
                                        goto finish;
                                }
                        }
                }
        }

        finish:
        return err;
}

/*==============================================================================
  End of file
==============================================================================*/
