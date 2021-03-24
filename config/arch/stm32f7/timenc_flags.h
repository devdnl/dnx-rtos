/*==============================================================================
File    timenc_flags.h

Author  Daniel Zorychta

Brief   Timer driven AB encoder

        Copyright (C) 2021 Daniel Zorychta <daniel.zorychta@gmail.com>

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

==============================================================================*/

/*
 * NOTE: All flags defined as: __FLAG_NAME__ (with doubled underscore as prefix
 *       and suffix) are exported to the single configuration file
 *       (by using Configtool) when entire project configuration is exported.
 *       All other flag definitions and statements are ignored.
 */

#ifndef _TIMENC_FLAGS_H_
#define _TIMENC_FLAGS_H_

/*--
this:SetLayout("TitledGridBack", 2, "Home > Microcontroller > TIMENC",
               function() this:LoadFile("arch/arch_flags.h") end)
++*/

/*--
this:AddWidget("Spinbox", 100, 100000, "Sample frequency [Hz]")
--*/
#define __TIMENC_FREQ_HZ__ 500

/*--
this:AddWidget("Spinbox", 1, 256, "Maximal number of encoders")
--*/
#define __TIMENC_MAX_ENCODERS__ 8

#endif /* _TIMENC_FLAGS_H_ */
/*==============================================================================
  End of file
==============================================================================*/
