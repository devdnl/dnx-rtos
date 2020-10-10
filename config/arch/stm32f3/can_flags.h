/*==============================================================================
File    can_flags.h

Author  Daniel Zorychta

Brief   CAN driver

        Copyright (C) 2020 Daniel Zorychta <daniel.zorychta@gmail.com>

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

#ifndef _CAN_FLAGS_H_
#define _CAN_FLAGS_H_

/*--
this:SetLayout("TitledGridBack", 2, "Home > Microcontroller > CAN",
               function() this:LoadFile("arch/arch_flags.h") end)
++*/

/*--
this:AddWidget("Checkbox", "Debug freeze")
--*/
#define __CAN_CFG_DEBUG_FREEZE__ _YES_

#endif /* _CAN_FLAGS_H_ */
/*==============================================================================
  End of file
==============================================================================*/
