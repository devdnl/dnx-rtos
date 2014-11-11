/*=========================================================================*//**
@file    mbus.c

@author  Daniel Zorychta

@brief   Message Bus Library

@note    Copyright (C) 2014 Daniel Zorychta <daniel.zorychta@gmail.com>

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

/*==============================================================================
  Include files
==============================================================================*/
#include "mbus.h"
#include "llist.h"
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdarg.h>
#include <dnx/thread.h>
#include <dnx/misc.h>
#include <dnx/timer.h>

/*==============================================================================
  Local macros
==============================================================================*/
#define REQUEST_QUEUE_LENGTH    8
#define REQUEST_WAIT_TIMEOUT    1000
#define MBUS_CYCLE_MS           10000
#define REQUEST_ACTION_TIMEOUT  25000
#define MAX_NAME_LENGTH         128

/*==============================================================================
  Local object types
==============================================================================*/
struct mbus {
        queue_t         *response;
        mbus_errno_t     errorno;
        struct mbus     *self;
};

struct mbus_mem {
        queue_t         *request;
        task_t          *owner;
        llist_t         *signals;
};

typedef struct {
        const char      *name;
        task_t          *owner;
        mbus_sig_perm_t  perm;
} signal_t;

typedef enum {
        CMD_GET_NUMBER_OF_SIGNALS,
        CMD_GET_SIGNAL_INFO,
        CMD_MBOX_CREATE,
        CMD_MBOX_DELETE,
        CMD_MBOX_SEND,
        CMD_MBOX_RECEIVE,
        CMD_VALUE_CREATE,
        CMD_VALUE_DELETE,
        CMD_VALUE_SET,
        CMD_VALUE_GET
} cmd_t;

typedef struct {
        cmd_t cmd;

        union {
                struct RQ_CMD_GET_SIGNAL_INFO {
                        int             n;
                } CMD_GET_SIGNAL_INFO;

                struct RQ_CMD_MBOX_CREATE {
                        const char     *name;
                        size_t          size;
                        mbus_sig_perm_t perm;
                } CMD_MBOX_CREATE;

                struct RQ_CMD_MBOX_DELETE {
                        const char     *name;
                } CMD_MBOX_DELETE;

                struct RQ_CMD_MBOX_SEND {
                        const char     *name;
                        const void     *data;
                } CMD_MBOX_SEND;

                struct RQ_CMD_MBOX_RECEIVE {
                        const char     *name;
                } CMD_MBOX_RECEIVE;

                struct RQ_CMD_VALUE_CREATE {
                        const char     *name;
                        size_t          size;
                        mbus_sig_perm_t perm;
                } CMD_VALUE_CREATE;

                struct RQ_CMD_VALUE_DELETE {
                        const char     *name;
                } CMD_VALUE_DELETE;

                struct RQ_CMD_VALUE_SET {
                        const char     *name;
                        const void     *data;
                } CMD_VALUE_SET;

                struct RQ_CMD_VALUE_GET {
                        const char     *name;
                } CMD_VALUE_GET;
        } arg;

        queue_t *response;
        task_t  *owner;
} request_t;

typedef struct {
        union {
                struct RES_CMD_GET_NUMBER_OF_SIGNALS {
                        int             number_of_signals;
                } CMD_GET_NUMBER_OF_SIGNALS;

                struct RES_CMD_GET_SIGNAL_INFO {
                        mbus_sig_info_t info;
                } CMD_GET_SIGNAL_INFO;

//                struct RES_CMD_MBOX_CREATE {
//                        bool            status;
//                } CMD_MBOX_CREATE;

//                struct RES_CMD_MBOX_DELETE {
//                        bool            status;
//                } CMD_MBOX_DELETE;

//                struct RES_CMD_MBOX_SEND {
//                        bool            status;
//                } CMD_MBOX_SEND;

                struct RES_CMD_MBOX_RECEIVE {
                        const void     *data;
                        size_t          size;
                } CMD_MBOX_RECEIVE;

//                struct RES_CMD_VALUE_CREATE {
//                        bool            status;
//                } CMD_VALUE_CREATE;

//                struct RES_CMD_VALUE_DELETE {
//                        bool            status;
//                } CMD_VALUE_DELETE;

//                struct RES_CMD_VALUE_SET {
//                        bool            status;
//                } CMD_VALUE_SET;

                struct RES_CMD_VALUE_GET {
                        const void     *data;
                        size_t          size;
                } CMD_VALUE_GET;
        } of;

        mbus_errno_t errorno;
} response_t;

