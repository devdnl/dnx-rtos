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
#define REQUEST_ACTION_TIMEOUT  25000
#define MAX_NAME_LENGTH         128
#define MBOX_QUEUE_LENGTH       8
#define GARBAGE_LIVE_TIME       2000

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
        llist_t         *garbage;
};

typedef struct {
        char            *name;
        task_t          *owner;
        void            *data;
        void            *self;
        size_t           size;
        mbus_sig_perm_t  perm;
        mbus_sig_type_t  type;
} signal_t;

typedef struct {
        void            *data;
        void            *self;
        timer_t          timer;
} garbage_t;

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

                struct RES_CMD_MBOX_RECEIVE {
                        const void     *data;
                        size_t          size;
                } CMD_MBOX_RECEIVE;

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
 * @brief  Function send request to the daemon
 * @param  request              request to send
 * @return On success true is returned, otherwise false.
 */
//==============================================================================
static bool request_action(mbus_t *this, request_t *request, response_t *response)
{
        if (mbus) {
                request->response = this->response;
                request->owner    = task_get_handle();

                if (queue_send(mbus->request, request, REQUEST_ACTION_TIMEOUT)) {
                        if (queue_receive(this->response, response, REQUEST_ACTION_TIMEOUT)) {
                                this->errorno = response->errorno;
                                return true;
                        }
                }

                this->errorno = MBUS_ERRNO__TIMEOUT;
        } else {
                this->errorno = MBUS_ERRNO__DAEMON_NOT_RUNNING;
        }

        return false;
}

//==============================================================================
/**
 * @brief  Create new signal
 * @param  name                 name of signal
 * @param  owner                signal owner
 * @param  size                 signal size
 * @param  perm                 signal permissions
 * @param  type                 signal type
 * @return Created object or NULL on error.
 */
//==============================================================================
static signal_t *signal_new(const char *name, task_t *owner, size_t size, mbus_sig_perm_t perm, mbus_sig_type_t type)
{
        signal_t *this        = malloc(sizeof(signal_t));
        char     *signal_name = malloc(strnlen(name, MAX_NAME_LENGTH) + 1);
        void     *data        = NULL;

        if (type == MBUS_SIG_TYPE__MBOX) {
                data = queue_new(MBOX_QUEUE_LENGTH, sizeof(void*));

        } else if (type == MBUS_SIG_TYPE__VALUE) {
                data = calloc(1, size);
        }

        if (this && signal_name && data) {
                strcpy(signal_name, name);

                this->data  = data;
                this->name  = signal_name;
                this->owner = owner;
                this->perm  = perm;
                this->size  = size;
                this->type  = type;
                this->self  = this;

        } else {
                if (this) {
                        free(this);
                        this = NULL;
                }

                if (signal_name) {
                        free(signal_name);
                }

                if (data) {
                        free(data);
                }
        }

        return this;
}

//==============================================================================
/**
 * @brief  Function check if signal object is valid
 * @param  this                 signal object
 * @return If object is valid then true is returned, otherwise false.
 */
//==============================================================================
static bool signal_is_valid(signal_t *this)
{
        return this && this->self == this && this->name && this->data && this->size;
}

//==============================================================================
/**
 * @brief  Delete signal
 * @param  this                 signal to delete
 * @return ?
 */
//==============================================================================
static void signal_delete(signal_t *this)
{
        if (signal_is_valid(this)) {
                free(this->name);
                this->name  = NULL;
                this->self  = NULL;
                this->owner = NULL;
                this->perm  = MBUS_PERM__INVALID;
                this->size  = 0;
                this->type  = MBUS_SIG_TYPE__INVALID;

                if (this->type == MBUS_SIG_TYPE__MBOX) {
                        for (int i = 0; i < MBOX_QUEUE_LENGTH; i++) {
                                void *data = NULL;
                                if (queue_receive(this->data, &data, 0)) {
                                        if (data) {
                                                free(data);
                                        }
                                } else {
                                        break;
                                }
                        }

                        queue_delete(this->data);
                        this->data = NULL;

                } else if (this->type == MBUS_SIG_TYPE__VALUE) {
                        if (this->data) {
                                free(this->data);
                                this->data = NULL;
                        }
                }
        }
}

