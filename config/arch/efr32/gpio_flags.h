/*=========================================================================*//**
@file    gpio_flags.h

@author  Daniel Zorychta

@brief   GPIO module configuration flags.

@note    Copyright (C) 2017 Daniel Zorychta <daniel.zorychta@gmail.com>

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

#ifndef _GPIO_FLAGS_H_
#define _GPIO_FLAGS_H_

/*--
local function Configure(gpio) _GPIO_SELECTION_ = gpio this:Reload() end

local PACKAGE = {}
PACKAGE["L125"] = {A = 0x03FF, B = 0xFFC0, C = 0x0FFF, D = 0xFF00, F = 0xFFFF, I = 0x000F, J = 0xC000, K = 0x0007}
PACKAGE["M48"]  = {A = 0x003F, B = 0xF800, C = 0x0FC0, D = 0xFC00, F = 0x00FF}
PACKAGE["M32"]  = {A = 0x0003, B = 0xF800, C = 0x0C00, D = 0xE000, F = 0x000F}

local port = {"A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K"}
local pack = uC.NAME:gsub("EFR32.*([LM]%d+)$", "%1")

if _GPIO_SELECTION_ == nil then
    this:SetLayout("TitledGridBack", 2, "Home > Microcontroller > GPIO",
                   function() this:LoadFile("arch/arch_flags.h") end)

    this:AddExtraWidget("Label", "LabelTitle", "Available ports", -1, "bold")
    this:AddExtraWidget("Void", "VoidTitle")

    for i = 1, #port do
        if PACKAGE[pack][port[i]] ~= nil then
            local gpio = port[i]
            this:AddExtraWidget("Label", "Label_GPIO"..gpio, "GPIO"..gpio)
            this:AddExtraWidget("Hyperlink", "HL_GPIO"..gpio, "Configure")
            this:SetEvent("clicked", "HL_GPIO"..gpio, function() Configure("GPIO"..gpio) end)
        end
    end

    this:Finish()
else
    this:SetLayout("TitledGridBack", 4, "Home > Microcontroller > ".._GPIO_SELECTION_,
                   function() Configure(nil) end)

    this.AddPinMode = function(this, portLetter, pinNumber)
        this:AddItem("Disabled (state High for Pull-Up)", "_GPIO_MODE_DISABLED")
        this:AddItem("Input", "_GPIO_MODE_INPUT")
        this:AddItem("Input Pulled", "_GPIO_MODE_INPUTPULL")
        this:AddItem("Input Pulled (filtered)", "_GPIO_MODE_INPUTPULLFILTER")
        this:AddItem("Output Push Pull", "_GPIO_MODE_PUSHPULL")
        this:AddItem("Output Wired OR", "_GPIO_MODE_WIREDOR")
        this:AddItem("Output Wired OR Pull-Down", "_GPIO_MODE_WIREDORPULLDOWN")
        this:AddItem("Output Wired AND", "_GPIO_MODE_WIREDAND")
        this:AddItem("Output Wired AND (filtered)", "_GPIO_MODE_WIREDANDFILTER")
        this:AddItem("Output Wired AND Pull-Up", "_GPIO_MODE_WIREDANDPULLUP")
        this:AddItem("Output Wired AND Pull-Up (filtered)", "_GPIO_MODE_WIREDANDPULLUPFILTER")
        this:AddItem("Alternative Output Push Pull", "_GPIO_MODE_PUSHPULLALT")
        this:AddItem("Alternative Output Wired AND", "_GPIO_MODE_WIREDANDALT")
        this:AddItem("Alternative Output Wired AND (filtered)", "_GPIO_MODE_WIREDANDALTFILTER")
        this:AddItem("Alternative Output Wired AND Pull-Up", "_GPIO_MODE_WIREDANDALTPULLUP")
        this:AddItem("Alternative Output Wired AND Pull-Up (filtered)", "_GPIO_MODE_WIREDANDALTPULLUPFILTER")

        this:SetEvent("clicked", function()
            local mode  = this:GetFlagValue("__GPIO_P"..portLetter.."_PIN_"..pinNumber.."_MODE__")
            local state = "__GPIO_P"..portLetter.."_PIN_"..pinNumber.."_STATE__"

            if mode:match("_GPIO_MODE_INPUT$") or mode:match("ALT") then
                this:Enable(false, state)
            else
                this:Enable(true, state)
            end
        end)
    end

    this.AddPinState = function(this, portLetter, pinNumber)
        this:AddItem("Low", "_LOW")
        this:AddItem("High", "_HIGH")

        local mode = this:GetFlagValue("__GPIO_P"..portLetter.."_PIN_"..pinNumber.."_MODE__")

        if mode:match("_GPIO_MODE_INPUT$") or mode:match("ALT") then
            this:Enable(false)
        else
            this:Enable(true)
        end
    end

    this.PinExist = function(this, portLetter, pinNumber)
        if _GPIO_SELECTION_ ~= "GPIO"..portLetter then
            return false
        end

        if PACKAGE[pack][portLetter] ~= nil then
            return bit32.extract(PACKAGE[pack][portLetter], pinNumber) > 0
        else
            return false
        end
    end

    this.PortExist = function(this, portLetter)
        if _GPIO_SELECTION_ ~= "GPIO"..portLetter then
            return false
        else
            return true
        end
    end

    this.AddProposals = function(this, tab, portLetter, pinNumber)
        for i = 1, #tab do this:AddItem(tab[i].NAME, "") end
        this:SetEvent("clicked",
            function()
                local nameFlag  = "__GPIO_P"..portLetter.."_PIN_"..pinNumber.."_NAME__"
                local modeFlag  = "__GPIO_P"..portLetter.."_PIN_"..pinNumber.."_MODE__"
                local stateFlag = "__GPIO_P"..portLetter.."_PIN_"..pinNumber.."_STATE__"
                local selection = this:GetFlagValue(nameFlag)

                for i = 1, #tab do
                    if tab[i].NAME == selection then

                        this:SetFlagValue(modeFlag, tab[i].MODE)
                        this:SetFlagValue(stateFlag, tab[i].STATE)

                        if tab[i].MODE:match("_GPIO_MODE_INPUT$") or tab[i].MODE:match("ALT") then
                            this:Enable(false, stateFlag)
                        else
                            this:Enable(true, stateFlag)
                        end
                    end
                end
            end
        )
    end
end
++*/


/*----------------------------------------------------------------------------*/
/*--
this:AddExtraWidget("Void", "Void_GPIO_DRIVE_STRENGTH1")
++*/
/*--
if this:PortExist("A") then
    this:AddWidget("Combobox", "Drive strength")
    this:AddItem("Strong (10 mA)", "_GPIO_DRIVE_STRONG")
    this:AddItem("Weak (1 mA)", "_GPIO_DRIVE_WEAK")
    this:AddExtraWidget("Void", "Void_GPIO_DRIVE_STRENGTH2")
end
--*/
#define __GPIO_PA_DRIVE_STRENGTH__ _GPIO_DRIVE_STRONG
/*--
if this:PortExist("B") then
    this:AddWidget("Combobox", "Drive strength")
    this:AddItem("Strong (10 mA)", "_GPIO_DRIVE_STRONG")
    this:AddItem("Weak (1 mA)", "_GPIO_DRIVE_WEAK")
    this:AddExtraWidget("Void", "Void_GPIO_DRIVE_STRENGTH2")
end
--*/
#define __GPIO_PB_DRIVE_STRENGTH__ _GPIO_DRIVE_STRONG
/*--
if this:PortExist("C") then
    this:AddWidget("Combobox", "Drive strength")
    this:AddItem("Strong (10 mA)", "_GPIO_DRIVE_STRONG")
    this:AddItem("Weak (1 mA)", "_GPIO_DRIVE_WEAK")
    this:AddExtraWidget("Void", "Void_GPIO_DRIVE_STRENGTH2")
end
--*/
#define __GPIO_PC_DRIVE_STRENGTH__ _GPIO_DRIVE_STRONG
/*--
if this:PortExist("D") then
    this:AddWidget("Combobox", "Drive strength")
    this:AddItem("Strong (10 mA)", "_GPIO_DRIVE_STRONG")
    this:AddItem("Weak (1 mA)", "_GPIO_DRIVE_WEAK")
    this:AddExtraWidget("Void", "Void_GPIO_DRIVE_STRENGTH2")
end
--*/
#define __GPIO_PD_DRIVE_STRENGTH__ _GPIO_DRIVE_STRONG
/*--
if this:PortExist("E") then
    this:AddWidget("Combobox", "Drive strength")
    this:AddItem("Strong (10 mA)", "_GPIO_DRIVE_STRONG")
    this:AddItem("Weak (1 mA)", "_GPIO_DRIVE_WEAK")
    this:AddExtraWidget("Void", "Void_GPIO_DRIVE_STRENGTH2")
end
--*/
#define __GPIO_PE_DRIVE_STRENGTH__ _GPIO_DRIVE_STRONG
/*--
if this:PortExist("F") then
    this:AddWidget("Combobox", "Drive strength")
    this:AddItem("Strong (10 mA)", "_GPIO_DRIVE_STRONG")
    this:AddItem("Weak (1 mA)", "_GPIO_DRIVE_WEAK")
    this:AddExtraWidget("Void", "Void_GPIO_DRIVE_STRENGTH2")
end
--*/
#define __GPIO_PF_DRIVE_STRENGTH__ _GPIO_DRIVE_STRONG
/*--
if this:PortExist("G") then
    this:AddWidget("Combobox", "Drive strength")
    this:AddItem("Strong (10 mA)", "_GPIO_DRIVE_STRONG")
    this:AddItem("Weak (1 mA)", "_GPIO_DRIVE_WEAK")
    this:AddExtraWidget("Void", "Void_GPIO_DRIVE_STRENGTH2")
end
--*/
#define __GPIO_PG_DRIVE_STRENGTH__ _GPIO_DRIVE_STRONG
/*--
if this:PortExist("H") then
    this:AddWidget("Combobox", "Drive strength")
    this:AddItem("Strong (10 mA)", "_GPIO_DRIVE_STRONG")
    this:AddItem("Weak (1 mA)", "_GPIO_DRIVE_WEAK")
    this:AddExtraWidget("Void", "Void_GPIO_DRIVE_STRENGTH2")
end
--*/
#define __GPIO_PH_DRIVE_STRENGTH__ _GPIO_DRIVE_STRONG
/*--
if this:PortExist("I") then
    this:AddWidget("Combobox", "Drive strength")
    this:AddItem("Strong (10 mA)", "_GPIO_DRIVE_STRONG")
    this:AddItem("Weak (1 mA)", "_GPIO_DRIVE_WEAK")
    this:AddExtraWidget("Void", "Void_GPIO_DRIVE_STRENGTH2")
end
--*/
#define __GPIO_PI_DRIVE_STRENGTH__ _GPIO_DRIVE_STRONG
/*--
if this:PortExist("J") then
    this:AddWidget("Combobox", "Drive strength")
    this:AddItem("Strong (10 mA)", "_GPIO_DRIVE_STRONG")
    this:AddItem("Weak (1 mA)", "_GPIO_DRIVE_WEAK")
    this:AddExtraWidget("Void", "Void_GPIO_DRIVE_STRENGTH2")
end
--*/
#define __GPIO_PJ_DRIVE_STRENGTH__ _GPIO_DRIVE_STRONG
/*--
if this:PortExist("K") then
    this:AddWidget("Combobox", "Drive strength")
    this:AddItem("Strong (10 mA)", "_GPIO_DRIVE_STRONG")
    this:AddItem("Weak (1 mA)", "_GPIO_DRIVE_WEAK")
    this:AddExtraWidget("Void", "Void_GPIO_DRIVE_STRENGTH2")
end
--*/
#define __GPIO_PK_DRIVE_STRENGTH__ _GPIO_DRIVE_STRONG


/*--
this:AddExtraWidget("Void", "Void_GPIO_DRIVE_STRENGTH_ALT1")
++*/
/*--
if this:PortExist("A") then
    this:AddWidget("Combobox", "Drive strength alternative")
    this:AddItem("Strong (10 mA)", "_GPIO_DRIVE_STRONG")
    this:AddItem("Weak (1 mA)", "_GPIO_DRIVE_WEAK")
    this:AddExtraWidget("Void", "Void_GPIO_DRIVE_STRENGTH_ALT2")
end
--*/
#define __GPIO_PA_DRIVE_STRENGTH_ALT__ _GPIO_DRIVE_STRONG
/*--
if this:PortExist("B") then
    this:AddWidget("Combobox", "Drive strength alternative")
    this:AddItem("Strong (10 mA)", "_GPIO_DRIVE_STRONG")
    this:AddItem("Weak (1 mA)", "_GPIO_DRIVE_WEAK")
    this:AddExtraWidget("Void", "Void_GPIO_DRIVE_STRENGTH_ALT2")
end
--*/
#define __GPIO_PB_DRIVE_STRENGTH_ALT__ _GPIO_DRIVE_STRONG
/*--
if this:PortExist("C") then
    this:AddWidget("Combobox", "Drive strength alternative")
    this:AddItem("Strong (10 mA)", "_GPIO_DRIVE_STRONG")
    this:AddItem("Weak (1 mA)", "_GPIO_DRIVE_WEAK")
    this:AddExtraWidget("Void", "Void_GPIO_DRIVE_STRENGTH_ALT2")
end
--*/
#define __GPIO_PC_DRIVE_STRENGTH_ALT__ _GPIO_DRIVE_STRONG
/*--
if this:PortExist("D") then
    this:AddWidget("Combobox", "Drive strength alternative")
    this:AddItem("Strong (10 mA)", "_GPIO_DRIVE_STRONG")
    this:AddItem("Weak (1 mA)", "_GPIO_DRIVE_WEAK")
    this:AddExtraWidget("Void", "Void_GPIO_DRIVE_STRENGTH_ALT2")
end
--*/
#define __GPIO_PD_DRIVE_STRENGTH_ALT__ _GPIO_DRIVE_STRONG
/*--
if this:PortExist("E") then
    this:AddWidget("Combobox", "Drive strength alternative")
    this:AddItem("Strong (10 mA)", "_GPIO_DRIVE_STRONG")
    this:AddItem("Weak (1 mA)", "_GPIO_DRIVE_WEAK")
    this:AddExtraWidget("Void", "Void_GPIO_DRIVE_STRENGTH_ALT2")
end
--*/
#define __GPIO_PE_DRIVE_STRENGTH_ALT__ _GPIO_DRIVE_STRONG
/*--
if this:PortExist("F") then
    this:AddWidget("Combobox", "Drive strength alternative")
    this:AddItem("Strong (10 mA)", "_GPIO_DRIVE_STRONG")
    this:AddItem("Weak (1 mA)", "_GPIO_DRIVE_WEAK")
    this:AddExtraWidget("Void", "Void_GPIO_DRIVE_STRENGTH_ALT2")
end
--*/
#define __GPIO_PF_DRIVE_STRENGTH_ALT__ _GPIO_DRIVE_STRONG
/*--
if this:PortExist("G") then
    this:AddWidget("Combobox", "Drive strength alternative")
    this:AddItem("Strong (10 mA)", "_GPIO_DRIVE_STRONG")
    this:AddItem("Weak (1 mA)", "_GPIO_DRIVE_WEAK")
    this:AddExtraWidget("Void", "Void_GPIO_DRIVE_STRENGTH_ALT2")
end
--*/
#define __GPIO_PG_DRIVE_STRENGTH_ALT__ _GPIO_DRIVE_STRONG
/*--
if this:PortExist("H") then
    this:AddWidget("Combobox", "Drive strength alternative")
    this:AddItem("Strong (10 mA)", "_GPIO_DRIVE_STRONG")
    this:AddItem("Weak (1 mA)", "_GPIO_DRIVE_WEAK")
    this:AddExtraWidget("Void", "Void_GPIO_DRIVE_STRENGTH_ALT2")
end
--*/
#define __GPIO_PH_DRIVE_STRENGTH_ALT__ _GPIO_DRIVE_STRONG
/*--
if this:PortExist("I") then
    this:AddWidget("Combobox", "Drive strength alternative")
    this:AddItem("Strong (10 mA)", "_GPIO_DRIVE_STRONG")
    this:AddItem("Weak (1 mA)", "_GPIO_DRIVE_WEAK")
    this:AddExtraWidget("Void", "Void_GPIO_DRIVE_STRENGTH_ALT2")
end
--*/
#define __GPIO_PI_DRIVE_STRENGTH_ALT__ _GPIO_DRIVE_STRONG
/*--
if this:PortExist("J") then
    this:AddWidget("Combobox", "Drive strength alternative")
    this:AddItem("Strong (10 mA)", "_GPIO_DRIVE_STRONG")
    this:AddItem("Weak (1 mA)", "_GPIO_DRIVE_WEAK")
    this:AddExtraWidget("Void", "Void_GPIO_DRIVE_STRENGTH_ALT2")
end
--*/
#define __GPIO_PJ_DRIVE_STRENGTH_ALT__ _GPIO_DRIVE_STRONG
/*--
if this:PortExist("K") then
    this:AddWidget("Combobox", "Drive strength alternative")
    this:AddItem("Strong (10 mA)", "_GPIO_DRIVE_STRONG")
    this:AddItem("Weak (1 mA)", "_GPIO_DRIVE_WEAK")
    this:AddExtraWidget("Void", "Void_GPIO_DRIVE_STRENGTH_ALT2")
end
--*/
#define __GPIO_PK_DRIVE_STRENGTH_ALT__ _GPIO_DRIVE_STRONG


/*--
this:AddExtraWidget("Void", "Void_GPIO_SLEW_RATE1")
++*/
/*--
if this:PortExist("A") then
    this:AddWidget("Spinbox", 0, 7, "Slew rate")
    this:AddExtraWidget("Void", "Void_GPIO_SLEW_RATE2")
end
--*/
#define __GPIO_PA_SLEW_RATE__ 5
/*--
if this:PortExist("B") then
    this:AddWidget("Spinbox", 0, 7, "Slew rate")
    this:AddExtraWidget("Void", "Void_GPIO_SLEW_RATE2")
end
--*/
#define __GPIO_PB_SLEW_RATE__ 5
/*--
if this:PortExist("C") then
    this:AddWidget("Spinbox", 0, 7, "Slew rate")
    this:AddExtraWidget("Void", "Void_GPIO_SLEW_RATE2")
end
--*/
#define __GPIO_PC_SLEW_RATE__ 5
/*--
if this:PortExist("D") then
    this:AddWidget("Spinbox", 0, 7, "Slew rate")
    this:AddExtraWidget("Void", "Void_GPIO_SLEW_RATE2")
end
--*/
#define __GPIO_PD_SLEW_RATE__ 5
/*--
if this:PortExist("E") then
    this:AddWidget("Spinbox", 0, 7, "Slew rate")
    this:AddExtraWidget("Void", "Void_GPIO_SLEW_RATE2")
end
--*/
#define __GPIO_PE_SLEW_RATE__ 5
/*--
if this:PortExist("F") then
    this:AddWidget("Spinbox", 0, 7, "Slew rate")
    this:AddExtraWidget("Void", "Void_GPIO_SLEW_RATE2")
end
--*/
#define __GPIO_PF_SLEW_RATE__ 5
/*--
if this:PortExist("G") then
    this:AddWidget("Spinbox", 0, 7, "Slew rate")
    this:AddExtraWidget("Void", "Void_GPIO_SLEW_RATE2")
end
--*/
#define __GPIO_PG_SLEW_RATE__ 5
/*--
if this:PortExist("H") then
    this:AddWidget("Spinbox", 0, 7, "Slew rate")
    this:AddExtraWidget("Void", "Void_GPIO_SLEW_RATE2")
end
--*/
#define __GPIO_PH_SLEW_RATE__ 5
/*--
if this:PortExist("I") then
    this:AddWidget("Spinbox", 0, 7, "Slew rate")
    this:AddExtraWidget("Void", "Void_GPIO_SLEW_RATE2")
end
--*/
#define __GPIO_PI_SLEW_RATE__ 5
/*--
if this:PortExist("J") then
    this:AddWidget("Spinbox", 0, 7, "Slew rate")
    this:AddExtraWidget("Void", "Void_GPIO_SLEW_RATE2")
end
--*/
#define __GPIO_PJ_SLEW_RATE__ 5
/*--
if this:PortExist("K") then
    this:AddWidget("Spinbox", 0, 7, "Slew rate")
    this:AddExtraWidget("Void", "Void_GPIO_SLEW_RATE2")
end
--*/
#define __GPIO_PK_SLEW_RATE__ 5


/*--
this:AddExtraWidget("Void", "Void_GPIO_SLEW_RATE_ALT1")
++*/
/*--
if this:PortExist("A", 0) then
    this:AddWidget("Spinbox", 0, 7, "Slew rate alternative")
    this:AddExtraWidget("Void", "Void_GPIO_SLEW_RATE_ALT2")
end
--*/
#define __GPIO_PA_SLEW_RATE_ALT__ 5
/*--
if this:PortExist("B", 0) then
    this:AddWidget("Spinbox", 0, 7, "Slew rate alternative")
    this:AddExtraWidget("Void", "Void_GPIO_SLEW_RATE_ALT2")
end
--*/
#define __GPIO_PB_SLEW_RATE_ALT__ 5
/*--
if this:PortExist("C", 0) then
    this:AddWidget("Spinbox", 0, 7, "Slew rate alternative")
    this:AddExtraWidget("Void", "Void_GPIO_SLEW_RATE_ALT2")
end
--*/
#define __GPIO_PC_SLEW_RATE_ALT__ 5
/*--
if this:PortExist("D", 0) then
    this:AddWidget("Spinbox", 0, 7, "Slew rate alternative")
    this:AddExtraWidget("Void", "Void_GPIO_SLEW_RATE_ALT2")
end
--*/
#define __GPIO_PD_SLEW_RATE_ALT__ 5
/*--
if this:PortExist("E", 0) then
    this:AddWidget("Spinbox", 0, 7, "Slew rate alternative")
    this:AddExtraWidget("Void", "Void_GPIO_SLEW_RATE_ALT2")
end
--*/
#define __GPIO_PE_SLEW_RATE_ALT__ 5
/*--
if this:PortExist("F", 0) then
    this:AddWidget("Spinbox", 0, 7, "Slew rate alternative")
    this:AddExtraWidget("Void", "Void_GPIO_SLEW_RATE_ALT2")
end
--*/
#define __GPIO_PF_SLEW_RATE_ALT__ 5
/*--
if this:PortExist("G", 0) then
    this:AddWidget("Spinbox", 0, 7, "Slew rate alternative")
    this:AddExtraWidget("Void", "Void_GPIO_SLEW_RATE_ALT2")
end
--*/
#define __GPIO_PG_SLEW_RATE_ALT__ 5
/*--
if this:PortExist("H", 0) then
    this:AddWidget("Spinbox", 0, 7, "Slew rate alternative")
    this:AddExtraWidget("Void", "Void_GPIO_SLEW_RATE_ALT2")
end
--*/
#define __GPIO_PH_SLEW_RATE_ALT__ 5
/*--
if this:PortExist("I", 0) then
    this:AddWidget("Spinbox", 0, 7, "Slew rate alternative")
    this:AddExtraWidget("Void", "Void_GPIO_SLEW_RATE_ALT2")
end
--*/
#define __GPIO_PI_SLEW_RATE_ALT__ 5
/*--
if this:PortExist("J", 0) then
    this:AddWidget("Spinbox", 0, 7, "Slew rate alternative")
    this:AddExtraWidget("Void", "Void_GPIO_SLEW_RATE_ALT2")
end
--*/
#define __GPIO_PJ_SLEW_RATE_ALT__ 5
/*--
if this:PortExist("K", 0) then
    this:AddWidget("Spinbox", 0, 7, "Slew rate alternative")
    this:AddExtraWidget("Void", "Void_GPIO_SLEW_RATE_ALT2")
end
--*/
#define __GPIO_PK_SLEW_RATE_ALT__ 5


/*--
this:AddExtraWidget("Void", "Void_GPIO_IN_DISABLE1")
++*/
/*--
if this:PortExist("A") then
    this:AddWidget("Combobox", "Disable inputs")
    this:AddItem("No", "0")
    this:AddItem("Yes", "1")
    this:AddExtraWidget("Void", "Void_GPIO_IN_DISABLE2")
end
--*/
#define __GPIO_PA_INPUT_DISABLE__ 0
/*--
if this:PortExist("B") then
    this:AddWidget("Combobox", "Disable inputs")
    this:AddItem("No", "0")
    this:AddItem("Yes", "1")
    this:AddExtraWidget("Void", "Void_GPIO_IN_DISABLE2")
end
--*/
#define __GPIO_PB_INPUT_DISABLE__ 0
/*--
if this:PortExist("C") then
    this:AddWidget("Combobox", "Disable inputs")
    this:AddItem("No", "0")
    this:AddItem("Yes", "1")
    this:AddExtraWidget("Void", "Void_GPIO_IN_DISABLE2")
end
--*/
#define __GPIO_PC_INPUT_DISABLE__ 0
/*--
if this:PortExist("D") then
    this:AddWidget("Combobox", "Disable inputs")
    this:AddItem("No", "0")
    this:AddItem("Yes", "1")
    this:AddExtraWidget("Void", "Void_GPIO_IN_DISABLE2")
end
--*/
#define __GPIO_PD_INPUT_DISABLE__ 0
/*--
if this:PortExist("E") then
    this:AddWidget("Combobox", "Disable inputs")
    this:AddItem("No", "0")
    this:AddItem("Yes", "1")
    this:AddExtraWidget("Void", "Void_GPIO_IN_DISABLE2")
end
--*/
#define __GPIO_PE_INPUT_DISABLE__ 0
/*--
if this:PortExist("F") then
    this:AddWidget("Combobox", "Disable inputs")
    this:AddItem("No", "0")
    this:AddItem("Yes", "1")
    this:AddExtraWidget("Void", "Void_GPIO_IN_DISABLE2")
end
--*/
#define __GPIO_PF_INPUT_DISABLE__ 0
/*--
if this:PortExist("G") then
    this:AddWidget("Combobox", "Disable inputs")
    this:AddItem("No", "0")
    this:AddItem("Yes", "1")
    this:AddExtraWidget("Void", "Void_GPIO_IN_DISABLE2")
end
--*/
#define __GPIO_PG_INPUT_DISABLE__ 0
/*--
if this:PortExist("H") then
    this:AddWidget("Combobox", "Disable inputs")
    this:AddItem("No", "0")
    this:AddItem("Yes", "1")
    this:AddExtraWidget("Void", "Void_GPIO_IN_DISABLE2")
end
--*/
#define __GPIO_PH_INPUT_DISABLE__ 0
/*--
if this:PortExist("I") then
    this:AddWidget("Combobox", "Disable inputs")
    this:AddItem("No", "0")
    this:AddItem("Yes", "1")
    this:AddExtraWidget("Void", "Void_GPIO_IN_DISABLE2")
end
--*/
#define __GPIO_PI_INPUT_DISABLE__ 0
/*--
if this:PortExist("J") then
    this:AddWidget("Combobox", "Disable inputs")
    this:AddItem("No", "0")
    this:AddItem("Yes", "1")
    this:AddExtraWidget("Void", "Void_GPIO_IN_DISABLE2")
end
--*/
#define __GPIO_PJ_INPUT_DISABLE__ 0
/*--
if this:PortExist("K") then
    this:AddWidget("Combobox", "Disable inputs")
    this:AddItem("No", "0")
    this:AddItem("Yes", "1")
    this:AddExtraWidget("Void", "Void_GPIO_IN_DISABLE2")
end
--*/
#define __GPIO_PK_INPUT_DISABLE__ 0


/*--
this:AddExtraWidget("Void", "Void_GPIO_IN_DISABLE_ALT1")
++*/
/*--
if this:PortExist("A") then
    this:AddWidget("Combobox", "Disable alternative inputs")
    this:AddItem("No", "0")
    this:AddItem("Yes", "1")
    this:AddExtraWidget("Void", "Void_GPIO_IN_DISABLE_ALT2")
end
--*/
#define __GPIO_PA_INPUT_DISABLE_ALT__ 0
/*--
if this:PortExist("B") then
    this:AddWidget("Combobox", "Disable alternative inputs")
    this:AddItem("No", "0")
    this:AddItem("Yes", "1")
    this:AddExtraWidget("Void", "Void_GPIO_IN_DISABLE_ALT2")
end
--*/
#define __GPIO_PB_INPUT_DISABLE_ALT__ 0
/*--
if this:PortExist("C") then
    this:AddWidget("Combobox", "Disable alternative inputs")
    this:AddItem("No", "0")
    this:AddItem("Yes", "1")
    this:AddExtraWidget("Void", "Void_GPIO_IN_DISABLE_ALT2")
end
--*/
#define __GPIO_PC_INPUT_DISABLE_ALT__ 0
/*--
if this:PortExist("D") then
    this:AddWidget("Combobox", "Disable alternative inputs")
    this:AddItem("No", "0")
    this:AddItem("Yes", "1")
    this:AddExtraWidget("Void", "Void_GPIO_IN_DISABLE_ALT2")
end
--*/
#define __GPIO_PD_INPUT_DISABLE_ALT__ 0
/*--
if this:PortExist("E") then
    this:AddWidget("Combobox", "Disable alternative inputs")
    this:AddItem("No", "0")
    this:AddItem("Yes", "1")
    this:AddExtraWidget("Void", "Void_GPIO_IN_DISABLE_ALT2")
end
--*/
#define __GPIO_PE_INPUT_DISABLE_ALT__ 0
/*--
if this:PortExist("F") then
    this:AddWidget("Combobox", "Disable alternative inputs")
    this:AddItem("No", "0")
    this:AddItem("Yes", "1")
    this:AddExtraWidget("Void", "Void_GPIO_IN_DISABLE_ALT2")
end
--*/
#define __GPIO_PF_INPUT_DISABLE_ALT__ 0
/*--
if this:PortExist("G") then
    this:AddWidget("Combobox", "Disable alternative inputs")
    this:AddItem("No", "0")
    this:AddItem("Yes", "1")
    this:AddExtraWidget("Void", "Void_GPIO_IN_DISABLE_ALT2")
end
--*/
#define __GPIO_PG_INPUT_DISABLE_ALT__ 0
/*--
if this:PortExist("H") then
    this:AddWidget("Combobox", "Disable alternative inputs")
    this:AddItem("No", "0")
    this:AddItem("Yes", "1")
    this:AddExtraWidget("Void", "Void_GPIO_IN_DISABLE_ALT2")
end
--*/
#define __GPIO_PH_INPUT_DISABLE_ALT__ 0
/*--
if this:PortExist("I") then
    this:AddWidget("Combobox", "Disable alternative inputs")
    this:AddItem("No", "0")
    this:AddItem("Yes", "1")
    this:AddExtraWidget("Void", "Void_GPIO_IN_DISABLE_ALT2")
end
--*/
#define __GPIO_PI_INPUT_DISABLE_ALT__ 0
/*--
if this:PortExist("J") then
    this:AddWidget("Combobox", "Disable alternative inputs")
    this:AddItem("No", "0")
    this:AddItem("Yes", "1")
    this:AddExtraWidget("Void", "Void_GPIO_IN_DISABLE_ALT2")
end
--*/
#define __GPIO_PJ_INPUT_DISABLE_ALT__ 0
/*--
if this:PortExist("K") then
    this:AddWidget("Combobox", "Disable alternative inputs")
    this:AddItem("No", "0")
    this:AddItem("Yes", "1")
    this:AddExtraWidget("Void", "Void_GPIO_IN_DISABLE_ALT2")
end
--*/
#define __GPIO_PK_INPUT_DISABLE_ALT__ 0


/*----------------------------------------------------------------------------*/
/*--
this:AddExtraWidget("Label", "LabelPin", "\nPin")
this:AddExtraWidget("Label", "LabelName", "\nName")
this:AddExtraWidget("Label", "LabelMode", "\nMode")
this:AddExtraWidget("Label", "LabelState", "\nState")
++*/



/*----------------------------------------------------------------------------*/
/*--
if this:PinExist("A", 0) then
    local PA0 = {}
    PA0[#PA0 + 1] = {NAME = "PA0_NC", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}
    PA0[#PA0 + 1] = {NAME = "USX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PA0[#PA0 + 1] = {NAME = "USX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PA0[#PA0 + 1] = {NAME = "USX_CLK", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PA0[#PA0 + 1] = {NAME = "USX_CS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PA0[#PA0 + 1] = {NAME = "USX_CTS", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PA0[#PA0 + 1] = {NAME = "USX_RTS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PA0[#PA0 + 1] = {NAME = "LEUX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PA0[#PA0 + 1] = {NAME = "LEUX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PA0[#PA0 + 1] = {NAME = "I2CX_SCL", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PA0[#PA0 + 1] = {NAME = "I2CX_SDA", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PA0[#PA0 + 1] = {NAME = "ADCX", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}

    this:AddWidget("Textbox", "PA0")
    this:AddProposals(PA0, "A", 0)
end
--*/
#define __GPIO_PA_PIN_0_NAME__ US0_TX

/*--
if this:PinExist("A", 0) then
    this:AddWidget("Combobox")
    this:AddPinMode("A", 0)
end
--*/
#define __GPIO_PA_PIN_0_MODE__ _GPIO_MODE_PUSHPULLALT

/*--
if this:PinExist("A", 0) then
    this:AddWidget("Combobox")
    this:AddPinState("A", 0)
end
--*/
#define __GPIO_PA_PIN_0_STATE__ _HIGH

/*--
if this:PinExist("A", 1) then
    local PA1 = {}
    PA1[#PA1 + 1] = {NAME = "PA1_NC", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}
    PA1[#PA1 + 1] = {NAME = "USX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PA1[#PA1 + 1] = {NAME = "USX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PA1[#PA1 + 1] = {NAME = "USX_CLK", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PA1[#PA1 + 1] = {NAME = "USX_CS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PA1[#PA1 + 1] = {NAME = "USX_CTS", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PA1[#PA1 + 1] = {NAME = "USX_RTS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PA1[#PA1 + 1] = {NAME = "LEUX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PA1[#PA1 + 1] = {NAME = "LEUX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PA1[#PA1 + 1] = {NAME = "I2CX_SCL", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PA1[#PA1 + 1] = {NAME = "I2CX_SDA", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PA1[#PA1 + 1] = {NAME = "ADCX", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}

    this:AddWidget("Textbox", "PA1")
    this:AddProposals(PA1, "A", 1)
end
--*/
#define __GPIO_PA_PIN_1_NAME__ US0_RX

/*--
if this:PinExist("A", 1) then
    this:AddWidget("Combobox")
    this:AddPinMode("A", 1)
end
--*/
#define __GPIO_PA_PIN_1_MODE__ _GPIO_MODE_INPUTPULLFILTER

/*--
if this:PinExist("A", 1) then
    this:AddWidget("Combobox")
    this:AddPinState("A", 1)
end
--*/
#define __GPIO_PA_PIN_1_STATE__ _HIGH

/*--
if this:PinExist("A", 2) then
    local PA2 = {}
    PA2[#PA2 + 1] = {NAME = "PA2_NC", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}
    PA2[#PA2 + 1] = {NAME = "USX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PA2[#PA2 + 1] = {NAME = "USX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PA2[#PA2 + 1] = {NAME = "USX_CLK", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PA2[#PA2 + 1] = {NAME = "USX_CS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PA2[#PA2 + 1] = {NAME = "USX_CTS", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PA2[#PA2 + 1] = {NAME = "USX_RTS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PA2[#PA2 + 1] = {NAME = "LEUX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PA2[#PA2 + 1] = {NAME = "LEUX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PA2[#PA2 + 1] = {NAME = "I2CX_SCL", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PA2[#PA2 + 1] = {NAME = "I2CX_SDA", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PA2[#PA2 + 1] = {NAME = "ADCX", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}

    this:AddWidget("Textbox", "PA2")
    this:AddProposals(PA2, "A", 2)
end
--*/
#define __GPIO_PA_PIN_2_NAME__ PA2_NC

/*--
if this:PinExist("A", 2) then
    this:AddWidget("Combobox")
    this:AddPinMode("A", 2)
end
--*/
#define __GPIO_PA_PIN_2_MODE__ _GPIO_MODE_DISABLED

/*--
if this:PinExist("A", 2) then
    this:AddWidget("Combobox")
    this:AddPinState("A", 2)
end
--*/
#define __GPIO_PA_PIN_2_STATE__ _LOW

