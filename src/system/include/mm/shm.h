/*=========================================================================*//**
File     shm.h

Author   Daniel Zorychta

Brief    Shared memory management.

         Copyright (C) 2016 Daniel Zorychta <daniel.zorychta@gmail.com>

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

/**
@defgroup SHM_H_ SHM_H_

Shared memory management.
*/
/**@{*/

#ifndef _SHM_H_
#define _SHM_H_

/*==============================================================================
  Include files
==============================================================================*/
#include <stdint.h>
#include <stddef.h>
#include <sys/types.h>
#include <config.h>

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Exported macros
==============================================================================*/

/*==============================================================================
  Exported object types
==============================================================================*/

/*==============================================================================
  Exported objects
==============================================================================*/

/*==============================================================================
  Exported functions
==============================================================================*/
#if __OS_ENABLE_SHARED_MEMORY__ > 0
extern int _shm_init(void);
extern int _shm_create(const char *key, size_t size);
extern int _shm_destroy(const char *key);
extern int _shm_attach(const char *key, void **mem, size_t *size, pid_t pid);
extern int _shm_detach(const char *key, pid_t pid);
extern int _shm_detach_anywhere(pid_t pid);
#endif

/*==============================================================================
  Exported inline functions
==============================================================================*/

#ifdef __cplusplus
}
#endif

#endif /* _SHM_H_ */

/**@}*/
/*==============================================================================
  End of file
==============================================================================*/
