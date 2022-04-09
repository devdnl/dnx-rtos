#/*=============================================================================
# @file    arch_flags.h
#
# @author  Daniel Zorychta
#
# @brief   This file contains list of existing modules
#          Hybrid file: included both by Make and CC.
#
# @note    Copyright (C) 2017 Daniel Zorychta <daniel.zorychta@gmail.com>
#
#          This program is free software; you can redistribute it and/or modify
#          it under the terms of the GNU General Public License as published by
#          the Free Software Foundation and modified by the dnx RTOS exception.
#
#          NOTE: The modification  to the GPL is  included to allow you to
#                distribute a combined work that includes dnx RTOS without
#                being obliged to provide the source  code for proprietary
#                components outside of the dnx RTOS.
#
#          The dnx RTOS  is  distributed  in the hope  that  it will be useful,
#          but WITHOUT  ANY  WARRANTY;  without  even  the implied  warranty of
#          MERCHANTABILITY  or  FITNESS  FOR  A  PARTICULAR  PURPOSE.  See  the
#          GNU General Public License for more details.
#
#          Full license text is available on the following file: doc/license.txt.
#
#
#=============================================================================*/

#/*
#* NOTE: All flags defined as: __FLAG_NAME__ (with doubled underscore as suffix
#*       and prefix) are exported to the single configuration file
#*       (by using Configtool) when entire project configuration is exported.
#*       All other flag definitions and statements are ignored.
#*/

#ifndef _ARCH_FLAGS_H_
#define _ARCH_FLAGS_H_

#/*
# Modules in this list are disabled on import event. Function can be found in
# the config.h file.
#*/

