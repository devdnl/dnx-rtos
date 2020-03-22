/*=========================================================================*//**
@file    heap.c

@author  Daniel Zorychta

@brief   This file support dynamic memory management.

@note    Copyright (C) 2014 Daniel Zorychta <daniel.zorychta@gmail.com>

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
#include "kernel/errno.h"
#include "kernel/khooks.h"
#include "kernel/printk.h"
#include <string.h>

/*==============================================================================
  Local symbolic constants/macros
==============================================================================*/
/**
 * Calculate memory size for an aligned buffer - returns the next highest
 * multiple of MEM_ALIGNMENT (e.g. MEM_ALIGN_SIZE(3) and
 * MEM_ALIGN_SIZE(4) will both yield 4 for MEM_ALIGNMENT == 4).
 */
#define MEM_ALIGN_SIZE(size)            (((size) + _HEAP_ALIGN_ - 1) & ~(_HEAP_ALIGN_-1))

/** some alignment macros: we define them here for better source code layout */
#define BLOCK_MIN_SIZE_ALIGNED          MEM_ALIGN_SIZE(__HEAP_BLOCK_SIZE__)
#define SIZEOF_STRUCT_MEM               MEM_ALIGN_SIZE(sizeof(struct mem))

#if __OS_HEAP_OVERFLOW_CHECK__ == _YES_
#define MEM_SANITY_REGION_BEFORE_ALIGNED 8
#define MEM_SANITY_REGION_AFTER_ALIGNED  8
#else
#define MEM_SANITY_REGION_BEFORE_ALIGNED 0
#define MEM_SANITY_REGION_AFTER_ALIGNED  0
#endif

#define MEM_SANITY_OFFSET               MEM_SANITY_REGION_BEFORE_ALIGNED
#define MEM_SANITY_OVERHEAD             (MEM_SANITY_REGION_BEFORE_ALIGNED + MEM_SANITY_REGION_AFTER_ALIGNED)

#define PROTECT                         _kernel_scheduler_lock
#define UNPROTECT                       _kernel_scheduler_unlock

/*==============================================================================
  Local types, enums definitions
==============================================================================*/
/**
 * The heap is made up as a list of structs of this type.
 * This does not have to be aligned since for getting its size,
 * we only use the macro SIZEOF_STRUCT_MEM, which automatically alignes.
 */
struct mem {
        size_t next;            /**< index (-> ram[next]) of the next struct      */
        size_t prev;            /**< index (-> ram[prev]) of the previous struct  */
        u8_t   used;            /**< 1: this area is used; 0: this area is unused */
#if __OS_HEAP_OVERFLOW_CHECK__ == _YES_
        size_t user_size;       /**< user size used in overflow check */
#endif
};

/*==============================================================================
  Local function prototypes
==============================================================================*/
static void plug_holes(_heap_t *heap, struct mem *mem);

/*==============================================================================
  Local object definitions
==============================================================================*/

/*==============================================================================
  Exported object definitions
==============================================================================*/

/*==============================================================================
  Function definitions
==============================================================================*/
//==============================================================================
/**
 * @brief  Function convert heap pointer to block representation.
 *
 * @param  heap         heap instance
 * @param  ptr          pointer to convert
 *
 * @return Pointer to block.
 */
//==============================================================================
static struct mem *ptr_to_mem(_heap_t *heap, size_t ptr)
{
        return (struct mem *)(void *)&heap->ram[ptr];
}

//==============================================================================
/**
 * @brief  Function convert block representation to heap pointer.
 *
 * @param  heap         heap instance
 * @param  mem          block representation
 *
 * @return Heap pointer.
 */
//==============================================================================
static size_t mem_to_ptr(_heap_t *heap, volatile void *mem)
{
        return (size_t)((u8_t *)mem - heap->ram);
}

//==============================================================================
/**
 * @brief  "Plug holes" by combining adjacent empty struct mems.
 *         After this function is through, there should not exist
 *         one empty struct mem pointing to another empty struct mem.
 *         This assumes access to the heap is protected by the calling function
 *         already.
 *
 * @param  heap       heap object
 * @param  mem        this points to a struct mem which just has been freed
 */
