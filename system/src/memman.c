/*=============================================================================================*//**
@file    memman.c

@author  Daniel Zorychta

@brief   This file provide system initialisation and RTOS start.

@note    Copyright (C) 2012  Daniel Zorychta <daniel.zorychta@gmail.com>

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


*//*==============================================================================================*/
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
*//*==============================================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*==================================================================================================
                                             Include files
==================================================================================================*/
#include "memman.h"
#include <string.h>


/*==================================================================================================
                                   Local symbolic constants/macros
==================================================================================================*/
/** HEAP size */
#define HEAP_SIZE configTOTAL_HEAP_SIZE

/** MEM_ALIGNMENT: should be set to the alignment of the CPU for which
   lwIP is compiled. 4 byte alignment -> define MEM_ALIGNMENT to 4, 2
   byte alignment -> define MEM_ALIGNMENT to 2. */
#define MEM_ALIGNMENT           4

/** Calculate memory size for an aligned buffer - returns the next highest
 * multiple of MEM_ALIGNMENT (e.g. MEM_ALIGN_SIZE(3) and
 * MEM_ALIGN_SIZE(4) will both yield 4 for MEM_ALIGNMENT == 4).
 */
#define MEM_ALIGN_SIZE(size) (((size) + MEM_ALIGNMENT - 1) & ~(MEM_ALIGNMENT-1))

/** Calculate safe memory size for an aligned buffer when using an unaligned
 * type as storage. This includes a safety-margin on (MEM_ALIGNMENT - 1) at the
 * start (e.g. if buffer is u8_t[] and actual data will be u32_t*)
 */
#define MEM_ALIGN_BUFFER(size) (((size) + MEM_ALIGNMENT - 1))

/** Align a memory pointer to the alignment defined by MEM_ALIGNMENT
 * so that ADDR % MEM_ALIGNMENT == 0
 */
#define MEM_ALIGN(addr) ((void *)(((ptr_t)(addr) + MEM_ALIGNMENT - 1) & ~(ptr_t)(MEM_ALIGNMENT-1)))

/** All allocated blocks will be BLOCK_MIN_SIZE bytes big, at least!
 * BLOCK_MIN_SIZE can be overridden to suit your needs. Smaller values save space,
 * larger values could prevent too small blocks to fragment the RAM too much. */
#define BLOCK_MIN_SIZE             12

/** some alignment macros: we define them here for better source code layout */
#define BLOCK_MIN_SIZE_ALIGNED      MEM_ALIGN_SIZE(BLOCK_MIN_SIZE)
#define SIZEOF_STRUCT_MEM           MEM_ALIGN_SIZE(sizeof(struct mem))
#define MEM_SIZE_ALIGNED            MEM_ALIGN_SIZE(HEAP_SIZE)

/** heap protection */
#define MEM_FREE_PROTECT()          TaskEnterCritical()
#define MEM_FREE_UNPROTECT()        TaskExitCritical()

/** heap protection */
#define MEM_ALLOC_PROTECT()         TaskEnterCritical()
#define MEM_ALLOC_UNPROTECT()       TaskExitCritical()

/** RAM usage modifications */
#define MEM_STATS_INC_USED(size)    used_mem += size
#define MEM_STATS_DEC_USED(size)    used_mem -= size


/*==================================================================================================
                                   Local types, enums definitions
==================================================================================================*/
/** type used as pointer */
typedef u32_t ptr_t;

/**
 * The heap is made up as a list of structs of this type.
 * This does not have to be aligned since for getting its size,
 * we only use the macro SIZEOF_STRUCT_MEM, which automatically alignes.
 */
struct mem {
  /** index (-> ram[next]) of the next struct */
  size_t next;
  /** index (-> ram[prev]) of the previous struct */
  size_t prev;
  /** 1: this area is used; 0: this area is unused */
  u8_t used;
};


/*==================================================================================================
                                      Local function prototypes
==================================================================================================*/
static void plug_holes(struct mem *mem);


/*==================================================================================================
                                      Local object definitions
==================================================================================================*/
/** If you want to relocate the heap to external memory, simply define
 * ram_heap as a void-pointer to that location.
 * If so, make sure the memory at that location is big enough (see below on
 * how that space is calculated). */
/** the heap. we need one struct mem at the end and some room for alignment */
static u8_t ram_heap[MEM_SIZE_ALIGNED + (2*SIZEOF_STRUCT_MEM) + MEM_ALIGNMENT];

/** pointer to the heap (ram_heap): for alignment, ram is now a pointer instead of an array */
static u8_t *ram;

