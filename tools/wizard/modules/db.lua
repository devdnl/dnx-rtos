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
db.path                                 = {}
db.path.cwd                             = "../../config/"
db.path.project                         = {}
db.path.project.mk                      = db.path.cwd.."project/Makefile"
db.path.project.flags                   = db.path.cwd.."project/flags.h"
db.path.stm32f1                         = {}
db.path.stm32f1.cpu                     = {}
db.path.stm32f1.cpu.flags               = db.path.cwd.."stm32f1/cpu.h"
db.path.stm32f1.cpu.mk                  = db.path.cwd.."stm32f1/Makefile"
db.path.stm32f1.gpio                    = {}
db.path.stm32f1.gpio.flags              = db.path.cwd.."stm32f1/gpio_flags.h"
db.path.stm32f1.afio                    = {}
db.path.stm32f1.afio.flags              = db.path.cwd.."stm32f1/afio_flags.h"
db.path.stm32f1.pll                     = {}
db.path.stm32f1.pll.flags               = db.path.cwd.."stm32f1/pll_flags.h"
db.path.stm32f1.eth                     = {}
db.path.stm32f1.eth.flags               = db.path.cwd.."stm32f1/eth_flags.h"
db.path.stm32f1.sdspi                   = {}
db.path.stm32f1.sdspi.flags             = db.path.cwd.."stm32f1/sdspi_flags.h"
db.path.stm32f1.spi                     = {}
db.path.stm32f1.spi.flags               = db.path.cwd.."stm32f1/spi_flags.h"
db.path.stm32f1.uart                    = {}
db.path.stm32f1.uart.flags              = db.path.cwd.."stm32f1/uart_flags.h"
db.path.stm32f1.wdg                     = {}
db.path.stm32f1.wdg.flags               = db.path.cwd.."stm32f1/wdg_flags.h"
db.path.stm32f1.usb                     = {}
db.path.stm32f1.usb.flags               = db.path.cwd.."stm32f1/usb_flags.h"
db.path.noarch                          = {}
db.path.noarch.tty                      = {}
db.path.noarch.tty.flags                = db.path.cwd.."noarch/tty_flags.h"

local module                            = {}
module.list                             = {"GPIO", "AFIO", "CRC", "ETH", "PLL", "SDSPI", "SPI", "TTY", "UART", "WDG", "I2S",
                                           "USB", "USBOTG", "I2C", "ADC", "DAC", "SDIO", "FSMC", "HDMICEC", "CAN"}
module.description                      = {}
module.description.GPIO                 = "GPIO - General Purpose Input Output"
module.description.AFIO                 = "AFIO - Alternative Function Input Output"
module.description.CRC                  = "CRC - CRC caluclation unit"
module.description.ETH                  = "ETH - Ethernet interface"
module.description.PLL                  = "PLL - Phase Lock Loop"
module.description.SDSPI                = "SDSPI - Communication with SD Card by SPI interface"
module.description.SPI                  = "SPI - Serial Pheripheral Interface"
module.description.TTY                  = "TTY - Virtual terminal driver"
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
module.description.CAN                  = "CAN - Controlled Area Network"

local arch                              = {}
arch.list                               = {"stm32f1"} --, "stm32f2", "stm32f3", "stm32f4"}

-- STM32F1 ---------------------------------------------------------------------
arch.stm32f1                            = {}
arch.stm32f1.description                = "stm32f1 microcontroller family"
arch.stm32f1.mcu                        = {}
arch.stm32f1.mcu.list                   = {"STM32F100C8xx", "STM32F100RBxx", "STM32F100RExx", "STM32F100VBxx", "STM32F100ZCxx", "STM32F101C8xx",
                                           "STM32F101CBxx", "STM32F101R8xx", "STM32F101RBxx", "STM32F101V8xx", "STM32F101VBxx", "STM32F103C6xx",
                                           "STM32F103C8xx", "STM32F103CBxx", "STM32F103R6xx", "STM32F103R8xx", "STM32F103RBxx", "STM32F103RCxx",
                                           "STM32F103RDxx", "STM32F103RExx", "STM32F103RGxx", "STM32F103T8xx", "STM32F103V8xx", "STM32F103VBxx",
                                           "STM32F103VCxx", "STM32F103VDxx", "STM32F103VExx", "STM32F103VGxx", "STM32F103ZCxx", "STM32F103ZExx",
                                           "STM32F103ZGxx", "STM32F105RBxx", "STM32F105RCxx", "STM32F105VBxx", "STM32F105VCxx", "STM32F107RCxx",
                                           "STM32F107VBxx", "STM32F107VCxx"}

arch.stm32f1.mcu.STM32F100C8xx          = {}
arch.stm32f1.mcu.STM32F100C8xx.ram      = 8192
arch.stm32f1.mcu.STM32F100C8xx.rom      = 65536
arch.stm32f1.mcu.STM32F100C8xx.family   = "STM32F10X_MD_VL"
arch.stm32f1.mcu.STM32F100C8xx.modules  = {"TTY", "GPIO", "AFIO", "CRC", "PLL", "SDSPI", "SPI", "WDG", "UART"}
arch.stm32f1.mcu.STM32F100C8xx.TIM      = {6, 1, 0}
arch.stm32f1.mcu.STM32F100C8xx.SPI      = {2}
arch.stm32f1.mcu.STM32F100C8xx.I2C      = {2}
arch.stm32f1.mcu.STM32F100C8xx.UART     = {3}
arch.stm32f1.mcu.STM32F100C8xx.CEC      = {1}
arch.stm32f1.mcu.STM32F100C8xx.ADC      = {1, 10}
arch.stm32f1.mcu.STM32F100C8xx.GPIO     = {4}
arch.stm32f1.mcu.STM32F100C8xx.DAC      = {2, 2}

arch.stm32f1.mcu.STM32F100RBxx          = {}
arch.stm32f1.mcu.STM32F100RBxx.ram      = 8192
arch.stm32f1.mcu.STM32F100RBxx.rom      = 131072
arch.stm32f1.mcu.STM32F100RBxx.family   = "STM32F10X_MD_VL"
arch.stm32f1.mcu.STM32F100RBxx.modules  = {"TTY", "GPIO", "AFIO", "CRC", "PLL", "SDSPI", "SPI", "WDG", "UART"}
arch.stm32f1.mcu.STM32F100RBxx.TIM      = {6, 1, 0}
arch.stm32f1.mcu.STM32F100RBxx.SPI      = {2}
arch.stm32f1.mcu.STM32F100RBxx.I2C      = {2}
arch.stm32f1.mcu.STM32F100RBxx.UART     = {3}
arch.stm32f1.mcu.STM32F100RBxx.CEC      = {1}
arch.stm32f1.mcu.STM32F100RBxx.ADC      = {1, 16}
arch.stm32f1.mcu.STM32F100RBxx.GPIO     = {4}
arch.stm32f1.mcu.STM32F100RBxx.DAC      = {2, 2}

