/*=========================================================================*//**
@file    wdg_cfg.h

@author  Daniel Zorychta

@brief   WDG driver configuration

@note    Copyright (C) 2014 Daniel Zorychta <daniel.zorychta@gmail.com>

         This program is free software; you can redistribute it and/or modify
         it under the terms of the GNU General Public License as published by
         the Free Software Foundation and modified by the dnx RTOS exception.

         NOTE: The modification  to the GPL is  included to allow you to
               distribute a combined work that includes FreeRTOS without
               being obliged to provide the source  code for proprietary
               components outside of the dnx RTOS.

         The dnx RTOS  is  distributed  in the hope  that  it will be useful,
         but WITHOUT  ANY  WARRANTY;  without  even  the implied  warranty of
         MERCHANTABILITY  or  FITNESS  FOR  A  PARTICULAR  PURPOSE.  See  the
         GNU General Public License for more details.

         Full license text is available on the following file: doc/license.txt.


*//*==========================================================================*/

#ifndef _WDG_CFG_H_
#define _WDG_CFG_H_

/*==============================================================================
  Include files
==============================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Exported macros
==============================================================================*/
/*
 * Enable device lock for 1st task (e.g. Watchdog daemon)
 *
 * true:  device locked at open() operation
 * false: device not locked at open() operation
 */
#define _WDG_CFG_OPEN_LOCK              __WDG_DEVICE_LOCK_AT_OPEN__

/*
 * Set Watchdog prescaler (LSI / prescaler; LSI = 40 kHz)
 *
 * 4, 8, 16, 32, 64, 128, 256: possible dividers
 */
#define _WDG_CFG_DIVIDER                __WDG_CLK_DIVIDER__

/*
 * Reload value (period = LSI / _WDG_CFG_DIVIDER / _WDG_CFG_RELOAD)
 *
 * 0-0xFFF (0-4095)
 */
#define _WDG_CFG_RELOAD                 __WDG_RELOAD_VALUE__

/*
 * Disable WDG on debug mode
 *
 * true : WDG disabled on debug
 * false: WDG continue counting
 */
#define _WDG_CFG_DISABLE_ON_DEBUG       __WDG_DISABLE_ON_DEBUG__

/*==============================================================================
  Exported object types
==============================================================================*/

/*==============================================================================
  Exported objects
==============================================================================*/

/*==============================================================================
  Exported functions
==============================================================================*/

#ifdef __cplusplus
}
#endif

#endif /* _WDG_CFG_H_ */
/*==============================================================================
  End of file
==============================================================================*/
