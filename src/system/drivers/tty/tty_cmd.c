/*=========================================================================*//**
@file    tty_cmd.c

@author  Daniel Zorychta

@brief   Code in this file is responsible for decode VT100 commands.

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

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Include files
==============================================================================*/
#include "system/dnxmodule.h"
#include "tty.h"
#include "tty_cfg.h"
#include "system/timer.h"

/*==============================================================================
  Local macros
==============================================================================*/
#define TTYCMD_VALIDATION                       (u32_t)0x7D8498F1

/*==============================================================================
  Local object types
==============================================================================*/
struct ttycmd {
        u32_t   valid;
        bool    analyzing;
        u8_t    analyze_step;
        timer_t timer;
};

/*==============================================================================
  Local function prototypes
==============================================================================*/

/*==============================================================================
  Local objects
==============================================================================*/
MODULE_NAME("TTY");

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
 * @brief Initialize command object
 *
 * @return pointer to object if success, NULL on error
 */
//==============================================================================
ttycmd_t *ttycmd_new()
{
        ttycmd_t *ttycmd = calloc(1, sizeof(ttycmd_t));
        if (ttycmd) {
                ttycmd->valid = TTYCMD_VALIDATION;
        }

        return ttycmd;
}

//==============================================================================
/**
 * @brief Delete command object
 *
 * @param ttycmd        command analyze object
 */
//==============================================================================
void ttycmd_delete(ttycmd_t *ttycmd)
{
        if (ttycmd) {
                if (ttycmd->valid == TTYCMD_VALIDATION) {
                        ttycmd->valid = 0;
                        free(ttycmd);
                }
        }
}

//==============================================================================
/**
 * @brief Function analyze input stream
 *
 * @param ttycmd        command analyze object
 * @param c             input character
 *
 * @return analyzed command
 */
//==============================================================================
ttycmd_resp_t ttycmd_analyze(ttycmd_t *ttycmd, const char c)
{

}

//==============================================================================
/**
 * @brief Check if command decoder is in idle state
 *
 * @param ttycmd        command analyze object
 *
 * @return true if idle, false if operation in progress
 */
//==============================================================================
bool ttycmd_is_idle(ttycmd_t *ttycmd)
{

}

#ifdef __cplusplus
}
#endif

/*==============================================================================
  End of file
==============================================================================*/
