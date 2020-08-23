/*=========================================================================*//**
@file    mm.h

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

#ifndef _MM_H_
#define _MM_H_

/*==============================================================================
  Include files
==============================================================================*/
#include <stdbool.h>
#include <sys/types.h>
#include "heap.h"

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Exported macros
==============================================================================*/
#define _mm_align(_size)         (((_size) + _HEAP_ALIGN_ - 1) & ~(_HEAP_ALIGN_-1))

/*==============================================================================
  Exported object types
==============================================================================*/
typedef struct {
        i32_t static_memory_usage;
        i32_t kernel_memory_usage;
        i32_t filesystems_memory_usage;
        i32_t network_memory_usage;
        i32_t modules_memory_usage;
        i32_t programs_memory_usage;
        i32_t shared_memory_usage;
        i32_t cached_memory_usage;
} _mm_mem_usage_t;

enum _mm_mem {
        _MM_KRN,        //!< allocate memory for kernel purposes
        _MM_FS,         //!< allocate memory for file system purposes
        _MM_NET,        //!< allocate memory for network purposes
        _MM_PROG,       //!< allocate memory for program purposes
        _MM_SHM,        //!< allocate memory for share purposes
        _MM_CACHE,      //!< allocate memory for cache purposes
        _MM_MOD,        //!< allocate memory for modules purposes
        _MM_COUNT
};

typedef struct _mm_region {
        _heap_t            heap;
        struct _mm_region *next;
} _mm_region_t;

/*==============================================================================
  Exported objects
==============================================================================*/

/*==============================================================================
  Exported functions
==============================================================================*/
extern int    _mm_init(void);
extern int    _mm_register_region(_mm_region_t*, void*, size_t);
extern int    _mm_get_mem_usage_details(_mm_mem_usage_t*);
extern int    _mm_get_module_mem_usage(uint module, i32_t *usage);
extern size_t _mm_get_block_size(void*);
extern size_t _mm_get_mem_free(void);
extern size_t _mm_get_mem_usage(void);
extern size_t _mm_get_mem_size(void);
extern bool   _mm_is_object_in_heap(void *ptr);
extern bool   _mm_is_rom_address(void *ptr);
extern int    _kzalloc(enum _mm_mem, const size_t, void**, ...);
extern int    _kmalloc(enum _mm_mem, const size_t, void**, ...);
extern int    _kfree(enum _mm_mem, void**, ...);

/*==============================================================================
  Exported inline functions
==============================================================================*/

#ifdef __cplusplus
}
#endif

#endif /* _MM_H_ */
/*==============================================================================
  End of file
==============================================================================*/
