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

/**
 * EXTI0 PORT SELECTION: 0-6 for PA-PG
 */
#define _AFM_EXTI0_PORT                 __AFM_EXTI0_PORT__

/**
 * EXTI1 PORT SELECTION: 0-6 for PA-PG
 */
#define _AFM_EXTI1_PORT                 __AFM_EXTI1_PORT__

/**
 * EXTI2 PORT SELECTION: 0-6 for PA-PG
 */
#define _AFM_EXTI2_PORT                 __AFM_EXTI2_PORT__

/**
 * EXTI3 PORT SELECTION: 0-6 for PA-PG
 */
#define _AFM_EXTI3_PORT                 __AFM_EXTI3_PORT__

/**
 * EXTI4 PORT SELECTION: 0-6 for PA-PG
 */
#define _AFM_EXTI4_PORT                 __AFM_EXTI4_PORT__

/**
 * EXTI5 PORT SELECTION: 0-6 for PA-PG
 */
#define _AFM_EXTI5_PORT                 __AFM_EXTI5_PORT__

/**
 * EXTI6 PORT SELECTION: 0-6 for PA-PG
 */
#define _AFM_EXTI6_PORT                 __AFM_EXTI6_PORT__

/**
 * EXTI7 PORT SELECTION: 0-6 for PA-PG
 */
#define _AFM_EXTI7_PORT                 __AFM_EXTI7_PORT__

/**
 * EXTI8 PORT SELECTION: 0-6 for PA-PG
 */
#define _AFM_EXTI8_PORT                 __AFM_EXTI8_PORT__

/**
 * EXTI9 PORT SELECTION: 0-6 for PA-PG
 */
#define _AFM_EXTI9_PORT                 __AFM_EXTI9_PORT__

/**
 * EXTI10 PORT SELECTION: 0-6 for PA-PG
 */
#define _AFM_EXTI10_PORT                __AFM_EXTI10_PORT__

/**
 * EXTI11 PORT SELECTION: 0-6 for PA-PG
 */
#define _AFM_EXTI11_PORT                __AFM_EXTI11_PORT__

/**
 * EXTI12 PORT SELECTION: 0-6 for PA-PG
 */
#define _AFM_EXTI12_PORT                __AFM_EXTI12_PORT__

/**
 * EXTI13 PORT SELECTION: 0-6 for PA-PG
 */
#define _AFM_EXTI13_PORT                __AFM_EXTI13_PORT__

/**
 * EXTI14 PORT SELECTION: 0-6 for PA-PG
 */
#define _AFM_EXTI14_PORT                __AFM_EXTI14_PORT__

/**
 * EXTI15 PORT SELECTION: 0-6 for PA-PG
 */
#define _AFM_EXTI15_PORT                __AFM_EXTI15_PORT__

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
