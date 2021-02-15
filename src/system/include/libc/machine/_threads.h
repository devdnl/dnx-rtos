/*==============================================================================
File    _threads.h

Author  Daniel Zorychta

Brief   C11 threads library implementation.

        Copyright (C) 2021 Daniel Zorychta <daniel.zorychta@gmail.com>

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

==============================================================================*/

/**
@defgroup _MACHINE__THREADS_H_ _MACHINE__THREADS_H_

Detailed Doxygen description.
*/
/**@{*/

#pragma once

/*==============================================================================
  Include files
==============================================================================*/
#include <kernel/kwrapper.h>

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Exported macros
==============================================================================*/
#define __BEGIN_DECLS
#define __requires_exclusive(_)
#define __requires_unlocked(_)
#define __locks_exclusive(_)
#define __trylocks_exclusive(a, b)
#define __unlocks(_)
#define __END_DECLS

#define ONCE_FLAG_INIT 0

/*==============================================================================
  Exported object types
==============================================================================*/
typedef int once_flag;
typedef sem_t *cnd_t;
typedef mutex_t *mtx_t;
typedef int tss_t;
typedef tid_t thrd_t;

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

/**@}*/
/*==============================================================================
  End of file
==============================================================================*/
