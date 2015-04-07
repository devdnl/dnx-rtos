/*=========================================================================*//**
@file    heap.c

@author  Daniel Zorychta

@brief   This file support dynamic memory management.

@note    Copyright (C) 2014 Daniel Zorychta <daniel.zorychta@gmail.com>

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
/*
* Copyright (c) 2001-2004 Swedish Institute of Computer Science.
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without modification,
* are permitted provided that the following conditions are met:
*
* 1. Redistributions of source code must retain the above copyright notice,
*    this list of conditions and the following disclaimer.
* 2. Redistributions in binary form must reproduce the above copyright notice,
*    this list of conditions and the following disclaimer in the documentation
*    and/or other materials provided with the distribution.
* 3. The name of the author may not be used to endorse or promote products
*    derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
* WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
* MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
* SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
* EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
* OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
* INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
* CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
* IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
* OF SUCH DAMAGE.
*
* This file is part of the lwIP TCP/IP stack.
*
* Author: Adam Dunkels <adam@sics.se>
*         Simon Goldschmidt
*
*//*==========================================================================*/

/*==============================================================================
  Include files
==============================================================================*/
#include "config.h"
#include "mm/heap.h"
#include "kernel/kwrapper.h"
#include <string.h>

/*==============================================================================
  Local symbolic constants/macros
==============================================================================*/
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

/** some alignment macros: we define them here for better source code layout */
#define BLOCK_MIN_SIZE_ALIGNED          MEM_ALIGN_SIZE(CONFIG_HEAP_BLOCK_SIZE)
#define SIZEOF_STRUCT_MEM               MEM_ALIGN_SIZE(sizeof(struct mem))
#define MEM_SIZE_ALIGNED                MEM_ALIGN_SIZE(_HEAP_SIZE - SIZEOF_STRUCT_MEM)

/*==============================================================================
  Local types, enums definitions
==============================================================================*/
/**
 * The heap is made up as a list of structs of this type.
 * This does not have to be aligned since for getting its size,
 * we only use the macro SIZEOF_STRUCT_MEM, which automatically alignes.
 */
struct mem {
        size_t next;    /**< index (-> ram[next]) of the next struct      */
        size_t prev;    /**< index (-> ram[prev]) of the previous struct  */
        u8_t   used;    /**< 1: this area is used; 0: this area is unused */
};

/*==============================================================================
  Local function prototypes
==============================================================================*/
static void plug_holes(struct mem *mem);

/*==============================================================================
                                      Local object definitions
==============================================================================*/
/** pointer to the heap (ram_heap): for alignment, ram is now a pointer instead of an array */
static u8_t *heap;

/** the last entry, always unused! */
static struct mem *heap_end;

/** pointer to the lowest free block, this is used for faster search */
static struct mem *lfree;

/** RAM usage */
static u32_t used_mem;

/*==============================================================================
  Exported object definitions
==============================================================================*/

/*==============================================================================
  Function definitions
==============================================================================*/

//==============================================================================
/**
 * @brief  Function calls user defined function when memory is allocated or
 *         freed. On error function is not called. If memory was allocated then
 *         positive value of size is passed. If memory was freed then negative
 *         value of size is passed.
 * @param  func         user defined function
 * @param  arg          argument to pass via function
 * @param  size         allocated/freed size
 * @return None
 */
//==============================================================================
static inline void call_user_function(_heap_func_t func, void *arg, i32_t size)
{
        if (func) {
                func(arg, size);
        }
}

//==============================================================================
/**
 * @brief  "Plug holes" by combining adjacent empty struct mems.
 *         After this function is through, there should not exist
 *         one empty struct mem pointing to another empty struct mem.
 *         This assumes access to the heap is protected by the calling function
 *         already.
 * @param  mem        this points to a struct mem which just has been freed
 * @return None
 */
