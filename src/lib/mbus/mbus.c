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
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <dnx/thread.h>

/*==============================================================================
  Local macros
==============================================================================*/
#define NUMBER_OF_SLOTS_IN_CHAIN        4
#define variant(_v)                     (variant_t)_v

/*==============================================================================
  Local object types
==============================================================================*/
enum rqid {
        CREATE_SLOT,
        DESTROY_SLOT,
        GET_SLOT,
        CLEAR_SLOT,
        SEND_MSG,
        RECEIVE_MSG,
        CLEAR_MSG
};

typedef union {
        const void     *src_msg;
        void           *dst_msg;
        queue_t        *queue;
        const char     *string;
        mbus_slot_ID_t  slot_ID;
        size_t          size;
        int             integer;
} variant_t;

struct mbus {
        queue_t        *response;
        u32_t           magic;
};

struct slot {
        char           *name;
        void           *data;
        size_t          size;
        mbus_slot_ID_t  ID;
};

struct chain {
        struct slot     slot[NUMBER_OF_SLOTS_IN_CHAIN];
        u8_t            used;
        struct chain   *prev;
        struct chain   *next;
};

struct mbus_mem {
        queue_t        *request_queue;
        task_t         *task;
        struct chain    slots;
        mbus_slot_ID_t  ID_counter;
};

struct request {
        queue_t        *response;
        enum rqid       request;
        variant_t       arg1;
        variant_t       arg2;
};

struct response {
        mbus_status_t   status;
        variant_t       arg1;
};

/*==============================================================================
  Local function prototypes
==============================================================================*/

/*==============================================================================
  Local objects
==============================================================================*/
static const size_t request_queue_len   = 8;
static const size_t name_max_len        = 64;
static const u32_t  mbus_magic          = 0x367C01D2;
static const u32_t  slot_magic          = 0xB15D6196;

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
 * @brief
 */
//==============================================================================
static struct response daemon_request(mbus_t *bus, enum rqid request, variant_t arg1, variant_t arg2)
{
        struct request cmd;
        cmd.request  = request;
        cmd.response = bus->response;
        cmd.arg1     = arg1;
        cmd.arg2     = arg2;

        struct response response;
        response.arg1.size = 0;
        response.status    = MBUS_STATUS_ERROR;

        if (queue_send(mbus->request_queue, &cmd, MAX_DELAY_MS)) {
                queue_receive(bus->response, &response, MAX_DELAY_MS);
        }

        return response;
}

//==============================================================================
/**
 * @brief
 */
//==============================================================================
static mbus_status_t add_slot(const char *slot_name, size_t msg_size)
{
        struct chain *chain = &mbus->slots;
        struct chain *last_chain = chain;

        while (true) {
                for (; chain != NULL; chain = chain->next) {
                        for (int i = 0; i < NUMBER_OF_SLOTS_IN_CHAIN; i++) {
                                struct slot *slot = &chain->slot[i];

                                if (slot->name == NULL) {
                                        size_t len = strnlen(slot_name, name_max_len);
                                        char *name = calloc(len + 1, 1);
                                        if (name) {
                                                strncpy(name, slot_name, len);
                                                slot->name = name;
                                                slot->data = NULL;
                                                slot->size = msg_size;
                                                slot->ID   = ++mbus->ID_counter;
                                                chain->used++;
                                                return MBUS_STATUS_SUCCESS;
                                        } else {
                                                return MBUS_STATUS_ERROR;
                                        }
                                } else {
                                        if (strncmp(slot->name, slot_name, name_max_len) == 0) {
                                                return MBUS_STATUS_SLOT_EXIST;
                                        }
                                }
                        }

                        last_chain = chain;
                }

                /* all slots are used - create new chain */
                struct chain *new_chain = calloc(1, sizeof(struct chain));
                if (new_chain) {
                        new_chain->prev  = last_chain;
                        last_chain->next = new_chain;
                        chain            = new_chain;
                } else {
                        return MBUS_STATUS_ERROR;
                }
        }

        return MBUS_STATUS_ERROR;
}

