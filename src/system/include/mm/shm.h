/*=========================================================================*//**
File     shm.h

Author   Daniel Zorychta

Brief    Shared memory management.

         Copyright (C) 2017 Daniel Zorychta <daniel.zorychta@gmail.com>

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
