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
 * 0: IRQ DISABLED
 * 1: TRIGGER ON FALLING EDGE
 * 2: TRIGGER ON RISING EDGE
 * 3: TRIGGER ON FALLING AND RISING EDGE
 */
#define __IRQ_LINE_0_MODE 1
#define __IRQ_LINE_1_MODE 1
#define __IRQ_LINE_2_MODE 1
#define __IRQ_LINE_3_MODE 1
#define __IRQ_LINE_4_MODE 1
#define __IRQ_LINE_5_MODE 1
#define __IRQ_LINE_6_MODE 1
#define __IRQ_LINE_7_MODE 1
#define __IRQ_LINE_8_MODE 1
#define __IRQ_LINE_9_MODE 1
#define __IRQ_LINE_10_MODE 1
#define __IRQ_LINE_11_MODE 1
#define __IRQ_LINE_12_MODE 1
#define __IRQ_LINE_13_MODE 1
#define __IRQ_LINE_14_MODE 1
#define __IRQ_LINE_15_MODE 1

#endif /* _IRQ_FLAGS_H_ */
/*==============================================================================
  End of file
==============================================================================*/
