/*==============================================================================
File    snd_flags.h

Author  Daniel Zorychta

Brief   Sound interface driver

        Copyright (C) 2020 Daniel Zorychta <daniel.zorychta@gmail.com>

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

==============================================================================*/

/*
 * NOTE: All flags defined as: __FLAG_NAME__ (with doubled underscore as prefix
 *       and suffix) are exported to the single configuration file
 *       (by using Configtool) when entire project configuration is exported.
 *       All other flag definitions and statements are ignored.
 */

#ifndef _SND_FLAGS_H_
#define _SND_FLAGS_H_

/*--
this:SetLayout("TitledGridBack", 2, "Home > Microcontroller > SND (SAI)",
               function() this:LoadFile("arch/arch_flags.h") end)

this.PortExist = function(this, devNo)
    local SAI_PER = {}
    SAI_PER["STM32F401CBxx"] = 0
    SAI_PER["STM32F401CCxx"] = 0
    SAI_PER["STM32F401CDxx"] = 0
    SAI_PER["STM32F401CExx"] = 0
    SAI_PER["STM32F401RBxx"] = 0
    SAI_PER["STM32F401RCxx"] = 0
    SAI_PER["STM32F401RDxx"] = 0
    SAI_PER["STM32F401RExx"] = 0
    SAI_PER["STM32F401VBxx"] = 0
    SAI_PER["STM32F401VCxx"] = 0
    SAI_PER["STM32F401VDxx"] = 0
    SAI_PER["STM32F401VExx"] = 0
    SAI_PER["STM32F405OExx"] = 0
    SAI_PER["STM32F405OGxx"] = 0
    SAI_PER["STM32F405RGxx"] = 0
    SAI_PER["STM32F405VGxx"] = 0
    SAI_PER["STM32F405ZGxx"] = 0
    SAI_PER["STM32F407IExx"] = 0
    SAI_PER["STM32F407IGxx"] = 0
    SAI_PER["STM32F407VExx"] = 0
    SAI_PER["STM32F407VGxx"] = 0
    SAI_PER["STM32F407ZExx"] = 0
    SAI_PER["STM32F407ZGxx"] = 0
    SAI_PER["STM32F410C8xx"] = 0
    SAI_PER["STM32F410CBxx"] = 0
    SAI_PER["STM32F410R8xx"] = 0
    SAI_PER["STM32F410RBxx"] = 0
    SAI_PER["STM32F410T8xx"] = 0
    SAI_PER["STM32F410TBxx"] = 0
    SAI_PER["STM32F411CCxx"] = 0
    SAI_PER["STM32F411CExx"] = 0
    SAI_PER["STM32F411RCxx"] = 0
    SAI_PER["STM32F411RExx"] = 0
    SAI_PER["STM32F411VCxx"] = 0
    SAI_PER["STM32F411VExx"] = 0
    SAI_PER["STM32F412CExx"] = 0
    SAI_PER["STM32F412CGxx"] = 0
    SAI_PER["STM32F412RExx"] = 0
    SAI_PER["STM32F412RGxx"] = 0
    SAI_PER["STM32F412VExx"] = 0
    SAI_PER["STM32F412VGxx"] = 0
    SAI_PER["STM32F412ZExx"] = 0
    SAI_PER["STM32F412ZGxx"] = 0
    SAI_PER["STM32F413CGxx"] = 1
    SAI_PER["STM32F413CHxx"] = 1
    SAI_PER["STM32F413MGxx"] = 1
    SAI_PER["STM32F413MHxx"] = 1
    SAI_PER["STM32F413RGxx"] = 1
    SAI_PER["STM32F413RHxx"] = 1
    SAI_PER["STM32F413VGxx"] = 1
    SAI_PER["STM32F413VHxx"] = 1
    SAI_PER["STM32F413ZGxx"] = 1
    SAI_PER["STM32F413ZHxx"] = 1
    SAI_PER["STM32F415OGxx"] = 0
    SAI_PER["STM32F415RGxx"] = 0
    SAI_PER["STM32F415VGxx"] = 0
    SAI_PER["STM32F415ZGxx"] = 0
    SAI_PER["STM32F417IExx"] = 0
    SAI_PER["STM32F417IGxx"] = 0
    SAI_PER["STM32F417VExx"] = 0
    SAI_PER["STM32F417VGxx"] = 0
    SAI_PER["STM32F417ZExx"] = 0
    SAI_PER["STM32F417ZGxx"] = 0
    SAI_PER["STM32F423CHxx"] = 1
    SAI_PER["STM32F423MHxx"] = 1
    SAI_PER["STM32F423RHxx"] = 1
    SAI_PER["STM32F423VHxx"] = 1
    SAI_PER["STM32F423ZHxx"] = 1
    SAI_PER["STM32F427AGxx"] = 1
    SAI_PER["STM32F427AIxx"] = 1
    SAI_PER["STM32F427IGxx"] = 1
    SAI_PER["STM32F427IIxx"] = 1
    SAI_PER["STM32F427VGxx"] = 1
    SAI_PER["STM32F427VIxx"] = 1
    SAI_PER["STM32F427ZGxx"] = 1
    SAI_PER["STM32F427ZIxx"] = 1
    SAI_PER["STM32F429AGxx"] = 1
    SAI_PER["STM32F429AIxx"] = 1
    SAI_PER["STM32F429BExx"] = 1
    SAI_PER["STM32F429BGxx"] = 1
    SAI_PER["STM32F429BIxx"] = 1
    SAI_PER["STM32F429IExx"] = 1
    SAI_PER["STM32F429IGxx"] = 1
    SAI_PER["STM32F429IIxx"] = 1
    SAI_PER["STM32F429NExx"] = 1
    SAI_PER["STM32F429NGxx"] = 1
    SAI_PER["STM32F429NIxx"] = 1
    SAI_PER["STM32F429VExx"] = 1
    SAI_PER["STM32F429VGxx"] = 1
    SAI_PER["STM32F429VIxx"] = 1
    SAI_PER["STM32F429ZExx"] = 1
    SAI_PER["STM32F429ZGxx"] = 1
    SAI_PER["STM32F429ZIxx"] = 1
    SAI_PER["STM32F437AIxx"] = 1
    SAI_PER["STM32F437IGxx"] = 1
    SAI_PER["STM32F437IIxx"] = 1
    SAI_PER["STM32F437VGxx"] = 1
    SAI_PER["STM32F437VIxx"] = 1
    SAI_PER["STM32F437ZGxx"] = 1
    SAI_PER["STM32F437ZIxx"] = 1
    SAI_PER["STM32F439AIxx"] = 1
    SAI_PER["STM32F439BGxx"] = 1
    SAI_PER["STM32F439BIxx"] = 1
    SAI_PER["STM32F439IGxx"] = 1
    SAI_PER["STM32F439IIxx"] = 1
    SAI_PER["STM32F439NGxx"] = 1
    SAI_PER["STM32F439NIxx"] = 1
    SAI_PER["STM32F439VGxx"] = 1
    SAI_PER["STM32F439VIxx"] = 1
    SAI_PER["STM32F439ZGxx"] = 1
    SAI_PER["STM32F439ZIxx"] = 1
    SAI_PER["STM32F446MCxx"] = 2
    SAI_PER["STM32F446MExx"] = 2
    SAI_PER["STM32F446RCxx"] = 2
    SAI_PER["STM32F446RExx"] = 2
    SAI_PER["STM32F446VCxx"] = 2
    SAI_PER["STM32F446VExx"] = 2
    SAI_PER["STM32F446ZCxx"] = 2
    SAI_PER["STM32F446ZExx"] = 2
    SAI_PER["STM32F469AExx"] = 1
    SAI_PER["STM32F469AGxx"] = 1
    SAI_PER["STM32F469AIxx"] = 1
    SAI_PER["STM32F469BExx"] = 1
    SAI_PER["STM32F469BGxx"] = 1
    SAI_PER["STM32F469BIxx"] = 1
    SAI_PER["STM32F469IExx"] = 1
    SAI_PER["STM32F469IGxx"] = 1
    SAI_PER["STM32F469IIxx"] = 1
    SAI_PER["STM32F469NExx"] = 1
    SAI_PER["STM32F469NGxx"] = 1
    SAI_PER["STM32F469NIxx"] = 1
    SAI_PER["STM32F469VExx"] = 1
    SAI_PER["STM32F469VGxx"] = 1
    SAI_PER["STM32F469VIxx"] = 1
    SAI_PER["STM32F469ZExx"] = 1
    SAI_PER["STM32F469ZGxx"] = 1
    SAI_PER["STM32F469ZIxx"] = 1
    SAI_PER["STM32F479AGxx"] = 1
    SAI_PER["STM32F479AIxx"] = 1
    SAI_PER["STM32F479BGxx"] = 1
    SAI_PER["STM32F479BIxx"] = 1
    SAI_PER["STM32F479IGxx"] = 1
    SAI_PER["STM32F479IIxx"] = 1
    SAI_PER["STM32F479NGxx"] = 1
    SAI_PER["STM32F479NIxx"] = 1
    SAI_PER["STM32F479VGxx"] = 1
    SAI_PER["STM32F479VIxx"] = 1
    SAI_PER["STM32F479ZGxx"] = 1
    SAI_PER["STM32F479ZIxx"] = 1

    return SAI_PER[uC.NAME] >= devNo
end
++*/

