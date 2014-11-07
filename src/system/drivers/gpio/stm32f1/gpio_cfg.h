/*=========================================================================*//**
@file    gpio_cfg.h

@author  Daniel Zorychta

@brief   This driver support GPIO. Set here driver configuration.

@note    Copyright (C) 2012  Daniel Zorychta <daniel.zorychta@gmail.com>

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

#ifndef _GPIO_CFG_H_
#define _GPIO_CFG_H_

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Include files
==============================================================================*/
#include "stm32f1/stm32f10x.h"
#include "stm32f1/gpio_macros.h"

/*==============================================================================
  Exported symbolic constants/macros
==============================================================================*/
/** macro creates an enumerator with pin data */
#define _PIN_CONFIGURATION(port, number, pin_name, mode, state) \
enum port##_##number##_CFG {\
        pin_name,\
        _CONCAT(_GPIO_, pin_name) = port##_BASE,\
        _CONCAT(_BP_, pin_name) = (number),\
        _CONCAT(_BM_, pin_name) = (1 << (number)),\
        _##port##_PIN_##number##_MODE  = (mode),\
        _##port##_PIN_##number##_STATE = (state)\
}

/** enable (1) or disable (0) GPIOA port */
#define _GPIOA_EN                               __GPIO_PA_ENABLE__

/** enable (1) or disable (0) GPIOB port */
#define _GPIOB_EN                               __GPIO_PB_ENABLE__

/** enable (1) or disable (0) GPIOC port */
#define _GPIOC_EN                               __GPIO_PC_ENABLE__

/** enable (1) or disable (0) GPIOD port */
#define _GPIOD_EN                               __GPIO_PD_ENABLE__

/** enable (1) or disable (0) GPIOE port */
#define _GPIOE_EN                               __GPIO_PE_ENABLE__

/** enable (1) or disable (0) GPIOF port */
#define _GPIOF_EN                               __GPIO_PF_ENABLE__

/** enable (1) or disable (0) GPIOG port */
#define _GPIOG_EN                               __GPIO_PG_ENABLE__

/** GPIO pin NONE definition */
#define GPIO_NONE_BASE 0
_PIN_CONFIGURATION(GPIO_NONE, 0, NONE, _GPIO_ANALOG, _GPIO_IN_FLOAT);

