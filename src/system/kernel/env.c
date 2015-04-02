/*=========================================================================*//**
@file    env.c

@author  Daniel Zorychta

@brief   dnx RTOS environment functions

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
#include "kernel/env.h"
#include "config.h"

// TODO if callback will be applied then this file is not necessary

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
 * @return The current calendar time as a time_t object.
 *         If the argument is not a null pointer, the return value is the same
 *         as the one stored in the location pointed by argument timer.
 *         If the function could not retrieve the calendar time, it returns
 *         a value of -1.
 */
//==============================================================================
time_t _time(time_t *timer)
{
//        time_t t   = -1; // TODO _time function not completed (syscall time)
//        FILE  *rtc = _vfs_fopen(CONFIG_RTC_FILE_PATH, "r");
//        if (rtc) {
//                if (_vfs_fread(&t, sizeof(time_t), 1, rtc) == 1) {
//                        if (timer) {
//                                *timer = t;
//                        }
//                }
//                _vfs_fclose(rtc);
//        }
//
//        return t;
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
 * @return On success 0 is returned.
 *         On error -1 is returned.
 */
//==============================================================================
int _stime(time_t *timer)
{
        int    ret = -1;
//        FILE  *rtc = _vfs_fopen(CONFIG_RTC_FILE_PATH, "w"); // TODO stime() syscall time
//        if (rtc) {
//                ret = _vfs_fwrite(timer, sizeof(time_t), 1, rtc) == 1 ? 0 : -1;
//                _vfs_fclose(rtc);
//        }

        return ret;
}

/*==============================================================================
  End of file
==============================================================================*/
