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
#         this:AddItem("Priority 0 (the highest)", "0x0F")
#         for i = 1, 12 do this:AddItem("Priority "..i, "0x"..i.."F") end
#         this:AddItem("Priority 13 (the lowest)", "0xDF")
#         if no_default ~= true then
#             this:AddItem("Default priority", "__CPU_IRQ_USER_PRIORITY__")
#         end
#     end
#
#     uC.PERIPH["STM32F100C8xx"] = {GPIO = true, CLK = true, CRC = true, WDG = true, UART = true, SPI = true, AFM = true, IRQ = true, I2C = true, RTC = true}
#     uC.PERIPH["STM32F100RBxx"] = {GPIO = true, CLK = true, CRC = true, WDG = true, UART = true, SPI = true, AFM = true, IRQ = true, I2C = true, RTC = true}
#     uC.PERIPH["STM32F100RExx"] = {GPIO = true, CLK = true, CRC = true, WDG = true, UART = true, SPI = true, AFM = true, IRQ = true, I2C = true, RTC = true}
#     uC.PERIPH["STM32F100VBxx"] = {GPIO = true, CLK = true, CRC = true, WDG = true, UART = true, SPI = true, AFM = true, IRQ = true, I2C = true, RTC = true}
#     uC.PERIPH["STM32F100ZCxx"] = {GPIO = true, CLK = true, CRC = true, WDG = true, UART = true, SPI = true, AFM = true, IRQ = true, I2C = true, RTC = true}
#     uC.PERIPH["STM32F101C8xx"] = {GPIO = true, CLK = true, CRC = true, WDG = true, UART = true, SPI = true, AFM = true, IRQ = true, I2C = true, RTC = true}
#     uC.PERIPH["STM32F101CBxx"] = {GPIO = true, CLK = true, CRC = true, WDG = true, UART = true, SPI = true, AFM = true, IRQ = true, I2C = true, RTC = true}
#     uC.PERIPH["STM32F101R8xx"] = {GPIO = true, CLK = true, CRC = true, WDG = true, UART = true, SPI = true, AFM = true, IRQ = true, I2C = true, RTC = true}
#     uC.PERIPH["STM32F101RBxx"] = {GPIO = true, CLK = true, CRC = true, WDG = true, UART = true, SPI = true, AFM = true, IRQ = true, I2C = true, RTC = true}
#     uC.PERIPH["STM32F101V8xx"] = {GPIO = true, CLK = true, CRC = true, WDG = true, UART = true, SPI = true, AFM = true, IRQ = true, I2C = true, RTC = true}
#     uC.PERIPH["STM32F101VBxx"] = {GPIO = true, CLK = true, CRC = true, WDG = true, UART = true, SPI = true, AFM = true, IRQ = true, I2C = true, RTC = true}
#     uC.PERIPH["STM32F103C6xx"] = {GPIO = true, CLK = true, CRC = true, WDG = true, UART = true, SPI = true, AFM = true, IRQ = true, I2C = true, RTC = true, USBD = true}
#     uC.PERIPH["STM32F103C8xx"] = {GPIO = true, CLK = true, CRC = true, WDG = true, UART = true, SPI = true, AFM = true, IRQ = true, I2C = true, RTC = true, USBD = true}
#     uC.PERIPH["STM32F103CBxx"] = {GPIO = true, CLK = true, CRC = true, WDG = true, UART = true, SPI = true, AFM = true, IRQ = true, I2C = true, RTC = true, USBD = true}
#     uC.PERIPH["STM32F103R6xx"] = {GPIO = true, CLK = true, CRC = true, WDG = true, UART = true, SPI = true, AFM = true, IRQ = true, I2C = true, RTC = true, USBD = true}
#     uC.PERIPH["STM32F103R8xx"] = {GPIO = true, CLK = true, CRC = true, WDG = true, UART = true, SPI = true, AFM = true, IRQ = true, I2C = true, RTC = true, USBD = true}
#     uC.PERIPH["STM32F103RBxx"] = {GPIO = true, CLK = true, CRC = true, WDG = true, UART = true, SPI = true, AFM = true, IRQ = true, I2C = true, RTC = true, USBD = true}
#     uC.PERIPH["STM32F103RCxx"] = {GPIO = true, CLK = true, CRC = true, WDG = true, UART = true, SPI = true, AFM = true, IRQ = true, I2C = true, RTC = true, USBD = true}
#     uC.PERIPH["STM32F103RDxx"] = {GPIO = true, CLK = true, CRC = true, WDG = true, UART = true, SPI = true, AFM = true, IRQ = true, I2C = true, RTC = true, USBD = true}
#     uC.PERIPH["STM32F103RExx"] = {GPIO = true, CLK = true, CRC = true, WDG = true, UART = true, SPI = true, AFM = true, IRQ = true, I2C = true, RTC = true, USBD = true}
#     uC.PERIPH["STM32F103RGxx"] = {GPIO = true, CLK = true, CRC = true, WDG = true, UART = true, SPI = true, AFM = true, IRQ = true, I2C = true, RTC = true, USBD = true}
#     uC.PERIPH["STM32F103T8xx"] = {GPIO = true, CLK = true, CRC = true, WDG = true, UART = true, SPI = true, AFM = true, IRQ = true, I2C = true, RTC = true, USBD = true}
#     uC.PERIPH["STM32F103TBxx"] = {GPIO = true, CLK = true, CRC = true, WDG = true, UART = true, SPI = true, AFM = true, IRQ = true, I2C = true, RTC = true, USBD = true}
#     uC.PERIPH["STM32F103V8xx"] = {GPIO = true, CLK = true, CRC = true, WDG = true, UART = true, SPI = true, AFM = true, IRQ = true, I2C = true, RTC = true, USBD = true}
#     uC.PERIPH["STM32F103VBxx"] = {GPIO = true, CLK = true, CRC = true, WDG = true, UART = true, SPI = true, AFM = true, IRQ = true, I2C = true, RTC = true, USBD = true}
#     uC.PERIPH["STM32F103VCxx"] = {GPIO = true, CLK = true, CRC = true, WDG = true, UART = true, SPI = true, AFM = true, IRQ = true, I2C = true, RTC = true, USBD = true}
#     uC.PERIPH["STM32F103VDxx"] = {GPIO = true, CLK = true, CRC = true, WDG = true, UART = true, SPI = true, AFM = true, IRQ = true, I2C = true, RTC = true, USBD = true}
#     uC.PERIPH["STM32F103VExx"] = {GPIO = true, CLK = true, CRC = true, WDG = true, UART = true, SPI = true, AFM = true, IRQ = true, I2C = true, RTC = true, USBD = true}
#     uC.PERIPH["STM32F103VGxx"] = {GPIO = true, CLK = true, CRC = true, WDG = true, UART = true, SPI = true, AFM = true, IRQ = true, I2C = true, RTC = true, USBD = true}
#     uC.PERIPH["STM32F103ZCxx"] = {GPIO = true, CLK = true, CRC = true, WDG = true, UART = true, SPI = true, AFM = true, IRQ = true, I2C = true, RTC = true, USBD = true}
#     uC.PERIPH["STM32F103ZExx"] = {GPIO = true, CLK = true, CRC = true, WDG = true, UART = true, SPI = true, AFM = true, IRQ = true, I2C = true, RTC = true, USBD = true}
#     uC.PERIPH["STM32F103ZGxx"] = {GPIO = true, CLK = true, CRC = true, WDG = true, UART = true, SPI = true, AFM = true, IRQ = true, I2C = true, RTC = true, USBD = true}
#     uC.PERIPH["STM32F105RBxx"] = {GPIO = true, CLK = true, CRC = true, WDG = true, UART = true, SPI = true, AFM = true, IRQ = true, I2C = true, RTC = true}
#     uC.PERIPH["STM32F105RCxx"] = {GPIO = true, CLK = true, CRC = true, WDG = true, UART = true, SPI = true, AFM = true, IRQ = true, I2C = true, RTC = true}
#     uC.PERIPH["STM32F105VBxx"] = {GPIO = true, CLK = true, CRC = true, WDG = true, UART = true, SPI = true, AFM = true, IRQ = true, I2C = true, RTC = true}
#     uC.PERIPH["STM32F105VCxx"] = {GPIO = true, CLK = true, CRC = true, WDG = true, UART = true, SPI = true, AFM = true, IRQ = true, I2C = true, RTC = true}
#     uC.PERIPH["STM32F107RCxx"] = {GPIO = true, CLK = true, CRC = true, WDG = true, UART = true, SPI = true, AFM = true, IRQ = true, I2C = true, RTC = true, ETHMAC = true}
#     uC.PERIPH["STM32F107VBxx"] = {GPIO = true, CLK = true, CRC = true, WDG = true, UART = true, SPI = true, AFM = true, IRQ = true, I2C = true, RTC = true, ETHMAC = true}
#     uC.PERIPH["STM32F107VCxx"] = {GPIO = true, CLK = true, CRC = true, WDG = true, UART = true, SPI = true, AFM = true, IRQ = true, I2C = true, RTC = true, ETHMAC = true}
# end
#
# if uC.ARCH == "stm32f4" then
#     uC.AddPriorityItems = function(this, no_default)
#         this:AddItem("Priority 0 (the highest)", "0x0F")
#         for i = 1, 12 do this:AddItem("Priority "..i, "0x"..i.."F") end
#         this:AddItem("Priority 13 (the lowest)", "0xDF")
#         if no_default ~= true then
#             this:AddItem("Default priority", "__CPU_IRQ_USER_PRIORITY__")
#         end
#     end
#
#     uC.PERIPH["STM32F401CBxx"] = {}
#     uC.PERIPH["STM32F401RBxx"] = {}
#     uC.PERIPH["STM32F401VBxx"] = {}
#     uC.PERIPH["STM32F401CCxx"] = {}
#     uC.PERIPH["STM32F401RCxx"] = {}
#     uC.PERIPH["STM32F401VCxx"] = {}
#     uC.PERIPH["STM32F401CDxx"] = {}
#     uC.PERIPH["STM32F401RDxx"] = {}
#     uC.PERIPH["STM32F401VDxx"] = {}
#     uC.PERIPH["STM32F401CExx"] = {}
#     uC.PERIPH["STM32F401RExx"] = {}
#     uC.PERIPH["STM32F401VExx"] = {}
#     uC.PERIPH["STM32F405RGxx"] = {}
#     uC.PERIPH["STM32F405OExx"] = {}
#     uC.PERIPH["STM32F405OGxx"] = {}
#     uC.PERIPH["STM32F405VGxx"] = {}
#     uC.PERIPH["STM32F405ZGxx"] = {}
#     uC.PERIPH["STM32F407VExx"] = {}
#     uC.PERIPH["STM32F407VGxx"] = {}
#     uC.PERIPH["STM32F407ZExx"] = {}
#     uC.PERIPH["STM32F407ZGxx"] = {}
#     uC.PERIPH["STM32F407IExx"] = {}
#     uC.PERIPH["STM32F407IGxx"] = {}
#     uC.PERIPH["STM32F410T8Yx"] = {}
#     uC.PERIPH["STM32F410TBYx"] = {}
#     uC.PERIPH["STM32F410C8Ux"] = {}
#     uC.PERIPH["STM32F410CBUx"] = {}
#     uC.PERIPH["STM32F410C8Tx"] = {}
#     uC.PERIPH["STM32F410CBTx"] = {}
#     uC.PERIPH["STM32F410R8Tx"] = {}
#     uC.PERIPH["STM32F410RBTx"] = {}
#     uC.PERIPH["STM32F410R8Ix"] = {}
#     uC.PERIPH["STM32F410RBIx"] = {}
#     uC.PERIPH["STM32F411CCxx"] = {}
#     uC.PERIPH["STM32F411RCxx"] = {}
#     uC.PERIPH["STM32F411VCxx"] = {}
#     uC.PERIPH["STM32F411CExx"] = {}
#     uC.PERIPH["STM32F411RExx"] = {}
#     uC.PERIPH["STM32F411VExx"] = {}
#     uC.PERIPH["STM32F412CExx"] = {}
#     uC.PERIPH["STM32F412CGxx"] = {}
#     uC.PERIPH["STM32F412RExx"] = {}
#     uC.PERIPH["STM32F412RGxx"] = {}
#     uC.PERIPH["STM32F412VExx"] = {}
#     uC.PERIPH["STM32F412VGxx"] = {}
#     uC.PERIPH["STM32F412ZExx"] = {}
#     uC.PERIPH["STM32F412ZGxx"] = {}
#     uC.PERIPH["STM32F413CGxx"] = {}
#     uC.PERIPH["STM32F413RGxx"] = {}
#     uC.PERIPH["STM32F413MGxx"] = {}
#     uC.PERIPH["STM32F413VGxx"] = {}
#     uC.PERIPH["STM32F413ZGxx"] = {}
#     uC.PERIPH["STM32F413CHxx"] = {}
#     uC.PERIPH["STM32F413RHxx"] = {}
#     uC.PERIPH["STM32F413MHxx"] = {}
#     uC.PERIPH["STM32F413VHxx"] = {}
#     uC.PERIPH["STM32F413ZHxx"] = {}
#     uC.PERIPH["STM32F415RGxx"] = {}
#     uC.PERIPH["STM32F415OGxx"] = {}
#     uC.PERIPH["STM32F415VGxx"] = {}
#     uC.PERIPH["STM32F415ZGxx"] = {}
#     uC.PERIPH["STM32F417VExx"] = {}
#     uC.PERIPH["STM32F417VGxx"] = {}
#     uC.PERIPH["STM32F417ZExx"] = {}
#     uC.PERIPH["STM32F417ZGxx"] = {}
#     uC.PERIPH["STM32F417IExx"] = {}
#     uC.PERIPH["STM32F417IGxx"] = {}
#     uC.PERIPH["STM32F423CHxx"] = {}
#     uC.PERIPH["STM32F423RHxx"] = {}
#     uC.PERIPH["STM32F423MHxx"] = {}
#     uC.PERIPH["STM32F423VHxx"] = {}
#     uC.PERIPH["STM32F423ZHxx"] = {}
#     uC.PERIPH["STM32F427VGxx"] = {}
#     uC.PERIPH["STM32F427VIxx"] = {}
#     uC.PERIPH["STM32F427ZGxx"] = {}
#     uC.PERIPH["STM32F427ZIxx"] = {}
#     uC.PERIPH["STM32F427AGxx"] = {}
#     uC.PERIPH["STM32F427AIxx"] = {}
#     uC.PERIPH["STM32F427IGxx"] = {}
#     uC.PERIPH["STM32F427IIxx"] = {}
#     uC.PERIPH["STM32F429VExx"] = {}
#     uC.PERIPH["STM32F429VGxx"] = {}
#     uC.PERIPH["STM32F429VIxx"] = {}
#     uC.PERIPH["STM32F429ZExx"] = {}
#     uC.PERIPH["STM32F429ZGxx"] = {}
#     uC.PERIPH["STM32F429ZIxx"] = {}
#     uC.PERIPH["STM32F429AGxx"] = {}
#     uC.PERIPH["STM32F429AIxx"] = {}
#     uC.PERIPH["STM32F429IExx"] = {}
#     uC.PERIPH["STM32F429IGxx"] = {}
#     uC.PERIPH["STM32F429IIxx"] = {}
#     uC.PERIPH["STM32F429BExx"] = {}
#     uC.PERIPH["STM32F429BGxx"] = {}
#     uC.PERIPH["STM32F429BIxx"] = {}
#     uC.PERIPH["STM32F429NExx"] = {}
#     uC.PERIPH["STM32F429NGxx"] = {}
#     uC.PERIPH["STM32F429NIxx"] = {}
#     uC.PERIPH["STM32F437VGxx"] = {}
#     uC.PERIPH["STM32F437VIxx"] = {}
#     uC.PERIPH["STM32F437ZGxx"] = {}
#     uC.PERIPH["STM32F437ZIxx"] = {}
#     uC.PERIPH["STM32F437IGxx"] = {}
#     uC.PERIPH["STM32F437IIxx"] = {}
#     uC.PERIPH["STM32F437AIxx"] = {}
#     uC.PERIPH["STM32F439VGxx"] = {}
#     uC.PERIPH["STM32F439VIxx"] = {}
#     uC.PERIPH["STM32F439ZGxx"] = {}
#     uC.PERIPH["STM32F439ZIxx"] = {}
#     uC.PERIPH["STM32F439IGxx"] = {}
#     uC.PERIPH["STM32F439IIxx"] = {}
#     uC.PERIPH["STM32F439BGxx"] = {}
#     uC.PERIPH["STM32F439BIxx"] = {}
#     uC.PERIPH["STM32F439NGxx"] = {}
#     uC.PERIPH["STM32F439NIxx"] = {}
#     uC.PERIPH["STM32F439AIxx"] = {}
#     uC.PERIPH["STM32F446MCxx"] = {}
#     uC.PERIPH["STM32F446RCxx"] = {}
#     uC.PERIPH["STM32F446VCxx"] = {}
#     uC.PERIPH["STM32F446ZCxx"] = {}
#     uC.PERIPH["STM32F446MExx"] = {}
#     uC.PERIPH["STM32F446RExx"] = {}
#     uC.PERIPH["STM32F446VExx"] = {}
#     uC.PERIPH["STM32F446ZExx"] = {}
#     uC.PERIPH["STM32F469VExx"] = {}
#     uC.PERIPH["STM32F469VGxx"] = {}
#     uC.PERIPH["STM32F469VIxx"] = {}
#     uC.PERIPH["STM32F469ZExx"] = {}
#     uC.PERIPH["STM32F469ZGxx"] = {}
#     uC.PERIPH["STM32F469ZIxx"] = {}
#     uC.PERIPH["STM32F469AExx"] = {}
#     uC.PERIPH["STM32F469AGxx"] = {}
#     uC.PERIPH["STM32F469AIxx"] = {}
#     uC.PERIPH["STM32F469IExx"] = {}
#     uC.PERIPH["STM32F469IGxx"] = {}
#     uC.PERIPH["STM32F469IIxx"] = {}
#     uC.PERIPH["STM32F469BExx"] = {}
#     uC.PERIPH["STM32F469BGxx"] = {}
#     uC.PERIPH["STM32F469BIxx"] = {}
#     uC.PERIPH["STM32F469NExx"] = {}
#     uC.PERIPH["STM32F469NGxx"] = {}
#     uC.PERIPH["STM32F469NIxx"] = {}
#     uC.PERIPH["STM32F479VGxx"] = {}
#     uC.PERIPH["STM32F479VIxx"] = {}
#     uC.PERIPH["STM32F479ZGxx"] = {}
#     uC.PERIPH["STM32F479ZIxx"] = {}
#     uC.PERIPH["STM32F479AGxx"] = {}
#     uC.PERIPH["STM32F479AIxx"] = {}
#     uC.PERIPH["STM32F479IGxx"] = {}
#     uC.PERIPH["STM32F479IIxx"] = {}
#     uC.PERIPH["STM32F479BGxx"] = {}
#     uC.PERIPH["STM32F479BIxx"] = {}
#     uC.PERIPH["STM32F479NGxx"] = {}
#     uC.PERIPH["STM32F479NIxx"] = {}
# end
#++*/