/*--
if this:PinExist("A", 3) then
    local PA3 = {}
    PA3[#PA3 + 1] = {NAME = "PA3_NC", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}
    PA3[#PA3 + 1] = {NAME = "USX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PA3[#PA3 + 1] = {NAME = "USX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PA3[#PA3 + 1] = {NAME = "USX_CLK", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PA3[#PA3 + 1] = {NAME = "USX_CS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PA3[#PA3 + 1] = {NAME = "USX_CTS", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PA3[#PA3 + 1] = {NAME = "USX_RTS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PA3[#PA3 + 1] = {NAME = "LEUX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PA3[#PA3 + 1] = {NAME = "LEUX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PA3[#PA3 + 1] = {NAME = "I2CX_SCL", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PA3[#PA3 + 1] = {NAME = "I2CX_SDA", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PA3[#PA3 + 1] = {NAME = "ADCX", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}

    this:AddWidget("Textbox", "PA3")
    this:AddProposals(PA3, "A", 3)
end
--*/
#define __GPIO_PA_PIN_3_NAME__ PA3_NC

/*--
if this:PinExist("A", 3) then
    this:AddWidget("Combobox")
    this:AddPinMode("A", 3)
end
--*/
#define __GPIO_PA_PIN_3_MODE__ _GPIO_MODE_DISABLED

/*--
if this:PinExist("A", 3) then
    this:AddWidget("Combobox")
    this:AddPinState("A", 3)
end
--*/
#define __GPIO_PA_PIN_3_STATE__ _LOW

/*--
if this:PinExist("A", 4) then
    local PA4 = {}
    PA4[#PA4 + 1] = {NAME = "PA4_NC", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}
    PA4[#PA4 + 1] = {NAME = "USX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PA4[#PA4 + 1] = {NAME = "USX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PA4[#PA4 + 1] = {NAME = "USX_CLK", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PA4[#PA4 + 1] = {NAME = "USX_CS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PA4[#PA4 + 1] = {NAME = "USX_CTS", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PA4[#PA4 + 1] = {NAME = "USX_RTS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PA4[#PA4 + 1] = {NAME = "LEUX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PA4[#PA4 + 1] = {NAME = "LEUX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PA4[#PA4 + 1] = {NAME = "I2CX_SCL", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PA4[#PA4 + 1] = {NAME = "I2CX_SDA", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PA4[#PA4 + 1] = {NAME = "ADCX", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}

    this:AddWidget("Textbox", "PA4")
    this:AddProposals(PA4, "A", 4)
end
--*/
#define __GPIO_PA_PIN_4_NAME__ PA4_NC

/*--
if this:PinExist("A", 4) then
    this:AddWidget("Combobox")
    this:AddPinMode("A", 4)
end
--*/
#define __GPIO_PA_PIN_4_MODE__ _GPIO_MODE_DISABLED

/*--
if this:PinExist("A", 4) then
    this:AddWidget("Combobox")
    this:AddPinState("A", 4)
end
--*/
#define __GPIO_PA_PIN_4_STATE__ _LOW

/*--
if this:PinExist("A", 5) then
    local PA5 = {}
    PA5[#PA5 + 1] = {NAME = "PA5_NC", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}
    PA5[#PA5 + 1] = {NAME = "USX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PA5[#PA5 + 1] = {NAME = "USX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PA5[#PA5 + 1] = {NAME = "USX_CLK", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PA5[#PA5 + 1] = {NAME = "USX_CS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PA5[#PA5 + 1] = {NAME = "USX_CTS", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PA5[#PA5 + 1] = {NAME = "USX_RTS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PA5[#PA5 + 1] = {NAME = "LEUX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PA5[#PA5 + 1] = {NAME = "LEUX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PA5[#PA5 + 1] = {NAME = "I2CX_SCL", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PA5[#PA5 + 1] = {NAME = "I2CX_SDA", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PA5[#PA5 + 1] = {NAME = "ADCX", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}

    this:AddWidget("Textbox", "PA5")
    this:AddProposals(PA5, "A", 5)
end
--*/
#define __GPIO_PA_PIN_5_NAME__ PA5_NC

/*--
if this:PinExist("A", 5) then
    this:AddWidget("Combobox")
    this:AddPinMode("A", 5)
end
--*/
#define __GPIO_PA_PIN_5_MODE__ _GPIO_MODE_DISABLED

/*--
if this:PinExist("A", 5) then
    this:AddWidget("Combobox")
    this:AddPinState("A", 5)
end
--*/
#define __GPIO_PA_PIN_5_STATE__ _LOW

/*--
if this:PinExist("A", 6) then
    local PA6 = {}
    PA6[#PA6 + 1] = {NAME = "PA6_NC", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}
    PA6[#PA6 + 1] = {NAME = "USX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PA6[#PA6 + 1] = {NAME = "USX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PA6[#PA6 + 1] = {NAME = "USX_CLK", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PA6[#PA6 + 1] = {NAME = "USX_CS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PA6[#PA6 + 1] = {NAME = "USX_CTS", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PA6[#PA6 + 1] = {NAME = "USX_RTS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PA6[#PA6 + 1] = {NAME = "LEUX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PA6[#PA6 + 1] = {NAME = "LEUX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PA6[#PA6 + 1] = {NAME = "I2CX_SCL", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PA6[#PA6 + 1] = {NAME = "I2CX_SDA", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PA6[#PA6 + 1] = {NAME = "ADCX", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}

    this:AddWidget("Textbox", "PA6")
    this:AddProposals(PA6, "A", 6)
end
--*/
#define __GPIO_PA_PIN_6_NAME__ PA6_NC

/*--
if this:PinExist("A", 6) then
    this:AddWidget("Combobox")
    this:AddPinMode("A", 6)
end
--*/
#define __GPIO_PA_PIN_6_MODE__ _GPIO_MODE_DISABLED

/*--
if this:PinExist("A", 6) then
    this:AddWidget("Combobox")
    this:AddPinState("A", 6)
end
--*/
#define __GPIO_PA_PIN_6_STATE__ _LOW

/*--
if this:PinExist("A", 7) then
    local PA7 = {}
    PA7[#PA7 + 1] = {NAME = "PA7_NC", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}
    PA7[#PA7 + 1] = {NAME = "USX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PA7[#PA7 + 1] = {NAME = "USX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PA7[#PA7 + 1] = {NAME = "USX_CLK", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PA7[#PA7 + 1] = {NAME = "USX_CS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PA7[#PA7 + 1] = {NAME = "USX_CTS", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PA7[#PA7 + 1] = {NAME = "USX_RTS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PA7[#PA7 + 1] = {NAME = "LEUX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PA7[#PA7 + 1] = {NAME = "LEUX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PA7[#PA7 + 1] = {NAME = "I2CX_SCL", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PA7[#PA7 + 1] = {NAME = "I2CX_SDA", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PA7[#PA7 + 1] = {NAME = "ADCX", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}

    this:AddWidget("Textbox", "PA7")
    this:AddProposals(PA7, "A", 7)
end
--*/
#define __GPIO_PA_PIN_7_NAME__ PA7_NC

/*--
if this:PinExist("A", 7) then
    this:AddWidget("Combobox")
    this:AddPinMode("A", 7)
end
--*/
#define __GPIO_PA_PIN_7_MODE__ _GPIO_MODE_DISABLED

/*--
if this:PinExist("A", 7) then
    this:AddWidget("Combobox")
    this:AddPinState("A", 7)
end
--*/
#define __GPIO_PA_PIN_7_STATE__ _LOW

/*--
if this:PinExist("A", 8) then
    local PA8 = {}
    PA8[#PA8 + 1] = {NAME = "PA8_NC", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}
    PA8[#PA8 + 1] = {NAME = "USX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PA8[#PA8 + 1] = {NAME = "USX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PA8[#PA8 + 1] = {NAME = "USX_CLK", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PA8[#PA8 + 1] = {NAME = "USX_CS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PA8[#PA8 + 1] = {NAME = "USX_CTS", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PA8[#PA8 + 1] = {NAME = "USX_RTS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PA8[#PA8 + 1] = {NAME = "LEUX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PA8[#PA8 + 1] = {NAME = "LEUX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PA8[#PA8 + 1] = {NAME = "I2CX_SCL", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PA8[#PA8 + 1] = {NAME = "I2CX_SDA", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PA8[#PA8 + 1] = {NAME = "ADCX", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}

    this:AddWidget("Textbox", "PA8")
    this:AddProposals(PA8, "A", 8)
end
--*/
#define __GPIO_PA_PIN_8_NAME__ PA8_NC

/*--
if this:PinExist("A", 8) then
    this:AddWidget("Combobox")
    this:AddPinMode("A", 8)
end
--*/
#define __GPIO_PA_PIN_8_MODE__ _GPIO_MODE_DISABLED

/*--
if this:PinExist("A", 8) then
    this:AddWidget("Combobox")
    this:AddPinState("A", 8)
end
--*/
#define __GPIO_PA_PIN_8_STATE__ _LOW

/*--
if this:PinExist("A", 9) then
    local PA9 = {}
    PA9[#PA9 + 1] = {NAME = "PA9_NC", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}
    PA9[#PA9 + 1] = {NAME = "USX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PA9[#PA9 + 1] = {NAME = "USX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PA9[#PA9 + 1] = {NAME = "USX_CLK", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PA9[#PA9 + 1] = {NAME = "USX_CS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PA9[#PA9 + 1] = {NAME = "USX_CTS", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PA9[#PA9 + 1] = {NAME = "USX_RTS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PA9[#PA9 + 1] = {NAME = "LEUX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PA9[#PA9 + 1] = {NAME = "LEUX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PA9[#PA9 + 1] = {NAME = "I2CX_SCL", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PA9[#PA9 + 1] = {NAME = "I2CX_SDA", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PA9[#PA9 + 1] = {NAME = "ADCX", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}

    this:AddWidget("Textbox", "PA9")
    this:AddProposals(PA9, "A", 9)
end
--*/
#define __GPIO_PA_PIN_9_NAME__ PA9_NC

/*--
if this:PinExist("A", 9) then
    this:AddWidget("Combobox")
    this:AddPinMode("A", 9)
end
--*/
#define __GPIO_PA_PIN_9_MODE__ _GPIO_MODE_DISABLED

/*--
if this:PinExist("A", 9) then
    this:AddWidget("Combobox")
    this:AddPinState("A", 9)
end
--*/
#define __GPIO_PA_PIN_9_STATE__ _LOW

/*--
if this:PinExist("A", 10) then
    local PA10 = {}
    PA10[#PA10 + 1] = {NAME = "PA10_NC", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}
    PA10[#PA10 + 1] = {NAME = "USX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PA10[#PA10 + 1] = {NAME = "USX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PA10[#PA10 + 1] = {NAME = "USX_CLK", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PA10[#PA10 + 1] = {NAME = "USX_CS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PA10[#PA10 + 1] = {NAME = "USX_CTS", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PA10[#PA10 + 1] = {NAME = "USX_RTS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PA10[#PA10 + 1] = {NAME = "LEUX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PA10[#PA10 + 1] = {NAME = "LEUX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PA10[#PA10 + 1] = {NAME = "I2CX_SCL", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PA10[#PA10 + 1] = {NAME = "I2CX_SDA", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PA10[#PA10 + 1] = {NAME = "ADCX", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}

    this:AddWidget("Textbox", "PA10")
    this:AddProposals(PA10, "A", 10)
end
--*/
#define __GPIO_PA_PIN_10_NAME__ PA10_NC

/*--
if this:PinExist("A", 10) then
    this:AddWidget("Combobox")
    this:AddPinMode("A", 10)
end
--*/
#define __GPIO_PA_PIN_10_MODE__ _GPIO_MODE_DISABLED

/*--
if this:PinExist("A", 10) then
    this:AddWidget("Combobox")
    this:AddPinState("A", 10)
end
--*/
#define __GPIO_PA_PIN_10_STATE__ _LOW

/*--
if this:PinExist("A", 11) then
    local PA11 = {}
    PA11[#PA11 + 1] = {NAME = "PA11_NC", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}
    PA11[#PA11 + 1] = {NAME = "USX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PA11[#PA11 + 1] = {NAME = "USX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PA11[#PA11 + 1] = {NAME = "USX_CLK", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PA11[#PA11 + 1] = {NAME = "USX_CS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PA11[#PA11 + 1] = {NAME = "USX_CTS", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PA11[#PA11 + 1] = {NAME = "USX_RTS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PA11[#PA11 + 1] = {NAME = "LEUX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PA11[#PA11 + 1] = {NAME = "LEUX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PA11[#PA11 + 1] = {NAME = "I2CX_SCL", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PA11[#PA11 + 1] = {NAME = "I2CX_SDA", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PA11[#PA11 + 1] = {NAME = "ADCX", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}

    this:AddWidget("Textbox", "PA11")
    this:AddProposals(PA11, "A", 11)
end
--*/
#define __GPIO_PA_PIN_11_NAME__ PA11_NC

/*--
if this:PinExist("A", 11) then
    this:AddWidget("Combobox")
    this:AddPinMode("A", 11)
end
--*/
#define __GPIO_PA_PIN_11_MODE__ _GPIO_MODE_DISABLED

/*--
if this:PinExist("A", 11) then
    this:AddWidget("Combobox")
    this:AddPinState("A", 11)
end
--*/
#define __GPIO_PA_PIN_11_STATE__ _LOW

/*--
if this:PinExist("A", 12) then
    local PA12 = {}
    PA12[#PA12 + 1] = {NAME = "PA12_NC", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}
    PA12[#PA12 + 1] = {NAME = "USX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PA12[#PA12 + 1] = {NAME = "USX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PA12[#PA12 + 1] = {NAME = "USX_CLK", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PA12[#PA12 + 1] = {NAME = "USX_CS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PA12[#PA12 + 1] = {NAME = "USX_CTS", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PA12[#PA12 + 1] = {NAME = "USX_RTS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PA12[#PA12 + 1] = {NAME = "LEUX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PA12[#PA12 + 1] = {NAME = "LEUX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PA12[#PA12 + 1] = {NAME = "I2CX_SCL", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PA12[#PA12 + 1] = {NAME = "I2CX_SDA", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PA12[#PA12 + 1] = {NAME = "ADCX", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}

    this:AddWidget("Textbox", "PA12")
    this:AddProposals(PA12, "A", 12)
end
--*/
#define __GPIO_PA_PIN_12_NAME__ PA12_NC

/*--
if this:PinExist("A", 12) then
    this:AddWidget("Combobox")
    this:AddPinMode("A", 12)
end
--*/
#define __GPIO_PA_PIN_12_MODE__ _GPIO_MODE_DISABLED

/*--
if this:PinExist("A", 12) then
    this:AddWidget("Combobox")
    this:AddPinState("A", 12)
end
--*/
#define __GPIO_PA_PIN_12_STATE__ _LOW

/*--
if this:PinExist("A", 13) then
    local PA13 = {}
    PA13[#PA13 + 1] = {NAME = "PA13_NC", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}
    PA13[#PA13 + 1] = {NAME = "USX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PA13[#PA13 + 1] = {NAME = "USX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PA13[#PA13 + 1] = {NAME = "USX_CLK", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PA13[#PA13 + 1] = {NAME = "USX_CS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PA13[#PA13 + 1] = {NAME = "USX_CTS", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PA13[#PA13 + 1] = {NAME = "USX_RTS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PA13[#PA13 + 1] = {NAME = "LEUX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PA13[#PA13 + 1] = {NAME = "LEUX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PA13[#PA13 + 1] = {NAME = "I2CX_SCL", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PA13[#PA13 + 1] = {NAME = "I2CX_SDA", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PA13[#PA13 + 1] = {NAME = "ADCX", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}

    this:AddWidget("Textbox", "PA13")
    this:AddProposals(PA13, "A", 13)
end
--*/
#define __GPIO_PA_PIN_13_NAME__ PA13_NC

/*--
if this:PinExist("A", 13) then
    this:AddWidget("Combobox")
    this:AddPinMode("A", 13)
end
--*/
#define __GPIO_PA_PIN_13_MODE__ _GPIO_MODE_DISABLED

/*--
if this:PinExist("A", 13) then
    this:AddWidget("Combobox")
    this:AddPinState("A", 13)
end
--*/
#define __GPIO_PA_PIN_13_STATE__ _LOW

/*--
if this:PinExist("A", 14) then
    local PA14 = {}
    PA14[#PA14 + 1] = {NAME = "PA14_NC", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}
    PA14[#PA14 + 1] = {NAME = "USX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PA14[#PA14 + 1] = {NAME = "USX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PA14[#PA14 + 1] = {NAME = "USX_CLK", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PA14[#PA14 + 1] = {NAME = "USX_CS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PA14[#PA14 + 1] = {NAME = "USX_CTS", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PA14[#PA14 + 1] = {NAME = "USX_RTS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PA14[#PA14 + 1] = {NAME = "LEUX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PA14[#PA14 + 1] = {NAME = "LEUX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PA14[#PA14 + 1] = {NAME = "I2CX_SCL", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PA14[#PA14 + 1] = {NAME = "I2CX_SDA", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PA14[#PA14 + 1] = {NAME = "ADCX", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}

    this:AddWidget("Textbox", "PA14")
    this:AddProposals(PA14, "A", 14)
end
--*/
#define __GPIO_PA_PIN_14_NAME__ PA14_NC

/*--
if this:PinExist("A", 14) then
    this:AddWidget("Combobox")
    this:AddPinMode("A", 14)
end
--*/
#define __GPIO_PA_PIN_14_MODE__ _GPIO_MODE_DISABLED

/*--
if this:PinExist("A", 14) then
    this:AddWidget("Combobox")
    this:AddPinState("A", 14)
end
--*/
#define __GPIO_PA_PIN_14_STATE__ _LOW

/*--
if this:PinExist("A", 15) then
    local PA15 = {}
    PA15[#PA15 + 1] = {NAME = "PA15_NC", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}
    PA15[#PA15 + 1] = {NAME = "USX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PA15[#PA15 + 1] = {NAME = "USX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PA15[#PA15 + 1] = {NAME = "USX_CLK", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PA15[#PA15 + 1] = {NAME = "USX_CS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PA15[#PA15 + 1] = {NAME = "USX_CTS", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PA15[#PA15 + 1] = {NAME = "USX_RTS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PA15[#PA15 + 1] = {NAME = "LEUX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PA15[#PA15 + 1] = {NAME = "LEUX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PA15[#PA15 + 1] = {NAME = "I2CX_SCL", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PA15[#PA15 + 1] = {NAME = "I2CX_SDA", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PA15[#PA15 + 1] = {NAME = "ADCX", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}

    this:AddWidget("Textbox", "PA15")
    this:AddProposals(PA15, "A", 15)
end
--*/
#define __GPIO_PA_PIN_15_NAME__ PA15_NC

/*--
if this:PinExist("A", 15) then
    this:AddWidget("Combobox")
    this:AddPinMode("A", 15)
end
--*/
#define __GPIO_PA_PIN_15_MODE__ _GPIO_MODE_DISABLED

/*--
if this:PinExist("A", 15) then
    this:AddWidget("Combobox")
    this:AddPinState("A", 15)
end
--*/
#define __GPIO_PA_PIN_15_STATE__ _LOW

/*----------------------------------------------------------------------------*/
/*--
if this:PinExist("B", 0) then
    local PB0 = {}
    PB0[#PB0 + 1] = {NAME = "PB0_NC", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}
    PB0[#PB0 + 1] = {NAME = "USX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PB0[#PB0 + 1] = {NAME = "USX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PB0[#PB0 + 1] = {NAME = "USX_CLK", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PB0[#PB0 + 1] = {NAME = "USX_CS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PB0[#PB0 + 1] = {NAME = "USX_CTS", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PB0[#PB0 + 1] = {NAME = "USX_RTS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PB0[#PB0 + 1] = {NAME = "LEUX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PB0[#PB0 + 1] = {NAME = "LEUX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PB0[#PB0 + 1] = {NAME = "I2CX_SCL", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PB0[#PB0 + 1] = {NAME = "I2CX_SDA", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PB0[#PB0 + 1] = {NAME = "ADCX", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}

    this:AddWidget("Textbox", "PB0")
    this:AddProposals(PB0, "B", 0)
end
--*/
#define __GPIO_PB_PIN_0_NAME__ PB0_NC

/*--
if this:PinExist("B", 0) then
    this:AddWidget("Combobox")
    this:AddPinMode("B", 0)
end
--*/
#define __GPIO_PB_PIN_0_MODE__ _GPIO_MODE_DISABLED

/*--
if this:PinExist("B", 0) then
    this:AddWidget("Combobox")
    this:AddPinState("B", 0)
end
--*/
#define __GPIO_PB_PIN_0_STATE__ _LOW

/*--
if this:PinExist("B", 1) then
    local PB1 = {}
    PB1[#PB1 + 1] = {NAME = "PB1_NC", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}
    PB1[#PB1 + 1] = {NAME = "USX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PB1[#PB1 + 1] = {NAME = "USX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PB1[#PB1 + 1] = {NAME = "USX_CLK", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PB1[#PB1 + 1] = {NAME = "USX_CS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PB1[#PB1 + 1] = {NAME = "USX_CTS", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PB1[#PB1 + 1] = {NAME = "USX_RTS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PB1[#PB1 + 1] = {NAME = "LEUX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PB1[#PB1 + 1] = {NAME = "LEUX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PB1[#PB1 + 1] = {NAME = "I2CX_SCL", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PB1[#PB1 + 1] = {NAME = "I2CX_SDA", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PB1[#PB1 + 1] = {NAME = "ADCX", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}

    this:AddWidget("Textbox", "PB1")
    this:AddProposals(PB1, "B", 1)
end
--*/
#define __GPIO_PB_PIN_1_NAME__ PB1_NC

/*--
if this:PinExist("B", 1) then
    this:AddWidget("Combobox")
    this:AddPinMode("B", 1)
end
--*/
#define __GPIO_PB_PIN_1_MODE__ _GPIO_MODE_DISABLED

/*--
if this:PinExist("B", 1) then
    this:AddWidget("Combobox")
    this:AddPinState("B", 1)
end
--*/
#define __GPIO_PB_PIN_1_STATE__ _LOW

/*--
if this:PinExist("B", 2) then
    local PB2 = {}
    PB2[#PB2 + 1] = {NAME = "PB2_NC", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}
    PB2[#PB2 + 1] = {NAME = "USX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PB2[#PB2 + 1] = {NAME = "USX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PB2[#PB2 + 1] = {NAME = "USX_CLK", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PB2[#PB2 + 1] = {NAME = "USX_CS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PB2[#PB2 + 1] = {NAME = "USX_CTS", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PB2[#PB2 + 1] = {NAME = "USX_RTS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PB2[#PB2 + 1] = {NAME = "LEUX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PB2[#PB2 + 1] = {NAME = "LEUX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PB2[#PB2 + 1] = {NAME = "I2CX_SCL", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PB2[#PB2 + 1] = {NAME = "I2CX_SDA", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PB2[#PB2 + 1] = {NAME = "ADCX", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}

    this:AddWidget("Textbox", "PB2")
    this:AddProposals(PB2, "B", 2)
end
--*/
#define __GPIO_PB_PIN_2_NAME__ PB2_NC

/*--
if this:PinExist("B", 2) then
    this:AddWidget("Combobox")
    this:AddPinMode("B", 2)
end
--*/
#define __GPIO_PB_PIN_2_MODE__ _GPIO_MODE_DISABLED

/*--
if this:PinExist("B", 2) then
    this:AddWidget("Combobox")
    this:AddPinState("B", 2)
end
--*/
#define __GPIO_PB_PIN_2_STATE__ _LOW

/*--
if this:PinExist("B", 3) then
    local PB3 = {}
    PB3[#PB3 + 1] = {NAME = "PB3_NC", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}
    PB3[#PB3 + 1] = {NAME = "USX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PB3[#PB3 + 1] = {NAME = "USX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PB3[#PB3 + 1] = {NAME = "USX_CLK", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PB3[#PB3 + 1] = {NAME = "USX_CS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PB3[#PB3 + 1] = {NAME = "USX_CTS", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PB3[#PB3 + 1] = {NAME = "USX_RTS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PB3[#PB3 + 1] = {NAME = "LEUX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PB3[#PB3 + 1] = {NAME = "LEUX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PB3[#PB3 + 1] = {NAME = "I2CX_SCL", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PB3[#PB3 + 1] = {NAME = "I2CX_SDA", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PB3[#PB3 + 1] = {NAME = "ADCX", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}

    this:AddWidget("Textbox", "PB3")
    this:AddProposals(PB3, "B", 3)
end
--*/
#define __GPIO_PB_PIN_3_NAME__ PB3_NC

/*--
if this:PinExist("B", 3) then
    this:AddWidget("Combobox")
    this:AddPinMode("B", 3)
end
--*/
#define __GPIO_PB_PIN_3_MODE__ _GPIO_MODE_DISABLED

/*--
if this:PinExist("B", 3) then
    this:AddWidget("Combobox")
    this:AddPinState("B", 3)
end
--*/
#define __GPIO_PB_PIN_3_STATE__ _LOW

/*--
if this:PinExist("B", 4) then
    local PB4 = {}
    PB4[#PB4 + 1] = {NAME = "PB4_NC", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}
    PB4[#PB4 + 1] = {NAME = "USX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PB4[#PB4 + 1] = {NAME = "USX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PB4[#PB4 + 1] = {NAME = "USX_CLK", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PB4[#PB4 + 1] = {NAME = "USX_CS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PB4[#PB4 + 1] = {NAME = "USX_CTS", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PB4[#PB4 + 1] = {NAME = "USX_RTS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PB4[#PB4 + 1] = {NAME = "LEUX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PB4[#PB4 + 1] = {NAME = "LEUX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PB4[#PB4 + 1] = {NAME = "I2CX_SCL", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PB4[#PB4 + 1] = {NAME = "I2CX_SDA", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PB4[#PB4 + 1] = {NAME = "ADCX", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}

    this:AddWidget("Textbox", "PB4")
    this:AddProposals(PB4, "B", 4)
end
--*/
#define __GPIO_PB_PIN_4_NAME__ PB4_NC

/*--
if this:PinExist("B", 4) then
    this:AddWidget("Combobox")
    this:AddPinMode("B", 4)
end
--*/
#define __GPIO_PB_PIN_4_MODE__ _GPIO_MODE_DISABLED

/*--
if this:PinExist("B", 4) then
    this:AddWidget("Combobox")
    this:AddPinState("B", 4)
end
--*/
#define __GPIO_PB_PIN_4_STATE__ _LOW

/*--
if this:PinExist("B", 5) then
    local PB5 = {}
    PB5[#PB5 + 1] = {NAME = "PB5_NC", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}
    PB5[#PB5 + 1] = {NAME = "USX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PB5[#PB5 + 1] = {NAME = "USX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PB5[#PB5 + 1] = {NAME = "USX_CLK", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PB5[#PB5 + 1] = {NAME = "USX_CS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PB5[#PB5 + 1] = {NAME = "USX_CTS", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PB5[#PB5 + 1] = {NAME = "USX_RTS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PB5[#PB5 + 1] = {NAME = "LEUX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PB5[#PB5 + 1] = {NAME = "LEUX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PB5[#PB5 + 1] = {NAME = "I2CX_SCL", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PB5[#PB5 + 1] = {NAME = "I2CX_SDA", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PB5[#PB5 + 1] = {NAME = "ADCX", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}

    this:AddWidget("Textbox", "PB5")
    this:AddProposals(PB5, "B", 5)
end
--*/
#define __GPIO_PB_PIN_5_NAME__ PB5_NC

/*--
if this:PinExist("B", 5) then
    this:AddWidget("Combobox")
    this:AddPinMode("B", 5)
end
--*/
#define __GPIO_PB_PIN_5_MODE__ _GPIO_MODE_DISABLED

/*--
if this:PinExist("B", 5) then
    this:AddWidget("Combobox")
    this:AddPinState("B", 5)
end
--*/
#define __GPIO_PB_PIN_5_STATE__ _LOW

/*--
if this:PinExist("B", 6) then
    local PB6 = {}
    PB6[#PB6 + 1] = {NAME = "PB6_NC", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}
    PB6[#PB6 + 1] = {NAME = "USX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PB6[#PB6 + 1] = {NAME = "USX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PB6[#PB6 + 1] = {NAME = "USX_CLK", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PB6[#PB6 + 1] = {NAME = "USX_CS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PB6[#PB6 + 1] = {NAME = "USX_CTS", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PB6[#PB6 + 1] = {NAME = "USX_RTS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PB6[#PB6 + 1] = {NAME = "LEUX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PB6[#PB6 + 1] = {NAME = "LEUX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PB6[#PB6 + 1] = {NAME = "I2CX_SCL", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PB6[#PB6 + 1] = {NAME = "I2CX_SDA", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PB6[#PB6 + 1] = {NAME = "ADCX", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}

    this:AddWidget("Textbox", "PB6")
    this:AddProposals(PB6, "B", 6)
end
--*/
#define __GPIO_PB_PIN_6_NAME__ PB6_NC

/*--
if this:PinExist("B", 6) then
    this:AddWidget("Combobox")
    this:AddPinMode("B", 6)
end
--*/
#define __GPIO_PB_PIN_6_MODE__ _GPIO_MODE_DISABLED

/*--
if this:PinExist("B", 6) then
    this:AddWidget("Combobox")
    this:AddPinState("B", 6)
end
--*/
#define __GPIO_PB_PIN_6_STATE__ _LOW

/*--
if this:PinExist("B", 7) then
    local PB7 = {}
    PB7[#PB7 + 1] = {NAME = "PB7_NC", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}
    PB7[#PB7 + 1] = {NAME = "USX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PB7[#PB7 + 1] = {NAME = "USX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PB7[#PB7 + 1] = {NAME = "USX_CLK", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PB7[#PB7 + 1] = {NAME = "USX_CS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PB7[#PB7 + 1] = {NAME = "USX_CTS", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PB7[#PB7 + 1] = {NAME = "USX_RTS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PB7[#PB7 + 1] = {NAME = "LEUX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PB7[#PB7 + 1] = {NAME = "LEUX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PB7[#PB7 + 1] = {NAME = "I2CX_SCL", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PB7[#PB7 + 1] = {NAME = "I2CX_SDA", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PB7[#PB7 + 1] = {NAME = "ADCX", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}

    this:AddWidget("Textbox", "PB7")
    this:AddProposals(PB7, "B", 7)
end
--*/
#define __GPIO_PB_PIN_7_NAME__ PB7_NC

/*--
if this:PinExist("B", 7) then
    this:AddWidget("Combobox")
    this:AddPinMode("B", 7)
end
--*/
#define __GPIO_PB_PIN_7_MODE__ _GPIO_MODE_DISABLED

/*--
if this:PinExist("B", 7) then
    this:AddWidget("Combobox")
    this:AddPinState("B", 7)
end
--*/
#define __GPIO_PB_PIN_7_STATE__ _LOW

/*--
if this:PinExist("B", 8) then
    local PB8 = {}
    PB8[#PB8 + 1] = {NAME = "PB8_NC", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}
    PB8[#PB8 + 1] = {NAME = "USX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PB8[#PB8 + 1] = {NAME = "USX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PB8[#PB8 + 1] = {NAME = "USX_CLK", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PB8[#PB8 + 1] = {NAME = "USX_CS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PB8[#PB8 + 1] = {NAME = "USX_CTS", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PB8[#PB8 + 1] = {NAME = "USX_RTS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PB8[#PB8 + 1] = {NAME = "LEUX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PB8[#PB8 + 1] = {NAME = "LEUX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PB8[#PB8 + 1] = {NAME = "I2CX_SCL", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PB8[#PB8 + 1] = {NAME = "I2CX_SDA", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PB8[#PB8 + 1] = {NAME = "ADCX", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}

    this:AddWidget("Textbox", "PB8")
    this:AddProposals(PB8, "B", 8)
end
--*/
#define __GPIO_PB_PIN_8_NAME__ PB8_NC

/*--
if this:PinExist("B", 8) then
    this:AddWidget("Combobox")
    this:AddPinMode("B", 8)
end
--*/
#define __GPIO_PB_PIN_8_MODE__ _GPIO_MODE_DISABLED

/*--
if this:PinExist("B", 8) then
    this:AddWidget("Combobox")
    this:AddPinState("B", 8)
end
--*/
#define __GPIO_PB_PIN_8_STATE__ _LOW

/*--
if this:PinExist("B", 9) then
    local PB9 = {}
    PB9[#PB9 + 1] = {NAME = "PB9_NC", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}
    PB9[#PB9 + 1] = {NAME = "USX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PB9[#PB9 + 1] = {NAME = "USX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PB9[#PB9 + 1] = {NAME = "USX_CLK", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PB9[#PB9 + 1] = {NAME = "USX_CS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PB9[#PB9 + 1] = {NAME = "USX_CTS", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PB9[#PB9 + 1] = {NAME = "USX_RTS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PB9[#PB9 + 1] = {NAME = "LEUX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PB9[#PB9 + 1] = {NAME = "LEUX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PB9[#PB9 + 1] = {NAME = "I2CX_SCL", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PB9[#PB9 + 1] = {NAME = "I2CX_SDA", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PB9[#PB9 + 1] = {NAME = "ADCX", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}

    this:AddWidget("Textbox", "PB9")
    this:AddProposals(PB9, "B", 9)
end
--*/
#define __GPIO_PB_PIN_9_NAME__ PB9_NC

/*--
if this:PinExist("B", 9) then
    this:AddWidget("Combobox")
    this:AddPinMode("B", 9)
end
--*/
#define __GPIO_PB_PIN_9_MODE__ _GPIO_MODE_DISABLED

/*--
if this:PinExist("B", 9) then
    this:AddWidget("Combobox")
    this:AddPinState("B", 9)
end
--*/
#define __GPIO_PB_PIN_9_STATE__ _LOW

/*--
if this:PinExist("B", 10) then
    local PB10 = {}
    PB10[#PB10 + 1] = {NAME = "PB10_NC", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}
    PB10[#PB10 + 1] = {NAME = "USX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PB10[#PB10 + 1] = {NAME = "USX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PB10[#PB10 + 1] = {NAME = "USX_CLK", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PB10[#PB10 + 1] = {NAME = "USX_CS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PB10[#PB10 + 1] = {NAME = "USX_CTS", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PB10[#PB10 + 1] = {NAME = "USX_RTS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PB10[#PB10 + 1] = {NAME = "LEUX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PB10[#PB10 + 1] = {NAME = "LEUX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PB10[#PB10 + 1] = {NAME = "I2CX_SCL", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PB10[#PB10 + 1] = {NAME = "I2CX_SDA", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PB10[#PB10 + 1] = {NAME = "ADCX", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}

    this:AddWidget("Textbox", "PB10")
    this:AddProposals(PB10, "B", 10)
end
--*/
#define __GPIO_PB_PIN_10_NAME__ PB10_NC

/*--
if this:PinExist("B", 10) then
    this:AddWidget("Combobox")
    this:AddPinMode("B", 10)
end
--*/
#define __GPIO_PB_PIN_10_MODE__ _GPIO_MODE_DISABLED

/*--
if this:PinExist("B", 10) then
    this:AddWidget("Combobox")
    this:AddPinState("B", 10)
end
--*/
#define __GPIO_PB_PIN_10_STATE__ _LOW

/*--
if this:PinExist("B", 11) then
    local PB11 = {}
    PB11[#PB11 + 1] = {NAME = "PB11_NC", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}
    PB11[#PB11 + 1] = {NAME = "USX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PB11[#PB11 + 1] = {NAME = "USX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PB11[#PB11 + 1] = {NAME = "USX_CLK", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PB11[#PB11 + 1] = {NAME = "USX_CS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PB11[#PB11 + 1] = {NAME = "USX_CTS", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PB11[#PB11 + 1] = {NAME = "USX_RTS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PB11[#PB11 + 1] = {NAME = "LEUX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PB11[#PB11 + 1] = {NAME = "LEUX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PB11[#PB11 + 1] = {NAME = "I2CX_SCL", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PB11[#PB11 + 1] = {NAME = "I2CX_SDA", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PB11[#PB11 + 1] = {NAME = "ADCX", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}

    this:AddWidget("Textbox", "PB11")
    this:AddProposals(PB11, "B", 11)
end
--*/
#define __GPIO_PB_PIN_11_NAME__ PB11_NC

/*--
if this:PinExist("B", 11) then
    this:AddWidget("Combobox")
    this:AddPinMode("B", 11)
end
--*/
#define __GPIO_PB_PIN_11_MODE__ _GPIO_MODE_DISABLED

/*--
if this:PinExist("B", 11) then
    this:AddWidget("Combobox")
    this:AddPinState("B", 11)
end
--*/
#define __GPIO_PB_PIN_11_STATE__ _LOW