//==============================================================================
static void plug_holes(struct mem *mem)
{
        struct mem *nmem;
        struct mem *pmem;

        /* plug hole forward */
        nmem = (struct mem *)(void *)&heap[mem->next];

        if (mem != nmem && nmem->used == 0 && (u8_t *)nmem != (u8_t *)heap_end) {
                /* if mem->next is unused and not end of ram, combine mem and mem->next */
                if (lfree == nmem) {
                        lfree = mem;
                }

                mem->next = nmem->next;

                ((struct mem*)(void*)&heap[nmem->next])->prev = (size_t)((u8_t*)mem - heap);
        }

        /* plug hole backward */
        pmem = (struct mem *)(void *)&heap[mem->prev];

        if (pmem != mem && pmem->used == 0) {
                /* if mem->prev is unused, combine mem and mem->prev */
                if (lfree == mem) {
                        lfree = pmem;
                }

                pmem->next = mem->next;

                ((struct mem *)(void *)&heap[mem->next])->prev = (size_t)((u8_t *)pmem - heap);
        }
}

//==============================================================================
/**
* @brief  Zero the heap and initialize start, end and lowest-free
* @param  None
* @return None
*/
//==============================================================================
void _heap_init(void)
{
        /* align the heap */
        heap = (u8_t*)_HEAP_HEAP_START;

        /* initialize the start of the heap */
        struct mem *mem = (struct mem *)heap;
        mem->next = MEM_SIZE_ALIGNED;
        mem->prev = 0;
        mem->used = 0;

        /* initialize the end of the heap */
        heap_end = (struct mem *)&heap[MEM_SIZE_ALIGNED];
        heap_end->used = 1;
        heap_end->next = MEM_SIZE_ALIGNED;
        heap_end->prev = MEM_SIZE_ALIGNED;

        /* initialize the lowest-free pointer to the start of the heap */
        lfree = (struct mem *)heap;
}

//==============================================================================
/**
 * @brief  Put a struct mem back on the heap
 * @param  rmem         is the data portion of a struct mem as returned by a previous
 *                      call to mem_malloc()
 * @param  func         user defined function called if memory was successfully
 *                      freed
 * @param  arg          user defined argument that is passed via 'func'
 * @return Number of really freed bytes
 */
//==============================================================================
void _heap_free(void *rmem, _heap_func_t func, void *arg)
{
        if ((u8_t *)rmem >= (u8_t *)heap && (u8_t *)rmem < (u8_t *)heap_end)
        {
                vTaskSuspendAll();

                /* Get the corresponding struct mem ... */
                struct mem *mem = (struct mem *)(void *)((u8_t *)rmem - SIZEOF_STRUCT_MEM);
                /* ... which has to be in a used state and is now unused. */
                mem->used = 0;

                if (mem < lfree) {
                        /* the newly freed struct is now the lowest */
                        lfree = mem;
                }

                size_t freed = (mem->next - (size_t)(((u8_t *)mem - heap)));
                used_mem -= freed;

                call_user_function(func, arg, -freed);

                plug_holes(mem);

                xTaskResumeAll();
        }
}

//==============================================================================
/**
 * @brief  Allocate a block of memory with a minimum of 'size' bytes.
 *         Note that the returned value will always be aligned (as defined by MEM_ALIGNMENT).
 * @param  size    is the minimum size of the requested block in bytes.
 * @param  func    user defined function called when memory was allocated
 * @param  arg     user defined argument passed via 'func' function
 * @return Pointer to allocated memory or NULL if no free memory was found.
 */
