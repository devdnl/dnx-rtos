/*=========================================================================*//**
@file    mm.h

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

#ifndef _MM_H_
#define _MM_H_

/*==============================================================================
  Include files
==============================================================================*/
#include "sys/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Exported macros
==============================================================================*/

/*==============================================================================
  Exported object types
==============================================================================*/
typedef struct {
        i32_t kernel_memory_usage;
        i32_t filesystems_memory_usage;
        i32_t network_memory_usage;
        i32_t modules_memory_usage;
        i32_t applications_memory_usage;
} _mm_mem_usage_t;

/*==============================================================================
  Exported objects
==============================================================================*/

/*==============================================================================
  Exported functions
==============================================================================*/
extern void  _mm_init(void);

extern void *_kcalloc(size_t, size_t);
extern void *_kmalloc(size_t);
extern void  _kfree(void*);

extern void *_fscalloc(size_t, size_t);
extern void *_fsmalloc(size_t);
extern void  _fsfree(void*);

extern void *_modcalloc(size_t, size_t, size_t);
extern void *_modmalloc(size_t, size_t);
extern void  _modfree(void*, size_t);

extern void *_netcalloc(size_t, size_t);
extern void *_netmalloc(size_t);
extern void  _netfree(void*);

extern void *_appcalloc(size_t, size_t);
extern void *_appmalloc(size_t);
extern void  _appfree(void*);

extern int   _mm_get_mem_usage(_mm_mem_usage_t*);
extern int   _mm_get_module_mem_usage(uint module, i32_t *usage);

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