//==============================================================================
/**
 * @brief  Add data to the selected signal
 * @param  this                 signal object
 * @param  data                 data to set
 * @return Error number
 */
//==============================================================================
static mbus_errno_t signal_set_data(signal_t *this, const void *data)
{
        mbus_errno_t err = MBUS_ERRNO__INTERNAL_ERROR;

        if (signal_is_valid(this) && data) {
                if (this->type == MBUS_SIG_TYPE__MBOX) {
                        void *item = malloc(this->size);
                        if (item) {
                                memcpy(item, data, this->size);

                                if (!queue_send(this->data, &item, 0)) {
                                        err = MBUS_ERRNO__MBOX_IS_FULL;
                                        free(item);
                                }
                        } else {
                                err = MBUS_ERRNO__NOT_ENOUGH_FREE_MEMORY;
                        }

                } else if (this->type == MBUS_SIG_TYPE__VALUE) {
                        memcpy(this->data, data, this->size);
                }
        }

        return err;
}

//==============================================================================
/**
 * @brief  Get data from selected signal
 * @param  this                 signal object
 * @param  data                 data destination
 * @return Error number
 */
//==============================================================================
static mbus_errno_t signal_get_data(signal_t *this, void **data)
{
        mbus_errno_t err = MBUS_ERRNO__INTERNAL_ERROR;

        if (signal_is_valid(this) && data) {
                err = MBUS_ERRNO__NO_ERROR;

                if (this->type == MBUS_SIG_TYPE__MBOX) {
                        if (!queue_receive(this->data, data, 0)) {
                                err = MBUS_ERRNO__MBOX_EMPTY;
                        }
                } else if (this->type == MBUS_SIG_TYPE__VALUE) {
                        *data = this->data;
                }
        }

        return err;
}

//==============================================================================
/**
 * @brief  Create new garbage object
 * @param  data                 garbage data to link
 * @return Garbage object or NULL on error.
 */
//==============================================================================
static garbage_t *garbage_new(void *data)
{
        garbage_t *this = malloc(sizeof(garbage_t));
        if (this) {
                this->data  = data;
                this->timer = timer_reset();
                this->self  = this;
        }

        return this;
}

//==============================================================================
/**
 * @brief  Function check if signal object is valid
 * @param  this                 signal object
 * @return If object is valid then true is returned, otherwise false.
 */
//==============================================================================
static bool garbage_is_valid(garbage_t *this)
{
        return this && this->self == this && this->data;
}

//==============================================================================
/**
 * @brief  Remove garbage object and linked data
 * @param  this                 garbage object
 * @return None
 */
//==============================================================================
static void garbage_delete(garbage_t *this)
{
        if (garbage_is_valid(this)) {
                if (this->data)
                        free(this->data);

                this->self = NULL;

                free(this);
        }
}

//==============================================================================
/**
 * @brief  Check if garbage time expired
 * @param  this                 garbage object
 * @return True if time expired, otherwise false.
 */