/*==============================================================================
  Local function prototypes
==============================================================================*/

/*==============================================================================
  Local objects
==============================================================================*/

/*==============================================================================
  Shared objects
==============================================================================*/
static struct mbus_mem *mbus;

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
 * @brief  Function check if mbus object is valid
 * @param  this                 mbus object
 * @return If object is valid then true is returned, otherwise false.
 */
//==============================================================================
static bool is_valid(mbus_t *this)
{
        return this && this->self == this;
}

//==============================================================================
/**
 * @brief  Function send request to the daemon
 * @param  request              request to send
 * @return On success true is returned, otherwise false.
 */
//==============================================================================
static bool request_action(mbus_t *this, request_t *request, response_t *response)
{
        request->response = this->response;
        request->owner    = task_get_handle();

        if (queue_send(mbus->request, request, REQUEST_ACTION_TIMEOUT)) {
                if (queue_receive(this->response, response, REQUEST_ACTION_TIMEOUT)) {
                        this->errorno = response->errorno;
                        return true;
                }
        }

        this->errorno = MBUS_ERRNO__TIMEOUT;
        return false;
}

//==============================================================================
/**
 * @brief  Function realize CMD_GET_NUMBER_OF_SIGNALS command
 * @param  request              request data
 * @return None
 */
//==============================================================================
static void realize_CMD_GET_NUMBER_OF_SIGNALS(request_t *request)
{
        printk("%s\n", __func__);

        response_t response;
        response.of.CMD_GET_NUMBER_OF_SIGNALS.number_of_signals = llist_size(mbus->signals);
        response.errorno = MBUS_ERRNO__NO_ERROR;
        queue_send(request->response, &response, REQUEST_ACTION_TIMEOUT);
}

//==============================================================================
/**
 * @brief  Function realize CMD_GET_SIGNAL_INFO command
 * @param  request              request data
 * @return None
 */
//==============================================================================
static void realize_CMD_GET_SIGNAL_INFO(request_t *request)
{
        printk("%s\n", __func__);
}

//==============================================================================
/**
 * @brief  Function realize CMD_MBOX_CREATE command
 * @param  request              request data
 * @return None
 */
//==============================================================================
static void realize_CMD_MBOX_CREATE(request_t *request)
{
        printk("%s\n", __func__);
}

//==============================================================================
/**
 * @brief  Function realize CMD_MBOX_DELETE command
 * @param  request              request data
 * @return None
 */
//==============================================================================
static void realize_CMD_MBOX_DELETE(request_t *request)
{
        printk("%s\n", __func__);
}

//==============================================================================
/**
 * @brief  Function realize CMD_MBOX_SEND command
 * @param  request              request data
 * @return None
 */
//==============================================================================
static void realize_CMD_MBOX_SEND(request_t *request)
{
        printk("%s\n", __func__);
}

//==============================================================================
/**
 * @brief  Function realize CMD_MBOX_RECEIVE command
 * @param  request              request data
 * @return None
 */
//==============================================================================
static void realize_CMD_MBOX_RECEIVE(request_t *request)
{
        printk("%s\n", __func__);
}

//==============================================================================
/**
 * @brief  Function realize CMD_VALUE_CREATE command
 * @param  request              request data
 * @return None
 */
//==============================================================================
static void realize_CMD_VALUE_CREATE(request_t *request)
{
        printk("%s\n", __func__);
}

//==============================================================================
/**
 * @brief  Function realize CMD_VALUE_DELETE command
 * @param  request              request data
 * @return None
 */
//==============================================================================
static void realize_CMD_VALUE_DELETE(request_t *request)
{
        printk("%s\n", __func__);
}

//==============================================================================
/**
 * @brief  Function realize CMD_VALUE_SET command
 * @param  request              request data
 * @return None
 */
