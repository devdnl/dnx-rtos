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

#ifndef _IRQ_FLAGS_H_
#define _IRQ_FLAGS_H_

/*
 * Interrupts Default Configuration
 * 0: IRQ DISABLED                              (_IRQ_MODE_DISABLED)
 * 1: TRIGGER ON FALLING EDGE                   (_IRQ_MODE_FALLING_EDGE)
 * 2: TRIGGER ON RISING EDGE                    (_IRQ_MODE_RISING_EDGE)
 * 3: TRIGGER ON FALLING AND RISING EDGE        (_IRQ_MODE_FALLING_AND_RISING_EDGE)
 */
#define __IRQ_LINE_0_MODE 0
#define __IRQ_LINE_1_MODE 0
#define __IRQ_LINE_2_MODE 0
#define __IRQ_LINE_3_MODE 0
#define __IRQ_LINE_4_MODE 0
#define __IRQ_LINE_5_MODE 0
#define __IRQ_LINE_6_MODE 0
#define __IRQ_LINE_7_MODE 0
#define __IRQ_LINE_8_MODE 0
#define __IRQ_LINE_9_MODE 0
#define __IRQ_LINE_10_MODE 0
#define __IRQ_LINE_11_MODE 0
#define __IRQ_LINE_12_MODE 0
#define __IRQ_LINE_13_MODE 0
#define __IRQ_LINE_14_MODE 0
#define __IRQ_LINE_15_MODE 0

#define __IRQ_LINE_0_PRIO CONFIG_USER_IRQ_PRIORITY
#define __IRQ_LINE_1_PRIO CONFIG_USER_IRQ_PRIORITY
#define __IRQ_LINE_2_PRIO CONFIG_USER_IRQ_PRIORITY
#define __IRQ_LINE_3_PRIO CONFIG_USER_IRQ_PRIORITY
#define __IRQ_LINE_4_PRIO CONFIG_USER_IRQ_PRIORITY
#define __IRQ_LINE_5_PRIO CONFIG_USER_IRQ_PRIORITY
#define __IRQ_LINE_6_PRIO CONFIG_USER_IRQ_PRIORITY
#define __IRQ_LINE_7_PRIO CONFIG_USER_IRQ_PRIORITY
#define __IRQ_LINE_8_PRIO CONFIG_USER_IRQ_PRIORITY
#define __IRQ_LINE_9_PRIO CONFIG_USER_IRQ_PRIORITY
#define __IRQ_LINE_10_PRIO CONFIG_USER_IRQ_PRIORITY
#define __IRQ_LINE_11_PRIO CONFIG_USER_IRQ_PRIORITY
#define __IRQ_LINE_12_PRIO CONFIG_USER_IRQ_PRIORITY
#define __IRQ_LINE_13_PRIO CONFIG_USER_IRQ_PRIORITY
#define __IRQ_LINE_14_PRIO CONFIG_USER_IRQ_PRIORITY
#define __IRQ_LINE_15_PRIO CONFIG_USER_IRQ_PRIORITY

#endif /* _IRQ_FLAGS_H_ */
/*==============================================================================
  End of file
==============================================================================*/
