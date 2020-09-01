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
#ifndef IPC_DEBUG_ON
#define IPC_DEBUG_ON            0
#endif

#define IPC_DEBUG_ONLY_ERRORS   1

#if IPC_DEBUG_ON
#include <stdio.h>
#include <unistd.h>
#include <dnx/vt100.h>
#include <dnx/os.h>
static void ipc_msg(const char *msg, int err, void *host, void *client)
{
        if (!IPC_DEBUG_ONLY_ERRORS || err) {
                printf("%s(P:%d) %s->%d C:%p H:%p%s\n",
                       err ? VT100_FONT_COLOR_RED : VT100_RESET_ATTRIBUTES,
                       getpid(), msg, err, client, host, VT100_RESET_ATTRIBUTES);
        }
}
#define IPC_DEBUG(_msg, _err, _host, _client)   ipc_msg(_msg, _err, _host, _client)
#else
#define IPC_DEBUG(...)
#endif

#define IPC_MUTEX_TIMEOUT       1000

/*==============================================================================
  Local object types
==============================================================================*/
/**
 * Host object representation.
 */
struct ipc_client {
        void       *this;       /*!< This pointer */
        ipc_host_t *host;       /*!< Host object reference */
        mutex_t    *mtx;        /*!< Access mutex */
        sem_t      *ans_sem;    /*!< Answer semaphore */
        void       *cmd_data;   /*!< Command data object */
        void       *ans_data;   /*!< Answer data object */

#if IPC_DEBUG_ON
        pid_t       caller_pid;
        bool        waiting_for_response;
        u32_t       last_call;
#endif
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
                                *host = NULL;
                                err   = errno;
                                free(this);
                        }
                } else {
                        err = errno;
                }
        }

        IPC_DEBUG("host create", err, *host, NULL);

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
                IPC_DEBUG("host destroy", 0, host, NULL);
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
                        *client = NULL;
                        err = errno;
                }
        }

        IPC_DEBUG("host receive request", err, host, *client);

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
                IPC_DEBUG("host send response", err, client->host, client);
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
                // check if object is valid
                if (queue_get_number_of_items(host->cmd_queue) < 0) {
                        IPC_DEBUG("client connect (invalid object)", -1, host, *client);
                        return -1;
                }

                ipc_client_t *clt = malloc(sizeof(ipc_client_t));
                if (clt) {
                        clt->host     = host;
                        clt->cmd_data = calloc(1, cmd_data_len);
                        clt->ans_data = calloc(1, ans_data_len);
                        clt->ans_sem  = semaphore_new(1, 0);
                        clt->mtx      = mutex_new(MUTEX_TYPE_NORMAL);

                        if (clt->ans_sem && clt->mtx && clt->cmd_data && clt->ans_data) {
                                clt->this = clt;
                                *client   = clt;
                                err       = 0;

                        } else {
                                *client = NULL;
                                err     = ENOMEM;

                                if (clt->ans_sem) {
                                        semaphore_delete(clt->ans_sem);
                                }

                                if (clt->cmd_data) {
                                        free(clt->cmd_data);
                                }

                                if (clt->ans_data) {
                                        free(clt->ans_data);
                                }

                                if (clt->mtx) {
                                        mutex_delete(clt->mtx);
                                }

                                free(clt);
                        }
                } else {
                        err = errno;
                }
        }

        IPC_DEBUG("client connect", err, host, *client);

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
                IPC_DEBUG("client disconnect", 0, client->host, client);
                client->host = NULL;
                free(client->ans_data);
                free(client->cmd_data);
                semaphore_delete(client->ans_sem);
                mutex_delete(client->mtx);
                client->this = NULL;
                free(client);
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

                if (mutex_lock(client->mtx, IPC_MUTEX_TIMEOUT)) {

                        #if IPC_DEBUG_ON
                        client->caller_pid = getpid();
                        client->waiting_for_response = true;
                        client->last_call = get_time_ms();
                        #endif

                        semaphore_wait(client->ans_sem, 0); // reset semaphore

                        IPC_DEBUG("client calling...", 0, client->host, client);

                        if (queue_send(client->host->cmd_queue, &client, MAX_DELAY_MS)) {
                                if (semaphore_wait(client->ans_sem, MAX_DELAY_MS)) {
                                        err = 0;
                                } else {
                                        err = errno;
                                }
                        } else {
                                err = errno;
                        }

                        #if IPC_DEBUG_ON
                        client->caller_pid = 0;
                        client->waiting_for_response = false;
                        #endif

                        mutex_unlock(client->mtx);
                } else {
                        err = ETIME;
                }
        }

        IPC_DEBUG("client call", err, client->host, client);

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
