/*=========================================================================*//**
@file    wdg_flags.h

@author  Daniel Zorychta

@brief   WDG module configuration flags.

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

#ifndef _WDG_FLAGS_H_
#define _WDG_FLAGS_H_

/*--
this:SetLayout("TitledGridBack", 2, "Home > Microcontroller > WDG",
               function() this:LoadFile("arch/arch_flags.h") end)

-- timeout to register
local T2R     = {}
T2R[#T2R + 1] = {TIME = "4 ms",   DIV = 4,   RELOAD = 40  }
T2R[#T2R + 1] = {TIME = "8 ms",   DIV = 4,   RELOAD = 80  }
T2R[#T2R + 1] = {TIME = "16 ms",  DIV = 4,   RELOAD = 160 }
T2R[#T2R + 1] = {TIME = "32 ms",  DIV = 4,   RELOAD = 320 }
T2R[#T2R + 1] = {TIME = "64 ms",  DIV = 8,   RELOAD = 320 }
T2R[#T2R + 1] = {TIME = "125 ms", DIV = 16,  RELOAD = 313 }
T2R[#T2R + 1] = {TIME = "250 ms", DIV = 32,  RELOAD = 313 }
T2R[#T2R + 1] = {TIME = "500 ms", DIV = 64,  RELOAD = 313 }
T2R[#T2R + 1] = {TIME = "1 s",    DIV = 256, RELOAD = 157 }
T2R[#T2R + 1] = {TIME = "5 s",    DIV = 256, RELOAD = 782 }
T2R[#T2R + 1] = {TIME = "10 s",   DIV = 256, RELOAD = 1563}
T2R[#T2R + 1] = {TIME = "15 s",   DIV = 256, RELOAD = 2344}
T2R[#T2R + 1] = {TIME = "20 s",   DIV = 256, RELOAD = 3125}
T2R[#T2R + 1] = {TIME = "25 s",   DIV = 256, RELOAD = 3907}

this.AddTimeouts = function(this, flagName)
    for i = 1, #T2R do
        this:AddItem(T2R[i].TIME, T2R[i].TIME, flagName)
    end
end

this.GetDivReload = function(this, time)
    for i = 1, #T2R do
        if T2R[i].TIME == time then
            return T2R[i].DIV, T2R[i].RELOAD
        end
    end
end

this.GetTime = function(this, div, reload)
    for i = 1, #T2R do
        if T2R[i].DIV == tonumber(div) and T2R[i].RELOAD == tonumber(reload) then
            return T2R[i].TIME
        end
    end

    return T2R[1].TIME
end
++*/

/*--
this:AddWidget("Combobox", "Lock device at open")
this:AddItem("No", "_NO_")
this:AddItem("Yes", "_YES_")
--*/
#define __WDG_DEVICE_LOCK_AT_OPEN__ _YES_

/*--
this:AddWidget("Combobox", "Disable Watchdog on debug")
this:AddItem("No", "_NO_")
this:AddItem("Yes", "_YES_")
--*/
#define __WDG_DISABLE_ON_DEBUG__ _YES_

/*--
this:AddWidget("Value")
--*/
#define __WDG_CLK_DIVIDER__ 256

/*--
this:AddWidget("Value")
--*/
#define __WDG_RELOAD_VALUE__ 1563

/*--
-- widget with timeout selection
this:AddExtraWidget("Combobox", "ComboboxTimeout", "Watchdog timeout")
this:AddTimeouts("ComboboxTimeout")
this:SetEvent("clicked", "ComboboxTimeout",
    function()
        local selection   = this:GetFlagValue("ComboboxTimeout")
        local DIV, RELOAD = this:GetDivReload(selection)

        this:SetFlagValue("__WDG_CLK_DIVIDER__", tostring(DIV))
        this:SetFlagValue("__WDG_RELOAD_VALUE__", tostring(RELOAD))
    end
)

-- set timout selection widget according to selected divider and reload value
local DIV    = this:GetFlagValue("__WDG_CLK_DIVIDER__")
local RELOAD = this:GetFlagValue("__WDG_RELOAD_VALUE__")
local time   = this:GetTime(DIV, RELOAD)
this:SetFlagValue("ComboboxTimeout", time)
++*/

#endif /* _WDG_FLAGS_H_ */
/*==============================================================================
  End of file
==============================================================================*/