/** GPIOA pins configuration */
#if _GPIOA_EN
_PIN_CONFIGURATION(GPIOA,  0, __GPIO_PA_PIN_0_NAME__ , __GPIO_PA_PIN_0_MODE__ , __GPIO_PA_PIN_0_STATE__ );
_PIN_CONFIGURATION(GPIOA,  1, __GPIO_PA_PIN_1_NAME__ , __GPIO_PA_PIN_1_MODE__ , __GPIO_PA_PIN_1_STATE__ );
_PIN_CONFIGURATION(GPIOA,  2, __GPIO_PA_PIN_2_NAME__ , __GPIO_PA_PIN_2_MODE__ , __GPIO_PA_PIN_2_STATE__ );
_PIN_CONFIGURATION(GPIOA,  3, __GPIO_PA_PIN_3_NAME__ , __GPIO_PA_PIN_3_MODE__ , __GPIO_PA_PIN_3_STATE__ );
_PIN_CONFIGURATION(GPIOA,  4, __GPIO_PA_PIN_4_NAME__ , __GPIO_PA_PIN_4_MODE__ , __GPIO_PA_PIN_4_STATE__ );
_PIN_CONFIGURATION(GPIOA,  5, __GPIO_PA_PIN_5_NAME__ , __GPIO_PA_PIN_5_MODE__ , __GPIO_PA_PIN_5_STATE__ );
_PIN_CONFIGURATION(GPIOA,  6, __GPIO_PA_PIN_6_NAME__ , __GPIO_PA_PIN_6_MODE__ , __GPIO_PA_PIN_6_STATE__ );
_PIN_CONFIGURATION(GPIOA,  7, __GPIO_PA_PIN_7_NAME__ , __GPIO_PA_PIN_7_MODE__ , __GPIO_PA_PIN_7_STATE__ );
_PIN_CONFIGURATION(GPIOA,  8, __GPIO_PA_PIN_8_NAME__ , __GPIO_PA_PIN_8_MODE__ , __GPIO_PA_PIN_8_STATE__ );
_PIN_CONFIGURATION(GPIOA,  9, __GPIO_PA_PIN_9_NAME__ , __GPIO_PA_PIN_9_MODE__ , __GPIO_PA_PIN_9_STATE__ );
_PIN_CONFIGURATION(GPIOA, 10, __GPIO_PA_PIN_10_NAME__, __GPIO_PA_PIN_10_MODE__, __GPIO_PA_PIN_10_STATE__);
_PIN_CONFIGURATION(GPIOA, 11, __GPIO_PA_PIN_11_NAME__, __GPIO_PA_PIN_11_MODE__, __GPIO_PA_PIN_11_STATE__);
_PIN_CONFIGURATION(GPIOA, 12, __GPIO_PA_PIN_12_NAME__, __GPIO_PA_PIN_12_MODE__, __GPIO_PA_PIN_12_STATE__);
_PIN_CONFIGURATION(GPIOA, 13, __GPIO_PA_PIN_13_NAME__, __GPIO_PA_PIN_13_MODE__, __GPIO_PA_PIN_13_STATE__);
_PIN_CONFIGURATION(GPIOA, 14, __GPIO_PA_PIN_14_NAME__, __GPIO_PA_PIN_14_MODE__, __GPIO_PA_PIN_14_STATE__);
_PIN_CONFIGURATION(GPIOA, 15, __GPIO_PA_PIN_15_NAME__, __GPIO_PA_PIN_15_MODE__, __GPIO_PA_PIN_15_STATE__);
#endif

/** GPIOB pins configuration */
#if _GPIOB_EN
_PIN_CONFIGURATION(GPIOB,  0, __GPIO_PB_PIN_0_NAME__ , __GPIO_PB_PIN_0_MODE__ , __GPIO_PB_PIN_0_STATE__ );
_PIN_CONFIGURATION(GPIOB,  1, __GPIO_PB_PIN_1_NAME__ , __GPIO_PB_PIN_1_MODE__ , __GPIO_PB_PIN_1_STATE__ );
_PIN_CONFIGURATION(GPIOB,  2, __GPIO_PB_PIN_2_NAME__ , __GPIO_PB_PIN_2_MODE__ , __GPIO_PB_PIN_2_STATE__ );
_PIN_CONFIGURATION(GPIOB,  3, __GPIO_PB_PIN_3_NAME__ , __GPIO_PB_PIN_3_MODE__ , __GPIO_PB_PIN_3_STATE__ );
_PIN_CONFIGURATION(GPIOB,  4, __GPIO_PB_PIN_4_NAME__ , __GPIO_PB_PIN_4_MODE__ , __GPIO_PB_PIN_4_STATE__ );
_PIN_CONFIGURATION(GPIOB,  5, __GPIO_PB_PIN_5_NAME__ , __GPIO_PB_PIN_5_MODE__ , __GPIO_PB_PIN_5_STATE__ );
_PIN_CONFIGURATION(GPIOB,  6, __GPIO_PB_PIN_6_NAME__ , __GPIO_PB_PIN_6_MODE__ , __GPIO_PB_PIN_6_STATE__ );
_PIN_CONFIGURATION(GPIOB,  7, __GPIO_PB_PIN_7_NAME__ , __GPIO_PB_PIN_7_MODE__ , __GPIO_PB_PIN_7_STATE__ );
_PIN_CONFIGURATION(GPIOB,  8, __GPIO_PB_PIN_8_NAME__ , __GPIO_PB_PIN_8_MODE__ , __GPIO_PB_PIN_8_STATE__ );
_PIN_CONFIGURATION(GPIOB,  9, __GPIO_PB_PIN_9_NAME__ , __GPIO_PB_PIN_9_MODE__ , __GPIO_PB_PIN_9_STATE__ );
_PIN_CONFIGURATION(GPIOB, 10, __GPIO_PB_PIN_10_NAME__, __GPIO_PB_PIN_10_MODE__, __GPIO_PB_PIN_10_STATE__);
_PIN_CONFIGURATION(GPIOB, 11, __GPIO_PB_PIN_11_NAME__, __GPIO_PB_PIN_11_MODE__, __GPIO_PB_PIN_11_STATE__);
_PIN_CONFIGURATION(GPIOB, 12, __GPIO_PB_PIN_12_NAME__, __GPIO_PB_PIN_12_MODE__, __GPIO_PB_PIN_12_STATE__);
_PIN_CONFIGURATION(GPIOB, 13, __GPIO_PB_PIN_13_NAME__, __GPIO_PB_PIN_13_MODE__, __GPIO_PB_PIN_13_STATE__);
_PIN_CONFIGURATION(GPIOB, 14, __GPIO_PB_PIN_14_NAME__, __GPIO_PB_PIN_14_MODE__, __GPIO_PB_PIN_14_STATE__);
_PIN_CONFIGURATION(GPIOB, 15, __GPIO_PB_PIN_15_NAME__, __GPIO_PB_PIN_15_MODE__, __GPIO_PB_PIN_15_STATE__);
#endif