//==============================================================================
static void plug_holes(_heap_t *heap, struct mem *mem)
{
        /* plug hole forward */
        struct mem *nmem = ptr_to_mem(heap, mem->next);

        if (mem != nmem && nmem->used == 0 && (u8_t *)nmem != (u8_t *)heap->ram_end) {
                /* if mem->next is unused and not end of ram, combine mem and mem->next */
                if (heap->lfree == nmem) {
                        heap->lfree = mem;
                }

                mem->next = nmem->next;
                if (nmem->next != heap->size) {
                        ptr_to_mem(heap, nmem->next)->prev = mem_to_ptr(heap, mem);
                }
        }

        /* plug hole backward */
        struct mem *pmem = ptr_to_mem(heap, mem->prev);

        if (pmem != mem && pmem->used == 0) {
                /* if mem->prev is unused, combine mem and mem->prev */
                if (heap->lfree == mem) {
                        heap->lfree = pmem;
                }

                pmem->next = mem->next;
                if (mem->next != heap->size) {
                        ptr_to_mem(heap, mem->next)->prev = mem_to_ptr(heap, pmem);
                }
        }
}

//==============================================================================
/**
 * @brief  Function check if memory block representation is valid.
 *
 * @param  heap         heap instance
 * @param  mem          memory block representation
 *
 * @return 0: link invalid; 1: link valid.
 */
//==============================================================================
static int link_valid(_heap_t *heap, struct mem *mem)
{
        size_t rmem_idx  = mem_to_ptr(heap, mem);
        struct mem *nmem = ptr_to_mem(heap, mem->next);
        struct mem *pmem = ptr_to_mem(heap, mem->prev);

        if (  (mem->next > heap->size) || (mem->prev > heap->size)
           || ((mem->prev != rmem_idx) && (pmem->next != rmem_idx))
           || ((nmem != heap->ram_end) && (nmem->prev != rmem_idx))) {

                return 0;

        } else {
                return 1;
        }
}

//==============================================================================
/**
 * @brief  Function check heap consistency.
 *
 * @param  heap         heap instance
 */
//==============================================================================
static void sanity(_heap_t *heap)
{
#if __OS_HEAP_SANITY_CHECK__ == _YES_
        _assert_msg(MEM_ALIGN_SIZE(heap->size) == heap->size, "HEAP: invalid heap size alignment");

        /* begin with first element here */
        struct mem *mem = (struct mem *)heap->ram;

        _assert_msg((mem->used == 0) || (mem->used == 1), "HEAP: element used invalid");

        u8_t last_used = mem->used;

        _assert_msg(mem->prev == 0, "HEAP: element prev ptr not valid");

        _assert_msg(mem->next <= heap->size, "HEAP: element next ptr invalid");

        _assert_msg(MEM_ALIGN_SIZE(ptr_to_mem(heap, mem->next) == ptr_to_mem(heap, mem->next)),
                    "HEAP: element next ptr unaligned");

        /* check all elements before the end of the heap */
        size_t max = heap->size / SIZEOF_STRUCT_MEM;

        for (mem = ptr_to_mem(heap, mem->next);
             ((u8_t *) mem > heap->ram) && (mem < heap->ram_end);
             mem = ptr_to_mem(heap, mem->next)) {

                _assert_msg(MEM_ALIGN_SIZE((uintptr_t)mem) == (uintptr_t)mem, "HEAP: element unaligned");

                _assert_msg(mem->prev <= heap->size, "HEAP: element prev ptr invalid");

                _assert_msg(mem->next <= heap->size, "HEAP: element next ptr invalid");

                _assert_msg(MEM_ALIGN_SIZE(ptr_to_mem(heap, mem->prev) == ptr_to_mem(heap, mem->prev)),
                            "HEAP: element prev ptr unaligned");

                _assert_msg(MEM_ALIGN_SIZE(ptr_to_mem(heap, mem->next) == ptr_to_mem(heap, mem->next)),
                            "HEAP: element next ptr unaligned");

                if (last_used == 0) {
                        /* 2 unused elements in a row? */
                        _assert_msg(mem->used == 1, "HEAP: 2 element unused in row?");
                } else {
                        _assert_msg((mem->used == 0) || (mem->used == 1), "HEAP: element used invalid");
                }

                _assert_msg(link_valid(heap, mem), "HEAP: element link invalid");

                /* used/unused altering */
                last_used = mem->used;

                _assert_msg(--max > 0, "HEAP: sanity: dead loop");
        }

        _assert_msg(mem == ptr_to_mem(heap, heap->size), "HEAP: end ptr sanity");
        _assert_msg(mem->used == 1, "HEAP: element used invalid");
        _assert_msg(mem->prev == heap->size, "HEAP: element prev ptr invalid");
        _assert_msg(mem->next == heap->size, "HEAP: element next ptr invalid");
#else
        (void)(heap);
#endif
}