arch.stm32f1.mcu.STM32F100RExx          = {}
arch.stm32f1.mcu.STM32F100RExx.ram      = 32768
arch.stm32f1.mcu.STM32F100RExx.rom      = 524288
arch.stm32f1.mcu.STM32F100RExx.family   = "STM32F10X_HD_VL"
arch.stm32f1.mcu.STM32F100RExx.modules  = {"TTY", "GPIO", "AFIO", "CRC", "PLL", "SDSPI", "SPI", "WDG", "UART"}
arch.stm32f1.mcu.STM32F100RExx.TIM      = {10, 1, 0}
arch.stm32f1.mcu.STM32F100RExx.SPI      = {3}
arch.stm32f1.mcu.STM32F100RExx.I2C      = {2}
arch.stm32f1.mcu.STM32F100RExx.UART     = {5}
arch.stm32f1.mcu.STM32F100RExx.CEC      = {1}
arch.stm32f1.mcu.STM32F100RExx.ADC      = {1, 16}
arch.stm32f1.mcu.STM32F100RExx.GPIO     = {4}
arch.stm32f1.mcu.STM32F100RExx.DAC      = {2, 2}

arch.stm32f1.mcu.STM32F100VBxx          = {}
arch.stm32f1.mcu.STM32F100VBxx.ram      = 8192
arch.stm32f1.mcu.STM32F100VBxx.rom      = 131072
arch.stm32f1.mcu.STM32F100VBxx.family   = "STM32F10X_MD_VL"
arch.stm32f1.mcu.STM32F100VBxx.modules  = {"TTY", "GPIO", "AFIO", "CRC", "PLL", "SDSPI", "SPI", "WDG", "UART"}
arch.stm32f1.mcu.STM32F100VBxx.TIM      = {6, 1, 0}
arch.stm32f1.mcu.STM32F100VBxx.SPI      = {2}
arch.stm32f1.mcu.STM32F100VBxx.I2C      = {2}
arch.stm32f1.mcu.STM32F100VBxx.UART     = {3}
arch.stm32f1.mcu.STM32F100VBxx.CEC      = {1}
arch.stm32f1.mcu.STM32F100VBxx.ADC      = {1, 16}
arch.stm32f1.mcu.STM32F100VBxx.GPIO     = {5}
arch.stm32f1.mcu.STM32F100VBxx.DAC      = {2, 2}

arch.stm32f1.mcu.STM32F100ZCxx          = {}
arch.stm32f1.mcu.STM32F100ZCxx.ram      = 24576
arch.stm32f1.mcu.STM32F100ZCxx.rom      = 262144
arch.stm32f1.mcu.STM32F100ZCxx.family   = "STM32F10X_HD_VL"
arch.stm32f1.mcu.STM32F100ZCxx.modules  = {"TTY", "GPIO", "AFIO", "CRC", "PLL", "SDSPI", "SPI", "WDG", "UART"}
arch.stm32f1.mcu.STM32F100ZCxx.FSMC     = {}
arch.stm32f1.mcu.STM32F100ZCxx.TIM      = {10, 1, 0}
arch.stm32f1.mcu.STM32F100ZCxx.SPI      = {3}
arch.stm32f1.mcu.STM32F100ZCxx.I2C      = {2}
arch.stm32f1.mcu.STM32F100ZCxx.UART     = {5}
arch.stm32f1.mcu.STM32F100C8xx.CEC      = {1}
arch.stm32f1.mcu.STM32F100ZCxx.ADC      = {1, 16}
arch.stm32f1.mcu.STM32F100ZCxx.GPIO     = {7}
arch.stm32f1.mcu.STM32F100ZCxx.DAC      = {2, 2}

arch.stm32f1.mcu.STM32F101C8xx          = {}
arch.stm32f1.mcu.STM32F101C8xx.ram      = 10240
arch.stm32f1.mcu.STM32F101C8xx.rom      = 65536
arch.stm32f1.mcu.STM32F101C8xx.family   = "STM32F10X_MD"
arch.stm32f1.mcu.STM32F101C8xx.modules  = {"TTY", "GPIO", "AFIO", "CRC", "PLL", "SDSPI", "SPI", "WDG", "UART"}
arch.stm32f1.mcu.STM32F101C8xx.TIM      = {3, 0, 0}
arch.stm32f1.mcu.STM32F101C8xx.SPI      = {2}
arch.stm32f1.mcu.STM32F101C8xx.I2C      = {2}
arch.stm32f1.mcu.STM32F101C8xx.UART     = {3}
arch.stm32f1.mcu.STM32F101C8xx.ADC      = {1, 10}
arch.stm32f1.mcu.STM32F101C8xx.GPIO     = {4}

arch.stm32f1.mcu.STM32F101CBxx          = {}
arch.stm32f1.mcu.STM32F101CBxx.ram      = 16384
arch.stm32f1.mcu.STM32F101CBxx.rom      = 131072
arch.stm32f1.mcu.STM32F101CBxx.family   = "STM32F10X_MD"
arch.stm32f1.mcu.STM32F101CBxx.modules  = {"TTY", "GPIO", "AFIO", "CRC", "PLL", "SDSPI", "SPI", "WDG", "UART"}
arch.stm32f1.mcu.STM32F101CBxx.TIM      = {3, 0, 0}
arch.stm32f1.mcu.STM32F101CBxx.GPIO     = {4}
arch.stm32f1.mcu.STM32F101CBxx.SPI      = {2}
arch.stm32f1.mcu.STM32F101CBxx.I2C      = {2}
arch.stm32f1.mcu.STM32F101CBxx.UART     = {3}
arch.stm32f1.mcu.STM32F101CBxx.ADC      = {1, 10}

arch.stm32f1.mcu.STM32F101R8xx          = {}
arch.stm32f1.mcu.STM32F101R8xx.ram      = 10240
arch.stm32f1.mcu.STM32F101R8xx.rom      = 65536
arch.stm32f1.mcu.STM32F101R8xx.family   = "STM32F10X_MD"
arch.stm32f1.mcu.STM32F101R8xx.modules  = {"TTY", "GPIO", "AFIO", "CRC", "PLL", "SDSPI", "SPI", "WDG", "UART"}
arch.stm32f1.mcu.STM32F101R8xx.TIM      = {3, 0, 0}
arch.stm32f1.mcu.STM32F101R8xx.GPIO     = {4}
arch.stm32f1.mcu.STM32F101R8xx.SPI      = {2}
arch.stm32f1.mcu.STM32F101R8xx.I2C      = {2}
arch.stm32f1.mcu.STM32F101R8xx.UART     = {3}
arch.stm32f1.mcu.STM32F101R8xx.ADC      = {1, 16}

