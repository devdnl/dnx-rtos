/*=========================================================================*//**
@file    irq_flags.h

@author  Daniel Zorychta

@brief   This driver support external interrupts (EXTI).

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

/*
 * NOTE: All flags defined as: __FLAG_NAME__ (with doubled underscore as suffix
 *       and prefix) are exported to the single configuration file
 *       (by using Configtool) when entire project configuration is exported.
 *       All other flag definitions and statements are ignored.
 */

#ifndef _IRQ_FLAGS_H_
#define _IRQ_FLAGS_H_

/*
 * Interrupts Default Configuration
 * 0: IRQ DISABLED                              (_IRQ_MODE_DISABLED)
 * 1: TRIGGER ON FALLING EDGE                   (_IRQ_MODE_FALLING_EDGE)
 * 2: TRIGGER ON RISING EDGE                    (_IRQ_MODE_RISING_EDGE)
 * 3: TRIGGER ON FALLING AND RISING EDGE        (_IRQ_MODE_FALLING_AND_RISING_EDGE)
 */
#define __IRQ_LINE_0_MODE__ _IRQ_MODE_DISABLED
#define __IRQ_LINE_1_MODE__ _IRQ_MODE_DISABLED
#define __IRQ_LINE_2_MODE__ _IRQ_MODE_DISABLED
#define __IRQ_LINE_3_MODE__ _IRQ_MODE_DISABLED
#define __IRQ_LINE_4_MODE__ _IRQ_MODE_DISABLED
#define __IRQ_LINE_5_MODE__ _IRQ_MODE_DISABLED
#define __IRQ_LINE_6_MODE__ _IRQ_MODE_DISABLED
#define __IRQ_LINE_7_MODE__ _IRQ_MODE_DISABLED
#define __IRQ_LINE_8_MODE__ _IRQ_MODE_DISABLED
#define __IRQ_LINE_9_MODE__ _IRQ_MODE_DISABLED
#define __IRQ_LINE_10_MODE__ _IRQ_MODE_DISABLED
#define __IRQ_LINE_11_MODE__ _IRQ_MODE_DISABLED
#define __IRQ_LINE_12_MODE__ _IRQ_MODE_DISABLED
#define __IRQ_LINE_13_MODE__ _IRQ_MODE_DISABLED
#define __IRQ_LINE_14_MODE__ _IRQ_MODE_DISABLED
#define __IRQ_LINE_15_MODE__ _IRQ_MODE_DISABLED

#define __IRQ_LINE_0_PRIO__ CONFIG_USER_IRQ_PRIORITY
#define __IRQ_LINE_1_PRIO__ CONFIG_USER_IRQ_PRIORITY
#define __IRQ_LINE_2_PRIO__ CONFIG_USER_IRQ_PRIORITY
#define __IRQ_LINE_3_PRIO__ CONFIG_USER_IRQ_PRIORITY
#define __IRQ_LINE_4_PRIO__ CONFIG_USER_IRQ_PRIORITY
#define __IRQ_LINE_5_PRIO__ CONFIG_USER_IRQ_PRIORITY
#define __IRQ_LINE_6_PRIO__ CONFIG_USER_IRQ_PRIORITY
#define __IRQ_LINE_7_PRIO__ CONFIG_USER_IRQ_PRIORITY
#define __IRQ_LINE_8_PRIO__ CONFIG_USER_IRQ_PRIORITY
#define __IRQ_LINE_9_PRIO__ CONFIG_USER_IRQ_PRIORITY
#define __IRQ_LINE_10_PRIO__ CONFIG_USER_IRQ_PRIORITY
#define __IRQ_LINE_11_PRIO__ CONFIG_USER_IRQ_PRIORITY
#define __IRQ_LINE_12_PRIO__ CONFIG_USER_IRQ_PRIORITY
#define __IRQ_LINE_13_PRIO__ CONFIG_USER_IRQ_PRIORITY
#define __IRQ_LINE_14_PRIO__ CONFIG_USER_IRQ_PRIORITY
#define __IRQ_LINE_15_PRIO__ CONFIG_USER_IRQ_PRIORITY

#endif /* _IRQ_FLAGS_H_ */
/*==============================================================================
  End of file
==============================================================================*/