//==============================================================================
/**
 * @brief
 */
//==============================================================================
static mbus_status_t delete_slot(const char *slot_name)
{
        for (struct chain *chain = &mbus->slots; chain != NULL; chain = chain->next) {
                for (int i = 0; i < NUMBER_OF_SLOTS_IN_CHAIN; i++) {
                        struct slot *slot = &chain->slot[i];

                        if (slot->name) {
                                if (strncmp(slot->name, slot_name, name_max_len) == 0) {
                                        free(slot->name);
                                        slot->name = NULL;
                                        slot->size = 0;
                                        slot->ID   = 0;

                                        if (slot->data) {
                                                free(slot->data);
                                                slot->data = NULL;
                                        }

                                        if (--chain->used == 0 && chain != &mbus->slots) {
                                                chain->prev->next = chain->next;
                                                free(chain);
                                        }

                                        return MBUS_STATUS_SUCCESS;
                                }
                        }
                }
        }

        return MBUS_STATUS_SLOT_NOT_EXIST;
}

//==============================================================================
/**
 * @brief
 */
//==============================================================================
static mbus_slot_ID_t get_slot(const char *slot_name)
{
        for (struct chain *chain = &mbus->slots; chain != NULL; chain = chain->next) {
                for (int i = 0; i < NUMBER_OF_SLOTS_IN_CHAIN; i++) {
                        struct slot *slot = &chain->slot[i];

                        if (slot->name) {
                                if (strncmp(slot->name, slot_name, name_max_len) == 0) {
                                        return slot->ID;
                                }
                        }
                }
        }

        return 0;
}

//==============================================================================
/**
 * @brief
 */
//==============================================================================
static mbus_status_t write_slot(mbus_slot_ID_t slot_ID, const void *msg)
{
        for (struct chain *chain = &mbus->slots; chain != NULL; chain = chain->next) {
                for (int i = 0; i < NUMBER_OF_SLOTS_IN_CHAIN; i++) {
                        if (chain->slot[i].ID == slot_ID) {
                                struct slot *slot = &chain->slot[i];

                                void *data = malloc(slot->size);
                                if (data) {
                                        memcpy(data, msg, slot->size);

                                        if (slot->data) {
                                                free(slot->data);
                                        }
                                        slot->data = data;

                                        return MBUS_STATUS_SUCCESS;
                                } else {
                                        return MBUS_STATUS_ERROR;
                                }
                        }
                }
        }

        return MBUS_STATUS_SLOT_NOT_EXIST;
}

//==============================================================================
/**
 * @brief
 */
//==============================================================================
static mbus_status_t read_slot(mbus_slot_ID_t slot_ID, void *msg)
{
        for (struct chain *chain = &mbus->slots; chain != NULL; chain = chain->next) {
                for (int i = 0; i < NUMBER_OF_SLOTS_IN_CHAIN; i++) {
                        if (chain->slot[i].ID == slot_ID) {
                                struct slot *slot = &chain->slot[i];

                                if (slot->data) {
                                        memcpy(msg, slot->data, slot->size);
                                        return MBUS_STATUS_SUCCESS;
                                } else {
                                        return MBUS_STATUS_SLOT_EMPTY;
                                }
                        }
                }
        }

        return MBUS_STATUS_SLOT_NOT_EXIST;
}

//==============================================================================
/**
 * @brief
 */
//==============================================================================
static mbus_status_t clear_slot(mbus_slot_ID_t slot_ID)
{
        for (struct chain *chain = &mbus->slots; chain != NULL; chain = chain->next) {
                for (int i = 0; i < NUMBER_OF_SLOTS_IN_CHAIN; i++) {
                        if (chain->slot[i].ID == slot_ID) {
                                struct slot *slot = &chain->slot[i];

                                if (slot->data) {
                                        free(slot->data);
                                        slot->data = NULL;
                                        return MBUS_STATUS_SUCCESS;
                                } else {
                                        return MBUS_STATUS_SLOT_EMPTY;
                                }
                        }
                }
        }

        return MBUS_STATUS_SLOT_NOT_EXIST;
}

