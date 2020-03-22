/*=========================================================================*//**
@file    sys_arch.h

@author  Daniel Zorychta

@brief

@note    Copyright (C) 2013 Daniel Zorychta <daniel.zorychta@gmail.com>

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

#ifndef _SYS_ARCH_H_
#define _SYS_ARCH_H_

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Include files
==============================================================================*/
#include <dnx/os.h>
#include "kernel/sysfunc.h"

/*==============================================================================
  Exported macros
=============================================================================*/
#define SYS_MBOX_NULL                   NULL
#define SYS_SEM_NULL                    NULL

/*==============================================================================
  Exported object types
==============================================================================*/
typedef tid_t            sys_thread_t;
typedef int              sys_prot_t;
typedef mutex_t         *sys_mtx_t;
typedef sem_t           *sys_sem_t;
typedef queue_t         *sys_mbox_t;

/*==============================================================================
  Exported objects
==============================================================================*/

/*==============================================================================
  Exported functions
==============================================================================*/
extern void sys_free(void *mem);
extern void *sys_malloc(size_t blksz);
extern void *sys_calloc(size_t n, size_t blksz);
extern int sys_rand(void);

/*==============================================================================
  Exported inline functions
==============================================================================*/

#ifdef __cplusplus
}
#endif

#endif /* _SYS_ARCH_H_ */
/*==============================================================================
  End of file
==============================================================================*/