arch.stm32f1.mcu.STM32F101RBxx          = {}
arch.stm32f1.mcu.STM32F101RBxx.ram      = 16384
arch.stm32f1.mcu.STM32F101RBxx.rom      = 131072
arch.stm32f1.mcu.STM32F101RBxx.family   = "STM32F10X_MD"
arch.stm32f1.mcu.STM32F101RBxx.modules  = {"TTY", "GPIO", "AFIO", "CRC", "PLL", "SDSPI", "SPI", "WDG", "UART"}
arch.stm32f1.mcu.STM32F101RBxx.TIM      = {3, 0, 0}
arch.stm32f1.mcu.STM32F101RBxx.GPIO     = {4}
arch.stm32f1.mcu.STM32F101RBxx.SPI      = {2}
arch.stm32f1.mcu.STM32F101RBxx.I2C      = {2}
arch.stm32f1.mcu.STM32F101RBxx.UART     = {3}
arch.stm32f1.mcu.STM32F101RBxx.ADC      = {1, 16}

arch.stm32f1.mcu.STM32F101V8xx          = {}
arch.stm32f1.mcu.STM32F101V8xx.ram      = 10240
arch.stm32f1.mcu.STM32F101V8xx.rom      = 65536
arch.stm32f1.mcu.STM32F101V8xx.family   = "STM32F10X_MD"
arch.stm32f1.mcu.STM32F101V8xx.modules  = {"TTY", "GPIO", "AFIO", "CRC", "PLL", "SDSPI", "SPI", "WDG", "UART"}
arch.stm32f1.mcu.STM32F101V8xx.TIM      = {3, 0, 0}
arch.stm32f1.mcu.STM32F101V8xx.GPIO     = {5}
arch.stm32f1.mcu.STM32F101V8xx.SPI      = {2}
arch.stm32f1.mcu.STM32F101V8xx.I2C      = {2}
arch.stm32f1.mcu.STM32F101V8xx.UART     = {3}
arch.stm32f1.mcu.STM32F101V8xx.ADC      = {1, 16}

arch.stm32f1.mcu.STM32F101VBxx          = {}
arch.stm32f1.mcu.STM32F101VBxx.ram      = 16384
arch.stm32f1.mcu.STM32F101VBxx.rom      = 131072
arch.stm32f1.mcu.STM32F101VBxx.family   = "STM32F10X_MD"
arch.stm32f1.mcu.STM32F101VBxx.modules  = {"TTY", "GPIO", "AFIO", "CRC", "PLL", "SDSPI", "SPI", "WDG", "UART"}
arch.stm32f1.mcu.STM32F101VBxx.TIM      = {3, 0, 0}
arch.stm32f1.mcu.STM32F101VBxx.GPIO     = {5}
arch.stm32f1.mcu.STM32F101VBxx.SPI      = {2}
arch.stm32f1.mcu.STM32F101VBxx.I2C      = {2}
arch.stm32f1.mcu.STM32F101VBxx.UART     = {3}
arch.stm32f1.mcu.STM32F101VBxx.ADC      = {1, 16}

arch.stm32f1.mcu.STM32F103C6xx          = {}
arch.stm32f1.mcu.STM32F103C6xx.ram      = 10240
arch.stm32f1.mcu.STM32F103C6xx.rom      = 32768
arch.stm32f1.mcu.STM32F103C6xx.family   = "STM32F10X_LD"
arch.stm32f1.mcu.STM32F103C6xx.modules  = {"TTY", "GPIO", "AFIO", "CRC", "PLL", "SDSPI", "SPI", "WDG", "UART", "USB"}
arch.stm32f1.mcu.STM32F103C6xx.TIM      = {2, 1, 0}
arch.stm32f1.mcu.STM32F103C6xx.SPI      = {1}
arch.stm32f1.mcu.STM32F103C6xx.I2C      = {1}
arch.stm32f1.mcu.STM32F103C6xx.UART     = {2}
arch.stm32f1.mcu.STM32F103C6xx.USB      = {1}
arch.stm32f1.mcu.STM32F103C6xx.CAN      = {1}
arch.stm32f1.mcu.STM32F103C6xx.GPIO     = {4}
arch.stm32f1.mcu.STM32F103C6xx.ADC      = {2, 10}

arch.stm32f1.mcu.STM32F103C8xx          = {}
arch.stm32f1.mcu.STM32F103C8xx.ram      = 20480
arch.stm32f1.mcu.STM32F103C8xx.rom      = 65536
arch.stm32f1.mcu.STM32F103C8xx.family   = "STM32F10X_MD"
arch.stm32f1.mcu.STM32F103C8xx.modules  = {"TTY", "GPIO", "AFIO", "CRC", "PLL", "SDSPI", "SPI", "WDG", "UART", "USB"}
arch.stm32f1.mcu.STM32F103C8xx.TIM      = {3, 1, 0}
arch.stm32f1.mcu.STM32F103C8xx.SPI      = {2}
arch.stm32f1.mcu.STM32F103C8xx.I2C      = {2}
arch.stm32f1.mcu.STM32F103C8xx.UART     = {3}
arch.stm32f1.mcu.STM32F103C8xx.USB      = {1}
arch.stm32f1.mcu.STM32F103C8xx.CAN      = {1}
arch.stm32f1.mcu.STM32F103C8xx.GPIO     = {4}
arch.stm32f1.mcu.STM32F103C8xx.ADC      = {2, 10}

arch.stm32f1.mcu.STM32F103CBxx          = {}
arch.stm32f1.mcu.STM32F103CBxx.ram      = 20480
arch.stm32f1.mcu.STM32F103CBxx.rom      = 131072
arch.stm32f1.mcu.STM32F103CBxx.family   = "STM32F10X_MD"
arch.stm32f1.mcu.STM32F103CBxx.modules  = {"TTY", "GPIO", "AFIO", "CRC", "PLL", "SDSPI", "SPI", "WDG", "UART", "USB"}
arch.stm32f1.mcu.STM32F103CBxx.TIM      = {3, 1, 0}
arch.stm32f1.mcu.STM32F103CBxx.SPI      = {2}
arch.stm32f1.mcu.STM32F103CBxx.I2C      = {2}
arch.stm32f1.mcu.STM32F103CBxx.UART     = {3}
arch.stm32f1.mcu.STM32F103CBxx.USB      = {1}
arch.stm32f1.mcu.STM32F103CBxx.CAN      = {1}
arch.stm32f1.mcu.STM32F103CBxx.GPIO     = {4}
arch.stm32f1.mcu.STM32F103CBxx.ADC      = {2, 10}