/** the last entry, always unused! */
static struct mem *ram_end;

/** pointer to the lowest free block, this is used for faster search */
static struct mem *lfree;

/** RAM usage */
static u32_t used_mem;


/*==================================================================================================
                                     Exported object definitions
==================================================================================================*/


/*==================================================================================================
                                         Function definitions
==================================================================================================*/

//================================================================================================//
/**
 * "Plug holes" by combining adjacent empty struct mems.
 * After this function is through, there should not exist
 * one empty struct mem pointing to another empty struct mem.
 *
 * @param mem this points to a struct mem which just has been freed
 * @internal this function is only called by mem_free() and mem_trim()
 *
 * This assumes access to the heap is protected by the calling function
 * already.
 */
//================================================================================================//
static void plug_holes(struct mem *mem)
{
      struct mem *nmem;
      struct mem *pmem;

      /* plug hole forward */
      nmem = (struct mem *)(void *)&ram[mem->next];

      if (mem != nmem && nmem->used == 0 && (u8_t *)nmem != (u8_t *)ram_end)
      {
            /* if mem->next is unused and not end of ram, combine mem and mem->next */
            if (lfree == nmem)
            {
                  lfree = mem;
            }

            mem->next = nmem->next;

            ((struct mem *)(void *)&ram[nmem->next])->prev = (size_t)((u8_t *)mem - ram);
      }

      /* plug hole backward */
      pmem = (struct mem *)(void *)&ram[mem->prev];

      if (pmem != mem && pmem->used == 0)
      {
            /* if mem->prev is unused, combine mem and mem->prev */
            if (lfree == mem)
            {
                  lfree = pmem;
            }

            pmem->next = mem->next;

            ((struct mem *)(void *)&ram[mem->next])->prev = (size_t)((u8_t *)pmem - ram);
      }
}


//================================================================================================//
/**
* @brief Zero the heap and initialize start, end and lowest-free
*/
//================================================================================================//
void memman_init(void)
{
      struct mem *mem;

      /* align the heap */
      ram = (u8_t *)MEM_ALIGN(ram_heap);

      /* initialize the start of the heap */
      mem = (struct mem *)(void *)ram;
      mem->next = MEM_SIZE_ALIGNED;
      mem->prev = 0;
      mem->used = 0;

      /* initialize the end of the heap */
      ram_end = (struct mem *)(void *)&ram[MEM_SIZE_ALIGNED];
      ram_end->used = 1;
      ram_end->next = MEM_SIZE_ALIGNED;
      ram_end->prev = MEM_SIZE_ALIGNED;

      /* initialize the lowest-free pointer to the start of the heap */
      lfree = (struct mem *)(void *)ram;

      //  if(sys_mutex_new(&mem_mutex) != ERR_OK) {
      //    LWIP_ASSERT("failed to create mem_mutex", 0);
      //  }
}


//================================================================================================//
/**
 * @brief Put a struct mem back on the heap
 *
 * @param rmem is the data portion of a struct mem as returned by a previous
 *             call to mem_malloc()
 */
//================================================================================================//
void Free(void *rmem)
{
      struct mem *mem;
      //  MEM_FREE_DECL_PROTECT();

      if (rmem == NULL)
      {
            return;
      }

      if ((u8_t *)rmem < (u8_t *)ram || (u8_t *)rmem >= (u8_t *)ram_end)
      {
            return;
      }

      /* protect the heap from concurrent access */
      MEM_FREE_PROTECT();

      /* Get the corresponding struct mem ... */
      mem = (struct mem *)(void *)((u8_t *)rmem - SIZEOF_STRUCT_MEM);
      /* ... which has to be in a used state ... */
      /* ... and is now unused. */
      mem->used = 0;

      if (mem < lfree)
      {
            /* the newly freed struct is now the lowest */
            lfree = mem;
      }

      MEM_STATS_DEC_USED(mem->next - (size_t)(((u8_t *)mem - ram)));

      /* finally, see if prev or next are free also */
      plug_holes(mem);

      MEM_FREE_UNPROTECT();
}


//================================================================================================//
/**
 * @brief Shrink memory returned by mem_malloc().
 *
 * @param rmem pointer to memory allocated by mem_malloc the is to be shrinked
 * @param newsize required size after shrinking (needs to be smaller than or
 *                equal to the previous size)
 * @return for compatibility reasons: is always == rmem, at the moment
 *         or NULL if newsize is > old size, in which case rmem is NOT touched
 *         or freed!
 */