/** GPIOC pins configuration */
#if _GPIOC_EN
_PIN_CONFIGURATION(GPIOC,  0, __GPIO_PC_PIN_0_NAME__ , __GPIO_PC_PIN_0_MODE__ , __GPIO_PC_PIN_0_STATE__ );
_PIN_CONFIGURATION(GPIOC,  1, __GPIO_PC_PIN_1_NAME__ , __GPIO_PC_PIN_1_MODE__ , __GPIO_PC_PIN_1_STATE__ );
_PIN_CONFIGURATION(GPIOC,  2, __GPIO_PC_PIN_2_NAME__ , __GPIO_PC_PIN_2_MODE__ , __GPIO_PC_PIN_2_STATE__ );
_PIN_CONFIGURATION(GPIOC,  3, __GPIO_PC_PIN_3_NAME__ , __GPIO_PC_PIN_3_MODE__ , __GPIO_PC_PIN_3_STATE__ );
_PIN_CONFIGURATION(GPIOC,  4, __GPIO_PC_PIN_4_NAME__ , __GPIO_PC_PIN_4_MODE__ , __GPIO_PC_PIN_4_STATE__ );
_PIN_CONFIGURATION(GPIOC,  5, __GPIO_PC_PIN_5_NAME__ , __GPIO_PC_PIN_5_MODE__ , __GPIO_PC_PIN_5_STATE__ );
_PIN_CONFIGURATION(GPIOC,  6, __GPIO_PC_PIN_6_NAME__ , __GPIO_PC_PIN_6_MODE__ , __GPIO_PC_PIN_6_STATE__ );
_PIN_CONFIGURATION(GPIOC,  7, __GPIO_PC_PIN_7_NAME__ , __GPIO_PC_PIN_7_MODE__ , __GPIO_PC_PIN_7_STATE__ );
_PIN_CONFIGURATION(GPIOC,  8, __GPIO_PC_PIN_8_NAME__ , __GPIO_PC_PIN_8_MODE__ , __GPIO_PC_PIN_8_STATE__ );
_PIN_CONFIGURATION(GPIOC,  9, __GPIO_PC_PIN_9_NAME__ , __GPIO_PC_PIN_9_MODE__ , __GPIO_PC_PIN_9_STATE__ );
_PIN_CONFIGURATION(GPIOC, 10, __GPIO_PC_PIN_10_NAME__, __GPIO_PC_PIN_10_MODE__, __GPIO_PC_PIN_10_STATE__);
_PIN_CONFIGURATION(GPIOC, 11, __GPIO_PC_PIN_11_NAME__, __GPIO_PC_PIN_11_MODE__, __GPIO_PC_PIN_11_STATE__);
_PIN_CONFIGURATION(GPIOC, 12, __GPIO_PC_PIN_12_NAME__, __GPIO_PC_PIN_12_MODE__, __GPIO_PC_PIN_12_STATE__);
_PIN_CONFIGURATION(GPIOC, 13, __GPIO_PC_PIN_13_NAME__, __GPIO_PC_PIN_13_MODE__, __GPIO_PC_PIN_13_STATE__);
_PIN_CONFIGURATION(GPIOC, 14, __GPIO_PC_PIN_14_NAME__, __GPIO_PC_PIN_14_MODE__, __GPIO_PC_PIN_14_STATE__);
_PIN_CONFIGURATION(GPIOC, 15, __GPIO_PC_PIN_15_NAME__, __GPIO_PC_PIN_15_MODE__, __GPIO_PC_PIN_15_STATE__);
#endif