/*--
if this:PinExist("B", 12) then
    local PB12 = {}
    PB12[#PB12 + 1] = {NAME = "PB12_NC", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}
    PB12[#PB12 + 1] = {NAME = "USX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PB12[#PB12 + 1] = {NAME = "USX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PB12[#PB12 + 1] = {NAME = "USX_CLK", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PB12[#PB12 + 1] = {NAME = "USX_CS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PB12[#PB12 + 1] = {NAME = "USX_CTS", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PB12[#PB12 + 1] = {NAME = "USX_RTS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PB12[#PB12 + 1] = {NAME = "LEUX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PB12[#PB12 + 1] = {NAME = "LEUX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PB12[#PB12 + 1] = {NAME = "I2CX_SCL", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PB12[#PB12 + 1] = {NAME = "I2CX_SDA", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PB12[#PB12 + 1] = {NAME = "ADCX", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}

    this:AddWidget("Textbox", "PB12")
    this:AddProposals(PB12, "B", 12)
end
--*/
#define __GPIO_PB_PIN_12_NAME__ PB12_NC

/*--
if this:PinExist("B", 12) then
    this:AddWidget("Combobox")
    this:AddPinMode("B", 12)
end
--*/
#define __GPIO_PB_PIN_12_MODE__ _GPIO_MODE_DISABLED

/*--
if this:PinExist("B", 12) then
    this:AddWidget("Combobox")
    this:AddPinState("B", 12)
end
--*/
#define __GPIO_PB_PIN_12_STATE__ _LOW

/*--
if this:PinExist("B", 13) then
    local PB13 = {}
    PB13[#PB13 + 1] = {NAME = "PB13_NC", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}
    PB13[#PB13 + 1] = {NAME = "USX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PB13[#PB13 + 1] = {NAME = "USX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PB13[#PB13 + 1] = {NAME = "USX_CLK", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PB13[#PB13 + 1] = {NAME = "USX_CS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PB13[#PB13 + 1] = {NAME = "USX_CTS", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PB13[#PB13 + 1] = {NAME = "USX_RTS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PB13[#PB13 + 1] = {NAME = "LEUX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PB13[#PB13 + 1] = {NAME = "LEUX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PB13[#PB13 + 1] = {NAME = "I2CX_SCL", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PB13[#PB13 + 1] = {NAME = "I2CX_SDA", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PB13[#PB13 + 1] = {NAME = "ADCX", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}

    this:AddWidget("Textbox", "PB13")
    this:AddProposals(PB13, "B", 13)
end
--*/
#define __GPIO_PB_PIN_13_NAME__ PB13_NC

/*--
if this:PinExist("B", 13) then
    this:AddWidget("Combobox")
    this:AddPinMode("B", 13)
end
--*/
#define __GPIO_PB_PIN_13_MODE__ _GPIO_MODE_DISABLED

/*--
if this:PinExist("B", 13) then
    this:AddWidget("Combobox")
    this:AddPinState("B", 13)
end
--*/
#define __GPIO_PB_PIN_13_STATE__ _LOW

/*--
if this:PinExist("B", 14) then
    local PB14 = {}
    PB14[#PB14 + 1] = {NAME = "PB14_NC", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}
    PB14[#PB14 + 1] = {NAME = "USX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PB14[#PB14 + 1] = {NAME = "USX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PB14[#PB14 + 1] = {NAME = "USX_CLK", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PB14[#PB14 + 1] = {NAME = "USX_CS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PB14[#PB14 + 1] = {NAME = "USX_CTS", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PB14[#PB14 + 1] = {NAME = "USX_RTS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PB14[#PB14 + 1] = {NAME = "LEUX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PB14[#PB14 + 1] = {NAME = "LEUX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PB14[#PB14 + 1] = {NAME = "I2CX_SCL", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PB14[#PB14 + 1] = {NAME = "I2CX_SDA", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PB14[#PB14 + 1] = {NAME = "ADCX", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}

    this:AddWidget("Textbox", "PB14")
    this:AddProposals(PB14, "B", 14)
end
--*/
#define __GPIO_PB_PIN_14_NAME__ PB14_NC

/*--
if this:PinExist("B", 14) then
    this:AddWidget("Combobox")
    this:AddPinMode("B", 14)
end
--*/
#define __GPIO_PB_PIN_14_MODE__ _GPIO_MODE_DISABLED

/*--
if this:PinExist("B", 14) then
    this:AddWidget("Combobox")
    this:AddPinState("B", 14)
end
--*/
#define __GPIO_PB_PIN_14_STATE__ _LOW

/*--
if this:PinExist("B", 15) then
    local PB15 = {}
    PB15[#PB15 + 1] = {NAME = "PB15_NC", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}
    PB15[#PB15 + 1] = {NAME = "USX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PB15[#PB15 + 1] = {NAME = "USX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PB15[#PB15 + 1] = {NAME = "USX_CLK", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PB15[#PB15 + 1] = {NAME = "USX_CS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PB15[#PB15 + 1] = {NAME = "USX_CTS", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PB15[#PB15 + 1] = {NAME = "USX_RTS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PB15[#PB15 + 1] = {NAME = "LEUX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PB15[#PB15 + 1] = {NAME = "LEUX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PB15[#PB15 + 1] = {NAME = "I2CX_SCL", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PB15[#PB15 + 1] = {NAME = "I2CX_SDA", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PB15[#PB15 + 1] = {NAME = "ADCX", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}

    this:AddWidget("Textbox", "PB15")
    this:AddProposals(PB15, "B", 15)
end
--*/
#define __GPIO_PB_PIN_15_NAME__ PB15_NC

/*--
if this:PinExist("B", 15) then
    this:AddWidget("Combobox")
    this:AddPinMode("B", 15)
end
--*/
#define __GPIO_PB_PIN_15_MODE__ _GPIO_MODE_DISABLED

/*--
if this:PinExist("B", 15) then
    this:AddWidget("Combobox")
    this:AddPinState("B", 15)
end
--*/
#define __GPIO_PB_PIN_15_STATE__ _LOW

/*----------------------------------------------------------------------------*/
/*--
if this:PinExist("C", 0) then
    local PC0 = {}
    PC0[#PC0 + 1] = {NAME = "PC0_NC", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}
    PC0[#PC0 + 1] = {NAME = "USX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PC0[#PC0 + 1] = {NAME = "USX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PC0[#PC0 + 1] = {NAME = "USX_CLK", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PC0[#PC0 + 1] = {NAME = "USX_CS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PC0[#PC0 + 1] = {NAME = "USX_CTS", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PC0[#PC0 + 1] = {NAME = "USX_RTS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PC0[#PC0 + 1] = {NAME = "LEUX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PC0[#PC0 + 1] = {NAME = "LEUX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PC0[#PC0 + 1] = {NAME = "I2CX_SCL", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PC0[#PC0 + 1] = {NAME = "I2CX_SDA", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PC0[#PC0 + 1] = {NAME = "ADCX", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}

    this:AddWidget("Textbox", "PC0")
    this:AddProposals(PC0, "C", 0)
end
--*/
#define __GPIO_PC_PIN_0_NAME__ PC0_NC

/*--
if this:PinExist("C", 0) then
    this:AddWidget("Combobox")
    this:AddPinMode("C", 0)
end
--*/
#define __GPIO_PC_PIN_0_MODE__ _GPIO_MODE_DISABLED

/*--
if this:PinExist("C", 0) then
    this:AddWidget("Combobox")
    this:AddPinState("C", 0)
end
--*/
#define __GPIO_PC_PIN_0_STATE__ _LOW

/*--
if this:PinExist("C", 1) then
    local PC1 = {}
    PC1[#PC1 + 1] = {NAME = "PC1_NC", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}
    PC1[#PC1 + 1] = {NAME = "USX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PC1[#PC1 + 1] = {NAME = "USX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PC1[#PC1 + 1] = {NAME = "USX_CLK", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PC1[#PC1 + 1] = {NAME = "USX_CS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PC1[#PC1 + 1] = {NAME = "USX_CTS", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PC1[#PC1 + 1] = {NAME = "USX_RTS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PC1[#PC1 + 1] = {NAME = "LEUX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PC1[#PC1 + 1] = {NAME = "LEUX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PC1[#PC1 + 1] = {NAME = "I2CX_SCL", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PC1[#PC1 + 1] = {NAME = "I2CX_SDA", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PC1[#PC1 + 1] = {NAME = "ADCX", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}

    this:AddWidget("Textbox", "PC1")
    this:AddProposals(PC1, "C", 1)
end
--*/
#define __GPIO_PC_PIN_1_NAME__ PC1_NC

/*--
if this:PinExist("C", 1) then
    this:AddWidget("Combobox")
    this:AddPinMode("C", 1)
end
--*/
#define __GPIO_PC_PIN_1_MODE__ _GPIO_MODE_DISABLED

/*--
if this:PinExist("C", 1) then
    this:AddWidget("Combobox")
    this:AddPinState("C", 1)
end
--*/
#define __GPIO_PC_PIN_1_STATE__ _LOW

/*--
if this:PinExist("C", 2) then
    local PC2 = {}
    PC2[#PC2 + 1] = {NAME = "PC2_NC", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}
    PC2[#PC2 + 1] = {NAME = "USX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PC2[#PC2 + 1] = {NAME = "USX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PC2[#PC2 + 1] = {NAME = "USX_CLK", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PC2[#PC2 + 1] = {NAME = "USX_CS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PC2[#PC2 + 1] = {NAME = "USX_CTS", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PC2[#PC2 + 1] = {NAME = "USX_RTS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PC2[#PC2 + 1] = {NAME = "LEUX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PC2[#PC2 + 1] = {NAME = "LEUX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PC2[#PC2 + 1] = {NAME = "I2CX_SCL", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PC2[#PC2 + 1] = {NAME = "I2CX_SDA", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PC2[#PC2 + 1] = {NAME = "ADCX", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}

    this:AddWidget("Textbox", "PC2")
    this:AddProposals(PC2, "C", 2)
end
--*/
#define __GPIO_PC_PIN_2_NAME__ PC2_NC

/*--
if this:PinExist("C", 2) then
    this:AddWidget("Combobox")
    this:AddPinMode("C", 2)
end
--*/
#define __GPIO_PC_PIN_2_MODE__ _GPIO_MODE_DISABLED

/*--
if this:PinExist("C", 2) then
    this:AddWidget("Combobox")
    this:AddPinState("C", 2)
end
--*/
#define __GPIO_PC_PIN_2_STATE__ _LOW

/*--
if this:PinExist("C", 3) then
    local PC3 = {}
    PC3[#PC3 + 1] = {NAME = "PC3_NC", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}
    PC3[#PC3 + 1] = {NAME = "USX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PC3[#PC3 + 1] = {NAME = "USX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PC3[#PC3 + 1] = {NAME = "USX_CLK", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PC3[#PC3 + 1] = {NAME = "USX_CS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PC3[#PC3 + 1] = {NAME = "USX_CTS", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PC3[#PC3 + 1] = {NAME = "USX_RTS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PC3[#PC3 + 1] = {NAME = "LEUX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PC3[#PC3 + 1] = {NAME = "LEUX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PC3[#PC3 + 1] = {NAME = "I2CX_SCL", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PC3[#PC3 + 1] = {NAME = "I2CX_SDA", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PC3[#PC3 + 1] = {NAME = "ADCX", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}

    this:AddWidget("Textbox", "PC3")
    this:AddProposals(PC3, "C", 3)
end
--*/
#define __GPIO_PC_PIN_3_NAME__ PC3_NC

/*--
if this:PinExist("C", 3) then
    this:AddWidget("Combobox")
    this:AddPinMode("C", 3)
end
--*/
#define __GPIO_PC_PIN_3_MODE__ _GPIO_MODE_DISABLED

/*--
if this:PinExist("C", 3) then
    this:AddWidget("Combobox")
    this:AddPinState("C", 3)
end
--*/
#define __GPIO_PC_PIN_3_STATE__ _LOW

/*--
if this:PinExist("C", 4) then
    local PC4 = {}
    PC4[#PC4 + 1] = {NAME = "PC4_NC", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}
    PC4[#PC4 + 1] = {NAME = "USX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PC4[#PC4 + 1] = {NAME = "USX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PC4[#PC4 + 1] = {NAME = "USX_CLK", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PC4[#PC4 + 1] = {NAME = "USX_CS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PC4[#PC4 + 1] = {NAME = "USX_CTS", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PC4[#PC4 + 1] = {NAME = "USX_RTS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PC4[#PC4 + 1] = {NAME = "LEUX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PC4[#PC4 + 1] = {NAME = "LEUX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PC4[#PC4 + 1] = {NAME = "I2CX_SCL", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PC4[#PC4 + 1] = {NAME = "I2CX_SDA", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PC4[#PC4 + 1] = {NAME = "ADCX", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}

    this:AddWidget("Textbox", "PC4")
    this:AddProposals(PC4, "C", 4)
end
--*/
#define __GPIO_PC_PIN_4_NAME__ PC4_NC

/*--
if this:PinExist("C", 4) then
    this:AddWidget("Combobox")
    this:AddPinMode("C", 4)
end
--*/
#define __GPIO_PC_PIN_4_MODE__ _GPIO_MODE_DISABLED

/*--
if this:PinExist("C", 4) then
    this:AddWidget("Combobox")
    this:AddPinState("C", 4)
end
--*/
#define __GPIO_PC_PIN_4_STATE__ _LOW

/*--
if this:PinExist("C", 5) then
    local PC5 = {}
    PC5[#PC5 + 1] = {NAME = "PC5_NC", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}
    PC5[#PC5 + 1] = {NAME = "USX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PC5[#PC5 + 1] = {NAME = "USX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PC5[#PC5 + 1] = {NAME = "USX_CLK", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PC5[#PC5 + 1] = {NAME = "USX_CS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PC5[#PC5 + 1] = {NAME = "USX_CTS", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PC5[#PC5 + 1] = {NAME = "USX_RTS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PC5[#PC5 + 1] = {NAME = "LEUX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PC5[#PC5 + 1] = {NAME = "LEUX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PC5[#PC5 + 1] = {NAME = "I2CX_SCL", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PC5[#PC5 + 1] = {NAME = "I2CX_SDA", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PC5[#PC5 + 1] = {NAME = "ADCX", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}

    this:AddWidget("Textbox", "PC5")
    this:AddProposals(PC5, "C", 5)
end
--*/
#define __GPIO_PC_PIN_5_NAME__ PC5_NC

/*--
if this:PinExist("C", 5) then
    this:AddWidget("Combobox")
    this:AddPinMode("C", 5)
end
--*/
#define __GPIO_PC_PIN_5_MODE__ _GPIO_MODE_DISABLED

/*--
if this:PinExist("C", 5) then
    this:AddWidget("Combobox")
    this:AddPinState("C", 5)
end
--*/
#define __GPIO_PC_PIN_5_STATE__ _LOW

/*--
if this:PinExist("C", 6) then
    local PC6 = {}
    PC6[#PC6 + 1] = {NAME = "PC6_NC", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}
    PC6[#PC6 + 1] = {NAME = "USX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PC6[#PC6 + 1] = {NAME = "USX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PC6[#PC6 + 1] = {NAME = "USX_CLK", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PC6[#PC6 + 1] = {NAME = "USX_CS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PC6[#PC6 + 1] = {NAME = "USX_CTS", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PC6[#PC6 + 1] = {NAME = "USX_RTS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PC6[#PC6 + 1] = {NAME = "LEUX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PC6[#PC6 + 1] = {NAME = "LEUX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PC6[#PC6 + 1] = {NAME = "I2CX_SCL", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PC6[#PC6 + 1] = {NAME = "I2CX_SDA", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PC6[#PC6 + 1] = {NAME = "ADCX", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}

    this:AddWidget("Textbox", "PC6")
    this:AddProposals(PC6, "C", 6)
end
--*/
#define __GPIO_PC_PIN_6_NAME__ PC6_NC

/*--
if this:PinExist("C", 6) then
    this:AddWidget("Combobox")
    this:AddPinMode("C", 6)
end
--*/
#define __GPIO_PC_PIN_6_MODE__ _GPIO_MODE_DISABLED

/*--
if this:PinExist("C", 6) then
    this:AddWidget("Combobox")
    this:AddPinState("C", 6)
end
--*/
#define __GPIO_PC_PIN_6_STATE__ _LOW

/*--
if this:PinExist("C", 7) then
    local PC7 = {}
    PC7[#PC7 + 1] = {NAME = "PC7_NC", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}
    PC7[#PC7 + 1] = {NAME = "USX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PC7[#PC7 + 1] = {NAME = "USX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PC7[#PC7 + 1] = {NAME = "USX_CLK", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PC7[#PC7 + 1] = {NAME = "USX_CS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PC7[#PC7 + 1] = {NAME = "USX_CTS", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PC7[#PC7 + 1] = {NAME = "USX_RTS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PC7[#PC7 + 1] = {NAME = "LEUX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PC7[#PC7 + 1] = {NAME = "LEUX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PC7[#PC7 + 1] = {NAME = "I2CX_SCL", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PC7[#PC7 + 1] = {NAME = "I2CX_SDA", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PC7[#PC7 + 1] = {NAME = "ADCX", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}

    this:AddWidget("Textbox", "PC7")
    this:AddProposals(PC7, "C", 7)
end
--*/
#define __GPIO_PC_PIN_7_NAME__ PC7_NC

/*--
if this:PinExist("C", 7) then
    this:AddWidget("Combobox")
    this:AddPinMode("C", 7)
end
--*/
#define __GPIO_PC_PIN_7_MODE__ _GPIO_MODE_DISABLED

/*--
if this:PinExist("C", 7) then
    this:AddWidget("Combobox")
    this:AddPinState("C", 7)
end
--*/
#define __GPIO_PC_PIN_7_STATE__ _LOW

/*--
if this:PinExist("C", 8) then
    local PC8 = {}
    PC8[#PC8 + 1] = {NAME = "PC8_NC", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}
    PC8[#PC8 + 1] = {NAME = "USX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PC8[#PC8 + 1] = {NAME = "USX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PC8[#PC8 + 1] = {NAME = "USX_CLK", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PC8[#PC8 + 1] = {NAME = "USX_CS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PC8[#PC8 + 1] = {NAME = "USX_CTS", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PC8[#PC8 + 1] = {NAME = "USX_RTS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PC8[#PC8 + 1] = {NAME = "LEUX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PC8[#PC8 + 1] = {NAME = "LEUX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PC8[#PC8 + 1] = {NAME = "I2CX_SCL", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PC8[#PC8 + 1] = {NAME = "I2CX_SDA", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PC8[#PC8 + 1] = {NAME = "ADCX", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}

    this:AddWidget("Textbox", "PC8")
    this:AddProposals(PC8, "C", 8)
end
--*/
#define __GPIO_PC_PIN_8_NAME__ PC8_NC

/*--
if this:PinExist("C", 8) then
    this:AddWidget("Combobox")
    this:AddPinMode("C", 8)
end
--*/
#define __GPIO_PC_PIN_8_MODE__ _GPIO_MODE_DISABLED

/*--
if this:PinExist("C", 8) then
    this:AddWidget("Combobox")
    this:AddPinState("C", 8)
end
--*/
#define __GPIO_PC_PIN_8_STATE__ _LOW

/*--
if this:PinExist("C", 9) then
    local PC9 = {}
    PC9[#PC9 + 1] = {NAME = "PC9_NC", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}
    PC9[#PC9 + 1] = {NAME = "USX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PC9[#PC9 + 1] = {NAME = "USX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PC9[#PC9 + 1] = {NAME = "USX_CLK", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PC9[#PC9 + 1] = {NAME = "USX_CS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PC9[#PC9 + 1] = {NAME = "USX_CTS", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PC9[#PC9 + 1] = {NAME = "USX_RTS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PC9[#PC9 + 1] = {NAME = "LEUX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PC9[#PC9 + 1] = {NAME = "LEUX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PC9[#PC9 + 1] = {NAME = "I2CX_SCL", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PC9[#PC9 + 1] = {NAME = "I2CX_SDA", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PC9[#PC9 + 1] = {NAME = "ADCX", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}

    this:AddWidget("Textbox", "PC9")
    this:AddProposals(PC9, "C", 9)
end
--*/
#define __GPIO_PC_PIN_9_NAME__ PC9_NC

/*--
if this:PinExist("C", 9) then
    this:AddWidget("Combobox")
    this:AddPinMode("C", 9)
end
--*/
#define __GPIO_PC_PIN_9_MODE__ _GPIO_MODE_DISABLED

/*--
if this:PinExist("C", 9) then
    this:AddWidget("Combobox")
    this:AddPinState("C", 9)
end
--*/
#define __GPIO_PC_PIN_9_STATE__ _LOW

/*--
if this:PinExist("C", 10) then
    local PC10 = {}
    PC10[#PC10 + 1] = {NAME = "PC10_NC", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}
    PC10[#PC10 + 1] = {NAME = "USX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PC10[#PC10 + 1] = {NAME = "USX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PC10[#PC10 + 1] = {NAME = "USX_CLK", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PC10[#PC10 + 1] = {NAME = "USX_CS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PC10[#PC10 + 1] = {NAME = "USX_CTS", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PC10[#PC10 + 1] = {NAME = "USX_RTS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PC10[#PC10 + 1] = {NAME = "LEUX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PC10[#PC10 + 1] = {NAME = "LEUX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PC10[#PC10 + 1] = {NAME = "I2CX_SCL", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PC10[#PC10 + 1] = {NAME = "I2CX_SDA", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PC10[#PC10 + 1] = {NAME = "ADCX", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}

    this:AddWidget("Textbox", "PC10")
    this:AddProposals(PC10, "C", 10)
end
--*/
#define __GPIO_PC_PIN_10_NAME__ PC10_NC

/*--
if this:PinExist("C", 10) then
    this:AddWidget("Combobox")
    this:AddPinMode("C", 10)
end
--*/
#define __GPIO_PC_PIN_10_MODE__ _GPIO_MODE_DISABLED

/*--
if this:PinExist("C", 10) then
    this:AddWidget("Combobox")
    this:AddPinState("C", 10)
end
--*/
#define __GPIO_PC_PIN_10_STATE__ _LOW

/*--
if this:PinExist("C", 11) then
    local PC11 = {}
    PC11[#PC11 + 1] = {NAME = "PC11_NC", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}
    PC11[#PC11 + 1] = {NAME = "USX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PC11[#PC11 + 1] = {NAME = "USX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PC11[#PC11 + 1] = {NAME = "USX_CLK", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PC11[#PC11 + 1] = {NAME = "USX_CS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PC11[#PC11 + 1] = {NAME = "USX_CTS", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PC11[#PC11 + 1] = {NAME = "USX_RTS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PC11[#PC11 + 1] = {NAME = "LEUX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PC11[#PC11 + 1] = {NAME = "LEUX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PC11[#PC11 + 1] = {NAME = "I2CX_SCL", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PC11[#PC11 + 1] = {NAME = "I2CX_SDA", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PC11[#PC11 + 1] = {NAME = "ADCX", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}

    this:AddWidget("Textbox", "PC11")
    this:AddProposals(PC11, "C", 11)
end
--*/
#define __GPIO_PC_PIN_11_NAME__ PC11_NC

/*--
if this:PinExist("C", 11) then
    this:AddWidget("Combobox")
    this:AddPinMode("C", 11)
end
--*/
#define __GPIO_PC_PIN_11_MODE__ _GPIO_MODE_DISABLED

/*--
if this:PinExist("C", 11) then
    this:AddWidget("Combobox")
    this:AddPinState("C", 11)
end
--*/
#define __GPIO_PC_PIN_11_STATE__ _LOW

/*--
if this:PinExist("C", 12) then
    local PC12 = {}
    PC12[#PC12 + 1] = {NAME = "PC12_NC", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}
    PC12[#PC12 + 1] = {NAME = "USX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PC12[#PC12 + 1] = {NAME = "USX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PC12[#PC12 + 1] = {NAME = "USX_CLK", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PC12[#PC12 + 1] = {NAME = "USX_CS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PC12[#PC12 + 1] = {NAME = "USX_CTS", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PC12[#PC12 + 1] = {NAME = "USX_RTS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PC12[#PC12 + 1] = {NAME = "LEUX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PC12[#PC12 + 1] = {NAME = "LEUX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PC12[#PC12 + 1] = {NAME = "I2CX_SCL", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PC12[#PC12 + 1] = {NAME = "I2CX_SDA", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PC12[#PC12 + 1] = {NAME = "ADCX", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}

    this:AddWidget("Textbox", "PC12")
    this:AddProposals(PC12, "C", 12)
end
--*/
#define __GPIO_PC_PIN_12_NAME__ PC12_NC

/*--
if this:PinExist("C", 12) then
    this:AddWidget("Combobox")
    this:AddPinMode("C", 12)
end
--*/
#define __GPIO_PC_PIN_12_MODE__ _GPIO_MODE_DISABLED

/*--
if this:PinExist("C", 12) then
    this:AddWidget("Combobox")
    this:AddPinState("C", 12)
end
--*/
#define __GPIO_PC_PIN_12_STATE__ _LOW

/*--
if this:PinExist("C", 13) then
    local PC13 = {}
    PC13[#PC13 + 1] = {NAME = "PC13_NC", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}
    PC13[#PC13 + 1] = {NAME = "USX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PC13[#PC13 + 1] = {NAME = "USX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PC13[#PC13 + 1] = {NAME = "USX_CLK", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PC13[#PC13 + 1] = {NAME = "USX_CS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PC13[#PC13 + 1] = {NAME = "USX_CTS", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PC13[#PC13 + 1] = {NAME = "USX_RTS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PC13[#PC13 + 1] = {NAME = "LEUX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PC13[#PC13 + 1] = {NAME = "LEUX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PC13[#PC13 + 1] = {NAME = "I2CX_SCL", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PC13[#PC13 + 1] = {NAME = "I2CX_SDA", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PC13[#PC13 + 1] = {NAME = "ADCX", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}

    this:AddWidget("Textbox", "PC13")
    this:AddProposals(PC13, "C", 13)
end
--*/
#define __GPIO_PC_PIN_13_NAME__ PC13_NC

/*--
if this:PinExist("C", 13) then
    this:AddWidget("Combobox")
    this:AddPinMode("C", 13)
end
--*/
#define __GPIO_PC_PIN_13_MODE__ _GPIO_MODE_DISABLED

/*--
if this:PinExist("C", 13) then
    this:AddWidget("Combobox")
    this:AddPinState("C", 13)
end
--*/
#define __GPIO_PC_PIN_13_STATE__ _LOW

/*--
if this:PinExist("C", 14) then
    local PC14 = {}
    PC14[#PC14 + 1] = {NAME = "PC14_NC", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}
    PC14[#PC14 + 1] = {NAME = "USX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PC14[#PC14 + 1] = {NAME = "USX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PC14[#PC14 + 1] = {NAME = "USX_CLK", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PC14[#PC14 + 1] = {NAME = "USX_CS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PC14[#PC14 + 1] = {NAME = "USX_CTS", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PC14[#PC14 + 1] = {NAME = "USX_RTS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PC14[#PC14 + 1] = {NAME = "LEUX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PC14[#PC14 + 1] = {NAME = "LEUX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PC14[#PC14 + 1] = {NAME = "I2CX_SCL", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PC14[#PC14 + 1] = {NAME = "I2CX_SDA", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PC14[#PC14 + 1] = {NAME = "ADCX", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}

    this:AddWidget("Textbox", "PC14")
    this:AddProposals(PC14, "C", 14)
end
--*/
#define __GPIO_PC_PIN_14_NAME__ PC14_NC

/*--
if this:PinExist("C", 14) then
    this:AddWidget("Combobox")
    this:AddPinMode("C", 14)
end
--*/
#define __GPIO_PC_PIN_14_MODE__ _GPIO_MODE_DISABLED

/*--
if this:PinExist("C", 14) then
    this:AddWidget("Combobox")
    this:AddPinState("C", 14)
end
--*/
#define __GPIO_PC_PIN_14_STATE__ _LOW

/*--
if this:PinExist("C", 15) then
    local PC15 = {}
    PC15[#PC15 + 1] = {NAME = "PC15_NC", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}
    PC15[#PC15 + 1] = {NAME = "USX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PC15[#PC15 + 1] = {NAME = "USX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PC15[#PC15 + 1] = {NAME = "USX_CLK", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PC15[#PC15 + 1] = {NAME = "USX_CS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PC15[#PC15 + 1] = {NAME = "USX_CTS", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PC15[#PC15 + 1] = {NAME = "USX_RTS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PC15[#PC15 + 1] = {NAME = "LEUX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PC15[#PC15 + 1] = {NAME = "LEUX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PC15[#PC15 + 1] = {NAME = "I2CX_SCL", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PC15[#PC15 + 1] = {NAME = "I2CX_SDA", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PC15[#PC15 + 1] = {NAME = "ADCX", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}

    this:AddWidget("Textbox", "PC15")
    this:AddProposals(PC15, "C", 15)
end
--*/
#define __GPIO_PC_PIN_15_NAME__ PC15_NC

/*--
if this:PinExist("C", 15) then
    this:AddWidget("Combobox")
    this:AddPinMode("C", 15)
end
--*/
#define __GPIO_PC_PIN_15_MODE__ _GPIO_MODE_DISABLED

/*--
if this:PinExist("C", 15) then
    this:AddWidget("Combobox")
    this:AddPinState("C", 15)
end
--*/
#define __GPIO_PC_PIN_15_STATE__ _LOW

/*----------------------------------------------------------------------------*/
/*--
if this:PinExist("D", 0) then
    local PD0 = {}
    PD0[#PD0 + 1] = {NAME = "PD0_NC", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}
    PD0[#PD0 + 1] = {NAME = "USX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PD0[#PD0 + 1] = {NAME = "USX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PD0[#PD0 + 1] = {NAME = "USX_CLK", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PD0[#PD0 + 1] = {NAME = "USX_CS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PD0[#PD0 + 1] = {NAME = "USX_CTS", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PD0[#PD0 + 1] = {NAME = "USX_RTS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PD0[#PD0 + 1] = {NAME = "LEUX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PD0[#PD0 + 1] = {NAME = "LEUX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PD0[#PD0 + 1] = {NAME = "I2CX_SCL", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PD0[#PD0 + 1] = {NAME = "I2CX_SDA", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PD0[#PD0 + 1] = {NAME = "ADCX", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}

    this:AddWidget("Textbox", "PD0")
    this:AddProposals(PD0, "D", 0)
end
--*/
#define __GPIO_PD_PIN_0_NAME__ PD0_NC

/*--
if this:PinExist("D", 0) then
    this:AddWidget("Combobox")
    this:AddPinMode("D", 0)
end
--*/
#define __GPIO_PD_PIN_0_MODE__ _GPIO_MODE_DISABLED

/*--
if this:PinExist("D", 0) then
    this:AddWidget("Combobox")
    this:AddPinState("D", 0)
end
--*/
#define __GPIO_PD_PIN_0_STATE__ _LOW

/*--
if this:PinExist("D", 1) then
    local PD1 = {}
    PD1[#PD1 + 1] = {NAME = "PD1_NC", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}
    PD1[#PD1 + 1] = {NAME = "USX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PD1[#PD1 + 1] = {NAME = "USX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PD1[#PD1 + 1] = {NAME = "USX_CLK", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PD1[#PD1 + 1] = {NAME = "USX_CS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PD1[#PD1 + 1] = {NAME = "USX_CTS", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PD1[#PD1 + 1] = {NAME = "USX_RTS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PD1[#PD1 + 1] = {NAME = "LEUX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PD1[#PD1 + 1] = {NAME = "LEUX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PD1[#PD1 + 1] = {NAME = "I2CX_SCL", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PD1[#PD1 + 1] = {NAME = "I2CX_SDA", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PD1[#PD1 + 1] = {NAME = "ADCX", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}

    this:AddWidget("Textbox", "PD1")
    this:AddProposals(PD1, "D", 1)
end
--*/
#define __GPIO_PD_PIN_1_NAME__ PD1_NC

/*--
if this:PinExist("D", 1) then
    this:AddWidget("Combobox")
    this:AddPinMode("D", 1)
end
--*/
#define __GPIO_PD_PIN_1_MODE__ _GPIO_MODE_DISABLED

/*--
if this:PinExist("D", 1) then
    this:AddWidget("Combobox")
    this:AddPinState("D", 1)
end
--*/
#define __GPIO_PD_PIN_1_STATE__ _LOW

/*--
if this:PinExist("D", 2) then
    local PD2 = {}
    PD2[#PD2 + 1] = {NAME = "PD2_NC", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}
    PD2[#PD2 + 1] = {NAME = "USX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PD2[#PD2 + 1] = {NAME = "USX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PD2[#PD2 + 1] = {NAME = "USX_CLK", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PD2[#PD2 + 1] = {NAME = "USX_CS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PD2[#PD2 + 1] = {NAME = "USX_CTS", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PD2[#PD2 + 1] = {NAME = "USX_RTS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PD2[#PD2 + 1] = {NAME = "LEUX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PD2[#PD2 + 1] = {NAME = "LEUX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PD2[#PD2 + 1] = {NAME = "I2CX_SCL", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PD2[#PD2 + 1] = {NAME = "I2CX_SDA", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PD2[#PD2 + 1] = {NAME = "ADCX", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}

    this:AddWidget("Textbox", "PD2")
    this:AddProposals(PD2, "D", 2)
end
--*/
#define __GPIO_PD_PIN_2_NAME__ PD2_NC

/*--
if this:PinExist("D", 2) then
    this:AddWidget("Combobox")
    this:AddPinMode("D", 2)
end
--*/
#define __GPIO_PD_PIN_2_MODE__ _GPIO_MODE_DISABLED

/*--
if this:PinExist("D", 2) then
    this:AddWidget("Combobox")
    this:AddPinState("D", 2)
end
--*/
#define __GPIO_PD_PIN_2_STATE__ _LOW

/*--
if this:PinExist("D", 3) then
    local PD3 = {}
    PD3[#PD3 + 1] = {NAME = "PD3_NC", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}
    PD3[#PD3 + 1] = {NAME = "USX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PD3[#PD3 + 1] = {NAME = "USX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PD3[#PD3 + 1] = {NAME = "USX_CLK", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PD3[#PD3 + 1] = {NAME = "USX_CS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PD3[#PD3 + 1] = {NAME = "USX_CTS", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PD3[#PD3 + 1] = {NAME = "USX_RTS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PD3[#PD3 + 1] = {NAME = "LEUX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PD3[#PD3 + 1] = {NAME = "LEUX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PD3[#PD3 + 1] = {NAME = "I2CX_SCL", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PD3[#PD3 + 1] = {NAME = "I2CX_SDA", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PD3[#PD3 + 1] = {NAME = "ADCX", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}

    this:AddWidget("Textbox", "PD3")
    this:AddProposals(PD3, "D", 3)
end
--*/
#define __GPIO_PD_PIN_3_NAME__ PD3_NC

/*--
if this:PinExist("D", 3) then
    this:AddWidget("Combobox")
    this:AddPinMode("D", 3)
end
--*/
#define __GPIO_PD_PIN_3_MODE__ _GPIO_MODE_DISABLED

/*--
if this:PinExist("D", 3) then
    this:AddWidget("Combobox")
    this:AddPinState("D", 3)
end
--*/
#define __GPIO_PD_PIN_3_STATE__ _LOW

/*--
if this:PinExist("D", 4) then
    local PD4 = {}
    PD4[#PD4 + 1] = {NAME = "PD4_NC", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}
    PD4[#PD4 + 1] = {NAME = "USX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PD4[#PD4 + 1] = {NAME = "USX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PD4[#PD4 + 1] = {NAME = "USX_CLK", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PD4[#PD4 + 1] = {NAME = "USX_CS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PD4[#PD4 + 1] = {NAME = "USX_CTS", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PD4[#PD4 + 1] = {NAME = "USX_RTS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PD4[#PD4 + 1] = {NAME = "LEUX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PD4[#PD4 + 1] = {NAME = "LEUX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PD4[#PD4 + 1] = {NAME = "I2CX_SCL", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PD4[#PD4 + 1] = {NAME = "I2CX_SDA", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PD4[#PD4 + 1] = {NAME = "ADCX", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}

    this:AddWidget("Textbox", "PD4")
    this:AddProposals(PD4, "D", 4)
end
--*/
#define __GPIO_PD_PIN_4_NAME__ PD4_NC

/*--
if this:PinExist("D", 4) then
    this:AddWidget("Combobox")
    this:AddPinMode("D", 4)
end
--*/
#define __GPIO_PD_PIN_4_MODE__ _GPIO_MODE_DISABLED