//==============================================================================
/**
 * @brief
 */
//==============================================================================
mbus_status_t mbus_daemon()
{
        if (mbus) {
                if (task_is_exist(mbus->task)) {
                        return MBUS_STATUS_DAEMON_IS_RUNNING;
                } else {
                        if (mbus->request_queue)
                                queue_delete(mbus->request_queue);

                        mbus = NULL;
                }
        }


        if (!mbus) {
                mbus = calloc(1, sizeof(struct mbus_mem));
                if (!mbus) {
                        return MBUS_STATUS_ERROR;
                }

                mbus->task       = task_get_handle();
                mbus->ID_counter = 0;
        }


        mbus->request_queue = queue_new(request_queue_len, sizeof(struct request));
        if (!mbus->request_queue) {
                free(mbus);
                mbus = NULL;
                return MBUS_STATUS_ERROR;
        }


        while (true) {
                struct request  request;
                struct response response;
                response.status    = MBUS_STATUS_ERROR;
                response.arg1.size = 0;

                if (queue_receive(mbus->request_queue, &request, MAX_DELAY_MS)) {
                        switch (request.request) {
                        case CREATE_SLOT: {
                                response.status = add_slot(request.arg1.string, request.arg2.size);
                                if (response.status == MBUS_STATUS_SUCCESS) {
                                        response.arg1.slot_ID = mbus->ID_counter;
                                }
                                break;
                        }

                        case DESTROY_SLOT: {
                                response.status = delete_slot(request.arg1.string);
                                break;
                        }

                        case GET_SLOT: {
                                mbus_slot_ID_t id = get_slot(request.arg1.string);

                                if (id == 0) {
                                        response.status = MBUS_STATUS_SLOT_NOT_EXIST;
                                } else {
                                        response.status = MBUS_STATUS_SUCCESS;
                                }
                                response.arg1.slot_ID = id;
                                break;
                        }

                        case SEND_MSG: {
                                response.status = write_slot(request.arg1.slot_ID, request.arg2.src_msg);
                                break;
                        }

                        case RECEIVE_MSG: {
                                response.status = read_slot(request.arg1.slot_ID, request.arg2.dst_msg);
                                break;
                        }

                        case CLEAR_MSG: {
                                response.status = clear_slot(request.arg1.slot_ID);
                                break;
                        }

                        default:
                                break;
                        }

                        queue_send(request.response, &response, 0);
                }
        }

        return MBUS_STATUS_ERROR;
}

//==============================================================================
/**
 * @brief
 */
//==============================================================================
mbus_t *mbus_bus_new()
{
        mbus_t *bus = calloc(1, sizeof(mbus_t));
        if (bus) {
                bus->response = queue_new(1, sizeof(struct response));
                if (bus->response) {
                        bus->magic = mbus_magic;
                } else {
                        free(bus);
                        bus = NULL;
                }
        }

        return bus;
}

//==============================================================================
/**
 * @brief
 */
//==============================================================================
mbus_status_t mbus_bus_delete(mbus_t *mbus)
{
        if (mbus) {
                if (mbus->magic == mbus_magic) {
                        queue_delete(mbus->response);
                        mbus->response = NULL;
                        mbus->magic    = 0;
                        free(mbus);
                        return MBUS_STATUS_SUCCESS;
                } else {
                        return MBUS_STATUS_INVALID_OBJECT;
                }
        } else {
                return MBUS_STATUS_INVALID_ARGUMENTS;
        }
}

//==============================================================================
/**
 * @brief
 */
//==============================================================================
mbus_status_t mbus_slot_create(mbus_t *mbus, const char *name, size_t msg_size, mbus_slot_t *slot)
{
        if (mbus && name && msg_size) {
                if (mbus->magic == mbus_magic) {
                        struct response response;
                        response = daemon_request(mbus, CREATE_SLOT, variant(name), variant(msg_size));

                        if (slot) {
                                mbus_slot_t s;
                                s.ID    = response.arg1.slot_ID;
                                s.magic = slot_magic;
                                *slot   = s;
                        }

                        return response.status;
                } else {
                        return MBUS_STATUS_INVALID_OBJECT;
                }
        } else {
                return MBUS_STATUS_INVALID_ARGUMENTS;
        }
}

