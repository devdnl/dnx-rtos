/*=========================================================================*//**
@file    afm_flags.h

@author  Daniel Zorychta

@brief   AFIO Module configuration.

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

#ifndef _AFM_FLAGS_H_
#define _AFM_FLAGS_H_

/*--
this:SetLayout("TitledGridBack", 2, "Home > Microcontroller > AFM",
               function() this:LoadFile("arch/arch_flags.h") end)
++*/



/*--
this:AddExtraWidget("Label", "LabelEvent", "Cortex Event Output", -1, "bold")
this:AddExtraWidget("Void", "VoidEvent")
++*/
/*--
this:AddWidget("Combobox", "Output")
this:AddItem("Disable", "_NO_")
this:AddItem("Enable", "_YES_")
--*/
#define __AFM_EVENT_OUT_ENABLE__ _NO_

/*--
this:AddWidget("Combobox", "Port")
for i = 0, 6 do local c = string.char(65 + i) this:AddItem("P"..c, tostring(i)) end
--*/
#define __AFM_EVENT_OUT_PORT__ 0

/*--
this:AddWidget("Combobox", "Pin number")
for i = 0, 15 do this:AddItem(tostring(i), tostring(i)) end
--*/
#define __AFM_EVENT_OUT_PIN__ 0



/*--
this:AddExtraWidget("Label", "LabelRemap", "\nPeripheral remap", -1, "bold")
this:AddExtraWidget("Void", "VoidRemap")
++*/
/*--
this:AddWidget("Combobox", "Remap SPI1")
this:AddItem("No (NSS/PA4, SCK/PA5, MISO/PA6, MOSI/PA7)", "0")
this:AddItem("Yes (NSS/PA15, SCK/PB3, MISO/PB4, MOSI/PB5)", "1")
--*/
#define __AFM_REMAP_SPI1__ 0

/*--
this:AddWidget("Combobox", "Remap I2C1")
this:AddItem("No (SCL/PB6, SDA/PB7)", "0")
this:AddItem("Yes (SCL/PA9, SDA/PB9)", "1")
--*/
#define __AFM_REMAP_I2C1__ 0

/*--
this:AddWidget("Combobox", "Remap USART1")
this:AddItem("No (TX/PA9, RX/PA10)", "0")
this:AddItem("Yes (TX/PB6, RX/PB7)", "1")
--*/
#define __AFM_REMAP_USART1__ 0

/*--
this:AddWidget("Combobox", "Remap USART2")
this:AddItem("No (CTS/PA0, RTS/PA1, TX/PA2, RX/PA3, CK/PA4)", "0")
this:AddItem("Yes (CTS/PD3, RTS/PD4, TX/PD5, RX/PD6, CK/PD7)", "1")
--*/
#define __AFM_REMAP_USART2__ 1

/*--
this:AddWidget("Combobox", "Remap USART3")
this:AddItem("No (TX/PB10, RX/PB11, CK/PB12, CTS/PB13, RTS/PB14)", "0")
this:AddItem("Partial (CTS/PD3, RTS/PD4, TX/PD5, RX/PD6, CK/PD7)", "1")
this:AddItem("Full (TX/PD8, RX/PD9, CK/PD10, CTS/PD11, RTS/PD12)", "2")
--*/
#define __AFM_REMAP_USART3__ 0

/*--
this:AddWidget("Combobox", "Remap TIM1")
this:AddItem("No (ETR/PA12, CH1-4/PA8-11, BKIN/PB12, CH1-3N/PB13-15)", "0")
this:AddItem("Partial (ETR/PA12, CH1-4/PA8-11, BKIN/PA6, CH1-3N/PA7/PB0-1)", "1")
this:AddItem("Full (ETR/PE7, CH1-4/PE9/11/13/14, BKIN/PE15, CH1-3N/PE8/10/12)", "2")
--*/
#define __AFM_REMAP_TIM1__ 0

/*--
this:AddWidget("Combobox", "Remap TIM2")
this:AddItem("No (CH1/ETR/PA0, CH2/PA1, CH3/PA2, CH4/PA3)", "0")
this:AddItem("Partial (CH1/ETR/PA15, CH2/PB3, CH3/PA2, CH4/PA3)", "1")
this:AddItem("Partial (CH1/ETR/PA0, CH2/PA1, CH3/PB10, CH4/PB11)", "2")
this:AddItem("Full (CH1/ETR/PA15, CH2/PB3, CH3/PB10, CH4/PB11)", "3")
--*/
#define __AFM_REMAP_TIM2__ 0