//==============================================================================
static void realize_CMD_VALUE_SET(request_t *request)
{
        printk("%s\n", __func__);
}

//==============================================================================
/**
 * @brief  Function realize CMD_VALUE_GET command
 * @param  request              request data
 * @return None
 */
//==============================================================================
static void realize_CMD_VALUE_GET(request_t *request)
{
        printk("%s\n", __func__);
}

//==============================================================================
/**
 * @brief  Main daemon function
 * @param  None
 * @return Function not return if daemon started correctly, otherwise specified
 *         error is returned.
 */
//==============================================================================
mbus_errno_t mbus_daemon()
{
        mbus_errno_t err = MBUS_ERRNO__DAEMON_IS_ALREADY_STARTED;

        // check if mbus is orphaned
        if (mbus && !task_is_exist(mbus->owner)) {
                mbus->owner = task_get_handle();
        }

        // create new object if started first time
        if (!mbus) {
                mbus             = malloc(sizeof(struct mbus_mem));
                queue_t *request = queue_new(REQUEST_QUEUE_LENGTH, sizeof(request_t));
                llist_t *signals = llist_new(NULL, NULL);

                if (mbus && request && signals) {
                        mbus->request = request;
                        mbus->signals = signals;
                        mbus->owner   = task_get_handle();

                } else {
                        err = MBUS_ERRNO__NOT_ENOUGH_MEMORY;

                        if (mbus) {
                                free(mbus); mbus = NULL;
                        }

                        if (request) {
                                queue_delete(request);
                        }

                        if (signals) {
                                llist_delete(signals);
                        }
                }
        }

        // execute requests
        timer_t cycle_timer = timer_reset();
        while (mbus) {
                request_t request;
                if (queue_receive(mbus->request, &request, REQUEST_WAIT_TIMEOUT)) {
                        switch (request.cmd) {
                        case CMD_GET_NUMBER_OF_SIGNALS:
                                realize_CMD_GET_NUMBER_OF_SIGNALS(&request);
                                break;

                        case CMD_GET_SIGNAL_INFO:
                                realize_CMD_GET_SIGNAL_INFO(&request);
                                break;

                        case CMD_MBOX_CREATE:
                                realize_CMD_MBOX_CREATE(&request);
                                break;

                        case CMD_MBOX_DELETE:
                                realize_CMD_MBOX_DELETE(&request);
                                break;

                        case CMD_MBOX_SEND:
                                realize_CMD_MBOX_SEND(&request);
                                break;

                        case CMD_MBOX_RECEIVE:
                                realize_CMD_MBOX_RECEIVE(&request);
                                break;

                        case CMD_VALUE_CREATE:
                                realize_CMD_VALUE_CREATE(&request);
                                break;

                        case CMD_VALUE_DELETE:
                                realize_CMD_VALUE_DELETE(&request);
                                break;

                        case CMD_VALUE_SET:
                                realize_CMD_VALUE_SET(&request);
                                break;

                        case CMD_VALUE_GET:
                                realize_CMD_VALUE_GET(&request);
                                break;

                        default:
                                break;
                        }
                }

                if (timer_is_expired(cycle_timer, MBUS_CYCLE_MS)) {
                        cycle_timer = timer_reset();
                }
        }

        // exit on error
        return err;
}

//==============================================================================
/**
 * @brief  Create a new mbus object
 * @param  this                 mbus object
 * @return On success mbus object is returned, otherwise NULL.
 */
//==============================================================================
mbus_t *mbus_new()
{
        mbus_t *this = malloc(sizeof(mbus_t));
        if (this) {
                queue_t *response = queue_new(1, sizeof(response_t));
                if (response) {
                        this->response = response;
                        this->errorno  = MBUS_ERRNO__NO_ERROR;
                        this->self     = this;
                } else {
                        free(this);
                        this = NULL;
                }
        }

        return this;
}

//==============================================================================
/**
 * @brief  Delete created mbus object
 * @param  this                 mbus object
 * @return On success true is returned. On error false and error number is set.
 */
//==============================================================================
bool mbus_delete(mbus_t *this)
{
        bool status = false;

        if (is_valid(this)) {
                queue_delete(this->response);
                this->self = NULL;
                free(this);
        }

        return status;
}