//================================================================================================//
void *Trim(void *rmem, size_t newsize)
{
      size_t size;
      size_t ptr, ptr2;
      struct mem *mem, *mem2;

      /* Expand the size of the allocated memory region so that we can adjust for alignment. */
      newsize = MEM_ALIGN_SIZE(newsize);

      if(newsize < BLOCK_MIN_SIZE_ALIGNED)
      {
            /* every data block must be at least BLOCK_MIN_SIZE_ALIGNED long */
            newsize = BLOCK_MIN_SIZE_ALIGNED;
      }

      if (newsize > MEM_SIZE_ALIGNED)
      {
            return NULL;
      }

      if ((u8_t *)rmem < (u8_t *)ram || (u8_t *)rmem >= (u8_t *)ram_end)
      {
            return rmem;
      }
      /* Get the corresponding struct mem ... */
      mem = (struct mem *)(void *)((u8_t *)rmem - SIZEOF_STRUCT_MEM);

      /* ... and its offset pointer */
      ptr = (size_t)((u8_t *)mem - ram);

      size = mem->next - ptr - SIZEOF_STRUCT_MEM;

      if (newsize > size)
      {
            /* not supported */
            return NULL;
      }

      if (newsize == size)
      {
            /* No change in size, simply return */
            return rmem;
      }

      /* protect the heap from concurrent access */
      MEM_FREE_PROTECT();

      mem2 = (struct mem *)(void *)&ram[mem->next];

      if(mem2->used == 0)
      {
            /* The next struct is unused, we can simply move it at little */
            size_t next;

            /* remember the old next pointer */
            next = mem2->next;

            /* create new struct mem which is moved directly after the shrinked mem */
            ptr2 = ptr + SIZEOF_STRUCT_MEM + newsize;

            if (lfree == mem2)
            {
                  lfree = (struct mem *)(void *)&ram[ptr2];
            }

            mem2 = (struct mem *)(void *)&ram[ptr2];
            mem2->used = 0;

            /* restore the next pointer */
            mem2->next = next;

            /* link it back to mem */
            mem2->prev = ptr;

            /* link mem to it */
            mem->next = ptr2;

            /* last thing to restore linked list: as we have moved mem2,
            * let 'mem2->next->prev' point to mem2 again. but only if mem2->next is not
            * the end of the heap */
            if (mem2->next != MEM_SIZE_ALIGNED)
            {
                  ((struct mem *)(void *)&ram[mem2->next])->prev = ptr2;
            }

            MEM_STATS_DEC_USED(size - newsize);
            /* no need to plug holes, we've already done that */
      }
      else if (newsize + SIZEOF_STRUCT_MEM + BLOCK_MIN_SIZE_ALIGNED <= size)
      {
            /* Next struct is used but there's room for another struct mem with
            * at least BLOCK_MIN_SIZE_ALIGNED of data.
            * Old size ('size') must be big enough to contain at least 'newsize' plus a struct mem
            * ('SIZEOF_STRUCT_MEM') with some data ('BLOCK_MIN_SIZE_ALIGNED').
            * @todo we could leave out BLOCK_MIN_SIZE_ALIGNED. We would create an empty
            *       region that couldn't hold data, but when mem->next gets freed,
            *       the 2 regions would be combined, resulting in more free memory */
            ptr2 = ptr + SIZEOF_STRUCT_MEM + newsize;
            mem2 = (struct mem *)(void *)&ram[ptr2];

            if (mem2 < lfree)
            {
                  lfree = mem2;
            }

            mem2->used = 0;
            mem2->next = mem->next;
            mem2->prev = ptr;
            mem->next = ptr2;

            if (mem2->next != MEM_SIZE_ALIGNED)
            {
                  ((struct mem *)(void *)&ram[mem2->next])->prev = ptr2;
            }

            MEM_STATS_DEC_USED(size - newsize);
            /* the original mem->next is used, so no need to plug holes! */
      }

      MEM_FREE_UNPROTECT();

      return rmem;
}


//================================================================================================//
/**
 * Adam's mem_malloc() plus solution for bug #17922
 * Allocate a block of memory with a minimum of 'size' bytes.
 *
 * @param size is the minimum size of the requested block in bytes.
 * @return pointer to allocated memory or NULL if no free memory was found.
 *
 * Note that the returned value will always be aligned (as defined by MEM_ALIGNMENT).
 */