/*--
this:AddWidget("Combobox", "Remap TIM3")
this:AddItem("No (CH1/PA6, CH2/PA7, CH3/PB0, CH4/PB1)", "0")
this:AddItem("Partial (CH1/PB4, CH2/PB5, CH3/PB0, CH4/PB1)", "1")
this:AddItem("Full (CH1/PC6, CH2/PC7, CH3/PC8, CH4/PC9)", "2")
--*/
#define __AFM_REMAP_TIM3__ 0

/*--
this:AddWidget("Combobox", "Remap TIM4")
this:AddItem("No (TIM4_CH1/PB6, TIM4_CH2/PB7, TIM4_CH3/PB8, TIM4_CH4/PB9)", "0")
this:AddItem("Yes (TIM4_CH1/PD12, TIM4_CH2/PD13, TIM4_CH3/PD14, TIM4_CH4/PD15)", "1")
--*/
#define __AFM_REMAP_TIM4__ 0

/*--
this:AddWidget("Combobox", "Remap TIM5_CH4")
this:AddItem("No (TIM5_CH4 connected to PA3)", "0")
this:AddItem("Yes (LSI internal clock connected to TIM5_CH4)", "1")
--*/
#define __AFM_REMAP_TIM5CH4__ 0

/*--
this:AddWidget("Combobox", "Remap CAN")
this:AddItem("No (CAN_RX/PA11, CAN_TX/PA12)", "0")
this:AddItem("Variant 1 (CAN_RX/PB8, CAN_TX/PB9)", "1")
this:AddItem("Variant 2 (CAN_RX/PD0, CAN_TX/PD1)", "2")
--*/
#define __AFM_REMAP_CAN__ 0

/*--
this:AddWidget("Combobox", "Remap PD0/PD1")
this:AddItem("No (PD0/PD1 are not present in the 36-,48-,64-pin packages)", "0")
this:AddItem("Yes (PD0/OSC_IN, PD1/OSC_OUT)", "1")
--*/
#define __AFM_REMAP_PD01__ 0

/*--
this:AddWidget("Combobox", "Remap ADC1 ETRGINJ")
this:AddItem("No (ADC1 Ext. Trigger injected conversion conn. to EXTI15)", "0")
this:AddItem("Yes (ADC1 Ext. Event injected conversion conn. to TIM8_CH4)", "1")
--*/
#define __AFM_REMAP_ADC1_ETRGINJ__ 0

/*--
this:AddWidget("Combobox", "Remap ADC1 ETRGREG")
this:AddItem("No (ADC1 Ext. Trigger regular conversion conn. to EXTI11)", "0")
this:AddItem("Yes (ADC1 Ext. Event regular conversion conn. to TIM8_TRGO)", "1")
--*/
#define __AFM_REMAP_ADC1_ETRGREG__ 0

/*--
this:AddWidget("Combobox", "Remap ADC2 ETRGINJ")
this:AddItem("No (ADC2 Ext. Trigger injected conversion conn. to EXTI15)", "0")
this:AddItem("Yes (ADC2 Ext. Event injected conversion conn. to TIM8_CH4)", "1")
--*/
#define __AFM_REMAP_ADC2_ETRGINJ__ 0

/*--
this:AddWidget("Combobox", "Remap ADC2 ETRGREG")
this:AddItem("No (ADC2 Ext. Trigger regular conversion conn. to EXTI11)", "0")
this:AddItem("Yes (ADC2 Ext. Event regular conversion conn. to TIM8_TRGO)", "1")
--*/
#define __AFM_REMAP_ADC2_ETRGREG__ 0

/*--
this:AddWidget("Combobox", "Serial wire JTAG setup")
this:AddItem("Full SWJ (JTAG-DP + SW-DP): Reset State", "0")
this:AddItem("Full SWJ (JTAG-DP + SW-DP) w/o NJTRST", "1")
this:AddItem("JTAG-DP Disabled and SW-DP Enabled", "2")
this:AddItem("JTAG-DP Disabled and SW-DP Disabled", "3")
--*/
#define __AFM_REMAP_SWJ_CFG__ 0