arch.stm32f1.mcu.STM32F103R6xx          = {}
arch.stm32f1.mcu.STM32F103R6xx.ram      = 10240
arch.stm32f1.mcu.STM32F103R6xx.rom      = 32768
arch.stm32f1.mcu.STM32F103R6xx.family   = "STM32F10X_LD"
arch.stm32f1.mcu.STM32F103R6xx.modules  = {"TTY", "GPIO", "AFIO", "CRC", "PLL", "SDSPI", "SPI", "WDG", "UART", "USB"}
arch.stm32f1.mcu.STM32F103R6xx.TIM      = {2, 1, 0}
arch.stm32f1.mcu.STM32F103R6xx.SPI      = {1}
arch.stm32f1.mcu.STM32F103R6xx.I2C      = {1}
arch.stm32f1.mcu.STM32F103R6xx.UART     = {2}
arch.stm32f1.mcu.STM32F103R6xx.USB      = {1}
arch.stm32f1.mcu.STM32F103R6xx.CAN      = {1}
arch.stm32f1.mcu.STM32F103R6xx.GPIO     = {4}
arch.stm32f1.mcu.STM32F103R6xx.ADC      = {2, 16}

arch.stm32f1.mcu.STM32F103R8xx          = {}
arch.stm32f1.mcu.STM32F103R8xx.ram      = 20480
arch.stm32f1.mcu.STM32F103R8xx.rom      = 65536
arch.stm32f1.mcu.STM32F103R8xx.family   = "STM32F10X_MD"
arch.stm32f1.mcu.STM32F103R8xx.modules  = {"TTY", "GPIO", "AFIO", "CRC", "PLL", "SDSPI", "SPI", "WDG", "UART", "USB"}
arch.stm32f1.mcu.STM32F103R8xx.TIM      = {3, 1, 0}
arch.stm32f1.mcu.STM32F103R8xx.SPI      = {2}
arch.stm32f1.mcu.STM32F103R8xx.I2C      = {2}
arch.stm32f1.mcu.STM32F103R8xx.UART     = {3}
arch.stm32f1.mcu.STM32F103R8xx.USB      = {1}
arch.stm32f1.mcu.STM32F103R8xx.CAN      = {1}
arch.stm32f1.mcu.STM32F103R8xx.GPIO     = {4}
arch.stm32f1.mcu.STM32F103R8xx.ADC      = {2, 16}

arch.stm32f1.mcu.STM32F103RBxx          = {}
arch.stm32f1.mcu.STM32F103RBxx.ram      = 20480
arch.stm32f1.mcu.STM32F103RBxx.rom      = 131072
arch.stm32f1.mcu.STM32F103RBxx.family   = "STM32F10X_MD"
arch.stm32f1.mcu.STM32F103RBxx.modules  = {"TTY", "GPIO", "AFIO", "CRC", "PLL", "SDSPI", "SPI", "WDG", "UART", "USB"}
arch.stm32f1.mcu.STM32F103RBxx.TIM      = {3, 1, 0}
arch.stm32f1.mcu.STM32F103RBxx.SPI      = {2}
arch.stm32f1.mcu.STM32F103RBxx.I2C      = {2}
arch.stm32f1.mcu.STM32F103RBxx.UART     = {3}
arch.stm32f1.mcu.STM32F103RBxx.USB      = {1}
arch.stm32f1.mcu.STM32F103RBxx.CAN      = {1}
arch.stm32f1.mcu.STM32F103RBxx.GPIO     = {4}
arch.stm32f1.mcu.STM32F103RBxx.ADC      = {2, 16}

arch.stm32f1.mcu.STM32F103RCxx          = {}
arch.stm32f1.mcu.STM32F103RCxx.ram      = 49152
arch.stm32f1.mcu.STM32F103RCxx.rom      = 262144
arch.stm32f1.mcu.STM32F103RCxx.family   = "STM32F10X_HD"
arch.stm32f1.mcu.STM32F103RCxx.modules  = {"TTY", "GPIO", "AFIO", "CRC", "PLL", "SDSPI", "SPI", "WDG", "UART", "USB"}
arch.stm32f1.mcu.STM32F103RCxx.TIM      = {4, 2, 2}
arch.stm32f1.mcu.STM32F103RCxx.SPI      = {3}
arch.stm32f1.mcu.STM32F103RCxx.I2S      = {2}
arch.stm32f1.mcu.STM32F103RCxx.I2C      = {2}
arch.stm32f1.mcu.STM32F103RCxx.UART     = {5}
arch.stm32f1.mcu.STM32F103RCxx.USB      = {1}
arch.stm32f1.mcu.STM32F103RCxx.CAN      = {1}
arch.stm32f1.mcu.STM32F103RCxx.SDIO     = {1}
arch.stm32f1.mcu.STM32F103RCxx.GPIO     = {4}
arch.stm32f1.mcu.STM32F103RCxx.ADC      = {3, 16}
arch.stm32f1.mcu.STM32F103RCxx.DAC      = {2, 2}

arch.stm32f1.mcu.STM32F103RDxx          = {}
arch.stm32f1.mcu.STM32F103RDxx.ram      = 65536
arch.stm32f1.mcu.STM32F103RDxx.rom      = 393216
arch.stm32f1.mcu.STM32F103RDxx.family   = "STM32F10X_HD"
arch.stm32f1.mcu.STM32F103RDxx.modules  = {"TTY", "GPIO", "AFIO", "CRC", "PLL", "SDSPI", "SPI", "WDG", "UART", "USB"}
arch.stm32f1.mcu.STM32F103RDxx.TIM      = {4, 2, 2}
arch.stm32f1.mcu.STM32F103RDxx.SPI      = {3}
arch.stm32f1.mcu.STM32F103RDxx.I2S      = {2}
arch.stm32f1.mcu.STM32F103RDxx.I2C      = {2}
arch.stm32f1.mcu.STM32F103RDxx.UART     = {5}
arch.stm32f1.mcu.STM32F103RDxx.USB      = {1}
arch.stm32f1.mcu.STM32F103RDxx.CAN      = {1}
arch.stm32f1.mcu.STM32F103RDxx.SDIO     = {1}
arch.stm32f1.mcu.STM32F103RDxx.GPIO     = {4}
arch.stm32f1.mcu.STM32F103RDxx.ADC      = {3, 16}
arch.stm32f1.mcu.STM32F103RDxx.DAC      = {2, 2}

