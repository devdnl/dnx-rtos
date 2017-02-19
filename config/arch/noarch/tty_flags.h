/*=========================================================================*//**
@file    tty_flags.h

@author  Daniel Zorychta

@brief   TTY module configuration flags.

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

/*--
this:AddWidget("Editline", true, "Terminal input file")
--*/
#define __TTY_TERM_IN_FILE__ "/dev/ttyS0"

/*--
this:AddWidget("Editline", true, "Terminal output file")
--*/
#define __TTY_TERM_OUT_FILE__ "/dev/ttyS0"

#endif /* _TTY_FLAGS_H_ */
/*==============================================================================
  End of file
==============================================================================*/