#/*--
# this:SetLayout("TitledGridBack", 2, "Home > Microcontroller",
#                function() this:LoadFile("config.h") end)
#
#
# this.PutWidgets = function(this, name, file)
#     if type(name) == "string" then
#         this:AddWidget("Checkbox", "Enable "..name.." module")
#
#         if type(file) == "string" then
#             this:AddExtraWidget("Hyperlink", "HL_"..name, "Configure")
#             this:SetEvent("clicked", "HL_"..name, function() this:LoadFile(file) end)
#         else
#             this:AddExtraWidget("Void", "Void_"..name)
#         end
#     end
# end
#
# -- variable is cleared when architecture is selecting (may not be selected)
# -- variable is visible in all modules configuration
# uC = {}
#
# uC.ARCH    = this:GetFlagValue("__CPU_ARCH__", "project/project_flags.h")
# uC.OSCFREQ = this:GetFlagValue("__CPU_OSC_FREQ__", "project/project_flags.h")
# uC.NAME    = this:GetFlagValue("__CPU_NAME__", "arch/"..uC.ARCH.."/cpu_flags.h")
# uC.FAMILY  = this:GetFlagValue("__CPU_FAMILY__", "arch/"..uC.ARCH.."/cpu_flags.h"):match("_(.*)_")
# uC.PERIPH  = {}
#
# if uC.ARCH == "stm32f1" then
#     uC.AddPriorityItems = function(this, no_default)
#         this:AddItem("Priority 0 (the highest)", "0")
#         for i = 1, 14 do this:AddItem("Priority "..i, tostring(i)) end
#         this:AddItem("Priority 15 (the lowest, safe for kernel API)", "15")
#         if no_default ~= true then
#             this:AddItem("Default priority", "__CPU_DEFAULT_IRQ_PRIORITY__")
#         end
#     end
#
#     uC.PERIPH["STM32F100C8xx"] = {GPIO = true, CLK = true, CRC = true, DMA = true, PWM = true, WDG = true, UART = true, SPI = true, AFM = true, IRQ = true, I2C = true, NVM = true, RTC = true}
#     uC.PERIPH["STM32F100RBxx"] = {GPIO = true, CLK = true, CRC = true, DMA = true, PWM = true, WDG = true, UART = true, SPI = true, AFM = true, IRQ = true, I2C = true, NVM = true, RTC = true}
#     uC.PERIPH["STM32F100RExx"] = {GPIO = true, CLK = true, CRC = true, DMA = true, PWM = true, WDG = true, UART = true, SPI = true, AFM = true, IRQ = true, I2C = true, NVM = true, RTC = true}
#     uC.PERIPH["STM32F100VBxx"] = {GPIO = true, CLK = true, CRC = true, DMA = true, PWM = true, WDG = true, UART = true, SPI = true, AFM = true, IRQ = true, I2C = true, NVM = true, RTC = true}
#     uC.PERIPH["STM32F100ZCxx"] = {GPIO = true, CLK = true, CRC = true, DMA = true, PWM = true, WDG = true, UART = true, SPI = true, AFM = true, IRQ = true, I2C = true, NVM = true, RTC = true}
#     uC.PERIPH["STM32F101C8xx"] = {GPIO = true, CLK = true, CRC = true, DMA = true, PWM = true, WDG = true, UART = true, SPI = true, AFM = true, IRQ = true, I2C = true, NVM = true, RTC = true}
#     uC.PERIPH["STM32F101CBxx"] = {GPIO = true, CLK = true, CRC = true, DMA = true, PWM = true, WDG = true, UART = true, SPI = true, AFM = true, IRQ = true, I2C = true, NVM = true, RTC = true}
#     uC.PERIPH["STM32F101R8xx"] = {GPIO = true, CLK = true, CRC = true, DMA = true, PWM = true, WDG = true, UART = true, SPI = true, AFM = true, IRQ = true, I2C = true, NVM = true, RTC = true}
#     uC.PERIPH["STM32F101RBxx"] = {GPIO = true, CLK = true, CRC = true, DMA = true, PWM = true, WDG = true, UART = true, SPI = true, AFM = true, IRQ = true, I2C = true, NVM = true, RTC = true}
#     uC.PERIPH["STM32F101V8xx"] = {GPIO = true, CLK = true, CRC = true, DMA = true, PWM = true, WDG = true, UART = true, SPI = true, AFM = true, IRQ = true, I2C = true, NVM = true, RTC = true}
#     uC.PERIPH["STM32F101VBxx"] = {GPIO = true, CLK = true, CRC = true, DMA = true, PWM = true, WDG = true, UART = true, SPI = true, AFM = true, IRQ = true, I2C = true, NVM = true, RTC = true}
#     uC.PERIPH["STM32F103C6xx"] = {GPIO = true, CLK = true, CRC = true, DMA = true, PWM = true, WDG = true, UART = true, SPI = true, AFM = true, IRQ = true, I2C = true, NVM = true, RTC = true, CAN = true, USBD = true}
#     uC.PERIPH["STM32F103C8xx"] = {GPIO = true, CLK = true, CRC = true, DMA = true, PWM = true, WDG = true, UART = true, SPI = true, AFM = true, IRQ = true, I2C = true, NVM = true, RTC = true, CAN = true, USBD = true}
#     uC.PERIPH["STM32F103CBxx"] = {GPIO = true, CLK = true, CRC = true, DMA = true, PWM = true, WDG = true, UART = true, SPI = true, AFM = true, IRQ = true, I2C = true, NVM = true, RTC = true, CAN = true, USBD = true}
#     uC.PERIPH["STM32F103R6xx"] = {GPIO = true, CLK = true, CRC = true, DMA = true, PWM = true, WDG = true, UART = true, SPI = true, AFM = true, IRQ = true, I2C = true, NVM = true, RTC = true, CAN = true, USBD = true}
#     uC.PERIPH["STM32F103R8xx"] = {GPIO = true, CLK = true, CRC = true, DMA = true, PWM = true, WDG = true, UART = true, SPI = true, AFM = true, IRQ = true, I2C = true, NVM = true, RTC = true, CAN = true, USBD = true}
#     uC.PERIPH["STM32F103RBxx"] = {GPIO = true, CLK = true, CRC = true, DMA = true, PWM = true, WDG = true, UART = true, SPI = true, AFM = true, IRQ = true, I2C = true, NVM = true, RTC = true, CAN = true, USBD = true}
#     uC.PERIPH["STM32F103RCxx"] = {GPIO = true, CLK = true, CRC = true, DMA = true, PWM = true, WDG = true, UART = true, SPI = true, AFM = true, IRQ = true, I2C = true, NVM = true, RTC = true, CAN = true, USBD = true}
#     uC.PERIPH["STM32F103RDxx"] = {GPIO = true, CLK = true, CRC = true, DMA = true, PWM = true, WDG = true, UART = true, SPI = true, AFM = true, IRQ = true, I2C = true, NVM = true, RTC = true, CAN = true, USBD = true}
#     uC.PERIPH["STM32F103RExx"] = {GPIO = true, CLK = true, CRC = true, DMA = true, PWM = true, WDG = true, UART = true, SPI = true, AFM = true, IRQ = true, I2C = true, NVM = true, RTC = true, CAN = true, USBD = true}
#     uC.PERIPH["STM32F103RGxx"] = {GPIO = true, CLK = true, CRC = true, DMA = true, PWM = true, WDG = true, UART = true, SPI = true, AFM = true, IRQ = true, I2C = true, NVM = true, RTC = true, CAN = true, USBD = true}
#     uC.PERIPH["STM32F103T8xx"] = {GPIO = true, CLK = true, CRC = true, DMA = true, PWM = true, WDG = true, UART = true, SPI = true, AFM = true, IRQ = true, I2C = true, NVM = true, RTC = true, CAN = true, USBD = true}
#     uC.PERIPH["STM32F103TBxx"] = {GPIO = true, CLK = true, CRC = true, DMA = true, PWM = true, WDG = true, UART = true, SPI = true, AFM = true, IRQ = true, I2C = true, NVM = true, RTC = true, CAN = true, USBD = true}
#     uC.PERIPH["STM32F103V8xx"] = {GPIO = true, CLK = true, CRC = true, DMA = true, PWM = true, WDG = true, UART = true, SPI = true, AFM = true, IRQ = true, I2C = true, NVM = true, RTC = true, CAN = true, USBD = true}
#     uC.PERIPH["STM32F103VBxx"] = {GPIO = true, CLK = true, CRC = true, DMA = true, PWM = true, WDG = true, UART = true, SPI = true, AFM = true, IRQ = true, I2C = true, NVM = true, RTC = true, CAN = true, USBD = true}
#     uC.PERIPH["STM32F103VCxx"] = {GPIO = true, CLK = true, CRC = true, DMA = true, PWM = true, WDG = true, UART = true, SPI = true, AFM = true, IRQ = true, I2C = true, NVM = true, RTC = true, CAN = true, USBD = true}
#     uC.PERIPH["STM32F103VDxx"] = {GPIO = true, CLK = true, CRC = true, DMA = true, PWM = true, WDG = true, UART = true, SPI = true, AFM = true, IRQ = true, I2C = true, NVM = true, RTC = true, CAN = true, USBD = true}
#     uC.PERIPH["STM32F103VExx"] = {GPIO = true, CLK = true, CRC = true, DMA = true, PWM = true, WDG = true, UART = true, SPI = true, AFM = true, IRQ = true, I2C = true, NVM = true, RTC = true, CAN = true, USBD = true}
#     uC.PERIPH["STM32F103VGxx"] = {GPIO = true, CLK = true, CRC = true, DMA = true, PWM = true, WDG = true, UART = true, SPI = true, AFM = true, IRQ = true, I2C = true, NVM = true, RTC = true, CAN = true, USBD = true}
#     uC.PERIPH["STM32F103ZCxx"] = {GPIO = true, CLK = true, CRC = true, DMA = true, PWM = true, WDG = true, UART = true, SPI = true, AFM = true, IRQ = true, I2C = true, NVM = true, RTC = true, CAN = true, USBD = true}
#     uC.PERIPH["STM32F103ZExx"] = {GPIO = true, CLK = true, CRC = true, DMA = true, PWM = true, WDG = true, UART = true, SPI = true, AFM = true, IRQ = true, I2C = true, NVM = true, RTC = true, CAN = true, USBD = true}
#     uC.PERIPH["STM32F103ZGxx"] = {GPIO = true, CLK = true, CRC = true, DMA = true, PWM = true, WDG = true, UART = true, SPI = true, AFM = true, IRQ = true, I2C = true, NVM = true, RTC = true, CAN = true, USBD = true}
#     uC.PERIPH["STM32F105RBxx"] = {GPIO = true, CLK = true, CRC = true, DMA = true, PWM = true, WDG = true, UART = true, SPI = true, AFM = true, IRQ = true, I2C = true, NVM = true, RTC = true, CAN = true}
#     uC.PERIPH["STM32F105RCxx"] = {GPIO = true, CLK = true, CRC = true, DMA = true, PWM = true, WDG = true, UART = true, SPI = true, AFM = true, IRQ = true, I2C = true, NVM = true, RTC = true, CAN = true}
#     uC.PERIPH["STM32F105VBxx"] = {GPIO = true, CLK = true, CRC = true, DMA = true, PWM = true, WDG = true, UART = true, SPI = true, AFM = true, IRQ = true, I2C = true, NVM = true, RTC = true, CAN = true}
#     uC.PERIPH["STM32F105VCxx"] = {GPIO = true, CLK = true, CRC = true, DMA = true, PWM = true, WDG = true, UART = true, SPI = true, AFM = true, IRQ = true, I2C = true, NVM = true, RTC = true, CAN = true}
#     uC.PERIPH["STM32F107RCxx"] = {GPIO = true, CLK = true, CRC = true, DMA = true, PWM = true, WDG = true, UART = true, SPI = true, AFM = true, IRQ = true, I2C = true, NVM = true, RTC = true, CAN = true, ETH = true}
#     uC.PERIPH["STM32F107VBxx"] = {GPIO = true, CLK = true, CRC = true, DMA = true, PWM = true, WDG = true, UART = true, SPI = true, AFM = true, IRQ = true, I2C = true, NVM = true, RTC = true, CAN = true, ETH = true}
#     uC.PERIPH["STM32F107VCxx"] = {GPIO = true, CLK = true, CRC = true, DMA = true, PWM = true, WDG = true, UART = true, SPI = true, AFM = true, IRQ = true, I2C = true, NVM = true, RTC = true, CAN = true, ETH = true}
# end
#
# if uC.ARCH == "stm32f3" then
#     uC.AddPriorityItems = function(this, no_default)
#         this:AddItem("Priority 0 (the highest)", "0")
#         for i = 1, 14 do this:AddItem("Priority "..i, tostring(i)) end
#         this:AddItem("Priority 15 (the lowest, safe for kernel API)", "15")
#         if no_default ~= true then
#             this:AddItem("Default priority", "__CPU_DEFAULT_IRQ_PRIORITY__")
#         end
#     end
#
#     uC.PERIPH["STM32F301C6xx"] = {GPIO = true, CLK = true, I2C = true, IRQ = true, DMA = true, PWM = true, UART = true}
#     uC.PERIPH["STM32F301C8xx"] = {GPIO = true, CLK = true, I2C = true, IRQ = true, DMA = true, PWM = true, UART = true}
#     uC.PERIPH["STM32F301K6xx"] = {GPIO = true, CLK = true, I2C = true, IRQ = true, DMA = true, PWM = true, UART = true}
#     uC.PERIPH["STM32F301K8xx"] = {GPIO = true, CLK = true, I2C = true, IRQ = true, DMA = true, PWM = true, UART = true}
#     uC.PERIPH["STM32F301R6xx"] = {GPIO = true, CLK = true, I2C = true, IRQ = true, DMA = true, PWM = true, UART = true}
#     uC.PERIPH["STM32F301R8xx"] = {GPIO = true, CLK = true, I2C = true, IRQ = true, DMA = true, PWM = true, UART = true}
#     uC.PERIPH["STM32F302C6xx"] = {GPIO = true, CLK = true, I2C = true, IRQ = true, DMA = true, PWM = true, UART = true, CAN = true}
#     uC.PERIPH["STM32F302C8xx"] = {GPIO = true, CLK = true, I2C = true, IRQ = true, DMA = true, PWM = true, UART = true, CAN = true}
#     uC.PERIPH["STM32F302CBxx"] = {GPIO = true, CLK = true, I2C = true, IRQ = true, DMA = true, PWM = true, UART = true, CAN = true}
#     uC.PERIPH["STM32F302CCxx"] = {GPIO = true, CLK = true, I2C = true, IRQ = true, DMA = true, PWM = true, UART = true, CAN = true}
#     uC.PERIPH["STM32F302K6xx"] = {GPIO = true, CLK = true, I2C = true, IRQ = true, DMA = true, PWM = true, UART = true, CAN = true}
#     uC.PERIPH["STM32F302K8xx"] = {GPIO = true, CLK = true, I2C = true, IRQ = true, DMA = true, PWM = true, UART = true, CAN = true}
#     uC.PERIPH["STM32F302R6xx"] = {GPIO = true, CLK = true, I2C = true, IRQ = true, DMA = true, PWM = true, UART = true, CAN = true}
#     uC.PERIPH["STM32F302R8xx"] = {GPIO = true, CLK = true, I2C = true, IRQ = true, DMA = true, PWM = true, UART = true, CAN = true}
#     uC.PERIPH["STM32F302RBxx"] = {GPIO = true, CLK = true, I2C = true, IRQ = true, DMA = true, PWM = true, UART = true, CAN = true}
#     uC.PERIPH["STM32F302RCxx"] = {GPIO = true, CLK = true, I2C = true, IRQ = true, DMA = true, PWM = true, UART = true, CAN = true}
#     uC.PERIPH["STM32F302RDxx"] = {GPIO = true, CLK = true, I2C = true, IRQ = true, DMA = true, PWM = true, UART = true, CAN = true}
#     uC.PERIPH["STM32F302RExx"] = {GPIO = true, CLK = true, I2C = true, IRQ = true, DMA = true, PWM = true, UART = true, CAN = true}
#     uC.PERIPH["STM32F302VBxx"] = {GPIO = true, CLK = true, I2C = true, IRQ = true, DMA = true, PWM = true, UART = true, CAN = true}
#     uC.PERIPH["STM32F302VCxx"] = {GPIO = true, CLK = true, I2C = true, IRQ = true, DMA = true, PWM = true, UART = true, CAN = true}
#     uC.PERIPH["STM32F302VDxx"] = {GPIO = true, CLK = true, I2C = true, IRQ = true, DMA = true, PWM = true, UART = true, CAN = true}
#     uC.PERIPH["STM32F302VExx"] = {GPIO = true, CLK = true, I2C = true, IRQ = true, DMA = true, PWM = true, UART = true, CAN = true}
#     uC.PERIPH["STM32F302ZDxx"] = {GPIO = true, CLK = true, I2C = true, IRQ = true, DMA = true, PWM = true, UART = true, CAN = true}
#     uC.PERIPH["STM32F302ZExx"] = {GPIO = true, CLK = true, I2C = true, IRQ = true, DMA = true, PWM = true, UART = true, CAN = true}
#     uC.PERIPH["STM32F303C6xx"] = {GPIO = true, CLK = true, I2C = true, IRQ = true, DMA = true, PWM = true, UART = true, CAN = true}
#     uC.PERIPH["STM32F303C8xx"] = {GPIO = true, CLK = true, I2C = true, IRQ = true, DMA = true, PWM = true, UART = true, CAN = true}
#     uC.PERIPH["STM32F303CBxx"] = {GPIO = true, CLK = true, I2C = true, IRQ = true, DMA = true, PWM = true, UART = true, CAN = true}
#     uC.PERIPH["STM32F303CCxx"] = {GPIO = true, CLK = true, I2C = true, IRQ = true, DMA = true, PWM = true, UART = true, CAN = true}
#     uC.PERIPH["STM32F303K6xx"] = {GPIO = true, CLK = true, I2C = true, IRQ = true, DMA = true, PWM = true, UART = true, CAN = true}
#     uC.PERIPH["STM32F303K8xx"] = {GPIO = true, CLK = true, I2C = true, IRQ = true, DMA = true, PWM = true, UART = true, CAN = true}
#     uC.PERIPH["STM32F303R6xx"] = {GPIO = true, CLK = true, I2C = true, IRQ = true, DMA = true, PWM = true, UART = true, CAN = true}
#     uC.PERIPH["STM32F303R8xx"] = {GPIO = true, CLK = true, I2C = true, IRQ = true, DMA = true, PWM = true, UART = true, CAN = true}
#     uC.PERIPH["STM32F303RBxx"] = {GPIO = true, CLK = true, I2C = true, IRQ = true, DMA = true, PWM = true, UART = true, CAN = true}
#     uC.PERIPH["STM32F303RCxx"] = {GPIO = true, CLK = true, I2C = true, IRQ = true, DMA = true, PWM = true, UART = true, CAN = true}
#     uC.PERIPH["STM32F303RDxx"] = {GPIO = true, CLK = true, I2C = true, IRQ = true, DMA = true, PWM = true, UART = true, CAN = true}
#     uC.PERIPH["STM32F303RExx"] = {GPIO = true, CLK = true, I2C = true, IRQ = true, DMA = true, PWM = true, UART = true, CAN = true}
#     uC.PERIPH["STM32F303VBxx"] = {GPIO = true, CLK = true, I2C = true, IRQ = true, DMA = true, PWM = true, UART = true, CAN = true}
#     uC.PERIPH["STM32F303VCxx"] = {GPIO = true, CLK = true, I2C = true, IRQ = true, DMA = true, PWM = true, UART = true, CAN = true}
#     uC.PERIPH["STM32F303VDxx"] = {GPIO = true, CLK = true, I2C = true, IRQ = true, DMA = true, PWM = true, UART = true, CAN = true}
#     uC.PERIPH["STM32F303VExx"] = {GPIO = true, CLK = true, I2C = true, IRQ = true, DMA = true, PWM = true, UART = true, CAN = true}
#     uC.PERIPH["STM32F303ZDxx"] = {GPIO = true, CLK = true, I2C = true, IRQ = true, DMA = true, PWM = true, UART = true, CAN = true}
#     uC.PERIPH["STM32F303ZExx"] = {GPIO = true, CLK = true, I2C = true, IRQ = true, DMA = true, PWM = true, UART = true, CAN = true}
#     uC.PERIPH["STM32F318C8xx"] = {GPIO = true, CLK = true, I2C = true, IRQ = true, DMA = true, PWM = true, UART = true}
#     uC.PERIPH["STM32F318K8xx"] = {GPIO = true, CLK = true, I2C = true, IRQ = true, DMA = true, PWM = true, UART = true}
#     uC.PERIPH["STM32F328C8xx"] = {GPIO = true, CLK = true, I2C = true, IRQ = true, DMA = true, PWM = true, UART = true, CAN = true}
#     uC.PERIPH["STM32F334C4xx"] = {GPIO = true, CLK = true, I2C = true, IRQ = true, DMA = true, PWM = true, UART = true, CAN = true}
#     uC.PERIPH["STM32F334C6xx"] = {GPIO = true, CLK = true, I2C = true, IRQ = true, DMA = true, PWM = true, UART = true, CAN = true}
#     uC.PERIPH["STM32F334C8xx"] = {GPIO = true, CLK = true, I2C = true, IRQ = true, DMA = true, PWM = true, UART = true, CAN = true}
#     uC.PERIPH["STM32F334K4xx"] = {GPIO = true, CLK = true, I2C = true, IRQ = true, DMA = true, PWM = true, UART = true, CAN = true}
#     uC.PERIPH["STM32F334K6xx"] = {GPIO = true, CLK = true, I2C = true, IRQ = true, DMA = true, PWM = true, UART = true, CAN = true}
#     uC.PERIPH["STM32F334K8xx"] = {GPIO = true, CLK = true, I2C = true, IRQ = true, DMA = true, PWM = true, UART = true, CAN = true}
#     uC.PERIPH["STM32F334R6xx"] = {GPIO = true, CLK = true, I2C = true, IRQ = true, DMA = true, PWM = true, UART = true, CAN = true}
#     uC.PERIPH["STM32F334R8xx"] = {GPIO = true, CLK = true, I2C = true, IRQ = true, DMA = true, PWM = true, UART = true, CAN = true}
#     uC.PERIPH["STM32F358CCxx"] = {GPIO = true, CLK = true, I2C = true, IRQ = true, DMA = true, PWM = true, UART = true, CAN = true}
#     uC.PERIPH["STM32F358RCxx"] = {GPIO = true, CLK = true, I2C = true, IRQ = true, DMA = true, PWM = true, UART = true, CAN = true}
#     uC.PERIPH["STM32F358VCxx"] = {GPIO = true, CLK = true, I2C = true, IRQ = true, DMA = true, PWM = true, UART = true, CAN = true}
#     uC.PERIPH["STM32F373C8xx"] = {GPIO = true, CLK = true, I2C = true, IRQ = true, DMA = true, PWM = true, UART = true, CAN = true}
#     uC.PERIPH["STM32F373CBxx"] = {GPIO = true, CLK = true, I2C = true, IRQ = true, DMA = true, PWM = true, UART = true, CAN = true}
#     uC.PERIPH["STM32F373CCxx"] = {GPIO = true, CLK = true, I2C = true, IRQ = true, DMA = true, PWM = true, UART = true, CAN = true}
#     uC.PERIPH["STM32F373R8xx"] = {GPIO = true, CLK = true, I2C = true, IRQ = true, DMA = true, PWM = true, UART = true, CAN = true}
#     uC.PERIPH["STM32F373RBxx"] = {GPIO = true, CLK = true, I2C = true, IRQ = true, DMA = true, PWM = true, UART = true, CAN = true}
#     uC.PERIPH["STM32F373RCxx"] = {GPIO = true, CLK = true, I2C = true, IRQ = true, DMA = true, PWM = true, UART = true, CAN = true}
#     uC.PERIPH["STM32F373V8xx"] = {GPIO = true, CLK = true, I2C = true, IRQ = true, DMA = true, PWM = true, UART = true, CAN = true}
#     uC.PERIPH["STM32F373VBxx"] = {GPIO = true, CLK = true, I2C = true, IRQ = true, DMA = true, PWM = true, UART = true, CAN = true}
#     uC.PERIPH["STM32F373VCxx"] = {GPIO = true, CLK = true, I2C = true, IRQ = true, DMA = true, PWM = true, UART = true, CAN = true}
#     uC.PERIPH["STM32F378CCxx"] = {GPIO = true, CLK = true, I2C = true, IRQ = true, DMA = true, PWM = true, UART = true, CAN = true}
#     uC.PERIPH["STM32F378RCxx"] = {GPIO = true, CLK = true, I2C = true, IRQ = true, DMA = true, PWM = true, UART = true, CAN = true}
#     uC.PERIPH["STM32F378VCxx"] = {GPIO = true, CLK = true, I2C = true, IRQ = true, DMA = true, PWM = true, UART = true, CAN = true}
#     uC.PERIPH["STM32F398VExx"] = {GPIO = true, CLK = true, I2C = true, IRQ = true, DMA = true, PWM = true, UART = true, CAN = true}
# end
#
# if uC.ARCH == "stm32f4" then
#     uC.AddPriorityItems = function(this, no_default)
#         this:AddItem("Priority 0 (the highest)", "0")
#         for i = 1, 14 do this:AddItem("Priority "..i, tostring(i)) end
#         this:AddItem("Priority 15 (the lowest, safe for kernel API)", "15")
#         if no_default ~= true then
#             this:AddItem("Default priority", "__CPU_DEFAULT_IRQ_PRIORITY__")
#         end
#     end
#
#     uC.PERIPH["STM32F401CBxx"] = {GPIO = true, CLK = true, CRC = true, RTC = true, I2C = true, DMA = true, AFM = true, IRQ = true, NVM = true, PWM = true, WDG = true, SPI = true, UART = true}
#     uC.PERIPH["STM32F401RBxx"] = {GPIO = true, CLK = true, CRC = true, RTC = true, I2C = true, DMA = true, AFM = true, IRQ = true, NVM = true, PWM = true, SDIO = true, WDG = true, SPI = true, UART = true}
#     uC.PERIPH["STM32F401VBxx"] = {GPIO = true, CLK = true, CRC = true, RTC = true, I2C = true, DMA = true, AFM = true, IRQ = true, NVM = true, PWM = true, SDIO = true, WDG = true, SPI = true, UART = true}
#     uC.PERIPH["STM32F401CCxx"] = {GPIO = true, CLK = true, CRC = true, RTC = true, I2C = true, DMA = true, AFM = true, IRQ = true, NVM = true, PWM = true, WDG = true, SPI = true, UART = true}
#     uC.PERIPH["STM32F401RCxx"] = {GPIO = true, CLK = true, CRC = true, RTC = true, I2C = true, DMA = true, AFM = true, IRQ = true, NVM = true, PWM = true, SDIO = true, WDG = true, SPI = true, UART = true}
#     uC.PERIPH["STM32F401VCxx"] = {GPIO = true, CLK = true, CRC = true, RTC = true, I2C = true, DMA = true, AFM = true, IRQ = true, NVM = true, PWM = true, SDIO = true, WDG = true, SPI = true, UART = true}
#     uC.PERIPH["STM32F401CDxx"] = {GPIO = true, CLK = true, CRC = true, RTC = true, I2C = true, DMA = true, AFM = true, IRQ = true, NVM = true, PWM = true, WDG = true, SPI = true, UART = true}
#     uC.PERIPH["STM32F401RDxx"] = {GPIO = true, CLK = true, CRC = true, RTC = true, I2C = true, DMA = true, AFM = true, IRQ = true, NVM = true, PWM = true, SDIO = true, WDG = true, SPI = true, UART = true}
#     uC.PERIPH["STM32F401VDxx"] = {GPIO = true, CLK = true, CRC = true, RTC = true, I2C = true, DMA = true, AFM = true, IRQ = true, NVM = true, PWM = true, SDIO = true, WDG = true, SPI = true, UART = true}
#     uC.PERIPH["STM32F401CExx"] = {GPIO = true, CLK = true, CRC = true, RTC = true, I2C = true, DMA = true, AFM = true, IRQ = true, NVM = true, PWM = true, WDG = true, SPI = true, UART = true}
#     uC.PERIPH["STM32F401RExx"] = {GPIO = true, CLK = true, CRC = true, RTC = true, I2C = true, DMA = true, AFM = true, IRQ = true, NVM = true, PWM = true, SDIO = true, WDG = true, SPI = true, UART = true}
#     uC.PERIPH["STM32F401VExx"] = {GPIO = true, CLK = true, CRC = true, RTC = true, I2C = true, DMA = true, AFM = true, IRQ = true, NVM = true, PWM = true, SDIO = true, WDG = true, SPI = true, UART = true}
#     uC.PERIPH["STM32F405RGxx"] = {GPIO = true, CLK = true, CRC = true, RTC = true, I2C = true, DMA = true, AFM = true, IRQ = true, NVM = true, PWM = true, CAN = true, WDG = true, SPI = true, UART = true, FSMC = true}
#     uC.PERIPH["STM32F405OExx"] = {GPIO = true, CLK = true, CRC = true, RTC = true, I2C = true, DMA = true, AFM = true, IRQ = true, NVM = true, PWM = true, CAN = true, SDIO = true, WDG = true, SPI = true, UART = true, FSMC = true}
#     uC.PERIPH["STM32F405OGxx"] = {GPIO = true, CLK = true, CRC = true, RTC = true, I2C = true, DMA = true, AFM = true, IRQ = true, NVM = true, PWM = true, CAN = true, SDIO = true, WDG = true, SPI = true, UART = true, FSMC = true}
#     uC.PERIPH["STM32F405VGxx"] = {GPIO = true, CLK = true, CRC = true, RTC = true, I2C = true, DMA = true, AFM = true, IRQ = true, NVM = true, PWM = true, CAN = true, SDIO = true, WDG = true, SPI = true, UART = true, FSMC = true}
#     uC.PERIPH["STM32F405ZGxx"] = {GPIO = true, CLK = true, CRC = true, RTC = true, I2C = true, DMA = true, AFM = true, IRQ = true, NVM = true, PWM = true, CAN = true, SDIO = true, WDG = true, SPI = true, UART = true, FSMC = true}
#     uC.PERIPH["STM32F407VExx"] = {GPIO = true, CLK = true, CRC = true, RTC = true, I2C = true, DMA = true, AFM = true, IRQ = true, NVM = true, PWM = true, CAN = true, SDIO = true, WDG = true, SPI = true, UART = true, FSMC = true, ETH = true, DCI = true}
#     uC.PERIPH["STM32F407VGxx"] = {GPIO = true, CLK = true, CRC = true, RTC = true, I2C = true, DMA = true, AFM = true, IRQ = true, NVM = true, PWM = true, CAN = true, SDIO = true, WDG = true, SPI = true, UART = true, FSMC = true, ETH = true, DCI = true}
#     uC.PERIPH["STM32F407ZExx"] = {GPIO = true, CLK = true, CRC = true, RTC = true, I2C = true, DMA = true, AFM = true, IRQ = true, NVM = true, PWM = true, CAN = true, SDIO = true, WDG = true, SPI = true, UART = true, FSMC = true, ETH = true, DCI = true}
#     uC.PERIPH["STM32F407ZGxx"] = {GPIO = true, CLK = true, CRC = true, RTC = true, I2C = true, DMA = true, AFM = true, IRQ = true, NVM = true, PWM = true, CAN = true, SDIO = true, WDG = true, SPI = true, UART = true, FSMC = true, ETH = true, DCI = true}
#     uC.PERIPH["STM32F407IExx"] = {GPIO = true, CLK = true, CRC = true, RTC = true, I2C = true, DMA = true, AFM = true, IRQ = true, NVM = true, PWM = true, CAN = true, SDIO = true, WDG = true, SPI = true, UART = true, FSMC = true, ETH = true, DCI = true}
#     uC.PERIPH["STM32F407IGxx"] = {GPIO = true, CLK = true, CRC = true, RTC = true, I2C = true, DMA = true, AFM = true, IRQ = true, NVM = true, PWM = true, CAN = true, SDIO = true, WDG = true, SPI = true, UART = true, FSMC = true, ETH = true, DCI = true}
#     uC.PERIPH["STM32F410T8xx"] = {GPIO = true, CLK = true, CRC = true, RTC = true, I2C = true, DMA = true, AFM = true, IRQ = true, NVM = true, PWM = true, WDG = true, SPI = true, UART = true}
#     uC.PERIPH["STM32F410TBxx"] = {GPIO = true, CLK = true, CRC = true, RTC = true, I2C = true, DMA = true, AFM = true, IRQ = true, NVM = true, PWM = true, WDG = true, SPI = true, UART = true}
#     uC.PERIPH["STM32F410C8xx"] = {GPIO = true, CLK = true, CRC = true, RTC = true, I2C = true, DMA = true, AFM = true, IRQ = true, NVM = true, PWM = true, WDG = true, SPI = true, UART = true}
#     uC.PERIPH["STM32F410CBxx"] = {GPIO = true, CLK = true, CRC = true, RTC = true, I2C = true, DMA = true, AFM = true, IRQ = true, NVM = true, PWM = true, WDG = true, SPI = true, UART = true}
#     uC.PERIPH["STM32F410R8xx"] = {GPIO = true, CLK = true, CRC = true, RTC = true, I2C = true, DMA = true, AFM = true, IRQ = true, NVM = true, PWM = true, WDG = true, SPI = true, UART = true}
#     uC.PERIPH["STM32F410RBxx"] = {GPIO = true, CLK = true, CRC = true, RTC = true, I2C = true, DMA = true, AFM = true, IRQ = true, NVM = true, PWM = true, WDG = true, SPI = true, UART = true}
#     uC.PERIPH["STM32F411CCxx"] = {GPIO = true, CLK = true, CRC = true, RTC = true, I2C = true, DMA = true, AFM = true, IRQ = true, NVM = true, PWM = true, SDIO = true, WDG = true, SPI = true, UART = true}
#     uC.PERIPH["STM32F411RCxx"] = {GPIO = true, CLK = true, CRC = true, RTC = true, I2C = true, DMA = true, AFM = true, IRQ = true, NVM = true, PWM = true, SDIO = true, WDG = true, SPI = true, UART = true}
#     uC.PERIPH["STM32F411VCxx"] = {GPIO = true, CLK = true, CRC = true, RTC = true, I2C = true, DMA = true, AFM = true, IRQ = true, NVM = true, PWM = true, SDIO = true, WDG = true, SPI = true, UART = true}
#     uC.PERIPH["STM32F411CExx"] = {GPIO = true, CLK = true, CRC = true, RTC = true, I2C = true, DMA = true, AFM = true, IRQ = true, NVM = true, PWM = true, SDIO = true, WDG = true, SPI = true, UART = true}
#     uC.PERIPH["STM32F411RExx"] = {GPIO = true, CLK = true, CRC = true, RTC = true, I2C = true, DMA = true, AFM = true, IRQ = true, NVM = true, PWM = true, SDIO = true, WDG = true, SPI = true, UART = true}
#     uC.PERIPH["STM32F411VExx"] = {GPIO = true, CLK = true, CRC = true, RTC = true, I2C = true, DMA = true, AFM = true, IRQ = true, NVM = true, PWM = true, SDIO = true, WDG = true, SPI = true, UART = true}
#     uC.PERIPH["STM32F412CExx"] = {GPIO = true, CLK = true, CRC = true, RTC = true, I2C = true, DMA = true, AFM = true, IRQ = true, NVM = true, PWM = true, SDIO = true, WDG = true, SPI = true, UART = true, EMI = true}
#     uC.PERIPH["STM32F412CGxx"] = {GPIO = true, CLK = true, CRC = true, RTC = true, I2C = true, DMA = true, AFM = true, IRQ = true, NVM = true, PWM = true, SDIO = true, WDG = true, SPI = true, UART = true, EMI = true}
#     uC.PERIPH["STM32F412RExx"] = {GPIO = true, CLK = true, CRC = true, RTC = true, I2C = true, DMA = true, AFM = true, IRQ = true, NVM = true, PWM = true, SDIO = true, WDG = true, SPI = true, UART = true, EMI = true}
#     uC.PERIPH["STM32F412RGxx"] = {GPIO = true, CLK = true, CRC = true, RTC = true, I2C = true, DMA = true, AFM = true, IRQ = true, NVM = true, PWM = true, SDIO = true, WDG = true, SPI = true, UART = true, EMI = true}
#     uC.PERIPH["STM32F412VExx"] = {GPIO = true, CLK = true, CRC = true, RTC = true, I2C = true, DMA = true, AFM = true, IRQ = true, NVM = true, PWM = true, SDIO = true, WDG = true, SPI = true, UART = true, EMI = true}
#     uC.PERIPH["STM32F412VGxx"] = {GPIO = true, CLK = true, CRC = true, RTC = true, I2C = true, DMA = true, AFM = true, IRQ = true, NVM = true, PWM = true, SDIO = true, WDG = true, SPI = true, UART = true, EMI = true}
#     uC.PERIPH["STM32F412ZExx"] = {GPIO = true, CLK = true, CRC = true, RTC = true, I2C = true, DMA = true, AFM = true, IRQ = true, NVM = true, PWM = true, SDIO = true, WDG = true, SPI = true, UART = true, EMI = true}
#     uC.PERIPH["STM32F412ZGxx"] = {GPIO = true, CLK = true, CRC = true, RTC = true, I2C = true, DMA = true, AFM = true, IRQ = true, NVM = true, PWM = true, SDIO = true, WDG = true, SPI = true, UART = true, EMI = true}
#     uC.PERIPH["STM32F413CGxx"] = {GPIO = true, CLK = true, CRC = true, RTC = true, I2C = true, DMA = true, AFM = true, SND = true, IRQ = true, NVM = true, PWM = true, SDIO = true, WDG = true, SPI = true, UART = true}
#     uC.PERIPH["STM32F413RGxx"] = {GPIO = true, CLK = true, CRC = true, RTC = true, I2C = true, DMA = true, AFM = true, SND = true, IRQ = true, NVM = true, PWM = true, SDIO = true, WDG = true, SPI = true, UART = true, FSMC = true}
#     uC.PERIPH["STM32F413MGxx"] = {GPIO = true, CLK = true, CRC = true, RTC = true, I2C = true, DMA = true, AFM = true, SND = true, IRQ = true, NVM = true, PWM = true, SDIO = true, WDG = true, SPI = true, UART = true, FSMC = true}
#     uC.PERIPH["STM32F413VGxx"] = {GPIO = true, CLK = true, CRC = true, RTC = true, I2C = true, DMA = true, AFM = true, SND = true, IRQ = true, NVM = true, PWM = true, SDIO = true, WDG = true, SPI = true, UART = true, FSMC = true}
#     uC.PERIPH["STM32F413ZGxx"] = {GPIO = true, CLK = true, CRC = true, RTC = true, I2C = true, DMA = true, AFM = true, SND = true, IRQ = true, NVM = true, PWM = true, SDIO = true, WDG = true, SPI = true, UART = true, FSMC = true}
#     uC.PERIPH["STM32F413CHxx"] = {GPIO = true, CLK = true, CRC = true, RTC = true, I2C = true, DMA = true, AFM = true, SND = true, IRQ = true, NVM = true, PWM = true, SDIO = true, WDG = true, SPI = true, UART = true}
#     uC.PERIPH["STM32F413RHxx"] = {GPIO = true, CLK = true, CRC = true, RTC = true, I2C = true, DMA = true, AFM = true, SND = true, IRQ = true, NVM = true, PWM = true, SDIO = true, WDG = true, SPI = true, UART = true, FSMC = true}
#     uC.PERIPH["STM32F413MHxx"] = {GPIO = true, CLK = true, CRC = true, RTC = true, I2C = true, DMA = true, AFM = true, SND = true, IRQ = true, NVM = true, PWM = true, SDIO = true, WDG = true, SPI = true, UART = true, FSMC = true}
#     uC.PERIPH["STM32F413VHxx"] = {GPIO = true, CLK = true, CRC = true, RTC = true, I2C = true, DMA = true, AFM = true, SND = true, IRQ = true, NVM = true, PWM = true, SDIO = true, WDG = true, SPI = true, UART = true, FSMC = true}
#     uC.PERIPH["STM32F413ZHxx"] = {GPIO = true, CLK = true, CRC = true, RTC = true, I2C = true, DMA = true, AFM = true, SND = true, IRQ = true, NVM = true, PWM = true, SDIO = true, WDG = true, SPI = true, UART = true, FSMC = true}
#     uC.PERIPH["STM32F415RGxx"] = {GPIO = true, CLK = true, CRC = true, RTC = true, I2C = true, DMA = true, AFM = true, IRQ = true, NVM = true, PWM = true, SDIO = true, WDG = true, SPI = true, UART = true, FSMC = true}
#     uC.PERIPH["STM32F415OGxx"] = {GPIO = true, CLK = true, CRC = true, RTC = true, I2C = true, DMA = true, AFM = true, IRQ = true, NVM = true, PWM = true, SDIO = true, WDG = true, SPI = true, UART = true, FSMC = true}
#     uC.PERIPH["STM32F415VGxx"] = {GPIO = true, CLK = true, CRC = true, RTC = true, I2C = true, DMA = true, AFM = true, IRQ = true, NVM = true, PWM = true, SDIO = true, WDG = true, SPI = true, UART = true, FSMC = true}
#     uC.PERIPH["STM32F415ZGxx"] = {GPIO = true, CLK = true, CRC = true, RTC = true, I2C = true, DMA = true, AFM = true, IRQ = true, NVM = true, PWM = true, SDIO = true, WDG = true, SPI = true, UART = true, FSMC = true}
#     uC.PERIPH["STM32F417VExx"] = {GPIO = true, CLK = true, CRC = true, RTC = true, I2C = true, DMA = true, AFM = true, IRQ = true, NVM = true, PWM = true, SDIO = true, WDG = true, SPI = true, UART = true, FSMC = true, ETH = true, DCI = true}
#     uC.PERIPH["STM32F417VGxx"] = {GPIO = true, CLK = true, CRC = true, RTC = true, I2C = true, DMA = true, AFM = true, IRQ = true, NVM = true, PWM = true, SDIO = true, WDG = true, SPI = true, UART = true, FSMC = true, ETH = true, DCI = true}
#     uC.PERIPH["STM32F417ZExx"] = {GPIO = true, CLK = true, CRC = true, RTC = true, I2C = true, DMA = true, AFM = true, IRQ = true, NVM = true, PWM = true, SDIO = true, WDG = true, SPI = true, UART = true, FSMC = true, ETH = true, DCI = true}
#     uC.PERIPH["STM32F417ZGxx"] = {GPIO = true, CLK = true, CRC = true, RTC = true, I2C = true, DMA = true, AFM = true, IRQ = true, NVM = true, PWM = true, SDIO = true, WDG = true, SPI = true, UART = true, FSMC = true, ETH = true, DCI = true}
#     uC.PERIPH["STM32F417IExx"] = {GPIO = true, CLK = true, CRC = true, RTC = true, I2C = true, DMA = true, AFM = true, IRQ = true, NVM = true, PWM = true, SDIO = true, WDG = true, SPI = true, UART = true, FSMC = true, ETH = true, DCI = true}
#     uC.PERIPH["STM32F417IGxx"] = {GPIO = true, CLK = true, CRC = true, RTC = true, I2C = true, DMA = true, AFM = true, IRQ = true, NVM = true, PWM = true, SDIO = true, WDG = true, SPI = true, UART = true, FSMC = true, ETH = true, DCI = true}
#     uC.PERIPH["STM32F423CHxx"] = {GPIO = true, CLK = true, CRC = true, RTC = true, I2C = true, DMA = true, AFM = true, SND = true, IRQ = true, NVM = true, PWM = true, SDIO = true, WDG = true, SPI = true, UART = true}
#     uC.PERIPH["STM32F423RHxx"] = {GPIO = true, CLK = true, CRC = true, RTC = true, I2C = true, DMA = true, AFM = true, SND = true, IRQ = true, NVM = true, PWM = true, SDIO = true, WDG = true, SPI = true, UART = true, FSMC = true}
#     uC.PERIPH["STM32F423MHxx"] = {GPIO = true, CLK = true, CRC = true, RTC = true, I2C = true, DMA = true, AFM = true, SND = true, IRQ = true, NVM = true, PWM = true, SDIO = true, WDG = true, SPI = true, UART = true, FSMC = true}
#     uC.PERIPH["STM32F423VHxx"] = {GPIO = true, CLK = true, CRC = true, RTC = true, I2C = true, DMA = true, AFM = true, SND = true, IRQ = true, NVM = true, PWM = true, SDIO = true, WDG = true, SPI = true, UART = true, FSMC = true}
#     uC.PERIPH["STM32F423ZHxx"] = {GPIO = true, CLK = true, CRC = true, RTC = true, I2C = true, DMA = true, AFM = true, SND = true, IRQ = true, NVM = true, PWM = true, SDIO = true, WDG = true, SPI = true, UART = true, FSMC = true}
#     uC.PERIPH["STM32F427VGxx"] = {GPIO = true, CLK = true, CRC = true, RTC = true, I2C = true, DMA = true, AFM = true, SND = true, IRQ = true, NVM = true, PWM = true, CAN = true, SDIO = true, WDG = true, SPI = true, UART = true, EMI = true, ETH = true, DCI = true}
#     uC.PERIPH["STM32F427VIxx"] = {GPIO = true, CLK = true, CRC = true, RTC = true, I2C = true, DMA = true, AFM = true, SND = true, IRQ = true, NVM = true, PWM = true, CAN = true, SDIO = true, WDG = true, SPI = true, UART = true, EMI = true, ETH = true, DCI = true}
#     uC.PERIPH["STM32F427ZGxx"] = {GPIO = true, CLK = true, CRC = true, RTC = true, I2C = true, DMA = true, AFM = true, SND = true, IRQ = true, NVM = true, PWM = true, CAN = true, SDIO = true, WDG = true, SPI = true, UART = true, EMI = true, ETH = true, DCI = true}
#     uC.PERIPH["STM32F427ZIxx"] = {GPIO = true, CLK = true, CRC = true, RTC = true, I2C = true, DMA = true, AFM = true, SND = true, IRQ = true, NVM = true, PWM = true, CAN = true, SDIO = true, WDG = true, SPI = true, UART = true, EMI = true, ETH = true, DCI = true}
#     uC.PERIPH["STM32F427AGxx"] = {GPIO = true, CLK = true, CRC = true, RTC = true, I2C = true, DMA = true, AFM = true, SND = true, IRQ = true, NVM = true, PWM = true, CAN = true, SDIO = true, WDG = true, SPI = true, UART = true, FSMC = true, ETH = true, DCI = true}
#     uC.PERIPH["STM32F427AIxx"] = {GPIO = true, CLK = true, CRC = true, RTC = true, I2C = true, DMA = true, AFM = true, SND = true, IRQ = true, NVM = true, PWM = true, CAN = true, SDIO = true, WDG = true, SPI = true, UART = true, EMI = true, ETH = true, DCI = true}
#     uC.PERIPH["STM32F427IGxx"] = {GPIO = true, CLK = true, CRC = true, RTC = true, I2C = true, DMA = true, AFM = true, SND = true, IRQ = true, NVM = true, PWM = true, CAN = true, SDIO = true, WDG = true, SPI = true, UART = true, EMI = true, ETH = true, DCI = true}
#     uC.PERIPH["STM32F427IIxx"] = {GPIO = true, CLK = true, CRC = true, RTC = true, I2C = true, DMA = true, AFM = true, SND = true, IRQ = true, NVM = true, PWM = true, CAN = true, SDIO = true, WDG = true, SPI = true, UART = true, EMI = true, ETH = true, DCI = true}
#     uC.PERIPH["STM32F429VExx"] = {GPIO = true, CLK = true, CRC = true, RTC = true, I2C = true, DMA = true, AFM = true, SND = true, IRQ = true, NVM = true, PWM = true, CAN = true, SDIO = true, WDG = true, SPI = true, UART = true, EMI = true, ETH = true, DCI = true, USBH = true}
#     uC.PERIPH["STM32F429VGxx"] = {GPIO = true, CLK = true, CRC = true, RTC = true, I2C = true, DMA = true, AFM = true, SND = true, IRQ = true, NVM = true, PWM = true, CAN = true, SDIO = true, WDG = true, SPI = true, UART = true, EMI = true, ETH = true, DCI = true, USBH = true}
#     uC.PERIPH["STM32F429VIxx"] = {GPIO = true, CLK = true, CRC = true, RTC = true, I2C = true, DMA = true, AFM = true, SND = true, IRQ = true, NVM = true, PWM = true, CAN = true, SDIO = true, WDG = true, SPI = true, UART = true, EMI = true, ETH = true, DCI = true, USBH = true}
#     uC.PERIPH["STM32F429ZExx"] = {GPIO = true, CLK = true, CRC = true, RTC = true, I2C = true, DMA = true, AFM = true, SND = true, IRQ = true, NVM = true, PWM = true, CAN = true, SDIO = true, WDG = true, SPI = true, UART = true, EMI = true, ETH = true, DCI = true, USBH = true}
#     uC.PERIPH["STM32F429ZGxx"] = {GPIO = true, CLK = true, CRC = true, RTC = true, I2C = true, DMA = true, AFM = true, SND = true, IRQ = true, NVM = true, PWM = true, CAN = true, SDIO = true, WDG = true, SPI = true, UART = true, EMI = true, ETH = true, DCI = true, USBH = true}
#     uC.PERIPH["STM32F429ZIxx"] = {GPIO = true, CLK = true, CRC = true, RTC = true, I2C = true, DMA = true, AFM = true, SND = true, IRQ = true, NVM = true, PWM = true, CAN = true, SDIO = true, WDG = true, SPI = true, UART = true, EMI = true, ETH = true, DCI = true, USBH = true}
#     uC.PERIPH["STM32F429AGxx"] = {GPIO = true, CLK = true, CRC = true, RTC = true, I2C = true, DMA = true, AFM = true, SND = true, IRQ = true, NVM = true, PWM = true, CAN = true, SDIO = true, WDG = true, SPI = true, UART = true, EMI = true, ETH = true, DCI = true, USBH = true}
#     uC.PERIPH["STM32F429AIxx"] = {GPIO = true, CLK = true, CRC = true, RTC = true, I2C = true, DMA = true, AFM = true, SND = true, IRQ = true, NVM = true, PWM = true, CAN = true, SDIO = true, WDG = true, SPI = true, UART = true, EMI = true, ETH = true, DCI = true, USBH = true}
#     uC.PERIPH["STM32F429IExx"] = {GPIO = true, CLK = true, CRC = true, RTC = true, I2C = true, DMA = true, AFM = true, SND = true, IRQ = true, NVM = true, PWM = true, CAN = true, SDIO = true, WDG = true, SPI = true, UART = true, EMI = true, ETH = true, DCI = true, USBH = true}
#     uC.PERIPH["STM32F429IGxx"] = {GPIO = true, CLK = true, CRC = true, RTC = true, I2C = true, DMA = true, AFM = true, SND = true, IRQ = true, NVM = true, PWM = true, CAN = true, SDIO = true, WDG = true, SPI = true, UART = true, EMI = true, ETH = true, DCI = true, USBH = true}
#     uC.PERIPH["STM32F429IIxx"] = {GPIO = true, CLK = true, CRC = true, RTC = true, I2C = true, DMA = true, AFM = true, SND = true, IRQ = true, NVM = true, PWM = true, CAN = true, SDIO = true, WDG = true, SPI = true, UART = true, EMI = true, ETH = true, DCI = true, USBH = true}
#     uC.PERIPH["STM32F429BExx"] = {GPIO = true, CLK = true, CRC = true, RTC = true, I2C = true, DMA = true, AFM = true, SND = true, IRQ = true, NVM = true, PWM = true, CAN = true, SDIO = true, WDG = true, SPI = true, UART = true, EMI = true, ETH = true, DCI = true, USBH = true}
#     uC.PERIPH["STM32F429BGxx"] = {GPIO = true, CLK = true, CRC = true, RTC = true, I2C = true, DMA = true, AFM = true, SND = true, IRQ = true, NVM = true, PWM = true, CAN = true, SDIO = true, WDG = true, SPI = true, UART = true, EMI = true, ETH = true, DCI = true, USBH = true}
#     uC.PERIPH["STM32F429BIxx"] = {GPIO = true, CLK = true, CRC = true, RTC = true, I2C = true, DMA = true, AFM = true, SND = true, IRQ = true, NVM = true, PWM = true, CAN = true, SDIO = true, WDG = true, SPI = true, UART = true, EMI = true, ETH = true, DCI = true, USBH = true}
#     uC.PERIPH["STM32F429NExx"] = {GPIO = true, CLK = true, CRC = true, RTC = true, I2C = true, DMA = true, AFM = true, SND = true, IRQ = true, NVM = true, PWM = true, CAN = true, SDIO = true, WDG = true, SPI = true, UART = true, EMI = true, ETH = true, DCI = true, USBH = true}
#     uC.PERIPH["STM32F429NGxx"] = {GPIO = true, CLK = true, CRC = true, RTC = true, I2C = true, DMA = true, AFM = true, SND = true, IRQ = true, NVM = true, PWM = true, CAN = true, SDIO = true, WDG = true, SPI = true, UART = true, EMI = true, ETH = true, DCI = true, USBH = true}
#     uC.PERIPH["STM32F429NIxx"] = {GPIO = true, CLK = true, CRC = true, RTC = true, I2C = true, DMA = true, AFM = true, SND = true, IRQ = true, NVM = true, PWM = true, CAN = true, SDIO = true, WDG = true, SPI = true, UART = true, EMI = true, ETH = true, DCI = true, USBH = true}
#     uC.PERIPH["STM32F437VGxx"] = {GPIO = true, CLK = true, CRC = true, RTC = true, I2C = true, DMA = true, AFM = true, SND = true, IRQ = true, NVM = true, PWM = true, SDIO = true, WDG = true, SPI = true, UART = true, EMI = true, ETH = true, DCI = true}
#     uC.PERIPH["STM32F437VIxx"] = {GPIO = true, CLK = true, CRC = true, RTC = true, I2C = true, DMA = true, AFM = true, SND = true, IRQ = true, NVM = true, PWM = true, SDIO = true, WDG = true, SPI = true, UART = true, EMI = true, ETH = true, DCI = true}
#     uC.PERIPH["STM32F437ZGxx"] = {GPIO = true, CLK = true, CRC = true, RTC = true, I2C = true, DMA = true, AFM = true, SND = true, IRQ = true, NVM = true, PWM = true, SDIO = true, WDG = true, SPI = true, UART = true, EMI = true, ETH = true, DCI = true}
#     uC.PERIPH["STM32F437ZIxx"] = {GPIO = true, CLK = true, CRC = true, RTC = true, I2C = true, DMA = true, AFM = true, SND = true, IRQ = true, NVM = true, PWM = true, SDIO = true, WDG = true, SPI = true, UART = true, EMI = true, ETH = true, DCI = true}
#     uC.PERIPH["STM32F437IGxx"] = {GPIO = true, CLK = true, CRC = true, RTC = true, I2C = true, DMA = true, AFM = true, SND = true, IRQ = true, NVM = true, PWM = true, SDIO = true, WDG = true, SPI = true, UART = true, EMI = true, ETH = true, DCI = true}
#     uC.PERIPH["STM32F437IIxx"] = {GPIO = true, CLK = true, CRC = true, RTC = true, I2C = true, DMA = true, AFM = true, SND = true, IRQ = true, NVM = true, PWM = true, SDIO = true, WDG = true, SPI = true, UART = true, EMI = true, ETH = true, DCI = true}
#     uC.PERIPH["STM32F437AIxx"] = {GPIO = true, CLK = true, CRC = true, RTC = true, I2C = true, DMA = true, AFM = true, SND = true, IRQ = true, NVM = true, PWM = true, SDIO = true, WDG = true, SPI = true, UART = true, EMI = true, ETH = true, DCI = true}
#     uC.PERIPH["STM32F439VGxx"] = {GPIO = true, CLK = true, CRC = true, RTC = true, I2C = true, DMA = true, AFM = true, SND = true, IRQ = true, NVM = true, PWM = true, SDIO = true, WDG = true, SPI = true, UART = true, EMI = true, ETH = true, DCI = true}
#     uC.PERIPH["STM32F439VIxx"] = {GPIO = true, CLK = true, CRC = true, RTC = true, I2C = true, DMA = true, AFM = true, SND = true, IRQ = true, NVM = true, PWM = true, SDIO = true, WDG = true, SPI = true, UART = true, EMI = true, ETH = true, DCI = true}
#     uC.PERIPH["STM32F439ZGxx"] = {GPIO = true, CLK = true, CRC = true, RTC = true, I2C = true, DMA = true, AFM = true, SND = true, IRQ = true, NVM = true, PWM = true, SDIO = true, WDG = true, SPI = true, UART = true, EMI = true, ETH = true, DCI = true}
#     uC.PERIPH["STM32F439ZIxx"] = {GPIO = true, CLK = true, CRC = true, RTC = true, I2C = true, DMA = true, AFM = true, SND = true, IRQ = true, NVM = true, PWM = true, SDIO = true, WDG = true, SPI = true, UART = true, EMI = true, ETH = true, DCI = true}
#     uC.PERIPH["STM32F439IGxx"] = {GPIO = true, CLK = true, CRC = true, RTC = true, I2C = true, DMA = true, AFM = true, SND = true, IRQ = true, NVM = true, PWM = true, SDIO = true, WDG = true, SPI = true, UART = true, EMI = true, ETH = true, DCI = true}
#     uC.PERIPH["STM32F439IIxx"] = {GPIO = true, CLK = true, CRC = true, RTC = true, I2C = true, DMA = true, AFM = true, SND = true, IRQ = true, NVM = true, PWM = true, SDIO = true, WDG = true, SPI = true, UART = true, EMI = true, ETH = true, DCI = true}
#     uC.PERIPH["STM32F439BGxx"] = {GPIO = true, CLK = true, CRC = true, RTC = true, I2C = true, DMA = true, AFM = true, SND = true, IRQ = true, NVM = true, PWM = true, SDIO = true, WDG = true, SPI = true, UART = true, EMI = true, ETH = true, DCI = true}
#     uC.PERIPH["STM32F439BIxx"] = {GPIO = true, CLK = true, CRC = true, RTC = true, I2C = true, DMA = true, AFM = true, SND = true, IRQ = true, NVM = true, PWM = true, SDIO = true, WDG = true, SPI = true, UART = true, EMI = true, ETH = true, DCI = true}
#     uC.PERIPH["STM32F439NGxx"] = {GPIO = true, CLK = true, CRC = true, RTC = true, I2C = true, DMA = true, AFM = true, SND = true, IRQ = true, NVM = true, PWM = true, SDIO = true, WDG = true, SPI = true, UART = true, EMI = true, ETH = true, DCI = true}
#     uC.PERIPH["STM32F439NIxx"] = {GPIO = true, CLK = true, CRC = true, RTC = true, I2C = true, DMA = true, AFM = true, SND = true, IRQ = true, NVM = true, PWM = true, SDIO = true, WDG = true, SPI = true, UART = true, EMI = true, ETH = true, DCI = true}
#     uC.PERIPH["STM32F439AIxx"] = {GPIO = true, CLK = true, CRC = true, RTC = true, I2C = true, DMA = true, AFM = true, SND = true, IRQ = true, NVM = true, PWM = true, SDIO = true, WDG = true, SPI = true, UART = true, EMI = true, ETH = true, DCI = true}
#     uC.PERIPH["STM32F446MCxx"] = {GPIO = true, CLK = true, CRC = true, RTC = true, I2C = true, DMA = true, AFM = true, SND = true, IRQ = true, NVM = true, PWM = true, CAN = true, WDG = true, SPI = true, UART = true, EMI = true, DCI = true}
#     uC.PERIPH["STM32F446RCxx"] = {GPIO = true, CLK = true, CRC = true, RTC = true, I2C = true, DMA = true, AFM = true, SND = true, IRQ = true, NVM = true, PWM = true, CAN = true, SDIO = true, WDG = true, SPI = true, UART = true, DCI = true}
#     uC.PERIPH["STM32F446VCxx"] = {GPIO = true, CLK = true, CRC = true, RTC = true, I2C = true, DMA = true, AFM = true, SND = true, IRQ = true, NVM = true, PWM = true, CAN = true, SDIO = true, WDG = true, SPI = true, UART = true, EMI = true, DCI = true}
#     uC.PERIPH["STM32F446ZCxx"] = {GPIO = true, CLK = true, CRC = true, RTC = true, I2C = true, DMA = true, AFM = true, SND = true, IRQ = true, NVM = true, PWM = true, CAN = true, SDIO = true, WDG = true, SPI = true, UART = true, EMI = true, DCI = true}
#     uC.PERIPH["STM32F446MExx"] = {GPIO = true, CLK = true, CRC = true, RTC = true, I2C = true, DMA = true, AFM = true, SND = true, IRQ = true, NVM = true, PWM = true, CAN = true, WDG = true, SPI = true, UART = true, EMI = true, DCI = true}
#     uC.PERIPH["STM32F446RExx"] = {GPIO = true, CLK = true, CRC = true, RTC = true, I2C = true, DMA = true, AFM = true, SND = true, IRQ = true, NVM = true, PWM = true, CAN = true, SDIO = true, WDG = true, SPI = true, UART = true, EMI = true, DCI = true}
#     uC.PERIPH["STM32F446VExx"] = {GPIO = true, CLK = true, CRC = true, RTC = true, I2C = true, DMA = true, AFM = true, SND = true, IRQ = true, NVM = true, PWM = true, CAN = true, SDIO = true, WDG = true, SPI = true, UART = true, EMI = true, DCI = true}
#     uC.PERIPH["STM32F446ZExx"] = {GPIO = true, CLK = true, CRC = true, RTC = true, I2C = true, DMA = true, AFM = true, SND = true, IRQ = true, NVM = true, PWM = true, CAN = true, SDIO = true, WDG = true, SPI = true, UART = true, EMI = true, DCI = true}
#     uC.PERIPH["STM32F469VExx"] = {GPIO = true, CLK = true, CRC = true, RTC = true, I2C = true, DMA = true, AFM = true, SND = true, IRQ = true, NVM = true, PWM = true, CAN = true, SDIO = true, WDG = true, SPI = true, UART = true, EMI = true, DCI = true}
#     uC.PERIPH["STM32F469VGxx"] = {GPIO = true, CLK = true, CRC = true, RTC = true, I2C = true, DMA = true, AFM = true, SND = true, IRQ = true, NVM = true, PWM = true, CAN = true, SDIO = true, WDG = true, SPI = true, UART = true, EMI = true, DCI = true}
#     uC.PERIPH["STM32F469VIxx"] = {GPIO = true, CLK = true, CRC = true, RTC = true, I2C = true, DMA = true, AFM = true, SND = true, IRQ = true, NVM = true, PWM = true, CAN = true, SDIO = true, WDG = true, SPI = true, UART = true, EMI = true, DCI = true}
#     uC.PERIPH["STM32F469ZExx"] = {GPIO = true, CLK = true, CRC = true, RTC = true, I2C = true, DMA = true, AFM = true, SND = true, IRQ = true, NVM = true, PWM = true, CAN = true, SDIO = true, WDG = true, SPI = true, UART = true, EMI = true, ETH = true, DCI = true}
#     uC.PERIPH["STM32F469ZGxx"] = {GPIO = true, CLK = true, CRC = true, RTC = true, I2C = true, DMA = true, AFM = true, SND = true, IRQ = true, NVM = true, PWM = true, CAN = true, SDIO = true, WDG = true, SPI = true, UART = true, EMI = true, ETH = true, DCI = true}
#     uC.PERIPH["STM32F469ZIxx"] = {GPIO = true, CLK = true, CRC = true, RTC = true, I2C = true, DMA = true, AFM = true, SND = true, IRQ = true, NVM = true, PWM = true, CAN = true, SDIO = true, WDG = true, SPI = true, UART = true, EMI = true, ETH = true, DCI = true}
#     uC.PERIPH["STM32F469AExx"] = {GPIO = true, CLK = true, CRC = true, RTC = true, I2C = true, DMA = true, AFM = true, SND = true, IRQ = true, NVM = true, PWM = true, CAN = true, SDIO = true, WDG = true, SPI = true, UART = true, EMI = true, DCI = true}
#     uC.PERIPH["STM32F469AGxx"] = {GPIO = true, CLK = true, CRC = true, RTC = true, I2C = true, DMA = true, AFM = true, SND = true, IRQ = true, NVM = true, PWM = true, CAN = true, SDIO = true, WDG = true, SPI = true, UART = true, EMI = true, DCI = true}
#     uC.PERIPH["STM32F469AIxx"] = {GPIO = true, CLK = true, CRC = true, RTC = true, I2C = true, DMA = true, AFM = true, SND = true, IRQ = true, NVM = true, PWM = true, CAN = true, SDIO = true, WDG = true, SPI = true, UART = true, EMI = true, DCI = true}
#     uC.PERIPH["STM32F469IExx"] = {GPIO = true, CLK = true, CRC = true, RTC = true, I2C = true, DMA = true, AFM = true, SND = true, IRQ = true, NVM = true, PWM = true, CAN = true, SDIO = true, WDG = true, SPI = true, UART = true, EMI = true, ETH = true, DCI = true}
#     uC.PERIPH["STM32F469IGxx"] = {GPIO = true, CLK = true, CRC = true, RTC = true, I2C = true, DMA = true, AFM = true, SND = true, IRQ = true, NVM = true, PWM = true, CAN = true, SDIO = true, WDG = true, SPI = true, UART = true, EMI = true, ETH = true, DCI = true}
#     uC.PERIPH["STM32F469IIxx"] = {GPIO = true, CLK = true, CRC = true, RTC = true, I2C = true, DMA = true, AFM = true, SND = true, IRQ = true, NVM = true, PWM = true, CAN = true, SDIO = true, WDG = true, SPI = true, UART = true, EMI = true, ETH = true, DCI = true}
#     uC.PERIPH["STM32F469BExx"] = {GPIO = true, CLK = true, CRC = true, RTC = true, I2C = true, DMA = true, AFM = true, SND = true, IRQ = true, NVM = true, PWM = true, CAN = true, SDIO = true, WDG = true, SPI = true, UART = true, EMI = true, ETH = true, DCI = true}
#     uC.PERIPH["STM32F469BGxx"] = {GPIO = true, CLK = true, CRC = true, RTC = true, I2C = true, DMA = true, AFM = true, SND = true, IRQ = true, NVM = true, PWM = true, CAN = true, SDIO = true, WDG = true, SPI = true, UART = true, EMI = true, ETH = true, DCI = true}
#     uC.PERIPH["STM32F469BIxx"] = {GPIO = true, CLK = true, CRC = true, RTC = true, I2C = true, DMA = true, AFM = true, SND = true, IRQ = true, NVM = true, PWM = true, CAN = true, SDIO = true, WDG = true, SPI = true, UART = true, EMI = true, ETH = true, DCI = true}
#     uC.PERIPH["STM32F469NExx"] = {GPIO = true, CLK = true, CRC = true, RTC = true, I2C = true, DMA = true, AFM = true, SND = true, IRQ = true, NVM = true, PWM = true, CAN = true, SDIO = true, WDG = true, SPI = true, UART = true, EMI = true, ETH = true, DCI = true}
#     uC.PERIPH["STM32F469NGxx"] = {GPIO = true, CLK = true, CRC = true, RTC = true, I2C = true, DMA = true, AFM = true, SND = true, IRQ = true, NVM = true, PWM = true, CAN = true, SDIO = true, WDG = true, SPI = true, UART = true, EMI = true, ETH = true, DCI = true}
#     uC.PERIPH["STM32F469NIxx"] = {GPIO = true, CLK = true, CRC = true, RTC = true, I2C = true, DMA = true, AFM = true, SND = true, IRQ = true, NVM = true, PWM = true, CAN = true, SDIO = true, WDG = true, SPI = true, UART = true, EMI = true, ETH = true, DCI = true}
#     uC.PERIPH["STM32F479VGxx"] = {GPIO = true, CLK = true, CRC = true, RTC = true, I2C = true, DMA = true, AFM = true, SND = true, IRQ = true, NVM = true, PWM = true, SDIO = true, WDG = true, SPI = true, UART = true, EMI = true, DCI = true}
#     uC.PERIPH["STM32F479VIxx"] = {GPIO = true, CLK = true, CRC = true, RTC = true, I2C = true, DMA = true, AFM = true, SND = true, IRQ = true, NVM = true, PWM = true, SDIO = true, WDG = true, SPI = true, UART = true, EMI = true, DCI = true}
#     uC.PERIPH["STM32F479ZGxx"] = {GPIO = true, CLK = true, CRC = true, RTC = true, I2C = true, DMA = true, AFM = true, SND = true, IRQ = true, NVM = true, PWM = true, SDIO = true, WDG = true, SPI = true, UART = true, EMI = true, ETH = true, DCI = true}
#     uC.PERIPH["STM32F479ZIxx"] = {GPIO = true, CLK = true, CRC = true, RTC = true, I2C = true, DMA = true, AFM = true, SND = true, IRQ = true, NVM = true, PWM = true, SDIO = true, WDG = true, SPI = true, UART = true, EMI = true, ETH = true, DCI = true}
#     uC.PERIPH["STM32F479AGxx"] = {GPIO = true, CLK = true, CRC = true, RTC = true, I2C = true, DMA = true, AFM = true, SND = true, IRQ = true, NVM = true, PWM = true, SDIO = true, WDG = true, SPI = true, UART = true, EMI = true, DCI = true}
#     uC.PERIPH["STM32F479AIxx"] = {GPIO = true, CLK = true, CRC = true, RTC = true, I2C = true, DMA = true, AFM = true, SND = true, IRQ = true, NVM = true, PWM = true, SDIO = true, WDG = true, SPI = true, UART = true, EMI = true, DCI = true}
#     uC.PERIPH["STM32F479IGxx"] = {GPIO = true, CLK = true, CRC = true, RTC = true, I2C = true, DMA = true, AFM = true, SND = true, IRQ = true, NVM = true, PWM = true, SDIO = true, WDG = true, SPI = true, UART = true, EMI = true, ETH = true, DCI = true}
#     uC.PERIPH["STM32F479IIxx"] = {GPIO = true, CLK = true, CRC = true, RTC = true, I2C = true, DMA = true, AFM = true, SND = true, IRQ = true, NVM = true, PWM = true, SDIO = true, WDG = true, SPI = true, UART = true, EMI = true, ETH = true, DCI = true}
#     uC.PERIPH["STM32F479BGxx"] = {GPIO = true, CLK = true, CRC = true, RTC = true, I2C = true, DMA = true, AFM = true, SND = true, IRQ = true, NVM = true, PWM = true, SDIO = true, WDG = true, SPI = true, UART = true, EMI = true, ETH = true, DCI = true}
#     uC.PERIPH["STM32F479BIxx"] = {GPIO = true, CLK = true, CRC = true, RTC = true, I2C = true, DMA = true, AFM = true, SND = true, IRQ = true, NVM = true, PWM = true, SDIO = true, WDG = true, SPI = true, UART = true, EMI = true, ETH = true, DCI = true}
#     uC.PERIPH["STM32F479NGxx"] = {GPIO = true, CLK = true, CRC = true, RTC = true, I2C = true, DMA = true, AFM = true, SND = true, IRQ = true, NVM = true, PWM = true, SDIO = true, WDG = true, SPI = true, UART = true, EMI = true, ETH = true, DCI = true}
#     uC.PERIPH["STM32F479NIxx"] = {GPIO = true, CLK = true, CRC = true, RTC = true, I2C = true, DMA = true, AFM = true, SND = true, IRQ = true, NVM = true, PWM = true, SDIO = true, WDG = true, SPI = true, UART = true, EMI = true, ETH = true, DCI = true}
# end
#
# if uC.ARCH == "stm32f7" then
#     uC.AddPriorityItems = function(this, no_default)
#         this:AddItem("Priority 0 (the highest)", "0")
#         for i = 1, 14 do this:AddItem("Priority "..i, tostring(i)) end
#         this:AddItem("Priority 15 (the lowest, safe for kernel API)", "15")
#         if no_default ~= true then
#             this:AddItem("Default priority", "__CPU_DEFAULT_IRQ_PRIORITY__")
#         end
#     end
#
#     uC.PERIPH["STM32F722ICxx"] = {GPIO = true, WDG = true, UART = true, AFM = true, DMA = true, PWM = true, IRQ = true, SPI = true, CAN = true, EMI = true, CLK = true, I2C = true, NVM = true, SDIO = true, RTC = true, TIMENC = true}
#     uC.PERIPH["STM32F722IExx"] = {GPIO = true, WDG = true, UART = true, AFM = true, DMA = true, PWM = true, IRQ = true, SPI = true, CAN = true, EMI = true, CLK = true, I2C = true, NVM = true, SDIO = true, RTC = true, TIMENC = true}
#     uC.PERIPH["STM32F722RCxx"] = {GPIO = true, WDG = true, UART = true, AFM = true, DMA = true, PWM = true, IRQ = true, SPI = true, CAN = true, EMI = true, CLK = true, I2C = true, NVM = true, SDIO = true, RTC = true, TIMENC = true}
#     uC.PERIPH["STM32F722RExx"] = {GPIO = true, WDG = true, UART = true, AFM = true, DMA = true, PWM = true, IRQ = true, SPI = true, CAN = true, EMI = true, CLK = true, I2C = true, NVM = true, SDIO = true, RTC = true, TIMENC = true}
#     uC.PERIPH["STM32F722VCxx"] = {GPIO = true, WDG = true, UART = true, AFM = true, DMA = true, PWM = true, IRQ = true, SPI = true, CAN = true, EMI = true, CLK = true, I2C = true, NVM = true, SDIO = true, RTC = true, TIMENC = true}
#     uC.PERIPH["STM32F722VExx"] = {GPIO = true, WDG = true, UART = true, AFM = true, DMA = true, PWM = true, IRQ = true, SPI = true, CAN = true, EMI = true, CLK = true, I2C = true, NVM = true, SDIO = true, RTC = true, TIMENC = true}
#     uC.PERIPH["STM32F722ZCxx"] = {GPIO = true, WDG = true, UART = true, AFM = true, DMA = true, PWM = true, IRQ = true, SPI = true, CAN = true, EMI = true, CLK = true, I2C = true, NVM = true, SDIO = true, RTC = true, TIMENC = true}
#     uC.PERIPH["STM32F722ZExx"] = {GPIO = true, WDG = true, UART = true, AFM = true, DMA = true, PWM = true, IRQ = true, SPI = true, CAN = true, EMI = true, CLK = true, I2C = true, NVM = true, SDIO = true, RTC = true, TIMENC = true}
#     uC.PERIPH["STM32F723ICxx"] = {GPIO = true, WDG = true, UART = true, AFM = true, DMA = true, PWM = true, IRQ = true, SPI = true, CAN = true, EMI = true, CLK = true, I2C = true, NVM = true, SDIO = true, RTC = true, TIMENC = true}
#     uC.PERIPH["STM32F723IExx"] = {GPIO = true, WDG = true, UART = true, AFM = true, DMA = true, PWM = true, IRQ = true, SPI = true, CAN = true, EMI = true, CLK = true, I2C = true, NVM = true, SDIO = true, RTC = true, TIMENC = true}
#     uC.PERIPH["STM32F723VCxx"] = {GPIO = true, WDG = true, UART = true, AFM = true, DMA = true, PWM = true, IRQ = true, SPI = true, CAN = true, EMI = true, CLK = true, I2C = true, NVM = true, SDIO = true, RTC = true, TIMENC = true}
#     uC.PERIPH["STM32F723VExx"] = {GPIO = true, WDG = true, UART = true, AFM = true, DMA = true, PWM = true, IRQ = true, SPI = true, CAN = true, EMI = true, CLK = true, I2C = true, NVM = true, SDIO = true, RTC = true, TIMENC = true}
#     uC.PERIPH["STM32F723ZCxx"] = {GPIO = true, WDG = true, UART = true, AFM = true, DMA = true, PWM = true, IRQ = true, SPI = true, CAN = true, EMI = true, CLK = true, I2C = true, NVM = true, SDIO = true, RTC = true, TIMENC = true}
#     uC.PERIPH["STM32F723ZExx"] = {GPIO = true, WDG = true, UART = true, AFM = true, DMA = true, PWM = true, IRQ = true, SPI = true, CAN = true, EMI = true, CLK = true, I2C = true, NVM = true, SDIO = true, RTC = true, TIMENC = true}
#     uC.PERIPH["STM32F730I8xx"] = {GPIO = true, WDG = true, UART = true, AFM = true, DMA = true, PWM = true, IRQ = true, SPI = true, CAN = true, EMI = true, CLK = true, I2C = true, NVM = true, SDIO = true, RTC = true, TIMENC = true}
#     uC.PERIPH["STM32F730R8xx"] = {GPIO = true, WDG = true, UART = true, AFM = true, DMA = true, PWM = true, IRQ = true, SPI = true, CAN = true, EMI = true, CLK = true, I2C = true, NVM = true, SDIO = true, RTC = true, TIMENC = true}
#     uC.PERIPH["STM32F730V8xx"] = {GPIO = true, WDG = true, UART = true, AFM = true, DMA = true, PWM = true, IRQ = true, SPI = true, CAN = true, EMI = true, CLK = true, I2C = true, NVM = true, SDIO = true, RTC = true, TIMENC = true}
#     uC.PERIPH["STM32F730Z8xx"] = {GPIO = true, WDG = true, UART = true, AFM = true, DMA = true, PWM = true, IRQ = true, SPI = true, CAN = true, EMI = true, CLK = true, I2C = true, NVM = true, SDIO = true, RTC = true, TIMENC = true}
#     uC.PERIPH["STM32F732IExx"] = {GPIO = true, WDG = true, UART = true, AFM = true, DMA = true, PWM = true, IRQ = true, SPI = true, CAN = true, EMI = true, CLK = true, I2C = true, NVM = true, SDIO = true, RTC = true, TIMENC = true}
#     uC.PERIPH["STM32F732RExx"] = {GPIO = true, WDG = true, UART = true, AFM = true, DMA = true, PWM = true, IRQ = true, SPI = true, CAN = true, EMI = true, CLK = true, I2C = true, NVM = true, SDIO = true, RTC = true, TIMENC = true}
#     uC.PERIPH["STM32F732VExx"] = {GPIO = true, WDG = true, UART = true, AFM = true, DMA = true, PWM = true, IRQ = true, SPI = true, CAN = true, EMI = true, CLK = true, I2C = true, NVM = true, SDIO = true, RTC = true, TIMENC = true}
#     uC.PERIPH["STM32F732ZExx"] = {GPIO = true, WDG = true, UART = true, AFM = true, DMA = true, PWM = true, IRQ = true, SPI = true, CAN = true, EMI = true, CLK = true, I2C = true, NVM = true, SDIO = true, RTC = true, TIMENC = true}
#     uC.PERIPH["STM32F733IExx"] = {GPIO = true, WDG = true, UART = true, AFM = true, DMA = true, PWM = true, IRQ = true, SPI = true, CAN = true, EMI = true, CLK = true, I2C = true, NVM = true, SDIO = true, RTC = true, TIMENC = true}
#     uC.PERIPH["STM32F733VExx"] = {GPIO = true, WDG = true, UART = true, AFM = true, DMA = true, PWM = true, IRQ = true, SPI = true, CAN = true, EMI = true, CLK = true, I2C = true, NVM = true, SDIO = true, RTC = true, TIMENC = true}
#     uC.PERIPH["STM32F733ZExx"] = {GPIO = true, WDG = true, UART = true, AFM = true, DMA = true, PWM = true, IRQ = true, SPI = true, CAN = true, EMI = true, CLK = true, I2C = true, NVM = true, SDIO = true, RTC = true, TIMENC = true}
#     uC.PERIPH["STM32F745IExx"] = {GPIO = true, WDG = true, UART = true, AFM = true, DMA = true, PWM = true, IRQ = true, SPI = true, CAN = true, EMI = true, CLK = true, I2C = true, NVM = true, SDIO = true, RTC = true, TIMENC = true, ETH = true}
#     uC.PERIPH["STM32F745IGxx"] = {GPIO = true, WDG = true, UART = true, AFM = true, DMA = true, PWM = true, IRQ = true, SPI = true, CAN = true, EMI = true, CLK = true, I2C = true, NVM = true, SDIO = true, RTC = true, TIMENC = true, ETH = true}
#     uC.PERIPH["STM32F745VExx"] = {GPIO = true, WDG = true, UART = true, AFM = true, DMA = true, PWM = true, IRQ = true, SPI = true, CAN = true, EMI = true, CLK = true, I2C = true, NVM = true, SDIO = true, RTC = true, TIMENC = true, ETH = true}
#     uC.PERIPH["STM32F745VGxx"] = {GPIO = true, WDG = true, UART = true, AFM = true, DMA = true, PWM = true, IRQ = true, SPI = true, CAN = true, EMI = true, CLK = true, I2C = true, NVM = true, SDIO = true, RTC = true, TIMENC = true, ETH = true}
#     uC.PERIPH["STM32F745ZExx"] = {GPIO = true, WDG = true, UART = true, AFM = true, DMA = true, PWM = true, IRQ = true, SPI = true, CAN = true, EMI = true, CLK = true, I2C = true, NVM = true, SDIO = true, RTC = true, TIMENC = true, ETH = true}
#     uC.PERIPH["STM32F745ZGxx"] = {GPIO = true, WDG = true, UART = true, AFM = true, DMA = true, PWM = true, IRQ = true, SPI = true, CAN = true, EMI = true, CLK = true, I2C = true, NVM = true, SDIO = true, RTC = true, TIMENC = true, ETH = true}
#     uC.PERIPH["STM32F746BExx"] = {GPIO = true, WDG = true, UART = true, AFM = true, DMA = true, PWM = true, IRQ = true, SPI = true, CAN = true, EMI = true, CLK = true, I2C = true, NVM = true, SDIO = true, RTC = true, TIMENC = true, ETH = true}
#     uC.PERIPH["STM32F746BGxx"] = {GPIO = true, WDG = true, UART = true, AFM = true, DMA = true, PWM = true, IRQ = true, SPI = true, CAN = true, EMI = true, CLK = true, I2C = true, NVM = true, SDIO = true, RTC = true, TIMENC = true, ETH = true}
#     uC.PERIPH["STM32F746IExx"] = {GPIO = true, WDG = true, UART = true, AFM = true, DMA = true, PWM = true, IRQ = true, SPI = true, CAN = true, EMI = true, CLK = true, I2C = true, NVM = true, SDIO = true, RTC = true, TIMENC = true, ETH = true}
#     uC.PERIPH["STM32F746IGxx"] = {GPIO = true, WDG = true, UART = true, AFM = true, DMA = true, PWM = true, IRQ = true, SPI = true, CAN = true, EMI = true, CLK = true, I2C = true, NVM = true, SDIO = true, RTC = true, TIMENC = true, ETH = true}
#     uC.PERIPH["STM32F746NExx"] = {GPIO = true, WDG = true, UART = true, AFM = true, DMA = true, PWM = true, IRQ = true, SPI = true, CAN = true, EMI = true, CLK = true, I2C = true, NVM = true, SDIO = true, RTC = true, TIMENC = true, ETH = true}
#     uC.PERIPH["STM32F746NGxx"] = {GPIO = true, WDG = true, UART = true, AFM = true, DMA = true, PWM = true, IRQ = true, SPI = true, CAN = true, EMI = true, CLK = true, I2C = true, NVM = true, SDIO = true, RTC = true, TIMENC = true, ETH = true}
#     uC.PERIPH["STM32F746VExx"] = {GPIO = true, WDG = true, UART = true, AFM = true, DMA = true, PWM = true, IRQ = true, SPI = true, CAN = true, EMI = true, CLK = true, I2C = true, NVM = true, SDIO = true, RTC = true, TIMENC = true, ETH = true}
#     uC.PERIPH["STM32F746VGxx"] = {GPIO = true, WDG = true, UART = true, AFM = true, DMA = true, PWM = true, IRQ = true, SPI = true, CAN = true, EMI = true, CLK = true, I2C = true, NVM = true, SDIO = true, RTC = true, TIMENC = true, ETH = true}
#     uC.PERIPH["STM32F746ZExx"] = {GPIO = true, WDG = true, UART = true, AFM = true, DMA = true, PWM = true, IRQ = true, SPI = true, CAN = true, EMI = true, CLK = true, I2C = true, NVM = true, SDIO = true, RTC = true, TIMENC = true, ETH = true}
#     uC.PERIPH["STM32F746ZGxx"] = {GPIO = true, WDG = true, UART = true, AFM = true, DMA = true, PWM = true, IRQ = true, SPI = true, CAN = true, EMI = true, CLK = true, I2C = true, NVM = true, SDIO = true, RTC = true, TIMENC = true, ETH = true}
#     uC.PERIPH["STM32F750N8xx"] = {GPIO = true, WDG = true, UART = true, AFM = true, DMA = true, PWM = true, IRQ = true, SPI = true, CAN = true, EMI = true, CLK = true, I2C = true, NVM = true, SDIO = true, RTC = true, TIMENC = true, ETH = true}
#     uC.PERIPH["STM32F750V8xx"] = {GPIO = true, WDG = true, UART = true, AFM = true, DMA = true, PWM = true, IRQ = true, SPI = true, CAN = true, EMI = true, CLK = true, I2C = true, NVM = true, SDIO = true, RTC = true, TIMENC = true, ETH = true}
#     uC.PERIPH["STM32F750Z8xx"] = {GPIO = true, WDG = true, UART = true, AFM = true, DMA = true, PWM = true, IRQ = true, SPI = true, CAN = true, EMI = true, CLK = true, I2C = true, NVM = true, SDIO = true, RTC = true, TIMENC = true, ETH = true}
#     uC.PERIPH["STM32F756BGxx"] = {GPIO = true, WDG = true, UART = true, AFM = true, DMA = true, PWM = true, IRQ = true, SPI = true, CAN = true, CLK = true, I2C = true, NVM = true, SDIO = true, RTC = true, TIMENC = true, ETH = true}
#     uC.PERIPH["STM32F756IGxx"] = {GPIO = true, WDG = true, UART = true, AFM = true, DMA = true, PWM = true, IRQ = true, SPI = true, CAN = true, CLK = true, I2C = true, NVM = true, SDIO = true, RTC = true, TIMENC = true, ETH = true}
#     uC.PERIPH["STM32F756NGxx"] = {GPIO = true, WDG = true, UART = true, AFM = true, DMA = true, PWM = true, IRQ = true, SPI = true, CAN = true, CLK = true, I2C = true, NVM = true, SDIO = true, RTC = true, TIMENC = true, ETH = true}
#     uC.PERIPH["STM32F756VGxx"] = {GPIO = true, WDG = true, UART = true, AFM = true, DMA = true, PWM = true, IRQ = true, SPI = true, CAN = true, CLK = true, I2C = true, NVM = true, SDIO = true, RTC = true, TIMENC = true, ETH = true}
#     uC.PERIPH["STM32F756ZGxx"] = {GPIO = true, WDG = true, UART = true, AFM = true, DMA = true, PWM = true, IRQ = true, SPI = true, CAN = true, CLK = true, I2C = true, NVM = true, SDIO = true, RTC = true, TIMENC = true, ETH = true}
#     uC.PERIPH["STM32F765BGxx"] = {GPIO = true, WDG = true, UART = true, AFM = true, DMA = true, PWM = true, IRQ = true, SPI = true, CAN = true, EMI = true, CLK = true, I2C = true, NVM = true, SDIO = true, RTC = true, TIMENC = true, ETH = true}
#     uC.PERIPH["STM32F765BIxx"] = {GPIO = true, WDG = true, UART = true, AFM = true, DMA = true, PWM = true, IRQ = true, SPI = true, CAN = true, EMI = true, CLK = true, I2C = true, NVM = true, SDIO = true, RTC = true, TIMENC = true, ETH = true}
#     uC.PERIPH["STM32F765IGxx"] = {GPIO = true, WDG = true, UART = true, AFM = true, DMA = true, PWM = true, IRQ = true, SPI = true, CAN = true, EMI = true, CLK = true, I2C = true, NVM = true, SDIO = true, RTC = true, TIMENC = true, ETH = true}
#     uC.PERIPH["STM32F765IIxx"] = {GPIO = true, WDG = true, UART = true, AFM = true, DMA = true, PWM = true, IRQ = true, SPI = true, CAN = true, EMI = true, CLK = true, I2C = true, NVM = true, SDIO = true, RTC = true, TIMENC = true, ETH = true}
#     uC.PERIPH["STM32F765NGxx"] = {GPIO = true, WDG = true, UART = true, AFM = true, DMA = true, PWM = true, IRQ = true, SPI = true, CAN = true, EMI = true, CLK = true, I2C = true, NVM = true, SDIO = true, RTC = true, TIMENC = true, ETH = true}
#     uC.PERIPH["STM32F765NIxx"] = {GPIO = true, WDG = true, UART = true, AFM = true, DMA = true, PWM = true, IRQ = true, SPI = true, CAN = true, EMI = true, CLK = true, I2C = true, NVM = true, SDIO = true, RTC = true, TIMENC = true, ETH = true}
#     uC.PERIPH["STM32F765VGxx"] = {GPIO = true, WDG = true, UART = true, AFM = true, DMA = true, PWM = true, IRQ = true, SPI = true, CAN = true, EMI = true, CLK = true, I2C = true, NVM = true, SDIO = true, RTC = true, TIMENC = true, ETH = true}
#     uC.PERIPH["STM32F765VIxx"] = {GPIO = true, WDG = true, UART = true, AFM = true, DMA = true, PWM = true, IRQ = true, SPI = true, CAN = true, EMI = true, CLK = true, I2C = true, NVM = true, SDIO = true, RTC = true, TIMENC = true, ETH = true}
#     uC.PERIPH["STM32F765ZGxx"] = {GPIO = true, WDG = true, UART = true, AFM = true, DMA = true, PWM = true, IRQ = true, SPI = true, CAN = true, EMI = true, CLK = true, I2C = true, NVM = true, SDIO = true, RTC = true, TIMENC = true, ETH = true}
#     uC.PERIPH["STM32F765ZIxx"] = {GPIO = true, WDG = true, UART = true, AFM = true, DMA = true, PWM = true, IRQ = true, SPI = true, CAN = true, EMI = true, CLK = true, I2C = true, NVM = true, SDIO = true, RTC = true, TIMENC = true, ETH = true}
#     uC.PERIPH["STM32F767BGxx"] = {GPIO = true, WDG = true, UART = true, AFM = true, DMA = true, PWM = true, IRQ = true, SPI = true, CAN = true, EMI = true, CLK = true, I2C = true, NVM = true, SDIO = true, RTC = true, TIMENC = true, ETH = true}
#     uC.PERIPH["STM32F767BIxx"] = {GPIO = true, WDG = true, UART = true, AFM = true, DMA = true, PWM = true, IRQ = true, SPI = true, CAN = true, EMI = true, CLK = true, I2C = true, NVM = true, SDIO = true, RTC = true, TIMENC = true, ETH = true}
#     uC.PERIPH["STM32F767IGxx"] = {GPIO = true, WDG = true, UART = true, AFM = true, DMA = true, PWM = true, IRQ = true, SPI = true, CAN = true, EMI = true, CLK = true, I2C = true, NVM = true, SDIO = true, RTC = true, TIMENC = true, ETH = true}
#     uC.PERIPH["STM32F767IIxx"] = {GPIO = true, WDG = true, UART = true, AFM = true, DMA = true, PWM = true, IRQ = true, SPI = true, CAN = true, EMI = true, CLK = true, I2C = true, NVM = true, SDIO = true, RTC = true, TIMENC = true, ETH = true}
#     uC.PERIPH["STM32F767NGxx"] = {GPIO = true, WDG = true, UART = true, AFM = true, DMA = true, PWM = true, IRQ = true, SPI = true, CAN = true, EMI = true, CLK = true, I2C = true, NVM = true, SDIO = true, RTC = true, TIMENC = true, ETH = true}
#     uC.PERIPH["STM32F767NIxx"] = {GPIO = true, WDG = true, UART = true, AFM = true, DMA = true, PWM = true, IRQ = true, SPI = true, CAN = true, EMI = true, CLK = true, I2C = true, NVM = true, SDIO = true, RTC = true, TIMENC = true, ETH = true}
#     uC.PERIPH["STM32F767VGxx"] = {GPIO = true, WDG = true, UART = true, AFM = true, DMA = true, PWM = true, IRQ = true, SPI = true, CAN = true, EMI = true, CLK = true, I2C = true, NVM = true, SDIO = true, RTC = true, TIMENC = true, ETH = true}
#     uC.PERIPH["STM32F767VIxx"] = {GPIO = true, WDG = true, UART = true, AFM = true, DMA = true, PWM = true, IRQ = true, SPI = true, CAN = true, EMI = true, CLK = true, I2C = true, NVM = true, SDIO = true, RTC = true, TIMENC = true, ETH = true}
#     uC.PERIPH["STM32F767ZGxx"] = {GPIO = true, WDG = true, UART = true, AFM = true, DMA = true, PWM = true, IRQ = true, SPI = true, CAN = true, EMI = true, CLK = true, I2C = true, NVM = true, SDIO = true, RTC = true, TIMENC = true, ETH = true}
#     uC.PERIPH["STM32F767ZIxx"] = {GPIO = true, WDG = true, UART = true, AFM = true, DMA = true, PWM = true, IRQ = true, SPI = true, CAN = true, EMI = true, CLK = true, I2C = true, NVM = true, SDIO = true, RTC = true, TIMENC = true, ETH = true}
#     uC.PERIPH["STM32F768AIxx"] = {GPIO = true, WDG = true, UART = true, AFM = true, DMA = true, PWM = true, IRQ = true, SPI = true, CAN = true, EMI = true, CLK = true, I2C = true, NVM = true, SDIO = true, RTC = true, TIMENC = true, ETH = true}
#     uC.PERIPH["STM32F769AGxx"] = {GPIO = true, WDG = true, UART = true, AFM = true, DMA = true, PWM = true, IRQ = true, SPI = true, CAN = true, EMI = true, CLK = true, I2C = true, NVM = true, SDIO = true, RTC = true, TIMENC = true, ETH = true}
#     uC.PERIPH["STM32F769AIxx"] = {GPIO = true, WDG = true, UART = true, AFM = true, DMA = true, PWM = true, IRQ = true, SPI = true, CAN = true, EMI = true, CLK = true, I2C = true, NVM = true, SDIO = true, RTC = true, TIMENC = true, ETH = true}
#     uC.PERIPH["STM32F769BGxx"] = {GPIO = true, WDG = true, UART = true, AFM = true, DMA = true, PWM = true, IRQ = true, SPI = true, CAN = true, EMI = true, CLK = true, I2C = true, NVM = true, SDIO = true, RTC = true, TIMENC = true, ETH = true}
#     uC.PERIPH["STM32F769BIxx"] = {GPIO = true, WDG = true, UART = true, AFM = true, DMA = true, PWM = true, IRQ = true, SPI = true, CAN = true, EMI = true, CLK = true, I2C = true, NVM = true, SDIO = true, RTC = true, TIMENC = true, ETH = true}
#     uC.PERIPH["STM32F769IGxx"] = {GPIO = true, WDG = true, UART = true, AFM = true, DMA = true, PWM = true, IRQ = true, SPI = true, CAN = true, EMI = true, CLK = true, I2C = true, NVM = true, SDIO = true, RTC = true, TIMENC = true, ETH = true}
#     uC.PERIPH["STM32F769IIxx"] = {GPIO = true, WDG = true, UART = true, AFM = true, DMA = true, PWM = true, IRQ = true, SPI = true, CAN = true, EMI = true, CLK = true, I2C = true, NVM = true, SDIO = true, RTC = true, TIMENC = true, ETH = true}
#     uC.PERIPH["STM32F769NGxx"] = {GPIO = true, WDG = true, UART = true, AFM = true, DMA = true, PWM = true, IRQ = true, SPI = true, CAN = true, EMI = true, CLK = true, I2C = true, NVM = true, SDIO = true, RTC = true, TIMENC = true, ETH = true}
#     uC.PERIPH["STM32F769NIxx"] = {GPIO = true, WDG = true, UART = true, AFM = true, DMA = true, PWM = true, IRQ = true, SPI = true, CAN = true, EMI = true, CLK = true, I2C = true, NVM = true, SDIO = true, RTC = true, TIMENC = true, ETH = true}
#     uC.PERIPH["STM32F777BIxx"] = {GPIO = true, WDG = true, UART = true, AFM = true, DMA = true, PWM = true, IRQ = true, SPI = true, CAN = true, EMI = true, CLK = true, I2C = true, NVM = true, SDIO = true, RTC = true, TIMENC = true, ETH = true}
#     uC.PERIPH["STM32F777IIxx"] = {GPIO = true, WDG = true, UART = true, AFM = true, DMA = true, PWM = true, IRQ = true, SPI = true, CAN = true, EMI = true, CLK = true, I2C = true, NVM = true, SDIO = true, RTC = true, TIMENC = true, ETH = true}
#     uC.PERIPH["STM32F777NIxx"] = {GPIO = true, WDG = true, UART = true, AFM = true, DMA = true, PWM = true, IRQ = true, SPI = true, CAN = true, EMI = true, CLK = true, I2C = true, NVM = true, SDIO = true, RTC = true, TIMENC = true, ETH = true}
#     uC.PERIPH["STM32F777VIxx"] = {GPIO = true, WDG = true, UART = true, AFM = true, DMA = true, PWM = true, IRQ = true, SPI = true, CAN = true, EMI = true, CLK = true, I2C = true, NVM = true, SDIO = true, RTC = true, TIMENC = true, ETH = true}
#     uC.PERIPH["STM32F777ZIxx"] = {GPIO = true, WDG = true, UART = true, AFM = true, DMA = true, PWM = true, IRQ = true, SPI = true, CAN = true, EMI = true, CLK = true, I2C = true, NVM = true, SDIO = true, RTC = true, TIMENC = true, ETH = true}
#     uC.PERIPH["STM32F778AIxx"] = {GPIO = true, WDG = true, UART = true, AFM = true, DMA = true, PWM = true, IRQ = true, SPI = true, CAN = true, EMI = true, CLK = true, I2C = true, NVM = true, SDIO = true, RTC = true, TIMENC = true, ETH = true}
#     uC.PERIPH["STM32F779AIxx"] = {GPIO = true, WDG = true, UART = true, AFM = true, DMA = true, PWM = true, IRQ = true, SPI = true, CAN = true, EMI = true, CLK = true, I2C = true, NVM = true, SDIO = true, RTC = true, TIMENC = true, ETH = true}
#     uC.PERIPH["STM32F779BIxx"] = {GPIO = true, WDG = true, UART = true, AFM = true, DMA = true, PWM = true, IRQ = true, SPI = true, CAN = true, EMI = true, CLK = true, I2C = true, NVM = true, SDIO = true, RTC = true, TIMENC = true, ETH = true}
#     uC.PERIPH["STM32F779IIxx"] = {GPIO = true, WDG = true, UART = true, AFM = true, DMA = true, PWM = true, IRQ = true, SPI = true, CAN = true, EMI = true, CLK = true, I2C = true, NVM = true, SDIO = true, RTC = true, TIMENC = true, ETH = true}
#     uC.PERIPH["STM32F779NIxx"] = {GPIO = true, WDG = true, UART = true, AFM = true, DMA = true, PWM = true, IRQ = true, SPI = true, CAN = true, EMI = true, CLK = true, I2C = true, NVM = true, SDIO = true, RTC = true, TIMENC = true, ETH = true}
# end
#
# if uC.ARCH == "stm32h7" then
#     uC.AddPriorityItems = function(this, no_default)
#         this:AddItem("Priority 0 (the highest)", "0")
#         for i = 1, 14 do this:AddItem("Priority "..i, tostring(i)) end
#         this:AddItem("Priority 15 (the lowest, safe for kernel API)", "15")
#         if no_default ~= true then
#             this:AddItem("Default priority", "__CPU_DEFAULT_IRQ_PRIORITY__")
#         end
#     end
#
#     uC.PERIPH["STM32H723VExx" ] = {GPIO = true, CLK = true, AFM = true, DMA = true, UART = true, EMI = true, ETH = true}
#     uC.PERIPH["STM32H723VGxx" ] = {GPIO = true, CLK = true, AFM = true, DMA = true, UART = true, EMI = true, ETH = true}
#     uC.PERIPH["STM32H723ZExx" ] = {GPIO = true, CLK = true, AFM = true, DMA = true, UART = true, EMI = true, ETH = true}
#     uC.PERIPH["STM32H723ZGxx" ] = {GPIO = true, CLK = true, AFM = true, DMA = true, UART = true, EMI = true, ETH = true}
#     uC.PERIPH["STM32H725AExx" ] = {GPIO = true, CLK = true, AFM = true, DMA = true, UART = true, EMI = true, ETH = true}
#     uC.PERIPH["STM32H725AGxx" ] = {GPIO = true, CLK = true, AFM = true, DMA = true, UART = true, EMI = true, ETH = true}
#     uC.PERIPH["STM32H725IExx" ] = {GPIO = true, CLK = true, AFM = true, DMA = true, UART = true, EMI = true, ETH = true}
#     uC.PERIPH["STM32H725IGxx" ] = {GPIO = true, CLK = true, AFM = true, DMA = true, UART = true, EMI = true, ETH = true}
#     uC.PERIPH["STM32H725RExx" ] = {GPIO = true, CLK = true, AFM = true, DMA = true, UART = true, ETH = true}
#     uC.PERIPH["STM32H725RGxx" ] = {GPIO = true, CLK = true, AFM = true, DMA = true, UART = true, ETH = true}
#     uC.PERIPH["STM32H725VExx" ] = {GPIO = true, CLK = true, AFM = true, DMA = true, UART = true, EMI = true, ETH = true}
#     uC.PERIPH["STM32H725VGxx" ] = {GPIO = true, CLK = true, AFM = true, DMA = true, UART = true, EMI = true, ETH = true}
#     uC.PERIPH["STM32H725ZExx" ] = {GPIO = true, CLK = true, AFM = true, DMA = true, UART = true, EMI = true, ETH = true}
#     uC.PERIPH["STM32H725ZGxx" ] = {GPIO = true, CLK = true, AFM = true, DMA = true, UART = true, EMI = true, ETH = true}
#     uC.PERIPH["STM32H730ABxxQ"] = {GPIO = true, CLK = true, AFM = true, DMA = true, UART = true, EMI = true, ETH = true}
#     uC.PERIPH["STM32H730IBxxQ"] = {GPIO = true, CLK = true, AFM = true, DMA = true, UART = true, EMI = true, ETH = true}
#     uC.PERIPH["STM32H730VBxx" ] = {GPIO = true, CLK = true, AFM = true, DMA = true, UART = true, EMI = true, ETH = true}
#     uC.PERIPH["STM32H730ZBxx" ] = {GPIO = true, CLK = true, AFM = true, DMA = true, UART = true, EMI = true, ETH = true}
#     uC.PERIPH["STM32H733VGxx" ] = {GPIO = true, CLK = true, AFM = true, DMA = true, UART = true, EMI = true, ETH = true}
#     uC.PERIPH["STM32H733ZGxx" ] = {GPIO = true, CLK = true, AFM = true, DMA = true, UART = true, EMI = true, ETH = true}
#     uC.PERIPH["STM32H735AGxx" ] = {GPIO = true, CLK = true, AFM = true, DMA = true, UART = true, EMI = true, ETH = true}
#     uC.PERIPH["STM32H735IGxx" ] = {GPIO = true, CLK = true, AFM = true, DMA = true, UART = true, EMI = true, ETH = true}
#     uC.PERIPH["STM32H735RGxx" ] = {GPIO = true, CLK = true, AFM = true, DMA = true, UART = true, EMI = true, ETH = true}
#     uC.PERIPH["STM32H735VGxx" ] = {GPIO = true, CLK = true, AFM = true, DMA = true, UART = true, EMI = true, ETH = true}
#     uC.PERIPH["STM32H735ZGxx" ] = {GPIO = true, CLK = true, AFM = true, DMA = true, UART = true, EMI = true, ETH = true}
#     uC.PERIPH["STM32H742AGxx" ] = {GPIO = true, CLK = true, AFM = true, DMA = true, UART = true, EMI = true, ETH = true}
#     uC.PERIPH["STM32H742BGxx" ] = {GPIO = true, CLK = true, AFM = true, DMA = true, UART = true, EMI = true, ETH = true}
#     uC.PERIPH["STM32H742IGxx" ] = {GPIO = true, CLK = true, AFM = true, DMA = true, UART = true, EMI = true, ETH = true}
#     uC.PERIPH["STM32H742VGxx" ] = {GPIO = true, CLK = true, AFM = true, DMA = true, UART = true, EMI = true, ETH = true}
#     uC.PERIPH["STM32H742XGxx" ] = {GPIO = true, CLK = true, AFM = true, DMA = true, UART = true, EMI = true, ETH = true}
#     uC.PERIPH["STM32H742ZGxx" ] = {GPIO = true, CLK = true, AFM = true, DMA = true, UART = true, EMI = true, ETH = true}
#     uC.PERIPH["STM32H742AIxx" ] = {GPIO = true, CLK = true, AFM = true, DMA = true, UART = true, EMI = true, ETH = true}
#     uC.PERIPH["STM32H742BIxx" ] = {GPIO = true, CLK = true, AFM = true, DMA = true, UART = true, EMI = true, ETH = true}
#     uC.PERIPH["STM32H742IIxx" ] = {GPIO = true, CLK = true, AFM = true, DMA = true, UART = true, EMI = true, ETH = true}
#     uC.PERIPH["STM32H742VIxx" ] = {GPIO = true, CLK = true, AFM = true, DMA = true, UART = true, EMI = true, ETH = true}
#     uC.PERIPH["STM32H742XIxx" ] = {GPIO = true, CLK = true, AFM = true, DMA = true, UART = true, EMI = true, ETH = true}
#     uC.PERIPH["STM32H742ZIxx" ] = {GPIO = true, CLK = true, AFM = true, DMA = true, UART = true, EMI = true, ETH = true}
#     uC.PERIPH["STM32H743AGxx" ] = {GPIO = true, CLK = true, AFM = true, DMA = true, UART = true, EMI = true, ETH = true}
#     uC.PERIPH["STM32H743BGxx" ] = {GPIO = true, CLK = true, AFM = true, DMA = true, UART = true, EMI = true, ETH = true}
#     uC.PERIPH["STM32H743IGxx" ] = {GPIO = true, CLK = true, AFM = true, DMA = true, UART = true, EMI = true, ETH = true}
#     uC.PERIPH["STM32H743VGxx" ] = {GPIO = true, CLK = true, AFM = true, DMA = true, UART = true, EMI = true, ETH = true}
#     uC.PERIPH["STM32H743XGxx" ] = {GPIO = true, CLK = true, AFM = true, DMA = true, UART = true, EMI = true, ETH = true}
#     uC.PERIPH["STM32H743ZGxx" ] = {GPIO = true, CLK = true, AFM = true, DMA = true, UART = true, EMI = true, ETH = true}
#     uC.PERIPH["STM32H743AIxx" ] = {GPIO = true, CLK = true, AFM = true, DMA = true, UART = true, EMI = true, ETH = true}
#     uC.PERIPH["STM32H743BIxx" ] = {GPIO = true, CLK = true, AFM = true, DMA = true, UART = true, EMI = true, ETH = true}
#     uC.PERIPH["STM32H743IIxx" ] = {GPIO = true, CLK = true, AFM = true, DMA = true, UART = true, EMI = true, ETH = true}
#     uC.PERIPH["STM32H743VIxx" ] = {GPIO = true, CLK = true, AFM = true, DMA = true, UART = true, EMI = true, ETH = true}
#     uC.PERIPH["STM32H743XIxx" ] = {GPIO = true, CLK = true, AFM = true, DMA = true, UART = true, EMI = true, ETH = true}
#     uC.PERIPH["STM32H743ZIxx" ] = {GPIO = true, CLK = true, AFM = true, DMA = true, UART = true, EMI = true, ETH = true}
#     uC.PERIPH["STM32H745BGxx" ] = {GPIO = true, CLK = true, AFM = true, DMA = true, UART = true, EMI = true, ETH = true}
#     uC.PERIPH["STM32H745IGxx" ] = {GPIO = true, CLK = true, AFM = true, DMA = true, UART = true, EMI = true, ETH = true}
#     uC.PERIPH["STM32H745XGxx" ] = {GPIO = true, CLK = true, AFM = true, DMA = true, UART = true, EMI = true, ETH = true}
#     uC.PERIPH["STM32H745ZGxx" ] = {GPIO = true, CLK = true, AFM = true, DMA = true, UART = true, EMI = true, ETH = true}
#     uC.PERIPH["STM32H745BIxx" ] = {GPIO = true, CLK = true, AFM = true, DMA = true, UART = true, EMI = true, ETH = true}
#     uC.PERIPH["STM32H745IIxx" ] = {GPIO = true, CLK = true, AFM = true, DMA = true, UART = true, EMI = true, ETH = true}
#     uC.PERIPH["STM32H745XIxx" ] = {GPIO = true, CLK = true, AFM = true, DMA = true, UART = true, EMI = true, ETH = true}
#     uC.PERIPH["STM32H745ZIxx" ] = {GPIO = true, CLK = true, AFM = true, DMA = true, UART = true, EMI = true, ETH = true}
#     uC.PERIPH["STM32H747AGxx" ] = {GPIO = true, CLK = true, AFM = true, DMA = true, UART = true, EMI = true, ETH = true}
#     uC.PERIPH["STM32H747BGxx" ] = {GPIO = true, CLK = true, AFM = true, DMA = true, UART = true, EMI = true, ETH = true}
#     uC.PERIPH["STM32H747IGxx" ] = {GPIO = true, CLK = true, AFM = true, DMA = true, UART = true, EMI = true, ETH = true}
#     uC.PERIPH["STM32H747XGxx" ] = {GPIO = true, CLK = true, AFM = true, DMA = true, UART = true, EMI = true, ETH = true}
#     uC.PERIPH["STM32H747ZGxx" ] = {GPIO = true, CLK = true, AFM = true, DMA = true, UART = true, EMI = true, ETH = true}
#     uC.PERIPH["STM32H747AIxx" ] = {GPIO = true, CLK = true, AFM = true, DMA = true, UART = true, EMI = true, ETH = true}
#     uC.PERIPH["STM32H747BIxx" ] = {GPIO = true, CLK = true, AFM = true, DMA = true, UART = true, EMI = true, ETH = true}
#     uC.PERIPH["STM32H747IIxx" ] = {GPIO = true, CLK = true, AFM = true, DMA = true, UART = true, EMI = true, ETH = true}
#     uC.PERIPH["STM32H747XIxx" ] = {GPIO = true, CLK = true, AFM = true, DMA = true, UART = true, EMI = true, ETH = true}
#     uC.PERIPH["STM32H747ZIxx" ] = {GPIO = true, CLK = true, AFM = true, DMA = true, UART = true, EMI = true, ETH = true}
#     uC.PERIPH["STM32H750IBxx" ] = {GPIO = true, CLK = true, AFM = true, DMA = true, UART = true, EMI = true, ETH = true}
#     uC.PERIPH["STM32H750VBxx" ] = {GPIO = true, CLK = true, AFM = true, DMA = true, UART = true, EMI = true, ETH = true}
#     uC.PERIPH["STM32H750XBxx" ] = {GPIO = true, CLK = true, AFM = true, DMA = true, UART = true, EMI = true, ETH = true}
#     uC.PERIPH["STM32H750ZBxx" ] = {GPIO = true, CLK = true, AFM = true, DMA = true, UART = true, EMI = true, ETH = true}
#     uC.PERIPH["STM32H753AIxx" ] = {GPIO = true, CLK = true, AFM = true, DMA = true, UART = true, EMI = true, ETH = true}
#     uC.PERIPH["STM32H753BIxx" ] = {GPIO = true, CLK = true, AFM = true, DMA = true, UART = true, EMI = true, ETH = true}
#     uC.PERIPH["STM32H753IIxx" ] = {GPIO = true, CLK = true, AFM = true, DMA = true, UART = true, EMI = true, ETH = true}
#     uC.PERIPH["STM32H753VIxx" ] = {GPIO = true, CLK = true, AFM = true, DMA = true, UART = true, EMI = true, ETH = true}
#     uC.PERIPH["STM32H753XIxx" ] = {GPIO = true, CLK = true, AFM = true, DMA = true, UART = true, EMI = true, ETH = true}
#     uC.PERIPH["STM32H753ZIxx" ] = {GPIO = true, CLK = true, AFM = true, DMA = true, UART = true, EMI = true, ETH = true}
#     uC.PERIPH["STM32H755BIxx" ] = {GPIO = true, CLK = true, AFM = true, DMA = true, UART = true, EMI = true, ETH = true}
#     uC.PERIPH["STM32H755IIxx" ] = {GPIO = true, CLK = true, AFM = true, DMA = true, UART = true, EMI = true, ETH = true}
#     uC.PERIPH["STM32H755XIxx" ] = {GPIO = true, CLK = true, AFM = true, DMA = true, UART = true, EMI = true, ETH = true}
#     uC.PERIPH["STM32H755ZIxx" ] = {GPIO = true, CLK = true, AFM = true, DMA = true, UART = true, EMI = true, ETH = true}
#     uC.PERIPH["STM32H757AIxx" ] = {GPIO = true, CLK = true, AFM = true, DMA = true, UART = true, EMI = true, ETH = true}
#     uC.PERIPH["STM32H757BIxx" ] = {GPIO = true, CLK = true, AFM = true, DMA = true, UART = true, EMI = true, ETH = true}
#     uC.PERIPH["STM32H757IIxx" ] = {GPIO = true, CLK = true, AFM = true, DMA = true, UART = true, EMI = true, ETH = true}
#     uC.PERIPH["STM32H757XIxx" ] = {GPIO = true, CLK = true, AFM = true, DMA = true, UART = true, EMI = true, ETH = true}
#     uC.PERIPH["STM32H757ZIxx" ] = {GPIO = true, CLK = true, AFM = true, DMA = true, UART = true, EMI = true, ETH = true}
#     uC.PERIPH["STM32H7A3AGxxQ"] = {GPIO = true, CLK = true, AFM = true, DMA = true, UART = true, EMI = true}
#     uC.PERIPH["STM32H7A3IGxx" ] = {GPIO = true, CLK = true, AFM = true, DMA = true, UART = true, EMI = true}
#     uC.PERIPH["STM32H7A3IGxxQ"] = {GPIO = true, CLK = true, AFM = true, DMA = true, UART = true, EMI = true}
#     uC.PERIPH["STM32H7A3LGxxQ"] = {GPIO = true, CLK = true, AFM = true, DMA = true, UART = true, EMI = true}
#     uC.PERIPH["STM32H7A3NGxx" ] = {GPIO = true, CLK = true, AFM = true, DMA = true, UART = true, EMI = true}
#     uC.PERIPH["STM32H7A3QGxxQ"] = {GPIO = true, CLK = true, AFM = true, DMA = true, UART = true, EMI = true}
#     uC.PERIPH["STM32H7A3RGxx" ] = {GPIO = true, CLK = true, AFM = true, DMA = true, UART = true, EMI = true}
#     uC.PERIPH["STM32H7A3VGxx" ] = {GPIO = true, CLK = true, AFM = true, DMA = true, UART = true, EMI = true}
#     uC.PERIPH["STM32H7A3VGxxQ"] = {GPIO = true, CLK = true, AFM = true, DMA = true, UART = true, EMI = true}
#     uC.PERIPH["STM32H7A3ZGxx" ] = {GPIO = true, CLK = true, AFM = true, DMA = true, UART = true, EMI = true}
#     uC.PERIPH["STM32H7A3ZGxxQ"] = {GPIO = true, CLK = true, AFM = true, DMA = true, UART = true, EMI = true}
#     uC.PERIPH["STM32H7A3AIxxQ"] = {GPIO = true, CLK = true, AFM = true, DMA = true, UART = true, EMI = true}
#     uC.PERIPH["STM32H7A3IIxx" ] = {GPIO = true, CLK = true, AFM = true, DMA = true, UART = true, EMI = true}
#     uC.PERIPH["STM32H7A3IIxxQ"] = {GPIO = true, CLK = true, AFM = true, DMA = true, UART = true, EMI = true}
#     uC.PERIPH["STM32H7A3LIxxQ"] = {GPIO = true, CLK = true, AFM = true, DMA = true, UART = true, EMI = true}
#     uC.PERIPH["STM32H7A3NIxx" ] = {GPIO = true, CLK = true, AFM = true, DMA = true, UART = true, EMI = true}
#     uC.PERIPH["STM32H7A3QIxxQ"] = {GPIO = true, CLK = true, AFM = true, DMA = true, UART = true, EMI = true}
#     uC.PERIPH["STM32H7A3RIxx" ] = {GPIO = true, CLK = true, AFM = true, DMA = true, UART = true, EMI = true}
#     uC.PERIPH["STM32H7A3VIxx" ] = {GPIO = true, CLK = true, AFM = true, DMA = true, UART = true, EMI = true}
#     uC.PERIPH["STM32H7A3VIxxQ"] = {GPIO = true, CLK = true, AFM = true, DMA = true, UART = true, EMI = true}
#     uC.PERIPH["STM32H7A3ZIxx" ] = {GPIO = true, CLK = true, AFM = true, DMA = true, UART = true, EMI = true}
#     uC.PERIPH["STM32H7A3ZIxxQ"] = {GPIO = true, CLK = true, AFM = true, DMA = true, UART = true, EMI = true}
#     uC.PERIPH["STM32H7B0ABxxQ"] = {GPIO = true, CLK = true, AFM = true, DMA = true, UART = true, EMI = true}
#     uC.PERIPH["STM32H7B0IBKxQ"] = {GPIO = true, CLK = true, AFM = true, DMA = true, UART = true, EMI = true}
#     uC.PERIPH["STM32H7B0IBxx" ] = {GPIO = true, CLK = true, AFM = true, DMA = true, UART = true, EMI = true}
#     uC.PERIPH["STM32H7B0RBxx" ] = {GPIO = true, CLK = true, AFM = true, DMA = true, UART = true, EMI = true}
#     uC.PERIPH["STM32H7B0VBxx" ] = {GPIO = true, CLK = true, AFM = true, DMA = true, UART = true, EMI = true}
#     uC.PERIPH["STM32H7B0ZBxx" ] = {GPIO = true, CLK = true, AFM = true, DMA = true, UART = true, EMI = true}
#     uC.PERIPH["STM32H7B3AIxxQ"] = {GPIO = true, CLK = true, AFM = true, DMA = true, UART = true, EMI = true}
#     uC.PERIPH["STM32H7B3IIxx" ] = {GPIO = true, CLK = true, AFM = true, DMA = true, UART = true, EMI = true}
#     uC.PERIPH["STM32H7B3IIxxQ"] = {GPIO = true, CLK = true, AFM = true, DMA = true, UART = true, EMI = true}
#     uC.PERIPH["STM32H7B3LIxxQ"] = {GPIO = true, CLK = true, AFM = true, DMA = true, UART = true, EMI = true}
#     uC.PERIPH["STM32H7B3NIxx" ] = {GPIO = true, CLK = true, AFM = true, DMA = true, UART = true, EMI = true}
#     uC.PERIPH["STM32H7B3QIxxQ"] = {GPIO = true, CLK = true, AFM = true, DMA = true, UART = true, EMI = true}
#     uC.PERIPH["STM32H7B3RIxx" ] = {GPIO = true, CLK = true, AFM = true, DMA = true, UART = true, EMI = true}
#     uC.PERIPH["STM32H7B3VIxx" ] = {GPIO = true, CLK = true, AFM = true, DMA = true, UART = true, EMI = true}
#     uC.PERIPH["STM32H7B3VIxxQ"] = {GPIO = true, CLK = true, AFM = true, DMA = true, UART = true, EMI = true}
#     uC.PERIPH["STM32H7B3ZIxx" ] = {GPIO = true, CLK = true, AFM = true, DMA = true, UART = true, EMI = true}
#     uC.PERIPH["STM32H7B3ZIxxQ"] = {GPIO = true, CLK = true, AFM = true, DMA = true, UART = true, EMI = true}
# end
#++*/