arch.stm32f1.mcu.STM32F103RExx          = {}
arch.stm32f1.mcu.STM32F103RExx.ram      = 65536
arch.stm32f1.mcu.STM32F103RExx.rom      = 524288
arch.stm32f1.mcu.STM32F103RExx.family   = "STM32F10X_HD"
arch.stm32f1.mcu.STM32F103RExx.modules  = {"TTY", "GPIO", "AFIO", "CRC", "PLL", "SDSPI", "SPI", "WDG", "UART", "USB"}
arch.stm32f1.mcu.STM32F103RExx.TIM      = {4, 2, 2}
arch.stm32f1.mcu.STM32F103RExx.SPI      = {3}
arch.stm32f1.mcu.STM32F103RExx.I2S      = {2}
arch.stm32f1.mcu.STM32F103RExx.I2C      = {2}
arch.stm32f1.mcu.STM32F103RExx.UART     = {5}
arch.stm32f1.mcu.STM32F103RExx.USB      = {1}
arch.stm32f1.mcu.STM32F103RExx.CAN      = {1}
arch.stm32f1.mcu.STM32F103RExx.SDIO     = {1}
arch.stm32f1.mcu.STM32F103RExx.GPIO     = {4}
arch.stm32f1.mcu.STM32F103RExx.ADC      = {3, 16}
arch.stm32f1.mcu.STM32F103RExx.DAC      = {2, 2}

arch.stm32f1.mcu.STM32F103RGxx          = {}
arch.stm32f1.mcu.STM32F103RGxx.ram      = 98304
arch.stm32f1.mcu.STM32F103RGxx.rom      = 1048576
arch.stm32f1.mcu.STM32F103RGxx.family   = "STM32F10X_XL"
arch.stm32f1.mcu.STM32F103RGxx.modules  = {"TTY", "GPIO", "AFIO", "CRC", "PLL", "SDSPI", "SPI", "WDG", "UART", "USB"}
arch.stm32f1.mcu.STM32F103RGxx.TIM      = {10, 2, 2}
arch.stm32f1.mcu.STM32F103RGxx.SPI      = {3}
arch.stm32f1.mcu.STM32F103RGxx.I2S      = {2}
arch.stm32f1.mcu.STM32F103RGxx.I2C      = {2}
arch.stm32f1.mcu.STM32F103RGxx.UART     = {5}
arch.stm32f1.mcu.STM32F103RGxx.USB      = {1}
arch.stm32f1.mcu.STM32F103RGxx.CAN      = {1}
arch.stm32f1.mcu.STM32F103RGxx.SDIO     = {1}
arch.stm32f1.mcu.STM32F103RGxx.GPIO     = {4}
arch.stm32f1.mcu.STM32F103RGxx.ADC      = {3, 16}
arch.stm32f1.mcu.STM32F103RGxx.DAC      = {2, 2}

arch.stm32f1.mcu.STM32F103T8xx          = {}
arch.stm32f1.mcu.STM32F103T8xx.ram      = 20480
arch.stm32f1.mcu.STM32F103T8xx.rom      = 65536
arch.stm32f1.mcu.STM32F103T8xx.family   = "STM32F10X_MD"
arch.stm32f1.mcu.STM32F103T8xx.modules  = {"TTY", "GPIO", "AFIO", "CRC", "PLL", "SDSPI", "SPI", "WDG", "UART", "USB"}
arch.stm32f1.mcu.STM32F103T8xx.TIM      = {2, 1, 0}
arch.stm32f1.mcu.STM32F103T8xx.SPI      = {1}
arch.stm32f1.mcu.STM32F103T8xx.I2C      = {1}
arch.stm32f1.mcu.STM32F103T8xx.UART     = {2}
arch.stm32f1.mcu.STM32F103T8xx.USB      = {1}
arch.stm32f1.mcu.STM32F103T8xx.CAN      = {1}
arch.stm32f1.mcu.STM32F103T8xx.GPIO     = {2}
arch.stm32f1.mcu.STM32F103T8xx.ADC      = {2, 10}

arch.stm32f1.mcu.STM32F103V8xx          = {}
arch.stm32f1.mcu.STM32F103V8xx.ram      = 20480
arch.stm32f1.mcu.STM32F103V8xx.rom      = 65536
arch.stm32f1.mcu.STM32F103V8xx.family   = "STM32F10X_MD"
arch.stm32f1.mcu.STM32F103V8xx.modules  = {"TTY", "GPIO", "AFIO", "CRC", "PLL", "SDSPI", "SPI", "WDG", "UART", "USB"}
arch.stm32f1.mcu.STM32F103V8xx.TIM      = {3, 1, 0}
arch.stm32f1.mcu.STM32F103V8xx.SPI      = {2}
arch.stm32f1.mcu.STM32F103V8xx.I2C      = {2}
arch.stm32f1.mcu.STM32F103V8xx.UART     = {3}
arch.stm32f1.mcu.STM32F103V8xx.USB      = {1}
arch.stm32f1.mcu.STM32F103V8xx.CAN      = {1}
arch.stm32f1.mcu.STM32F103V8xx.GPIO     = {5}
arch.stm32f1.mcu.STM32F103V8xx.ADC      = {2, 16}

arch.stm32f1.mcu.STM32F103VBxx          = {}
arch.stm32f1.mcu.STM32F103VBxx.ram      = 20480
arch.stm32f1.mcu.STM32F103VBxx.rom      = 131072
arch.stm32f1.mcu.STM32F103VBxx.family   = "STM32F10X_MD"
arch.stm32f1.mcu.STM32F103VBxx.modules  = {"TTY", "GPIO", "AFIO", "CRC", "PLL", "SDSPI", "SPI", "WDG", "UART", "USB"}
arch.stm32f1.mcu.STM32F103VBxx.TIM      = {3, 1, 0}
arch.stm32f1.mcu.STM32F103VBxx.SPI      = {2}
arch.stm32f1.mcu.STM32F103VBxx.I2C      = {2}
arch.stm32f1.mcu.STM32F103VBxx.UART     = {3}
arch.stm32f1.mcu.STM32F103VBxx.USB      = {1}
arch.stm32f1.mcu.STM32F103VBxx.CAN      = {1}
arch.stm32f1.mcu.STM32F103VBxx.GPIO     = {5}
arch.stm32f1.mcu.STM32F103VBxx.ADC      = {2, 16}

arch.stm32f1.mcu.STM32F103VCxx          = {}
arch.stm32f1.mcu.STM32F103VCxx.ram      = 49152
arch.stm32f1.mcu.STM32F103VCxx.rom      = 262144
arch.stm32f1.mcu.STM32F103VCxx.family   = "STM32F10X_HD"
arch.stm32f1.mcu.STM32F103VCxx.modules  = {"TTY", "GPIO", "AFIO", "CRC", "PLL", "SDSPI", "SPI", "WDG", "UART", "USB"}
arch.stm32f1.mcu.STM32F103VCxx.FSMC     = {}
arch.stm32f1.mcu.STM32F103VCxx.TIM      = {4, 2, 2}
arch.stm32f1.mcu.STM32F103VCxx.SPI      = {3}
arch.stm32f1.mcu.STM32F103VCxx.I2S      = {2}
arch.stm32f1.mcu.STM32F103VCxx.I2C      = {2}
arch.stm32f1.mcu.STM32F103VCxx.UART     = {5}
arch.stm32f1.mcu.STM32F103VCxx.USB      = {1}
arch.stm32f1.mcu.STM32F103VCxx.CAN      = {1}
arch.stm32f1.mcu.STM32F103VCxx.SDIO     = {1}
arch.stm32f1.mcu.STM32F103VCxx.GPIO     = {5}
arch.stm32f1.mcu.STM32F103VCxx.ADC      = {3, 16}
arch.stm32f1.mcu.STM32F103VCxx.DAC      = {2, 2}