//==============================================================================
// SAI1
//==============================================================================
/*--
if this:PortExist(1) then
        this:AddExtraWidget("Label", "LABEL_SAI1", "SAI1", -1, "bold")
        this:AddExtraWidget("Void", "VOIDSAI1")
end
++*/
//------------------------------------------------------------------------------
// SAI1 BLOCK A
//------------------------------------------------------------------------------
/*--
if this:PortExist(1) then
        this:AddExtraWidget("Label", "LABEL_SAI1_BLOCK_A", "Block A", -1, "bold")
        this:AddExtraWidget("Void", "VOIDSAI1BlockA")
end
++*/

/*--
if this:PortExist(1) then
    this:AddWidget("Combobox", "SAI Clock Divider")
    this:AddItem("Enable", "0")
    this:AddItem("Disable (SAI_CK / 1)", "SAI_xCR1_NODIV")
end
--*/
#define __SND_CFG_SAI1_BLKA_NO_DIVIDER__ 0

/*--
if this:PortExist(1) then
    this:AddWidget("Combobox", "SAI Clock strobe")
    this:AddItem("Data valid on FALLING clock edge", "0")
    this:AddItem("Data valid on RISING clock edge", "SAI_xCR1_CKSTR")
end
--*/
#define __SND_CFG_SAI1_BLKA_CKSTR__ SAI_xCR1_CKSTR