/*--
if this:PinExist("D", 4) then
    this:AddWidget("Combobox")
    this:AddPinState("D", 4)
end
--*/
#define __GPIO_PD_PIN_4_STATE__ _LOW

/*--
if this:PinExist("D", 5) then
    local PD5 = {}
    PD5[#PD5 + 1] = {NAME = "PD5_NC", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}
    PD5[#PD5 + 1] = {NAME = "USX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PD5[#PD5 + 1] = {NAME = "USX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PD5[#PD5 + 1] = {NAME = "USX_CLK", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PD5[#PD5 + 1] = {NAME = "USX_CS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PD5[#PD5 + 1] = {NAME = "USX_CTS", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PD5[#PD5 + 1] = {NAME = "USX_RTS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PD5[#PD5 + 1] = {NAME = "LEUX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PD5[#PD5 + 1] = {NAME = "LEUX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PD5[#PD5 + 1] = {NAME = "I2CX_SCL", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PD5[#PD5 + 1] = {NAME = "I2CX_SDA", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PD5[#PD5 + 1] = {NAME = "ADCX", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}

    this:AddWidget("Textbox", "PD5")
    this:AddProposals(PD5, "D", 5)
end
--*/
#define __GPIO_PD_PIN_5_NAME__ PD5_NC

/*--
if this:PinExist("D", 5) then
    this:AddWidget("Combobox")
    this:AddPinMode("D", 5)
end
--*/
#define __GPIO_PD_PIN_5_MODE__ _GPIO_MODE_DISABLED

/*--
if this:PinExist("D", 5) then
    this:AddWidget("Combobox")
    this:AddPinState("D", 5)
end
--*/
#define __GPIO_PD_PIN_5_STATE__ _LOW

/*--
if this:PinExist("D", 6) then
    local PD6 = {}
    PD6[#PD6 + 1] = {NAME = "PD6_NC", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}
    PD6[#PD6 + 1] = {NAME = "USX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PD6[#PD6 + 1] = {NAME = "USX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PD6[#PD6 + 1] = {NAME = "USX_CLK", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PD6[#PD6 + 1] = {NAME = "USX_CS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PD6[#PD6 + 1] = {NAME = "USX_CTS", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PD6[#PD6 + 1] = {NAME = "USX_RTS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PD6[#PD6 + 1] = {NAME = "LEUX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PD6[#PD6 + 1] = {NAME = "LEUX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PD6[#PD6 + 1] = {NAME = "I2CX_SCL", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PD6[#PD6 + 1] = {NAME = "I2CX_SDA", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PD6[#PD6 + 1] = {NAME = "ADCX", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}

    this:AddWidget("Textbox", "PD6")
    this:AddProposals(PD6, "D", 6)
end
--*/
#define __GPIO_PD_PIN_6_NAME__ PD6_NC

/*--
if this:PinExist("D", 6) then
    this:AddWidget("Combobox")
    this:AddPinMode("D", 6)
end
--*/
#define __GPIO_PD_PIN_6_MODE__ _GPIO_MODE_DISABLED

/*--
if this:PinExist("D", 6) then
    this:AddWidget("Combobox")
    this:AddPinState("D", 6)
end
--*/
#define __GPIO_PD_PIN_6_STATE__ _LOW

/*--
if this:PinExist("D", 7) then
    local PD7 = {}
    PD7[#PD7 + 1] = {NAME = "PD7_NC", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}
    PD7[#PD7 + 1] = {NAME = "USX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PD7[#PD7 + 1] = {NAME = "USX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PD7[#PD7 + 1] = {NAME = "USX_CLK", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PD7[#PD7 + 1] = {NAME = "USX_CS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PD7[#PD7 + 1] = {NAME = "USX_CTS", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PD7[#PD7 + 1] = {NAME = "USX_RTS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PD7[#PD7 + 1] = {NAME = "LEUX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PD7[#PD7 + 1] = {NAME = "LEUX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PD7[#PD7 + 1] = {NAME = "I2CX_SCL", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PD7[#PD7 + 1] = {NAME = "I2CX_SDA", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PD7[#PD7 + 1] = {NAME = "ADCX", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}

    this:AddWidget("Textbox", "PD7")
    this:AddProposals(PD7, "D", 7)
end
--*/
#define __GPIO_PD_PIN_7_NAME__ PD7_NC

/*--
if this:PinExist("D", 7) then
    this:AddWidget("Combobox")
    this:AddPinMode("D", 7)
end
--*/
#define __GPIO_PD_PIN_7_MODE__ _GPIO_MODE_DISABLED

/*--
if this:PinExist("D", 7) then
    this:AddWidget("Combobox")
    this:AddPinState("D", 7)
end
--*/
#define __GPIO_PD_PIN_7_STATE__ _LOW

/*--
if this:PinExist("D", 8) then
    local PD8 = {}
    PD8[#PD8 + 1] = {NAME = "PD8_NC", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}
    PD8[#PD8 + 1] = {NAME = "USX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PD8[#PD8 + 1] = {NAME = "USX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PD8[#PD8 + 1] = {NAME = "USX_CLK", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PD8[#PD8 + 1] = {NAME = "USX_CS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PD8[#PD8 + 1] = {NAME = "USX_CTS", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PD8[#PD8 + 1] = {NAME = "USX_RTS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PD8[#PD8 + 1] = {NAME = "LEUX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PD8[#PD8 + 1] = {NAME = "LEUX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PD8[#PD8 + 1] = {NAME = "I2CX_SCL", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PD8[#PD8 + 1] = {NAME = "I2CX_SDA", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PD8[#PD8 + 1] = {NAME = "ADCX", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}

    this:AddWidget("Textbox", "PD8")
    this:AddProposals(PD8, "D", 8)
end
--*/
#define __GPIO_PD_PIN_8_NAME__ PD8_NC

/*--
if this:PinExist("D", 8) then
    this:AddWidget("Combobox")
    this:AddPinMode("D", 8)
end
--*/
#define __GPIO_PD_PIN_8_MODE__ _GPIO_MODE_DISABLED

/*--
if this:PinExist("D", 8) then
    this:AddWidget("Combobox")
    this:AddPinState("D", 8)
end
--*/
#define __GPIO_PD_PIN_8_STATE__ _LOW

/*--
if this:PinExist("D", 9) then
    local PD9 = {}
    PD9[#PD9 + 1] = {NAME = "PD9_NC", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}
    PD9[#PD9 + 1] = {NAME = "USX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PD9[#PD9 + 1] = {NAME = "USX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PD9[#PD9 + 1] = {NAME = "USX_CLK", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PD9[#PD9 + 1] = {NAME = "USX_CS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PD9[#PD9 + 1] = {NAME = "USX_CTS", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PD9[#PD9 + 1] = {NAME = "USX_RTS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PD9[#PD9 + 1] = {NAME = "LEUX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PD9[#PD9 + 1] = {NAME = "LEUX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PD9[#PD9 + 1] = {NAME = "I2CX_SCL", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PD9[#PD9 + 1] = {NAME = "I2CX_SDA", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PD9[#PD9 + 1] = {NAME = "ADCX", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}

    this:AddWidget("Textbox", "PD9")
    this:AddProposals(PD9, "D", 9)
end
--*/
#define __GPIO_PD_PIN_9_NAME__ PD9_NC

/*--
if this:PinExist("D", 9) then
    this:AddWidget("Combobox")
    this:AddPinMode("D", 9)
end
--*/
#define __GPIO_PD_PIN_9_MODE__ _GPIO_MODE_DISABLED

/*--
if this:PinExist("D", 9) then
    this:AddWidget("Combobox")
    this:AddPinState("D", 9)
end
--*/
#define __GPIO_PD_PIN_9_STATE__ _LOW

/*--
if this:PinExist("D", 10) then
    local PD10 = {}
    PD10[#PD10 + 1] = {NAME = "PD10_NC", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}
    PD10[#PD10 + 1] = {NAME = "USX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PD10[#PD10 + 1] = {NAME = "USX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PD10[#PD10 + 1] = {NAME = "USX_CLK", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PD10[#PD10 + 1] = {NAME = "USX_CS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PD10[#PD10 + 1] = {NAME = "USX_CTS", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PD10[#PD10 + 1] = {NAME = "USX_RTS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PD10[#PD10 + 1] = {NAME = "LEUX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PD10[#PD10 + 1] = {NAME = "LEUX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PD10[#PD10 + 1] = {NAME = "I2CX_SCL", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PD10[#PD10 + 1] = {NAME = "I2CX_SDA", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PD10[#PD10 + 1] = {NAME = "ADCX", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}

    this:AddWidget("Textbox", "PD10")
    this:AddProposals(PD10, "D", 10)
end
--*/
#define __GPIO_PD_PIN_10_NAME__ PD10_NC

/*--
if this:PinExist("D", 10) then
    this:AddWidget("Combobox")
    this:AddPinMode("D", 10)
end
--*/
#define __GPIO_PD_PIN_10_MODE__ _GPIO_MODE_DISABLED

/*--
if this:PinExist("D", 10) then
    this:AddWidget("Combobox")
    this:AddPinState("D", 10)
end
--*/
#define __GPIO_PD_PIN_10_STATE__ _LOW

/*--
if this:PinExist("D", 11) then
    local PD11 = {}
    PD11[#PD11 + 1] = {NAME = "PD11_NC", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}
    PD11[#PD11 + 1] = {NAME = "USX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PD11[#PD11 + 1] = {NAME = "USX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PD11[#PD11 + 1] = {NAME = "USX_CLK", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PD11[#PD11 + 1] = {NAME = "USX_CS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PD11[#PD11 + 1] = {NAME = "USX_CTS", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PD11[#PD11 + 1] = {NAME = "USX_RTS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PD11[#PD11 + 1] = {NAME = "LEUX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PD11[#PD11 + 1] = {NAME = "LEUX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PD11[#PD11 + 1] = {NAME = "I2CX_SCL", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PD11[#PD11 + 1] = {NAME = "I2CX_SDA", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PD11[#PD11 + 1] = {NAME = "ADCX", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}

    this:AddWidget("Textbox", "PD11")
    this:AddProposals(PD11, "D", 11)
end
--*/
#define __GPIO_PD_PIN_11_NAME__ PD11_NC

/*--
if this:PinExist("D", 11) then
    this:AddWidget("Combobox")
    this:AddPinMode("D", 11)
end
--*/
#define __GPIO_PD_PIN_11_MODE__ _GPIO_MODE_DISABLED

/*--
if this:PinExist("D", 11) then
    this:AddWidget("Combobox")
    this:AddPinState("D", 11)
end
--*/
#define __GPIO_PD_PIN_11_STATE__ _LOW

/*--
if this:PinExist("D", 12) then
    local PD12 = {}
    PD12[#PD12 + 1] = {NAME = "PD12_NC", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}
    PD12[#PD12 + 1] = {NAME = "USX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PD12[#PD12 + 1] = {NAME = "USX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PD12[#PD12 + 1] = {NAME = "USX_CLK", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PD12[#PD12 + 1] = {NAME = "USX_CS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PD12[#PD12 + 1] = {NAME = "USX_CTS", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PD12[#PD12 + 1] = {NAME = "USX_RTS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PD12[#PD12 + 1] = {NAME = "LEUX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PD12[#PD12 + 1] = {NAME = "LEUX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PD12[#PD12 + 1] = {NAME = "I2CX_SCL", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PD12[#PD12 + 1] = {NAME = "I2CX_SDA", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PD12[#PD12 + 1] = {NAME = "ADCX", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}

    this:AddWidget("Textbox", "PD12")
    this:AddProposals(PD12, "D", 12)
end
--*/
#define __GPIO_PD_PIN_12_NAME__ PD12_NC

/*--
if this:PinExist("D", 12) then
    this:AddWidget("Combobox")
    this:AddPinMode("D", 12)
end
--*/
#define __GPIO_PD_PIN_12_MODE__ _GPIO_MODE_DISABLED

/*--
if this:PinExist("D", 12) then
    this:AddWidget("Combobox")
    this:AddPinState("D", 12)
end
--*/
#define __GPIO_PD_PIN_12_STATE__ _LOW

/*--
if this:PinExist("D", 13) then
    local PD13 = {}
    PD13[#PD13 + 1] = {NAME = "PD13_NC", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}
    PD13[#PD13 + 1] = {NAME = "USX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PD13[#PD13 + 1] = {NAME = "USX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PD13[#PD13 + 1] = {NAME = "USX_CLK", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PD13[#PD13 + 1] = {NAME = "USX_CS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PD13[#PD13 + 1] = {NAME = "USX_CTS", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PD13[#PD13 + 1] = {NAME = "USX_RTS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PD13[#PD13 + 1] = {NAME = "LEUX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PD13[#PD13 + 1] = {NAME = "LEUX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PD13[#PD13 + 1] = {NAME = "I2CX_SCL", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PD13[#PD13 + 1] = {NAME = "I2CX_SDA", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PD13[#PD13 + 1] = {NAME = "ADCX", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}

    this:AddWidget("Textbox", "PD13")
    this:AddProposals(PD13, "D", 13)
end
--*/
#define __GPIO_PD_PIN_13_NAME__ PD13_NC

/*--
if this:PinExist("D", 13) then
    this:AddWidget("Combobox")
    this:AddPinMode("D", 13)
end
--*/
#define __GPIO_PD_PIN_13_MODE__ _GPIO_MODE_DISABLED

/*--
if this:PinExist("D", 13) then
    this:AddWidget("Combobox")
    this:AddPinState("D", 13)
end
--*/
#define __GPIO_PD_PIN_13_STATE__ _LOW

/*--
if this:PinExist("D", 14) then
    local PD14 = {}
    PD14[#PD14 + 1] = {NAME = "PD14_NC", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}
    PD14[#PD14 + 1] = {NAME = "USX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PD14[#PD14 + 1] = {NAME = "USX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PD14[#PD14 + 1] = {NAME = "USX_CLK", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PD14[#PD14 + 1] = {NAME = "USX_CS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PD14[#PD14 + 1] = {NAME = "USX_CTS", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PD14[#PD14 + 1] = {NAME = "USX_RTS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PD14[#PD14 + 1] = {NAME = "LEUX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PD14[#PD14 + 1] = {NAME = "LEUX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PD14[#PD14 + 1] = {NAME = "I2CX_SCL", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PD14[#PD14 + 1] = {NAME = "I2CX_SDA", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PD14[#PD14 + 1] = {NAME = "ADCX", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}

    this:AddWidget("Textbox", "PD14")
    this:AddProposals(PD14, "D", 14)
end
--*/
#define __GPIO_PD_PIN_14_NAME__ PD14_NC

/*--
if this:PinExist("D", 14) then
    this:AddWidget("Combobox")
    this:AddPinMode("D", 14)
end
--*/
#define __GPIO_PD_PIN_14_MODE__ _GPIO_MODE_DISABLED

/*--
if this:PinExist("D", 14) then
    this:AddWidget("Combobox")
    this:AddPinState("D", 14)
end
--*/
#define __GPIO_PD_PIN_14_STATE__ _LOW

/*--
if this:PinExist("D", 15) then
    local PD15 = {}
    PD15[#PD15 + 1] = {NAME = "PD15_NC", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}
    PD15[#PD15 + 1] = {NAME = "USX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PD15[#PD15 + 1] = {NAME = "USX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PD15[#PD15 + 1] = {NAME = "USX_CLK", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PD15[#PD15 + 1] = {NAME = "USX_CS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PD15[#PD15 + 1] = {NAME = "USX_CTS", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PD15[#PD15 + 1] = {NAME = "USX_RTS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PD15[#PD15 + 1] = {NAME = "LEUX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PD15[#PD15 + 1] = {NAME = "LEUX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PD15[#PD15 + 1] = {NAME = "I2CX_SCL", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PD15[#PD15 + 1] = {NAME = "I2CX_SDA", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PD15[#PD15 + 1] = {NAME = "ADCX", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}

    this:AddWidget("Textbox", "PD15")
    this:AddProposals(PD15, "D", 15)
end
--*/
#define __GPIO_PD_PIN_15_NAME__ PD15_NC

/*--
if this:PinExist("D", 15) then
    this:AddWidget("Combobox")
    this:AddPinMode("D", 15)
end
--*/
#define __GPIO_PD_PIN_15_MODE__ _GPIO_MODE_DISABLED

/*--
if this:PinExist("D", 15) then
    this:AddWidget("Combobox")
    this:AddPinState("D", 15)
end
--*/
#define __GPIO_PD_PIN_15_STATE__ _LOW

/*----------------------------------------------------------------------------*/
/*--
if this:PinExist("E", 0) then
    local PE0 = {}
    PE0[#PE0 + 1] = {NAME = "PE0_NC", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}
    PE0[#PE0 + 1] = {NAME = "USX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PE0[#PE0 + 1] = {NAME = "USX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PE0[#PE0 + 1] = {NAME = "USX_CLK", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PE0[#PE0 + 1] = {NAME = "USX_CS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PE0[#PE0 + 1] = {NAME = "USX_CTS", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PE0[#PE0 + 1] = {NAME = "USX_RTS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PE0[#PE0 + 1] = {NAME = "LEUX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PE0[#PE0 + 1] = {NAME = "LEUX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PE0[#PE0 + 1] = {NAME = "I2CX_SCL", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PE0[#PE0 + 1] = {NAME = "I2CX_SDA", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PE0[#PE0 + 1] = {NAME = "ADCX", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}

    this:AddWidget("Textbox", "PE0")
    this:AddProposals(PE0, "E", 0)
end
--*/
#define __GPIO_PE_PIN_0_NAME__ PE0_NC

/*--
if this:PinExist("E", 0) then
    this:AddWidget("Combobox")
    this:AddPinMode("E", 0)
end
--*/
#define __GPIO_PE_PIN_0_MODE__ _GPIO_MODE_DISABLED

/*--
if this:PinExist("E", 0) then
    this:AddWidget("Combobox")
    this:AddPinState("E", 0)
end
--*/
#define __GPIO_PE_PIN_0_STATE__ _LOW

/*--
if this:PinExist("E", 1) then
    local PE1 = {}
    PE1[#PE1 + 1] = {NAME = "PE1_NC", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}
    PE1[#PE1 + 1] = {NAME = "USX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PE1[#PE1 + 1] = {NAME = "USX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PE1[#PE1 + 1] = {NAME = "USX_CLK", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PE1[#PE1 + 1] = {NAME = "USX_CS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PE1[#PE1 + 1] = {NAME = "USX_CTS", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PE1[#PE1 + 1] = {NAME = "USX_RTS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PE1[#PE1 + 1] = {NAME = "LEUX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PE1[#PE1 + 1] = {NAME = "LEUX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PE1[#PE1 + 1] = {NAME = "I2CX_SCL", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PE1[#PE1 + 1] = {NAME = "I2CX_SDA", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PE1[#PE1 + 1] = {NAME = "ADCX", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}

    this:AddWidget("Textbox", "PE1")
    this:AddProposals(PE1, "E", 1)
end
--*/
#define __GPIO_PE_PIN_1_NAME__ PE1_NC

/*--
if this:PinExist("E", 1) then
    this:AddWidget("Combobox")
    this:AddPinMode("E", 1)
end
--*/
#define __GPIO_PE_PIN_1_MODE__ _GPIO_MODE_DISABLED

/*--
if this:PinExist("E", 1) then
    this:AddWidget("Combobox")
    this:AddPinState("E", 1)
end
--*/
#define __GPIO_PE_PIN_1_STATE__ _LOW

/*--
if this:PinExist("E", 2) then
    local PE2 = {}
    PE2[#PE2 + 1] = {NAME = "PE2_NC", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}
    PE2[#PE2 + 1] = {NAME = "USX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PE2[#PE2 + 1] = {NAME = "USX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PE2[#PE2 + 1] = {NAME = "USX_CLK", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PE2[#PE2 + 1] = {NAME = "USX_CS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PE2[#PE2 + 1] = {NAME = "USX_CTS", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PE2[#PE2 + 1] = {NAME = "USX_RTS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PE2[#PE2 + 1] = {NAME = "LEUX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PE2[#PE2 + 1] = {NAME = "LEUX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PE2[#PE2 + 1] = {NAME = "I2CX_SCL", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PE2[#PE2 + 1] = {NAME = "I2CX_SDA", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PE2[#PE2 + 1] = {NAME = "ADCX", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}

    this:AddWidget("Textbox", "PE2")
    this:AddProposals(PE2, "E", 2)
end
--*/
#define __GPIO_PE_PIN_2_NAME__ PE2_NC

/*--
if this:PinExist("E", 2) then
    this:AddWidget("Combobox")
    this:AddPinMode("E", 2)
end
--*/
#define __GPIO_PE_PIN_2_MODE__ _GPIO_MODE_DISABLED

/*--
if this:PinExist("E", 2) then
    this:AddWidget("Combobox")
    this:AddPinState("E", 2)
end
--*/
#define __GPIO_PE_PIN_2_STATE__ _LOW

/*--
if this:PinExist("E", 3) then
    local PE3 = {}
    PE3[#PE3 + 1] = {NAME = "PE3_NC", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}
    PE3[#PE3 + 1] = {NAME = "USX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PE3[#PE3 + 1] = {NAME = "USX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PE3[#PE3 + 1] = {NAME = "USX_CLK", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PE3[#PE3 + 1] = {NAME = "USX_CS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PE3[#PE3 + 1] = {NAME = "USX_CTS", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PE3[#PE3 + 1] = {NAME = "USX_RTS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PE3[#PE3 + 1] = {NAME = "LEUX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PE3[#PE3 + 1] = {NAME = "LEUX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PE3[#PE3 + 1] = {NAME = "I2CX_SCL", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PE3[#PE3 + 1] = {NAME = "I2CX_SDA", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PE3[#PE3 + 1] = {NAME = "ADCX", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}

    this:AddWidget("Textbox", "PE3")
    this:AddProposals(PE3, "E", 3)
end
--*/
#define __GPIO_PE_PIN_3_NAME__ PE3_NC

/*--
if this:PinExist("E", 3) then
    this:AddWidget("Combobox")
    this:AddPinMode("E", 3)
end
--*/
#define __GPIO_PE_PIN_3_MODE__ _GPIO_MODE_DISABLED

/*--
if this:PinExist("E", 3) then
    this:AddWidget("Combobox")
    this:AddPinState("E", 3)
end
--*/
#define __GPIO_PE_PIN_3_STATE__ _LOW

/*--
if this:PinExist("E", 4) then
    local PE4 = {}
    PE4[#PE4 + 1] = {NAME = "PE4_NC", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}
    PE4[#PE4 + 1] = {NAME = "USX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PE4[#PE4 + 1] = {NAME = "USX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PE4[#PE4 + 1] = {NAME = "USX_CLK", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PE4[#PE4 + 1] = {NAME = "USX_CS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PE4[#PE4 + 1] = {NAME = "USX_CTS", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PE4[#PE4 + 1] = {NAME = "USX_RTS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PE4[#PE4 + 1] = {NAME = "LEUX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PE4[#PE4 + 1] = {NAME = "LEUX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PE4[#PE4 + 1] = {NAME = "I2CX_SCL", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PE4[#PE4 + 1] = {NAME = "I2CX_SDA", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PE4[#PE4 + 1] = {NAME = "ADCX", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}

    this:AddWidget("Textbox", "PE4")
    this:AddProposals(PE4, "E", 4)
end
--*/
#define __GPIO_PE_PIN_4_NAME__ PE4_NC

/*--
if this:PinExist("E", 4) then
    this:AddWidget("Combobox")
    this:AddPinMode("E", 4)
end
--*/
#define __GPIO_PE_PIN_4_MODE__ _GPIO_MODE_DISABLED

/*--
if this:PinExist("E", 4) then
    this:AddWidget("Combobox")
    this:AddPinState("E", 4)
end
--*/
#define __GPIO_PE_PIN_4_STATE__ _LOW

/*--
if this:PinExist("E", 5) then
    local PE5 = {}
    PE5[#PE5 + 1] = {NAME = "PE5_NC", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}
    PE5[#PE5 + 1] = {NAME = "USX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PE5[#PE5 + 1] = {NAME = "USX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PE5[#PE5 + 1] = {NAME = "USX_CLK", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PE5[#PE5 + 1] = {NAME = "USX_CS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PE5[#PE5 + 1] = {NAME = "USX_CTS", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PE5[#PE5 + 1] = {NAME = "USX_RTS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PE5[#PE5 + 1] = {NAME = "LEUX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PE5[#PE5 + 1] = {NAME = "LEUX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PE5[#PE5 + 1] = {NAME = "I2CX_SCL", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PE5[#PE5 + 1] = {NAME = "I2CX_SDA", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PE5[#PE5 + 1] = {NAME = "ADCX", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}

    this:AddWidget("Textbox", "PE5")
    this:AddProposals(PE5, "E", 5)
end
--*/
#define __GPIO_PE_PIN_5_NAME__ PE5_NC

/*--
if this:PinExist("E", 5) then
    this:AddWidget("Combobox")
    this:AddPinMode("E", 5)
end
--*/
#define __GPIO_PE_PIN_5_MODE__ _GPIO_MODE_DISABLED

/*--
if this:PinExist("E", 5) then
    this:AddWidget("Combobox")
    this:AddPinState("E", 5)
end
--*/
#define __GPIO_PE_PIN_5_STATE__ _LOW

/*--
if this:PinExist("E", 6) then
    local PE6 = {}
    PE6[#PE6 + 1] = {NAME = "PE6_NC", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}
    PE6[#PE6 + 1] = {NAME = "USX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PE6[#PE6 + 1] = {NAME = "USX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PE6[#PE6 + 1] = {NAME = "USX_CLK", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PE6[#PE6 + 1] = {NAME = "USX_CS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PE6[#PE6 + 1] = {NAME = "USX_CTS", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PE6[#PE6 + 1] = {NAME = "USX_RTS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PE6[#PE6 + 1] = {NAME = "LEUX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PE6[#PE6 + 1] = {NAME = "LEUX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PE6[#PE6 + 1] = {NAME = "I2CX_SCL", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PE6[#PE6 + 1] = {NAME = "I2CX_SDA", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PE6[#PE6 + 1] = {NAME = "ADCX", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}

    this:AddWidget("Textbox", "PE6")
    this:AddProposals(PE6, "E", 6)
end
--*/
#define __GPIO_PE_PIN_6_NAME__ PE6_NC

/*--
if this:PinExist("E", 6) then
    this:AddWidget("Combobox")
    this:AddPinMode("E", 6)
end
--*/
#define __GPIO_PE_PIN_6_MODE__ _GPIO_MODE_DISABLED

/*--
if this:PinExist("E", 6) then
    this:AddWidget("Combobox")
    this:AddPinState("E", 6)
end
--*/
#define __GPIO_PE_PIN_6_STATE__ _LOW

/*--
if this:PinExist("E", 7) then
    local PE7 = {}
    PE7[#PE7 + 1] = {NAME = "PE7_NC", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}
    PE7[#PE7 + 1] = {NAME = "USX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PE7[#PE7 + 1] = {NAME = "USX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PE7[#PE7 + 1] = {NAME = "USX_CLK", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PE7[#PE7 + 1] = {NAME = "USX_CS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PE7[#PE7 + 1] = {NAME = "USX_CTS", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PE7[#PE7 + 1] = {NAME = "USX_RTS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PE7[#PE7 + 1] = {NAME = "LEUX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PE7[#PE7 + 1] = {NAME = "LEUX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PE7[#PE7 + 1] = {NAME = "I2CX_SCL", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PE7[#PE7 + 1] = {NAME = "I2CX_SDA", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PE7[#PE7 + 1] = {NAME = "ADCX", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}

    this:AddWidget("Textbox", "PE7")
    this:AddProposals(PE7, "E", 7)
end
--*/
#define __GPIO_PE_PIN_7_NAME__ PE7_NC

/*--
if this:PinExist("E", 7) then
    this:AddWidget("Combobox")
    this:AddPinMode("E", 7)
end
--*/
#define __GPIO_PE_PIN_7_MODE__ _GPIO_MODE_DISABLED

/*--
if this:PinExist("E", 7) then
    this:AddWidget("Combobox")
    this:AddPinState("E", 7)
end
--*/
#define __GPIO_PE_PIN_7_STATE__ _LOW

/*--
if this:PinExist("E", 8) then
    local PE8 = {}
    PE8[#PE8 + 1] = {NAME = "PE8_NC", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}
    PE8[#PE8 + 1] = {NAME = "USX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PE8[#PE8 + 1] = {NAME = "USX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PE8[#PE8 + 1] = {NAME = "USX_CLK", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PE8[#PE8 + 1] = {NAME = "USX_CS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PE8[#PE8 + 1] = {NAME = "USX_CTS", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PE8[#PE8 + 1] = {NAME = "USX_RTS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PE8[#PE8 + 1] = {NAME = "LEUX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PE8[#PE8 + 1] = {NAME = "LEUX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PE8[#PE8 + 1] = {NAME = "I2CX_SCL", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PE8[#PE8 + 1] = {NAME = "I2CX_SDA", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PE8[#PE8 + 1] = {NAME = "ADCX", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}

    this:AddWidget("Textbox", "PE8")
    this:AddProposals(PE8, "E", 8)
end
--*/
#define __GPIO_PE_PIN_8_NAME__ PE8_NC

/*--
if this:PinExist("E", 8) then
    this:AddWidget("Combobox")
    this:AddPinMode("E", 8)
end
--*/
#define __GPIO_PE_PIN_8_MODE__ _GPIO_MODE_DISABLED

/*--
if this:PinExist("E", 8) then
    this:AddWidget("Combobox")
    this:AddPinState("E", 8)
end
--*/
#define __GPIO_PE_PIN_8_STATE__ _LOW

/*--
if this:PinExist("E", 9) then
    local PE9 = {}
    PE9[#PE9 + 1] = {NAME = "PE9_NC", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}
    PE9[#PE9 + 1] = {NAME = "USX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PE9[#PE9 + 1] = {NAME = "USX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PE9[#PE9 + 1] = {NAME = "USX_CLK", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PE9[#PE9 + 1] = {NAME = "USX_CS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PE9[#PE9 + 1] = {NAME = "USX_CTS", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PE9[#PE9 + 1] = {NAME = "USX_RTS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PE9[#PE9 + 1] = {NAME = "LEUX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PE9[#PE9 + 1] = {NAME = "LEUX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PE9[#PE9 + 1] = {NAME = "I2CX_SCL", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PE9[#PE9 + 1] = {NAME = "I2CX_SDA", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PE9[#PE9 + 1] = {NAME = "ADCX", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}

    this:AddWidget("Textbox", "PE9")
    this:AddProposals(PE9, "E", 9)
end
--*/
#define __GPIO_PE_PIN_9_NAME__ PE9_NC

/*--
if this:PinExist("E", 9) then
    this:AddWidget("Combobox")
    this:AddPinMode("E", 9)
end
--*/
#define __GPIO_PE_PIN_9_MODE__ _GPIO_MODE_DISABLED

/*--
if this:PinExist("E", 9) then
    this:AddWidget("Combobox")
    this:AddPinState("E", 9)
end
--*/
#define __GPIO_PE_PIN_9_STATE__ _LOW

/*--
if this:PinExist("E", 10) then
    local PE10 = {}
    PE10[#PE10 + 1] = {NAME = "PE10_NC", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}
    PE10[#PE10 + 1] = {NAME = "USX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PE10[#PE10 + 1] = {NAME = "USX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PE10[#PE10 + 1] = {NAME = "USX_CLK", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PE10[#PE10 + 1] = {NAME = "USX_CS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PE10[#PE10 + 1] = {NAME = "USX_CTS", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PE10[#PE10 + 1] = {NAME = "USX_RTS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PE10[#PE10 + 1] = {NAME = "LEUX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PE10[#PE10 + 1] = {NAME = "LEUX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PE10[#PE10 + 1] = {NAME = "I2CX_SCL", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PE10[#PE10 + 1] = {NAME = "I2CX_SDA", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PE10[#PE10 + 1] = {NAME = "ADCX", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}

    this:AddWidget("Textbox", "PE10")
    this:AddProposals(PE10, "E", 10)
end
--*/
#define __GPIO_PE_PIN_10_NAME__ PE10_NC

/*--
if this:PinExist("E", 10) then
    this:AddWidget("Combobox")
    this:AddPinMode("E", 10)
end
--*/
#define __GPIO_PE_PIN_10_MODE__ _GPIO_MODE_DISABLED

/*--
if this:PinExist("E", 10) then
    this:AddWidget("Combobox")
    this:AddPinState("E", 10)
end
--*/
#define __GPIO_PE_PIN_10_STATE__ _LOW

/*--
if this:PinExist("E", 11) then
    local PE11 = {}
    PE11[#PE11 + 1] = {NAME = "PE11_NC", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}
    PE11[#PE11 + 1] = {NAME = "USX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PE11[#PE11 + 1] = {NAME = "USX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PE11[#PE11 + 1] = {NAME = "USX_CLK", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PE11[#PE11 + 1] = {NAME = "USX_CS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PE11[#PE11 + 1] = {NAME = "USX_CTS", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PE11[#PE11 + 1] = {NAME = "USX_RTS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PE11[#PE11 + 1] = {NAME = "LEUX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PE11[#PE11 + 1] = {NAME = "LEUX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PE11[#PE11 + 1] = {NAME = "I2CX_SCL", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PE11[#PE11 + 1] = {NAME = "I2CX_SDA", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PE11[#PE11 + 1] = {NAME = "ADCX", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}

    this:AddWidget("Textbox", "PE11")
    this:AddProposals(PE11, "E", 11)
end
--*/
#define __GPIO_PE_PIN_11_NAME__ PE11_NC

/*--
if this:PinExist("E", 11) then
    this:AddWidget("Combobox")
    this:AddPinMode("E", 11)
end
--*/
#define __GPIO_PE_PIN_11_MODE__ _GPIO_MODE_DISABLED

/*--
if this:PinExist("E", 11) then
    this:AddWidget("Combobox")
    this:AddPinState("E", 11)
end
--*/
#define __GPIO_PE_PIN_11_STATE__ _LOW

/*--
if this:PinExist("E", 12) then
    local PE12 = {}
    PE12[#PE12 + 1] = {NAME = "PE12_NC", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}
    PE12[#PE12 + 1] = {NAME = "USX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PE12[#PE12 + 1] = {NAME = "USX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PE12[#PE12 + 1] = {NAME = "USX_CLK", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PE12[#PE12 + 1] = {NAME = "USX_CS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PE12[#PE12 + 1] = {NAME = "USX_CTS", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PE12[#PE12 + 1] = {NAME = "USX_RTS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PE12[#PE12 + 1] = {NAME = "LEUX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PE12[#PE12 + 1] = {NAME = "LEUX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PE12[#PE12 + 1] = {NAME = "I2CX_SCL", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PE12[#PE12 + 1] = {NAME = "I2CX_SDA", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PE12[#PE12 + 1] = {NAME = "ADCX", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}

    this:AddWidget("Textbox", "PE12")
    this:AddProposals(PE12, "E", 12)
end
--*/
#define __GPIO_PE_PIN_12_NAME__ PE12_NC

/*--
if this:PinExist("E", 12) then
    this:AddWidget("Combobox")
    this:AddPinMode("E", 12)
end
--*/
#define __GPIO_PE_PIN_12_MODE__ _GPIO_MODE_DISABLED

/*--
if this:PinExist("E", 12) then
    this:AddWidget("Combobox")
    this:AddPinState("E", 12)
end
--*/
#define __GPIO_PE_PIN_12_STATE__ _LOW

/*--
if this:PinExist("E", 13) then
    local PE13 = {}
    PE13[#PE13 + 1] = {NAME = "PE13_NC", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}
    PE13[#PE13 + 1] = {NAME = "USX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PE13[#PE13 + 1] = {NAME = "USX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PE13[#PE13 + 1] = {NAME = "USX_CLK", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PE13[#PE13 + 1] = {NAME = "USX_CS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PE13[#PE13 + 1] = {NAME = "USX_CTS", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PE13[#PE13 + 1] = {NAME = "USX_RTS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PE13[#PE13 + 1] = {NAME = "LEUX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PE13[#PE13 + 1] = {NAME = "LEUX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PE13[#PE13 + 1] = {NAME = "I2CX_SCL", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PE13[#PE13 + 1] = {NAME = "I2CX_SDA", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PE13[#PE13 + 1] = {NAME = "ADCX", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}

    this:AddWidget("Textbox", "PE13")
    this:AddProposals(PE13, "E", 13)
end
--*/
#define __GPIO_PE_PIN_13_NAME__ PE13_NC