arch.stm32f1.mcu.STM32F103VDxx          = {}
arch.stm32f1.mcu.STM32F103VDxx.ram      = 65536
arch.stm32f1.mcu.STM32F103VDxx.rom      = 393216
arch.stm32f1.mcu.STM32F103VDxx.family   = "STM32F10X_HD"
arch.stm32f1.mcu.STM32F103VDxx.modules  = {"TTY", "GPIO", "AFIO", "CRC", "PLL", "SDSPI", "SPI", "WDG", "UART", "USB"}
arch.stm32f1.mcu.STM32F103VDxx.FSMC     = {}
arch.stm32f1.mcu.STM32F103VDxx.TIM      = {4, 2, 2}
arch.stm32f1.mcu.STM32F103VDxx.SPI      = {3}
arch.stm32f1.mcu.STM32F103VDxx.I2S      = {2}
arch.stm32f1.mcu.STM32F103VDxx.I2C      = {2}
arch.stm32f1.mcu.STM32F103VDxx.UART     = {5}
arch.stm32f1.mcu.STM32F103VDxx.USB      = {1}
arch.stm32f1.mcu.STM32F103VDxx.CAN      = {1}
arch.stm32f1.mcu.STM32F103VDxx.SDIO     = {1}
arch.stm32f1.mcu.STM32F103VDxx.GPIO     = {5}
arch.stm32f1.mcu.STM32F103VDxx.ADC      = {3, 16}
arch.stm32f1.mcu.STM32F103VDxx.DAC      = {2, 2}

arch.stm32f1.mcu.STM32F103VExx          = {}
arch.stm32f1.mcu.STM32F103VExx.ram      = 65536
arch.stm32f1.mcu.STM32F103VExx.rom      = 524288
arch.stm32f1.mcu.STM32F103VExx.family   = "STM32F10X_HD"
arch.stm32f1.mcu.STM32F103VExx.modules  = {"TTY", "GPIO", "AFIO", "CRC", "PLL", "SDSPI", "SPI", "WDG", "UART", "USB"}
arch.stm32f1.mcu.STM32F103VExx.FSMC     = {}
arch.stm32f1.mcu.STM32F103VExx.TIM      = {4, 2, 2}
arch.stm32f1.mcu.STM32F103VExx.SPI      = {3}
arch.stm32f1.mcu.STM32F103VExx.I2S      = {2}
arch.stm32f1.mcu.STM32F103VExx.I2C      = {2}
arch.stm32f1.mcu.STM32F103VExx.UART     = {5}
arch.stm32f1.mcu.STM32F103VExx.USB      = {1}
arch.stm32f1.mcu.STM32F103VExx.CAN      = {1}
arch.stm32f1.mcu.STM32F103VExx.SDIO     = {1}
arch.stm32f1.mcu.STM32F103VExx.GPIO     = {5}
arch.stm32f1.mcu.STM32F103VExx.ADC      = {3, 16}
arch.stm32f1.mcu.STM32F103VExx.DAC      = {2, 2}

arch.stm32f1.mcu.STM32F103VGxx          = {}
arch.stm32f1.mcu.STM32F103VGxx.ram      = 98304
arch.stm32f1.mcu.STM32F103VGxx.rom      = 1048576
arch.stm32f1.mcu.STM32F103VGxx.family   = "STM32F10X_XL"
arch.stm32f1.mcu.STM32F103VGxx.modules  = {"TTY", "GPIO", "AFIO", "CRC", "PLL", "SDSPI", "SPI", "WDG", "UART", "USB"}
arch.stm32f1.mcu.STM32F103VGxx.FSMC     = {}
arch.stm32f1.mcu.STM32F103VGxx.TIM      = {10, 2, 2}
arch.stm32f1.mcu.STM32F103VGxx.SPI      = {3}
arch.stm32f1.mcu.STM32F103VGxx.I2S      = {2}
arch.stm32f1.mcu.STM32F103VGxx.I2C      = {2}
arch.stm32f1.mcu.STM32F103VGxx.UART     = {5}
arch.stm32f1.mcu.STM32F103VGxx.USB      = {1}
arch.stm32f1.mcu.STM32F103VGxx.CAN      = {1}
arch.stm32f1.mcu.STM32F103VGxx.SDIO     = {1}
arch.stm32f1.mcu.STM32F103VGxx.GPIO     = {5}
arch.stm32f1.mcu.STM32F103VGxx.ADC      = {3, 16}
arch.stm32f1.mcu.STM32F103VGxx.DAC      = {2, 2}

arch.stm32f1.mcu.STM32F103ZCxx          = {}
arch.stm32f1.mcu.STM32F103ZCxx.ram      = 49152
arch.stm32f1.mcu.STM32F103ZCxx.rom      = 262144
arch.stm32f1.mcu.STM32F103ZCxx.family   = "STM32F10X_HD"
arch.stm32f1.mcu.STM32F103ZCxx.modules  = {"TTY", "GPIO", "AFIO", "CRC", "PLL", "SDSPI", "SPI", "WDG", "UART", "USB"}
arch.stm32f1.mcu.STM32F103ZCxx.FSMC     = {}
arch.stm32f1.mcu.STM32F103ZCxx.TIM      = {4, 2, 2}
arch.stm32f1.mcu.STM32F103ZCxx.SPI      = {3}
arch.stm32f1.mcu.STM32F103ZCxx.I2S      = {2}
arch.stm32f1.mcu.STM32F103ZCxx.I2C      = {2}
arch.stm32f1.mcu.STM32F103ZCxx.UART     = {5}
arch.stm32f1.mcu.STM32F103ZCxx.USB      = {1}
arch.stm32f1.mcu.STM32F103ZCxx.CAN      = {1}
arch.stm32f1.mcu.STM32F103ZCxx.SDIO     = {1}
arch.stm32f1.mcu.STM32F103ZCxx.GPIO     = {7}
arch.stm32f1.mcu.STM32F103ZCxx.ADC      = {3, 21}
arch.stm32f1.mcu.STM32F103ZCxx.DAC      = {2, 2}

