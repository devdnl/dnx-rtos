/*=========================================================================*//**
@file    unistd.h

@author  Daniel Zorychta

@brief   Unix standard library.

@note    Copyright (C) 2013 Daniel Zorychta <daniel.zorychta@gmail.com>

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

#ifndef _UNISTD_H_
#define _UNISTD_H_

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Include files
==============================================================================*/
#include "kernel/kwrapper.h"

/*==============================================================================
  Exported macros
==============================================================================*/

/*==============================================================================
  Exported object types
==============================================================================*/

/*==============================================================================
  Exported objects
==============================================================================*/

/*==============================================================================
  Exported functions
==============================================================================*/

/*==============================================================================
  Exported inline functions
==============================================================================*/
//==============================================================================
/**
 * @brief Suspend task for defined time in seconds
 *
 * @param[in] seconds
 */
//==============================================================================
static inline void sleep(const uint seconds)
{
        _sleep(seconds);
}

//==============================================================================
/**
 * @brief Suspend task for defined time in milliseconds
 *
 * @param[in] milliseconds
 */
//==============================================================================
static inline void sleep_ms(const uint milliseconds)
{
        _sleep_ms(milliseconds);
}

//==============================================================================
/**
 * @brief Function sleep task in regular periods (reference argument)
 *
 * @param milliseconds          milliseconds
 * @param ref_time_ticks        reference time in OS ticks
 */
//==============================================================================
static inline void sleep_until_ms(const uint milliseconds, int *ref_time_ticks)
{
        _sleep_until_ms(milliseconds, ref_time_ticks);
}

//==============================================================================
/**
 * @brief Function sleep task in regular periods (reference argument)
 *
 * @param seconds       seconds
 * @param ref_time_ticks        reference time in OS ticks
 */
//==============================================================================
static inline void sleep_until(const uint seconds, int *ref_time_ticks)
{
        _sleep_until(seconds, ref_time_ticks);
}

//==============================================================================
/**
 * @brief Function return x name
 *
 * @param[out] *buf     output buffer
 * @param[in]   size    buffer size
 *
 * @return buf pointer on success, otherwise NULL pointer
 */
//==============================================================================
static inline char *getcwd(char *buf, size_t size)
{
        return strncpy(buf, _task_get_data()->f_cwd, size);
}

#ifdef __cplusplus
}
#endif

#endif /* _UNISTD_H_ */
/*==============================================================================
  End of file
==============================================================================*/