/** GPIOD pins configuration */
#if _GPIOD_EN
_PIN_CONFIGURATION(GPIOD,  0, __GPIO_PD_PIN_0_NAME__ , __GPIO_PD_PIN_0_MODE__ , __GPIO_PD_PIN_0_STATE__ );
_PIN_CONFIGURATION(GPIOD,  1, __GPIO_PD_PIN_1_NAME__ , __GPIO_PD_PIN_1_MODE__ , __GPIO_PD_PIN_1_STATE__ );
_PIN_CONFIGURATION(GPIOD,  2, __GPIO_PD_PIN_2_NAME__ , __GPIO_PD_PIN_2_MODE__ , __GPIO_PD_PIN_2_STATE__ );
_PIN_CONFIGURATION(GPIOD,  3, __GPIO_PD_PIN_3_NAME__ , __GPIO_PD_PIN_3_MODE__ , __GPIO_PD_PIN_3_STATE__ );
_PIN_CONFIGURATION(GPIOD,  4, __GPIO_PD_PIN_4_NAME__ , __GPIO_PD_PIN_4_MODE__ , __GPIO_PD_PIN_4_STATE__ );
_PIN_CONFIGURATION(GPIOD,  5, __GPIO_PD_PIN_5_NAME__ , __GPIO_PD_PIN_5_MODE__ , __GPIO_PD_PIN_5_STATE__ );
_PIN_CONFIGURATION(GPIOD,  6, __GPIO_PD_PIN_6_NAME__ , __GPIO_PD_PIN_6_MODE__ , __GPIO_PD_PIN_6_STATE__ );
_PIN_CONFIGURATION(GPIOD,  7, __GPIO_PD_PIN_7_NAME__ , __GPIO_PD_PIN_7_MODE__ , __GPIO_PD_PIN_7_STATE__ );
_PIN_CONFIGURATION(GPIOD,  8, __GPIO_PD_PIN_8_NAME__ , __GPIO_PD_PIN_8_MODE__ , __GPIO_PD_PIN_8_STATE__ );
_PIN_CONFIGURATION(GPIOD,  9, __GPIO_PD_PIN_9_NAME__ , __GPIO_PD_PIN_9_MODE__ , __GPIO_PD_PIN_9_STATE__ );
_PIN_CONFIGURATION(GPIOD, 10, __GPIO_PD_PIN_10_NAME__, __GPIO_PD_PIN_10_MODE__, __GPIO_PD_PIN_10_STATE__);
_PIN_CONFIGURATION(GPIOD, 11, __GPIO_PD_PIN_11_NAME__, __GPIO_PD_PIN_11_MODE__, __GPIO_PD_PIN_11_STATE__);
_PIN_CONFIGURATION(GPIOD, 12, __GPIO_PD_PIN_12_NAME__, __GPIO_PD_PIN_12_MODE__, __GPIO_PD_PIN_12_STATE__);
_PIN_CONFIGURATION(GPIOD, 13, __GPIO_PD_PIN_13_NAME__, __GPIO_PD_PIN_13_MODE__, __GPIO_PD_PIN_13_STATE__);
_PIN_CONFIGURATION(GPIOD, 14, __GPIO_PD_PIN_14_NAME__, __GPIO_PD_PIN_14_MODE__, __GPIO_PD_PIN_14_STATE__);
_PIN_CONFIGURATION(GPIOD, 15, __GPIO_PD_PIN_15_NAME__, __GPIO_PD_PIN_15_MODE__, __GPIO_PD_PIN_15_STATE__);
#endif