#/* include of CPU mandatory file in Makefile
include ./config/arch/$(__CPU_ARCH__)/cpu_flags.h
#*/

#define ARCH_noarch
#include "noarch/zero_flags.h"
#include "noarch/urandom_flags.h"
#include "noarch/part_flags.h"
#include "noarch/spiee_flags.h"
#include "noarch/loop_flags.h"
#include "noarch/tty_flags.h"
#include "noarch/sdspi_flags.h"
#include "noarch/dht11_flags.h"
#include "noarch/i2cee_flags.h"

#if (__CPU_ARCH__ == stm32f1)
#include "stm32f1/can_flags.h"
#include "stm32f1/cpu_flags.h"
#include "stm32f1/rtc_flags.h"
#include "stm32f1/crc_flags.h"
#include "stm32f1/gpio_flags.h"
#include "stm32f1/afm_flags.h"
#include "stm32f1/eth_flags.h"
#include "stm32f1/clk_flags.h"
#include "stm32f1/spi_flags.h"
#include "stm32f1/wdg_flags.h"
#include "stm32f1/uart_flags.h"
#include "stm32f1/usbd_flags.h"
#include "stm32f1/irq_flags.h"
#include "stm32f1/i2c_flags.h"
#include "stm32f1/nvm_flags.h"
#include "stm32f1/pwm_flags.h"
#elif (__CPU_ARCH__ == stm32f3)
#include "stm32f3/cpu_flags.h"
#include "stm32f3/gpio_flags.h"
#include "stm32f3/uart_flags.h"
#include "stm32f3/clk_flags.h"
#include "stm32f3/can_flags.h"
#include "stm32f3/i2c_flags.h"
#include "stm32f3/irq_flags.h"
#include "stm32f3/pwm_flags.h"
#elif (__CPU_ARCH__ == stm32f4)
#include "stm32f4/can_flags.h"
#include "stm32f4/cpu_flags.h"
#include "stm32f4/gpio_flags.h"
#include "stm32f4/rtc_flags.h"
#include "stm32f4/crc_flags.h"
#include "stm32f4/irq_flags.h"
#include "stm32f4/uart_flags.h"
#include "stm32f4/clk_flags.h"
#include "stm32f4/emi_flags.h"
#include "stm32f4/i2c_flags.h"
#include "stm32f4/afm_flags.h"
#include "stm32f4/eth_flags.h"
#include "stm32f4/dci_flags.h"
#include "stm32f4/spi_flags.h"
#include "stm32f4/wdg_flags.h"
#include "stm32f4/sdio_flags.h"
#include "stm32f4/nvm_flags.h"
#include "stm32f4/pwm_flags.h"
#include "stm32f4/snd_flags.h"
#include "stm32f4/usbh_flags.h"
#elif (__CPU_ARCH__ == stm32f7)
#include "stm32f7/cpu_flags.h"
#include "stm32f7/gpio_flags.h"
#include "stm32f7/uart_flags.h"
#include "stm32f7/wdg_flags.h"
#include "stm32f7/afm_flags.h"
#include "stm32f7/clk_flags.h"
#include "stm32f7/can_flags.h"
#include "stm32f7/i2c_flags.h"
#include "stm32f7/rtc_flags.h"
#include "stm32f7/spi_flags.h"
#include "stm32f7/irq_flags.h"
#include "stm32f7/pwm_flags.h"
#include "stm32f7/eth_flags.h"
#include "stm32f7/sdio_flags.h"
#include "stm32f7/nvm_flags.h"
#include "stm32f7/timenc_flags.h"
#include "stm32f7/emi_flags.h"
#elif (__CPU_ARCH__ == stm32h7)
#include "stm32h7/cpu_flags.h"
#include "stm32h7/gpio_flags.h"
#include "stm32h7/uart_flags.h"
#include "stm32h7/clk_flags.h"
#include "stm32h7/eth_flags.h"
#include "stm32h7/afm_flags.h"
#include "stm32h7/emi_flags.h"
#endif

