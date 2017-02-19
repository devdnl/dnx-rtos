/*=========================================================================*//**
@file    gpio_flags.h

@author  Daniel Zorychta

@brief   GPIO module configuration flags.

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

#ifndef _GPIO_FLAGS_H_
#define _GPIO_FLAGS_H_

/*--
local function Configure(gpio) _GPIO_SELECTION_ = gpio this:Reload() end

local GPIO_OF            = {}
GPIO_OF["STM32F100C8xx"] = {A = 0xFFFF, B = 0xFFFF, C = 0xE000, D = 0x0003}
GPIO_OF["STM32F100RBxx"] = {A = 0xFFFF, B = 0xFFFF, C = 0xFFFF, D = 0x0007}
GPIO_OF["STM32F100RExx"] = {A = 0xFFFF, B = 0xFFFF, C = 0xFFFF, D = 0x0007}
GPIO_OF["STM32F100VBxx"] = {A = 0xFFFF, B = 0xFFFF, C = 0xFFFF, D = 0xFFFF, E = 0xFFFF}
GPIO_OF["STM32F100ZCxx"] = {A = 0xFFFF, B = 0xFFFF, C = 0xFFFF, D = 0xFFFF, E = 0xFFFF, F = 0xFFFF, G = 0xFFFF}
GPIO_OF["STM32F101C8xx"] = {A = 0xFFFF, B = 0xFFFF, C = 0xE000, D = 0x0003}
GPIO_OF["STM32F101CBxx"] = {A = 0xFFFF, B = 0xFFFF, C = 0xE000, D = 0x0003}
GPIO_OF["STM32F101R8xx"] = {A = 0xFFFF, B = 0xFFFF, C = 0xFFFF, D = 0x0007}
GPIO_OF["STM32F101RBxx"] = {A = 0xFFFF, B = 0xFFFF, C = 0xFFFF, D = 0x0007}
GPIO_OF["STM32F101V8xx"] = {A = 0xFFFF, B = 0xFFFF, C = 0xFFFF, D = 0xFFFF, E = 0xFFFF}
GPIO_OF["STM32F101VBxx"] = {A = 0xFFFF, B = 0xFFFF, C = 0xFFFF, D = 0xFFFF, E = 0xFFFF}
GPIO_OF["STM32F103C6xx"] = {A = 0xFFFF, B = 0xFFFF, C = 0xE000, D = 0x0003}
GPIO_OF["STM32F103C8xx"] = {A = 0xFFFF, B = 0xFFFF, C = 0xE000, D = 0x0003}
GPIO_OF["STM32F103CBxx"] = {A = 0xFFFF, B = 0xFFFF, C = 0xE000, D = 0x0003}
GPIO_OF["STM32F103R6xx"] = {A = 0xFFFF, B = 0xFFFF, C = 0xFFFF, D = 0x0007}
GPIO_OF["STM32F103R8xx"] = {A = 0xFFFF, B = 0xFFFF, C = 0xFFFF, D = 0x0007}
GPIO_OF["STM32F103RBxx"] = {A = 0xFFFF, B = 0xFFFF, C = 0xFFFF, D = 0x0007}
GPIO_OF["STM32F103RCxx"] = {A = 0xFFFF, B = 0xFFFF, C = 0xFFFF, D = 0x0007}
GPIO_OF["STM32F103RDxx"] = {A = 0xFFFF, B = 0xFFFF, C = 0xFFFF, D = 0x0007}
GPIO_OF["STM32F103RExx"] = {A = 0xFFFF, B = 0xFFFF, C = 0xFFFF, D = 0x0007}
GPIO_OF["STM32F103RGxx"] = {A = 0xFFFF, B = 0xFFFF, C = 0xFFFF, D = 0x0007}
GPIO_OF["STM32F103T8xx"] = {A = 0xFFFF, B = 0x00FF, D = 0x0003}
GPIO_OF["STM32F103TBxx"] = {A = 0xFFFF, B = 0x00FF, D = 0x0003}
GPIO_OF["STM32F103V8xx"] = {A = 0xFFFF, B = 0xFFFF, C = 0xFFFF, D = 0xFFFF, E = 0xFFFF}
GPIO_OF["STM32F103VBxx"] = {A = 0xFFFF, B = 0xFFFF, C = 0xFFFF, D = 0xFFFF, E = 0xFFFF}
GPIO_OF["STM32F103VCxx"] = {A = 0xFFFF, B = 0xFFFF, C = 0xFFFF, D = 0xFFFF, E = 0xFFFF}
GPIO_OF["STM32F103VDxx"] = {A = 0xFFFF, B = 0xFFFF, C = 0xFFFF, D = 0xFFFF, E = 0xFFFF}
GPIO_OF["STM32F103VExx"] = {A = 0xFFFF, B = 0xFFFF, C = 0xFFFF, D = 0xFFFF, E = 0xFFFF}
GPIO_OF["STM32F103VGxx"] = {A = 0xFFFF, B = 0xFFFF, C = 0xFFFF, D = 0xFFFF, E = 0xFFFF}
GPIO_OF["STM32F103ZCxx"] = {A = 0xFFFF, B = 0xFFFF, C = 0xFFFF, D = 0xFFFF, E = 0xFFFF, F = 0xFFFF, G = 0xFFFF}
GPIO_OF["STM32F103ZExx"] = {A = 0xFFFF, B = 0xFFFF, C = 0xFFFF, D = 0xFFFF, E = 0xFFFF, F = 0xFFFF, G = 0xFFFF}
GPIO_OF["STM32F103ZGxx"] = {A = 0xFFFF, B = 0xFFFF, C = 0xFFFF, D = 0xFFFF, E = 0xFFFF, F = 0xFFFF, G = 0xFFFF}
GPIO_OF["STM32F105RBxx"] = {A = 0xFFFF, B = 0xFFFF, C = 0xFFFF, D = 0x0007}
GPIO_OF["STM32F105RCxx"] = {A = 0xFFFF, B = 0xFFFF, C = 0xFFFF, D = 0x0007}
GPIO_OF["STM32F105VBxx"] = {A = 0xFFFF, B = 0xFFFF, C = 0xFFFF, D = 0xFFFF, E = 0xFFFF}
GPIO_OF["STM32F105VCxx"] = {A = 0xFFFF, B = 0xFFFF, C = 0xFFFF, D = 0xFFFF, E = 0xFFFF}
GPIO_OF["STM32F107RCxx"] = {A = 0xFFFF, B = 0xFFFF, C = 0xFFFF, D = 0x0007}
GPIO_OF["STM32F107VBxx"] = {A = 0xFFFF, B = 0xFFFF, C = 0xFFFF, D = 0xFFFF, E = 0xFFFF}
GPIO_OF["STM32F107VCxx"] = {A = 0xFFFF, B = 0xFFFF, C = 0xFFFF, D = 0xFFFF, E = 0xFFFF}

if _GPIO_SELECTION_ == nil then
    this:SetLayout("TitledGridBack", 2, "Home > Microcontroller > GPIO",
                   function() this:LoadFile("arch/arch_flags.h") end)

    this:AddExtraWidget("Label", "LabelTitle", "Available ports", -1, "bold")
    this:AddExtraWidget("Void", "VoidTitle")

    if GPIO_OF[uC.NAME].A ~= nil then
        this:AddExtraWidget("Label", "Label_GPIOA", "GPIOA")
        this:AddExtraWidget("Hyperlink", "HL_GPIOA", "Configure")
        this:SetEvent("clicked", "HL_GPIOA", function() Configure("GPIOA") end)
    end

    if GPIO_OF[uC.NAME].B ~= nil then
        this:AddExtraWidget("Label", "Label_GPIOB", "GPIOB")
        this:AddExtraWidget("Hyperlink", "HL_GPIOB", "Configure")
        this:SetEvent("clicked", "HL_GPIOB", function() Configure("GPIOB") end)
    end

    if GPIO_OF[uC.NAME].C ~= nil then
        this:AddExtraWidget("Label", "Label_GPIOC", "GPIOC")
        this:AddExtraWidget("Hyperlink", "HL_GPIOC", "Configure")
        this:SetEvent("clicked", "HL_GPIOC", function() Configure("GPIOC") end)
    end

    if GPIO_OF[uC.NAME].D ~= nil then
        this:AddExtraWidget("Label", "Label_GPIOD", "GPIOD")
        this:AddExtraWidget("Hyperlink", "HL_GPIOD", "Configure")
        this:SetEvent("clicked", "HL_GPIOD", function() Configure("GPIOD") end)
    end

    if GPIO_OF[uC.NAME].E ~= nil then
        this:AddExtraWidget("Label", "Label_GPIOE", "GPIOE")
        this:AddExtraWidget("Hyperlink", "HL_GPIOE", "Configure")
        this:SetEvent("clicked", "HL_GPIOE", function() Configure("GPIOE") end)
    end

    if GPIO_OF[uC.NAME].F ~= nil then
        this:AddExtraWidget("Label", "Label_GPIOF", "GPIOF")
        this:AddExtraWidget("Hyperlink", "HL_GPIOF", "Configure")
        this:SetEvent("clicked", "HL_GPIOF", function() Configure("GPIOF") end)
    end

    if GPIO_OF[uC.NAME].G ~= nil then
        this:AddExtraWidget("Label", "Label_GPIOG", "GPIOG")
        this:AddExtraWidget("Hyperlink", "HL_GPIOG", "Configure")
        this:SetEvent("clicked", "HL_GPIOG", function() Configure("GPIOG") end)
    end

    this:Finish()
else
    this:SetLayout("TitledGridBack", 4, "Home > Microcontroller > ".._GPIO_SELECTION_,
                   function() Configure(nil) end)

    this:AddExtraWidget("Label", "LabelPin", "Pin")
    this:AddExtraWidget("Label", "LabelName", "Name")
    this:AddExtraWidget("Label", "LabelMode", "Mode")
    this:AddExtraWidget("Label", "LabelState", "State")

    this.AddPinMode = function(this, portLetter, pinNumber)
        this:AddItem("Output Push-Pull 2MHz",               "_GPIO_OUT_PUSH_PULL_2MHZ")
        this:AddItem("Output Push-Pull 10MHz",              "_GPIO_OUT_PUSH_PULL_10MHZ")
        this:AddItem("Output Push-Pull 50MHz",              "_GPIO_OUT_PUSH_PULL_50MHZ")
        this:AddItem("Output Open drain 2MHz",              "_GPIO_OUT_OPEN_DRAIN_2MHZ")
        this:AddItem("Output Open drain 10MHz",             "_GPIO_OUT_OPEN_DRAIN_10MHZ")
        this:AddItem("Output Open drain 50MHz",             "_GPIO_OUT_OPEN_DRAIN_50MHZ")
        this:AddItem("Alternative output Push-Pull 2MHz",   "_GPIO_ALT_OUT_PUSH_PULL_2MHZ")
        this:AddItem("Alternative output Push-Pull 10MHz",  "_GPIO_ALT_OUT_PUSH_PULL_10MHZ")
        this:AddItem("Alternative output Push-Pull 50MHz",  "_GPIO_ALT_OUT_PUSH_PULL_50MHZ")
        this:AddItem("Alternative output Open drain 2MHz",  "_GPIO_ALT_OUT_OPEN_DRAIN_2MHZ")
        this:AddItem("Alternative output Open drain 10MHz", "_GPIO_ALT_OUT_OPEN_DRAIN_10MHZ")
        this:AddItem("Alternative output Open drain 50MHz", "_GPIO_ALT_OUT_OPEN_DRAIN_50MHZ")
        this:AddItem("Analog",                              "_GPIO_ANALOG")
        this:AddItem("Float input",                         "_GPIO_IN_FLOAT")
        this:AddItem("Input pulled",                        "_GPIO_IN_PULLED")

        this:SetEvent("clicked", function()
            local mode = this:GetFlagValue("__GPIO_P"..portLetter.."_PIN_"..pinNumber.."_MODE__")
            if mode == "_GPIO_ANALOG" or mode == "_GPIO_IN_FLOAT" then
                    this:Enable(false, "__GPIO_P"..portLetter.."_PIN_"..pinNumber.."_STATE__")
            else
                    this:Enable(true, "__GPIO_P"..portLetter.."_PIN_"..pinNumber.."_STATE__")
            end
        end)
    end

    this.AddPinState = function(this, portLetter, pinNumber)
        this:AddItem("Low", "_LOW")
        this:AddItem("High", "_HIGH")

        local mode = this:GetFlagValue("__GPIO_P"..portLetter.."_PIN_"..pinNumber.."_MODE__")
        if mode == "_GPIO_ANALOG" or mode == "_GPIO_IN_FLOAT" then
                this:Enable(false)
        else
                this:Enable(true)
        end
    end

    this.PinExist = function(this, portLetter, pinNumber)
        if _GPIO_SELECTION_ ~= "GPIO"..portLetter then
            return false
        end

        if GPIO_OF[uC.NAME][portLetter] ~= nil then
            return bit32.extract(GPIO_OF[uC.NAME][portLetter], pinNumber) > 0
        else
            return false
        end
    end

    this.AddProposals = function(this, tab, portLetter, pinNumber)
        for i = 1, #tab do this:AddItem(tab[i].NAME, "") end
        this:SetEvent("clicked",
            function()
                local nameFlag  = "__GPIO_P"..portLetter.."_PIN_"..pinNumber.."_NAME__"
                local modeFlag  = "__GPIO_P"..portLetter.."_PIN_"..pinNumber.."_MODE__"
                local statFlag  = "__GPIO_P"..portLetter.."_PIN_"..pinNumber.."_STATE__"
                local selection = this:GetFlagValue(nameFlag)

                for i = 1, #tab do
                    if tab[i].NAME == selection then
                        this:SetFlagValue(modeFlag, tab[i].MODE)
                        this:SetFlagValue(statFlag, tab[i].STATE)

                        if tab[i].MODE == "_GPIO_ANALOG" or tab[i].MODE == "_GPIO_IN_FLOAT" then
                            this:Enable(false, statFlag)
                        else
                            this:Enable(true, statFlag)
                        end
                    end
                end
            end
        )
    end
end
++*/


/*------------------------------------------------------------------------------
if this:PinExist("A", 0) then
    local PA0 = {}
    PA0[#PA0 + 1] = {NAME = "PA0_NC", MODE = "_GPIO_ANALOG", STATE = "_LOW"}
    PA0[#PA0 + 1] = {NAME = "ADC12_IN0", MODE = "_GPIO_ANALOG", STATE = "_LOW"}
    PA0[#PA0 + 1] = {NAME = "ADC123_IN0", MODE = "_GPIO_ANALOG", STATE = "_LOW"}
    PA0[#PA0 + 1] = {NAME = "ADC_IN0", MODE = "_GPIO_ANALOG", STATE = "_LOW"}
    PA0[#PA0 + 1] = {NAME = "ETH_MII_CRS_WKUP", MODE = "_GPIO_IN_FLOAT", STATE = "_LOW"}
    PA0[#PA0 + 1] = {NAME = "TIM2_CH1_ETR", MODE = "_GPIO_IN_FLOAT", STATE = "_LOW"}
    PA0[#PA0 + 1] = {NAME = "TIM5_CH1_IC", MODE = "_GPIO_IN_FLOAT", STATE = "_LOW"}
    PA0[#PA0 + 1] = {NAME = "TIM5_CH1_OC", MODE = "_GPIO_ALT_OUT_PUSH_PULL_10MHZ", STATE = "_LOW"}
    PA0[#PA0 + 1] = {NAME = "TIM8_ETR", MODE = "_GPIO_IN_FLOAT", STATE = "_LOW"}
    PA0[#PA0 + 1] = {NAME = "USART2_CTS", MODE = "_GPIO_IN_PULLED", STATE = "_HIGH"}
    PA0[#PA0 + 1] = {NAME = "WKUP", MODE = "_GPIO_IN_PULLED", STATE = "_HIGH"}

    this:AddWidget("Textbox", "PA0")
    this:AddProposals(PA0, "A", 0)
end
--*/
#define __GPIO_PA_PIN_0_NAME__ PA0_NC

