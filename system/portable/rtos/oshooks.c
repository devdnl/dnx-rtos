/*=========================================================================*//**
@file    oshooks.c

@author  Daniel Zorychta

@brief   This file support all operating system hooks

@note    Copyright (C) 2012 Daniel Zorychta <daniel.zorychta@gmail.com>

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
#include "oshooks.h"
#include "io.h"

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Local symbolic constants/macros
==============================================================================*/

/*==============================================================================
  Local types, enums definitions
==============================================================================*/

/*==============================================================================
  Local function prototypes
==============================================================================*/

/*==============================================================================
  Local object definitions
==============================================================================*/
/** uptime counter */
u32_t uptimeCnt;
u32_t uptimeDiv;

/*==============================================================================
  Exported object definitions
==============================================================================*/

/*==============================================================================
  Function definitions
==============================================================================*/

//==============================================================================
/**
 * @brief Application Idle hook (idle task - don't use loop)
 */
//==============================================================================
void vApplicationIdleHook(void)
{
}

//==============================================================================
/**
 * @brief Stack overflow hook
 */
//==============================================================================
void vApplicationStackOverflowHook(task_t taskHdl, signed char *taskName)
{
        TaskDelete(taskHdl);
        kprint(FONT_COLOR_RED"Task %s stack overflow!"RESET_ATTRIBUTES"\n", taskName);
}

//==============================================================================
/**
 * @brief Hook when system tick was increased
 */
//==============================================================================
void vApplicationTickHook(void)
{
        if (++uptimeDiv >= configTICK_RATE_HZ) {
                uptimeDiv = 0;
                uptimeCnt++;
        }
}

//==============================================================================
/**
 * @brief Function return uptime counter
 *
 * @return uptime counter
 */
//==============================================================================
u32_t GetUptimeCnt(void)
{
        return uptimeCnt;
}

#ifdef __cplusplus
}
#endif

/*==============================================================================
  End of file
==============================================================================*/
