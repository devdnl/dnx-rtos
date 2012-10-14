#ifndef __MEMMAN_H__
#define __MEMMAN_H__
/*=============================================================================================*//**
@file    printf.h

@author  Daniel Zorychta

@brief   This file support message printing

@note    Copyright (C) 2012 Daniel Zorychta <daniel.zorychta@gmail.com>

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
*
* Modified: Daniel Zorychta
*//*==============================================================================================*/


#ifdef __cplusplus
extern "C" {
#endif

/*==================================================================================================
                                            Include files
==================================================================================================*/
#include "basic_types.h"
#include "FreeRTOS.h"
#include "task.h"


/*==================================================================================================
                                 Exported symbolic constants/macros
==================================================================================================*/
/** HEAP size */
#define MEMMAN_HEAP_SIZE                   (size_t)(63*1024)


/*==================================================================================================
                                  Exported types, enums definitions
==================================================================================================*/


/*==================================================================================================
                                     Exported object declarations
==================================================================================================*/


/*==================================================================================================
                                     Exported function prototypes
==================================================================================================*/
extern void  memman_init(void);
extern void  *Malloc(size_t size);
extern void  *Calloc(size_t count, size_t size);
extern void  *Trim(void *rmem, size_t newsize);
extern void  Free(void *mem);
extern u32_t GetFreeHeapSize(void);

#ifdef __cplusplus
}
#endif

#endif /* __MEMMAN_H__ */

/*==================================================================================================
                                            End of file
==================================================================================================*/