/** GPIOE pins configuration */
#if _GPIOE_EN
_PIN_CONFIGURATION(GPIOE,  0, __GPIO_PE_PIN_0_NAME__ , __GPIO_PE_PIN_0_MODE__ , __GPIO_PE_PIN_0_STATE__ );
_PIN_CONFIGURATION(GPIOE,  1, __GPIO_PE_PIN_1_NAME__ , __GPIO_PE_PIN_1_MODE__ , __GPIO_PE_PIN_1_STATE__ );
_PIN_CONFIGURATION(GPIOE,  2, __GPIO_PE_PIN_2_NAME__ , __GPIO_PE_PIN_2_MODE__ , __GPIO_PE_PIN_2_STATE__ );
_PIN_CONFIGURATION(GPIOE,  3, __GPIO_PE_PIN_3_NAME__ , __GPIO_PE_PIN_3_MODE__ , __GPIO_PE_PIN_3_STATE__ );
_PIN_CONFIGURATION(GPIOE,  4, __GPIO_PE_PIN_4_NAME__ , __GPIO_PE_PIN_4_MODE__ , __GPIO_PE_PIN_4_STATE__ );
_PIN_CONFIGURATION(GPIOE,  5, __GPIO_PE_PIN_5_NAME__ , __GPIO_PE_PIN_5_MODE__ , __GPIO_PE_PIN_5_STATE__ );
_PIN_CONFIGURATION(GPIOE,  6, __GPIO_PE_PIN_6_NAME__ , __GPIO_PE_PIN_6_MODE__ , __GPIO_PE_PIN_6_STATE__ );
_PIN_CONFIGURATION(GPIOE,  7, __GPIO_PE_PIN_7_NAME__ , __GPIO_PE_PIN_7_MODE__ , __GPIO_PE_PIN_7_STATE__ );
_PIN_CONFIGURATION(GPIOE,  8, __GPIO_PE_PIN_8_NAME__ , __GPIO_PE_PIN_8_MODE__ , __GPIO_PE_PIN_8_STATE__ );
_PIN_CONFIGURATION(GPIOE,  9, __GPIO_PE_PIN_9_NAME__ , __GPIO_PE_PIN_9_MODE__ , __GPIO_PE_PIN_9_STATE__ );
_PIN_CONFIGURATION(GPIOE, 10, __GPIO_PE_PIN_10_NAME__, __GPIO_PE_PIN_10_MODE__, __GPIO_PE_PIN_10_STATE__);
_PIN_CONFIGURATION(GPIOE, 11, __GPIO_PE_PIN_11_NAME__, __GPIO_PE_PIN_11_MODE__, __GPIO_PE_PIN_11_STATE__);
_PIN_CONFIGURATION(GPIOE, 12, __GPIO_PE_PIN_12_NAME__, __GPIO_PE_PIN_12_MODE__, __GPIO_PE_PIN_12_STATE__);
_PIN_CONFIGURATION(GPIOE, 13, __GPIO_PE_PIN_13_NAME__, __GPIO_PE_PIN_13_MODE__, __GPIO_PE_PIN_13_STATE__);
_PIN_CONFIGURATION(GPIOE, 14, __GPIO_PE_PIN_14_NAME__, __GPIO_PE_PIN_14_MODE__, __GPIO_PE_PIN_14_STATE__);
_PIN_CONFIGURATION(GPIOE, 15, __GPIO_PE_PIN_15_NAME__, __GPIO_PE_PIN_15_MODE__, __GPIO_PE_PIN_15_STATE__);
#endif

