/**************************************************************************//**
 * @file efr32xx.h
 * @brief CMSIS Cortex-M Peripheral Access Layer for Silicon Laboratories
 *        microcontroller devices
 *
 * This is a convenience header file for defining the part number on the
 * build command line, instead of specifying the part specific header file.
 *
 * @verbatim
 * Example: Add "-DEFM32G890F128" to your build options, to define part
 *          Add "#include "em_device.h" to your source files
 *
 *
 * @endverbatim
 * @version 5.1.2
 ******************************************************************************
 * @section License
 * <b>Copyright 2017 Silicon Laboratories, Inc. http://www.silabs.com</b>
 ******************************************************************************
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software.@n
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.@n
 * 3. This notice may not be removed or altered from any source distribution.
 *
 * DISCLAIMER OF WARRANTY/LIMITATION OF REMEDIES: Silicon Laboratories, Inc.
 * has no obligation to support this Software. Silicon Laboratories, Inc. is
 * providing the Software "AS IS", with no express or implied warranties of any
 * kind, including, but not limited to, any implied warranties of
 * merchantability or fitness for any particular purpose or warranties against
 * infringement of any proprietary rights of a third party.
 *
 * Silicon Laboratories, Inc. will not be liable for any consequential,
 * incidental, or special damages, or any other relief, or for any claim by
 * any third party, arising from your use of this Software.
 *
 *****************************************************************************/

#ifndef EFR32XX_H
#define EFR32XX_H

#include "config.h"

#if defined(EFR32MG1B131F256GM32)
#include "efr32mg1b131f256gm32.h"

#elif defined(EFR32MG1B131F256GM48)
#include "EFR32MG1B/efr32mg1b131f256gm48.h"

#elif defined(EFR32MG1B132F256GJ43)
#include "EFR32MG1B/efr32mg1b132f256gj43.h"

#elif defined(EFR32MG1B132F256GM32)
#include "EFR32MG1B/efr32mg1b132f256gm32.h"

#elif defined(EFR32MG1B132F256GM48)
#include "EFR32MG1B/efr32mg1b132f256gm48.h"

#elif defined(EFR32MG1B231F256GM32)
#include "EFR32MG1B/efr32mg1b231f256gm32.h"

#elif defined(EFR32MG1B231F256GM48)
#include "EFR32MG1B/efr32mg1b231f256gm48.h"

#elif defined(EFR32MG1B232F256GJ43)
#include "EFR32MG1B/efr32mg1b232f256gj43.h"

#elif defined(EFR32MG1B232F256GM32)
#include "EFR32MG1B/efr32mg1b232f256gm32.h"

#elif defined(EFR32MG1B232F256GM48)
#include "EFR32MG1B/efr32mg1b232f256gm48.h"

#elif defined(EFR32MG1B232F256IM48)
#include "EFR32MG1B/efr32mg1b232f256im48.h"

#elif defined(EFR32MG1B632F256GM32)
#include "EFR32MG1B/efr32mg1b632f256gm32.h"

#elif defined(EFR32MG1B632F256IM32)
#include "EFR32MG1B/efr32mg1b632f256im32.h"

#elif defined(EFR32MG1B732F256GM32)
#include "EFR32MG1B/efr32mg1b732f256gm32.h"

#elif defined(EFR32MG1B732F256IM32)
#include "EFR32MG1B/efr32mg1b732f256im32.h"

#elif defined(EFR32MG1P131F256GM48)
#include "EFR32MG1P/efr32mg1p131f256gm48.h"

#elif defined(EFR32MG1P132F256GJ43)
#include "EFR32MG1P/efr32mg1p132f256gj43.h"

#elif defined(EFR32MG1P132F256GM32)
#include "EFR32MG1P/efr32mg1p132f256gm32.h"

#elif defined(EFR32MG1P132F256GM48)
#include "EFR32MG1P/efr32mg1p132f256gm48.h"

#elif defined(EFR32MG1P132F256IM32)
#include "EFR32MG1P/efr32mg1p132f256im32.h"

#elif defined(EFR32MG1P133F256GM48)
#include "EFR32MG1P/efr32mg1p133f256gm48.h"

