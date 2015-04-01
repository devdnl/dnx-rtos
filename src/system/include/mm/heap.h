/*=========================================================================*//**
@file    heap.h

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
*
*//*==========================================================================*/

#ifndef _HEAP_H_
#define _HEAP_H_

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Include files
==============================================================================*/
#include <sys/types.h>
#include <stddef.h>

/*==============================================================================
  Exported symbolic constants/macros
==============================================================================*/
#define _HEAP_RAM_SIZE        ((size_t)&__ram_size)
#define _HEAP_HEAP_SIZE       ((size_t)&__heap_size)
#define _HEAP_HEAP_START      ((void *)&__heap_start)

/*==============================================================================
  Exported types, enums definitions
==============================================================================*/
/**
 * @brief  Type define the function that is called when a block is successfully allocated or freed.
 * @param  arg          user defined argument
 * @param  size         size of allocated (positive value) or freed (negative value) block
 * @return None
 */
typedef void (*_heap_func_t)(void *arg, i32_t size);

/*==============================================================================
  Exported object declarations
==============================================================================*/
extern void *__ram_size;
extern void *__heap_size;
extern void *__heap_start;

/*==============================================================================
  Exported function prototypes
==============================================================================*/
extern void   _heap_init(void);
extern void  *_heap_malloc(size_t, _heap_func_t, void*);
extern void  *_heap_calloc(size_t count, size_t size, _heap_func_t, void*);
extern void   _heap_free(void *mem, _heap_func_t, void*);
extern u32_t  _heap_get_free_heap(void);

#ifdef __cplusplus
}
#endif

#endif /* _HEAP_H_ */
/*==============================================================================
  End of file
==============================================================================*/
