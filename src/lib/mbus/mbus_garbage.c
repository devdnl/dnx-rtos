/*=========================================================================*//**
 @file    mbus_garbage.c

 @author  Daniel Zorychta

 @brief   Message Bus Library. Garbage class.

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
#include "mbus_garbage.h"
#include <stdlib.h>
#include <dnx/misc.h>
#include <dnx/timer.h>

/*==============================================================================
 Local macros
 ==============================================================================*/
#define GARBAGE_LIVE_TIME       2000

/*==============================================================================
 Local object types
 ==============================================================================*/
struct _mbus_garbage {
        void            *data;
        void            *self;
        timer_t          timer;
};

/*==============================================================================
 Local function prototypes
 ==============================================================================*/

/*==============================================================================
 Local objects
 ==============================================================================*/

/*==============================================================================
 Shared objects
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
 * @brief  Function check if signal object is valid
 * @param  this                 signal object
 * @return If object is valid then true is returned, otherwise false.
 */
//==============================================================================
static bool garbage_is_valid(_mbus_garbage_t *this)
{
        return this && this->self == this && this->data;
}

//==============================================================================
/**
 * @brief  Create new garbage object
 * @param  data                 garbage data to link
 * @return Garbage object or NULL on error.
 */
//==============================================================================
_mbus_garbage_t *_mbus_garbage_new(void *data)
{
        _mbus_garbage_t *this = malloc(sizeof(_mbus_garbage_t));
        if (this) {
                this->data  = data;
                this->timer = timer_reset();
                this->self  = this;
        }

        return this;
}

//==============================================================================
/**
 * @brief  Remove garbage object and linked data
 * @param  this                 garbage object
 * @return None
 */
//==============================================================================
void _mbus_garbage_delete(_mbus_garbage_t *this)
{
        if (garbage_is_valid(this)) {
                if (this->data)
                        free(this->data);

                this->self = NULL;

                free(this);
        }
}

//==============================================================================
/**
 * @brief  Check if garbage time expired
 * @param  this                 garbage object
 * @return True if time expired, otherwise false.
 */
//==============================================================================
bool _mbus_garbage_is_time_expired(_mbus_garbage_t *this)
{
        if (garbage_is_valid(this)) {
                return timer_is_expired(this->timer, GARBAGE_LIVE_TIME);
        } else {
                return false;
        }
}

/*==============================================================================
 End of file
 ==============================================================================*/
