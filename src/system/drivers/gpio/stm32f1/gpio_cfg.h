/*=========================================================================*//**
@file    gpio_cfg.h

@author  Daniel Zorychta

@brief   This driver support GPIO. Set here driver configuration.

@note    Copyright (C) 2012  Daniel Zorychta <daniel.zorychta@gmail.com>

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
#define _PIN_CONFIGURATION(port, port_idx, number, pin_name, mode, state) \
enum port##_##number##_CFG {\
        _CONCAT(IOCTL_GPIO_PIN_IDX__, pin_name) = number,\
        _CONCAT(IOCTL_GPIO_PIN_MASK__, pin_name) = (1 << (number)),\
        _CONCAT(IOCTL_GPIO_PORT_IDX__, pin_name) = port_idx,\
        _CONCAT(_GPIO_, pin_name) = port##_BASE,\
        _CONCAT(_BP_, pin_name) = (number),\
        _CONCAT(_BM_, pin_name) = (1 << (number)),\
        _##port##_PIN_##number##_MODE  = (mode),\
        _##port##_PIN_##number##_STATE = (state)\
}

/** GPIO pin NONE definition */
#define _GPIO_NONE_BASE 0
_PIN_CONFIGURATION(_GPIO_NONE, 16, 17, NONE, _GPIO_ANALOG, _GPIO_IN_FLOAT);

/** GPIOA pins configuration */
#if defined(RCC_APB2ENR_IOPAEN)
_PIN_CONFIGURATION(GPIOA, 0,  0, __GPIO_PA_PIN_0_NAME__ , __GPIO_PA_PIN_0_MODE__ , __GPIO_PA_PIN_0_STATE__ );
_PIN_CONFIGURATION(GPIOA, 0,  1, __GPIO_PA_PIN_1_NAME__ , __GPIO_PA_PIN_1_MODE__ , __GPIO_PA_PIN_1_STATE__ );
_PIN_CONFIGURATION(GPIOA, 0,  2, __GPIO_PA_PIN_2_NAME__ , __GPIO_PA_PIN_2_MODE__ , __GPIO_PA_PIN_2_STATE__ );
_PIN_CONFIGURATION(GPIOA, 0,  3, __GPIO_PA_PIN_3_NAME__ , __GPIO_PA_PIN_3_MODE__ , __GPIO_PA_PIN_3_STATE__ );
_PIN_CONFIGURATION(GPIOA, 0,  4, __GPIO_PA_PIN_4_NAME__ , __GPIO_PA_PIN_4_MODE__ , __GPIO_PA_PIN_4_STATE__ );
_PIN_CONFIGURATION(GPIOA, 0,  5, __GPIO_PA_PIN_5_NAME__ , __GPIO_PA_PIN_5_MODE__ , __GPIO_PA_PIN_5_STATE__ );
_PIN_CONFIGURATION(GPIOA, 0,  6, __GPIO_PA_PIN_6_NAME__ , __GPIO_PA_PIN_6_MODE__ , __GPIO_PA_PIN_6_STATE__ );
_PIN_CONFIGURATION(GPIOA, 0,  7, __GPIO_PA_PIN_7_NAME__ , __GPIO_PA_PIN_7_MODE__ , __GPIO_PA_PIN_7_STATE__ );
_PIN_CONFIGURATION(GPIOA, 0,  8, __GPIO_PA_PIN_8_NAME__ , __GPIO_PA_PIN_8_MODE__ , __GPIO_PA_PIN_8_STATE__ );
_PIN_CONFIGURATION(GPIOA, 0,  9, __GPIO_PA_PIN_9_NAME__ , __GPIO_PA_PIN_9_MODE__ , __GPIO_PA_PIN_9_STATE__ );
_PIN_CONFIGURATION(GPIOA, 0, 10, __GPIO_PA_PIN_10_NAME__, __GPIO_PA_PIN_10_MODE__, __GPIO_PA_PIN_10_STATE__);
_PIN_CONFIGURATION(GPIOA, 0, 11, __GPIO_PA_PIN_11_NAME__, __GPIO_PA_PIN_11_MODE__, __GPIO_PA_PIN_11_STATE__);
_PIN_CONFIGURATION(GPIOA, 0, 12, __GPIO_PA_PIN_12_NAME__, __GPIO_PA_PIN_12_MODE__, __GPIO_PA_PIN_12_STATE__);
_PIN_CONFIGURATION(GPIOA, 0, 13, __GPIO_PA_PIN_13_NAME__, __GPIO_PA_PIN_13_MODE__, __GPIO_PA_PIN_13_STATE__);
_PIN_CONFIGURATION(GPIOA, 0, 14, __GPIO_PA_PIN_14_NAME__, __GPIO_PA_PIN_14_MODE__, __GPIO_PA_PIN_14_STATE__);
_PIN_CONFIGURATION(GPIOA, 0, 15, __GPIO_PA_PIN_15_NAME__, __GPIO_PA_PIN_15_MODE__, __GPIO_PA_PIN_15_STATE__);
#endif

