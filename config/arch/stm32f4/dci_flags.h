/*==============================================================================
File     dcmi_flags.h

Author   Daniel Zorychta

Brief    Digital Camera Interface Driver

         Copyright (C) 2017 Daniel Zorychta <daniel.zorychta>

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


==============================================================================*/

/*
 * NOTE: All flags defined as: __FLAG_NAME__ (with doubled underscore as prefix
 *       and suffix) are exported to the single configuration file
 *       (by using Configtool) when entire project configuration is exported.
 *       All other flag definitions and statements are ignored.
 */

#ifndef _DCI_FLAGS_H_
#define _DCI_FLAGS_H_

/*--
this:SetLayout("TitledGridBack", 2, "Home > Microcontroller > DCI",
               function() this:LoadFile("arch/arch_flags.h") end)
++*/

/*--
this:AddExtraWidget("Label", "LabelGC", "\nGeneral configuration", -1, "bold")
this:AddExtraWidget("Void", "VoidGC")
++*/
/*--
this:AddWidget("Spinbox", 1, 2048, "Camera resolution X")
--*/
#define __DCI_CAM_RES_X__ 800

/*--
this:AddWidget("Spinbox", 1, 2048, "Camera resolution Y")
--*/
#define __DCI_CAM_RES_Y__ 600

/*--
this:AddWidget("Combobox", "Bytes per pixel (color)")
this:AddItem("1 Byte",  "1")
this:AddItem("2 Bytes", "2")
this:AddItem("3 Bytes", "3")
this:AddItem("4 Bytes", "4")
--*/
#define __DCI_BYTES_PER_PIXEL__ 2

/*--
this:AddWidget("Combobox", "Extended data mode")
this:AddItem("8-bit data on every pixel clock", "(0 << 10)")
this:AddItem("10-bit data on every pixel clock", "(1 << 10)")
this:AddItem("12-bit data on every pixel clock", "(2 << 10)")
this:AddItem("14-bit data on every pixel clock", "(3 << 10)")
--*/
#define __DCI_EDM__ (0 << 10)

/* NOTE: THIS OPTION DISABLED BECAUSE CONTINOUS FRAME RATE NOT SUPPORTED YET
this:AddWidget("Combobox", "Frame capture rate")
this:AddItem("All frames are captured", "(0 << 8)")
this:AddItem("Every alternate frame captured (50% bandwidth reduction)", "(1 << 8)")
this:AddItem("One frame in 4 frames captured (75% bandwidth reduction)", "(2 << 8)")
this:SetToolTip("They are meaningful only in Continuous grab mode.")
--*/
#define __DCI_FCRC__ (0 << 8)

/*--
this:AddWidget("Combobox", "Vertical synchronization polarity")
this:AddItem("VSYNC active low", "(0 << 7)")
this:AddItem("VSYNC active high", "(1 << 7)")
--*/
#define __DCI_VSPOL__ (0 << 7)

/*--
this:AddWidget("Combobox", "Horizontal synchronization polarity")
this:AddItem("HSYNC active low", "(0 << 6)")
this:AddItem("HSYNC active high", "(1 << 6)")
--*/
#define __DCI_HSPOL__ (0 << 6)

/*--
this:AddWidget("Combobox", "Pixel clock polarity")
this:AddItem("Falling edge active", "(0 << 5)")
this:AddItem("Rising edge active", "(1 << 5)")
--*/
#define __DCI_PCKPOL__ (1 << 5)

/*--
this:AddWidget("Combobox", "JPEG format")
this:AddItem("Uncompressed video format", "(0 << 3)")
this:AddItem("JPEG data transfer", "(1 << 3)")
this:SetToolTip("JPEG data transfer: crop and embedded sync. cannot be used.")
--*/
#define __DCI_JPEG__ (0 << 3)


/*--
this:AddExtraWidget("Label", "LabelSM", "\nSynchronization mode", -1, "bold")
this:AddExtraWidget("Void", "VoidSM")
++*/
/*--
this:AddWidget("Combobox", "Embedded synchronization select")
this:AddItem("Hardware sync. data capture (HSYNC/VSYNC signals)", "(0 << 4)")
this:AddItem("Synchronization codes embedded in data flow", "(1 << 4)")
this:SetToolTip("Valid only for 8-bit parallel data.")
--*/
#define __DCI_ESS__ (0 << 4)

