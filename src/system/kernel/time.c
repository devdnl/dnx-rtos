/*=========================================================================*//**
@file    time.c

@author  Daniel Zorychta

@brief   dnx RTOS time functions

@note    Copyright (C) 2018 Daniel Zorychta <daniel.zorychta@gmail.com>

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
#include "kernel/time.h"
#include "kernel/errno.h"
#include "kernel/sysfunc.h"
#include "fs/vfs.h"

/*==============================================================================
  Local macros
==============================================================================*/

/*==============================================================================
  Local object types
==============================================================================*/

/*==============================================================================
  Local function prototypes
==============================================================================*/

/*==============================================================================
  Local objects
==============================================================================*/
#if __OS_ENABLE_TIMEMAN__ == _YES_
static FILE  *RTC;
static time_t last_sec;
static u32_t  last_msec;
static u32_t  usec;
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
#if __OS_ENABLE_TIMEMAN__ == _YES_
//==============================================================================
/**
 * @brief  Function open RTC file.
 */
//==============================================================================
static int open_RTC(void)
{
        int err = 0;

        if (RTC == NULL) {
                struct vfs_path cpath;
                cpath.CWD  = NULL;
                cpath.PATH = __OS_RTC_FILE_PATH__;

                err = _vfs_fopen(&cpath, "r+", &RTC);
        }

        return err;
}

//==============================================================================
/**
 * @brief  Function simulate microseconds
 *
 * @param  timeval      time structure
 *
 * @return Calculated value of microseconds.
 */
//==============================================================================
static void simulate_usec(struct timeval *timeval)
{
        u32_t ms_now = sys_get_uptime_ms();

        if (last_sec != timeval->tv_sec) {
                usec = 0;
        } else {
                if (usec < 998500) {
                        usec += (ms_now - last_msec) * 1000;
                        usec  = min(usec, 998500);
                } else {
                        usec++;
                }
        }

        timeval->tv_usec = usec;
        last_msec = ms_now;
        last_sec  = timeval->tv_sec;
}

//==============================================================================
/**
 * @brief  Get current time
 *
 * The function can get the time.
 *
 * @param  timeval      Pointer to an object of type struct timeval, where the
 *                      time value is stored.
 *
 * @return One of errno value.
 */
//==============================================================================
int _gettime(struct timeval *timeval)
{
        int err = EINVAL;

        if (timeval) {
                if (RTC == NULL) {
                        err = open_RTC();
                }

                if (RTC) {
                        _vfs_fseek(RTC, 0, VFS_SEEK_SET);

                        size_t rdcnt;
                        err = _vfs_fread(&timeval->tv_sec, sizeof(time_t), &rdcnt, RTC);

                        if (!err) {
                                simulate_usec(timeval);
                        }
                }
        }

        return err;
}

//==============================================================================
/**
 * @brief  Set system's time
 *
 * The function sets the system's idea of the time and date. The time, pointed to by
 * timer, is measured in seconds since the Epoch, 1970-01-01 00:00:00 +0000 (UTC).
 *
 * @param  timer        pointer to an object of type time_t, where the time
 *                      value is stored.
 *
 * @return One of errno value.
 */
//==============================================================================
int _settime(time_t *timer)
{
        int err = EINVAL;

        if (timer) {
                if (RTC == NULL) {
                        err = open_RTC();
                }

                if (RTC) {
                        _vfs_fseek(RTC, 0, VFS_SEEK_SET);

                        size_t wrcnt;
                        err = _vfs_fwrite(timer, sizeof(time_t), &wrcnt, RTC);
                }
        }

        return err;
}
#endif

/*==============================================================================
  End of file
==============================================================================*/