#/*--
# this:AddExtraWidget("Label", "LabelCPU", "Microcontroller", -1, "bold")
# this:AddExtraWidget("Void", "VoidCPU")
#++*/
#/*--
# this:AddExtraWidget("Label", "LabeluCName", uC.NAME)
# this:AddExtraWidget("Hyperlink", "HL_CPU", "Configure")
# this:SetEvent("clicked", "HL_CPU", function() this:LoadFile("arch/"..uC.ARCH.."/cpu_flags.h") end)
#++*/


#/*--
# this:AddExtraWidget("Label", "LabelModules", "\nModules", -1, "bold")
# this:AddExtraWidget("Void", "VoidModules")
#++*/


#// MODULE LIST BEGIN
#/*--
# if uC.PERIPH[uC.NAME].USBH ~= nil then
#     this:PutWidgets("USBH", "arch/"..uC.ARCH.."/usbh_flags.h")
#     this:SetToolTip("USB Host driver")
# else
#     this:AddWidget("Value")
#     this:SetFlagValue("__ENABLE_USBH__", "_NO_")
# end
#--*/
#define __ENABLE_USBH__ _NO_
#/*
__ENABLE_USBH__=_NO_
#*/

#/*--
# if uC.PERIPH[uC.NAME].GPIO ~= nil then
#     this:PutWidgets("GPIO", "arch/"..uC.ARCH.."/gpio_flags.h")
# else
#     this:AddWidget("Value")
#     this:SetFlagValue("__ENABLE_GPIO__", "_NO_")
# end
#--*/
#define __ENABLE_GPIO__ _YES_
#/*
__ENABLE_GPIO__=_YES_
#*/

