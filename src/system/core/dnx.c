/*=========================================================================*//**
@file    dnx.c

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

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Include files
==============================================================================*/
#include "system/dnx.h"
#include "user/initd.h"

#if (CONFIG_NETWORK_ENABLE != 0)
#       include "arch/ethif.h"
#endif

/*==============================================================================
  Local symbolic constants/macros
==============================================================================*/

/*==============================================================================
  Local types, enums definitions
==============================================================================*/

/*==============================================================================
  Local function prototypes
==============================================================================*/

/*==============================================================================
  Local object definitions
==============================================================================*/

/*==============================================================================
  Exported object definitions
==============================================================================*/

/*==============================================================================
  Function definitions
==============================================================================*/

//==============================================================================
/**
 * @brief Initialize dnx system
 */
//==============================================================================
void dnx_init(void)
{
        static bool initialized = false;

        if (!initialized) {
                _cpuctl_init();
                memman_init();
                _stop_if(vfs_init() != STD_RET_OK);
                _stop_if(sysm_init() != STD_RET_OK);
                new_task(task_initd, INITD_NAME, INITD_STACK_DEPTH, INITD_ARGS);

#if (CONFIG_NETWORK_ENABLE != 0)
                _ethif_start_lwIP_daemon();
#endif
                initialized = true;
        }
}

#ifdef __cplusplus
}
#endif

/*==============================================================================
  End of file
==============================================================================*/
