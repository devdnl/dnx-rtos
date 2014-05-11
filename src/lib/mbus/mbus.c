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
#include <stdarg.h>
#include <dnx/thread.h>

/*==============================================================================
  Local macros
==============================================================================*/
/** a number of slots in a single chain */
#define NUMBER_OF_SLOTS_IN_CHAIN        4

/** type casting of variable to variant type */
#define variant(_v)                     (variant_t)_v

/*==============================================================================
  Local object types
==============================================================================*/
/** daemon requests */
enum rqid {
        CREATE_SLOT,            //!< Creates a new slot
        DESTROY_SLOT,           //!< Destroys an existing slot
        GET_SLOT_ID,            //!< Gets a slot ID
        CLEAR_SLOT,             //!< Clears a selected slot
        SEND_MSG,               //!< Sends a message to a selected slot
        RECEIVE_MSG,            //!< Receives a message from a selected slot
        HAS_DATA,               //!< Checks if a slot contains a message
        GET_NUMBER_OF_SLOTS,    //!< Returns a number of created slots
        GET_SLOT_NAME,          //!< Returns a slot name by using interation
        NUMBER_OF_REQUESTS      //!< A number of requests
};

/** type used to pass an arguments in the request and response  */
typedef union {
        const void     *src_msg;
        void           *dst_msg;
        queue_t        *queue;
        const char     *const_string;
        char           *string;
        mbus_slot_ID_t  slot_ID;
        size_t          size;
        int             integer;
} variant_t;

/** main mbus object used to communicate with the daemon */
struct mbus {
        queue_t        *response;
        u32_t           magic;
};

/** a slot definition used internally by the deamon */
struct slot {
        char           *name;
        void           *data;
        size_t          size;
        mbus_slot_ID_t  ID;
};

/** a chain that contains slots */
struct chain {
        struct slot     slot[NUMBER_OF_SLOTS_IN_CHAIN];
        u8_t            used;
        struct chain   *prev;
        struct chain   *next;
};

/** the daemon memory allocation definition */
struct mbus_mem {
        queue_t        *request_queue;
        task_t         *task;
        struct chain    slots;
        mbus_slot_ID_t  ID_counter;
};

/** the deamon request object type */
struct request {
        queue_t        *response;
        enum rqid       request;
        variant_t       arg[3];
};

/** the daemon-client communication object type */
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

/** the shared object */
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
 * @brief Creates the request
 *
 * @param bus           a bus object
 * @param request       a request ID
 * @param ...           an additional arguments depending on the request
 *
 * @return Response object data
 */
//==============================================================================
static struct response daemon_request(mbus_t *bus, enum rqid request, ...)
{
        static const u8_t args[NUMBER_OF_REQUESTS] = {
                [CREATE_SLOT]         = 2,
                [DESTROY_SLOT]        = 1,
                [GET_SLOT_ID]         = 1,
                [CLEAR_SLOT]          = 1,
                [SEND_MSG]            = 2,
                [RECEIVE_MSG]         = 2,
                [HAS_DATA]            = 1,
                [GET_NUMBER_OF_SLOTS] = 0,
                [GET_SLOT_NAME]       = 3,
        };

        va_list arg;
        va_start(arg, request);

        struct request cmd;
        cmd.request  = request;
        cmd.response = bus->response;

        for (int i = 0; i < args[request]; i++) {
                cmd.arg[i] = va_arg(arg, variant_t);
        }

        va_end(arg);

        struct response response;
        response.arg1.size = 0;
        response.status    = MBUS_STATUS_ERROR;

        if (mbus) {
                if (queue_send(mbus->request_queue, &cmd, MAX_DELAY_MS)) {
                        queue_receive(bus->response, &response, MAX_DELAY_MS);
                }
        }

        return response;
}

//==============================================================================
/**
 * @brief Adds a new slot to the chain
 *
 * @param slot_name     a slot name
 * @param msg_size      a message size
 *
 * @return Operation status
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
 * @brief Deletes a selected slot
 *
 * @param slot_name     a slot name to delete
 *
 * @return Operation status
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
 * @brief Returns an ID of a selected slot by its name
 *
 * @param slot_name     a slot name
 *
 * @return A slot ID
 */