#/*--
# if uC.PERIPH[uC.NAME].AFM ~= nil then
#     this:PutWidgets("AFM", "arch/"..uC.ARCH.."/afm_flags.h")
#     this:SetToolTip("Alternative Function Module. Here one can control alternative "..
#                     "function of selected microcontroller.")
# else
#     this:AddWidget("Value")
#     this:SetFlagValue("__ENABLE_AFM__", "_NO_")
# end
#--*/
#define __ENABLE_AFM__ _NO_
#/*
__ENABLE_AFM__=_NO_
#*/

#/*--
# if uC.PERIPH[uC.NAME].CLK ~= nil then
#     this:PutWidgets("CLK", "arch/"..uC.ARCH.."/clk_flags.h")
# else
#     this:AddWidget("Value")
#     this:SetFlagValue("__ENABLE_CLK__", "_NO_")
# end
#--*/
#define __ENABLE_CLK__ _YES_
#/*
__ENABLE_CLK__=_YES_
#*/

#/*--
# if uC.PERIPH[uC.NAME].RTC ~= nil then
#     this:PutWidgets("RTC")
# else
#     this:AddWidget("Value")
#     this:SetFlagValue("__ENABLE_RTC__", "_NO_")
# end
#--*/
#define __ENABLE_RTC__ _NO_
#/*
__ENABLE_RTC__=_NO_
#*/

