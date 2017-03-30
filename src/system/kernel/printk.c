/*=========================================================================*//**
@file    printk.c

@author  Daniel Zorychta

@brief   Kernel print support

@note    Copyright (C) 2015 Daniel Zorychta <daniel.zorychta@gmail.com>

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
#include <stddef.h>
#include "kernel/kwrapper.h"
#include "kernel/printk.h"
#include "config.h"
#include "fs/vfs.h"
#include "mm/mm.h"
#include "dnx/misc.h"
#include "lib/vsnprintf.h"
#include "libc/errno.h"

#if ((__OS_SYSTEM_MSG_ENABLE__ > 0) && (__OS_PRINTF_ENABLE__ > 0))

/*==============================================================================
  Local macros
==============================================================================*/

/*==============================================================================
  Local object types
==============================================================================*/
typedef struct {
        struct {
                u32_t timestamp;
                char  str[__OS_SYSTEM_MSG_COLS__];
        } msg[__OS_SYSTEM_MSG_ROWS__];

        uint8_t wridx;
        uint8_t rdidx;
        uint8_t size;
} printk_log_t;

/*==============================================================================
  Local function prototypes
==============================================================================*/

/*==============================================================================
  Local objects
==============================================================================*/
static printk_log_t log_buf;

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
 * @brief Function send kernel message on terminal
 *
 * @param *format             formated text
 * @param ...                 format arguments
 */
//==============================================================================
void _printk(const char *format, ...)
{
        _kernel_scheduler_lock();
        {
                if (log_buf.size >= __OS_SYSTEM_MSG_ROWS__) {
                        log_buf.size = __OS_SYSTEM_MSG_ROWS__;
                        if (++log_buf.rdidx >= __OS_SYSTEM_MSG_ROWS__) {
                                log_buf.rdidx = 0;
                        }
                } else {
                        log_buf.size++;
                }

                va_list args;
                va_start(args, format);
                _vsnprintf(log_buf.msg[log_buf.wridx].str,
                           __OS_SYSTEM_MSG_COLS__, format, args);
                va_end(args);

                log_buf.msg[log_buf.wridx].timestamp = _kernel_get_time_ms();

                if (++log_buf.wridx >= __OS_SYSTEM_MSG_ROWS__) {
                        log_buf.wridx = 0;
                }
        }
        _kernel_scheduler_unlock();
}

//==============================================================================
/**
 * Function read log message.
 *
 * @param str           destination buffer
 * @param len           destination buffer length
 * @param timestamp_ms  message timestamp in milliseconds
 *
 * @return Number of bytes copied to the buffer. 0 if message is empty.
 */
//==============================================================================
size_t _printk_read(char *str, size_t len, u32_t *timestamp_ms)
{
        size_t n = 0;

        if (str && len) {
                _kernel_scheduler_lock();
                {
                        if (log_buf.size > 0) {
                                log_buf.size--;

                                strlcpy(str, log_buf.msg[log_buf.rdidx].str, len);

                                n = min(len, strnlen(log_buf.msg[log_buf.rdidx].str,
                                                     __OS_SYSTEM_MSG_COLS__));

                                if (timestamp_ms) {
                                        *timestamp_ms = log_buf.msg[log_buf.rdidx].timestamp;
                                }

                                if (++log_buf.rdidx >= __OS_SYSTEM_MSG_ROWS__) {
                                        log_buf.rdidx = 0;
                                }
                        }
                }
                _kernel_scheduler_unlock();
        }

        return n;
}

//==============================================================================
/**
 * Function clear system circular buffer.
 */
//==============================================================================
void _printk_clear(void)
{
        _kernel_scheduler_lock();
        {
                memset(&log_buf, 0, sizeof(log_buf));
        }
        _kernel_scheduler_unlock();
}

#endif

/*==============================================================================
  End of file
==============================================================================*/