/*--
if this:PortExist(1) then
    this:AddWidget("Combobox", "Bit order")
    this:AddItem("MSB first", "0")
    this:AddItem("LSB first", "SAI_xCR1_LSBFIRST")
end
--*/
#define __SND_CFG_SAI1_BLKA_LSBFRIST__ 0

/*--
if this:PortExist(1) then
    this:AddWidget("Combobox", "Data size")
    this:AddItem("8-bit", "2")
    this:AddItem("10-bit", "3")
    this:AddItem("16-bit", "4")
    this:AddItem("20-bit", "5")
    this:AddItem("24-bit", "6")
    this:AddItem("32-bit", "7")
end
--*/
#define __SND_CFG_SAI1_BLKA_DS__ 4

/*--
if this:PortExist(1) then
    this:AddWidget("Combobox", "Protocol")
    this:AddItem("Free protocol", "0")
    this:AddItem("AC'97", "2")
end
--*/
#define __SND_CFG_SAI1_BLKA_PRTCFG__ 0

/*--
if this:PortExist(1) then
    this:AddWidget("Combobox", "Audio block mode")
    this:AddItem("Master transmitter", "0")
    this:AddItem("Master received", "1")
    this:AddItem("Slave transmitter", "2")
    this:AddItem("Slave received", "3")
end
--*/
#define __SND_CFG_SAI1_BLKA_MODE__ 0

/*--
if this:PortExist(1) then
    this:AddWidget("Combobox", "Companding mode")
    this:AddItem("No companding algorithm", "0")
    this:AddItem("µ-Law algorithm", "2")
    this:AddItem("A-Law algorithm", "3")
end
--*/
#define __SND_CFG_SAI1_BLKA_COMP__ 0