/*--
if this:PinExist("E", 13) then
    this:AddWidget("Combobox")
    this:AddPinMode("E", 13)
end
--*/
#define __GPIO_PE_PIN_13_MODE__ _GPIO_MODE_DISABLED

/*--
if this:PinExist("E", 13) then
    this:AddWidget("Combobox")
    this:AddPinState("E", 13)
end
--*/
#define __GPIO_PE_PIN_13_STATE__ _LOW

/*--
if this:PinExist("E", 14) then
    local PE14 = {}
    PE14[#PE14 + 1] = {NAME = "PE14_NC", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}
    PE14[#PE14 + 1] = {NAME = "USX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PE14[#PE14 + 1] = {NAME = "USX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PE14[#PE14 + 1] = {NAME = "USX_CLK", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PE14[#PE14 + 1] = {NAME = "USX_CS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PE14[#PE14 + 1] = {NAME = "USX_CTS", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PE14[#PE14 + 1] = {NAME = "USX_RTS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PE14[#PE14 + 1] = {NAME = "LEUX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PE14[#PE14 + 1] = {NAME = "LEUX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PE14[#PE14 + 1] = {NAME = "I2CX_SCL", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PE14[#PE14 + 1] = {NAME = "I2CX_SDA", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PE14[#PE14 + 1] = {NAME = "ADCX", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}

    this:AddWidget("Textbox", "PE14")
    this:AddProposals(PE14, "E", 14)
end
--*/
#define __GPIO_PE_PIN_14_NAME__ PE14_NC

/*--
if this:PinExist("E", 14) then
    this:AddWidget("Combobox")
    this:AddPinMode("E", 14)
end
--*/
#define __GPIO_PE_PIN_14_MODE__ _GPIO_MODE_DISABLED

/*--
if this:PinExist("E", 14) then
    this:AddWidget("Combobox")
    this:AddPinState("E", 14)
end
--*/
#define __GPIO_PE_PIN_14_STATE__ _LOW

/*--
if this:PinExist("E", 15) then
    local PE15 = {}
    PE15[#PE15 + 1] = {NAME = "PE15_NC", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}
    PE15[#PE15 + 1] = {NAME = "USX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PE15[#PE15 + 1] = {NAME = "USX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PE15[#PE15 + 1] = {NAME = "USX_CLK", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PE15[#PE15 + 1] = {NAME = "USX_CS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PE15[#PE15 + 1] = {NAME = "USX_CTS", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PE15[#PE15 + 1] = {NAME = "USX_RTS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PE15[#PE15 + 1] = {NAME = "LEUX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PE15[#PE15 + 1] = {NAME = "LEUX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PE15[#PE15 + 1] = {NAME = "I2CX_SCL", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PE15[#PE15 + 1] = {NAME = "I2CX_SDA", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PE15[#PE15 + 1] = {NAME = "ADCX", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}

    this:AddWidget("Textbox", "PE15")
    this:AddProposals(PE15, "E", 15)
end
--*/
#define __GPIO_PE_PIN_15_NAME__ PE15_NC

/*--
if this:PinExist("E", 15) then
    this:AddWidget("Combobox")
    this:AddPinMode("E", 15)
end
--*/
#define __GPIO_PE_PIN_15_MODE__ _GPIO_MODE_DISABLED

/*--
if this:PinExist("E", 15) then
    this:AddWidget("Combobox")
    this:AddPinState("E", 15)
end
--*/
#define __GPIO_PE_PIN_15_STATE__ _LOW

/*----------------------------------------------------------------------------*/
/*--
if this:PinExist("F", 0) then
    local PF0 = {}
    PF0[#PF0 + 1] = {NAME = "PF0_NC", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}
    PF0[#PF0 + 1] = {NAME = "USX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PF0[#PF0 + 1] = {NAME = "USX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PF0[#PF0 + 1] = {NAME = "USX_CLK", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PF0[#PF0 + 1] = {NAME = "USX_CS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PF0[#PF0 + 1] = {NAME = "USX_CTS", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PF0[#PF0 + 1] = {NAME = "USX_RTS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PF0[#PF0 + 1] = {NAME = "LEUX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PF0[#PF0 + 1] = {NAME = "LEUX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PF0[#PF0 + 1] = {NAME = "I2CX_SCL", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PF0[#PF0 + 1] = {NAME = "I2CX_SDA", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PF0[#PF0 + 1] = {NAME = "ADCX", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}

    this:AddWidget("Textbox", "PF0")
    this:AddProposals(PF0, "F", 0)
end
--*/
#define __GPIO_PF_PIN_0_NAME__ PF0_NC

/*--
if this:PinExist("F", 0) then
    this:AddWidget("Combobox")
    this:AddPinMode("F", 0)
end
--*/
#define __GPIO_PF_PIN_0_MODE__ _GPIO_MODE_DISABLED

/*--
if this:PinExist("F", 0) then
    this:AddWidget("Combobox")
    this:AddPinState("F", 0)
end
--*/
#define __GPIO_PF_PIN_0_STATE__ _LOW

/*--
if this:PinExist("F", 1) then
    local PF1 = {}
    PF1[#PF1 + 1] = {NAME = "PF1_NC", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}
    PF1[#PF1 + 1] = {NAME = "USX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PF1[#PF1 + 1] = {NAME = "USX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PF1[#PF1 + 1] = {NAME = "USX_CLK", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PF1[#PF1 + 1] = {NAME = "USX_CS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PF1[#PF1 + 1] = {NAME = "USX_CTS", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PF1[#PF1 + 1] = {NAME = "USX_RTS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PF1[#PF1 + 1] = {NAME = "LEUX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PF1[#PF1 + 1] = {NAME = "LEUX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PF1[#PF1 + 1] = {NAME = "I2CX_SCL", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PF1[#PF1 + 1] = {NAME = "I2CX_SDA", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PF1[#PF1 + 1] = {NAME = "ADCX", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}

    this:AddWidget("Textbox", "PF1")
    this:AddProposals(PF1, "F", 1)
end
--*/
#define __GPIO_PF_PIN_1_NAME__ PF1_NC

/*--
if this:PinExist("F", 1) then
    this:AddWidget("Combobox")
    this:AddPinMode("F", 1)
end
--*/
#define __GPIO_PF_PIN_1_MODE__ _GPIO_MODE_DISABLED

/*--
if this:PinExist("F", 1) then
    this:AddWidget("Combobox")
    this:AddPinState("F", 1)
end
--*/
#define __GPIO_PF_PIN_1_STATE__ _LOW

/*--
if this:PinExist("F", 2) then
    local PF2 = {}
    PF2[#PF2 + 1] = {NAME = "PF2_NC", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}
    PF2[#PF2 + 1] = {NAME = "USX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PF2[#PF2 + 1] = {NAME = "USX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PF2[#PF2 + 1] = {NAME = "USX_CLK", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PF2[#PF2 + 1] = {NAME = "USX_CS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PF2[#PF2 + 1] = {NAME = "USX_CTS", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PF2[#PF2 + 1] = {NAME = "USX_RTS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PF2[#PF2 + 1] = {NAME = "LEUX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PF2[#PF2 + 1] = {NAME = "LEUX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PF2[#PF2 + 1] = {NAME = "I2CX_SCL", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PF2[#PF2 + 1] = {NAME = "I2CX_SDA", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PF2[#PF2 + 1] = {NAME = "ADCX", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}

    this:AddWidget("Textbox", "PF2")
    this:AddProposals(PF2, "F", 2)
end
--*/
#define __GPIO_PF_PIN_2_NAME__ PF2_NC

/*--
if this:PinExist("F", 2) then
    this:AddWidget("Combobox")
    this:AddPinMode("F", 2)
end
--*/
#define __GPIO_PF_PIN_2_MODE__ _GPIO_MODE_DISABLED

/*--
if this:PinExist("F", 2) then
    this:AddWidget("Combobox")
    this:AddPinState("F", 2)
end
--*/
#define __GPIO_PF_PIN_2_STATE__ _LOW

/*--
if this:PinExist("F", 3) then
    local PF3 = {}
    PF3[#PF3 + 1] = {NAME = "PF3_NC", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}
    PF3[#PF3 + 1] = {NAME = "USX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PF3[#PF3 + 1] = {NAME = "USX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PF3[#PF3 + 1] = {NAME = "USX_CLK", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PF3[#PF3 + 1] = {NAME = "USX_CS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PF3[#PF3 + 1] = {NAME = "USX_CTS", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PF3[#PF3 + 1] = {NAME = "USX_RTS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PF3[#PF3 + 1] = {NAME = "LEUX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PF3[#PF3 + 1] = {NAME = "LEUX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PF3[#PF3 + 1] = {NAME = "I2CX_SCL", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PF3[#PF3 + 1] = {NAME = "I2CX_SDA", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PF3[#PF3 + 1] = {NAME = "ADCX", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}

    this:AddWidget("Textbox", "PF3")
    this:AddProposals(PF3, "F", 3)
end
--*/
#define __GPIO_PF_PIN_3_NAME__ PF3_NC

/*--
if this:PinExist("F", 3) then
    this:AddWidget("Combobox")
    this:AddPinMode("F", 3)
end
--*/
#define __GPIO_PF_PIN_3_MODE__ _GPIO_MODE_DISABLED

/*--
if this:PinExist("F", 3) then
    this:AddWidget("Combobox")
    this:AddPinState("F", 3)
end
--*/
#define __GPIO_PF_PIN_3_STATE__ _LOW

/*--
if this:PinExist("F", 4) then
    local PF4 = {}
    PF4[#PF4 + 1] = {NAME = "PF4_NC", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}
    PF4[#PF4 + 1] = {NAME = "USX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PF4[#PF4 + 1] = {NAME = "USX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PF4[#PF4 + 1] = {NAME = "USX_CLK", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PF4[#PF4 + 1] = {NAME = "USX_CS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PF4[#PF4 + 1] = {NAME = "USX_CTS", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PF4[#PF4 + 1] = {NAME = "USX_RTS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PF4[#PF4 + 1] = {NAME = "LEUX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PF4[#PF4 + 1] = {NAME = "LEUX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PF4[#PF4 + 1] = {NAME = "I2CX_SCL", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PF4[#PF4 + 1] = {NAME = "I2CX_SDA", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PF4[#PF4 + 1] = {NAME = "ADCX", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}

    this:AddWidget("Textbox", "PF4")
    this:AddProposals(PF4, "F", 4)
end
--*/
#define __GPIO_PF_PIN_4_NAME__ PF4_NC

/*--
if this:PinExist("F", 4) then
    this:AddWidget("Combobox")
    this:AddPinMode("F", 4)
end
--*/
#define __GPIO_PF_PIN_4_MODE__ _GPIO_MODE_DISABLED

/*--
if this:PinExist("F", 4) then
    this:AddWidget("Combobox")
    this:AddPinState("F", 4)
end
--*/
#define __GPIO_PF_PIN_4_STATE__ _LOW

/*--
if this:PinExist("F", 5) then
    local PF5 = {}
    PF5[#PF5 + 1] = {NAME = "PF5_NC", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}
    PF5[#PF5 + 1] = {NAME = "USX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PF5[#PF5 + 1] = {NAME = "USX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PF5[#PF5 + 1] = {NAME = "USX_CLK", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PF5[#PF5 + 1] = {NAME = "USX_CS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PF5[#PF5 + 1] = {NAME = "USX_CTS", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PF5[#PF5 + 1] = {NAME = "USX_RTS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PF5[#PF5 + 1] = {NAME = "LEUX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PF5[#PF5 + 1] = {NAME = "LEUX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PF5[#PF5 + 1] = {NAME = "I2CX_SCL", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PF5[#PF5 + 1] = {NAME = "I2CX_SDA", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PF5[#PF5 + 1] = {NAME = "ADCX", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}

    this:AddWidget("Textbox", "PF5")
    this:AddProposals(PF5, "F", 5)
end
--*/
#define __GPIO_PF_PIN_5_NAME__ PF5_NC

/*--
if this:PinExist("F", 5) then
    this:AddWidget("Combobox")
    this:AddPinMode("F", 5)
end
--*/
#define __GPIO_PF_PIN_5_MODE__ _GPIO_MODE_DISABLED

/*--
if this:PinExist("F", 5) then
    this:AddWidget("Combobox")
    this:AddPinState("F", 5)
end
--*/
#define __GPIO_PF_PIN_5_STATE__ _LOW

/*--
if this:PinExist("F", 6) then
    local PF6 = {}
    PF6[#PF6 + 1] = {NAME = "PF6_NC", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}
    PF6[#PF6 + 1] = {NAME = "USX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PF6[#PF6 + 1] = {NAME = "USX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PF6[#PF6 + 1] = {NAME = "USX_CLK", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PF6[#PF6 + 1] = {NAME = "USX_CS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PF6[#PF6 + 1] = {NAME = "USX_CTS", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PF6[#PF6 + 1] = {NAME = "USX_RTS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PF6[#PF6 + 1] = {NAME = "LEUX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PF6[#PF6 + 1] = {NAME = "LEUX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PF6[#PF6 + 1] = {NAME = "I2CX_SCL", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PF6[#PF6 + 1] = {NAME = "I2CX_SDA", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PF6[#PF6 + 1] = {NAME = "ADCX", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}

    this:AddWidget("Textbox", "PF6")
    this:AddProposals(PF6, "F", 6)
end
--*/
#define __GPIO_PF_PIN_6_NAME__ PF6_NC

/*--
if this:PinExist("F", 6) then
    this:AddWidget("Combobox")
    this:AddPinMode("F", 6)
end
--*/
#define __GPIO_PF_PIN_6_MODE__ _GPIO_MODE_DISABLED

/*--
if this:PinExist("F", 6) then
    this:AddWidget("Combobox")
    this:AddPinState("F", 6)
end
--*/
#define __GPIO_PF_PIN_6_STATE__ _LOW

/*--
if this:PinExist("F", 7) then
    local PF7 = {}
    PF7[#PF7 + 1] = {NAME = "PF7_NC", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}
    PF7[#PF7 + 1] = {NAME = "USX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PF7[#PF7 + 1] = {NAME = "USX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PF7[#PF7 + 1] = {NAME = "USX_CLK", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PF7[#PF7 + 1] = {NAME = "USX_CS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PF7[#PF7 + 1] = {NAME = "USX_CTS", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PF7[#PF7 + 1] = {NAME = "USX_RTS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PF7[#PF7 + 1] = {NAME = "LEUX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PF7[#PF7 + 1] = {NAME = "LEUX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PF7[#PF7 + 1] = {NAME = "I2CX_SCL", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PF7[#PF7 + 1] = {NAME = "I2CX_SDA", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PF7[#PF7 + 1] = {NAME = "ADCX", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}

    this:AddWidget("Textbox", "PF7")
    this:AddProposals(PF7, "F", 7)
end
--*/
#define __GPIO_PF_PIN_7_NAME__ PF7_NC

/*--
if this:PinExist("F", 7) then
    this:AddWidget("Combobox")
    this:AddPinMode("F", 7)
end
--*/
#define __GPIO_PF_PIN_7_MODE__ _GPIO_MODE_DISABLED

/*--
if this:PinExist("F", 7) then
    this:AddWidget("Combobox")
    this:AddPinState("F", 7)
end
--*/
#define __GPIO_PF_PIN_7_STATE__ _LOW

/*--
if this:PinExist("F", 8) then
    local PF8 = {}
    PF8[#PF8 + 1] = {NAME = "PF8_NC", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}
    PF8[#PF8 + 1] = {NAME = "USX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PF8[#PF8 + 1] = {NAME = "USX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PF8[#PF8 + 1] = {NAME = "USX_CLK", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PF8[#PF8 + 1] = {NAME = "USX_CS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PF8[#PF8 + 1] = {NAME = "USX_CTS", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PF8[#PF8 + 1] = {NAME = "USX_RTS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PF8[#PF8 + 1] = {NAME = "LEUX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PF8[#PF8 + 1] = {NAME = "LEUX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PF8[#PF8 + 1] = {NAME = "I2CX_SCL", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PF8[#PF8 + 1] = {NAME = "I2CX_SDA", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PF8[#PF8 + 1] = {NAME = "ADCX", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}

    this:AddWidget("Textbox", "PF8")
    this:AddProposals(PF8, "F", 8)
end
--*/
#define __GPIO_PF_PIN_8_NAME__ PF8_NC

/*--
if this:PinExist("F", 8) then
    this:AddWidget("Combobox")
    this:AddPinMode("F", 8)
end
--*/
#define __GPIO_PF_PIN_8_MODE__ _GPIO_MODE_DISABLED

/*--
if this:PinExist("F", 8) then
    this:AddWidget("Combobox")
    this:AddPinState("F", 8)
end
--*/
#define __GPIO_PF_PIN_8_STATE__ _LOW

/*--
if this:PinExist("F", 9) then
    local PF9 = {}
    PF9[#PF9 + 1] = {NAME = "PF9_NC", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}
    PF9[#PF9 + 1] = {NAME = "USX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PF9[#PF9 + 1] = {NAME = "USX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PF9[#PF9 + 1] = {NAME = "USX_CLK", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PF9[#PF9 + 1] = {NAME = "USX_CS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PF9[#PF9 + 1] = {NAME = "USX_CTS", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PF9[#PF9 + 1] = {NAME = "USX_RTS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PF9[#PF9 + 1] = {NAME = "LEUX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PF9[#PF9 + 1] = {NAME = "LEUX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PF9[#PF9 + 1] = {NAME = "I2CX_SCL", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PF9[#PF9 + 1] = {NAME = "I2CX_SDA", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PF9[#PF9 + 1] = {NAME = "ADCX", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}

    this:AddWidget("Textbox", "PF9")
    this:AddProposals(PF9, "F", 9)
end
--*/
#define __GPIO_PF_PIN_9_NAME__ PF9_NC

/*--
if this:PinExist("F", 9) then
    this:AddWidget("Combobox")
    this:AddPinMode("F", 9)
end
--*/
#define __GPIO_PF_PIN_9_MODE__ _GPIO_MODE_DISABLED

/*--
if this:PinExist("F", 9) then
    this:AddWidget("Combobox")
    this:AddPinState("F", 9)
end
--*/
#define __GPIO_PF_PIN_9_STATE__ _LOW

/*--
if this:PinExist("F", 10) then
    local PF10 = {}
    PF10[#PF10 + 1] = {NAME = "PF10_NC", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}
    PF10[#PF10 + 1] = {NAME = "USX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PF10[#PF10 + 1] = {NAME = "USX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PF10[#PF10 + 1] = {NAME = "USX_CLK", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PF10[#PF10 + 1] = {NAME = "USX_CS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PF10[#PF10 + 1] = {NAME = "USX_CTS", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PF10[#PF10 + 1] = {NAME = "USX_RTS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PF10[#PF10 + 1] = {NAME = "LEUX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PF10[#PF10 + 1] = {NAME = "LEUX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PF10[#PF10 + 1] = {NAME = "I2CX_SCL", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PF10[#PF10 + 1] = {NAME = "I2CX_SDA", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PF10[#PF10 + 1] = {NAME = "ADCX", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}

    this:AddWidget("Textbox", "PF10")
    this:AddProposals(PF10, "F", 10)
end
--*/
#define __GPIO_PF_PIN_10_NAME__ PF10_NC

/*--
if this:PinExist("F", 10) then
    this:AddWidget("Combobox")
    this:AddPinMode("F", 10)
end
--*/
#define __GPIO_PF_PIN_10_MODE__ _GPIO_MODE_DISABLED

/*--
if this:PinExist("F", 10) then
    this:AddWidget("Combobox")
    this:AddPinState("F", 10)
end
--*/
#define __GPIO_PF_PIN_10_STATE__ _LOW

/*--
if this:PinExist("F", 11) then
    local PF11 = {}
    PF11[#PF11 + 1] = {NAME = "PF11_NC", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}
    PF11[#PF11 + 1] = {NAME = "USX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PF11[#PF11 + 1] = {NAME = "USX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PF11[#PF11 + 1] = {NAME = "USX_CLK", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PF11[#PF11 + 1] = {NAME = "USX_CS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PF11[#PF11 + 1] = {NAME = "USX_CTS", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PF11[#PF11 + 1] = {NAME = "USX_RTS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PF11[#PF11 + 1] = {NAME = "LEUX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PF11[#PF11 + 1] = {NAME = "LEUX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PF11[#PF11 + 1] = {NAME = "I2CX_SCL", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PF11[#PF11 + 1] = {NAME = "I2CX_SDA", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PF11[#PF11 + 1] = {NAME = "ADCX", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}

    this:AddWidget("Textbox", "PF11")
    this:AddProposals(PF11, "F", 11)
end
--*/
#define __GPIO_PF_PIN_11_NAME__ PF11_NC

/*--
if this:PinExist("F", 11) then
    this:AddWidget("Combobox")
    this:AddPinMode("F", 11)
end
--*/
#define __GPIO_PF_PIN_11_MODE__ _GPIO_MODE_DISABLED

/*--
if this:PinExist("F", 11) then
    this:AddWidget("Combobox")
    this:AddPinState("F", 11)
end
--*/
#define __GPIO_PF_PIN_11_STATE__ _LOW

/*--
if this:PinExist("F", 12) then
    local PF12 = {}
    PF12[#PF12 + 1] = {NAME = "PF12_NC", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}
    PF12[#PF12 + 1] = {NAME = "USX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PF12[#PF12 + 1] = {NAME = "USX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PF12[#PF12 + 1] = {NAME = "USX_CLK", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PF12[#PF12 + 1] = {NAME = "USX_CS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PF12[#PF12 + 1] = {NAME = "USX_CTS", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PF12[#PF12 + 1] = {NAME = "USX_RTS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PF12[#PF12 + 1] = {NAME = "LEUX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PF12[#PF12 + 1] = {NAME = "LEUX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PF12[#PF12 + 1] = {NAME = "I2CX_SCL", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PF12[#PF12 + 1] = {NAME = "I2CX_SDA", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PF12[#PF12 + 1] = {NAME = "ADCX", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}

    this:AddWidget("Textbox", "PF12")
    this:AddProposals(PF12, "F", 12)
end
--*/
#define __GPIO_PF_PIN_12_NAME__ PF12_NC

/*--
if this:PinExist("F", 12) then
    this:AddWidget("Combobox")
    this:AddPinMode("F", 12)
end
--*/
#define __GPIO_PF_PIN_12_MODE__ _GPIO_MODE_DISABLED

/*--
if this:PinExist("F", 12) then
    this:AddWidget("Combobox")
    this:AddPinState("F", 12)
end
--*/
#define __GPIO_PF_PIN_12_STATE__ _LOW

/*--
if this:PinExist("F", 13) then
    local PF13 = {}
    PF13[#PF13 + 1] = {NAME = "PF13_NC", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}
    PF13[#PF13 + 1] = {NAME = "USX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PF13[#PF13 + 1] = {NAME = "USX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PF13[#PF13 + 1] = {NAME = "USX_CLK", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PF13[#PF13 + 1] = {NAME = "USX_CS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PF13[#PF13 + 1] = {NAME = "USX_CTS", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PF13[#PF13 + 1] = {NAME = "USX_RTS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PF13[#PF13 + 1] = {NAME = "LEUX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PF13[#PF13 + 1] = {NAME = "LEUX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PF13[#PF13 + 1] = {NAME = "I2CX_SCL", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PF13[#PF13 + 1] = {NAME = "I2CX_SDA", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PF13[#PF13 + 1] = {NAME = "ADCX", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}

    this:AddWidget("Textbox", "PF13")
    this:AddProposals(PF13, "F", 13)
end
--*/
#define __GPIO_PF_PIN_13_NAME__ PF13_NC

/*--
if this:PinExist("F", 13) then
    this:AddWidget("Combobox")
    this:AddPinMode("F", 13)
end
--*/
#define __GPIO_PF_PIN_13_MODE__ _GPIO_MODE_DISABLED

/*--
if this:PinExist("F", 13) then
    this:AddWidget("Combobox")
    this:AddPinState("F", 13)
end
--*/
#define __GPIO_PF_PIN_13_STATE__ _LOW

/*--
if this:PinExist("F", 14) then
    local PF14 = {}
    PF14[#PF14 + 1] = {NAME = "PF14_NC", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}
    PF14[#PF14 + 1] = {NAME = "USX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PF14[#PF14 + 1] = {NAME = "USX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PF14[#PF14 + 1] = {NAME = "USX_CLK", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PF14[#PF14 + 1] = {NAME = "USX_CS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PF14[#PF14 + 1] = {NAME = "USX_CTS", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PF14[#PF14 + 1] = {NAME = "USX_RTS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PF14[#PF14 + 1] = {NAME = "LEUX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PF14[#PF14 + 1] = {NAME = "LEUX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PF14[#PF14 + 1] = {NAME = "I2CX_SCL", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PF14[#PF14 + 1] = {NAME = "I2CX_SDA", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PF14[#PF14 + 1] = {NAME = "ADCX", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}

    this:AddWidget("Textbox", "PF14")
    this:AddProposals(PF14, "F", 14)
end
--*/
#define __GPIO_PF_PIN_14_NAME__ PF14_NC

/*--
if this:PinExist("F", 14) then
    this:AddWidget("Combobox")
    this:AddPinMode("F", 14)
end
--*/
#define __GPIO_PF_PIN_14_MODE__ _GPIO_MODE_DISABLED

/*--
if this:PinExist("F", 14) then
    this:AddWidget("Combobox")
    this:AddPinState("F", 14)
end
--*/
#define __GPIO_PF_PIN_14_STATE__ _LOW

/*--
if this:PinExist("F", 15) then
    local PF15 = {}
    PF15[#PF15 + 1] = {NAME = "PF15_NC", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}
    PF15[#PF15 + 1] = {NAME = "USX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PF15[#PF15 + 1] = {NAME = "USX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PF15[#PF15 + 1] = {NAME = "USX_CLK", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PF15[#PF15 + 1] = {NAME = "USX_CS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PF15[#PF15 + 1] = {NAME = "USX_CTS", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PF15[#PF15 + 1] = {NAME = "USX_RTS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PF15[#PF15 + 1] = {NAME = "LEUX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PF15[#PF15 + 1] = {NAME = "LEUX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PF15[#PF15 + 1] = {NAME = "I2CX_SCL", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PF15[#PF15 + 1] = {NAME = "I2CX_SDA", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PF15[#PF15 + 1] = {NAME = "ADCX", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}

    this:AddWidget("Textbox", "PF15")
    this:AddProposals(PF15, "F", 15)
end
--*/
#define __GPIO_PF_PIN_15_NAME__ PF15_NC

/*--
if this:PinExist("F", 15) then
    this:AddWidget("Combobox")
    this:AddPinMode("F", 15)
end
--*/
#define __GPIO_PF_PIN_15_MODE__ _GPIO_MODE_DISABLED

/*--
if this:PinExist("F", 15) then
    this:AddWidget("Combobox")
    this:AddPinState("F", 15)
end
--*/
#define __GPIO_PF_PIN_15_STATE__ _LOW

/*----------------------------------------------------------------------------*/
/*--
if this:PinExist("G", 0) then
    local PG0 = {}
    PG0[#PG0 + 1] = {NAME = "PG0_NC", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}
    PG0[#PG0 + 1] = {NAME = "USX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PG0[#PG0 + 1] = {NAME = "USX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PG0[#PG0 + 1] = {NAME = "USX_CLK", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PG0[#PG0 + 1] = {NAME = "USX_CS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PG0[#PG0 + 1] = {NAME = "USX_CTS", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PG0[#PG0 + 1] = {NAME = "USX_RTS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PG0[#PG0 + 1] = {NAME = "LEUX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PG0[#PG0 + 1] = {NAME = "LEUX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PG0[#PG0 + 1] = {NAME = "I2CX_SCL", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PG0[#PG0 + 1] = {NAME = "I2CX_SDA", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PG0[#PG0 + 1] = {NAME = "ADCX", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}

    this:AddWidget("Textbox", "PG0")
    this:AddProposals(PG0, "G", 0)
end
--*/
#define __GPIO_PG_PIN_0_NAME__ PG0_NC

/*--
if this:PinExist("G", 0) then
    this:AddWidget("Combobox")
    this:AddPinMode("G", 0)
end
--*/
#define __GPIO_PG_PIN_0_MODE__ _GPIO_MODE_DISABLED

/*--
if this:PinExist("G", 0) then
    this:AddWidget("Combobox")
    this:AddPinState("G", 0)
end
--*/
#define __GPIO_PG_PIN_0_STATE__ _LOW

/*--
if this:PinExist("G", 1) then
    local PG1 = {}
    PG1[#PG1 + 1] = {NAME = "PG1_NC", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}
    PG1[#PG1 + 1] = {NAME = "USX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PG1[#PG1 + 1] = {NAME = "USX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PG1[#PG1 + 1] = {NAME = "USX_CLK", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PG1[#PG1 + 1] = {NAME = "USX_CS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PG1[#PG1 + 1] = {NAME = "USX_CTS", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PG1[#PG1 + 1] = {NAME = "USX_RTS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PG1[#PG1 + 1] = {NAME = "LEUX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PG1[#PG1 + 1] = {NAME = "LEUX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PG1[#PG1 + 1] = {NAME = "I2CX_SCL", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PG1[#PG1 + 1] = {NAME = "I2CX_SDA", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PG1[#PG1 + 1] = {NAME = "ADCX", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}

    this:AddWidget("Textbox", "PG1")
    this:AddProposals(PG1, "G", 1)
end
--*/
#define __GPIO_PG_PIN_1_NAME__ PG1_NC

/*--
if this:PinExist("G", 1) then
    this:AddWidget("Combobox")
    this:AddPinMode("G", 1)
end
--*/
#define __GPIO_PG_PIN_1_MODE__ _GPIO_MODE_DISABLED

/*--
if this:PinExist("G", 1) then
    this:AddWidget("Combobox")
    this:AddPinState("G", 1)
end
--*/
#define __GPIO_PG_PIN_1_STATE__ _LOW

/*--
if this:PinExist("G", 2) then
    local PG2 = {}
    PG2[#PG2 + 1] = {NAME = "PG2_NC", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}
    PG2[#PG2 + 1] = {NAME = "USX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PG2[#PG2 + 1] = {NAME = "USX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PG2[#PG2 + 1] = {NAME = "USX_CLK", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PG2[#PG2 + 1] = {NAME = "USX_CS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PG2[#PG2 + 1] = {NAME = "USX_CTS", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PG2[#PG2 + 1] = {NAME = "USX_RTS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PG2[#PG2 + 1] = {NAME = "LEUX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PG2[#PG2 + 1] = {NAME = "LEUX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PG2[#PG2 + 1] = {NAME = "I2CX_SCL", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PG2[#PG2 + 1] = {NAME = "I2CX_SDA", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PG2[#PG2 + 1] = {NAME = "ADCX", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}

    this:AddWidget("Textbox", "PG2")
    this:AddProposals(PG2, "G", 2)
end
--*/
#define __GPIO_PG_PIN_2_NAME__ PG2_NC

/*--
if this:PinExist("G", 2) then
    this:AddWidget("Combobox")
    this:AddPinMode("G", 2)
end
--*/
#define __GPIO_PG_PIN_2_MODE__ _GPIO_MODE_DISABLED

/*--
if this:PinExist("G", 2) then
    this:AddWidget("Combobox")
    this:AddPinState("G", 2)
end
--*/
#define __GPIO_PG_PIN_2_STATE__ _LOW

/*--
if this:PinExist("G", 3) then
    local PG3 = {}
    PG3[#PG3 + 1] = {NAME = "PG3_NC", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}
    PG3[#PG3 + 1] = {NAME = "USX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PG3[#PG3 + 1] = {NAME = "USX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PG3[#PG3 + 1] = {NAME = "USX_CLK", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PG3[#PG3 + 1] = {NAME = "USX_CS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PG3[#PG3 + 1] = {NAME = "USX_CTS", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PG3[#PG3 + 1] = {NAME = "USX_RTS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PG3[#PG3 + 1] = {NAME = "LEUX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PG3[#PG3 + 1] = {NAME = "LEUX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PG3[#PG3 + 1] = {NAME = "I2CX_SCL", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PG3[#PG3 + 1] = {NAME = "I2CX_SDA", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PG3[#PG3 + 1] = {NAME = "ADCX", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}

    this:AddWidget("Textbox", "PG3")
    this:AddProposals(PG3, "G", 3)
end
--*/
#define __GPIO_PG_PIN_3_NAME__ PG3_NC

/*--
if this:PinExist("G", 3) then
    this:AddWidget("Combobox")
    this:AddPinMode("G", 3)
end
--*/
#define __GPIO_PG_PIN_3_MODE__ _GPIO_MODE_DISABLED

/*--
if this:PinExist("G", 3) then
    this:AddWidget("Combobox")
    this:AddPinState("G", 3)
end
--*/
#define __GPIO_PG_PIN_3_STATE__ _LOW

/*--
if this:PinExist("G", 4) then
    local PG4 = {}
    PG4[#PG4 + 1] = {NAME = "PG4_NC", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}
    PG4[#PG4 + 1] = {NAME = "USX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PG4[#PG4 + 1] = {NAME = "USX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PG4[#PG4 + 1] = {NAME = "USX_CLK", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PG4[#PG4 + 1] = {NAME = "USX_CS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PG4[#PG4 + 1] = {NAME = "USX_CTS", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PG4[#PG4 + 1] = {NAME = "USX_RTS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PG4[#PG4 + 1] = {NAME = "LEUX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PG4[#PG4 + 1] = {NAME = "LEUX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PG4[#PG4 + 1] = {NAME = "I2CX_SCL", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PG4[#PG4 + 1] = {NAME = "I2CX_SDA", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PG4[#PG4 + 1] = {NAME = "ADCX", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}

    this:AddWidget("Textbox", "PG4")
    this:AddProposals(PG4, "G", 4)
end
--*/
#define __GPIO_PG_PIN_4_NAME__ PG4_NC

/*--
if this:PinExist("G", 4) then
    this:AddWidget("Combobox")
    this:AddPinMode("G", 4)
end
--*/
#define __GPIO_PG_PIN_4_MODE__ _GPIO_MODE_DISABLED

/*--
if this:PinExist("G", 4) then
    this:AddWidget("Combobox")
    this:AddPinState("G", 4)
end
--*/
#define __GPIO_PG_PIN_4_STATE__ _LOW

/*--
if this:PinExist("G", 5) then
    local PG5 = {}
    PG5[#PG5 + 1] = {NAME = "PG5_NC", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}
    PG5[#PG5 + 1] = {NAME = "USX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PG5[#PG5 + 1] = {NAME = "USX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PG5[#PG5 + 1] = {NAME = "USX_CLK", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PG5[#PG5 + 1] = {NAME = "USX_CS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PG5[#PG5 + 1] = {NAME = "USX_CTS", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PG5[#PG5 + 1] = {NAME = "USX_RTS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PG5[#PG5 + 1] = {NAME = "LEUX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PG5[#PG5 + 1] = {NAME = "LEUX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PG5[#PG5 + 1] = {NAME = "I2CX_SCL", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PG5[#PG5 + 1] = {NAME = "I2CX_SDA", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PG5[#PG5 + 1] = {NAME = "ADCX", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}

    this:AddWidget("Textbox", "PG5")
    this:AddProposals(PG5, "G", 5)
end
--*/
#define __GPIO_PG_PIN_5_NAME__ PG5_NC

/*--
if this:PinExist("G", 5) then
    this:AddWidget("Combobox")
    this:AddPinMode("G", 5)
end
--*/
#define __GPIO_PG_PIN_5_MODE__ _GPIO_MODE_DISABLED

/*--
if this:PinExist("G", 5) then
    this:AddWidget("Combobox")
    this:AddPinState("G", 5)
end
--*/
#define __GPIO_PG_PIN_5_STATE__ _LOW

/*--
if this:PinExist("G", 6) then
    local PG6 = {}
    PG6[#PG6 + 1] = {NAME = "PG6_NC", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}
    PG6[#PG6 + 1] = {NAME = "USX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PG6[#PG6 + 1] = {NAME = "USX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PG6[#PG6 + 1] = {NAME = "USX_CLK", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PG6[#PG6 + 1] = {NAME = "USX_CS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PG6[#PG6 + 1] = {NAME = "USX_CTS", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PG6[#PG6 + 1] = {NAME = "USX_RTS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PG6[#PG6 + 1] = {NAME = "LEUX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PG6[#PG6 + 1] = {NAME = "LEUX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PG6[#PG6 + 1] = {NAME = "I2CX_SCL", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PG6[#PG6 + 1] = {NAME = "I2CX_SDA", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PG6[#PG6 + 1] = {NAME = "ADCX", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}

    this:AddWidget("Textbox", "PG6")
    this:AddProposals(PG6, "G", 6)
end
--*/
#define __GPIO_PG_PIN_6_NAME__ PG6_NC