arch.stm32f1.mcu.STM32F103ZExx          = {}
arch.stm32f1.mcu.STM32F103ZExx.ram      = 65536
arch.stm32f1.mcu.STM32F103ZExx.rom      = 524288
arch.stm32f1.mcu.STM32F103ZExx.family   = "STM32F10X_HD"
arch.stm32f1.mcu.STM32F103ZExx.modules  = {"TTY", "GPIO", "AFIO", "CRC", "PLL", "SDSPI", "SPI", "WDG", "UART", "USB"}
arch.stm32f1.mcu.STM32F103ZExx.FSMC     = {}
arch.stm32f1.mcu.STM32F103ZExx.TIM      = {4, 2, 2}
arch.stm32f1.mcu.STM32F103ZExx.SPI      = {3}
arch.stm32f1.mcu.STM32F103ZExx.I2S      = {2}
arch.stm32f1.mcu.STM32F103ZExx.I2C      = {2}
arch.stm32f1.mcu.STM32F103ZExx.UART     = {5}
arch.stm32f1.mcu.STM32F103ZExx.USB      = {1}
arch.stm32f1.mcu.STM32F103ZExx.CAN      = {1}
arch.stm32f1.mcu.STM32F103ZExx.SDIO     = {1}
arch.stm32f1.mcu.STM32F103ZExx.GPIO     = {7}
arch.stm32f1.mcu.STM32F103ZExx.ADC      = {3, 21}
arch.stm32f1.mcu.STM32F103ZExx.DAC      = {2, 2}

arch.stm32f1.mcu.STM32F103ZGxx          = {}
arch.stm32f1.mcu.STM32F103ZGxx.ram      = 98304
arch.stm32f1.mcu.STM32F103ZGxx.rom      = 1048576
arch.stm32f1.mcu.STM32F103ZGxx.family   = "STM32F10X_XL"
arch.stm32f1.mcu.STM32F103ZGxx.modules  = {"TTY", "GPIO", "AFIO", "CRC", "PLL", "SDSPI", "SPI", "WDG", "UART", "USB"}
arch.stm32f1.mcu.STM32F103ZGxx.FSMC     = {}
arch.stm32f1.mcu.STM32F103ZGxx.TIM      = {10, 2, 2}
arch.stm32f1.mcu.STM32F103ZGxx.SPI      = {3}
arch.stm32f1.mcu.STM32F103ZGxx.I2S      = {2}
arch.stm32f1.mcu.STM32F103ZGxx.I2C      = {2}
arch.stm32f1.mcu.STM32F103ZGxx.UART     = {5}
arch.stm32f1.mcu.STM32F103ZGxx.USB      = {1}
arch.stm32f1.mcu.STM32F103ZGxx.CAN      = {1}
arch.stm32f1.mcu.STM32F103ZGxx.SDIO     = {1}
arch.stm32f1.mcu.STM32F103ZGxx.GPIO     = {7}
arch.stm32f1.mcu.STM32F103ZGxx.ADC      = {3, 21}
arch.stm32f1.mcu.STM32F103ZGxx.DAC      = {2, 2}

arch.stm32f1.mcu.STM32F105RBxx          = {}
arch.stm32f1.mcu.STM32F105RBxx.ram      = 65536
arch.stm32f1.mcu.STM32F105RBxx.rom      = 131072
arch.stm32f1.mcu.STM32F105RBxx.family   = "STM32F10X_CL"
arch.stm32f1.mcu.STM32F105RBxx.modules  = {"TTY", "GPIO", "AFIO", "CRC", "PLL", "SDSPI", "SPI", "WDG", "UART"}
arch.stm32f1.mcu.STM32F105RBxx.TIM      = {4, 1, 2}
arch.stm32f1.mcu.STM32F105RBxx.SPI      = {3}
arch.stm32f1.mcu.STM32F105RBxx.I2S      = {2}
arch.stm32f1.mcu.STM32F105RBxx.I2C      = {2}
arch.stm32f1.mcu.STM32F105RBxx.UART     = {5}
arch.stm32f1.mcu.STM32F105RBxx.USBOTG   = {}
arch.stm32f1.mcu.STM32F105RBxx.CAN      = {2}
arch.stm32f1.mcu.STM32F105RBxx.GPIO     = {4}
arch.stm32f1.mcu.STM32F105RBxx.ADC      = {2, 16}
arch.stm32f1.mcu.STM32F105RBxx.DAC      = {2, 2}

arch.stm32f1.mcu.STM32F105RCxx          = {}
arch.stm32f1.mcu.STM32F105RCxx.ram      = 65536
arch.stm32f1.mcu.STM32F105RCxx.rom      = 262144
arch.stm32f1.mcu.STM32F105RCxx.family   = "STM32F10X_CL"
arch.stm32f1.mcu.STM32F105RCxx.modules  = {"TTY", "GPIO", "AFIO", "CRC", "PLL", "SDSPI", "SPI", "WDG", "UART"}
arch.stm32f1.mcu.STM32F105RCxx.TIM      = {4, 1, 2}
arch.stm32f1.mcu.STM32F105RCxx.SPI      = {3}
arch.stm32f1.mcu.STM32F105RCxx.I2S      = {2}
arch.stm32f1.mcu.STM32F105RCxx.I2C      = {2}
arch.stm32f1.mcu.STM32F105RCxx.UART     = {5}
arch.stm32f1.mcu.STM32F105RCxx.USBOTG   = {}
arch.stm32f1.mcu.STM32F105RCxx.CAN      = {2}
arch.stm32f1.mcu.STM32F105RCxx.GPIO     = {4}
arch.stm32f1.mcu.STM32F105RCxx.ADC      = {2, 16}
arch.stm32f1.mcu.STM32F105RCxx.DAC      = {2, 2}

arch.stm32f1.mcu.STM32F105VBxx          = {}
arch.stm32f1.mcu.STM32F105VBxx.ram      = 65536
arch.stm32f1.mcu.STM32F105VBxx.rom      = 131072
arch.stm32f1.mcu.STM32F105VBxx.family   = "STM32F10X_CL"
arch.stm32f1.mcu.STM32F105VBxx.modules  = {"TTY", "GPIO", "AFIO", "CRC", "PLL", "SDSPI", "SPI", "WDG", "UART"}
arch.stm32f1.mcu.STM32F105VBxx.TIM      = {4, 1, 2}
arch.stm32f1.mcu.STM32F105VBxx.SPI      = {3}
arch.stm32f1.mcu.STM32F105VBxx.I2S      = {2}
arch.stm32f1.mcu.STM32F105VBxx.I2C      = {2}
arch.stm32f1.mcu.STM32F105VBxx.UART     = {5}
arch.stm32f1.mcu.STM32F105VBxx.USBOTG   = {}
arch.stm32f1.mcu.STM32F105VBxx.CAN      = {2}
arch.stm32f1.mcu.STM32F105VBxx.GPIO     = {5}
arch.stm32f1.mcu.STM32F105VBxx.ADC      = {2, 16}
arch.stm32f1.mcu.STM32F105VBxx.DAC      = {2, 2}

