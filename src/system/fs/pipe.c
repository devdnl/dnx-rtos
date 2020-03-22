/*=========================================================================*//**
@file    pipe.c

@author  Daniel Zorychta

@brief   File support creating of pipies in file systems.

@note    Copyright (C) 2014-2015 Daniel Zorychta <daniel.zorychta@gmail.com>

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


*//*==========================================================================*/

/*==============================================================================
  Include files
==============================================================================*/
#include "config.h"
#include <sys/types.h>
#include "dnx/misc.h"
#include "libc/errno.h"
#include "kernel/kwrapper.h"
#include "fs/pipe.h"

/*==============================================================================
  Local macros
==============================================================================*/
#define CLOSED          (1<<0)
#define PERMANENT       (1<<1)

/*==============================================================================
  Local object types
==============================================================================*/
struct pipe {
        queue_t     *queue;
        struct pipe *self;
        u32_t        flag;
};

/*==============================================================================
  Local function prototypes
==============================================================================*/

/*==============================================================================
  Local objects
==============================================================================*/
#if __OS_ENABLE_MKFIFO__ == _YES_
static const u32_t PIPE_READ_TIMEOUT  = MAX_DELAY_MS;
static const u32_t PIPE_WRITE_TIMEOUT = MAX_DELAY_MS;
#endif

/*==============================================================================
  Exported objects
==============================================================================*/

/*==============================================================================
  External objects
==============================================================================*/

/*==============================================================================
  Function definitions
==============================================================================*/

#if __OS_ENABLE_MKFIFO__ == _YES_
//==============================================================================
/**
 * @brief  Check if pipe is valid
 * @param  this         pipe object
 * @return true if valid, otherwise false
 */
//==============================================================================
static bool is_valid(pipe_t *this)
{
        return this && this->self == this;
}
#endif

//==============================================================================
/**
 * @brief Create pipe object
 *
 * @param[out] pipe     pointer to pointer of pipe handle
 *
 * @return One of errno value.
 */
//==============================================================================
int _pipe_create(pipe_t **pipe)
{
#if __OS_ENABLE_MKFIFO__ == _YES_
        int err = EINVAL;

        if (pipe) {
                err = _kmalloc(_MM_KRN, sizeof(pipe_t), cast(void**, pipe));
                if (err == ESUCC) {

                        err = _queue_create(__OS_PIPE_LENGTH__, sizeof(u8_t), &(*pipe)->queue);
                        if (err == ESUCC) {
                                (*pipe)->self = *pipe;
                                (*pipe)->flag = 0;
                        } else {
                                _kfree(_MM_KRN, cast(void**, pipe));
                        }
                }
        }

        return err;
#else
        UNUSED_ARG1(pipe);
        return ENOTSUP;
#endif
}

//==============================================================================
/**
 * @brief Destroy pipe object
 *
 * @param pipe          a pipe object
 *
 * @return One of errno value.
 */
//==============================================================================
int _pipe_destroy(pipe_t *pipe)
{
#if __OS_ENABLE_MKFIFO__ == _YES_
        if (is_valid(pipe)) {
                _queue_destroy(pipe->queue);
                pipe->self = NULL;
                _kfree(_MM_KRN, cast(void**, &pipe));
                return ESUCC;
        } else {
                return EINVAL;
        }
#else
        UNUSED_ARG1(pipe);
        return ENOTSUP;
#endif
}

//==============================================================================
/**
 * @brief Return length of pipe
 *
 * @param pipe          a pipe object
 * @param len           a pipe length
 *
 * @return One of errno value.
 */
//==============================================================================
int _pipe_get_length(pipe_t *pipe, size_t *len)
{
#if __OS_ENABLE_MKFIFO__ == _YES_
        if (len && is_valid(pipe)) {
                return _queue_get_number_of_items(pipe->queue, len);
        } else {
                return EINVAL;
        }
#else
        UNUSED_ARG2(pipe, len);
        return ENOTSUP;
#endif
}

