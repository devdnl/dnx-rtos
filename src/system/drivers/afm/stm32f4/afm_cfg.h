/*=========================================================================*//**
@file    afm_cfg.h

@author  Daniel Zorychta

@brief   This driver support AFM.

@note    Copyright (C) 2014  Daniel Zorychta <daniel.zorychta@gmail.com>

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

#ifndef _AFM_CFG_H_
#define _AFM_CFG_H_

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Include files
==============================================================================*/

/*==============================================================================
  Exported symbolic constants/macros
==============================================================================*/
/**
 * FMC swap
 */
#define _AFM_SWP_FMC                    __AFM_SWP_FMC__

/**
 * Flash bank mode
 */
#define _AFM_FB_MODE                    __AFM_FB_MODE__

/**
 * Memory map mode
 */
#define _AFM_MEM_MODE_AUTO              (7 << 0)
#define _AFM_MEM_MODE                   __AFM_MEM_MODE__

/**
 * Ethernet interface selection
 */
#define _AFM_MII_RMII_SEL               __AFM_MII_RMII_SEL__

/*==============================================================================
  Exported types, enums definitions
==============================================================================*/

/*==============================================================================
  Exported object declarations
==============================================================================*/

/*==============================================================================
  Exported function prototypes
==============================================================================*/

#ifdef __cplusplus
}
#endif

#endif /* _AFM_CFG_H_ */
/*==============================================================================
  End of file
==============================================================================*/
