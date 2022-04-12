/*==============================================================================
File    ltdc_flags.h

Author  Daniel Zorychta

Brief   LCD-TFT Display Controller module

        Copyright (C) 2022 Daniel Zorychta <daniel.zorychta@gmail.com>

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

#ifndef _LTDC_FLAGS_H_
#define _LTDC_FLAGS_H_

/*--
this:SetLayout("TitledGridBack", 2, "Home > Microcontroller > LTDC",
               function() this:LoadFile("arch/arch_flags.h") end)
++*/
/*--
this:AddWidget("Spinbox", 1, 4096, "Active width")
--*/
#define __LTDC_WIDTH__      480

/*--
this:AddWidget("Spinbox", 1, 2048, "Active height")
--*/
#define __LTDC_HEIGHT__     272

/*--
this:AddWidget("Spinbox", 1, 4096, "HSYNC width")
--*/
#define __LTDC_HSYNC__      41

/*--
this:AddWidget("Spinbox", 1, 1024, "Horizontal back porch")
--*/
#define __LTDC_HBP__        13

/*--
this:AddWidget("Spinbox", 1, 1024, "Horizontal front porch")
--*/
#define __LTDC_HFP__        32

/*--
this:AddWidget("Spinbox", 1, 2048, "VSYNC width")
--*/
#define __LTDC_VSYNC__      10

/*--
this:AddWidget("Spinbox", 1, 1024, "Vertical back porch")
--*/
#define __LTDC_VBP__        2

/*--
this:AddWidget("Spinbox", 1, 1024, "Vertical front porch")
--*/
#define __LTDC_VFP__        2

/*--
this:AddWidget("Combobox", "HSYNC polarity")
this:AddItem("Active low", "LTDC_HSPOLARITY_AL")
this:AddItem("Active high", "LTDC_HSPOLARITY_AH")
--*/
#define __LTDC_HSPOLARITY__ LTDC_HSPOLARITY_AL

/*--
this:AddWidget("Combobox", "VSYNC polarity")
this:AddItem("Active low", "LTDC_VSPOLARITY_AL")
this:AddItem("Active high", "LTDC_VSPOLARITY_AH")
--*/
#define __LTDC_VSPOLARITY__ LTDC_VSPOLARITY_AL

/*--
this:AddWidget("Combobox", "DE polarity")
this:AddItem("Active low", "LTDC_DEPOLARITY_AL")
this:AddItem("Active high", "LTDC_DEPOLARITY_AH")
--*/
#define __LTDC_DEPOLARITY__ LTDC_DEPOLARITY_AL

/*--
this:AddWidget("Combobox", "DE polarity")
this:AddItem("Input pixel clock", "LTDC_PCPOLARITY_IPC")
this:AddItem("Inverted input pixel clock", "LTDC_PCPOLARITY_IIPC")
--*/
#define __LTDC_PCPOLARITY__ LTDC_PCPOLARITY_IPC

#endif /* _LTDC_FLAGS_H_ */
/*==============================================================================
  End of file
==============================================================================*/