/** GPIOB pins configuration */
#if defined(RCC_APB2ENR_IOPBEN)
_PIN_CONFIGURATION(GPIOB, 1,  0, __GPIO_PB_PIN_0_NAME__ , __GPIO_PB_PIN_0_MODE__ , __GPIO_PB_PIN_0_STATE__ );
_PIN_CONFIGURATION(GPIOB, 1,  1, __GPIO_PB_PIN_1_NAME__ , __GPIO_PB_PIN_1_MODE__ , __GPIO_PB_PIN_1_STATE__ );
_PIN_CONFIGURATION(GPIOB, 1,  2, __GPIO_PB_PIN_2_NAME__ , __GPIO_PB_PIN_2_MODE__ , __GPIO_PB_PIN_2_STATE__ );
_PIN_CONFIGURATION(GPIOB, 1,  3, __GPIO_PB_PIN_3_NAME__ , __GPIO_PB_PIN_3_MODE__ , __GPIO_PB_PIN_3_STATE__ );
_PIN_CONFIGURATION(GPIOB, 1,  4, __GPIO_PB_PIN_4_NAME__ , __GPIO_PB_PIN_4_MODE__ , __GPIO_PB_PIN_4_STATE__ );
_PIN_CONFIGURATION(GPIOB, 1,  5, __GPIO_PB_PIN_5_NAME__ , __GPIO_PB_PIN_5_MODE__ , __GPIO_PB_PIN_5_STATE__ );
_PIN_CONFIGURATION(GPIOB, 1,  6, __GPIO_PB_PIN_6_NAME__ , __GPIO_PB_PIN_6_MODE__ , __GPIO_PB_PIN_6_STATE__ );
_PIN_CONFIGURATION(GPIOB, 1,  7, __GPIO_PB_PIN_7_NAME__ , __GPIO_PB_PIN_7_MODE__ , __GPIO_PB_PIN_7_STATE__ );
_PIN_CONFIGURATION(GPIOB, 1,  8, __GPIO_PB_PIN_8_NAME__ , __GPIO_PB_PIN_8_MODE__ , __GPIO_PB_PIN_8_STATE__ );
_PIN_CONFIGURATION(GPIOB, 1,  9, __GPIO_PB_PIN_9_NAME__ , __GPIO_PB_PIN_9_MODE__ , __GPIO_PB_PIN_9_STATE__ );
_PIN_CONFIGURATION(GPIOB, 1, 10, __GPIO_PB_PIN_10_NAME__, __GPIO_PB_PIN_10_MODE__, __GPIO_PB_PIN_10_STATE__);
_PIN_CONFIGURATION(GPIOB, 1, 11, __GPIO_PB_PIN_11_NAME__, __GPIO_PB_PIN_11_MODE__, __GPIO_PB_PIN_11_STATE__);
_PIN_CONFIGURATION(GPIOB, 1, 12, __GPIO_PB_PIN_12_NAME__, __GPIO_PB_PIN_12_MODE__, __GPIO_PB_PIN_12_STATE__);
_PIN_CONFIGURATION(GPIOB, 1, 13, __GPIO_PB_PIN_13_NAME__, __GPIO_PB_PIN_13_MODE__, __GPIO_PB_PIN_13_STATE__);
_PIN_CONFIGURATION(GPIOB, 1, 14, __GPIO_PB_PIN_14_NAME__, __GPIO_PB_PIN_14_MODE__, __GPIO_PB_PIN_14_STATE__);
_PIN_CONFIGURATION(GPIOB, 1, 15, __GPIO_PB_PIN_15_NAME__, __GPIO_PB_PIN_15_MODE__, __GPIO_PB_PIN_15_STATE__);
#endif