//==============================================================================
/**
 * @brief Read data from pipe
 *
 * @param pipe          a pipe object
 * @param buf           a destination buffer
 * @param count         a count of bytes to read
 * @param rdcnt         a number of read bytes
 * @param non_blocking  a non-blocking access mode
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
int _pipe_read(pipe_t *pipe, u8_t *buf, size_t count, size_t *rdcnt, bool non_blocking)
{
#if __OS_ENABLE_MKFIFO__ == _YES_
        if (is_valid(pipe) && buf && count) {

                size_t n = 0;
                for (; n < count; n++) {

                        size_t noitm = 0;
                        _queue_get_number_of_items(pipe->queue, &noitm);

                        if ((pipe->flag & CLOSED) && noitm == 0) {
                                u8_t null = '\0';
                                _queue_send(pipe->queue, &null, PIPE_WRITE_TIMEOUT);
                                break;
                        }

                        u32_t tout = non_blocking || n ? 10 : PIPE_READ_TIMEOUT;
                        if (_queue_receive(pipe->queue, &buf[n], tout) != ESUCC) {
                                break;
                        }
                }

                *rdcnt = n;
                return ESUCC;
        } else {
                return EINVAL;
        }
#else
        UNUSED_ARG5(pipe, buf, count, rdcnt, non_blocking);
        return ENOTSUP;
#endif
}

//==============================================================================
/**
 * @brief Read data from pipe
 *
 * @param pipe          a pipe object
 * @param buf           a destination buffer
 * @param count         a count of bytes to read
 * @param wrcnt         a number of written bytes
 * @param non_blocking  a non-blocking access mode
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
int _pipe_write(pipe_t *pipe, const u8_t *buf, size_t count, size_t *wrcnt, bool non_blocking)
{
#if __OS_ENABLE_MKFIFO__ == _YES_
        if (is_valid(pipe) && buf && count) {

                size_t n = 0;
                for (; n < count; n++) {

                        size_t noitm = 0;
                        _queue_get_number_of_items(pipe->queue, &noitm);

                        if ((pipe->flag & CLOSED) && noitm == 0) {
                                break;
                        }

                        u32_t tout = non_blocking ? 10 : PIPE_WRITE_TIMEOUT;
                        if (_queue_send(pipe->queue, &buf[n], tout) != ESUCC) {
                                break;
                        }
                }

                *wrcnt = n;
                return ESUCC;
        } else {
                return EINVAL;
        }
#else
        UNUSED_ARG5(pipe, buf, count, wrcnt, non_blocking);
        return ENOTSUP;
#endif
}

//==============================================================================
/**
 * @brief Close pipe
 *
 * @param pipe          a pipe object
 *
 * @return One of errno value.
 */
//==============================================================================
int _pipe_close(pipe_t *pipe)
{
#if __OS_ENABLE_MKFIFO__ == _YES_
        if (is_valid(pipe)) {

                if (not (pipe->flag & PERMANENT)) {
                        pipe->flag |= CLOSED;

                        u8_t nul = '\0';
                        _queue_send(pipe->queue, &nul, 10);
                }

                return ESUCC;
        } else {
                return EINVAL;
        }
#else
        UNUSED_ARG1(pipe);
        return ENOTSUP;
#endif
}

//==============================================================================
/**
 * @brief  Clear pipe
 *
 * @param  pipe         a pipe object
 *
 * @return One of errno value.
 */
//==============================================================================
int _pipe_clear(pipe_t *pipe)
{
#if __OS_ENABLE_MKFIFO__ == _YES_
        if (is_valid(pipe)) {
                return _queue_reset(pipe->queue);
        } else {
                return EINVAL;
        }
#else
        UNUSED_ARG1(pipe);
        return ENOTSUP;
#endif
}

//==============================================================================
/**
 * @brief  Permanent pipe. FIFO is not closed at file close.
 *
 * @param  pipe         a pipe object
 *
 * @return One of errno value.
 */
//==============================================================================
int _pipe_permanent(pipe_t *pipe)
{
#if __OS_ENABLE_MKFIFO__ == _YES_
        if (is_valid(pipe)) {
                pipe->flag |= PERMANENT;
                return ESUCC;
        } else {
                return EINVAL;
        }
#else
        UNUSED_ARG1(pipe);
        return ENOTSUP;
#endif
}

/*==============================================================================
  End of file
==============================================================================*/
