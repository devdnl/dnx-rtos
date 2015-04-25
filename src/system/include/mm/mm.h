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
        i32_t static_memory_usage;
        i32_t kernel_memory_usage;
        i32_t filesystems_memory_usage;
        i32_t network_memory_usage;
        i32_t modules_memory_usage;
        i32_t programs_memory_usage;
} _mm_mem_usage_t;

enum _mm_mem {
        _MM_KRN,        //!< allocate memory for kernel purposes
        _MM_FS,         //!< allocate memory for file system purposes
        _MM_NET,        //!< allocate memory for network purposes
        _MM_PROG,       //!< allocate memory for program purposes
        _MM_MOD,        //!< allocate memorr for modules purposes
        _MM_COUNT
};

/*==============================================================================
  Exported objects
==============================================================================*/

/*==============================================================================
  Exported functions
==============================================================================*/
extern int _mm_init(void);
extern int _kzalloc(enum _mm_mem, const size_t, void**, ...);
extern int _kmalloc(enum _mm_mem, const size_t, void**, ...);
extern int _kfree(enum _mm_mem, void**, ...);
extern int _mm_get_mem_usage(_mm_mem_usage_t*);
extern int _mm_get_module_mem_usage(uint module, i32_t *usage);

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