#/* include of CPU mandatory file in Makefile
include ./config/arch/$(__CPU_ARCH__)/cpu_flags.h
#*/

#define ARCH_noarch
#include "noarch/loop_flags.h"
#include "noarch/tty_flags.h"
#include "noarch/sdspi_flags.h"
#include "noarch/dht11_flags.h"
#include "noarch/i2cee_flags.h"

#if (__CPU_ARCH__ == stm32f1)
#include "stm32f1/cpu_flags.h"
#include "stm32f1/rtc_flags.h"
#include "stm32f1/crc_flags.h"
#include "stm32f1/gpio_flags.h"
#include "stm32f1/afm_flags.h"
#include "stm32f1/ethmac_flags.h"
#include "stm32f1/clk_flags.h"
#include "stm32f1/spi_flags.h"
#include "stm32f1/wdg_flags.h"
#include "stm32f1/uart_flags.h"
#include "stm32f1/usbd_flags.h"
#include "stm32f1/irq_flags.h"
#include "stm32f1/i2c_flags.h"
#endif

#if (__CPU_ARCH__ == stm32f4)
#include "stm32f4/cpu_flags.h"
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
#define __ENABLE_CLK__ _NO_
#/*
__ENABLE_CLK__=_NO_
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
# if uC.PERIPH[uC.NAME].ETHMAC ~= nil then
#     this:PutWidgets("ETHMAC", "arch/"..uC.ARCH.."/ethmac_flags.h")
# else
#     this:AddWidget("Value")
#     this:SetFlagValue("__ENABLE_ETHMAC__", "_NO_")
# end
#--*/
#define __ENABLE_ETHMAC__ _NO_
#/*
__ENABLE_ETHMAC__=_NO_
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
# if uC.PERIPH[uC.NAME].PWM ~= nil then
#     this:PutWidgets("PWM")
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

#// MODULE LIST END
#//-----------------------------------------------------------------------------
#/*-- save current configuration if CPU was changed
# this:Save()
#++*/

#endif /* _ARCH_FLAGS_H_ */
#/*=============================================================================
#  End of file
#=============================================================================*/