/*--
if this:PinExist("G", 6) then
    this:AddWidget("Combobox")
    this:AddPinMode("G", 6)
end
--*/
#define __GPIO_PG_PIN_6_MODE__ _GPIO_MODE_DISABLED

/*--
if this:PinExist("G", 6) then
    this:AddWidget("Combobox")
    this:AddPinState("G", 6)
end
--*/
#define __GPIO_PG_PIN_6_STATE__ _LOW

/*--
if this:PinExist("G", 7) then
    local PG7 = {}
    PG7[#PG7 + 1] = {NAME = "PG7_NC", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}
    PG7[#PG7 + 1] = {NAME = "USX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PG7[#PG7 + 1] = {NAME = "USX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PG7[#PG7 + 1] = {NAME = "USX_CLK", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PG7[#PG7 + 1] = {NAME = "USX_CS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PG7[#PG7 + 1] = {NAME = "USX_CTS", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PG7[#PG7 + 1] = {NAME = "USX_RTS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PG7[#PG7 + 1] = {NAME = "LEUX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PG7[#PG7 + 1] = {NAME = "LEUX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PG7[#PG7 + 1] = {NAME = "I2CX_SCL", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PG7[#PG7 + 1] = {NAME = "I2CX_SDA", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PG7[#PG7 + 1] = {NAME = "ADCX", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}

    this:AddWidget("Textbox", "PG7")
    this:AddProposals(PG7, "G", 7)
end
--*/
#define __GPIO_PG_PIN_7_NAME__ PG7_NC

/*--
if this:PinExist("G", 7) then
    this:AddWidget("Combobox")
    this:AddPinMode("G", 7)
end
--*/
#define __GPIO_PG_PIN_7_MODE__ _GPIO_MODE_DISABLED

/*--
if this:PinExist("G", 7) then
    this:AddWidget("Combobox")
    this:AddPinState("G", 7)
end
--*/
#define __GPIO_PG_PIN_7_STATE__ _LOW

/*--
if this:PinExist("G", 8) then
    local PG8 = {}
    PG8[#PG8 + 1] = {NAME = "PG8_NC", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}
    PG8[#PG8 + 1] = {NAME = "USX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PG8[#PG8 + 1] = {NAME = "USX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PG8[#PG8 + 1] = {NAME = "USX_CLK", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PG8[#PG8 + 1] = {NAME = "USX_CS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PG8[#PG8 + 1] = {NAME = "USX_CTS", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PG8[#PG8 + 1] = {NAME = "USX_RTS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PG8[#PG8 + 1] = {NAME = "LEUX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PG8[#PG8 + 1] = {NAME = "LEUX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PG8[#PG8 + 1] = {NAME = "I2CX_SCL", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PG8[#PG8 + 1] = {NAME = "I2CX_SDA", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PG8[#PG8 + 1] = {NAME = "ADCX", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}

    this:AddWidget("Textbox", "PG8")
    this:AddProposals(PG8, "G", 8)
end
--*/
#define __GPIO_PG_PIN_8_NAME__ PG8_NC

/*--
if this:PinExist("G", 8) then
    this:AddWidget("Combobox")
    this:AddPinMode("G", 8)
end
--*/
#define __GPIO_PG_PIN_8_MODE__ _GPIO_MODE_DISABLED

/*--
if this:PinExist("G", 8) then
    this:AddWidget("Combobox")
    this:AddPinState("G", 8)
end
--*/
#define __GPIO_PG_PIN_8_STATE__ _LOW

/*--
if this:PinExist("G", 9) then
    local PG9 = {}
    PG9[#PG9 + 1] = {NAME = "PG9_NC", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}
    PG9[#PG9 + 1] = {NAME = "USX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PG9[#PG9 + 1] = {NAME = "USX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PG9[#PG9 + 1] = {NAME = "USX_CLK", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PG9[#PG9 + 1] = {NAME = "USX_CS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PG9[#PG9 + 1] = {NAME = "USX_CTS", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PG9[#PG9 + 1] = {NAME = "USX_RTS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PG9[#PG9 + 1] = {NAME = "LEUX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PG9[#PG9 + 1] = {NAME = "LEUX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PG9[#PG9 + 1] = {NAME = "I2CX_SCL", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PG9[#PG9 + 1] = {NAME = "I2CX_SDA", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PG9[#PG9 + 1] = {NAME = "ADCX", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}

    this:AddWidget("Textbox", "PG9")
    this:AddProposals(PG9, "G", 9)
end
--*/
#define __GPIO_PG_PIN_9_NAME__ PG9_NC

/*--
if this:PinExist("G", 9) then
    this:AddWidget("Combobox")
    this:AddPinMode("G", 9)
end
--*/
#define __GPIO_PG_PIN_9_MODE__ _GPIO_MODE_DISABLED

/*--
if this:PinExist("G", 9) then
    this:AddWidget("Combobox")
    this:AddPinState("G", 9)
end
--*/
#define __GPIO_PG_PIN_9_STATE__ _LOW

/*--
if this:PinExist("G", 10) then
    local PG10 = {}
    PG10[#PG10 + 1] = {NAME = "PG10_NC", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}
    PG10[#PG10 + 1] = {NAME = "USX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PG10[#PG10 + 1] = {NAME = "USX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PG10[#PG10 + 1] = {NAME = "USX_CLK", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PG10[#PG10 + 1] = {NAME = "USX_CS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PG10[#PG10 + 1] = {NAME = "USX_CTS", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PG10[#PG10 + 1] = {NAME = "USX_RTS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PG10[#PG10 + 1] = {NAME = "LEUX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PG10[#PG10 + 1] = {NAME = "LEUX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PG10[#PG10 + 1] = {NAME = "I2CX_SCL", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PG10[#PG10 + 1] = {NAME = "I2CX_SDA", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PG10[#PG10 + 1] = {NAME = "ADCX", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}

    this:AddWidget("Textbox", "PG10")
    this:AddProposals(PG10, "G", 10)
end
--*/
#define __GPIO_PG_PIN_10_NAME__ PG10_NC

/*--
if this:PinExist("G", 10) then
    this:AddWidget("Combobox")
    this:AddPinMode("G", 10)
end
--*/
#define __GPIO_PG_PIN_10_MODE__ _GPIO_MODE_DISABLED

/*--
if this:PinExist("G", 10) then
    this:AddWidget("Combobox")
    this:AddPinState("G", 10)
end
--*/
#define __GPIO_PG_PIN_10_STATE__ _LOW

/*--
if this:PinExist("G", 11) then
    local PG11 = {}
    PG11[#PG11 + 1] = {NAME = "PG11_NC", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}
    PG11[#PG11 + 1] = {NAME = "USX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PG11[#PG11 + 1] = {NAME = "USX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PG11[#PG11 + 1] = {NAME = "USX_CLK", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PG11[#PG11 + 1] = {NAME = "USX_CS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PG11[#PG11 + 1] = {NAME = "USX_CTS", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PG11[#PG11 + 1] = {NAME = "USX_RTS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PG11[#PG11 + 1] = {NAME = "LEUX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PG11[#PG11 + 1] = {NAME = "LEUX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PG11[#PG11 + 1] = {NAME = "I2CX_SCL", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PG11[#PG11 + 1] = {NAME = "I2CX_SDA", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PG11[#PG11 + 1] = {NAME = "ADCX", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}

    this:AddWidget("Textbox", "PG11")
    this:AddProposals(PG11, "G", 11)
end
--*/
#define __GPIO_PG_PIN_11_NAME__ PG11_NC

/*--
if this:PinExist("G", 11) then
    this:AddWidget("Combobox")
    this:AddPinMode("G", 11)
end
--*/
#define __GPIO_PG_PIN_11_MODE__ _GPIO_MODE_DISABLED

/*--
if this:PinExist("G", 11) then
    this:AddWidget("Combobox")
    this:AddPinState("G", 11)
end
--*/
#define __GPIO_PG_PIN_11_STATE__ _LOW

/*--
if this:PinExist("G", 12) then
    local PG12 = {}
    PG12[#PG12 + 1] = {NAME = "PG12_NC", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}
    PG12[#PG12 + 1] = {NAME = "USX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PG12[#PG12 + 1] = {NAME = "USX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PG12[#PG12 + 1] = {NAME = "USX_CLK", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PG12[#PG12 + 1] = {NAME = "USX_CS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PG12[#PG12 + 1] = {NAME = "USX_CTS", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PG12[#PG12 + 1] = {NAME = "USX_RTS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PG12[#PG12 + 1] = {NAME = "LEUX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PG12[#PG12 + 1] = {NAME = "LEUX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PG12[#PG12 + 1] = {NAME = "I2CX_SCL", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PG12[#PG12 + 1] = {NAME = "I2CX_SDA", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PG12[#PG12 + 1] = {NAME = "ADCX", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}

    this:AddWidget("Textbox", "PG12")
    this:AddProposals(PG12, "G", 12)
end
--*/
#define __GPIO_PG_PIN_12_NAME__ PG12_NC

/*--
if this:PinExist("G", 12) then
    this:AddWidget("Combobox")
    this:AddPinMode("G", 12)
end
--*/
#define __GPIO_PG_PIN_12_MODE__ _GPIO_MODE_DISABLED

/*--
if this:PinExist("G", 12) then
    this:AddWidget("Combobox")
    this:AddPinState("G", 12)
end
--*/
#define __GPIO_PG_PIN_12_STATE__ _LOW

/*--
if this:PinExist("G", 13) then
    local PG13 = {}
    PG13[#PG13 + 1] = {NAME = "PG13_NC", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}
    PG13[#PG13 + 1] = {NAME = "USX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PG13[#PG13 + 1] = {NAME = "USX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PG13[#PG13 + 1] = {NAME = "USX_CLK", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PG13[#PG13 + 1] = {NAME = "USX_CS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PG13[#PG13 + 1] = {NAME = "USX_CTS", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PG13[#PG13 + 1] = {NAME = "USX_RTS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PG13[#PG13 + 1] = {NAME = "LEUX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PG13[#PG13 + 1] = {NAME = "LEUX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PG13[#PG13 + 1] = {NAME = "I2CX_SCL", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PG13[#PG13 + 1] = {NAME = "I2CX_SDA", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PG13[#PG13 + 1] = {NAME = "ADCX", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}

    this:AddWidget("Textbox", "PG13")
    this:AddProposals(PG13, "G", 13)
end
--*/
#define __GPIO_PG_PIN_13_NAME__ PG13_NC

/*--
if this:PinExist("G", 13) then
    this:AddWidget("Combobox")
    this:AddPinMode("G", 13)
end
--*/
#define __GPIO_PG_PIN_13_MODE__ _GPIO_MODE_DISABLED

/*--
if this:PinExist("G", 13) then
    this:AddWidget("Combobox")
    this:AddPinState("G", 13)
end
--*/
#define __GPIO_PG_PIN_13_STATE__ _LOW

/*--
if this:PinExist("G", 14) then
    local PG14 = {}
    PG14[#PG14 + 1] = {NAME = "PG14_NC", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}
    PG14[#PG14 + 1] = {NAME = "USX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PG14[#PG14 + 1] = {NAME = "USX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PG14[#PG14 + 1] = {NAME = "USX_CLK", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PG14[#PG14 + 1] = {NAME = "USX_CS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PG14[#PG14 + 1] = {NAME = "USX_CTS", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PG14[#PG14 + 1] = {NAME = "USX_RTS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PG14[#PG14 + 1] = {NAME = "LEUX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PG14[#PG14 + 1] = {NAME = "LEUX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PG14[#PG14 + 1] = {NAME = "I2CX_SCL", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PG14[#PG14 + 1] = {NAME = "I2CX_SDA", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PG14[#PG14 + 1] = {NAME = "ADCX", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}

    this:AddWidget("Textbox", "PG14")
    this:AddProposals(PG14, "G", 14)
end
--*/
#define __GPIO_PG_PIN_14_NAME__ PG14_NC

/*--
if this:PinExist("G", 14) then
    this:AddWidget("Combobox")
    this:AddPinMode("G", 14)
end
--*/
#define __GPIO_PG_PIN_14_MODE__ _GPIO_MODE_DISABLED

/*--
if this:PinExist("G", 14) then
    this:AddWidget("Combobox")
    this:AddPinState("G", 14)
end
--*/
#define __GPIO_PG_PIN_14_STATE__ _LOW

/*--
if this:PinExist("G", 15) then
    local PG15 = {}
    PG15[#PG15 + 1] = {NAME = "PG15_NC", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}
    PG15[#PG15 + 1] = {NAME = "USX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PG15[#PG15 + 1] = {NAME = "USX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PG15[#PG15 + 1] = {NAME = "USX_CLK", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PG15[#PG15 + 1] = {NAME = "USX_CS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PG15[#PG15 + 1] = {NAME = "USX_CTS", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PG15[#PG15 + 1] = {NAME = "USX_RTS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PG15[#PG15 + 1] = {NAME = "LEUX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PG15[#PG15 + 1] = {NAME = "LEUX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PG15[#PG15 + 1] = {NAME = "I2CX_SCL", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PG15[#PG15 + 1] = {NAME = "I2CX_SDA", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PG15[#PG15 + 1] = {NAME = "ADCX", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}

    this:AddWidget("Textbox", "PG15")
    this:AddProposals(PG15, "G", 15)
end
--*/
#define __GPIO_PG_PIN_15_NAME__ PG15_NC

/*--
if this:PinExist("G", 15) then
    this:AddWidget("Combobox")
    this:AddPinMode("G", 15)
end
--*/
#define __GPIO_PG_PIN_15_MODE__ _GPIO_MODE_DISABLED

/*--
if this:PinExist("G", 15) then
    this:AddWidget("Combobox")
    this:AddPinState("G", 15)
end
--*/
#define __GPIO_PG_PIN_15_STATE__ _LOW

/*----------------------------------------------------------------------------*/
/*--
if this:PinExist("H", 0) then
    local PH0 = {}
    PH0[#PH0 + 1] = {NAME = "PH0_NC", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}
    PH0[#PH0 + 1] = {NAME = "USX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PH0[#PH0 + 1] = {NAME = "USX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PH0[#PH0 + 1] = {NAME = "USX_CLK", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PH0[#PH0 + 1] = {NAME = "USX_CS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PH0[#PH0 + 1] = {NAME = "USX_CTS", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PH0[#PH0 + 1] = {NAME = "USX_RTS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PH0[#PH0 + 1] = {NAME = "LEUX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PH0[#PH0 + 1] = {NAME = "LEUX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PH0[#PH0 + 1] = {NAME = "I2CX_SCL", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PH0[#PH0 + 1] = {NAME = "I2CX_SDA", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PH0[#PH0 + 1] = {NAME = "ADCX", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}

    this:AddWidget("Textbox", "PH0")
    this:AddProposals(PH0, "H", 0)
end
--*/
#define __GPIO_PH_PIN_0_NAME__ PH0_NC

/*--
if this:PinExist("H", 0) then
    this:AddWidget("Combobox")
    this:AddPinMode("H", 0)
end
--*/
#define __GPIO_PH_PIN_0_MODE__ _GPIO_MODE_DISABLED

/*--
if this:PinExist("H", 0) then
    this:AddWidget("Combobox")
    this:AddPinState("H", 0)
end
--*/
#define __GPIO_PH_PIN_0_STATE__ _LOW

/*--
if this:PinExist("H", 1) then
    local PH1 = {}
    PH1[#PH1 + 1] = {NAME = "PH1_NC", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}
    PH1[#PH1 + 1] = {NAME = "USX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PH1[#PH1 + 1] = {NAME = "USX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PH1[#PH1 + 1] = {NAME = "USX_CLK", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PH1[#PH1 + 1] = {NAME = "USX_CS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PH1[#PH1 + 1] = {NAME = "USX_CTS", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PH1[#PH1 + 1] = {NAME = "USX_RTS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PH1[#PH1 + 1] = {NAME = "LEUX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PH1[#PH1 + 1] = {NAME = "LEUX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PH1[#PH1 + 1] = {NAME = "I2CX_SCL", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PH1[#PH1 + 1] = {NAME = "I2CX_SDA", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PH1[#PH1 + 1] = {NAME = "ADCX", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}

    this:AddWidget("Textbox", "PH1")
    this:AddProposals(PH1, "H", 1)
end
--*/
#define __GPIO_PH_PIN_1_NAME__ PH1_NC

/*--
if this:PinExist("H", 1) then
    this:AddWidget("Combobox")
    this:AddPinMode("H", 1)
end
--*/
#define __GPIO_PH_PIN_1_MODE__ _GPIO_MODE_DISABLED

/*--
if this:PinExist("H", 1) then
    this:AddWidget("Combobox")
    this:AddPinState("H", 1)
end
--*/
#define __GPIO_PH_PIN_1_STATE__ _LOW

/*--
if this:PinExist("H", 2) then
    local PH2 = {}
    PH2[#PH2 + 1] = {NAME = "PH2_NC", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}
    PH2[#PH2 + 1] = {NAME = "USX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PH2[#PH2 + 1] = {NAME = "USX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PH2[#PH2 + 1] = {NAME = "USX_CLK", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PH2[#PH2 + 1] = {NAME = "USX_CS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PH2[#PH2 + 1] = {NAME = "USX_CTS", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PH2[#PH2 + 1] = {NAME = "USX_RTS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PH2[#PH2 + 1] = {NAME = "LEUX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PH2[#PH2 + 1] = {NAME = "LEUX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PH2[#PH2 + 1] = {NAME = "I2CX_SCL", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PH2[#PH2 + 1] = {NAME = "I2CX_SDA", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PH2[#PH2 + 1] = {NAME = "ADCX", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}

    this:AddWidget("Textbox", "PH2")
    this:AddProposals(PH2, "H", 2)
end
--*/
#define __GPIO_PH_PIN_2_NAME__ PH2_NC

/*--
if this:PinExist("H", 2) then
    this:AddWidget("Combobox")
    this:AddPinMode("H", 2)
end
--*/
#define __GPIO_PH_PIN_2_MODE__ _GPIO_MODE_DISABLED

/*--
if this:PinExist("H", 2) then
    this:AddWidget("Combobox")
    this:AddPinState("H", 2)
end
--*/
#define __GPIO_PH_PIN_2_STATE__ _LOW

/*--
if this:PinExist("H", 3) then
    local PH3 = {}
    PH3[#PH3 + 1] = {NAME = "PH3_NC", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}
    PH3[#PH3 + 1] = {NAME = "USX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PH3[#PH3 + 1] = {NAME = "USX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PH3[#PH3 + 1] = {NAME = "USX_CLK", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PH3[#PH3 + 1] = {NAME = "USX_CS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PH3[#PH3 + 1] = {NAME = "USX_CTS", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PH3[#PH3 + 1] = {NAME = "USX_RTS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PH3[#PH3 + 1] = {NAME = "LEUX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PH3[#PH3 + 1] = {NAME = "LEUX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PH3[#PH3 + 1] = {NAME = "I2CX_SCL", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PH3[#PH3 + 1] = {NAME = "I2CX_SDA", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PH3[#PH3 + 1] = {NAME = "ADCX", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}

    this:AddWidget("Textbox", "PH3")
    this:AddProposals(PH3, "H", 3)
end
--*/
#define __GPIO_PH_PIN_3_NAME__ PH3_NC

/*--
if this:PinExist("H", 3) then
    this:AddWidget("Combobox")
    this:AddPinMode("H", 3)
end
--*/
#define __GPIO_PH_PIN_3_MODE__ _GPIO_MODE_DISABLED

/*--
if this:PinExist("H", 3) then
    this:AddWidget("Combobox")
    this:AddPinState("H", 3)
end
--*/
#define __GPIO_PH_PIN_3_STATE__ _LOW

/*--
if this:PinExist("H", 4) then
    local PH4 = {}
    PH4[#PH4 + 1] = {NAME = "PH4_NC", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}
    PH4[#PH4 + 1] = {NAME = "USX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PH4[#PH4 + 1] = {NAME = "USX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PH4[#PH4 + 1] = {NAME = "USX_CLK", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PH4[#PH4 + 1] = {NAME = "USX_CS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PH4[#PH4 + 1] = {NAME = "USX_CTS", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PH4[#PH4 + 1] = {NAME = "USX_RTS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PH4[#PH4 + 1] = {NAME = "LEUX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PH4[#PH4 + 1] = {NAME = "LEUX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PH4[#PH4 + 1] = {NAME = "I2CX_SCL", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PH4[#PH4 + 1] = {NAME = "I2CX_SDA", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PH4[#PH4 + 1] = {NAME = "ADCX", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}

    this:AddWidget("Textbox", "PH4")
    this:AddProposals(PH4, "H", 4)
end
--*/
#define __GPIO_PH_PIN_4_NAME__ PH4_NC

/*--
if this:PinExist("H", 4) then
    this:AddWidget("Combobox")
    this:AddPinMode("H", 4)
end
--*/
#define __GPIO_PH_PIN_4_MODE__ _GPIO_MODE_DISABLED

/*--
if this:PinExist("H", 4) then
    this:AddWidget("Combobox")
    this:AddPinState("H", 4)
end
--*/
#define __GPIO_PH_PIN_4_STATE__ _LOW

/*--
if this:PinExist("H", 5) then
    local PH5 = {}
    PH5[#PH5 + 1] = {NAME = "PH5_NC", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}
    PH5[#PH5 + 1] = {NAME = "USX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PH5[#PH5 + 1] = {NAME = "USX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PH5[#PH5 + 1] = {NAME = "USX_CLK", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PH5[#PH5 + 1] = {NAME = "USX_CS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PH5[#PH5 + 1] = {NAME = "USX_CTS", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PH5[#PH5 + 1] = {NAME = "USX_RTS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PH5[#PH5 + 1] = {NAME = "LEUX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PH5[#PH5 + 1] = {NAME = "LEUX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PH5[#PH5 + 1] = {NAME = "I2CX_SCL", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PH5[#PH5 + 1] = {NAME = "I2CX_SDA", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PH5[#PH5 + 1] = {NAME = "ADCX", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}

    this:AddWidget("Textbox", "PH5")
    this:AddProposals(PH5, "H", 5)
end
--*/
#define __GPIO_PH_PIN_5_NAME__ PH5_NC

/*--
if this:PinExist("H", 5) then
    this:AddWidget("Combobox")
    this:AddPinMode("H", 5)
end
--*/
#define __GPIO_PH_PIN_5_MODE__ _GPIO_MODE_DISABLED

/*--
if this:PinExist("H", 5) then
    this:AddWidget("Combobox")
    this:AddPinState("H", 5)
end
--*/
#define __GPIO_PH_PIN_5_STATE__ _LOW

/*--
if this:PinExist("H", 6) then
    local PH6 = {}
    PH6[#PH6 + 1] = {NAME = "PH6_NC", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}
    PH6[#PH6 + 1] = {NAME = "USX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PH6[#PH6 + 1] = {NAME = "USX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PH6[#PH6 + 1] = {NAME = "USX_CLK", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PH6[#PH6 + 1] = {NAME = "USX_CS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PH6[#PH6 + 1] = {NAME = "USX_CTS", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PH6[#PH6 + 1] = {NAME = "USX_RTS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PH6[#PH6 + 1] = {NAME = "LEUX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PH6[#PH6 + 1] = {NAME = "LEUX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PH6[#PH6 + 1] = {NAME = "I2CX_SCL", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PH6[#PH6 + 1] = {NAME = "I2CX_SDA", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PH6[#PH6 + 1] = {NAME = "ADCX", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}

    this:AddWidget("Textbox", "PH6")
    this:AddProposals(PH6, "H", 6)
end
--*/
#define __GPIO_PH_PIN_6_NAME__ PH6_NC

/*--
if this:PinExist("H", 6) then
    this:AddWidget("Combobox")
    this:AddPinMode("H", 6)
end
--*/
#define __GPIO_PH_PIN_6_MODE__ _GPIO_MODE_DISABLED

/*--
if this:PinExist("H", 6) then
    this:AddWidget("Combobox")
    this:AddPinState("H", 6)
end
--*/
#define __GPIO_PH_PIN_6_STATE__ _LOW

/*--
if this:PinExist("H", 7) then
    local PH7 = {}
    PH7[#PH7 + 1] = {NAME = "PH7_NC", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}
    PH7[#PH7 + 1] = {NAME = "USX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PH7[#PH7 + 1] = {NAME = "USX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PH7[#PH7 + 1] = {NAME = "USX_CLK", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PH7[#PH7 + 1] = {NAME = "USX_CS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PH7[#PH7 + 1] = {NAME = "USX_CTS", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PH7[#PH7 + 1] = {NAME = "USX_RTS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PH7[#PH7 + 1] = {NAME = "LEUX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PH7[#PH7 + 1] = {NAME = "LEUX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PH7[#PH7 + 1] = {NAME = "I2CX_SCL", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PH7[#PH7 + 1] = {NAME = "I2CX_SDA", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PH7[#PH7 + 1] = {NAME = "ADCX", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}

    this:AddWidget("Textbox", "PH7")
    this:AddProposals(PH7, "H", 7)
end
--*/
#define __GPIO_PH_PIN_7_NAME__ PH7_NC

/*--
if this:PinExist("H", 7) then
    this:AddWidget("Combobox")
    this:AddPinMode("H", 7)
end
--*/
#define __GPIO_PH_PIN_7_MODE__ _GPIO_MODE_DISABLED

/*--
if this:PinExist("H", 7) then
    this:AddWidget("Combobox")
    this:AddPinState("H", 7)
end
--*/
#define __GPIO_PH_PIN_7_STATE__ _LOW

/*--
if this:PinExist("H", 8) then
    local PH8 = {}
    PH8[#PH8 + 1] = {NAME = "PH8_NC", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}
    PH8[#PH8 + 1] = {NAME = "USX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PH8[#PH8 + 1] = {NAME = "USX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PH8[#PH8 + 1] = {NAME = "USX_CLK", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PH8[#PH8 + 1] = {NAME = "USX_CS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PH8[#PH8 + 1] = {NAME = "USX_CTS", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PH8[#PH8 + 1] = {NAME = "USX_RTS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PH8[#PH8 + 1] = {NAME = "LEUX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PH8[#PH8 + 1] = {NAME = "LEUX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PH8[#PH8 + 1] = {NAME = "I2CX_SCL", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PH8[#PH8 + 1] = {NAME = "I2CX_SDA", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PH8[#PH8 + 1] = {NAME = "ADCX", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}

    this:AddWidget("Textbox", "PH8")
    this:AddProposals(PH8, "H", 8)
end
--*/
#define __GPIO_PH_PIN_8_NAME__ PH8_NC

/*--
if this:PinExist("H", 8) then
    this:AddWidget("Combobox")
    this:AddPinMode("H", 8)
end
--*/
#define __GPIO_PH_PIN_8_MODE__ _GPIO_MODE_DISABLED

/*--
if this:PinExist("H", 8) then
    this:AddWidget("Combobox")
    this:AddPinState("H", 8)
end
--*/
#define __GPIO_PH_PIN_8_STATE__ _LOW

/*--
if this:PinExist("H", 9) then
    local PH9 = {}
    PH9[#PH9 + 1] = {NAME = "PH9_NC", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}
    PH9[#PH9 + 1] = {NAME = "USX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PH9[#PH9 + 1] = {NAME = "USX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PH9[#PH9 + 1] = {NAME = "USX_CLK", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PH9[#PH9 + 1] = {NAME = "USX_CS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PH9[#PH9 + 1] = {NAME = "USX_CTS", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PH9[#PH9 + 1] = {NAME = "USX_RTS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PH9[#PH9 + 1] = {NAME = "LEUX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PH9[#PH9 + 1] = {NAME = "LEUX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PH9[#PH9 + 1] = {NAME = "I2CX_SCL", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PH9[#PH9 + 1] = {NAME = "I2CX_SDA", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PH9[#PH9 + 1] = {NAME = "ADCX", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}

    this:AddWidget("Textbox", "PH9")
    this:AddProposals(PH9, "H", 9)
end
--*/
#define __GPIO_PH_PIN_9_NAME__ PH9_NC

/*--
if this:PinExist("H", 9) then
    this:AddWidget("Combobox")
    this:AddPinMode("H", 9)
end
--*/
#define __GPIO_PH_PIN_9_MODE__ _GPIO_MODE_DISABLED

/*--
if this:PinExist("H", 9) then
    this:AddWidget("Combobox")
    this:AddPinState("H", 9)
end
--*/
#define __GPIO_PH_PIN_9_STATE__ _LOW

/*--
if this:PinExist("H", 10) then
    local PH10 = {}
    PH10[#PH10 + 1] = {NAME = "PH10_NC", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}
    PH10[#PH10 + 1] = {NAME = "USX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PH10[#PH10 + 1] = {NAME = "USX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PH10[#PH10 + 1] = {NAME = "USX_CLK", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PH10[#PH10 + 1] = {NAME = "USX_CS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PH10[#PH10 + 1] = {NAME = "USX_CTS", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PH10[#PH10 + 1] = {NAME = "USX_RTS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PH10[#PH10 + 1] = {NAME = "LEUX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PH10[#PH10 + 1] = {NAME = "LEUX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PH10[#PH10 + 1] = {NAME = "I2CX_SCL", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PH10[#PH10 + 1] = {NAME = "I2CX_SDA", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PH10[#PH10 + 1] = {NAME = "ADCX", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}

    this:AddWidget("Textbox", "PH10")
    this:AddProposals(PH10, "H", 10)
end
--*/
#define __GPIO_PH_PIN_10_NAME__ PH10_NC

/*--
if this:PinExist("H", 10) then
    this:AddWidget("Combobox")
    this:AddPinMode("H", 10)
end
--*/
#define __GPIO_PH_PIN_10_MODE__ _GPIO_MODE_DISABLED

/*--
if this:PinExist("H", 10) then
    this:AddWidget("Combobox")
    this:AddPinState("H", 10)
end
--*/
#define __GPIO_PH_PIN_10_STATE__ _LOW

/*--
if this:PinExist("H", 11) then
    local PH11 = {}
    PH11[#PH11 + 1] = {NAME = "PH11_NC", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}
    PH11[#PH11 + 1] = {NAME = "USX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PH11[#PH11 + 1] = {NAME = "USX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PH11[#PH11 + 1] = {NAME = "USX_CLK", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PH11[#PH11 + 1] = {NAME = "USX_CS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PH11[#PH11 + 1] = {NAME = "USX_CTS", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PH11[#PH11 + 1] = {NAME = "USX_RTS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PH11[#PH11 + 1] = {NAME = "LEUX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PH11[#PH11 + 1] = {NAME = "LEUX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PH11[#PH11 + 1] = {NAME = "I2CX_SCL", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PH11[#PH11 + 1] = {NAME = "I2CX_SDA", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PH11[#PH11 + 1] = {NAME = "ADCX", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}

    this:AddWidget("Textbox", "PH11")
    this:AddProposals(PH11, "H", 11)
end
--*/
#define __GPIO_PH_PIN_11_NAME__ PH11_NC

/*--
if this:PinExist("H", 11) then
    this:AddWidget("Combobox")
    this:AddPinMode("H", 11)
end
--*/
#define __GPIO_PH_PIN_11_MODE__ _GPIO_MODE_DISABLED

/*--
if this:PinExist("H", 11) then
    this:AddWidget("Combobox")
    this:AddPinState("H", 11)
end
--*/
#define __GPIO_PH_PIN_11_STATE__ _LOW

/*--
if this:PinExist("H", 12) then
    local PH12 = {}
    PH12[#PH12 + 1] = {NAME = "PH12_NC", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}
    PH12[#PH12 + 1] = {NAME = "USX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PH12[#PH12 + 1] = {NAME = "USX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PH12[#PH12 + 1] = {NAME = "USX_CLK", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PH12[#PH12 + 1] = {NAME = "USX_CS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PH12[#PH12 + 1] = {NAME = "USX_CTS", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PH12[#PH12 + 1] = {NAME = "USX_RTS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PH12[#PH12 + 1] = {NAME = "LEUX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PH12[#PH12 + 1] = {NAME = "LEUX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PH12[#PH12 + 1] = {NAME = "I2CX_SCL", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PH12[#PH12 + 1] = {NAME = "I2CX_SDA", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PH12[#PH12 + 1] = {NAME = "ADCX", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}

    this:AddWidget("Textbox", "PH12")
    this:AddProposals(PH12, "H", 12)
end
--*/
#define __GPIO_PH_PIN_12_NAME__ PH12_NC

/*--
if this:PinExist("H", 12) then
    this:AddWidget("Combobox")
    this:AddPinMode("H", 12)
end
--*/
#define __GPIO_PH_PIN_12_MODE__ _GPIO_MODE_DISABLED

/*--
if this:PinExist("H", 12) then
    this:AddWidget("Combobox")
    this:AddPinState("H", 12)
end
--*/
#define __GPIO_PH_PIN_12_STATE__ _LOW

/*--
if this:PinExist("H", 13) then
    local PH13 = {}
    PH13[#PH13 + 1] = {NAME = "PH13_NC", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}
    PH13[#PH13 + 1] = {NAME = "USX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PH13[#PH13 + 1] = {NAME = "USX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PH13[#PH13 + 1] = {NAME = "USX_CLK", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PH13[#PH13 + 1] = {NAME = "USX_CS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PH13[#PH13 + 1] = {NAME = "USX_CTS", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PH13[#PH13 + 1] = {NAME = "USX_RTS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PH13[#PH13 + 1] = {NAME = "LEUX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PH13[#PH13 + 1] = {NAME = "LEUX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PH13[#PH13 + 1] = {NAME = "I2CX_SCL", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PH13[#PH13 + 1] = {NAME = "I2CX_SDA", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PH13[#PH13 + 1] = {NAME = "ADCX", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}

    this:AddWidget("Textbox", "PH13")
    this:AddProposals(PH13, "H", 13)
end
--*/
#define __GPIO_PH_PIN_13_NAME__ PH13_NC

/*--
if this:PinExist("H", 13) then
    this:AddWidget("Combobox")
    this:AddPinMode("H", 13)
end
--*/
#define __GPIO_PH_PIN_13_MODE__ _GPIO_MODE_DISABLED

/*--
if this:PinExist("H", 13) then
    this:AddWidget("Combobox")
    this:AddPinState("H", 13)
end
--*/
#define __GPIO_PH_PIN_13_STATE__ _LOW

/*--
if this:PinExist("H", 14) then
    local PH14 = {}
    PH14[#PH14 + 1] = {NAME = "PH14_NC", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}
    PH14[#PH14 + 1] = {NAME = "USX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PH14[#PH14 + 1] = {NAME = "USX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PH14[#PH14 + 1] = {NAME = "USX_CLK", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PH14[#PH14 + 1] = {NAME = "USX_CS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PH14[#PH14 + 1] = {NAME = "USX_CTS", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PH14[#PH14 + 1] = {NAME = "USX_RTS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PH14[#PH14 + 1] = {NAME = "LEUX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PH14[#PH14 + 1] = {NAME = "LEUX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PH14[#PH14 + 1] = {NAME = "I2CX_SCL", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PH14[#PH14 + 1] = {NAME = "I2CX_SDA", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PH14[#PH14 + 1] = {NAME = "ADCX", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}

    this:AddWidget("Textbox", "PH14")
    this:AddProposals(PH14, "H", 14)
end
--*/
#define __GPIO_PH_PIN_14_NAME__ PH14_NC

/*--
if this:PinExist("H", 14) then
    this:AddWidget("Combobox")
    this:AddPinMode("H", 14)
end
--*/
#define __GPIO_PH_PIN_14_MODE__ _GPIO_MODE_DISABLED

/*--
if this:PinExist("H", 14) then
    this:AddWidget("Combobox")
    this:AddPinState("H", 14)
end
--*/
#define __GPIO_PH_PIN_14_STATE__ _LOW

/*--
if this:PinExist("H", 15) then
    local PH15 = {}
    PH15[#PH15 + 1] = {NAME = "PH15_NC", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}
    PH15[#PH15 + 1] = {NAME = "USX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PH15[#PH15 + 1] = {NAME = "USX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PH15[#PH15 + 1] = {NAME = "USX_CLK", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PH15[#PH15 + 1] = {NAME = "USX_CS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PH15[#PH15 + 1] = {NAME = "USX_CTS", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PH15[#PH15 + 1] = {NAME = "USX_RTS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PH15[#PH15 + 1] = {NAME = "LEUX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PH15[#PH15 + 1] = {NAME = "LEUX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PH15[#PH15 + 1] = {NAME = "I2CX_SCL", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PH15[#PH15 + 1] = {NAME = "I2CX_SDA", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PH15[#PH15 + 1] = {NAME = "ADCX", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}

    this:AddWidget("Textbox", "PH15")
    this:AddProposals(PH15, "H", 15)
end
--*/
#define __GPIO_PH_PIN_15_NAME__ PH15_NC

