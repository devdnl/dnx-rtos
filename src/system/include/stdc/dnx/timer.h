/*=========================================================================*//**
@file    timer.h

@author  Daniel Zorychta

@brief   Software timer library.

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

#ifndef _TIMER_H_
#define _TIMER_H_

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
typedef int timer_t;

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
 * @brief timer_t timer_reset(void)
 * The function <b>timer_reset</b>() resets timer. Function synchronize selected
 * timer to system time.
 *
 * @param None
 *
 * @errors None
 *
 * @return Synchronized timer object.
 *
 * @example
 * // ...
 *
 * timer_t timer = timer_reset();
 *
 * while (timer_is_not_expired(timer, 100)) {
 *         // try to do operations by 100ms
 *         // ...
 * }
 *
 * // ...
 */
//==============================================================================
static inline timer_t timer_reset(void)
{
        return _kernel_get_time_ms();
}

//==============================================================================
/**
 * @brief bool timer_is_expired(timer_t timer, int time)
 * The function <b>timer_is_expired</b>() check if time <i>time</i> of <i>timer</i>
 * was expired.
 *
 * @param timer         timer
 * @param time          time to check
 *
 * @errors None
 *
 * @return If time expired then <b>true</b> is returned, otherwise <b>false</b>.
 *
 * @example
 * // ...
 *
 * timer_t timer = timer_reset();
 *
 * while (!timer_is_expired(timer, 100)) {
 *         // try to do operations by 100ms
 *         // ...
 * }
 *
 * // ...
 */
//==============================================================================
static inline bool timer_is_expired(timer_t timer, int time)
{
        return (_kernel_get_time_ms() - timer >= time);
}

//==============================================================================
/**
 * @brief bool timer_is_not_expired(timer_t timer, int time)
 * The function <b>timer_is_not_expired</b>() check if time <i>time</i> of <i>timer</i>
 * was not expired.
 *
 * @param timer         timer
 * @param time          time to check
 *
 * @errors None
 *
 * @return If time not expired then <b>true</b> is returned, otherwise <b>false</b>.
 *
 * @example
 * // ...
 *
 * timer_t timer = timer_reset();
 *
 * while (timer_is_not_expired(timer, 100)) {
 *         // try to do operations by 100ms
 *         // ...
 * }
 *
 * // ...
 */
//==============================================================================
static inline bool timer_is_not_expired(timer_t timer, int time)
{
        return (_kernel_get_time_ms() - timer < time);
}

//==============================================================================
/**
 * @brief timer_t timer_set_expired(void)
 * The function <b>timer_set_expired</b>() set timer to expired value.
 *
 * @param None
 *
 * @errors None
 *
 * @return Timer object with expired value.
 *
 * @example
 * // ...
 *
 * timer_t timer = timer_set_expired();
 *
 * while (true) {
 *         // ...
 *
 *         if (timer_is_expired(timer, 10)) {
 *                 // refresh screen
 *
 *                 timer = timer_reset();
 *         }
 * }
 *
 * // ...
 */
//==============================================================================
static inline timer_t timer_set_expired(void)
{
        return 0;
}

//==============================================================================
/**
 * @brief int timer_difftime(timer_t timer1, timer_t timer2)
 * The function <b>timer_difftime</b>() calculate difference between <i>timer1</i>
 * and <i>timer2</i>.
 *
 * @param timer1        timer 1
 * @param timer2        timer 2
 *
 * @errors None
 *
 * @return Returns difference between <i>timer1</i> and <i>timer2</i>.
 *
 * @example
 * // ...
 *
 * timer_t timer1 = timer_reset();
 * timer_t timer2 = timer_reset();
 *
 * // ...
 *
 * if (timer_difftime(timer1, timer2) == 100) {
 *         // ...
 * } else {
 *         // ...
 * }
 *
 * // ...
 */
//==============================================================================
static inline int timer_difftime(timer_t timer1, timer_t timer2)
{
        return timer1 - timer2;
}

#ifdef __cplusplus
}
#endif

#endif /* _TIMER_H_ */
/*==============================================================================
  End of file
==============================================================================*/