/*--
if this:PortExist(1) then
    this:AddWidget("Combobox", "Complement bit")
    this:AddItem("1's complement", "0")
    this:AddItem("2's complement", "SAI_xCR2_CPL")
end
--*/
#define __SND_CFG_SAI1_BLKA_CPL__ 0

/*--
if this:PortExist(1) then
    this:AddWidget("Combobox", "Tristate data line")
    this:AddItem("Disable", "0")
    this:AddItem("Enable", "SAI_xCR2_TRIS")
end
--*/
#define __SND_CFG_SAI1_BLKA_TRIS__ 0

/*--
if this:PortExist(1) then
    this:AddWidget("Combobox", "Frame synchronization offset")
    this:AddItem("FS is asserted on the first bit of the slot 0", "0")
    this:AddItem("FS is asserted one bit before the first bit of slot 0", "SAI_xFRCR_FSOFF")
end
--*/
#define __SND_CFG_SAI1_BLKA_FSOFF__ 0

/*--
if this:PortExist(1) then
    this:AddWidget("Combobox", "Frame synchronization polarity")
    this:AddItem("FS is active low (falling edge)", "0")
    this:AddItem("FS is active high (rising edge)", "SAI_xFRCR_FSPOL")
end
--*/
#define __SND_CFG_SAI1_BLKA_FSPOL__ 0

/*--
if this:PortExist(1) then
    this:AddWidget("Combobox", "Frame synchronization definition")
    this:AddItem("FS signal is a start frame signal", "0")
    this:AddItem("FS signal is a start of frame signal + channel side identification", "SAI_xFRCR_FSDEF")
end
--*/
#define __SND_CFG_SAI1_BLKA_FSDEF__ SAI_xFRCR_FSDEF

/*--
if this:PortExist(1) then
    this:AddWidget("Spinbox", 1, 128, "Frame synchronization active level length")
end
--*/
#define __SND_CFG_SAI1_BLKA_FSALL__ 16

/*--
if this:PortExist(1) then
    this:AddWidget("Spinbox", 1, 32, "Frame length (should be even)")
end
--*/
#define __SND_CFG_SAI1_BLKA_FRL__ 32

/*--
if this:PortExist(1) then
    this:AddWidget("Spinbox", 1, 16, "Number of slots in an audio frame")
end
--*/
#define __SND_CFG_SAI1_BLKA_NBSLOT__ 2

/*--
if this:PortExist(1) then
    this:AddWidget("Combobox", "Slot size")
    this:AddItem("The slot size is equivalent to the data size", "0")
    this:AddItem("16-bit", "1")
    this:AddItem("32-bit", "2")
end
--*/
#define __SND_CFG_SAI1_BLKA_SLOTSZ__ 1

/*--
if this:PortExist(1) then
    this:AddWidget("Spinbox", 0, 31, "First bit offset")
end
--*/
#define __SND_CFG_SAI1_BLKA_FBOFF__ 1


//------------------------------------------------------------------------------
// SAI1 BLOCK B
//------------------------------------------------------------------------------
/*--
if this:PortExist(1) then
        this:AddExtraWidget("Label", "LABEL_SAI1_BLOCK_B", "\nBlock B", -1, "bold")
        this:AddExtraWidget("Void", "VOIDSAI1BlockB")
end
++*/

/*--
if this:PortExist(1) then
    this:AddWidget("Combobox", "SAI Clock Divider")
    this:AddItem("Enable", "0")
    this:AddItem("Disable (SAI_CK / 1)", "SAI_xCR1_NODIV")
end
--*/
#define __SND_CFG_SAI1_BLKB_NO_DIVIDER__ 0

/*--
if this:PortExist(1) then
    this:AddWidget("Combobox", "SAI Clock strobe")
    this:AddItem("Data valid on FALLING clock edge", "0")
    this:AddItem("Data valid on RISING clock edge", "SAI_xCR1_CKSTR")
end
--*/
#define __SND_CFG_SAI1_BLKB_CKSTR__ SAI_xCR1_CKSTR