/*--
this:AddWidget("Editline", false, "Frame end delimiter code (FEC)")
this:SetToolTip("This byte specifies the code of the frame end delimiter.\n"..
                "The code consists of 4 bytes in the form of 0xFF, 0x00, 0x00, FEC.\n"..
                "If FEC is programmed to 0xFF, all the unused codes (0xFF0000XY) are\n"..
                "interpreted as frame end delimiters.")
--*/
#define __DCI_FEC__ 0xFF

/*--
this:AddWidget("Editline", false, "Frame end delimiter unmask (FEU)")
this:SetToolTip("This byte specifies the mask to be applied to the code of the frame end delimiter.\n"..
                "0: bit masked, 1: bit compared.")
--*/
#define __DCI_FEU__ 0xFF

/*--
this:AddWidget("Editline", false, "Line end delimiter code (LEC)")
this:SetToolTip("This byte specifies the code of the line end delimiter.\n"..
                "The code consists of 4 bytes in the form of 0xFF, 0x00, 0x00, LEC.")
--*/
#define __DCI_LEC__ 0xFF

/*--
this:AddWidget("Editline", false, "Line end delimiter unmask (LEU)")
this:SetToolTip("This byte specifies the mask to be applied to the code of the frame end delimiter.\n"..
                "0: bit masked, 1: bit compared.")
--*/
#define __DCI_LEU__ 0xFF

/*--
this:AddWidget("Editline", false, "Line start delimiter code (LSC)")
this:SetToolTip("This byte specifies the code of the line end delimiter.\n"..
                "The code consists of 4 bytes in the form of 0xFF, 0x00, 0x00, LSC.")
--*/
#define __DCI_LSC__ 0xFF

/*--
this:AddWidget("Editline", false, "Line start delimiter unmask (LSU)")
this:SetToolTip("This byte specifies the mask to be applied to the code of the frame end delimiter.\n"..
                "0: bit masked, 1: bit compared.")
--*/
#define __DCI_LSU__ 0xFF

/*--
this:AddWidget("Editline", false, "Frame start delimiter code (FSC)")
this:SetToolTip("This byte specifies the code of the line end delimiter.\n"..
                "The code consists of 4 bytes in the form of 0xFF, 0x00, 0x00, FSC.")
--*/
#define __DCI_FSC__ 0xFF

/*--
this:AddWidget("Editline", false, "Frame start delimiter unmask (FSU)")
this:SetToolTip("This byte specifies the mask to be applied to the code of the frame end delimiter.\n"..
                "0: bit masked, 1: bit compared.")
--*/
#define __DCI_FSU__ 0xFF


/*--
this:AddExtraWidget("Label", "LabelCF", "\nCrop feature", -1, "bold")
this:AddExtraWidget("Void", "VoidCF")
++*/
/*--
this:AddWidget("Combobox", "Crop feature")
this:AddItem("The full image is captured", "(0 << 2)")
this:AddItem("Only the data inside window will be captured", "(1 << 2)")
--*/
#define __DCI_CROP__ (0 << 2)

/*--
this:AddWidget("Spinbox", 0, 2047, "Crop start X")
--*/
#define __DCI_CROP_START_X__ 0

/*--
this:AddWidget("Spinbox", 0, 2047, "Crop start Y")
--*/
#define __DCI_CROP_START_Y__ 0

/*--
this:AddWidget("Spinbox", 1, 2048, "Crop height")
--*/
#define __DCI_CROP_HEIGHT__ 1

/*--
this:AddWidget("Spinbox", 1, 2048, "Crop width")
--*/
#define __DCI_CROP_WIDTH__ 1


/*--
this:AddExtraWidget("Label", "LabelIRQ", "\nIRQ configuration", -1, "bold")
this:AddExtraWidget("Void", "VoidIRQ")
++*/
/*--
this:AddWidget("Combobox", "IRQ priority")
uC.AddPriorityItems(this, false)
--*/
#define __DCI_IRQ_PRIORITY__ 12


#endif /* _DCI_FLAGS_H_ */
/*==============================================================================
  End of file
==============================================================================*/
