/*=========================================================================*//**
@file    localtime_r.c

@author  Daniel Zorychta

@brief

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
#include <time.h>
#include <config.h>
#include <lib/conv.h>

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
 * @brief  Convert time_t to tm as local time
 *
 * Uses the value pointed by timer to fill a tm structure with the values that
 * represent the corresponding time, expressed for the local timezone.
 *
 * @param  timer        Pointer to an object of type time_t that contains a time value.
 *                      time_t is an alias of a fundamental arithmetic type
 *                      capable of representing times as returned by function time.
 *
 * @return A pointer to a tm structure with its members filled with the values
 *         that correspond to the local time representation of timer.
 */
//==============================================================================
struct tm *localtime(const time_t *timer)
{
        return _builtinfunc(localtime_r, timer, &_tmbuf);
}

//==============================================================================
/**
 * @brief  Convert time_t to tm as local time
 *
 * Uses the value pointed by timer to fill a tm structure with the values that
 * represent the corresponding time, expressed for the local timezone.
 *
 * @param[in]  timer    Pointer to an object of type time_t that contains a time value.
 *                      time_t is an alias of a fundamental arithmetic type
 *                      capable of representing times as returned by function time.
 *
 * @param[out] tm       Pointer to an object of type struct tm that will contains
 *                      converted timer value to time structure.
 *
 * @return A pointer to a tm structure with its members filled with the values
 *         that correspond to the local time representation of timer.
 */
//==============================================================================
struct tm *localtime_r(const time_t *timer, struct tm *tm)
{
        return _builtinfunc(localtime_r, timer, tm);
}

/*==============================================================================
  End of file
==============================================================================*/
