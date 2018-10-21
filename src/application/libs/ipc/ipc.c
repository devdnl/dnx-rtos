/*==============================================================================
File    ipc.c

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

/*==============================================================================
  Include files
==============================================================================*/
#include "ipc.h"

#include <string.h>
#include <stdlib.h>
#include <dnx/thread.h>
#include <errno.h>

/*==============================================================================
  Local macros
==============================================================================*/

/*==============================================================================
  Local object types
==============================================================================*/
/**
 * Host object representation.
 */
struct ipc_client {
        void       *this;       /*!< This pointer */
        ipc_host_t *host;       /*!< Host object reference */
        sem_t      *ans_sem;    /*!< Answer semaphore */
        void       *cmd_data;   /*!< Command data object */
        void       *ans_data;   /*!< Answer data object */
};

/**
 * Client object representation.
 */
struct ipc_host {
        queue_t *cmd_queue;     /*!< Command queue */
};

/*==============================================================================
  Local function prototypes
==============================================================================*/

/*==============================================================================
  Local objects
==============================================================================*/

/*==============================================================================
  Exported objects
==============================================================================*/

/*==============================================================================
  External objects
==============================================================================*/

/*==============================================================================
  Function definitions
==============================================================================*/
//==============================================================================
/**
* @brief  Function create new host instance.
*
* @param  host          host object destination pointer
* @param  queue_len     command queue length
*
* @return One of errno value.
*/
//==============================================================================
int ipc_host_create(ipc_host_t **host, size_t queue_len)
{
        int err = EINVAL;

        if (host && queue_len) {
                ipc_host_t *this = malloc(sizeof(ipc_host_t));
                if (this) {
                        this->cmd_queue = queue_new(queue_len, sizeof(ipc_client_t*));

                        if (this->cmd_queue) {
                                *host = this;
                                err   = ESUCC;
                        } else {
                                err = errno;
                                free(this);
                        }
                } else {
                        err = errno;
                }
        }

        return err;
}

//==============================================================================
/**
* @brief  Function destroy host instance.
*
* @param  host          host object
*/
//==============================================================================
void ipc_host_destroy(ipc_host_t *host)
{
        if (host) {
                queue_delete(host->cmd_queue);
                host->cmd_queue = NULL;
                free(host);
        }
}

//==============================================================================
/**
* @brief  Function receive request from client.
*
* @param  host          host object
* @param  client        client destination pointer
* @param  timeout       maximum wait time in milliseconds
*
* @return One of errno value.
*/
//==============================================================================
int ipc_host_recv_request(ipc_host_t *host, ipc_client_t **client, uint32_t timeout)
{
        int err = EINVAL;

        if (host && client) {
                ipc_client_t *clt = NULL;

                if (queue_receive(host->cmd_queue, &clt, timeout)) {
                        *client = clt;
                        err     = 0;
                } else {
                        err = errno;
                }
        }

        return err;
}

//==============================================================================
/**
* @brief  Function send response to client.
*
* @param  client        client object
*
* @return One of errno value.
*/
//==============================================================================
int ipc_host_send_response(ipc_client_t *client)
{
        int err = EINVAL;

        if (client && (client->this == client)) {
                err = semaphore_signal(client->ans_sem) ? 0 : errno;
        }

        return err;
}

//==============================================================================
/**
* @brief  Function connect to selected host.
*
* @param  host          host object
* @param  client        client destination pointer
* @param  cmd_data_len  command data size
* @param  ans_data_len  answer data size
*
* @return One of errno value.
*/
//==============================================================================
int ipc_client_connect(ipc_host_t *host, ipc_client_t **client, size_t cmd_data_len, size_t ans_data_len)
{
        int err = EINVAL;

        if (host && client && cmd_data_len && ans_data_len) {
                ipc_client_t *clt = malloc(sizeof(ipc_client_t));
                if (clt) {
                        clt->host     = host;
                        clt->cmd_data = calloc(1, cmd_data_len);
                        clt->ans_data = calloc(1, ans_data_len);
                        clt->ans_sem  = semaphore_new(1, 0);

                        if (clt->ans_sem && clt->cmd_data && clt->ans_data) {
                                clt->this = clt;
                                *client   = clt;
                                err       = 0;

                        } else {
                                err = ENOMEM;

                                if (clt->ans_sem) {
                                        semaphore_delete(clt->ans_sem);
                                }

                                if (clt->cmd_data) {
                                        free(clt->cmd_data);
                                }

                                if (clt->ans_data) {
                                        free(clt->ans_data);
                                }

                                free(clt);
                        }
                } else {
                        err = errno;
                }
        }

        return err;
}

//==============================================================================
/**
* @brief  Function disconnect client.
*
* @param  client        client object to disconnect
*
* @return One of errno value.
*/
//==============================================================================
void ipc_client_disconnect(ipc_client_t *client)
{
        if (client && (client->this == client)) {
                client->host = NULL;
                free(client->ans_data);
                free(client->cmd_data);
                semaphore_delete(client->ans_sem);
                client->this = NULL;
        }
}

//==============================================================================
/**
* @brief  Function send request and receive response from host.
*
* @param  client        client object
*
* @return One of errno value.
*/
//==============================================================================
int ipc_client_call(ipc_client_t *client)
{
        int err = EINVAL;

        if (client && (client->this == client)) {
                semaphore_wait(client->ans_sem, 0); // reset semaphore

                if (queue_send(client->host->cmd_queue, &client, MAX_DELAY_MS)) {
                        if (semaphore_wait(client->ans_sem, MAX_DELAY_MS)) {
                                err = 0;
                        } else {
                                err = errno;
                        }
                } else {
                        err = errno;
                }
        }

        return err;
}

//==============================================================================
/**
 * @brief  Function return command data pointer.
 *
 * @param  client        client object
 *
 * @return Command data pointer or NULL on error.
 */
//==============================================================================
void *ipc_get_cmd_data(ipc_client_t *client)
{
        if (client && (client->this == client)) {
                return client->cmd_data;
        } else {
                return NULL;
        }
}

//==============================================================================
/**
 * @brief  Function return answer data pointer.
 *
 * @param  client        client object
 *
 * @return Answer data pointer or NULL on error.
 */
//==============================================================================
void *ipc_get_ans_data(ipc_client_t *client)
{
        if (client && (client->this == client)) {
                return client->ans_data;
        } else {
                return NULL;
        }
}

/*==============================================================================
  End of file
==============================================================================*/
