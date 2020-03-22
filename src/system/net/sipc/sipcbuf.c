/*==============================================================================
File    sipcbuf.c

Author  Daniel Zorychta

Brief   SIPC Network management - buffer.

        Copyright (C) 2019 Daniel Zorychta <daniel.zorychta@gmail.com>

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
#include "sipcbuf.h"
#include "kernel/errno.h"
#include "dnx/misc.h"
#include "kernel/sysfunc.h"

/*==============================================================================
  Local macros
==============================================================================*/

/*==============================================================================
  Local object types
==============================================================================*/
typedef struct data_chain {
        struct data_chain *next;
        bool reference;
        const u8_t *buf;
        size_t len;
} data_chain_t;

struct sipcbuf {
        mutex_t      *access;
        data_chain_t *begin;
        data_chain_t *end;
        size_t        total_size;
        size_t        seek;
        size_t        soft_max_capacity;
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
 * @param  sipcbuf      pointer to destination pointer
 * @param  max_capacity maximum buffer capacity (can exceeds up to MTU)
 *
 * @return One of errno value.
 */
//==============================================================================
int sipcbuf__create(sipcbuf_t **sipcbuf, size_t max_capacity)
{
        sipcbuf_t *this = NULL;

        int err = _kzalloc(_MM_NET, sizeof(sipcbuf_t), (void*)&this);

        if (!err) {
                err = sys_mutex_create(MUTEX_TYPE_NORMAL, &this->access);

                if (!err) {
                        this->soft_max_capacity = max(256, max_capacity);
                        *sipcbuf = this;
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
 * @param  sipcbuf      buffer instance
 */
//==============================================================================
void sipcbuf__destroy(sipcbuf_t *sipcbuf)
{
        if (sipcbuf) {
                sipcbuf__clear(sipcbuf);

                if (sys_mutex_lock(sipcbuf->access, MAX_DELAY_MS)) {
                        mutex_t *mtx = sipcbuf->access;
                        sipcbuf->access = NULL;
                        sys_mutex_unlock(mtx);
                        sys_mutex_destroy(mtx);
                }

                _kfree(_MM_NET, (void*)&sipcbuf);
        }
}

//==============================================================================
/**
 * @brief  Function write data to buffer.
 *
 * @param  sipcbuf      buffer instance
 * @param  data         data source pointer
 * @param  size         data size
 * @param  reference    buffer is reference (no copy)
 *
 * @return One of errno value.
 */
//==============================================================================
int sipcbuf__write(sipcbuf_t *sipcbuf, const u8_t *data, size_t size, bool reference)
{
        int err = EINVAL;

        if (sipcbuf && data && size) {
                err = sys_mutex_lock(sipcbuf->access, MAX_DELAY_MS);
                if (!err) {
                        data_chain_t *chain = NULL;

                        err = _kzalloc(_MM_NET, sizeof(data_chain_t), (void*)&chain);

                        if (!err) {
                                chain->len = size;
                                chain->reference = reference;
                                chain->buf = data;

                                if (sipcbuf->begin == NULL) {
                                        sipcbuf->begin = chain;
                                        sipcbuf->end   = chain;
                                } else {
                                        sipcbuf->end->next = chain;
                                        sipcbuf->end = chain;
                                }

                                sipcbuf->total_size += size;
                        }

                        sys_mutex_unlock(sipcbuf->access);
                }
        }

        return err;
}

//==============================================================================
/**
 * @brief  Function read data from buffer.
 *
 * @param  sipcbuf      buffer instance
 * @param  data         destination data buffer
 * @param  size         destination buffer size
 * @param  rdctr        read counter - number of read bytes
 *
 * @return One of errno value.
 */
//==============================================================================
int sipcbuf__read(sipcbuf_t *sipcbuf, u8_t *data, size_t size, size_t *rdctr)
{
        int err = EINVAL;

        if (sipcbuf && data && size && rdctr) {
                err = sys_mutex_lock(sipcbuf->access, MAX_DELAY_MS);
                if (!err) {

                        *rdctr = 0;
                        data_chain_t *chain = sipcbuf->begin;

                        while (chain && size > 0) {
                                data_chain_t *next = chain->next;

                                size_t tocpy = chain->len - sipcbuf->seek;
                                       tocpy = min(tocpy, size);

                                memcpy(data, &chain->buf[sipcbuf->seek], tocpy);
                                data   += tocpy;
                                *rdctr += tocpy;
                                size   -= tocpy;

                                sipcbuf->seek += tocpy;
                                sipcbuf->total_size -= tocpy;

                                if (sipcbuf->seek >= chain->len) {

                                        sipcbuf->seek = 0;

                                        if (!chain->reference) {
                                                _kfree(_MM_NET, (void*)&chain->buf);
                                        }

                                        _kfree(_MM_NET, (void*)&chain);

                                        sipcbuf->begin = next;

                                        if (sipcbuf->begin == NULL) {
                                                sipcbuf->end = NULL;
                                        }
                                }

                                chain = next;
                        }

                        sys_mutex_unlock(sipcbuf->access);
                }
        }

        return err;
}

//==============================================================================
/**
 * @brief  Function clear data in buffer.
 *
 * @param  sipcbuf      buffer instance
 */
//==============================================================================
void sipcbuf__clear(sipcbuf_t *sipcbuf)
{
        if (sipcbuf) {
                int err = sys_mutex_lock(sipcbuf->access, MAX_DELAY_MS);
                if (!err) {
                        data_chain_t *data = sipcbuf->begin;

                        while (data) {
                                data_chain_t *next = data->next;

                                if (!data->reference) {
                                        _kfree(_MM_NET, (void*)&data->buf);
                                }

                                _kfree(_MM_NET, (void*)&data);

                                data = next;
                        }

                        sipcbuf->begin      = NULL;
                        sipcbuf->end        = NULL;
                        sipcbuf->seek       = 0;
                        sipcbuf->total_size = 0;

                        sys_mutex_unlock(sipcbuf->access);
                }
        }
}

//==============================================================================
/**
 * @brief  Function check if buffer is full
 *
 * @param  sipcbuf      buffer instance
 *
 * @return If buffer is full then true is returned, otherwise false.
 */
//==============================================================================
bool sipcbuf__is_full(sipcbuf_t *sipcbuf)
{
        bool is_full = false;

        if (sipcbuf) {
                int err = sys_mutex_lock(sipcbuf->access, MAX_DELAY_MS);
                if (!err) {
                        is_full = sipcbuf->total_size >= sipcbuf->soft_max_capacity;
                        sys_mutex_unlock(sipcbuf->access);
                }
        }

        return is_full;
}

/*==============================================================================
  End of file
==============================================================================*/