//==============================================================================
static mbus_slot_ID_t get_slot_ID(const char *slot_name)
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
 * @brief Writes a data to a selected slot
 *
 * @param slot_ID       a slot ID
 * @param msg           a message to write
 *
 * @return Operation status
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
 * @brief Reads a data from a selected slot
 *
 * @param slot_ID       a slot ID
 * @param msg           a buffer where a message will be copied
 *
 * @return Operation status
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
 * @brief Clears a message from a selected slot
 *
 * @param slot_ID       a slot ID
 *
 * @return Operation status
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
 * @brief Checks if a slot contains message
 *
 * @param slot_ID       a slot ID
 *
 * @return Operation status
 */
//==============================================================================
static mbus_status_t is_msg_in_slot(mbus_slot_ID_t slot_ID)
{
        for (struct chain *chain = &mbus->slots; chain != NULL; chain = chain->next) {
                for (int i = 0; i < NUMBER_OF_SLOTS_IN_CHAIN; i++) {
                        if (chain->slot[i].ID == slot_ID) {
                                struct slot *slot = &chain->slot[i];

                                if (slot->data) {
                                        return MBUS_STATUS_SLOT_HAS_MSG;
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
 * @brief Returns a number of all created slots
 *
 * @return A number of all created slots
 */
//==============================================================================
static uint get_number_of_slots()
{
        uint counter = 0;

        for (struct chain *chain = &mbus->slots; chain != NULL; chain = chain->next) {
                counter += chain->used;
        }

        return counter;
}

//==============================================================================
/**
 * @brief Returns a name of a selected slot by using interation
 *
 * @return A number of all created slots
 */
//==============================================================================
static mbus_status_t get_slot_name(uint n, char *buf, size_t buf_len)
{
        uint counter = 0;

        for (struct chain *chain = &mbus->slots; chain != NULL; chain = chain->next) {
                for (int i = 0; i < NUMBER_OF_SLOTS_IN_CHAIN; i++) {
                        struct slot *slot = &chain->slot[i];

                        if (slot->name && n == counter) {
                                strncpy(buf, slot->name, buf_len - 1);
                                return MBUS_STATUS_SUCCESS;
                        } else {
                                counter++;
                        }
                }
        }

        return MBUS_STATUS_SLOT_NOT_EXIST;
}

//==============================================================================
/**
 * @brief The main function of the daemon. This function must be started in the
 *        program (or task).
 *
 * @return The function if starts correctly never returns. If error will occured
 *         the function returns an operation error.
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
                                response.status = add_slot(request.arg[0].const_string, request.arg[1].size);
                                if (response.status == MBUS_STATUS_SUCCESS) {
                                        response.arg1.slot_ID = mbus->ID_counter;
                                }
                                break;
                        }

                        case DESTROY_SLOT: {
                                response.status = delete_slot(request.arg[0].const_string);
                                break;
                        }

                        case GET_SLOT_ID: {
                                mbus_slot_ID_t id = get_slot_ID(request.arg[0].const_string);

                                if (id == 0) {
                                        response.status = MBUS_STATUS_SLOT_NOT_EXIST;
                                } else {
                                        response.status = MBUS_STATUS_SUCCESS;
                                }
                                response.arg1.slot_ID = id;
                                break;
                        }

                        case SEND_MSG: {
                                response.status = write_slot(request.arg[0].slot_ID, request.arg[1].src_msg);
                                break;
                        }

                        case RECEIVE_MSG: {
                                response.status = read_slot(request.arg[0].slot_ID, request.arg[1].dst_msg);
                                break;
                        }

                        case CLEAR_SLOT: {
                                response.status = clear_slot(request.arg[0].slot_ID);
                                break;
                        }

                        case HAS_DATA: {
                                response.status = is_msg_in_slot(request.arg[0].slot_ID);
                                break;
                        }

                        case GET_NUMBER_OF_SLOTS: {
                                response.status       = MBUS_STATUS_SUCCESS;
                                response.arg1.integer = get_number_of_slots();
                                break;
                        }

                        case GET_SLOT_NAME: {
                                response.status = get_slot_name(request.arg[0].integer,
                                                                request.arg[1].string,
                                                                request.arg[2].size);
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
 * @brief Creates a new bus communication object
 *
 * @return On success, bus object is returned. On error, NULL is returned.
 *
 * @example
 * #include "lib/mbus/mbus.h"
 *
 * // ...
 *
 * mbus_t *bus = mbus_bus_new();
 * if (bus) {
 *
 *      // creates a new slot to communicate with
 *      mbus_slot_t slot;
 *      mbus_slot_create(bus, "slot_name", sizeof(u32_t),&slot);
 *
 *      // sends a data object
 *      u32_t data = 15;
 *      mbus_msg_send(bus, &slot, &data);
 *
 *      // ...
 *
 *      mbus_msg_receive(bus, &slot, &data);
 *
 *      // ...
 *
 *      // clears a slot
 *      mbus_slot_clear(bus, &slot);
 *
 *      // disconnects a slot
 *      mbus_slot_disconnect(bus, &slot);
 *
 *      // destroys created slot
 *      mbus_slot_destroy(bus, "slot_name");
 *
 *      // deletes communication object
 *      mbus_bus_delete(bus);
 * }
 */
//==============================================================================
mbus_t *mbus_bus_new()
{
        mbus_t *bus = NULL;

        if (mbus) {
                if (task_is_exist(mbus->task)) {
                        bus = calloc(1, sizeof(mbus_t));
                        if (bus) {
                                bus->response = queue_new(1, sizeof(struct response));
                                if (bus->response) {
                                        bus->magic = mbus_magic;
                                } else {
                                        free(bus);
                                        bus = NULL;
                                }
                        }
                }
        }

        return bus;
}

//==============================================================================
/**
 * @brief Deletes a selected bus object (closes the communication with mbus deamon)
 *
 * @param mbus          a bus object
 *
 * @return Operation status
 *
 * @example
 * #include "lib/mbus/mbus.h"
 *
 * // ...
 *
 * mbus_t *bus = mbus_bus_new();
 * if (bus) {
 *
 *      // creates a new slot to communicate with
 *      mbus_slot_t slot;
 *      mbus_slot_create(bus, "slot_name", sizeof(u32_t),&slot);
 *
 *      // sends a data object
 *      u32_t data = 15;
 *      mbus_msg_send(bus, &slot, &data);
 *
 *      // ...
 *
 *      mbus_msg_receive(bus, &slot, &data);
 *
 *      // ...
 *
 *      // clears a slot
 *      mbus_slot_clear(bus, &slot);
 *
 *      // disconnects a slot
 *      mbus_slot_disconnect(bus, &slot);
 *
 *      // destroys created slot
 *      mbus_slot_destroy(bus, "slot_name");
 *
 *      // deletes communication object
 *      mbus_bus_delete(bus);
 * }
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
 * @brief Returns a number of created slots
 *
 * @param mbus          a bus object
 * @param number        an output pointer to variable
 *
 * @return Operation status
 *
 * @example
 * #include "lib/mbus/mbus.h"
 *
 * // ...
 *
 * mbus_t *bus = mbus_bus_new();
 * if (bus) {
 *
 *      // creates a new slot to communicate with
 *      mbus_slot_t slot;
 *      mbus_slot_create(bus, "slot_name", sizeof(u32_t),&slot);
 *
 *      // ...
 *
 *      char *name = calloc(1, 64);
 *      if (name) {
 *            uint slots = 0;
 *            mbus_bus_get_number_of_slots(bus, &slots);
 *
 *            for (uint slot = 0; slot < slots; slot++) {
 *                  mbus_bus_get_slot_name(bus, slot, name, 64);
 *                  puts(name);
 *            }
 *
 *            free(name);
 *      }
 *
 *      // ...
 *
 *      // destroys created slot
 *      mbus_slot_destroy(bus, "slot_name");
 *
 *      // deletes communication object
 *      mbus_bus_delete(bus);
 * }
 */
//==============================================================================
mbus_status_t mbus_bus_get_number_of_slots(mbus_t *mbus, uint *number)
{
        if (mbus && number) {
                if (mbus->magic == mbus_magic) {
                        struct response response;
                        response = daemon_request(mbus, GET_NUMBER_OF_SLOTS);
                        *number  = response.arg1.integer;
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
 * @brief Returns a name of a selected slot by using iteration
 *
 * @param mbus          a bus object
 * @param n             a n-slot
 * @param name          a pointer to buffer where name will be written
 * @param buf_len       a size of a buffer used to store a slot name
 *
 * @return Operation status
 *
 * @example
 * #include "lib/mbus/mbus.h"
 *
 * // ...
 *
 * mbus_t *bus = mbus_bus_new();
 * if (bus) {
 *
 *      // creates a new slot to communicate with
 *      mbus_slot_t slot;
 *      mbus_slot_create(bus, "slot_name", sizeof(u32_t),&slot);
 *
 *      // ...
 *
 *      char *name = calloc(1, 64);
 *      if (name) {
 *            uint slots = 0;
 *            mbus_bus_get_number_of_slots(bus, &slots);
 *
 *            for (uint slot = 0; slot < slots; slot++) {
 *                  mbus_bus_get_slot_name(bus, slot, name, 64);
 *                  puts(name);
 *            }
 *
 *            free(name);
 *      }
 *
 *      // ...
 *
 *      // destroys created slot
 *      mbus_slot_destroy(bus, "slot_name");
 *
 *      // deletes communication object
 *      mbus_bus_delete(bus);
 * }
 */
//==============================================================================
mbus_status_t mbus_bus_get_slot_name(mbus_t *mbus, uint n, char *name, size_t buf_len)
{
        if (mbus && name && buf_len) {
                if (mbus->magic == mbus_magic) {
                        return daemon_request(mbus, GET_SLOT_NAME,
                                              variant(n),
                                              variant(name),
                                              variant(buf_len)).status;
                } else {
                        return MBUS_STATUS_INVALID_OBJECT;
                }
        } else {
                return MBUS_STATUS_INVALID_ARGUMENTS;
        }
}

//==============================================================================
/**
 * @brief Creates a new slot by name and a message size
 *
 * @param mbus          a bus object
 * @param name          a new slot name
 * @param msg_size      a new slot message size
 * @param slot          an output pointer to a slot object (can be NULL)
 *
 * @return Operation status
 *
 * @example
 * #include "lib/mbus/mbus.h"
 *
 * // ...
 *
 * mbus_t *bus = mbus_bus_new();
 * if (bus) {
 *
 *      // creates a new slot to communicate with
 *      mbus_slot_t slot;
 *      mbus_slot_create(bus, "slot_name", sizeof(u32_t),&slot);
 *
 *      // sends a data object
 *      u32_t data = 15;
 *      mbus_msg_send(bus, &slot, &data);
 *
 *      // ...
 *
 *      mbus_msg_receive(bus, &slot, &data);
 *
 *      // ...
 *
 *      // clears a slot
 *      mbus_slot_clear(bus, &slot);
 *
 *      // disconnects a slot
 *      mbus_slot_disconnect(bus, &slot);
 *
 *      // destroys created slot
 *      mbus_slot_destroy(bus, "slot_name");
 *
 *      // deletes communication object
 *      mbus_bus_delete(bus);
 * }
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
 * @brief Destroys a selected slot by name
 *
 * @param mbus          a bus object
 * @param name          a slot name to destroy
 *
 * @return Operation status
 *
 * @example
 * #include "lib/mbus/mbus.h"
 *
 * // ...
 *
 * mbus_t *bus = mbus_bus_new();
 * if (bus) {
 *
 *      // creates a new slot to communicate with
 *      mbus_slot_t slot;
 *      mbus_slot_create(bus, "slot_name", sizeof(u32_t),&slot);
 *
 *      // sends a data object
 *      u32_t data = 15;
 *      mbus_msg_send(bus, &slot, &data);
 *
 *      // ...
 *
 *      mbus_msg_receive(bus, &slot, &data);
 *
 *      // ...
 *
 *      // clears a slot
 *      mbus_slot_clear(bus, &slot);
 *
 *      // disconnects a slot
 *      mbus_slot_disconnect(bus, &slot);
 *
 *      // destroys created slot
 *      mbus_slot_destroy(bus, "slot_name");
 *
 *      // deletes communication object
 *      mbus_bus_delete(bus);
 */
//==============================================================================
mbus_status_t mbus_slot_destroy(mbus_t *mbus, const char *name)
{
        if (mbus && name) {
                if (mbus->magic == mbus_magic) {
                        return daemon_request(mbus, DESTROY_SLOT, variant(name)).status;
                } else {
                        return MBUS_STATUS_INVALID_OBJECT;
                }
        } else {
                return MBUS_STATUS_INVALID_ARGUMENTS;
        }
}

//==============================================================================
/**
 * @brief Connects to a selected slot
 *
 * @param mbus          a bus object
 * @param name          a slot name to connect
 * @param slot          a output pointer to slot object
 *
 * @return Operation status
 *
 * @example
 * #include "lib/mbus/mbus.h"
 *
 * // ...
 *
 * mbus_t *bus = mbus_bus_new();
 * if (bus) {
 *
 *      // creates a new slot to communicate with
 *      mbus_slot_t slot;
 *      mbus_slot_create(bus, "slot_name", sizeof(u32_t),&slot);
 *
 *      // sends a data object
 *      u32_t data = 15;
 *      mbus_msg_send(bus, &slot, &data);
 *
 *      // ...
 *
 *      mbus_msg_receive(bus, &slot, &data);
 *
 *      // ...
 *
 *      // clears a slot
 *      mbus_slot_clear(bus, &slot);
 *
 *      // disconnects a slot
 *      mbus_slot_disconnect(bus, &slot);
 *
 *      // destroys created slot
 *      mbus_slot_destroy(bus, "slot_name");
 *
 *      // deletes communication object
 *      mbus_bus_delete(bus);
 */
//==============================================================================
mbus_status_t mbus_slot_connect(mbus_t *mbus, const char *name, mbus_slot_t *slot)
{
        if (mbus && name && slot) {
                if (mbus->magic == mbus_magic) {
                        struct response response;
                        response = daemon_request(mbus, GET_SLOT_ID, variant(name));

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
 * @brief Disconnects a selected slot
 *
 * @param mbus          a bus object
 * @param slot          a slot to disconnect
 *
 * @return Operation status
 *
 * @example
 * #include "lib/mbus/mbus.h"
 *
 * // ...
 *
 * mbus_t *bus = mbus_bus_new();
 * if (bus) {
 *
 *      // creates a new slot to communicate with
 *      mbus_slot_t slot;
 *      mbus_slot_create(bus, "slot_name", sizeof(u32_t),&slot);
 *
 *      // sends a data object
 *      u32_t data = 15;
 *      mbus_msg_send(bus, &slot, &data);
 *
 *      // ...
 *
 *      mbus_msg_receive(bus, &slot, &data);
 *
 *      // ...
 *
 *      // clears a slot
 *      mbus_slot_clear(bus, &slot);
 *
 *      // disconnects a slot
 *      mbus_slot_disconnect(bus, &slot);
 *
 *      // destroys created slot
 *      mbus_slot_destroy(bus, "slot_name");
 *
 *      // deletes communication object
 *      mbus_bus_delete(bus);
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
 * @brief Clears a data from a selected slot
 *
 * @param mbus          a bus object
 * @param slot          a slot to clear
 *
 * @return Operation status
 *
 * @example
 * #include "lib/mbus/mbus.h"
 *
 * // ...
 *
 * mbus_t *bus = mbus_bus_new();
 * if (bus) {
 *
 *      // creates a new slot to communicate with
 *      mbus_slot_t slot;
 *      mbus_slot_create(bus, "slot_name", sizeof(u32_t),&slot);
 *
 *      // sends a data object
 *      u32_t data = 15;
 *      mbus_msg_send(bus, &slot, &data);
 *
 *      // ...
 *
 *      mbus_msg_receive(bus, &slot, &data);
 *
 *      // ...
 *
 *      // clears a slot
 *      mbus_slot_clear(bus, &slot);
 *
 *      // disconnects a slot
 *      mbus_slot_disconnect(bus, &slot);
 *
 *      // destroys created slot
 *      mbus_slot_destroy(bus, "slot_name");
 *
 *      // deletes communication object
 *      mbus_bus_delete(bus);
 */
//==============================================================================
mbus_status_t mbus_slot_clear(mbus_t *mbus, mbus_slot_t *slot)
{
        if (mbus && slot) {
                if (mbus->magic == mbus_magic && slot->magic == slot_magic && slot->ID) {
                        return daemon_request(mbus, CLEAR_SLOT, variant(slot->ID)).status;
                } else {
                        return MBUS_STATUS_INVALID_OBJECT;
                }
        } else {
                return MBUS_STATUS_INVALID_ARGUMENTS;
        }
}

//==============================================================================
/**
 * @brief Checks if a message exist in a selected slot
 *
 * @param mbus          a bus object
 * @param slot          a slot to check
 *
 * @return Operation status
 *
 * @example
 * #include "lib/mbus/mbus.h"
 * #include <unistd.h>
 *
 * // ...
 *
 * mbus_t *bus = mbus_bus_new();
 * if (bus) {
 *
 *      // creates a new slot to communicate with
 *      mbus_slot_t slot;
 *      mbus_slot_create(bus, "slot_name", sizeof(u32_t),&slot);
 *
 *      // sends a data object
 *      u32_t data = 15;
 *      mbus_msg_send(bus, &slot, &data);
 *
 *      // ...
 *
 *      while (mbus_slot_has_msg != MBUS_STATUS_SLOT_WITH_DATA) {
 *           sleep_ms(100);
 *      }
 *
 *      mbus_msg_receive(bus, &slot, &data);
 *
 *      // ...
 *
 *      // clears a slot
 *      mbus_slot_clear(bus, &slot);
 *
 *      // disconnects a slot
 *      mbus_slot_disconnect(bus, &slot);
 *
 *      // destroys created slot
 *      mbus_slot_destroy(bus, "slot_name");
 *
 *      // deletes communication object
 *      mbus_bus_delete(bus);
 *
 */
//==============================================================================
mbus_status_t mbus_slot_has_msg(mbus_t *mbus, mbus_slot_t *slot)
{
        if (mbus && slot) {
                if (mbus->magic == mbus_magic && slot->magic == slot_magic && slot->ID) {
                        return daemon_request(mbus, HAS_DATA, variant(slot->ID)).status;
                } else {
                        return MBUS_STATUS_INVALID_OBJECT;
                }
        } else {
                return MBUS_STATUS_INVALID_ARGUMENTS;
        }
}

//==============================================================================
/**
 * @brief Sends a message to a selected slot
 *
 * @param mbus          a bus object
 * @param slot          a slot object to write to
 * @param msg           a message to send
 *
 * @return Operation status
 *
 * @example
 * #include "lib/mbus/mbus.h"
 *
 * // ...
 *
 * mbus_t *bus = mbus_bus_new();
 * if (bus) {
 *
 *      // creates a new slot to communicate with
 *      mbus_slot_t slot;
 *      mbus_slot_create(bus, "slot_name", sizeof(u32_t),&slot);
 *
 *      // sends a data object
 *      u32_t data = 15;
 *      mbus_msg_send(bus, &slot, &data);
 *
 *      // ...
 *
 *      mbus_msg_receive(bus, &slot, &data);
 *
 *      // ...
 *
 *      // clears a slot
 *      mbus_slot_clear(bus, &slot);
 *
 *      // disconnects a slot
 *      mbus_slot_disconnect(bus, &slot);
 *
 *      // destroys created slot
 *      mbus_slot_destroy(bus, "slot_name");
 *
 *      // deletes communication object
 *      mbus_bus_delete(bus);
 */
//==============================================================================
mbus_status_t mbus_msg_send(mbus_t *mbus, mbus_slot_t *slot, const void *msg)
{
        if (mbus && slot && msg) {
                if (mbus->magic == mbus_magic && slot->magic == slot_magic && slot->ID) {
                        return daemon_request(mbus, SEND_MSG, variant(slot->ID), variant(msg)).status;
                } else {
                        return MBUS_STATUS_INVALID_OBJECT;
                }
        } else {
                return MBUS_STATUS_INVALID_ARGUMENTS;
        }
}

//==============================================================================
/**
 * @brief Receives a data from a selected slot
 *
 * @param mbus          a bus object
 * @param slot          a slot object to read from
 * @param msg           a message pointer where data will be stored
 *
 * @return Operation status
 *
 * @example
 * #include "lib/mbus/mbus.h"
 *
 * // ...
 *
 * mbus_t *bus = mbus_bus_new();
 * if (bus) {
 *
 *      // creates a new slot to communicate with
 *      mbus_slot_t slot;
 *      mbus_slot_create(bus, "slot_name", sizeof(u32_t),&slot);
 *
 *      // sends a data object
 *      u32_t data = 15;
 *      mbus_msg_send(bus, &slot, &data);
 *
 *      // ...
 *
 *      mbus_msg_receive(bus, &slot, &data);
 *
 *      // ...
 *
 *      // clears a slot
 *      mbus_slot_clear(bus, &slot);
 *
 *      // disconnects a slot
 *      mbus_slot_disconnect(bus, &slot);
 *
 *      // destroys created slot
 *      mbus_slot_destroy(bus, "slot_name");
 *
 *      // deletes communication object
 *      mbus_bus_delete(bus);
 */
//==============================================================================
mbus_status_t mbus_msg_receive(mbus_t *mbus, mbus_slot_t *slot, void *msg)
{
        if (mbus && slot && msg) {
                if (mbus->magic == mbus_magic && slot->magic == slot_magic && slot->ID) {
                        return daemon_request(mbus, RECEIVE_MSG, variant(slot->ID), variant(msg)).status;
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