/*--
if this:PortExist(1) then
    this:AddWidget("Combobox", "Bit order")
    this:AddItem("MSB first", "0")
    this:AddItem("LSB first", "SAI_xCR1_LSBFIRST")
end
--*/
#define __SND_CFG_SAI1_BLKB_LSBFRIST__ 0

/*--
if this:PortExist(1) then
    this:AddWidget("Combobox", "Data size")
    this:AddItem("8-bit", "2")
    this:AddItem("10-bit", "3")
    this:AddItem("16-bit", "4")
    this:AddItem("20-bit", "5")
    this:AddItem("24-bit", "6")
    this:AddItem("32-bit", "7")
end
--*/
#define __SND_CFG_SAI1_BLKB_DS__ 4

/*--
if this:PortExist(1) then
    this:AddWidget("Combobox", "Protocol")
    this:AddItem("Free protocol", "0")
    this:AddItem("AC'97", "2")
end
--*/
#define __SND_CFG_SAI1_BLKB_PRTCFG__ 0

/*--
if this:PortExist(1) then
    this:AddWidget("Combobox", "Audio block mode")
    this:AddItem("Master transmitter", "0")
    this:AddItem("Master received", "1")
    this:AddItem("Slave transmitter", "2")
    this:AddItem("Slave received", "3")
end
--*/
#define __SND_CFG_SAI1_BLKB_MODE__ 0

/*--
if this:PortExist(1) then
    this:AddWidget("Combobox", "Companding mode")
    this:AddItem("No companding algorithm", "0")
    this:AddItem("µ-Law algorithm", "2")
    this:AddItem("A-Law algorithm", "3")
end
--*/
#define __SND_CFG_SAI1_BLKB_COMP__ 0

/*--
if this:PortExist(1) then
    this:AddWidget("Combobox", "Complement bit")
    this:AddItem("1's complement", "0")
    this:AddItem("2's complement", "SAI_xCR2_CPL")
end
--*/
#define __SND_CFG_SAI1_BLKB_CPL__ 0

/*--
if this:PortExist(1) then
    this:AddWidget("Combobox", "Tristate data line")
    this:AddItem("Disable", "0")
    this:AddItem("Enable", "SAI_xCR2_TRIS")
end
--*/
#define __SND_CFG_SAI1_BLKB_TRIS__ 0

/*--
if this:PortExist(1) then
    this:AddWidget("Combobox", "Frame synchronization offset")
    this:AddItem("FS is asserted on the first bit of the slot 0", "0")
    this:AddItem("FS is asserted one bit before the first bit of slot 0", "SAI_xFRCR_FSOFF")
end
--*/
#define __SND_CFG_SAI1_BLKB_FSOFF__ 0

/*--
if this:PortExist(1) then
    this:AddWidget("Combobox", "Frame synchronization polarity")
    this:AddItem("FS is active low (falling edge)", "0")
    this:AddItem("FS is active high (rising edge)", "SAI_xFRCR_FSPOL")
end
--*/
#define __SND_CFG_SAI1_BLKB_FSPOL__ 0

/*--
if this:PortExist(1) then
    this:AddWidget("Combobox", "Frame synchronization definition")
    this:AddItem("FS signal is a start frame signal", "0")
    this:AddItem("FS signal is a start of frame signal + channel side identification", "SAI_xFRCR_FSDEF")
end
--*/
#define __SND_CFG_SAI1_BLKB_FSDEF__ SAI_xFRCR_FSDEF

/*--
if this:PortExist(1) then
    this:AddWidget("Spinbox", 1, 128, "Frame synchronization active level length")
end
--*/
#define __SND_CFG_SAI1_BLKB_FSALL__ 16

/*--
if this:PortExist(1) then
    this:AddWidget("Spinbox", 1, 32, "Frame length (should be even)")
end
--*/
#define __SND_CFG_SAI1_BLKB_FRL__ 32

/*--
if this:PortExist(1) then
    this:AddWidget("Spinbox", 1, 16, "Number of slots in an audio frame")
end
--*/
#define __SND_CFG_SAI1_BLKB_NBSLOT__ 2