//================================================================================================//
void *Malloc(size_t size)
{
      size_t ptr, ptr2;
      struct mem *mem, *mem2;

      if (size == 0)
      {
            return NULL;
      }

      /* Expand the size of the allocated memory region so that we can adjust for alignment. */
      size = MEM_ALIGN_SIZE(size);

      if(size < BLOCK_MIN_SIZE_ALIGNED)
      {
            /* every data block must be at least BLOCK_MIN_SIZE_ALIGNED long */
            size = BLOCK_MIN_SIZE_ALIGNED;
      }

      if (size > MEM_SIZE_ALIGNED)
      {
            return NULL;
      }

      /* protect the heap from concurrent access */
      //  sys_mutex_lock(&mem_mutex);
      MEM_ALLOC_PROTECT();

      /* Scan through the heap searching for a free block that is big enough,
      * beginning with the lowest free block.
      */
      for (ptr = (size_t)((u8_t *)lfree - ram);
           ptr < MEM_SIZE_ALIGNED - size;
           ptr = ((struct mem *)(void *)&ram[ptr])->next)
      {
           mem = (struct mem *)(void *)&ram[ptr];

            if ((!mem->used) && (mem->next - (ptr + SIZEOF_STRUCT_MEM)) >= size)
            {
                  /* mem is not used and at least perfect fit is possible:
                   * mem->next - (ptr + SIZEOF_STRUCT_MEM) gives us the 'user data size' of mem */

                  if (mem->next - (ptr + SIZEOF_STRUCT_MEM) >= (size + SIZEOF_STRUCT_MEM + BLOCK_MIN_SIZE_ALIGNED))
                  {
                        /* (in addition to the above, we test if another struct mem (SIZEOF_STRUCT_MEM) containing
                        * at least BLOCK_MIN_SIZE_ALIGNED of data also fits in the 'user data space' of 'mem')
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
                        mem2 = (struct mem *)(void *)&ram[ptr2];
                        mem2->used = 0;
                        mem2->next = mem->next;
                        mem2->prev = ptr;

                        /* and insert it between mem and mem->next */
                        mem->next = ptr2;
                        mem->used = 1;

                        if (mem2->next != MEM_SIZE_ALIGNED)
                        {
                              ((struct mem *)(void *)&ram[mem2->next])->prev = ptr2;
                        }

                        MEM_STATS_INC_USED(size + SIZEOF_STRUCT_MEM);
                  }
                  else
                  {
                        /* (a mem2 struct does no fit into the user data space of mem and mem->next will always
                        * be used at this point: if not we have 2 unused structs in a row, plug_holes should have
                        * take care of this).
                        * -> near fit or excact fit: do not split, no mem2 creation
                        * also can't move mem->next directly behind mem, since mem->next
                        * will always be used at this point!
                        */
                        mem->used = 1;

                        MEM_STATS_INC_USED(mem->next - (size_t)((u8_t *)mem - ram));
                  }

                  if (mem == lfree)
                  {
                        /* Find next free block after mem and update lowest free pointer */
                        while (lfree->used && lfree != ram_end)
                        {
                              MEM_ALLOC_UNPROTECT();

                              /* prevent high interrupt latency... */
                              MEM_ALLOC_PROTECT();
                              lfree = (struct mem *)(void *)&ram[lfree->next];
                        }
                  }

                  MEM_ALLOC_UNPROTECT();

                  //        sys_mutex_unlock(&mem_mutex);

                  return (u8_t *)mem + SIZEOF_STRUCT_MEM;
            }
      }

      MEM_ALLOC_UNPROTECT();
      //  sys_mutex_unlock(&mem_mutex);

      return NULL;
}


//================================================================================================//
/**
 * Contiguously allocates enough space for count objects that are size bytes
 * of memory each and returns a pointer to the allocated memory.
 *
 * The allocated memory is filled with bytes of value zero.
 *
 * @param count number of objects to allocate
 * @param size size of the objects to allocate
 * @return pointer to allocated memory / NULL pointer if there is an error
 */
//================================================================================================//
void *Calloc(size_t count, size_t size)
{
      void *p;

      /* allocate 'count' objects of size 'size' */
      p = Malloc(count * size);

      if (p)
      {
            /* zero the memory */
            memset(p, 0, count * size);
      }

      return p;
}


//================================================================================================//
/**
 * @brief Function return free memory
 *
 * @return free memory count
 */
//================================================================================================//
u32_t GetFreeHeapSize(void)
{
      return (HEAP_SIZE - used_mem);
}


#ifdef __cplusplus
}
#endif

/*==================================================================================================
                                            End of file
==================================================================================================*/
