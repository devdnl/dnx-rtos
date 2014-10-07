/*=========================================================================*//**
@file    wdg_flags.h

@author  Daniel Zorychta

@brief   WDG module configuration flags.

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

#ifndef _WDG_FLAGS_H_
#define _WDG_FLAGS_H_

#define __WDG_DEVICE_LOCK_AT_OPEN__ _YES_
#define __WDG_CLK_DIVIDER__ 256
#define __WDG_RELOAD_VALUE__ 1563
#define __WDG_DISABLE_ON_DEBUG__ _YES_

#endif /* _WDG_FLAGS_H_ */
/*==============================================================================
  End of file
==============================================================================*/
