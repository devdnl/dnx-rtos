/*=========================================================================*//**
@file    pipe.c

@author  Daniel Zorychta

@brief   File support creating of pipies in file systems.

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
#include "config.h"
#include <stdbool.h>
#include <sys/types.h>
#include <dnx/thread.h>
#include "core/pipe.h"
#include "core/sysmoni.h"

/*==============================================================================
  Local macros
==============================================================================*/

/*==============================================================================
  Local object types
==============================================================================*/
struct pipe {
        queue_t *queue;
        u32_t    valid;
        bool     closed;
};

/*==============================================================================
  Local function prototypes
==============================================================================*/

/*==============================================================================
  Local objects
==============================================================================*/
static const u32_t pipe_validation_number = 0x2B1D0852;
static const int   pipe_read_timeout      = MAX_DELAY_MS;
static const int   pipe_write_timeout     = MAX_DELAY_MS;

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
 * @brief Create pipe object
 *
 * @return pointer to pipe object
 */
//==============================================================================
pipe_t *pipe_new()
{
        pipe_t  *pipe  = sysm_sysmalloc(sizeof(pipe_t));
        queue_t *queue = queue_new(CONFIG_PIPE_LENGTH, sizeof(u8_t));

        if (pipe && queue) {

                pipe->queue  = queue;
                pipe->valid  = pipe_validation_number;
                pipe->closed = false;

        } else {
                if (queue) {
                        queue_delete(queue);
                }

                if (pipe) {
                        sysm_sysfree(pipe);
                        pipe = NULL;
                }
        }

        return pipe;
}

//==============================================================================
/**
 * @brief Destroy pipe object
 *
 * @param pipe          a pipe object
 */
//==============================================================================
void pipe_delete(pipe_t *pipe)
{
        if (pipe) {
                if (pipe->valid == pipe_validation_number) {
                        queue_delete(pipe->queue);
                }
        }
}

//==============================================================================
/**
 * @brief Return length of pipe
 *
 * @param pipe          a pipe object
 *
 * @return length or -1 if error
 */
//==============================================================================
int pipe_get_length(pipe_t *pipe)
{
        if (pipe) {
                if (pipe->valid == pipe_validation_number) {
                        return queue_get_number_of_items(pipe->queue);
                }
        }

        return -1;
}

//==============================================================================
/**
 * @brief Read data from pipe
 *
 * @param pipe          a pipe object
 * @param buf           a destination buffer
 * @param count         a count of bytes to read
 * @param non_blocking  a non-blocking access mode
 *
 * @return number of read bytes, -1 if error
 */
//==============================================================================
int pipe_read(pipe_t *pipe, u8_t *buf, size_t count, bool non_blocking)
{
        if (pipe && buf && count) {
                if (pipe->valid == pipe_validation_number) {
                        int n = 0;
                        for (; n < (int)count; n++) {

                                if (queue_get_number_of_items(pipe->queue) <= 0 && pipe->closed) {
                                        u8_t null = '\0';
                                        queue_send(pipe->queue, &null, pipe_write_timeout);
                                        break;
                                }

                                if (!queue_receive(pipe->queue, &buf[n], non_blocking ? 0 : pipe_read_timeout)) {
                                        break;
                                }
                        }

                        return n;
                }
        }

        return -1;
}

//==============================================================================
/**
 * @brief Write data to pipe
 *
 * @param pipe          a pipe object
 * @param buf           a source buffer
 * @param count         a count of bytes to write
 * @param non_blocking  a non-blocking access mode
 *
 * @return number of read bytes, -1 if error
 */
//==============================================================================
int pipe_write(pipe_t *pipe, const u8_t *buf, size_t count, bool non_blocking)
{
        if (pipe && buf && count) {
                if (pipe->valid == pipe_validation_number) {
                        int n = 0;
                        for (; n < (int)count; n++) {

                                if (queue_get_number_of_items(pipe->queue) <= 0 && pipe->closed) {
                                        break;
                                }

                                if (!queue_send(pipe->queue, &buf[n], non_blocking ? 0 : pipe_write_timeout)) {
                                        break;
                                }
                        }

                        return n;
                }
        }

        return -1;
}

//==============================================================================
/**
 * @brief Close pipe
 *
 * @param pipe          a pipe object
 *
 * @return true if pipe closed, otherwise false
 */
//==============================================================================
bool pipe_close(pipe_t *pipe)
{
        if (pipe) {
                if (pipe->valid == pipe_validation_number) {
                        pipe->closed = true;

                        u8_t null = '\0';
                        if (queue_send(pipe->queue, &null, pipe_write_timeout)) {
                                return true;
                        }
                }
        }

        return false;
}

/*==============================================================================
  End of file
==============================================================================*/