/*--
if this:PortExist(1) then
    this:AddWidget("Combobox", "Slot size")
    this:AddItem("The slot size is equivalent to the data size", "0")
    this:AddItem("16-bit", "1")
    this:AddItem("32-bit", "2")
end
--*/
#define __SND_CFG_SAI1_BLKB_SLOTSZ__ 1

/*--
if this:PortExist(1) then
    this:AddWidget("Spinbox", 0, 31, "First bit offset")
end
--*/
#define __SND_CFG_SAI1_BLKB_FBOFF__ 0




//==============================================================================
// SAI2
//==============================================================================
/*--
if this:PortExist(2) then
        this:AddExtraWidget("Label", "LABEL_SAI2", "\n\nSAI2", -1, "bold")
        this:AddExtraWidget("Void", "VOIDSAI2")
end
++*/
//------------------------------------------------------------------------------
// SAI2 BLOCK A
//------------------------------------------------------------------------------
/*--
if this:PortExist(2) then
        this:AddExtraWidget("Label", "LABEL_SAI2_BLOCK_A", "Block A", -1, "bold")
        this:AddExtraWidget("Void", "VOIDSAI2BlockA")
end
++*/

/*--
if this:PortExist(2) then
    this:AddWidget("Combobox", "SAI Clock Divider")
    this:AddItem("Enable", "0")
    this:AddItem("Disable (SAI_CK / 1)", "SAI_xCR1_NODIV")
end
--*/
#define __SND_CFG_SAI2_BLKA_NO_DIVIDER__ 0

/*--
if this:PortExist(2) then
    this:AddWidget("Combobox", "SAI Clock strobe")
    this:AddItem("Data valid on FALLING clock edge", "0")
    this:AddItem("Data valid on RISING clock edge", "SAI_xCR1_CKSTR")
end
--*/
#define __SND_CFG_SAI2_BLKA_CKSTR__ SAI_xCR1_CKSTR

/*--
if this:PortExist(2) then
    this:AddWidget("Combobox", "Bit order")
    this:AddItem("MSB first", "0")
    this:AddItem("LSB first", "SAI_xCR1_LSBFIRST")
end
--*/
#define __SND_CFG_SAI2_BLKA_LSBFRIST__ 0

/*--
if this:PortExist(2) then
    this:AddWidget("Combobox", "Data size")
    this:AddItem("8-bit", "2")
    this:AddItem("10-bit", "3")
    this:AddItem("16-bit", "4")
    this:AddItem("20-bit", "5")
    this:AddItem("24-bit", "6")
    this:AddItem("32-bit", "7")
end
--*/
#define __SND_CFG_SAI2_BLKA_DS__ 4

/*--
if this:PortExist(2) then
    this:AddWidget("Combobox", "Protocol")
    this:AddItem("Free protocol", "0")
    this:AddItem("AC'97", "2")
end
--*/
#define __SND_CFG_SAI2_BLKA_PRTCFG__ 0

/*--
if this:PortExist(2) then
    this:AddWidget("Combobox", "Audio block mode")
    this:AddItem("Master transmitter", "0")
    this:AddItem("Master received", "1")
    this:AddItem("Slave transmitter", "2")
    this:AddItem("Slave received", "3")
end
--*/
#define __SND_CFG_SAI2_BLKA_MODE__ 0

/*--
if this:PortExist(2) then
    this:AddWidget("Combobox", "Companding mode")
    this:AddItem("No companding algorithm", "0")
    this:AddItem("µ-Law algorithm", "2")
    this:AddItem("A-Law algorithm", "3")
end
--*/
#define __SND_CFG_SAI2_BLKA_COMP__ 0

/*--
if this:PortExist(2) then
    this:AddWidget("Combobox", "Complement bit")
    this:AddItem("1's complement", "0")
    this:AddItem("2's complement", "SAI_xCR2_CPL")
end
--*/
#define __SND_CFG_SAI2_BLKA_CPL__ 0

/*--
if this:PortExist(2) then
    this:AddWidget("Combobox", "Tristate data line")
    this:AddItem("Disable", "0")
    this:AddItem("Enable", "SAI_xCR2_TRIS")
end
--*/
#define __SND_CFG_SAI2_BLKA_TRIS__ 0

