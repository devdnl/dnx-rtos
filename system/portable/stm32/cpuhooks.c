/*=========================================================================*//**
@file    cpuhooks.c

@author  Daniel Zorychta

@brief   This file support all CPU hooks

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
#include "cpuhooks.h"
#include "io.h"
#include "oswrap.h"

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

/*==============================================================================
  Exported object definitions
==============================================================================*/

/*==============================================================================
 Function definitions
==============================================================================*/

//==============================================================================
/**
 * @brief Hard Fault ISR
 */
//==============================================================================
void HardFault_Handler(void)
{
        ch_t *name = TaskGetName(THIS_TASK);
        kprint(FONT_COLOR_RED"Task %s generated Hard Fault!"RESET_ATTRIBUTES"\n", name);
        TaskDelete(TaskGetCurrentTaskHandle());
}

//==============================================================================
/**
 * @brief Memory Management failure ISR
 */
//==============================================================================
void MemManage_Handler(void)
{
        while (TRUE);
}

//==============================================================================
/**
 * @brief Bus Fault ISR
 */
//==============================================================================
void BusFault_Handler(void)
{
        while (TRUE);
}

//==============================================================================
/**
 * @brief Usage Fault ISR
 */
//==============================================================================
void UsageFault_Handler(void)
{
        while (TRUE);
}

/*==============================================================================
  End of file
==============================================================================*/