#if __OS_HEAP_OVERFLOW_CHECK__ == _YES_
//==============================================================================
/**
 * @brief  Function check block overflow.
 *
 * @param  p            pointer to examine
 * @param  size         allocation size
 */
//==============================================================================
static void overflow_check_raw(void *p, size_t size)
{
        u8_t *m = (u8_t *) p + size;
        for (u16_t k = 0; k < MEM_SANITY_REGION_AFTER_ALIGNED; k++) {
                if (m[k] != 0xcd) {
                        _printk("HEAP: block overflow @ %p", p);
                        break;
                }
        }

        m = (u8_t *) p - MEM_SANITY_REGION_BEFORE_ALIGNED;
        for (u16_t k = 0; k < MEM_SANITY_REGION_BEFORE_ALIGNED; k++) {
                if (m[k] != 0xcd) {
                        _printk("HEAP: block underflow @ %p", p);
                        break;
                }
        }
}

//==============================================================================
/**
 * @brief  Function initialize overflow parameters.
 *
 * @param  p            block
 * @param  size         alloc size
 */
//==============================================================================
static void mem_overflow_init_raw(void *p, size_t size)
{
        u8_t *m = (u8_t *) p - MEM_SANITY_REGION_BEFORE_ALIGNED;
        memset(m, 0xcd, MEM_SANITY_REGION_BEFORE_ALIGNED);

        m = (u8_t *) p + size;
        memset(m, 0xcd, MEM_SANITY_REGION_AFTER_ALIGNED);
}
#endif

//==============================================================================
/**
 * @brief  Function initialize overflow parameters of memory block representation.
 *
 * @param  mem          memory block
 * @param  user_size    allocated size
 */
//==============================================================================
static void overflow_init_element(struct mem *mem, size_t user_size)
{
#if __OS_HEAP_OVERFLOW_CHECK__ == _YES_
        void *p = (u8_t *) mem + SIZEOF_STRUCT_MEM + MEM_SANITY_OFFSET;
        mem->user_size = user_size;
        mem_overflow_init_raw(p, user_size);
#else
        (void)mem;
        (void)user_size;
#endif
}

//==============================================================================
/**
 * @brief  Function check if block overflow occurred.
 *
 * @param  mem          memory block
 */
//==============================================================================
static void overflow_check_element(struct mem *mem)
{
#if __OS_HEAP_OVERFLOW_CHECK__ == _YES_
        void *p = (u8_t *) mem + SIZEOF_STRUCT_MEM + MEM_SANITY_OFFSET;
        overflow_check_raw(p, mem->user_size);
#else
        (void)mem;
#endif
}


