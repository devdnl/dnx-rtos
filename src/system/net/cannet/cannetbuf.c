/*==============================================================================
File    socbuf.c

Author  Daniel Zorychta

Brief   Socket buffer.

        Copyright (C) 2021 Daniel Zorychta <daniel.zorychta@gmail.com>

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
#include <string.h>
#include "net/cannet/cannet.h"
#include "kernel/errno.h"
#include "lib/misc.h"
#include "kernel/sysfunc.h"

/*==============================================================================
  Local macros
==============================================================================*/
#define CHAIN_BUFFER_LENGTH     32

/*==============================================================================
  Local object types
==============================================================================*/
typedef struct data_chain {
        struct data_chain *next;
        u8_t   seek;
        u8_t   skip;
        u8_t   buf[CHAIN_BUFFER_LENGTH];
} data_chain_t;

struct cannetbuf {
        mutex_t      *mutex;
        data_chain_t *begin;
        data_chain_t *end;
        size_t        total_size;
        size_t        max_capacity;
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
 * @brief  Function create new buffer.
 *
 * @param  self         pointer to destination pointer
 * @param  max_capacity maximum buffer capacity
 *
 * @return One of errno value.
 */
//==============================================================================
int cannetbuf__create(cannetbuf_t **cannetbuf, size_t max_capacity)
{
        cannetbuf_t *this = NULL;

        int err = _kzalloc(_MM_NET, sizeof(cannetbuf_t), NULL, 0, 0, (void*)&this);

        if (!err) {
                err = sys_mutex_create(MUTEX_TYPE_RECURSIVE, &this->mutex);

                if (!err) {
                        this->max_capacity = (CEILING(max(CHAIN_BUFFER_LENGTH, max_capacity),
                                                      CHAIN_BUFFER_LENGTH))
                                           * CHAIN_BUFFER_LENGTH;
                        *cannetbuf = this;
                } else {
                        _kfree(_MM_NET, (void*)&this);
                }
        }

        return err;
}

//==============================================================================
/**
 * @brief  Destroy selected buffer.
 *
 * @param  cannetbuf    buffer instance
 */
//==============================================================================
void cannetbuf__destroy(cannetbuf_t *cannetbuf)
{
        if (cannetbuf) {
                mutex_t *mtx = cannetbuf->mutex;

                if (sys_mutex_lock(cannetbuf->mutex, MAX_DELAY_MS)) {
                        cannetbuf__clear(cannetbuf);
                        cannetbuf->mutex = NULL;
                        sys_mutex_unlock(mtx);
                }

                if (sys_mutex_lock(mtx, MAX_DELAY_MS)) {
                        sys_mutex_unlock(mtx);
                }

                sys_mutex_destroy(mtx);

                _kfree(_MM_NET, (void*)&cannetbuf);
        }
}

//==============================================================================
/**
 * @brief  Function write data to buffer.
 *
 * @param  cannetbuf    buffer instance
 * @param  data         data source pointer
 * @param  size         data size
 *
 * @return One of errno value.
 */
//==============================================================================
int cannetbuf__write(cannetbuf_t *cannetbuf, const u8_t *data, size_t size)
{
        int err = EINVAL;

        if (cannetbuf && data && size) {

                err = sys_mutex_lock(cannetbuf->mutex, MAX_DELAY_MS);
                if (!err) {
                        data_chain_t **chain = &cannetbuf->end;

                        while (!err and (size > 0)) {

                                if (*chain and ((*chain)->seek < sizeof((*chain)->buf))) {

                                        size_t len = min(sizeof((*chain)->buf) - (*chain)->seek, size);
                                        memcpy((*chain)->buf + (*chain)->seek, data, len);
                                        data += len;
                                        size -= len;
                                        (*chain)->seek += len;
                                        cannetbuf->total_size += len;

                                        if ((*chain)->seek >= sizeof((*chain)->buf)) {
                                                chain = &(*chain)->next;
                                        }

                                } else if (cannetbuf->total_size >= cannetbuf->max_capacity) {
                                        err = ENOSPC;

                                } else {
                                        data_chain_t *new_chain;
                                        err = _kzalloc(_MM_NET, sizeof(*new_chain), NULL, 0, 0, (void*)&new_chain);
                                        if (!err) {
                                                if (*chain) {
                                                        (*chain)->next = new_chain;
                                                }

                                                *chain = new_chain;
                                                cannetbuf->end = new_chain;

                                                if (cannetbuf->begin == NULL) {
                                                        cannetbuf->begin = new_chain;
                                                }
                                        }
                                }
                        }

                        sys_mutex_unlock(cannetbuf->mutex);
                }
        }

        return err;
}

//==============================================================================
/**
 * @brief  Copy data from source cannet buffer to another cannet buffer.
 *
 * @param  cannetbuf_dst        destination buffer
 * @param  cannetbuf_src        source buffer
 *
 * @return One of errno value.
 */
//==============================================================================
int cannetbuf__move(cannetbuf_t *cannetbuf_dst, cannetbuf_t *cannetbuf_src)
{
        int err = EINVAL;

        if (cannetbuf_src and cannetbuf_dst) {

                size_t rdctr;
                do {
                        u8_t buf[sizeof(cannetbuf_src->begin->buf)];
                        err = cannetbuf__read(cannetbuf_src, buf, sizeof(buf), &rdctr);

                        if (!err and (rdctr > 0)) {
                                err = cannetbuf__write(cannetbuf_dst, buf, rdctr);
                        }
                } while (!err and (rdctr > 0));
        }

        return err;
}

//==============================================================================
/**
 * @brief  Function read data from buffer.
 *
 * @param  cannetbuf    buffer instance
 * @param  data         destination data buffer
 * @param  size         destination buffer size
 * @param  rdctr        read counter - number of read bytes
 *
 * @return One of errno value.
 */
//==============================================================================
int cannetbuf__read(cannetbuf_t *cannetbuf, u8_t *data, size_t size, size_t *rdctr)
{
        int err = EINVAL;

        if (cannetbuf && data && size && rdctr) {

                err = sys_mutex_lock(cannetbuf->mutex, MAX_DELAY_MS);
                if (!err) {

                        *rdctr = 0;
                        data_chain_t *chain = cannetbuf->begin;

                        while (chain && (size > 0) && (cannetbuf->total_size)) {
                                data_chain_t *next = chain->next;

                                size_t tocpy = chain->seek - chain->skip;
                                       tocpy = min(tocpy, size);

                                memcpy(data, &chain->buf[chain->skip], tocpy);
                                data   += tocpy;
                                *rdctr += tocpy;
                                size   -= tocpy;

                                chain->skip += tocpy;
                                cannetbuf->total_size -= tocpy;

                                if (chain->skip >= chain->seek) {

                                        _kfree(_MM_NET, (void*)&chain);

                                        cannetbuf->begin = next;

                                        if (cannetbuf->begin == NULL) {
                                                cannetbuf->end = NULL;
                                        }
                                }

                                chain = next;
                        }

                        sys_mutex_unlock(cannetbuf->mutex);
                }
        }

        return err;
}

//==============================================================================
/**
 * @brief  Function clear data in buffer.
 *
 * @param  cannetbuf    buffer instance
 */
//==============================================================================
void cannetbuf__clear(cannetbuf_t *cannetbuf)
{
        if (cannetbuf) {
                int err = sys_mutex_lock(cannetbuf->mutex, MAX_DELAY_MS);
                if (!err) {
                        data_chain_t *chain = cannetbuf->begin;

                        while (chain) {
                                data_chain_t *next = chain->next;
                                _kfree(_MM_NET, (void*)&chain);
                                chain = next;
                        }

                        cannetbuf->begin      = NULL;
                        cannetbuf->end        = NULL;
                        cannetbuf->total_size = 0;

                        sys_mutex_unlock(cannetbuf->mutex);
                }
        }
}

//==============================================================================
/**
 * @brief  Function check if buffer is full
 *
 * @param  cannetbuf    buffer instance
 *
 * @return If buffer is full then true is returned, otherwise false.
 */
//==============================================================================
bool cannetbuf__is_full(cannetbuf_t *cannetbuf)
{
        bool is_full = false;

        if (cannetbuf) {
                int err = sys_mutex_lock(cannetbuf->mutex, MAX_DELAY_MS);
                if (!err) {
                        is_full = cannetbuf->total_size >= cannetbuf->max_capacity;
                        sys_mutex_unlock(cannetbuf->mutex);
                }
        }

        return is_full;
}

//==============================================================================
/**
 * @brief  Function return available space in buffer.
 *
 * @param  cannetbuf    buffer instance
 * @param  available    available space
 *
 * @return One of errno value.
 */
//==============================================================================
int cannetbuf__get_available(cannetbuf_t *cannetbuf, size_t *available)
{
        int err = EINVAL;

        if (cannetbuf and available) {
                int err = sys_mutex_lock(cannetbuf->mutex, MAX_DELAY_MS);
                if (!err) {
                        *available = cannetbuf->max_capacity - cannetbuf->total_size;
                        sys_mutex_unlock(cannetbuf->mutex);
                }
        }

        return err;
}

/*==============================================================================
  End of file
==============================================================================*/