#/*--
# if uC.PERIPH[uC.NAME].RTC ~= nil then
#     this:PutWidgets("CRC")
# else
#     this:AddWidget("Value")
#     this:SetFlagValue("__ENABLE_CRC__", "_NO_")
# end
#--*/
#define __ENABLE_CRC__ _NO_
#/*
__ENABLE_CRC__=_NO_
#*/

#/*--
# if uC.PERIPH[uC.NAME].ETH ~= nil then
#     this:PutWidgets("ETH", "arch/"..uC.ARCH.."/eth_flags.h")
# else
#     this:AddWidget("Value")
#     this:SetFlagValue("__ENABLE_ETH__", "_NO_")
# end
#--*/
#define __ENABLE_ETH__ _NO_
#/*
__ENABLE_ETH__=_NO_
#*/

#/*--
# if uC.PERIPH[uC.NAME].SPI ~= nil then
#     this:PutWidgets("SPI", "arch/"..uC.ARCH.."/spi_flags.h")
# else
#     this:AddWidget("Value")
#     this:SetFlagValue("__ENABLE_SPI__", "_NO_")
# end
#--*/
#define __ENABLE_SPI__ _NO_
#/*
__ENABLE_SPI__=_NO_
#*/

#/*--
# if uC.PERIPH[uC.NAME].UART ~= nil then
#     this:PutWidgets("UART", "arch/"..uC.ARCH.."/uart_flags.h")
# else
#     this:AddWidget("Value")
#     this:SetFlagValue("__ENABLE_UART__", "_NO_")
# end
#--*/
#define __ENABLE_UART__ _YES_
#/*
__ENABLE_UART__=_YES_
#*/