//==============================================================================
static bool garbage_is_time_expired(garbage_t *this)
{
        if (garbage_is_valid(this)) {
                return timer_is_expired(this->timer, GARBAGE_LIVE_TIME);
        } else {
                return false;
        }
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
        printk("%s\n", __func__); // TEST

        int count = llist_size(mbus->signals);

        response_t response;
        response.of.CMD_GET_NUMBER_OF_SIGNALS.number_of_signals = count;
        response.errorno = count == -1 ? MBUS_ERRNO__INTERNAL_ERROR : MBUS_ERRNO__NO_ERROR;
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
        printk("%s\n", __func__); // TEST

        response_t response;
        signal_t  *sig = llist_at(mbus->signals, request->arg.CMD_GET_SIGNAL_INFO.n);
        if (sig) {
                response.of.CMD_GET_SIGNAL_INFO.info.name        = sig->name;
                response.of.CMD_GET_SIGNAL_INFO.info.permissions = sig->perm;
                response.of.CMD_GET_SIGNAL_INFO.info.size        = sig->size;
                response.of.CMD_GET_SIGNAL_INFO.info.type        = sig->type;
                response.errorno                                 = MBUS_ERRNO__NO_ERROR;

        } else {
                response.errorno = MBUS_ERRNO__NO_ITEM;
        }

        queue_send(request->response, &response, REQUEST_ACTION_TIMEOUT);
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
        printk("%s\n", __func__); // TEST

        response_t response;
        response.errorno = MBUS_ERRNO__NOT_ENOUGH_FREE_MEMORY;

        signal_t *sig = signal_new(request->arg.CMD_MBOX_CREATE.name,
                                   request->owner,
                                   request->arg.CMD_MBOX_CREATE.size,
                                   request->arg.CMD_MBOX_CREATE.perm,
                                   MBUS_SIG_TYPE__MBOX);

        if (sig) {
                if (llist_push_back(mbus->signals, sig)) {
                        response.errorno = MBUS_ERRNO__NO_ERROR;
                } else {
                        signal_delete(sig);
                }
        }

        queue_send(request->response, &response, REQUEST_ACTION_TIMEOUT);
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
        printk("%s\n", __func__); // TEST

        mbus_errno_t err = MBUS_ERRNO__NO_ERROR;
        bool found       = false;
        int  n           = 0;

        llist_foreach(signal_t*, sig, mbus->signals) {
                if (strcmp(sig->name, request->arg.CMD_MBOX_DELETE.name) == 0) {
                        found = true;

                        if (sig->owner == request->owner) {
                                llist_erase(mbus->signals, n);
                        } else {
                                err = MBUS_ERRNO__ACCESS_DENIED;
                        }

                        break;
                }

                n++;
        }

        if (!found) {
                err = MBUS_ERRNO__NO_ITEM;
        }

        response_t response;
        response.errorno = err;
        queue_send(request->response, &response, REQUEST_ACTION_TIMEOUT);
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
        printk("%s\n", __func__); // TEST


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
        printk("%s\n", __func__); // TEST
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
        printk("%s\n", __func__); // TEST
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
        printk("%s\n", __func__); // TEST
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
        printk("%s\n", __func__); // TEST
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
        printk("%s\n", __func__); // TEST
}

//==============================================================================
/**
 * @brief  Function check if mbus object is valid
 * @param  this                 mbus object
 * @return If object is valid then true is returned, otherwise false.
 */
//==============================================================================
static bool mbus_is_valid(mbus_t *this)
{
        return this && this->self == this;
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
                llist_t *signals = llist_new(NULL, (llist_obj_dtor_t)signal_delete);

                if (mbus && request && signals) {
                        mbus->request = request;
                        mbus->signals = signals;
                        mbus->owner   = task_get_handle();

                } else {
                        err = MBUS_ERRNO__NOT_ENOUGH_FREE_MEMORY;

                        if (mbus) {
                                free(mbus);
                                mbus = NULL;
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

                llist_foreach(garbage_t*, g, mbus->garbage) {
                        if (garbage_is_time_expired(g)) {
                                printk("Remove garbage\n"); // TEST
                                garbage_delete(g);
                        }
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

        if (mbus_is_valid(this)) {
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
        if (mbus_is_valid(this)) {
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

        if (mbus_is_valid(this)) {
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

        if (mbus_is_valid(this) && n > 0 && info) {
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

        if (mbus_is_valid(this) && name && size > 0) {
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

        if (mbus_is_valid(this) && name) {
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

        if (mbus_is_valid(this) && name && data) {
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

        if (mbus_is_valid(this) && name && data) {
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

        if (mbus_is_valid(this) && name && size > 0) {
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

        if (mbus_is_valid(this) && name) {
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

        if (mbus_is_valid(this) && name && data) {
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

        if (mbus_is_valid(this) && name && data) {
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