/** GPIOC pins configuration */
#if defined(RCC_APB2ENR_IOPCEN)
_PIN_CONFIGURATION(GPIOC, 2,  0, __GPIO_PC_PIN_0_NAME__ , __GPIO_PC_PIN_0_MODE__ , __GPIO_PC_PIN_0_STATE__ );
_PIN_CONFIGURATION(GPIOC, 2,  1, __GPIO_PC_PIN_1_NAME__ , __GPIO_PC_PIN_1_MODE__ , __GPIO_PC_PIN_1_STATE__ );
_PIN_CONFIGURATION(GPIOC, 2,  2, __GPIO_PC_PIN_2_NAME__ , __GPIO_PC_PIN_2_MODE__ , __GPIO_PC_PIN_2_STATE__ );
_PIN_CONFIGURATION(GPIOC, 2,  3, __GPIO_PC_PIN_3_NAME__ , __GPIO_PC_PIN_3_MODE__ , __GPIO_PC_PIN_3_STATE__ );
_PIN_CONFIGURATION(GPIOC, 2,  4, __GPIO_PC_PIN_4_NAME__ , __GPIO_PC_PIN_4_MODE__ , __GPIO_PC_PIN_4_STATE__ );
_PIN_CONFIGURATION(GPIOC, 2,  5, __GPIO_PC_PIN_5_NAME__ , __GPIO_PC_PIN_5_MODE__ , __GPIO_PC_PIN_5_STATE__ );
_PIN_CONFIGURATION(GPIOC, 2,  6, __GPIO_PC_PIN_6_NAME__ , __GPIO_PC_PIN_6_MODE__ , __GPIO_PC_PIN_6_STATE__ );
_PIN_CONFIGURATION(GPIOC, 2,  7, __GPIO_PC_PIN_7_NAME__ , __GPIO_PC_PIN_7_MODE__ , __GPIO_PC_PIN_7_STATE__ );
_PIN_CONFIGURATION(GPIOC, 2,  8, __GPIO_PC_PIN_8_NAME__ , __GPIO_PC_PIN_8_MODE__ , __GPIO_PC_PIN_8_STATE__ );
_PIN_CONFIGURATION(GPIOC, 2,  9, __GPIO_PC_PIN_9_NAME__ , __GPIO_PC_PIN_9_MODE__ , __GPIO_PC_PIN_9_STATE__ );
_PIN_CONFIGURATION(GPIOC, 2, 10, __GPIO_PC_PIN_10_NAME__, __GPIO_PC_PIN_10_MODE__, __GPIO_PC_PIN_10_STATE__);
_PIN_CONFIGURATION(GPIOC, 2, 11, __GPIO_PC_PIN_11_NAME__, __GPIO_PC_PIN_11_MODE__, __GPIO_PC_PIN_11_STATE__);
_PIN_CONFIGURATION(GPIOC, 2, 12, __GPIO_PC_PIN_12_NAME__, __GPIO_PC_PIN_12_MODE__, __GPIO_PC_PIN_12_STATE__);
_PIN_CONFIGURATION(GPIOC, 2, 13, __GPIO_PC_PIN_13_NAME__, __GPIO_PC_PIN_13_MODE__, __GPIO_PC_PIN_13_STATE__);
_PIN_CONFIGURATION(GPIOC, 2, 14, __GPIO_PC_PIN_14_NAME__, __GPIO_PC_PIN_14_MODE__, __GPIO_PC_PIN_14_STATE__);
_PIN_CONFIGURATION(GPIOC, 2, 15, __GPIO_PC_PIN_15_NAME__, __GPIO_PC_PIN_15_MODE__, __GPIO_PC_PIN_15_STATE__);
#endif