#/*--
# if uC.PERIPH[uC.NAME].WDG ~= nil then
#     this:PutWidgets("WDG", "arch/"..uC.ARCH.."/wdg_flags.h")
# else
#     this:AddWidget("Value")
#     this:SetFlagValue("__ENABLE_WDG__", "_NO_")
# end
#--*/
#define __ENABLE_WDG__ _NO_
#/*
__ENABLE_WDG__=_NO_
#*/

#/*--
# if uC.PERIPH[uC.NAME].USBD ~= nil then
#     this:PutWidgets("USBD", "arch/"..uC.ARCH.."/usbd_flags.h")
# else
#     this:AddWidget("Value")
#     this:SetFlagValue("__ENABLE_USBD__", "_NO_")
# end
#--*/
#define __ENABLE_USBD__ _NO_
#/*
__ENABLE_USBD__=_NO_
#*/

#/*--
# if uC.PERIPH[uC.NAME].I2C ~= nil then
#     this:PutWidgets("I2C", "arch/"..uC.ARCH.."/i2c_flags.h")
# else
#     this:AddWidget("Value")
#     this:SetFlagValue("__ENABLE_I2C__", "_NO_")
# end
#--*/
#define __ENABLE_I2C__ _NO_
#/*
__ENABLE_I2C__=_NO_
#*/