/*--
if uC.FAMILY == "STM32F10X_CL" then
    this:AddWidget("Combobox", "Remap Ethernet I/O")
    this:AddItem("No (RX_DV-CRS_DV/PA7, RXD0/PC4, RXD1/PC5, RXD2/PB0, RXD3/PB1)", "0")
    this:AddItem("Yes (RX_DV-CRS_DV/PD8, RXD0/PD9, RXD1/PD10, RXD2/PD11, RXD3/PD12)", "1")
end
--*/
#define __AFM_REMAP_ETH__ 1

/*--
if uC.FAMILY == "STM32F10X_CL" then
    this:AddWidget("Combobox", "Ethernet MII/RMII PHY")
    this:AddItem("MII PHY", "0")
    this:AddItem("RMII PHY", "1")
end
--*/
#define __AFM_REMAP_MII_RMII_SEL__ 1

/*--
if uC.FAMILY == "STM32F10X_CL" then
    this:AddWidget("Combobox", "Ethernet PTP PPS")
    this:AddItem("No (PTP_PPS not output on PB5)", "0")
    this:AddItem("Yes (PTP_PPS is output on PB5)", "1")
end
--*/
#define __AFM_REMAP_PTP_PPS__ 0

/*--
if uC.FAMILY == "STM32F10X_CL" then
    this:AddWidget("Combobox", "Remap CAN2")
    this:AddItem("No (CAN2_RX/PB12, CAN2_TX/PB13)", "0")
    this:AddItem("Yes (CAN2_RX/PB5, CAN2_TX/PB6)", "1")
end
--*/
#define __AFM_REMAP_CAN2__ 0

/*--
if uC.FAMILY == "STM32F10X_CL" then
    this:AddWidget("Combobox", "Remap SPI3")
    this:AddItem("No (NSS-WS/PA15, SCK-CK/PB3, MISO/PB4, MOSI-SD/PB5)", "0")
    this:AddItem("Yes (NSS-WS/PA4, SCK-CK/PC10, MISO/PC11, MOSI-SD/PC12)", "1")
end
--*/
#define __AFM_REMAP_SPI3__ 1

/*--
if uC.FAMILY == "STM32F10X_CL" then
    this:AddWidget("Combobox", "Remap TIM2_ITR1")
    this:AddItem("No (TIM2_ITR1 conn. internally to the Ethernet PTP output)", "0")
    this:AddItem("Yes (USB OTG SOF output conn. to TIM2_ITR1)", "1")
end
--*/
#define __AFM_REMAP_TIM2ITR1__ 0

/*--
if uC.FAMILY:match("STM32F10X_.D_VL") then
    this:AddWidget("Combobox", "Remap TIM15")
    this:AddItem("No (CH1/PA2, CH2/PA3)", "0")
    this:AddItem("Yes (CH1/PB14, CH2/PB15)", "1")
end
--*/
#define __AFM_REMAP_TIM15__ 0

/*--
if uC.FAMILY:match("STM32F10X_.D_VL") then
    this:AddWidget("Combobox", "Remap TIM16")
    this:AddItem("No (CH1/PB8)", "0")
    this:AddItem("Yes (CH1/PA6)", "1")
end
--*/
#define __AFM_REMAP_TIM16__ 0

/*--
if uC.FAMILY:match("STM32F10X_.D_VL") then
    this:AddWidget("Combobox", "Remap TIM17")
    this:AddItem("No (CH1/PB9)", "0")
    this:AddItem("Yes (CH1/PA7)", "1")
end
--*/
#define __AFM_REMAP_TIM17__ 0

/*--
if uC.FAMILY:match("STM32F10X_.D_VL") then
    this:AddWidget("Combobox", "Remap CEC")
    this:AddItem("No (CEC/PB8)", "0")
    this:AddItem("Yes (CEC/PB10)", "1")
end
--*/
#define __AFM_REMAP_CEC__ 0

/*--
if uC.FAMILY:match("STM32F10X_.D_VL") then
    this:AddWidget("Combobox", "Remap TIM1 DMA")
    this:AddItem("No (TIM1_CH1 DMA req./DMA1 Ch. 2, TIM1_CH2 DMA req./DMA1 Ch. 3)", "0")
    this:AddItem("Yes (TIM1_CH1 DMA req./DMA1 Ch. 6, TIM1_CH2 DMA req./DMA1 Ch. 6)", "1")
end
--*/
#define __AFM_REMAP_TIM1_DMA__ 0

