/*=========================================================================*//**
 @file    mbus.c

 @author  Daniel Zorychta

 @brief   Message Bus Library. Signal class.

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
#include <string.h>
#include <stdlib.h>
#include <dnx/thread.h>
#include <dnx/misc.h>

/*==============================================================================
 Local macros
 ==============================================================================*/
#define MAX_NAME_LENGTH         128
#define MBOX_QUEUE_LENGTH       8

/*==============================================================================
 Local object types
 ==============================================================================*/
struct _mbus_signal {
        char            *name;
        void            *data;
        void            *self;
        _mbus_owner_ID_t owner_ID;
        size_t           size;
        mbus_sig_perm_t  perm;
        mbus_sig_type_t  type;
};

/*==============================================================================
 Local function prototypes
 ==============================================================================*/

/*==============================================================================
 Local objects
 ==============================================================================*/

/*==============================================================================
 Shared objects
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
 * @brief  Function check if signal object is valid
 * @param  this                 signal object
 * @return If object is valid then true is returned, otherwise false.
 */
//==============================================================================
static bool signal_is_valid(_mbus_signal_t *this)
{
        return this && this->self == this && this->name && this->data && this->size && this->owner_ID > 0;
}

//==============================================================================
/**
 * @brief  Create new signal
 * @param  name                 name of signal
 * @param  owner_ID             signal owner
 * @param  size                 signal size
 * @param  perm                 signal permissions
 * @param  type                 signal type
 * @return Created object or NULL on error.
 */
//==============================================================================
_mbus_signal_t *_mbus_signal_new(const char *name, _mbus_owner_ID_t owner_ID, size_t size,
                                 mbus_sig_perm_t perm, mbus_sig_type_t type)
{
        _mbus_signal_t *this  = malloc(sizeof(_mbus_signal_t));
        char     *signal_name = malloc(strnlen(name, MAX_NAME_LENGTH) + 1);
        void     *data        = NULL;

        if (type == MBUS_SIG_TYPE__MBOX) {
                data = queue_new(MBOX_QUEUE_LENGTH, sizeof(void*));

        } else if (type == MBUS_SIG_TYPE__VALUE) {
                data = calloc(1, size);
        }

        if (this && signal_name && data) {
                strcpy(signal_name, name);

                this->data     = data;
                this->name     = signal_name;
                this->owner_ID = owner_ID;
                this->perm     = perm;
                this->size     = size;
                this->type     = type;
                this->self     = this;

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
 * @brief  Delete signal
 * @param  this                 signal to delete
 * @return None
 */
//==============================================================================
void _mbus_signal_delete(_mbus_signal_t *this)
{
        if (signal_is_valid(this)) {
                free(this->name);
                this->name     = NULL;
                this->self     = NULL;
                this->owner_ID = 0;
                this->size     = 0;
                this->perm     = MBUS_SIG_PERM__INVALID;
                this->type     = MBUS_SIG_TYPE__INVALID;

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
 * @brief  Get name of the signal
 * @param  this                 signal object
 * @return Return name pointer or empty string on error.
 */
//==============================================================================
char *_mbus_signal_get_name(_mbus_signal_t *this)
{
        if (signal_is_valid(this)) {
                return this->name;
        } else {
                return "";
        }
}

//==============================================================================
/**
 * @brief  Get signal type
 * @param  this                 signal object
 * @return Return signal type
 */
//==============================================================================
mbus_sig_type_t _mbus_signal_get_type(_mbus_signal_t *this)
{
        if (signal_is_valid(this)) {
                return this->type;
        } else {
                return MBUS_SIG_TYPE__INVALID;
        }
}

//==============================================================================
/**
 * @brief  Get signal size
 * @param  this                 signal object
 * @return Return signal size, 0 on error
 */
//==============================================================================
size_t _mbus_signal_get_size(_mbus_signal_t *this)
{
        if (signal_is_valid(this)) {
                return this->size;
        } else {
                return 0;
        }
}

//==============================================================================
/**
 * @brief  Get signal owner
 * @param  this                 signal object
 * @return Return signal owner or 0 on error.
 */
//==============================================================================
_mbus_owner_ID_t _mbus_signal_get_owner(_mbus_signal_t *this)
{
        if (signal_is_valid(this)) {
                return this->owner_ID;
        } else {
                return 0;
        }
}

//==============================================================================
/**
 * @brief  Get signal permissions
 * @param  this                 signal object
 * @return Return permissions.
 */
//==============================================================================
mbus_sig_perm_t _mbus_signal_get_permissions(_mbus_signal_t *this)
{
        if (signal_is_valid(this)) {
                return this->perm;
        } else {
                return MBUS_SIG_PERM__INVALID;
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
mbus_errno_t _mbus_signal_set_data(_mbus_signal_t *this, const void *data)
{
        mbus_errno_t err = MBUS_ERRNO__INTERNAL_ERROR;

        if (signal_is_valid(this) && data) {
                err = MBUS_ERRNO__NO_ERROR;

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
mbus_errno_t _mbus_signal_get_data(_mbus_signal_t *this, void **data)
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
 * @brief  Compare signals (by name)
 * @param  first                first object
 * @param  second               second object
 * @return Error number
 */
//==============================================================================
int _mbus_signal_compare(_mbus_signal_t *first, _mbus_signal_t *second)
{
        if (signal_is_valid(first) && signal_is_valid(second)) {
                return strcmp(first->name, second->name);
        } else {
                return 1;
        }
}

/*==============================================================================
 End of file
 ==============================================================================*/