/*--
if this:PinExist("H", 15) then
    this:AddWidget("Combobox")
    this:AddPinMode("H", 15)
end
--*/
#define __GPIO_PH_PIN_15_MODE__ _GPIO_MODE_DISABLED

/*--
if this:PinExist("H", 15) then
    this:AddWidget("Combobox")
    this:AddPinState("H", 15)
end
--*/
#define __GPIO_PH_PIN_15_STATE__ _LOW

/*----------------------------------------------------------------------------*/
/*--
if this:PinExist("I", 0) then
    local PI0 = {}
    PI0[#PI0 + 1] = {NAME = "PI0_NC", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}
    PI0[#PI0 + 1] = {NAME = "USX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PI0[#PI0 + 1] = {NAME = "USX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PI0[#PI0 + 1] = {NAME = "USX_CLK", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PI0[#PI0 + 1] = {NAME = "USX_CS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PI0[#PI0 + 1] = {NAME = "USX_CTS", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PI0[#PI0 + 1] = {NAME = "USX_RTS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PI0[#PI0 + 1] = {NAME = "LEUX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PI0[#PI0 + 1] = {NAME = "LEUX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PI0[#PI0 + 1] = {NAME = "I2CX_SCL", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PI0[#PI0 + 1] = {NAME = "I2CX_SDA", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PI0[#PI0 + 1] = {NAME = "ADCX", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}

    this:AddWidget("Textbox", "PI0")
    this:AddProposals(PI0, "I", 0)
end
--*/
#define __GPIO_PI_PIN_0_NAME__ PI0_NC

/*--
if this:PinExist("I", 0) then
    this:AddWidget("Combobox")
    this:AddPinMode("I", 0)
end
--*/
#define __GPIO_PI_PIN_0_MODE__ _GPIO_MODE_DISABLED

/*--
if this:PinExist("I", 0) then
    this:AddWidget("Combobox")
    this:AddPinState("I", 0)
end
--*/
#define __GPIO_PI_PIN_0_STATE__ _LOW

/*--
if this:PinExist("I", 1) then
    local PI1 = {}
    PI1[#PI1 + 1] = {NAME = "PI1_NC", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}
    PI1[#PI1 + 1] = {NAME = "USX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PI1[#PI1 + 1] = {NAME = "USX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PI1[#PI1 + 1] = {NAME = "USX_CLK", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PI1[#PI1 + 1] = {NAME = "USX_CS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PI1[#PI1 + 1] = {NAME = "USX_CTS", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PI1[#PI1 + 1] = {NAME = "USX_RTS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PI1[#PI1 + 1] = {NAME = "LEUX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PI1[#PI1 + 1] = {NAME = "LEUX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PI1[#PI1 + 1] = {NAME = "I2CX_SCL", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PI1[#PI1 + 1] = {NAME = "I2CX_SDA", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PI1[#PI1 + 1] = {NAME = "ADCX", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}

    this:AddWidget("Textbox", "PI1")
    this:AddProposals(PI1, "I", 1)
end
--*/
#define __GPIO_PI_PIN_1_NAME__ PI1_NC

/*--
if this:PinExist("I", 1) then
    this:AddWidget("Combobox")
    this:AddPinMode("I", 1)
end
--*/
#define __GPIO_PI_PIN_1_MODE__ _GPIO_MODE_DISABLED

/*--
if this:PinExist("I", 1) then
    this:AddWidget("Combobox")
    this:AddPinState("I", 1)
end
--*/
#define __GPIO_PI_PIN_1_STATE__ _LOW

/*--
if this:PinExist("I", 2) then
    local PI2 = {}
    PI2[#PI2 + 1] = {NAME = "PI2_NC", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}
    PI2[#PI2 + 1] = {NAME = "USX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PI2[#PI2 + 1] = {NAME = "USX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PI2[#PI2 + 1] = {NAME = "USX_CLK", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PI2[#PI2 + 1] = {NAME = "USX_CS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PI2[#PI2 + 1] = {NAME = "USX_CTS", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PI2[#PI2 + 1] = {NAME = "USX_RTS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PI2[#PI2 + 1] = {NAME = "LEUX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PI2[#PI2 + 1] = {NAME = "LEUX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PI2[#PI2 + 1] = {NAME = "I2CX_SCL", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PI2[#PI2 + 1] = {NAME = "I2CX_SDA", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PI2[#PI2 + 1] = {NAME = "ADCX", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}

    this:AddWidget("Textbox", "PI2")
    this:AddProposals(PI2, "I", 2)
end
--*/
#define __GPIO_PI_PIN_2_NAME__ PI2_NC

/*--
if this:PinExist("I", 2) then
    this:AddWidget("Combobox")
    this:AddPinMode("I", 2)
end
--*/
#define __GPIO_PI_PIN_2_MODE__ _GPIO_MODE_DISABLED

/*--
if this:PinExist("I", 2) then
    this:AddWidget("Combobox")
    this:AddPinState("I", 2)
end
--*/
#define __GPIO_PI_PIN_2_STATE__ _LOW

/*--
if this:PinExist("I", 3) then
    local PI3 = {}
    PI3[#PI3 + 1] = {NAME = "PI3_NC", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}
    PI3[#PI3 + 1] = {NAME = "USX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PI3[#PI3 + 1] = {NAME = "USX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PI3[#PI3 + 1] = {NAME = "USX_CLK", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PI3[#PI3 + 1] = {NAME = "USX_CS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PI3[#PI3 + 1] = {NAME = "USX_CTS", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PI3[#PI3 + 1] = {NAME = "USX_RTS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PI3[#PI3 + 1] = {NAME = "LEUX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PI3[#PI3 + 1] = {NAME = "LEUX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PI3[#PI3 + 1] = {NAME = "I2CX_SCL", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PI3[#PI3 + 1] = {NAME = "I2CX_SDA", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PI3[#PI3 + 1] = {NAME = "ADCX", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}

    this:AddWidget("Textbox", "PI3")
    this:AddProposals(PI3, "I", 3)
end
--*/
#define __GPIO_PI_PIN_3_NAME__ PI3_NC

/*--
if this:PinExist("I", 3) then
    this:AddWidget("Combobox")
    this:AddPinMode("I", 3)
end
--*/
#define __GPIO_PI_PIN_3_MODE__ _GPIO_MODE_DISABLED

/*--
if this:PinExist("I", 3) then
    this:AddWidget("Combobox")
    this:AddPinState("I", 3)
end
--*/
#define __GPIO_PI_PIN_3_STATE__ _LOW

/*--
if this:PinExist("I", 4) then
    local PI4 = {}
    PI4[#PI4 + 1] = {NAME = "PI4_NC", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}
    PI4[#PI4 + 1] = {NAME = "USX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PI4[#PI4 + 1] = {NAME = "USX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PI4[#PI4 + 1] = {NAME = "USX_CLK", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PI4[#PI4 + 1] = {NAME = "USX_CS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PI4[#PI4 + 1] = {NAME = "USX_CTS", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PI4[#PI4 + 1] = {NAME = "USX_RTS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PI4[#PI4 + 1] = {NAME = "LEUX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PI4[#PI4 + 1] = {NAME = "LEUX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PI4[#PI4 + 1] = {NAME = "I2CX_SCL", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PI4[#PI4 + 1] = {NAME = "I2CX_SDA", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PI4[#PI4 + 1] = {NAME = "ADCX", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}

    this:AddWidget("Textbox", "PI4")
    this:AddProposals(PI4, "I", 4)
end
--*/
#define __GPIO_PI_PIN_4_NAME__ PI4_NC

/*--
if this:PinExist("I", 4) then
    this:AddWidget("Combobox")
    this:AddPinMode("I", 4)
end
--*/
#define __GPIO_PI_PIN_4_MODE__ _GPIO_MODE_DISABLED

/*--
if this:PinExist("I", 4) then
    this:AddWidget("Combobox")
    this:AddPinState("I", 4)
end
--*/
#define __GPIO_PI_PIN_4_STATE__ _LOW

/*--
if this:PinExist("I", 5) then
    local PI5 = {}
    PI5[#PI5 + 1] = {NAME = "PI5_NC", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}
    PI5[#PI5 + 1] = {NAME = "USX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PI5[#PI5 + 1] = {NAME = "USX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PI5[#PI5 + 1] = {NAME = "USX_CLK", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PI5[#PI5 + 1] = {NAME = "USX_CS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PI5[#PI5 + 1] = {NAME = "USX_CTS", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PI5[#PI5 + 1] = {NAME = "USX_RTS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PI5[#PI5 + 1] = {NAME = "LEUX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PI5[#PI5 + 1] = {NAME = "LEUX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PI5[#PI5 + 1] = {NAME = "I2CX_SCL", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PI5[#PI5 + 1] = {NAME = "I2CX_SDA", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PI5[#PI5 + 1] = {NAME = "ADCX", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}

    this:AddWidget("Textbox", "PI5")
    this:AddProposals(PI5, "I", 5)
end
--*/
#define __GPIO_PI_PIN_5_NAME__ PI5_NC

/*--
if this:PinExist("I", 5) then
    this:AddWidget("Combobox")
    this:AddPinMode("I", 5)
end
--*/
#define __GPIO_PI_PIN_5_MODE__ _GPIO_MODE_DISABLED

/*--
if this:PinExist("I", 5) then
    this:AddWidget("Combobox")
    this:AddPinState("I", 5)
end
--*/
#define __GPIO_PI_PIN_5_STATE__ _LOW

/*--
if this:PinExist("I", 6) then
    local PI6 = {}
    PI6[#PI6 + 1] = {NAME = "PI6_NC", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}
    PI6[#PI6 + 1] = {NAME = "USX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PI6[#PI6 + 1] = {NAME = "USX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PI6[#PI6 + 1] = {NAME = "USX_CLK", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PI6[#PI6 + 1] = {NAME = "USX_CS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PI6[#PI6 + 1] = {NAME = "USX_CTS", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PI6[#PI6 + 1] = {NAME = "USX_RTS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PI6[#PI6 + 1] = {NAME = "LEUX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PI6[#PI6 + 1] = {NAME = "LEUX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PI6[#PI6 + 1] = {NAME = "I2CX_SCL", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PI6[#PI6 + 1] = {NAME = "I2CX_SDA", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PI6[#PI6 + 1] = {NAME = "ADCX", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}

    this:AddWidget("Textbox", "PI6")
    this:AddProposals(PI6, "I", 6)
end
--*/
#define __GPIO_PI_PIN_6_NAME__ PI6_NC

/*--
if this:PinExist("I", 6) then
    this:AddWidget("Combobox")
    this:AddPinMode("I", 6)
end
--*/
#define __GPIO_PI_PIN_6_MODE__ _GPIO_MODE_DISABLED

/*--
if this:PinExist("I", 6) then
    this:AddWidget("Combobox")
    this:AddPinState("I", 6)
end
--*/
#define __GPIO_PI_PIN_6_STATE__ _LOW

/*--
if this:PinExist("I", 7) then
    local PI7 = {}
    PI7[#PI7 + 1] = {NAME = "PI7_NC", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}
    PI7[#PI7 + 1] = {NAME = "USX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PI7[#PI7 + 1] = {NAME = "USX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PI7[#PI7 + 1] = {NAME = "USX_CLK", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PI7[#PI7 + 1] = {NAME = "USX_CS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PI7[#PI7 + 1] = {NAME = "USX_CTS", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PI7[#PI7 + 1] = {NAME = "USX_RTS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PI7[#PI7 + 1] = {NAME = "LEUX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PI7[#PI7 + 1] = {NAME = "LEUX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PI7[#PI7 + 1] = {NAME = "I2CX_SCL", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PI7[#PI7 + 1] = {NAME = "I2CX_SDA", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PI7[#PI7 + 1] = {NAME = "ADCX", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}

    this:AddWidget("Textbox", "PI7")
    this:AddProposals(PI7, "I", 7)
end
--*/
#define __GPIO_PI_PIN_7_NAME__ PI7_NC

/*--
if this:PinExist("I", 7) then
    this:AddWidget("Combobox")
    this:AddPinMode("I", 7)
end
--*/
#define __GPIO_PI_PIN_7_MODE__ _GPIO_MODE_DISABLED

/*--
if this:PinExist("I", 7) then
    this:AddWidget("Combobox")
    this:AddPinState("I", 7)
end
--*/
#define __GPIO_PI_PIN_7_STATE__ _LOW

/*--
if this:PinExist("I", 8) then
    local PI8 = {}
    PI8[#PI8 + 1] = {NAME = "PI8_NC", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}
    PI8[#PI8 + 1] = {NAME = "USX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PI8[#PI8 + 1] = {NAME = "USX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PI8[#PI8 + 1] = {NAME = "USX_CLK", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PI8[#PI8 + 1] = {NAME = "USX_CS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PI8[#PI8 + 1] = {NAME = "USX_CTS", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PI8[#PI8 + 1] = {NAME = "USX_RTS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PI8[#PI8 + 1] = {NAME = "LEUX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PI8[#PI8 + 1] = {NAME = "LEUX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PI8[#PI8 + 1] = {NAME = "I2CX_SCL", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PI8[#PI8 + 1] = {NAME = "I2CX_SDA", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PI8[#PI8 + 1] = {NAME = "ADCX", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}

    this:AddWidget("Textbox", "PI8")
    this:AddProposals(PI8, "I", 8)
end
--*/
#define __GPIO_PI_PIN_8_NAME__ PI8_NC

/*--
if this:PinExist("I", 8) then
    this:AddWidget("Combobox")
    this:AddPinMode("I", 8)
end
--*/
#define __GPIO_PI_PIN_8_MODE__ _GPIO_MODE_DISABLED

/*--
if this:PinExist("I", 8) then
    this:AddWidget("Combobox")
    this:AddPinState("I", 8)
end
--*/
#define __GPIO_PI_PIN_8_STATE__ _LOW

/*--
if this:PinExist("I", 9) then
    local PI9 = {}
    PI9[#PI9 + 1] = {NAME = "PI9_NC", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}
    PI9[#PI9 + 1] = {NAME = "USX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PI9[#PI9 + 1] = {NAME = "USX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PI9[#PI9 + 1] = {NAME = "USX_CLK", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PI9[#PI9 + 1] = {NAME = "USX_CS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PI9[#PI9 + 1] = {NAME = "USX_CTS", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PI9[#PI9 + 1] = {NAME = "USX_RTS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PI9[#PI9 + 1] = {NAME = "LEUX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PI9[#PI9 + 1] = {NAME = "LEUX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PI9[#PI9 + 1] = {NAME = "I2CX_SCL", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PI9[#PI9 + 1] = {NAME = "I2CX_SDA", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PI9[#PI9 + 1] = {NAME = "ADCX", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}

    this:AddWidget("Textbox", "PI9")
    this:AddProposals(PI9, "I", 9)
end
--*/
#define __GPIO_PI_PIN_9_NAME__ PI9_NC

/*--
if this:PinExist("I", 9) then
    this:AddWidget("Combobox")
    this:AddPinMode("I", 9)
end
--*/
#define __GPIO_PI_PIN_9_MODE__ _GPIO_MODE_DISABLED

/*--
if this:PinExist("I", 9) then
    this:AddWidget("Combobox")
    this:AddPinState("I", 9)
end
--*/
#define __GPIO_PI_PIN_9_STATE__ _LOW

/*--
if this:PinExist("I", 10) then
    local PI10 = {}
    PI10[#PI10 + 1] = {NAME = "PI10_NC", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}
    PI10[#PI10 + 1] = {NAME = "USX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PI10[#PI10 + 1] = {NAME = "USX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PI10[#PI10 + 1] = {NAME = "USX_CLK", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PI10[#PI10 + 1] = {NAME = "USX_CS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PI10[#PI10 + 1] = {NAME = "USX_CTS", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PI10[#PI10 + 1] = {NAME = "USX_RTS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PI10[#PI10 + 1] = {NAME = "LEUX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PI10[#PI10 + 1] = {NAME = "LEUX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PI10[#PI10 + 1] = {NAME = "I2CX_SCL", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PI10[#PI10 + 1] = {NAME = "I2CX_SDA", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PI10[#PI10 + 1] = {NAME = "ADCX", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}

    this:AddWidget("Textbox", "PI10")
    this:AddProposals(PI10, "I", 10)
end
--*/
#define __GPIO_PI_PIN_10_NAME__ PI10_NC

/*--
if this:PinExist("I", 10) then
    this:AddWidget("Combobox")
    this:AddPinMode("I", 10)
end
--*/
#define __GPIO_PI_PIN_10_MODE__ _GPIO_MODE_DISABLED

/*--
if this:PinExist("I", 10) then
    this:AddWidget("Combobox")
    this:AddPinState("I", 10)
end
--*/
#define __GPIO_PI_PIN_10_STATE__ _LOW

/*--
if this:PinExist("I", 11) then
    local PI11 = {}
    PI11[#PI11 + 1] = {NAME = "PI11_NC", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}
    PI11[#PI11 + 1] = {NAME = "USX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PI11[#PI11 + 1] = {NAME = "USX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PI11[#PI11 + 1] = {NAME = "USX_CLK", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PI11[#PI11 + 1] = {NAME = "USX_CS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PI11[#PI11 + 1] = {NAME = "USX_CTS", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PI11[#PI11 + 1] = {NAME = "USX_RTS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PI11[#PI11 + 1] = {NAME = "LEUX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PI11[#PI11 + 1] = {NAME = "LEUX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PI11[#PI11 + 1] = {NAME = "I2CX_SCL", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PI11[#PI11 + 1] = {NAME = "I2CX_SDA", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PI11[#PI11 + 1] = {NAME = "ADCX", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}

    this:AddWidget("Textbox", "PI11")
    this:AddProposals(PI11, "I", 11)
end
--*/
#define __GPIO_PI_PIN_11_NAME__ PI11_NC

/*--
if this:PinExist("I", 11) then
    this:AddWidget("Combobox")
    this:AddPinMode("I", 11)
end
--*/
#define __GPIO_PI_PIN_11_MODE__ _GPIO_MODE_DISABLED

/*--
if this:PinExist("I", 11) then
    this:AddWidget("Combobox")
    this:AddPinState("I", 11)
end
--*/
#define __GPIO_PI_PIN_11_STATE__ _LOW

/*--
if this:PinExist("I", 12) then
    local PI12 = {}
    PI12[#PI12 + 1] = {NAME = "PI12_NC", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}
    PI12[#PI12 + 1] = {NAME = "USX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PI12[#PI12 + 1] = {NAME = "USX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PI12[#PI12 + 1] = {NAME = "USX_CLK", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PI12[#PI12 + 1] = {NAME = "USX_CS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PI12[#PI12 + 1] = {NAME = "USX_CTS", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PI12[#PI12 + 1] = {NAME = "USX_RTS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PI12[#PI12 + 1] = {NAME = "LEUX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PI12[#PI12 + 1] = {NAME = "LEUX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PI12[#PI12 + 1] = {NAME = "I2CX_SCL", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PI12[#PI12 + 1] = {NAME = "I2CX_SDA", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PI12[#PI12 + 1] = {NAME = "ADCX", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}

    this:AddWidget("Textbox", "PI12")
    this:AddProposals(PI12, "I", 12)
end
--*/
#define __GPIO_PI_PIN_12_NAME__ PI12_NC

/*--
if this:PinExist("I", 12) then
    this:AddWidget("Combobox")
    this:AddPinMode("I", 12)
end
--*/
#define __GPIO_PI_PIN_12_MODE__ _GPIO_MODE_DISABLED

/*--
if this:PinExist("I", 12) then
    this:AddWidget("Combobox")
    this:AddPinState("I", 12)
end
--*/
#define __GPIO_PI_PIN_12_STATE__ _LOW

/*--
if this:PinExist("I", 13) then
    local PI13 = {}
    PI13[#PI13 + 1] = {NAME = "PI13_NC", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}
    PI13[#PI13 + 1] = {NAME = "USX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PI13[#PI13 + 1] = {NAME = "USX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PI13[#PI13 + 1] = {NAME = "USX_CLK", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PI13[#PI13 + 1] = {NAME = "USX_CS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PI13[#PI13 + 1] = {NAME = "USX_CTS", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PI13[#PI13 + 1] = {NAME = "USX_RTS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PI13[#PI13 + 1] = {NAME = "LEUX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PI13[#PI13 + 1] = {NAME = "LEUX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PI13[#PI13 + 1] = {NAME = "I2CX_SCL", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PI13[#PI13 + 1] = {NAME = "I2CX_SDA", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PI13[#PI13 + 1] = {NAME = "ADCX", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}

    this:AddWidget("Textbox", "PI13")
    this:AddProposals(PI13, "I", 13)
end
--*/
#define __GPIO_PI_PIN_13_NAME__ PI13_NC

/*--
if this:PinExist("I", 13) then
    this:AddWidget("Combobox")
    this:AddPinMode("I", 13)
end
--*/
#define __GPIO_PI_PIN_13_MODE__ _GPIO_MODE_DISABLED

/*--
if this:PinExist("I", 13) then
    this:AddWidget("Combobox")
    this:AddPinState("I", 13)
end
--*/
#define __GPIO_PI_PIN_13_STATE__ _LOW

/*--
if this:PinExist("I", 14) then
    local PI14 = {}
    PI14[#PI14 + 1] = {NAME = "PI14_NC", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}
    PI14[#PI14 + 1] = {NAME = "USX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PI14[#PI14 + 1] = {NAME = "USX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PI14[#PI14 + 1] = {NAME = "USX_CLK", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PI14[#PI14 + 1] = {NAME = "USX_CS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PI14[#PI14 + 1] = {NAME = "USX_CTS", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PI14[#PI14 + 1] = {NAME = "USX_RTS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PI14[#PI14 + 1] = {NAME = "LEUX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PI14[#PI14 + 1] = {NAME = "LEUX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PI14[#PI14 + 1] = {NAME = "I2CX_SCL", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PI14[#PI14 + 1] = {NAME = "I2CX_SDA", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PI14[#PI14 + 1] = {NAME = "ADCX", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}

    this:AddWidget("Textbox", "PI14")
    this:AddProposals(PI14, "I", 14)
end
--*/
#define __GPIO_PI_PIN_14_NAME__ PI14_NC

/*--
if this:PinExist("I", 14) then
    this:AddWidget("Combobox")
    this:AddPinMode("I", 14)
end
--*/
#define __GPIO_PI_PIN_14_MODE__ _GPIO_MODE_DISABLED

/*--
if this:PinExist("I", 14) then
    this:AddWidget("Combobox")
    this:AddPinState("I", 14)
end
--*/
#define __GPIO_PI_PIN_14_STATE__ _LOW

/*--
if this:PinExist("I", 15) then
    local PI15 = {}
    PI15[#PI15 + 1] = {NAME = "PI15_NC", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}
    PI15[#PI15 + 1] = {NAME = "USX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PI15[#PI15 + 1] = {NAME = "USX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PI15[#PI15 + 1] = {NAME = "USX_CLK", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PI15[#PI15 + 1] = {NAME = "USX_CS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PI15[#PI15 + 1] = {NAME = "USX_CTS", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PI15[#PI15 + 1] = {NAME = "USX_RTS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PI15[#PI15 + 1] = {NAME = "LEUX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PI15[#PI15 + 1] = {NAME = "LEUX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PI15[#PI15 + 1] = {NAME = "I2CX_SCL", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PI15[#PI15 + 1] = {NAME = "I2CX_SDA", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PI15[#PI15 + 1] = {NAME = "ADCX", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}

    this:AddWidget("Textbox", "PI15")
    this:AddProposals(PI15, "I", 15)
end
--*/
#define __GPIO_PI_PIN_15_NAME__ PI15_NC

/*--
if this:PinExist("I", 15) then
    this:AddWidget("Combobox")
    this:AddPinMode("I", 15)
end
--*/
#define __GPIO_PI_PIN_15_MODE__ _GPIO_MODE_DISABLED

/*--
if this:PinExist("I", 15) then
    this:AddWidget("Combobox")
    this:AddPinState("I", 15)
end
--*/
#define __GPIO_PI_PIN_15_STATE__ _LOW

/*----------------------------------------------------------------------------*/
/*--
if this:PinExist("J", 0) then
    local PJ0 = {}
    PJ0[#PJ0 + 1] = {NAME = "PJ0_NC", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}
    PJ0[#PJ0 + 1] = {NAME = "USX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PJ0[#PJ0 + 1] = {NAME = "USX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PJ0[#PJ0 + 1] = {NAME = "USX_CLK", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PJ0[#PJ0 + 1] = {NAME = "USX_CS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PJ0[#PJ0 + 1] = {NAME = "USX_CTS", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PJ0[#PJ0 + 1] = {NAME = "USX_RTS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PJ0[#PJ0 + 1] = {NAME = "LEUX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PJ0[#PJ0 + 1] = {NAME = "LEUX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PJ0[#PJ0 + 1] = {NAME = "I2CX_SCL", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PJ0[#PJ0 + 1] = {NAME = "I2CX_SDA", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PJ0[#PJ0 + 1] = {NAME = "ADCX", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}

    this:AddWidget("Textbox", "PJ0")
    this:AddProposals(PJ0, "J", 0)
end
--*/
#define __GPIO_PJ_PIN_0_NAME__ PJ0_NC

/*--
if this:PinExist("J", 0) then
    this:AddWidget("Combobox")
    this:AddPinMode("J", 0)
end
--*/
#define __GPIO_PJ_PIN_0_MODE__ _GPIO_MODE_DISABLED

/*--
if this:PinExist("J", 0) then
    this:AddWidget("Combobox")
    this:AddPinState("J", 0)
end
--*/
#define __GPIO_PJ_PIN_0_STATE__ _LOW

/*--
if this:PinExist("J", 1) then
    local PJ1 = {}
    PJ1[#PJ1 + 1] = {NAME = "PJ1_NC", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}
    PJ1[#PJ1 + 1] = {NAME = "USX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PJ1[#PJ1 + 1] = {NAME = "USX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PJ1[#PJ1 + 1] = {NAME = "USX_CLK", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PJ1[#PJ1 + 1] = {NAME = "USX_CS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PJ1[#PJ1 + 1] = {NAME = "USX_CTS", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PJ1[#PJ1 + 1] = {NAME = "USX_RTS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PJ1[#PJ1 + 1] = {NAME = "LEUX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PJ1[#PJ1 + 1] = {NAME = "LEUX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PJ1[#PJ1 + 1] = {NAME = "I2CX_SCL", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PJ1[#PJ1 + 1] = {NAME = "I2CX_SDA", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PJ1[#PJ1 + 1] = {NAME = "ADCX", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}

    this:AddWidget("Textbox", "PJ1")
    this:AddProposals(PJ1, "J", 1)
end
--*/
#define __GPIO_PJ_PIN_1_NAME__ PJ1_NC

/*--
if this:PinExist("J", 1) then
    this:AddWidget("Combobox")
    this:AddPinMode("J", 1)
end
--*/
#define __GPIO_PJ_PIN_1_MODE__ _GPIO_MODE_DISABLED

/*--
if this:PinExist("J", 1) then
    this:AddWidget("Combobox")
    this:AddPinState("J", 1)
end
--*/
#define __GPIO_PJ_PIN_1_STATE__ _LOW

/*--
if this:PinExist("J", 2) then
    local PJ2 = {}
    PJ2[#PJ2 + 1] = {NAME = "PJ2_NC", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}
    PJ2[#PJ2 + 1] = {NAME = "USX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PJ2[#PJ2 + 1] = {NAME = "USX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PJ2[#PJ2 + 1] = {NAME = "USX_CLK", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PJ2[#PJ2 + 1] = {NAME = "USX_CS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PJ2[#PJ2 + 1] = {NAME = "USX_CTS", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PJ2[#PJ2 + 1] = {NAME = "USX_RTS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PJ2[#PJ2 + 1] = {NAME = "LEUX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PJ2[#PJ2 + 1] = {NAME = "LEUX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PJ2[#PJ2 + 1] = {NAME = "I2CX_SCL", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PJ2[#PJ2 + 1] = {NAME = "I2CX_SDA", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PJ2[#PJ2 + 1] = {NAME = "ADCX", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}

    this:AddWidget("Textbox", "PJ2")
    this:AddProposals(PJ2, "J", 2)
end
--*/
#define __GPIO_PJ_PIN_2_NAME__ PJ2_NC

/*--
if this:PinExist("J", 2) then
    this:AddWidget("Combobox")
    this:AddPinMode("J", 2)
end
--*/
#define __GPIO_PJ_PIN_2_MODE__ _GPIO_MODE_DISABLED

/*--
if this:PinExist("J", 2) then
    this:AddWidget("Combobox")
    this:AddPinState("J", 2)
end
--*/
#define __GPIO_PJ_PIN_2_STATE__ _LOW

/*--
if this:PinExist("J", 3) then
    local PJ3 = {}
    PJ3[#PJ3 + 1] = {NAME = "PJ3_NC", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}
    PJ3[#PJ3 + 1] = {NAME = "USX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PJ3[#PJ3 + 1] = {NAME = "USX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PJ3[#PJ3 + 1] = {NAME = "USX_CLK", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PJ3[#PJ3 + 1] = {NAME = "USX_CS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PJ3[#PJ3 + 1] = {NAME = "USX_CTS", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PJ3[#PJ3 + 1] = {NAME = "USX_RTS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PJ3[#PJ3 + 1] = {NAME = "LEUX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PJ3[#PJ3 + 1] = {NAME = "LEUX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PJ3[#PJ3 + 1] = {NAME = "I2CX_SCL", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PJ3[#PJ3 + 1] = {NAME = "I2CX_SDA", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PJ3[#PJ3 + 1] = {NAME = "ADCX", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}

    this:AddWidget("Textbox", "PJ3")
    this:AddProposals(PJ3, "J", 3)
end
--*/
#define __GPIO_PJ_PIN_3_NAME__ PJ3_NC

/*--
if this:PinExist("J", 3) then
    this:AddWidget("Combobox")
    this:AddPinMode("J", 3)
end
--*/
#define __GPIO_PJ_PIN_3_MODE__ _GPIO_MODE_DISABLED

/*--
if this:PinExist("J", 3) then
    this:AddWidget("Combobox")
    this:AddPinState("J", 3)
end
--*/
#define __GPIO_PJ_PIN_3_STATE__ _LOW

/*--
if this:PinExist("J", 4) then
    local PJ4 = {}
    PJ4[#PJ4 + 1] = {NAME = "PJ4_NC", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}
    PJ4[#PJ4 + 1] = {NAME = "USX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PJ4[#PJ4 + 1] = {NAME = "USX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PJ4[#PJ4 + 1] = {NAME = "USX_CLK", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PJ4[#PJ4 + 1] = {NAME = "USX_CS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PJ4[#PJ4 + 1] = {NAME = "USX_CTS", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PJ4[#PJ4 + 1] = {NAME = "USX_RTS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PJ4[#PJ4 + 1] = {NAME = "LEUX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PJ4[#PJ4 + 1] = {NAME = "LEUX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PJ4[#PJ4 + 1] = {NAME = "I2CX_SCL", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PJ4[#PJ4 + 1] = {NAME = "I2CX_SDA", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PJ4[#PJ4 + 1] = {NAME = "ADCX", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}

    this:AddWidget("Textbox", "PJ4")
    this:AddProposals(PJ4, "J", 4)
end
--*/
#define __GPIO_PJ_PIN_4_NAME__ PJ4_NC

/*--
if this:PinExist("J", 4) then
    this:AddWidget("Combobox")
    this:AddPinMode("J", 4)
end
--*/
#define __GPIO_PJ_PIN_4_MODE__ _GPIO_MODE_DISABLED

/*--
if this:PinExist("J", 4) then
    this:AddWidget("Combobox")
    this:AddPinState("J", 4)
end
--*/
#define __GPIO_PJ_PIN_4_STATE__ _LOW

/*--
if this:PinExist("J", 5) then
    local PJ5 = {}
    PJ5[#PJ5 + 1] = {NAME = "PJ5_NC", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}
    PJ5[#PJ5 + 1] = {NAME = "USX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PJ5[#PJ5 + 1] = {NAME = "USX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PJ5[#PJ5 + 1] = {NAME = "USX_CLK", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PJ5[#PJ5 + 1] = {NAME = "USX_CS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PJ5[#PJ5 + 1] = {NAME = "USX_CTS", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PJ5[#PJ5 + 1] = {NAME = "USX_RTS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PJ5[#PJ5 + 1] = {NAME = "LEUX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PJ5[#PJ5 + 1] = {NAME = "LEUX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PJ5[#PJ5 + 1] = {NAME = "I2CX_SCL", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PJ5[#PJ5 + 1] = {NAME = "I2CX_SDA", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PJ5[#PJ5 + 1] = {NAME = "ADCX", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}

    this:AddWidget("Textbox", "PJ5")
    this:AddProposals(PJ5, "J", 5)
end
--*/
#define __GPIO_PJ_PIN_5_NAME__ PJ5_NC

/*--
if this:PinExist("J", 5) then
    this:AddWidget("Combobox")
    this:AddPinMode("J", 5)
end
--*/
#define __GPIO_PJ_PIN_5_MODE__ _GPIO_MODE_DISABLED

/*--
if this:PinExist("J", 5) then
    this:AddWidget("Combobox")
    this:AddPinState("J", 5)
end
--*/
#define __GPIO_PJ_PIN_5_STATE__ _LOW

/*--
if this:PinExist("J", 6) then
    local PJ6 = {}
    PJ6[#PJ6 + 1] = {NAME = "PJ6_NC", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}
    PJ6[#PJ6 + 1] = {NAME = "USX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PJ6[#PJ6 + 1] = {NAME = "USX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PJ6[#PJ6 + 1] = {NAME = "USX_CLK", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PJ6[#PJ6 + 1] = {NAME = "USX_CS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PJ6[#PJ6 + 1] = {NAME = "USX_CTS", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PJ6[#PJ6 + 1] = {NAME = "USX_RTS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PJ6[#PJ6 + 1] = {NAME = "LEUX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PJ6[#PJ6 + 1] = {NAME = "LEUX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PJ6[#PJ6 + 1] = {NAME = "I2CX_SCL", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PJ6[#PJ6 + 1] = {NAME = "I2CX_SDA", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PJ6[#PJ6 + 1] = {NAME = "ADCX", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}

    this:AddWidget("Textbox", "PJ6")
    this:AddProposals(PJ6, "J", 6)
end
--*/
#define __GPIO_PJ_PIN_6_NAME__ PJ6_NC

/*--
if this:PinExist("J", 6) then
    this:AddWidget("Combobox")
    this:AddPinMode("J", 6)
end
--*/
#define __GPIO_PJ_PIN_6_MODE__ _GPIO_MODE_DISABLED

/*--
if this:PinExist("J", 6) then
    this:AddWidget("Combobox")
    this:AddPinState("J", 6)
end
--*/
#define __GPIO_PJ_PIN_6_STATE__ _LOW

/*--
if this:PinExist("J", 7) then
    local PJ7 = {}
    PJ7[#PJ7 + 1] = {NAME = "PJ7_NC", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}
    PJ7[#PJ7 + 1] = {NAME = "USX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PJ7[#PJ7 + 1] = {NAME = "USX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PJ7[#PJ7 + 1] = {NAME = "USX_CLK", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PJ7[#PJ7 + 1] = {NAME = "USX_CS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PJ7[#PJ7 + 1] = {NAME = "USX_CTS", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PJ7[#PJ7 + 1] = {NAME = "USX_RTS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PJ7[#PJ7 + 1] = {NAME = "LEUX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PJ7[#PJ7 + 1] = {NAME = "LEUX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PJ7[#PJ7 + 1] = {NAME = "I2CX_SCL", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PJ7[#PJ7 + 1] = {NAME = "I2CX_SDA", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PJ7[#PJ7 + 1] = {NAME = "ADCX", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}

    this:AddWidget("Textbox", "PJ7")
    this:AddProposals(PJ7, "J", 7)
end
--*/
#define __GPIO_PJ_PIN_7_NAME__ PJ7_NC

/*--
if this:PinExist("J", 7) then
    this:AddWidget("Combobox")
    this:AddPinMode("J", 7)
end
--*/
#define __GPIO_PJ_PIN_7_MODE__ _GPIO_MODE_DISABLED