#/*--
# if uC.PERIPH[uC.NAME].IRQ ~= nil then
#     this:PutWidgets("IRQ", "arch/"..uC.ARCH.."/irq_flags.h")
# else
#     this:AddWidget("Value")
#     this:SetFlagValue("__ENABLE_IRQ__", "_NO_")
# end
#--*/
#define __ENABLE_IRQ__ _NO_
#/*
__ENABLE_IRQ__=_NO_
#*/

#/*--
# this:PutWidgets("LOOP")
#--*/
#define __ENABLE_LOOP__ _NO_
#/*
__ENABLE_LOOP__=_NO_
#*/

#/*--
# this:PutWidgets("I2CEE")
# this:SetToolTip("I2C EEPROM driver for 24Cxx devices.")
#--*/
#define __ENABLE_I2CEE__ _NO_
#/*
__ENABLE_I2CEE__=_NO_
#*/

#/*--
# if uC.PERIPH[uC.NAME].SDIO ~= nil then
#     this:PutWidgets("SDIO", "arch/"..uC.ARCH.."/sdio_flags.h")
#     this:SetToolTip("SD Card Interface Driver")
# else
#     this:AddWidget("Value")
#     this:SetFlagValue("__ENABLE_SDIO__", "_NO_")
# end
#--*/
#define __ENABLE_SDIO__ _NO_
#/*
__ENABLE_SDIO__=_NO_
#*/

#/*--
# this:PutWidgets("SDSPI")
# this:SetToolTip("SD Card driver for SPI interface.\n"..
#                 "Note: make sure that the MISO pin is set to input\n"..
#                 "pulled-up mode or is connected to external pull-up resistor.")
#--*/
#define __ENABLE_SDSPI__ _NO_
#/*
__ENABLE_SDSPI__=_NO_
#*/

#/*--
# this:PutWidgets("TTY", "arch/noarch/tty_flags.h")
#--*/
#define __ENABLE_TTY__ _YES_
#/*
__ENABLE_TTY__=_YES_
#*/

#/*--
# this:PutWidgets("DHT11")
# this:SetToolTip("To use this module set devices pins to open drain.")
#--*/
#define __ENABLE_DHT11__ _NO_
#/*
__ENABLE_DHT11__=_NO_
#*/

#/*--
# if uC.PERIPH[uC.NAME].EMI ~= nil then
#     this:PutWidgets("EMI", "arch/"..uC.ARCH.."/emi_flags.h")
#     this:SetToolTip("External Memory Interface")
# else
#     this:AddWidget("Value")
#     this:SetFlagValue("__ENABLE_EMI__", "_NO_")
# end
#--*/
#define __ENABLE_EMI__ _NO_
#/*
__ENABLE_EMI__=_NO_
#*/

#/*--
# if uC.PERIPH[uC.NAME].DMA ~= nil then
#     this:PutWidgets("DMA")
#     this:SetToolTip("General usage DMA driver.")
# else
#     this:AddWidget("Value")
#     this:SetFlagValue("__ENABLE_DMA__", "_NO_")
# end
#--*/
#define __ENABLE_DMA__ _NO_
#/*
__ENABLE_DMA__=_NO_
#*/

#/*--
# if uC.PERIPH[uC.NAME].DCI ~= nil then
#     this:PutWidgets("DCI", "arch/"..uC.ARCH.."/dci_flags.h")
#     this:SetToolTip("Digital Camera Interface Driver")
# else
#     this:AddWidget("Value")
#     this:SetFlagValue("__ENABLE_DCI__", "_NO_")
# end
#--*/
#define __ENABLE_DCI__ _NO_
#/*
__ENABLE_DCI__=_NO_
#*/

#/*--
# if uC.PERIPH[uC.NAME].CAN ~= nil then
#     this:PutWidgets("CAN", "arch/"..uC.ARCH.."/can_flags.h")
#     this:SetToolTip("CAN driver")
# else
#     this:AddWidget("Value")
#     this:SetFlagValue("__ENABLE_CAN__", "_NO_")
# end
#--*/
#define __ENABLE_CAN__ _NO_
#/*
__ENABLE_CAN__=_NO_
#*/

#/*--
# if uC.PERIPH[uC.NAME].NVM ~= nil then
#     this:PutWidgets("NVM")
#     this:SetToolTip("NVM driver")
# else
#     this:AddWidget("Value")
#     this:SetFlagValue("__ENABLE_NVM__", "_NO_")
# end
#--*/
#define __ENABLE_NVM__ _NO_
#/*
__ENABLE_NVM__=_NO_
#*/

#/*--
# if uC.PERIPH[uC.NAME].PWM ~= nil then
#     this:PutWidgets("PWM")
#     this:SetToolTip("PWM driver")
# else
#     this:AddWidget("Value")
#     this:SetFlagValue("__ENABLE_PWM__", "_NO_")
# end
#--*/
#define __ENABLE_PWM__ _NO_
#/*
__ENABLE_PWM__=_NO_
#*/

#/*--
# this:PutWidgets("SPIEE")
# this:SetToolTip("SPI EEPROM")
#--*/
#define __ENABLE_SPIEE__ _NO_
#/*
__ENABLE_SPIEE__=_NO_
#*/

#/*--
# if uC.PERIPH[uC.NAME].SND ~= nil then
#     this:PutWidgets("SND", "arch/"..uC.ARCH.."/snd_flags.h")
#     this:SetToolTip("Sound interface driver")
# else
#     this:AddWidget("Value")
#     this:SetFlagValue("__ENABLE_SND__", "_NO_")
# end
#--*/
#define __ENABLE_SND__ _NO_
#/*
__ENABLE_SND__=_NO_
#*/

#/*--
# this:PutWidgets("PART")
# this:SetToolTip("Partition device driver")
#--*/
#define __ENABLE_PART__ _NO_
#/*
__ENABLE_PART__=_NO_
#*/

#/*--
# this:PutWidgets("URANDOM")
# this:SetToolTip("Pseudo random generator device")
#--*/
#define __ENABLE_URANDOM__ _NO_
#/*
__ENABLE_URANDOM__=_NO_
#*/

#/*--
# this:PutWidgets("ZERO")
# this:SetToolTip("Zero data device")
#--*/
#define __ENABLE_ZERO__ _NO_
#/*
__ENABLE_ZERO__=_NO_
#*/

#/*--
# if uC.PERIPH[uC.NAME].TIMENC ~= nil then
#     this:PutWidgets("TIMENC", "arch/"..uC.ARCH.."/timenc_flags.h")
#     this:SetToolTip("Timer driven AB encoder")
# else
#     this:AddWidget("Value")
#     this:SetFlagValue("__ENABLE_TIMENC__", "_NO_")
# end
#--*/
#define __ENABLE_TIMENC__ _NO_
#/*
__ENABLE_TIMENC__=_NO_
#*/

#// MODULE LIST END
#//-----------------------------------------------------------------------------
#/*-- save current configuration if CPU was changed
# this:Save()
#++*/

#endif /* _ARCH_FLAGS_H_ */
#/*=============================================================================
#  End of file
#=============================================================================*/