/** GPIOD pins configuration */
#if defined(RCC_APB2ENR_IOPDEN)
_PIN_CONFIGURATION(GPIOD, 3,  0, __GPIO_PD_PIN_0_NAME__ , __GPIO_PD_PIN_0_MODE__ , __GPIO_PD_PIN_0_STATE__ );
_PIN_CONFIGURATION(GPIOD, 3,  1, __GPIO_PD_PIN_1_NAME__ , __GPIO_PD_PIN_1_MODE__ , __GPIO_PD_PIN_1_STATE__ );
_PIN_CONFIGURATION(GPIOD, 3,  2, __GPIO_PD_PIN_2_NAME__ , __GPIO_PD_PIN_2_MODE__ , __GPIO_PD_PIN_2_STATE__ );
_PIN_CONFIGURATION(GPIOD, 3,  3, __GPIO_PD_PIN_3_NAME__ , __GPIO_PD_PIN_3_MODE__ , __GPIO_PD_PIN_3_STATE__ );
_PIN_CONFIGURATION(GPIOD, 3,  4, __GPIO_PD_PIN_4_NAME__ , __GPIO_PD_PIN_4_MODE__ , __GPIO_PD_PIN_4_STATE__ );
_PIN_CONFIGURATION(GPIOD, 3,  5, __GPIO_PD_PIN_5_NAME__ , __GPIO_PD_PIN_5_MODE__ , __GPIO_PD_PIN_5_STATE__ );
_PIN_CONFIGURATION(GPIOD, 3,  6, __GPIO_PD_PIN_6_NAME__ , __GPIO_PD_PIN_6_MODE__ , __GPIO_PD_PIN_6_STATE__ );
_PIN_CONFIGURATION(GPIOD, 3,  7, __GPIO_PD_PIN_7_NAME__ , __GPIO_PD_PIN_7_MODE__ , __GPIO_PD_PIN_7_STATE__ );
_PIN_CONFIGURATION(GPIOD, 3,  8, __GPIO_PD_PIN_8_NAME__ , __GPIO_PD_PIN_8_MODE__ , __GPIO_PD_PIN_8_STATE__ );
_PIN_CONFIGURATION(GPIOD, 3,  9, __GPIO_PD_PIN_9_NAME__ , __GPIO_PD_PIN_9_MODE__ , __GPIO_PD_PIN_9_STATE__ );
_PIN_CONFIGURATION(GPIOD, 3, 10, __GPIO_PD_PIN_10_NAME__, __GPIO_PD_PIN_10_MODE__, __GPIO_PD_PIN_10_STATE__);
_PIN_CONFIGURATION(GPIOD, 3, 11, __GPIO_PD_PIN_11_NAME__, __GPIO_PD_PIN_11_MODE__, __GPIO_PD_PIN_11_STATE__);
_PIN_CONFIGURATION(GPIOD, 3, 12, __GPIO_PD_PIN_12_NAME__, __GPIO_PD_PIN_12_MODE__, __GPIO_PD_PIN_12_STATE__);
_PIN_CONFIGURATION(GPIOD, 3, 13, __GPIO_PD_PIN_13_NAME__, __GPIO_PD_PIN_13_MODE__, __GPIO_PD_PIN_13_STATE__);
_PIN_CONFIGURATION(GPIOD, 3, 14, __GPIO_PD_PIN_14_NAME__, __GPIO_PD_PIN_14_MODE__, __GPIO_PD_PIN_14_STATE__);
_PIN_CONFIGURATION(GPIOD, 3, 15, __GPIO_PD_PIN_15_NAME__, __GPIO_PD_PIN_15_MODE__, __GPIO_PD_PIN_15_STATE__);
#endif