//==============================================================================
/**
 * @brief
 */
//==============================================================================
mbus_status_t mbus_slot_destroy(mbus_t *mbus, const char *name)
{
        if (mbus && name) {
                if (mbus->magic == mbus_magic) {
                        struct response response;
                        response = daemon_request(mbus, DESTROY_SLOT, variant(name), variant(0));
                        return response.status;
                } else {
                        return MBUS_STATUS_INVALID_OBJECT;
                }
        } else {
                return MBUS_STATUS_INVALID_ARGUMENTS;
        }
}

//==============================================================================
/**
 * @brief
 */
//==============================================================================
mbus_status_t mbus_slot_connect(mbus_t *mbus, const char *name, mbus_slot_t *slot)
{
        if (mbus && name && slot) {
                if (mbus->magic == mbus_magic) {
                        struct response response;
                        response = daemon_request(mbus, GET_SLOT, variant(name), variant(0));

                        slot->ID    = response.arg1.slot_ID;
                        slot->magic = slot_magic;

                        return response.status;
                } else {
                        return MBUS_STATUS_INVALID_OBJECT;
                }
        } else {
                return MBUS_STATUS_INVALID_ARGUMENTS;
        }
}

//==============================================================================
/**
 * @brief
 */
//==============================================================================
mbus_status_t mbus_slot_disconnect(mbus_t *mbus, mbus_slot_t *slot)
{
        if (mbus && slot) {
                if (mbus->magic == mbus_magic && slot->magic == slot_magic) {
                        slot->ID    = 0;
                        slot->magic = 0;
                        return MBUS_STATUS_SUCCESS;
                } else {
                        return MBUS_STATUS_INVALID_OBJECT;
                }
        } else {
                return MBUS_STATUS_INVALID_ARGUMENTS;
        }
}

//==============================================================================
/**
 * @brief
 */
//==============================================================================
mbus_status_t mbus_msg_send(mbus_t *mbus, mbus_slot_t *slot, const void *msg)
{
        if (mbus && slot && msg) {
                if (mbus->magic == mbus_magic && slot->magic == slot_magic && slot->ID) {
                        struct response response;
                        response = daemon_request(mbus, SEND_MSG, variant(slot->ID), variant(msg));
                        return response.status;
                } else {
                        return MBUS_STATUS_INVALID_OBJECT;
                }
        } else {
                return MBUS_STATUS_INVALID_ARGUMENTS;
        }
}

//==============================================================================
/**
 * @brief
 */
//==============================================================================
mbus_status_t mbus_msg_receive(mbus_t *mbus, mbus_slot_t *slot, void *msg)
{
        if (mbus && slot && msg) {
                if (mbus->magic == mbus_magic && slot->magic == slot_magic && slot->ID) {
                        struct response response;
                        response = daemon_request(mbus, RECEIVE_MSG, variant(slot->ID), variant(msg));
                        return response.status;
                } else {
                        return MBUS_STATUS_INVALID_OBJECT;
                }
        } else {
                return MBUS_STATUS_INVALID_ARGUMENTS;
        }
}

//==============================================================================
/**
 * @brief
 */
//==============================================================================
mbus_status_t mbus_msg_clear(mbus_t *mbus, mbus_slot_t *slot)
{
        if (mbus && slot) {
                if (mbus->magic == mbus_magic && slot->magic == slot_magic && slot->ID) {
                        struct response response;
                        response = daemon_request(mbus, CLEAR_MSG, variant(slot->ID), variant(0));
                        return response.status;
                } else {
                        return MBUS_STATUS_INVALID_OBJECT;
                }
        } else {
                return MBUS_STATUS_INVALID_ARGUMENTS;
        }
}

/*==============================================================================
  End of file
==============================================================================*/
