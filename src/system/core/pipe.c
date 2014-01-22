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
#define PIPE_READ_TIMEOUT               MAX_DELAY
#define PIPE_WRITE_TIMEOUT              MAX_DELAY
#define PIPE_VALIDATION_NUMBER          (u32_t)0x2B1D0852

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
                pipe->valid  = PIPE_VALIDATION_NUMBER;
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
                if (pipe->valid == PIPE_VALIDATION_NUMBER) {
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
                if (pipe->valid == PIPE_VALIDATION_NUMBER) {
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
 *
 * @return number of read bytes, -1 if error
 */
//==============================================================================
int pipe_read(pipe_t *pipe, u8_t *buf, size_t count)
{
        if (pipe && buf && count) {
                if (pipe->valid == PIPE_VALIDATION_NUMBER) {
                        int n = 0;
                        for (; n < (int)count && !pipe->closed; n++) {

                                if (!queue_receive(pipe, &buf[n], PIPE_READ_TIMEOUT)) {
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
 *
 * @return number of read bytes, -1 if error
 */
//==============================================================================
int pipe_write(pipe_t *pipe, const u8_t *buf, size_t count)
{
        if (pipe && buf && count) {
                if (pipe->valid == PIPE_VALIDATION_NUMBER) {
                        int n = 0;
                        for (; n < (int)count && !pipe->closed; n++) {

                                if (!queue_send(pipe, &buf[n], PIPE_WRITE_TIMEOUT)) {
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
                if (pipe->valid == PIPE_VALIDATION_NUMBER) {
                        pipe->closed = true;

                        u8_t null = '\0';
                        if (queue_send(pipe, &null, PIPE_WRITE_TIMEOUT)) {
                                return true;
                        }
                }
        }

        return false;
}

/*==============================================================================
  End of file
==============================================================================*/