/** GPIOF pins configuration */
#if _GPIOF_EN
_PIN_CONFIGURATION(GPIOF,  0, __GPIO_PF_PIN_0_NAME__ , __GPIO_PF_PIN_0_MODE__ , __GPIO_PF_PIN_0_STATE__ );
_PIN_CONFIGURATION(GPIOF,  1, __GPIO_PF_PIN_1_NAME__ , __GPIO_PF_PIN_1_MODE__ , __GPIO_PF_PIN_1_STATE__ );
_PIN_CONFIGURATION(GPIOF,  2, __GPIO_PF_PIN_2_NAME__ , __GPIO_PF_PIN_2_MODE__ , __GPIO_PF_PIN_2_STATE__ );
_PIN_CONFIGURATION(GPIOF,  3, __GPIO_PF_PIN_3_NAME__ , __GPIO_PF_PIN_3_MODE__ , __GPIO_PF_PIN_3_STATE__ );
_PIN_CONFIGURATION(GPIOF,  4, __GPIO_PF_PIN_4_NAME__ , __GPIO_PF_PIN_4_MODE__ , __GPIO_PF_PIN_4_STATE__ );
_PIN_CONFIGURATION(GPIOF,  5, __GPIO_PF_PIN_5_NAME__ , __GPIO_PF_PIN_5_MODE__ , __GPIO_PF_PIN_5_STATE__ );
_PIN_CONFIGURATION(GPIOF,  6, __GPIO_PF_PIN_6_NAME__ , __GPIO_PF_PIN_6_MODE__ , __GPIO_PF_PIN_6_STATE__ );
_PIN_CONFIGURATION(GPIOF,  7, __GPIO_PF_PIN_7_NAME__ , __GPIO_PF_PIN_7_MODE__ , __GPIO_PF_PIN_7_STATE__ );
_PIN_CONFIGURATION(GPIOF,  8, __GPIO_PF_PIN_8_NAME__ , __GPIO_PF_PIN_8_MODE__ , __GPIO_PF_PIN_8_STATE__ );
_PIN_CONFIGURATION(GPIOF,  9, __GPIO_PF_PIN_9_NAME__ , __GPIO_PF_PIN_9_MODE__ , __GPIO_PF_PIN_9_STATE__ );
_PIN_CONFIGURATION(GPIOF, 10, __GPIO_PF_PIN_10_NAME__, __GPIO_PF_PIN_10_MODE__, __GPIO_PF_PIN_10_STATE__);
_PIN_CONFIGURATION(GPIOF, 11, __GPIO_PF_PIN_11_NAME__, __GPIO_PF_PIN_11_MODE__, __GPIO_PF_PIN_11_STATE__);
_PIN_CONFIGURATION(GPIOF, 12, __GPIO_PF_PIN_12_NAME__, __GPIO_PF_PIN_12_MODE__, __GPIO_PF_PIN_12_STATE__);
_PIN_CONFIGURATION(GPIOF, 13, __GPIO_PF_PIN_13_NAME__, __GPIO_PF_PIN_13_MODE__, __GPIO_PF_PIN_13_STATE__);
_PIN_CONFIGURATION(GPIOF, 14, __GPIO_PF_PIN_14_NAME__, __GPIO_PF_PIN_14_MODE__, __GPIO_PF_PIN_14_STATE__);
_PIN_CONFIGURATION(GPIOF, 15, __GPIO_PF_PIN_15_NAME__, __GPIO_PF_PIN_15_MODE__, __GPIO_PF_PIN_15_STATE__);
#endif