//==============================================================================
/**
* @brief  Zero the heap and initialize start, end and lowest-free
*
* @param  heap          heap object
* @param  start         memory start address
* @param  size          memory size
*
* @return One of errno value.
*/
//==============================================================================
int _heap_init(_heap_t *heap, void *start, size_t size)
{
        int err = EINVAL;

        if (heap && start && size) {
                heap->used     = 0;
                heap->used_max = 0;

                /* align the heap */
                heap->ram = start;

                /* aligned memory size */
                heap->size = MEM_ALIGN_SIZE(size - SIZEOF_STRUCT_MEM - MEM_SANITY_OFFSET);

                /* initialize the start of the heap */
                struct mem *mem = (struct mem *)heap->ram;
                mem->next = heap->size;
                mem->prev = 0;
                mem->used = 0;

                /* initialize the end of the heap */
                heap->ram_end = ptr_to_mem(heap, heap->size);
                heap->ram_end->used = 1;
                heap->ram_end->next = heap->size;
                heap->ram_end->prev = heap->size;

                sanity(heap);

                /* initialize the lowest-free pointer to the start of the heap */
                heap->lfree = (struct mem *)heap->ram;

                err = ESUCC;
        }

        return err;
}

//==============================================================================
/**
 * @brief  Put a struct mem back on the heap
 *
 * @param  heap         heap object
 * @param  rmem         is the data portion of a struct mem as returned by a previous
 *                      call to _heap_alloc()
 * @param  freed        freed block size (can be NULL)
 */
//==============================================================================
void _heap_free(_heap_t *heap, void *rmem, size_t *freed)
{
        if (!heap || !rmem) {
                return;
        }

        PROTECT();

        if ((((uintptr_t)rmem) & (_HEAP_ALIGN_ - 1)) != 0) {
                _printk("HEAP: free: unaligned pointer");
                UNPROTECT();
                return;
        }

        struct mem *mem = (struct mem *)(void *)((u8_t *)rmem - (SIZEOF_STRUCT_MEM + MEM_SANITY_OFFSET));
        if ((u8_t *)mem < heap->ram || (u8_t *)rmem + BLOCK_MIN_SIZE_ALIGNED > (u8_t *)heap->ram_end) {
                _printk("HEAP: free: illegal pointer");
                UNPROTECT();
                return;
        }

        overflow_check_element(mem);

        if (mem->used > 1) {
                _printk("HEAP: free: invalid usage indicator");
                UNPROTECT();
                return;
        }

        if (!mem->used) {
                _printk("HEAP: free: block unused");
                UNPROTECT();
                return;
        }

        if (!link_valid(heap, mem)) {
                _printk("HEAP: free: invalid link");
                UNPROTECT();
                return;
        }

        mem->used = 0;

        if (mem < heap->lfree) {
                /* the newly freed struct is now the lowest */
                heap->lfree = mem;
        }

        size_t blksize = (mem->next - (size_t)(((u8_t *)mem - heap->ram)));
        heap->used -= blksize;
        if (freed) *freed = blksize;

        plug_holes(heap, mem);

        sanity(heap);

        UNPROTECT();
}

//==============================================================================
/**
 * @brief  Allocate a block of memory with a minimum of 'size' bytes.
 *         Note that the returned value will always be aligned (as defined by MEM_ALIGNMENT).
 *
 * @param  heap         heap object
 * @param  size         is the minimum size of the requested block in bytes.
 * @param  allocated    real size of allocated block (it can be bigger than size)

 * @return Pointer to allocated memory or NULL if no free memory was found.
 */