/*--
if uC.FAMILY == "STM32F10X_HD_VL" then
    this:AddWidget("Combobox", "Remap TIM76 DAC DMA")
    this:AddItem("No (TIM6_DAC1 DMA req./DMA2 Ch. 3, TIM7_DAC2 DMA req./DMA2 Ch. 4)", "0")
    this:AddItem("Yes (TIM6_DAC1 DMA req./DMA1 Ch. 3, TIM7_DAC2 DMA req./DMA1 Ch. 4)", "1")
end
--*/
#define __AFM_REMAP_TIM76_DAC_DMA__ 0

/*--
if uC.FAMILY == "STM32F10X_HD_VL" then
    this:AddWidget("Combobox", "Remap TIM12")
    this:AddItem("No (CH1/PC4, CH2/PC5)", "0")
    this:AddItem("Yes (CH1/PB12, CH2/PB13)", "1")
end
--*/
#define __AFM_REMAP_TIM12__ 0

/*--
if uC.FAMILY == "STM32F10X_HD_VL" then
    this:AddWidget("Combobox", "Remap miscellaneous")
    this:AddItem("No (DMA2_CH5 IRQ is mapped with DMA2_CH4 at pos. 59, TIM5_TRGO event is selected as DAC Trigger 3, TIM5 triggers TIM1/3)", "0")
    this:AddItem("Yes (DMA2_CH5 IRQ is mapped separately at position 60, TIM15_TRGO event is selected as DAC Trigger 3, TIM15 triggers TIM1/3)", "1")
end
--*/
#define __AFM_REMAP_MISC__ 0

/*--
if uC.FAMILY:match("STM32F10X_[LMHX][DL]$") then
    this:AddWidget("Combobox", "Remap TIM9")
    this:AddItem("No (TIM9_CH1/PA2 and TIM9_CH2/PA3)", "0")
    this:AddItem("Yes (TIM9_CH1/PE5 and TIM9_CH2/PE6)", "1")
end
--*/
#define __AFM_REMAP_TIM9__ 0

/*--
if uC.FAMILY:match("STM32F10X_[LMHX][DL]$") then
    this:AddWidget("Combobox", "Remap TIM10")
    this:AddItem("No (TIM10_CH1/PB8)", "0")
    this:AddItem("Yes (TIM10_CH1/PF6)", "1")
end
--*/
#define __AFM_REMAP_TIM10__ 0

/*--
if uC.FAMILY:match("STM32F10X_[LMHX][DL]$") then
    this:AddWidget("Combobox", "Remap TIM11")
    this:AddItem("No (TIM11_CH1/PB9)", "0")
    this:AddItem("Yes (TIM11_CH1/PF7)", "1")
end
--*/
#define __AFM_REMAP_TIM11__ 0

/*--
if uC.FAMILY:match("STM32F10X_[LMHX][DL]$") or uC.FAMILY == "STM32F10X_HD_VL" then
    this:AddWidget("Combobox", "Remap TIM13")
    this:AddItem("No (TIM13_CH1/PC8 [VL] or PA6)", "0")
    this:AddItem("Yes (TIM13_CH1/PB0 [VL] or PF8)", "1")
end
--*/
#define __AFM_REMAP_TIM13__ 0

/*--
if uC.FAMILY:match("STM32F10X_[LMHX][DL]$") or uC.FAMILY == "STM32F10X_HD_VL" then
    this:AddWidget("Combobox", "Remap TIM14")
    this:AddItem("No (TIM14_CH1/PC9 [VL] or PA7)", "0")
    this:AddItem("Yes (TIM14_CH1/PB1 [VL] or PF9)", "1")
end
--*/
#define __AFM_REMAP_TIM14__ 0

/*--
if uC.FAMILY:match("STM32F10X_[LMHX][DL]$") or uC.FAMILY == "STM32F10X_HD_VL" then
    this:AddWidget("Combobox", "NADV connect")
    this:AddItem("The NADV signal is connected to the output", "0")
    this:AddItem("The NADV signal is NOT connected", "1")
end
--*/
#define __AFM_REMAP_FSMC_NADV__ 0



