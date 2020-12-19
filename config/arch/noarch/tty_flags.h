/*=========================================================================*//**
@file    tty_flags.h

@author  Daniel Zorychta

@brief   TTY module configuration flags.

@note    Copyright (C) 2014 Daniel Zorychta <daniel.zorychta@gmail.com>

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

/*
 * NOTE: All flags defined as: __FLAG_NAME__ (with doubled underscore as suffix
 *       and prefix) are exported to the single configuration file
 *       (by using Configtool) when entire project configuration is exported.
 *       All other flag definitions and statements are ignored.
 */

#ifndef _TTY_FLAGS_H_
#define _TTY_FLAGS_H_

/*--
this:SetLayout("TitledGridBack", 2, "Home > Microcontroller > TTY",
               function() this:LoadFile("arch/arch_flags.h") end)
++*/

/*--
this:AddWidget("Spinbox", 20, 200, "Number of terminal columns")
--*/
#define __TTY_TERM_COLS__ 80

/*--
this:AddWidget("Spinbox", 1, 80, "Number of terminal rows")
--*/
#define __TTY_TERM_ROWS__ 24

/*--
this:AddWidget("Spinbox", 20, 200, "Output stream length (prompt length)")
--*/
#define __TTY_OUT_STREAM_LEN__ 80

/*--
this:AddWidget("Spinbox", 1, 12, "Number of terminals")
--*/
#define __TTY_NUMBER_OF_TERM__ 2

#endif /* _TTY_FLAGS_H_ */
/*==============================================================================
  End of file
==============================================================================*/