/*--
if this:PinExist("A", 0) then
    this:AddWidget("Combobox")
    this:AddPinMode("A", 0)
end
--*/
#define __GPIO_PA_PIN_0_MODE__ _GPIO_ANALOG

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
    PA1[#PA1 + 1] = {NAME = "PA1_NC", MODE = "_GPIO_ANALOG", STATE = "_LOW"}
    PA1[#PA1 + 1] = {NAME = "ADC12_IN1", MODE = "_GPIO_ANALOG", STATE = "_LOW"}
    PA1[#PA1 + 1] = {NAME = "ADC123_IN1", MODE = "_GPIO_ANALOG", STATE = "_LOW"}
    PA1[#PA1 + 1] = {NAME = "ADC_IN1", MODE = "_GPIO_ANALOG", STATE = "_LOW"}
    PA1[#PA1 + 1] = {NAME = "ETH_MII_RX_CLK", MODE = "_GPIO_IN_FLOAT", STATE = "_LOW"}
    PA1[#PA1 + 1] = {NAME = "ETH_RMII_REF_CLK", MODE = "_GPIO_IN_FLOAT", STATE = "_LOW"}
    PA1[#PA1 + 1] = {NAME = "TIM2_CH2_IC", MODE = "_GPIO_IN_FLOAT", STATE = "_LOW"}
    PA1[#PA1 + 1] = {NAME = "TIM2_CH2_OC", MODE = "_GPIO_ALT_OUT_PUSH_PULL_10MHZ", STATE = "_LOW"}
    PA1[#PA1 + 1] = {NAME = "TIM5_CH2_IC", MODE = "_GPIO_IN_FLOAT", STATE = "_LOW"}
    PA1[#PA1 + 1] = {NAME = "TIM5_CH2_OC", MODE = "_GPIO_ALT_OUT_PUSH_PULL_10MHZ", STATE = "_LOW"}
    PA1[#PA1 + 1] = {NAME = "USART2_RTS", MODE = "_GPIO_ALT_OUT_PUSH_PULL_10MHZ", STATE = "_LOW"}

    this:AddWidget("Textbox", "PA1")
    this:AddProposals(PA1, "A", 1)
end
--*/
#define __GPIO_PA_PIN_1_NAME__ PA1_NC

/*--
if this:PinExist("A", 1) then
    this:AddWidget("Combobox")
    this:AddPinMode("A", 1)
end
--*/
#define __GPIO_PA_PIN_1_MODE__ _GPIO_ANALOG

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
    PA2[#PA2 + 1] = {NAME = "PA2_NC", MODE = "_GPIO_ANALOG", STATE = "_LOW"}
    PA2[#PA2 + 1] = {NAME = "ADC12_IN2", MODE = "_GPIO_ANALOG", STATE = "_LOW"}
    PA2[#PA2 + 1] = {NAME = "ADC123_IN2", MODE = "_GPIO_ANALOG", STATE = "_LOW"}
    PA2[#PA2 + 1] = {NAME = "ADC_IN2", MODE = "_GPIO_ANALOG", STATE = "_LOW"}
    PA2[#PA2 + 1] = {NAME = "ETH_MII_MDIO", MODE = "_GPIO_ALT_OUT_PUSH_PULL_50MHZ", STATE = "_LOW"}
    PA2[#PA2 + 1] = {NAME = "ETH_RMII_MDIO", MODE = "_GPIO_ALT_OUT_PUSH_PULL_50MHZ", STATE = "_LOW"}
    PA2[#PA2 + 1] = {NAME = "TIM2_CH3_IC", MODE = "_GPIO_IN_FLOAT", STATE = "_LOW"}
    PA2[#PA2 + 1] = {NAME = "TIM2_CH3_OC", MODE = "_GPIO_ALT_OUT_PUSH_PULL_10MHZ", STATE = "_LOW"}
    PA2[#PA2 + 1] = {NAME = "TIM5_CH3_IC", MODE = "_GPIO_IN_FLOAT", STATE = "_LOW"}
    PA2[#PA2 + 1] = {NAME = "TIM5_CH3_OC", MODE = "_GPIO_ALT_OUT_PUSH_PULL_10MHZ", STATE = "_LOW"}
    PA2[#PA2 + 1] = {NAME = "TIM9_CH1_IC", MODE = "_GPIO_IN_FLOAT", STATE = "_LOW"}
    PA2[#PA2 + 1] = {NAME = "TIM9_CH1_OC", MODE = "_GPIO_ALT_OUT_PUSH_PULL_10MHZ", STATE = "_LOW"}
    PA2[#PA2 + 1] = {NAME = "TIM15_CH1_IC", MODE = "_GPIO_IN_FLOAT", STATE = "_LOW"}
    PA2[#PA2 + 1] = {NAME = "TIM15_CH1_OC", MODE = "_GPIO_ALT_OUT_PUSH_PULL_10MHZ", STATE = "_LOW"}
    PA2[#PA2 + 1] = {NAME = "USART2_TX", MODE = "_GPIO_ALT_OUT_PUSH_PULL_10MHZ", STATE = "_HIGH"}

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
#define __GPIO_PA_PIN_2_MODE__ _GPIO_ANALOG

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
    PA3[#PA3 + 1] = {NAME = "PA3_NC", MODE = "_GPIO_ANALOG", STATE = "_LOW"}
    PA3[#PA3 + 1] = {NAME = "ADC12_IN3", MODE = "_GPIO_ANALOG", STATE = "_LOW"}
    PA3[#PA3 + 1] = {NAME = "ADC123_IN3", MODE = "_GPIO_ANALOG", STATE = "_LOW"}
    PA3[#PA3 + 1] = {NAME = "ADC_IN3", MODE = "_GPIO_ANALOG", STATE = "_LOW"}
    PA3[#PA3 + 1] = {NAME = "ETH_MII_COL", MODE = "_GPIO_IN_FLOAT", STATE = "_LOW"}
    PA3[#PA3 + 1] = {NAME = "TIM2_CH4_IC", MODE = "_GPIO_IN_FLOAT", STATE = "_LOW"}
    PA3[#PA3 + 1] = {NAME = "TIM2_CH4_OC", MODE = "_GPIO_ALT_OUT_PUSH_PULL_10MHZ", STATE = "_LOW"}
    PA3[#PA3 + 1] = {NAME = "TIM5_CH4_IC", MODE = "_GPIO_IN_FLOAT", STATE = "_LOW"}
    PA3[#PA3 + 1] = {NAME = "TIM5_CH4_OC", MODE = "_GPIO_ALT_OUT_PUSH_PULL_10MHZ", STATE = "_LOW"}
    PA3[#PA3 + 1] = {NAME = "TIM9_CH4_IC", MODE = "_GPIO_IN_FLOAT", STATE = "_LOW"}
    PA3[#PA3 + 1] = {NAME = "TIM9_CH4_OC", MODE = "_GPIO_ALT_OUT_PUSH_PULL_10MHZ", STATE = "_LOW"}
    PA3[#PA3 + 1] = {NAME = "TIM15_CH2_IC", MODE = "_GPIO_IN_FLOAT", STATE = "_LOW"}
    PA3[#PA3 + 1] = {NAME = "TIM15_CH2_OC", MODE = "_GPIO_ALT_OUT_PUSH_PULL_10MHZ", STATE = "_LOW"}
    PA3[#PA3 + 1] = {NAME = "USART2_RX", MODE = "_GPIO_IN_PULLED", STATE = "_HIGH"}

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
#define __GPIO_PA_PIN_3_MODE__ _GPIO_ANALOG

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
    PA4[#PA4 + 1] = {NAME = "PA4_NC", MODE = "_GPIO_ANALOG", STATE = "_LOW"}
    PA4[#PA4 + 1] = {NAME = "ADC12_IN4", MODE = "_GPIO_ANALOG", STATE = "_LOW"}
    PA4[#PA4 + 1] = {NAME = "ADC_IN4", MODE = "_GPIO_ANALOG", STATE = "_LOW"}
    PA4[#PA4 + 1] = {NAME = "DAC_OUT1", MODE = "_GPIO_ANALOG", STATE = "_LOW"}
    PA4[#PA4 + 1] = {NAME = "I2S3_WS_M", MODE = "_GPIO_ALT_OUT_PUSH_PULL_50MHZ", STATE = "_LOW"}
    PA4[#PA4 + 1] = {NAME = "I2S3_WS_S", MODE = "_GPIO_IN_FLOAT", STATE = "_LOW"}
    PA4[#PA4 + 1] = {NAME = "SPI1_NSS", MODE = "_GPIO_OUT_PUSH_PULL_2MHZ", STATE = "_HIGH"}
    PA4[#PA4 + 1] = {NAME = "SPI3_NSS", MODE = "_GPIO_OUT_PUSH_PULL_2MHZ", STATE = "_HIGH"}
    PA4[#PA4 + 1] = {NAME = "USART2_CK", MODE = "_GPIO_ALT_OUT_PUSH_PULL_10MHZ", STATE = "_HIGH"}

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
#define __GPIO_PA_PIN_4_MODE__ _GPIO_ANALOG

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
    PA5[#PA5 + 1] = {NAME = "PA5_NC", MODE = "_GPIO_ANALOG", STATE = "_LOW"}
    PA5[#PA5 + 1] = {NAME = "ADC12_IN5", MODE = "_GPIO_ANALOG", STATE = "_LOW"}
    PA5[#PA5 + 1] = {NAME = "ADC_IN5", MODE = "_GPIO_ANALOG", STATE = "_LOW"}
    PA5[#PA5 + 1] = {NAME = "DAC_OUT2", MODE = "_GPIO_ANALOG", STATE = "_LOW"}
    PA5[#PA5 + 1] = {NAME = "SPI1_SCK", MODE = "_GPIO_ALT_OUT_PUSH_PULL_50MHZ", STATE = "_LOW"}

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
#define __GPIO_PA_PIN_5_MODE__ _GPIO_ANALOG

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
    PA6[#PA6 + 1] = {NAME = "PA6_NC", MODE = "_GPIO_ANALOG", STATE = "_LOW"}
    PA6[#PA6 + 1] = {NAME = "ADC12_IN6", MODE = "_GPIO_ANALOG", STATE = "_LOW"}
    PA6[#PA6 + 1] = {NAME = "ADC_IN6", MODE = "_GPIO_ANALOG", STATE = "_LOW"}
    PA6[#PA6 + 1] = {NAME = "SPI1_MISO", MODE = "_GPIO_IN_PULLED", STATE = "_LOW"}
    PA6[#PA6 + 1] = {NAME = "TIM1_BKIN", MODE = "_GPIO_IN_FLOAT", STATE = "_LOW"}
    PA6[#PA6 + 1] = {NAME = "TIM1_CH1_IC", MODE = "_GPIO_IN_FLOAT", STATE = "_LOW"}
    PA6[#PA6 + 1] = {NAME = "TIM1_CH1_OC", MODE = "_GPIO_ALT_OUT_PUSH_PULL_10MHZ", STATE = "_LOW"}
    PA6[#PA6 + 1] = {NAME = "TIM3_CH1_IC", MODE = "_GPIO_IN_FLOAT", STATE = "_LOW"}
    PA6[#PA6 + 1] = {NAME = "TIM3_CH1_OC", MODE = "_GPIO_ALT_OUT_PUSH_PULL_10MHZ", STATE = "_LOW"}
    PA6[#PA6 + 1] = {NAME = "TIM8_BKIN", MODE = "_GPIO_IN_FLOAT", STATE = "_LOW"}
    PA6[#PA6 + 1] = {NAME = "TIM13_CH1_IC", MODE = "_GPIO_IN_FLOAT", STATE = "_LOW"}
    PA6[#PA6 + 1] = {NAME = "TIM13_CH1_OC", MODE = "_GPIO_ALT_OUT_PUSH_PULL_10MHZ", STATE = "_LOW"}
    PA6[#PA6 + 1] = {NAME = "TIM16_CH1_IC", MODE = "_GPIO_IN_FLOAT", STATE = "_LOW"}
    PA6[#PA6 + 1] = {NAME = "TIM16_CH1_OC", MODE = "_GPIO_ALT_OUT_PUSH_PULL_10MHZ", STATE = "_LOW"}

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
#define __GPIO_PA_PIN_6_MODE__ _GPIO_ANALOG

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
    PA7[#PA7 + 1] = {NAME = "PA7_NC", MODE = "_GPIO_ANALOG", STATE = "_LOW"}
    PA7[#PA7 + 1] = {NAME = "ADC12_IN7", MODE = "_GPIO_ANALOG", STATE = "_LOW"}
    PA7[#PA7 + 1] = {NAME = "ADC_IN7", MODE = "_GPIO_ANALOG", STATE = "_LOW"}
    PA7[#PA7 + 1] = {NAME = "ETH_MII_RX_DV", MODE = "_GPIO_IN_FLOAT", STATE = "_LOW"}
    PA7[#PA7 + 1] = {NAME = "ETH_RMII_CRS_DV", MODE = "_GPIO_IN_FLOAT", STATE = "_LOW"}
    PA7[#PA7 + 1] = {NAME = "SPI1_MOSI", MODE = "_GPIO_ALT_OUT_PUSH_PULL_50MHZ", STATE = "_LOW"}
    PA7[#PA7 + 1] = {NAME = "TIM1_CH1N", MODE = "_GPIO_ALT_OUT_PUSH_PULL_10MHZ", STATE = "_LOW"}
    PA7[#PA7 + 1] = {NAME = "TIM1_CH1_IC", MODE = "_GPIO_IN_FLOAT", STATE = "_LOW"}
    PA7[#PA7 + 1] = {NAME = "TIM1_CH1_OC", MODE = "_GPIO_ALT_OUT_PUSH_PULL_10MHZ", STATE = "_LOW"}
    PA7[#PA7 + 1] = {NAME = "TIM3_CH2_IC", MODE = "_GPIO_IN_FLOAT", STATE = "_LOW"}
    PA7[#PA7 + 1] = {NAME = "TIM3_CH2_OC", MODE = "_GPIO_ALT_OUT_PUSH_PULL_10MHZ", STATE = "_LOW"}
    PA7[#PA7 + 1] = {NAME = "TIM8_CH1N", MODE = "_GPIO_ALT_OUT_PUSH_PULL_10MHZ", STATE = "_LOW"}
    PA7[#PA7 + 1] = {NAME = "TIM14_CH1_IC", MODE = "_GPIO_IN_FLOAT", STATE = "_LOW"}
    PA7[#PA7 + 1] = {NAME = "TIM14_CH1_OC", MODE = "_GPIO_ALT_OUT_PUSH_PULL_10MHZ", STATE = "_LOW"}
    PA7[#PA7 + 1] = {NAME = "TIM17_CH1_IC", MODE = "_GPIO_IN_FLOAT", STATE = "_LOW"}
    PA7[#PA7 + 1] = {NAME = "TIM17_CH1_OC", MODE = "_GPIO_ALT_OUT_PUSH_PULL_10MHZ", STATE = "_LOW"}

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
#define __GPIO_PA_PIN_7_MODE__ _GPIO_ANALOG

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
    PA8[#PA8 + 1] = {NAME = "PA8_NC", MODE = "_GPIO_ANALOG", STATE = "_LOW"}
    PA8[#PA8 + 1] = {NAME = "MCO", MODE = "_GPIO_ALT_OUT_PUSH_PULL_50MHZ", STATE = "_LOW"}
    PA8[#PA8 + 1] = {NAME = "OTG_FS_SOF", MODE = "_GPIO_ALT_OUT_PUSH_PULL_50MHZ", STATE = "_LOW"}
    PA8[#PA8 + 1] = {NAME = "TIM1_CH1_IC", MODE = "_GPIO_IN_FLOAT", STATE = "_LOW"}
    PA8[#PA8 + 1] = {NAME = "TIM1_CH1_OC", MODE = "_GPIO_ALT_OUT_PUSH_PULL_10MHZ", STATE = "_LOW"}
    PA8[#PA8 + 1] = {NAME = "USART1_CK", MODE = "_GPIO_ALT_OUT_PUSH_PULL_10MHZ", STATE = "_HIGH"}

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
#define __GPIO_PA_PIN_8_MODE__ _GPIO_ANALOG

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
    PA9[#PA9 + 1] = {NAME = "PA9_NC", MODE = "_GPIO_ANALOG", STATE = "_LOW"}
    PA9[#PA9 + 1] = {NAME = "OTG_FS_VBUS", MODE = "_GPIO_IN_FLOAT", STATE = "_LOW"}
    PA9[#PA9 + 1] = {NAME = "TIM1_CH2_IC", MODE = "_GPIO_IN_FLOAT", STATE = "_LOW"}
    PA9[#PA9 + 1] = {NAME = "TIM1_CH2_OC", MODE = "_GPIO_ALT_OUT_PUSH_PULL_10MHZ", STATE = "_LOW"}
    PA9[#PA9 + 1] = {NAME = "TIM15_BKIN", MODE = "_GPIO_IN_FLOAT", STATE = "_LOW"}
    PA9[#PA9 + 1] = {NAME = "USART1_TX", MODE = "_GPIO_ALT_OUT_PUSH_PULL_10MHZ", STATE = "_HIGH"}

    this:AddWidget("Textbox", "PA9")
    this:AddProposals(PA9, "A", 9)
end
--*/
#define __GPIO_PA_PIN_9_NAME__ USART1_TX

/*--
if this:PinExist("A", 9) then
    this:AddWidget("Combobox")
    this:AddPinMode("A", 9)
end
--*/
#define __GPIO_PA_PIN_9_MODE__ _GPIO_ALT_OUT_PUSH_PULL_10MHZ

/*--
if this:PinExist("A", 9) then
    this:AddWidget("Combobox")
    this:AddPinState("A", 9)
end
--*/
#define __GPIO_PA_PIN_9_STATE__ _HIGH

/*--
if this:PinExist("A", 10) then
    local PA10 = {}
    PA10[#PA10 + 1] = {NAME = "PA10_NC", MODE = "_GPIO_ANALOG", STATE = "_LOW"}
    PA10[#PA10 + 1] = {NAME = "OTG_FS_ID", MODE = "_GPIO_IN_PULLED", STATE = "_HIGH"}
    PA10[#PA10 + 1] = {NAME = "TIM1_CH3_IC", MODE = "_GPIO_IN_FLOAT", STATE = "_LOW"}
    PA10[#PA10 + 1] = {NAME = "TIM1_CH3_OC", MODE = "_GPIO_ALT_OUT_PUSH_PULL_10MHZ", STATE = "_LOW"}
    PA10[#PA10 + 1] = {NAME = "TIM17_BKIN", MODE = "_GPIO_IN_FLOAT", STATE = "_LOW"}
    PA10[#PA10 + 1] = {NAME = "USART1_RX", MODE = "_GPIO_IN_PULLED", STATE = "_HIGH"}

    this:AddWidget("Textbox", "PA10")
    this:AddProposals(PA10, "A", 10)
end
--*/
#define __GPIO_PA_PIN_10_NAME__ USART1_RX

/*--
if this:PinExist("A", 10) then
    this:AddWidget("Combobox")
    this:AddPinMode("A", 10)
end
--*/
#define __GPIO_PA_PIN_10_MODE__ _GPIO_IN_PULLED

/*--
if this:PinExist("A", 10) then
    this:AddWidget("Combobox")
    this:AddPinState("A", 10)
end
--*/
#define __GPIO_PA_PIN_10_STATE__ _HIGH

/*--
if this:PinExist("A", 11) then
    local PA11 = {}
    PA11[#PA11 + 1] = {NAME = "PA11_NC", MODE = "_GPIO_ANALOG", STATE = "_LOW"}
    PA11[#PA11 + 1] = {NAME = "CAN2_RX", MODE = "_GPIO_IN_FLOAT", STATE = "_LOW"}
    PA11[#PA11 + 1] = {NAME = "CAN_RX", MODE = "_GPIO_IN_FLOAT", STATE = "_LOW"}
    PA11[#PA11 + 1] = {NAME = "OTG_FS_DM", MODE = "_GPIO_IN_FLOAT", STATE = "_LOW"}
    PA11[#PA11 + 1] = {NAME = "TIM1_CH4_IC", MODE = "_GPIO_IN_FLOAT", STATE = "_LOW"}
    PA11[#PA11 + 1] = {NAME = "TIM1_CH4_OC", MODE = "_GPIO_ALT_OUT_PUSH_PULL_10MHZ", STATE = "_LOW"}
    PA11[#PA11 + 1] = {NAME = "USART1_CTS", MODE = "_GPIO_IN_PULLED", STATE = "_HIGH"}
    PA11[#PA11 + 1] = {NAME = "USBDM", MODE = "_GPIO_IN_FLOAT", STATE = "_LOW"}

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
#define __GPIO_PA_PIN_11_MODE__ _GPIO_ANALOG

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
    PA12[#PA12 + 1] = {NAME = "PA12_NC", MODE = "_GPIO_ANALOG", STATE = "_LOW"}
    PA12[#PA12 + 1] = {NAME = "CAN2_TX", MODE = "_GPIO_ALT_OUT_PUSH_PULL_10MHZ", STATE = "_LOW"}
    PA12[#PA12 + 1] = {NAME = "CAN_TX", MODE = "_GPIO_ALT_OUT_PUSH_PULL_10MHZ", STATE = "_LOW"}
    PA12[#PA12 + 1] = {NAME = "OTG_FS_DP", MODE = "_GPIO_IN_FLOAT", STATE = "_LOW"}
    PA12[#PA12 + 1] = {NAME = "TIM1_ETR", MODE = "_GPIO_IN_FLOAT", STATE = "_LOW"}
    PA12[#PA12 + 1] = {NAME = "USART1_RTS", MODE = "_GPIO_ALT_OUT_PUSH_PULL_10MHZ", STATE = "_LOW"}
    PA12[#PA12 + 1] = {NAME = "USBDP", MODE = "_GPIO_IN_FLOAT", STATE = "_LOW"}

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
#define __GPIO_PA_PIN_12_MODE__ _GPIO_ANALOG

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
    PA13[#PA13 + 1] = {NAME = "PA13_NC", MODE = "_GPIO_ANALOG", STATE = "_LOW"}
    PA13[#PA13 + 1] = {NAME = "JTMS_SWDIO", MODE = "_GPIO_IN_FLOAT", STATE = "_LOW"}

    this:AddWidget("Textbox", "PA13")
    this:AddProposals(PA13, "A", 13)
end
--*/
#define __GPIO_PA_PIN_13_NAME__ JTMS_SWDIO

/*--
if this:PinExist("A", 13) then
    this:AddWidget("Combobox")
    this:AddPinMode("A", 13)
end
--*/
#define __GPIO_PA_PIN_13_MODE__ _GPIO_IN_FLOAT

/*--
if this:PinExist("A", 13) then
    this:AddWidget("Combobox")
    this:AddPinState("A", 13)
end
--*/
#define __GPIO_PA_PIN_13_STATE__ _FLOAT

/*--
if this:PinExist("A", 14) then
    local PA14 = {}
    PA14[#PA14 + 1] = {NAME = "PA14_NC", MODE = "_GPIO_ANALOG", STATE = "_LOW"}
    PA14[#PA14 + 1] = {NAME = "JTCK_SWCLK", MODE = "_GPIO_IN_FLOAT", STATE = "_LOW"}

    this:AddWidget("Textbox", "PA14")
    this:AddProposals(PA14, "A", 14)
end
--*/
#define __GPIO_PA_PIN_14_NAME__ JTCK_SWCLK

/*--
if this:PinExist("A", 14) then
    this:AddWidget("Combobox")
    this:AddPinMode("A", 14)
end
--*/
#define __GPIO_PA_PIN_14_MODE__ _GPIO_IN_FLOAT

/*--
if this:PinExist("A", 14) then
    this:AddWidget("Combobox")
    this:AddPinState("A", 14)
end
--*/
#define __GPIO_PA_PIN_14_STATE__ _FLOAT

/*--
if this:PinExist("A", 15) then
    local PA15 = {}
    PA15[#PA15 + 1] = {NAME = "PA15_NC", MODE = "_GPIO_ANALOG", STATE = "_LOW"}
    PA15[#PA15 + 1] = {NAME = "I2S3_WS_M", MODE = "_GPIO_ALT_OUT_PUSH_PULL_50MHZ", STATE = "_LOW"}
    PA15[#PA15 + 1] = {NAME = "I2S3_WS_S", MODE = "_GPIO_IN_FLOAT", STATE = "_LOW"}
    PA15[#PA15 + 1] = {NAME = "JTDI", MODE = "_GPIO_IN_FLOAT", STATE = "_LOW"}
    PA15[#PA15 + 1] = {NAME = "SPI1_NSS", MODE = "_GPIO_OUT_PUSH_PULL_2MHZ", STATE = "_HIGH"}
    PA15[#PA15 + 1] = {NAME = "SPI3_NSS", MODE = "_GPIO_OUT_PUSH_PULL_2MHZ", STATE = "_HIGH"}
    PA15[#PA15 + 1] = {NAME = "TIM2_CH1_ETR", MODE = "_GPIO_IN_FLOAT", STATE = "_LOW"}

    this:AddWidget("Textbox", "PA15")
    this:AddProposals(PA15, "A", 15)
end
--*/
#define __GPIO_PA_PIN_15_NAME__ JTDI

/*--
if this:PinExist("A", 15) then
    this:AddWidget("Combobox")
    this:AddPinMode("A", 15)
end
--*/
#define __GPIO_PA_PIN_15_MODE__ _GPIO_IN_FLOAT

/*--
if this:PinExist("A", 15) then
    this:AddWidget("Combobox")
    this:AddPinState("A", 15)
    this:Finish()
end
--*/
#define __GPIO_PA_PIN_15_STATE__ _FLOAT


/*------------------------------------------------------------------------------
if this:PinExist("B", 0) then
    local PB0 = {}
    PB0[#PB0 + 1] = {NAME = "PB0_NC", MODE = "_GPIO_ANALOG", STATE = "_LOW"}
    PB0[#PB0 + 1] = {NAME = "ADC12_IN8", MODE = "_GPIO_ANALOG", STATE = "_LOW"}
    PB0[#PB0 + 1] = {NAME = "ADC_IN8", MODE = "_GPIO_ANALOG", STATE = "_LOW"}
    PB0[#PB0 + 1] = {NAME = "ETH_MII_RXD2", MODE = "_GPIO_IN_FLOAT", STATE = "_LOW"}
    PB0[#PB0 + 1] = {NAME = "TIM1_CH2N", MODE = "_GPIO_ALT_OUT_PUSH_PULL_10MHZ", STATE = "_LOW"}
    PB0[#PB0 + 1] = {NAME = "TIM3_CH3_IC", MODE = "_GPIO_IN_FLOAT", STATE = "_LOW"}
    PB0[#PB0 + 1] = {NAME = "TIM3_CH3_OC", MODE = "_GPIO_ALT_OUT_PUSH_PULL_10MHZ", STATE = "_LOW"}
    PB0[#PB0 + 1] = {NAME = "TIM8_CH2N", MODE = "_GPIO_ALT_OUT_PUSH_PULL_10MHZ", STATE = "_LOW"}
    PB0[#PB0 + 1] = {NAME = "TIM13_CH1_IC", MODE = "_GPIO_IN_FLOAT", STATE = "_LOW"}
    PB0[#PB0 + 1] = {NAME = "TIM13_CH1_OC", MODE = "_GPIO_ALT_OUT_PUSH_PULL_10MHZ", STATE = "_LOW"}

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
#define __GPIO_PB_PIN_0_MODE__ _GPIO_ANALOG

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
    PB1[#PB1 + 1] = {NAME = "PB1_NC", MODE = "_GPIO_ANALOG", STATE = "_LOW"}
    PB1[#PB1 + 1] = {NAME = "ADC12_IN9", MODE = "_GPIO_ANALOG", STATE = "_LOW"}
    PB1[#PB1 + 1] = {NAME = "ADC_IN9", MODE = "_GPIO_ANALOG", STATE = "_LOW"}
    PB1[#PB1 + 1] = {NAME = "ETH_MII_RXD3", MODE = "_GPIO_IN_FLOAT", STATE = "_LOW"}
    PB1[#PB1 + 1] = {NAME = "TIM1_CH3N", MODE = "_GPIO_ALT_OUT_PUSH_PULL_10MHZ", STATE = "_LOW"}
    PB1[#PB1 + 1] = {NAME = "TIM3_CH4_IC", MODE = "_GPIO_IN_FLOAT", STATE = "_LOW"}
    PB1[#PB1 + 1] = {NAME = "TIM3_CH4_OC", MODE = "_GPIO_ALT_OUT_PUSH_PULL_10MHZ", STATE = "_LOW"}
    PB1[#PB1 + 1] = {NAME = "TIM8_CH3N", MODE = "_GPIO_ALT_OUT_PUSH_PULL_10MHZ", STATE = "_LOW"}
    PB1[#PB1 + 1] = {NAME = "TIM14_CH1_IC", MODE = "_GPIO_IN_FLOAT", STATE = "_LOW"}
    PB1[#PB1 + 1] = {NAME = "TIM14_CH1_OC", MODE = "_GPIO_ALT_OUT_PUSH_PULL_10MHZ", STATE = "_LOW"}

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
#define __GPIO_PB_PIN_1_MODE__ _GPIO_ANALOG

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
    PB2[#PB2 + 1] = {NAME = "PB2_NC", MODE = "_GPIO_ANALOG", STATE = "_LOW"}
    PB2[#PB2 + 1] = {NAME = "BOOT1", MODE = "_GPIO_IN_FLOAT", STATE = "_LOW"}

    this:AddWidget("Textbox", "PB2")
    this:AddProposals(PB2, "B", 2)
end
--*/
#define __GPIO_PB_PIN_2_NAME__ BOOT1

/*--
if this:PinExist("B", 2) then
    this:AddWidget("Combobox")
    this:AddPinMode("B", 2)
end
--*/
#define __GPIO_PB_PIN_2_MODE__ _GPIO_IN_FLOAT

/*--
if this:PinExist("B", 2) then
    this:AddWidget("Combobox")
    this:AddPinState("B", 2)
end
--*/
#define __GPIO_PB_PIN_2_STATE__ _FLOAT

/*--
if this:PinExist("B", 3) then
    local PB3 = {}
    PB3[#PB3 + 1] = {NAME = "PB3_NC", MODE = "_GPIO_ANALOG", STATE = "_LOW"}
    PB3[#PB3 + 1] = {NAME = "I2S3_CK_M", MODE = "_GPIO_ALT_OUT_PUSH_PULL_10MHZ", STATE = "_LOW"}
    PB3[#PB3 + 1] = {NAME = "I2S3_CK_S", MODE = "_GPIO_IN_FLOAT", STATE = "_LOW"}
    PB3[#PB3 + 1] = {NAME = "JTDO", MODE = "_GPIO_IN_FLOAT", STATE = "_LOW"}
    PB3[#PB3 + 1] = {NAME = "SPI1_SCK", MODE = "_GPIO_ALT_OUT_PUSH_PULL_50MHZ", STATE = "_LOW"}
    PB3[#PB3 + 1] = {NAME = "SPI3_SCK", MODE = "_GPIO_ALT_OUT_PUSH_PULL_50MHZ", STATE = "_LOW"}
    PB3[#PB3 + 1] = {NAME = "TIM2_CH2_IC", MODE = "_GPIO_IN_FLOAT", STATE = "_LOW"}
    PB3[#PB3 + 1] = {NAME = "TIM2_CH2_OC", MODE = "_GPIO_ALT_OUT_PUSH_PULL_10MHZ", STATE = "_LOW"}
    PB3[#PB3 + 1] = {NAME = "TRACESWO", MODE = "_GPIO_IN_FLOAT", STATE = "_LOW"}

    this:AddWidget("Textbox", "PB3")
    this:AddProposals(PB3, "B", 3)
end
--*/
#define __GPIO_PB_PIN_3_NAME__ JTDO

/*--
if this:PinExist("B", 3) then
    this:AddWidget("Combobox")
    this:AddPinMode("B", 3)
end
--*/
#define __GPIO_PB_PIN_3_MODE__ _GPIO_IN_FLOAT

/*--
if this:PinExist("B", 3) then
    this:AddWidget("Combobox")
    this:AddPinState("B", 3)
end
--*/
#define __GPIO_PB_PIN_3_STATE__ _FLOAT

/*--
if this:PinExist("B", 4) then
    local PB4 = {}
    PB4[#PB4 + 1] = {NAME = "PB4_NC", MODE = "_GPIO_ANALOG", STATE = "_LOW"}
    PB4[#PB4 + 1] = {NAME = "NJTRST", MODE = "_GPIO_IN_FLOAT", STATE = "_LOW"}
    PB4[#PB4 + 1] = {NAME = "SPI1_MISO", MODE = "_GPIO_IN_PULLED", STATE = "_LOW"}
    PB4[#PB4 + 1] = {NAME = "SPI3_MISO", MODE = "_GPIO_IN_PULLED", STATE = "_LOW"}
    PB4[#PB4 + 1] = {NAME = "TIM3_CH1_IC", MODE = "_GPIO_IN_FLOAT", STATE = "_LOW"}
    PB4[#PB4 + 1] = {NAME = "TIM3_CH1_OC", MODE = "_GPIO_ALT_OUT_PUSH_PULL_10MHZ", STATE = "_LOW"}

    this:AddWidget("Textbox", "PB4")
    this:AddProposals(PB4, "B", 4)
end
--*/
#define __GPIO_PB_PIN_4_NAME__ JTRST

/*--
if this:PinExist("B", 4) then
    this:AddWidget("Combobox")
    this:AddPinMode("B", 4)
end
--*/
#define __GPIO_PB_PIN_4_MODE__ _GPIO_IN_FLOAT

/*--
if this:PinExist("B", 4) then
    this:AddWidget("Combobox")
    this:AddPinState("B", 4)
end
--*/
#define __GPIO_PB_PIN_4_STATE__ _FLOAT

/*--
if this:PinExist("B", 5) then
    local PB5 = {}
    PB5[#PB5 + 1] = {NAME = "PB5_NC", MODE = "_GPIO_ANALOG", STATE = "_LOW"}
    PB5[#PB5 + 1] = {NAME = "CAN2_RX", MODE = "_GPIO_IN_FLOAT", STATE = "_LOW"}
    PB5[#PB5 + 1] = {NAME = "ETH_MII_PPS_OUT", MODE = "_GPIO_ALT_OUT_PUSH_PULL_50MHZ", STATE = "_HIGH"}
    PB5[#PB5 + 1] = {NAME = "ETH_RMII_PPS_OUT", MODE = "_GPIO_ALT_OUT_PUSH_PULL_50MHZ", STATE = "_HIGH"}
    PB5[#PB5 + 1] = {NAME = "I2C1_SMBA", MODE = "_GPIO_ALT_OUT_OPEN_DRAIN_10MHZ", STATE = "_HIGH"}
    PB5[#PB5 + 1] = {NAME = "I2S3_SD_R", MODE = "_GPIO_IN_PULLED", STATE = "_LOW"}
    PB5[#PB5 + 1] = {NAME = "I2S3_SD_T", MODE = "_GPIO_ALT_OUT_PUSH_PULL_50MHZ", STATE = "_LOW"}
    PB5[#PB5 + 1] = {NAME = "SPI1_MOSI", MODE = "_GPIO_ALT_OUT_PUSH_PULL_50MHZ", STATE = "_LOW"}
    PB5[#PB5 + 1] = {NAME = "SPI3_MOSI", MODE = "_GPIO_ALT_OUT_PUSH_PULL_50MHZ", STATE = "_LOW"}
    PB5[#PB5 + 1] = {NAME = "TIM3_CH2_IC", MODE = "_GPIO_IN_FLOAT", STATE = "_LOW"}
    PB5[#PB5 + 1] = {NAME = "TIM3_CH2_OC", MODE = "_GPIO_ALT_OUT_PUSH_PULL_10MHZ", STATE = "_LOW"}
    PB5[#PB5 + 1] = {NAME = "TIM16_BKIN", MODE = "_GPIO_IN_FLOAT", STATE = "_LOW"}

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
#define __GPIO_PB_PIN_5_MODE__ _GPIO_ANALOG

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
    PB6[#PB6 + 1] = {NAME = "PB6_NC", MODE = "_GPIO_ANALOG", STATE = "_LOW"}
    PB6[#PB6 + 1] = {NAME = "CAN2_TX", MODE = "_GPIO_ALT_OUT_PUSH_PULL_10MHZ", STATE = "_LOW"}
    PB6[#PB6 + 1] = {NAME = "I2C1_SCL", MODE = "_GPIO_ALT_OUT_OPEN_DRAIN_10MHZ", STATE = "_HIGH"}
    PB6[#PB6 + 1] = {NAME = "TIM4_CH1_IC", MODE = "_GPIO_IN_FLOAT", STATE = "_LOW"}
    PB6[#PB6 + 1] = {NAME = "TIM4_CH1_OC", MODE = "_GPIO_ALT_OUT_PUSH_PULL_10MHZ", STATE = "_LOW"}
    PB6[#PB6 + 1] = {NAME = "TIM16_CH1N", MODE = "_GPIO_ALT_OUT_PUSH_PULL_10MHZ", STATE = "_LOW"}
    PB6[#PB6 + 1] = {NAME = "USART1_TX", MODE = "_GPIO_ALT_OUT_PUSH_PULL_10MHZ", STATE = "_HIGH"}

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
#define __GPIO_PB_PIN_6_MODE__ _GPIO_ANALOG

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
    PB7[#PB7 + 1] = {NAME = "PB7_NC", MODE = "_GPIO_ANALOG", STATE = "_LOW"}
    PB7[#PB7 + 1] = {NAME = "FSMC_NADV", MODE = "_GPIO_IN_FLOAT", STATE = "_LOW"}
    PB7[#PB7 + 1] = {NAME = "I2C1_SDA", MODE = "_GPIO_ALT_OUT_OPEN_DRAIN_10MHZ", STATE = "_HIGH"}
    PB7[#PB7 + 1] = {NAME = "TIM4_CH2_IC", MODE = "_GPIO_IN_FLOAT", STATE = "_LOW"}
    PB7[#PB7 + 1] = {NAME = "TIM4_CH2_OC", MODE = "_GPIO_ALT_OUT_PUSH_PULL_10MHZ", STATE = "_LOW"}
    PB7[#PB7 + 1] = {NAME = "TIM17_CH1N", MODE = "_GPIO_ALT_OUT_PUSH_PULL_10MHZ", STATE = "_LOW"}
    PB7[#PB7 + 1] = {NAME = "USART1_RX", MODE = "_GPIO_IN_PULLED", STATE = "_HIGH"}

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
#define __GPIO_PB_PIN_7_MODE__ _GPIO_ANALOG

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
    PB8[#PB8 + 1] = {NAME = "PB8_NC", MODE = "_GPIO_ANALOG", STATE = "_LOW"}
    PB8[#PB8 + 1] = {NAME = "CAN1_RX", MODE = "_GPIO_IN_FLOAT", STATE = "_LOW"}
    PB8[#PB8 + 1] = {NAME = "CAN_RX", MODE = "_GPIO_IN_FLOAT", STATE = "_LOW"}
    PB8[#PB8 + 1] = {NAME = "ETH_MII_TXD3", MODE = "_GPIO_ALT_OUT_PUSH_PULL_50MHZ", STATE = "_LOW"}
    PB8[#PB8 + 1] = {NAME = "HDMI_CEC", MODE = "_GPIO_ALT_OUT_OPEN_DRAIN_2MHZ", STATE = "_HIGH"}
    PB8[#PB8 + 1] = {NAME = "I2C1_SCL", MODE = "_GPIO_ALT_OUT_OPEN_DRAIN_10MHZ", STATE = "_HIGH"}
    PB8[#PB8 + 1] = {NAME = "SDIO_D4", MODE = "_GPIO_ALT_OUT_PUSH_PULL_50MHZ", STATE = "_LOW"}
    PB8[#PB8 + 1] = {NAME = "TIM4_CH3_IC", MODE = "_GPIO_IN_FLOAT", STATE = "_LOW"}
    PB8[#PB8 + 1] = {NAME = "TIM4_CH3_OC", MODE = "_GPIO_ALT_OUT_PUSH_PULL_10MHZ", STATE = "_LOW"}
    PB8[#PB8 + 1] = {NAME = "TIM10_CH1_IC", MODE = "_GPIO_IN_FLOAT", STATE = "_LOW"}
    PB8[#PB8 + 1] = {NAME = "TIM10_CH1_OC", MODE = "_GPIO_ALT_OUT_PUSH_PULL_10MHZ", STATE = "_LOW"}
    PB8[#PB8 + 1] = {NAME = "TIM16_CH1_IC", MODE = "_GPIO_IN_FLOAT", STATE = "_LOW"}
    PB8[#PB8 + 1] = {NAME = "TIM16_CH1_OC", MODE = "_GPIO_ALT_OUT_PUSH_PULL_10MHZ", STATE = "_LOW"}

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
#define __GPIO_PB_PIN_8_MODE__ _GPIO_ANALOG

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
    PB9[#PB9 + 1] = {NAME = "PB9_NC", MODE = "_GPIO_ANALOG", STATE = "_LOW"}
    PB9[#PB9 + 1] = {NAME = "CAN1_TX", MODE = "_GPIO_ALT_OUT_PUSH_PULL_10MHZ", STATE = "_LOW"}
    PB9[#PB9 + 1] = {NAME = "CAN_TX", MODE = "_GPIO_ALT_OUT_PUSH_PULL_10MHZ", STATE = "_LOW"}
    PB9[#PB9 + 1] = {NAME = "I2C1_SDA", MODE = "_GPIO_ALT_OUT_OPEN_DRAIN_10MHZ", STATE = "_HIGH"}
    PB9[#PB9 + 1] = {NAME = "SDIO_D5", MODE = "_GPIO_ALT_OUT_PUSH_PULL_50MHZ", STATE = "_LOW"}
    PB9[#PB9 + 1] = {NAME = "TIM4_CH4_IC", MODE = "_GPIO_IN_FLOAT", STATE = "_LOW"}
    PB9[#PB9 + 1] = {NAME = "TIM4_CH4_OC", MODE = "_GPIO_ALT_OUT_PUSH_PULL_10MHZ", STATE = "_LOW"}
    PB9[#PB9 + 1] = {NAME = "TIM11_CH1_IC", MODE = "_GPIO_IN_FLOAT", STATE = "_LOW"}
    PB9[#PB9 + 1] = {NAME = "TIM11_CH1_OC", MODE = "_GPIO_ALT_OUT_PUSH_PULL_10MHZ", STATE = "_LOW"}
    PB9[#PB9 + 1] = {NAME = "TIM17_CH1_IC", MODE = "_GPIO_IN_FLOAT", STATE = "_LOW"}
    PB9[#PB9 + 1] = {NAME = "TIM17_CH1_OC", MODE = "_GPIO_ALT_OUT_PUSH_PULL_10MHZ", STATE = "_LOW"}

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
#define __GPIO_PB_PIN_9_MODE__ _GPIO_ANALOG

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
    PB10[#PB10 + 1] = {NAME = "PB10_NC", MODE = "_GPIO_ANALOG", STATE = "_LOW"}
    PB10[#PB10 + 1] = {NAME = "ETH_MII_RX_ER", MODE = "_GPIO_IN_FLOAT", STATE = "_LOW"}
    PB10[#PB10 + 1] = {NAME = "HDMI_CEC", MODE = "_GPIO_ALT_OUT_OPEN_DRAIN_2MHZ", STATE = "_HIGH"}
    PB10[#PB10 + 1] = {NAME = "I2C2_SCL", MODE = "_GPIO_ALT_OUT_OPEN_DRAIN_10MHZ", STATE = "_HIGH"}
    PB10[#PB10 + 1] = {NAME = "TIM2_CH3_IC", MODE = "_GPIO_IN_FLOAT", STATE = "_LOW"}
    PB10[#PB10 + 1] = {NAME = "TIM2_CH3_OC", MODE = "_GPIO_ALT_OUT_PUSH_PULL_10MHZ", STATE = "_LOW"}
    PB10[#PB10 + 1] = {NAME = "USART3_TX", MODE = "_GPIO_ALT_OUT_PUSH_PULL_10MHZ", STATE = "_HIGH"}

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
#define __GPIO_PB_PIN_10_MODE__ _GPIO_ANALOG

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
    PB11[#PB11 + 1] = {NAME = "PB11_NC", MODE = "_GPIO_ANALOG", STATE = "_LOW"}
    PB11[#PB11 + 1] = {NAME = "ETH_MII_TX_EN", MODE = "_GPIO_ALT_OUT_PUSH_PULL_50MHZ", STATE = "_LOW"}
    PB11[#PB11 + 1] = {NAME = "ETH_RMII_TX_EN", MODE = "_GPIO_ALT_OUT_PUSH_PULL_50MHZ", STATE = "_LOW"}
    PB11[#PB11 + 1] = {NAME = "I2C2_SDA", MODE = "_GPIO_ALT_OUT_OPEN_DRAIN_10MHZ", STATE = "_HIGH"}
    PB11[#PB11 + 1] = {NAME = "TIM2_CH4_IC", MODE = "_GPIO_IN_FLOAT", STATE = "_LOW"}
    PB11[#PB11 + 1] = {NAME = "TIM2_CH4_OC", MODE = "_GPIO_ALT_OUT_PUSH_PULL_10MHZ", STATE = "_LOW"}
    PB11[#PB11 + 1] = {NAME = "USART3_RX", MODE = "_GPIO_IN_PULLED", STATE = "_HIGH"}

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
#define __GPIO_PB_PIN_11_MODE__ _GPIO_ANALOG

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
    PB12[#PB12 + 1] = {NAME = "PB12_NC", MODE = "_GPIO_ANALOG", STATE = "_LOW"}
    PB12[#PB12 + 1] = {NAME = "CAN2_RX", MODE = "_GPIO_IN_FLOAT", STATE = "_LOW"}
    PB12[#PB12 + 1] = {NAME = "ETH_MII_TXD0", MODE = "_GPIO_ALT_OUT_PUSH_PULL_50MHZ", STATE = "_LOW"}
    PB12[#PB12 + 1] = {NAME = "ETH_RMII_TXD0", MODE = "_GPIO_ALT_OUT_PUSH_PULL_50MHZ", STATE = "_LOW"}
    PB12[#PB12 + 1] = {NAME = "I2C2_SMBA", MODE = "_GPIO_ALT_OUT_OPEN_DRAIN_10MHZ", STATE = "_HIGH"}
    PB12[#PB12 + 1] = {NAME = "I2S2_WS_M", MODE = "_GPIO_ALT_OUT_PUSH_PULL_50MHZ", STATE = "_LOW"}
    PB12[#PB12 + 1] = {NAME = "I2S2_WS_S", MODE = "_GPIO_IN_FLOAT", STATE = "_LOW"}
    PB12[#PB12 + 1] = {NAME = "SPI2_NSS", MODE = "_GPIO_OUT_PUSH_PULL_2MHZ", STATE = "_HIGH"}
    PB12[#PB12 + 1] = {NAME = "TIM1_BKIN", MODE = "_GPIO_IN_FLOAT", STATE = "_LOW"}
    PB12[#PB12 + 1] = {NAME = "TIM12_CH1_IC", MODE = "_GPIO_IN_FLOAT", STATE = "_LOW"}
    PB12[#PB12 + 1] = {NAME = "TIM12_CH1_OC", MODE = "_GPIO_ALT_OUT_PUSH_PULL_10MHZ", STATE = "_LOW"}
    PB12[#PB12 + 1] = {NAME = "USART3_CK", MODE = "_GPIO_ALT_OUT_PUSH_PULL_10MHZ", STATE = "_HIGH"}

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
#define __GPIO_PB_PIN_12_MODE__ _GPIO_ANALOG

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
    PB13[#PB13 + 1] = {NAME = "PB13_NC", MODE = "_GPIO_ANALOG", STATE = "_LOW"}
    PB13[#PB13 + 1] = {NAME = "CAN2_TX", MODE = "_GPIO_ALT_OUT_PUSH_PULL_10MHZ", STATE = "_LOW"}
    PB13[#PB13 + 1] = {NAME = "ETH_MII_TXD1", MODE = "_GPIO_ALT_OUT_PUSH_PULL_50MHZ", STATE = "_LOW"}
    PB13[#PB13 + 1] = {NAME = "ETH_RMII_TXD1", MODE = "_GPIO_ALT_OUT_PUSH_PULL_50MHZ", STATE = "_LOW"}
    PB13[#PB13 + 1] = {NAME = "I2S2_CK_M", MODE = "_GPIO_ALT_OUT_PUSH_PULL_10MHZ", STATE = "_LOW"}
    PB13[#PB13 + 1] = {NAME = "I2S2_CK_S", MODE = "_GPIO_IN_FLOAT", STATE = "_LOW"}
    PB13[#PB13 + 1] = {NAME = "SPI2_SCK", MODE = "_GPIO_ALT_OUT_PUSH_PULL_50MHZ", STATE = "_LOW"}
    PB13[#PB13 + 1] = {NAME = "TIM1_CH1N", MODE = "_GPIO_ALT_OUT_PUSH_PULL_10MHZ", STATE = "_LOW"}
    PB13[#PB13 + 1] = {NAME = "TIM12_CH2_IC", MODE = "_GPIO_IN_FLOAT", STATE = "_LOW"}
    PB13[#PB13 + 1] = {NAME = "TIM12_CH2_OC", MODE = "_GPIO_ALT_OUT_PUSH_PULL_10MHZ", STATE = "_LOW"}
    PB13[#PB13 + 1] = {NAME = "USART3_CTS", MODE = "_GPIO_IN_PULLED", STATE = "_HIGH"}

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
#define __GPIO_PB_PIN_13_MODE__ _GPIO_ANALOG

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
    PB14[#PB14 + 1] = {NAME = "PB14_NC", MODE = "_GPIO_ANALOG", STATE = "_LOW"}
    PB14[#PB14 + 1] = {NAME = "SPI2_MISO", MODE = "_GPIO_IN_PULLED", STATE = "_LOW"}
    PB14[#PB14 + 1] = {NAME = "TIM1_CH2N", MODE = "_GPIO_ALT_OUT_PUSH_PULL_10MHZ", STATE = "_LOW"}
    PB14[#PB14 + 1] = {NAME = "TIM12_CH1_IC", MODE = "_GPIO_IN_FLOAT", STATE = "_LOW"}
    PB14[#PB14 + 1] = {NAME = "TIM12_CH1_OC", MODE = "_GPIO_ALT_OUT_PUSH_PULL_10MHZ", STATE = "_LOW"}
    PB14[#PB14 + 1] = {NAME = "TIM15_CH1_IC", MODE = "_GPIO_IN_FLOAT", STATE = "_LOW"}
    PB14[#PB14 + 1] = {NAME = "TIM15_CH1_OC", MODE = "_GPIO_ALT_OUT_PUSH_PULL_10MHZ", STATE = "_LOW"}
    PB14[#PB14 + 1] = {NAME = "USART3_RTS", MODE = "_GPIO_ALT_OUT_PUSH_PULL_10MHZ", STATE = "_LOW"}

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
#define __GPIO_PB_PIN_14_MODE__ _GPIO_ANALOG

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
    PB15[#PB15 + 1] = {NAME = "PB15_NC", MODE = "_GPIO_ANALOG", STATE = "_LOW"}
    PB15[#PB15 + 1] = {NAME = "I2S2_SD_R", MODE = "_GPIO_IN_PULLED", STATE = "_LOW"}
    PB15[#PB15 + 1] = {NAME = "I2S2_SD_T", MODE = "_GPIO_ALT_OUT_PUSH_PULL_50MHZ", STATE = "_LOW"}
    PB15[#PB15 + 1] = {NAME = "SPI2_MOSI", MODE = "_GPIO_ALT_OUT_PUSH_PULL_50MHZ", STATE = "_LOW"}
    PB15[#PB15 + 1] = {NAME = "TIM1_CH3N", MODE = "_GPIO_ALT_OUT_PUSH_PULL_10MHZ", STATE = "_LOW"}
    PB15[#PB15 + 1] = {NAME = "TIM12_CH2_IC", MODE = "_GPIO_IN_FLOAT", STATE = "_LOW"}
    PB15[#PB15 + 1] = {NAME = "TIM12_CH2_OC", MODE = "_GPIO_ALT_OUT_PUSH_PULL_10MHZ", STATE = "_LOW"}
    PB15[#PB15 + 1] = {NAME = "TIM15_CH1N", MODE = "_GPIO_ALT_OUT_PUSH_PULL_10MHZ", STATE = "_LOW"}
    PB15[#PB15 + 1] = {NAME = "TIM15_CH2_IC", MODE = "_GPIO_IN_FLOAT", STATE = "_LOW"}
    PB15[#PB15 + 1] = {NAME = "TIM15_CH2_OC", MODE = "_GPIO_ALT_OUT_PUSH_PULL_10MHZ", STATE = "_LOW"}

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
#define __GPIO_PB_PIN_15_MODE__ _GPIO_ANALOG

/*--
if this:PinExist("B", 15) then
    this:AddWidget("Combobox")
    this:AddPinState("B", 15)
    this:Finish()
end
--*/
#define __GPIO_PB_PIN_15_STATE__ _LOW



/*------------------------------------------------------------------------------
if this:PinExist("C", 0) then
    local PC0 = {}
    PC0[#PC0 + 1] = {NAME = "PC0_NC", MODE = "_GPIO_ANALOG", STATE = "_LOW"}
    PC0[#PC0 + 1] = {NAME = "ADC12_IN10", MODE = "_GPIO_ANALOG", STATE = "_LOW"}
    PC0[#PC0 + 1] = {NAME = "ADC123_IN10", MODE = "_GPIO_ANALOG", STATE = "_LOW"}
    PC0[#PC0 + 1] = {NAME = "ADC_IN10", MODE = "_GPIO_ANALOG", STATE = "_LOW"}

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
#define __GPIO_PC_PIN_0_MODE__ _GPIO_ANALOG

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
    PC1[#PC1 + 1] = {NAME = "PC1_NC", MODE = "_GPIO_ANALOG", STATE = "_LOW"}
    PC1[#PC1 + 1] = {NAME = "ADC12_IN11", MODE = "_GPIO_ANALOG", STATE = "_LOW"}
    PC1[#PC1 + 1] = {NAME = "ADC123_IN11", MODE = "_GPIO_ANALOG", STATE = "_LOW"}
    PC1[#PC1 + 1] = {NAME = "ADC_IN11", MODE = "_GPIO_ANALOG", STATE = "_LOW"}
    PC1[#PC1 + 1] = {NAME = "ETH_MII_MDC", MODE = "_GPIO_ALT_OUT_PUSH_PULL_50MHZ", STATE = "_LOW"}
    PC1[#PC1 + 1] = {NAME = "ETH_RMII_MDC", MODE = "_GPIO_ALT_OUT_PUSH_PULL_50MHZ", STATE = "_LOW"}

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
#define __GPIO_PC_PIN_1_MODE__ _GPIO_ANALOG

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
    PC2[#PC2 + 1] = {NAME = "PC2_NC", MODE = "_GPIO_ANALOG", STATE = "_LOW"}
    PC2[#PC2 + 1] = {NAME = "ADC12_IN12", MODE = "_GPIO_ANALOG", STATE = "_LOW"}
    PC2[#PC2 + 1] = {NAME = "ADC123_IN12", MODE = "_GPIO_ANALOG", STATE = "_LOW"}
    PC2[#PC2 + 1] = {NAME = "ADC_IN12", MODE = "_GPIO_ANALOG", STATE = "_LOW"}
    PC2[#PC2 + 1] = {NAME = "ETH_MII_TXD2", MODE = "_GPIO_ALT_OUT_PUSH_PULL_50MHZ", STATE = "_LOW"}

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
#define __GPIO_PC_PIN_2_MODE__ _GPIO_ANALOG

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
    PC3[#PC3 + 1] = {NAME = "PC3_NC", MODE = "_GPIO_ANALOG", STATE = "_LOW"}
    PC3[#PC3 + 1] = {NAME = "ADC12_IN13", MODE = "_GPIO_ANALOG", STATE = "_LOW"}
    PC3[#PC3 + 1] = {NAME = "ADC123_IN13", MODE = "_GPIO_ANALOG", STATE = "_LOW"}
    PC3[#PC3 + 1] = {NAME = "ADC_IN13", MODE = "_GPIO_ANALOG", STATE = "_LOW"}
    PC3[#PC3 + 1] = {NAME = "ETH_MII_TX_CLK", MODE = "_GPIO_IN_FLOAT", STATE = "_LOW"}

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
#define __GPIO_PC_PIN_3_MODE__ _GPIO_ANALOG

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
    PC4[#PC4 + 1] = {NAME = "PC4_NC", MODE = "_GPIO_ANALOG", STATE = "_LOW"}
    PC4[#PC4 + 1] = {NAME = "ADC12_IN14", MODE = "_GPIO_ANALOG", STATE = "_LOW"}
    PC4[#PC4 + 1] = {NAME = "ADC_IN14", MODE = "_GPIO_ANALOG", STATE = "_LOW"}
    PC4[#PC4 + 1] = {NAME = "ETH_MII_RXD0", MODE = "_GPIO_IN_FLOAT", STATE = "_LOW"}
    PC4[#PC4 + 1] = {NAME = "ETH_RMII_RXD0", MODE = "_GPIO_IN_FLOAT", STATE = "_LOW"}
    PC4[#PC4 + 1] = {NAME = "TIM12_CH1_IC", MODE = "_GPIO_IN_FLOAT", STATE = "_LOW"}
    PC4[#PC4 + 1] = {NAME = "TIM12_CH1_OC", MODE = "_GPIO_ALT_OUT_PUSH_PULL_10MHZ", STATE = "_LOW"}

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
#define __GPIO_PC_PIN_4_MODE__ _GPIO_ANALOG

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
    PC5[#PC5 + 1] = {NAME = "PC5_NC", MODE = "_GPIO_ANALOG", STATE = "_LOW"}
    PC5[#PC5 + 1] = {NAME = "ADC12_IN15", MODE = "_GPIO_ANALOG", STATE = "_LOW"}
    PC5[#PC5 + 1] = {NAME = "ADC_IN15", MODE = "_GPIO_ANALOG", STATE = "_LOW"}
    PC5[#PC5 + 1] = {NAME = "ETH_MII_RXD1", MODE = "_GPIO_IN_FLOAT", STATE = "_LOW"}
    PC5[#PC5 + 1] = {NAME = "ETH_RMII_RXD1", MODE = "_GPIO_IN_FLOAT", STATE = "_LOW"}
    PC5[#PC5 + 1] = {NAME = "TIM12_CH2_IC", MODE = "_GPIO_IN_FLOAT", STATE = "_LOW"}
    PC5[#PC5 + 1] = {NAME = "TIM12_CH2_OC", MODE = "_GPIO_ALT_OUT_PUSH_PULL_10MHZ", STATE = "_LOW"}

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
#define __GPIO_PC_PIN_5_MODE__ _GPIO_ANALOG

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
    PC6[#PC6 + 1] = {NAME = "PC6_NC", MODE = "_GPIO_ANALOG", STATE = "_LOW"}
    PC6[#PC6 + 1] = {NAME = "I2S2_MCK_M", MODE = "_GPIO_ALT_OUT_PUSH_PULL_50MHZ", STATE = "_LOW"}
    PC6[#PC6 + 1] = {NAME = "SDIO_D6", MODE = "_GPIO_ALT_OUT_PUSH_PULL_50MHZ", STATE = "_LOW"}
    PC6[#PC6 + 1] = {NAME = "TIM3_CH1_IC", MODE = "_GPIO_IN_FLOAT", STATE = "_LOW"}
    PC6[#PC6 + 1] = {NAME = "TIM3_CH1_OC", MODE = "_GPIO_ALT_OUT_PUSH_PULL_10MHZ", STATE = "_LOW"}
    PC6[#PC6 + 1] = {NAME = "TIM8_CH1_IC", MODE = "_GPIO_IN_FLOAT", STATE = "_LOW"}
    PC6[#PC6 + 1] = {NAME = "TIM8_CH1_OC", MODE = "_GPIO_ALT_OUT_PUSH_PULL_10MHZ", STATE = "_LOW"}

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
#define __GPIO_PC_PIN_6_MODE__ _GPIO_ANALOG

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
    PC7[#PC7 + 1] = {NAME = "PC7_NC", MODE = "_GPIO_ANALOG", STATE = "_LOW"}
    PC7[#PC7 + 1] = {NAME = "I2S3_MCK_M", MODE = "_GPIO_ALT_OUT_PUSH_PULL_50MHZ", STATE = "_LOW"}
    PC7[#PC7 + 1] = {NAME = "SDIO_D7", MODE = "_GPIO_ALT_OUT_PUSH_PULL_50MHZ", STATE = "_LOW"}
    PC7[#PC7 + 1] = {NAME = "TIM3_CH2_IC", MODE = "_GPIO_IN_FLOAT", STATE = "_LOW"}
    PC7[#PC7 + 1] = {NAME = "TIM3_CH2_OC", MODE = "_GPIO_ALT_OUT_PUSH_PULL_10MHZ", STATE = "_LOW"}
    PC7[#PC7 + 1] = {NAME = "TIM8_CH2_IC", MODE = "_GPIO_IN_FLOAT", STATE = "_LOW"}
    PC7[#PC7 + 1] = {NAME = "TIM8_CH2_OC", MODE = "_GPIO_ALT_OUT_PUSH_PULL_10MHZ", STATE = "_LOW"}

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
#define __GPIO_PC_PIN_7_MODE__ _GPIO_ANALOG

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
    PC8[#PC8 + 1] = {NAME = "PC8_NC", MODE = "_GPIO_ANALOG", STATE = "_LOW"}
    PC8[#PC8 + 1] = {NAME = "SDIO_D0", MODE = "_GPIO_ALT_OUT_PUSH_PULL_50MHZ", STATE = "_LOW"}
    PC8[#PC8 + 1] = {NAME = "TIM3_CH3_IC", MODE = "_GPIO_IN_FLOAT", STATE = "_LOW"}
    PC8[#PC8 + 1] = {NAME = "TIM3_CH3_OC", MODE = "_GPIO_ALT_OUT_PUSH_PULL_10MHZ", STATE = "_LOW"}
    PC8[#PC8 + 1] = {NAME = "TIM8_CH3_IC", MODE = "_GPIO_IN_FLOAT", STATE = "_LOW"}
    PC8[#PC8 + 1] = {NAME = "TIM8_CH3_OC", MODE = "_GPIO_ALT_OUT_PUSH_PULL_10MHZ", STATE = "_LOW"}
    PC8[#PC8 + 1] = {NAME = "TIM13_CH1_IC", MODE = "_GPIO_IN_FLOAT", STATE = "_LOW"}
    PC8[#PC8 + 1] = {NAME = "TIM13_CH1_OC", MODE = "_GPIO_ALT_OUT_PUSH_PULL_10MHZ", STATE = "_LOW"}

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
#define __GPIO_PC_PIN_8_MODE__ _GPIO_ANALOG

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
    PC9[#PC9 + 1] = {NAME = "PC9_NC", MODE = "_GPIO_ANALOG", STATE = "_LOW"}
    PC9[#PC9 + 1] = {NAME = "SDIO_D1", MODE = "_GPIO_ALT_OUT_PUSH_PULL_50MHZ", STATE = "_LOW"}
    PC9[#PC9 + 1] = {NAME = "TIM3_CH4_IC", MODE = "_GPIO_IN_FLOAT", STATE = "_LOW"}
    PC9[#PC9 + 1] = {NAME = "TIM3_CH4_OC", MODE = "_GPIO_ALT_OUT_PUSH_PULL_10MHZ", STATE = "_LOW"}
    PC9[#PC9 + 1] = {NAME = "TIM8_CH4_IC", MODE = "_GPIO_IN_FLOAT", STATE = "_LOW"}
    PC9[#PC9 + 1] = {NAME = "TIM8_CH4_OC", MODE = "_GPIO_ALT_OUT_PUSH_PULL_10MHZ", STATE = "_LOW"}
    PC9[#PC9 + 1] = {NAME = "TIM14_CH1_IC", MODE = "_GPIO_IN_FLOAT", STATE = "_LOW"}
    PC9[#PC9 + 1] = {NAME = "TIM14_CH1_OC", MODE = "_GPIO_ALT_OUT_PUSH_PULL_10MHZ", STATE = "_LOW"}

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
#define __GPIO_PC_PIN_9_MODE__ _GPIO_ANALOG

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
    PC10[#PC10 + 1] = {NAME = "PC10_NC", MODE = "_GPIO_ANALOG", STATE = "_LOW"}
    PC10[#PC10 + 1] = {NAME = "I2S3_CK_M", MODE = "_GPIO_ALT_OUT_PUSH_PULL_10MHZ", STATE = "_LOW"}
    PC10[#PC10 + 1] = {NAME = "I2S3_CK_S", MODE = "_GPIO_IN_FLOAT", STATE = "_LOW"}
    PC10[#PC10 + 1] = {NAME = "SDIO_D2", MODE = "_GPIO_ALT_OUT_PUSH_PULL_50MHZ", STATE = "_LOW"}
    PC10[#PC10 + 1] = {NAME = "SPI3_SCK", MODE = "_GPIO_ALT_OUT_PUSH_PULL_50MHZ", STATE = "_LOW"}
    PC10[#PC10 + 1] = {NAME = "UART4_TX", MODE = "_GPIO_ALT_OUT_PUSH_PULL_10MHZ", STATE = "_HIGH"}
    PC10[#PC10 + 1] = {NAME = "USART3_TX", MODE = "_GPIO_ALT_OUT_PUSH_PULL_10MHZ", STATE = "_HIGH"}

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
#define __GPIO_PC_PIN_10_MODE__ _GPIO_ANALOG

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
    PC11[#PC11 + 1] = {NAME = "PC11_NC", MODE = "_GPIO_ANALOG", STATE = "_LOW"}
    PC11[#PC11 + 1] = {NAME = "SDIO_D3", MODE = "_GPIO_ALT_OUT_PUSH_PULL_50MHZ", STATE = "_LOW"}
    PC11[#PC11 + 1] = {NAME = "SPI3_MISO", MODE = "_GPIO_IN_PULLED", STATE = "_LOW"}
    PC11[#PC11 + 1] = {NAME = "UART4_RX", MODE = "_GPIO_IN_PULLED", STATE = "_HIGH"}
    PC11[#PC11 + 1] = {NAME = "USART3_RX", MODE = "_GPIO_IN_PULLED", STATE = "_HIGH"}

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
#define __GPIO_PC_PIN_11_MODE__ _GPIO_ANALOG

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
    PC12[#PC12 + 1] = {NAME = "PC12_NC", MODE = "_GPIO_ANALOG", STATE = "_LOW"}
    PC12[#PC12 + 1] = {NAME = "I2S3_SD_R", MODE = "_GPIO_IN_PULLED", STATE = "_LOW"}
    PC12[#PC12 + 1] = {NAME = "I2S3_SD_T", MODE = "_GPIO_ALT_OUT_PUSH_PULL_50MHZ", STATE = "_LOW"}
    PC12[#PC12 + 1] = {NAME = "SDIO_CK", MODE = "_GPIO_ALT_OUT_PUSH_PULL_50MHZ", STATE = "_LOW"}
    PC12[#PC12 + 1] = {NAME = "SPI3_MOSI", MODE = "_GPIO_ALT_OUT_PUSH_PULL_50MHZ", STATE = "_LOW"}
    PC12[#PC12 + 1] = {NAME = "UART5_TX", MODE = "_GPIO_ALT_OUT_PUSH_PULL_10MHZ", STATE = "_HIGH"}
    PC12[#PC12 + 1] = {NAME = "USART3_CK", MODE = "_GPIO_ALT_OUT_PUSH_PULL_10MHZ", STATE = "_HIGH"}

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
#define __GPIO_PC_PIN_12_MODE__ _GPIO_ANALOG

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
    PC13[#PC13 + 1] = {NAME = "PC13_NC", MODE = "_GPIO_ANALOG", STATE = "_LOW"}
    PC13[#PC13 + 1] = {NAME = "TAMPER_RTC", MODE = "_GPIO_IN_FLOAT", STATE = "_LOW"}

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
#define __GPIO_PC_PIN_13_MODE__ _GPIO_ANALOG

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
    PC14[#PC14 + 1] = {NAME = "PC14_NC", MODE = "_GPIO_ANALOG", STATE = "_LOW"}
    PC14[#PC14 + 1] = {NAME = "OSC32_IN", MODE = "_GPIO_IN_FLOAT", STATE = "_LOW"}

    this:AddWidget("Textbox", "PC14")
    this:AddProposals(PC14, "C", 14)
end
--*/
#define __GPIO_PC_PIN_14_NAME__ OSC32_IN

/*--
if this:PinExist("C", 14) then
    this:AddWidget("Combobox")
    this:AddPinMode("C", 14)
end
--*/
#define __GPIO_PC_PIN_14_MODE__ _GPIO_IN_FLOAT

/*--
if this:PinExist("C", 14) then
    this:AddWidget("Combobox")
    this:AddPinState("C", 14)
end
--*/
#define __GPIO_PC_PIN_14_STATE__ _FLOAT

/*--
if this:PinExist("C", 15) then
    local PC15 = {}
    PC15[#PC15 + 1] = {NAME = "PC15_NC", MODE = "_GPIO_ANALOG", STATE = "_LOW"}
    PC15[#PC15 + 1] = {NAME = "OSC32_OUT", MODE = "_GPIO_IN_FLOAT", STATE = "_LOW"}

    this:AddWidget("Textbox", "PC15")
    this:AddProposals(PC15, "C", 15)
end
--*/
#define __GPIO_PC_PIN_15_NAME__ OSC32_OUT

/*--
if this:PinExist("C", 15) then
    this:AddWidget("Combobox")
    this:AddPinMode("C", 15)
end
--*/
#define __GPIO_PC_PIN_15_MODE__ _GPIO_IN_FLOAT

/*--
if this:PinExist("C", 15) then
    this:AddWidget("Combobox")
    this:AddPinState("C", 15)
    this:Finish()
end
--*/
#define __GPIO_PC_PIN_15_STATE__ _FLOAT



/*------------------------------------------------------------------------------
if this:PinExist("D", 0) then
    local PD0 = {}
    PD0[#PD0 + 1] = {NAME = "PD0_NC", MODE = "_GPIO_ANALOG", STATE = "_LOW"}
    PD0[#PD0 + 1] = {NAME = "CAN1_RX", MODE = "_GPIO_IN_FLOAT", STATE = "_LOW"}
    PD0[#PD0 + 1] = {NAME = "CAN_RX", MODE = "_GPIO_IN_FLOAT", STATE = "_LOW"}
    PD0[#PD0 + 1] = {NAME = "FSMC_D2", MODE = "_GPIO_ALT_OUT_PUSH_PULL_50MHZ", STATE = "_LOW"}
    PD0[#PD0 + 1] = {NAME = "OSC_IN", MODE = "_GPIO_IN_FLOAT", STATE = "_LOW"}

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
#define __GPIO_PD_PIN_0_MODE__ _GPIO_ANALOG

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
    PD1[#PD1 + 1] = {NAME = "PD1_NC", MODE = "_GPIO_ANALOG", STATE = "_LOW"}
    PD1[#PD1 + 1] = {NAME = "CAN1_TX", MODE = "_GPIO_ALT_OUT_PUSH_PULL_10MHZ", STATE = "_LOW"}
    PD1[#PD1 + 1] = {NAME = "CAN_TX", MODE = "_GPIO_ALT_OUT_PUSH_PULL_10MHZ", STATE = "_LOW"}
    PD1[#PD1 + 1] = {NAME = "FSMC_D3", MODE = "_GPIO_ALT_OUT_PUSH_PULL_50MHZ", STATE = "_LOW"}
    PD1[#PD1 + 1] = {NAME = "OSC_OUT", MODE = "_GPIO_IN_FLOAT", STATE = "_LOW"}

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
#define __GPIO_PD_PIN_1_MODE__ _GPIO_ANALOG

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
    PD2[#PD2 + 1] = {NAME = "PD2_NC", MODE = "_GPIO_ANALOG", STATE = "_LOW"}
    PD2[#PD2 + 1] = {NAME = "SDIO_CMD", MODE = "_GPIO_ALT_OUT_PUSH_PULL_50MHZ", STATE = "_LOW"}
    PD2[#PD2 + 1] = {NAME = "TIM3_ETR", MODE = "_GPIO_IN_FLOAT", STATE = "_LOW"}
    PD2[#PD2 + 1] = {NAME = "UART5_RX", MODE = "_GPIO_IN_PULLED", STATE = "_HIGH"}

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
#define __GPIO_PD_PIN_2_MODE__ _GPIO_ANALOG

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
    PD3[#PD3 + 1] = {NAME = "PD3_NC", MODE = "_GPIO_ANALOG", STATE = "_LOW"}
    PD3[#PD3 + 1] = {NAME = "FSMC_CLK", MODE = "_GPIO_ALT_OUT_PUSH_PULL_50MHZ", STATE = "_LOW"}
    PD3[#PD3 + 1] = {NAME = "USART2_CTS", MODE = "_GPIO_IN_PULLED", STATE = "_HIGH"}

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
#define __GPIO_PD_PIN_3_MODE__ _GPIO_ANALOG

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
    PD4[#PD4 + 1] = {NAME = "PD4_NC", MODE = "_GPIO_ANALOG", STATE = "_LOW"}
    PD4[#PD4 + 1] = {NAME = "FSMC_NOE", MODE = "_GPIO_ALT_OUT_PUSH_PULL_50MHZ", STATE = "_HIGH"}
    PD4[#PD4 + 1] = {NAME = "USART2_RTS", MODE = "_GPIO_ALT_OUT_PUSH_PULL_10MHZ", STATE = "_LOW"}

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
#define __GPIO_PD_PIN_4_MODE__ _GPIO_ANALOG

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
    PD5[#PD5 + 1] = {NAME = "PD5_NC", MODE = "_GPIO_ANALOG", STATE = "_LOW"}
    PD5[#PD5 + 1] = {NAME = "FSMC_NWE", MODE = "_GPIO_ALT_OUT_PUSH_PULL_50MHZ", STATE = "_HIGH"}
    PD5[#PD5 + 1] = {NAME = "USART2_TX", MODE = "_GPIO_ALT_OUT_PUSH_PULL_10MHZ", STATE = "_HIGH"}

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
#define __GPIO_PD_PIN_5_MODE__ _GPIO_ANALOG

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
    PD6[#PD6 + 1] = {NAME = "PD6_NC", MODE = "_GPIO_ANALOG", STATE = "_LOW"}
    PD6[#PD6 + 1] = {NAME = "FSMC_NWAIT", MODE = "_GPIO_IN_PULLED", STATE = "_HIGH"}
    PD6[#PD6 + 1] = {NAME = "USART2_RX", MODE = "_GPIO_IN_PULLED", STATE = "_HIGH"}

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
#define __GPIO_PD_PIN_6_MODE__ _GPIO_ANALOG

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
    PD7[#PD7 + 1] = {NAME = "PD7_NC", MODE = "_GPIO_ANALOG", STATE = "_LOW"}
    PD7[#PD7 + 1] = {NAME = "FSMC_NCE2", MODE = "_GPIO_ALT_OUT_PUSH_PULL_50MHZ", STATE = "_LOW"}
    PD7[#PD7 + 1] = {NAME = "FSMC_NE1", MODE = "_GPIO_ALT_OUT_PUSH_PULL_50MHZ", STATE = "_LOW"}
    PD7[#PD7 + 1] = {NAME = "USART2_CK", MODE = "_GPIO_ALT_OUT_PUSH_PULL_10MHZ", STATE = "_HIGH"}

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
#define __GPIO_PD_PIN_7_MODE__ _GPIO_ANALOG

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
    PD8[#PD8 + 1] = {NAME = "PD8_NC", MODE = "_GPIO_ANALOG", STATE = "_LOW"}
    PD8[#PD8 + 1] = {NAME = "ETH_MII_RX_DV", MODE = "_GPIO_IN_FLOAT", STATE = "_LOW"}
    PD8[#PD8 + 1] = {NAME = "ETH_RMII_CRS_DV", MODE = "_GPIO_IN_FLOAT", STATE = "_LOW"}
    PD8[#PD8 + 1] = {NAME = "FSMC_D13", MODE = "_GPIO_ALT_OUT_PUSH_PULL_50MHZ", STATE = "_LOW"}
    PD8[#PD8 + 1] = {NAME = "USART3_TX", MODE = "_GPIO_ALT_OUT_PUSH_PULL_10MHZ", STATE = "_HIGH"}

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
#define __GPIO_PD_PIN_8_MODE__ _GPIO_ANALOG

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
    PD9[#PD9 + 1] = {NAME = "PD9_NC", MODE = "_GPIO_ANALOG", STATE = "_LOW"}
    PD9[#PD9 + 1] = {NAME = "ETH_MII_RXD0", MODE = "_GPIO_IN_FLOAT", STATE = "_LOW"}
    PD9[#PD9 + 1] = {NAME = "ETH_RMII_RXD0", MODE = "_GPIO_IN_FLOAT", STATE = "_LOW"}
    PD9[#PD9 + 1] = {NAME = "FSMC_D14", MODE = "_GPIO_ALT_OUT_PUSH_PULL_50MHZ", STATE = "_LOW"}
    PD9[#PD9 + 1] = {NAME = "USART3_RX", MODE = "_GPIO_IN_PULLED", STATE = "_HIGH"}

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
#define __GPIO_PD_PIN_9_MODE__ _GPIO_ANALOG

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
    PD10[#PD10 + 1] = {NAME = "PD10_NC", MODE = "_GPIO_ANALOG", STATE = "_LOW"}
    PD10[#PD10 + 1] = {NAME = "ETH_MII_RXD1", MODE = "_GPIO_IN_FLOAT", STATE = "_LOW"}
    PD10[#PD10 + 1] = {NAME = "ETH_RMII_RXD1", MODE = "_GPIO_IN_FLOAT", STATE = "_LOW"}
    PD10[#PD10 + 1] = {NAME = "FSMC_D15", MODE = "_GPIO_ALT_OUT_PUSH_PULL_50MHZ", STATE = "_LOW"}
    PD10[#PD10 + 1] = {NAME = "USART3_CK", MODE = "_GPIO_ALT_OUT_PUSH_PULL_10MHZ", STATE = "_HIGH"}

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
#define __GPIO_PD_PIN_10_MODE__ _GPIO_ANALOG

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
    PD11[#PD11 + 1] = {NAME = "PD11_NC", MODE = "_GPIO_ANALOG", STATE = "_LOW"}
    PD11[#PD11 + 1] = {NAME = "ETH_MII_RXD2", MODE = "_GPIO_IN_FLOAT", STATE = "_LOW"}
    PD11[#PD11 + 1] = {NAME = "FSMC_A16", MODE = "_GPIO_ALT_OUT_PUSH_PULL_50MHZ", STATE = "_LOW"}
    PD11[#PD11 + 1] = {NAME = "USART3_CTS", MODE = "_GPIO_IN_PULLED", STATE = "_HIGH"}

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
#define __GPIO_PD_PIN_11_MODE__ _GPIO_ANALOG

/*--
if this:PinExist("D", 11) then
    this:AddWidget("Combobox")
    this:AddPinState("D", 11)
end
--*/
#define __GPIO_PD_PIN_11_STATE__ _FLOAT

/*--
if this:PinExist("D", 12) then
    local PD12 = {}
    PD12[#PD12 + 1] = {NAME = "PD12_NC", MODE = "_GPIO_ANALOG", STATE = "_LOW"}
    PD12[#PD12 + 1] = {NAME = "ETH_MII_RXD3", MODE = "_GPIO_IN_FLOAT", STATE = "_LOW"}
    PD12[#PD12 + 1] = {NAME = "FSMC_A17", MODE = "_GPIO_ALT_OUT_PUSH_PULL_50MHZ", STATE = "_LOW"}
    PD12[#PD12 + 1] = {NAME = "TIM4_CH1_IC", MODE = "_GPIO_IN_FLOAT", STATE = "_LOW"}
    PD12[#PD12 + 1] = {NAME = "TIM4_CH1_OC", MODE = "_GPIO_ALT_OUT_PUSH_PULL_10MHZ", STATE = "_LOW"}
    PD12[#PD12 + 1] = {NAME = "USART3_RTS", MODE = "_GPIO_ALT_OUT_PUSH_PULL_10MHZ", STATE = "_LOW"}

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
#define __GPIO_PD_PIN_12_MODE__ _GPIO_ANALOG

/*--
if this:PinExist("D", 12) then
    this:AddWidget("Combobox")
    this:AddPinState("D", 12)
end
--*/
#define __GPIO_PD_PIN_12_STATE__ _FLOAT

/*--
if this:PinExist("D", 13) then
    local PD13 = {}
    PD13[#PD13 + 1] = {NAME = "PD13_NC", MODE = "_GPIO_ANALOG", STATE = "_LOW"}
    PD13[#PD13 + 1] = {NAME = "FSMC_A18", MODE = "_GPIO_ALT_OUT_PUSH_PULL_50MHZ", STATE = "_LOW"}
    PD13[#PD13 + 1] = {NAME = "TIM4_CH2_IC", MODE = "_GPIO_IN_FLOAT", STATE = "_LOW"}
    PD13[#PD13 + 1] = {NAME = "TIM4_CH2_OC", MODE = "_GPIO_ALT_OUT_PUSH_PULL_10MHZ", STATE = "_LOW"}

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
#define __GPIO_PD_PIN_13_MODE__ _GPIO_ANALOG

/*--
if this:PinExist("D", 13) then
    this:AddWidget("Combobox")
    this:AddPinState("D", 13)
end
--*/
#define __GPIO_PD_PIN_13_STATE__ _FLOAT

/*--
if this:PinExist("D", 14) then
    local PD14 = {}
    PD14[#PD14 + 1] = {NAME = "PD14_NC", MODE = "_GPIO_ANALOG", STATE = "_LOW"}
    PD14[#PD14 + 1] = {NAME = "FSMC_D0", MODE = "_GPIO_ALT_OUT_PUSH_PULL_50MHZ", STATE = "_LOW"}
    PD14[#PD14 + 1] = {NAME = "TIM4_CH3_IC", MODE = "_GPIO_IN_FLOAT", STATE = "_LOW"}
    PD14[#PD14 + 1] = {NAME = "TIM4_CH3_OC", MODE = "_GPIO_ALT_OUT_PUSH_PULL_10MHZ", STATE = "_LOW"}

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
#define __GPIO_PD_PIN_14_MODE__ _GPIO_ANALOG

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
    PD15[#PD15 + 1] = {NAME = "PD15_NC", MODE = "_GPIO_ANALOG", STATE = "_LOW"}
    PD15[#PD15 + 1] = {NAME = "FSMC_D1", MODE = "_GPIO_ALT_OUT_PUSH_PULL_50MHZ", STATE = "_LOW"}
    PD15[#PD15 + 1] = {NAME = "TIM4_CH4_IC", MODE = "_GPIO_IN_FLOAT", STATE = "_LOW"}
    PD15[#PD15 + 1] = {NAME = "TIM4_CH4_OC", MODE = "_GPIO_ALT_OUT_PUSH_PULL_10MHZ", STATE = "_LOW"}

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
#define __GPIO_PD_PIN_15_MODE__ _GPIO_ANALOG

/*--
if this:PinExist("D", 15) then
    this:AddWidget("Combobox")
    this:AddPinState("D", 15)
    this:Finish()
end
--*/
#define __GPIO_PD_PIN_15_STATE__ _LOW



/*------------------------------------------------------------------------------
if this:PinExist("E", 0) then
    local PE0 = {}
    PE0[#PE0 + 1] = {NAME = "PE0_NC", MODE = "_GPIO_ANALOG", STATE = "_LOW"}
    PE0[#PE0 + 1] = {NAME = "FSMC_NBL0", MODE = "_GPIO_ALT_OUT_PUSH_PULL_50MHZ", STATE = "_LOW"}
    PE0[#PE0 + 1] = {NAME = "TIM4_ETR", MODE = "_GPIO_IN_FLOAT", STATE = "_LOW"}

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
#define __GPIO_PE_PIN_0_MODE__ _GPIO_ANALOG

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
    PE1[#PE1 + 1] = {NAME = "PE1_NC", MODE = "_GPIO_ANALOG", STATE = "_LOW"}
    PE1[#PE1 + 1] = {NAME = "FSMC_NBL1", MODE = "_GPIO_ALT_OUT_PUSH_PULL_50MHZ", STATE = "_LOW"}

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
#define __GPIO_PE_PIN_1_MODE__ _GPIO_ANALOG

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
    PE2[#PE2 + 1] = {NAME = "PE2_NC", MODE = "_GPIO_ANALOG", STATE = "_LOW"}
    PE2[#PE2 + 1] = {NAME = "FSMC_A23", MODE = "_GPIO_ALT_OUT_PUSH_PULL_50MHZ", STATE = "_LOW"}
    PE2[#PE2 + 1] = {NAME = "TRACECK", MODE = "_GPIO_IN_FLOAT", STATE = "_LOW"}

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
#define __GPIO_PE_PIN_2_MODE__ _GPIO_ANALOG

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
    PE3[#PE3 + 1] = {NAME = "PE3_NC", MODE = "_GPIO_ANALOG", STATE = "_LOW"}
    PE3[#PE3 + 1] = {NAME = "FSMC_A19", MODE = "_GPIO_ALT_OUT_PUSH_PULL_50MHZ", STATE = "_LOW"}
    PE3[#PE3 + 1] = {NAME = "TRACED0", MODE = "_GPIO_IN_FLOAT", STATE = "_LOW"}

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
#define __GPIO_PE_PIN_3_MODE__ _GPIO_ANALOG

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
    PE4[#PE4 + 1] = {NAME = "PE4_NC", MODE = "_GPIO_ANALOG", STATE = "_LOW"}
    PE4[#PE4 + 1] = {NAME = "FSMC_A20", MODE = "_GPIO_ALT_OUT_PUSH_PULL_50MHZ", STATE = "_LOW"}
    PE4[#PE4 + 1] = {NAME = "TRACED1", MODE = "_GPIO_IN_FLOAT", STATE = "_LOW"}

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
#define __GPIO_PE_PIN_4_MODE__ _GPIO_ANALOG

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
    PE5[#PE5 + 1] = {NAME = "PE5_NC", MODE = "_GPIO_ANALOG", STATE = "_LOW"}
    PE5[#PE5 + 1] = {NAME = "FSMC_A21", MODE = "_GPIO_ALT_OUT_PUSH_PULL_50MHZ", STATE = "_LOW"}
    PE5[#PE5 + 1] = {NAME = "TIM9_CH1_IC", MODE = "_GPIO_IN_FLOAT", STATE = "_LOW"}
    PE5[#PE5 + 1] = {NAME = "TIM9_CH1_OC", MODE = "_GPIO_ALT_OUT_PUSH_PULL_10MHZ", STATE = "_LOW"}
    PE5[#PE5 + 1] = {NAME = "TRACED2", MODE = "_GPIO_IN_FLOAT", STATE = "_LOW"}

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
#define __GPIO_PE_PIN_5_MODE__ _GPIO_ANALOG

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
    PE6[#PE6 + 1] = {NAME = "PE6_NC", MODE = "_GPIO_ANALOG", STATE = "_LOW"}
    PE6[#PE6 + 1] = {NAME = "FSMC_A22", MODE = "_GPIO_ALT_OUT_PUSH_PULL_50MHZ", STATE = "_LOW"}
    PE6[#PE6 + 1] = {NAME = "TIM9_CH2_IC", MODE = "_GPIO_IN_FLOAT", STATE = "_LOW"}
    PE6[#PE6 + 1] = {NAME = "TIM9_CH2_OC", MODE = "_GPIO_ALT_OUT_PUSH_PULL_10MHZ", STATE = "_LOW"}
    PE6[#PE6 + 1] = {NAME = "TRACED3", MODE = "_GPIO_IN_FLOAT", STATE = "_LOW"}

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
#define __GPIO_PE_PIN_6_MODE__ _GPIO_ANALOG

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
    PE7[#PE7 + 1] = {NAME = "PE7_NC", MODE = "_GPIO_ANALOG", STATE = "_LOW"}
    PE7[#PE7 + 1] = {NAME = "FSMC_D4", MODE = "_GPIO_ALT_OUT_PUSH_PULL_50MHZ", STATE = "_LOW"}
    PE7[#PE7 + 1] = {NAME = "TIM1_ETR", MODE = "_GPIO_IN_FLOAT", STATE = "_LOW"}

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
#define __GPIO_PE_PIN_7_MODE__ _GPIO_ANALOG

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
    PE8[#PE8 + 1] = {NAME = "PE8_NC", MODE = "_GPIO_ANALOG", STATE = "_LOW"}
    PE8[#PE8 + 1] = {NAME = "FSMC_D5", MODE = "_GPIO_ALT_OUT_PUSH_PULL_50MHZ", STATE = "_LOW"}
    PE8[#PE8 + 1] = {NAME = "TIM1_CH1N", MODE = "_GPIO_ALT_OUT_PUSH_PULL_10MHZ", STATE = "_LOW"}

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
#define __GPIO_PE_PIN_8_MODE__ _GPIO_ANALOG

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
    PE9[#PE9 + 1] = {NAME = "PE9_NC", MODE = "_GPIO_ANALOG", STATE = "_LOW"}
    PE9[#PE9 + 1] = {NAME = "FSMC_D6", MODE = "_GPIO_ALT_OUT_PUSH_PULL_50MHZ", STATE = "_LOW"}
    PE9[#PE9 + 1] = {NAME = "TIM1_CH1_IC", MODE = "_GPIO_IN_FLOAT", STATE = "_LOW"}
    PE9[#PE9 + 1] = {NAME = "TIM1_CH1_OC", MODE = "_GPIO_ALT_OUT_PUSH_PULL_10MHZ", STATE = "_LOW"}

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
#define __GPIO_PE_PIN_9_MODE__ _GPIO_ANALOG

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
    PE10[#PE10 + 1] = {NAME = "PE10_NC", MODE = "_GPIO_ANALOG", STATE = "_LOW"}
    PE10[#PE10 + 1] = {NAME = "FSMC_D7", MODE = "_GPIO_ALT_OUT_PUSH_PULL_50MHZ", STATE = "_LOW"}
    PE10[#PE10 + 1] = {NAME = "TIM1_CH2N", MODE = "_GPIO_ALT_OUT_PUSH_PULL_10MHZ", STATE = "_LOW"}

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
#define __GPIO_PE_PIN_10_MODE__ _GPIO_ANALOG

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
    PE11[#PE11 + 1] = {NAME = "PE11_NC", MODE = "_GPIO_ANALOG", STATE = "_LOW"}
    PE11[#PE11 + 1] = {NAME = "FSMC_D8", MODE = "_GPIO_ALT_OUT_PUSH_PULL_50MHZ", STATE = "_LOW"}
    PE11[#PE11 + 1] = {NAME = "TIM1_CH2_IC", MODE = "_GPIO_IN_FLOAT", STATE = "_LOW"}
    PE11[#PE11 + 1] = {NAME = "TIM1_CH2_OC", MODE = "_GPIO_ALT_OUT_PUSH_PULL_10MHZ", STATE = "_LOW"}

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
#define __GPIO_PE_PIN_11_MODE__ _GPIO_ANALOG

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
    PE12[#PE12 + 1] = {NAME = "PE12_NC", MODE = "_GPIO_ANALOG", STATE = "_LOW"}
    PE12[#PE12 + 1] = {NAME = "FSMC_D9", MODE = "_GPIO_ALT_OUT_PUSH_PULL_50MHZ", STATE = "_LOW"}
    PE12[#PE12 + 1] = {NAME = "TIM1_CH3N", MODE = "_GPIO_ALT_OUT_PUSH_PULL_10MHZ", STATE = "_LOW"}

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
#define __GPIO_PE_PIN_12_MODE__ _GPIO_ANALOG

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
    PE13[#PE13 + 1] = {NAME = "PE13_NC", MODE = "_GPIO_ANALOG", STATE = "_LOW"}
    PE13[#PE13 + 1] = {NAME = "FSMC_D10", MODE = "_GPIO_ALT_OUT_PUSH_PULL_50MHZ", STATE = "_LOW"}
    PE13[#PE13 + 1] = {NAME = "TIM1_CH3_IC", MODE = "_GPIO_IN_FLOAT", STATE = "_LOW"}
    PE13[#PE13 + 1] = {NAME = "TIM1_CH3_OC", MODE = "_GPIO_ALT_OUT_PUSH_PULL_10MHZ", STATE = "_LOW"}

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
#define __GPIO_PE_PIN_13_MODE__ _GPIO_ANALOG

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
    PE14[#PE14 + 1] = {NAME = "PE14_NC", MODE = "_GPIO_ANALOG", STATE = "_LOW"}
    PE14[#PE14 + 1] = {NAME = "FSMC_D11", MODE = "_GPIO_ALT_OUT_PUSH_PULL_50MHZ", STATE = "_LOW"}
    PE14[#PE14 + 1] = {NAME = "TIM1_CH4_IC", MODE = "_GPIO_IN_FLOAT", STATE = "_LOW"}
    PE14[#PE14 + 1] = {NAME = "TIM1_CH4_OC", MODE = "_GPIO_ALT_OUT_PUSH_PULL_10MHZ", STATE = "_LOW"}

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
#define __GPIO_PE_PIN_14_MODE__ _GPIO_ANALOG

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
    PE15[#PE15 + 1] = {NAME = "PE15_NC", MODE = "_GPIO_ANALOG", STATE = "_LOW"}
    PE15[#PE15 + 1] = {NAME = "FSMC_D12", MODE = "_GPIO_ALT_OUT_PUSH_PULL_50MHZ", STATE = "_LOW"}
    PE15[#PE15 + 1] = {NAME = "TIM1_BKIN", MODE = "_GPIO_IN_FLOAT", STATE = "_LOW"}

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
#define __GPIO_PE_PIN_15_MODE__ _GPIO_ANALOG

/*--
if this:PinExist("E", 15) then
    this:AddWidget("Combobox")
    this:AddPinState("E", 15)
    this:Finish()
end
--*/
#define __GPIO_PE_PIN_15_STATE__ _LOW


/*------------------------------------------------------------------------------
if this:PinExist("F", 0) then
    local PF0 = {}
    PF0[#PF0 + 1] = {NAME = "PF0_NC", MODE = "_GPIO_ANALOG", STATE = "_LOW"}
    PF0[#PF0 + 1] = {NAME = "FSMC_A0", MODE = "_GPIO_ALT_OUT_PUSH_PULL_50MHZ", STATE = "_LOW"}

    this:AddWidget("Textbox", "PF0")
    this:AddProposals(PF0, "F", 0)
end
--*/
#define __GPIO_PF_PIN_0_NAME__ NC_GPIOF_0

/*--
if this:PinExist("F", 0) then
    this:AddWidget("Combobox")
    this:AddPinMode("F", 0)
end
--*/
#define __GPIO_PF_PIN_0_MODE__ _GPIO_IN_FLOAT

/*--
if this:PinExist("F", 0) then
    this:AddWidget("Combobox")
    this:AddPinState("F", 0)
end
--*/
#define __GPIO_PF_PIN_0_STATE__ _FLOAT

/*--
if this:PinExist("F", 1) then
    local PF1 = {}
    PF1[#PF1 + 1] = {NAME = "PF1_NC", MODE = "_GPIO_ANALOG", STATE = "_LOW"}
    PF1[#PF1 + 1] = {NAME = "FSMC_A1", MODE = "_GPIO_ALT_OUT_PUSH_PULL_50MHZ", STATE = "_LOW"}

    this:AddWidget("Textbox", "PF1")
    this:AddProposals(PF1, "F", 1)
end
--*/
#define __GPIO_PF_PIN_1_NAME__ NC_GPIOF_1

/*--
if this:PinExist("F", 1) then
    this:AddWidget("Combobox")
    this:AddPinMode("F", 1)
end
--*/
#define __GPIO_PF_PIN_1_MODE__ _GPIO_IN_FLOAT

/*--
if this:PinExist("F", 1) then
    this:AddWidget("Combobox")
    this:AddPinState("F", 1)
end
--*/
#define __GPIO_PF_PIN_1_STATE__ _FLOAT

/*--
if this:PinExist("F", 2) then
    local PF2 = {}
    PF2[#PF2 + 1] = {NAME = "PF2_NC", MODE = "_GPIO_ANALOG", STATE = "_LOW"}
    PF2[#PF2 + 1] = {NAME = "FSMC_A2", MODE = "_GPIO_ALT_OUT_PUSH_PULL_50MHZ", STATE = "_LOW"}

    this:AddWidget("Textbox", "PF2")
    this:AddProposals(PF2, "F", 2)
end
--*/
#define __GPIO_PF_PIN_2_NAME__ NC_GPIOF_2

/*--
if this:PinExist("F", 2) then
    this:AddWidget("Combobox")
    this:AddPinMode("F", 2)
end
--*/
#define __GPIO_PF_PIN_2_MODE__ _GPIO_IN_FLOAT

/*--
if this:PinExist("F", 2) then
    this:AddWidget("Combobox")
    this:AddPinState("F", 2)
end
--*/
#define __GPIO_PF_PIN_2_STATE__ _FLOAT

/*--
if this:PinExist("F", 3) then
    local PF3 = {}
    PF3[#PF3 + 1] = {NAME = "PF3_NC", MODE = "_GPIO_ANALOG", STATE = "_LOW"}
    PF3[#PF3 + 1] = {NAME = "FSMC_A3", MODE = "_GPIO_ALT_OUT_PUSH_PULL_50MHZ", STATE = "_LOW"}

    this:AddWidget("Textbox", "PF3")
    this:AddProposals(PF3, "F", 3)
end
--*/
#define __GPIO_PF_PIN_3_NAME__ NC_GPIOF_3

/*--
if this:PinExist("F", 3) then
    this:AddWidget("Combobox")
    this:AddPinMode("F", 3)
end
--*/
#define __GPIO_PF_PIN_3_MODE__ _GPIO_IN_FLOAT

/*--
if this:PinExist("F", 3) then
    this:AddWidget("Combobox")
    this:AddPinState("F", 3)
end
--*/
#define __GPIO_PF_PIN_3_STATE__ _FLOAT

/*--
if this:PinExist("F", 4) then
    local PF4 = {}
    PF4[#PF4 + 1] = {NAME = "PF4_NC", MODE = "_GPIO_ANALOG", STATE = "_LOW"}
    PF4[#PF4 + 1] = {NAME = "FSMC_A4", MODE = "_GPIO_ALT_OUT_PUSH_PULL_50MHZ", STATE = "_LOW"}

    this:AddWidget("Textbox", "PF4")
    this:AddProposals(PF4, "F", 4)
end
--*/
#define __GPIO_PF_PIN_4_NAME__ NC_GPIOF_4

/*--
if this:PinExist("F", 4) then
    this:AddWidget("Combobox")
    this:AddPinMode("F", 4)
end
--*/
#define __GPIO_PF_PIN_4_MODE__ _GPIO_IN_FLOAT

/*--
if this:PinExist("F", 4) then
    this:AddWidget("Combobox")
    this:AddPinState("F", 4)
end
--*/
#define __GPIO_PF_PIN_4_STATE__ _FLOAT

/*--
if this:PinExist("F", 5) then
    local PF5 = {}
    PF5[#PF5 + 1] = {NAME = "PF5_NC", MODE = "_GPIO_ANALOG", STATE = "_LOW"}
    PF5[#PF5 + 1] = {NAME = "FSMC_A5", MODE = "_GPIO_ALT_OUT_PUSH_PULL_50MHZ", STATE = "_LOW"}

    this:AddWidget("Textbox", "PF5")
    this:AddProposals(PF5, "F", 5)
end
--*/
#define __GPIO_PF_PIN_5_NAME__ NC_GPIOF_5

/*--
if this:PinExist("F", 5) then
    this:AddWidget("Combobox")
    this:AddPinMode("F", 5)
end
--*/
#define __GPIO_PF_PIN_5_MODE__ _GPIO_IN_FLOAT

/*--
if this:PinExist("F", 5) then
    this:AddWidget("Combobox")
    this:AddPinState("F", 5)
end
--*/
#define __GPIO_PF_PIN_5_STATE__ _FLOAT

/*--
if this:PinExist("F", 6) then
    local PF6 = {}
    PF6[#PF6 + 1] = {NAME = "PF6_NC", MODE = "_GPIO_ANALOG", STATE = "_LOW"}
    PF6[#PF6 + 1] = {NAME = "ADC3_IN4", MODE = "_GPIO_ANALOG", STATE = "_LOW"}
    PF6[#PF6 + 1] = {NAME = "FSMC_NIORD", MODE = "_GPIO_ALT_OUT_PUSH_PULL_50MHZ", STATE = "_HIGH"}
    PF6[#PF6 + 1] = {NAME = "TIM10_CH1_IC", MODE = "_GPIO_IN_FLOAT", STATE = "_LOW"}
    PF6[#PF6 + 1] = {NAME = "TIM10_CH1_OC", MODE = "_GPIO_ALT_OUT_PUSH_PULL_10MHZ", STATE = "_LOW"}

    this:AddWidget("Textbox", "PF6")
    this:AddProposals(PF6, "F", 6)
end
--*/
#define __GPIO_PF_PIN_6_NAME__ NC_GPIOF_6

/*--
if this:PinExist("F", 6) then
    this:AddWidget("Combobox")
    this:AddPinMode("F", 6)
end
--*/
#define __GPIO_PF_PIN_6_MODE__ _GPIO_IN_FLOAT

/*--
if this:PinExist("F", 6) then
    this:AddWidget("Combobox")
    this:AddPinState("F", 6)
end
--*/
#define __GPIO_PF_PIN_6_STATE__ _FLOAT

/*--
if this:PinExist("F", 7) then
    local PF7 = {}
    PF7[#PF7 + 1] = {NAME = "PF7_NC", MODE = "_GPIO_ANALOG", STATE = "_LOW"}
    PF7[#PF7 + 1] = {NAME = "ADC3_IN5", MODE = "_GPIO_ANALOG", STATE = "_LOW"}
    PF7[#PF7 + 1] = {NAME = "FSMC_NREG", MODE = "_GPIO_ALT_OUT_PUSH_PULL_50MHZ", STATE = "_HIGH"}
    PF7[#PF7 + 1] = {NAME = "TIM11_CH1_IC", MODE = "_GPIO_IN_FLOAT", STATE = "_LOW"}
    PF7[#PF7 + 1] = {NAME = "TIM11_CH1_OC", MODE = "_GPIO_ALT_OUT_PUSH_PULL_10MHZ", STATE = "_LOW"}

    this:AddWidget("Textbox", "PF7")
    this:AddProposals(PF7, "F", 7)
end
--*/
#define __GPIO_PF_PIN_7_NAME__ NC_GPIOF_7

/*--
if this:PinExist("F", 7) then
    this:AddWidget("Combobox")
    this:AddPinMode("F", 7)
end
--*/
#define __GPIO_PF_PIN_7_MODE__ _GPIO_IN_FLOAT

/*--
if this:PinExist("F", 7) then
    this:AddWidget("Combobox")
    this:AddPinState("F", 7)
end
--*/
#define __GPIO_PF_PIN_7_STATE__ _FLOAT

/*--
if this:PinExist("F", 8) then
    local PF8 = {}
    PF8[#PF8 + 1] = {NAME = "PF8_NC", MODE = "_GPIO_ANALOG", STATE = "_LOW"}
    PF8[#PF8 + 1] = {NAME = "ADC3_IN6", MODE = "_GPIO_ANALOG", STATE = "_LOW"}
    PF8[#PF8 + 1] = {NAME = "FSMC_NIOWR", MODE = "_GPIO_ALT_OUT_PUSH_PULL_50MHZ", STATE = "_HIGH"}
    PF8[#PF8 + 1] = {NAME = "TIM13_CH1_IC", MODE = "_GPIO_IN_FLOAT", STATE = "_LOW"}
    PF8[#PF8 + 1] = {NAME = "TIM13_CH1_OC", MODE = "_GPIO_ALT_OUT_PUSH_PULL_10MHZ", STATE = "_LOW"}

    this:AddWidget("Textbox", "PF8")
    this:AddProposals(PF8, "F", 8)
end
--*/
#define __GPIO_PF_PIN_8_NAME__ NC_GPIOF_8

/*--
if this:PinExist("F", 8) then
    this:AddWidget("Combobox")
    this:AddPinMode("F", 8)
end
--*/
#define __GPIO_PF_PIN_8_MODE__ _GPIO_IN_FLOAT

/*--
if this:PinExist("F", 8) then
    this:AddWidget("Combobox")
    this:AddPinState("F", 8)
end
--*/
#define __GPIO_PF_PIN_8_STATE__ _FLOAT

/*--
if this:PinExist("F", 9) then
    local PF9 = {}
    PF9[#PF9 + 1] = {NAME = "PF9_NC", MODE = "_GPIO_ANALOG", STATE = "_LOW"}
    PF9[#PF9 + 1] = {NAME = "ADC3_IN7", MODE = "_GPIO_ANALOG", STATE = "_LOW"}
    PF9[#PF9 + 1] = {NAME = "FSMC_CD", MODE = "_GPIO_IN_PULLED", STATE = "_HIGH"}
    PF9[#PF9 + 1] = {NAME = "TIM14_CH1_IC", MODE = "_GPIO_IN_FLOAT", STATE = "_LOW"}
    PF9[#PF9 + 1] = {NAME = "TIM14_CH1_OC", MODE = "_GPIO_ALT_OUT_PUSH_PULL_10MHZ", STATE = "_LOW"}

    this:AddWidget("Textbox", "PF9")
    this:AddProposals(PF9, "F", 9)
end
--*/
#define __GPIO_PF_PIN_9_NAME__ NC_GPIOF_9

/*--
if this:PinExist("F", 9) then
    this:AddWidget("Combobox")
    this:AddPinMode("F", 9)
end
--*/
#define __GPIO_PF_PIN_9_MODE__ _GPIO_IN_FLOAT

/*--
if this:PinExist("F", 9) then
    this:AddWidget("Combobox")
    this:AddPinState("F", 9)
end
--*/
#define __GPIO_PF_PIN_9_STATE__ _FLOAT

/*--
if this:PinExist("F", 10) then
    local PF10 = {}
    PF10[#PF10 + 1] = {NAME = "PF10_NC", MODE = "_GPIO_ANALOG", STATE = "_LOW"}
    PF10[#PF10 + 1] = {NAME = "ADC3_IN8", MODE = "_GPIO_ANALOG", STATE = "_LOW"}
    PF10[#PF10 + 1] = {NAME = "FSMC_INTR", MODE = "_GPIO_IN_FLOAT", STATE = "_LOW"}

    this:AddWidget("Textbox", "PF10")
    this:AddProposals(PF10, "F", 10)
end
--*/
#define __GPIO_PF_PIN_10_NAME__ NC_GPIOF_10

/*--
if this:PinExist("F", 10) then
    this:AddWidget("Combobox")
    this:AddPinMode("F", 10)
end
--*/
#define __GPIO_PF_PIN_10_MODE__ _GPIO_IN_FLOAT

/*--
if this:PinExist("F", 10) then
    this:AddWidget("Combobox")
    this:AddPinState("F", 10)
end
--*/
#define __GPIO_PF_PIN_10_STATE__ _FLOAT

/*--
if this:PinExist("F", 11) then
    local PF11 = {}
    PF11[#PF11 + 1] = {NAME = "PF11_NC", MODE = "_GPIO_ANALOG", STATE = "_LOW"}
    PF11[#PF11 + 1] = {NAME = "FSMC_NIOS16", MODE = "_GPIO_IN_FLOAT", STATE = "_LOW"}

    this:AddWidget("Textbox", "PF11")
    this:AddProposals(PF11, "F", 11)
end
--*/
#define __GPIO_PF_PIN_11_NAME__ NC_GPIOF_11

/*--
if this:PinExist("F", 11) then
    this:AddWidget("Combobox")
    this:AddPinMode("F", 11)
end
--*/
#define __GPIO_PF_PIN_11_MODE__ _GPIO_IN_FLOAT

/*--
if this:PinExist("F", 11) then
    this:AddWidget("Combobox")
    this:AddPinState("F", 11)
end
--*/
#define __GPIO_PF_PIN_11_STATE__ _FLOAT

/*--
if this:PinExist("F", 12) then
    local PF12 = {}
    PF12[#PF12 + 1] = {NAME = "PF12_NC", MODE = "_GPIO_ANALOG", STATE = "_LOW"}
    PF12[#PF12 + 1] = {NAME = "FSMC_A6", MODE = "_GPIO_ALT_OUT_PUSH_PULL_50MHZ", STATE = "_LOW"}

    this:AddWidget("Textbox", "PF12")
    this:AddProposals(PF12, "F", 12)
end
--*/
#define __GPIO_PF_PIN_12_NAME__ NC_GPIOF_12

/*--
if this:PinExist("F", 12) then
    this:AddWidget("Combobox")
    this:AddPinMode("F", 12)
end
--*/
#define __GPIO_PF_PIN_12_MODE__ _GPIO_IN_FLOAT

/*--
if this:PinExist("F", 12) then
    this:AddWidget("Combobox")
    this:AddPinState("F", 12)
end
--*/
#define __GPIO_PF_PIN_12_STATE__ _FLOAT

/*--
if this:PinExist("F", 13) then
    local PF13 = {}
    PF13[#PF13 + 1] = {NAME = "PF13_NC", MODE = "_GPIO_ANALOG", STATE = "_LOW"}
    PF13[#PF13 + 1] = {NAME = "FSMC_A7", MODE = "_GPIO_ALT_OUT_PUSH_PULL_50MHZ", STATE = "_LOW"}

    this:AddWidget("Textbox", "PF13")
    this:AddProposals(PF13, "F", 13)
end
--*/
#define __GPIO_PF_PIN_13_NAME__ NC_GPIOF_13

/*--
if this:PinExist("F", 13) then
    this:AddWidget("Combobox")
    this:AddPinMode("F", 13)
end
--*/
#define __GPIO_PF_PIN_13_MODE__ _GPIO_IN_FLOAT

/*--
if this:PinExist("F", 13) then
    this:AddWidget("Combobox")
    this:AddPinState("F", 13)
end
--*/
#define __GPIO_PF_PIN_13_STATE__ _FLOAT

/*--
if this:PinExist("F", 14) then
    local PF14 = {}
    PF14[#PF14 + 1] = {NAME = "PF14_NC", MODE = "_GPIO_ANALOG", STATE = "_LOW"}
    PF14[#PF14 + 1] = {NAME = "FSMC_A8", MODE = "_GPIO_ALT_OUT_PUSH_PULL_50MHZ", STATE = "_LOW"}

    this:AddWidget("Textbox", "PF14")
    this:AddProposals(PF14, "F", 14)
end
--*/
#define __GPIO_PF_PIN_14_NAME__ NC_GPIOF_14

/*--
if this:PinExist("F", 14) then
    this:AddWidget("Combobox")
    this:AddPinMode("F", 14)
end
--*/
#define __GPIO_PF_PIN_14_MODE__ _GPIO_IN_FLOAT

/*--
if this:PinExist("F", 14) then
    this:AddWidget("Combobox")
    this:AddPinState("F", 14)
end
--*/
#define __GPIO_PF_PIN_14_STATE__ _FLOAT

/*--
if this:PinExist("F", 15) then
    local PF15 = {}
    PF15[#PF15 + 1] = {NAME = "PF15_NC", MODE = "_GPIO_ANALOG", STATE = "_LOW"}
    PF15[#PF15 + 1] = {NAME = "FSMC_A9", MODE = "_GPIO_ALT_OUT_PUSH_PULL_50MHZ", STATE = "_LOW"}

    this:AddWidget("Textbox", "PF15")
    this:AddProposals(PF15, "F", 15)
end
--*/
#define __GPIO_PF_PIN_15_NAME__ NC_GPIOF_15

/*--
if this:PinExist("F", 15) then
    this:AddWidget("Combobox")
    this:AddPinMode("F", 15)
end
--*/
#define __GPIO_PF_PIN_15_MODE__ _GPIO_IN_FLOAT

/*--
if this:PinExist("F", 15) then
    this:AddWidget("Combobox")
    this:AddPinState("F", 15)
    this:Finish()
end
--*/
#define __GPIO_PF_PIN_15_STATE__ _FLOAT



/*------------------------------------------------------------------------------
if this:PinExist("G", 0) then
    local PG0 = {}
    PG0[#PG0 + 1] = {NAME = "PG0_NC", MODE = "_GPIO_ANALOG", STATE = "_LOW"}
    PG0[#PG0 + 1] = {NAME = "FSMC_A10", MODE = "_GPIO_ALT_OUT_PUSH_PULL_50MHZ", STATE = "_LOW"}

    this:AddWidget("Textbox", "PG0")
    this:AddProposals(PG0, "G", 0)
end
--*/
#define __GPIO_PG_PIN_0_NAME__ NC_GPIOG_0

/*--
if this:PinExist("G", 0) then
    this:AddWidget("Combobox")
    this:AddPinMode("G", 0)
end
--*/
#define __GPIO_PG_PIN_0_MODE__ _GPIO_IN_FLOAT

/*--
if this:PinExist("G", 0) then
    this:AddWidget("Combobox")
    this:AddPinState("G", 0)
end
--*/
#define __GPIO_PG_PIN_0_STATE__ _FLOAT

/*--
if this:PinExist("G", 1) then
    local PG1 = {}
    PG1[#PG1 + 1] = {NAME = "PG1_NC", MODE = "_GPIO_ANALOG", STATE = "_LOW"}
    PG1[#PG1 + 1] = {NAME = "FSMC_A11", MODE = "_GPIO_ALT_OUT_PUSH_PULL_50MHZ", STATE = "_LOW"}

    this:AddWidget("Textbox", "PG1")
    this:AddProposals(PG1, "G", 1)
end
--*/
#define __GPIO_PG_PIN_1_NAME__ NC_GPIOG_1

/*--
if this:PinExist("G", 1) then
    this:AddWidget("Combobox")
    this:AddPinMode("G", 1)
end
--*/
#define __GPIO_PG_PIN_1_MODE__ _GPIO_IN_FLOAT

/*--
if this:PinExist("G", 1) then
    this:AddWidget("Combobox")
    this:AddPinState("G", 1)
end
--*/
#define __GPIO_PG_PIN_1_STATE__ _FLOAT

/*--
if this:PinExist("G", 2) then
    local PG2 = {}
    PG2[#PG2 + 1] = {NAME = "PG2_NC", MODE = "_GPIO_ANALOG", STATE = "_LOW"}
    PG2[#PG2 + 1] = {NAME = "FSMC_A12", MODE = "_GPIO_ALT_OUT_PUSH_PULL_50MHZ", STATE = "_LOW"}

    this:AddWidget("Textbox", "PG2")
    this:AddProposals(PG2, "G", 2)
end
--*/
#define __GPIO_PG_PIN_2_NAME__ NC_GPIOG_2

/*--
if this:PinExist("G", 2) then
    this:AddWidget("Combobox")
    this:AddPinMode("G", 2)
end
--*/
#define __GPIO_PG_PIN_2_MODE__ _GPIO_IN_FLOAT

/*--
if this:PinExist("G", 2) then
    this:AddWidget("Combobox")
    this:AddPinState("G", 2)
end
--*/
#define __GPIO_PG_PIN_2_STATE__ _FLOAT

/*--
if this:PinExist("G", 3) then
    local PG3 = {}
    PG3[#PG3 + 1] = {NAME = "PG3_NC", MODE = "_GPIO_ANALOG", STATE = "_LOW"}
    PG3[#PG3 + 1] = {NAME = "FSMC_A13", MODE = "_GPIO_ALT_OUT_PUSH_PULL_50MHZ", STATE = "_LOW"}

    this:AddWidget("Textbox", "PG3")
    this:AddProposals(PG3, "G", 3)
end
--*/
#define __GPIO_PG_PIN_3_NAME__ NC_GPIOG_3

/*--
if this:PinExist("G", 3) then
    this:AddWidget("Combobox")
    this:AddPinMode("G", 3)
end
--*/
#define __GPIO_PG_PIN_3_MODE__ _GPIO_IN_FLOAT

/*--
if this:PinExist("G", 3) then
    this:AddWidget("Combobox")
    this:AddPinState("G", 3)
end
--*/
#define __GPIO_PG_PIN_3_STATE__ _FLOAT

/*--
if this:PinExist("G", 4) then
    local PG4 = {}
    PG4[#PG4 + 1] = {NAME = "PG4_NC", MODE = "_GPIO_ANALOG", STATE = "_LOW"}
    PG4[#PG4 + 1] = {NAME = "FSMC_A14", MODE = "_GPIO_ALT_OUT_PUSH_PULL_50MHZ", STATE = "_LOW"}

    this:AddWidget("Textbox", "PG4")
    this:AddProposals(PG4, "G", 4)
end
--*/
#define __GPIO_PG_PIN_4_NAME__ NC_GPIOG_4

/*--
if this:PinExist("G", 4) then
    this:AddWidget("Combobox")
    this:AddPinMode("G", 4)
end
--*/
#define __GPIO_PG_PIN_4_MODE__ _GPIO_IN_FLOAT

/*--
if this:PinExist("G", 4) then
    this:AddWidget("Combobox")
    this:AddPinState("G", 4)
end
--*/
#define __GPIO_PG_PIN_4_STATE__ _FLOAT

/*--
if this:PinExist("G", 5) then
    local PG5 = {}
    PG5[#PG5 + 1] = {NAME = "PG5_NC", MODE = "_GPIO_ANALOG", STATE = "_LOW"}
    PG5[#PG5 + 1] = {NAME = "FSMC_A15", MODE = "_GPIO_ALT_OUT_PUSH_PULL_50MHZ", STATE = "_LOW"}

    this:AddWidget("Textbox", "PG5")
    this:AddProposals(PG5, "G", 5)
end
--*/
#define __GPIO_PG_PIN_5_NAME__ NC_GPIOG_5

/*--
if this:PinExist("G", 5) then
    this:AddWidget("Combobox")
    this:AddPinMode("G", 5)
end
--*/
#define __GPIO_PG_PIN_5_MODE__ _GPIO_IN_FLOAT

/*--
if this:PinExist("G", 5) then
    this:AddWidget("Combobox")
    this:AddPinState("G", 5)
end
--*/
#define __GPIO_PG_PIN_5_STATE__ _FLOAT

/*--
if this:PinExist("G", 6) then
    local PG6 = {}
    PG6[#PG6 + 1] = {NAME = "PG6_NC", MODE = "_GPIO_ANALOG", STATE = "_LOW"}
    PG6[#PG6 + 1] = {NAME = "FSMC_INT2", MODE = "_GPIO_IN_FLOAT", STATE = "_LOW"}

    this:AddWidget("Textbox", "PG6")
    this:AddProposals(PG6, "G", 6)
end
--*/
#define __GPIO_PG_PIN_6_NAME__ NC_GPIOG_6

/*--
if this:PinExist("G", 6) then
    this:AddWidget("Combobox")
    this:AddPinMode("G", 6)
end
--*/
#define __GPIO_PG_PIN_6_MODE__ _GPIO_IN_FLOAT

/*--
if this:PinExist("G", 6) then
    this:AddWidget("Combobox")
    this:AddPinState("G", 6)
end
--*/
#define __GPIO_PG_PIN_6_STATE__ _FLOAT

/*--
if this:PinExist("G", 7) then
    local PG7 = {}
    PG7[#PG7 + 1] = {NAME = "PG7_NC", MODE = "_GPIO_ANALOG", STATE = "_LOW"}
    PG7[#PG7 + 1] = {NAME = "FSMC_INT3", MODE = "_GPIO_IN_FLOAT", STATE = "_LOW"}

    this:AddWidget("Textbox", "PG7")
    this:AddProposals(PG7, "G", 7)
end
--*/
#define __GPIO_PG_PIN_7_NAME__ NC_GPIOG_7

/*--
if this:PinExist("G", 7) then
    this:AddWidget("Combobox")
    this:AddPinMode("G", 7)
end
--*/
#define __GPIO_PG_PIN_7_MODE__ _GPIO_IN_FLOAT

/*--
if this:PinExist("G", 7) then
    this:AddWidget("Combobox")
    this:AddPinState("G", 7)
end
--*/
#define __GPIO_PG_PIN_7_STATE__ _FLOAT

/*--
if this:PinExist("G", 8) then
    local PG8 = {}
    PG8[#PG8 + 1] = {NAME = "PG8_NC", MODE = "_GPIO_ANALOG", STATE = "_LOW"}

    this:AddWidget("Textbox", "PG8")
    this:AddProposals(PG8, "G", 8)
end
--*/
#define __GPIO_PG_PIN_8_NAME__ NC_GPIOG_8

/*--
if this:PinExist("G", 8) then
    this:AddWidget("Combobox")
    this:AddPinMode("G", 8)
end
--*/
#define __GPIO_PG_PIN_8_MODE__ _GPIO_IN_FLOAT

/*--
if this:PinExist("G", 8) then
    this:AddWidget("Combobox")
    this:AddPinState("G", 8)
end
--*/
#define __GPIO_PG_PIN_8_STATE__ _FLOAT

/*--
if this:PinExist("G", 9) then
    local PG9 = {}
    PG9[#PG9 + 1] = {NAME = "PG9_NC", MODE = "_GPIO_ANALOG", STATE = "_LOW"}
    PG9[#PG9 + 1] = {NAME = "FSMC_NCE3", MODE = "_GPIO_ALT_OUT_PUSH_PULL_50MHZ", STATE = "_LOW"}
    PG9[#PG9 + 1] = {NAME = "FSMC_NE2", MODE = "_GPIO_ALT_OUT_PUSH_PULL_50MHZ", STATE = "_LOW"}

    this:AddWidget("Textbox", "PG9")
    this:AddProposals(PG9, "G", 9)
end
--*/
#define __GPIO_PG_PIN_9_NAME__ NC_GPIOG_9

/*--
if this:PinExist("G", 9) then
    this:AddWidget("Combobox")
    this:AddPinMode("G", 9)
end
--*/
#define __GPIO_PG_PIN_9_MODE__ _GPIO_IN_FLOAT

/*--
if this:PinExist("G", 9) then
    this:AddWidget("Combobox")
    this:AddPinState("G", 9)
end
--*/
#define __GPIO_PG_PIN_9_STATE__ _FLOAT

/*--
if this:PinExist("G", 10) then
    local PG10 = {}
    PG10[#PG10 + 1] = {NAME = "PG10_NC", MODE = "_GPIO_ANALOG", STATE = "_LOW"}
    PG10[#PG10 + 1] = {NAME = "FSMC_NCE4_1", MODE = "_GPIO_ALT_OUT_PUSH_PULL_50MHZ", STATE = "_LOW"}
    PG10[#PG10 + 1] = {NAME = "FSMC_NE3", MODE = "_GPIO_ALT_OUT_PUSH_PULL_50MHZ", STATE = "_LOW"}

    this:AddWidget("Textbox", "PG10")
    this:AddProposals(PG10, "G", 10)
end
--*/
#define __GPIO_PG_PIN_10_NAME__ NC_GPIOG_10

/*--
if this:PinExist("G", 10) then
    this:AddWidget("Combobox")
    this:AddPinMode("G", 10)
end
--*/
#define __GPIO_PG_PIN_10_MODE__ _GPIO_IN_FLOAT

/*--
if this:PinExist("G", 10) then
    this:AddWidget("Combobox")
    this:AddPinState("G", 10)
end
--*/
#define __GPIO_PG_PIN_10_STATE__ _FLOAT

/*--
if this:PinExist("G", 11) then
    local PG11 = {}
    PG11[#PG11 + 1] = {NAME = "PG11_NC", MODE = "_GPIO_ANALOG", STATE = "_LOW"}
    PG11[#PG11 + 1] = {NAME = "FSMC_NCE4_2", MODE = "_GPIO_ALT_OUT_PUSH_PULL_50MHZ", STATE = "_LOW"}

    this:AddWidget("Textbox", "PG11")
    this:AddProposals(PG11, "G", 11)
end
--*/
#define __GPIO_PG_PIN_11_NAME__ NC_GPIOG_11

/*--
if this:PinExist("G", 11) then
    this:AddWidget("Combobox")
    this:AddPinMode("G", 11)
end
--*/
#define __GPIO_PG_PIN_11_MODE__ _GPIO_IN_FLOAT

/*--
if this:PinExist("G", 11) then
    this:AddWidget("Combobox")
    this:AddPinState("G", 11)
end
--*/
#define __GPIO_PG_PIN_11_STATE__ _FLOAT

/*--
if this:PinExist("G", 12) then
    local PG12 = {}
    PG12[#PG12 + 1] = {NAME = "PG12_NC", MODE = "_GPIO_ANALOG", STATE = "_LOW"}
    PG12[#PG12 + 1] = {NAME = "FSMC_NE4", MODE = "_GPIO_ALT_OUT_PUSH_PULL_50MHZ", STATE = "_LOW"}

    this:AddWidget("Textbox", "PG12")
    this:AddProposals(PG12, "G", 12)
end
--*/
#define __GPIO_PG_PIN_12_NAME__ NC_GPIOG_12

/*--
if this:PinExist("G", 12) then
    this:AddWidget("Combobox")
    this:AddPinMode("G", 12)
end
--*/
#define __GPIO_PG_PIN_12_MODE__ _GPIO_IN_FLOAT

/*--
if this:PinExist("G", 12) then
    this:AddWidget("Combobox")
    this:AddPinState("G", 12)
end
--*/
#define __GPIO_PG_PIN_12_STATE__ _FLOAT

/*--
if this:PinExist("G", 13) then
    local PG13 = {}
    PG13[#PG13 + 1] = {NAME = "PG13_NC", MODE = "_GPIO_ANALOG", STATE = "_LOW"}
    PG13[#PG13 + 1] = {NAME = "FSMC_A24", MODE = "_GPIO_ALT_OUT_PUSH_PULL_50MHZ", STATE = "_LOW"}

    this:AddWidget("Textbox", "PG13")
    this:AddProposals(PG13, "G", 13)
end
--*/
#define __GPIO_PG_PIN_13_NAME__ NC_GPIOG_13

/*--
if this:PinExist("G", 13) then
    this:AddWidget("Combobox")
    this:AddPinMode("G", 13)
end
--*/
#define __GPIO_PG_PIN_13_MODE__ _GPIO_IN_FLOAT

/*--
if this:PinExist("G", 13) then
    this:AddWidget("Combobox")
    this:AddPinState("G", 13)
end
--*/
#define __GPIO_PG_PIN_13_STATE__ _FLOAT

/*--
if this:PinExist("G", 14) then
    local PG14 = {}
    PG14[#PG14 + 1] = {NAME = "PG14_NC", MODE = "_GPIO_ANALOG", STATE = "_LOW"}
    PG14[#PG14 + 1] = {NAME = "FSMC_A25", MODE = "_GPIO_ALT_OUT_PUSH_PULL_50MHZ", STATE = "_LOW"}

    this:AddWidget("Textbox", "PG14")
    this:AddProposals(PG14, "G", 14)
end
--*/
#define __GPIO_PG_PIN_14_NAME__ NC_GPIOG_14

/*--
if this:PinExist("G", 14) then
    this:AddWidget("Combobox")
    this:AddPinMode("G", 14)
end
--*/
#define __GPIO_PG_PIN_14_MODE__ _GPIO_IN_FLOAT

/*--
if this:PinExist("G", 14) then
    this:AddWidget("Combobox")
    this:AddPinState("G", 14)
end
--*/
#define __GPIO_PG_PIN_14_STATE__ _FLOAT

/*--
if this:PinExist("G", 15) then
    local PG15 = {}
    PG15[#PG15 + 1] = {NAME = "PG15_NC", MODE = "_GPIO_ANALOG", STATE = "_LOW"}

    this:AddWidget("Textbox", "PG15")
    this:AddProposals(PG15, "G", 15)
end
--*/
#define __GPIO_PG_PIN_15_NAME__ NC_GPIOG_15

/*--
if this:PinExist("G", 15) then
    this:AddWidget("Combobox")
    this:AddPinMode("G", 15)
end
--*/
#define __GPIO_PG_PIN_15_MODE__ _GPIO_IN_FLOAT

/*--
if this:PinExist("G", 15) then
    this:AddWidget("Combobox")
    this:AddPinState("G", 15)
    this:Finish()
end
--*/
#define __GPIO_PG_PIN_15_STATE__ _FLOAT

#endif /* _GPIO_FLAGS_H_ */
/*==============================================================================
  End of file
==============================================================================*/