//==============================================================================
/**
 * @brief  Return last error number
 * @param  this                 mbus object
 * @return Last error number.
 */
//==============================================================================
mbus_errno_t mbus_get_errno(mbus_t *this)
{
        if (is_valid(this)) {
                mbus_errno_t err = this->errorno;
                this->errorno    = MBUS_ERRNO__NO_ERROR;
                return err;
        } else {
                return MBUS_ERRNO__INVALID_OBJECT_OR_ARGUMENT;
        }
}

//==============================================================================
/**
 * @brief  Return number of registered signals
 * @param  this                 mbus object
 * @return On success return number of signals, otherwise -1 and error number is set.
 */
//==============================================================================
int mbus_get_number_of_signals(mbus_t *this)
{
        int status = -1;

        if (is_valid(this)) {
                response_t response;
                request_t  request;
                request.cmd = CMD_GET_NUMBER_OF_SIGNALS;
                if (request_action(this, &request, &response)) {
                        return response.of.CMD_GET_NUMBER_OF_SIGNALS.number_of_signals;
                }
        }

        return status;
}

//==============================================================================
/**
 * @brief  Return information of selected signal
 * @param  this                 mbus object
 * @param  n                    n-element to get
 * @param  info                 pointer to the signal info object
 * @return On success true is returned. On error false and error number is set.
 */
//==============================================================================
bool mbus_get_signal_info(mbus_t *this, size_t n, mbus_sig_info_t *info)
{
        bool status = false;

        if (is_valid(this) && n > 0 && info) {
                response_t response;
                request_t  request;
                request.cmd = CMD_GET_SIGNAL_INFO;
                request.arg.CMD_GET_SIGNAL_INFO.n = n;
                if (request_action(this, &request, &response)) {
                        if (response.errorno == MBUS_ERRNO__NO_ERROR) {
                                *info = response.of.CMD_GET_SIGNAL_INFO.info;
                        }
                }
        }

        return status;
}

//==============================================================================
/**
 * @brief  Create new mbox
 * @param  this                 mbus object
 * @param  name                 mbox name
 * @param  size                 mbox size
 * @param  permissions          mbox permissions
 * @return On success true is returned. On error false and error number is set.
 */
//==============================================================================
bool mbus_mbox_create(mbus_t *this, const char *name, size_t size, mbus_sig_perm_t permissions)
{
        bool status = false;

        if (is_valid(this) && name && size > 0) {
                response_t response;
                request_t  request;
                request.cmd = CMD_MBOX_CREATE;
                request.arg.CMD_MBOX_CREATE.name = name;
                request.arg.CMD_MBOX_CREATE.size = size;
                request.arg.CMD_MBOX_CREATE.perm = permissions;
                if (request_action(this, &request, &response)) {
                        status = response.errorno == MBUS_ERRNO__NO_ERROR;
                }
        }

        return status;
}

//==============================================================================
/**
 * @brief  Delete mbox
 * @param  this                 mbus object
 * @param  name                 name of mbox to delete
 * @return On success true is returned. On error false and error number is set.
 */
//==============================================================================
bool mbus_mbox_delete(mbus_t *this, const char *name)
{
        bool status = false;

        if (is_valid(this) && name) {
                response_t response;
                request_t  request;
                request.cmd = CMD_MBOX_DELETE;
                request.arg.CMD_MBOX_DELETE.name = name;
                if (request_action(this, &request, &response)) {
                        status = response.errorno == MBUS_ERRNO__NO_ERROR;
                }
        }

        return status;
}

//==============================================================================
/**
 * @brief  Send selected data to mbox
 * @param  this                 mbus object
 * @param  name                 name of mbox
 * @param  data                 data to send
 * @return On success true is returned. On error false and error number is set.
 */
//==============================================================================
bool mbus_mbox_send(mbus_t *this, const char *name, const void *data)
{
        bool status = false;

        if (is_valid(this) && name && data) {
                response_t response;
                request_t  request;
                request.cmd = CMD_MBOX_SEND;
                request.arg.CMD_MBOX_SEND.name = name;
                request.arg.CMD_MBOX_SEND.data = data;
                if (request_action(this, &request, &response)) {
                        status = response.errorno == MBUS_ERRNO__NO_ERROR;
                }
        }

        return status;
}