//==============================================================================
void *_heap_alloc(_heap_t *heap, size_t size_in, size_t *allocated)
{
        size_t ptr, ptr2, size;
        struct mem *mem, *mem2;
        size_t used;

        if (!heap || size_in == 0) {
                return NULL;
        }

        /* Expand the size of the allocated memory region so that we can adjust for alignment. */
        size = MEM_ALIGN_SIZE(size_in);
        if (size < BLOCK_MIN_SIZE_ALIGNED) {
                /* every data block must be at least BLOCK_MIN_SIZE_ALIGNED long */
                size = BLOCK_MIN_SIZE_ALIGNED;
        }

        size += MEM_SANITY_REGION_BEFORE_ALIGNED + MEM_SANITY_REGION_AFTER_ALIGNED;

        if (size > heap->size || size < size_in) {
                return NULL;
        }

        /* protect the heap from concurrent access */
        PROTECT();

        /*
         * Scan through the heap searching for a free block that is big enough,
         * beginning with the lowest free block.
         */
        for (ptr = mem_to_ptr(heap, heap->lfree); ptr < heap->size - size; ptr = ptr_to_mem(heap, ptr)->next) {

                mem = ptr_to_mem(heap, ptr);

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
                                mem2 = ptr_to_mem(heap, ptr2);
                                mem2->used = 0;
                                mem2->next = mem->next;
                                mem2->prev = ptr;
                                /* and insert it between mem and mem->next */
                                mem->next = ptr2;
                                mem->used = 1;

                                if (mem2->next != heap->size) {
                                        ptr_to_mem(heap, mem2->next)->prev = ptr2;
                                }

                                used = (size + SIZEOF_STRUCT_MEM);
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

                                used = (mem->next - mem_to_ptr(heap, mem));
                        }

                        if (mem == heap->lfree) {
                                volatile struct mem *cur = heap->lfree;

                                /* Find next free block after mem and update lowest free pointer */
                                while (cur->used && cur != heap->ram_end) {
                                        cur = ptr_to_mem(heap, cur->next);
                                }

                                heap->lfree = cur;
                        }

                        heap->used    += used;
                        heap->used_max = heap->used_max < heap->used ? heap->used : heap->used_max;
                        if (allocated) *allocated = used;

                        overflow_init_element(mem, size_in);

                        sanity(heap);

                        UNPROTECT();

                        return (u8_t *)mem + SIZEOF_STRUCT_MEM + MEM_SANITY_OFFSET;
                }
        }

        UNPROTECT();

        return NULL;
}

//==============================================================================
/**
 * @brief  Function return free heap
 *
 * @param  heap         heap object
 *
 * @return Free heap value
 */
//==============================================================================
size_t _heap_get_free(_heap_t *heap)
{
        size_t size = 0;

        PROTECT();
        size = (heap->size - heap->used);
        UNPROTECT();

        return size;
}

//==============================================================================
/**
 * @brief  Function return used heap
 *
 * @param  heap         heap object
 *
 * @return Use heap value
 */
//==============================================================================
size_t _heap_get_used(_heap_t *heap)
{
        return heap->used;
}

//==============================================================================
/**
 * @brief  Function return heap size
 *
 * @param  heap         heap object
 *
 * @return Heap size
 */
//==============================================================================
size_t _heap_get_size(_heap_t *heap)
{
        return heap->size + SIZEOF_STRUCT_MEM;
}

//==============================================================================
/**
 * @brief  Function return size of selected block
 *
 * @param  heap     heap object
 * @param  rmem     memory block
 *
 * @return Block size, 0 on error
 */
//==============================================================================
size_t _heap_get_block_size(_heap_t *heap, void *rmem)
{
        if (!heap || !rmem) {
                return 0;
        }

        if ((((uintptr_t)rmem) & (_HEAP_ALIGN_ - 1)) != 0) {
                _printk("HEAP: blksize: unaligned pointer");
                return 0 ;
        }

        struct mem *mem = (struct mem *)(void *)((u8_t *)rmem - (SIZEOF_STRUCT_MEM + MEM_SANITY_OFFSET));
        if ((u8_t *)mem < heap->ram || (u8_t *)rmem + BLOCK_MIN_SIZE_ALIGNED > (u8_t *)heap->ram_end) {
                _printk("HEAP: blksize: illegal pointer");
                return 0;
        }

        PROTECT();

        if (mem->used > 1) {
                UNPROTECT();
                _printk("HEAP: blksize: invalid usage indicator");
                return 0;
        }

        if (!mem->used) {
                UNPROTECT();
                _printk("HEAP: blksize: block unused");
                return 0;
        }

        if (!link_valid(heap, mem)) {
                UNPROTECT();
                _printk("HEAP: blksize: invalid link");
                return 0;
        }

        size_t blksize = (mem->next - (size_t)(((u8_t *)mem - heap->ram)));

        UNPROTECT();

        return blksize;
}

/*==============================================================================
  End of file
==============================================================================*/