#elif defined(EFR32MG1P231F256GM48)
#include "EFR32MG1P/efr32mg1p231f256gm48.h"

#elif defined(EFR32MG1P232F256GJ43)
#include "EFR32MG1P/efr32mg1p232f256gj43.h"

#elif defined(EFR32MG1P232F256GM32)
#include "EFR32MG1P/efr32mg1p232f256gm32.h"

#elif defined(EFR32MG1P232F256GM48)
#include "EFR32MG1P/efr32mg1p232f256gm48.h"

#elif defined(EFR32MG1P233F256GM48)
#include "EFR32MG1P/efr32mg1p233f256gm48.h"

#elif defined(EFR32MG1P632F256GM32)
#include "EFR32MG1P/efr32mg1p632f256gm32.h"

#elif defined(EFR32MG1P632F256IM32)
#include "EFR32MG1P/efr32mg1p632f256im32.h"

#elif defined(EFR32MG1P732F256GM32)
#include "EFR32MG1P/efr32mg1p732f256gm32.h"

#elif defined(EFR32MG1P732F256IM32)
#include "EFR32MG1P/efr32mg1p732f256im32.h"

#elif defined(EFR32MG1V131F256GM32)
#include "EFR32MG1V/efr32mg1v131f256gm32.h"

#elif defined(EFR32MG1V131F256GM48)
#include "EFR32MG1V/efr32mg1v131f256gm48.h"

#elif defined(EFR32MG1V132F256GM32)
#include "EFR32MG1V/efr32mg1v132f256gm32.h"

#elif defined(EFR32MG1V132F256GM48)
#include "EFR32MG1V/efr32mg1v132f256gm48.h"

#elif defined(EFR32MG12P132F1024GL125)
#include "EFR32MG12P/efr32mg12p132f1024gl125.h"

#elif defined(EFR32MG12P132F1024GM48)
#include "EFR32MG12P/efr32mg12p132f1024gm48.h"

#elif defined(EFR32MG12P232F1024GL125)
#include "EFR32MG12P/efr32mg12p232f1024gl125.h"

#elif defined(EFR32MG12P232F1024GM48)
#include "EFR32MG12P/efr32mg12p232f1024gm48.h"

#elif defined(EFR32MG12P332F1024GL125)
#include "EFR32MG12P/efr32mg12p332f1024gl125.h"

#elif defined(EFR32MG12P332F1024GM48)
#include "EFR32MG12P/efr32mg12p332f1024gm48.h"

#elif defined(EFR32MG12P432F1024GL125)
#include "EFR32MG12P/efr32mg12p432f1024gl125.h"

#elif defined(EFR32MG12P432F1024GM48)
#include "EFR32MG12P/efr32mg12p432f1024gm48.h"

#elif defined(EFR32MG12P433F1024GL125)
#include "EFR32MG12P/efr32mg12p433f1024gl125.h"

#elif defined(EFR32MG12P433F1024GM48)
#include "EFR32MG12P/efr32mg12p433f1024gm48.h"

#elif defined(EFR32MG13P632F512GM32)
#include "EFR32MG13P/efr32mg13p632f512gm32.h"

#elif defined(EFR32MG13P632F512GM48)
#include "EFR32MG13P/efr32mg13p632f512gm48.h"

#elif defined(EFR32MG13P732F512GM32)
#include "EFR32MG13P/efr32mg13p732f512gm32.h"

#elif defined(EFR32MG13P732F512GM48)
#include "EFR32MG13P/efr32mg13p732f512gm48.h"

#elif defined(EFR32MG13P733F512GM48)
#include "EFR32MG13P/efr32mg13p733f512gm48.h"

#elif defined(EFR32MG13P832F512GM48)
#include "EFR32MG13P/efr32mg13p832f512gm48.h"

#elif defined(EFR32MG13P832F512IM48)
#include "EFR32MG13P/efr32mg13p832f512im48.h"

#elif defined(EFR32MG13P932F512GM48)
#include "EFR32MG13P/efr32mg13p932f512gm48.h"

#elif defined(EFR32MG13P932F512IM48)
#include "EFR32MG13P/efr32mg13p932f512im48.h"

#else
#error "PART NUMBER undefined"
#endif
#endif /* EFR32XX_H */