/** GPIOE pins configuration */
#if defined(RCC_APB2ENR_IOPEEN)
_PIN_CONFIGURATION(GPIOE, 4,  0, __GPIO_PE_PIN_0_NAME__ , __GPIO_PE_PIN_0_MODE__ , __GPIO_PE_PIN_0_STATE__ );
_PIN_CONFIGURATION(GPIOE, 4,  1, __GPIO_PE_PIN_1_NAME__ , __GPIO_PE_PIN_1_MODE__ , __GPIO_PE_PIN_1_STATE__ );
_PIN_CONFIGURATION(GPIOE, 4,  2, __GPIO_PE_PIN_2_NAME__ , __GPIO_PE_PIN_2_MODE__ , __GPIO_PE_PIN_2_STATE__ );
_PIN_CONFIGURATION(GPIOE, 4,  3, __GPIO_PE_PIN_3_NAME__ , __GPIO_PE_PIN_3_MODE__ , __GPIO_PE_PIN_3_STATE__ );
_PIN_CONFIGURATION(GPIOE, 4,  4, __GPIO_PE_PIN_4_NAME__ , __GPIO_PE_PIN_4_MODE__ , __GPIO_PE_PIN_4_STATE__ );
_PIN_CONFIGURATION(GPIOE, 4,  5, __GPIO_PE_PIN_5_NAME__ , __GPIO_PE_PIN_5_MODE__ , __GPIO_PE_PIN_5_STATE__ );
_PIN_CONFIGURATION(GPIOE, 4,  6, __GPIO_PE_PIN_6_NAME__ , __GPIO_PE_PIN_6_MODE__ , __GPIO_PE_PIN_6_STATE__ );
_PIN_CONFIGURATION(GPIOE, 4,  7, __GPIO_PE_PIN_7_NAME__ , __GPIO_PE_PIN_7_MODE__ , __GPIO_PE_PIN_7_STATE__ );
_PIN_CONFIGURATION(GPIOE, 4,  8, __GPIO_PE_PIN_8_NAME__ , __GPIO_PE_PIN_8_MODE__ , __GPIO_PE_PIN_8_STATE__ );
_PIN_CONFIGURATION(GPIOE, 4,  9, __GPIO_PE_PIN_9_NAME__ , __GPIO_PE_PIN_9_MODE__ , __GPIO_PE_PIN_9_STATE__ );
_PIN_CONFIGURATION(GPIOE, 4, 10, __GPIO_PE_PIN_10_NAME__, __GPIO_PE_PIN_10_MODE__, __GPIO_PE_PIN_10_STATE__);
_PIN_CONFIGURATION(GPIOE, 4, 11, __GPIO_PE_PIN_11_NAME__, __GPIO_PE_PIN_11_MODE__, __GPIO_PE_PIN_11_STATE__);
_PIN_CONFIGURATION(GPIOE, 4, 12, __GPIO_PE_PIN_12_NAME__, __GPIO_PE_PIN_12_MODE__, __GPIO_PE_PIN_12_STATE__);
_PIN_CONFIGURATION(GPIOE, 4, 13, __GPIO_PE_PIN_13_NAME__, __GPIO_PE_PIN_13_MODE__, __GPIO_PE_PIN_13_STATE__);
_PIN_CONFIGURATION(GPIOE, 4, 14, __GPIO_PE_PIN_14_NAME__, __GPIO_PE_PIN_14_MODE__, __GPIO_PE_PIN_14_STATE__);
_PIN_CONFIGURATION(GPIOE, 4, 15, __GPIO_PE_PIN_15_NAME__, __GPIO_PE_PIN_15_MODE__, __GPIO_PE_PIN_15_STATE__);
#endif

