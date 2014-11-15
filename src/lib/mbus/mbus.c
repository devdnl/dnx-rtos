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
#include "mbus_signal.h"
#include "mbus_garbage.h"
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

/*==============================================================================
 Local object types
 ==============================================================================*/
struct mbus {
        queue_t         *response;
        struct mbus     *self;
        mbus_errno_t     errorno;
        _mbus_owner_ID_t owner_ID;
};

struct mbus_mem {
        queue_t         *request;
        task_t          *mbus_owner;
        llist_t         *signals;
        llist_t         *garbage;
        _mbus_owner_ID_t owner_ID_cnt;
};

typedef enum {
        CMD_GET_NUMBER_OF_SIGNALS,
        CMD_GET_SIGNAL_INFO,
        CMD_SIGNAL_CREATE,
        CMD_SIGNAL_DELETE,
        CMD_SIGNAL_SET,
        CMD_SIGNAL_GET,
        CMD_SIGNAL_IS_EXIST
} cmd_t;

typedef struct {
        cmd_t cmd;

        union {
                struct RQ_CMD_GET_SIGNAL_INFO {
                        int n;
                } CMD_GET_SIGNAL_INFO;

                struct RQ_CMD_SIGNAL_CREATE {
                        const char     *name;
                        size_t          size:16;
                        mbus_sig_perm_t perm:8;
                        mbus_sig_type_t type:8;
                } CMD_SIGNAL_CREATE;

                struct RQ_CMD_SIGNAL_DELETE {
                        const char *name;
                        bool        all:1;
                        bool        force:1;
                } CMD_SIGNAL_DELETE;

                struct RQ_CMD_SIGNAL_SET {
                        const char *name;
                        const void *data;
                } CMD_SIGNAL_SET;

                struct RQ_CMD_SIGNAL_GET {
                        const char *name;
                } CMD_SIGNAL_GET;

                struct RQ_CMD_IS_SIGNAL_EXIST {
                        const char *name;
                } CMD_SIGNAL_IS_EXIST;
        } arg;

        queue_t          *response;
        _mbus_owner_ID_t  owner_ID;
} request_t;

