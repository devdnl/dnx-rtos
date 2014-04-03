--[[============================================================================
@file    cpu.lua

@author  Daniel Zorychta

@brief   CPU configuration file.

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


*//*========================================================================--]]

require "defs"

--------------------------------------------------------------------------------
-- GLOBAL OBJECTS
--------------------------------------------------------------------------------
-- public calls objects
cpu = {}

-- private class objects
local arch                   = ""

local uc                     = {}
uc.supported_arch            = {"stm32f1"} --, "stm32f2", "stm32f3", "stm32f4"}
uc.modules_list              = {"GPIO", "CRC", "ETH", "PLL", "SDSPI", "SPI", "TTY", "UART", "WDG", "I2S", "USB", "USBOTG", "I2C", "ADC", "DAC", "SDIO", "FSMC", "HDMICEC"}
uc.module_description        = {}
uc.module_description.GPIO   = "GPIO - General Purpose Input Output"
uc.module_description.CRC    = "CRC - CRC caluclation unit"
uc.module_description.ETH    = "ETH - Ethernet interface"
uc.module_description.PLL    = "PLL - Phase Lock Loop"
uc.module_description.SDSPI  = "SDSPI - communication with SD Card by SPI"
uc.module_description.SPI    = "SPI - Serial Pheripheral Interface"
uc.module_description.TTY    = "TTY - virtual terminal driver"
uc.module_description.UART   = "UART - Universal Asynchronous Receiver Transmitter"
uc.module_description.WDG    = "WDG - Watchdog"
uc.module_description.I2S    = "I2S - Integrated Interchip Sound"
uc.module_description.USB    = "USB - Universal Serial Bus (device mode)"
uc.module_description.USBOTG = "USB-OTG - Universal Serial Bus On-The-Go"
uc.module_description.I2C    = "I2C - Inter-Integrated Circuit"
uc.module_description.ADC    = "ADC - Analog To Digital Converter"
uc.module_description.DAC    = "DAC - Digital To Analog Converter"
uc.module_description.SDIO   = "SDIO - Secure Digital Card Input/Output"
uc.module_description.FSMC   = "FSMC - Flexible Static Memory Controller"
uc.module_description.HDMICEC= "HDMI-CEC - High Definition Media Interface - Customer Electronic Control"
uc.stm32f1                   = {}
uc.stm32f1.description       = "stm32f1 microcontroller family"
uc.stm32f1.config_steps      = 4
uc.stm32f1.mcu_list          = {"STM32F100C4xx", "STM32F100C6xx", "STM32F100C8xx", "STM32F100R6xx", "STM32F100RBxx", "STM32F100RExx",
                                "STM32F100VBxx", "STM32F100ZCxx", "STM32F101C6xx", "STM32F101C8xx", "STM32F101CBxx", "STM32F101R6xx",
                                "STM32F101R8xx", "STM32F101RBxx", "STM32F101V8xx", "STM32F101VBxx", "STM32F103C6xx", "STM32F103C8xx",
                                "STM32F103CBxx", "STM32F103R4xx", "STM32F103R6xx", "STM32F103R8xx", "STM32F103RBxx", "STM32F103RCxx",
                                "STM32F103RDxx", "STM32F103RExx", "STM32F103RGxx", "STM32F103T8xx", "STM32F103V8xx", "STM32F103VBxx",
                                "STM32F103VCxx", "STM32F103VDxx", "STM32F103VExx", "STM32F103VGxx", "STM32F103ZCxx", "STM32F103ZExx",
                                "STM32F103ZGxx", "STM32F105RBxx", "STM32F105RCxx", "STM32F105VBxx", "STM32F105VCxx", "STM32F107RCxx",
                                "STM32F107VBxx", "STM32F107VCxx"}
uc.stm32f1.mcu               = {}
uc.stm32f1.mcu.STM32F100C4xx = {["RAM"] = 4096 , ["family"] = "STM32F10X_LD_VL", ["modules"] = {"GPIO", "PLL", "SDSPI", "SPI", "TTY", "UART", "WDG", "I2S", "I2C", "ADC", "DAC", "HDMICEC"}}
uc.stm32f1.mcu.STM32F100C6xx = {["RAM"] = 4096 , ["family"] = "STM32F10X_LD_VL", ["modules"] = {}}
uc.stm32f1.mcu.STM32F100C8xx = {["RAM"] = 8192 , ["family"] = "STM32F10X_MD_VL", ["modules"] = {}}
uc.stm32f1.mcu.STM32F100R6xx = {["RAM"] = 4096 , ["family"] = "STM32F10X_LD_VL", ["modules"] = {}}
uc.stm32f1.mcu.STM32F100RBxx = {["RAM"] = 8192 , ["family"] = "STM32F10X_MD_VL", ["modules"] = {}}
uc.stm32f1.mcu.STM32F100RExx = {["RAM"] = 32768, ["family"] = "STM32F10X_HD_VL", ["modules"] = {}}
uc.stm32f1.mcu.STM32F100VBxx = {["RAM"] = 8192 , ["family"] = "STM32F10X_MD_VL", ["modules"] = {}}
uc.stm32f1.mcu.STM32F100ZCxx = {["RAM"] = 24576, ["family"] = "STM32F10X_HD_VL", ["modules"] = {}}
uc.stm32f1.mcu.STM32F101C6xx = {["RAM"] = 6144 , ["family"] = "STM32F10X_LD"   , ["modules"] = {}}
uc.stm32f1.mcu.STM32F101C8xx = {["RAM"] = 10240, ["family"] = "STM32F10X_MD"   , ["modules"] = {}}
uc.stm32f1.mcu.STM32F101CBxx = {["RAM"] = 16384, ["family"] = "STM32F10X_MD"   , ["modules"] = {}}
uc.stm32f1.mcu.STM32F101R6xx = {["RAM"] = 6144 , ["family"] = "STM32F10X_LD"   , ["modules"] = {}}
uc.stm32f1.mcu.STM32F101R8xx = {["RAM"] = 10240, ["family"] = "STM32F10X_MD"   , ["modules"] = {}}
uc.stm32f1.mcu.STM32F101RBxx = {["RAM"] = 16384, ["family"] = "STM32F10X_MD"   , ["modules"] = {}}
uc.stm32f1.mcu.STM32F101V8xx = {["RAM"] = 10240, ["family"] = "STM32F10X_MD"   , ["modules"] = {}}
uc.stm32f1.mcu.STM32F101VBxx = {["RAM"] = 16384, ["family"] = "STM32F10X_MD"   , ["modules"] = {}}
uc.stm32f1.mcu.STM32F103C6xx = {["RAM"] = 10240, ["family"] = "STM32F10X_LD"   , ["modules"] = {}}
uc.stm32f1.mcu.STM32F103C8xx = {["RAM"] = 20480, ["family"] = "STM32F10X_MD"   , ["modules"] = {}}
uc.stm32f1.mcu.STM32F103CBxx = {["RAM"] = 20480, ["family"] = "STM32F10X_MD"   , ["modules"] = {}}
uc.stm32f1.mcu.STM32F103R4xx = {["RAM"] = 6144 , ["family"] = "STM32F10X_LD"   , ["modules"] = {}}
uc.stm32f1.mcu.STM32F103R6xx = {["RAM"] = 10240, ["family"] = "STM32F10X_LD"   , ["modules"] = {}}
uc.stm32f1.mcu.STM32F103R8xx = {["RAM"] = 20480, ["family"] = "STM32F10X_MD"   , ["modules"] = {}}
uc.stm32f1.mcu.STM32F103RBxx = {["RAM"] = 20480, ["family"] = "STM32F10X_MD"   , ["modules"] = {}}
uc.stm32f1.mcu.STM32F103RCxx = {["RAM"] = 49152, ["family"] = "STM32F10X_HD"   , ["modules"] = {}}
uc.stm32f1.mcu.STM32F103RDxx = {["RAM"] = 65536, ["family"] = "STM32F10X_HD"   , ["modules"] = {}}
uc.stm32f1.mcu.STM32F103RExx = {["RAM"] = 65536, ["family"] = "STM32F10X_HD"   , ["modules"] = {}}
uc.stm32f1.mcu.STM32F103RGxx = {["RAM"] = 98304, ["family"] = "STM32F10X_XL"   , ["modules"] = {}}
uc.stm32f1.mcu.STM32F103T8xx = {["RAM"] = 20480, ["family"] = "STM32F10X_MD"   , ["modules"] = {}}
uc.stm32f1.mcu.STM32F103V8xx = {["RAM"] = 20480, ["family"] = "STM32F10X_MD"   , ["modules"] = {}}
uc.stm32f1.mcu.STM32F103VBxx = {["RAM"] = 20480, ["family"] = "STM32F10X_MD"   , ["modules"] = {}}
uc.stm32f1.mcu.STM32F103VCxx = {["RAM"] = 49152, ["family"] = "STM32F10X_HD"   , ["modules"] = {}}
uc.stm32f1.mcu.STM32F103VDxx = {["RAM"] = 65536, ["family"] = "STM32F10X_HD"   , ["modules"] = {}}
uc.stm32f1.mcu.STM32F103VExx = {["RAM"] = 65536, ["family"] = "STM32F10X_HD"   , ["modules"] = {}}
uc.stm32f1.mcu.STM32F103VGxx = {["RAM"] = 98304, ["family"] = "STM32F10X_XL"   , ["modules"] = {}}
uc.stm32f1.mcu.STM32F103ZCxx = {["RAM"] = 49152, ["family"] = "STM32F10X_HD"   , ["modules"] = {}}
uc.stm32f1.mcu.STM32F103ZExx = {["RAM"] = 65536, ["family"] = "STM32F10X_HD"   , ["modules"] = {}}
uc.stm32f1.mcu.STM32F103ZGxx = {["RAM"] = 98304, ["family"] = "STM32F10X_XL"   , ["modules"] = {}}
uc.stm32f1.mcu.STM32F105RBxx = {["RAM"] = 32768, ["family"] = "STM32F10X_CL"   , ["modules"] = {}}
uc.stm32f1.mcu.STM32F105RCxx = {["RAM"] = 65536, ["family"] = "STM32F10X_CL"   , ["modules"] = {}}
uc.stm32f1.mcu.STM32F105VBxx = {["RAM"] = 32768, ["family"] = "STM32F10X_CL"   , ["modules"] = {}}
uc.stm32f1.mcu.STM32F105VCxx = {["RAM"] = 65536, ["family"] = "STM32F10X_CL"   , ["modules"] = {}}
uc.stm32f1.mcu.STM32F107RCxx = {["RAM"] = 65536, ["family"] = "STM32F10X_CL"   , ["modules"] = {"GPIO", "PLL"}}
uc.stm32f1.mcu.STM32F107VBxx = {["RAM"] = 49152, ["family"] = "STM32F10X_CL"   , ["modules"] = {}}
uc.stm32f1.mcu.STM32F107VCxx = {["RAM"] = 65536, ["family"] = "STM32F10X_CL"   , ["modules"] = {}}

uc.stm32f2                   = {}
uc.stm32f2.description       = "stm32f2 microcontroller family"
uc.stm32f2.config_steps      = 4
uc.stm32f2.mcu_list          = {}
uc.stm32f2.mcu               = {}

uc.stm32f3                   = {}
uc.stm32f3.description       = "stm32f3 microcontroller family"
uc.stm32f3.config_steps      = 4
uc.stm32f3.mcu_list          = {}
uc.stm32f3.mcu               = {}

uc.stm32f4                   = {}
uc.stm32f4.description       = "stm32f4 microcontroller family"
uc.stm32f4.config_steps      = 4
uc.stm32f4.mcu_list          = {}
uc.stm32f4.mcu               = {}

--------------------------------------------------------------------------------
-- LOCAL FUNCTIONS
--------------------------------------------------------------------------------
--------------------------------------------------------------------------------
-- @brief Function calculate a total configuration steps
--------------------------------------------------------------------------------
local function calculate_total_steps()
        cpu:get_arch()
        set_total_steps(uc[arch].config_steps)
end

--------------------------------------------------------------------------------
-- @brief Function configure CPU architecture
--------------------------------------------------------------------------------
local function configure_cpu_arch()
        msg(progress() .. "CPU architecture configuration.")
        local choice = key_read("../project/Makefile", "PROJECT_CPU_ARCH")
        msg("Current CPU architecture is: " .. choice .. ".")
        for i in pairs(uc.supported_arch) do
                local arch = uc.supported_arch[i]
                add_item(arch, uc[arch].description)
        end
        choice = get_selection()
        if (can_be_saved(choice)) then
                cpu:set_arch(choice)
        end

        return choice
end

--------------------------------------------------------------------------------
-- @brief Configure CPU frequency
--------------------------------------------------------------------------------
local function configure_freq()
        msg(progress() .. "Configuration of oscillator frequency (generator, crystal, etc).")
        local value = key_read("../project/flags.h", "__CPU_OSC_FREQ__")
        msg("Current value is: " .. value .. " Hz.")
        value = get_number("dec", 0, 100e6)
        if (can_be_saved(value)) then
                key_save("../project/flags.h", "__CPU_OSC_FREQ__", value)
        end

        return value
end

--------------------------------------------------------------------------------
-- @brief Configure microcontroller name
--------------------------------------------------------------------------------
local function configure_mcu_name()
        local file   = "../"..arch.."/cpu.h"
        local choice = key_read(file, "__CPU_NAME__")

        msg(progress() .. "Which microcontroller do you want to use?")
        msg("Current microcontroller is: " .. choice .. ".")
        for i in pairs(uc[arch].mcu_list) do
                add_item(uc[arch].mcu_list[i], uc[arch].mcu_list[i])
        end
        choice = get_selection()
        if (can_be_saved(choice)) then
                key_save(file, "__CPU_NAME__", choice)
                key_save(file, "__CPU_FAMILY__", "__" .. uc[arch].mcu[choice].family .. "__")
                key_save(file, "__CPU_RAM_MEM_SIZE__", uc[arch].mcu[choice].RAM)

                -- disable not supported modules
                for i, m in pairs(uc.modules_list) do
                        if not cpu:is_module_supported(m) then
                                key_save("../project/Makefile", "ENABLE_"..m, no)
                                key_save("../project/flags.h", "__ENABLE_"..m.."__", no)
                        end
                end
        end

        return choice
end

--------------------------------------------------------------------------------
-- @brief Configure IRQ priorities
--------------------------------------------------------------------------------
local function configure_priorities()
        local choice = key_read("../project/flags.h", "__IRQ_USER_PRIORITY__")
        msg(progress() .. "Select default priority for user's interrputs.")

        if (arch == "stm32f1") then
                msg("Current priority is: priority " .. math.floor(tonumber(choice) / 16))
                add_item("0x0F", "Priority 0 (the highest)")
                add_item("0x1F", "Priority 1")
                add_item("0x2F", "Priority 2")
                add_item("0x3F", "Priority 3")
                add_item("0x4F", "Priority 4")
                add_item("0x5F", "Priority 5")
                add_item("0x6F", "Priority 6")
                add_item("0x7F", "Priority 7")
                add_item("0x8F", "Priority 8")
                add_item("0x9F", "Priority 9")
                add_item("0xAF", "Priority 10")
                add_item("0xBF", "Priority 11")
                add_item("0xCF", "Priority 12")
                add_item("0xDF", "Priority 13 (the lowest)")
        end

        choice = get_selection()
        if (can_be_saved(choice)) then
                local kernel_priority
                local syscall_priority

                if (arch == "stm32f1") then
                        kernel_priority  = "0xFF"
                        syscall_priority = "0xEF"
                end

                key_save("../project/flags.h", "__IRQ_RTOS_KERNEL_PRIORITY__", kernel_priority)
                key_save("../project/flags.h", "__IRQ_RTOS_SYSCALL_PRIORITY__", syscall_priority)
                key_save("../project/flags.h", "__IRQ_USER_PRIORITY__", choice)
        end

        return choice
end

--------------------------------------------------------------------------------
-- GLOBAL FUNCTIONS
--------------------------------------------------------------------------------
--------------------------------------------------------------------------------
-- @brief Function read current architecture from file
-- Function set architecture variable directly, and also return it.
-- @return current architecture
--------------------------------------------------------------------------------
function cpu:get_arch()
        arch = key_read("../project/Makefile", "PROJECT_CPU_ARCH")
        return arch
end

--------------------------------------------------------------------------------
-- @brief Function set CPU architecture
-- Function set architecture variable directly, and also return it.
-- @return current architecture
--------------------------------------------------------------------------------
function cpu:set_arch(val)
        key_save("../project/Makefile", "PROJECT_CPU_ARCH", val)
        key_save("../project/flags.h", "__CPU_ARCH__", val)
        arch = val
end

--------------------------------------------------------------------------------
-- @brief Function return MCU name
-- @return current CPU name
--------------------------------------------------------------------------------
function cpu:get_name()
        return key_read("../" .. cpu:get_arch() .. "/cpu.h", "__CPU_NAME__")
end

--------------------------------------------------------------------------------
-- @brief Function return list of modules to configuration
-- @param name          name of microcontroller
-- @return module list
--------------------------------------------------------------------------------
function cpu:get_modules()
        return uc[cpu:get_arch()].mcu[cpu:get_name()].modules
end

--------------------------------------------------------------------------------
-- @brief Function check if module is supported by selected MCU
-- @return true if modules is supported, otherwise false
--------------------------------------------------------------------------------
function cpu:is_module_supported(module_name)
        for i, m in pairs(uc[cpu:get_arch()].mcu[cpu:get_name()].modules) do
                if m == module_name then
                        return true
                end
        end

        return false
end

--------------------------------------------------------------------------------
-- @brief Function return module description
-- @param module_name   module name
-- @return module description
--------------------------------------------------------------------------------
function cpu:get_module_description(module_name)
        return uc.module_description[module_name]
end

--------------------------------------------------------------------------------
-- @brief Function return RAM size of selected CPU
-- @return RAM size
--------------------------------------------------------------------------------
function cpu:get_RAM_size()
        return uc[cpu:get_arch()].mcu[cpu:get_name()].RAM
end

--------------------------------------------------------------------------------
-- @brief Function execute configuration
--------------------------------------------------------------------------------
function cpu:configure()
        calculate_total_steps()

        title("CPU configuration (" .. cpu:get_arch() .. ")")

        ::common_1::
        if configure_cpu_arch() == back then
                return back
        else
                calculate_total_steps()
                title("CPU configuration (" .. cpu:get_arch() .. ")")
        end

        ::common_2::
        if configure_freq() == back then
                goto common_1
        end

        ::common_3::
        if configure_mcu_name() == back then
                goto common_2
        end

        ::common_4::
        if configure_priorities() == back then
                goto common_3
        end

        return next
end

-- started without master file
if (master ~= true) then
        show_no_master_info()
end

--------------------------------------------------------------------------------
-- END OF FILE
--------------------------------------------------------------------------------
