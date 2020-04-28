/*=========================================================================*//**
@file    gpio_flags.h

@author  Daniel Zorychta

@brief   GPIO module configuration flags.

@note    Copyright (C) 2020 Daniel Zorychta <daniel.zorychta@gmail.com>

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
PACKAGE["C"]  = {A = 0xFFFF, B = 0xF7FF, C = 0xE000, H = 0x0003}
PACKAGE["R"]  = {A = 0xFFFF, B = 0xF7FF, C = 0xFFFF, D = 0x0040, H = 0x0003}
PACKAGE["V"]  = {A = 0xFFFF, B = 0xFFFF, C = 0xFFFF, D = 0xFFFF, E = 0xFFFF, H = 0x0003}
PACKAGE["Z"]  = {A = 0xFFFF, B = 0xFFFF, C = 0xFFFF, D = 0xFFFF, E = 0xFFFF, F = 0xFFFF, G = 0xFFFF, H = 0x0003}
PACKAGE["A"]  = {A = 0xFFFF, B = 0xFFFF, C = 0xFFFF, D = 0xFFFF, E = 0xFFFF, F = 0xFFFF, G = 0xFFFF, H = 0xFFFF, I = 0x07FF}
PACKAGE["I"]  = {A = 0xFFFF, B = 0xFFFF, C = 0xFFFF, D = 0xFFFF, E = 0xFFFF, F = 0xFFFF, G = 0xFFFF, H = 0xFFFF, I = 0x07FF}
PACKAGE["B"]  = {A = 0xFFFF, B = 0xFFFF, C = 0xFFFF, D = 0xFFFF, E = 0xFFFF, F = 0xFFFF, G = 0xFFFF, H = 0xFFFF, I = 0xFFFF, J = 0xFFFF, K = 0x00FF}
PACKAGE["N"]  = {A = 0xFFFF, B = 0xFFFF, C = 0xFFFF, D = 0xFFFF, E = 0xFFFF, F = 0xFFFF, G = 0xFFFF, H = 0xFFFF, I = 0xFFFF, J = 0xFFFF, K = 0x00FF}

local port = {"A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K"}
local pack = uC.NAME:gsub("STM32F%d+(.).*", "%1")

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
    this:SetLayout("TitledGridBack", 6, "Home > Microcontroller > ".._GPIO_SELECTION_,
                   function() Configure(nil) end)

    this:AddExtraWidget("Label", "LabelPin", "Pin")
    this:AddExtraWidget("Label", "LabelName", "Name")
    this:AddExtraWidget("Label", "LabelSpeed", "Speed")
    this:AddExtraWidget("Label", "LabelMode", "Mode")
    this:AddExtraWidget("Label", "LabelAF", "Alternative")
    this:AddExtraWidget("Label", "LabelState", "State")

    this.AddPinSpeed = function(this, portLetter, pinNumber)
        this:AddItem("2 MHz", "_GPIO_SPEED_LOW")
        this:AddItem("25 MHz", "_GPIO_SPEED_MEDIUM")
        this:AddItem("50 MHz", "_GPIO_SPEED_HIGH")
        this:AddItem("100 MHz", "_GPIO_SPEED_VERY_HIGH")
    end

    this.AddPinAF = function(this, portLetter, pinNumber)
        this:AddItem("AF0 (SYS)", "0")
        this:AddItem("AF1 (TIM1..2)", "1")
        this:AddItem("AF2 (TIM3..TIM5)", "2")
        this:AddItem("AF3 (TIM8..TIM11)", "3")
        this:AddItem("AF4 (I2C1..3)", "4")
        this:AddItem("AF5 (SPI1..6, I2S2)", "5")
        this:AddItem("AF6 (SPI2..3, I2S3, SAI1)", "6")
        this:AddItem("AF7 (SPI2..3, I2S3, USART1..3)", "7")
        this:AddItem("AF8 (UART4..8)", "8")
        this:AddItem("AF9 (CAN1..2, TIM12..14, QSPI, LCD)", "9")
        this:AddItem("AF10 (OTG_FS, OTG_HS, QSPI)", "10")
        this:AddItem("AF11 (ETH)", "11")
        this:AddItem("AF12 (FSMC, FMC, SDIO, OTG_FS)", "12")
        this:AddItem("AF13 (DCMI, DSI HOST)", "13")
        this:AddItem("AF14 (LCD)", "14")
        this:AddItem("AF15 (EVENTOUT)", "15")

        local mode = this:GetFlagValue("__GPIO_P"..portLetter.."_PIN_"..pinNumber.."_MODE__")

        if mode:match("_GPIO_MODE_AF") then
            this:Enable(true)
        else
            this:Enable(false)
        end
    end

    this.AddPinMode = function(this, portLetter, pinNumber)
        this:AddItem("Output PP",         "_GPIO_MODE_PP")
        this:AddItem("Output OD",         "_GPIO_MODE_OD")
        this:AddItem("Output OD + PU",    "_GPIO_MODE_OD_PU")
        this:AddItem("Output OD + PD",    "_GPIO_MODE_OD_PD")
        this:AddItem("Alt. PP",           "_GPIO_MODE_AF_PP")
        this:AddItem("Alt. PP + PU",      "_GPIO_MODE_AF_PP_PU")
        this:AddItem("Alt. PP + PD",      "_GPIO_MODE_AF_PP_PD")
        this:AddItem("Alt. OD",           "_GPIO_MODE_AF_OD")
        this:AddItem("Alt. OD + PU",      "_GPIO_MODE_AF_OD_PU")
        this:AddItem("Alt. OD + PD",      "_GPIO_MODE_AF_OD_PD")
        this:AddItem("Input",             "_GPIO_MODE_IN")
        this:AddItem("Input PU",          "_GPIO_MODE_IN_PU")
        this:AddItem("Input PD",          "_GPIO_MODE_IN_PD")
        this:AddItem("Analog",            "_GPIO_MODE_ANALOG")

        this:SetEvent("clicked", function()
            local mode = this:GetFlagValue("__GPIO_P"..portLetter.."_PIN_"..pinNumber.."_MODE__")

            if mode == "_GPIO_MODE_ANALOG" or mode:match("_GPIO_MODE_IN") or mode:match("_GPIO_MODE_AF") then
                this:Enable(false, "__GPIO_P"..portLetter.."_PIN_"..pinNumber.."_STATE__")
            else
                this:Enable(true, "__GPIO_P"..portLetter.."_PIN_"..pinNumber.."_STATE__")
            end

            if mode:match("_GPIO_MODE_AF") then
                this:Enable(true, "__GPIO_P"..portLetter.."_PIN_"..pinNumber.."_AF__")
            else
                this:Enable(false, "__GPIO_P"..portLetter.."_PIN_"..pinNumber.."_AF__")
            end
        end)
    end

    this.AddPinState = function(this, portLetter, pinNumber)
        this:AddItem("Low", "_LOW")
        this:AddItem("High", "_HIGH")

        local mode = this:GetFlagValue("__GPIO_P"..portLetter.."_PIN_"..pinNumber.."_MODE__")

        if mode == "_GPIO_MODE_ANALOG" or mode:match("_GPIO_MODE_IN") or mode:match("_GPIO_MODE_AF") then
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

    this.AddProposals = function(this, tab, portLetter, pinNumber)
        for i = 1, #tab do this:AddItem(tab[i].NAME, "") end
        this:SetEvent("clicked",
            function()
                local nameFlag   = "__GPIO_P"..portLetter.."_PIN_"..pinNumber.."_NAME__"
                local modeFlag   = "__GPIO_P"..portLetter.."_PIN_"..pinNumber.."_MODE__"
                local altfFlag   = "__GPIO_P"..portLetter.."_PIN_"..pinNumber.."_AF__"
                local stateFlag  = "__GPIO_P"..portLetter.."_PIN_"..pinNumber.."_STATE__"
                local selection  = this:GetFlagValue(nameFlag)

                for i = 1, #tab do
                    if tab[i].NAME == selection then
                        this:SetFlagValue(modeFlag, tab[i].MODE)
                        this:SetFlagValue(stateFlag, tab[i].STATE)
                        this:SetFlagValue(altfFlag, tab[i].AF)

                        if tab[i].MODE == "_GPIO_MODE_ANALOG" or
                           tab[i].MODE:match("_GPIO_MODE_IN") or
                           tab[i].MODE:match("_GPIO_MODE_AF") then

                            this:Enable(false, stateFlag)
                        else
                            this:Enable(true, stateFlag)
                        end

                        if tab[i].MODE:match("_GPIO_MODE_AF") then
                           this:Enable(true, altfFlag)
                        else
                           this:Enable(false, altfFlag)
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
if this:PinExist("A", 0) then
    local PA0 = {}
    PA0[#PA0 + 1] = {NAME = "PA0_NC", MODE = "_GPIO_MODE_IN", AF = "0", STATE = "_LOW"}
    PA0[#PA0 + 1] = {NAME = "TIM2_CH1", MODE = "_GPIO_MODE_AF_PP_PD", AF = "1", STATE = "_LOW"}
    PA0[#PA0 + 1] = {NAME = "TIM2_ETR", MODE = "_GPIO_MODE_AF_PP_PD", AF = "1", STATE = "_LOW"}
    PA0[#PA0 + 1] = {NAME = "TIM4_CH1", MODE = "_GPIO_MODE_AF_PP_PD", AF = "2", STATE = "_LOW"}
    PA0[#PA0 + 1] = {NAME = "TIM5_CH1", MODE = "_GPIO_MODE_AF_PP_PD", AF = "2", STATE = "_LOW"}
    PA0[#PA0 + 1] = {NAME = "TIM8_ETR", MODE = "_GPIO_MODE_AF_PP_PD", AF = "3", STATE = "_LOW"}
    PA0[#PA0 + 1] = {NAME = "USART2_CTS", MODE = "_GPIO_MODE_AF_PP_PU", AF = "7", STATE = "_LOW"}
    PA0[#PA0 + 1] = {NAME = "UART4_TX", MODE = "_GPIO_MODE_AF_PP", AF = "8", STATE = "_LOW"}
    PA0[#PA0 + 1] = {NAME = "ETH_MII_CRS", MODE = "_GPIO_MODE_AF_PP", AF = "11", STATE = "_LOW"}
    PA0[#PA0 + 1] = {NAME = "EVENTOUT", MODE = "_GPIO_MODE_AF_PP", AF = "15", STATE = "_LOW"}

    this:AddWidget("Textbox", "PA0")
    this:AddProposals(PA0, "A", 0)
end
--*/
#define __GPIO_PA_PIN_0_NAME__ PA0_NC

/*--
if this:PinExist("A", 0) then
    this:AddWidget("Combobox")
    this:AddPinSpeed("A", 0)
end
--*/
#define __GPIO_PA_PIN_0_SPEED__ _GPIO_SPEED_LOW

/*--
if this:PinExist("A", 0) then
    this:AddWidget("Combobox")
    this:AddPinMode("A", 0)
end
--*/
#define __GPIO_PA_PIN_0_MODE__ _GPIO_MODE_IN

/*--
if this:PinExist("A", 0) then
    this:AddWidget("Combobox")
    this:AddPinAF("A", 0)
end
--*/
#define __GPIO_PA_PIN_0_AF__ 0

/*--
if this:PinExist("A", 0) then
    this:AddWidget("Combobox")
    this:AddPinState("A", 0)
end
--*/
#define __GPIO_PA_PIN_0_STATE__ _LOW

/*--
if this:PinExist("A", 1) then
    local PA1 = {}
    PA1[#PA1 + 1] = {NAME = "PA1_NC", MODE = "_GPIO_MODE_IN", AF = "0", STATE = "_LOW"}
    PA1[#PA1 + 1] = {NAME = "TIM2_CH2", MODE = "_GPIO_MODE_AF_PP_PD", AF = "1", STATE = "_LOW"}
    PA1[#PA1 + 1] = {NAME = "TIM5_CH2", MODE = "_GPIO_MODE_AF_PP_PD", AF = "2", STATE = "_LOW"}
    PA1[#PA1 + 1] = {NAME = "USART2_RTS", MODE = "_GPIO_MODE_AF_PP", AF = "7", STATE = "_LOW"}
    PA1[#PA1 + 1] = {NAME = "UART4_RX", MODE = "_GPIO_MODE_AF_PP_PU", AF = "8", STATE = "_LOW"}
    PA1[#PA1 + 1] = {NAME = "QSPI_BK1_IO3", MODE = "_GPIO_MODE_AF_PP_PD", AF = "9", STATE = "_LOW"}
    PA1[#PA1 + 1] = {NAME = "ETH_MII_RX_CLK", MODE = "_GPIO_MODE_AF_PP", AF = "11", STATE = "_LOW"}
    PA1[#PA1 + 1] = {NAME = "ETH_RMII_REF_CLK", MODE = "_GPIO_MODE_AF_PP", AF = "11", STATE = "_LOW"}
    PA1[#PA1 + 1] = {NAME = "LCD_R2", MODE = "_GPIO_MODE_AF_PP", AF = "14", STATE = "_LOW"}
    PA1[#PA1 + 1] = {NAME = "EVENTOUT", MODE = "_GPIO_MODE_AF_PP", AF = "15", STATE = "_LOW"}

    this:AddWidget("Textbox", "PA1")
    this:AddProposals(PA1, "A", 1)
end
--*/
#define __GPIO_PA_PIN_1_NAME__ PA1_NC

/*--
if this:PinExist("A", 1) then
    this:AddWidget("Combobox")
    this:AddPinSpeed("A", 1)
end
--*/
#define __GPIO_PA_PIN_1_SPEED__ _GPIO_SPEED_LOW

/*--
if this:PinExist("A", 1) then
    this:AddWidget("Combobox")
    this:AddPinMode("A", 1)
end
--*/
#define __GPIO_PA_PIN_1_MODE__ _GPIO_MODE_IN

/*--
if this:PinExist("A", 1) then
    this:AddWidget("Combobox")
    this:AddPinAF("A", 1)
end
--*/
#define __GPIO_PA_PIN_1_AF__ 0

/*--
if this:PinExist("A", 1) then
    this:AddWidget("Combobox")
    this:AddPinState("A", 1)
end
--*/
#define __GPIO_PA_PIN_1_STATE__ _LOW

/*--
if this:PinExist("A", 2) then
    local PA2 = {}
    PA2[#PA2 + 1] = {NAME = "PA2_NC", MODE = "_GPIO_MODE_IN", AF = "0", STATE = "_LOW"}
    PA2[#PA2 + 1] = {NAME = "TIM2_CH3", MODE = "_GPIO_MODE_AF_PP_PD", AF = "1", STATE = "_LOW"}
    PA2[#PA2 + 1] = {NAME = "TIM5_CH3", MODE = "_GPIO_MODE_AF_PP_PD", AF = "2", STATE = "_LOW"}
    PA2[#PA2 + 1] = {NAME = "TIM9_CH1", MODE = "_GPIO_MODE_AF_PP_PD", AF = "3", STATE = "_LOW"}
    PA2[#PA2 + 1] = {NAME = "USART2_TX", MODE = "_GPIO_MODE_AF_PP", AF = "7", STATE = "_LOW"}
    PA2[#PA2 + 1] = {NAME = "ETH_MDIO", MODE = "_GPIO_MODE_AF_PP", AF = "11", STATE = "_LOW"}
    PA2[#PA2 + 1] = {NAME = "LCD_R1", MODE = "_GPIO_MODE_AF_PP", AF = "14", STATE = "_LOW"}
    PA2[#PA2 + 1] = {NAME = "EVENTOUT", MODE = "_GPIO_MODE_AF_PP", AF = "15", STATE = "_LOW"}

    this:AddWidget("Textbox", "PA2")
    this:AddProposals(PA2, "A", 2)
end
--*/
#define __GPIO_PA_PIN_2_NAME__ PA2_NC

/*--
if this:PinExist("A", 2) then
    this:AddWidget("Combobox")
    this:AddPinSpeed("A", 2)
end
--*/
#define __GPIO_PA_PIN_2_SPEED__ _GPIO_SPEED_LOW

/*--
if this:PinExist("A", 2) then
    this:AddWidget("Combobox")
    this:AddPinMode("A", 2)
end
--*/
#define __GPIO_PA_PIN_2_MODE__ _GPIO_MODE_IN

/*--
if this:PinExist("A", 2) then
    this:AddWidget("Combobox")
    this:AddPinAF("A", 2)
end
--*/
#define __GPIO_PA_PIN_2_AF__ 0

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
    PA3[#PA3 + 1] = {NAME = "PA3_NC", MODE = "_GPIO_MODE_IN", AF = "0", STATE = "_LOW"}
    PA3[#PA3 + 1] = {NAME = "TIM2_CH4", MODE = "_GPIO_MODE_AF_PP_PD", AF = "1", STATE = "_LOW"}
    PA3[#PA3 + 1] = {NAME = "TIM5_CH4", MODE = "_GPIO_MODE_AF_PP_PD", AF = "2", STATE = "_LOW"}
    PA3[#PA3 + 1] = {NAME = "TIM9_CH2", MODE = "_GPIO_MODE_AF_PP_PD", AF = "3", STATE = "_LOW"}
    PA3[#PA3 + 1] = {NAME = "USART2_RX", MODE = "_GPIO_MODE_AF_PP_PU", AF = "7", STATE = "_LOW"}
    PA3[#PA3 + 1] = {NAME = "LCD_B2", MODE = "_GPIO_MODE_AF_PP", AF = "9", STATE = "_LOW"}
    PA3[#PA3 + 1] = {NAME = "OTG_HS_ULPI_D0", MODE = "_GPIO_MODE_AF_PP", AF = "10", STATE = "_LOW"}
    PA3[#PA3 + 1] = {NAME = "ETH_MII_COL", MODE = "_GPIO_MODE_AF_PP", AF = "11", STATE = "_LOW"}
    PA3[#PA3 + 1] = {NAME = "LCD_B5", MODE = "_GPIO_MODE_AF_PP", AF = "14", STATE = "_LOW"}
    PA3[#PA3 + 1] = {NAME = "EVENTOUT", MODE = "_GPIO_MODE_AF_PP", AF = "15", STATE = "_LOW"}

    this:AddWidget("Textbox", "PA3")
    this:AddProposals(PA3, "A", 3)
end
--*/
#define __GPIO_PA_PIN_3_NAME__ PA3_NC

/*--
if this:PinExist("A", 3) then
    this:AddWidget("Combobox")
    this:AddPinSpeed("A", 3)
end
--*/
#define __GPIO_PA_PIN_3_SPEED__ _GPIO_SPEED_LOW

/*--
if this:PinExist("A", 3) then
    this:AddWidget("Combobox")
    this:AddPinMode("A", 3)
end
--*/
#define __GPIO_PA_PIN_3_MODE__ _GPIO_MODE_IN

/*--
if this:PinExist("A", 3) then
    this:AddWidget("Combobox")
    this:AddPinAF("A", 3)
end
--*/
#define __GPIO_PA_PIN_3_AF__ 0

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
    PA4[#PA4 + 1] = {NAME = "PA4_NC", MODE = "_GPIO_MODE_IN", AF = "0", STATE = "_LOW"}
    PA4[#PA4 + 1] = {NAME = "SPI1_NSS", MODE = "_GPIO_MODE_AF_PP_PU", AF = "5", STATE = "_LOW"}
    PA4[#PA4 + 1] = {NAME = "SPI3_NSS", MODE = "_GPIO_MODE_AF_PP_PU", AF = "6", STATE = "_LOW"}
    PA4[#PA4 + 1] = {NAME = "I2S3_WS", MODE = "_GPIO_MODE_AF_PP_PU", AF = "6", STATE = "_LOW"}
    PA4[#PA4 + 1] = {NAME = "UART2_CK", MODE = "_GPIO_MODE_AF_PP_PD", AF = "7", STATE = "_LOW"}
    PA4[#PA4 + 1] = {NAME = "OTG_HS_SOF", MODE = "_GPIO_MODE_AF_PP", AF = "12", STATE = "_LOW"}
    PA4[#PA4 + 1] = {NAME = "DCMI_HSYNC", MODE = "_GPIO_MODE_AF_PP", AF = "13", STATE = "_LOW"}
    PA4[#PA4 + 1] = {NAME = "LCD_VSYNC", MODE = "_GPIO_MODE_AF_PP", AF = "14", STATE = "_LOW"}
    PA4[#PA4 + 1] = {NAME = "EVENTOUT", MODE = "_GPIO_MODE_AF_PP", AF = "15", STATE = "_LOW"}

    this:AddWidget("Textbox", "PA4")
    this:AddProposals(PA4, "A", 4)
end
--*/
#define __GPIO_PA_PIN_4_NAME__ PA4_NC

/*--
if this:PinExist("A", 4) then
    this:AddWidget("Combobox")
    this:AddPinSpeed("A", 4)
end
--*/
#define __GPIO_PA_PIN_4_SPEED__ _GPIO_SPEED_LOW

/*--
if this:PinExist("A", 4) then
    this:AddWidget("Combobox")
    this:AddPinMode("A", 4)
end
--*/
#define __GPIO_PA_PIN_4_MODE__ _GPIO_MODE_IN

/*--
if this:PinExist("A", 4) then
    this:AddWidget("Combobox")
    this:AddPinAF("A", 4)
end
--*/
#define __GPIO_PA_PIN_4_AF__ 0

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
    PA5[#PA5 + 1] = {NAME = "PA5_NC", MODE = "_GPIO_MODE_IN", AF = "0", STATE = "_LOW"}
    PA5[#PA5 + 1] = {NAME = "TIM2_CH1", MODE = "_GPIO_MODE_AF_PP_PD", AF = "1", STATE = "_LOW"}
    PA5[#PA5 + 1] = {NAME = "TIM2_ETR", MODE = "_GPIO_MODE_AF_PP_PD", AF = "1", STATE = "_LOW"}
    PA5[#PA5 + 1] = {NAME = "TIM8_CH1N", MODE = "_GPIO_MODE_AF_PP_PD", AF = "3", STATE = "_LOW"}
    PA5[#PA5 + 1] = {NAME = "SPI1_SCK", MODE = "_GPIO_MODE_AF_PP_PD", AF = "5", STATE = "_LOW"}
    PA5[#PA5 + 1] = {NAME = "OTG_HS_ULPI_CK", MODE = "_GPIO_MODE_AF_PP", AF = "10", STATE = "_LOW"}
    PA5[#PA5 + 1] = {NAME = "LCD_R4", MODE = "_GPIO_MODE_AF_PP", AF = "14", STATE = "_LOW"}
    PA5[#PA5 + 1] = {NAME = "EVENTOUT", MODE = "_GPIO_MODE_AF_PP", AF = "15", STATE = "_LOW"}

    this:AddWidget("Textbox", "PA5")
    this:AddProposals(PA5, "A", 5)
end
--*/
#define __GPIO_PA_PIN_5_NAME__ PA5_NC

/*--
if this:PinExist("A", 5) then
    this:AddWidget("Combobox")
    this:AddPinSpeed("A", 5)
end
--*/
#define __GPIO_PA_PIN_5_SPEED__ _GPIO_SPEED_LOW

/*--
if this:PinExist("A", 5) then
    this:AddWidget("Combobox")
    this:AddPinMode("A", 5)
end
--*/
#define __GPIO_PA_PIN_5_MODE__ _GPIO_MODE_IN

/*--
if this:PinExist("A", 5) then
    this:AddWidget("Combobox")
    this:AddPinAF("A", 5)
end
--*/
#define __GPIO_PA_PIN_5_AF__ 0

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
    PA6[#PA6 + 1] = {NAME = "PA6_NC", MODE = "_GPIO_MODE_IN", AF = "0", STATE = "_LOW"}
    PA6[#PA6 + 1] = {NAME = "TIM1_BKIN", MODE = "_GPIO_MODE_AF_PP_PD", AF = "1", STATE = "_LOW"}
    PA6[#PA6 + 1] = {NAME = "TIM3_CH1", MODE = "_GPIO_MODE_AF_PP_PD", AF = "2", STATE = "_LOW"}
    PA6[#PA6 + 1] = {NAME = "TIM8_BKIN", MODE = "_GPIO_MODE_AF_PP_PD", AF = "3", STATE = "_LOW"}
    PA6[#PA6 + 1] = {NAME = "SPI1_MISO", MODE = "_GPIO_MODE_AF_PP_PD", AF = "5", STATE = "_LOW"}
    PA6[#PA6 + 1] = {NAME = "TIM13_CH1", MODE = "_GPIO_MODE_AF_PP_PD", AF = "9", STATE = "_LOW"}
    PA6[#PA6 + 1] = {NAME = "DCMI_PIXCLK", MODE = "_GPIO_MODE_AF_PP", AF = "13", STATE = "_LOW"}
    PA6[#PA6 + 1] = {NAME = "LCD_G2", MODE = "_GPIO_MODE_AF_PP", AF = "14", STATE = "_LOW"}
    PA6[#PA6 + 1] = {NAME = "EVENTOUT", MODE = "_GPIO_MODE_AF_PP", AF = "15", STATE = "_LOW"}

    this:AddWidget("Textbox", "PA6")
    this:AddProposals(PA6, "A", 6)
end
--*/
#define __GPIO_PA_PIN_6_NAME__ PA6_NC

/*--
if this:PinExist("A", 6) then
    this:AddWidget("Combobox")
    this:AddPinSpeed("A", 6)
end
--*/
#define __GPIO_PA_PIN_6_SPEED__ _GPIO_SPEED_LOW

/*--
if this:PinExist("A", 6) then
    this:AddWidget("Combobox")
    this:AddPinMode("A", 6)
end
--*/
#define __GPIO_PA_PIN_6_MODE__ _GPIO_MODE_IN

/*--
if this:PinExist("A", 6) then
    this:AddWidget("Combobox")
    this:AddPinAF("A", 6)
end
--*/
#define __GPIO_PA_PIN_6_AF__ 0

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
    PA7[#PA7 + 1] = {NAME = "PA7_NC", MODE = "_GPIO_MODE_IN", AF = "0", STATE = "_LOW"}
    PA7[#PA7 + 1] = {NAME = "TIM1_CH1N", MODE = "_GPIO_MODE_AF_PP_PD", AF = "1", STATE = "_LOW"}
    PA7[#PA7 + 1] = {NAME = "TIM3_CH2", MODE = "_GPIO_MODE_AF_PP_PD", AF = "2", STATE = "_LOW"}
    PA7[#PA7 + 1] = {NAME = "TIM8_CH1N", MODE = "_GPIO_MODE_AF_PP_PD", AF = "3", STATE = "_LOW"}
    PA7[#PA7 + 1] = {NAME = "SPI1_MOSI", MODE = "_GPIO_MODE_AF_PP", AF = "5", STATE = "_LOW"}
    PA7[#PA7 + 1] = {NAME = "TIM14_CH1", MODE = "_GPIO_MODE_AF_PP_PD", AF = "9", STATE = "_LOW"}
    PA7[#PA7 + 1] = {NAME = "QSPI_CLK", MODE = "_GPIO_MODE_AF_PP", AF = "10", STATE = "_LOW"}
    PA7[#PA7 + 1] = {NAME = "ETH_MII_RX_DV", MODE = "_GPIO_MODE_AF_PP", AF = "11", STATE = "_LOW"}
    PA7[#PA7 + 1] = {NAME = "ETH_RMII_CRS_DV", MODE = "_GPIO_MODE_AF_PP", AF = "11", STATE = "_LOW"}
    PA7[#PA7 + 1] = {NAME = "FMC_SDNWE", MODE = "_GPIO_MODE_AF_PP", AF = "12", STATE = "_LOW"}
    PA7[#PA7 + 1] = {NAME = "EVENTOUT", MODE = "_GPIO_MODE_AF_PP", AF = "15", STATE = "_LOW"}

    this:AddWidget("Textbox", "PA7")
    this:AddProposals(PA7, "A", 7)
end
--*/
#define __GPIO_PA_PIN_7_NAME__ PA7_NC

/*--
if this:PinExist("A", 7) then
    this:AddWidget("Combobox")
    this:AddPinSpeed("A", 7)
end
--*/
#define __GPIO_PA_PIN_7_SPEED__ _GPIO_SPEED_LOW

/*--
if this:PinExist("A", 7) then
    this:AddWidget("Combobox")
    this:AddPinMode("A", 7)
end
--*/
#define __GPIO_PA_PIN_7_MODE__ _GPIO_MODE_IN

/*--
if this:PinExist("A", 7) then
    this:AddWidget("Combobox")
    this:AddPinAF("A", 7)
end
--*/
#define __GPIO_PA_PIN_7_AF__ 0

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
    PA8[#PA8 + 1] = {NAME = "PA8_NC", MODE = "_GPIO_MODE_IN", AF = "0", STATE = "_LOW"}
    PA8[#PA8 + 1] = {NAME = "MCO1", MODE = "_GPIO_MODE_AF_PP", AF = "0", STATE = "_LOW"}
    PA8[#PA8 + 1] = {NAME = "TIM1_CH1", MODE = "_GPIO_MODE_AF_PP_PD", AF = "1", STATE = "_LOW"}
    PA8[#PA8 + 1] = {NAME = "I2C3_SCL", MODE = "_GPIO_MODE_AF_OD_PU", AF = "4", STATE = "_LOW"}
    PA8[#PA8 + 1] = {NAME = "USART1_CK", MODE = "_GPIO_MODE_AF_PP", AF = "7", STATE = "_LOW"}
    PA8[#PA8 + 1] = {NAME = "OTG_FS_SOF", MODE = "_GPIO_MODE_AF_PP", AF = "10", STATE = "_LOW"}
    PA8[#PA8 + 1] = {NAME = "LCD_R6", MODE = "_GPIO_MODE_AF_PP", AF = "14", STATE = "_LOW"}
    PA8[#PA8 + 1] = {NAME = "EVENTOUT", MODE = "_GPIO_MODE_AF_PP", AF = "15", STATE = "_LOW"}

    this:AddWidget("Textbox", "PA8")
    this:AddProposals(PA8, "A", 8)
end
--*/
#define __GPIO_PA_PIN_8_NAME__ PA8_NC

/*--
if this:PinExist("A", 8) then
    this:AddWidget("Combobox")
    this:AddPinSpeed("A", 8)
end
--*/
#define __GPIO_PA_PIN_8_SPEED__ _GPIO_SPEED_LOW

/*--
if this:PinExist("A", 8) then
    this:AddWidget("Combobox")
    this:AddPinMode("A", 8)
end
--*/
#define __GPIO_PA_PIN_8_MODE__ _GPIO_MODE_IN

/*--
if this:PinExist("A", 8) then
    this:AddWidget("Combobox")
    this:AddPinAF("A", 8)
end
--*/
#define __GPIO_PA_PIN_8_AF__ 0

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
    PA9[#PA9 + 1] = {NAME = "PA9_NC", MODE = "_GPIO_MODE_IN", AF = "0", STATE = "_LOW"}
    PA9[#PA9 + 1] = {NAME = "TIM1_CH2", MODE = "_GPIO_MODE_AF_PP_PD", AF = "1", STATE = "_LOW"}
    PA9[#PA9 + 1] = {NAME = "I2C3_SMBA", MODE = "_GPIO_MODE_AF_OD_PU", AF = "4", STATE = "_LOW"}
    PA9[#PA9 + 1] = {NAME = "SPI2_SCK", MODE = "_GPIO_MODE_AF_PP", AF = "5", STATE = "_LOW"}
    PA9[#PA9 + 1] = {NAME = "I2S2_CK", MODE = "_GPIO_MODE_AF_PP", AF = "5", STATE = "_LOW"}
    PA9[#PA9 + 1] = {NAME = "USART1_TX", MODE = "_GPIO_MODE_AF_PP", AF = "7", STATE = "_LOW"}
    PA9[#PA9 + 1] = {NAME = "DCMI_D0", MODE = "_GPIO_MODE_AF_PP", AF = "13", STATE = "_LOW"}
    PA9[#PA9 + 1] = {NAME = "EVENTOUT", MODE = "_GPIO_MODE_AF_PP", AF = "15", STATE = "_LOW"}

    this:AddWidget("Textbox", "PA9")
    this:AddProposals(PA9, "A", 9)
end
--*/
#define __GPIO_PA_PIN_9_NAME__ USART1_TX

/*--
if this:PinExist("A", 9) then
    this:AddWidget("Combobox")
    this:AddPinSpeed("A", 9)
end
--*/
#define __GPIO_PA_PIN_9_SPEED__ _GPIO_SPEED_LOW

/*--
if this:PinExist("A", 9) then
    this:AddWidget("Combobox")
    this:AddPinMode("A", 9)
end
--*/
#define __GPIO_PA_PIN_9_MODE__ _GPIO_MODE_AF_PP

/*--
if this:PinExist("A", 9) then
    this:AddWidget("Combobox")
    this:AddPinAF("A", 9)
end
--*/
#define __GPIO_PA_PIN_9_AF__ 7

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
    PA10[#PA10 + 1] = {NAME = "PA10_NC", MODE = "_GPIO_MODE_IN", AF = "0", STATE = "_LOW"}
    PA10[#PA10 + 1] = {NAME = "TIM1_CH3", MODE = "_GPIO_MODE_AF_PP_PD", AF = "1", STATE = "_LOW"}
    PA10[#PA10 + 1] = {NAME = "USART1_RX", MODE = "_GPIO_MODE_AF_PP_PU", AF = "7", STATE = "_LOW"}
    PA10[#PA10 + 1] = {NAME = "OTG_FS_ID", MODE = "_GPIO_MODE_AF_PP_PD", AF = "10", STATE = "_LOW"}
    PA10[#PA10 + 1] = {NAME = "DCMI_D1", MODE = "_GPIO_MODE_AF_PP", AF = "13", STATE = "_LOW"}
    PA10[#PA10 + 1] = {NAME = "EVENTOUT", MODE = "_GPIO_MODE_AF_PP", AF = "15", STATE = "_LOW"}

    this:AddWidget("Textbox", "PA10")
    this:AddProposals(PA10, "A", 10)
end
--*/
#define __GPIO_PA_PIN_10_NAME__ USART1_RX

/*--
if this:PinExist("A", 10) then
    this:AddWidget("Combobox")
    this:AddPinSpeed("A", 10)
end
--*/
#define __GPIO_PA_PIN_10_SPEED__ _GPIO_SPEED_LOW

/*--
if this:PinExist("A", 10) then
    this:AddWidget("Combobox")
    this:AddPinMode("A", 10)
end
--*/
#define __GPIO_PA_PIN_10_MODE__ _GPIO_MODE_AF_PP_PU

/*--
if this:PinExist("A", 10) then
    this:AddWidget("Combobox")
    this:AddPinAF("A", 10)
end
--*/
#define __GPIO_PA_PIN_10_AF__ 7

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
    PA11[#PA11 + 1] = {NAME = "PA11_NC", MODE = "_GPIO_MODE_IN", AF = "0", STATE = "_LOW"}
    PA11[#PA11 + 1] = {NAME = "TIM1_CH4", MODE = "_GPIO_MODE_AF_PP_PD", AF = "1", STATE = "_LOW"}
    PA11[#PA11 + 1] = {NAME = "USART1_CTS", MODE = "_GPIO_MODE_AF_PP_PU", AF = "7", STATE = "_LOW"}
    PA11[#PA11 + 1] = {NAME = "CAN1_RX", MODE = "_GPIO_MODE_AF_PP", AF = "9", STATE = "_LOW"}
    PA11[#PA11 + 1] = {NAME = "OTG_FS_DM", MODE = "_GPIO_MODE_AF_PP", AF = "10", STATE = "_LOW"}
    PA11[#PA11 + 1] = {NAME = "LCD_R4", MODE = "_GPIO_MODE_AF_PP", AF = "14", STATE = "_LOW"}
    PA11[#PA11 + 1] = {NAME = "EVENTOUT", MODE = "_GPIO_MODE_AF_PP", AF = "15", STATE = "_LOW"}

    this:AddWidget("Textbox", "PA11")
    this:AddProposals(PA11, "A", 11)
end
--*/
#define __GPIO_PA_PIN_11_NAME__ PA11_NC

/*--
if this:PinExist("A", 11) then
    this:AddWidget("Combobox")
    this:AddPinSpeed("A", 11)
end
--*/
#define __GPIO_PA_PIN_11_SPEED__ _GPIO_SPEED_LOW

/*--
if this:PinExist("A", 11) then
    this:AddWidget("Combobox")
    this:AddPinMode("A", 11)
end
--*/
#define __GPIO_PA_PIN_11_MODE__ _GPIO_MODE_IN

/*--
if this:PinExist("A", 11) then
    this:AddWidget("Combobox")
    this:AddPinAF("A", 11)
end
--*/
#define __GPIO_PA_PIN_11_AF__ 0

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
    PA12[#PA12 + 1] = {NAME = "PA12_NC", MODE = "_GPIO_MODE_IN", AF = "0", STATE = "_LOW"}
    PA12[#PA12 + 1] = {NAME = "TIM1_ETR", MODE = "_GPIO_MODE_AF_PP_PD", AF = "1", STATE = "_LOW"}
    PA12[#PA12 + 1] = {NAME = "USART1_RTS", MODE = "_GPIO_MODE_AF_PP", AF = "7", STATE = "_LOW"}
    PA12[#PA12 + 1] = {NAME = "CAN1_TX", MODE = "_GPIO_MODE_AF_PP", AF = "9", STATE = "_LOW"}
    PA12[#PA12 + 1] = {NAME = "OTG_FS_DP", MODE = "_GPIO_MODE_AF_PP", AF = "10", STATE = "_LOW"}
    PA12[#PA12 + 1] = {NAME = "LCD_R5", MODE = "_GPIO_MODE_AF_PP", AF = "14", STATE = "_LOW"}
    PA12[#PA12 + 1] = {NAME = "EVENTOUT", MODE = "_GPIO_MODE_AF_PP", AF = "15", STATE = "_LOW"}

    this:AddWidget("Textbox", "PA12")
    this:AddProposals(PA12, "A", 12)
end
--*/
#define __GPIO_PA_PIN_12_NAME__ PA12_NC

/*--
if this:PinExist("A", 12) then
    this:AddWidget("Combobox")
    this:AddPinSpeed("A", 12)
end
--*/
#define __GPIO_PA_PIN_12_SPEED__ _GPIO_SPEED_LOW

/*--
if this:PinExist("A", 12) then
    this:AddWidget("Combobox")
    this:AddPinMode("A", 12)
end
--*/
#define __GPIO_PA_PIN_12_MODE__ _GPIO_MODE_IN

/*--
if this:PinExist("A", 12) then
    this:AddWidget("Combobox")
    this:AddPinAF("A", 12)
end
--*/
#define __GPIO_PA_PIN_12_AF__ 0

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
    PA13[#PA13 + 1] = {NAME = "PA13_NC", MODE = "_GPIO_MODE_IN", AF = "0", STATE = "_LOW"}
    PA13[#PA13 + 1] = {NAME = "JTMS_SWDIO", MODE = "_GPIO_MODE_AF_PP", AF = "0", STATE = "_LOW"}
    PA13[#PA13 + 1] = {NAME = "EVENTOUT", MODE = "_GPIO_MODE_AF_PP", AF = "15", STATE = "_LOW"}

    this:AddWidget("Textbox", "PA13")
    this:AddProposals(PA13, "A", 13)
end
--*/
#define __GPIO_PA_PIN_13_NAME__ JTMS_SWDIO

/*--
if this:PinExist("A", 13) then
    this:AddWidget("Combobox")
    this:AddPinSpeed("A", 13)
end
--*/
#define __GPIO_PA_PIN_13_SPEED__ _GPIO_SPEED_HIGH

/*--
if this:PinExist("A", 13) then
    this:AddWidget("Combobox")
    this:AddPinMode("A", 13)
end
--*/
#define __GPIO_PA_PIN_13_MODE__ _GPIO_MODE_AF_PP

/*--
if this:PinExist("A", 13) then
    this:AddWidget("Combobox")
    this:AddPinAF("A", 13)
end
--*/
#define __GPIO_PA_PIN_13_AF__ 0

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
    PA14[#PA14 + 1] = {NAME = "PA14_NC", MODE = "_GPIO_MODE_IN", AF = "0", STATE = "_LOW"}
    PA14[#PA14 + 1] = {NAME = "JTCK_SWCLK", MODE = "_GPIO_MODE_AF_PP", AF = "0", STATE = "_LOW"}
    PA14[#PA14 + 1] = {NAME = "EVENTOUT", MODE = "_GPIO_MODE_AF_PP", AF = "15", STATE = "_LOW"}

    this:AddWidget("Textbox", "PA14")
    this:AddProposals(PA14, "A", 14)
end
--*/
#define __GPIO_PA_PIN_14_NAME__ JTCK_SWCLK

/*--
if this:PinExist("A", 14) then
    this:AddWidget("Combobox")
    this:AddPinSpeed("A", 14)
end
--*/
#define __GPIO_PA_PIN_14_SPEED__ _GPIO_SPEED_HIGH

/*--
if this:PinExist("A", 14) then
    this:AddWidget("Combobox")
    this:AddPinMode("A", 14)
end
--*/
#define __GPIO_PA_PIN_14_MODE__ _GPIO_MODE_AF_PP

/*--
if this:PinExist("A", 14) then
    this:AddWidget("Combobox")
    this:AddPinAF("A", 14)
end
--*/
#define __GPIO_PA_PIN_14_AF__ 0

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
    PA15[#PA15 + 1] = {NAME = "PA15_NC", MODE = "_GPIO_MODE_IN", AF = "0", STATE = "_LOW"}
    PA15[#PA15 + 1] = {NAME = "JTDI", MODE = "_GPIO_MODE_AF_PP", AF = "0", STATE = "_LOW"}
    PA15[#PA15 + 1] = {NAME = "TIM2_CH1", MODE = "_GPIO_MODE_AF_PP_PD", AF = "1", STATE = "_LOW"}
    PA15[#PA15 + 1] = {NAME = "TIM2_ETR", MODE = "_GPIO_MODE_AF_PP_PD", AF = "1", STATE = "_LOW"}
    PA15[#PA15 + 1] = {NAME = "SPI1_NSS", MODE = "_GPIO_MODE_AF_PP_PU", AF = "5", STATE = "_LOW"}
    PA15[#PA15 + 1] = {NAME = "SPI3_NSS", MODE = "_GPIO_MODE_AF_PP_PU", AF = "6", STATE = "_LOW"}
    PA15[#PA15 + 1] = {NAME = "I2S3_WS", MODE = "_GPIO_MODE_AF_PP", AF = "6", STATE = "_LOW"}
    PA15[#PA15 + 1] = {NAME = "EVENTOUT", MODE = "_GPIO_MODE_AF_PP", AF = "15", STATE = "_LOW"}

    this:AddWidget("Textbox", "PA15")
    this:AddProposals(PA15, "A", 15)
end
--*/
#define __GPIO_PA_PIN_15_NAME__ JTDI

/*--
if this:PinExist("A", 15) then
    this:AddWidget("Combobox")
    this:AddPinSpeed("A", 15)
end
--*/
#define __GPIO_PA_PIN_15_SPEED__ _GPIO_SPEED_HIGH

/*--
if this:PinExist("A", 15) then
    this:AddWidget("Combobox")
    this:AddPinMode("A", 15)
end
--*/
#define __GPIO_PA_PIN_15_MODE__ _GPIO_MODE_AF_PP

/*--
if this:PinExist("A", 15) then
    this:AddWidget("Combobox")
    this:AddPinAF("A", 15)
end
--*/
#define __GPIO_PA_PIN_15_AF__ 0

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
    PB0[#PB0 + 1] = {NAME = "PB0_NC", MODE = "_GPIO_MODE_IN", AF = "0", STATE = "_LOW"}
    PB0[#PB0 + 1] = {NAME = "TIM1_CH2N", MODE = "_GPIO_MODE_AF_PP_PD", AF = "1", STATE = "_LOW"}
    PB0[#PB0 + 1] = {NAME = "TIM3_CH3", MODE = "_GPIO_MODE_AF_PP_PD", AF = "2", STATE = "_LOW"}
    PB0[#PB0 + 1] = {NAME = "TIM8_CH2N", MODE = "_GPIO_MODE_AF_PP_PD", AF = "3", STATE = "_LOW"}
    PB0[#PB0 + 1] = {NAME = "LCD_R3", MODE = "_GPIO_MODE_AF_PP", AF = "9", STATE = "_LOW"}
    PB0[#PB0 + 1] = {NAME = "OTG_HS_ULPI_D1", MODE = "_GPIO_MODE_AF_PP", AF = "10", STATE = "_LOW"}
    PB0[#PB0 + 1] = {NAME = "ETH_MII_RXD2", MODE = "_GPIO_MODE_AF_PP", AF = "11", STATE = "_LOW"}
    PB0[#PB0 + 1] = {NAME = "LCD_G1", MODE = "_GPIO_MODE_AF_PP", AF = "14", STATE = "_LOW"}
    PB0[#PB0 + 1] = {NAME = "EVENTOUT", MODE = "_GPIO_MODE_AF_PP", AF = "15", STATE = "_LOW"}

    this:AddWidget("Textbox", "PB0")
    this:AddProposals(PB0, "B", 0)
end
--*/
#define __GPIO_PB_PIN_0_NAME__ PB0_NC

/*--
if this:PinExist("B", 0) then
    this:AddWidget("Combobox")
    this:AddPinSpeed("B", 0)
end
--*/
#define __GPIO_PB_PIN_0_SPEED__ _GPIO_SPEED_LOW

/*--
if this:PinExist("B", 0) then
    this:AddWidget("Combobox")
    this:AddPinMode("B", 0)
end
--*/
#define __GPIO_PB_PIN_0_MODE__ _GPIO_MODE_IN

/*--
if this:PinExist("B", 0) then
    this:AddWidget("Combobox")
    this:AddPinAF("B", 0)
end
--*/
#define __GPIO_PB_PIN_0_AF__ 0

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
    PB1[#PB1 + 1] = {NAME = "PB1_NC", MODE = "_GPIO_MODE_IN", AF = "0", STATE = "_LOW"}
    PB1[#PB1 + 1] = {NAME = "TIM1_CH3N", MODE = "_GPIO_MODE_AF_PP_PD", AF = "1", STATE = "_LOW"}
    PB1[#PB1 + 1] = {NAME = "TIM3_CH4", MODE = "_GPIO_MODE_AF_PP", AF = "2", STATE = "_LOW"}
    PB1[#PB1 + 1] = {NAME = "TIM8_CH3N", MODE = "_GPIO_MODE_AF_PP", AF = "3", STATE = "_LOW"}
    PB1[#PB1 + 1] = {NAME = "LCD_R6", MODE = "_GPIO_MODE_AF_PP", AF = "9", STATE = "_LOW"}
    PB1[#PB1 + 1] = {NAME = "OTG_HS_ULPI_D2", MODE = "_GPIO_MODE_AF_PP", AF = "10", STATE = "_LOW"}
    PB1[#PB1 + 1] = {NAME = "ETH_MII_RXD3", MODE = "_GPIO_MODE_AF_PP", AF = "11", STATE = "_LOW"}
    PB1[#PB1 + 1] = {NAME = "LCD_G0", MODE = "_GPIO_MODE_AF_PP", AF = "14", STATE = "_LOW"}
    PB1[#PB1 + 1] = {NAME = "EVENTOUT", MODE = "_GPIO_MODE_AF_PP", AF = "15", STATE = "_LOW"}

    this:AddWidget("Textbox", "PB1")
    this:AddProposals(PB1, "B", 1)
end
--*/
#define __GPIO_PB_PIN_1_NAME__ PB1_NC

/*--
if this:PinExist("B", 1) then
    this:AddWidget("Combobox")
    this:AddPinSpeed("B", 1)
end
--*/
#define __GPIO_PB_PIN_1_SPEED__ _GPIO_SPEED_LOW

/*--
if this:PinExist("B", 1) then
    this:AddWidget("Combobox")
    this:AddPinMode("B", 1)
end
--*/
#define __GPIO_PB_PIN_1_MODE__ _GPIO_MODE_IN

/*--
if this:PinExist("B", 1) then
    this:AddWidget("Combobox")
    this:AddPinAF("B", 1)
end
--*/
#define __GPIO_PB_PIN_1_AF__ 0

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
    PB2[#PB2 + 1] = {NAME = "PB2_NC", MODE = "_GPIO_MODE_IN", AF = "0", STATE = "_LOW"}
    PB2[#PB2 + 1] = {NAME = "EVENTOUT", MODE = "_GPIO_MODE_AF_PP", AF = "15", STATE = "_LOW"}

    this:AddWidget("Textbox", "PB2")
    this:AddProposals(PB2, "B", 2)
end
--*/
#define __GPIO_PB_PIN_2_NAME__ PB2_NC

/*--
if this:PinExist("B", 2) then
    this:AddWidget("Combobox")
    this:AddPinSpeed("B", 2)
end
--*/
#define __GPIO_PB_PIN_2_SPEED__ _GPIO_SPEED_LOW

/*--
if this:PinExist("B", 2) then
    this:AddWidget("Combobox")
    this:AddPinMode("B", 2)
end
--*/
#define __GPIO_PB_PIN_2_MODE__ _GPIO_MODE_IN

/*--
if this:PinExist("B", 2) then
    this:AddWidget("Combobox")
    this:AddPinAF("B", 2)
end
--*/
#define __GPIO_PB_PIN_2_AF__ 0

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
    PB3[#PB3 + 1] = {NAME = "PB3_NC", MODE = "_GPIO_MODE_IN", AF = "0", STATE = "_LOW"}
    PB3[#PB3 + 1] = {NAME = "JTDO", MODE = "_GPIO_MODE_AF_PP", AF = "0", STATE = "_LOW"}
    PB3[#PB3 + 1] = {NAME = "TRACESWO", MODE = "_GPIO_MODE_AF_PP", AF = "0", STATE = "_LOW"}
    PB3[#PB3 + 1] = {NAME = "TIM2_CH2", MODE = "_GPIO_MODE_AF_PP", AF = "1", STATE = "_LOW"}
    PB3[#PB3 + 1] = {NAME = "SPI1_SCK", MODE = "_GPIO_MODE_AF_PP", AF = "5", STATE = "_LOW"}
    PB3[#PB3 + 1] = {NAME = "SPI3_SCK", MODE = "_GPIO_MODE_AF_PP", AF = "6", STATE = "_LOW"}
    PB3[#PB3 + 1] = {NAME = "I2S3_CK", MODE = "_GPIO_MODE_AF_PP", AF = "6", STATE = "_LOW"}
    PB3[#PB3 + 1] = {NAME = "EVENTOUT", MODE = "_GPIO_MODE_AF_PP", AF = "15", STATE = "_LOW"}

    this:AddWidget("Textbox", "PB3")
    this:AddProposals(PB3, "B", 3)
end
--*/
#define __GPIO_PB_PIN_3_NAME__ PB3_NC

/*--
if this:PinExist("B", 3) then
    this:AddWidget("Combobox")
    this:AddPinSpeed("B", 3)
end
--*/
#define __GPIO_PB_PIN_3_SPEED__ _GPIO_SPEED_LOW

/*--
if this:PinExist("B", 3) then
    this:AddWidget("Combobox")
    this:AddPinMode("B", 3)
end
--*/
#define __GPIO_PB_PIN_3_MODE__ _GPIO_MODE_IN

/*--
if this:PinExist("B", 3) then
    this:AddWidget("Combobox")
    this:AddPinAF("B", 3)
end
--*/
#define __GPIO_PB_PIN_3_AF__ 0

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
    PB4[#PB4 + 1] = {NAME = "PB4_NC", MODE = "_GPIO_MODE_IN", AF = "0", STATE = "_LOW"}
    PB4[#PB4 + 1] = {NAME = "NJTRST", MODE = "_GPIO_MODE_AF_PP", AF = "0", STATE = "_LOW"}
    PB4[#PB4 + 1] = {NAME = "TIM3_CH1", MODE = "_GPIO_MODE_AF_PP", AF = "2", STATE = "_LOW"}
    PB4[#PB4 + 1] = {NAME = "SPI1_MISO", MODE = "_GPIO_MODE_AF_PP", AF = "5", STATE = "_LOW"}
    PB4[#PB4 + 1] = {NAME = "SPI3_MISO", MODE = "_GPIO_MODE_AF_PP", AF = "6", STATE = "_LOW"}
    PB4[#PB4 + 1] = {NAME = "I2S3EXT_SD", MODE = "_GPIO_MODE_AF_PP", AF = "7", STATE = "_LOW"}
    PB4[#PB4 + 1] = {NAME = "EVENTOUT", MODE = "_GPIO_MODE_AF_PP", AF = "15", STATE = "_LOW"}

    this:AddWidget("Textbox", "PB4")
    this:AddProposals(PB4, "B", 4)
end
--*/
#define __GPIO_PB_PIN_4_NAME__ PB4_NC

/*--
if this:PinExist("B", 4) then
    this:AddWidget("Combobox")
    this:AddPinSpeed("B", 4)
end
--*/
#define __GPIO_PB_PIN_4_SPEED__ _GPIO_SPEED_LOW

/*--
if this:PinExist("B", 4) then
    this:AddWidget("Combobox")
    this:AddPinMode("B", 4)
end
--*/
#define __GPIO_PB_PIN_4_MODE__ _GPIO_MODE_IN

/*--
if this:PinExist("B", 4) then
    this:AddWidget("Combobox")
    this:AddPinAF("B", 4)
end
--*/
#define __GPIO_PB_PIN_4_AF__ 0

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
    PB5[#PB5 + 1] = {NAME = "PB5_NC", MODE = "_GPIO_MODE_IN", AF = "0", STATE = "_LOW"}
    PB5[#PB5 + 1] = {NAME = "TIM3_CH2", MODE = "_GPIO_MODE_AF_PP", AF = "2", STATE = "_LOW"}
    PB5[#PB5 + 1] = {NAME = "I2C1_SMBA", MODE = "_GPIO_MODE_AF_OD_PU", AF = "4", STATE = "_LOW"}
    PB5[#PB5 + 1] = {NAME = "SPI1_MOSI", MODE = "_GPIO_MODE_AF_PP", AF = "5", STATE = "_LOW"}
    PB5[#PB5 + 1] = {NAME = "SPI3_MOSI", MODE = "_GPIO_MODE_AF_PP", AF = "6", STATE = "_LOW"}
    PB5[#PB5 + 1] = {NAME = "I2S3_SD", MODE = "_GPIO_MODE_AF_PP", AF = "6", STATE = "_LOW"}
    PB5[#PB5 + 1] = {NAME = "CAN2_RX", MODE = "_GPIO_MODE_AF_PP", AF = "9", STATE = "_LOW"}
    PB5[#PB5 + 1] = {NAME = "OTG_HS_ULPI_D7", MODE = "_GPIO_MODE_AF_PP", AF = "10", STATE = "_LOW"}
    PB5[#PB5 + 1] = {NAME = "ETH_PPS_OUT", MODE = "_GPIO_MODE_AF_PP", AF = "11", STATE = "_LOW"}
    PB5[#PB5 + 1] = {NAME = "FMC_SDCKE1", MODE = "_GPIO_MODE_AF_PP", AF = "12", STATE = "_LOW"}
    PB5[#PB5 + 1] = {NAME = "DCMI_D10", MODE = "_GPIO_MODE_AF_PP", AF = "13", STATE = "_LOW"}
    PB5[#PB5 + 1] = {NAME = "LCD_G7", MODE = "_GPIO_MODE_AF_PP", AF = "14", STATE = "_LOW"}
    PB5[#PB5 + 1] = {NAME = "EVENTOUT", MODE = "_GPIO_MODE_AF_PP", AF = "15", STATE = "_LOW"}

    this:AddWidget("Textbox", "PB5")
    this:AddProposals(PB5, "B", 5)
end
--*/
#define __GPIO_PB_PIN_5_NAME__ PB5_NC

/*--
if this:PinExist("B", 5) then
    this:AddWidget("Combobox")
    this:AddPinSpeed("B", 5)
end
--*/
#define __GPIO_PB_PIN_5_SPEED__ _GPIO_SPEED_LOW

/*--
if this:PinExist("B", 5) then
    this:AddWidget("Combobox")
    this:AddPinMode("B", 5)
end
--*/
#define __GPIO_PB_PIN_5_MODE__ _GPIO_MODE_IN

/*--
if this:PinExist("B", 5) then
    this:AddWidget("Combobox")
    this:AddPinAF("B", 5)
end
--*/
#define __GPIO_PB_PIN_5_AF__ 0

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
    PB6[#PB6 + 1] = {NAME = "PB6_NC", MODE = "_GPIO_MODE_IN", AF = "0", STATE = "_LOW"}
    PB6[#PB6 + 1] = {NAME = "TIM4_CH1", MODE = "_GPIO_MODE_AF_PP", AF = "2", STATE = "_LOW"}
    PB6[#PB6 + 1] = {NAME = "I2C1_SCL", MODE = "_GPIO_MODE_AF_OD_PU", AF = "4", STATE = "_LOW"}
    PB6[#PB6 + 1] = {NAME = "USART1_TX", MODE = "_GPIO_MODE_AF_PP", AF = "7", STATE = "_LOW"}
    PB6[#PB6 + 1] = {NAME = "CAN2_TX", MODE = "_GPIO_MODE_AF_PP", AF = "9", STATE = "_LOW"}
    PB6[#PB6 + 1] = {NAME = "QSPI_BK1_NCS", MODE = "_GPIO_MODE_AF_PP", AF = "10", STATE = "_LOW"}
    PB6[#PB6 + 1] = {NAME = "FMC_SDNE1", MODE = "_GPIO_MODE_AF_PP", AF = "12", STATE = "_LOW"}
    PB6[#PB6 + 1] = {NAME = "DCMI_D5", MODE = "_GPIO_MODE_AF_PP", AF = "13", STATE = "_LOW"}
    PB6[#PB6 + 1] = {NAME = "EVENTOUT", MODE = "_GPIO_MODE_AF_PP", AF = "15", STATE = "_LOW"}

    this:AddWidget("Textbox", "PB6")
    this:AddProposals(PB6, "B", 6)
end
--*/
#define __GPIO_PB_PIN_6_NAME__ PB6_NC

/*--
if this:PinExist("B", 6) then
    this:AddWidget("Combobox")
    this:AddPinSpeed("B", 6)
end
--*/
#define __GPIO_PB_PIN_6_SPEED__ _GPIO_SPEED_LOW

/*--
if this:PinExist("B", 6) then
    this:AddWidget("Combobox")
    this:AddPinMode("B", 6)
end
--*/
#define __GPIO_PB_PIN_6_MODE__ _GPIO_MODE_IN

/*--
if this:PinExist("B", 6) then
    this:AddWidget("Combobox")
    this:AddPinAF("B", 6)
end
--*/
#define __GPIO_PB_PIN_6_AF__ 0

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
    PB7[#PB7 + 1] = {NAME = "PB7_NC", MODE = "_GPIO_MODE_IN", AF = "0", STATE = "_LOW"}
    PB7[#PB7 + 1] = {NAME = "TIM3_CH2", MODE = "_GPIO_MODE_AF_PP", AF = "2", STATE = "_LOW"}
    PB7[#PB7 + 1] = {NAME = "I2C1_SDA", MODE = "_GPIO_MODE_AF_OD_PU", AF = "4", STATE = "_LOW"}
    PB7[#PB7 + 1] = {NAME = "USART1_RX", MODE = "_GPIO_MODE_AF_PP_PU", AF = "7", STATE = "_LOW"}
    PB7[#PB7 + 1] = {NAME = "FMC_NL", MODE = "_GPIO_MODE_AF_PP", AF = "12", STATE = "_LOW"}
    PB7[#PB7 + 1] = {NAME = "DCMI_VSYNC", MODE = "_GPIO_MODE_AF_PP", AF = "13", STATE = "_LOW"}
    PB7[#PB7 + 1] = {NAME = "EVENTOUT", MODE = "_GPIO_MODE_AF_PP", AF = "15", STATE = "_LOW"}

    this:AddWidget("Textbox", "PB7")
    this:AddProposals(PB7, "B", 7)
end
--*/
#define __GPIO_PB_PIN_7_NAME__ PB7_NC

/*--
if this:PinExist("B", 7) then
    this:AddWidget("Combobox")
    this:AddPinSpeed("B", 7)
end
--*/
#define __GPIO_PB_PIN_7_SPEED__ _GPIO_SPEED_LOW

/*--
if this:PinExist("B", 7) then
    this:AddWidget("Combobox")
    this:AddPinMode("B", 7)
end
--*/
#define __GPIO_PB_PIN_7_MODE__ _GPIO_MODE_IN

/*--
if this:PinExist("B", 7) then
    this:AddWidget("Combobox")
    this:AddPinAF("B", 7)
end
--*/
#define __GPIO_PB_PIN_7_AF__ 0

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
    PB8[#PB8 + 1] = {NAME = "PB8_NC", MODE = "_GPIO_MODE_IN", AF = "0", STATE = "_LOW"}
    PB8[#PB8 + 1] = {NAME = "TIM4_CH3", MODE = "_GPIO_MODE_AF_PP", AF = "2", STATE = "_LOW"}
    PB8[#PB8 + 1] = {NAME = "TIM10_CH1", MODE = "_GPIO_MODE_AF_PP", AF = "3", STATE = "_LOW"}
    PB8[#PB8 + 1] = {NAME = "I2C1_SCL", MODE = "_GPIO_MODE_AF_OD_PU", AF = "4", STATE = "_LOW"}
    PB8[#PB8 + 1] = {NAME = "CAN1_RX", MODE = "_GPIO_MODE_AF_PP", AF = "9", STATE = "_LOW"}
    PB8[#PB8 + 1] = {NAME = "ETH_MII_TXD3", MODE = "_GPIO_MODE_AF_PP", AF = "11", STATE = "_LOW"}
    PB8[#PB8 + 1] = {NAME = "SDIO_D4", MODE = "_GPIO_MODE_AF_PP", AF = "12", STATE = "_LOW"}
    PB8[#PB8 + 1] = {NAME = "DCMI_D6", MODE = "_GPIO_MODE_AF_PP", AF = "13", STATE = "_LOW"}
    PB8[#PB8 + 1] = {NAME = "LCD_B6", MODE = "_GPIO_MODE_AF_PP", AF = "14", STATE = "_LOW"}
    PB8[#PB8 + 1] = {NAME = "EVENTOUT", MODE = "_GPIO_MODE_AF_PP", AF = "15", STATE = "_LOW"}

    this:AddWidget("Textbox", "PB8")
    this:AddProposals(PB8, "B", 8)
end
--*/
#define __GPIO_PB_PIN_8_NAME__ PB8_NC

/*--
if this:PinExist("B", 8) then
    this:AddWidget("Combobox")
    this:AddPinSpeed("B", 8)
end
--*/
#define __GPIO_PB_PIN_8_SPEED__ _GPIO_SPEED_LOW

/*--
if this:PinExist("B", 8) then
    this:AddWidget("Combobox")
    this:AddPinMode("B", 8)
end
--*/
#define __GPIO_PB_PIN_8_MODE__ _GPIO_MODE_IN

/*--
if this:PinExist("B", 8) then
    this:AddWidget("Combobox")
    this:AddPinAF("B", 8)
end
--*/
#define __GPIO_PB_PIN_8_AF__ 0

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
    PB9[#PB9 + 1] = {NAME = "PB9_NC", MODE = "_GPIO_MODE_IN", AF = "0", STATE = "_LOW"}
    PB9[#PB9 + 1] = {NAME = "TIM4_CH4", MODE = "_GPIO_MODE_AF_PP", AF = "2", STATE = "_LOW"}
    PB9[#PB9 + 1] = {NAME = "TIM11_CH1", MODE = "_GPIO_MODE_AF_PP", AF = "3", STATE = "_LOW"}
    PB9[#PB9 + 1] = {NAME = "I2C1_SDA", MODE = "_GPIO_MODE_AF_OD_PU", AF = "4", STATE = "_LOW"}
    PB9[#PB9 + 1] = {NAME = "SPI2_NSS", MODE = "_GPIO_MODE_AF_PP_PU", AF = "5", STATE = "_LOW"}
    PB9[#PB9 + 1] = {NAME = "I2S2_WS", MODE = "_GPIO_MODE_AF_PP", AF = "5", STATE = "_LOW"}
    PB9[#PB9 + 1] = {NAME = "CAN1_TX", MODE = "_GPIO_MODE_AF_PP", AF = "9", STATE = "_LOW"}
    PB9[#PB9 + 1] = {NAME = "SDIO_D5", MODE = "_GPIO_MODE_AF_PP", AF = "12", STATE = "_LOW"}
    PB9[#PB9 + 1] = {NAME = "DCMI_D7", MODE = "_GPIO_MODE_AF_PP", AF = "13", STATE = "_LOW"}
    PB9[#PB9 + 1] = {NAME = "LCD_B7", MODE = "_GPIO_MODE_AF_PP", AF = "14", STATE = "_LOW"}
    PB9[#PB9 + 1] = {NAME = "EVENTOUT", MODE = "_GPIO_MODE_AF_PP", AF = "15", STATE = "_LOW"}

    this:AddWidget("Textbox", "PB9")
    this:AddProposals(PB9, "B", 9)
end
--*/
#define __GPIO_PB_PIN_9_NAME__ PB9_NC

/*--
if this:PinExist("B", 9) then
    this:AddWidget("Combobox")
    this:AddPinSpeed("B", 9)
end
--*/
#define __GPIO_PB_PIN_9_SPEED__ _GPIO_SPEED_LOW

/*--
if this:PinExist("B", 9) then
    this:AddWidget("Combobox")
    this:AddPinMode("B", 9)
end
--*/
#define __GPIO_PB_PIN_9_MODE__ _GPIO_MODE_IN

/*--
if this:PinExist("B", 9) then
    this:AddWidget("Combobox")
    this:AddPinAF("B", 9)
end
--*/
#define __GPIO_PB_PIN_9_AF__ 0

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
    PB10[#PB10 + 1] = {NAME = "PB10_NC", MODE = "_GPIO_MODE_IN", AF = "0", STATE = "_LOW"}
    PB10[#PB10 + 1] = {NAME = "TIM2_CH3", MODE = "_GPIO_MODE_AF_PP", AF = "1", STATE = "_LOW"}
    PB10[#PB10 + 1] = {NAME = "I2C2_SCL", MODE = "_GPIO_MODE_AF_OD_PU", AF = "4", STATE = "_LOW"}
    PB10[#PB10 + 1] = {NAME = "SPI2_SCK", MODE = "_GPIO_MODE_AF_PP", AF = "5", STATE = "_LOW"}
    PB10[#PB10 + 1] = {NAME = "I2S2_CK", MODE = "_GPIO_MODE_AF_PP", AF = "5", STATE = "_LOW"}
    PB10[#PB10 + 1] = {NAME = "USART3_TX", MODE = "_GPIO_MODE_AF_PP", AF = "7", STATE = "_LOW"}
    PB10[#PB10 + 1] = {NAME = "QSPI_BK1_NCS", MODE = "_GPIO_MODE_AF_PP", AF = "9", STATE = "_LOW"}
    PB10[#PB10 + 1] = {NAME = "OTG_HS_ULPI_D3", MODE = "_GPIO_MODE_AF_PP", AF = "10", STATE = "_LOW"}
    PB10[#PB10 + 1] = {NAME = "ETH_MII_RX_ER", MODE = "_GPIO_MODE_AF_PP", AF = "11", STATE = "_LOW"}
    PB10[#PB10 + 1] = {NAME = "LCD_G4", MODE = "_GPIO_MODE_AF_PP", AF = "14", STATE = "_LOW"}
    PB10[#PB10 + 1] = {NAME = "EVENTOUT", MODE = "_GPIO_MODE_AF_PP", AF = "15", STATE = "_LOW"}

    this:AddWidget("Textbox", "PB10")
    this:AddProposals(PB10, "B", 10)
end
--*/
#define __GPIO_PB_PIN_10_NAME__ PB10_NC

/*--
if this:PinExist("B", 10) then
    this:AddWidget("Combobox")
    this:AddPinSpeed("B", 10)
end
--*/
#define __GPIO_PB_PIN_10_SPEED__ _GPIO_SPEED_LOW

/*--
if this:PinExist("B", 10) then
    this:AddWidget("Combobox")
    this:AddPinMode("B", 10)
end
--*/
#define __GPIO_PB_PIN_10_MODE__ _GPIO_MODE_IN

/*--
if this:PinExist("B", 10) then
    this:AddWidget("Combobox")
    this:AddPinAF("B", 10)
end
--*/
#define __GPIO_PB_PIN_10_AF__ 0

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
    PB11[#PB11 + 1] = {NAME = "PB11_NC", MODE = "_GPIO_MODE_IN", AF = "0", STATE = "_LOW"}
    PB11[#PB11 + 1] = {NAME = "TIM2_CH4", MODE = "_GPIO_MODE_AF_PP", AF = "1", STATE = "_LOW"}
    PB11[#PB11 + 1] = {NAME = "I2C2_SDA", MODE = "_GPIO_MODE_AF_OD_PU", AF = "4", STATE = "_LOW"}
    PB11[#PB11 + 1] = {NAME = "USART3_RX", MODE = "_GPIO_MODE_AF_PP_PU", AF = "7", STATE = "_LOW"}
    PB11[#PB11 + 1] = {NAME = "OTG_HS_ULPI_D4", MODE = "_GPIO_MODE_AF_PP", AF = "10", STATE = "_LOW"}
    PB11[#PB11 + 1] = {NAME = "ETH_MII_TX_EN", MODE = "_GPIO_MODE_AF_PP", AF = "11", STATE = "_LOW"}
    PB11[#PB11 + 1] = {NAME = "ETH_RMII_TX_EN", MODE = "_GPIO_MODE_AF_PP", AF = "11", STATE = "_LOW"}
    PB11[#PB11 + 1] = {NAME = "DSIHOST_TE", MODE = "_GPIO_MODE_AF_PP", AF = "13", STATE = "_LOW"}
    PB11[#PB11 + 1] = {NAME = "LCD_G5", MODE = "_GPIO_MODE_AF_PP", AF = "14", STATE = "_LOW"}
    PB11[#PB11 + 1] = {NAME = "EVENTOUT", MODE = "_GPIO_MODE_AF_PP", AF = "15", STATE = "_LOW"}

    this:AddWidget("Textbox", "PB11")
    this:AddProposals(PB11, "B", 11)
end
--*/
#define __GPIO_PB_PIN_11_NAME__ PB11_NC

/*--
if this:PinExist("B", 11) then
    this:AddWidget("Combobox")
    this:AddPinSpeed("B", 11)
end
--*/
#define __GPIO_PB_PIN_11_SPEED__ _GPIO_SPEED_LOW

/*--
if this:PinExist("B", 11) then
    this:AddWidget("Combobox")
    this:AddPinMode("B", 11)
end
--*/
#define __GPIO_PB_PIN_11_MODE__ _GPIO_MODE_IN

/*--
if this:PinExist("B", 11) then
    this:AddWidget("Combobox")
    this:AddPinAF("B", 11)
end
--*/
#define __GPIO_PB_PIN_11_AF__ 0

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
    PB12[#PB12 + 1] = {NAME = "PB12_NC", MODE = "_GPIO_MODE_IN", AF = "0", STATE = "_LOW"}
    PB12[#PB12 + 1] = {NAME = "TIM1_BKIN", MODE = "_GPIO_MODE_AF_PP", AF = "1", STATE = "_LOW"}
    PB12[#PB12 + 1] = {NAME = "I2C2_SMBA", MODE = "_GPIO_MODE_AF_OD_PU", AF = "4", STATE = "_LOW"}
    PB12[#PB12 + 1] = {NAME = "SPI2_NSS", MODE = "_GPIO_MODE_AF_PP_PU", AF = "5", STATE = "_LOW"}
    PB12[#PB12 + 1] = {NAME = "I2S2_WS", MODE = "_GPIO_MODE_AF_PP", AF = "5", STATE = "_LOW"}
    PB12[#PB12 + 1] = {NAME = "USART3_CK", MODE = "_GPIO_MODE_AF_PP", AF = "7", STATE = "_LOW"}
    PB12[#PB12 + 1] = {NAME = "CAN2_RX", MODE = "_GPIO_MODE_AF_PP", AF = "9", STATE = "_LOW"}
    PB12[#PB12 + 1] = {NAME = "OTG_HS_ULPI_D5", MODE = "_GPIO_MODE_AF_PP", AF = "10", STATE = "_LOW"}
    PB12[#PB12 + 1] = {NAME = "ETH_MII_TXD0", MODE = "_GPIO_MODE_AF_PP", AF = "11", STATE = "_LOW"}
    PB12[#PB12 + 1] = {NAME = "ETH_RMII_TXD0", MODE = "_GPIO_MODE_AF_PP", AF = "11", STATE = "_LOW"}
    PB12[#PB12 + 1] = {NAME = "OTG_HS_ID", MODE = "_GPIO_MODE_AF_PP_PD", AF = "12", STATE = "_LOW"}
    PB12[#PB12 + 1] = {NAME = "EVENTOUT", MODE = "_GPIO_MODE_AF_PP", AF = "15", STATE = "_LOW"}

    this:AddWidget("Textbox", "PB12")
    this:AddProposals(PB12, "B", 12)
end
--*/
#define __GPIO_PB_PIN_12_NAME__ PB12_NC

/*--
if this:PinExist("B", 12) then
    this:AddWidget("Combobox")
    this:AddPinSpeed("B", 12)
end
--*/
#define __GPIO_PB_PIN_12_SPEED__ _GPIO_SPEED_LOW

/*--
if this:PinExist("B", 12) then
    this:AddWidget("Combobox")
    this:AddPinMode("B", 12)
end
--*/
#define __GPIO_PB_PIN_12_MODE__ _GPIO_MODE_IN

/*--
if this:PinExist("B", 12) then
    this:AddWidget("Combobox")
    this:AddPinAF("B", 12)
end
--*/
#define __GPIO_PB_PIN_12_AF__ 0

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
    PB13[#PB13 + 1] = {NAME = "PB13_NC", MODE = "_GPIO_MODE_IN", AF = "0", STATE = "_LOW"}
    PB13[#PB13 + 1] = {NAME = "TIM1_CH1N", MODE = "_GPIO_MODE_AF_PP", AF = "1", STATE = "_LOW"}
    PB13[#PB13 + 1] = {NAME = "SPI2_SCK", MODE = "_GPIO_MODE_AF_PP", AF = "5", STATE = "_LOW"}
    PB13[#PB13 + 1] = {NAME = "I2S2_CK", MODE = "_GPIO_MODE_AF_PP", AF = "5", STATE = "_LOW"}
    PB13[#PB13 + 1] = {NAME = "USART3_CTS", MODE = "_GPIO_MODE_AF_PP_PU", AF = "7", STATE = "_LOW"}
    PB13[#PB13 + 1] = {NAME = "CAN2_TX", MODE = "_GPIO_MODE_AF_PP", AF = "9", STATE = "_LOW"}
    PB13[#PB13 + 1] = {NAME = "OTG_HS_ULPI_D6", MODE = "_GPIO_MODE_AF_PP", AF = "10", STATE = "_LOW"}
    PB13[#PB13 + 1] = {NAME = "ETH_MII_TXD1", MODE = "_GPIO_MODE_AF_PP", AF = "11", STATE = "_LOW"}
    PB13[#PB13 + 1] = {NAME = "ETH_RMII_TXD1", MODE = "_GPIO_MODE_AF_PP", AF = "11", STATE = "_LOW"}
    PB13[#PB13 + 1] = {NAME = "EVENTOUT", MODE = "_GPIO_MODE_AF_PP", AF = "15", STATE = "_LOW"}

    this:AddWidget("Textbox", "PB13")
    this:AddProposals(PB13, "B", 13)
end
--*/
#define __GPIO_PB_PIN_13_NAME__ PB13_NC

/*--
if this:PinExist("B", 13) then
    this:AddWidget("Combobox")
    this:AddPinSpeed("B", 13)
end
--*/
#define __GPIO_PB_PIN_13_SPEED__ _GPIO_SPEED_LOW

/*--
if this:PinExist("B", 13) then
    this:AddWidget("Combobox")
    this:AddPinMode("B", 13)
end
--*/
#define __GPIO_PB_PIN_13_MODE__ _GPIO_MODE_IN

/*--
if this:PinExist("B", 13) then
    this:AddWidget("Combobox")
    this:AddPinAF("B", 13)
end
--*/
#define __GPIO_PB_PIN_13_AF__ 0

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
    PB14[#PB14 + 1] = {NAME = "PB14_NC", MODE = "_GPIO_MODE_IN", AF = "0", STATE = "_LOW"}
    PB14[#PB14 + 1] = {NAME = "TIM1_CH2N", MODE = "_GPIO_MODE_AF_PP", AF = "1", STATE = "_LOW"}
    PB14[#PB14 + 1] = {NAME = "TIM8_CH2N", MODE = "_GPIO_MODE_AF_PP", AF = "3", STATE = "_LOW"}
    PB14[#PB14 + 1] = {NAME = "SPI2_MISO", MODE = "_GPIO_MODE_AF_PP", AF = "5", STATE = "_LOW"}
    PB14[#PB14 + 1] = {NAME = "I2S2EXT_SD", MODE = "_GPIO_MODE_AF_PP", AF = "6", STATE = "_LOW"}
    PB14[#PB14 + 1] = {NAME = "USART3_RTS", MODE = "_GPIO_MODE_AF_PP", AF = "7", STATE = "_LOW"}
    PB14[#PB14 + 1] = {NAME = "TIM12_CH1", MODE = "_GPIO_MODE_AF_PP", AF = "9", STATE = "_LOW"}
    PB14[#PB14 + 1] = {NAME = "OTG_HS_DM", MODE = "_GPIO_MODE_AF_PP", AF = "12", STATE = "_LOW"}
    PB14[#PB14 + 1] = {NAME = "EVENTOUT", MODE = "_GPIO_MODE_AF_PP", AF = "15", STATE = "_LOW"}

    this:AddWidget("Textbox", "PB14")
    this:AddProposals(PB14, "B", 14)
end
--*/
#define __GPIO_PB_PIN_14_NAME__ PB14_NC

/*--
if this:PinExist("B", 14) then
    this:AddWidget("Combobox")
    this:AddPinSpeed("B", 14)
end
--*/
#define __GPIO_PB_PIN_14_SPEED__ _GPIO_SPEED_LOW

/*--
if this:PinExist("B", 14) then
    this:AddWidget("Combobox")
    this:AddPinMode("B", 14)
end
--*/
#define __GPIO_PB_PIN_14_MODE__ _GPIO_MODE_IN

/*--
if this:PinExist("B", 14) then
    this:AddWidget("Combobox")
    this:AddPinAF("B", 14)
end
--*/
#define __GPIO_PB_PIN_14_AF__ 0

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
    PB15[#PB15 + 1] = {NAME = "PB15_NC", MODE = "_GPIO_MODE_IN", AF = "0", STATE = "_LOW"}
    PB15[#PB15 + 1] = {NAME = "RTC_REFIN", MODE = "_GPIO_MODE_AF_PP", AF = "0", STATE = "_LOW"}
    PB15[#PB15 + 1] = {NAME = "TIM1_CH3N", MODE = "_GPIO_MODE_AF_PP", AF = "1", STATE = "_LOW"}
    PB15[#PB15 + 1] = {NAME = "TIM8_CH3N", MODE = "_GPIO_MODE_AF_PP", AF = "3", STATE = "_LOW"}
    PB15[#PB15 + 1] = {NAME = "SPI2_MOSI", MODE = "_GPIO_MODE_AF_PP", AF = "5", STATE = "_LOW"}
    PB15[#PB15 + 1] = {NAME = "I2S2_SD", MODE = "_GPIO_MODE_AF_PP", AF = "5", STATE = "_LOW"}
    PB15[#PB15 + 1] = {NAME = "TIM12_CH2", MODE = "_GPIO_MODE_AF_PP", AF = "9", STATE = "_LOW"}
    PB15[#PB15 + 1] = {NAME = "OTG_HS_DP", MODE = "_GPIO_MODE_AF_PP", AF = "12", STATE = "_LOW"}
    PB15[#PB15 + 1] = {NAME = "EVENTOUT", MODE = "_GPIO_MODE_AF_PP", AF = "15", STATE = "_LOW"}

    this:AddWidget("Textbox", "PB15")
    this:AddProposals(PB15, "B", 15)
end
--*/
#define __GPIO_PB_PIN_15_NAME__ PB15_NC

/*--
if this:PinExist("B", 15) then
    this:AddWidget("Combobox")
    this:AddPinSpeed("B", 15)
end
--*/
#define __GPIO_PB_PIN_15_SPEED__ _GPIO_SPEED_LOW

/*--
if this:PinExist("B", 15) then
    this:AddWidget("Combobox")
    this:AddPinMode("B", 15)
end
--*/
#define __GPIO_PB_PIN_15_MODE__ _GPIO_MODE_IN

/*--
if this:PinExist("B", 15) then
    this:AddWidget("Combobox")
    this:AddPinAF("B", 15)
end
--*/
#define __GPIO_PB_PIN_15_AF__ 0

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
    PC0[#PC0 + 1] = {NAME = "PC0_NC", MODE = "_GPIO_MODE_IN", AF = "0", STATE = "_LOW"}
    PC0[#PC0 + 1] = {NAME = "OTG_HS_ULPI_STP", MODE = "_GPIO_MODE_AF_PP", AF = "10", STATE = "_LOW"}
    PC0[#PC0 + 1] = {NAME = "FMC_SDNWE", MODE = "_GPIO_MODE_AF_PP", AF = "12", STATE = "_LOW"}
    PC0[#PC0 + 1] = {NAME = "LCD_R5", MODE = "_GPIO_MODE_AF_PP", AF = "14", STATE = "_LOW"}
    PC0[#PC0 + 1] = {NAME = "EVENTOUT", MODE = "_GPIO_MODE_AF_PP", AF = "15", STATE = "_LOW"}

    this:AddWidget("Textbox", "PC0")
    this:AddProposals(PC0, "C", 0)
end
--*/
#define __GPIO_PC_PIN_0_NAME__ PC0_NC

/*--
if this:PinExist("C", 0) then
    this:AddWidget("Combobox")
    this:AddPinSpeed("C", 0)
end
--*/
#define __GPIO_PC_PIN_0_SPEED__ _GPIO_SPEED_LOW

/*--
if this:PinExist("C", 0) then
    this:AddWidget("Combobox")
    this:AddPinMode("C", 0)
end
--*/
#define __GPIO_PC_PIN_0_MODE__ _GPIO_MODE_IN

/*--
if this:PinExist("C", 0) then
    this:AddWidget("Combobox")
    this:AddPinAF("C", 0)
end
--*/
#define __GPIO_PC_PIN_0_AF__ 0

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
    PC1[#PC1 + 1] = {NAME = "PC1_NC", MODE = "_GPIO_MODE_IN", AF = "0", STATE = "_LOW"}
    PC1[#PC1 + 1] = {NAME = "TRACED0", MODE = "_GPIO_MODE_AF_PP", AF = "0", STATE = "_LOW"}
    PC1[#PC1 + 1] = {NAME = "SPI2_MOSI", MODE = "_GPIO_MODE_AF_PP", AF = "5", STATE = "_LOW"}
    PC1[#PC1 + 1] = {NAME = "I2S2_SD", MODE = "_GPIO_MODE_AF_PP", AF = "5", STATE = "_LOW"}
    PC1[#PC1 + 1] = {NAME = "SAI1_SD_A", MODE = "_GPIO_MODE_AF_PP", AF = "6", STATE = "_LOW"}
    PC1[#PC1 + 1] = {NAME = "ETH_MDC", MODE = "_GPIO_MODE_AF_PP", AF = "11", STATE = "_LOW"}
    PC1[#PC1 + 1] = {NAME = "EVENTOUT", MODE = "_GPIO_MODE_AF_PP", AF = "15", STATE = "_LOW"}

    this:AddWidget("Textbox", "PC1")
    this:AddProposals(PC1, "C", 1)
end
--*/
#define __GPIO_PC_PIN_1_NAME__ PC1_NC

/*--
if this:PinExist("C", 1) then
    this:AddWidget("Combobox")
    this:AddPinSpeed("C", 1)
end
--*/
#define __GPIO_PC_PIN_1_SPEED__ _GPIO_SPEED_LOW

/*--
if this:PinExist("C", 1) then
    this:AddWidget("Combobox")
    this:AddPinMode("C", 1)
end
--*/
#define __GPIO_PC_PIN_1_MODE__ _GPIO_MODE_IN

/*--
if this:PinExist("C", 1) then
    this:AddWidget("Combobox")
    this:AddPinAF("C", 1)
end
--*/
#define __GPIO_PC_PIN_1_AF__ 0

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
    PC2[#PC2 + 1] = {NAME = "PC2_NC", MODE = "_GPIO_MODE_IN", AF = "0", STATE = "_LOW"}
    PC2[#PC2 + 1] = {NAME = "SPI2_MISO", MODE = "_GPIO_MODE_AF_PP", AF = "5", STATE = "_LOW"}
    PC2[#PC2 + 1] = {NAME = "I2S2EXT_SD", MODE = "_GPIO_MODE_AF_PP", AF = "6", STATE = "_LOW"}
    PC2[#PC2 + 1] = {NAME = "OTG_HS_ULPI_DIR", MODE = "_GPIO_MODE_AF_PP", AF = "10", STATE = "_LOW"}
    PC2[#PC2 + 1] = {NAME = "ETH_MII_TXD2", MODE = "_GPIO_MODE_AF_PP", AF = "11", STATE = "_LOW"}
    PC2[#PC2 + 1] = {NAME = "FMC_SDNE0", MODE = "_GPIO_MODE_AF_PP", AF = "12", STATE = "_LOW"}
    PC2[#PC2 + 1] = {NAME = "EVENTOUT", MODE = "_GPIO_MODE_AF_PP", AF = "15", STATE = "_LOW"}

    this:AddWidget("Textbox", "PC2")
    this:AddProposals(PC2, "C", 2)
end
--*/
#define __GPIO_PC_PIN_2_NAME__ PC2_NC

/*--
if this:PinExist("C", 2) then
    this:AddWidget("Combobox")
    this:AddPinSpeed("C", 2)
end
--*/
#define __GPIO_PC_PIN_2_SPEED__ _GPIO_SPEED_LOW

/*--
if this:PinExist("C", 2) then
    this:AddWidget("Combobox")
    this:AddPinMode("C", 2)
end
--*/
#define __GPIO_PC_PIN_2_MODE__ _GPIO_MODE_IN

/*--
if this:PinExist("C", 2) then
    this:AddWidget("Combobox")
    this:AddPinAF("C", 2)
end
--*/
#define __GPIO_PC_PIN_2_AF__ 0

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
    PC3[#PC3 + 1] = {NAME = "PC3_NC", MODE = "_GPIO_MODE_IN", AF = "0", STATE = "_LOW"}
    PC3[#PC3 + 1] = {NAME = "SPI2_MOSI", MODE = "_GPIO_MODE_AF_PP", AF = "5", STATE = "_LOW"}
    PC3[#PC3 + 1] = {NAME = "I2S2_SD", MODE = "_GPIO_MODE_AF_PP", AF = "5", STATE = "_LOW"}
    PC3[#PC3 + 1] = {NAME = "OTG_HS_ULPI_NXT", MODE = "_GPIO_MODE_AF_PP", AF = "10", STATE = "_LOW"}
    PC3[#PC3 + 1] = {NAME = "ETH_MII_TX_CLK", MODE = "_GPIO_MODE_AF_PP", AF = "11", STATE = "_LOW"}
    PC3[#PC3 + 1] = {NAME = "FMC_SDCKE0", MODE = "_GPIO_MODE_AF_PP", AF = "12", STATE = "_LOW"}
    PC3[#PC3 + 1] = {NAME = "EVENTOUT", MODE = "_GPIO_MODE_AF_PP", AF = "15", STATE = "_LOW"}

    this:AddWidget("Textbox", "PC3")
    this:AddProposals(PC3, "C", 3)
end
--*/
#define __GPIO_PC_PIN_3_NAME__ PC3_NC

/*--
if this:PinExist("C", 3) then
    this:AddWidget("Combobox")
    this:AddPinSpeed("C", 3)
end
--*/
#define __GPIO_PC_PIN_3_SPEED__ _GPIO_SPEED_LOW

/*--
if this:PinExist("C", 3) then
    this:AddWidget("Combobox")
    this:AddPinMode("C", 3)
end
--*/
#define __GPIO_PC_PIN_3_MODE__ _GPIO_MODE_IN

/*--
if this:PinExist("C", 3) then
    this:AddWidget("Combobox")
    this:AddPinAF("C", 3)
end
--*/
#define __GPIO_PC_PIN_3_AF__ 0

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
    PC4[#PC4 + 1] = {NAME = "PC4_NC", MODE = "_GPIO_MODE_IN", AF = "0", STATE = "_LOW"}
    PC4[#PC4 + 1] = {NAME = "ETH_MII_RXD0", MODE = "_GPIO_MODE_AF_PP", AF = "11", STATE = "_LOW"}
    PC4[#PC4 + 1] = {NAME = "ETH_RMII_RDXD0", MODE = "_GPIO_MODE_AF_PP", AF = "11", STATE = "_LOW"}
    PC4[#PC4 + 1] = {NAME = "FMC_SDNE0", MODE = "_GPIO_MODE_AF_PP", AF = "12", STATE = "_LOW"}
    PC4[#PC4 + 1] = {NAME = "EVENTOUT", MODE = "_GPIO_MODE_AF_PP", AF = "15", STATE = "_LOW"}

    this:AddWidget("Textbox", "PC4")
    this:AddProposals(PC4, "C", 4)
end
--*/
#define __GPIO_PC_PIN_4_NAME__ PC4_NC

/*--
if this:PinExist("C", 4) then
    this:AddWidget("Combobox")
    this:AddPinSpeed("C", 4)
end
--*/
#define __GPIO_PC_PIN_4_SPEED__ _GPIO_SPEED_LOW

/*--
if this:PinExist("C", 4) then
    this:AddWidget("Combobox")
    this:AddPinMode("C", 4)
end
--*/
#define __GPIO_PC_PIN_4_MODE__ _GPIO_MODE_IN

/*--
if this:PinExist("C", 4) then
    this:AddWidget("Combobox")
    this:AddPinAF("C", 4)
end
--*/
#define __GPIO_PC_PIN_4_AF__ 0

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
    PC5[#PC5 + 1] = {NAME = "PC5_NC", MODE = "_GPIO_MODE_IN", AF = "0", STATE = "_LOW"}
    PC5[#PC5 + 1] = {NAME = "ETH_MII_RXD1", MODE = "_GPIO_MODE_AF_PP", AF = "11", STATE = "_LOW"}
    PC5[#PC5 + 1] = {NAME = "ETH_RMII_RXD1", MODE = "_GPIO_MODE_AF_PP", AF = "11", STATE = "_LOW"}
    PC5[#PC5 + 1] = {NAME = "FMC_SDCKE0", MODE = "_GPIO_MODE_AF_PP", AF = "12", STATE = "_LOW"}
    PC5[#PC5 + 1] = {NAME = "EVENTOUT", MODE = "_GPIO_MODE_AF_PP", AF = "15", STATE = "_LOW"}

    this:AddWidget("Textbox", "PC5")
    this:AddProposals(PC5, "C", 5)
end
--*/
#define __GPIO_PC_PIN_5_NAME__ PC5_NC

/*--
if this:PinExist("C", 5) then
    this:AddWidget("Combobox")
    this:AddPinSpeed("C", 5)
end
--*/
#define __GPIO_PC_PIN_5_SPEED__ _GPIO_SPEED_LOW

/*--
if this:PinExist("C", 5) then
    this:AddWidget("Combobox")
    this:AddPinMode("C", 5)
end
--*/
#define __GPIO_PC_PIN_5_MODE__ _GPIO_MODE_IN

/*--
if this:PinExist("C", 5) then
    this:AddWidget("Combobox")
    this:AddPinAF("C", 5)
end
--*/
#define __GPIO_PC_PIN_5_AF__ 0

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
    PC6[#PC6 + 1] = {NAME = "PC6_NC", MODE = "_GPIO_MODE_IN", AF = "0", STATE = "_LOW"}
    PC6[#PC6 + 1] = {NAME = "TIM3_CH1", MODE = "_GPIO_MODE_AF_PP", AF = "2", STATE = "_LOW"}
    PC6[#PC6 + 1] = {NAME = "TIM8_CH1", MODE = "_GPIO_MODE_AF_PP", AF = "3", STATE = "_LOW"}
    PC6[#PC6 + 1] = {NAME = "I2S2_MCK", MODE = "_GPIO_MODE_AF_PP", AF = "5", STATE = "_LOW"}
    PC6[#PC6 + 1] = {NAME = "USART6_TX", MODE = "_GPIO_MODE_AF_PP", AF = "8", STATE = "_LOW"}
    PC6[#PC6 + 1] = {NAME = "SDIO_D6", MODE = "_GPIO_MODE_AF_PP", AF = "12", STATE = "_LOW"}
    PC6[#PC6 + 1] = {NAME = "DCMI_D0", MODE = "_GPIO_MODE_AF_PP", AF = "13", STATE = "_LOW"}
    PC6[#PC6 + 1] = {NAME = "LCD_HSYNC", MODE = "_GPIO_MODE_AF_PP", AF = "14", STATE = "_LOW"}
    PC6[#PC6 + 1] = {NAME = "EVENTOUT", MODE = "_GPIO_MODE_AF_PP", AF = "15", STATE = "_LOW"}

    this:AddWidget("Textbox", "PC6")
    this:AddProposals(PC6, "C", 6)
end
--*/
#define __GPIO_PC_PIN_6_NAME__ PC6_NC

/*--
if this:PinExist("C", 6) then
    this:AddWidget("Combobox")
    this:AddPinSpeed("C", 6)
end
--*/
#define __GPIO_PC_PIN_6_SPEED__ _GPIO_SPEED_LOW

/*--
if this:PinExist("C", 6) then
    this:AddWidget("Combobox")
    this:AddPinMode("C", 6)
end
--*/
#define __GPIO_PC_PIN_6_MODE__ _GPIO_MODE_IN

/*--
if this:PinExist("C", 6) then
    this:AddWidget("Combobox")
    this:AddPinAF("C", 6)
end
--*/
#define __GPIO_PC_PIN_6_AF__ 0

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
    PC7[#PC7 + 1] = {NAME = "PC7_NC", MODE = "_GPIO_MODE_IN", AF = "0", STATE = "_LOW"}
    PC7[#PC7 + 1] = {NAME = "TIM3_CH2", MODE = "_GPIO_MODE_AF_PP", AF = "2", STATE = "_LOW"}
    PC7[#PC7 + 1] = {NAME = "TIM8_CH2", MODE = "_GPIO_MODE_AF_PP", AF = "3", STATE = "_LOW"}
    PC7[#PC7 + 1] = {NAME = "I2S3_MCK", MODE = "_GPIO_MODE_AF_PP", AF = "6", STATE = "_LOW"}
    PC7[#PC7 + 1] = {NAME = "USART6_RX", MODE = "_GPIO_MODE_AF_PP_PU", AF = "8", STATE = "_LOW"}
    PC7[#PC7 + 1] = {NAME = "SDIO_D7", MODE = "_GPIO_MODE_AF_PP", AF = "12", STATE = "_LOW"}
    PC7[#PC7 + 1] = {NAME = "DCMI_D1", MODE = "_GPIO_MODE_AF_PP", AF = "13", STATE = "_LOW"}
    PC7[#PC7 + 1] = {NAME = "LCD_G6", MODE = "_GPIO_MODE_AF_PP", AF = "14", STATE = "_LOW"}
    PC7[#PC7 + 1] = {NAME = "EVENTOUT", MODE = "_GPIO_MODE_AF_PP", AF = "15", STATE = "_LOW"}

    this:AddWidget("Textbox", "PC7")
    this:AddProposals(PC7, "C", 7)
end
--*/
#define __GPIO_PC_PIN_7_NAME__ PC7_NC

/*--
if this:PinExist("C", 7) then
    this:AddWidget("Combobox")
    this:AddPinSpeed("C", 7)
end
--*/
#define __GPIO_PC_PIN_7_SPEED__ _GPIO_SPEED_LOW

/*--
if this:PinExist("C", 7) then
    this:AddWidget("Combobox")
    this:AddPinMode("C", 7)
end
--*/
#define __GPIO_PC_PIN_7_MODE__ _GPIO_MODE_IN

/*--
if this:PinExist("C", 7) then
    this:AddWidget("Combobox")
    this:AddPinAF("C", 7)
end
--*/
#define __GPIO_PC_PIN_7_AF__ 0

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
    PC8[#PC8 + 1] = {NAME = "PC8_NC", MODE = "_GPIO_MODE_IN", AF = "0", STATE = "_LOW"}
    PC8[#PC8 + 1] = {NAME = "TRACED1", MODE = "_GPIO_MODE_AF_PP", AF = "0", STATE = "_LOW"}
    PC8[#PC8 + 1] = {NAME = "TIM3_CH3", MODE = "_GPIO_MODE_AF_PP", AF = "2", STATE = "_LOW"}
    PC8[#PC8 + 1] = {NAME = "TIM8_CH3", MODE = "_GPIO_MODE_AF_PP", AF = "3", STATE = "_LOW"}
    PC8[#PC8 + 1] = {NAME = "USART6_CK", MODE = "_GPIO_MODE_AF_PP", AF = "8", STATE = "_LOW"}
    PC8[#PC8 + 1] = {NAME = "SDIO_D0", MODE = "_GPIO_MODE_AF_PP", AF = "12", STATE = "_LOW"}
    PC8[#PC8 + 1] = {NAME = "DCMI_D2", MODE = "_GPIO_MODE_AF_PP", AF = "13", STATE = "_LOW"}
    PC8[#PC8 + 1] = {NAME = "EVENTOUT", MODE = "_GPIO_MODE_AF_PP", AF = "15", STATE = "_LOW"}

    this:AddWidget("Textbox", "PC8")
    this:AddProposals(PC8, "C", 8)
end
--*/
#define __GPIO_PC_PIN_8_NAME__ PC8_NC

/*--
if this:PinExist("C", 8) then
    this:AddWidget("Combobox")
    this:AddPinSpeed("C", 8)
end
--*/
#define __GPIO_PC_PIN_8_SPEED__ _GPIO_SPEED_LOW

/*--
if this:PinExist("C", 8) then
    this:AddWidget("Combobox")
    this:AddPinMode("C", 8)
end
--*/
#define __GPIO_PC_PIN_8_MODE__ _GPIO_MODE_IN

/*--
if this:PinExist("C", 8) then
    this:AddWidget("Combobox")
    this:AddPinAF("C", 8)
end
--*/
#define __GPIO_PC_PIN_8_AF__ 0

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
    PC9[#PC9 + 1] = {NAME = "PC9_NC", MODE = "_GPIO_MODE_IN", AF = "0", STATE = "_LOW"}
    PC9[#PC9 + 1] = {NAME = "MCO2", MODE = "_GPIO_MODE_AF_PP", AF = "0", STATE = "_LOW"}
    PC9[#PC9 + 1] = {NAME = "TIM3_CH4", MODE = "_GPIO_MODE_AF_PP", AF = "2", STATE = "_LOW"}
    PC9[#PC9 + 1] = {NAME = "TIM8_CH4", MODE = "_GPIO_MODE_AF_PP", AF = "3", STATE = "_LOW"}
    PC9[#PC9 + 1] = {NAME = "I2C3_SDA", MODE = "_GPIO_MODE_AF_OD_PU", AF = "4", STATE = "_LOW"}
    PC9[#PC9 + 1] = {NAME = "I2S_CKIN", MODE = "_GPIO_MODE_AF_PP", AF = "5", STATE = "_LOW"}
    PC9[#PC9 + 1] = {NAME = "QSPI_BK1_IO0", MODE = "_GPIO_MODE_AF_PP", AF = "9", STATE = "_LOW"}
    PC9[#PC9 + 1] = {NAME = "SDIO_D1", MODE = "_GPIO_MODE_AF_PP", AF = "12", STATE = "_LOW"}
    PC9[#PC9 + 1] = {NAME = "DCMI_D3", MODE = "_GPIO_MODE_AF_PP", AF = "13", STATE = "_LOW"}
    PC9[#PC9 + 1] = {NAME = "EVENTOUT", MODE = "_GPIO_MODE_AF_PP", AF = "15", STATE = "_LOW"}

    this:AddWidget("Textbox", "PC9")
    this:AddProposals(PC9, "C", 9)
end
--*/
#define __GPIO_PC_PIN_9_NAME__ PC9_NC

/*--
if this:PinExist("C", 9) then
    this:AddWidget("Combobox")
    this:AddPinSpeed("C", 9)
end
--*/
#define __GPIO_PC_PIN_9_SPEED__ _GPIO_SPEED_LOW

/*--
if this:PinExist("C", 9) then
    this:AddWidget("Combobox")
    this:AddPinMode("C", 9)
end
--*/
#define __GPIO_PC_PIN_9_MODE__ _GPIO_MODE_IN

/*--
if this:PinExist("C", 9) then
    this:AddWidget("Combobox")
    this:AddPinAF("C", 9)
end
--*/
#define __GPIO_PC_PIN_9_AF__ 0

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
    PC10[#PC10 + 1] = {NAME = "PC10_NC", MODE = "_GPIO_MODE_IN", AF = "0", STATE = "_LOW"}
    PC10[#PC10 + 1] = {NAME = "SPI3_SCK", MODE = "_GPIO_MODE_AF_PP", AF = "6", STATE = "_LOW"}
    PC10[#PC10 + 1] = {NAME = "I2S3_CK", MODE = "_GPIO_MODE_AF_PP", AF = "6", STATE = "_LOW"}
    PC10[#PC10 + 1] = {NAME = "USART3_TX", MODE = "_GPIO_MODE_AF_PP", AF = "7", STATE = "_LOW"}
    PC10[#PC10 + 1] = {NAME = "UART4_TX", MODE = "_GPIO_MODE_AF_PP", AF = "8", STATE = "_LOW"}
    PC10[#PC10 + 1] = {NAME = "QSPI_BK1_IO1", MODE = "_GPIO_MODE_AF_PP", AF = "9", STATE = "_LOW"}
    PC10[#PC10 + 1] = {NAME = "SDIO_D2", MODE = "_GPIO_MODE_AF_PP", AF = "12", STATE = "_LOW"}
    PC10[#PC10 + 1] = {NAME = "DCMI_D8", MODE = "_GPIO_MODE_AF_PP", AF = "13", STATE = "_LOW"}
    PC10[#PC10 + 1] = {NAME = "LCD_R2", MODE = "_GPIO_MODE_AF_PP", AF = "14", STATE = "_LOW"}
    PC10[#PC10 + 1] = {NAME = "EVENTOUT", MODE = "_GPIO_MODE_AF_PP", AF = "15", STATE = "_LOW"}

    this:AddWidget("Textbox", "PC10")
    this:AddProposals(PC10, "C", 10)
end
--*/
#define __GPIO_PC_PIN_10_NAME__ PC10_NC

/*--
if this:PinExist("C", 10) then
    this:AddWidget("Combobox")
    this:AddPinSpeed("C", 10)
end
--*/
#define __GPIO_PC_PIN_10_SPEED__ _GPIO_SPEED_LOW

/*--
if this:PinExist("C", 10) then
    this:AddWidget("Combobox")
    this:AddPinMode("C", 10)
end
--*/
#define __GPIO_PC_PIN_10_MODE__ _GPIO_MODE_IN

/*--
if this:PinExist("C", 10) then
    this:AddWidget("Combobox")
    this:AddPinAF("C", 10)
end
--*/
#define __GPIO_PC_PIN_10_AF__ 0

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
    PC11[#PC11 + 1] = {NAME = "PC11_NC", MODE = "_GPIO_MODE_IN", AF = "0", STATE = "_LOW"}
    PC11[#PC11 + 1] = {NAME = "I2S3EXT_SD", MODE = "_GPIO_MODE_AF_PP", AF = "5", STATE = "_LOW"}
    PC11[#PC11 + 1] = {NAME = "SPI3_MISO", MODE = "_GPIO_MODE_AF_PP", AF = "6", STATE = "_LOW"}
    PC11[#PC11 + 1] = {NAME = "USART3_RX", MODE = "_GPIO_MODE_AF_PP_PU", AF = "7", STATE = "_LOW"}
    PC11[#PC11 + 1] = {NAME = "UART4_RX", MODE = "_GPIO_MODE_AF_PP_PU", AF = "8", STATE = "_LOW"}
    PC11[#PC11 + 1] = {NAME = "QSPI_BK2_NCS", MODE = "_GPIO_MODE_AF_PP", AF = "9", STATE = "_LOW"}
    PC11[#PC11 + 1] = {NAME = "SDIO_D3", MODE = "_GPIO_MODE_AF_PP", AF = "12", STATE = "_LOW"}
    PC11[#PC11 + 1] = {NAME = "DCMI_D4", MODE = "_GPIO_MODE_AF_PP", AF = "13", STATE = "_LOW"}
    PC11[#PC11 + 1] = {NAME = "EVENTOUT", MODE = "_GPIO_MODE_AF_PP", AF = "15", STATE = "_LOW"}

    this:AddWidget("Textbox", "PC11")
    this:AddProposals(PC11, "C", 11)
end
--*/
#define __GPIO_PC_PIN_11_NAME__ PC11_NC

/*--
if this:PinExist("C", 11) then
    this:AddWidget("Combobox")
    this:AddPinSpeed("C", 11)
end
--*/
#define __GPIO_PC_PIN_11_SPEED__ _GPIO_SPEED_LOW

/*--
if this:PinExist("C", 11) then
    this:AddWidget("Combobox")
    this:AddPinMode("C", 11)
end
--*/
#define __GPIO_PC_PIN_11_MODE__ _GPIO_MODE_IN

/*--
if this:PinExist("C", 11) then
    this:AddWidget("Combobox")
    this:AddPinAF("C", 11)
end
--*/
#define __GPIO_PC_PIN_11_AF__ 0

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
    PC12[#PC12 + 1] = {NAME = "PC12_NC", MODE = "_GPIO_MODE_IN", AF = "0", STATE = "_LOW"}
    PC12[#PC12 + 1] = {NAME = "TRACED3", MODE = "_GPIO_MODE_AF_PP", AF = "0", STATE = "_LOW"}
    PC12[#PC12 + 1] = {NAME = "SPI3_MOSI", MODE = "_GPIO_MODE_AF_PP", AF = "6", STATE = "_LOW"}
    PC12[#PC12 + 1] = {NAME = "I2S3_SD", MODE = "_GPIO_MODE_AF_PP", AF = "6", STATE = "_LOW"}
    PC12[#PC12 + 1] = {NAME = "USART3_CK", MODE = "_GPIO_MODE_AF_PP", AF = "7", STATE = "_LOW"}
    PC12[#PC12 + 1] = {NAME = "UART5_TX", MODE = "_GPIO_MODE_AF_PP", AF = "8", STATE = "_LOW"}
    PC12[#PC12 + 1] = {NAME = "SDIO_CK", MODE = "_GPIO_MODE_AF_PP", AF = "12", STATE = "_LOW"}
    PC12[#PC12 + 1] = {NAME = "DCMI_D9", MODE = "_GPIO_MODE_AF_PP", AF = "13", STATE = "_LOW"}
    PC12[#PC12 + 1] = {NAME = "EVENTOUT", MODE = "_GPIO_MODE_AF_PP", AF = "15", STATE = "_LOW"}

    this:AddWidget("Textbox", "PC12")
    this:AddProposals(PC12, "C", 12)
end
--*/
#define __GPIO_PC_PIN_12_NAME__ PC12_NC

/*--
if this:PinExist("C", 12) then
    this:AddWidget("Combobox")
    this:AddPinSpeed("C", 12)
end
--*/
#define __GPIO_PC_PIN_12_SPEED__ _GPIO_SPEED_LOW

/*--
if this:PinExist("C", 12) then
    this:AddWidget("Combobox")
    this:AddPinMode("C", 12)
end
--*/
#define __GPIO_PC_PIN_12_MODE__ _GPIO_MODE_IN

/*--
if this:PinExist("C", 12) then
    this:AddWidget("Combobox")
    this:AddPinAF("C", 12)
end
--*/
#define __GPIO_PC_PIN_12_AF__ 0

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
    PC13[#PC13 + 1] = {NAME = "PC13_NC", MODE = "_GPIO_MODE_IN", AF = "0", STATE = "_LOW"}
    PC13[#PC13 + 1] = {NAME = "EVENTOUT", MODE = "_GPIO_MODE_AF_PP", AF = "", STATE = "_LOW"}

    this:AddWidget("Textbox", "PC13")
    this:AddProposals(PC13, "C", 13)
end
--*/
#define __GPIO_PC_PIN_13_NAME__ PC13_NC

/*--
if this:PinExist("C", 13) then
    this:AddWidget("Combobox")
    this:AddPinSpeed("C", 13)
end
--*/
#define __GPIO_PC_PIN_13_SPEED__ _GPIO_SPEED_LOW

/*--
if this:PinExist("C", 13) then
    this:AddWidget("Combobox")
    this:AddPinMode("C", 13)
end
--*/
#define __GPIO_PC_PIN_13_MODE__ _GPIO_MODE_IN

/*--
if this:PinExist("C", 13) then
    this:AddWidget("Combobox")
    this:AddPinAF("C", 13)
end
--*/
#define __GPIO_PC_PIN_13_AF__ 0

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
    PC14[#PC14 + 1] = {NAME = "PC14_NC", MODE = "_GPIO_MODE_IN", AF = "0", STATE = "_LOW"}
    PC14[#PC14 + 1] = {NAME = "EVENTOUT", MODE = "_GPIO_MODE_AF_PP", AF = "15", STATE = "_LOW"}

    this:AddWidget("Textbox", "PC14")
    this:AddProposals(PC14, "C", 14)
end
--*/
#define __GPIO_PC_PIN_14_NAME__ PC14_NC

/*--
if this:PinExist("C", 14) then
    this:AddWidget("Combobox")
    this:AddPinSpeed("C", 14)
end
--*/
#define __GPIO_PC_PIN_14_SPEED__ _GPIO_SPEED_LOW

/*--
if this:PinExist("C", 14) then
    this:AddWidget("Combobox")
    this:AddPinMode("C", 14)
end
--*/
#define __GPIO_PC_PIN_14_MODE__ _GPIO_MODE_IN

/*--
if this:PinExist("C", 14) then
    this:AddWidget("Combobox")
    this:AddPinAF("C", 14)
end
--*/
#define __GPIO_PC_PIN_14_AF__ 0

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
    PC15[#PC15 + 1] = {NAME = "PC15_NC", MODE = "_GPIO_MODE_IN", AF = "0", STATE = "_LOW"}
    PC15[#PC15 + 1] = {NAME = "EVENTOUT", MODE = "_GPIO_MODE_AF_PP", AF = "15", STATE = "_LOW"}

    this:AddWidget("Textbox", "PC15")
    this:AddProposals(PC15, "C", 15)
end
--*/
#define __GPIO_PC_PIN_15_NAME__ PC15_NC

/*--
if this:PinExist("C", 15) then
    this:AddWidget("Combobox")
    this:AddPinSpeed("C", 15)
end
--*/
#define __GPIO_PC_PIN_15_SPEED__ _GPIO_SPEED_LOW

/*--
if this:PinExist("C", 15) then
    this:AddWidget("Combobox")
    this:AddPinMode("C", 15)
end
--*/
#define __GPIO_PC_PIN_15_MODE__ _GPIO_MODE_IN

/*--
if this:PinExist("C", 15) then
    this:AddWidget("Combobox")
    this:AddPinAF("C", 15)
end
--*/
#define __GPIO_PC_PIN_15_AF__ 0

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
    PD0[#PD0 + 1] = {NAME = "PD0_NC", MODE = "_GPIO_MODE_IN", AF = "0", STATE = "_LOW"}
    PD0[#PD0 + 1] = {NAME = "CAN1_RX", MODE = "_GPIO_MODE_AF_PP", AF = "9", STATE = "_LOW"}
    PD0[#PD0 + 1] = {NAME = "FMC_D2", MODE = "_GPIO_MODE_AF_PP", AF = "12", STATE = "_LOW"}
    PD0[#PD0 + 1] = {NAME = "EVENTOUT", MODE = "_GPIO_MODE_AF_PP", AF = "15", STATE = "_LOW"}

    this:AddWidget("Textbox", "PD0")
    this:AddProposals(PD0, "D", 0)
end
--*/
#define __GPIO_PD_PIN_0_NAME__ PD0_NC

/*--
if this:PinExist("D", 0) then
    this:AddWidget("Combobox")
    this:AddPinSpeed("D", 0)
end
--*/
#define __GPIO_PD_PIN_0_SPEED__ _GPIO_SPEED_LOW

/*--
if this:PinExist("D", 0) then
    this:AddWidget("Combobox")
    this:AddPinMode("D", 0)
end
--*/
#define __GPIO_PD_PIN_0_MODE__ _GPIO_MODE_IN

/*--
if this:PinExist("D", 0) then
    this:AddWidget("Combobox")
    this:AddPinAF("D", 0)
end
--*/
#define __GPIO_PD_PIN_0_AF__ 0

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
    PD1[#PD1 + 1] = {NAME = "PD1_NC", MODE = "_GPIO_MODE_IN", AF = "0", STATE = "_LOW"}
    PD1[#PD1 + 1] = {NAME = "CAN1_Tx", MODE = "_GPIO_MODE_AF_PP", AF = "9", STATE = "_LOW"}
    PD1[#PD1 + 1] = {NAME = "FMC_D3", MODE = "_GPIO_MODE_AF_PP", AF = "12", STATE = "_LOW"}
    PD1[#PD1 + 1] = {NAME = "EVENTOUT", MODE = "_GPIO_MODE_AF_PP", AF = "15", STATE = "_LOW"}

    this:AddWidget("Textbox", "PD1")
    this:AddProposals(PD1, "D", 1)
end
--*/
#define __GPIO_PD_PIN_1_NAME__ PD1_NC

/*--
if this:PinExist("D", 1) then
    this:AddWidget("Combobox")
    this:AddPinSpeed("D", 1)
end
--*/
#define __GPIO_PD_PIN_1_SPEED__ _GPIO_SPEED_LOW

/*--
if this:PinExist("D", 1) then
    this:AddWidget("Combobox")
    this:AddPinMode("D", 1)
end
--*/
#define __GPIO_PD_PIN_1_MODE__ _GPIO_MODE_IN

/*--
if this:PinExist("D", 1) then
    this:AddWidget("Combobox")
    this:AddPinAF("D", 1)
end
--*/
#define __GPIO_PD_PIN_1_AF__ 0

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
    PD2[#PD2 + 1] = {NAME = "PD2_NC", MODE = "_GPIO_MODE_IN", AF = "0", STATE = "_LOW"}
    PD2[#PD2 + 1] = {NAME = "TRACED2", MODE = "_GPIO_MODE_AF_PP", AF = "0", STATE = "_LOW"}
    PD2[#PD2 + 1] = {NAME = "TIM3_ETR", MODE = "_GPIO_MODE_AF_PP", AF = "2", STATE = "_LOW"}
    PD2[#PD2 + 1] = {NAME = "UART5_RX", MODE = "_GPIO_MODE_AF_PP_PU", AF = "8", STATE = "_LOW"}
    PD2[#PD2 + 1] = {NAME = "SDIO_CMD", MODE = "_GPIO_MODE_AF_PP", AF = "12", STATE = "_LOW"}
    PD2[#PD2 + 1] = {NAME = "DCMI_D11", MODE = "_GPIO_MODE_AF_PP", AF = "13", STATE = "_LOW"}
    PD2[#PD2 + 1] = {NAME = "EVENTOUT", MODE = "_GPIO_MODE_AF_PP", AF = "15", STATE = "_LOW"}

    this:AddWidget("Textbox", "PD2")
    this:AddProposals(PD2, "D", 2)
end
--*/
#define __GPIO_PD_PIN_2_NAME__ PD2_NC

/*--
if this:PinExist("D", 2) then
    this:AddWidget("Combobox")
    this:AddPinSpeed("D", 2)
end
--*/
#define __GPIO_PD_PIN_2_SPEED__ _GPIO_SPEED_LOW

/*--
if this:PinExist("D", 2) then
    this:AddWidget("Combobox")
    this:AddPinMode("D", 2)
end
--*/
#define __GPIO_PD_PIN_2_MODE__ _GPIO_MODE_IN

/*--
if this:PinExist("D", 2) then
    this:AddWidget("Combobox")
    this:AddPinAF("D", 2)
end
--*/
#define __GPIO_PD_PIN_2_AF__ 0

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
    PD3[#PD3 + 1] = {NAME = "PD3_NC", MODE = "_GPIO_MODE_IN", AF = "0", STATE = "_LOW"}
    PD3[#PD3 + 1] = {NAME = "SPI2_SCK", MODE = "_GPIO_MODE_AF_PP", AF = "5", STATE = "_LOW"}
    PD3[#PD3 + 1] = {NAME = "I2S2_CK", MODE = "_GPIO_MODE_AF_PP", AF = "5", STATE = "_LOW"}
    PD3[#PD3 + 1] = {NAME = "USART2_CTS", MODE = "_GPIO_MODE_AF_PP_PU", AF = "7", STATE = "_LOW"}
    PD3[#PD3 + 1] = {NAME = "FMC_CLK", MODE = "_GPIO_MODE_AF_PP", AF = "12", STATE = "_LOW"}
    PD3[#PD3 + 1] = {NAME = "DCMI_D5", MODE = "_GPIO_MODE_AF_PP", AF = "13", STATE = "_LOW"}
    PD3[#PD3 + 1] = {NAME = "LCD_G7", MODE = "_GPIO_MODE_AF_PP", AF = "14", STATE = "_LOW"}
    PD3[#PD3 + 1] = {NAME = "EVENTOUT", MODE = "_GPIO_MODE_AF_PP", AF = "15", STATE = "_LOW"}

    this:AddWidget("Textbox", "PD3")
    this:AddProposals(PD3, "D", 3)
end
--*/
#define __GPIO_PD_PIN_3_NAME__ PD3_NC

/*--
if this:PinExist("D", 3) then
    this:AddWidget("Combobox")
    this:AddPinSpeed("D", 3)
end
--*/
#define __GPIO_PD_PIN_3_SPEED__ _GPIO_SPEED_LOW

/*--
if this:PinExist("D", 3) then
    this:AddWidget("Combobox")
    this:AddPinMode("D", 3)
end
--*/
#define __GPIO_PD_PIN_3_MODE__ _GPIO_MODE_IN

/*--
if this:PinExist("D", 3) then
    this:AddWidget("Combobox")
    this:AddPinAF("D", 3)
end
--*/
#define __GPIO_PD_PIN_3_AF__ 0

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
    PD4[#PD4 + 1] = {NAME = "PD4_NC", MODE = "_GPIO_MODE_IN", AF = "0", STATE = "_LOW"}
    PD4[#PD4 + 1] = {NAME = "USART2_RTS", MODE = "_GPIO_MODE_AF_PP", AF = "7", STATE = "_LOW"}
    PD4[#PD4 + 1] = {NAME = "FMC_NOE", MODE = "_GPIO_MODE_AF_PP", AF = "12", STATE = "_LOW"}
    PD4[#PD4 + 1] = {NAME = "EVENTOUT", MODE = "_GPIO_MODE_AF_PP", AF = "15", STATE = "_LOW"}

    this:AddWidget("Textbox", "PD4")
    this:AddProposals(PD4, "D", 4)
end
--*/
#define __GPIO_PD_PIN_4_NAME__ PD4_NC

/*--
if this:PinExist("D", 4) then
    this:AddWidget("Combobox")
    this:AddPinSpeed("D", 4)
end
--*/
#define __GPIO_PD_PIN_4_SPEED__ _GPIO_SPEED_LOW

/*--
if this:PinExist("D", 4) then
    this:AddWidget("Combobox")
    this:AddPinMode("D", 4)
end
--*/
#define __GPIO_PD_PIN_4_MODE__ _GPIO_MODE_IN

/*--
if this:PinExist("D", 4) then
    this:AddWidget("Combobox")
    this:AddPinAF("D", 4)
end
--*/
#define __GPIO_PD_PIN_4_AF__ 0

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
    PD5[#PD5 + 1] = {NAME = "PD5_NC", MODE = "_GPIO_MODE_IN", AF = "0", STATE = "_LOW"}
    PD5[#PD5 + 1] = {NAME = "USART2_TX", MODE = "_GPIO_MODE_AF_PP", AF = "7", STATE = "_LOW"}
    PD5[#PD5 + 1] = {NAME = "FMC_NWE", MODE = "_GPIO_MODE_AF_PP", AF = "12", STATE = "_LOW"}
    PD5[#PD5 + 1] = {NAME = "EVENTOUT", MODE = "_GPIO_MODE_AF_PP", AF = "15", STATE = "_LOW"}

    this:AddWidget("Textbox", "PD5")
    this:AddProposals(PD5, "D", 5)
end
--*/
#define __GPIO_PD_PIN_5_NAME__ PD5_NC

/*--
if this:PinExist("D", 5) then
    this:AddWidget("Combobox")
    this:AddPinSpeed("D", 5)
end
--*/
#define __GPIO_PD_PIN_5_SPEED__ _GPIO_SPEED_LOW

/*--
if this:PinExist("D", 5) then
    this:AddWidget("Combobox")
    this:AddPinMode("D", 5)
end
--*/
#define __GPIO_PD_PIN_5_MODE__ _GPIO_MODE_IN

/*--
if this:PinExist("D", 5) then
    this:AddWidget("Combobox")
    this:AddPinAF("D", 5)
end
--*/
#define __GPIO_PD_PIN_5_AF__ 0

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
    PD6[#PD6 + 1] = {NAME = "PD6_NC", MODE = "_GPIO_MODE_IN", AF = "0", STATE = "_LOW"}
    PD6[#PD6 + 1] = {NAME = "SPI3_MOSI", MODE = "_GPIO_MODE_AF_PP", AF = "5", STATE = "_LOW"}
    PD6[#PD6 + 1] = {NAME = "I2S3_SD", MODE = "_GPIO_MODE_AF_PP", AF = "5", STATE = "_LOW"}
    PD6[#PD6 + 1] = {NAME = "SAI1_SD_A", MODE = "_GPIO_MODE_AF_PP", AF = "6", STATE = "_LOW"}
    PD6[#PD6 + 1] = {NAME = "USART2_RX", MODE = "_GPIO_MODE_AF_PP_PU", AF = "7", STATE = "_LOW"}
    PD6[#PD6 + 1] = {NAME = "FMC_NWAIT", MODE = "_GPIO_MODE_AF_PP", AF = "12", STATE = "_LOW"}
    PD6[#PD6 + 1] = {NAME = "DCMI_D10", MODE = "_GPIO_MODE_AF_PP", AF = "13", STATE = "_LOW"}
    PD6[#PD6 + 1] = {NAME = "LCD_B2", MODE = "_GPIO_MODE_AF_PP", AF = "14", STATE = "_LOW"}
    PD6[#PD6 + 1] = {NAME = "EVENTOUT", MODE = "_GPIO_MODE_AF_PP", AF = "15", STATE = "_LOW"}

    this:AddWidget("Textbox", "PD6")
    this:AddProposals(PD6, "D", 6)
end
--*/
#define __GPIO_PD_PIN_6_NAME__ PD6_NC

/*--
if this:PinExist("D", 6) then
    this:AddWidget("Combobox")
    this:AddPinSpeed("D", 6)
end
--*/
#define __GPIO_PD_PIN_6_SPEED__ _GPIO_SPEED_LOW

/*--
if this:PinExist("D", 6) then
    this:AddWidget("Combobox")
    this:AddPinMode("D", 6)
end
--*/
#define __GPIO_PD_PIN_6_MODE__ _GPIO_MODE_IN

/*--
if this:PinExist("D", 6) then
    this:AddWidget("Combobox")
    this:AddPinAF("D", 6)
end
--*/
#define __GPIO_PD_PIN_6_AF__ 0

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
    PD7[#PD7 + 1] = {NAME = "PD7_NC", MODE = "_GPIO_MODE_IN", AF = "0", STATE = "_LOW"}
    PD7[#PD7 + 1] = {NAME = "USART2_CK", MODE = "_GPIO_MODE_AF_PP", AF = "7", STATE = "_LOW"}
    PD7[#PD7 + 1] = {NAME = "FMC_NE1", MODE = "_GPIO_MODE_AF_PP", AF = "12", STATE = "_LOW"}
    PD7[#PD7 + 1] = {NAME = "FMC_NE2", MODE = "_GPIO_MODE_AF_PP", AF = "12", STATE = "_LOW"}
    PD7[#PD7 + 1] = {NAME = "EVENTOUT", MODE = "_GPIO_MODE_AF_PP", AF = "15", STATE = "_LOW"}

    this:AddWidget("Textbox", "PD7")
    this:AddProposals(PD7, "D", 7)
end
--*/
#define __GPIO_PD_PIN_7_NAME__ PD7_NC

/*--
if this:PinExist("D", 7) then
    this:AddWidget("Combobox")
    this:AddPinSpeed("D", 7)
end
--*/
#define __GPIO_PD_PIN_7_SPEED__ _GPIO_SPEED_LOW

/*--
if this:PinExist("D", 7) then
    this:AddWidget("Combobox")
    this:AddPinMode("D", 7)
end
--*/
#define __GPIO_PD_PIN_7_MODE__ _GPIO_MODE_IN

/*--
if this:PinExist("D", 7) then
    this:AddWidget("Combobox")
    this:AddPinAF("D", 7)
end
--*/
#define __GPIO_PD_PIN_7_AF__ 0

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
    PD8[#PD8 + 1] = {NAME = "PD8_NC", MODE = "_GPIO_MODE_IN", AF = "0", STATE = "_LOW"}
    PD8[#PD8 + 1] = {NAME = "USART3_TX", MODE = "_GPIO_MODE_AF_PP", AF = "7", STATE = "_LOW"}
    PD8[#PD8 + 1] = {NAME = "FMC_D13", MODE = "_GPIO_MODE_AF_PP", AF = "12", STATE = "_LOW"}
    PD8[#PD8 + 1] = {NAME = "EVENTOUT", MODE = "_GPIO_MODE_AF_PP", AF = "15", STATE = "_LOW"}

    this:AddWidget("Textbox", "PD8")
    this:AddProposals(PD8, "D", 8)
end
--*/
#define __GPIO_PD_PIN_8_NAME__ PD8_NC

/*--
if this:PinExist("D", 8) then
    this:AddWidget("Combobox")
    this:AddPinSpeed("D", 8)
end
--*/
#define __GPIO_PD_PIN_8_SPEED__ _GPIO_SPEED_LOW

/*--
if this:PinExist("D", 8) then
    this:AddWidget("Combobox")
    this:AddPinMode("D", 8)
end
--*/
#define __GPIO_PD_PIN_8_MODE__ _GPIO_MODE_IN

/*--
if this:PinExist("D", 8) then
    this:AddWidget("Combobox")
    this:AddPinAF("D", 8)
end
--*/
#define __GPIO_PD_PIN_8_AF__ 0

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
    PD9[#PD9 + 1] = {NAME = "PD9_NC", MODE = "_GPIO_MODE_IN", AF = "0", STATE = "_LOW"}
    PD9[#PD9 + 1] = {NAME = "USART3_RX", MODE = "_GPIO_MODE_AF_PP_PU", AF = "7", STATE = "_LOW"}
    PD9[#PD9 + 1] = {NAME = "FMC_D14", MODE = "_GPIO_MODE_AF_PP", AF = "12", STATE = "_LOW"}
    PD9[#PD9 + 1] = {NAME = "EVENTOUT", MODE = "_GPIO_MODE_AF_PP", AF = "15", STATE = "_LOW"}

    this:AddWidget("Textbox", "PD9")
    this:AddProposals(PD9, "D", 9)
end
--*/
#define __GPIO_PD_PIN_9_NAME__ PD9_NC

/*--
if this:PinExist("D", 9) then
    this:AddWidget("Combobox")
    this:AddPinSpeed("D", 9)
end
--*/
#define __GPIO_PD_PIN_9_SPEED__ _GPIO_SPEED_LOW

/*--
if this:PinExist("D", 9) then
    this:AddWidget("Combobox")
    this:AddPinMode("D", 9)
end
--*/
#define __GPIO_PD_PIN_9_MODE__ _GPIO_MODE_IN

/*--
if this:PinExist("D", 9) then
    this:AddWidget("Combobox")
    this:AddPinAF("D", 9)
end
--*/
#define __GPIO_PD_PIN_9_AF__ 0

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
    PD10[#PD10 + 1] = {NAME = "PD10_NC", MODE = "_GPIO_MODE_IN", AF = "0", STATE = "_LOW"}
    PD10[#PD10 + 1] = {NAME = "USART3_CK", MODE = "_GPIO_MODE_AF_PP", AF = "7", STATE = "_LOW"}
    PD10[#PD10 + 1] = {NAME = "FMC_D15", MODE = "_GPIO_MODE_AF_PP", AF = "12", STATE = "_LOW"}
    PD10[#PD10 + 1] = {NAME = "LCD_B3", MODE = "_GPIO_MODE_AF_PP", AF = "14", STATE = "_LOW"}
    PD10[#PD10 + 1] = {NAME = "EVENTOUT", MODE = "_GPIO_MODE_AF_PP", AF = "15", STATE = "_LOW"}

    this:AddWidget("Textbox", "PD10")
    this:AddProposals(PD10, "D", 10)
end
--*/
#define __GPIO_PD_PIN_10_NAME__ PD10_NC

/*--
if this:PinExist("D", 10) then
    this:AddWidget("Combobox")
    this:AddPinSpeed("D", 10)
end
--*/
#define __GPIO_PD_PIN_10_SPEED__ _GPIO_SPEED_LOW

/*--
if this:PinExist("D", 10) then
    this:AddWidget("Combobox")
    this:AddPinMode("D", 10)
end
--*/
#define __GPIO_PD_PIN_10_MODE__ _GPIO_MODE_IN

/*--
if this:PinExist("D", 10) then
    this:AddWidget("Combobox")
    this:AddPinAF("D", 10)
end
--*/
#define __GPIO_PD_PIN_10_AF__ 0

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
    PD11[#PD11 + 1] = {NAME = "PD11_NC", MODE = "_GPIO_MODE_IN", AF = "0", STATE = "_LOW"}
    PD11[#PD11 + 1] = {NAME = "USART3_CTS", MODE = "_GPIO_MODE_AF_PP_PU", AF = "7", STATE = "_LOW"}
    PD11[#PD11 + 1] = {NAME = "QUADSPI_BK1_IO0", MODE = "_GPIO_MODE_AF_PP", AF = "9", STATE = "_LOW"}
    PD11[#PD11 + 1] = {NAME = "FMC_A16", MODE = "_GPIO_MODE_AF_PP", AF = "12", STATE = "_LOW"}
    PD11[#PD11 + 1] = {NAME = "FMC_CLE", MODE = "_GPIO_MODE_AF_PP", AF = "12", STATE = "_LOW"}
    PD11[#PD11 + 1] = {NAME = "EVENTOUT", MODE = "_GPIO_MODE_AF_PP", AF = "15", STATE = "_LOW"}

    this:AddWidget("Textbox", "PD11")
    this:AddProposals(PD11, "D", 11)
end
--*/
#define __GPIO_PD_PIN_11_NAME__ PD11_NC

/*--
if this:PinExist("D", 11) then
    this:AddWidget("Combobox")
    this:AddPinSpeed("D", 11)
end
--*/
#define __GPIO_PD_PIN_11_SPEED__ _GPIO_SPEED_LOW

/*--
if this:PinExist("D", 11) then
    this:AddWidget("Combobox")
    this:AddPinMode("D", 11)
end
--*/
#define __GPIO_PD_PIN_11_MODE__ _GPIO_MODE_IN

/*--
if this:PinExist("D", 11) then
    this:AddWidget("Combobox")
    this:AddPinAF("D", 11)
end
--*/
#define __GPIO_PD_PIN_11_AF__ 0

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
    PD12[#PD12 + 1] = {NAME = "PD12_NC", MODE = "_GPIO_MODE_IN", AF = "0", STATE = "_LOW"}
    PD12[#PD12 + 1] = {NAME = "TIM4_CH1", MODE = "_GPIO_MODE_AF_PP", AF = "2", STATE = "_LOW"}
    PD12[#PD12 + 1] = {NAME = "USART3_RTS", MODE = "_GPIO_MODE_AF_PP", AF = "7", STATE = "_LOW"}
    PD12[#PD12 + 1] = {NAME = "QUADSPI_BK1_IO1", MODE = "_GPIO_MODE_AF_PP", AF = "9", STATE = "_LOW"}
    PD12[#PD12 + 1] = {NAME = "FMC_A17", MODE = "_GPIO_MODE_AF_PP", AF = "12", STATE = "_LOW"}
    PD12[#PD12 + 1] = {NAME = "FMC_ALE", MODE = "_GPIO_MODE_AF_PP", AF = "12", STATE = "_LOW"}
    PD12[#PD12 + 1] = {NAME = "EVENTOUT", MODE = "_GPIO_MODE_AF_PP", AF = "15", STATE = "_LOW"}

    this:AddWidget("Textbox", "PD12")
    this:AddProposals(PD12, "D", 12)
end
--*/
#define __GPIO_PD_PIN_12_NAME__ PD12_NC

/*--
if this:PinExist("D", 12) then
    this:AddWidget("Combobox")
    this:AddPinSpeed("D", 12)
end
--*/
#define __GPIO_PD_PIN_12_SPEED__ _GPIO_SPEED_LOW

/*--
if this:PinExist("D", 12) then
    this:AddWidget("Combobox")
    this:AddPinMode("D", 12)
end
--*/
#define __GPIO_PD_PIN_12_MODE__ _GPIO_MODE_IN

/*--
if this:PinExist("D", 12) then
    this:AddWidget("Combobox")
    this:AddPinAF("D", 12)
end
--*/
#define __GPIO_PD_PIN_12_AF__ 0

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
    PD13[#PD13 + 1] = {NAME = "PD13_NC", MODE = "_GPIO_MODE_IN", AF = "0", STATE = "_LOW"}
    PD13[#PD13 + 1] = {NAME = "TIM4_CH2", MODE = "_GPIO_MODE_AF_PP", AF = "2", STATE = "_LOW"}
    PD13[#PD13 + 1] = {NAME = "QUADSPI_BK1_IO3", MODE = "_GPIO_MODE_AF_PP", AF = "9", STATE = "_LOW"}
    PD13[#PD13 + 1] = {NAME = "FMC_A18", MODE = "_GPIO_MODE_AF_PP", AF = "12", STATE = "_LOW"}
    PD13[#PD13 + 1] = {NAME = "EVENTOUT", MODE = "_GPIO_MODE_AF_PP", AF = "15", STATE = "_LOW"}

    this:AddWidget("Textbox", "PD13")
    this:AddProposals(PD13, "D", 13)
end
--*/
#define __GPIO_PD_PIN_13_NAME__ PD13_NC

/*--
if this:PinExist("D", 13) then
    this:AddWidget("Combobox")
    this:AddPinSpeed("D", 13)
end
--*/
#define __GPIO_PD_PIN_13_SPEED__ _GPIO_SPEED_LOW

/*--
if this:PinExist("D", 13) then
    this:AddWidget("Combobox")
    this:AddPinMode("D", 13)
end
--*/
#define __GPIO_PD_PIN_13_MODE__ _GPIO_MODE_IN

/*--
if this:PinExist("D", 13) then
    this:AddWidget("Combobox")
    this:AddPinAF("D", 13)
end
--*/
#define __GPIO_PD_PIN_13_AF__ 0

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
    PD14[#PD14 + 1] = {NAME = "PD14_NC", MODE = "_GPIO_MODE_IN", AF = "0", STATE = "_LOW"}
    PD14[#PD14 + 1] = {NAME = "TIM4_CH3", MODE = "_GPIO_MODE_AF_PP", AF = "2", STATE = "_LOW"}
    PD14[#PD14 + 1] = {NAME = "FMC_D0", MODE = "_GPIO_MODE_AF_PP", AF = "12", STATE = "_LOW"}
    PD14[#PD14 + 1] = {NAME = "EVENTOUT", MODE = "_GPIO_MODE_AF_PP", AF = "15", STATE = "_LOW"}

    this:AddWidget("Textbox", "PD14")
    this:AddProposals(PD14, "D", 14)
end
--*/
#define __GPIO_PD_PIN_14_NAME__ PD14_NC

/*--
if this:PinExist("D", 14) then
    this:AddWidget("Combobox")
    this:AddPinSpeed("D", 14)
end
--*/
#define __GPIO_PD_PIN_14_SPEED__ _GPIO_SPEED_LOW

/*--
if this:PinExist("D", 14) then
    this:AddWidget("Combobox")
    this:AddPinMode("D", 14)
end
--*/
#define __GPIO_PD_PIN_14_MODE__ _GPIO_MODE_IN

/*--
if this:PinExist("D", 14) then
    this:AddWidget("Combobox")
    this:AddPinAF("D", 14)
end
--*/
#define __GPIO_PD_PIN_14_AF__ 0

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
    PD15[#PD15 + 1] = {NAME = "PD15_NC", MODE = "_GPIO_MODE_IN", AF = "0", STATE = "_LOW"}
    PD15[#PD15 + 1] = {NAME = "TIM4_CH4", MODE = "_GPIO_MODE_AF_PP", AF = "2", STATE = "_LOW"}
    PD15[#PD15 + 1] = {NAME = "FMC_D1", MODE = "_GPIO_MODE_AF_PP", AF = "12", STATE = "_LOW"}
    PD15[#PD15 + 1] = {NAME = "EVENTOUT", MODE = "_GPIO_MODE_AF_PP", AF = "15", STATE = "_LOW"}

    this:AddWidget("Textbox", "PD15")
    this:AddProposals(PD15, "D", 15)
end
--*/
#define __GPIO_PD_PIN_15_NAME__ PD15_NC

/*--
if this:PinExist("D", 15) then
    this:AddWidget("Combobox")
    this:AddPinSpeed("D", 15)
end
--*/
#define __GPIO_PD_PIN_15_SPEED__ _GPIO_SPEED_LOW

/*--
if this:PinExist("D", 15) then
    this:AddWidget("Combobox")
    this:AddPinMode("D", 15)
end
--*/
#define __GPIO_PD_PIN_15_MODE__ _GPIO_MODE_IN

/*--
if this:PinExist("D", 15) then
    this:AddWidget("Combobox")
    this:AddPinAF("D", 15)
end
--*/
#define __GPIO_PD_PIN_15_AF__ 0

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
    PE0[#PE0 + 1] = {NAME = "PE0_NC", MODE = "_GPIO_MODE_IN", AF = "0", STATE = "_LOW"}
    PE0[#PE0 + 1] = {NAME = "TIM4_ETR", MODE = "_GPIO_MODE_AF_PP", AF = "2", STATE = "_LOW"}
    PE0[#PE0 + 1] = {NAME = "UART8_RX", MODE = "_GPIO_MODE_AF_PP_PU", AF = "8", STATE = "_LOW"}
    PE0[#PE0 + 1] = {NAME = "FMC_NBL0", MODE = "_GPIO_MODE_AF_PP", AF = "12", STATE = "_LOW"}
    PE0[#PE0 + 1] = {NAME = "DCMI_D2", MODE = "_GPIO_MODE_AF_PP", AF = "13", STATE = "_LOW"}
    PE0[#PE0 + 1] = {NAME = "EVENTOUT", MODE = "_GPIO_MODE_AF_PP", AF = "15", STATE = "_LOW"}

    this:AddWidget("Textbox", "PE0")
    this:AddProposals(PE0, "E", 0)
end
--*/
#define __GPIO_PE_PIN_0_NAME__ PE0_NC

/*--
if this:PinExist("E", 0) then
    this:AddWidget("Combobox")
    this:AddPinSpeed("E", 0)
end
--*/
#define __GPIO_PE_PIN_0_SPEED__ _GPIO_SPEED_LOW

/*--
if this:PinExist("E", 0) then
    this:AddWidget("Combobox")
    this:AddPinMode("E", 0)
end
--*/
#define __GPIO_PE_PIN_0_MODE__ _GPIO_MODE_IN

/*--
if this:PinExist("E", 0) then
    this:AddWidget("Combobox")
    this:AddPinAF("E", 0)
end
--*/
#define __GPIO_PE_PIN_0_AF__ 0

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
    PE1[#PE1 + 1] = {NAME = "PE1_NC", MODE = "_GPIO_MODE_IN", AF = "0", STATE = "_LOW"}
    PE1[#PE1 + 1] = {NAME = "UART8_TX", MODE = "_GPIO_MODE_AF_PP", AF = "8", STATE = "_LOW"}
    PE1[#PE1 + 1] = {NAME = "FMC_NBL1", MODE = "_GPIO_MODE_AF_PP", AF = "12", STATE = "_LOW"}
    PE1[#PE1 + 1] = {NAME = "DCMI_D3", MODE = "_GPIO_MODE_AF_PP", AF = "13", STATE = "_LOW"}
    PE1[#PE1 + 1] = {NAME = "EVENTOUT", MODE = "_GPIO_MODE_AF_PP", AF = "15", STATE = "_LOW"}

    this:AddWidget("Textbox", "PE1")
    this:AddProposals(PE1, "E", 1)
end
--*/
#define __GPIO_PE_PIN_1_NAME__ PE1_NC

/*--
if this:PinExist("E", 1) then
    this:AddWidget("Combobox")
    this:AddPinSpeed("E", 1)
end
--*/
#define __GPIO_PE_PIN_1_SPEED__ _GPIO_SPEED_LOW

/*--
if this:PinExist("E", 1) then
    this:AddWidget("Combobox")
    this:AddPinMode("E", 1)
end
--*/
#define __GPIO_PE_PIN_1_MODE__ _GPIO_MODE_IN

/*--
if this:PinExist("E", 1) then
    this:AddWidget("Combobox")
    this:AddPinAF("E", 1)
end
--*/
#define __GPIO_PE_PIN_1_AF__ 0

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
    PE2[#PE2 + 1] = {NAME = "PE2_NC", MODE = "_GPIO_MODE_IN", AF = "0", STATE = "_LOW"}
    PE2[#PE2 + 1] = {NAME = "TRACECLK", MODE = "_GPIO_MODE_AF_PP", AF = "0", STATE = "_LOW"}
    PE2[#PE2 + 1] = {NAME = "SPI4_SCK", MODE = "_GPIO_MODE_AF_PP", AF = "5", STATE = "_LOW"}
    PE2[#PE2 + 1] = {NAME = "SA1_MCLK_A", MODE = "_GPIO_MODE_AF_PP", AF = "6", STATE = "_LOW"}
    PE2[#PE2 + 1] = {NAME = "QUADSPI_BK1_IO2", MODE = "_GPIO_MODE_AF_PP", AF = "9", STATE = "_LOW"}
    PE2[#PE2 + 1] = {NAME = "ETH_MII_TXD3", MODE = "_GPIO_MODE_AF_PP", AF = "11", STATE = "_LOW"}
    PE2[#PE2 + 1] = {NAME = "FMC_A23", MODE = "_GPIO_MODE_AF_PP", AF = "12", STATE = "_LOW"}
    PE2[#PE2 + 1] = {NAME = "EVENTOUT", MODE = "_GPIO_MODE_AF_PP", AF = "15", STATE = "_LOW"}

    this:AddWidget("Textbox", "PE2")
    this:AddProposals(PE2, "E", 2)
end
--*/
#define __GPIO_PE_PIN_2_NAME__ PE2_NC

/*--
if this:PinExist("E", 2) then
    this:AddWidget("Combobox")
    this:AddPinSpeed("E", 2)
end
--*/
#define __GPIO_PE_PIN_2_SPEED__ _GPIO_SPEED_LOW

/*--
if this:PinExist("E", 2) then
    this:AddWidget("Combobox")
    this:AddPinMode("E", 2)
end
--*/
#define __GPIO_PE_PIN_2_MODE__ _GPIO_MODE_IN

/*--
if this:PinExist("E", 2) then
    this:AddWidget("Combobox")
    this:AddPinAF("E", 2)
end
--*/
#define __GPIO_PE_PIN_2_AF__ 0

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
    PE3[#PE3 + 1] = {NAME = "PE3_NC", MODE = "_GPIO_MODE_IN", AF = "0", STATE = "_LOW"}
    PE3[#PE3 + 1] = {NAME = "TRACED0", MODE = "_GPIO_MODE_AF_PP", AF = "0", STATE = "_LOW"}
    PE3[#PE3 + 1] = {NAME = "SAI1_SD_B", MODE = "_GPIO_MODE_AF_PP", AF = "6", STATE = "_LOW"}
    PE3[#PE3 + 1] = {NAME = "FMC_A19", MODE = "_GPIO_MODE_AF_PP", AF = "12", STATE = "_LOW"}
    PE3[#PE3 + 1] = {NAME = "EVENTOUT", MODE = "_GPIO_MODE_AF_PP", AF = "15", STATE = "_LOW"}

    this:AddWidget("Textbox", "PE3")
    this:AddProposals(PE3, "E", 3)
end
--*/
#define __GPIO_PE_PIN_3_NAME__ PE3_NC

/*--
if this:PinExist("E", 3) then
    this:AddWidget("Combobox")
    this:AddPinSpeed("E", 3)
end
--*/
#define __GPIO_PE_PIN_3_SPEED__ _GPIO_SPEED_LOW

/*--
if this:PinExist("E", 3) then
    this:AddWidget("Combobox")
    this:AddPinMode("E", 3)
end
--*/
#define __GPIO_PE_PIN_3_MODE__ _GPIO_MODE_IN

/*--
if this:PinExist("E", 3) then
    this:AddWidget("Combobox")
    this:AddPinAF("E", 3)
end
--*/
#define __GPIO_PE_PIN_3_AF__ 0

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
    PE4[#PE4 + 1] = {NAME = "PE4_NC", MODE = "_GPIO_MODE_IN", AF = "0", STATE = "_LOW"}
    PE4[#PE4 + 1] = {NAME = "TRACED1", MODE = "_GPIO_MODE_AF_PP", AF = "0", STATE = "_LOW"}
    PE4[#PE4 + 1] = {NAME = "SPI4_NSS", MODE = "_GPIO_MODE_AF_PP", AF = "5", STATE = "_LOW"}
    PE4[#PE4 + 1] = {NAME = "SAI1_FS_A", MODE = "_GPIO_MODE_AF_PP", AF = "6", STATE = "_LOW"}
    PE4[#PE4 + 1] = {NAME = "FMC_A20", MODE = "_GPIO_MODE_AF_PP", AF = "12", STATE = "_LOW"}
    PE4[#PE4 + 1] = {NAME = "DCMI_D4", MODE = "_GPIO_MODE_AF_PP", AF = "13", STATE = "_LOW"}
    PE4[#PE4 + 1] = {NAME = "LCD_B0", MODE = "_GPIO_MODE_AF_PP", AF = "14", STATE = "_LOW"}
    PE4[#PE4 + 1] = {NAME = "EVENTOUT", MODE = "_GPIO_MODE_AF_PP", AF = "15", STATE = "_LOW"}

    this:AddWidget("Textbox", "PE4")
    this:AddProposals(PE4, "E", 4)
end
--*/
#define __GPIO_PE_PIN_4_NAME__ PE4_NC

/*--
if this:PinExist("E", 4) then
    this:AddWidget("Combobox")
    this:AddPinSpeed("E", 4)
end
--*/
#define __GPIO_PE_PIN_4_SPEED__ _GPIO_SPEED_LOW

/*--
if this:PinExist("E", 4) then
    this:AddWidget("Combobox")
    this:AddPinMode("E", 4)
end
--*/
#define __GPIO_PE_PIN_4_MODE__ _GPIO_MODE_IN

/*--
if this:PinExist("E", 4) then
    this:AddWidget("Combobox")
    this:AddPinAF("E", 4)
end
--*/
#define __GPIO_PE_PIN_4_AF__ 0

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
    PE5[#PE5 + 1] = {NAME = "PE5_NC", MODE = "_GPIO_MODE_IN", AF = "0", STATE = "_LOW"}
    PE5[#PE5 + 1] = {NAME = "TRACED2", MODE = "_GPIO_MODE_AF_PP", AF = "0", STATE = "_LOW"}
    PE5[#PE5 + 1] = {NAME = "TIM9_CH1", MODE = "_GPIO_MODE_AF_PP", AF = "3", STATE = "_LOW"}
    PE5[#PE5 + 1] = {NAME = "SPI4_MISO", MODE = "_GPIO_MODE_AF_PP", AF = "5", STATE = "_LOW"}
    PE5[#PE5 + 1] = {NAME = "SAI1_SCK_A", MODE = "_GPIO_MODE_AF_PP", AF = "6", STATE = "_LOW"}
    PE5[#PE5 + 1] = {NAME = "FMC_A21", MODE = "_GPIO_MODE_AF_PP", AF = "12", STATE = "_LOW"}
    PE5[#PE5 + 1] = {NAME = "DCMI_D6", MODE = "_GPIO_MODE_AF_PP", AF = "13", STATE = "_LOW"}
    PE5[#PE5 + 1] = {NAME = "LCD_G0", MODE = "_GPIO_MODE_AF_PP", AF = "14", STATE = "_LOW"}
    PE5[#PE5 + 1] = {NAME = "EVENTOUT", MODE = "_GPIO_MODE_AF_PP", AF = "15", STATE = "_LOW"}

    this:AddWidget("Textbox", "PE5")
    this:AddProposals(PE5, "E", 5)
end
--*/
#define __GPIO_PE_PIN_5_NAME__ PE5_NC

/*--
if this:PinExist("E", 5) then
    this:AddWidget("Combobox")
    this:AddPinSpeed("E", 5)
end
--*/
#define __GPIO_PE_PIN_5_SPEED__ _GPIO_SPEED_LOW

/*--
if this:PinExist("E", 5) then
    this:AddWidget("Combobox")
    this:AddPinMode("E", 5)
end
--*/
#define __GPIO_PE_PIN_5_MODE__ _GPIO_MODE_IN

/*--
if this:PinExist("E", 5) then
    this:AddWidget("Combobox")
    this:AddPinAF("E", 5)
end
--*/
#define __GPIO_PE_PIN_5_AF__ 0

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
    PE6[#PE6 + 1] = {NAME = "PE6_NC", MODE = "_GPIO_MODE_IN", AF = "0", STATE = "_LOW"}
    PE6[#PE6 + 1] = {NAME = "TRACED3", MODE = "_GPIO_MODE_AF_PP", AF = "0", STATE = "_LOW"}
    PE6[#PE6 + 1] = {NAME = "TIM9_CH2", MODE = "_GPIO_MODE_AF_PP", AF = "3", STATE = "_LOW"}
    PE6[#PE6 + 1] = {NAME = "SPI4_MOSI", MODE = "_GPIO_MODE_AF_PP", AF = "5", STATE = "_LOW"}
    PE6[#PE6 + 1] = {NAME = "SAI1_SD_A", MODE = "_GPIO_MODE_AF_PP", AF = "6", STATE = "_LOW"}
    PE6[#PE6 + 1] = {NAME = "FMC_A22", MODE = "_GPIO_MODE_AF_PP", AF = "12", STATE = "_LOW"}
    PE6[#PE6 + 1] = {NAME = "DCMI_D7", MODE = "_GPIO_MODE_AF_PP", AF = "13", STATE = "_LOW"}
    PE6[#PE6 + 1] = {NAME = "LCD_G1", MODE = "_GPIO_MODE_AF_PP", AF = "14", STATE = "_LOW"}
    PE6[#PE6 + 1] = {NAME = "EVENTOUT", MODE = "_GPIO_MODE_AF_PP", AF = "15", STATE = "_LOW"}

    this:AddWidget("Textbox", "PE6")
    this:AddProposals(PE6, "E", 6)
end
--*/
#define __GPIO_PE_PIN_6_NAME__ PE6_NC

/*--
if this:PinExist("E", 6) then
    this:AddWidget("Combobox")
    this:AddPinSpeed("E", 6)
end
--*/
#define __GPIO_PE_PIN_6_SPEED__ _GPIO_SPEED_LOW

/*--
if this:PinExist("E", 6) then
    this:AddWidget("Combobox")
    this:AddPinMode("E", 6)
end
--*/
#define __GPIO_PE_PIN_6_MODE__ _GPIO_MODE_IN

/*--
if this:PinExist("E", 6) then
    this:AddWidget("Combobox")
    this:AddPinAF("E", 6)
end
--*/
#define __GPIO_PE_PIN_6_AF__ 0

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
    PE7[#PE7 + 1] = {NAME = "PE7_NC", MODE = "_GPIO_MODE_IN", AF = "0", STATE = "_LOW"}
    PE7[#PE7 + 1] = {NAME = "TIM1_ETR", MODE = "_GPIO_MODE_AF_PP", AF = "1", STATE = "_LOW"}
    PE7[#PE7 + 1] = {NAME = "UART7_RX", MODE = "_GPIO_MODE_AF_PP_PU", AF = "8", STATE = "_LOW"}
    PE7[#PE7 + 1] = {NAME = "QUADSPI_BK2_IO0", MODE = "_GPIO_MODE_AF_PP", AF = "10", STATE = "_LOW"}
    PE7[#PE7 + 1] = {NAME = "FMC_D4", MODE = "_GPIO_MODE_AF_PP", AF = "12", STATE = "_LOW"}
    PE7[#PE7 + 1] = {NAME = "EVENTOUT", MODE = "_GPIO_MODE_AF_PP", AF = "15", STATE = "_LOW"}

    this:AddWidget("Textbox", "PE7")
    this:AddProposals(PE7, "E", 7)
end
--*/
#define __GPIO_PE_PIN_7_NAME__ PE7_NC

/*--
if this:PinExist("E", 7) then
    this:AddWidget("Combobox")
    this:AddPinSpeed("E", 7)
end
--*/
#define __GPIO_PE_PIN_7_SPEED__ _GPIO_SPEED_LOW

/*--
if this:PinExist("E", 7) then
    this:AddWidget("Combobox")
    this:AddPinMode("E", 7)
end
--*/
#define __GPIO_PE_PIN_7_MODE__ _GPIO_MODE_IN

/*--
if this:PinExist("E", 7) then
    this:AddWidget("Combobox")
    this:AddPinAF("E", 7)
end
--*/
#define __GPIO_PE_PIN_7_AF__ 0

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
    PE8[#PE8 + 1] = {NAME = "PE8_NC", MODE = "_GPIO_MODE_IN", AF = "0", STATE = "_LOW"}
    PE8[#PE8 + 1] = {NAME = "TIM1_CH1N", MODE = "_GPIO_MODE_AF_PP", AF = "1", STATE = "_LOW"}
    PE8[#PE8 + 1] = {NAME = "UART7_TX", MODE = "_GPIO_MODE_AF_PP", AF = "8", STATE = "_LOW"}
    PE8[#PE8 + 1] = {NAME = "QUADSPI_BK2_IO1", MODE = "_GPIO_MODE_AF_PP", AF = "10", STATE = "_LOW"}
    PE8[#PE8 + 1] = {NAME = "FMC_D5", MODE = "_GPIO_MODE_AF_PP", AF = "12", STATE = "_LOW"}
    PE8[#PE8 + 1] = {NAME = "EVENTOUT", MODE = "_GPIO_MODE_AF_PP", AF = "15", STATE = "_LOW"}

    this:AddWidget("Textbox", "PE8")
    this:AddProposals(PE8, "E", 8)
end
--*/
#define __GPIO_PE_PIN_8_NAME__ PE8_NC

/*--
if this:PinExist("E", 8) then
    this:AddWidget("Combobox")
    this:AddPinSpeed("E", 8)
end
--*/
#define __GPIO_PE_PIN_8_SPEED__ _GPIO_SPEED_LOW

/*--
if this:PinExist("E", 8) then
    this:AddWidget("Combobox")
    this:AddPinMode("E", 8)
end
--*/
#define __GPIO_PE_PIN_8_MODE__ _GPIO_MODE_IN

/*--
if this:PinExist("E", 8) then
    this:AddWidget("Combobox")
    this:AddPinAF("E", 8)
end
--*/
#define __GPIO_PE_PIN_8_AF__ 0

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
    PE9[#PE9 + 1] = {NAME = "PE9_NC", MODE = "_GPIO_MODE_IN", AF = "0", STATE = "_LOW"}
    PE9[#PE9 + 1] = {NAME = "TIM1_CH1", MODE = "_GPIO_MODE_AF_PP", AF = "1", STATE = "_LOW"}
    PE9[#PE9 + 1] = {NAME = "QUADSPI_BK2_IO2", MODE = "_GPIO_MODE_AF_PP", AF = "10", STATE = "_LOW"}
    PE9[#PE9 + 1] = {NAME = "FMC_D6", MODE = "_GPIO_MODE_AF_PP", AF = "12", STATE = "_LOW"}
    PE9[#PE9 + 1] = {NAME = "EVENTOUT", MODE = "_GPIO_MODE_AF_PP", AF = "15", STATE = "_LOW"}

    this:AddWidget("Textbox", "PE9")
    this:AddProposals(PE9, "E", 9)
end
--*/
#define __GPIO_PE_PIN_9_NAME__ PE9_NC

/*--
if this:PinExist("E", 9) then
    this:AddWidget("Combobox")
    this:AddPinSpeed("E", 9)
end
--*/
#define __GPIO_PE_PIN_9_SPEED__ _GPIO_SPEED_LOW

/*--
if this:PinExist("E", 9) then
    this:AddWidget("Combobox")
    this:AddPinMode("E", 9)
end
--*/
#define __GPIO_PE_PIN_9_MODE__ _GPIO_MODE_IN

/*--
if this:PinExist("E", 9) then
    this:AddWidget("Combobox")
    this:AddPinAF("E", 9)
end
--*/
#define __GPIO_PE_PIN_9_AF__ 0

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
    PE10[#PE10 + 1] = {NAME = "PE10_NC", MODE = "_GPIO_MODE_IN", AF = "0", STATE = "_LOW"}
    PE10[#PE10 + 1] = {NAME = "TIM1_CH2N", MODE = "_GPIO_MODE_AF_PP", AF = "1", STATE = "_LOW"}
    PE10[#PE10 + 1] = {NAME = "QUADSPI_BK2_IO3", MODE = "_GPIO_MODE_AF_PP", AF = "10", STATE = "_LOW"}
    PE10[#PE10 + 1] = {NAME = "FMC_D7", MODE = "_GPIO_MODE_AF_PP", AF = "12", STATE = "_LOW"}
    PE10[#PE10 + 1] = {NAME = "EVENTOUT", MODE = "_GPIO_MODE_AF_PP", AF = "15", STATE = "_LOW"}

    this:AddWidget("Textbox", "PE10")
    this:AddProposals(PE10, "E", 10)
end
--*/
#define __GPIO_PE_PIN_10_NAME__ PE10_NC

/*--
if this:PinExist("E", 10) then
    this:AddWidget("Combobox")
    this:AddPinSpeed("E", 10)
end
--*/
#define __GPIO_PE_PIN_10_SPEED__ _GPIO_SPEED_LOW

/*--
if this:PinExist("E", 10) then
    this:AddWidget("Combobox")
    this:AddPinMode("E", 10)
end
--*/
#define __GPIO_PE_PIN_10_MODE__ _GPIO_MODE_IN

/*--
if this:PinExist("E", 10) then
    this:AddWidget("Combobox")
    this:AddPinAF("E", 10)
end
--*/
#define __GPIO_PE_PIN_10_AF__ 0

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
    PE11[#PE11 + 1] = {NAME = "PE11_NC", MODE = "_GPIO_MODE_IN", AF = "0", STATE = "_LOW"}
    PE11[#PE11 + 1] = {NAME = "TIM1_CH2", MODE = "_GPIO_MODE_AF_PP", AF = "1", STATE = "_LOW"}
    PE11[#PE11 + 1] = {NAME = "SPI4_NSS", MODE = "_GPIO_MODE_AF_PP", AF = "5", STATE = "_LOW"}
    PE11[#PE11 + 1] = {NAME = "FMC_D8", MODE = "_GPIO_MODE_AF_PP", AF = "12", STATE = "_LOW"}
    PE11[#PE11 + 1] = {NAME = "LCD_G3", MODE = "_GPIO_MODE_AF_PP", AF = "14", STATE = "_LOW"}
    PE11[#PE11 + 1] = {NAME = "EVENTOUT", MODE = "_GPIO_MODE_AF_PP", AF = "15", STATE = "_LOW"}

    this:AddWidget("Textbox", "PE11")
    this:AddProposals(PE11, "E", 11)
end
--*/
#define __GPIO_PE_PIN_11_NAME__ PE11_NC

/*--
if this:PinExist("E", 11) then
    this:AddWidget("Combobox")
    this:AddPinSpeed("E", 11)
end
--*/
#define __GPIO_PE_PIN_11_SPEED__ _GPIO_SPEED_LOW

/*--
if this:PinExist("E", 11) then
    this:AddWidget("Combobox")
    this:AddPinMode("E", 11)
end
--*/
#define __GPIO_PE_PIN_11_MODE__ _GPIO_MODE_IN

/*--
if this:PinExist("E", 11) then
    this:AddWidget("Combobox")
    this:AddPinAF("E", 11)
end
--*/
#define __GPIO_PE_PIN_11_AF__ 0

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
    PE12[#PE12 + 1] = {NAME = "PE12_NC", MODE = "_GPIO_MODE_IN", AF = "0", STATE = "_LOW"}
    PE12[#PE12 + 1] = {NAME = "TIM1_CH3N", MODE = "_GPIO_MODE_AF_PP", AF = "1", STATE = "_LOW"}
    PE12[#PE12 + 1] = {NAME = "SPI4_SCK", MODE = "_GPIO_MODE_AF_PP", AF = "5", STATE = "_LOW"}
    PE12[#PE12 + 1] = {NAME = "FMC_D9", MODE = "_GPIO_MODE_AF_PP", AF = "12", STATE = "_LOW"}
    PE12[#PE12 + 1] = {NAME = "LCD_B4", MODE = "_GPIO_MODE_AF_PP", AF = "14", STATE = "_LOW"}
    PE12[#PE12 + 1] = {NAME = "EVENTOUT", MODE = "_GPIO_MODE_AF_PP", AF = "15", STATE = "_LOW"}

    this:AddWidget("Textbox", "PE12")
    this:AddProposals(PE12, "E", 12)
end
--*/
#define __GPIO_PE_PIN_12_NAME__ PE12_NC

/*--
if this:PinExist("E", 12) then
    this:AddWidget("Combobox")
    this:AddPinSpeed("E", 12)
end
--*/
#define __GPIO_PE_PIN_12_SPEED__ _GPIO_SPEED_LOW

/*--
if this:PinExist("E", 12) then
    this:AddWidget("Combobox")
    this:AddPinMode("E", 12)
end
--*/
#define __GPIO_PE_PIN_12_MODE__ _GPIO_MODE_IN

/*--
if this:PinExist("E", 12) then
    this:AddWidget("Combobox")
    this:AddPinAF("E", 12)
end
--*/
#define __GPIO_PE_PIN_12_AF__ 0

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
    PE13[#PE13 + 1] = {NAME = "PE13_NC", MODE = "_GPIO_MODE_IN", AF = "0", STATE = "_LOW"}
    PE13[#PE13 + 1] = {NAME = "TIM1_CH3", MODE = "_GPIO_MODE_AF_PP", AF = "1", STATE = "_LOW"}
    PE13[#PE13 + 1] = {NAME = "SPI4_MISO", MODE = "_GPIO_MODE_AF_PP", AF = "5", STATE = "_LOW"}
    PE13[#PE13 + 1] = {NAME = "FMC_D10", MODE = "_GPIO_MODE_AF_PP", AF = "12", STATE = "_LOW"}
    PE13[#PE13 + 1] = {NAME = "LCD_DE", MODE = "_GPIO_MODE_AF_PP", AF = "14", STATE = "_LOW"}
    PE13[#PE13 + 1] = {NAME = "EVENTOUT", MODE = "_GPIO_MODE_AF_PP", AF = "15", STATE = "_LOW"}

    this:AddWidget("Textbox", "PE13")
    this:AddProposals(PE13, "E", 13)
end
--*/
#define __GPIO_PE_PIN_13_NAME__ PE13_NC

/*--
if this:PinExist("E", 13) then
    this:AddWidget("Combobox")
    this:AddPinSpeed("E", 13)
end
--*/
#define __GPIO_PE_PIN_13_SPEED__ _GPIO_SPEED_LOW

/*--
if this:PinExist("E", 13) then
    this:AddWidget("Combobox")
    this:AddPinMode("E", 13)
end
--*/
#define __GPIO_PE_PIN_13_MODE__ _GPIO_MODE_IN

/*--
if this:PinExist("E", 13) then
    this:AddWidget("Combobox")
    this:AddPinAF("E", 13)
end
--*/
#define __GPIO_PE_PIN_13_AF__ 0

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
    PE14[#PE14 + 1] = {NAME = "PE14_NC", MODE = "_GPIO_MODE_IN", AF = "0", STATE = "_LOW"}
    PE14[#PE14 + 1] = {NAME = "TIM1_CH4", MODE = "_GPIO_MODE_AF_PP", AF = "1", STATE = "_LOW"}
    PE14[#PE14 + 1] = {NAME = "SPI4_MOSI", MODE = "_GPIO_MODE_AF_PP", AF = "5", STATE = "_LOW"}
    PE14[#PE14 + 1] = {NAME = "FMC_D11", MODE = "_GPIO_MODE_AF_PP", AF = "12", STATE = "_LOW"}
    PE14[#PE14 + 1] = {NAME = "LCD_CLK", MODE = "_GPIO_MODE_AF_PP", AF = "14", STATE = "_LOW"}
    PE14[#PE14 + 1] = {NAME = "EVENTOUT", MODE = "_GPIO_MODE_AF_PP", AF = "15", STATE = "_LOW"}

    this:AddWidget("Textbox", "PE14")
    this:AddProposals(PE14, "E", 14)
end
--*/
#define __GPIO_PE_PIN_14_NAME__ PE14_NC

/*--
if this:PinExist("E", 14) then
    this:AddWidget("Combobox")
    this:AddPinSpeed("E", 14)
end
--*/
#define __GPIO_PE_PIN_14_SPEED__ _GPIO_SPEED_LOW

/*--
if this:PinExist("E", 14) then
    this:AddWidget("Combobox")
    this:AddPinMode("E", 14)
end
--*/
#define __GPIO_PE_PIN_14_MODE__ _GPIO_MODE_IN

/*--
if this:PinExist("E", 14) then
    this:AddWidget("Combobox")
    this:AddPinAF("E", 14)
end
--*/
#define __GPIO_PE_PIN_14_AF__ 0

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
    PE15[#PE15 + 1] = {NAME = "PE15_NC", MODE = "_GPIO_MODE_IN", AF = "0", STATE = "_LOW"}
    PE15[#PE15 + 1] = {NAME = "TIM1_BKIN", MODE = "_GPIO_MODE_AF_PP", AF = "1", STATE = "_LOW"}
    PE15[#PE15 + 1] = {NAME = "FMC_D12", MODE = "_GPIO_MODE_AF_PP", AF = "12", STATE = "_LOW"}
    PE15[#PE15 + 1] = {NAME = "LCD_R7", MODE = "_GPIO_MODE_AF_PP", AF = "14", STATE = "_LOW"}
    PE15[#PE15 + 1] = {NAME = "EVENTOUT", MODE = "_GPIO_MODE_AF_PP", AF = "15", STATE = "_LOW"}

    this:AddWidget("Textbox", "PE15")
    this:AddProposals(PE15, "E", 15)
end
--*/
#define __GPIO_PE_PIN_15_NAME__ PE15_NC

/*--
if this:PinExist("E", 15) then
    this:AddWidget("Combobox")
    this:AddPinSpeed("E", 15)
end
--*/
#define __GPIO_PE_PIN_15_SPEED__ _GPIO_SPEED_LOW

/*--
if this:PinExist("E", 15) then
    this:AddWidget("Combobox")
    this:AddPinMode("E", 15)
end
--*/
#define __GPIO_PE_PIN_15_MODE__ _GPIO_MODE_IN

/*--
if this:PinExist("E", 15) then
    this:AddWidget("Combobox")
    this:AddPinAF("E", 15)
end
--*/
#define __GPIO_PE_PIN_15_AF__ 0

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
    PF0[#PF0 + 1] = {NAME = "PF0_NC", MODE = "_GPIO_MODE_IN", AF = "0", STATE = "_LOW"}
    PF0[#PF0 + 1] = {NAME = "I2C2_SDA", MODE = "_GPIO_MODE_AF_OD_PU", AF = "4", STATE = "_LOW"}
    PF0[#PF0 + 1] = {NAME = "FMC_A0", MODE = "_GPIO_MODE_AF_PP", AF = "12", STATE = "_LOW"}
    PF0[#PF0 + 1] = {NAME = "EVENTOUT", MODE = "_GPIO_MODE_AF_PP", AF = "15", STATE = "_LOW"}

    this:AddWidget("Textbox", "PF0")
    this:AddProposals(PF0, "F", 0)
end
--*/
#define __GPIO_PF_PIN_0_NAME__ PF0_NC

/*--
if this:PinExist("F", 0) then
    this:AddWidget("Combobox")
    this:AddPinSpeed("F", 0)
end
--*/
#define __GPIO_PF_PIN_0_SPEED__ _GPIO_SPEED_LOW

/*--
if this:PinExist("F", 0) then
    this:AddWidget("Combobox")
    this:AddPinMode("F", 0)
end
--*/
#define __GPIO_PF_PIN_0_MODE__ _GPIO_MODE_IN

/*--
if this:PinExist("F", 0) then
    this:AddWidget("Combobox")
    this:AddPinAF("F", 0)
end
--*/
#define __GPIO_PF_PIN_0_AF__ 0

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
    PF1[#PF1 + 1] = {NAME = "PF1_NC", MODE = "_GPIO_MODE_IN", AF = "0", STATE = "_LOW"}
    PF1[#PF1 + 1] = {NAME = "I2C2_SCL", MODE = "_GPIO_MODE_AF_OD_PU", AF = "4", STATE = "_LOW"}
    PF1[#PF1 + 1] = {NAME = "FMC_A1", MODE = "_GPIO_MODE_AF_PP", AF = "12", STATE = "_LOW"}
    PF1[#PF1 + 1] = {NAME = "EVENTOUT", MODE = "_GPIO_MODE_AF_PP", AF = "15", STATE = "_LOW"}

    this:AddWidget("Textbox", "PF1")
    this:AddProposals(PF1, "F", 1)
end
--*/
#define __GPIO_PF_PIN_1_NAME__ PF1_NC

/*--
if this:PinExist("F", 1) then
    this:AddWidget("Combobox")
    this:AddPinSpeed("F", 1)
end
--*/
#define __GPIO_PF_PIN_1_SPEED__ _GPIO_SPEED_LOW

/*--
if this:PinExist("F", 1) then
    this:AddWidget("Combobox")
    this:AddPinMode("F", 1)
end
--*/
#define __GPIO_PF_PIN_1_MODE__ _GPIO_MODE_IN

/*--
if this:PinExist("F", 1) then
    this:AddWidget("Combobox")
    this:AddPinAF("F", 1)
end
--*/
#define __GPIO_PF_PIN_1_AF__ 0

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
    PF2[#PF2 + 1] = {NAME = "PF2_NC", MODE = "_GPIO_MODE_IN", AF = "0", STATE = "_LOW"}
    PF2[#PF2 + 1] = {NAME = "I2C2_SMBA", MODE = "_GPIO_MODE_AF_OD_PU", AF = "4", STATE = "_LOW"}
    PF2[#PF2 + 1] = {NAME = "FMC_A2", MODE = "_GPIO_MODE_AF_PP", AF = "12", STATE = "_LOW"}
    PF2[#PF2 + 1] = {NAME = "EVENTOUT", MODE = "_GPIO_MODE_AF_PP", AF = "15", STATE = "_LOW"}

    this:AddWidget("Textbox", "PF2")
    this:AddProposals(PF2, "F", 2)
end
--*/
#define __GPIO_PF_PIN_2_NAME__ PF2_NC

/*--
if this:PinExist("F", 2) then
    this:AddWidget("Combobox")
    this:AddPinSpeed("F", 2)
end
--*/
#define __GPIO_PF_PIN_2_SPEED__ _GPIO_SPEED_LOW

/*--
if this:PinExist("F", 2) then
    this:AddWidget("Combobox")
    this:AddPinMode("F", 2)
end
--*/
#define __GPIO_PF_PIN_2_MODE__ _GPIO_MODE_IN

/*--
if this:PinExist("F", 2) then
    this:AddWidget("Combobox")
    this:AddPinAF("F", 2)
end
--*/
#define __GPIO_PF_PIN_2_AF__ 0

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
    PF3[#PF3 + 1] = {NAME = "PF3_NC", MODE = "_GPIO_MODE_IN", AF = "0", STATE = "_LOW"}
    PF3[#PF3 + 1] = {NAME = "FMC_A3", MODE = "_GPIO_MODE_AF_PP", AF = "12", STATE = "_LOW"}
    PF3[#PF3 + 1] = {NAME = "EVENTOUT", MODE = "_GPIO_MODE_AF_PP", AF = "15", STATE = "_LOW"}

    this:AddWidget("Textbox", "PF3")
    this:AddProposals(PF3, "F", 3)
end
--*/
#define __GPIO_PF_PIN_3_NAME__ PF3_NC

/*--
if this:PinExist("F", 3) then
    this:AddWidget("Combobox")
    this:AddPinSpeed("F", 3)
end
--*/
#define __GPIO_PF_PIN_3_SPEED__ _GPIO_SPEED_LOW

/*--
if this:PinExist("F", 3) then
    this:AddWidget("Combobox")
    this:AddPinMode("F", 3)
end
--*/
#define __GPIO_PF_PIN_3_MODE__ _GPIO_MODE_IN

/*--
if this:PinExist("F", 3) then
    this:AddWidget("Combobox")
    this:AddPinAF("F", 3)
end
--*/
#define __GPIO_PF_PIN_3_AF__ 0

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
    PF4[#PF4 + 1] = {NAME = "PF4_NC", MODE = "_GPIO_MODE_IN", AF = "0", STATE = "_LOW"}
    PF4[#PF4 + 1] = {NAME = "FMC_A4", MODE = "_GPIO_MODE_AF_PP", AF = "12", STATE = "_LOW"}
    PF4[#PF4 + 1] = {NAME = "EVENTOUT", MODE = "_GPIO_MODE_AF_PP", AF = "15", STATE = "_LOW"}

    this:AddWidget("Textbox", "PF4")
    this:AddProposals(PF4, "F", 4)
end
--*/
#define __GPIO_PF_PIN_4_NAME__ PF4_NC

/*--
if this:PinExist("F", 4) then
    this:AddWidget("Combobox")
    this:AddPinSpeed("F", 4)
end
--*/
#define __GPIO_PF_PIN_4_SPEED__ _GPIO_SPEED_LOW

/*--
if this:PinExist("F", 4) then
    this:AddWidget("Combobox")
    this:AddPinMode("F", 4)
end
--*/
#define __GPIO_PF_PIN_4_MODE__ _GPIO_MODE_IN

/*--
if this:PinExist("F", 4) then
    this:AddWidget("Combobox")
    this:AddPinAF("F", 4)
end
--*/
#define __GPIO_PF_PIN_4_AF__ 0

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
    PF5[#PF5 + 1] = {NAME = "PF5_NC", MODE = "_GPIO_MODE_IN", AF = "0", STATE = "_LOW"}
    PF5[#PF5 + 1] = {NAME = "FMC_A5", MODE = "_GPIO_MODE_AF_PP", AF = "12", STATE = "_LOW"}
    PF5[#PF5 + 1] = {NAME = "EVENTOUT", MODE = "_GPIO_MODE_AF_PP", AF = "15", STATE = "_LOW"}

    this:AddWidget("Textbox", "PF5")
    this:AddProposals(PF5, "F", 5)
end
--*/
#define __GPIO_PF_PIN_5_NAME__ PF5_NC

/*--
if this:PinExist("F", 5) then
    this:AddWidget("Combobox")
    this:AddPinSpeed("F", 5)
end
--*/
#define __GPIO_PF_PIN_5_SPEED__ _GPIO_SPEED_LOW

/*--
if this:PinExist("F", 5) then
    this:AddWidget("Combobox")
    this:AddPinMode("F", 5)
end
--*/
#define __GPIO_PF_PIN_5_MODE__ _GPIO_MODE_IN

/*--
if this:PinExist("F", 5) then
    this:AddWidget("Combobox")
    this:AddPinAF("F", 5)
end
--*/
#define __GPIO_PF_PIN_5_AF__ 0

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
    PF6[#PF6 + 1] = {NAME = "PF6_NC", MODE = "_GPIO_MODE_IN", AF = "0", STATE = "_LOW"}
    PF6[#PF6 + 1] = {NAME = "TIM10_CH1", MODE = "_GPIO_MODE_AF_PP", AF = "3", STATE = "_LOW"}
    PF6[#PF6 + 1] = {NAME = "SPI5_NSS", MODE = "_GPIO_MODE_AF_PP", AF = "5", STATE = "_LOW"}
    PF6[#PF6 + 1] = {NAME = "SAI1_SD_B", MODE = "_GPIO_MODE_AF_PP", AF = "6", STATE = "_LOW"}
    PF6[#PF6 + 1] = {NAME = "UART7_RX", MODE = "_GPIO_MODE_AF_PP_PU", AF = "8", STATE = "_LOW"}
    PF6[#PF6 + 1] = {NAME = "QUADSPI_BK1_IO3", MODE = "_GPIO_MODE_AF_PP", AF = "9", STATE = "_LOW"}
    PF6[#PF6 + 1] = {NAME = "FMC_NIORD", MODE = "_GPIO_MODE_AF_PP", AF = "12", STATE = "_LOW"}
    PF6[#PF6 + 1] = {NAME = "EVENTOUT", MODE = "_GPIO_MODE_AF_PP", AF = "15", STATE = "_LOW"}

    this:AddWidget("Textbox", "PF6")
    this:AddProposals(PF6, "F", 6)
end
--*/
#define __GPIO_PF_PIN_6_NAME__ PF6_NC

/*--
if this:PinExist("F", 6) then
    this:AddWidget("Combobox")
    this:AddPinSpeed("F", 6)
end
--*/
#define __GPIO_PF_PIN_6_SPEED__ _GPIO_SPEED_LOW

/*--
if this:PinExist("F", 6) then
    this:AddWidget("Combobox")
    this:AddPinMode("F", 6)
end
--*/
#define __GPIO_PF_PIN_6_MODE__ _GPIO_MODE_IN

/*--
if this:PinExist("F", 6) then
    this:AddWidget("Combobox")
    this:AddPinAF("F", 6)
end
--*/
#define __GPIO_PF_PIN_6_AF__ 0

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
    PF7[#PF7 + 1] = {NAME = "PF7_NC", MODE = "_GPIO_MODE_IN", AF = "0", STATE = "_LOW"}
    PF7[#PF7 + 1] = {NAME = "TIM11_CH1", MODE = "_GPIO_MODE_AF_PP", AF = "3", STATE = "_LOW"}
    PF7[#PF7 + 1] = {NAME = "SPI5_SCK", MODE = "_GPIO_MODE_AF_PP", AF = "5", STATE = "_LOW"}
    PF7[#PF7 + 1] = {NAME = "SAI1_MCLK_B", MODE = "_GPIO_MODE_AF_PP", AF = "6", STATE = "_LOW"}
    PF7[#PF7 + 1] = {NAME = "UART7_TX", MODE = "_GPIO_MODE_AF_PP", AF = "8", STATE = "_LOW"}
    PF7[#PF7 + 1] = {NAME = "QUADSPI_BK1_IO2", MODE = "_GPIO_MODE_AF_PP", AF = "9", STATE = "_LOW"}
    PF7[#PF7 + 1] = {NAME = "FMC_NREG", MODE = "_GPIO_MODE_AF_PP", AF = "12", STATE = "_LOW"}
    PF7[#PF7 + 1] = {NAME = "EVENTOUT", MODE = "_GPIO_MODE_AF_PP", AF = "15", STATE = "_LOW"}

    this:AddWidget("Textbox", "PF7")
    this:AddProposals(PF7, "F", 7)
end
--*/
#define __GPIO_PF_PIN_7_NAME__ PF7_NC

/*--
if this:PinExist("F", 7) then
    this:AddWidget("Combobox")
    this:AddPinSpeed("F", 7)
end
--*/
#define __GPIO_PF_PIN_7_SPEED__ _GPIO_SPEED_LOW

/*--
if this:PinExist("F", 7) then
    this:AddWidget("Combobox")
    this:AddPinMode("F", 7)
end
--*/
#define __GPIO_PF_PIN_7_MODE__ _GPIO_MODE_IN

/*--
if this:PinExist("F", 7) then
    this:AddWidget("Combobox")
    this:AddPinAF("F", 7)
end
--*/
#define __GPIO_PF_PIN_7_AF__ 0

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
    PF8[#PF8 + 1] = {NAME = "PF8_NC", MODE = "_GPIO_MODE_IN", AF = "0", STATE = "_LOW"}
    PF8[#PF8 + 1] = {NAME = "SPI5_MISO", MODE = "_GPIO_MODE_AF_PP", AF = "5", STATE = "_LOW"}
    PF8[#PF8 + 1] = {NAME = "SAI1_SCK_B", MODE = "_GPIO_MODE_AF_PP", AF = "6", STATE = "_LOW"}
    PF8[#PF8 + 1] = {NAME = "TIM13_CH1", MODE = "_GPIO_MODE_AF_PP", AF = "9", STATE = "_LOW"}
    PF8[#PF8 + 1] = {NAME = "QUADSPI_BK1_IO0", MODE = "_GPIO_MODE_AF_PP", AF = "10", STATE = "_LOW"}
    PF8[#PF8 + 1] = {NAME = "FMC_NIOWR", MODE = "_GPIO_MODE_AF_PP", AF = "12", STATE = "_LOW"}
    PF8[#PF8 + 1] = {NAME = "EVENTOUT", MODE = "_GPIO_MODE_AF_PP", AF = "15", STATE = "_LOW"}

    this:AddWidget("Textbox", "PF8")
    this:AddProposals(PF8, "F", 8)
end
--*/
#define __GPIO_PF_PIN_8_NAME__ PF8_NC

/*--
if this:PinExist("F", 8) then
    this:AddWidget("Combobox")
    this:AddPinSpeed("F", 8)
end
--*/
#define __GPIO_PF_PIN_8_SPEED__ _GPIO_SPEED_LOW

/*--
if this:PinExist("F", 8) then
    this:AddWidget("Combobox")
    this:AddPinMode("F", 8)
end
--*/
#define __GPIO_PF_PIN_8_MODE__ _GPIO_MODE_IN

/*--
if this:PinExist("F", 8) then
    this:AddWidget("Combobox")
    this:AddPinAF("F", 8)
end
--*/
#define __GPIO_PF_PIN_8_AF__ 0

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
    PF9[#PF9 + 1] = {NAME = "PF9_NC", MODE = "_GPIO_MODE_IN", AF = "0", STATE = "_LOW"}
    PF9[#PF9 + 1] = {NAME = "SPI5_MOSI", MODE = "_GPIO_MODE_AF_PP", AF = "5", STATE = "_LOW"}
    PF9[#PF9 + 1] = {NAME = "SAI1_FS_B", MODE = "_GPIO_MODE_AF_PP", AF = "6", STATE = "_LOW"}
    PF9[#PF9 + 1] = {NAME = "TIM14_CH1", MODE = "_GPIO_MODE_AF_PP", AF = "9", STATE = "_LOW"}
    PF9[#PF9 + 1] = {NAME = "QUADSPI_BK1_IO1", MODE = "_GPIO_MODE_AF_PP", AF = "10", STATE = "_LOW"}
    PF9[#PF9 + 1] = {NAME = "FMC_CD", MODE = "_GPIO_MODE_AF_PP", AF = "12", STATE = "_LOW"}
    PF9[#PF9 + 1] = {NAME = "EVENTOUT", MODE = "_GPIO_MODE_AF_PP", AF = "15", STATE = "_LOW"}

    this:AddWidget("Textbox", "PF9")
    this:AddProposals(PF9, "F", 9)
end
--*/
#define __GPIO_PF_PIN_9_NAME__ PF9_NC

/*--
if this:PinExist("F", 9) then
    this:AddWidget("Combobox")
    this:AddPinSpeed("F", 9)
end
--*/
#define __GPIO_PF_PIN_9_SPEED__ _GPIO_SPEED_LOW

/*--
if this:PinExist("F", 9) then
    this:AddWidget("Combobox")
    this:AddPinMode("F", 9)
end
--*/
#define __GPIO_PF_PIN_9_MODE__ _GPIO_MODE_IN

/*--
if this:PinExist("F", 9) then
    this:AddWidget("Combobox")
    this:AddPinAF("F", 9)
end
--*/
#define __GPIO_PF_PIN_9_AF__ 0

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
    PF10[#PF10 + 1] = {NAME = "PF10_NC", MODE = "_GPIO_MODE_IN", AF = "0", STATE = "_LOW"}
    PF10[#PF10 + 1] = {NAME = "QUADSPI_CLK", MODE = "_GPIO_MODE_AF_PP", AF = "9", STATE = "_LOW"}
    PF10[#PF10 + 1] = {NAME = "FMC_INTR", MODE = "_GPIO_MODE_AF_PP", AF = "12", STATE = "_LOW"}
    PF10[#PF10 + 1] = {NAME = "DCMI_D11", MODE = "_GPIO_MODE_AF_PP", AF = "13", STATE = "_LOW"}
    PF10[#PF10 + 1] = {NAME = "LCD_DE", MODE = "_GPIO_MODE_AF_PP", AF = "14", STATE = "_LOW"}
    PF10[#PF10 + 1] = {NAME = "EVENTOUT", MODE = "_GPIO_MODE_AF_PP", AF = "15", STATE = "_LOW"}

    this:AddWidget("Textbox", "PF10")
    this:AddProposals(PF10, "F", 10)
end
--*/
#define __GPIO_PF_PIN_10_NAME__ PF10_NC

/*--
if this:PinExist("F", 10) then
    this:AddWidget("Combobox")
    this:AddPinSpeed("F", 10)
end
--*/
#define __GPIO_PF_PIN_10_SPEED__ _GPIO_SPEED_LOW

/*--
if this:PinExist("F", 10) then
    this:AddWidget("Combobox")
    this:AddPinMode("F", 10)
end
--*/
#define __GPIO_PF_PIN_10_MODE__ _GPIO_MODE_IN

/*--
if this:PinExist("F", 10) then
    this:AddWidget("Combobox")
    this:AddPinAF("F", 10)
end
--*/
#define __GPIO_PF_PIN_10_AF__ 0

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
    PF11[#PF11 + 1] = {NAME = "PF11_NC", MODE = "_GPIO_MODE_IN", AF = "0", STATE = "_LOW"}
    PF11[#PF11 + 1] = {NAME = "SPI5_MOSI", MODE = "_GPIO_MODE_AF_PP", AF = "5", STATE = "_LOW"}
    PF11[#PF11 + 1] = {NAME = "FMC_SDNRAS", MODE = "_GPIO_MODE_AF_PP", AF = "12", STATE = "_LOW"}
    PF11[#PF11 + 1] = {NAME = "DCMI_D12", MODE = "_GPIO_MODE_AF_PP", AF = "13", STATE = "_LOW"}
    PF11[#PF11 + 1] = {NAME = "EVENTOUT", MODE = "_GPIO_MODE_AF_PP", AF = "15", STATE = "_LOW"}

    this:AddWidget("Textbox", "PF11")
    this:AddProposals(PF11, "F", 11)
end
--*/
#define __GPIO_PF_PIN_11_NAME__ PF11_NC

/*--
if this:PinExist("F", 11) then
    this:AddWidget("Combobox")
    this:AddPinSpeed("F", 11)
end
--*/
#define __GPIO_PF_PIN_11_SPEED__ _GPIO_SPEED_LOW

/*--
if this:PinExist("F", 11) then
    this:AddWidget("Combobox")
    this:AddPinMode("F", 11)
end
--*/
#define __GPIO_PF_PIN_11_MODE__ _GPIO_MODE_IN

/*--
if this:PinExist("F", 11) then
    this:AddWidget("Combobox")
    this:AddPinAF("F", 11)
end
--*/
#define __GPIO_PF_PIN_11_AF__ 0

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
    PF12[#PF12 + 1] = {NAME = "PF12_NC", MODE = "_GPIO_MODE_IN", AF = "0", STATE = "_LOW"}
    PF12[#PF12 + 1] = {NAME = "FMC_A6", MODE = "_GPIO_MODE_AF_PP", AF = "12", STATE = "_LOW"}
    PF12[#PF12 + 1] = {NAME = "EVENTOUT", MODE = "_GPIO_MODE_AF_PP", AF = "15", STATE = "_LOW"}

    this:AddWidget("Textbox", "PF12")
    this:AddProposals(PF12, "F", 12)
end
--*/
#define __GPIO_PF_PIN_12_NAME__ PF12_NC

/*--
if this:PinExist("F", 12) then
    this:AddWidget("Combobox")
    this:AddPinSpeed("F", 12)
end
--*/
#define __GPIO_PF_PIN_12_SPEED__ _GPIO_SPEED_LOW

/*--
if this:PinExist("F", 12) then
    this:AddWidget("Combobox")
    this:AddPinMode("F", 12)
end
--*/
#define __GPIO_PF_PIN_12_MODE__ _GPIO_MODE_IN

/*--
if this:PinExist("F", 12) then
    this:AddWidget("Combobox")
    this:AddPinAF("F", 12)
end
--*/
#define __GPIO_PF_PIN_12_AF__ 0

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
    PF13[#PF13 + 1] = {NAME = "PF13_NC", MODE = "_GPIO_MODE_IN", AF = "0", STATE = "_LOW"}
    PF13[#PF13 + 1] = {NAME = "FMC_A7", MODE = "_GPIO_MODE_AF_PP", AF = "12", STATE = "_LOW"}
    PF13[#PF13 + 1] = {NAME = "EVENTOUT", MODE = "_GPIO_MODE_AF_PP", AF = "15", STATE = "_LOW"}

    this:AddWidget("Textbox", "PF13")
    this:AddProposals(PF13, "F", 13)
end
--*/
#define __GPIO_PF_PIN_13_NAME__ PF13_NC

/*--
if this:PinExist("F", 13) then
    this:AddWidget("Combobox")
    this:AddPinSpeed("F", 13)
end
--*/
#define __GPIO_PF_PIN_13_SPEED__ _GPIO_SPEED_LOW

/*--
if this:PinExist("F", 13) then
    this:AddWidget("Combobox")
    this:AddPinMode("F", 13)
end
--*/
#define __GPIO_PF_PIN_13_MODE__ _GPIO_MODE_IN

/*--
if this:PinExist("F", 13) then
    this:AddWidget("Combobox")
    this:AddPinAF("F", 13)
end
--*/
#define __GPIO_PF_PIN_13_AF__ 0

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
    PF14[#PF14 + 1] = {NAME = "PF14_NC", MODE = "_GPIO_MODE_IN", AF = "0", STATE = "_LOW"}
    PF14[#PF14 + 1] = {NAME = "FMC_A8", MODE = "_GPIO_MODE_AF_PP", AF = "12", STATE = "_LOW"}
    PF14[#PF14 + 1] = {NAME = "EVENTOUT", MODE = "_GPIO_MODE_AF_PP", AF = "15", STATE = "_LOW"}

    this:AddWidget("Textbox", "PF14")
    this:AddProposals(PF14, "F", 14)
end
--*/
#define __GPIO_PF_PIN_14_NAME__ PF14_NC

/*--
if this:PinExist("F", 14) then
    this:AddWidget("Combobox")
    this:AddPinSpeed("F", 14)
end
--*/
#define __GPIO_PF_PIN_14_SPEED__ _GPIO_SPEED_LOW

/*--
if this:PinExist("F", 14) then
    this:AddWidget("Combobox")
    this:AddPinMode("F", 14)
end
--*/
#define __GPIO_PF_PIN_14_MODE__ _GPIO_MODE_IN

/*--
if this:PinExist("F", 14) then
    this:AddWidget("Combobox")
    this:AddPinAF("F", 14)
end
--*/
#define __GPIO_PF_PIN_14_AF__ 0

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
    PF15[#PF15 + 1] = {NAME = "PF15_NC", MODE = "_GPIO_MODE_IN", AF = "0", STATE = "_LOW"}
    PF15[#PF15 + 1] = {NAME = "FMC_A9", MODE = "_GPIO_MODE_AF_PP", AF = "12", STATE = "_LOW"}
    PF15[#PF15 + 1] = {NAME = "EVENTOUT", MODE = "_GPIO_MODE_AF_PP", AF = "15", STATE = "_LOW"}

    this:AddWidget("Textbox", "PF15")
    this:AddProposals(PF15, "F", 15)
end
--*/
#define __GPIO_PF_PIN_15_NAME__ PF15_NC

/*--
if this:PinExist("F", 15) then
    this:AddWidget("Combobox")
    this:AddPinSpeed("F", 15)
end
--*/
#define __GPIO_PF_PIN_15_SPEED__ _GPIO_SPEED_LOW

/*--
if this:PinExist("F", 15) then
    this:AddWidget("Combobox")
    this:AddPinMode("F", 15)
end
--*/
#define __GPIO_PF_PIN_15_MODE__ _GPIO_MODE_IN

/*--
if this:PinExist("F", 15) then
    this:AddWidget("Combobox")
    this:AddPinAF("F", 15)
end
--*/
#define __GPIO_PF_PIN_15_AF__ 0

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
    PG0[#PG0 + 1] = {NAME = "PG0_NC", MODE = "_GPIO_MODE_IN", AF = "0", STATE = "_LOW"}
    PG0[#PG0 + 1] = {NAME = "FMC_A10", MODE = "_GPIO_MODE_AF_PP", AF = "12", STATE = "_LOW"}
    PG0[#PG0 + 1] = {NAME = "EVENTOUT", MODE = "_GPIO_MODE_AF_PP", AF = "15", STATE = "_LOW"}

    this:AddWidget("Textbox", "PG0")
    this:AddProposals(PG0, "G", 0)
end
--*/
#define __GPIO_PG_PIN_0_NAME__ PG0_NC

/*--
if this:PinExist("G", 0) then
    this:AddWidget("Combobox")
    this:AddPinSpeed("G", 0)
end
--*/
#define __GPIO_PG_PIN_0_SPEED__ _GPIO_SPEED_LOW

/*--
if this:PinExist("G", 0) then
    this:AddWidget("Combobox")
    this:AddPinMode("G", 0)
end
--*/
#define __GPIO_PG_PIN_0_MODE__ _GPIO_MODE_IN

/*--
if this:PinExist("G", 0) then
    this:AddWidget("Combobox")
    this:AddPinAF("G", 0)
end
--*/
#define __GPIO_PG_PIN_0_AF__ 0

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
    PG1[#PG1 + 1] = {NAME = "PG1_NC", MODE = "_GPIO_MODE_IN", AF = "0", STATE = "_LOW"}
    PG1[#PG1 + 1] = {NAME = "FMC_A11", MODE = "_GPIO_MODE_AF_PP", AF = "12", STATE = "_LOW"}
    PG1[#PG1 + 1] = {NAME = "EVENTOUT", MODE = "_GPIO_MODE_AF_PP", AF = "15", STATE = "_LOW"}

    this:AddWidget("Textbox", "PG1")
    this:AddProposals(PG1, "G", 1)
end
--*/
#define __GPIO_PG_PIN_1_NAME__ PG1_NC

/*--
if this:PinExist("G", 1) then
    this:AddWidget("Combobox")
    this:AddPinSpeed("G", 1)
end
--*/
#define __GPIO_PG_PIN_1_SPEED__ _GPIO_SPEED_LOW

/*--
if this:PinExist("G", 1) then
    this:AddWidget("Combobox")
    this:AddPinMode("G", 1)
end
--*/
#define __GPIO_PG_PIN_1_MODE__ _GPIO_MODE_IN

/*--
if this:PinExist("G", 1) then
    this:AddWidget("Combobox")
    this:AddPinAF("G", 1)
end
--*/
#define __GPIO_PG_PIN_1_AF__ 0

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
    PG2[#PG2 + 1] = {NAME = "PG2_NC", MODE = "_GPIO_MODE_IN", AF = "0", STATE = "_LOW"}
    PG2[#PG2 + 1] = {NAME = "FMC_A12", MODE = "_GPIO_MODE_AF_PP", AF = "12", STATE = "_LOW"}
    PG2[#PG2 + 1] = {NAME = "EVENTOUT", MODE = "_GPIO_MODE_AF_PP", AF = "15", STATE = "_LOW"}

    this:AddWidget("Textbox", "PG2")
    this:AddProposals(PG2, "G", 2)
end
--*/
#define __GPIO_PG_PIN_2_NAME__ PG2_NC

/*--
if this:PinExist("G", 2) then
    this:AddWidget("Combobox")
    this:AddPinSpeed("G", 2)
end
--*/
#define __GPIO_PG_PIN_2_SPEED__ _GPIO_SPEED_LOW

/*--
if this:PinExist("G", 2) then
    this:AddWidget("Combobox")
    this:AddPinMode("G", 2)
end
--*/
#define __GPIO_PG_PIN_2_MODE__ _GPIO_MODE_IN

/*--
if this:PinExist("G", 2) then
    this:AddWidget("Combobox")
    this:AddPinAF("G", 2)
end
--*/
#define __GPIO_PG_PIN_2_AF__ 0

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
    PG3[#PG3 + 1] = {NAME = "PG3_NC", MODE = "_GPIO_MODE_IN", AF = "0", STATE = "_LOW"}
    PG3[#PG3 + 1] = {NAME = "FMC_A13", MODE = "_GPIO_MODE_AF_PP", AF = "12", STATE = "_LOW"}
    PG3[#PG3 + 1] = {NAME = "EVENTOUT", MODE = "_GPIO_MODE_AF_PP", AF = "15", STATE = "_LOW"}

    this:AddWidget("Textbox", "PG3")
    this:AddProposals(PG3, "G", 3)
end
--*/
#define __GPIO_PG_PIN_3_NAME__ PG3_NC

/*--
if this:PinExist("G", 3) then
    this:AddWidget("Combobox")
    this:AddPinSpeed("G", 3)
end
--*/
#define __GPIO_PG_PIN_3_SPEED__ _GPIO_SPEED_LOW

/*--
if this:PinExist("G", 3) then
    this:AddWidget("Combobox")
    this:AddPinMode("G", 3)
end
--*/
#define __GPIO_PG_PIN_3_MODE__ _GPIO_MODE_IN

/*--
if this:PinExist("G", 3) then
    this:AddWidget("Combobox")
    this:AddPinAF("G", 3)
end
--*/
#define __GPIO_PG_PIN_3_AF__ 0

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
    PG4[#PG4 + 1] = {NAME = "PG4_NC", MODE = "_GPIO_MODE_IN", AF = "0", STATE = "_LOW"}
    PG4[#PG4 + 1] = {NAME = "FMC_A14", MODE = "_GPIO_MODE_AF_PP", AF = "12", STATE = "_LOW"}
    PG4[#PG4 + 1] = {NAME = "FMC_BA0", MODE = "_GPIO_MODE_AF_PP", AF = "12", STATE = "_LOW"}
    PG4[#PG4 + 1] = {NAME = "EVENTOUT", MODE = "_GPIO_MODE_AF_PP", AF = "15", STATE = "_LOW"}

    this:AddWidget("Textbox", "PG4")
    this:AddProposals(PG4, "G", 4)
end
--*/
#define __GPIO_PG_PIN_4_NAME__ PG4_NC

/*--
if this:PinExist("G", 4) then
    this:AddWidget("Combobox")
    this:AddPinSpeed("G", 4)
end
--*/
#define __GPIO_PG_PIN_4_SPEED__ _GPIO_SPEED_LOW

/*--
if this:PinExist("G", 4) then
    this:AddWidget("Combobox")
    this:AddPinMode("G", 4)
end
--*/
#define __GPIO_PG_PIN_4_MODE__ _GPIO_MODE_IN

/*--
if this:PinExist("G", 4) then
    this:AddWidget("Combobox")
    this:AddPinAF("G", 4)
end
--*/
#define __GPIO_PG_PIN_4_AF__ 0

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
    PG5[#PG5 + 1] = {NAME = "PG5_NC", MODE = "_GPIO_MODE_IN", AF = "0", STATE = "_LOW"}
    PG5[#PG5 + 1] = {NAME = "FMC_A15", MODE = "_GPIO_MODE_AF_PP", AF = "12", STATE = "_LOW"}
    PG5[#PG5 + 1] = {NAME = "FMC_BA1", MODE = "_GPIO_MODE_AF_PP", AF = "12", STATE = "_LOW"}
    PG5[#PG5 + 1] = {NAME = "EVENTOUT", MODE = "_GPIO_MODE_AF_PP", AF = "15", STATE = "_LOW"}

    this:AddWidget("Textbox", "PG5")
    this:AddProposals(PG5, "G", 5)
end
--*/
#define __GPIO_PG_PIN_5_NAME__ PG5_NC

/*--
if this:PinExist("G", 5) then
    this:AddWidget("Combobox")
    this:AddPinSpeed("G", 5)
end
--*/
#define __GPIO_PG_PIN_5_SPEED__ _GPIO_SPEED_LOW

/*--
if this:PinExist("G", 5) then
    this:AddWidget("Combobox")
    this:AddPinMode("G", 5)
end
--*/
#define __GPIO_PG_PIN_5_MODE__ _GPIO_MODE_IN

/*--
if this:PinExist("G", 5) then
    this:AddWidget("Combobox")
    this:AddPinAF("G", 5)
end
--*/
#define __GPIO_PG_PIN_5_AF__ 0

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
    PG6[#PG6 + 1] = {NAME = "PG6_NC", MODE = "_GPIO_MODE_IN", AF = "0", STATE = "_LOW"}
    PG6[#PG6 + 1] = {NAME = "FMC_INT2", MODE = "_GPIO_MODE_AF_PP", AF = "12", STATE = "_LOW"}
    PG6[#PG6 + 1] = {NAME = "DCMI_D12", MODE = "_GPIO_MODE_AF_PP", AF = "13", STATE = "_LOW"}
    PG6[#PG6 + 1] = {NAME = "LCD_R7", MODE = "_GPIO_MODE_AF_PP", AF = "14", STATE = "_LOW"}
    PG6[#PG6 + 1] = {NAME = "EVENTOUT", MODE = "_GPIO_MODE_AF_PP", AF = "15", STATE = "_LOW"}

    this:AddWidget("Textbox", "PG6")
    this:AddProposals(PG6, "G", 6)
end
--*/
#define __GPIO_PG_PIN_6_NAME__ PG6_NC

/*--
if this:PinExist("G", 6) then
    this:AddWidget("Combobox")
    this:AddPinSpeed("G", 6)
end
--*/
#define __GPIO_PG_PIN_6_SPEED__ _GPIO_SPEED_LOW

/*--
if this:PinExist("G", 6) then
    this:AddWidget("Combobox")
    this:AddPinMode("G", 6)
end
--*/
#define __GPIO_PG_PIN_6_MODE__ _GPIO_MODE_IN

/*--
if this:PinExist("G", 6) then
    this:AddWidget("Combobox")
    this:AddPinAF("G", 6)
end
--*/
#define __GPIO_PG_PIN_6_AF__ 0

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
    PG7[#PG7 + 1] = {NAME = "PG7_NC", MODE = "_GPIO_MODE_IN", AF = "0", STATE = "_LOW"}
    PG7[#PG7 + 1] = {NAME = "SAI_1_MCLK_A", MODE = "_GPIO_MODE_AF_PP", AF = "6", STATE = "_LOW"}
    PG7[#PG7 + 1] = {NAME = "USART6_CK", MODE = "_GPIO_MODE_AF_PP", AF = "8", STATE = "_LOW"}
    PG7[#PG7 + 1] = {NAME = "FMC_INT", MODE = "_GPIO_MODE_AF_PP", AF = "12", STATE = "_LOW"}
    PG7[#PG7 + 1] = {NAME = "FMC_INT3", MODE = "_GPIO_MODE_AF_PP", AF = "12", STATE = "_LOW"}
    PG7[#PG7 + 1] = {NAME = "DCMI_D13", MODE = "_GPIO_MODE_AF_PP", AF = "13", STATE = "_LOW"}
    PG7[#PG7 + 1] = {NAME = "LCD_CLK", MODE = "_GPIO_MODE_AF_PP", AF = "15", STATE = "_LOW"}
    PG7[#PG7 + 1] = {NAME = "EVENTOUT", MODE = "_GPIO_MODE_AF_PP", AF = "15", STATE = "_LOW"}

    this:AddWidget("Textbox", "PG7")
    this:AddProposals(PG7, "G", 7)
end
--*/
#define __GPIO_PG_PIN_7_NAME__ PG7_NC

/*--
if this:PinExist("G", 7) then
    this:AddWidget("Combobox")
    this:AddPinSpeed("G", 7)
end
--*/
#define __GPIO_PG_PIN_7_SPEED__ _GPIO_SPEED_LOW

/*--
if this:PinExist("G", 7) then
    this:AddWidget("Combobox")
    this:AddPinMode("G", 7)
end
--*/
#define __GPIO_PG_PIN_7_MODE__ _GPIO_MODE_IN

/*--
if this:PinExist("G", 7) then
    this:AddWidget("Combobox")
    this:AddPinAF("G", 7)
end
--*/
#define __GPIO_PG_PIN_7_AF__ 0

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
    PG8[#PG8 + 1] = {NAME = "PG8_NC", MODE = "_GPIO_MODE_IN", AF = "0", STATE = "_LOW"}
    PG8[#PG8 + 1] = {NAME = "SPI6_NSS", MODE = "_GPIO_MODE_AF_PP", AF = "5", STATE = "_LOW"}
    PG8[#PG8 + 1] = {NAME = "USART6_RTS", MODE = "_GPIO_MODE_AF_PP", AF = "8", STATE = "_LOW"}
    PG8[#PG8 + 1] = {NAME = "ETH_PPS_OUT", MODE = "_GPIO_MODE_AF_PP", AF = "11", STATE = "_LOW"}
    PG8[#PG8 + 1] = {NAME = "FMC_SDCLK", MODE = "_GPIO_MODE_AF_PP", AF = "12", STATE = "_LOW"}
    PG8[#PG8 + 1] = {NAME = "LCD_G7", MODE = "_GPIO_MODE_AF_PP", AF = "14", STATE = "_LOW"}
    PG8[#PG8 + 1] = {NAME = "EVENTOUT", MODE = "_GPIO_MODE_AF_PP", AF = "15", STATE = "_LOW"}

    this:AddWidget("Textbox", "PG8")
    this:AddProposals(PG8, "G", 8)
end
--*/
#define __GPIO_PG_PIN_8_NAME__ PG8_NC

/*--
if this:PinExist("G", 8) then
    this:AddWidget("Combobox")
    this:AddPinSpeed("G", 8)
end
--*/
#define __GPIO_PG_PIN_8_SPEED__ _GPIO_SPEED_LOW

/*--
if this:PinExist("G", 8) then
    this:AddWidget("Combobox")
    this:AddPinMode("G", 8)
end
--*/
#define __GPIO_PG_PIN_8_MODE__ _GPIO_MODE_IN

/*--
if this:PinExist("G", 8) then
    this:AddWidget("Combobox")
    this:AddPinAF("G", 8)
end
--*/
#define __GPIO_PG_PIN_8_AF__ 0

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
    PG9[#PG9 + 1] = {NAME = "PG9_NC", MODE = "_GPIO_MODE_IN", AF = "0", STATE = "_LOW"}
    PG9[#PG9 + 1] = {NAME = "USART6_RX", MODE = "_GPIO_MODE_AF_PP_PU", AF = "8", STATE = "_LOW"}
    PG9[#PG9 + 1] = {NAME = "QSPI_BK2_IO2", MODE = "_GPIO_MODE_AF_PP", AF = "9", STATE = "_LOW"}
    PG9[#PG9 + 1] = {NAME = "FMC_NE2", MODE = "_GPIO_MODE_AF_PP", AF = "12", STATE = "_LOW"}
    PG9[#PG9 + 1] = {NAME = "FMC_NCE", MODE = "_GPIO_MODE_AF_PP", AF = "12", STATE = "_LOW"}
    PG9[#PG9 + 1] = {NAME = "FMC_NCE3", MODE = "_GPIO_MODE_AF_PP", AF = "12", STATE = "_LOW"}
    PG9[#PG9 + 1] = {NAME = "DCMI_VSYNC", MODE = "_GPIO_MODE_AF_PP", AF = "13", STATE = "_LOW"}
    PG9[#PG9 + 1] = {NAME = "EVENTOUT", MODE = "_GPIO_MODE_AF_PP", AF = "15", STATE = "_LOW"}

    this:AddWidget("Textbox", "PG9")
    this:AddProposals(PG9, "G", 9)
end
--*/
#define __GPIO_PG_PIN_9_NAME__ PG9_NC

/*--
if this:PinExist("G", 9) then
    this:AddWidget("Combobox")
    this:AddPinSpeed("G", 9)
end
--*/
#define __GPIO_PG_PIN_9_SPEED__ _GPIO_SPEED_LOW

/*--
if this:PinExist("G", 9) then
    this:AddWidget("Combobox")
    this:AddPinMode("G", 9)
end
--*/
#define __GPIO_PG_PIN_9_MODE__ _GPIO_MODE_IN

/*--
if this:PinExist("G", 9) then
    this:AddWidget("Combobox")
    this:AddPinAF("G", 9)
end
--*/
#define __GPIO_PG_PIN_9_AF__ 0

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
    PG10[#PG10 + 1] = {NAME = "PG10_NC", MODE = "_GPIO_MODE_IN", AF = "0", STATE = "_LOW"}
    PG10[#PG10 + 1] = {NAME = "LCD_G3", MODE = "_GPIO_MODE_AF_PP", AF = "9", STATE = "_LOW"}
    PG10[#PG10 + 1] = {NAME = "FMC_NE3", MODE = "_GPIO_MODE_AF_PP", AF = "12", STATE = "_LOW"}
    PG10[#PG10 + 1] = {NAME = "FMC_NE4_1", MODE = "_GPIO_MODE_AF_PP", AF = "12", STATE = "_LOW"}
    PG10[#PG10 + 1] = {NAME = "DCMI_D2", MODE = "_GPIO_MODE_AF_PP", AF = "13", STATE = "_LOW"}
    PG10[#PG10 + 1] = {NAME = "LCD_B2", MODE = "_GPIO_MODE_AF_PP", AF = "14", STATE = "_LOW"}
    PG10[#PG10 + 1] = {NAME = "EVENTOUT", MODE = "_GPIO_MODE_AF_PP", AF = "15", STATE = "_LOW"}

    this:AddWidget("Textbox", "PG10")
    this:AddProposals(PG10, "G", 10)
end
--*/
#define __GPIO_PG_PIN_10_NAME__ PG10_NC

/*--
if this:PinExist("G", 10) then
    this:AddWidget("Combobox")
    this:AddPinSpeed("G", 10)
end
--*/
#define __GPIO_PG_PIN_10_SPEED__ _GPIO_SPEED_LOW

/*--
if this:PinExist("G", 10) then
    this:AddWidget("Combobox")
    this:AddPinMode("G", 10)
end
--*/
#define __GPIO_PG_PIN_10_MODE__ _GPIO_MODE_IN

/*--
if this:PinExist("G", 10) then
    this:AddWidget("Combobox")
    this:AddPinAF("G", 10)
end
--*/
#define __GPIO_PG_PIN_10_AF__ 0

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
    PG11[#PG11 + 1] = {NAME = "PG11_NC", MODE = "_GPIO_MODE_IN", AF = "0", STATE = "_LOW"}
    PG11[#PG11 + 1] = {NAME = "ETH_MII_TX_EN", MODE = "_GPIO_MODE_AF_PP", AF = "11", STATE = "_LOW"}
    PG11[#PG11 + 1] = {NAME = "ETH_RMII_TX_EN", MODE = "_GPIO_MODE_AF_PP", AF = "11", STATE = "_LOW"}
    PG11[#PG11 + 1] = {NAME = "FMC_NCE4_2", MODE = "_GPIO_MODE_AF_PP", AF = "12", STATE = "_LOW"}
    PG11[#PG11 + 1] = {NAME = "DCMI_D3", MODE = "_GPIO_MODE_AF_PP", AF = "13", STATE = "_LOW"}
    PG11[#PG11 + 1] = {NAME = "LCD_B3", MODE = "_GPIO_MODE_AF_PP", AF = "14", STATE = "_LOW"}
    PG11[#PG11 + 1] = {NAME = "EVENTOUT", MODE = "_GPIO_MODE_AF_PP", AF = "15", STATE = "_LOW"}

    this:AddWidget("Textbox", "PG11")
    this:AddProposals(PG11, "G", 11)
end
--*/
#define __GPIO_PG_PIN_11_NAME__ PG11_NC

/*--
if this:PinExist("G", 11) then
    this:AddWidget("Combobox")
    this:AddPinSpeed("G", 11)
end
--*/
#define __GPIO_PG_PIN_11_SPEED__ _GPIO_SPEED_LOW

/*--
if this:PinExist("G", 11) then
    this:AddWidget("Combobox")
    this:AddPinMode("G", 11)
end
--*/
#define __GPIO_PG_PIN_11_MODE__ _GPIO_MODE_IN

/*--
if this:PinExist("G", 11) then
    this:AddWidget("Combobox")
    this:AddPinAF("G", 11)
end
--*/
#define __GPIO_PG_PIN_11_AF__ 0

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
    PG12[#PG12 + 1] = {NAME = "PG12_NC", MODE = "_GPIO_MODE_IN", AF = "0", STATE = "_LOW"}
    PG12[#PG12 + 1] = {NAME = "SPI6_MISO", MODE = "_GPIO_MODE_AF_PP", AF = "5", STATE = "_LOW"}
    PG12[#PG12 + 1] = {NAME = "USART6_RTS", MODE = "_GPIO_MODE_AF_PP", AF = "8", STATE = "_LOW"}
    PG12[#PG12 + 1] = {NAME = "LCD_B4", MODE = "_GPIO_MODE_AF_PP", AF = "9", STATE = "_LOW"}
    PG12[#PG12 + 1] = {NAME = "FMC_NE4", MODE = "_GPIO_MODE_AF_PP", AF = "12", STATE = "_LOW"}
    PG12[#PG12 + 1] = {NAME = "LCD_B1", MODE = "_GPIO_MODE_AF_PP", AF = "14", STATE = "_LOW"}
    PG12[#PG12 + 1] = {NAME = "EVENTOUT", MODE = "_GPIO_MODE_AF_PP", AF = "15", STATE = "_LOW"}

    this:AddWidget("Textbox", "PG12")
    this:AddProposals(PG12, "G", 12)
end
--*/
#define __GPIO_PG_PIN_12_NAME__ PG12_NC

/*--
if this:PinExist("G", 12) then
    this:AddWidget("Combobox")
    this:AddPinSpeed("G", 12)
end
--*/
#define __GPIO_PG_PIN_12_SPEED__ _GPIO_SPEED_LOW

/*--
if this:PinExist("G", 12) then
    this:AddWidget("Combobox")
    this:AddPinMode("G", 12)
end
--*/
#define __GPIO_PG_PIN_12_MODE__ _GPIO_MODE_IN

/*--
if this:PinExist("G", 12) then
    this:AddWidget("Combobox")
    this:AddPinAF("G", 12)
end
--*/
#define __GPIO_PG_PIN_12_AF__ 0

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
    PG13[#PG13 + 1] = {NAME = "PG13_NC", MODE = "_GPIO_MODE_IN", AF = "0", STATE = "_LOW"}
    PG13[#PG13 + 1] = {NAME = "TRACED0", MODE = "_GPIO_MODE_AF_PP", AF = "0", STATE = "_LOW"}
    PG13[#PG13 + 1] = {NAME = "SPI6_SCK", MODE = "_GPIO_MODE_AF_PP", AF = "5", STATE = "_LOW"}
    PG13[#PG13 + 1] = {NAME = "USART6_CTS", MODE = "_GPIO_MODE_AF_PP", AF = "8", STATE = "_LOW"}
    PG13[#PG13 + 1] = {NAME = "ETH_MII_TXD0", MODE = "_GPIO_MODE_AF_PP", AF = "11", STATE = "_LOW"}
    PG13[#PG13 + 1] = {NAME = "ETH_RMII_TXD0", MODE = "_GPIO_MODE_AF_PP", AF = "11", STATE = "_LOW"}
    PG13[#PG13 + 1] = {NAME = "FMC_A24", MODE = "_GPIO_MODE_AF_PP", AF = "12", STATE = "_LOW"}
    PG13[#PG13 + 1] = {NAME = "LCD_R0", MODE = "_GPIO_MODE_AF_PP", AF = "14", STATE = "_LOW"}
    PG13[#PG13 + 1] = {NAME = "EVENTOUT", MODE = "_GPIO_MODE_AF_PP", AF = "15", STATE = "_LOW"}

    this:AddWidget("Textbox", "PG13")
    this:AddProposals(PG13, "G", 13)
end
--*/
#define __GPIO_PG_PIN_13_NAME__ PG13_NC

/*--
if this:PinExist("G", 13) then
    this:AddWidget("Combobox")
    this:AddPinSpeed("G", 13)
end
--*/
#define __GPIO_PG_PIN_13_SPEED__ _GPIO_SPEED_LOW

/*--
if this:PinExist("G", 13) then
    this:AddWidget("Combobox")
    this:AddPinMode("G", 13)
end
--*/
#define __GPIO_PG_PIN_13_MODE__ _GPIO_MODE_IN

/*--
if this:PinExist("G", 13) then
    this:AddWidget("Combobox")
    this:AddPinAF("G", 13)
end
--*/
#define __GPIO_PG_PIN_13_AF__ 0

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
    PG14[#PG14 + 1] = {NAME = "PG14_NC", MODE = "_GPIO_MODE_IN", AF = "0", STATE = "_LOW"}
    PG14[#PG14 + 1] = {NAME = "TRACED1", MODE = "_GPIO_MODE_AF_PP", AF = "0", STATE = "_LOW"}
    PG14[#PG14 + 1] = {NAME = "SPI6_MOSI", MODE = "_GPIO_MODE_AF_PP", AF = "5", STATE = "_LOW"}
    PG14[#PG14 + 1] = {NAME = "USART6_TX", MODE = "_GPIO_MODE_AF_PP", AF = "8", STATE = "_LOW"}
    PG14[#PG14 + 1] = {NAME = "QSPI_BK2_IO3", MODE = "_GPIO_MODE_AF_PP", AF = "9", STATE = "_LOW"}
    PG14[#PG14 + 1] = {NAME = "ETH_MII_TXD1", MODE = "_GPIO_MODE_AF_PP", AF = "11", STATE = "_LOW"}
    PG14[#PG14 + 1] = {NAME = "ETH_RMII_TXD1", MODE = "_GPIO_MODE_AF_PP", AF = "11", STATE = "_LOW"}
    PG14[#PG14 + 1] = {NAME = "FMC_A25", MODE = "_GPIO_MODE_AF_PP", AF = "12", STATE = "_LOW"}
    PG14[#PG14 + 1] = {NAME = "LCD_B0", MODE = "_GPIO_MODE_AF_PP", AF = "14", STATE = "_LOW"}
    PG14[#PG14 + 1] = {NAME = "EVENTOUT", MODE = "_GPIO_MODE_AF_PP", AF = "15", STATE = "_LOW"}

    this:AddWidget("Textbox", "PG14")
    this:AddProposals(PG14, "G", 14)
end
--*/
#define __GPIO_PG_PIN_14_NAME__ PG14_NC

/*--
if this:PinExist("G", 14) then
    this:AddWidget("Combobox")
    this:AddPinSpeed("G", 14)
end
--*/
#define __GPIO_PG_PIN_14_SPEED__ _GPIO_SPEED_LOW

/*--
if this:PinExist("G", 14) then
    this:AddWidget("Combobox")
    this:AddPinMode("G", 14)
end
--*/
#define __GPIO_PG_PIN_14_MODE__ _GPIO_MODE_IN

/*--
if this:PinExist("G", 14) then
    this:AddWidget("Combobox")
    this:AddPinAF("G", 14)
end
--*/
#define __GPIO_PG_PIN_14_AF__ 0

/*--
if this:PinExist("G", 14) then
    this:AddWidget("Combobox")
    this:AddPinState("G", 14)
end
--*/
#define __GPIO_PG_PIN_14_STATE__ _HIGH

/*--
if this:PinExist("G", 15) then
    local PG15 = {}
    PG15[#PG15 + 1] = {NAME = "PG15_NC", MODE = "_GPIO_MODE_IN", AF = "0", STATE = "_LOW"}
    PG15[#PG15 + 1] = {NAME = "USART6_CTS", MODE = "_GPIO_MODE_AF_PP", AF = "8", STATE = "_LOW"}
    PG15[#PG15 + 1] = {NAME = "FMC_SDNCAS", MODE = "_GPIO_MODE_AF_PP", AF = "12", STATE = "_LOW"}
    PG15[#PG15 + 1] = {NAME = "DCMI_D13", MODE = "_GPIO_MODE_AF_PP", AF = "13", STATE = "_LOW"}
    PG15[#PG15 + 1] = {NAME = "EVENTOUT", MODE = "_GPIO_MODE_AF_PP", AF = "15", STATE = "_LOW"}

    this:AddWidget("Textbox", "PG15")
    this:AddProposals(PG15, "G", 15)
end
--*/
#define __GPIO_PG_PIN_15_NAME__ PG15_NC

/*--
if this:PinExist("G", 15) then
    this:AddWidget("Combobox")
    this:AddPinSpeed("G", 15)
end
--*/
#define __GPIO_PG_PIN_15_SPEED__ _GPIO_SPEED_LOW

/*--
if this:PinExist("G", 15) then
    this:AddWidget("Combobox")
    this:AddPinMode("G", 15)
end
--*/
#define __GPIO_PG_PIN_15_MODE__ _GPIO_MODE_IN

/*--
if this:PinExist("G", 15) then
    this:AddWidget("Combobox")
    this:AddPinAF("G", 15)
end
--*/
#define __GPIO_PG_PIN_15_AF__ 0

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
    PH0[#PH0 + 1] = {NAME = "PH0_NC", MODE = "_GPIO_MODE_IN", AF = "0", STATE = "_LOW"}
    PH0[#PH0 + 1] = {NAME = "EVENTOUT", MODE = "_GPIO_MODE_AF_PP", AF = "15", STATE = "_LOW"}

    this:AddWidget("Textbox", "PH0")
    this:AddProposals(PH0, "H", 0)
end
--*/
#define __GPIO_PH_PIN_0_NAME__ PH0_NC

/*--
if this:PinExist("H", 0) then
    this:AddWidget("Combobox")
    this:AddPinSpeed("H", 0)
end
--*/
#define __GPIO_PH_PIN_0_SPEED__ _GPIO_SPEED_LOW

/*--
if this:PinExist("H", 0) then
    this:AddWidget("Combobox")
    this:AddPinMode("H", 0)
end
--*/
#define __GPIO_PH_PIN_0_MODE__ _GPIO_MODE_IN

/*--
if this:PinExist("H", 0) then
    this:AddWidget("Combobox")
    this:AddPinAF("H", 0)
end
--*/
#define __GPIO_PH_PIN_0_AF__ 0

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
    PH1[#PH1 + 1] = {NAME = "PH1_NC", MODE = "_GPIO_MODE_IN", AF = "0", STATE = "_LOW"}
    PH1[#PH1 + 1] = {NAME = "EVENTOUT", MODE = "_GPIO_MODE_AF_PP", AF = "15", STATE = "_LOW"}

    this:AddWidget("Textbox", "PH1")
    this:AddProposals(PH1, "H", 1)
end
--*/
#define __GPIO_PH_PIN_1_NAME__ PH1_NC

/*--
if this:PinExist("H", 1) then
    this:AddWidget("Combobox")
    this:AddPinSpeed("H", 1)
end
--*/
#define __GPIO_PH_PIN_1_SPEED__ _GPIO_SPEED_LOW

/*--
if this:PinExist("H", 1) then
    this:AddWidget("Combobox")
    this:AddPinMode("H", 1)
end
--*/
#define __GPIO_PH_PIN_1_MODE__ _GPIO_MODE_IN

/*--
if this:PinExist("H", 1) then
    this:AddWidget("Combobox")
    this:AddPinAF("H", 1)
end
--*/
#define __GPIO_PH_PIN_1_AF__ 0

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
    PH2[#PH2 + 1] = {NAME = "PH2_NC", MODE = "_GPIO_MODE_IN", AF = "0", STATE = "_LOW"}
    PH2[#PH2 + 1] = {NAME = "QSPI_BK2_IO0", MODE = "_GPIO_MODE_AF_PP", AF = "9", STATE = "_LOW"}
    PH2[#PH2 + 1] = {NAME = "ETH_MII_CRS", MODE = "_GPIO_MODE_AF_PP", AF = "11", STATE = "_LOW"}
    PH2[#PH2 + 1] = {NAME = "FMC_SDCKE0", MODE = "_GPIO_MODE_AF_PP", AF = "12", STATE = "_LOW"}
    PH2[#PH2 + 1] = {NAME = "LCD_R0", MODE = "_GPIO_MODE_AF_PP", AF = "14", STATE = "_LOW"}
    PH2[#PH2 + 1] = {NAME = "EVENTOUT", MODE = "_GPIO_MODE_AF_PP", AF = "15", STATE = "_LOW"}

    this:AddWidget("Textbox", "PH2")
    this:AddProposals(PH2, "H", 2)
end
--*/
#define __GPIO_PH_PIN_2_NAME__ PH2_NC

/*--
if this:PinExist("H", 2) then
    this:AddWidget("Combobox")
    this:AddPinSpeed("H", 2)
end
--*/
#define __GPIO_PH_PIN_2_SPEED__ _GPIO_SPEED_LOW

/*--
if this:PinExist("H", 2) then
    this:AddWidget("Combobox")
    this:AddPinMode("H", 2)
end
--*/
#define __GPIO_PH_PIN_2_MODE__ _GPIO_MODE_IN

/*--
if this:PinExist("H", 2) then
    this:AddWidget("Combobox")
    this:AddPinAF("H", 2)
end
--*/
#define __GPIO_PH_PIN_2_AF__ 0

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
    PH3[#PH3 + 1] = {NAME = "PH3_NC", MODE = "_GPIO_MODE_IN", AF = "0", STATE = "_LOW"}
    PH3[#PH3 + 1] = {NAME = "QSPI_BK2_IO1", MODE = "_GPIO_MODE_AF_PP", AF = "9", STATE = "_LOW"}
    PH3[#PH3 + 1] = {NAME = "ETH_MII_COL", MODE = "_GPIO_MODE_AF_PP", AF = "11", STATE = "_LOW"}
    PH3[#PH3 + 1] = {NAME = "FMC_SDNE0", MODE = "_GPIO_MODE_AF_PP", AF = "12", STATE = "_LOW"}
    PH3[#PH3 + 1] = {NAME = "LCD_R1", MODE = "_GPIO_MODE_AF_PP", AF = "14", STATE = "_LOW"}
    PH3[#PH3 + 1] = {NAME = "EVENTOUT", MODE = "_GPIO_MODE_AF_PP", AF = "15", STATE = "_LOW"}

    this:AddWidget("Textbox", "PH3")
    this:AddProposals(PH3, "H", 3)
end
--*/
#define __GPIO_PH_PIN_3_NAME__ PH3_NC

/*--
if this:PinExist("H", 3) then
    this:AddWidget("Combobox")
    this:AddPinSpeed("H", 3)
end
--*/
#define __GPIO_PH_PIN_3_SPEED__ _GPIO_SPEED_LOW

/*--
if this:PinExist("H", 3) then
    this:AddWidget("Combobox")
    this:AddPinMode("H", 3)
end
--*/
#define __GPIO_PH_PIN_3_MODE__ _GPIO_MODE_IN

/*--
if this:PinExist("H", 3) then
    this:AddWidget("Combobox")
    this:AddPinAF("H", 3)
end
--*/
#define __GPIO_PH_PIN_3_AF__ 0

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
    PH4[#PH4 + 1] = {NAME = "PH4_NC", MODE = "_GPIO_MODE_IN", AF = "0", STATE = "_LOW"}
    PH4[#PH4 + 1] = {NAME = "I2C2_SCL", MODE = "_GPIO_MODE_AF_PP_PU", AF = "4", STATE = "_LOW"}
    PH4[#PH4 + 1] = {NAME = "LCD_G5", MODE = "_GPIO_MODE_AF_PP", AF = "9", STATE = "_LOW"}
    PH4[#PH4 + 1] = {NAME = "OTG_HS_ULPI_NXT", MODE = "_GPIO_MODE_AF_PP", AF = "10", STATE = "_LOW"}
    PH4[#PH4 + 1] = {NAME = "LCD_G4", MODE = "_GPIO_MODE_AF_PP", AF = "14", STATE = "_LOW"}
    PH4[#PH4 + 1] = {NAME = "EVENTOUT", MODE = "_GPIO_MODE_AF_PP", AF = "15", STATE = "_LOW"}

    this:AddWidget("Textbox", "PH4")
    this:AddProposals(PH4, "H", 4)
end
--*/
#define __GPIO_PH_PIN_4_NAME__ PH4_NC

/*--
if this:PinExist("H", 4) then
    this:AddWidget("Combobox")
    this:AddPinSpeed("H", 4)
end
--*/
#define __GPIO_PH_PIN_4_SPEED__ _GPIO_SPEED_LOW

/*--
if this:PinExist("H", 4) then
    this:AddWidget("Combobox")
    this:AddPinMode("H", 4)
end
--*/
#define __GPIO_PH_PIN_4_MODE__ _GPIO_MODE_IN

/*--
if this:PinExist("H", 4) then
    this:AddWidget("Combobox")
    this:AddPinAF("H", 4)
end
--*/
#define __GPIO_PH_PIN_4_AF__ 0

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
    PH5[#PH5 + 1] = {NAME = "PH5_NC", MODE = "_GPIO_MODE_IN", AF = "0", STATE = "_LOW"}
    PH5[#PH5 + 1] = {NAME = "I2C2_SDA", MODE = "_GPIO_MODE_AF_PP_PU", AF = "4", STATE = "_LOW"}
    PH5[#PH5 + 1] = {NAME = "SPI5_NSS", MODE = "_GPIO_MODE_AF_PP_PU", AF = "5", STATE = "_LOW"}
    PH5[#PH5 + 1] = {NAME = "FMC_SDNWE", MODE = "_GPIO_MODE_AF_PP", AF = "12", STATE = "_LOW"}
    PH5[#PH5 + 1] = {NAME = "EVENTOUT", MODE = "_GPIO_MODE_AF_PP", AF = "15", STATE = "_LOW"}

    this:AddWidget("Textbox", "PH5")
    this:AddProposals(PH5, "H", 5)
end
--*/
#define __GPIO_PH_PIN_5_NAME__ PH5_NC

/*--
if this:PinExist("H", 5) then
    this:AddWidget("Combobox")
    this:AddPinSpeed("H", 5)
end
--*/
#define __GPIO_PH_PIN_5_SPEED__ _GPIO_SPEED_LOW

/*--
if this:PinExist("H", 5) then
    this:AddWidget("Combobox")
    this:AddPinMode("H", 5)
end
--*/
#define __GPIO_PH_PIN_5_MODE__ _GPIO_MODE_IN

/*--
if this:PinExist("H", 5) then
    this:AddWidget("Combobox")
    this:AddPinAF("H", 5)
end
--*/
#define __GPIO_PH_PIN_5_AF__ 0

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
    PH6[#PH6 + 1] = {NAME = "PH6_NC", MODE = "_GPIO_MODE_IN", AF = "0", STATE = "_LOW"}
    PH6[#PH6 + 1] = {NAME = "I2C2_SMBA", MODE = "_GPIO_MODE_AF_PP", AF = "4", STATE = "_LOW"}
    PH6[#PH6 + 1] = {NAME = "SPI5_SCK", MODE = "_GPIO_MODE_AF_PP", AF = "5", STATE = "_LOW"}
    PH6[#PH6 + 1] = {NAME = "TIM12_CH1", MODE = "_GPIO_MODE_AF_PP", AF = "9", STATE = "_LOW"}
    PH6[#PH6 + 1] = {NAME = "ETH_MII_RXD2", MODE = "_GPIO_MODE_AF_PP", AF = "11", STATE = "_LOW"}
    PH6[#PH6 + 1] = {NAME = "FMC_SDNE1", MODE = "_GPIO_MODE_AF_PP", AF = "12", STATE = "_LOW"}
    PH6[#PH6 + 1] = {NAME = "DCMI_D8", MODE = "_GPIO_MODE_AF_PP", AF = "13", STATE = "_LOW"}
    PH6[#PH6 + 1] = {NAME = "EVENTOUT", MODE = "_GPIO_MODE_AF_PP", AF = "15", STATE = "_LOW"}

    this:AddWidget("Textbox", "PH6")
    this:AddProposals(PH6, "H", 6)
end
--*/
#define __GPIO_PH_PIN_6_NAME__ PH6_NC

/*--
if this:PinExist("H", 6) then
    this:AddWidget("Combobox")
    this:AddPinSpeed("H", 6)
end
--*/
#define __GPIO_PH_PIN_6_SPEED__ _GPIO_SPEED_LOW

/*--
if this:PinExist("H", 6) then
    this:AddWidget("Combobox")
    this:AddPinMode("H", 6)
end
--*/
#define __GPIO_PH_PIN_6_MODE__ _GPIO_MODE_IN

/*--
if this:PinExist("H", 6) then
    this:AddWidget("Combobox")
    this:AddPinAF("H", 6)
end
--*/
#define __GPIO_PH_PIN_6_AF__ 0

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
    PH7[#PH7 + 1] = {NAME = "PH7_NC", MODE = "_GPIO_MODE_IN", AF = "0", STATE = "_LOW"}
    PH7[#PH7 + 1] = {NAME = "I2C3_SCL", MODE = "_GPIO_MODE_AF_PP_PU", AF = "4", STATE = "_LOW"}
    PH7[#PH7 + 1] = {NAME = "SPI5_MISO", MODE = "_GPIO_MODE_AF_PP", AF = "5", STATE = "_LOW"}
    PH7[#PH7 + 1] = {NAME = "ETH_MII_RXD3", MODE = "_GPIO_MODE_AF_PP", AF = "11", STATE = "_LOW"}
    PH7[#PH7 + 1] = {NAME = "FMC_SDCKE1", MODE = "_GPIO_MODE_AF_PP", AF = "12", STATE = "_LOW"}
    PH7[#PH7 + 1] = {NAME = "DCMI_D9", MODE = "_GPIO_MODE_AF_PP", AF = "13", STATE = "_LOW"}
    PH7[#PH7 + 1] = {NAME = "EVENTOUT", MODE = "_GPIO_MODE_AF_PP", AF = "15", STATE = "_LOW"}

    this:AddWidget("Textbox", "PH7")
    this:AddProposals(PH7, "H", 7)
end
--*/
#define __GPIO_PH_PIN_7_NAME__ PH7_NC

/*--
if this:PinExist("H", 7) then
    this:AddWidget("Combobox")
    this:AddPinSpeed("H", 7)
end
--*/
#define __GPIO_PH_PIN_7_SPEED__ _GPIO_SPEED_LOW

/*--
if this:PinExist("H", 7) then
    this:AddWidget("Combobox")
    this:AddPinMode("H", 7)
end
--*/
#define __GPIO_PH_PIN_7_MODE__ _GPIO_MODE_IN

/*--
if this:PinExist("H", 7) then
    this:AddWidget("Combobox")
    this:AddPinAF("H", 7)
end
--*/
#define __GPIO_PH_PIN_7_AF__ 0

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
    PH8[#PH8 + 1] = {NAME = "PH8_NC", MODE = "_GPIO_MODE_IN", AF = "0", STATE = "_LOW"}
    PH8[#PH8 + 1] = {NAME = "I2C3_SDA", MODE = "_GPIO_MODE_AF_PP_PU", AF = "4", STATE = "_LOW"}
    PH8[#PH8 + 1] = {NAME = "FMC_D16", MODE = "_GPIO_MODE_AF_PP", AF = "12", STATE = "_LOW"}
    PH8[#PH8 + 1] = {NAME = "DCMI_HSYNC", MODE = "_GPIO_MODE_AF_PP", AF = "13", STATE = "_LOW"}
    PH8[#PH8 + 1] = {NAME = "LCD_R2", MODE = "_GPIO_MODE_AF_PP", AF = "14", STATE = "_LOW"}
    PH8[#PH8 + 1] = {NAME = "EVENTOUT", MODE = "_GPIO_MODE_AF_PP", AF = "15", STATE = "_LOW"}

    this:AddWidget("Textbox", "PH8")
    this:AddProposals(PH8, "H", 8)
end
--*/
#define __GPIO_PH_PIN_8_NAME__ PH8_NC

/*--
if this:PinExist("H", 8) then
    this:AddWidget("Combobox")
    this:AddPinSpeed("H", 8)
end
--*/
#define __GPIO_PH_PIN_8_SPEED__ _GPIO_SPEED_LOW

/*--
if this:PinExist("H", 8) then
    this:AddWidget("Combobox")
    this:AddPinMode("H", 8)
end
--*/
#define __GPIO_PH_PIN_8_MODE__ _GPIO_MODE_IN

/*--
if this:PinExist("H", 8) then
    this:AddWidget("Combobox")
    this:AddPinAF("H", 8)
end
--*/
#define __GPIO_PH_PIN_8_AF__ 0

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
    PH9[#PH9 + 1] = {NAME = "PH9_NC", MODE = "_GPIO_MODE_IN", AF = "0", STATE = "_LOW"}
    PH9[#PH9 + 1] = {NAME = "I2C3_SMBA", MODE = "_GPIO_MODE_AF_PP", AF = "4", STATE = "_LOW"}
    PH9[#PH9 + 1] = {NAME = "TIM12_CH2", MODE = "_GPIO_MODE_AF_PP", AF = "9", STATE = "_LOW"}
    PH9[#PH9 + 1] = {NAME = "FMC_D17", MODE = "_GPIO_MODE_AF_PP", AF = "12", STATE = "_LOW"}
    PH9[#PH9 + 1] = {NAME = "DCMI_D0", MODE = "_GPIO_MODE_AF_PP", AF = "13", STATE = "_LOW"}
    PH9[#PH9 + 1] = {NAME = "LCD_R3", MODE = "_GPIO_MODE_AF_PP", AF = "14", STATE = "_LOW"}
    PH9[#PH9 + 1] = {NAME = "EVENTOUT", MODE = "_GPIO_MODE_AF_PP", AF = "15", STATE = "_LOW"}

    this:AddWidget("Textbox", "PH9")
    this:AddProposals(PH9, "H", 9)
end
--*/
#define __GPIO_PH_PIN_9_NAME__ PH9_NC

/*--
if this:PinExist("H", 9) then
    this:AddWidget("Combobox")
    this:AddPinSpeed("H", 9)
end
--*/
#define __GPIO_PH_PIN_9_SPEED__ _GPIO_SPEED_LOW

/*--
if this:PinExist("H", 9) then
    this:AddWidget("Combobox")
    this:AddPinMode("H", 9)
end
--*/
#define __GPIO_PH_PIN_9_MODE__ _GPIO_MODE_IN

/*--
if this:PinExist("H", 9) then
    this:AddWidget("Combobox")
    this:AddPinAF("H", 9)
end
--*/
#define __GPIO_PH_PIN_9_AF__ 0

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
    PH10[#PH10 + 1] = {NAME = "PH10_NC", MODE = "_GPIO_MODE_IN", AF = "0", STATE = "_LOW"}
    PH10[#PH10 + 1] = {NAME = "TIM5_CH1", MODE = "_GPIO_MODE_AF_PP", AF = "2", STATE = "_LOW"}
    PH10[#PH10 + 1] = {NAME = "FMC_D18", MODE = "_GPIO_MODE_AF_PP", AF = "12", STATE = "_LOW"}
    PH10[#PH10 + 1] = {NAME = "DCMI_D1", MODE = "_GPIO_MODE_AF_PP", AF = "13", STATE = "_LOW"}
    PH10[#PH10 + 1] = {NAME = "LCD_R4", MODE = "_GPIO_MODE_AF_PP", AF = "14", STATE = "_LOW"}
    PH10[#PH10 + 1] = {NAME = "EVENTOUT", MODE = "_GPIO_MODE_AF_PP", AF = "15", STATE = "_LOW"}

    this:AddWidget("Textbox", "PH10")
    this:AddProposals(PH10, "H", 10)
end
--*/
#define __GPIO_PH_PIN_10_NAME__ PH10_NC

/*--
if this:PinExist("H", 10) then
    this:AddWidget("Combobox")
    this:AddPinSpeed("H", 10)
end
--*/
#define __GPIO_PH_PIN_10_SPEED__ _GPIO_SPEED_LOW

/*--
if this:PinExist("H", 10) then
    this:AddWidget("Combobox")
    this:AddPinMode("H", 10)
end
--*/
#define __GPIO_PH_PIN_10_MODE__ _GPIO_MODE_IN

/*--
if this:PinExist("H", 10) then
    this:AddWidget("Combobox")
    this:AddPinAF("H", 10)
end
--*/
#define __GPIO_PH_PIN_10_AF__ 0

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
    PH11[#PH11 + 1] = {NAME = "PH11_NC", MODE = "_GPIO_MODE_IN", AF = "0", STATE = "_LOW"}
    PH11[#PH11 + 1] = {NAME = "TIM5_CH2", MODE = "_GPIO_MODE_AF_PP", AF = "2", STATE = "_LOW"}
    PH11[#PH11 + 1] = {NAME = "FMC_D19", MODE = "_GPIO_MODE_AF_PP", AF = "12", STATE = "_LOW"}
    PH11[#PH11 + 1] = {NAME = "DCMI_D2", MODE = "_GPIO_MODE_AF_PP", AF = "13", STATE = "_LOW"}
    PH11[#PH11 + 1] = {NAME = "LCD_R5", MODE = "_GPIO_MODE_AF_PP", AF = "14", STATE = "_LOW"}
    PH11[#PH11 + 1] = {NAME = "EVENTOUT", MODE = "_GPIO_MODE_AF_PP", AF = "15", STATE = "_LOW"}

    this:AddWidget("Textbox", "PH11")
    this:AddProposals(PH11, "H", 11)
end
--*/
#define __GPIO_PH_PIN_11_NAME__ PH11_NC

/*--
if this:PinExist("H", 11) then
    this:AddWidget("Combobox")
    this:AddPinSpeed("H", 11)
end
--*/
#define __GPIO_PH_PIN_11_SPEED__ _GPIO_SPEED_LOW

/*--
if this:PinExist("H", 11) then
    this:AddWidget("Combobox")
    this:AddPinMode("H", 11)
end
--*/
#define __GPIO_PH_PIN_11_MODE__ _GPIO_MODE_IN

/*--
if this:PinExist("H", 11) then
    this:AddWidget("Combobox")
    this:AddPinAF("H", 11)
end
--*/
#define __GPIO_PH_PIN_11_AF__ 0

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
    PH12[#PH12 + 1] = {NAME = "PH12_NC", MODE = "_GPIO_MODE_IN", AF = "0", STATE = "_LOW"}
    PH12[#PH12 + 1] = {NAME = "TIM5_CH3", MODE = "_GPIO_MODE_AF_PP", AF = "2", STATE = "_LOW"}
    PH12[#PH12 + 1] = {NAME = "FMC_D20", MODE = "_GPIO_MODE_AF_PP", AF = "12", STATE = "_LOW"}
    PH12[#PH12 + 1] = {NAME = "DCMI_D3", MODE = "_GPIO_MODE_AF_PP", AF = "13", STATE = "_LOW"}
    PH12[#PH12 + 1] = {NAME = "LCD_R6", MODE = "_GPIO_MODE_AF_PP", AF = "14", STATE = "_LOW"}
    PH12[#PH12 + 1] = {NAME = "EVENTOUT", MODE = "_GPIO_MODE_AF_PP", AF = "15", STATE = "_LOW"}

    this:AddWidget("Textbox", "PH12")
    this:AddProposals(PH12, "H", 12)
end
--*/
#define __GPIO_PH_PIN_12_NAME__ PH12_NC

/*--
if this:PinExist("H", 12) then
    this:AddWidget("Combobox")
    this:AddPinSpeed("H", 12)
end
--*/
#define __GPIO_PH_PIN_12_SPEED__ _GPIO_SPEED_LOW

/*--
if this:PinExist("H", 12) then
    this:AddWidget("Combobox")
    this:AddPinMode("H", 12)
end
--*/
#define __GPIO_PH_PIN_12_MODE__ _GPIO_MODE_IN

/*--
if this:PinExist("H", 12) then
    this:AddWidget("Combobox")
    this:AddPinAF("H", 12)
end
--*/
#define __GPIO_PH_PIN_12_AF__ 0

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
    PH13[#PH13 + 1] = {NAME = "PH13_NC", MODE = "_GPIO_MODE_IN", AF = "0", STATE = "_LOW"}
    PH13[#PH13 + 1] = {NAME = "TIM8_CH1N", MODE = "_GPIO_MODE_AF_PP", AF = "3", STATE = "_LOW"}
    PH13[#PH13 + 1] = {NAME = "CAN1_TX", MODE = "_GPIO_MODE_AF_PP", AF = "9", STATE = "_LOW"}
    PH13[#PH13 + 1] = {NAME = "FMC_D21", MODE = "_GPIO_MODE_AF_PP", AF = "12", STATE = "_LOW"}
    PH13[#PH13 + 1] = {NAME = "LCD_G2", MODE = "_GPIO_MODE_AF_PP", AF = "14", STATE = "_LOW"}
    PH13[#PH13 + 1] = {NAME = "EVENTOUT", MODE = "_GPIO_MODE_AF_PP", AF = "15", STATE = "_LOW"}

    this:AddWidget("Textbox", "PH13")
    this:AddProposals(PH13, "H", 13)
end
--*/
#define __GPIO_PH_PIN_13_NAME__ PH13_NC

/*--
if this:PinExist("H", 13) then
    this:AddWidget("Combobox")
    this:AddPinSpeed("H", 13)
end
--*/
#define __GPIO_PH_PIN_13_SPEED__ _GPIO_SPEED_LOW

/*--
if this:PinExist("H", 13) then
    this:AddWidget("Combobox")
    this:AddPinMode("H", 13)
end
--*/
#define __GPIO_PH_PIN_13_MODE__ _GPIO_MODE_IN

/*--
if this:PinExist("H", 13) then
    this:AddWidget("Combobox")
    this:AddPinAF("H", 13)
end
--*/
#define __GPIO_PH_PIN_13_AF__ 0

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
    PH14[#PH14 + 1] = {NAME = "PH14_NC", MODE = "_GPIO_MODE_IN", AF = "0", STATE = "_LOW"}
    PH14[#PH14 + 1] = {NAME = "TIM8_CH2N", MODE = "_GPIO_MODE_AF_PP", AF = "3", STATE = "_LOW"}
    PH14[#PH14 + 1] = {NAME = "FMC_D22", MODE = "_GPIO_MODE_AF_PP", AF = "12", STATE = "_LOW"}
    PH14[#PH14 + 1] = {NAME = "DCMI_D4", MODE = "_GPIO_MODE_AF_PP", AF = "13", STATE = "_LOW"}
    PH14[#PH14 + 1] = {NAME = "LCD_G3", MODE = "_GPIO_MODE_AF_PP", AF = "14", STATE = "_LOW"}
    PH14[#PH14 + 1] = {NAME = "EVENTOUT", MODE = "_GPIO_MODE_AF_PP", AF = "15", STATE = "_LOW"}

    this:AddWidget("Textbox", "PH14")
    this:AddProposals(PH14, "H", 14)
end
--*/
#define __GPIO_PH_PIN_14_NAME__ PH14_NC

/*--
if this:PinExist("H", 14) then
    this:AddWidget("Combobox")
    this:AddPinSpeed("H", 14)
end
--*/
#define __GPIO_PH_PIN_14_SPEED__ _GPIO_SPEED_LOW

/*--
if this:PinExist("H", 14) then
    this:AddWidget("Combobox")
    this:AddPinMode("H", 14)
end
--*/
#define __GPIO_PH_PIN_14_MODE__ _GPIO_MODE_IN

/*--
if this:PinExist("H", 14) then
    this:AddWidget("Combobox")
    this:AddPinAF("H", 14)
end
--*/
#define __GPIO_PH_PIN_14_AF__ 0

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
    PH15[#PH15 + 1] = {NAME = "PH15_NC", MODE = "_GPIO_MODE_IN", AF = "0", STATE = "_LOW"}
    PH15[#PH15 + 1] = {NAME = "TIM8_CH3N", MODE = "_GPIO_MODE_AF_PP", AF = "3", STATE = "_LOW"}
    PH15[#PH15 + 1] = {NAME = "FMC_D23", MODE = "_GPIO_MODE_AF_PP", AF = "12", STATE = "_LOW"}
    PH15[#PH15 + 1] = {NAME = "DCMI_D11", MODE = "_GPIO_MODE_AF_PP", AF = "13", STATE = "_LOW"}
    PH15[#PH15 + 1] = {NAME = "LCD_G4", MODE = "_GPIO_MODE_AF_PP", AF = "14", STATE = "_LOW"}
    PH15[#PH15 + 1] = {NAME = "EVENTOUT", MODE = "_GPIO_MODE_AF_PP", AF = "15", STATE = "_LOW"}

    this:AddWidget("Textbox", "PH15")
    this:AddProposals(PH15, "H", 15)
end
--*/
#define __GPIO_PH_PIN_15_NAME__ PH15_NC

/*--
if this:PinExist("H", 15) then
    this:AddWidget("Combobox")
    this:AddPinSpeed("H", 15)
end
--*/
#define __GPIO_PH_PIN_15_SPEED__ _GPIO_SPEED_LOW

/*--
if this:PinExist("H", 15) then
    this:AddWidget("Combobox")
    this:AddPinMode("H", 15)
end
--*/
#define __GPIO_PH_PIN_15_MODE__ _GPIO_MODE_IN

/*--
if this:PinExist("H", 15) then
    this:AddWidget("Combobox")
    this:AddPinAF("H", 15)
end
--*/
#define __GPIO_PH_PIN_15_AF__ 0

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
    PI0[#PI0 + 1] = {NAME = "PI0_NC", MODE = "_GPIO_MODE_IN", AF = "0", STATE = "_LOW"}
    PI0[#PI0 + 1] = {NAME = "TIM5_CH4", MODE = "_GPIO_MODE_AF_PP", AF = "2", STATE = "_LOW"}
    PI0[#PI0 + 1] = {NAME = "SPI2_NSS", MODE = "_GPIO_MODE_AF_PP_PU", AF = "5", STATE = "_LOW"}
    PI0[#PI0 + 1] = {NAME = "I2S2_WS", MODE = "_GPIO_MODE_AF_PP", AF = "5", STATE = "_LOW"}
    PI0[#PI0 + 1] = {NAME = "FMC_D24", MODE = "_GPIO_MODE_AF_PP", AF = "12", STATE = "_LOW"}
    PI0[#PI0 + 1] = {NAME = "DCMI_D13", MODE = "_GPIO_MODE_AF_PP", AF = "13", STATE = "_LOW"}
    PI0[#PI0 + 1] = {NAME = "LCD_G5", MODE = "_GPIO_MODE_AF_PP", AF = "14", STATE = "_LOW"}
    PI0[#PI0 + 1] = {NAME = "EVENTOUT", MODE = "_GPIO_MODE_AF_PP", AF = "15", STATE = "_LOW"}

    this:AddWidget("Textbox", "PI0")
    this:AddProposals(PI0, "I", 0)
end
--*/
#define __GPIO_PI_PIN_0_NAME__ PI0_NC

/*--
if this:PinExist("I", 0) then
    this:AddWidget("Combobox")
    this:AddPinSpeed("I", 0)
end
--*/
#define __GPIO_PI_PIN_0_SPEED__ _GPIO_SPEED_LOW

/*--
if this:PinExist("I", 0) then
    this:AddWidget("Combobox")
    this:AddPinMode("I", 0)
end
--*/
#define __GPIO_PI_PIN_0_MODE__ _GPIO_MODE_IN

/*--
if this:PinExist("I", 0) then
    this:AddWidget("Combobox")
    this:AddPinAF("I", 0)
end
--*/
#define __GPIO_PI_PIN_0_AF__ 0

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
    PI1[#PI1 + 1] = {NAME = "PI1_NC", MODE = "_GPIO_MODE_IN", AF = "0", STATE = "_LOW"}
    PI1[#PI1 + 1] = {NAME = "SPI2_SCK", MODE = "_GPIO_MODE_AF_PP", AF = "5", STATE = "_LOW"}
    PI1[#PI1 + 1] = {NAME = "I2S2_CK", MODE = "_GPIO_MODE_AF_PP", AF = "5", STATE = "_LOW"}
    PI1[#PI1 + 1] = {NAME = "FMC_D25", MODE = "_GPIO_MODE_AF_PP", AF = "12", STATE = "_LOW"}
    PI1[#PI1 + 1] = {NAME = "DCMI_D8", MODE = "_GPIO_MODE_AF_PP", AF = "13", STATE = "_LOW"}
    PI1[#PI1 + 1] = {NAME = "LCD_G6", MODE = "_GPIO_MODE_AF_PP", AF = "14", STATE = "_LOW"}
    PI1[#PI1 + 1] = {NAME = "EVENTOUT", MODE = "_GPIO_MODE_AF_PP", AF = "15", STATE = "_LOW"}

    this:AddWidget("Textbox", "PI1")
    this:AddProposals(PI1, "I", 1)
end
--*/
#define __GPIO_PI_PIN_1_NAME__ PI1_NC

/*--
if this:PinExist("I", 1) then
    this:AddWidget("Combobox")
    this:AddPinSpeed("I", 1)
end
--*/
#define __GPIO_PI_PIN_1_SPEED__ _GPIO_SPEED_LOW

/*--
if this:PinExist("I", 1) then
    this:AddWidget("Combobox")
    this:AddPinMode("I", 1)
end
--*/
#define __GPIO_PI_PIN_1_MODE__ _GPIO_MODE_IN

/*--
if this:PinExist("I", 1) then
    this:AddWidget("Combobox")
    this:AddPinAF("I", 1)
end
--*/
#define __GPIO_PI_PIN_1_AF__ 0

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
    PI2[#PI2 + 1] = {NAME = "PI2_NC", MODE = "_GPIO_MODE_IN", AF = "0", STATE = "_LOW"}
    PI2[#PI2 + 1] = {NAME = "TIM8_CH4", MODE = "_GPIO_MODE_AF_PP", AF = "3", STATE = "_LOW"}
    PI2[#PI2 + 1] = {NAME = "SPI2_MISO", MODE = "_GPIO_MODE_AF_PP", AF = "5", STATE = "_LOW"}
    PI2[#PI2 + 1] = {NAME = "I2S2EXT_SD", MODE = "_GPIO_MODE_AF_PP", AF = "6", STATE = "_LOW"}
    PI2[#PI2 + 1] = {NAME = "FMC_D26", MODE = "_GPIO_MODE_AF_PP", AF = "12", STATE = "_LOW"}
    PI2[#PI2 + 1] = {NAME = "DCMI_D9", MODE = "_GPIO_MODE_AF_PP", AF = "13", STATE = "_LOW"}
    PI2[#PI2 + 1] = {NAME = "LCD_G7", MODE = "_GPIO_MODE_AF_PP", AF = "14", STATE = "_LOW"}
    PI2[#PI2 + 1] = {NAME = "EVENTOUT", MODE = "_GPIO_MODE_AF_PP", AF = "15", STATE = "_LOW"}

    this:AddWidget("Textbox", "PI2")
    this:AddProposals(PI2, "I", 2)
end
--*/
#define __GPIO_PI_PIN_2_NAME__ PI2_NC

/*--
if this:PinExist("I", 2) then
    this:AddWidget("Combobox")
    this:AddPinSpeed("I", 2)
end
--*/
#define __GPIO_PI_PIN_2_SPEED__ _GPIO_SPEED_LOW

/*--
if this:PinExist("I", 2) then
    this:AddWidget("Combobox")
    this:AddPinMode("I", 2)
end
--*/
#define __GPIO_PI_PIN_2_MODE__ _GPIO_MODE_IN

/*--
if this:PinExist("I", 2) then
    this:AddWidget("Combobox")
    this:AddPinAF("I", 2)
end
--*/
#define __GPIO_PI_PIN_2_AF__ 0

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
    PI3[#PI3 + 1] = {NAME = "PI3_NC", MODE = "_GPIO_MODE_IN", AF = "0", STATE = "_LOW"}
    PI3[#PI3 + 1] = {NAME = "TIM8_ETR", MODE = "_GPIO_MODE_AF_PP", AF = "3", STATE = "_LOW"}
    PI3[#PI3 + 1] = {NAME = "SPI2_MOSI", MODE = "_GPIO_MODE_AF_PP", AF = "5", STATE = "_LOW"}
    PI3[#PI3 + 1] = {NAME = "I2S2_SD", MODE = "_GPIO_MODE_AF_PP", AF = "5", STATE = "_LOW"}
    PI3[#PI3 + 1] = {NAME = "FMC_D27", MODE = "_GPIO_MODE_AF_PP", AF = "12", STATE = "_LOW"}
    PI3[#PI3 + 1] = {NAME = "DCMI_D10", MODE = "_GPIO_MODE_AF_PP", AF = "13", STATE = "_LOW"}
    PI3[#PI3 + 1] = {NAME = "EVENTOUT", MODE = "_GPIO_MODE_AF_PP", AF = "15", STATE = "_LOW"}

    this:AddWidget("Textbox", "PI3")
    this:AddProposals(PI3, "I", 3)
end
--*/
#define __GPIO_PI_PIN_3_NAME__ PI3_NC

/*--
if this:PinExist("I", 3) then
    this:AddWidget("Combobox")
    this:AddPinSpeed("I", 3)
end
--*/
#define __GPIO_PI_PIN_3_SPEED__ _GPIO_SPEED_LOW

/*--
if this:PinExist("I", 3) then
    this:AddWidget("Combobox")
    this:AddPinMode("I", 3)
end
--*/
#define __GPIO_PI_PIN_3_MODE__ _GPIO_MODE_IN

/*--
if this:PinExist("I", 3) then
    this:AddWidget("Combobox")
    this:AddPinAF("I", 3)
end
--*/
#define __GPIO_PI_PIN_3_AF__ 0

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
    PI4[#PI4 + 1] = {NAME = "PI4_NC", MODE = "_GPIO_MODE_IN", AF = "0", STATE = "_LOW"}
    PI4[#PI4 + 1] = {NAME = "TIM8_BKIN", MODE = "_GPIO_MODE_AF_PP", AF = "3", STATE = "_LOW"}
    PI4[#PI4 + 1] = {NAME = "FMC_NBL2", MODE = "_GPIO_MODE_AF_PP", AF = "12", STATE = "_LOW"}
    PI4[#PI4 + 1] = {NAME = "DCMI_D5", MODE = "_GPIO_MODE_AF_PP", AF = "13", STATE = "_LOW"}
    PI4[#PI4 + 1] = {NAME = "LCD_B4", MODE = "_GPIO_MODE_AF_PP", AF = "14", STATE = "_LOW"}
    PI4[#PI4 + 1] = {NAME = "EVENTOUT", MODE = "_GPIO_MODE_AF_PP", AF = "15", STATE = "_LOW"}

    this:AddWidget("Textbox", "PI4")
    this:AddProposals(PI4, "I", 4)
end
--*/
#define __GPIO_PI_PIN_4_NAME__ PI4_NC

/*--
if this:PinExist("I", 4) then
    this:AddWidget("Combobox")
    this:AddPinSpeed("I", 4)
end
--*/
#define __GPIO_PI_PIN_4_SPEED__ _GPIO_SPEED_LOW

/*--
if this:PinExist("I", 4) then
    this:AddWidget("Combobox")
    this:AddPinMode("I", 4)
end
--*/
#define __GPIO_PI_PIN_4_MODE__ _GPIO_MODE_IN

/*--
if this:PinExist("I", 4) then
    this:AddWidget("Combobox")
    this:AddPinAF("I", 4)
end
--*/
#define __GPIO_PI_PIN_4_AF__ 0

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
    PI5[#PI5 + 1] = {NAME = "PI5_NC", MODE = "_GPIO_MODE_IN", AF = "0", STATE = "_LOW"}
    PI5[#PI5 + 1] = {NAME = "TIM8_CH1", MODE = "_GPIO_MODE_AF_PP", AF = "3", STATE = "_LOW"}
    PI5[#PI5 + 1] = {NAME = "FMC_NBL3", MODE = "_GPIO_MODE_AF_PP", AF = "12", STATE = "_LOW"}
    PI5[#PI5 + 1] = {NAME = "DCMI_VSYNC", MODE = "_GPIO_MODE_AF_PP", AF = "13", STATE = "_LOW"}
    PI5[#PI5 + 1] = {NAME = "LCD_B5", MODE = "_GPIO_MODE_AF_PP", AF = "14", STATE = "_LOW"}
    PI5[#PI5 + 1] = {NAME = "EVENTOUT", MODE = "_GPIO_MODE_AF_PP", AF = "15", STATE = "_LOW"}

    this:AddWidget("Textbox", "PI5")
    this:AddProposals(PI5, "I", 5)
end
--*/
#define __GPIO_PI_PIN_5_NAME__ PI5_NC

/*--
if this:PinExist("I", 5) then
    this:AddWidget("Combobox")
    this:AddPinSpeed("I", 5)
end
--*/
#define __GPIO_PI_PIN_5_SPEED__ _GPIO_SPEED_LOW

/*--
if this:PinExist("I", 5) then
    this:AddWidget("Combobox")
    this:AddPinMode("I", 5)
end
--*/
#define __GPIO_PI_PIN_5_MODE__ _GPIO_MODE_IN

/*--
if this:PinExist("I", 5) then
    this:AddWidget("Combobox")
    this:AddPinAF("I", 5)
end
--*/
#define __GPIO_PI_PIN_5_AF__ 0

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
    PI6[#PI6 + 1] = {NAME = "PI6_NC", MODE = "_GPIO_MODE_IN", AF = "0", STATE = "_LOW"}
    PI6[#PI6 + 1] = {NAME = "TIM8_CH2", MODE = "_GPIO_MODE_AF_PP", AF = "3", STATE = "_LOW"}
    PI6[#PI6 + 1] = {NAME = "FMC_D28", MODE = "_GPIO_MODE_AF_PP", AF = "12", STATE = "_LOW"}
    PI6[#PI6 + 1] = {NAME = "DCMI_D6", MODE = "_GPIO_MODE_AF_PP", AF = "13", STATE = "_LOW"}
    PI6[#PI6 + 1] = {NAME = "LCD_B6", MODE = "_GPIO_MODE_AF_PP", AF = "14", STATE = "_LOW"}
    PI6[#PI6 + 1] = {NAME = "EVENTOUT", MODE = "_GPIO_MODE_AF_PP", AF = "15", STATE = "_LOW"}

    this:AddWidget("Textbox", "PI6")
    this:AddProposals(PI6, "I", 6)
end
--*/
#define __GPIO_PI_PIN_6_NAME__ PI6_NC

/*--
if this:PinExist("I", 6) then
    this:AddWidget("Combobox")
    this:AddPinSpeed("I", 6)
end
--*/
#define __GPIO_PI_PIN_6_SPEED__ _GPIO_SPEED_LOW

/*--
if this:PinExist("I", 6) then
    this:AddWidget("Combobox")
    this:AddPinMode("I", 6)
end
--*/
#define __GPIO_PI_PIN_6_MODE__ _GPIO_MODE_IN

/*--
if this:PinExist("I", 6) then
    this:AddWidget("Combobox")
    this:AddPinAF("I", 6)
end
--*/
#define __GPIO_PI_PIN_6_AF__ 0

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
    PI7[#PI7 + 1] = {NAME = "PI7_NC", MODE = "_GPIO_MODE_IN", AF = "0", STATE = "_LOW"}
    PI7[#PI7 + 1] = {NAME = "TIM8_CH3", MODE = "_GPIO_MODE_AF_PP", AF = "3", STATE = "_LOW"}
    PI7[#PI7 + 1] = {NAME = "FMC_D29", MODE = "_GPIO_MODE_AF_PP", AF = "12", STATE = "_LOW"}
    PI7[#PI7 + 1] = {NAME = "DCMI_D7", MODE = "_GPIO_MODE_AF_PP", AF = "13", STATE = "_LOW"}
    PI7[#PI7 + 1] = {NAME = "LCD_B7", MODE = "_GPIO_MODE_AF_PP", AF = "14", STATE = "_LOW"}
    PI7[#PI7 + 1] = {NAME = "EVENTOUT", MODE = "_GPIO_MODE_AF_PP", AF = "15", STATE = "_LOW"}

    this:AddWidget("Textbox", "PI7")
    this:AddProposals(PI7, "I", 7)
end
--*/
#define __GPIO_PI_PIN_7_NAME__ PI7_NC

/*--
if this:PinExist("I", 7) then
    this:AddWidget("Combobox")
    this:AddPinSpeed("I", 7)
end
--*/
#define __GPIO_PI_PIN_7_SPEED__ _GPIO_SPEED_LOW

/*--
if this:PinExist("I", 7) then
    this:AddWidget("Combobox")
    this:AddPinMode("I", 7)
end
--*/
#define __GPIO_PI_PIN_7_MODE__ _GPIO_MODE_IN

/*--
if this:PinExist("I", 7) then
    this:AddWidget("Combobox")
    this:AddPinAF("I", 7)
end
--*/
#define __GPIO_PI_PIN_7_AF__ 0

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
    PI8[#PI8 + 1] = {NAME = "PI8_NC", MODE = "_GPIO_MODE_IN", AF = "0", STATE = "_LOW"}
    PI8[#PI8 + 1] = {NAME = "EVENTOUT", MODE = "_GPIO_MODE_AF_PP", AF = "15", STATE = "_LOW"}

    this:AddWidget("Textbox", "PI8")
    this:AddProposals(PI8, "I", 8)
end
--*/
#define __GPIO_PI_PIN_8_NAME__ PI8_NC

/*--
if this:PinExist("I", 8) then
    this:AddWidget("Combobox")
    this:AddPinSpeed("I", 8)
end
--*/
#define __GPIO_PI_PIN_8_SPEED__ _GPIO_SPEED_LOW

/*--
if this:PinExist("I", 8) then
    this:AddWidget("Combobox")
    this:AddPinMode("I", 8)
end
--*/
#define __GPIO_PI_PIN_8_MODE__ _GPIO_MODE_IN

/*--
if this:PinExist("I", 8) then
    this:AddWidget("Combobox")
    this:AddPinAF("I", 8)
end
--*/
#define __GPIO_PI_PIN_8_AF__ 0

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
    PI9[#PI9 + 1] = {NAME = "PI9_NC", MODE = "_GPIO_MODE_IN", AF = "0", STATE = "_LOW"}
    PI9[#PI9 + 1] = {NAME = "CAN1_RX", MODE = "_GPIO_MODE_AF_PP", AF = "9", STATE = "_LOW"}
    PI9[#PI9 + 1] = {NAME = "FMC_D30", MODE = "_GPIO_MODE_AF_PP", AF = "12", STATE = "_LOW"}
    PI9[#PI9 + 1] = {NAME = "LCD_VSYNC", MODE = "_GPIO_MODE_AF_PP", AF = "14", STATE = "_LOW"}
    PI9[#PI9 + 1] = {NAME = "EVENTOUT", MODE = "_GPIO_MODE_AF_PP", AF = "15", STATE = "_LOW"}

    this:AddWidget("Textbox", "PI9")
    this:AddProposals(PI9, "I", 9)
end
--*/
#define __GPIO_PI_PIN_9_NAME__ PI9_NC

/*--
if this:PinExist("I", 9) then
    this:AddWidget("Combobox")
    this:AddPinSpeed("I", 9)
end
--*/
#define __GPIO_PI_PIN_9_SPEED__ _GPIO_SPEED_LOW

/*--
if this:PinExist("I", 9) then
    this:AddWidget("Combobox")
    this:AddPinMode("I", 9)
end
--*/
#define __GPIO_PI_PIN_9_MODE__ _GPIO_MODE_IN

/*--
if this:PinExist("I", 9) then
    this:AddWidget("Combobox")
    this:AddPinAF("I", 9)
end
--*/
#define __GPIO_PI_PIN_9_AF__ 0

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
    PI10[#PI10 + 1] = {NAME = "PI10_NC", MODE = "_GPIO_MODE_IN", AF = "0", STATE = "_LOW"}
    PI10[#PI10 + 1] = {NAME = "ETH_MII_RX_ER", MODE = "_GPIO_MODE_AF_PP", AF = "11", STATE = "_LOW"}
    PI10[#PI10 + 1] = {NAME = "FMC_D31", MODE = "_GPIO_MODE_AF_PP", AF = "12", STATE = "_LOW"}
    PI10[#PI10 + 1] = {NAME = "LCD_HSYNC", MODE = "_GPIO_MODE_AF_PP", AF = "14", STATE = "_LOW"}
    PI10[#PI10 + 1] = {NAME = "EVENTOUT", MODE = "_GPIO_MODE_AF_PP", AF = "15", STATE = "_LOW"}

    this:AddWidget("Textbox", "PI10")
    this:AddProposals(PI10, "I", 10)
end
--*/
#define __GPIO_PI_PIN_10_NAME__ PI10_NC

/*--
if this:PinExist("I", 10) then
    this:AddWidget("Combobox")
    this:AddPinSpeed("I", 10)
end
--*/
#define __GPIO_PI_PIN_10_SPEED__ _GPIO_SPEED_LOW

/*--
if this:PinExist("I", 10) then
    this:AddWidget("Combobox")
    this:AddPinMode("I", 10)
end
--*/
#define __GPIO_PI_PIN_10_MODE__ _GPIO_MODE_IN

/*--
if this:PinExist("I", 10) then
    this:AddWidget("Combobox")
    this:AddPinAF("I", 10)
end
--*/
#define __GPIO_PI_PIN_10_AF__ 0

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
    PI11[#PI11 + 1] = {NAME = "PI11_NC", MODE = "_GPIO_MODE_IN", AF = "0", STATE = "_LOW"}
    PI11[#PI11 + 1] = {NAME = "LCD_G6", MODE = "_GPIO_MODE_AF_PP", AF = "9", STATE = "_LOW"}
    PI11[#PI11 + 1] = {NAME = "OTG_HS_ULPI_DIR", MODE = "_GPIO_MODE_AF_PP", AF = "10", STATE = "_LOW"}
    PI11[#PI11 + 1] = {NAME = "EVENTOUT", MODE = "_GPIO_MODE_AF_PP", AF = "15", STATE = "_LOW"}

    this:AddWidget("Textbox", "PI11")
    this:AddProposals(PI11, "I", 11)
end
--*/
#define __GPIO_PI_PIN_11_NAME__ PI11_NC

/*--
if this:PinExist("I", 11) then
    this:AddWidget("Combobox")
    this:AddPinSpeed("I", 11)
end
--*/
#define __GPIO_PI_PIN_11_SPEED__ _GPIO_SPEED_LOW

/*--
if this:PinExist("I", 11) then
    this:AddWidget("Combobox")
    this:AddPinMode("I", 11)
end
--*/
#define __GPIO_PI_PIN_11_MODE__ _GPIO_MODE_IN

/*--
if this:PinExist("I", 11) then
    this:AddWidget("Combobox")
    this:AddPinAF("I", 11)
end
--*/
#define __GPIO_PI_PIN_11_AF__ 0

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
    PI12[#PI12 + 1] = {NAME = "PI12_NC", MODE = "_GPIO_MODE_IN", AF = "0", STATE = "_LOW"}
    PI12[#PI12 + 1] = {NAME = "LCD_HSYNC", MODE = "_GPIO_MODE_AF_PP", AF = "14", STATE = "_LOW"}
    PI12[#PI12 + 1] = {NAME = "EVENTOUT", MODE = "_GPIO_MODE_AF_PP", AF = "15", STATE = "_LOW"}

    this:AddWidget("Textbox", "PI12")
    this:AddProposals(PI12, "I", 12)
end
--*/
#define __GPIO_PI_PIN_12_NAME__ PI12_NC

/*--
if this:PinExist("I", 12) then
    this:AddWidget("Combobox")
    this:AddPinSpeed("I", 12)
end
--*/
#define __GPIO_PI_PIN_12_SPEED__ _GPIO_SPEED_LOW

/*--
if this:PinExist("I", 12) then
    this:AddWidget("Combobox")
    this:AddPinMode("I", 12)
end
--*/
#define __GPIO_PI_PIN_12_MODE__ _GPIO_MODE_IN

/*--
if this:PinExist("I", 12) then
    this:AddWidget("Combobox")
    this:AddPinAF("I", 12)
end
--*/
#define __GPIO_PI_PIN_12_AF__ 0

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
    PI13[#PI13 + 1] = {NAME = "PI13_NC", MODE = "_GPIO_MODE_IN", AF = "0", STATE = "_LOW"}
    PI13[#PI13 + 1] = {NAME = "LCD_VSYNC", MODE = "_GPIO_MODE_AF_PP", AF = "14", STATE = "_LOW"}
    PI13[#PI13 + 1] = {NAME = "EVENTOUT", MODE = "_GPIO_MODE_AF_PP", AF = "15", STATE = "_LOW"}

    this:AddWidget("Textbox", "PI13")
    this:AddProposals(PI13, "I", 13)
end
--*/
#define __GPIO_PI_PIN_13_NAME__ PI13_NC

/*--
if this:PinExist("I", 13) then
    this:AddWidget("Combobox")
    this:AddPinSpeed("I", 13)
end
--*/
#define __GPIO_PI_PIN_13_SPEED__ _GPIO_SPEED_LOW

/*--
if this:PinExist("I", 13) then
    this:AddWidget("Combobox")
    this:AddPinMode("I", 13)
end
--*/
#define __GPIO_PI_PIN_13_MODE__ _GPIO_MODE_IN

/*--
if this:PinExist("I", 13) then
    this:AddWidget("Combobox")
    this:AddPinAF("I", 13)
end
--*/
#define __GPIO_PI_PIN_13_AF__ 0

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
    PI14[#PI14 + 1] = {NAME = "PI14_NC", MODE = "_GPIO_MODE_IN", AF = "0", STATE = "_LOW"}
    PI14[#PI14 + 1] = {NAME = "LCD_CLK", MODE = "_GPIO_MODE_AF_PP", AF = "14", STATE = "_LOW"}
    PI14[#PI14 + 1] = {NAME = "EVENTOUT", MODE = "_GPIO_MODE_AF_PP", AF = "15", STATE = "_LOW"}

    this:AddWidget("Textbox", "PI14")
    this:AddProposals(PI14, "I", 14)
end
--*/
#define __GPIO_PI_PIN_14_NAME__ PI14_NC

/*--
if this:PinExist("I", 14) then
    this:AddWidget("Combobox")
    this:AddPinSpeed("I", 14)
end
--*/
#define __GPIO_PI_PIN_14_SPEED__ _GPIO_SPEED_LOW

/*--
if this:PinExist("I", 14) then
    this:AddWidget("Combobox")
    this:AddPinMode("I", 14)
end
--*/
#define __GPIO_PI_PIN_14_MODE__ _GPIO_MODE_IN

/*--
if this:PinExist("I", 14) then
    this:AddWidget("Combobox")
    this:AddPinAF("I", 14)
end
--*/
#define __GPIO_PI_PIN_14_AF__ 0

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
    PI15[#PI15 + 1] = {NAME = "PI15_NC", MODE = "_GPIO_MODE_IN", AF = "0", STATE = "_LOW"}
    PI15[#PI15 + 1] = {NAME = "LCD_G2", MODE = "_GPIO_MODE_AF_PP", AF = "9", STATE = "_LOW"}
    PI15[#PI15 + 1] = {NAME = "LCD_R0", MODE = "_GPIO_MODE_AF_PP", AF = "14", STATE = "_LOW"}
    PI15[#PI15 + 1] = {NAME = "EVENTOUT", MODE = "_GPIO_MODE_AF_PP", AF = "15", STATE = "_LOW"}

    this:AddWidget("Textbox", "PI15")
    this:AddProposals(PI15, "I", 15)
end
--*/
#define __GPIO_PI_PIN_15_NAME__ PI15_NC

/*--
if this:PinExist("I", 15) then
    this:AddWidget("Combobox")
    this:AddPinSpeed("I", 15)
end
--*/
#define __GPIO_PI_PIN_15_SPEED__ _GPIO_SPEED_LOW

/*--
if this:PinExist("I", 15) then
    this:AddWidget("Combobox")
    this:AddPinMode("I", 15)
end
--*/
#define __GPIO_PI_PIN_15_MODE__ _GPIO_MODE_IN

/*--
if this:PinExist("I", 15) then
    this:AddWidget("Combobox")
    this:AddPinAF("I", 15)
end
--*/
#define __GPIO_PI_PIN_15_AF__ 0

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
    PJ0[#PJ0 + 1] = {NAME = "PJ0_NC", MODE = "_GPIO_MODE_IN", AF = "0", STATE = "_LOW"}
    PJ0[#PJ0 + 1] = {NAME = "LCD_R7", MODE = "_GPIO_MODE_AF_PP", AF = "9", STATE = "_LOW"}
    PJ0[#PJ0 + 1] = {NAME = "LCD_R1", MODE = "_GPIO_MODE_AF_PP", AF = "14", STATE = "_LOW"}
    PJ0[#PJ0 + 1] = {NAME = "EVENTOUT", MODE = "_GPIO_MODE_AF_PP", AF = "15", STATE = "_LOW"}

    this:AddWidget("Textbox", "PJ0")
    this:AddProposals(PJ0, "J", 0)
end
--*/
#define __GPIO_PJ_PIN_0_NAME__ PJ0_NC

/*--
if this:PinExist("J", 0) then
    this:AddWidget("Combobox")
    this:AddPinSpeed("J", 0)
end
--*/
#define __GPIO_PJ_PIN_0_SPEED__ _GPIO_SPEED_LOW

/*--
if this:PinExist("J", 0) then
    this:AddWidget("Combobox")
    this:AddPinMode("J", 0)
end
--*/
#define __GPIO_PJ_PIN_0_MODE__ _GPIO_MODE_IN

/*--
if this:PinExist("J", 0) then
    this:AddWidget("Combobox")
    this:AddPinAF("J", 0)
end
--*/
#define __GPIO_PJ_PIN_0_AF__ 0

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
    PJ1[#PJ1 + 1] = {NAME = "PJ1_NC", MODE = "_GPIO_MODE_IN", AF = "0", STATE = "_LOW"}
    PJ1[#PJ1 + 1] = {NAME = "LCD_R2", MODE = "_GPIO_MODE_AF_PP", AF = "14", STATE = "_LOW"}
    PJ1[#PJ1 + 1] = {NAME = "EVENTOUT", MODE = "_GPIO_MODE_AF_PP", AF = "15", STATE = "_LOW"}

    this:AddWidget("Textbox", "PJ1")
    this:AddProposals(PJ1, "J", 1)
end
--*/
#define __GPIO_PJ_PIN_1_NAME__ PJ1_NC

/*--
if this:PinExist("J", 1) then
    this:AddWidget("Combobox")
    this:AddPinSpeed("J", 1)
end
--*/
#define __GPIO_PJ_PIN_1_SPEED__ _GPIO_SPEED_LOW

/*--
if this:PinExist("J", 1) then
    this:AddWidget("Combobox")
    this:AddPinMode("J", 1)
end
--*/
#define __GPIO_PJ_PIN_1_MODE__ _GPIO_MODE_IN

/*--
if this:PinExist("J", 1) then
    this:AddWidget("Combobox")
    this:AddPinAF("J", 1)
end
--*/
#define __GPIO_PJ_PIN_1_AF__ 0

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
    PJ2[#PJ2 + 1] = {NAME = "PJ2_NC", MODE = "_GPIO_MODE_IN", AF = "0", STATE = "_LOW"}
    PJ2[#PJ2 + 1] = {NAME = "DSIHOST_TE", MODE = "_GPIO_MODE_AF_PP", AF = "13", STATE = "_LOW"}
    PJ2[#PJ2 + 1] = {NAME = "LCD_R3", MODE = "_GPIO_MODE_AF_PP", AF = "14", STATE = "_LOW"}
    PJ2[#PJ2 + 1] = {NAME = "EVENTOUT", MODE = "_GPIO_MODE_AF_PP", AF = "15", STATE = "_LOW"}

    this:AddWidget("Textbox", "PJ2")
    this:AddProposals(PJ2, "J", 2)
end
--*/
#define __GPIO_PJ_PIN_2_NAME__ PJ2_NC

/*--
if this:PinExist("J", 2) then
    this:AddWidget("Combobox")
    this:AddPinSpeed("J", 2)
end
--*/
#define __GPIO_PJ_PIN_2_SPEED__ _GPIO_SPEED_LOW

/*--
if this:PinExist("J", 2) then
    this:AddWidget("Combobox")
    this:AddPinMode("J", 2)
end
--*/
#define __GPIO_PJ_PIN_2_MODE__ _GPIO_MODE_IN

/*--
if this:PinExist("J", 2) then
    this:AddWidget("Combobox")
    this:AddPinAF("J", 2)
end
--*/
#define __GPIO_PJ_PIN_2_AF__ 0

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
    PJ3[#PJ3 + 1] = {NAME = "PJ3_NC", MODE = "_GPIO_MODE_IN", AF = "0", STATE = "_LOW"}
    PJ3[#PJ3 + 1] = {NAME = "LCD_R4", MODE = "_GPIO_MODE_AF_PP", AF = "14", STATE = "_LOW"}
    PJ3[#PJ3 + 1] = {NAME = "EVENTOUT", MODE = "_GPIO_MODE_AF_PP", AF = "15", STATE = "_LOW"}

    this:AddWidget("Textbox", "PJ3")
    this:AddProposals(PJ3, "J", 3)
end
--*/
#define __GPIO_PJ_PIN_3_NAME__ PJ3_NC

/*--
if this:PinExist("J", 3) then
    this:AddWidget("Combobox")
    this:AddPinSpeed("J", 3)
end
--*/
#define __GPIO_PJ_PIN_3_SPEED__ _GPIO_SPEED_LOW

/*--
if this:PinExist("J", 3) then
    this:AddWidget("Combobox")
    this:AddPinMode("J", 3)
end
--*/
#define __GPIO_PJ_PIN_3_MODE__ _GPIO_MODE_IN

/*--
if this:PinExist("J", 3) then
    this:AddWidget("Combobox")
    this:AddPinAF("J", 3)
end
--*/
#define __GPIO_PJ_PIN_3_AF__ 0

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
    PJ4[#PJ4 + 1] = {NAME = "PJ4_NC", MODE = "_GPIO_MODE_IN", AF = "0", STATE = "_LOW"}
    PJ4[#PJ4 + 1] = {NAME = "LCD_R5", MODE = "_GPIO_MODE_AF_PP", AF = "14", STATE = "_LOW"}
    PJ4[#PJ4 + 1] = {NAME = "EVENTOUT", MODE = "_GPIO_MODE_AF_PP", AF = "15", STATE = "_LOW"}

    this:AddWidget("Textbox", "PJ4")
    this:AddProposals(PJ4, "J", 4)
end
--*/
#define __GPIO_PJ_PIN_4_NAME__ PJ4_NC

/*--
if this:PinExist("J", 4) then
    this:AddWidget("Combobox")
    this:AddPinSpeed("J", 4)
end
--*/
#define __GPIO_PJ_PIN_4_SPEED__ _GPIO_SPEED_LOW

/*--
if this:PinExist("J", 4) then
    this:AddWidget("Combobox")
    this:AddPinMode("J", 4)
end
--*/
#define __GPIO_PJ_PIN_4_MODE__ _GPIO_MODE_IN

/*--
if this:PinExist("J", 4) then
    this:AddWidget("Combobox")
    this:AddPinAF("J", 4)
end
--*/
#define __GPIO_PJ_PIN_4_AF__ 0

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
    PJ5[#PJ5 + 1] = {NAME = "PJ5_NC", MODE = "_GPIO_MODE_IN", AF = "0", STATE = "_LOW"}
    PJ5[#PJ5 + 1] = {NAME = "LCD_R6", MODE = "_GPIO_MODE_AF_PP", AF = "14", STATE = "_LOW"}
    PJ5[#PJ5 + 1] = {NAME = "EVENTOUT", MODE = "_GPIO_MODE_AF_PP", AF = "15", STATE = "_LOW"}

    this:AddWidget("Textbox", "PJ5")
    this:AddProposals(PJ5, "J", 5)
end
--*/
#define __GPIO_PJ_PIN_5_NAME__ PJ5_NC

/*--
if this:PinExist("J", 5) then
    this:AddWidget("Combobox")
    this:AddPinSpeed("J", 5)
end
--*/
#define __GPIO_PJ_PIN_5_SPEED__ _GPIO_SPEED_LOW

/*--
if this:PinExist("J", 5) then
    this:AddWidget("Combobox")
    this:AddPinMode("J", 5)
end
--*/
#define __GPIO_PJ_PIN_5_MODE__ _GPIO_MODE_IN

/*--
if this:PinExist("J", 5) then
    this:AddWidget("Combobox")
    this:AddPinAF("J", 5)
end
--*/
#define __GPIO_PJ_PIN_5_AF__ 0

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
    PJ6[#PJ6 + 1] = {NAME = "PJ6_NC", MODE = "_GPIO_MODE_IN", AF = "0", STATE = "_LOW"}
    PJ6[#PJ6 + 1] = {NAME = "LCD_R7", MODE = "_GPIO_MODE_IN", AF = "14", STATE = "_LOW"}
    PJ6[#PJ6 + 1] = {NAME = "EVENTOUT", MODE = "_GPIO_MODE_AF_PP", AF = "15", STATE = "_LOW"}

    this:AddWidget("Textbox", "PJ6")
    this:AddProposals(PJ6, "J", 6)
end
--*/
#define __GPIO_PJ_PIN_6_NAME__ PJ6_NC

/*--
if this:PinExist("J", 6) then
    this:AddWidget("Combobox")
    this:AddPinSpeed("J", 6)
end
--*/
#define __GPIO_PJ_PIN_6_SPEED__ _GPIO_SPEED_LOW

/*--
if this:PinExist("J", 6) then
    this:AddWidget("Combobox")
    this:AddPinMode("J", 6)
end
--*/
#define __GPIO_PJ_PIN_6_MODE__ _GPIO_MODE_IN

/*--
if this:PinExist("J", 6) then
    this:AddWidget("Combobox")
    this:AddPinAF("J", 6)
end
--*/
#define __GPIO_PJ_PIN_6_AF__ 0

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
    PJ7[#PJ7 + 1] = {NAME = "PJ7_NC", MODE = "_GPIO_MODE_IN", AF = "0", STATE = "_LOW"}
    PJ7[#PJ7 + 1] = {NAME = "LCD_G0", MODE = "_GPIO_MODE_IN", AF = "14", STATE = "_LOW"}
    PJ7[#PJ7 + 1] = {NAME = "EVENTOUT", MODE = "_GPIO_MODE_AF_PP", AF = "15", STATE = "_LOW"}

    this:AddWidget("Textbox", "PJ7")
    this:AddProposals(PJ7, "J", 7)
end
--*/
#define __GPIO_PJ_PIN_7_NAME__ PJ7_NC

/*--
if this:PinExist("J", 7) then
    this:AddWidget("Combobox")
    this:AddPinSpeed("J", 7)
end
--*/
#define __GPIO_PJ_PIN_7_SPEED__ _GPIO_SPEED_LOW

/*--
if this:PinExist("J", 7) then
    this:AddWidget("Combobox")
    this:AddPinMode("J", 7)
end
--*/
#define __GPIO_PJ_PIN_7_MODE__ _GPIO_MODE_IN

/*--
if this:PinExist("J", 7) then
    this:AddWidget("Combobox")
    this:AddPinAF("J", 7)
end
--*/
#define __GPIO_PJ_PIN_7_AF__ 0

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
    PJ8[#PJ8 + 1] = {NAME = "PJ8_NC", MODE = "_GPIO_MODE_IN", AF = "0", STATE = "_LOW"}
    PJ8[#PJ8 + 1] = {NAME = "LCD_G1", MODE = "_GPIO_MODE_IN", AF = "14", STATE = "_LOW"}
    PJ8[#PJ8 + 1] = {NAME = "EVENTOUT", MODE = "_GPIO_MODE_AF_PP", AF = "15", STATE = "_LOW"}

    this:AddWidget("Textbox", "PJ8")
    this:AddProposals(PJ8, "J", 8)
end
--*/
#define __GPIO_PJ_PIN_8_NAME__ PJ8_NC

/*--
if this:PinExist("J", 8) then
    this:AddWidget("Combobox")
    this:AddPinSpeed("J", 8)
end
--*/
#define __GPIO_PJ_PIN_8_SPEED__ _GPIO_SPEED_LOW

/*--
if this:PinExist("J", 8) then
    this:AddWidget("Combobox")
    this:AddPinMode("J", 8)
end
--*/
#define __GPIO_PJ_PIN_8_MODE__ _GPIO_MODE_IN

/*--
if this:PinExist("J", 8) then
    this:AddWidget("Combobox")
    this:AddPinAF("J", 8)
end
--*/
#define __GPIO_PJ_PIN_8_AF__ 0

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
    PJ9[#PJ9 + 1] = {NAME = "PJ9_NC", MODE = "_GPIO_MODE_IN", AF = "0", STATE = "_LOW"}
    PJ9[#PJ9 + 1] = {NAME = "LCD_G2", MODE = "_GPIO_MODE_IN", AF = "14", STATE = "_LOW"}
    PJ9[#PJ9 + 1] = {NAME = "EVENTOUT", MODE = "_GPIO_MODE_AF_PP", AF = "15", STATE = "_LOW"}

    this:AddWidget("Textbox", "PJ9")
    this:AddProposals(PJ9, "J", 9)
end
--*/
#define __GPIO_PJ_PIN_9_NAME__ PJ9_NC

/*--
if this:PinExist("J", 9) then
    this:AddWidget("Combobox")
    this:AddPinSpeed("J", 9)
end
--*/
#define __GPIO_PJ_PIN_9_SPEED__ _GPIO_SPEED_LOW

/*--
if this:PinExist("J", 9) then
    this:AddWidget("Combobox")
    this:AddPinMode("J", 9)
end
--*/
#define __GPIO_PJ_PIN_9_MODE__ _GPIO_MODE_IN

/*--
if this:PinExist("J", 9) then
    this:AddWidget("Combobox")
    this:AddPinAF("J", 9)
end
--*/
#define __GPIO_PJ_PIN_9_AF__ 0

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
    PJ10[#PJ10 + 1] = {NAME = "PJ10_NC", MODE = "_GPIO_MODE_IN", AF = "0", STATE = "_LOW"}
    PJ10[#PJ10 + 1] = {NAME = "LCD_G3", MODE = "_GPIO_MODE_IN", AF = "14", STATE = "_LOW"}
    PJ10[#PJ10 + 1] = {NAME = "EVENTOUT", MODE = "_GPIO_MODE_AF_PP", AF = "15", STATE = "_LOW"}

    this:AddWidget("Textbox", "PJ10")
    this:AddProposals(PJ10, "J", 10)
end
--*/
#define __GPIO_PJ_PIN_10_NAME__ PJ10_NC

/*--
if this:PinExist("J", 10) then
    this:AddWidget("Combobox")
    this:AddPinSpeed("J", 10)
end
--*/
#define __GPIO_PJ_PIN_10_SPEED__ _GPIO_SPEED_LOW

/*--
if this:PinExist("J", 10) then
    this:AddWidget("Combobox")
    this:AddPinMode("J", 10)
end
--*/
#define __GPIO_PJ_PIN_10_MODE__ _GPIO_MODE_IN

/*--
if this:PinExist("J", 10) then
    this:AddWidget("Combobox")
    this:AddPinAF("J", 10)
end
--*/
#define __GPIO_PJ_PIN_10_AF__ 0

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
    PJ11[#PJ11 + 1] = {NAME = "PJ11_NC", MODE = "_GPIO_MODE_IN", AF = "0", STATE = "_LOW"}
    PJ11[#PJ11 + 1] = {NAME = "LCD_G4", MODE = "_GPIO_MODE_IN", AF = "14", STATE = "_LOW"}
    PJ11[#PJ11 + 1] = {NAME = "EVENTOUT", MODE = "_GPIO_MODE_AF_PP", AF = "15", STATE = "_LOW"}

    this:AddWidget("Textbox", "PJ11")
    this:AddProposals(PJ11, "J", 11)
end
--*/
#define __GPIO_PJ_PIN_11_NAME__ PJ11_NC

/*--
if this:PinExist("J", 11) then
    this:AddWidget("Combobox")
    this:AddPinSpeed("J", 11)
end
--*/
#define __GPIO_PJ_PIN_11_SPEED__ _GPIO_SPEED_LOW

/*--
if this:PinExist("J", 11) then
    this:AddWidget("Combobox")
    this:AddPinMode("J", 11)
end
--*/
#define __GPIO_PJ_PIN_11_MODE__ _GPIO_MODE_IN

/*--
if this:PinExist("J", 11) then
    this:AddWidget("Combobox")
    this:AddPinAF("J", 11)
end
--*/
#define __GPIO_PJ_PIN_11_AF__ 0

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
    PJ12[#PJ12 + 1] = {NAME = "PJ12_NC", MODE = "_GPIO_MODE_IN", AF = "0", STATE = "_LOW"}
    PJ12[#PJ12 + 1] = {NAME = "LCD_G3", MODE = "_GPIO_MODE_AF_PP", AF = "9", STATE = "_LOW"}
    PJ12[#PJ12 + 1] = {NAME = "LCD_B0", MODE = "_GPIO_MODE_AF_PP", AF = "14", STATE = "_LOW"}
    PJ12[#PJ12 + 1] = {NAME = "EVENTOUT", MODE = "_GPIO_MODE_AF_PP", AF = "15", STATE = "_LOW"}

    this:AddWidget("Textbox", "PJ12")
    this:AddProposals(PJ12, "J", 12)
end
--*/
#define __GPIO_PJ_PIN_12_NAME__ PJ12_NC

/*--
if this:PinExist("J", 12) then
    this:AddWidget("Combobox")
    this:AddPinSpeed("J", 12)
end
--*/
#define __GPIO_PJ_PIN_12_SPEED__ _GPIO_SPEED_LOW

/*--
if this:PinExist("J", 12) then
    this:AddWidget("Combobox")
    this:AddPinMode("J", 12)
end
--*/
#define __GPIO_PJ_PIN_12_MODE__ _GPIO_MODE_IN

/*--
if this:PinExist("J", 12) then
    this:AddWidget("Combobox")
    this:AddPinAF("J", 12)
end
--*/
#define __GPIO_PJ_PIN_12_AF__ 0

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
    PJ13[#PJ13 + 1] = {NAME = "PJ13_NC", MODE = "_GPIO_MODE_IN", AF = "0", STATE = "_LOW"}
    PJ13[#PJ13 + 1] = {NAME = "LCD_G4", MODE = "_GPIO_MODE_AF_PP", AF = "9", STATE = "_LOW"}
    PJ13[#PJ13 + 1] = {NAME = "LCD_B1", MODE = "_GPIO_MODE_AF_PP", AF = "14", STATE = "_LOW"}
    PJ13[#PJ13 + 1] = {NAME = "EVENTOUT", MODE = "_GPIO_MODE_AF_PP", AF = "15", STATE = "_LOW"}

    this:AddWidget("Textbox", "PJ13")
    this:AddProposals(PJ13, "J", 13)
end
--*/
#define __GPIO_PJ_PIN_13_NAME__ PJ13_NC

/*--
if this:PinExist("J", 13) then
    this:AddWidget("Combobox")
    this:AddPinSpeed("J", 13)
end
--*/
#define __GPIO_PJ_PIN_13_SPEED__ _GPIO_SPEED_LOW

/*--
if this:PinExist("J", 13) then
    this:AddWidget("Combobox")
    this:AddPinMode("J", 13)
end
--*/
#define __GPIO_PJ_PIN_13_MODE__ _GPIO_MODE_IN

/*--
if this:PinExist("J", 13) then
    this:AddWidget("Combobox")
    this:AddPinAF("J", 13)
end
--*/
#define __GPIO_PJ_PIN_13_AF__ 0

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
    PJ14[#PJ14 + 1] = {NAME = "PJ14_NC", MODE = "_GPIO_MODE_IN", AF = "0", STATE = "_LOW"}
    PJ14[#PJ14 + 1] = {NAME = "LCD_B2", MODE = "_GPIO_MODE_AF_PP", AF = "14", STATE = "_LOW"}
    PJ14[#PJ14 + 1] = {NAME = "EVENTOUT", MODE = "_GPIO_MODE_AF_PP", AF = "15", STATE = "_LOW"}

    this:AddWidget("Textbox", "PJ14")
    this:AddProposals(PJ14, "J", 14)
end
--*/
#define __GPIO_PJ_PIN_14_NAME__ PJ14_NC

/*--
if this:PinExist("J", 14) then
    this:AddWidget("Combobox")
    this:AddPinSpeed("J", 14)
end
--*/
#define __GPIO_PJ_PIN_14_SPEED__ _GPIO_SPEED_LOW

/*--
if this:PinExist("J", 14) then
    this:AddWidget("Combobox")
    this:AddPinMode("J", 14)
end
--*/
#define __GPIO_PJ_PIN_14_MODE__ _GPIO_MODE_IN

/*--
if this:PinExist("J", 14) then
    this:AddWidget("Combobox")
    this:AddPinAF("J", 14)
end
--*/
#define __GPIO_PJ_PIN_14_AF__ 0

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
    PJ15[#PJ15 + 1] = {NAME = "PJ15_NC", MODE = "_GPIO_MODE_IN", AF = "0", STATE = "_LOW"}
    PJ15[#PJ15 + 1] = {NAME = "LCD_B3", MODE = "_GPIO_MODE_AF_PP", AF = "14", STATE = "_LOW"}
    PJ15[#PJ15 + 1] = {NAME = "EVENTOUT", MODE = "_GPIO_MODE_AF_PP", AF = "15", STATE = "_LOW"}

    this:AddWidget("Textbox", "PJ15")
    this:AddProposals(PJ15, "J", 15)
end
--*/
#define __GPIO_PJ_PIN_15_NAME__ PJ15_NC

/*--
if this:PinExist("J", 15) then
    this:AddWidget("Combobox")
    this:AddPinSpeed("J", 15)
end
--*/
#define __GPIO_PJ_PIN_15_SPEED__ _GPIO_SPEED_LOW

/*--
if this:PinExist("J", 15) then
    this:AddWidget("Combobox")
    this:AddPinMode("J", 15)
end
--*/
#define __GPIO_PJ_PIN_15_MODE__ _GPIO_MODE_IN

/*--
if this:PinExist("J", 15) then
    this:AddWidget("Combobox")
    this:AddPinAF("J", 15)
end
--*/
#define __GPIO_PJ_PIN_15_AF__ 0

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
    PK0[#PK0 + 1] = {NAME = "PK0_NC", MODE = "_GPIO_MODE_IN", AF = "0", STATE = "_LOW"}
    PK0[#PK0 + 1] = {NAME = "LCD_G5", MODE = "_GPIO_MODE_IN", AF = "14", STATE = "_LOW"}
    PK0[#PK0 + 1] = {NAME = "EVENTOUT", MODE = "_GPIO_MODE_AF_PP", AF = "15", STATE = "_LOW"}

    this:AddWidget("Textbox", "PK0")
    this:AddProposals(PK0, "K", 0)
end
--*/
#define __GPIO_PK_PIN_0_NAME__ PK0_NC

/*--
if this:PinExist("K", 0) then
    this:AddWidget("Combobox")
    this:AddPinSpeed("K", 0)
end
--*/
#define __GPIO_PK_PIN_0_SPEED__ _GPIO_SPEED_LOW

/*--
if this:PinExist("K", 0) then
    this:AddWidget("Combobox")
    this:AddPinMode("K", 0)
end
--*/
#define __GPIO_PK_PIN_0_MODE__ _GPIO_MODE_IN

/*--
if this:PinExist("K", 0) then
    this:AddWidget("Combobox")
    this:AddPinAF("K", 0)
end
--*/
#define __GPIO_PK_PIN_0_AF__ 0

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
    PK1[#PK1 + 1] = {NAME = "PK1_NC", MODE = "_GPIO_MODE_IN", AF = "0", STATE = "_LOW"}
    PK1[#PK1 + 1] = {NAME = "LCD_G6", MODE = "_GPIO_MODE_IN", AF = "14", STATE = "_LOW"}
    PK1[#PK1 + 1] = {NAME = "EVENTOUT", MODE = "_GPIO_MODE_AF_PP", AF = "15", STATE = "_LOW"}

    this:AddWidget("Textbox", "PK1")
    this:AddProposals(PK1, "K", 1)
end
--*/
#define __GPIO_PK_PIN_1_NAME__ PK1_NC

/*--
if this:PinExist("K", 1) then
    this:AddWidget("Combobox")
    this:AddPinSpeed("K", 1)
end
--*/
#define __GPIO_PK_PIN_1_SPEED__ _GPIO_SPEED_LOW

/*--
if this:PinExist("K", 1) then
    this:AddWidget("Combobox")
    this:AddPinMode("K", 1)
end
--*/
#define __GPIO_PK_PIN_1_MODE__ _GPIO_MODE_IN

/*--
if this:PinExist("K", 1) then
    this:AddWidget("Combobox")
    this:AddPinAF("K", 1)
end
--*/
#define __GPIO_PK_PIN_1_AF__ 0

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
    PK2[#PK2 + 1] = {NAME = "PK2_NC", MODE = "_GPIO_MODE_IN", AF = "0", STATE = "_LOW"}
    PK2[#PK2 + 1] = {NAME = "LCD_G7", MODE = "_GPIO_MODE_IN", AF = "14", STATE = "_LOW"}
    PK2[#PK2 + 1] = {NAME = "EVENTOUT", MODE = "_GPIO_MODE_AF_PP", AF = "15", STATE = "_LOW"}

    this:AddWidget("Textbox", "PK2")
    this:AddProposals(PK2, "K", 2)
end
--*/
#define __GPIO_PK_PIN_2_NAME__ PK2_NC

/*--
if this:PinExist("K", 2) then
    this:AddWidget("Combobox")
    this:AddPinSpeed("K", 2)
end
--*/
#define __GPIO_PK_PIN_2_SPEED__ _GPIO_SPEED_LOW

/*--
if this:PinExist("K", 2) then
    this:AddWidget("Combobox")
    this:AddPinMode("K", 2)
end
--*/
#define __GPIO_PK_PIN_2_MODE__ _GPIO_MODE_IN

/*--
if this:PinExist("K", 2) then
    this:AddWidget("Combobox")
    this:AddPinAF("K", 2)
end
--*/
#define __GPIO_PK_PIN_2_AF__ 0

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
    PK3[#PK3 + 1] = {NAME = "PK3_NC", MODE = "_GPIO_MODE_IN", AF = "0", STATE = "_LOW"}
    PK3[#PK3 + 1] = {NAME = "LCD_B4", MODE = "_GPIO_MODE_AF_PP", AF = "14", STATE = "_LOW"}
    PK3[#PK3 + 1] = {NAME = "EVENTOUT", MODE = "_GPIO_MODE_AF_PP", AF = "15", STATE = "_LOW"}

    this:AddWidget("Textbox", "PK3")
    this:AddProposals(PK3, "K", 3)
end
--*/
#define __GPIO_PK_PIN_3_NAME__ PK3_NC

/*--
if this:PinExist("K", 3) then
    this:AddWidget("Combobox")
    this:AddPinSpeed("K", 3)
end
--*/
#define __GPIO_PK_PIN_3_SPEED__ _GPIO_SPEED_LOW

/*--
if this:PinExist("K", 3) then
    this:AddWidget("Combobox")
    this:AddPinMode("K", 3)
end
--*/
#define __GPIO_PK_PIN_3_MODE__ _GPIO_MODE_IN

/*--
if this:PinExist("K", 3) then
    this:AddWidget("Combobox")
    this:AddPinAF("K", 3)
end
--*/
#define __GPIO_PK_PIN_3_AF__ 0

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
    PK4[#PK4 + 1] = {NAME = "PK4_NC", MODE = "_GPIO_MODE_IN", AF = "0", STATE = "_LOW"}
    PK4[#PK4 + 1] = {NAME = "LCD_B5", MODE = "_GPIO_MODE_AF_PP", AF = "14", STATE = "_LOW"}
    PK4[#PK4 + 1] = {NAME = "EVENTOUT", MODE = "_GPIO_MODE_AF_PP", AF = "15", STATE = "_LOW"}

    this:AddWidget("Textbox", "PK4")
    this:AddProposals(PK4, "K", 4)
end
--*/
#define __GPIO_PK_PIN_4_NAME__ PK4_NC

/*--
if this:PinExist("K", 4) then
    this:AddWidget("Combobox")
    this:AddPinSpeed("K", 4)
end
--*/
#define __GPIO_PK_PIN_4_SPEED__ _GPIO_SPEED_LOW

/*--
if this:PinExist("K", 4) then
    this:AddWidget("Combobox")
    this:AddPinMode("K", 4)
end
--*/
#define __GPIO_PK_PIN_4_MODE__ _GPIO_MODE_IN

/*--
if this:PinExist("K", 4) then
    this:AddWidget("Combobox")
    this:AddPinAF("K", 4)
end
--*/
#define __GPIO_PK_PIN_4_AF__ 0

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
    PK5[#PK5 + 1] = {NAME = "PK5_NC", MODE = "_GPIO_MODE_IN", AF = "0", STATE = "_LOW"}
    PK5[#PK5 + 1] = {NAME = "LCD_B6", MODE = "_GPIO_MODE_AF_PP", AF = "14", STATE = "_LOW"}
    PK5[#PK5 + 1] = {NAME = "EVENTOUT", MODE = "_GPIO_MODE_AF_PP", AF = "15", STATE = "_LOW"}

    this:AddWidget("Textbox", "PK5")
    this:AddProposals(PK5, "K", 5)
end
--*/
#define __GPIO_PK_PIN_5_NAME__ PK5_NC

/*--
if this:PinExist("K", 5) then
    this:AddWidget("Combobox")
    this:AddPinSpeed("K", 5)
end
--*/
#define __GPIO_PK_PIN_5_SPEED__ _GPIO_SPEED_LOW

/*--
if this:PinExist("K", 5) then
    this:AddWidget("Combobox")
    this:AddPinMode("K", 5)
end
--*/
#define __GPIO_PK_PIN_5_MODE__ _GPIO_MODE_IN

/*--
if this:PinExist("K", 5) then
    this:AddWidget("Combobox")
    this:AddPinAF("K", 5)
end
--*/
#define __GPIO_PK_PIN_5_AF__ 0

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
    PK6[#PK6 + 1] = {NAME = "PK6_NC", MODE = "_GPIO_MODE_IN", AF = "0", STATE = "_LOW"}
    PK6[#PK6 + 1] = {NAME = "LCD_B7", MODE = "_GPIO_MODE_AF_PP", AF = "14", STATE = "_LOW"}
    PK6[#PK6 + 1] = {NAME = "EVENTOUT", MODE = "_GPIO_MODE_AF_PP", AF = "15", STATE = "_LOW"}

    this:AddWidget("Textbox", "PK6")
    this:AddProposals(PK6, "K", 6)
end
--*/
#define __GPIO_PK_PIN_6_NAME__ PK6_NC

/*--
if this:PinExist("K", 6) then
    this:AddWidget("Combobox")
    this:AddPinSpeed("K", 6)
end
--*/
#define __GPIO_PK_PIN_6_SPEED__ _GPIO_SPEED_LOW

/*--
if this:PinExist("K", 6) then
    this:AddWidget("Combobox")
    this:AddPinMode("K", 6)
end
--*/
#define __GPIO_PK_PIN_6_MODE__ _GPIO_MODE_IN

/*--
if this:PinExist("K", 6) then
    this:AddWidget("Combobox")
    this:AddPinAF("K", 6)
end
--*/
#define __GPIO_PK_PIN_6_AF__ 0

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
    PK7[#PK7 + 1] = {NAME = "PK7_NC", MODE = "_GPIO_MODE_IN", AF = "0", STATE = "_LOW"}
    PK7[#PK7 + 1] = {NAME = "LCD_DE", MODE = "_GPIO_MODE_AF_PP", AF = "14", STATE = "_LOW"}
    PK7[#PK7 + 1] = {NAME = "EVENTOUT", MODE = "_GPIO_MODE_AF_PP", AF = "15", STATE = "_LOW"}

    this:AddWidget("Textbox", "PK7")
    this:AddProposals(PK7, "K", 7)
end
--*/
#define __GPIO_PK_PIN_7_NAME__ PK7_NC

/*--
if this:PinExist("K", 7) then
    this:AddWidget("Combobox")
    this:AddPinSpeed("K", 7)
end
--*/
#define __GPIO_PK_PIN_7_SPEED__ _GPIO_SPEED_LOW

/*--
if this:PinExist("K", 7) then
    this:AddWidget("Combobox")
    this:AddPinMode("K", 7)
end
--*/
#define __GPIO_PK_PIN_7_MODE__ _GPIO_MODE_IN

/*--
if this:PinExist("K", 7) then
    this:AddWidget("Combobox")
    this:AddPinAF("K", 7)
end
--*/
#define __GPIO_PK_PIN_7_AF__ 0

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
    PK8[#PK8 + 1] = {NAME = "PK8_NC", MODE = "_GPIO_MODE_IN", AF = "0", STATE = "_LOW"}
    PK8[#PK8 + 1] = {NAME = "EVENTOUT", MODE = "_GPIO_MODE_AF_PP", AF = "15", STATE = "_LOW"}

    this:AddWidget("Textbox", "PK8")
    this:AddProposals(PK8, "K", 8)
end
--*/
#define __GPIO_PK_PIN_8_NAME__ PK8_NC

/*--
if this:PinExist("K", 8) then
    this:AddWidget("Combobox")
    this:AddPinSpeed("K", 8)
end
--*/
#define __GPIO_PK_PIN_8_SPEED__ _GPIO_SPEED_LOW

/*--
if this:PinExist("K", 8) then
    this:AddWidget("Combobox")
    this:AddPinMode("K", 8)
end
--*/
#define __GPIO_PK_PIN_8_MODE__ _GPIO_MODE_IN

/*--
if this:PinExist("K", 8) then
    this:AddWidget("Combobox")
    this:AddPinAF("K", 8)
end
--*/
#define __GPIO_PK_PIN_8_AF__ 0

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
    PK9[#PK9 + 1] = {NAME = "PK9_NC", MODE = "_GPIO_MODE_IN", AF = "0", STATE = "_LOW"}
    PK9[#PK9 + 1] = {NAME = "EVENTOUT", MODE = "_GPIO_MODE_AF_PP", AF = "15", STATE = "_LOW"}

    this:AddWidget("Textbox", "PK9")
    this:AddProposals(PK9, "K", 9)
end
--*/
#define __GPIO_PK_PIN_9_NAME__ PK9_NC

/*--
if this:PinExist("K", 9) then
    this:AddWidget("Combobox")
    this:AddPinSpeed("K", 9)
end
--*/
#define __GPIO_PK_PIN_9_SPEED__ _GPIO_SPEED_LOW

/*--
if this:PinExist("K", 9) then
    this:AddWidget("Combobox")
    this:AddPinMode("K", 9)
end
--*/
#define __GPIO_PK_PIN_9_MODE__ _GPIO_MODE_IN

/*--
if this:PinExist("K", 9) then
    this:AddWidget("Combobox")
    this:AddPinAF("K", 9)
end
--*/
#define __GPIO_PK_PIN_9_AF__ 0

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
    PK10[#PK10 + 1] = {NAME = "PK10_NC", MODE = "_GPIO_MODE_IN", AF = "0", STATE = "_LOW"}
    PK10[#PK10 + 1] = {NAME = "EVENTOUT", MODE = "_GPIO_MODE_AF_PP", AF = "15", STATE = "_LOW"}

    this:AddWidget("Textbox", "PK10")
    this:AddProposals(PK10, "K", 10)
end
--*/
#define __GPIO_PK_PIN_10_NAME__ PK10_NC

/*--
if this:PinExist("K", 10) then
    this:AddWidget("Combobox")
    this:AddPinSpeed("K", 10)
end
--*/
#define __GPIO_PK_PIN_10_SPEED__ _GPIO_SPEED_LOW

/*--
if this:PinExist("K", 10) then
    this:AddWidget("Combobox")
    this:AddPinMode("K", 10)
end
--*/
#define __GPIO_PK_PIN_10_MODE__ _GPIO_MODE_IN

/*--
if this:PinExist("K", 10) then
    this:AddWidget("Combobox")
    this:AddPinAF("K", 10)
end
--*/
#define __GPIO_PK_PIN_10_AF__ 0

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
    PK11[#PK11 + 1] = {NAME = "PK11_NC", MODE = "_GPIO_MODE_IN", AF = "0", STATE = "_LOW"}
    PK11[#PK11 + 1] = {NAME = "EVENTOUT", MODE = "_GPIO_MODE_AF_PP", AF = "15", STATE = "_LOW"}

    this:AddWidget("Textbox", "PK11")
    this:AddProposals(PK11, "K", 11)
end
--*/
#define __GPIO_PK_PIN_11_NAME__ PK11_NC

/*--
if this:PinExist("K", 11) then
    this:AddWidget("Combobox")
    this:AddPinSpeed("K", 11)
end
--*/
#define __GPIO_PK_PIN_11_SPEED__ _GPIO_SPEED_LOW

/*--
if this:PinExist("K", 11) then
    this:AddWidget("Combobox")
    this:AddPinMode("K", 11)
end
--*/
#define __GPIO_PK_PIN_11_MODE__ _GPIO_MODE_IN

/*--
if this:PinExist("K", 11) then
    this:AddWidget("Combobox")
    this:AddPinAF("K", 11)
end
--*/
#define __GPIO_PK_PIN_11_AF__ 0

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
    PK12[#PK12 + 1] = {NAME = "PK12_NC", MODE = "_GPIO_MODE_IN", AF = "0", STATE = "_LOW"}
    PK12[#PK12 + 1] = {NAME = "EVENTOUT", MODE = "_GPIO_MODE_AF_PP", AF = "15", STATE = "_LOW"}

    this:AddWidget("Textbox", "PK12")
    this:AddProposals(PK12, "K", 12)
end
--*/
#define __GPIO_PK_PIN_12_NAME__ PK12_NC

/*--
if this:PinExist("K", 12) then
    this:AddWidget("Combobox")
    this:AddPinSpeed("K", 12)
end
--*/
#define __GPIO_PK_PIN_12_SPEED__ _GPIO_SPEED_LOW

/*--
if this:PinExist("K", 12) then
    this:AddWidget("Combobox")
    this:AddPinMode("K", 12)
end
--*/
#define __GPIO_PK_PIN_12_MODE__ _GPIO_MODE_IN

/*--
if this:PinExist("K", 12) then
    this:AddWidget("Combobox")
    this:AddPinAF("K", 12)
end
--*/
#define __GPIO_PK_PIN_12_AF__ 0

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
    PK13[#PK13 + 1] = {NAME = "PK13_NC", MODE = "_GPIO_MODE_IN", AF = "0", STATE = "_LOW"}
    PK13[#PK13 + 1] = {NAME = "EVENTOUT", MODE = "_GPIO_MODE_AF_PP", AF = "15", STATE = "_LOW"}

    this:AddWidget("Textbox", "PK13")
    this:AddProposals(PK13, "K", 13)
end
--*/
#define __GPIO_PK_PIN_13_NAME__ PK13_NC

/*--
if this:PinExist("K", 13) then
    this:AddWidget("Combobox")
    this:AddPinSpeed("K", 13)
end
--*/
#define __GPIO_PK_PIN_13_SPEED__ _GPIO_SPEED_LOW

/*--
if this:PinExist("K", 13) then
    this:AddWidget("Combobox")
    this:AddPinMode("K", 13)
end
--*/
#define __GPIO_PK_PIN_13_MODE__ _GPIO_MODE_IN

/*--
if this:PinExist("K", 13) then
    this:AddWidget("Combobox")
    this:AddPinAF("K", 13)
end
--*/
#define __GPIO_PK_PIN_13_AF__ 0

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
    PK14[#PK14 + 1] = {NAME = "PK14_NC", MODE = "_GPIO_MODE_IN", AF = "0", STATE = "_LOW"}
    PK14[#PK14 + 1] = {NAME = "EVENTOUT", MODE = "_GPIO_MODE_AF_PP", AF = "15", STATE = "_LOW"}

    this:AddWidget("Textbox", "PK14")
    this:AddProposals(PK14, "K", 14)
end
--*/
#define __GPIO_PK_PIN_14_NAME__ PK14_NC

/*--
if this:PinExist("K", 14) then
    this:AddWidget("Combobox")
    this:AddPinSpeed("K", 14)
end
--*/
#define __GPIO_PK_PIN_14_SPEED__ _GPIO_SPEED_LOW

/*--
if this:PinExist("K", 14) then
    this:AddWidget("Combobox")
    this:AddPinMode("K", 14)
end
--*/
#define __GPIO_PK_PIN_14_MODE__ _GPIO_MODE_IN

/*--
if this:PinExist("K", 14) then
    this:AddWidget("Combobox")
    this:AddPinAF("K", 14)
end
--*/
#define __GPIO_PK_PIN_14_AF__ 0

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
    PK15[#PK15 + 1] = {NAME = "PK15_NC", MODE = "_GPIO_MODE_IN", AF = "0", STATE = "_LOW"}
    PK15[#PK15 + 1] = {NAME = "EVENTOUT", MODE = "_GPIO_MODE_AF_PP", AF = "15", STATE = "_LOW"}

    this:AddWidget("Textbox", "PK15")
    this:AddProposals(PK15, "K", 15)
end
--*/
#define __GPIO_PK_PIN_15_NAME__ PK15_NC

/*--
if this:PinExist("K", 15) then
    this:AddWidget("Combobox")
    this:AddPinSpeed("K", 15)
end
--*/
#define __GPIO_PK_PIN_15_SPEED__ _GPIO_SPEED_LOW

/*--
if this:PinExist("K", 15) then
    this:AddWidget("Combobox")
    this:AddPinMode("K", 15)
end
--*/
#define __GPIO_PK_PIN_15_MODE__ _GPIO_MODE_IN

/*--
if this:PinExist("K", 15) then
    this:AddWidget("Combobox")
    this:AddPinAF("K", 15)
end
--*/
#define __GPIO_PK_PIN_15_AF__ 0

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