/** GPIOF pins configuration */
#if defined(RCC_APB2ENR_IOPFEN)
_PIN_CONFIGURATION(GPIOF, 5,  0, __GPIO_PF_PIN_0_NAME__ , __GPIO_PF_PIN_0_MODE__ , __GPIO_PF_PIN_0_STATE__ );
_PIN_CONFIGURATION(GPIOF, 5,  1, __GPIO_PF_PIN_1_NAME__ , __GPIO_PF_PIN_1_MODE__ , __GPIO_PF_PIN_1_STATE__ );
_PIN_CONFIGURATION(GPIOF, 5,  2, __GPIO_PF_PIN_2_NAME__ , __GPIO_PF_PIN_2_MODE__ , __GPIO_PF_PIN_2_STATE__ );
_PIN_CONFIGURATION(GPIOF, 5,  3, __GPIO_PF_PIN_3_NAME__ , __GPIO_PF_PIN_3_MODE__ , __GPIO_PF_PIN_3_STATE__ );
_PIN_CONFIGURATION(GPIOF, 5,  4, __GPIO_PF_PIN_4_NAME__ , __GPIO_PF_PIN_4_MODE__ , __GPIO_PF_PIN_4_STATE__ );
_PIN_CONFIGURATION(GPIOF, 5,  5, __GPIO_PF_PIN_5_NAME__ , __GPIO_PF_PIN_5_MODE__ , __GPIO_PF_PIN_5_STATE__ );
_PIN_CONFIGURATION(GPIOF, 5,  6, __GPIO_PF_PIN_6_NAME__ , __GPIO_PF_PIN_6_MODE__ , __GPIO_PF_PIN_6_STATE__ );
_PIN_CONFIGURATION(GPIOF, 5,  7, __GPIO_PF_PIN_7_NAME__ , __GPIO_PF_PIN_7_MODE__ , __GPIO_PF_PIN_7_STATE__ );
_PIN_CONFIGURATION(GPIOF, 5,  8, __GPIO_PF_PIN_8_NAME__ , __GPIO_PF_PIN_8_MODE__ , __GPIO_PF_PIN_8_STATE__ );
_PIN_CONFIGURATION(GPIOF, 5,  9, __GPIO_PF_PIN_9_NAME__ , __GPIO_PF_PIN_9_MODE__ , __GPIO_PF_PIN_9_STATE__ );
_PIN_CONFIGURATION(GPIOF, 5, 10, __GPIO_PF_PIN_10_NAME__, __GPIO_PF_PIN_10_MODE__, __GPIO_PF_PIN_10_STATE__);
_PIN_CONFIGURATION(GPIOF, 5, 11, __GPIO_PF_PIN_11_NAME__, __GPIO_PF_PIN_11_MODE__, __GPIO_PF_PIN_11_STATE__);
_PIN_CONFIGURATION(GPIOF, 5, 12, __GPIO_PF_PIN_12_NAME__, __GPIO_PF_PIN_12_MODE__, __GPIO_PF_PIN_12_STATE__);
_PIN_CONFIGURATION(GPIOF, 5, 13, __GPIO_PF_PIN_13_NAME__, __GPIO_PF_PIN_13_MODE__, __GPIO_PF_PIN_13_STATE__);
_PIN_CONFIGURATION(GPIOF, 5, 14, __GPIO_PF_PIN_14_NAME__, __GPIO_PF_PIN_14_MODE__, __GPIO_PF_PIN_14_STATE__);
_PIN_CONFIGURATION(GPIOF, 5, 15, __GPIO_PF_PIN_15_NAME__, __GPIO_PF_PIN_15_MODE__, __GPIO_PF_PIN_15_STATE__);
#endif