/*--
this:AddExtraWidget("Label", "LabelEXTI", "\nEXTI ports", -1, "bold")
this:AddExtraWidget("Void", "VoidEXTI")
++*/
/*--
this:AddWidget("Combobox", "EXTI0 port")
for i = 0, 6 do local c = string.char(65 + i) this:AddItem("P"..c.."0", tostring(i)) end
--*/
#define __AFM_EXTI0_PORT__ 0

/*--
this:AddWidget("Combobox", "EXTI1 port")
for i = 0, 6 do local c = string.char(65 + i) this:AddItem("P"..c.."1", tostring(i)) end
--*/
#define __AFM_EXTI1_PORT__ 0

/*--
this:AddWidget("Combobox", "EXTI2 port")
for i = 0, 6 do local c = string.char(65 + i) this:AddItem("P"..c.."2", tostring(i)) end
--*/
#define __AFM_EXTI2_PORT__ 0

/*--
this:AddWidget("Combobox", "EXTI3 port")
for i = 0, 6 do local c = string.char(65 + i) this:AddItem("P"..c.."3", tostring(i)) end
--*/
#define __AFM_EXTI3_PORT__ 0

/*--
this:AddWidget("Combobox", "EXTI4 port")
for i = 0, 6 do local c = string.char(65 + i) this:AddItem("P"..c.."4", tostring(i)) end
--*/
#define __AFM_EXTI4_PORT__ 0

/*--
this:AddWidget("Combobox", "EXTI5 port")
for i = 0, 6 do local c = string.char(65 + i) this:AddItem("P"..c.."5", tostring(i)) end
--*/
#define __AFM_EXTI5_PORT__ 0

/*--
this:AddWidget("Combobox", "EXTI6 port")
for i = 0, 6 do local c = string.char(65 + i) this:AddItem("P"..c.."6", tostring(i)) end
--*/
#define __AFM_EXTI6_PORT__ 0

/*--
this:AddWidget("Combobox", "EXTI7 port")
for i = 0, 6 do local c = string.char(65 + i) this:AddItem("P"..c.."7", tostring(i)) end
--*/
#define __AFM_EXTI7_PORT__ 0

/*--
this:AddWidget("Combobox", "EXTI8 port")
for i = 0, 6 do local c = string.char(65 + i) this:AddItem("P"..c.."8", tostring(i)) end
--*/
#define __AFM_EXTI8_PORT__ 0

/*--
this:AddWidget("Combobox", "EXTI9 port")
for i = 0, 6 do local c = string.char(65 + i) this:AddItem("P"..c.."9", tostring(i)) end
--*/
#define __AFM_EXTI9_PORT__ 0

/*--
this:AddWidget("Combobox", "EXTI10 port")
for i = 0, 6 do local c = string.char(65 + i) this:AddItem("P"..c.."10", tostring(i)) end
--*/
#define __AFM_EXTI10_PORT__ 0

/*--
this:AddWidget("Combobox", "EXTI11 port")
for i = 0, 6 do local c = string.char(65 + i) this:AddItem("P"..c.."11", tostring(i)) end
--*/
#define __AFM_EXTI11_PORT__ 0

/*--
this:AddWidget("Combobox", "EXTI12 port")
for i = 0, 6 do local c = string.char(65 + i) this:AddItem("P"..c.."12", tostring(i)) end
--*/
#define __AFM_EXTI12_PORT__ 0

/*--
this:AddWidget("Combobox", "EXTI13 port")
for i = 0, 6 do local c = string.char(65 + i) this:AddItem("P"..c.."13", tostring(i)) end
--*/
#define __AFM_EXTI13_PORT__ 0

/*--
this:AddWidget("Combobox", "EXTI14 port")
for i = 0, 6 do local c = string.char(65 + i) this:AddItem("P"..c.."14", tostring(i)) end
--*/
#define __AFM_EXTI14_PORT__ 0

/*--
this:AddWidget("Combobox", "EXTI15 port")
for i = 0, 6 do local c = string.char(65 + i) this:AddItem("P"..c.."15", tostring(i)) end
--*/
#define __AFM_EXTI15_PORT__ 0

#endif /* _AFIO_FLAGS_H_ */
/*==============================================================================
  End of file
==============================================================================*/