typedef struct {
        union {
                struct RES_CMD_GET_NUMBER_OF_SIGNALS {
                        int number_of_signals;
                } CMD_GET_NUMBER_OF_SIGNALS;

                struct RES_CMD_GET_SIGNAL_INFO {
                        mbus_sig_info_t info;
                } CMD_GET_SIGNAL_INFO;

                struct RES_CMD_SIGNAL_GET {
                        const void *data;
                        size_t      size;
                } CMD_SIGNAL_GET;

                struct RES_CMD_IS_SIGNAL_EXIST {
                        bool exist;
                } CMD_SIGNAL_IS_EXIST;
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
 * @param  this                 mbus context
 * @param  request              request to send
 * @param  response             response data
 * @return On success true is returned, otherwise false.
 */
//==============================================================================
static bool request_action(mbus_t *this, request_t *request, response_t *response)
{
        if (mbus) {
                request->response = this->response;
                request->owner_ID = this->owner_ID;

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
 * @brief  Function send response to request
 * @param  request              request to response
 * @param  response             response
 * @return None
 */
//==============================================================================
static void send_response(request_t *request, response_t *response)
{
        queue_send(request->response, response, REQUEST_ACTION_TIMEOUT);
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
        int count = llist_size(mbus->signals);

        response_t response;
        response.of.CMD_GET_NUMBER_OF_SIGNALS.number_of_signals = count;
        response.errorno = count == -1 ? MBUS_ERRNO__INTERNAL_ERROR : MBUS_ERRNO__NO_ERROR;
        send_response(request, &response);
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
        response_t response;
        _mbus_signal_t *sig = llist_at(mbus->signals, request->arg.CMD_GET_SIGNAL_INFO.n);
        if (sig) {
                response.of.CMD_GET_SIGNAL_INFO.info.name        = _mbus_signal_get_name(sig);
                response.of.CMD_GET_SIGNAL_INFO.info.permissions = _mbus_signal_get_permissions(sig);
                response.of.CMD_GET_SIGNAL_INFO.info.size        = _mbus_signal_get_size(sig);
                response.of.CMD_GET_SIGNAL_INFO.info.type        = _mbus_signal_get_type(sig);
                response.errorno                                 = MBUS_ERRNO__NO_ERROR;

        } else {
                response.errorno = MBUS_ERRNO__SIGNAL_NOT_EXIST;
        }

        send_response(request, &response);
}

//==============================================================================
/**
 * @brief  Function realize CMD_SIGNAL_CREATE command
 * @param  request              request data
 * @return None
 */
//==============================================================================
static void realize_CMD_SIGNAL_CREATE(request_t *request)
{
        response_t response;
        response.errorno = MBUS_ERRNO__NOT_ENOUGH_FREE_MEMORY;

        _mbus_signal_t *sig = _mbus_signal_new(request->arg.CMD_SIGNAL_CREATE.name,
                                         request->owner_ID,
                                         request->arg.CMD_SIGNAL_CREATE.size,
                                         request->arg.CMD_SIGNAL_CREATE.perm,
                                         request->arg.CMD_SIGNAL_CREATE.type);
        if (sig) {
                if (llist_find_begin(mbus->signals, sig) == -1) {
                        if (llist_push_back(mbus->signals, sig)) {
                                response.errorno = MBUS_ERRNO__NO_ERROR;
                                llist_sort(mbus->signals);
                        } else {
                                _mbus_signal_delete(sig);
                        }
                } else {
                        _mbus_signal_delete(sig);
                        response.errorno = MBUS_ERRNO__SIGNAL_EXIST;
                }
        }

        send_response(request, &response);
}

//==============================================================================
/**
 * @brief  Function realize CMD_SIGNAL_DELETE command
 * @param  request              request data
 * @return None
 */
//==============================================================================
static void realize_CMD_SIGNAL_DELETE(request_t *request)
{
        response_t response;
        response.errorno = MBUS_ERRNO__SIGNAL_NOT_EXIST;
        int n = 0;

        if (request->arg.CMD_SIGNAL_DELETE.all) {
                int n = llist_size(mbus->signals);
                int i = 0;
                while (i < n) {
                        _mbus_signal_t *sig = llist_at(mbus->signals, i);
                        if (sig) {
                                if (_mbus_signal_get_owner(sig) == request->owner_ID) {
                                        if (llist_erase(mbus->signals, i)) {
                                                continue;
                                        }
                                }
                        } else {
                                break;
                        }

                        i++;
                }

                response.errorno = MBUS_ERRNO__NO_ERROR;

        } else {
                llist_foreach(_mbus_signal_t*, sig, mbus->signals) {
                        if (strcmp(_mbus_signal_get_name(sig), request->arg.CMD_SIGNAL_DELETE.name) == 0) {

                                if (  _mbus_signal_get_owner(sig) == request->owner_ID
                                   || request->arg.CMD_SIGNAL_DELETE.force ) {

                                        llist_erase(mbus->signals, n);
                                        response.errorno = MBUS_ERRNO__NO_ERROR;

                                } else {
                                        response.errorno = MBUS_ERRNO__ACCESS_DENIED;
                                }

                                break;
                        }

                        n++;
                }
        }

        send_response(request, &response);
}

//==============================================================================
/**
 * @brief  Function realize CMD_SIGNAL_SET command
 * @param  request              request data
 * @return None
 */
//==============================================================================
static void realize_CMD_SIGNAL_SET(request_t *request)
{
        response_t response;
        response.errorno = MBUS_ERRNO__SIGNAL_NOT_EXIST;

        llist_foreach(_mbus_signal_t*, sig, mbus->signals) {
                if (strcmp(_mbus_signal_get_name(sig), request->arg.CMD_SIGNAL_SET.name) == 0) {

                        bool is_owner = _mbus_signal_get_owner(sig) == request->owner_ID;
                        mbus_sig_perm_t perm = _mbus_signal_get_permissions(sig);

                        if (is_owner || perm == MBUS_SIG_PERM__READ_WRITE || perm == MBUS_SIG_PERM__WRITE) {
                                response.errorno = _mbus_signal_set_data(sig, request->arg.CMD_SIGNAL_SET.data);
                        } else {
                                response.errorno = MBUS_ERRNO__ACCESS_DENIED;
                        }

                        break;
                }
        }

        send_response(request, &response);
}

//==============================================================================
/**
 * @brief  Function realize CMD_SIGNAL_GET command
 * @param  request              request data
 * @return None
 */
//==============================================================================
static void realize_CMD_SIGNAL_GET(request_t *request)
{
        response_t response;
        response.errorno = MBUS_ERRNO__SIGNAL_NOT_EXIST;
        response.of.CMD_SIGNAL_GET.data = NULL;
        response.of.CMD_SIGNAL_GET.size = 0;

        llist_foreach(_mbus_signal_t*, sig, mbus->signals) {
                if (strcmp(_mbus_signal_get_name(sig), request->arg.CMD_SIGNAL_SET.name) == 0) {

                        bool is_owner = _mbus_signal_get_owner(sig) == request->owner_ID;
                        mbus_sig_perm_t perm = _mbus_signal_get_permissions(sig);

                        if (is_owner || perm == MBUS_SIG_PERM__READ || perm == MBUS_SIG_PERM__READ_WRITE) {

                                void *data;
                                response.errorno = _mbus_signal_get_data(sig, &data);

                                if (response.errorno == MBUS_ERRNO__NO_ERROR) {
                                        response.of.CMD_SIGNAL_GET.data = data;
                                        response.of.CMD_SIGNAL_GET.size = _mbus_signal_get_size(sig);

                                        if (_mbus_signal_get_type(sig) == MBUS_SIG_TYPE__MBOX) {
                                                if (llist_push_back(mbus->garbage, _mbus_garbage_new(data)) == NULL) {
                                                        response.of.CMD_SIGNAL_GET.data = NULL;
                                                        response.of.CMD_SIGNAL_GET.size = 0;
                                                        response.errorno = MBUS_ERRNO__NOT_ENOUGH_FREE_MEMORY;
                                                        free(data);
                                                }
                                        }
                                }
                        } else {
                                response.errorno = MBUS_ERRNO__ACCESS_DENIED;
                        }

                        break;
                }
        }

        send_response(request, &response);
}

//==============================================================================
/**
 * @brief  Function realize CMD_SIGNAL_IS_EXIST command
 * @param  request              request data
 * @return None
 */
//==============================================================================
static void realize_CMD_SIGNAL_IS_EXIST(request_t *request)
{
        response_t response;
        response.errorno = MBUS_ERRNO__SIGNAL_NOT_EXIST;
        response.of.CMD_SIGNAL_IS_EXIST.exist = false;

        llist_foreach(_mbus_signal_t*, sig, mbus->signals) {
                if (strcmp(_mbus_signal_get_name(sig), request->arg.CMD_SIGNAL_IS_EXIST.name) == 0) {
                        response.of.CMD_SIGNAL_IS_EXIST.exist = true;
                        response.errorno = MBUS_ERRNO__NO_ERROR;
                        break;
                }
        }

        send_response(request, &response);
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
        if (mbus && !task_is_exist(mbus->mbus_owner)) {
                mbus->mbus_owner = task_get_handle();
        } else if (mbus) {
                return err;
        }

        // create new object if started first time
        if (!mbus) {
                mbus = malloc(sizeof(struct mbus_mem));
                queue_t *request = queue_new(REQUEST_QUEUE_LENGTH, sizeof(request_t));

                llist_t *signals = llist_new(reinterpret_cast(llist_cmp_functor_t, _mbus_signal_compare),
                                             reinterpret_cast(llist_obj_dtor_t, _mbus_signal_delete));

                llist_t *garbage = llist_new(NULL, reinterpret_cast(llist_obj_dtor_t, _mbus_garbage_delete));

                if (mbus && request && signals && garbage) {
                        mbus->request      = request;
                        mbus->signals      = signals;
                        mbus->garbage      = garbage;
                        mbus->owner_ID_cnt = 0;
                        mbus->mbus_owner   = task_get_handle();

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

                        if (garbage) {
                                llist_delete(garbage);
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

                        case CMD_SIGNAL_CREATE:
                                realize_CMD_SIGNAL_CREATE(&request);
                                break;

                        case CMD_SIGNAL_DELETE:
                                realize_CMD_SIGNAL_DELETE(&request);
                                break;

                        case CMD_SIGNAL_SET:
                                realize_CMD_SIGNAL_SET(&request);
                                break;

                        case CMD_SIGNAL_GET:
                                realize_CMD_SIGNAL_GET(&request);
                                break;

                        case CMD_SIGNAL_IS_EXIST:
                                realize_CMD_SIGNAL_IS_EXIST(&request);
                                break;

                        default:
                                break;
                        }
                }

                int n = llist_size(mbus->garbage);
                int i = 0;
                while (i < n) {
                        _mbus_garbage_t *g = llist_at(mbus->garbage, i);
                        if (g) {
                                if (_mbus_garbage_is_time_expired(g)) {
                                        if (llist_erase(mbus->garbage, i)) {
                                                continue;
                                        }
                                }
                        } else {
                                break;
                        }

                        i++;
                }
        }

        // exit on error
        return err;
}

//==============================================================================
/**
 * @brief  Create a new mbus object
 * @param  None
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
                        this->owner_ID = ++mbus->owner_ID_cnt;
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
 * @param  delete_signals       delete all signals of this owner
 * @return On success true is returned. On error false and error number is set.
 */
//==============================================================================
bool mbus_delete(mbus_t *this, bool delete_signals)
{
        bool status = false;

        if (mbus_is_valid(this)) {
                status = true;

                if (delete_signals) {
                        response_t response;
                        request_t  request;
                        request.cmd = CMD_SIGNAL_DELETE;
                        request.arg.CMD_SIGNAL_DELETE.name = "";
                        request.arg.CMD_SIGNAL_DELETE.all  = true;
                        if (request_action(this, &request, &response)) {
                                status = response.errorno == MBUS_ERRNO__NO_ERROR;
                        }
                }

                if (status) {
                        queue_delete(this->response);
                        this->self = NULL;
                        free(this);
                }
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
                return MBUS_ERRNO__INVALID_OBJECT;
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

        if (mbus_is_valid(this)) {
                if (info) {
                        response_t response;
                        request_t  request;
                        request.cmd = CMD_GET_SIGNAL_INFO;
                        request.arg.CMD_GET_SIGNAL_INFO.n = n;
                        if (request_action(this, &request, &response)) {
                                if (response.errorno == MBUS_ERRNO__NO_ERROR) {
                                        *info  = response.of.CMD_GET_SIGNAL_INFO.info;
                                        status = true;
                                }
                        }
                } else {
                        this->errorno = MBUS_ERRNO__INVALID_ARGUMENT;
                }
        }

        return status;
}

//==============================================================================
/**
 * @brief  Create new signal
 * @param  this                 mbus object
 * @param  name                 signal name
 * @param  size                 signal size
 * @param  type                 signal type
 * @param  permissions          signal permissions
 * @return On success true is returned. On error false and error number is set.
 */
//==============================================================================
bool mbus_signal_create(mbus_t *this, const char *name, size_t size, mbus_sig_type_t type, mbus_sig_perm_t permissions)
{
        bool status = false;

        if (mbus_is_valid(this)) {
                if (name && size > 0 && type < MBUS_SIG_TYPE__INVALID) {
                        response_t response;
                        request_t  request;
                        request.cmd = CMD_SIGNAL_CREATE;
                        request.arg.CMD_SIGNAL_CREATE.name = name;
                        request.arg.CMD_SIGNAL_CREATE.size = size;
                        request.arg.CMD_SIGNAL_CREATE.perm = permissions;
                        request.arg.CMD_SIGNAL_CREATE.type = type;
                        if (request_action(this, &request, &response)) {
                                status = response.errorno == MBUS_ERRNO__NO_ERROR;
                        }
                } else {
                        this->errorno = MBUS_ERRNO__INVALID_ARGUMENT;
                }
        }

        return status;
}

//==============================================================================
/**
 * @brief  Delete signal
 * @param  this                 mbus object
 * @param  name                 name of signal to delete
 * @return On success true is returned. On error false and error number is set.
 */
//==============================================================================
bool mbus_signal_delete(mbus_t *this, const char *name)
{
        bool status = false;

        if (mbus_is_valid(this)) {
                if (name) {
                        response_t response;
                        request_t  request;
                        request.cmd = CMD_SIGNAL_DELETE;
                        request.arg.CMD_SIGNAL_DELETE.name  = name;
                        request.arg.CMD_SIGNAL_DELETE.all   = false;
                        request.arg.CMD_SIGNAL_DELETE.force = false;
                        if (request_action(this, &request, &response)) {
                                status = response.errorno == MBUS_ERRNO__NO_ERROR;
                        }
                } else {
                        this->errorno = MBUS_ERRNO__INVALID_ARGUMENT;
                }
        }

        return status;
}

//==============================================================================
/**
 * @brief  Force delete signal (even signal that caller is not owner)
 * @param  this                 mbus object
 * @param  name                 name of signal to delete
 * @return On success true is returned. On error false and error number is set.
 */
//==============================================================================
bool mbus_signal_force_delete(mbus_t *this, const char *name)
{
        bool status = false;

        if (mbus_is_valid(this)) {
                if (name) {
                        response_t response;
                        request_t  request;
                        request.cmd = CMD_SIGNAL_DELETE;
                        request.arg.CMD_SIGNAL_DELETE.name  = name;
                        request.arg.CMD_SIGNAL_DELETE.all   = false;
                        request.arg.CMD_SIGNAL_DELETE.force = true;
                        if (request_action(this, &request, &response)) {
                                status = response.errorno == MBUS_ERRNO__NO_ERROR;
                        }
                } else {
                        this->errorno = MBUS_ERRNO__INVALID_ARGUMENT;
                }
        }

        return status;
}

//==============================================================================
/**
 * @brief  Set selected signal
 * @param  this                 mbus object
 * @param  name                 name of signal
 * @param  data                 data to set
 * @return On success true is returned. On error false and error number is set.
 */
//==============================================================================
bool mbus_signal_set(mbus_t *this, const char *name, const void *data)
{
        bool status = false;

        if (mbus_is_valid(this)) {
                if (name && data) {
                        response_t response;
                        request_t  request;
                        request.cmd = CMD_SIGNAL_SET;
                        request.arg.CMD_SIGNAL_SET.name = name;
                        request.arg.CMD_SIGNAL_SET.data = data;
                        if (request_action(this, &request, &response)) {
                                status = response.errorno == MBUS_ERRNO__NO_ERROR;
                        }
                } else {
                        this->errorno = MBUS_ERRNO__INVALID_ARGUMENT;
                }
        }

        return status;
}

//==============================================================================
/**
 * @brief  Get data from signal
 * @param  this                 mbus object
 * @param  name                 name of signal
 * @param  data                 data destination
 * @return On success true is returned. On error false and error number is set.
 */
//==============================================================================
bool mbus_signal_get(mbus_t *this, const char *name, void *data)
{
        bool status = false;

        if (mbus_is_valid(this)) {
                if (name && data) {
                        response_t response;
                        request_t  request;
                        request.cmd = CMD_SIGNAL_GET;
                        request.arg.CMD_SIGNAL_GET.name = name;
                        if (request_action(this, &request, &response)) {
                                if (response.of.CMD_SIGNAL_GET.data) {
                                        memcpy(data, response.of.CMD_SIGNAL_GET.data,
                                               response.of.CMD_SIGNAL_GET.size);

                                        status = true;
                                }
                        }
                } else {
                        this->errorno = MBUS_ERRNO__INVALID_ARGUMENT;
                }
        }

        return status;
}

//==============================================================================
/**
 * @brief  Check if signal exists
 * @param  this                 mbus object
 * @param  name                 signal name
 * @return If exists then 1 is returned.
 *         If not exists then 0 is returned.
 *         On error -1 is returned and errorno is set.
 */
//==============================================================================
int mbus_signal_is_exist(mbus_t *this, const char *name)
{
        int status = -1;

        if (mbus_is_valid(this)) {
                if (name) {
                        response_t response;
                        request_t  request;
                        request.cmd = CMD_SIGNAL_IS_EXIST;
                        request.arg.CMD_SIGNAL_IS_EXIST.name = name;
                        if (request_action(this, &request, &response)) {
                                if (response.of.CMD_SIGNAL_IS_EXIST.exist) {
                                        status = 1;
                                } else {
                                        status = 0;
                                }
                        }
                } else {
                        this->errorno = MBUS_ERRNO__INVALID_ARGUMENT;
                }
        }

        return status;
}

/*==============================================================================
 End of file
 ==============================================================================*/
