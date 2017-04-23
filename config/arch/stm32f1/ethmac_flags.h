/*=========================================================================*//**
@file    ethmac_flags.h

@author  Daniel Zorychta

@brief   ETHMAC module configuration flags.

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

#ifndef _ETHMAC_FLAGS_H_
#define _ETHMAC_FLAGS_H_

/*--
this:SetLayout("TitledGridBack", 2, "Home > Microcontroller > ETHMAC",
               function() this:LoadFile("arch/arch_flags.h") end)
++*/



/*--
this:AddExtraWidget("Label", "LabelGeneral", "General", -1, "bold")
this:AddExtraWidget("Void", "VoidGeneral")
++*/
/*--
this:AddWidget("Combobox", "Hardware checksum")
this:AddItem("No", "_NO_")
this:AddItem("Yes", "_YES_")
--*/
#define __ETHMAC_CHECKSUM_BY_HARDWARE__ _YES_

/*--
this:AddWidget("Combobox", "Interrupt priority")
uC.AddPriorityItems(this)
--*/
#define __ETHMAC_IRQ_PRIORITY__ __CPU_IRQ_USER_PRIORITY__

/*--
this:AddWidget("Combobox", "Link speed")
this:AddItem("10 Mbps", "ETH_Speed_10M")
this:AddItem("100 Mbps", "ETH_Speed_100M")
--*/
#define __ETHMAC_SPEED__ ETH_Speed_100M


/*--
this:AddExtraWidget("Label", "LabelPHY", "\nPHY", -1, "bold")
this:AddExtraWidget("Void", "VoidPHY")
++*/
/*--
this:AddWidget("Combobox", "Device")
this:AddItem("DP83848", "DP83848")
this:AddItem("LAN8700", "LAN8700")
this:AddItem("STE100P", "STE100P")
this:SetEvent("clicked",
    function()
        local dev    = this:GetFlagValue("__ETHMAC_DEVICE__")
        local DEVICE = {}
        DEVICE["DP83848"] = {SR = "16", SPEED_BM = "0x0002", DUPLEX_BM = "0x0004"}
        DEVICE["LAN8700"] = {SR = "31", SPEED_BM = "0x0004", DUPLEX_BM = "0x0010"}
        DEVICE["STE100P"] = {SR = "19", SPEED_BM = "0x0004", DUPLEX_BM = "0x0010"}

        this:SetFlagValue("__ETHMAC_PHY_SR__", DEVICE[dev].SR)
        this:SetFlagValue("__ETHMAC_PHY_SPEED_STATUS_BM__", DEVICE[dev].SPEED_BM)
        this:SetFlagValue("__ETHMAC_PHY_DUPLEX_STATUS_BM__", DEVICE[dev].DUPLEX_BM)
    end
)
--*/
#define __ETHMAC_DEVICE__ DP83848

/*-- Value set by __ETHMAC_DEVICE__ event
this:AddWidget("Value")
--*/
#define __ETHMAC_PHY_SR__ 16

/*-- Value set by __ETHMAC_DEVICE__ event
this:AddWidget("Value")
--*/
#define __ETHMAC_PHY_SPEED_STATUS_BM__ 0x0002

/*-- Value set by __ETHMAC_DEVICE__ event
this:AddWidget("Value")
--*/
#define __ETHMAC_PHY_DUPLEX_STATUS_BM__ 0x0004

/*--
this:AddWidget("Spinbox", 0, 255, "Address")
--*/
#define __ETHMAC_PHY_ADDRESS__ 1

/*--
this:AddWidget("Spinbox", 100, 2000, "Reset delay [ms]")
--*/
#define __ETHMAC_PHY_RESET_DELAY__ 250

/*--
this:AddWidget("Spinbox", 100, 2000, "Setup delay [ms]")
--*/
#define __ETHMAC_PHY_CONFIG_DELAY__ 500

/*--
this:AddWidget("Combobox", "Auto negotiation")
this:AddItem("Disable", "ETH_AutoNegotiation_Disable")
this:AddItem("Enable", "ETH_AutoNegotiation_Enable")
this:SetToolTip("If auto negotiation is enabled then link should be connected to PHY. "..
                "If link is not connected then module does not initialize.")
--*/
#define __ETHMAC_PHY_AUTONEGOTIATION__ ETH_AutoNegotiation_Disable

#endif /* _ETHMAC_FLAGS_H_ */
/*==============================================================================
  End of file
==============================================================================*/
