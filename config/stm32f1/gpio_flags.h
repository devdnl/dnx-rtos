/*=========================================================================*//**
@file    gpio_flags.h

@author  Daniel Zorychta

@brief   GPIO module configuration flags.

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

#ifndef _GPIO_FLAGS_H_
#define _GPIO_FLAGS_H_

#define __GPIO_PA_ENABLE__ _YES_
#define __GPIO_PB_ENABLE__ _YES_
#define __GPIO_PC_ENABLE__ _YES_
#define __GPIO_PD_ENABLE__ _YES_
#define __GPIO_PE_ENABLE__ _NO_
#define __GPIO_PF_ENABLE__ _NO_
#define __GPIO_PG_ENABLE__ _NO_

#define __GPIO_PA_PIN_0_NAME__ NC_PA0
#define __GPIO_PA_PIN_0_MODE__ _GPIO_IN_FLOAT
#define __GPIO_PA_PIN_0_STATE__ _FLOAT
#define __GPIO_PA_PIN_1_NAME__ NC_PA1
#define __GPIO_PA_PIN_1_MODE__ _GPIO_IN_FLOAT
#define __GPIO_PA_PIN_1_STATE__ _FLOAT
#define __GPIO_PA_PIN_2_NAME__ NC_PA2
#define __GPIO_PA_PIN_2_MODE__ _GPIO_IN_FLOAT
#define __GPIO_PA_PIN_2_STATE__ _FLOAT
#define __GPIO_PA_PIN_3_NAME__ NC_PA3
#define __GPIO_PA_PIN_3_MODE__ _GPIO_IN_FLOAT
#define __GPIO_PA_PIN_3_STATE__ _FLOAT
#define __GPIO_PA_PIN_4_NAME__ NC_PA4
#define __GPIO_PA_PIN_4_MODE__ _GPIO_IN_FLOAT
#define __GPIO_PA_PIN_4_STATE__ _FLOAT
#define __GPIO_PA_PIN_5_NAME__ NC_PA5
#define __GPIO_PA_PIN_5_MODE__ _GPIO_IN_FLOAT
#define __GPIO_PA_PIN_5_STATE__ _FLOAT
#define __GPIO_PA_PIN_6_NAME__ NC_PA6
#define __GPIO_PA_PIN_6_MODE__ _GPIO_IN_FLOAT
#define __GPIO_PA_PIN_6_STATE__ _FLOAT
#define __GPIO_PA_PIN_7_NAME__ NC_PA7
#define __GPIO_PA_PIN_7_MODE__ _GPIO_IN_FLOAT
#define __GPIO_PA_PIN_7_STATE__ _FLOAT
#define __GPIO_PA_PIN_8_NAME__ NC_PA8
#define __GPIO_PA_PIN_8_MODE__ _GPIO_IN_FLOAT
#define __GPIO_PA_PIN_8_STATE__ _FLOAT
#define __GPIO_PA_PIN_9_NAME__ USART1_TX
#define __GPIO_PA_PIN_9_MODE__ _GPIO_ALT_OUT_PUSH_PULL_10MHZ
#define __GPIO_PA_PIN_9_STATE__ _HIGH
#define __GPIO_PA_PIN_10_NAME__ USART1_RX
#define __GPIO_PA_PIN_10_MODE__ _GPIO_IN_PULLED
#define __GPIO_PA_PIN_10_STATE__ _HIGH
#define __GPIO_PA_PIN_11_NAME__ NC_PA11
#define __GPIO_PA_PIN_11_MODE__ _GPIO_IN_FLOAT
#define __GPIO_PA_PIN_11_STATE__ _FLOAT
#define __GPIO_PA_PIN_12_NAME__ NC_PA12
#define __GPIO_PA_PIN_12_MODE__ _GPIO_IN_FLOAT
#define __GPIO_PA_PIN_12_STATE__ _FLOAT
#define __GPIO_PA_PIN_13_NAME__ JTMS_SWDIO
#define __GPIO_PA_PIN_13_MODE__ _GPIO_IN_FLOAT
#define __GPIO_PA_PIN_13_STATE__ _FLOAT
#define __GPIO_PA_PIN_14_NAME__ JTCK_SWCLK
#define __GPIO_PA_PIN_14_MODE__ _GPIO_IN_FLOAT
#define __GPIO_PA_PIN_14_STATE__ _FLOAT
#define __GPIO_PA_PIN_15_NAME__ JTDI
#define __GPIO_PA_PIN_15_MODE__ _GPIO_IN_FLOAT
#define __GPIO_PA_PIN_15_STATE__ _FLOAT
#define __GPIO_PB_PIN_0_NAME__ NC_PB0
#define __GPIO_PB_PIN_0_MODE__ _GPIO_IN_FLOAT
#define __GPIO_PB_PIN_0_STATE__ _FLOAT
#define __GPIO_PB_PIN_1_NAME__ NC_PB1
#define __GPIO_PB_PIN_1_MODE__ _GPIO_IN_FLOAT
#define __GPIO_PB_PIN_1_STATE__ _FLOAT
#define __GPIO_PB_PIN_2_NAME__ BOOT1
#define __GPIO_PB_PIN_2_MODE__ _GPIO_IN_FLOAT
#define __GPIO_PB_PIN_2_STATE__ _FLOAT
#define __GPIO_PB_PIN_3_NAME__ JTDO
#define __GPIO_PB_PIN_3_MODE__ _GPIO_IN_FLOAT
#define __GPIO_PB_PIN_3_STATE__ _FLOAT
#define __GPIO_PB_PIN_4_NAME__ NJTRST
#define __GPIO_PB_PIN_4_MODE__ _GPIO_IN_FLOAT
#define __GPIO_PB_PIN_4_STATE__ _FLOAT
#define __GPIO_PB_PIN_5_NAME__ NC_PB5
#define __GPIO_PB_PIN_5_MODE__ _GPIO_IN_FLOAT
#define __GPIO_PB_PIN_5_STATE__ _FLOAT
#define __GPIO_PB_PIN_6_NAME__ NC_PB6
#define __GPIO_PB_PIN_6_MODE__ _GPIO_IN_FLOAT
#define __GPIO_PB_PIN_6_STATE__ _FLOAT
#define __GPIO_PB_PIN_7_NAME__ NC_PB7
#define __GPIO_PB_PIN_7_MODE__ _GPIO_IN_FLOAT
#define __GPIO_PB_PIN_7_STATE__ _FLOAT
#define __GPIO_PB_PIN_8_NAME__ NC_PB8
#define __GPIO_PB_PIN_8_MODE__ _GPIO_IN_FLOAT
#define __GPIO_PB_PIN_8_STATE__ _FLOAT
#define __GPIO_PB_PIN_9_NAME__ NC_PB9
#define __GPIO_PB_PIN_9_MODE__ _GPIO_IN_FLOAT
#define __GPIO_PB_PIN_9_STATE__ _FLOAT
#define __GPIO_PB_PIN_10_NAME__ NC_PB10
#define __GPIO_PB_PIN_10_MODE__ _GPIO_IN_FLOAT
#define __GPIO_PB_PIN_10_STATE__ _FLOAT
#define __GPIO_PB_PIN_11_NAME__ NC_PB11
#define __GPIO_PB_PIN_11_MODE__ _GPIO_IN_FLOAT
#define __GPIO_PB_PIN_11_STATE__ _FLOAT
#define __GPIO_PB_PIN_12_NAME__ NC_PB12
#define __GPIO_PB_PIN_12_MODE__ _GPIO_IN_FLOAT
#define __GPIO_PB_PIN_12_STATE__ _FLOAT
#define __GPIO_PB_PIN_13_NAME__ NC_PB13
#define __GPIO_PB_PIN_13_MODE__ _GPIO_IN_FLOAT
#define __GPIO_PB_PIN_13_STATE__ _FLOAT
#define __GPIO_PB_PIN_14_NAME__ NC_PB14
#define __GPIO_PB_PIN_14_MODE__ _GPIO_IN_FLOAT
#define __GPIO_PB_PIN_14_STATE__ _FLOAT
#define __GPIO_PB_PIN_15_NAME__ NC_PB15
#define __GPIO_PB_PIN_15_MODE__ _GPIO_IN_FLOAT
#define __GPIO_PB_PIN_15_STATE__ _FLOAT
#define __GPIO_PC_PIN_0_NAME__ NC_PC0
#define __GPIO_PC_PIN_0_MODE__ _GPIO_IN_FLOAT
#define __GPIO_PC_PIN_0_STATE__ _FLOAT
#define __GPIO_PC_PIN_1_NAME__ NC_PC1
#define __GPIO_PC_PIN_1_MODE__ _GPIO_IN_FLOAT
#define __GPIO_PC_PIN_1_STATE__ _FLOAT
#define __GPIO_PC_PIN_2_NAME__ NC_PC2
#define __GPIO_PC_PIN_2_MODE__ _GPIO_IN_FLOAT
#define __GPIO_PC_PIN_2_STATE__ _FLOAT
#define __GPIO_PC_PIN_3_NAME__ NC_PC3
#define __GPIO_PC_PIN_3_MODE__ _GPIO_IN_FLOAT
#define __GPIO_PC_PIN_3_STATE__ _FLOAT
#define __GPIO_PC_PIN_4_NAME__ NC_PC4
#define __GPIO_PC_PIN_4_MODE__ _GPIO_IN_FLOAT
#define __GPIO_PC_PIN_4_STATE__ _FLOAT
#define __GPIO_PC_PIN_5_NAME__ NC_PC5
#define __GPIO_PC_PIN_5_MODE__ _GPIO_IN_FLOAT
#define __GPIO_PC_PIN_5_STATE__ _FLOAT
#define __GPIO_PC_PIN_6_NAME__ NC_PC6
#define __GPIO_PC_PIN_6_MODE__ _GPIO_IN_FLOAT
#define __GPIO_PC_PIN_6_STATE__ _FLOAT
#define __GPIO_PC_PIN_7_NAME__ NC_PC7
#define __GPIO_PC_PIN_7_MODE__ _GPIO_IN_FLOAT
#define __GPIO_PC_PIN_7_STATE__ _FLOAT
#define __GPIO_PC_PIN_8_NAME__ NC_PC8
#define __GPIO_PC_PIN_8_MODE__ _GPIO_IN_FLOAT
#define __GPIO_PC_PIN_8_STATE__ _FLOAT
#define __GPIO_PC_PIN_9_NAME__ NC_PC9
#define __GPIO_PC_PIN_9_MODE__ _GPIO_IN_FLOAT
#define __GPIO_PC_PIN_9_STATE__ _FLOAT
#define __GPIO_PC_PIN_10_NAME__ NC_PC10
#define __GPIO_PC_PIN_10_MODE__ _GPIO_IN_FLOAT
#define __GPIO_PC_PIN_10_STATE__ _FLOAT
#define __GPIO_PC_PIN_11_NAME__ NC_PC11
#define __GPIO_PC_PIN_11_MODE__ _GPIO_IN_FLOAT
#define __GPIO_PC_PIN_11_STATE__ _FLOAT
#define __GPIO_PC_PIN_12_NAME__ NC_PC12
#define __GPIO_PC_PIN_12_MODE__ _GPIO_IN_FLOAT
#define __GPIO_PC_PIN_12_STATE__ _FLOAT
#define __GPIO_PC_PIN_13_NAME__ NC_PC13
#define __GPIO_PC_PIN_13_MODE__ _GPIO_IN_FLOAT
#define __GPIO_PC_PIN_13_STATE__ _FLOAT
#define __GPIO_PC_PIN_14_NAME__ OSC32_IN
#define __GPIO_PC_PIN_14_MODE__ _GPIO_IN_FLOAT
#define __GPIO_PC_PIN_14_STATE__ _FLOAT
#define __GPIO_PC_PIN_15_NAME__ OSC32_OUT
#define __GPIO_PC_PIN_15_MODE__ _GPIO_IN_FLOAT
#define __GPIO_PC_PIN_15_STATE__ _FLOAT
#define __GPIO_PD_PIN_0_NAME__ OSC_IN
#define __GPIO_PD_PIN_0_MODE__ _GPIO_IN_FLOAT
#define __GPIO_PD_PIN_0_STATE__ _FLOAT
#define __GPIO_PD_PIN_1_NAME__ OSC_OUT
#define __GPIO_PD_PIN_1_MODE__ _GPIO_IN_FLOAT
#define __GPIO_PD_PIN_1_STATE__ _FLOAT
#define __GPIO_PD_PIN_2_NAME__ NC_PD2
#define __GPIO_PD_PIN_2_MODE__ _GPIO_IN_FLOAT
#define __GPIO_PD_PIN_2_STATE__ _FLOAT
#define __GPIO_PD_PIN_3_NAME__ NC_PD3
#define __GPIO_PD_PIN_3_MODE__ _GPIO_IN_FLOAT
#define __GPIO_PD_PIN_3_STATE__ _FLOAT
#define __GPIO_PD_PIN_4_NAME__ NC_PD4
#define __GPIO_PD_PIN_4_MODE__ _GPIO_IN_FLOAT
#define __GPIO_PD_PIN_4_STATE__ _FLOAT
#define __GPIO_PD_PIN_5_NAME__ NC_PD5
#define __GPIO_PD_PIN_5_MODE__ _GPIO_IN_FLOAT
#define __GPIO_PD_PIN_5_STATE__ _FLOAT
#define __GPIO_PD_PIN_6_NAME__ NC_PD6
#define __GPIO_PD_PIN_6_MODE__ _GPIO_IN_FLOAT
#define __GPIO_PD_PIN_6_STATE__ _FLOAT
#define __GPIO_PD_PIN_7_NAME__ NC_PD7
#define __GPIO_PD_PIN_7_MODE__ _GPIO_IN_FLOAT
#define __GPIO_PD_PIN_7_STATE__ _FLOAT
#define __GPIO_PD_PIN_8_NAME__ NC_PD8
#define __GPIO_PD_PIN_8_MODE__ _GPIO_IN_FLOAT
#define __GPIO_PD_PIN_8_STATE__ _FLOAT
#define __GPIO_PD_PIN_9_NAME__ NC_PD9
#define __GPIO_PD_PIN_9_MODE__ _GPIO_IN_FLOAT
#define __GPIO_PD_PIN_9_STATE__ _FLOAT
#define __GPIO_PD_PIN_10_NAME__ NC_PD10
#define __GPIO_PD_PIN_10_MODE__ _GPIO_IN_FLOAT
#define __GPIO_PD_PIN_10_STATE__ _FLOAT
#define __GPIO_PD_PIN_11_NAME__ NC_PD11
#define __GPIO_PD_PIN_11_MODE__ _GPIO_IN_FLOAT
#define __GPIO_PD_PIN_11_STATE__ _FLOAT
#define __GPIO_PD_PIN_12_NAME__ NC_PD12
#define __GPIO_PD_PIN_12_MODE__ _GPIO_IN_FLOAT
#define __GPIO_PD_PIN_12_STATE__ _FLOAT
#define __GPIO_PD_PIN_13_NAME__ NC_PD13
#define __GPIO_PD_PIN_13_MODE__ _GPIO_IN_FLOAT
#define __GPIO_PD_PIN_13_STATE__ _FLOAT
#define __GPIO_PD_PIN_14_NAME__ NC_PD14
#define __GPIO_PD_PIN_14_MODE__ _GPIO_IN_FLOAT
#define __GPIO_PD_PIN_14_STATE__ _FLOAT
#define __GPIO_PD_PIN_15_NAME__ NC_PD15
#define __GPIO_PD_PIN_15_MODE__ _GPIO_IN_FLOAT
#define __GPIO_PD_PIN_15_STATE__ _FLOAT
#define __GPIO_PE_PIN_0_NAME__ NC_PE0
#define __GPIO_PE_PIN_0_MODE__ _GPIO_IN_FLOAT
#define __GPIO_PE_PIN_0_STATE__ _FLOAT
#define __GPIO_PE_PIN_1_NAME__ NC_PE1
#define __GPIO_PE_PIN_1_MODE__ _GPIO_IN_FLOAT
#define __GPIO_PE_PIN_1_STATE__ _FLOAT
#define __GPIO_PE_PIN_2_NAME__ NC_PE2
#define __GPIO_PE_PIN_2_MODE__ _GPIO_IN_FLOAT
#define __GPIO_PE_PIN_2_STATE__ _FLOAT
#define __GPIO_PE_PIN_3_NAME__ NC_PE3
#define __GPIO_PE_PIN_3_MODE__ _GPIO_IN_FLOAT
#define __GPIO_PE_PIN_3_STATE__ _FLOAT
#define __GPIO_PE_PIN_4_NAME__ NC_PE4
#define __GPIO_PE_PIN_4_MODE__ _GPIO_IN_FLOAT
#define __GPIO_PE_PIN_4_STATE__ _FLOAT
#define __GPIO_PE_PIN_5_NAME__ NC_PE5
#define __GPIO_PE_PIN_5_MODE__ _GPIO_IN_FLOAT
#define __GPIO_PE_PIN_5_STATE__ _FLOAT
#define __GPIO_PE_PIN_6_NAME__ NC_PE6
#define __GPIO_PE_PIN_6_MODE__ _GPIO_IN_FLOAT
#define __GPIO_PE_PIN_6_STATE__ _FLOAT
#define __GPIO_PE_PIN_7_NAME__ NC_PE7
#define __GPIO_PE_PIN_7_MODE__ _GPIO_IN_FLOAT
#define __GPIO_PE_PIN_7_STATE__ _FLOAT
#define __GPIO_PE_PIN_8_NAME__ NC_PE8
#define __GPIO_PE_PIN_8_MODE__ _GPIO_IN_FLOAT
#define __GPIO_PE_PIN_8_STATE__ _FLOAT
#define __GPIO_PE_PIN_9_NAME__ NC_PE9
#define __GPIO_PE_PIN_9_MODE__ _GPIO_IN_FLOAT
#define __GPIO_PE_PIN_9_STATE__ _FLOAT
#define __GPIO_PE_PIN_10_NAME__ NC_PE10
#define __GPIO_PE_PIN_10_MODE__ _GPIO_IN_FLOAT
#define __GPIO_PE_PIN_10_STATE__ _FLOAT
#define __GPIO_PE_PIN_11_NAME__ NC_PE11
#define __GPIO_PE_PIN_11_MODE__ _GPIO_IN_FLOAT
#define __GPIO_PE_PIN_11_STATE__ _FLOAT
#define __GPIO_PE_PIN_12_NAME__ NC_PE12
#define __GPIO_PE_PIN_12_MODE__ _GPIO_IN_FLOAT
#define __GPIO_PE_PIN_12_STATE__ _FLOAT
#define __GPIO_PE_PIN_13_NAME__ NC_PE13
#define __GPIO_PE_PIN_13_MODE__ _GPIO_IN_FLOAT
#define __GPIO_PE_PIN_13_STATE__ _FLOAT
#define __GPIO_PE_PIN_14_NAME__ NC_PE14
#define __GPIO_PE_PIN_14_MODE__ _GPIO_IN_FLOAT
#define __GPIO_PE_PIN_14_STATE__ _FLOAT
#define __GPIO_PE_PIN_15_NAME__ NC_PE15
#define __GPIO_PE_PIN_15_MODE__ _GPIO_IN_FLOAT
#define __GPIO_PE_PIN_15_STATE__ _FLOAT
#define __GPIO_PF_PIN_0_NAME__ NC_PF0
#define __GPIO_PF_PIN_0_MODE__ _GPIO_IN_FLOAT
#define __GPIO_PF_PIN_0_STATE__ _FLOAT
#define __GPIO_PF_PIN_1_NAME__ NC_PF1
#define __GPIO_PF_PIN_1_MODE__ _GPIO_IN_FLOAT
#define __GPIO_PF_PIN_1_STATE__ _FLOAT
#define __GPIO_PF_PIN_2_NAME__ NC_PF2
#define __GPIO_PF_PIN_2_MODE__ _GPIO_IN_FLOAT
#define __GPIO_PF_PIN_2_STATE__ _FLOAT
#define __GPIO_PF_PIN_3_NAME__ NC_PF3
#define __GPIO_PF_PIN_3_MODE__ _GPIO_IN_FLOAT
#define __GPIO_PF_PIN_3_STATE__ _FLOAT
#define __GPIO_PF_PIN_4_NAME__ NC_PF4
#define __GPIO_PF_PIN_4_MODE__ _GPIO_IN_FLOAT
#define __GPIO_PF_PIN_4_STATE__ _FLOAT
#define __GPIO_PF_PIN_5_NAME__ NC_PF5
#define __GPIO_PF_PIN_5_MODE__ _GPIO_IN_FLOAT
#define __GPIO_PF_PIN_5_STATE__ _FLOAT
#define __GPIO_PF_PIN_6_NAME__ NC_PF6
#define __GPIO_PF_PIN_6_MODE__ _GPIO_IN_FLOAT
#define __GPIO_PF_PIN_6_STATE__ _FLOAT
#define __GPIO_PF_PIN_7_NAME__ NC_PF7
#define __GPIO_PF_PIN_7_MODE__ _GPIO_IN_FLOAT
#define __GPIO_PF_PIN_7_STATE__ _FLOAT
#define __GPIO_PF_PIN_8_NAME__ NC_PF8
#define __GPIO_PF_PIN_8_MODE__ _GPIO_IN_FLOAT
#define __GPIO_PF_PIN_8_STATE__ _FLOAT
#define __GPIO_PF_PIN_9_NAME__ NC_PF9
#define __GPIO_PF_PIN_9_MODE__ _GPIO_IN_FLOAT
#define __GPIO_PF_PIN_9_STATE__ _FLOAT
#define __GPIO_PF_PIN_10_NAME__ NC_PF10
#define __GPIO_PF_PIN_10_MODE__ _GPIO_IN_FLOAT
#define __GPIO_PF_PIN_10_STATE__ _FLOAT
#define __GPIO_PF_PIN_11_NAME__ NC_PF11
#define __GPIO_PF_PIN_11_MODE__ _GPIO_IN_FLOAT
#define __GPIO_PF_PIN_11_STATE__ _FLOAT
#define __GPIO_PF_PIN_12_NAME__ NC_PF12
#define __GPIO_PF_PIN_12_MODE__ _GPIO_IN_FLOAT
#define __GPIO_PF_PIN_12_STATE__ _FLOAT
#define __GPIO_PF_PIN_13_NAME__ NC_PF13
#define __GPIO_PF_PIN_13_MODE__ _GPIO_IN_FLOAT
#define __GPIO_PF_PIN_13_STATE__ _FLOAT
#define __GPIO_PF_PIN_14_NAME__ NC_PF14
#define __GPIO_PF_PIN_14_MODE__ _GPIO_IN_FLOAT
#define __GPIO_PF_PIN_14_STATE__ _FLOAT
#define __GPIO_PF_PIN_15_NAME__ NC_PF15
#define __GPIO_PF_PIN_15_MODE__ _GPIO_IN_FLOAT
#define __GPIO_PF_PIN_15_STATE__ _FLOAT
#define __GPIO_PG_PIN_0_NAME__ NC_PG0
#define __GPIO_PG_PIN_0_MODE__ _GPIO_IN_FLOAT
#define __GPIO_PG_PIN_0_STATE__ _FLOAT
#define __GPIO_PG_PIN_1_NAME__ NC_PG1
#define __GPIO_PG_PIN_1_MODE__ _GPIO_IN_FLOAT
#define __GPIO_PG_PIN_1_STATE__ _FLOAT
#define __GPIO_PG_PIN_2_NAME__ NC_PG2
#define __GPIO_PG_PIN_2_MODE__ _GPIO_IN_FLOAT
#define __GPIO_PG_PIN_2_STATE__ _FLOAT
#define __GPIO_PG_PIN_3_NAME__ NC_PG3
#define __GPIO_PG_PIN_3_MODE__ _GPIO_IN_FLOAT
#define __GPIO_PG_PIN_3_STATE__ _FLOAT
#define __GPIO_PG_PIN_4_NAME__ NC_PG4
#define __GPIO_PG_PIN_4_MODE__ _GPIO_IN_FLOAT
#define __GPIO_PG_PIN_4_STATE__ _FLOAT
#define __GPIO_PG_PIN_5_NAME__ NC_PG5
#define __GPIO_PG_PIN_5_MODE__ _GPIO_IN_FLOAT
#define __GPIO_PG_PIN_5_STATE__ _FLOAT
#define __GPIO_PG_PIN_6_NAME__ NC_PG6
#define __GPIO_PG_PIN_6_MODE__ _GPIO_IN_FLOAT
#define __GPIO_PG_PIN_6_STATE__ _FLOAT
#define __GPIO_PG_PIN_7_NAME__ NC_PG7
#define __GPIO_PG_PIN_7_MODE__ _GPIO_IN_FLOAT
#define __GPIO_PG_PIN_7_STATE__ _FLOAT
#define __GPIO_PG_PIN_8_NAME__ NC_PG8
#define __GPIO_PG_PIN_8_MODE__ _GPIO_IN_FLOAT
#define __GPIO_PG_PIN_8_STATE__ _FLOAT
#define __GPIO_PG_PIN_9_NAME__ NC_PG9
#define __GPIO_PG_PIN_9_MODE__ _GPIO_IN_FLOAT
#define __GPIO_PG_PIN_9_STATE__ _FLOAT
#define __GPIO_PG_PIN_10_NAME__ NC_PG10
#define __GPIO_PG_PIN_10_MODE__ _GPIO_IN_FLOAT
#define __GPIO_PG_PIN_10_STATE__ _FLOAT
#define __GPIO_PG_PIN_11_NAME__ NC_PG11
#define __GPIO_PG_PIN_11_MODE__ _GPIO_IN_FLOAT
#define __GPIO_PG_PIN_11_STATE__ _FLOAT
#define __GPIO_PG_PIN_12_NAME__ NC_PG12
#define __GPIO_PG_PIN_12_MODE__ _GPIO_IN_FLOAT
#define __GPIO_PG_PIN_12_STATE__ _FLOAT
#define __GPIO_PG_PIN_13_NAME__ NC_PG13
#define __GPIO_PG_PIN_13_MODE__ _GPIO_IN_FLOAT
#define __GPIO_PG_PIN_13_STATE__ _FLOAT
#define __GPIO_PG_PIN_14_NAME__ NC_PG14
#define __GPIO_PG_PIN_14_MODE__ _GPIO_IN_FLOAT
#define __GPIO_PG_PIN_14_STATE__ _FLOAT
#define __GPIO_PG_PIN_15_NAME__ NC_PG15
#define __GPIO_PG_PIN_15_MODE__ _GPIO_IN_FLOAT
#define __GPIO_PG_PIN_15_STATE__ _FLOAT

#endif /* _GPIO_FLAGS_H_ */
/*==============================================================================
  End of file
==============================================================================*/