arch.stm32f1.mcu.STM32F105VCxx          = {}
arch.stm32f1.mcu.STM32F105VCxx.ram      = 65536
arch.stm32f1.mcu.STM32F105VCxx.rom      = 262144
arch.stm32f1.mcu.STM32F105VCxx.family   = "STM32F10X_CL"
arch.stm32f1.mcu.STM32F105VCxx.modules  = {"TTY", "GPIO", "AFIO", "CRC", "PLL", "SDSPI", "SPI", "WDG", "UART"}
arch.stm32f1.mcu.STM32F105VCxx.TIM      = {4, 1, 2}
arch.stm32f1.mcu.STM32F105VCxx.SPI      = {3}
arch.stm32f1.mcu.STM32F105VCxx.I2S      = {2}
arch.stm32f1.mcu.STM32F105VCxx.I2C      = {2}
arch.stm32f1.mcu.STM32F105VCxx.UART     = {5}
arch.stm32f1.mcu.STM32F105VCxx.USBOTG   = {}
arch.stm32f1.mcu.STM32F105VCxx.CAN      = {2}
arch.stm32f1.mcu.STM32F105VCxx.GPIO     = {5}
arch.stm32f1.mcu.STM32F105VCxx.ADC      = {2, 16}
arch.stm32f1.mcu.STM32F105VCxx.DAC      = {2, 2}

arch.stm32f1.mcu.STM32F107RCxx          = {}
arch.stm32f1.mcu.STM32F107RCxx.ram      = 65536
arch.stm32f1.mcu.STM32F107RCxx.rom      = 262144
arch.stm32f1.mcu.STM32F107RCxx.family   = "STM32F10X_CL"
arch.stm32f1.mcu.STM32F107RCxx.modules  = {"TTY", "GPIO", "AFIO", "CRC", "PLL", "SDSPI", "SPI", "WDG", "UART", "ETH"}
arch.stm32f1.mcu.STM32F107RCxx.ETH      = {}
arch.stm32f1.mcu.STM32F107RCxx.TIM      = {4, 1, 2}
arch.stm32f1.mcu.STM32F107RCxx.SPI      = {3}
arch.stm32f1.mcu.STM32F107RCxx.I2S      = {2}
arch.stm32f1.mcu.STM32F107RCxx.I2C      = {1}
arch.stm32f1.mcu.STM32F107RCxx.UART     = {5}
arch.stm32f1.mcu.STM32F107RCxx.USBOTG   = {}
arch.stm32f1.mcu.STM32F107RCxx.CAN      = {2}
arch.stm32f1.mcu.STM32F107RCxx.GPIO     = {4}
arch.stm32f1.mcu.STM32F107RCxx.ADC      = {2, 16}
arch.stm32f1.mcu.STM32F107RCxx.DAC      = {2, 2}

arch.stm32f1.mcu.STM32F107VBxx          = {}
arch.stm32f1.mcu.STM32F107VBxx.ram      = 65536
arch.stm32f1.mcu.STM32F107VBxx.rom      = 131072
arch.stm32f1.mcu.STM32F107VBxx.family   = "STM32F10X_CL"
arch.stm32f1.mcu.STM32F107VBxx.modules  = {"TTY", "GPIO", "AFIO", "CRC", "PLL", "SDSPI", "SPI", "WDG", "UART", "ETH"}
arch.stm32f1.mcu.STM32F107VBxx.ETH      = {}
arch.stm32f1.mcu.STM32F107VBxx.TIM      = {4, 1, 2}
arch.stm32f1.mcu.STM32F107VBxx.SPI      = {3}
arch.stm32f1.mcu.STM32F107VBxx.I2S      = {2}
arch.stm32f1.mcu.STM32F107VBxx.I2C      = {1}
arch.stm32f1.mcu.STM32F107VBxx.UART     = {5}
arch.stm32f1.mcu.STM32F107VBxx.USBOTG   = {}
arch.stm32f1.mcu.STM32F107VBxx.CAN      = {2}
arch.stm32f1.mcu.STM32F107VBxx.GPIO     = {5}
arch.stm32f1.mcu.STM32F107VBxx.ADC      = {2, 16}
arch.stm32f1.mcu.STM32F107VBxx.DAC      = {2, 2}

arch.stm32f1.mcu.STM32F107VCxx          = {}
arch.stm32f1.mcu.STM32F107VCxx.ram      = 65536
arch.stm32f1.mcu.STM32F107VCxx.rom      = 262144
arch.stm32f1.mcu.STM32F107VCxx.family   = "STM32F10X_CL"
arch.stm32f1.mcu.STM32F107VCxx.modules  = {"TTY", "GPIO", "AFIO", "CRC", "PLL", "SDSPI", "SPI", "WDG", "UART", "ETH"}
arch.stm32f1.mcu.STM32F107VCxx.ETH      = {}
arch.stm32f1.mcu.STM32F107VCxx.TIM      = {4, 1, 2}
arch.stm32f1.mcu.STM32F107VCxx.SPI      = {3}
arch.stm32f1.mcu.STM32F107VCxx.I2S      = {2}
arch.stm32f1.mcu.STM32F107VCxx.I2C      = {1}
arch.stm32f1.mcu.STM32F107VCxx.UART     = {5}
arch.stm32f1.mcu.STM32F107VCxx.USBOTG   = {}
arch.stm32f1.mcu.STM32F107VCxx.CAN      = {2}
arch.stm32f1.mcu.STM32F107VCxx.GPIO     = {5}
arch.stm32f1.mcu.STM32F107VCxx.ADC      = {2, 16}
arch.stm32f1.mcu.STM32F107VCxx.DAC      = {2, 2}

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
-- @brief Method return current architecture
-- @param None
-- @return Architecture
--------------------------------------------------------------------------------
function db:get_arch()
        return key_read(db.path.project.mk, "PROJECT_CPU_ARCH")
end

--------------------------------------------------------------------------------
-- @brief Method return current CPU name
-- @param None
-- @return CPU name string
--------------------------------------------------------------------------------
function db:get_cpu_name()
        return key_read(db.path[db:get_arch()].cpu.flags, "__CPU_NAME__")
end

--------------------------------------------------------------------------------
-- @brief Method set current architecture
-- @param arch          architecture to set
-- @return None
--------------------------------------------------------------------------------
function db:set_arch(new_arch)
        for i = 1, #arch.list do
                if arch.list[i] == new_arch then
                        key_save(db.path.project.mk, "PROJECT_CPU_ARCH", new_arch)
                        key_save(db.path.project.flags, "__CPU_ARCH__",  new_arch)
                end
        end
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
-- @brief Method return ROM size for selected MCU
-- @param mcu_name      MCU name
-- @return ROM size
--------------------------------------------------------------------------------
function db:get_mcu_rom_size(mcu_name)
        return arch[get_mcu_arch(mcu_name)].mcu[mcu_name].rom
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