/*--
if this:PortExist(2) then
    this:AddWidget("Combobox", "Frame synchronization offset")
    this:AddItem("FS is asserted on the first bit of the slot 0", "0")
    this:AddItem("FS is asserted one bit before the first bit of slot 0", "SAI_xFRCR_FSOFF")
end
--*/
#define __SND_CFG_SAI2_BLKA_FSOFF__ 0

/*--
if this:PortExist(2) then
    this:AddWidget("Combobox", "Frame synchronization polarity")
    this:AddItem("FS is active low (falling edge)", "0")
    this:AddItem("FS is active high (rising edge)", "SAI_xFRCR_FSPOL")
end
--*/
#define __SND_CFG_SAI2_BLKA_FSPOL__ 0

/*--
if this:PortExist(2) then
    this:AddWidget("Combobox", "Frame synchronization definition")
    this:AddItem("FS signal is a start frame signal", "0")
    this:AddItem("FS signal is a start of frame signal + channel side identification", "SAI_xFRCR_FSDEF")
end
--*/
#define __SND_CFG_SAI2_BLKA_FSDEF__ SAI_xFRCR_FSDEF

/*--
if this:PortExist(2) then
    this:AddWidget("Spinbox", 1, 128, "Frame synchronization active level length")
end
--*/
#define __SND_CFG_SAI2_BLKA_FSALL__ 16

/*--
if this:PortExist(2) then
    this:AddWidget("Spinbox", 1, 32, "Frame length (should be even)")
end
--*/
#define __SND_CFG_SAI2_BLKA_FRL__ 32

/*--
if this:PortExist(2) then
    this:AddWidget("Spinbox", 1, 16, "Number of slots in an audio frame")
end
--*/
#define __SND_CFG_SAI2_BLKA_NBSLOT__ 2

/*--
if this:PortExist(2) then
    this:AddWidget("Combobox", "Slot size")
    this:AddItem("The slot size is equivalent to the data size", "0")
    this:AddItem("16-bit", "1")
    this:AddItem("32-bit", "2")
end
--*/
#define __SND_CFG_SAI2_BLKA_SLOTSZ__ 1

/*--
if this:PortExist(2) then
    this:AddWidget("Spinbox", 0, 31, "First bit offset")
end
--*/
#define __SND_CFG_SAI2_BLKA_FBOFF__ 0


//------------------------------------------------------------------------------
// SAI2 BLOCK B
//------------------------------------------------------------------------------
/*--
if this:PortExist(2) then
        this:AddExtraWidget("Label", "LABEL_SAI2_BLOCK_B", "\nBlock B", -1, "bold")
        this:AddExtraWidget("Void", "VOIDSAI2BlockB")
end
++*/

/*--
if this:PortExist(2) then
    this:AddWidget("Combobox", "SAI Clock Divider")
    this:AddItem("Enable", "0")
    this:AddItem("Disable (SAI_CK / 1)", "SAI_xCR1_NODIV")
end
--*/
#define __SND_CFG_SAI2_BLKB_NO_DIVIDER__ 0

/*--
if this:PortExist(2) then
    this:AddWidget("Combobox", "SAI Clock strobe")
    this:AddItem("Data valid on FALLING clock edge", "0")
    this:AddItem("Data valid on RISING clock edge", "SAI_xCR1_CKSTR")
end
--*/
#define __SND_CFG_SAI2_BLKB_CKSTR__ SAI_xCR1_CKSTR

/*--
if this:PortExist(2) then
    this:AddWidget("Combobox", "Bit order")
    this:AddItem("MSB first", "0")
    this:AddItem("LSB first", "SAI_xCR1_LSBFIRST")
end
--*/
#define __SND_CFG_SAI2_BLKB_LSBFRIST__ 0

/*--
if this:PortExist(2) then
    this:AddWidget("Combobox", "Data size")
    this:AddItem("8-bit", "2")
    this:AddItem("10-bit", "3")
    this:AddItem("16-bit", "4")
    this:AddItem("20-bit", "5")
    this:AddItem("24-bit", "6")
    this:AddItem("32-bit", "7")
end
--*/
#define __SND_CFG_SAI2_BLKB_DS__ 4

