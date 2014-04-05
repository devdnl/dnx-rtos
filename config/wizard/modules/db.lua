--[[============================================================================
@file    db.lua

@author  Daniel Zorychta

@brief   Wizard database.

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

--==============================================================================
-- GLOBAL OBJECTS
--------------------------------------------------------------------------------
db = {}

local module                            = {}
module.list                             = {"GPIO", "CRC", "ETH", "PLL", "SDSPI", "SPI", "TTY", "UART", "WDG", "I2S", "USB", "USBOTG", "I2C", "ADC", "DAC", "SDIO", "FSMC", "HDMICEC"}
module.description                      = {}
module.description.GPIO                 = "GPIO - General Purpose Input Output"
module.description.CRC                  = "CRC - CRC caluclation unit"
module.description.ETH                  = "ETH - Ethernet interface"
module.description.PLL                  = "PLL - Phase Lock Loop"
module.description.SDSPI                = "SDSPI - communication with SD Card by SPI"
module.description.SPI                  = "SPI - Serial Pheripheral Interface"
module.description.TTY                  = "TTY - virtual terminal driver"
module.description.UART                 = "UART - Universal Asynchronous Receiver Transmitter"
module.description.WDG                  = "WDG - Watchdog"
module.description.I2S                  = "I2S - Integrated Interchip Sound"
module.description.USB                  = "USB - Universal Serial Bus (device mode)"
module.description.USBOTG               = "USB-OTG - Universal Serial Bus On-The-Go"
module.description.I2C                  = "I2C - Inter-Integrated Circuit"
module.description.ADC                  = "ADC - Analog To Digital Converter"
module.description.DAC                  = "DAC - Digital To Analog Converter"
module.description.SDIO                 = "SDIO - Secure Digital Card Input/Output"
module.description.FSMC                 = "FSMC - Flexible Static Memory Controller"
module.description.HDMICEC              = "HDMI-CEC - High Definition Media Interface - Customer Electronic Control"

local arch                              = {}
arch.list                               = {"stm32f1"} --, "stm32f2", "stm32f3", "stm32f4"}

-- STM32F1 ---------------------------------------------------------------------
arch.stm32f1                            = {}
arch.stm32f1.description                = "stm32f1 microcontroller family"
arch.stm32f1.mcu                        = {}
arch.stm32f1.mcu.list                   = {"STM32F100C4xx", "STM32F100C6xx", "STM32F100C8xx", "STM32F100R6xx", "STM32F100RBxx", "STM32F100RExx",
                                           "STM32F100VBxx", "STM32F100ZCxx", "STM32F101C6xx", "STM32F101C8xx", "STM32F101CBxx", "STM32F101R6xx",
                                           "STM32F101R8xx", "STM32F101RBxx", "STM32F101V8xx", "STM32F101VBxx", "STM32F103C6xx", "STM32F103C8xx",
                                           "STM32F103CBxx", "STM32F103R4xx", "STM32F103R6xx", "STM32F103R8xx", "STM32F103RBxx", "STM32F103RCxx",
                                           "STM32F103RDxx", "STM32F103RExx", "STM32F103RGxx", "STM32F103T8xx", "STM32F103V8xx", "STM32F103VBxx",
                                           "STM32F103VCxx", "STM32F103VDxx", "STM32F103VExx", "STM32F103VGxx", "STM32F103ZCxx", "STM32F103ZExx",
                                           "STM32F103ZGxx", "STM32F105RBxx", "STM32F105RCxx", "STM32F105VBxx", "STM32F105VCxx", "STM32F107RCxx",
                                           "STM32F107VBxx", "STM32F107VCxx"}

arch.stm32f1.mcu.STM32F100C4xx          = {}
arch.stm32f1.mcu.STM32F100C4xx.ram      = 4096
arch.stm32f1.mcu.STM32F100C4xx.family   = "STM32F10X_LD_VL"
arch.stm32f1.mcu.STM32F100C4xx.modules  = {"GPIO", "PLL", "SDSPI", "SPI", "TTY", "UART", "WDG", "I2S", "I2C", "ADC", "DAC", "HDMICEC"}
arch.stm32f1.mcu.STM32F100C4xx.GPIO     = {4}
arch.stm32f1.mcu.STM32F100C4xx.SPI      = {1}
arch.stm32f1.mcu.STM32F100C4xx.UART     = {2}

arch.stm32f1.mcu.STM32F100C6xx          = {}
arch.stm32f1.mcu.STM32F100C6xx.ram      = 4096
arch.stm32f1.mcu.STM32F100C6xx.family   = "STM32F10X_LD_VL"
arch.stm32f1.mcu.STM32F100C6xx.modules  = {"GPIO", "PLL", "SDSPI", "SPI", "TTY", "UART", "WDG", "I2S", "I2C", "ADC", "DAC", "HDMICEC"}
arch.stm32f1.mcu.STM32F100C6xx.GPIO     = {4}
arch.stm32f1.mcu.STM32F100C6xx.SPI      = {1}
arch.stm32f1.mcu.STM32F100C6xx.UART     = {2}

arch.stm32f1.mcu.STM32F100C8xx          = {}
arch.stm32f1.mcu.STM32F100C8xx.ram      = 8192
arch.stm32f1.mcu.STM32F100C8xx.family   = "STM32F10X_MD_VL"
arch.stm32f1.mcu.STM32F100C8xx.modules  = {"GPIO", "PLL", "SDSPI", "SPI", "TTY", "UART", "WDG", "I2S", "I2C", "ADC", "DAC", "HDMICEC"}
arch.stm32f1.mcu.STM32F100C6xx.GPIO     = {4}
arch.stm32f1.mcu.STM32F100C6xx.SPI      = {1}
arch.stm32f1.mcu.STM32F100C6xx.UART     = {2}


-- uc.stm32f1.mcu.STM32F100R6xx    = {["RAM"] = 4096 , ["family"] = "STM32F10X_LD_VL", ["modules"] = {}}
-- uc.stm32f1.mcu.STM32F100RBxx    = {["RAM"] = 8192 , ["family"] = "STM32F10X_MD_VL", ["modules"] = {}}
-- uc.stm32f1.mcu.STM32F100RExx    = {["RAM"] = 32768, ["family"] = "STM32F10X_HD_VL", ["modules"] = {}}
-- uc.stm32f1.mcu.STM32F100VBxx    = {["RAM"] = 8192 , ["family"] = "STM32F10X_MD_VL", ["modules"] = {}}
-- uc.stm32f1.mcu.STM32F100ZCxx    = {["RAM"] = 24576, ["family"] = "STM32F10X_HD_VL", ["modules"] = {}}
-- uc.stm32f1.mcu.STM32F101C6xx    = {["RAM"] = 6144 , ["family"] = "STM32F10X_LD"   , ["modules"] = {}}
-- uc.stm32f1.mcu.STM32F101C8xx    = {["RAM"] = 10240, ["family"] = "STM32F10X_MD"   , ["modules"] = {}}
-- uc.stm32f1.mcu.STM32F101CBxx    = {["RAM"] = 16384, ["family"] = "STM32F10X_MD"   , ["modules"] = {}}
-- uc.stm32f1.mcu.STM32F101R6xx    = {["RAM"] = 6144 , ["family"] = "STM32F10X_LD"   , ["modules"] = {}}
-- uc.stm32f1.mcu.STM32F101R8xx    = {["RAM"] = 10240, ["family"] = "STM32F10X_MD"   , ["modules"] = {}}
-- uc.stm32f1.mcu.STM32F101RBxx    = {["RAM"] = 16384, ["family"] = "STM32F10X_MD"   , ["modules"] = {}}
-- uc.stm32f1.mcu.STM32F101V8xx    = {["RAM"] = 10240, ["family"] = "STM32F10X_MD"   , ["modules"] = {}}
-- uc.stm32f1.mcu.STM32F101VBxx    = {["RAM"] = 16384, ["family"] = "STM32F10X_MD"   , ["modules"] = {}}
-- uc.stm32f1.mcu.STM32F103C6xx    = {["RAM"] = 10240, ["family"] = "STM32F10X_LD"   , ["modules"] = {}}
-- uc.stm32f1.mcu.STM32F103C8xx    = {["RAM"] = 20480, ["family"] = "STM32F10X_MD"   , ["modules"] = {}}
-- uc.stm32f1.mcu.STM32F103CBxx    = {["RAM"] = 20480, ["family"] = "STM32F10X_MD"   , ["modules"] = {}}
-- uc.stm32f1.mcu.STM32F103R4xx    = {["RAM"] = 6144 , ["family"] = "STM32F10X_LD"   , ["modules"] = {}}
-- uc.stm32f1.mcu.STM32F103R6xx    = {["RAM"] = 10240, ["family"] = "STM32F10X_LD"   , ["modules"] = {}}
-- uc.stm32f1.mcu.STM32F103R8xx    = {["RAM"] = 20480, ["family"] = "STM32F10X_MD"   , ["modules"] = {}}
-- uc.stm32f1.mcu.STM32F103RBxx    = {["RAM"] = 20480, ["family"] = "STM32F10X_MD"   , ["modules"] = {}}
-- uc.stm32f1.mcu.STM32F103RCxx    = {["RAM"] = 49152, ["family"] = "STM32F10X_HD"   , ["modules"] = {}}
-- uc.stm32f1.mcu.STM32F103RDxx    = {["RAM"] = 65536, ["family"] = "STM32F10X_HD"   , ["modules"] = {}}
-- uc.stm32f1.mcu.STM32F103RExx    = {["RAM"] = 65536, ["family"] = "STM32F10X_HD"   , ["modules"] = {}}
-- uc.stm32f1.mcu.STM32F103RGxx    = {["RAM"] = 98304, ["family"] = "STM32F10X_XL"   , ["modules"] = {}}
-- uc.stm32f1.mcu.STM32F103T8xx    = {["RAM"] = 20480, ["family"] = "STM32F10X_MD"   , ["modules"] = {}}
-- uc.stm32f1.mcu.STM32F103V8xx    = {["RAM"] = 20480, ["family"] = "STM32F10X_MD"   , ["modules"] = {}}
-- uc.stm32f1.mcu.STM32F103VBxx    = {["RAM"] = 20480, ["family"] = "STM32F10X_MD"   , ["modules"] = {}}
-- uc.stm32f1.mcu.STM32F103VCxx    = {["RAM"] = 49152, ["family"] = "STM32F10X_HD"   , ["modules"] = {}}
-- uc.stm32f1.mcu.STM32F103VDxx    = {["RAM"] = 65536, ["family"] = "STM32F10X_HD"   , ["modules"] = {}}
-- uc.stm32f1.mcu.STM32F103VExx    = {["RAM"] = 65536, ["family"] = "STM32F10X_HD"   , ["modules"] = {}}
-- uc.stm32f1.mcu.STM32F103VGxx    = {["RAM"] = 98304, ["family"] = "STM32F10X_XL"   , ["modules"] = {}}
-- uc.stm32f1.mcu.STM32F103ZCxx    = {["RAM"] = 49152, ["family"] = "STM32F10X_HD"   , ["modules"] = {}}
-- uc.stm32f1.mcu.STM32F103ZExx    = {["RAM"] = 65536, ["family"] = "STM32F10X_HD"   , ["modules"] = {}}
-- uc.stm32f1.mcu.STM32F103ZGxx    = {["RAM"] = 98304, ["family"] = "STM32F10X_XL"   , ["modules"] = {}}
-- uc.stm32f1.mcu.STM32F105RBxx    = {["RAM"] = 32768, ["family"] = "STM32F10X_CL"   , ["modules"] = {}}
-- uc.stm32f1.mcu.STM32F105RCxx    = {["RAM"] = 65536, ["family"] = "STM32F10X_CL"   , ["modules"] = {}}
-- uc.stm32f1.mcu.STM32F105VBxx    = {["RAM"] = 32768, ["family"] = "STM32F10X_CL"   , ["modules"] = {}}
-- uc.stm32f1.mcu.STM32F105VCxx    = {["RAM"] = 65536, ["family"] = "STM32F10X_CL"   , ["modules"] = {}}
-- uc.stm32f1.mcu.STM32F107RCxx    = {["RAM"] = 65536, ["family"] = "STM32F10X_CL"   , ["modules"] = {"GPIO", "PLL"}}
-- uc.stm32f1.mcu.STM32F107VBxx    = {["RAM"] = 49152, ["family"] = "STM32F10X_CL"   , ["modules"] = {}}
-- uc.stm32f1.mcu.STM32F107VCxx    = {["RAM"] = 65536, ["family"] = "STM32F10X_CL"   , ["modules"] = {}}

-- STM32F2 ---------------------------------------------------------------------
arch.stm32f2                            = {}
arch.stm32f2.description                = "stm32f2 microcontroller family"
arch.stm32f2.mcu                        = {}
arch.stm32f2.mcu.list                   = {}

-- STM32F3 ---------------------------------------------------------------------
arch.stm32f3                            = {}
arch.stm32f3.description                = "stm32f3 microcontroller family"
arch.stm32f3.mcu                        = {}
arch.stm32f3.mcu.list                   = {}

-- STM32F4 ---------------------------------------------------------------------
arch.stm32f4                            = {}
arch.stm32f4.description                = "stm32f4 microcontroller family"
arch.stm32f4.mcu                        = {}
arch.stm32f4.mcu.list                   = {}

--------------------------------------------------------------------------------
-- LOCAL FUNCTIONS
--------------------------------------------------------------------------------
--------------------------------------------------------------------------------
-- @brief Function find architecture for selected MCU
-- @param mcu_name      MCU name
-- @return On success architecture string is returned, on error nil
--------------------------------------------------------------------------------
local function get_mcu_arch(mcu_name)
        for i = 1, #arch.list do
                local arch_name = arch.list[i]
                if arch[arch_name].mcu[mcu_name] ~= nil then
                        return arch_name
                end
        end

        return nil
end

--------------------------------------------------------------------------------
-- METHODS
--------------------------------------------------------------------------------
--------------------------------------------------------------------------------
-- @brief Method returns modules list
-- @param None
-- @return Modules list
--------------------------------------------------------------------------------
function db:get_modules_list()
        return module.list
end

--------------------------------------------------------------------------------
-- @brief Method return module description
-- @param module_name           module name to get description
-- @return Module description.
--------------------------------------------------------------------------------
function db:get_module_description(module_name)
        return module.description[module_name]
end

--------------------------------------------------------------------------------
-- @brief Method return list of architectures
-- @param None
-- @return Architecture list
--------------------------------------------------------------------------------
function db:get_arch_list()
        return arch.list
end

--------------------------------------------------------------------------------
-- @brief Method return architecture description
-- @param arch_name     architecture name
-- @return Architecture description
--------------------------------------------------------------------------------
function db:get_arch_description(arch_name)
        return arch[arch_name].description
end

--------------------------------------------------------------------------------
-- @brief Method returns MCU list for selected architecture
-- @param arch_name     architecture name
-- @return MCU list
--------------------------------------------------------------------------------
function db:get_mcu_list(arch_name)
        return arch[arch_name].mcu.list
end

--------------------------------------------------------------------------------
-- @brief Method return RAM size for selected MCU
-- @param mcu_name      MCU name
-- @return RAM size
--------------------------------------------------------------------------------
function db:get_mcu_ram_size(mcu_name)
        return arch[get_mcu_arch(mcu_name)].mcu[mcu_name].ram
end

--------------------------------------------------------------------------------
-- @brief Method return family of selected MCU
-- @param mcu_name      MCU name
-- @return
--------------------------------------------------------------------------------
function db:get_mcu_family(mcu_name)
        return arch[get_mcu_arch(mcu_name)].mcu[mcu_name].family
end

--------------------------------------------------------------------------------
-- @brief Method return list of supported modules by selected MCU
-- @param mcu_name      MCU name
-- @return List of supported modules
--------------------------------------------------------------------------------
function db:get_mcu_modules_list(mcu_name)
        return arch[get_mcu_arch(mcu_name)].mcu[mcu_name].modules
end

--------------------------------------------------------------------------------
-- @brief Method returns data of selected MCU's module
-- @param mcu_name      MCU name
-- @param module_name   module name
-- @return List of module data
--------------------------------------------------------------------------------
function db:get_mcu_module_data(mcu_name, module_name)
        return arch[get_mcu_arch(mcu_name)].mcu[mcu_name][module_name]
end

--------------------------------------------------------------------------------
-- @brief Method check if selected MCU's module is supported
-- @param mcu_name      MCU name
-- @param module_name   module name
-- @return true if module is supported, otherwise false
--------------------------------------------------------------------------------
function db:is_module_supported(mcu_name, module_name)
        for i, m in pairs(arch[get_mcu_arch(mcu_name)].mcu[mcu_name].modules) do
                if m == module_name then
                        return true
                end
        end

        return false
end

-- started without master file
if (master ~= true) then
        show_no_master_info()
end
