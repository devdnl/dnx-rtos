/*=========================================================================*//**
@file    printk.c

@author  Daniel Zorychta

@brief   Kernel print support

@note    Copyright (C) 2015 Daniel Zorychta <daniel.zorychta@gmail.com>

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
        struct msg {
                struct timeval timestamp;
                char  str[__OS_SYSTEM_MSG_COLS__];
        } msg[__OS_SYSTEM_MSG_ROWS__];

        uint16_t head;
        uint16_t count;
} printk_log_t;

/*==============================================================================
  Local function prototypes
==============================================================================*/

/*==============================================================================
  Local objects
==============================================================================*/
static printk_log_t logbuf;

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
                u16_t prev_head = logbuf.head;

                if (logbuf.count < __OS_SYSTEM_MSG_ROWS__) {
                        logbuf.count++;
                }

                if (++logbuf.head >= __OS_SYSTEM_MSG_ROWS__) {
                        logbuf.head = 0;
                }

                va_list args;
                va_start(args, format);
                int len = _vsnprintf(logbuf.msg[logbuf.head].str,
                                     __OS_SYSTEM_MSG_COLS__, format, args);
                va_end(args);

                if (logbuf.msg[logbuf.head].str[len - 1] == '\n') {
                        logbuf.msg[logbuf.head].str[len - 1] = '\0';
                }

                u64_t now = _kernel_get_time_ms();
                logbuf.msg[logbuf.head].timestamp.tv_sec  = now / 1000;
                logbuf.msg[logbuf.head].timestamp.tv_usec = (now % 1000) * 1000;

                if (logbuf.msg[logbuf.head].timestamp.tv_sec == logbuf.msg[prev_head].timestamp.tv_sec) {
                        if (logbuf.msg[prev_head].timestamp.tv_usec >= logbuf.msg[logbuf.head].timestamp.tv_usec) {
                                logbuf.msg[logbuf.head].timestamp.tv_usec = logbuf.msg[prev_head].timestamp.tv_usec + 1;
                        }
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
 * @param from_time     log search time starting from system start
 * @param msg_time      current message time from system start
 *
 * @return Number of bytes copied to the buffer. 0 if message is empty.
 */
//==============================================================================
size_t _printk_read(char *str, size_t len, const struct timeval *from_time, struct timeval *msg_time)
{
        size_t n = 0;

        if (str && len && from_time && msg_time) {
                _kernel_scheduler_lock();
                {
                        if (logbuf.count > 0) {

                                i32_t idx = logbuf.head - logbuf.count + 1;
                                      idx = idx < 0
                                            ? cast(i32_t, ARRAY_SIZE(logbuf.msg)) + idx
                                            : idx;

                                size_t msgs = logbuf.count;

                                while (msgs--) {

                                        struct msg *m = &logbuf.msg[idx];

                                        if (  (m->timestamp.tv_sec > from_time->tv_sec)
                                           || (  (m->timestamp.tv_sec == from_time->tv_sec)
                                              && (m->timestamp.tv_usec > from_time->tv_usec)) ) {

                                                strlcpy(str, logbuf.msg[idx].str, len);

                                                n = min(len, strnlen(logbuf.msg[idx].str,
                                                                     __OS_SYSTEM_MSG_COLS__));

                                                *msg_time = m->timestamp;

                                                break;

                                        } else {
                                                if (++idx >= cast(i32_t, ARRAY_SIZE(logbuf.msg))) {
                                                        idx = 0;
                                                }
                                        }
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
                memset(&logbuf, 0, sizeof(logbuf));
        }
        _kernel_scheduler_unlock();
}

#endif

/*==============================================================================
  End of file
==============================================================================*/