/*--
if this:PortExist(2) then
    this:AddWidget("Combobox", "Protocol")
    this:AddItem("Free protocol", "0")
    this:AddItem("AC'97", "2")
end
--*/
#define __SND_CFG_SAI2_BLKB_PRTCFG__ 0

/*--
if this:PortExist(2) then
    this:AddWidget("Combobox", "Audio block mode")
    this:AddItem("Master transmitter", "0")
    this:AddItem("Master received", "1")
    this:AddItem("Slave transmitter", "2")
    this:AddItem("Slave received", "3")
end
--*/
#define __SND_CFG_SAI2_BLKB_MODE__ 0

/*--
if this:PortExist(2) then
    this:AddWidget("Combobox", "Companding mode")
    this:AddItem("No companding algorithm", "0")
    this:AddItem("µ-Law algorithm", "2")
    this:AddItem("A-Law algorithm", "3")
end
--*/
#define __SND_CFG_SAI2_BLKB_COMP__ 0

/*--
if this:PortExist(2) then
    this:AddWidget("Combobox", "Complement bit")
    this:AddItem("1's complement", "0")
    this:AddItem("2's complement", "SAI_xCR2_CPL")
end
--*/
#define __SND_CFG_SAI2_BLKB_CPL__ 0

/*--
if this:PortExist(2) then
    this:AddWidget("Combobox", "Tristate data line")
    this:AddItem("Disable", "0")
    this:AddItem("Enable", "SAI_xCR2_TRIS")
end
--*/
#define __SND_CFG_SAI2_BLKB_TRIS__ 0

/*--
if this:PortExist(2) then
    this:AddWidget("Combobox", "Frame synchronization offset")
    this:AddItem("FS is asserted on the first bit of the slot 0", "0")
    this:AddItem("FS is asserted one bit before the first bit of slot 0", "SAI_xFRCR_FSOFF")
end
--*/
#define __SND_CFG_SAI2_BLKB_FSOFF__ 0

/*--
if this:PortExist(2) then
    this:AddWidget("Combobox", "Frame synchronization polarity")
    this:AddItem("FS is active low (falling edge)", "0")
    this:AddItem("FS is active high (rising edge)", "SAI_xFRCR_FSPOL")
end
--*/
#define __SND_CFG_SAI2_BLKB_FSPOL__ 0

/*--
if this:PortExist(2) then
    this:AddWidget("Combobox", "Frame synchronization definition")
    this:AddItem("FS signal is a start frame signal", "0")
    this:AddItem("FS signal is a start of frame signal + channel side identification", "SAI_xFRCR_FSDEF")
end
--*/
#define __SND_CFG_SAI2_BLKB_FSDEF__ SAI_xFRCR_FSDEF

/*--
if this:PortExist(2) then
    this:AddWidget("Spinbox", 1, 128, "Frame synchronization active level length")
end
--*/
#define __SND_CFG_SAI2_BLKB_FSALL__ 16

/*--
if this:PortExist(2) then
    this:AddWidget("Spinbox", 1, 32, "Frame length (should be even)")
end
--*/
#define __SND_CFG_SAI2_BLKB_FRL__ 32

/*--
if this:PortExist(2) then
    this:AddWidget("Spinbox", 1, 16, "Number of slots in an audio frame")
end
--*/
#define __SND_CFG_SAI2_BLKB_NBSLOT__ 2

/*--
if this:PortExist(2) then
    this:AddWidget("Combobox", "Slot size")
    this:AddItem("The slot size is equivalent to the data size", "0")
    this:AddItem("16-bit", "1")
    this:AddItem("32-bit", "2")
end
--*/
#define __SND_CFG_SAI2_BLKB_SLOTSZ__ 1

/*--
if this:PortExist(2) then
    this:AddWidget("Spinbox", 0, 31, "First bit offset")
end
--*/
#define __SND_CFG_SAI2_BLKB_FBOFF__ 0


#endif /* _SND_FLAGS_H_ */
/*==============================================================================
  End of file
==============================================================================*/