/** GPIOG pins configuration */
#if _GPIOG_EN
_PIN_CONFIGURATION(GPIOG,  0, __GPIO_PG_PIN_0_NAME__ , __GPIO_PG_PIN_0_MODE__ , __GPIO_PG_PIN_0_STATE__ );
_PIN_CONFIGURATION(GPIOG,  1, __GPIO_PG_PIN_1_NAME__ , __GPIO_PG_PIN_1_MODE__ , __GPIO_PG_PIN_1_STATE__ );
_PIN_CONFIGURATION(GPIOG,  2, __GPIO_PG_PIN_2_NAME__ , __GPIO_PG_PIN_2_MODE__ , __GPIO_PG_PIN_2_STATE__ );
_PIN_CONFIGURATION(GPIOG,  3, __GPIO_PG_PIN_3_NAME__ , __GPIO_PG_PIN_3_MODE__ , __GPIO_PG_PIN_3_STATE__ );
_PIN_CONFIGURATION(GPIOG,  4, __GPIO_PG_PIN_4_NAME__ , __GPIO_PG_PIN_4_MODE__ , __GPIO_PG_PIN_4_STATE__ );
_PIN_CONFIGURATION(GPIOG,  5, __GPIO_PG_PIN_5_NAME__ , __GPIO_PG_PIN_5_MODE__ , __GPIO_PG_PIN_5_STATE__ );
_PIN_CONFIGURATION(GPIOG,  6, __GPIO_PG_PIN_6_NAME__ , __GPIO_PG_PIN_6_MODE__ , __GPIO_PG_PIN_6_STATE__ );
_PIN_CONFIGURATION(GPIOG,  7, __GPIO_PG_PIN_7_NAME__ , __GPIO_PG_PIN_7_MODE__ , __GPIO_PG_PIN_7_STATE__ );
_PIN_CONFIGURATION(GPIOG,  8, __GPIO_PG_PIN_8_NAME__ , __GPIO_PG_PIN_8_MODE__ , __GPIO_PG_PIN_8_STATE__ );
_PIN_CONFIGURATION(GPIOG,  9, __GPIO_PG_PIN_9_NAME__ , __GPIO_PG_PIN_9_MODE__ , __GPIO_PG_PIN_9_STATE__ );
_PIN_CONFIGURATION(GPIOG, 10, __GPIO_PG_PIN_10_NAME__, __GPIO_PG_PIN_10_MODE__, __GPIO_PG_PIN_10_STATE__);
_PIN_CONFIGURATION(GPIOG, 11, __GPIO_PG_PIN_11_NAME__, __GPIO_PG_PIN_11_MODE__, __GPIO_PG_PIN_11_STATE__);
_PIN_CONFIGURATION(GPIOG, 12, __GPIO_PG_PIN_12_NAME__, __GPIO_PG_PIN_12_MODE__, __GPIO_PG_PIN_12_STATE__);
_PIN_CONFIGURATION(GPIOG, 13, __GPIO_PG_PIN_13_NAME__, __GPIO_PG_PIN_13_MODE__, __GPIO_PG_PIN_13_STATE__);
_PIN_CONFIGURATION(GPIOG, 14, __GPIO_PG_PIN_14_NAME__, __GPIO_PG_PIN_14_MODE__, __GPIO_PG_PIN_14_STATE__);
_PIN_CONFIGURATION(GPIOG, 15, __GPIO_PG_PIN_15_NAME__, __GPIO_PG_PIN_15_MODE__, __GPIO_PG_PIN_15_STATE__);
#endif

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

#endif /* _GPIO_CFG_H_ */
/*==============================================================================
  End of file
==============================================================================*/
