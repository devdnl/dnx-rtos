/*=========================================================================*//**
@file    wdg_cfg.h

@author  Daniel Zorychta

@brief   WDG driver configuration

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
 * Enable device lock for 1 task (e.g. Watchdog daemon)
 *
 * true:  device locked at first file open
 * false: device can open all tasks
 */
#define _WDG_CFG_OPEN_LOCK              true

/*
 * Set Watchdog prescaler (LSI / prescaler; LSI = 40 kHz)
 *
 * 4, 8, 16, 32, 64, 128, 256: possible dividers
 */
#define _WDG_CFG_DIVIDER                128

/*
 * Reload value (period = LSI / _WDG_CFG_DIVIDER / _WDG_CFG_RELOAD)
 *
 * 0-0xFFF
 */
#define _WDG_CFG_RELOAD                 2500    /* 8s */

/*
 * Disable WDG on debug mode
 *
 * true : WDG disabled on debug
 * false: WDG continue counting
 */
#define _WDG_CFG_DISABLE_ON_DEBUG       true

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