//==============================================================================
/**
 * @brief  Receive data from mbox
 * @param  this                 mbus object
 * @param  name                 name of mbox
 * @param  data                 data destination
 * @return On success true is returned. On error false and error number is set.
 */
//==============================================================================
bool mbus_mbox_receive(mbus_t *this, const char *name, void *data)
{
        bool status = false;

        if (is_valid(this) && name && data) {
                response_t response;
                request_t  request;
                request.cmd = CMD_MBOX_RECEIVE;
                request.arg.CMD_MBOX_RECEIVE.name = name;
                if (request_action(this, &request, &response)) {
                        if (response.of.CMD_MBOX_RECEIVE.data) {
                                memcpy(data, response.of.CMD_MBOX_RECEIVE.data, response.of.CMD_MBOX_RECEIVE.size);
                                status = true;
                        }
                }
        }

        return status;
}

//==============================================================================
/**
 * @brief  Create new value
 * @param  this                 mbus object
 * @param  name                 name of value
 * @param  size                 size of value
 * @param  permissions          value permissions
 * @return On success true is returned. On error false and error number is set.
 */
//==============================================================================
bool mbus_value_create(mbus_t *this, const char *name, size_t size, mbus_sig_perm_t permissions)
{
        bool status = false;

        if (is_valid(this) && name && size > 0) {
                response_t response;
                request_t  request;
                request.cmd = CMD_VALUE_CREATE;
                request.arg.CMD_VALUE_CREATE.name = name;
                request.arg.CMD_VALUE_CREATE.size = size;
                request.arg.CMD_VALUE_CREATE.perm = permissions;
                if (request_action(this, &request, &response)) {
                        status = response.errorno == MBUS_ERRNO__NO_ERROR;
                }
        }

        return status;
}

//==============================================================================
/**
 * @brief  Delete selected value
 * @param  this                 mbus object
 * @param  name                 name of value
 * @return On success true is returned. On error false and error number is set.
 */
//==============================================================================
bool mbus_value_delete(mbus_t *this, const char *name)
{
        bool status = false;

        if (is_valid(this) && name) {
                response_t response;
                request_t  request;
                request.cmd = CMD_VALUE_DELETE;
                request.arg.CMD_VALUE_DELETE.name = name;
                if (request_action(this, &request, &response)) {
                        status = response.errorno == MBUS_ERRNO__NO_ERROR;
                }
        }

        return status;
}

//==============================================================================
/**
 * @brief  Set selected value
 * @param  this                 mbus object
 * @param  name                 name of value
 * @param  data                 data of value
 * @return On success true is returned. On error false and error number is set.
 */
//==============================================================================
bool mbus_value_set(mbus_t *this, const char *name, const void *data)
{
        bool status = false;

        if (is_valid(this) && name && data) {
                response_t response;
                request_t  request;
                request.cmd = CMD_VALUE_SET;
                request.arg.CMD_VALUE_SET.name = name;
                request.arg.CMD_VALUE_SET.data = data;
                if (request_action(this, &request, &response)) {
                        status = response.errorno == MBUS_ERRNO__NO_ERROR;
                }
        }

        return status;
}

//==============================================================================
/**
 * @brief  Get selected value
 * @param  this                 mbus object
 * @param  name                 name of value
 * @param  data                 value destination
 * @return On success true is returned. On error false and error number is set.
 */
//==============================================================================
bool mbus_value_get(mbus_t *this, const char *name, void *data)
{
        bool status = false;

        if (is_valid(this) && name && data) {
                response_t response;
                request_t  request;
                request.cmd = CMD_VALUE_GET;
                request.arg.CMD_VALUE_GET.name = name;
                if (request_action(this, &request, &response)) {
                        if (response.of.CMD_VALUE_GET.data) {
                                memcpy(data, response.of.CMD_VALUE_GET.data, response.of.CMD_VALUE_GET.size);
                                status = true;
                        }
                }
        }

        return status;
}

/*==============================================================================
  End of file
==============================================================================*/