/** GPIOG pins configuration */
#if defined(RCC_APB2ENR_IOPGEN)
_PIN_CONFIGURATION(GPIOG, 6,  0, __GPIO_PG_PIN_0_NAME__ , __GPIO_PG_PIN_0_MODE__ , __GPIO_PG_PIN_0_STATE__ );
_PIN_CONFIGURATION(GPIOG, 6,  1, __GPIO_PG_PIN_1_NAME__ , __GPIO_PG_PIN_1_MODE__ , __GPIO_PG_PIN_1_STATE__ );
_PIN_CONFIGURATION(GPIOG, 6,  2, __GPIO_PG_PIN_2_NAME__ , __GPIO_PG_PIN_2_MODE__ , __GPIO_PG_PIN_2_STATE__ );
_PIN_CONFIGURATION(GPIOG, 6,  3, __GPIO_PG_PIN_3_NAME__ , __GPIO_PG_PIN_3_MODE__ , __GPIO_PG_PIN_3_STATE__ );
_PIN_CONFIGURATION(GPIOG, 6,  4, __GPIO_PG_PIN_4_NAME__ , __GPIO_PG_PIN_4_MODE__ , __GPIO_PG_PIN_4_STATE__ );
_PIN_CONFIGURATION(GPIOG, 6,  5, __GPIO_PG_PIN_5_NAME__ , __GPIO_PG_PIN_5_MODE__ , __GPIO_PG_PIN_5_STATE__ );
_PIN_CONFIGURATION(GPIOG, 6,  6, __GPIO_PG_PIN_6_NAME__ , __GPIO_PG_PIN_6_MODE__ , __GPIO_PG_PIN_6_STATE__ );
_PIN_CONFIGURATION(GPIOG, 6,  7, __GPIO_PG_PIN_7_NAME__ , __GPIO_PG_PIN_7_MODE__ , __GPIO_PG_PIN_7_STATE__ );
_PIN_CONFIGURATION(GPIOG, 6,  8, __GPIO_PG_PIN_8_NAME__ , __GPIO_PG_PIN_8_MODE__ , __GPIO_PG_PIN_8_STATE__ );
_PIN_CONFIGURATION(GPIOG, 6,  9, __GPIO_PG_PIN_9_NAME__ , __GPIO_PG_PIN_9_MODE__ , __GPIO_PG_PIN_9_STATE__ );
_PIN_CONFIGURATION(GPIOG, 6, 10, __GPIO_PG_PIN_10_NAME__, __GPIO_PG_PIN_10_MODE__, __GPIO_PG_PIN_10_STATE__);
_PIN_CONFIGURATION(GPIOG, 6, 11, __GPIO_PG_PIN_11_NAME__, __GPIO_PG_PIN_11_MODE__, __GPIO_PG_PIN_11_STATE__);
_PIN_CONFIGURATION(GPIOG, 6, 12, __GPIO_PG_PIN_12_NAME__, __GPIO_PG_PIN_12_MODE__, __GPIO_PG_PIN_12_STATE__);
_PIN_CONFIGURATION(GPIOG, 6, 13, __GPIO_PG_PIN_13_NAME__, __GPIO_PG_PIN_13_MODE__, __GPIO_PG_PIN_13_STATE__);
_PIN_CONFIGURATION(GPIOG, 6, 14, __GPIO_PG_PIN_14_NAME__, __GPIO_PG_PIN_14_MODE__, __GPIO_PG_PIN_14_STATE__);
_PIN_CONFIGURATION(GPIOG, 6, 15, __GPIO_PG_PIN_15_NAME__, __GPIO_PG_PIN_15_MODE__, __GPIO_PG_PIN_15_STATE__);
#endif

#define _CNF(_v)                        (((_v) >> 4) & 3)
#define _MODER(_v)                      (((_v) >> 2) & 3)
#define _PXODR(_v)                      (((_v) >> 0) & 3)
#define _MODE(_cnf, _moder, _pxodr)     ( (((_cnf) & 3) << 4) | (((_moder) & 3) << 2) | (((_pxodr) & 3) << 0) )

/*==============================================================================
  Exported types, enums definitions
==============================================================================*/
typedef enum {
        GPIO_MODE__PP_10MHz      = _MODE(0, 1, 2),
        GPIO_MODE__PP_2MHz       = _MODE(0, 2, 2),
        GPIO_MODE__PP_50MHz      = _MODE(0, 3, 2),
        GPIO_MODE__OD_10MHz      = _MODE(1, 1, 2),
        GPIO_MODE__OD_2MHz       = _MODE(1, 2, 2),
        GPIO_MODE__OD_50MHz      = _MODE(1, 3, 2),
        GPIO_MODE__AF_PP_10MHz   = _MODE(2, 1, 2),
        GPIO_MODE__AF_PP_2MHz    = _MODE(2, 2, 2),
        GPIO_MODE__AF_PP_50MHz   = _MODE(2, 3, 2),
        GPIO_MODE__AF_OD_10MHz   = _MODE(3, 1, 2),
        GPIO_MODE__AF_OD_2MHz    = _MODE(3, 2, 2),
        GPIO_MODE__AF_OD_50MHz   = _MODE(3, 3, 2),
        GPIO_MODE__ANALOG        = _MODE(0, 0, 2),
        GPIO_MODE__IN            = _MODE(1, 0, 2),
        GPIO_MODE__IN_PD         = _MODE(2, 0, 0),
        GPIO_MODE__IN_PU         = _MODE(2, 0, 1),
} GPIO_mode_t;

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
