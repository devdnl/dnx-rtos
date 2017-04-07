/*=========================================================================*//**
@file    time.c

@author  Daniel Zorychta

@brief   dnx RTOS environment functions

@note    Copyright (C) 2015 Daniel Zorychta <daniel.zorychta@gmail.com>

         This program is free software; you can redistribute it and/or modify
         it under the terms of the GNU General Public License as published by
         the Free Software Foundation and modified by the dnx RTOS exception.

         NOTE: The modification  to the GPL is  included to allow you to
               distribute a combined work that includes FreeRTOS without
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
 * @brief  Get current time
 *
 * The function returns this value, and if the argument is not a null pointer,
 * it also sets this value to the object pointed by timer.
 * The value returned generally represents the number of seconds since 00:00
 * hours, Jan 1, 1970 UTC (i.e., the current unix timestamp). Although libraries
 * may use a different representation of time: Portable programs should not use
 * the value returned by this function directly, but always rely on calls to
 * other elements of the standard library to translate them to portable types
 * (such as localtime, gmtime or difftime).
 *
 * @param  timer        Pointer to an object of type time_t, where the time
 *                      value is stored.
 *                      Alternatively, this parameter can be a null pointer,
 *                      in which case the parameter is not used (the function
 *                      still returns a value of type time_t with the result).
 *
 * @return One of errno value.
 */
//==============================================================================
int _gettime(time_t *timer)
{
        static uint32_t time_ref  = 0;
        static time_t   timecache = 0;

        int err = EINVAL;

        if (timer) {
                if ((time_ref == 0) || sys_time_is_expired(time_ref, 500)) {
                        FILE *rtc;

                        struct vfs_path cpath;
                        cpath.CWD  = NULL;
                        cpath.PATH = __OS_RTC_FILE_PATH__;

                        err = _vfs_fopen(&cpath, "r", &rtc);
                        if (err == ESUCC) {
                                size_t rdcnt;
                                err = _vfs_fread(timer, sizeof(time_t), &rdcnt, rtc);
                                timecache = *timer;
                                _vfs_fclose(rtc, false);

                                time_ref = sys_time_get_reference();
                        }
                } else {
                        *timer = timecache;
                        err    = ESUCC;
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
        int result = EINVAL;

        if (timer) {
                FILE *rtc;

                struct vfs_path cpath;
                cpath.CWD  = NULL;
                cpath.PATH = __OS_RTC_FILE_PATH__;

                result = _vfs_fopen(&cpath, "w", &rtc);
                if (result == ESUCC) {
                        size_t wrcnt;
                        result = _vfs_fwrite(timer, sizeof(time_t), &wrcnt, rtc);
                        _vfs_fclose(rtc, false);
                }
        }

        return result;
}
#endif

/*==============================================================================
  End of file
==============================================================================*/
