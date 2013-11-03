/*=========================================================================*//**
@file    sys_arch.h

@author  Daniel Zorychta

@brief

@note    Copyright (C) 2013 Daniel Zorychta <daniel.zorychta@gmail.com>

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

#ifndef _SYS_ARCH_H_
#define _SYS_ARCH_H_

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Include files
==============================================================================*/
#include "system/dnx.h"

/*==============================================================================
  Exported macros
=============================================================================*/
#define SYS_MBOX_NULL                   NULL
#define SYS_SEM_NULL                    NULL

extern void _ethif_manager(void);
#define LWIP_TCPIP_THREAD_ALIVE()       _ethif_manager()

/*==============================================================================
  Exported object types
==============================================================================*/
typedef mutex_t         *sys_mutex_t;
typedef sem_t           *sys_sem_t;
typedef queue_t         *sys_mbox_t;
typedef task_t          *sys_thread_t;
typedef int              sys_prot_t;

/*==============================================================================
  Exported objects
==============================================================================*/

/*==============================================================================
  Exported functions
==============================================================================*/

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