/*--
if this:PinExist("J", 7) then
    this:AddWidget("Combobox")
    this:AddPinState("J", 7)
end
--*/
#define __GPIO_PJ_PIN_7_STATE__ _LOW

/*--
if this:PinExist("J", 8) then
    local PJ8 = {}
    PJ8[#PJ8 + 1] = {NAME = "PJ8_NC", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}
    PJ8[#PJ8 + 1] = {NAME = "USX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PJ8[#PJ8 + 1] = {NAME = "USX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PJ8[#PJ8 + 1] = {NAME = "USX_CLK", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PJ8[#PJ8 + 1] = {NAME = "USX_CS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PJ8[#PJ8 + 1] = {NAME = "USX_CTS", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PJ8[#PJ8 + 1] = {NAME = "USX_RTS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PJ8[#PJ8 + 1] = {NAME = "LEUX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PJ8[#PJ8 + 1] = {NAME = "LEUX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PJ8[#PJ8 + 1] = {NAME = "I2CX_SCL", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PJ8[#PJ8 + 1] = {NAME = "I2CX_SDA", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PJ8[#PJ8 + 1] = {NAME = "ADCX", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}

    this:AddWidget("Textbox", "PJ8")
    this:AddProposals(PJ8, "J", 8)
end
--*/
#define __GPIO_PJ_PIN_8_NAME__ PJ8_NC

/*--
if this:PinExist("J", 8) then
    this:AddWidget("Combobox")
    this:AddPinMode("J", 8)
end
--*/
#define __GPIO_PJ_PIN_8_MODE__ _GPIO_MODE_DISABLED

/*--
if this:PinExist("J", 8) then
    this:AddWidget("Combobox")
    this:AddPinState("J", 8)
end
--*/
#define __GPIO_PJ_PIN_8_STATE__ _LOW

/*--
if this:PinExist("J", 9) then
    local PJ9 = {}
    PJ9[#PJ9 + 1] = {NAME = "PJ9_NC", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}
    PJ9[#PJ9 + 1] = {NAME = "USX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PJ9[#PJ9 + 1] = {NAME = "USX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PJ9[#PJ9 + 1] = {NAME = "USX_CLK", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PJ9[#PJ9 + 1] = {NAME = "USX_CS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PJ9[#PJ9 + 1] = {NAME = "USX_CTS", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PJ9[#PJ9 + 1] = {NAME = "USX_RTS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PJ9[#PJ9 + 1] = {NAME = "LEUX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PJ9[#PJ9 + 1] = {NAME = "LEUX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PJ9[#PJ9 + 1] = {NAME = "I2CX_SCL", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PJ9[#PJ9 + 1] = {NAME = "I2CX_SDA", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PJ9[#PJ9 + 1] = {NAME = "ADCX", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}

    this:AddWidget("Textbox", "PJ9")
    this:AddProposals(PJ9, "J", 9)
end
--*/
#define __GPIO_PJ_PIN_9_NAME__ PJ9_NC

/*--
if this:PinExist("J", 9) then
    this:AddWidget("Combobox")
    this:AddPinMode("J", 9)
end
--*/
#define __GPIO_PJ_PIN_9_MODE__ _GPIO_MODE_DISABLED

/*--
if this:PinExist("J", 9) then
    this:AddWidget("Combobox")
    this:AddPinState("J", 9)
end
--*/
#define __GPIO_PJ_PIN_9_STATE__ _LOW

/*--
if this:PinExist("J", 10) then
    local PJ10 = {}
    PJ10[#PJ10 + 1] = {NAME = "PJ10_NC", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}
    PJ10[#PJ10 + 1] = {NAME = "USX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PJ10[#PJ10 + 1] = {NAME = "USX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PJ10[#PJ10 + 1] = {NAME = "USX_CLK", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PJ10[#PJ10 + 1] = {NAME = "USX_CS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PJ10[#PJ10 + 1] = {NAME = "USX_CTS", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PJ10[#PJ10 + 1] = {NAME = "USX_RTS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PJ10[#PJ10 + 1] = {NAME = "LEUX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PJ10[#PJ10 + 1] = {NAME = "LEUX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PJ10[#PJ10 + 1] = {NAME = "I2CX_SCL", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PJ10[#PJ10 + 1] = {NAME = "I2CX_SDA", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PJ10[#PJ10 + 1] = {NAME = "ADCX", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}

    this:AddWidget("Textbox", "PJ10")
    this:AddProposals(PJ10, "J", 10)
end
--*/
#define __GPIO_PJ_PIN_10_NAME__ PJ10_NC

/*--
if this:PinExist("J", 10) then
    this:AddWidget("Combobox")
    this:AddPinMode("J", 10)
end
--*/
#define __GPIO_PJ_PIN_10_MODE__ _GPIO_MODE_DISABLED

/*--
if this:PinExist("J", 10) then
    this:AddWidget("Combobox")
    this:AddPinState("J", 10)
end
--*/
#define __GPIO_PJ_PIN_10_STATE__ _LOW

/*--
if this:PinExist("J", 11) then
    local PJ11 = {}
    PJ11[#PJ11 + 1] = {NAME = "PJ11_NC", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}
    PJ11[#PJ11 + 1] = {NAME = "USX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PJ11[#PJ11 + 1] = {NAME = "USX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PJ11[#PJ11 + 1] = {NAME = "USX_CLK", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PJ11[#PJ11 + 1] = {NAME = "USX_CS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PJ11[#PJ11 + 1] = {NAME = "USX_CTS", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PJ11[#PJ11 + 1] = {NAME = "USX_RTS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PJ11[#PJ11 + 1] = {NAME = "LEUX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PJ11[#PJ11 + 1] = {NAME = "LEUX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PJ11[#PJ11 + 1] = {NAME = "I2CX_SCL", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PJ11[#PJ11 + 1] = {NAME = "I2CX_SDA", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PJ11[#PJ11 + 1] = {NAME = "ADCX", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}

    this:AddWidget("Textbox", "PJ11")
    this:AddProposals(PJ11, "J", 11)
end
--*/
#define __GPIO_PJ_PIN_11_NAME__ PJ11_NC

/*--
if this:PinExist("J", 11) then
    this:AddWidget("Combobox")
    this:AddPinMode("J", 11)
end
--*/
#define __GPIO_PJ_PIN_11_MODE__ _GPIO_MODE_DISABLED

/*--
if this:PinExist("J", 11) then
    this:AddWidget("Combobox")
    this:AddPinState("J", 11)
end
--*/
#define __GPIO_PJ_PIN_11_STATE__ _LOW

/*--
if this:PinExist("J", 12) then
    local PJ12 = {}
    PJ12[#PJ12 + 1] = {NAME = "PJ12_NC", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}
    PJ12[#PJ12 + 1] = {NAME = "USX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PJ12[#PJ12 + 1] = {NAME = "USX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PJ12[#PJ12 + 1] = {NAME = "USX_CLK", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PJ12[#PJ12 + 1] = {NAME = "USX_CS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PJ12[#PJ12 + 1] = {NAME = "USX_CTS", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PJ12[#PJ12 + 1] = {NAME = "USX_RTS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PJ12[#PJ12 + 1] = {NAME = "LEUX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PJ12[#PJ12 + 1] = {NAME = "LEUX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PJ12[#PJ12 + 1] = {NAME = "I2CX_SCL", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PJ12[#PJ12 + 1] = {NAME = "I2CX_SDA", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PJ12[#PJ12 + 1] = {NAME = "ADCX", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}

    this:AddWidget("Textbox", "PJ12")
    this:AddProposals(PJ12, "J", 12)
end
--*/
#define __GPIO_PJ_PIN_12_NAME__ PJ12_NC

/*--
if this:PinExist("J", 12) then
    this:AddWidget("Combobox")
    this:AddPinMode("J", 12)
end
--*/
#define __GPIO_PJ_PIN_12_MODE__ _GPIO_MODE_DISABLED

/*--
if this:PinExist("J", 12) then
    this:AddWidget("Combobox")
    this:AddPinState("J", 12)
end
--*/
#define __GPIO_PJ_PIN_12_STATE__ _LOW

/*--
if this:PinExist("J", 13) then
    local PJ13 = {}
    PJ13[#PJ13 + 1] = {NAME = "PJ13_NC", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}
    PJ13[#PJ13 + 1] = {NAME = "USX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PJ13[#PJ13 + 1] = {NAME = "USX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PJ13[#PJ13 + 1] = {NAME = "USX_CLK", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PJ13[#PJ13 + 1] = {NAME = "USX_CS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PJ13[#PJ13 + 1] = {NAME = "USX_CTS", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PJ13[#PJ13 + 1] = {NAME = "USX_RTS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PJ13[#PJ13 + 1] = {NAME = "LEUX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PJ13[#PJ13 + 1] = {NAME = "LEUX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PJ13[#PJ13 + 1] = {NAME = "I2CX_SCL", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PJ13[#PJ13 + 1] = {NAME = "I2CX_SDA", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PJ13[#PJ13 + 1] = {NAME = "ADCX", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}

    this:AddWidget("Textbox", "PJ13")
    this:AddProposals(PJ13, "J", 13)
end
--*/
#define __GPIO_PJ_PIN_13_NAME__ PJ13_NC

/*--
if this:PinExist("J", 13) then
    this:AddWidget("Combobox")
    this:AddPinMode("J", 13)
end
--*/
#define __GPIO_PJ_PIN_13_MODE__ _GPIO_MODE_DISABLED

/*--
if this:PinExist("J", 13) then
    this:AddWidget("Combobox")
    this:AddPinState("J", 13)
end
--*/
#define __GPIO_PJ_PIN_13_STATE__ _LOW

/*--
if this:PinExist("J", 14) then
    local PJ14 = {}
    PJ14[#PJ14 + 1] = {NAME = "PJ14_NC", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}
    PJ14[#PJ14 + 1] = {NAME = "USX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PJ14[#PJ14 + 1] = {NAME = "USX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PJ14[#PJ14 + 1] = {NAME = "USX_CLK", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PJ14[#PJ14 + 1] = {NAME = "USX_CS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PJ14[#PJ14 + 1] = {NAME = "USX_CTS", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PJ14[#PJ14 + 1] = {NAME = "USX_RTS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PJ14[#PJ14 + 1] = {NAME = "LEUX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PJ14[#PJ14 + 1] = {NAME = "LEUX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PJ14[#PJ14 + 1] = {NAME = "I2CX_SCL", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PJ14[#PJ14 + 1] = {NAME = "I2CX_SDA", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PJ14[#PJ14 + 1] = {NAME = "ADCX", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}

    this:AddWidget("Textbox", "PJ14")
    this:AddProposals(PJ14, "J", 14)
end
--*/
#define __GPIO_PJ_PIN_14_NAME__ PJ14_NC

/*--
if this:PinExist("J", 14) then
    this:AddWidget("Combobox")
    this:AddPinMode("J", 14)
end
--*/
#define __GPIO_PJ_PIN_14_MODE__ _GPIO_MODE_DISABLED

/*--
if this:PinExist("J", 14) then
    this:AddWidget("Combobox")
    this:AddPinState("J", 14)
end
--*/
#define __GPIO_PJ_PIN_14_STATE__ _LOW

/*--
if this:PinExist("J", 15) then
    local PJ15 = {}
    PJ15[#PJ15 + 1] = {NAME = "PJ15_NC", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}
    PJ15[#PJ15 + 1] = {NAME = "USX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PJ15[#PJ15 + 1] = {NAME = "USX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PJ15[#PJ15 + 1] = {NAME = "USX_CLK", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PJ15[#PJ15 + 1] = {NAME = "USX_CS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PJ15[#PJ15 + 1] = {NAME = "USX_CTS", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PJ15[#PJ15 + 1] = {NAME = "USX_RTS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PJ15[#PJ15 + 1] = {NAME = "LEUX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PJ15[#PJ15 + 1] = {NAME = "LEUX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PJ15[#PJ15 + 1] = {NAME = "I2CX_SCL", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PJ15[#PJ15 + 1] = {NAME = "I2CX_SDA", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PJ15[#PJ15 + 1] = {NAME = "ADCX", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}

    this:AddWidget("Textbox", "PJ15")
    this:AddProposals(PJ15, "J", 15)
end
--*/
#define __GPIO_PJ_PIN_15_NAME__ PJ15_NC

/*--
if this:PinExist("J", 15) then
    this:AddWidget("Combobox")
    this:AddPinMode("J", 15)
end
--*/
#define __GPIO_PJ_PIN_15_MODE__ _GPIO_MODE_DISABLED

/*--
if this:PinExist("J", 15) then
    this:AddWidget("Combobox")
    this:AddPinState("J", 15)
end
--*/
#define __GPIO_PJ_PIN_15_STATE__ _LOW

/*----------------------------------------------------------------------------*/
/*--
if this:PinExist("K", 0) then
    local PK0 = {}
    PK0[#PK0 + 1] = {NAME = "PK0_NC", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}
    PK0[#PK0 + 1] = {NAME = "USX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PK0[#PK0 + 1] = {NAME = "USX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PK0[#PK0 + 1] = {NAME = "USX_CLK", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PK0[#PK0 + 1] = {NAME = "USX_CS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PK0[#PK0 + 1] = {NAME = "USX_CTS", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PK0[#PK0 + 1] = {NAME = "USX_RTS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PK0[#PK0 + 1] = {NAME = "LEUX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PK0[#PK0 + 1] = {NAME = "LEUX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PK0[#PK0 + 1] = {NAME = "I2CX_SCL", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PK0[#PK0 + 1] = {NAME = "I2CX_SDA", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PK0[#PK0 + 1] = {NAME = "ADCX", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}

    this:AddWidget("Textbox", "PK0")
    this:AddProposals(PK0, "K", 0)
end
--*/
#define __GPIO_PK_PIN_0_NAME__ PK0_NC

/*--
if this:PinExist("K", 0) then
    this:AddWidget("Combobox")
    this:AddPinMode("K", 0)
end
--*/
#define __GPIO_PK_PIN_0_MODE__ _GPIO_MODE_DISABLED

/*--
if this:PinExist("K", 0) then
    this:AddWidget("Combobox")
    this:AddPinState("K", 0)
end
--*/
#define __GPIO_PK_PIN_0_STATE__ _LOW

/*--
if this:PinExist("K", 1) then
    local PK1 = {}
    PK1[#PK1 + 1] = {NAME = "PK1_NC", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}
    PK1[#PK1 + 1] = {NAME = "USX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PK1[#PK1 + 1] = {NAME = "USX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PK1[#PK1 + 1] = {NAME = "USX_CLK", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PK1[#PK1 + 1] = {NAME = "USX_CS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PK1[#PK1 + 1] = {NAME = "USX_CTS", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PK1[#PK1 + 1] = {NAME = "USX_RTS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PK1[#PK1 + 1] = {NAME = "LEUX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PK1[#PK1 + 1] = {NAME = "LEUX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PK1[#PK1 + 1] = {NAME = "I2CX_SCL", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PK1[#PK1 + 1] = {NAME = "I2CX_SDA", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PK1[#PK1 + 1] = {NAME = "ADCX", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}

    this:AddWidget("Textbox", "PK1")
    this:AddProposals(PK1, "K", 1)
end
--*/
#define __GPIO_PK_PIN_1_NAME__ PK1_NC

/*--
if this:PinExist("K", 1) then
    this:AddWidget("Combobox")
    this:AddPinMode("K", 1)
end
--*/
#define __GPIO_PK_PIN_1_MODE__ _GPIO_MODE_DISABLED

/*--
if this:PinExist("K", 1) then
    this:AddWidget("Combobox")
    this:AddPinState("K", 1)
end
--*/
#define __GPIO_PK_PIN_1_STATE__ _LOW

/*--
if this:PinExist("K", 2) then
    local PK2 = {}
    PK2[#PK2 + 1] = {NAME = "PK2_NC", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}
    PK2[#PK2 + 1] = {NAME = "USX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PK2[#PK2 + 1] = {NAME = "USX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PK2[#PK2 + 1] = {NAME = "USX_CLK", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PK2[#PK2 + 1] = {NAME = "USX_CS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PK2[#PK2 + 1] = {NAME = "USX_CTS", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PK2[#PK2 + 1] = {NAME = "USX_RTS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PK2[#PK2 + 1] = {NAME = "LEUX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PK2[#PK2 + 1] = {NAME = "LEUX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PK2[#PK2 + 1] = {NAME = "I2CX_SCL", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PK2[#PK2 + 1] = {NAME = "I2CX_SDA", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PK2[#PK2 + 1] = {NAME = "ADCX", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}

    this:AddWidget("Textbox", "PK2")
    this:AddProposals(PK2, "K", 2)
end
--*/
#define __GPIO_PK_PIN_2_NAME__ PK2_NC

/*--
if this:PinExist("K", 2) then
    this:AddWidget("Combobox")
    this:AddPinMode("K", 2)
end
--*/
#define __GPIO_PK_PIN_2_MODE__ _GPIO_MODE_DISABLED

/*--
if this:PinExist("K", 2) then
    this:AddWidget("Combobox")
    this:AddPinState("K", 2)
end
--*/
#define __GPIO_PK_PIN_2_STATE__ _LOW

/*--
if this:PinExist("K", 3) then
    local PK3 = {}
    PK3[#PK3 + 1] = {NAME = "PK3_NC", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}
    PK3[#PK3 + 1] = {NAME = "USX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PK3[#PK3 + 1] = {NAME = "USX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PK3[#PK3 + 1] = {NAME = "USX_CLK", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PK3[#PK3 + 1] = {NAME = "USX_CS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PK3[#PK3 + 1] = {NAME = "USX_CTS", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PK3[#PK3 + 1] = {NAME = "USX_RTS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PK3[#PK3 + 1] = {NAME = "LEUX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PK3[#PK3 + 1] = {NAME = "LEUX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PK3[#PK3 + 1] = {NAME = "I2CX_SCL", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PK3[#PK3 + 1] = {NAME = "I2CX_SDA", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PK3[#PK3 + 1] = {NAME = "ADCX", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}

    this:AddWidget("Textbox", "PK3")
    this:AddProposals(PK3, "K", 3)
end
--*/
#define __GPIO_PK_PIN_3_NAME__ PK3_NC

/*--
if this:PinExist("K", 3) then
    this:AddWidget("Combobox")
    this:AddPinMode("K", 3)
end
--*/
#define __GPIO_PK_PIN_3_MODE__ _GPIO_MODE_DISABLED

/*--
if this:PinExist("K", 3) then
    this:AddWidget("Combobox")
    this:AddPinState("K", 3)
end
--*/
#define __GPIO_PK_PIN_3_STATE__ _LOW

/*--
if this:PinExist("K", 4) then
    local PK4 = {}
    PK4[#PK4 + 1] = {NAME = "PK4_NC", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}
    PK4[#PK4 + 1] = {NAME = "USX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PK4[#PK4 + 1] = {NAME = "USX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PK4[#PK4 + 1] = {NAME = "USX_CLK", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PK4[#PK4 + 1] = {NAME = "USX_CS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PK4[#PK4 + 1] = {NAME = "USX_CTS", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PK4[#PK4 + 1] = {NAME = "USX_RTS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PK4[#PK4 + 1] = {NAME = "LEUX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PK4[#PK4 + 1] = {NAME = "LEUX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PK4[#PK4 + 1] = {NAME = "I2CX_SCL", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PK4[#PK4 + 1] = {NAME = "I2CX_SDA", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PK4[#PK4 + 1] = {NAME = "ADCX", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}

    this:AddWidget("Textbox", "PK4")
    this:AddProposals(PK4, "K", 4)
end
--*/
#define __GPIO_PK_PIN_4_NAME__ PK4_NC

/*--
if this:PinExist("K", 4) then
    this:AddWidget("Combobox")
    this:AddPinMode("K", 4)
end
--*/
#define __GPIO_PK_PIN_4_MODE__ _GPIO_MODE_DISABLED

/*--
if this:PinExist("K", 4) then
    this:AddWidget("Combobox")
    this:AddPinState("K", 4)
end
--*/
#define __GPIO_PK_PIN_4_STATE__ _LOW

/*--
if this:PinExist("K", 5) then
    local PK5 = {}
    PK5[#PK5 + 1] = {NAME = "PK5_NC", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}
    PK5[#PK5 + 1] = {NAME = "USX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PK5[#PK5 + 1] = {NAME = "USX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PK5[#PK5 + 1] = {NAME = "USX_CLK", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PK5[#PK5 + 1] = {NAME = "USX_CS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PK5[#PK5 + 1] = {NAME = "USX_CTS", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PK5[#PK5 + 1] = {NAME = "USX_RTS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PK5[#PK5 + 1] = {NAME = "LEUX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PK5[#PK5 + 1] = {NAME = "LEUX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PK5[#PK5 + 1] = {NAME = "I2CX_SCL", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PK5[#PK5 + 1] = {NAME = "I2CX_SDA", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PK5[#PK5 + 1] = {NAME = "ADCX", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}

    this:AddWidget("Textbox", "PK5")
    this:AddProposals(PK5, "K", 5)
end
--*/
#define __GPIO_PK_PIN_5_NAME__ PK5_NC

/*--
if this:PinExist("K", 5) then
    this:AddWidget("Combobox")
    this:AddPinMode("K", 5)
end
--*/
#define __GPIO_PK_PIN_5_MODE__ _GPIO_MODE_DISABLED

/*--
if this:PinExist("K", 5) then
    this:AddWidget("Combobox")
    this:AddPinState("K", 5)
end
--*/
#define __GPIO_PK_PIN_5_STATE__ _LOW

/*--
if this:PinExist("K", 6) then
    local PK6 = {}
    PK6[#PK6 + 1] = {NAME = "PK6_NC", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}
    PK6[#PK6 + 1] = {NAME = "USX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PK6[#PK6 + 1] = {NAME = "USX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PK6[#PK6 + 1] = {NAME = "USX_CLK", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PK6[#PK6 + 1] = {NAME = "USX_CS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PK6[#PK6 + 1] = {NAME = "USX_CTS", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PK6[#PK6 + 1] = {NAME = "USX_RTS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PK6[#PK6 + 1] = {NAME = "LEUX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PK6[#PK6 + 1] = {NAME = "LEUX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PK6[#PK6 + 1] = {NAME = "I2CX_SCL", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PK6[#PK6 + 1] = {NAME = "I2CX_SDA", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PK6[#PK6 + 1] = {NAME = "ADCX", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}

    this:AddWidget("Textbox", "PK6")
    this:AddProposals(PK6, "K", 6)
end
--*/
#define __GPIO_PK_PIN_6_NAME__ PK6_NC

/*--
if this:PinExist("K", 6) then
    this:AddWidget("Combobox")
    this:AddPinMode("K", 6)
end
--*/
#define __GPIO_PK_PIN_6_MODE__ _GPIO_MODE_DISABLED

/*--
if this:PinExist("K", 6) then
    this:AddWidget("Combobox")
    this:AddPinState("K", 6)
end
--*/
#define __GPIO_PK_PIN_6_STATE__ _LOW

/*--
if this:PinExist("K", 7) then
    local PK7 = {}
    PK7[#PK7 + 1] = {NAME = "PK7_NC", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}
    PK7[#PK7 + 1] = {NAME = "USX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PK7[#PK7 + 1] = {NAME = "USX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PK7[#PK7 + 1] = {NAME = "USX_CLK", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PK7[#PK7 + 1] = {NAME = "USX_CS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PK7[#PK7 + 1] = {NAME = "USX_CTS", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PK7[#PK7 + 1] = {NAME = "USX_RTS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PK7[#PK7 + 1] = {NAME = "LEUX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PK7[#PK7 + 1] = {NAME = "LEUX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PK7[#PK7 + 1] = {NAME = "I2CX_SCL", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PK7[#PK7 + 1] = {NAME = "I2CX_SDA", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PK7[#PK7 + 1] = {NAME = "ADCX", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}

    this:AddWidget("Textbox", "PK7")
    this:AddProposals(PK7, "K", 7)
end
--*/
#define __GPIO_PK_PIN_7_NAME__ PK7_NC

/*--
if this:PinExist("K", 7) then
    this:AddWidget("Combobox")
    this:AddPinMode("K", 7)
end
--*/
#define __GPIO_PK_PIN_7_MODE__ _GPIO_MODE_DISABLED

/*--
if this:PinExist("K", 7) then
    this:AddWidget("Combobox")
    this:AddPinState("K", 7)
end
--*/
#define __GPIO_PK_PIN_7_STATE__ _LOW

/*--
if this:PinExist("K", 8) then
    local PK8 = {}
    PK8[#PK8 + 1] = {NAME = "PK8_NC", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}
    PK8[#PK8 + 1] = {NAME = "USX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PK8[#PK8 + 1] = {NAME = "USX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PK8[#PK8 + 1] = {NAME = "USX_CLK", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PK8[#PK8 + 1] = {NAME = "USX_CS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PK8[#PK8 + 1] = {NAME = "USX_CTS", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PK8[#PK8 + 1] = {NAME = "USX_RTS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PK8[#PK8 + 1] = {NAME = "LEUX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PK8[#PK8 + 1] = {NAME = "LEUX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PK8[#PK8 + 1] = {NAME = "I2CX_SCL", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PK8[#PK8 + 1] = {NAME = "I2CX_SDA", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PK8[#PK8 + 1] = {NAME = "ADCX", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}

    this:AddWidget("Textbox", "PK8")
    this:AddProposals(PK8, "K", 8)
end
--*/
#define __GPIO_PK_PIN_8_NAME__ PK8_NC

/*--
if this:PinExist("K", 8) then
    this:AddWidget("Combobox")
    this:AddPinMode("K", 8)
end
--*/
#define __GPIO_PK_PIN_8_MODE__ _GPIO_MODE_DISABLED

/*--
if this:PinExist("K", 8) then
    this:AddWidget("Combobox")
    this:AddPinState("K", 8)
end
--*/
#define __GPIO_PK_PIN_8_STATE__ _LOW

/*--
if this:PinExist("K", 9) then
    local PK9 = {}
    PK9[#PK9 + 1] = {NAME = "PK9_NC", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}
    PK9[#PK9 + 1] = {NAME = "USX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PK9[#PK9 + 1] = {NAME = "USX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PK9[#PK9 + 1] = {NAME = "USX_CLK", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PK9[#PK9 + 1] = {NAME = "USX_CS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PK9[#PK9 + 1] = {NAME = "USX_CTS", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PK9[#PK9 + 1] = {NAME = "USX_RTS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PK9[#PK9 + 1] = {NAME = "LEUX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PK9[#PK9 + 1] = {NAME = "LEUX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PK9[#PK9 + 1] = {NAME = "I2CX_SCL", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PK9[#PK9 + 1] = {NAME = "I2CX_SDA", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PK9[#PK9 + 1] = {NAME = "ADCX", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}

    this:AddWidget("Textbox", "PK9")
    this:AddProposals(PK9, "K", 9)
end
--*/
#define __GPIO_PK_PIN_9_NAME__ PK9_NC

/*--
if this:PinExist("K", 9) then
    this:AddWidget("Combobox")
    this:AddPinMode("K", 9)
end
--*/
#define __GPIO_PK_PIN_9_MODE__ _GPIO_MODE_DISABLED

/*--
if this:PinExist("K", 9) then
    this:AddWidget("Combobox")
    this:AddPinState("K", 9)
end
--*/
#define __GPIO_PK_PIN_9_STATE__ _LOW

/*--
if this:PinExist("K", 10) then
    local PK10 = {}
    PK10[#PK10 + 1] = {NAME = "PK10_NC", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}
    PK10[#PK10 + 1] = {NAME = "USX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PK10[#PK10 + 1] = {NAME = "USX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PK10[#PK10 + 1] = {NAME = "USX_CLK", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PK10[#PK10 + 1] = {NAME = "USX_CS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PK10[#PK10 + 1] = {NAME = "USX_CTS", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PK10[#PK10 + 1] = {NAME = "USX_RTS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PK10[#PK10 + 1] = {NAME = "LEUX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PK10[#PK10 + 1] = {NAME = "LEUX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PK10[#PK10 + 1] = {NAME = "I2CX_SCL", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PK10[#PK10 + 1] = {NAME = "I2CX_SDA", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PK10[#PK10 + 1] = {NAME = "ADCX", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}

    this:AddWidget("Textbox", "PK10")
    this:AddProposals(PK10, "K", 10)
end
--*/
#define __GPIO_PK_PIN_10_NAME__ PK10_NC

/*--
if this:PinExist("K", 10) then
    this:AddWidget("Combobox")
    this:AddPinMode("K", 10)
end
--*/
#define __GPIO_PK_PIN_10_MODE__ _GPIO_MODE_DISABLED

/*--
if this:PinExist("K", 10) then
    this:AddWidget("Combobox")
    this:AddPinState("K", 10)
end
--*/
#define __GPIO_PK_PIN_10_STATE__ _LOW

/*--
if this:PinExist("K", 11) then
    local PK11 = {}
    PK11[#PK11 + 1] = {NAME = "PK11_NC", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}
    PK11[#PK11 + 1] = {NAME = "USX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PK11[#PK11 + 1] = {NAME = "USX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PK11[#PK11 + 1] = {NAME = "USX_CLK", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PK11[#PK11 + 1] = {NAME = "USX_CS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PK11[#PK11 + 1] = {NAME = "USX_CTS", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PK11[#PK11 + 1] = {NAME = "USX_RTS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PK11[#PK11 + 1] = {NAME = "LEUX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PK11[#PK11 + 1] = {NAME = "LEUX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PK11[#PK11 + 1] = {NAME = "I2CX_SCL", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PK11[#PK11 + 1] = {NAME = "I2CX_SDA", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PK11[#PK11 + 1] = {NAME = "ADCX", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}

    this:AddWidget("Textbox", "PK11")
    this:AddProposals(PK11, "K", 11)
end
--*/
#define __GPIO_PK_PIN_11_NAME__ PK11_NC

/*--
if this:PinExist("K", 11) then
    this:AddWidget("Combobox")
    this:AddPinMode("K", 11)
end
--*/
#define __GPIO_PK_PIN_11_MODE__ _GPIO_MODE_DISABLED

/*--
if this:PinExist("K", 11) then
    this:AddWidget("Combobox")
    this:AddPinState("K", 11)
end
--*/
#define __GPIO_PK_PIN_11_STATE__ _LOW

/*--
if this:PinExist("K", 12) then
    local PK12 = {}
    PK12[#PK12 + 1] = {NAME = "PK12_NC", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}
    PK12[#PK12 + 1] = {NAME = "USX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PK12[#PK12 + 1] = {NAME = "USX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PK12[#PK12 + 1] = {NAME = "USX_CLK", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PK12[#PK12 + 1] = {NAME = "USX_CS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PK12[#PK12 + 1] = {NAME = "USX_CTS", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PK12[#PK12 + 1] = {NAME = "USX_RTS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PK12[#PK12 + 1] = {NAME = "LEUX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PK12[#PK12 + 1] = {NAME = "LEUX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PK12[#PK12 + 1] = {NAME = "I2CX_SCL", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PK12[#PK12 + 1] = {NAME = "I2CX_SDA", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PK12[#PK12 + 1] = {NAME = "ADCX", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}

    this:AddWidget("Textbox", "PK12")
    this:AddProposals(PK12, "K", 12)
end
--*/
#define __GPIO_PK_PIN_12_NAME__ PK12_NC

/*--
if this:PinExist("K", 12) then
    this:AddWidget("Combobox")
    this:AddPinMode("K", 12)
end
--*/
#define __GPIO_PK_PIN_12_MODE__ _GPIO_MODE_DISABLED

/*--
if this:PinExist("K", 12) then
    this:AddWidget("Combobox")
    this:AddPinState("K", 12)
end
--*/
#define __GPIO_PK_PIN_12_STATE__ _LOW

/*--
if this:PinExist("K", 13) then
    local PK13 = {}
    PK13[#PK13 + 1] = {NAME = "PK13_NC", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}
    PK13[#PK13 + 1] = {NAME = "USX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PK13[#PK13 + 1] = {NAME = "USX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PK13[#PK13 + 1] = {NAME = "USX_CLK", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PK13[#PK13 + 1] = {NAME = "USX_CS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PK13[#PK13 + 1] = {NAME = "USX_CTS", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PK13[#PK13 + 1] = {NAME = "USX_RTS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PK13[#PK13 + 1] = {NAME = "LEUX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PK13[#PK13 + 1] = {NAME = "LEUX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PK13[#PK13 + 1] = {NAME = "I2CX_SCL", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PK13[#PK13 + 1] = {NAME = "I2CX_SDA", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PK13[#PK13 + 1] = {NAME = "ADCX", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}

    this:AddWidget("Textbox", "PK13")
    this:AddProposals(PK13, "K", 13)
end
--*/
#define __GPIO_PK_PIN_13_NAME__ PK13_NC

/*--
if this:PinExist("K", 13) then
    this:AddWidget("Combobox")
    this:AddPinMode("K", 13)
end
--*/
#define __GPIO_PK_PIN_13_MODE__ _GPIO_MODE_DISABLED

/*--
if this:PinExist("K", 13) then
    this:AddWidget("Combobox")
    this:AddPinState("K", 13)
end
--*/
#define __GPIO_PK_PIN_13_STATE__ _LOW

/*--
if this:PinExist("K", 14) then
    local PK14 = {}
    PK14[#PK14 + 1] = {NAME = "PK14_NC", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}
    PK14[#PK14 + 1] = {NAME = "USX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PK14[#PK14 + 1] = {NAME = "USX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PK14[#PK14 + 1] = {NAME = "USX_CLK", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PK14[#PK14 + 1] = {NAME = "USX_CS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PK14[#PK14 + 1] = {NAME = "USX_CTS", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PK14[#PK14 + 1] = {NAME = "USX_RTS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PK14[#PK14 + 1] = {NAME = "LEUX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PK14[#PK14 + 1] = {NAME = "LEUX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PK14[#PK14 + 1] = {NAME = "I2CX_SCL", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PK14[#PK14 + 1] = {NAME = "I2CX_SDA", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PK14[#PK14 + 1] = {NAME = "ADCX", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}

    this:AddWidget("Textbox", "PK14")
    this:AddProposals(PK14, "K", 14)
end
--*/
#define __GPIO_PK_PIN_14_NAME__ PK14_NC

/*--
if this:PinExist("K", 14) then
    this:AddWidget("Combobox")
    this:AddPinMode("K", 14)
end
--*/
#define __GPIO_PK_PIN_14_MODE__ _GPIO_MODE_DISABLED

/*--
if this:PinExist("K", 14) then
    this:AddWidget("Combobox")
    this:AddPinState("K", 14)
end
--*/
#define __GPIO_PK_PIN_14_STATE__ _LOW

/*--
if this:PinExist("K", 15) then
    local PK15 = {}
    PK15[#PK15 + 1] = {NAME = "PK15_NC", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}
    PK15[#PK15 + 1] = {NAME = "USX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PK15[#PK15 + 1] = {NAME = "USX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PK15[#PK15 + 1] = {NAME = "USX_CLK", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PK15[#PK15 + 1] = {NAME = "USX_CS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PK15[#PK15 + 1] = {NAME = "USX_CTS", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PK15[#PK15 + 1] = {NAME = "USX_RTS", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PK15[#PK15 + 1] = {NAME = "LEUX_TX", MODE = "_GPIO_MODE_PUSHPULLALT", STATE = "_HIGH"}
    PK15[#PK15 + 1] = {NAME = "LEUX_RX", MODE = "_GPIO_MODE_INPUTPULLFILTER", STATE = "_HIGH"}
    PK15[#PK15 + 1] = {NAME = "I2CX_SCL", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PK15[#PK15 + 1] = {NAME = "I2CX_SDA", MODE = "_GPIO_MODE_WIREDANDALTPULLUPFILTER", STATE = "_HIGH"}
    PK15[#PK15 + 1] = {NAME = "ADCX", MODE = "_GPIO_MODE_DISABLED", STATE = "_LOW"}

    this:AddWidget("Textbox", "PK15")
    this:AddProposals(PK15, "K", 15)
end
--*/
#define __GPIO_PK_PIN_15_NAME__ PK15_NC

/*--
if this:PinExist("K", 15) then
    this:AddWidget("Combobox")
    this:AddPinMode("K", 15)
end
--*/
#define __GPIO_PK_PIN_15_MODE__ _GPIO_MODE_DISABLED

/*--
if this:PinExist("K", 15) then
    this:AddWidget("Combobox")
    this:AddPinState("K", 15)
end
--*/
#define __GPIO_PK_PIN_15_STATE__ _LOW




#endif /* _GPIO_FLAGS_H_ */
/*==============================================================================
  End of file
==============================================================================*/
