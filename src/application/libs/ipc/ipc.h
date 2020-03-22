/*==============================================================================
File    ipc.h

Author  Daniel Zorychta

Brief   IPC library.

        Copyright (C) 2018 Daniel Zorychta <daniel.zorychta@gmail.com>

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
@defgroup IPC_H_ IPC_H_

Detailed Doxygen description.
*/
/**@{*/

#ifndef _IPC_H_
#define _IPC_H_

/*==============================================================================
  Include files
==============================================================================*/
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <dnx/thread.h>

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Exported macros
==============================================================================*/

/*==============================================================================
  Exported object types
==============================================================================*/
/**
 * Host object representation.
 */
typedef struct ipc_host ipc_host_t;

/**
 * Client object representation.
 */
typedef struct ipc_client ipc_client_t;

/*==============================================================================
  Exported objects
==============================================================================*/

/*==============================================================================
  Exported functions
==============================================================================*/
extern int   ipc_host_create(ipc_host_t**, size_t);
extern void  ipc_host_destroy(ipc_host_t*);
extern int   ipc_host_recv_request(ipc_host_t*, ipc_client_t**, uint32_t);
extern int   ipc_host_send_response(ipc_client_t*);
extern int   ipc_client_connect(ipc_host_t*, ipc_client_t**, size_t, size_t);
extern void  ipc_client_disconnect(ipc_client_t*);
extern int   ipc_client_call(ipc_client_t*);
extern void *ipc_get_cmd_data(ipc_client_t*);
extern void *ipc_get_ans_data(ipc_client_t*);

/*==============================================================================
  Exported inline functions
==============================================================================*/

#ifdef __cplusplus
}
#endif

#endif /* _IPC_H_ */

/**@}*/
/*==============================================================================
  End of file
==============================================================================*/