//==============================================================================
void *_heap_malloc(size_t size, _heap_func_t func, void *arg)
{
        size_t ptr, ptr2;
        struct mem *mem, *mem2;
        size_t used;

        if (size == 0) {
                return NULL;
        }

        /* Expand the size of the allocated memory region so that we can adjust for alignment. */
        size = MEM_ALIGN_SIZE(size);

        if(size < BLOCK_MIN_SIZE_ALIGNED) {
                /* every data block must be at least BLOCK_MIN_SIZE_ALIGNED long */
                size = BLOCK_MIN_SIZE_ALIGNED;
        }

        if (size > MEM_SIZE_ALIGNED) {
                return NULL;
        }

        /* protect the heap from concurrent access */
        vTaskSuspendAll();

        /*
         * Scan through the heap searching for a free block that is big enough,
         * beginning with the lowest free block.
         */
        for (ptr = (size_t)((u8_t *)lfree - heap);
             ptr < MEM_SIZE_ALIGNED - size;
             ptr = ((struct mem *)(void *)&heap[ptr])->next) {

                mem = (struct mem *)(void *)&heap[ptr];

                if ((!mem->used) && (mem->next - (ptr + SIZEOF_STRUCT_MEM)) >= size) {
                        /*
                         * mem is not used and at least perfect fit is possible:
                         * mem->next - (ptr + SIZEOF_STRUCT_MEM) gives us the 'user data size' of mem
                         */

                        if (mem->next - (ptr + SIZEOF_STRUCT_MEM)
                           >= (size + SIZEOF_STRUCT_MEM + BLOCK_MIN_SIZE_ALIGNED)) {
                                /* (in addition to the above, we test if another struct mem
                                * (SIZEOF_STRUCT_MEM) containing
                                * at least BLOCK_MIN_SIZE_ALIGNED of data also fits in the 'user
                                * data space' of 'mem')
                                * -> split large block, create empty remainder,
                                * remainder must be large enough to contain BLOCK_MIN_SIZE_ALIGNED data: if
                                * mem->next - (ptr + (2*SIZEOF_STRUCT_MEM)) == size,
                                * struct mem would fit in but no data between mem2 and mem2->next
                                * @todo we could leave out BLOCK_MIN_SIZE_ALIGNED. We would create an empty
                                *       region that couldn't hold data, but when mem->next gets freed,
                                *       the 2 regions would be combined, resulting in more free memory
                                */
                                ptr2 = ptr + SIZEOF_STRUCT_MEM + size;

                                /* create mem2 struct */
                                mem2 = (struct mem *)(void *)&heap[ptr2];
                                mem2->used = 0;
                                mem2->next = mem->next;
                                mem2->prev = ptr;

                                /* and insert it between mem and mem->next */
                                mem->next = ptr2;
                                mem->used = 1;

                                if (mem2->next != MEM_SIZE_ALIGNED) {
                                        ((struct mem *)(void *)&heap[mem2->next])->prev = ptr2;
                                }

                                used = (size + SIZEOF_STRUCT_MEM);
                                used_mem += used;
                        } else {
                                /* (a mem2 struct does no fit into the user data space of
                                 *  mem and mem->next will always
                                 * be used at this point: if not we have 2 unused structs
                                 * in a row, plug_holes should have
                                 * take care of this).
                                 * -> near fit or excact fit: do not split, no mem2 creation
                                 * also can't move mem->next directly behind mem, since mem->next
                                 * will always be used at this point!
                                 */
                                mem->used = 1;

                                used = (mem->next - (size_t)((u8_t *)mem - heap));
                                used_mem += used;
                        }

                        if (mem == lfree) {
                                /* Find next free block after mem and update lowest free pointer */
                                while (lfree->used && lfree != heap_end) {
                                        lfree = (struct mem *)(void *)&heap[lfree->next];
                                }
                        }

                        call_user_function(func, arg, used);

                        xTaskResumeAll();
                        return (u8_t *)mem + SIZEOF_STRUCT_MEM;
                }
        }

        xTaskResumeAll();
        return NULL;
}

//==============================================================================
/**
 * @brief  Contiguously allocates enough space for count objects that are size
 *         bytes of memory each and returns a pointer to the allocated memory.
 *         The allocated memory is filled with bytes of value zero.
 * @param  count   number of objects to allocate
 * @param  size    size of the objects to allocate
 * @param  func    user defined function called when memory was allocated
 * @param  arg     user defined argument passed via 'func' function
 * @return Pointer to allocated memory or NULL if no free memory was found.
 */
//==============================================================================
void *_heap_calloc(size_t count, size_t size, _heap_func_t func, void *arg)
{
        void *p = _heap_malloc(count * size, func, arg);
        if (p) {
                memset(p, 0, MEM_ALIGN_SIZE(count * size));
        }

        return p;
}

//==============================================================================
/**
 * @brief  Function return free memory
 * @param  None
 * @return Free memory value
 */
//==============================================================================
u32_t _heap_get_free_heap(void)
{
        return (_HEAP_SIZE - used_mem);
}

/*==============================================================================
  End of file
==============================================================================*/
