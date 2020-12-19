/*=========================================================================*//**
@file    gpio_cfg.h

@author  Daniel Zorychta

@brief   This driver support GPIO. Set here driver configuration.

@note    Copyright (C) 2020  Daniel Zorychta <daniel.zorychta@gmail.com>

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

#ifdef ARCH_stm32f3

/*==============================================================================
  Include files
==============================================================================*/
#include "stm32f3/stm32f3xx.h"
#include "stm32f3/gpio_macros.h"

/*==============================================================================
  Exported symbolic constants/macros
==============================================================================*/
/** macro creates an enumerator with pin data */
#define _PIN_CONFIGURATION(port, port_idx, number, pin_name, mode, speed, af, state) \
enum port##_##number##_CFG {\
        _CONCAT(IOCTL_GPIO_PIN_IDX__, pin_name) = number,\
        _CONCAT(IOCTL_GPIO_PIN_MASK__, pin_name) = (1 << (number)),\
        _CONCAT(IOCTL_GPIO_PORT_IDX__, pin_name) = port_idx,\
        _CONCAT(_GPIO_, pin_name) = port##_BASE,\
        _CONCAT(_BP_, pin_name) = (number),\
        _CONCAT(_BM_, pin_name) = (1 << (number)),\
        _##port##_PIN_##number##_MODE  = (((u32_t)(mode) >> 8) & 0x3),\
        _##port##_PIN_##number##_TYPE  = (((u32_t)(mode) >> 4) & 0x1),\
        _##port##_PIN_##number##_PUPD  = (((u32_t)(mode) >> 0) & 0x3),\
        _##port##_PIN_##number##_SPEED = (speed),\
        _##port##_PIN_##number##_STATE = (state),\
        _##port##_PIN_##number##_AF    = (af),\
}

/** GPIO pin NONE definition */
#define _GPIO_NONE_BASE 0
_PIN_CONFIGURATION(_GPIO_NONE, 16, 17, NONE, _GPIO_MODE_ANALOG, _GPIO_SPEED_LOW, 0, _LOW);

/** GPIOA pins configuration */
#if defined(RCC_AHBENR_GPIOAEN)
_PIN_CONFIGURATION(GPIOA, 0, 0, __GPIO_PA_PIN_0_NAME__, __GPIO_PA_PIN_0_MODE__, __GPIO_PA_PIN_0_SPEED__, __GPIO_PA_PIN_0_AF__, __GPIO_PA_PIN_0_STATE__);
_PIN_CONFIGURATION(GPIOA, 0, 1, __GPIO_PA_PIN_1_NAME__, __GPIO_PA_PIN_1_MODE__, __GPIO_PA_PIN_1_SPEED__, __GPIO_PA_PIN_1_AF__, __GPIO_PA_PIN_1_STATE__);
_PIN_CONFIGURATION(GPIOA, 0, 2, __GPIO_PA_PIN_2_NAME__, __GPIO_PA_PIN_2_MODE__, __GPIO_PA_PIN_2_SPEED__, __GPIO_PA_PIN_2_AF__, __GPIO_PA_PIN_2_STATE__);
_PIN_CONFIGURATION(GPIOA, 0, 3, __GPIO_PA_PIN_3_NAME__, __GPIO_PA_PIN_3_MODE__, __GPIO_PA_PIN_3_SPEED__, __GPIO_PA_PIN_3_AF__, __GPIO_PA_PIN_3_STATE__);
_PIN_CONFIGURATION(GPIOA, 0, 4, __GPIO_PA_PIN_4_NAME__, __GPIO_PA_PIN_4_MODE__, __GPIO_PA_PIN_4_SPEED__, __GPIO_PA_PIN_4_AF__, __GPIO_PA_PIN_4_STATE__);
_PIN_CONFIGURATION(GPIOA, 0, 5, __GPIO_PA_PIN_5_NAME__, __GPIO_PA_PIN_5_MODE__, __GPIO_PA_PIN_5_SPEED__, __GPIO_PA_PIN_5_AF__, __GPIO_PA_PIN_5_STATE__);
_PIN_CONFIGURATION(GPIOA, 0, 6, __GPIO_PA_PIN_6_NAME__, __GPIO_PA_PIN_6_MODE__, __GPIO_PA_PIN_6_SPEED__, __GPIO_PA_PIN_6_AF__, __GPIO_PA_PIN_6_STATE__);
_PIN_CONFIGURATION(GPIOA, 0, 7, __GPIO_PA_PIN_7_NAME__, __GPIO_PA_PIN_7_MODE__, __GPIO_PA_PIN_7_SPEED__, __GPIO_PA_PIN_7_AF__, __GPIO_PA_PIN_7_STATE__);
_PIN_CONFIGURATION(GPIOA, 0, 8, __GPIO_PA_PIN_8_NAME__, __GPIO_PA_PIN_8_MODE__, __GPIO_PA_PIN_8_SPEED__, __GPIO_PA_PIN_8_AF__, __GPIO_PA_PIN_8_STATE__);
_PIN_CONFIGURATION(GPIOA, 0, 9, __GPIO_PA_PIN_9_NAME__, __GPIO_PA_PIN_9_MODE__, __GPIO_PA_PIN_9_SPEED__, __GPIO_PA_PIN_9_AF__, __GPIO_PA_PIN_9_STATE__);
_PIN_CONFIGURATION(GPIOA, 0, 10, __GPIO_PA_PIN_10_NAME__, __GPIO_PA_PIN_10_MODE__, __GPIO_PA_PIN_10_SPEED__, __GPIO_PA_PIN_10_AF__, __GPIO_PA_PIN_10_STATE__);
_PIN_CONFIGURATION(GPIOA, 0, 11, __GPIO_PA_PIN_11_NAME__, __GPIO_PA_PIN_11_MODE__, __GPIO_PA_PIN_11_SPEED__, __GPIO_PA_PIN_11_AF__, __GPIO_PA_PIN_11_STATE__);
_PIN_CONFIGURATION(GPIOA, 0, 12, __GPIO_PA_PIN_12_NAME__, __GPIO_PA_PIN_12_MODE__, __GPIO_PA_PIN_12_SPEED__, __GPIO_PA_PIN_12_AF__, __GPIO_PA_PIN_12_STATE__);
_PIN_CONFIGURATION(GPIOA, 0, 13, __GPIO_PA_PIN_13_NAME__, __GPIO_PA_PIN_13_MODE__, __GPIO_PA_PIN_13_SPEED__, __GPIO_PA_PIN_13_AF__, __GPIO_PA_PIN_13_STATE__);
_PIN_CONFIGURATION(GPIOA, 0, 14, __GPIO_PA_PIN_14_NAME__, __GPIO_PA_PIN_14_MODE__, __GPIO_PA_PIN_14_SPEED__, __GPIO_PA_PIN_14_AF__, __GPIO_PA_PIN_14_STATE__);
_PIN_CONFIGURATION(GPIOA, 0, 15, __GPIO_PA_PIN_15_NAME__, __GPIO_PA_PIN_15_MODE__, __GPIO_PA_PIN_15_SPEED__, __GPIO_PA_PIN_15_AF__, __GPIO_PA_PIN_15_STATE__);
#endif

/** GPIOB pins configuration */
#if defined(RCC_AHBENR_GPIOBEN)
_PIN_CONFIGURATION(GPIOB, 1, 0, __GPIO_PB_PIN_0_NAME__, __GPIO_PB_PIN_0_MODE__, __GPIO_PB_PIN_0_SPEED__, __GPIO_PB_PIN_0_AF__, __GPIO_PB_PIN_0_STATE__);
_PIN_CONFIGURATION(GPIOB, 1, 1, __GPIO_PB_PIN_1_NAME__, __GPIO_PB_PIN_1_MODE__, __GPIO_PB_PIN_1_SPEED__, __GPIO_PB_PIN_1_AF__, __GPIO_PB_PIN_1_STATE__);
_PIN_CONFIGURATION(GPIOB, 1, 2, __GPIO_PB_PIN_2_NAME__, __GPIO_PB_PIN_2_MODE__, __GPIO_PB_PIN_2_SPEED__, __GPIO_PB_PIN_2_AF__, __GPIO_PB_PIN_2_STATE__);
_PIN_CONFIGURATION(GPIOB, 1, 3, __GPIO_PB_PIN_3_NAME__, __GPIO_PB_PIN_3_MODE__, __GPIO_PB_PIN_3_SPEED__, __GPIO_PB_PIN_3_AF__, __GPIO_PB_PIN_3_STATE__);
_PIN_CONFIGURATION(GPIOB, 1, 4, __GPIO_PB_PIN_4_NAME__, __GPIO_PB_PIN_4_MODE__, __GPIO_PB_PIN_4_SPEED__, __GPIO_PB_PIN_4_AF__, __GPIO_PB_PIN_4_STATE__);
_PIN_CONFIGURATION(GPIOB, 1, 5, __GPIO_PB_PIN_5_NAME__, __GPIO_PB_PIN_5_MODE__, __GPIO_PB_PIN_5_SPEED__, __GPIO_PB_PIN_5_AF__, __GPIO_PB_PIN_5_STATE__);
_PIN_CONFIGURATION(GPIOB, 1, 6, __GPIO_PB_PIN_6_NAME__, __GPIO_PB_PIN_6_MODE__, __GPIO_PB_PIN_6_SPEED__, __GPIO_PB_PIN_6_AF__, __GPIO_PB_PIN_6_STATE__);
_PIN_CONFIGURATION(GPIOB, 1, 7, __GPIO_PB_PIN_7_NAME__, __GPIO_PB_PIN_7_MODE__, __GPIO_PB_PIN_7_SPEED__, __GPIO_PB_PIN_7_AF__, __GPIO_PB_PIN_7_STATE__);
_PIN_CONFIGURATION(GPIOB, 1, 8, __GPIO_PB_PIN_8_NAME__, __GPIO_PB_PIN_8_MODE__, __GPIO_PB_PIN_8_SPEED__, __GPIO_PB_PIN_8_AF__, __GPIO_PB_PIN_8_STATE__);
_PIN_CONFIGURATION(GPIOB, 1, 9, __GPIO_PB_PIN_9_NAME__, __GPIO_PB_PIN_9_MODE__, __GPIO_PB_PIN_9_SPEED__, __GPIO_PB_PIN_9_AF__, __GPIO_PB_PIN_9_STATE__);
_PIN_CONFIGURATION(GPIOB, 1, 10, __GPIO_PB_PIN_10_NAME__, __GPIO_PB_PIN_10_MODE__, __GPIO_PB_PIN_10_SPEED__, __GPIO_PB_PIN_10_AF__, __GPIO_PB_PIN_10_STATE__);
_PIN_CONFIGURATION(GPIOB, 1, 11, __GPIO_PB_PIN_11_NAME__, __GPIO_PB_PIN_11_MODE__, __GPIO_PB_PIN_11_SPEED__, __GPIO_PB_PIN_11_AF__, __GPIO_PB_PIN_11_STATE__);
_PIN_CONFIGURATION(GPIOB, 1, 12, __GPIO_PB_PIN_12_NAME__, __GPIO_PB_PIN_12_MODE__, __GPIO_PB_PIN_12_SPEED__, __GPIO_PB_PIN_12_AF__, __GPIO_PB_PIN_12_STATE__);
_PIN_CONFIGURATION(GPIOB, 1, 13, __GPIO_PB_PIN_13_NAME__, __GPIO_PB_PIN_13_MODE__, __GPIO_PB_PIN_13_SPEED__, __GPIO_PB_PIN_13_AF__, __GPIO_PB_PIN_13_STATE__);
_PIN_CONFIGURATION(GPIOB, 1, 14, __GPIO_PB_PIN_14_NAME__, __GPIO_PB_PIN_14_MODE__, __GPIO_PB_PIN_14_SPEED__, __GPIO_PB_PIN_14_AF__, __GPIO_PB_PIN_14_STATE__);
_PIN_CONFIGURATION(GPIOB, 1, 15, __GPIO_PB_PIN_15_NAME__, __GPIO_PB_PIN_15_MODE__, __GPIO_PB_PIN_15_SPEED__, __GPIO_PB_PIN_15_AF__, __GPIO_PB_PIN_15_STATE__);
#endif

/** GPIOC pins configuration */
#if defined(RCC_AHBENR_GPIOCEN)
_PIN_CONFIGURATION(GPIOC, 2, 0, __GPIO_PC_PIN_0_NAME__, __GPIO_PC_PIN_0_MODE__, __GPIO_PC_PIN_0_SPEED__, __GPIO_PC_PIN_0_AF__, __GPIO_PC_PIN_0_STATE__);
_PIN_CONFIGURATION(GPIOC, 2, 1, __GPIO_PC_PIN_1_NAME__, __GPIO_PC_PIN_1_MODE__, __GPIO_PC_PIN_1_SPEED__, __GPIO_PC_PIN_1_AF__, __GPIO_PC_PIN_1_STATE__);
_PIN_CONFIGURATION(GPIOC, 2, 2, __GPIO_PC_PIN_2_NAME__, __GPIO_PC_PIN_2_MODE__, __GPIO_PC_PIN_2_SPEED__, __GPIO_PC_PIN_2_AF__, __GPIO_PC_PIN_2_STATE__);
_PIN_CONFIGURATION(GPIOC, 2, 3, __GPIO_PC_PIN_3_NAME__, __GPIO_PC_PIN_3_MODE__, __GPIO_PC_PIN_3_SPEED__, __GPIO_PC_PIN_3_AF__, __GPIO_PC_PIN_3_STATE__);
_PIN_CONFIGURATION(GPIOC, 2, 4, __GPIO_PC_PIN_4_NAME__, __GPIO_PC_PIN_4_MODE__, __GPIO_PC_PIN_4_SPEED__, __GPIO_PC_PIN_4_AF__, __GPIO_PC_PIN_4_STATE__);
_PIN_CONFIGURATION(GPIOC, 2, 5, __GPIO_PC_PIN_5_NAME__, __GPIO_PC_PIN_5_MODE__, __GPIO_PC_PIN_5_SPEED__, __GPIO_PC_PIN_5_AF__, __GPIO_PC_PIN_5_STATE__);
_PIN_CONFIGURATION(GPIOC, 2, 6, __GPIO_PC_PIN_6_NAME__, __GPIO_PC_PIN_6_MODE__, __GPIO_PC_PIN_6_SPEED__, __GPIO_PC_PIN_6_AF__, __GPIO_PC_PIN_6_STATE__);
_PIN_CONFIGURATION(GPIOC, 2, 7, __GPIO_PC_PIN_7_NAME__, __GPIO_PC_PIN_7_MODE__, __GPIO_PC_PIN_7_SPEED__, __GPIO_PC_PIN_7_AF__, __GPIO_PC_PIN_7_STATE__);
_PIN_CONFIGURATION(GPIOC, 2, 8, __GPIO_PC_PIN_8_NAME__, __GPIO_PC_PIN_8_MODE__, __GPIO_PC_PIN_8_SPEED__, __GPIO_PC_PIN_8_AF__, __GPIO_PC_PIN_8_STATE__);
_PIN_CONFIGURATION(GPIOC, 2, 9, __GPIO_PC_PIN_9_NAME__, __GPIO_PC_PIN_9_MODE__, __GPIO_PC_PIN_9_SPEED__, __GPIO_PC_PIN_9_AF__, __GPIO_PC_PIN_9_STATE__);
_PIN_CONFIGURATION(GPIOC, 2, 10, __GPIO_PC_PIN_10_NAME__, __GPIO_PC_PIN_10_MODE__, __GPIO_PC_PIN_10_SPEED__, __GPIO_PC_PIN_10_AF__, __GPIO_PC_PIN_10_STATE__);
_PIN_CONFIGURATION(GPIOC, 2, 11, __GPIO_PC_PIN_11_NAME__, __GPIO_PC_PIN_11_MODE__, __GPIO_PC_PIN_11_SPEED__, __GPIO_PC_PIN_11_AF__, __GPIO_PC_PIN_11_STATE__);
_PIN_CONFIGURATION(GPIOC, 2, 12, __GPIO_PC_PIN_12_NAME__, __GPIO_PC_PIN_12_MODE__, __GPIO_PC_PIN_12_SPEED__, __GPIO_PC_PIN_12_AF__, __GPIO_PC_PIN_12_STATE__);
_PIN_CONFIGURATION(GPIOC, 2, 13, __GPIO_PC_PIN_13_NAME__, __GPIO_PC_PIN_13_MODE__, __GPIO_PC_PIN_13_SPEED__, __GPIO_PC_PIN_13_AF__, __GPIO_PC_PIN_13_STATE__);
_PIN_CONFIGURATION(GPIOC, 2, 14, __GPIO_PC_PIN_14_NAME__, __GPIO_PC_PIN_14_MODE__, __GPIO_PC_PIN_14_SPEED__, __GPIO_PC_PIN_14_AF__, __GPIO_PC_PIN_14_STATE__);
_PIN_CONFIGURATION(GPIOC, 2, 15, __GPIO_PC_PIN_15_NAME__, __GPIO_PC_PIN_15_MODE__, __GPIO_PC_PIN_15_SPEED__, __GPIO_PC_PIN_15_AF__, __GPIO_PC_PIN_15_STATE__);
#endif

/** GPIOD pins configuration */
#if defined(RCC_AHBENR_GPIODEN)
_PIN_CONFIGURATION(GPIOD, 3, 0, __GPIO_PD_PIN_0_NAME__, __GPIO_PD_PIN_0_MODE__, __GPIO_PD_PIN_0_SPEED__, __GPIO_PD_PIN_0_AF__, __GPIO_PD_PIN_0_STATE__);
_PIN_CONFIGURATION(GPIOD, 3, 1, __GPIO_PD_PIN_1_NAME__, __GPIO_PD_PIN_1_MODE__, __GPIO_PD_PIN_1_SPEED__, __GPIO_PD_PIN_1_AF__, __GPIO_PD_PIN_1_STATE__);
_PIN_CONFIGURATION(GPIOD, 3, 2, __GPIO_PD_PIN_2_NAME__, __GPIO_PD_PIN_2_MODE__, __GPIO_PD_PIN_2_SPEED__, __GPIO_PD_PIN_2_AF__, __GPIO_PD_PIN_2_STATE__);
_PIN_CONFIGURATION(GPIOD, 3, 3, __GPIO_PD_PIN_3_NAME__, __GPIO_PD_PIN_3_MODE__, __GPIO_PD_PIN_3_SPEED__, __GPIO_PD_PIN_3_AF__, __GPIO_PD_PIN_3_STATE__);
_PIN_CONFIGURATION(GPIOD, 3, 4, __GPIO_PD_PIN_4_NAME__, __GPIO_PD_PIN_4_MODE__, __GPIO_PD_PIN_4_SPEED__, __GPIO_PD_PIN_4_AF__, __GPIO_PD_PIN_4_STATE__);
_PIN_CONFIGURATION(GPIOD, 3, 5, __GPIO_PD_PIN_5_NAME__, __GPIO_PD_PIN_5_MODE__, __GPIO_PD_PIN_5_SPEED__, __GPIO_PD_PIN_5_AF__, __GPIO_PD_PIN_5_STATE__);
_PIN_CONFIGURATION(GPIOD, 3, 6, __GPIO_PD_PIN_6_NAME__, __GPIO_PD_PIN_6_MODE__, __GPIO_PD_PIN_6_SPEED__, __GPIO_PD_PIN_6_AF__, __GPIO_PD_PIN_6_STATE__);
_PIN_CONFIGURATION(GPIOD, 3, 7, __GPIO_PD_PIN_7_NAME__, __GPIO_PD_PIN_7_MODE__, __GPIO_PD_PIN_7_SPEED__, __GPIO_PD_PIN_7_AF__, __GPIO_PD_PIN_7_STATE__);
_PIN_CONFIGURATION(GPIOD, 3, 8, __GPIO_PD_PIN_8_NAME__, __GPIO_PD_PIN_8_MODE__, __GPIO_PD_PIN_8_SPEED__, __GPIO_PD_PIN_8_AF__, __GPIO_PD_PIN_8_STATE__);
_PIN_CONFIGURATION(GPIOD, 3, 9, __GPIO_PD_PIN_9_NAME__, __GPIO_PD_PIN_9_MODE__, __GPIO_PD_PIN_9_SPEED__, __GPIO_PD_PIN_9_AF__, __GPIO_PD_PIN_9_STATE__);
_PIN_CONFIGURATION(GPIOD, 3, 10, __GPIO_PD_PIN_10_NAME__, __GPIO_PD_PIN_10_MODE__, __GPIO_PD_PIN_10_SPEED__, __GPIO_PD_PIN_10_AF__, __GPIO_PD_PIN_10_STATE__);
_PIN_CONFIGURATION(GPIOD, 3, 11, __GPIO_PD_PIN_11_NAME__, __GPIO_PD_PIN_11_MODE__, __GPIO_PD_PIN_11_SPEED__, __GPIO_PD_PIN_11_AF__, __GPIO_PD_PIN_11_STATE__);
_PIN_CONFIGURATION(GPIOD, 3, 12, __GPIO_PD_PIN_12_NAME__, __GPIO_PD_PIN_12_MODE__, __GPIO_PD_PIN_12_SPEED__, __GPIO_PD_PIN_12_AF__, __GPIO_PD_PIN_12_STATE__);
_PIN_CONFIGURATION(GPIOD, 3, 13, __GPIO_PD_PIN_13_NAME__, __GPIO_PD_PIN_13_MODE__, __GPIO_PD_PIN_13_SPEED__, __GPIO_PD_PIN_13_AF__, __GPIO_PD_PIN_13_STATE__);
_PIN_CONFIGURATION(GPIOD, 3, 14, __GPIO_PD_PIN_14_NAME__, __GPIO_PD_PIN_14_MODE__, __GPIO_PD_PIN_14_SPEED__, __GPIO_PD_PIN_14_AF__, __GPIO_PD_PIN_14_STATE__);
_PIN_CONFIGURATION(GPIOD, 3, 15, __GPIO_PD_PIN_15_NAME__, __GPIO_PD_PIN_15_MODE__, __GPIO_PD_PIN_15_SPEED__, __GPIO_PD_PIN_15_AF__, __GPIO_PD_PIN_15_STATE__);
#endif

/** GPIOE pins configuration */
#if defined(RCC_AHBENR_GPIOEEN)
_PIN_CONFIGURATION(GPIOE, 4, 0, __GPIO_PE_PIN_0_NAME__, __GPIO_PE_PIN_0_MODE__, __GPIO_PE_PIN_0_SPEED__, __GPIO_PE_PIN_0_AF__, __GPIO_PE_PIN_0_STATE__);
_PIN_CONFIGURATION(GPIOE, 4, 1, __GPIO_PE_PIN_1_NAME__, __GPIO_PE_PIN_1_MODE__, __GPIO_PE_PIN_1_SPEED__, __GPIO_PE_PIN_1_AF__, __GPIO_PE_PIN_1_STATE__);
_PIN_CONFIGURATION(GPIOE, 4, 2, __GPIO_PE_PIN_2_NAME__, __GPIO_PE_PIN_2_MODE__, __GPIO_PE_PIN_2_SPEED__, __GPIO_PE_PIN_2_AF__, __GPIO_PE_PIN_2_STATE__);
_PIN_CONFIGURATION(GPIOE, 4, 3, __GPIO_PE_PIN_3_NAME__, __GPIO_PE_PIN_3_MODE__, __GPIO_PE_PIN_3_SPEED__, __GPIO_PE_PIN_3_AF__, __GPIO_PE_PIN_3_STATE__);
_PIN_CONFIGURATION(GPIOE, 4, 4, __GPIO_PE_PIN_4_NAME__, __GPIO_PE_PIN_4_MODE__, __GPIO_PE_PIN_4_SPEED__, __GPIO_PE_PIN_4_AF__, __GPIO_PE_PIN_4_STATE__);
_PIN_CONFIGURATION(GPIOE, 4, 5, __GPIO_PE_PIN_5_NAME__, __GPIO_PE_PIN_5_MODE__, __GPIO_PE_PIN_5_SPEED__, __GPIO_PE_PIN_5_AF__, __GPIO_PE_PIN_5_STATE__);
_PIN_CONFIGURATION(GPIOE, 4, 6, __GPIO_PE_PIN_6_NAME__, __GPIO_PE_PIN_6_MODE__, __GPIO_PE_PIN_6_SPEED__, __GPIO_PE_PIN_6_AF__, __GPIO_PE_PIN_6_STATE__);
_PIN_CONFIGURATION(GPIOE, 4, 7, __GPIO_PE_PIN_7_NAME__, __GPIO_PE_PIN_7_MODE__, __GPIO_PE_PIN_7_SPEED__, __GPIO_PE_PIN_7_AF__, __GPIO_PE_PIN_7_STATE__);
_PIN_CONFIGURATION(GPIOE, 4, 8, __GPIO_PE_PIN_8_NAME__, __GPIO_PE_PIN_8_MODE__, __GPIO_PE_PIN_8_SPEED__, __GPIO_PE_PIN_8_AF__, __GPIO_PE_PIN_8_STATE__);
_PIN_CONFIGURATION(GPIOE, 4, 9, __GPIO_PE_PIN_9_NAME__, __GPIO_PE_PIN_9_MODE__, __GPIO_PE_PIN_9_SPEED__, __GPIO_PE_PIN_9_AF__, __GPIO_PE_PIN_9_STATE__);
_PIN_CONFIGURATION(GPIOE, 4, 10, __GPIO_PE_PIN_10_NAME__, __GPIO_PE_PIN_10_MODE__, __GPIO_PE_PIN_10_SPEED__, __GPIO_PE_PIN_10_AF__, __GPIO_PE_PIN_10_STATE__);
_PIN_CONFIGURATION(GPIOE, 4, 11, __GPIO_PE_PIN_11_NAME__, __GPIO_PE_PIN_11_MODE__, __GPIO_PE_PIN_11_SPEED__, __GPIO_PE_PIN_11_AF__, __GPIO_PE_PIN_11_STATE__);
_PIN_CONFIGURATION(GPIOE, 4, 12, __GPIO_PE_PIN_12_NAME__, __GPIO_PE_PIN_12_MODE__, __GPIO_PE_PIN_12_SPEED__, __GPIO_PE_PIN_12_AF__, __GPIO_PE_PIN_12_STATE__);
_PIN_CONFIGURATION(GPIOE, 4, 13, __GPIO_PE_PIN_13_NAME__, __GPIO_PE_PIN_13_MODE__, __GPIO_PE_PIN_13_SPEED__, __GPIO_PE_PIN_13_AF__, __GPIO_PE_PIN_13_STATE__);
_PIN_CONFIGURATION(GPIOE, 4, 14, __GPIO_PE_PIN_14_NAME__, __GPIO_PE_PIN_14_MODE__, __GPIO_PE_PIN_14_SPEED__, __GPIO_PE_PIN_14_AF__, __GPIO_PE_PIN_14_STATE__);
_PIN_CONFIGURATION(GPIOE, 4, 15, __GPIO_PE_PIN_15_NAME__, __GPIO_PE_PIN_15_MODE__, __GPIO_PE_PIN_15_SPEED__, __GPIO_PE_PIN_15_AF__, __GPIO_PE_PIN_15_STATE__);
#endif

/** GPIOF pins configuration */
#if defined(RCC_AHBENR_GPIOFEN)
_PIN_CONFIGURATION(GPIOF, 5, 0, __GPIO_PF_PIN_0_NAME__, __GPIO_PF_PIN_0_MODE__, __GPIO_PF_PIN_0_SPEED__, __GPIO_PF_PIN_0_AF__, __GPIO_PF_PIN_0_STATE__);
_PIN_CONFIGURATION(GPIOF, 5, 1, __GPIO_PF_PIN_1_NAME__, __GPIO_PF_PIN_1_MODE__, __GPIO_PF_PIN_1_SPEED__, __GPIO_PF_PIN_1_AF__, __GPIO_PF_PIN_1_STATE__);
_PIN_CONFIGURATION(GPIOF, 5, 2, __GPIO_PF_PIN_2_NAME__, __GPIO_PF_PIN_2_MODE__, __GPIO_PF_PIN_2_SPEED__, __GPIO_PF_PIN_2_AF__, __GPIO_PF_PIN_2_STATE__);
_PIN_CONFIGURATION(GPIOF, 5, 3, __GPIO_PF_PIN_3_NAME__, __GPIO_PF_PIN_3_MODE__, __GPIO_PF_PIN_3_SPEED__, __GPIO_PF_PIN_3_AF__, __GPIO_PF_PIN_3_STATE__);
_PIN_CONFIGURATION(GPIOF, 5, 4, __GPIO_PF_PIN_4_NAME__, __GPIO_PF_PIN_4_MODE__, __GPIO_PF_PIN_4_SPEED__, __GPIO_PF_PIN_4_AF__, __GPIO_PF_PIN_4_STATE__);
_PIN_CONFIGURATION(GPIOF, 5, 5, __GPIO_PF_PIN_5_NAME__, __GPIO_PF_PIN_5_MODE__, __GPIO_PF_PIN_5_SPEED__, __GPIO_PF_PIN_5_AF__, __GPIO_PF_PIN_5_STATE__);
_PIN_CONFIGURATION(GPIOF, 5, 6, __GPIO_PF_PIN_6_NAME__, __GPIO_PF_PIN_6_MODE__, __GPIO_PF_PIN_6_SPEED__, __GPIO_PF_PIN_6_AF__, __GPIO_PF_PIN_6_STATE__);
_PIN_CONFIGURATION(GPIOF, 5, 7, __GPIO_PF_PIN_7_NAME__, __GPIO_PF_PIN_7_MODE__, __GPIO_PF_PIN_7_SPEED__, __GPIO_PF_PIN_7_AF__, __GPIO_PF_PIN_7_STATE__);
_PIN_CONFIGURATION(GPIOF, 5, 8, __GPIO_PF_PIN_8_NAME__, __GPIO_PF_PIN_8_MODE__, __GPIO_PF_PIN_8_SPEED__, __GPIO_PF_PIN_8_AF__, __GPIO_PF_PIN_8_STATE__);
_PIN_CONFIGURATION(GPIOF, 5, 9, __GPIO_PF_PIN_9_NAME__, __GPIO_PF_PIN_9_MODE__, __GPIO_PF_PIN_9_SPEED__, __GPIO_PF_PIN_9_AF__, __GPIO_PF_PIN_9_STATE__);
_PIN_CONFIGURATION(GPIOF, 5, 10, __GPIO_PF_PIN_10_NAME__, __GPIO_PF_PIN_10_MODE__, __GPIO_PF_PIN_10_SPEED__, __GPIO_PF_PIN_10_AF__, __GPIO_PF_PIN_10_STATE__);
_PIN_CONFIGURATION(GPIOF, 5, 11, __GPIO_PF_PIN_11_NAME__, __GPIO_PF_PIN_11_MODE__, __GPIO_PF_PIN_11_SPEED__, __GPIO_PF_PIN_11_AF__, __GPIO_PF_PIN_11_STATE__);
_PIN_CONFIGURATION(GPIOF, 5, 12, __GPIO_PF_PIN_12_NAME__, __GPIO_PF_PIN_12_MODE__, __GPIO_PF_PIN_12_SPEED__, __GPIO_PF_PIN_12_AF__, __GPIO_PF_PIN_12_STATE__);
_PIN_CONFIGURATION(GPIOF, 5, 13, __GPIO_PF_PIN_13_NAME__, __GPIO_PF_PIN_13_MODE__, __GPIO_PF_PIN_13_SPEED__, __GPIO_PF_PIN_13_AF__, __GPIO_PF_PIN_13_STATE__);
_PIN_CONFIGURATION(GPIOF, 5, 14, __GPIO_PF_PIN_14_NAME__, __GPIO_PF_PIN_14_MODE__, __GPIO_PF_PIN_14_SPEED__, __GPIO_PF_PIN_14_AF__, __GPIO_PF_PIN_14_STATE__);
_PIN_CONFIGURATION(GPIOF, 5, 15, __GPIO_PF_PIN_15_NAME__, __GPIO_PF_PIN_15_MODE__, __GPIO_PF_PIN_15_SPEED__, __GPIO_PF_PIN_15_AF__, __GPIO_PF_PIN_15_STATE__);
#endif

/** GPIOG pins configuration */
#if defined(RCC_AHBENR_GPIOGEN)
_PIN_CONFIGURATION(GPIOG, 6, 0, __GPIO_PG_PIN_0_NAME__, __GPIO_PG_PIN_0_MODE__, __GPIO_PG_PIN_0_SPEED__, __GPIO_PG_PIN_0_AF__, __GPIO_PG_PIN_0_STATE__);
_PIN_CONFIGURATION(GPIOG, 6, 1, __GPIO_PG_PIN_1_NAME__, __GPIO_PG_PIN_1_MODE__, __GPIO_PG_PIN_1_SPEED__, __GPIO_PG_PIN_1_AF__, __GPIO_PG_PIN_1_STATE__);
_PIN_CONFIGURATION(GPIOG, 6, 2, __GPIO_PG_PIN_2_NAME__, __GPIO_PG_PIN_2_MODE__, __GPIO_PG_PIN_2_SPEED__, __GPIO_PG_PIN_2_AF__, __GPIO_PG_PIN_2_STATE__);
_PIN_CONFIGURATION(GPIOG, 6, 3, __GPIO_PG_PIN_3_NAME__, __GPIO_PG_PIN_3_MODE__, __GPIO_PG_PIN_3_SPEED__, __GPIO_PG_PIN_3_AF__, __GPIO_PG_PIN_3_STATE__);
_PIN_CONFIGURATION(GPIOG, 6, 4, __GPIO_PG_PIN_4_NAME__, __GPIO_PG_PIN_4_MODE__, __GPIO_PG_PIN_4_SPEED__, __GPIO_PG_PIN_4_AF__, __GPIO_PG_PIN_4_STATE__);
_PIN_CONFIGURATION(GPIOG, 6, 5, __GPIO_PG_PIN_5_NAME__, __GPIO_PG_PIN_5_MODE__, __GPIO_PG_PIN_5_SPEED__, __GPIO_PG_PIN_5_AF__, __GPIO_PG_PIN_5_STATE__);
_PIN_CONFIGURATION(GPIOG, 6, 6, __GPIO_PG_PIN_6_NAME__, __GPIO_PG_PIN_6_MODE__, __GPIO_PG_PIN_6_SPEED__, __GPIO_PG_PIN_6_AF__, __GPIO_PG_PIN_6_STATE__);
_PIN_CONFIGURATION(GPIOG, 6, 7, __GPIO_PG_PIN_7_NAME__, __GPIO_PG_PIN_7_MODE__, __GPIO_PG_PIN_7_SPEED__, __GPIO_PG_PIN_7_AF__, __GPIO_PG_PIN_7_STATE__);
_PIN_CONFIGURATION(GPIOG, 6, 8, __GPIO_PG_PIN_8_NAME__, __GPIO_PG_PIN_8_MODE__, __GPIO_PG_PIN_8_SPEED__, __GPIO_PG_PIN_8_AF__, __GPIO_PG_PIN_8_STATE__);
_PIN_CONFIGURATION(GPIOG, 6, 9, __GPIO_PG_PIN_9_NAME__, __GPIO_PG_PIN_9_MODE__, __GPIO_PG_PIN_9_SPEED__, __GPIO_PG_PIN_9_AF__, __GPIO_PG_PIN_9_STATE__);
_PIN_CONFIGURATION(GPIOG, 6, 10, __GPIO_PG_PIN_10_NAME__, __GPIO_PG_PIN_10_MODE__, __GPIO_PG_PIN_10_SPEED__, __GPIO_PG_PIN_10_AF__, __GPIO_PG_PIN_10_STATE__);
_PIN_CONFIGURATION(GPIOG, 6, 11, __GPIO_PG_PIN_11_NAME__, __GPIO_PG_PIN_11_MODE__, __GPIO_PG_PIN_11_SPEED__, __GPIO_PG_PIN_11_AF__, __GPIO_PG_PIN_11_STATE__);
_PIN_CONFIGURATION(GPIOG, 6, 12, __GPIO_PG_PIN_12_NAME__, __GPIO_PG_PIN_12_MODE__, __GPIO_PG_PIN_12_SPEED__, __GPIO_PG_PIN_12_AF__, __GPIO_PG_PIN_12_STATE__);
_PIN_CONFIGURATION(GPIOG, 6, 13, __GPIO_PG_PIN_13_NAME__, __GPIO_PG_PIN_13_MODE__, __GPIO_PG_PIN_13_SPEED__, __GPIO_PG_PIN_13_AF__, __GPIO_PG_PIN_13_STATE__);
_PIN_CONFIGURATION(GPIOG, 6, 14, __GPIO_PG_PIN_14_NAME__, __GPIO_PG_PIN_14_MODE__, __GPIO_PG_PIN_14_SPEED__, __GPIO_PG_PIN_14_AF__, __GPIO_PG_PIN_14_STATE__);
_PIN_CONFIGURATION(GPIOG, 6, 15, __GPIO_PG_PIN_15_NAME__, __GPIO_PG_PIN_15_MODE__, __GPIO_PG_PIN_15_SPEED__, __GPIO_PG_PIN_15_AF__, __GPIO_PG_PIN_15_STATE__);
#endif

/** GPIOH pins configuration */
#if defined(RCC_AHBENR_GPIOHEN)
_PIN_CONFIGURATION(GPIOH, 7, 0, __GPIO_PH_PIN_0_NAME__, __GPIO_PH_PIN_0_MODE__, __GPIO_PH_PIN_0_SPEED__, __GPIO_PH_PIN_0_AF__, __GPIO_PH_PIN_0_STATE__);
_PIN_CONFIGURATION(GPIOH, 7, 1, __GPIO_PH_PIN_1_NAME__, __GPIO_PH_PIN_1_MODE__, __GPIO_PH_PIN_1_SPEED__, __GPIO_PH_PIN_1_AF__, __GPIO_PH_PIN_1_STATE__);
_PIN_CONFIGURATION(GPIOH, 7, 2, __GPIO_PH_PIN_2_NAME__, __GPIO_PH_PIN_2_MODE__, __GPIO_PH_PIN_2_SPEED__, __GPIO_PH_PIN_2_AF__, __GPIO_PH_PIN_2_STATE__);
_PIN_CONFIGURATION(GPIOH, 7, 3, __GPIO_PH_PIN_3_NAME__, __GPIO_PH_PIN_3_MODE__, __GPIO_PH_PIN_3_SPEED__, __GPIO_PH_PIN_3_AF__, __GPIO_PH_PIN_3_STATE__);
_PIN_CONFIGURATION(GPIOH, 7, 4, __GPIO_PH_PIN_4_NAME__, __GPIO_PH_PIN_4_MODE__, __GPIO_PH_PIN_4_SPEED__, __GPIO_PH_PIN_4_AF__, __GPIO_PH_PIN_4_STATE__);
_PIN_CONFIGURATION(GPIOH, 7, 5, __GPIO_PH_PIN_5_NAME__, __GPIO_PH_PIN_5_MODE__, __GPIO_PH_PIN_5_SPEED__, __GPIO_PH_PIN_5_AF__, __GPIO_PH_PIN_5_STATE__);
_PIN_CONFIGURATION(GPIOH, 7, 6, __GPIO_PH_PIN_6_NAME__, __GPIO_PH_PIN_6_MODE__, __GPIO_PH_PIN_6_SPEED__, __GPIO_PH_PIN_6_AF__, __GPIO_PH_PIN_6_STATE__);
_PIN_CONFIGURATION(GPIOH, 7, 7, __GPIO_PH_PIN_7_NAME__, __GPIO_PH_PIN_7_MODE__, __GPIO_PH_PIN_7_SPEED__, __GPIO_PH_PIN_7_AF__, __GPIO_PH_PIN_7_STATE__);
_PIN_CONFIGURATION(GPIOH, 7, 8, __GPIO_PH_PIN_8_NAME__, __GPIO_PH_PIN_8_MODE__, __GPIO_PH_PIN_8_SPEED__, __GPIO_PH_PIN_8_AF__, __GPIO_PH_PIN_8_STATE__);
_PIN_CONFIGURATION(GPIOH, 7, 9, __GPIO_PH_PIN_9_NAME__, __GPIO_PH_PIN_9_MODE__, __GPIO_PH_PIN_9_SPEED__, __GPIO_PH_PIN_9_AF__, __GPIO_PH_PIN_9_STATE__);
_PIN_CONFIGURATION(GPIOH, 7, 10, __GPIO_PH_PIN_10_NAME__, __GPIO_PH_PIN_10_MODE__, __GPIO_PH_PIN_10_SPEED__, __GPIO_PH_PIN_10_AF__, __GPIO_PH_PIN_10_STATE__);
_PIN_CONFIGURATION(GPIOH, 7, 11, __GPIO_PH_PIN_11_NAME__, __GPIO_PH_PIN_11_MODE__, __GPIO_PH_PIN_11_SPEED__, __GPIO_PH_PIN_11_AF__, __GPIO_PH_PIN_11_STATE__);
_PIN_CONFIGURATION(GPIOH, 7, 12, __GPIO_PH_PIN_12_NAME__, __GPIO_PH_PIN_12_MODE__, __GPIO_PH_PIN_12_SPEED__, __GPIO_PH_PIN_12_AF__, __GPIO_PH_PIN_12_STATE__);
_PIN_CONFIGURATION(GPIOH, 7, 13, __GPIO_PH_PIN_13_NAME__, __GPIO_PH_PIN_13_MODE__, __GPIO_PH_PIN_13_SPEED__, __GPIO_PH_PIN_13_AF__, __GPIO_PH_PIN_13_STATE__);
_PIN_CONFIGURATION(GPIOH, 7, 14, __GPIO_PH_PIN_14_NAME__, __GPIO_PH_PIN_14_MODE__, __GPIO_PH_PIN_14_SPEED__, __GPIO_PH_PIN_14_AF__, __GPIO_PH_PIN_14_STATE__);
_PIN_CONFIGURATION(GPIOH, 7, 15, __GPIO_PH_PIN_15_NAME__, __GPIO_PH_PIN_15_MODE__, __GPIO_PH_PIN_15_SPEED__, __GPIO_PH_PIN_15_AF__, __GPIO_PH_PIN_15_STATE__);
#endif

/** GPIOI pins configuration */
#if defined(RCC_AHBENR_GPIOIEN)
_PIN_CONFIGURATION(GPIOI, 8, 0, __GPIO_PI_PIN_0_NAME__, __GPIO_PI_PIN_0_MODE__, __GPIO_PI_PIN_0_SPEED__, __GPIO_PI_PIN_0_AF__, __GPIO_PI_PIN_0_STATE__);
_PIN_CONFIGURATION(GPIOI, 8, 1, __GPIO_PI_PIN_1_NAME__, __GPIO_PI_PIN_1_MODE__, __GPIO_PI_PIN_1_SPEED__, __GPIO_PI_PIN_1_AF__, __GPIO_PI_PIN_1_STATE__);
_PIN_CONFIGURATION(GPIOI, 8, 2, __GPIO_PI_PIN_2_NAME__, __GPIO_PI_PIN_2_MODE__, __GPIO_PI_PIN_2_SPEED__, __GPIO_PI_PIN_2_AF__, __GPIO_PI_PIN_2_STATE__);
_PIN_CONFIGURATION(GPIOI, 8, 3, __GPIO_PI_PIN_3_NAME__, __GPIO_PI_PIN_3_MODE__, __GPIO_PI_PIN_3_SPEED__, __GPIO_PI_PIN_3_AF__, __GPIO_PI_PIN_3_STATE__);
_PIN_CONFIGURATION(GPIOI, 8, 4, __GPIO_PI_PIN_4_NAME__, __GPIO_PI_PIN_4_MODE__, __GPIO_PI_PIN_4_SPEED__, __GPIO_PI_PIN_4_AF__, __GPIO_PI_PIN_4_STATE__);
_PIN_CONFIGURATION(GPIOI, 8, 5, __GPIO_PI_PIN_5_NAME__, __GPIO_PI_PIN_5_MODE__, __GPIO_PI_PIN_5_SPEED__, __GPIO_PI_PIN_5_AF__, __GPIO_PI_PIN_5_STATE__);
_PIN_CONFIGURATION(GPIOI, 8, 6, __GPIO_PI_PIN_6_NAME__, __GPIO_PI_PIN_6_MODE__, __GPIO_PI_PIN_6_SPEED__, __GPIO_PI_PIN_6_AF__, __GPIO_PI_PIN_6_STATE__);
_PIN_CONFIGURATION(GPIOI, 8, 7, __GPIO_PI_PIN_7_NAME__, __GPIO_PI_PIN_7_MODE__, __GPIO_PI_PIN_7_SPEED__, __GPIO_PI_PIN_7_AF__, __GPIO_PI_PIN_7_STATE__);
_PIN_CONFIGURATION(GPIOI, 8, 8, __GPIO_PI_PIN_8_NAME__, __GPIO_PI_PIN_8_MODE__, __GPIO_PI_PIN_8_SPEED__, __GPIO_PI_PIN_8_AF__, __GPIO_PI_PIN_8_STATE__);
_PIN_CONFIGURATION(GPIOI, 8, 9, __GPIO_PI_PIN_9_NAME__, __GPIO_PI_PIN_9_MODE__, __GPIO_PI_PIN_9_SPEED__, __GPIO_PI_PIN_9_AF__, __GPIO_PI_PIN_9_STATE__);
_PIN_CONFIGURATION(GPIOI, 8, 10, __GPIO_PI_PIN_10_NAME__, __GPIO_PI_PIN_10_MODE__, __GPIO_PI_PIN_10_SPEED__, __GPIO_PI_PIN_10_AF__, __GPIO_PI_PIN_10_STATE__);
_PIN_CONFIGURATION(GPIOI, 8, 11, __GPIO_PI_PIN_11_NAME__, __GPIO_PI_PIN_11_MODE__, __GPIO_PI_PIN_11_SPEED__, __GPIO_PI_PIN_11_AF__, __GPIO_PI_PIN_11_STATE__);
_PIN_CONFIGURATION(GPIOI, 8, 12, __GPIO_PI_PIN_12_NAME__, __GPIO_PI_PIN_12_MODE__, __GPIO_PI_PIN_12_SPEED__, __GPIO_PI_PIN_12_AF__, __GPIO_PI_PIN_12_STATE__);
_PIN_CONFIGURATION(GPIOI, 8, 13, __GPIO_PI_PIN_13_NAME__, __GPIO_PI_PIN_13_MODE__, __GPIO_PI_PIN_13_SPEED__, __GPIO_PI_PIN_13_AF__, __GPIO_PI_PIN_13_STATE__);
_PIN_CONFIGURATION(GPIOI, 8, 14, __GPIO_PI_PIN_14_NAME__, __GPIO_PI_PIN_14_MODE__, __GPIO_PI_PIN_14_SPEED__, __GPIO_PI_PIN_14_AF__, __GPIO_PI_PIN_14_STATE__);
_PIN_CONFIGURATION(GPIOI, 8, 15, __GPIO_PI_PIN_15_NAME__, __GPIO_PI_PIN_15_MODE__, __GPIO_PI_PIN_15_SPEED__, __GPIO_PI_PIN_15_AF__, __GPIO_PI_PIN_15_STATE__);
#endif

/** GPIOJ pins configuration */
#if defined(RCC_AHBENR_GPIOJEN)
_PIN_CONFIGURATION(GPIOJ, 9, 0, __GPIO_PJ_PIN_0_NAME__, __GPIO_PJ_PIN_0_MODE__, __GPIO_PJ_PIN_0_SPEED__, __GPIO_PJ_PIN_0_AF__, __GPIO_PJ_PIN_0_STATE__);
_PIN_CONFIGURATION(GPIOJ, 9, 1, __GPIO_PJ_PIN_1_NAME__, __GPIO_PJ_PIN_1_MODE__, __GPIO_PJ_PIN_1_SPEED__, __GPIO_PJ_PIN_1_AF__, __GPIO_PJ_PIN_1_STATE__);
_PIN_CONFIGURATION(GPIOJ, 9, 2, __GPIO_PJ_PIN_2_NAME__, __GPIO_PJ_PIN_2_MODE__, __GPIO_PJ_PIN_2_SPEED__, __GPIO_PJ_PIN_2_AF__, __GPIO_PJ_PIN_2_STATE__);
_PIN_CONFIGURATION(GPIOJ, 9, 3, __GPIO_PJ_PIN_3_NAME__, __GPIO_PJ_PIN_3_MODE__, __GPIO_PJ_PIN_3_SPEED__, __GPIO_PJ_PIN_3_AF__, __GPIO_PJ_PIN_3_STATE__);
_PIN_CONFIGURATION(GPIOJ, 9, 4, __GPIO_PJ_PIN_4_NAME__, __GPIO_PJ_PIN_4_MODE__, __GPIO_PJ_PIN_4_SPEED__, __GPIO_PJ_PIN_4_AF__, __GPIO_PJ_PIN_4_STATE__);
_PIN_CONFIGURATION(GPIOJ, 9, 5, __GPIO_PJ_PIN_5_NAME__, __GPIO_PJ_PIN_5_MODE__, __GPIO_PJ_PIN_5_SPEED__, __GPIO_PJ_PIN_5_AF__, __GPIO_PJ_PIN_5_STATE__);
_PIN_CONFIGURATION(GPIOJ, 9, 6, __GPIO_PJ_PIN_6_NAME__, __GPIO_PJ_PIN_6_MODE__, __GPIO_PJ_PIN_6_SPEED__, __GPIO_PJ_PIN_6_AF__, __GPIO_PJ_PIN_6_STATE__);
_PIN_CONFIGURATION(GPIOJ, 9, 7, __GPIO_PJ_PIN_7_NAME__, __GPIO_PJ_PIN_7_MODE__, __GPIO_PJ_PIN_7_SPEED__, __GPIO_PJ_PIN_7_AF__, __GPIO_PJ_PIN_7_STATE__);
_PIN_CONFIGURATION(GPIOJ, 9, 8, __GPIO_PJ_PIN_8_NAME__, __GPIO_PJ_PIN_8_MODE__, __GPIO_PJ_PIN_8_SPEED__, __GPIO_PJ_PIN_8_AF__, __GPIO_PJ_PIN_8_STATE__);
_PIN_CONFIGURATION(GPIOJ, 9, 9, __GPIO_PJ_PIN_9_NAME__, __GPIO_PJ_PIN_9_MODE__, __GPIO_PJ_PIN_9_SPEED__, __GPIO_PJ_PIN_9_AF__, __GPIO_PJ_PIN_9_STATE__);
_PIN_CONFIGURATION(GPIOJ, 9, 10, __GPIO_PJ_PIN_10_NAME__, __GPIO_PJ_PIN_10_MODE__, __GPIO_PJ_PIN_10_SPEED__, __GPIO_PJ_PIN_10_AF__, __GPIO_PJ_PIN_10_STATE__);
_PIN_CONFIGURATION(GPIOJ, 9, 11, __GPIO_PJ_PIN_11_NAME__, __GPIO_PJ_PIN_11_MODE__, __GPIO_PJ_PIN_11_SPEED__, __GPIO_PJ_PIN_11_AF__, __GPIO_PJ_PIN_11_STATE__);
_PIN_CONFIGURATION(GPIOJ, 9, 12, __GPIO_PJ_PIN_12_NAME__, __GPIO_PJ_PIN_12_MODE__, __GPIO_PJ_PIN_12_SPEED__, __GPIO_PJ_PIN_12_AF__, __GPIO_PJ_PIN_12_STATE__);
_PIN_CONFIGURATION(GPIOJ, 9, 13, __GPIO_PJ_PIN_13_NAME__, __GPIO_PJ_PIN_13_MODE__, __GPIO_PJ_PIN_13_SPEED__, __GPIO_PJ_PIN_13_AF__, __GPIO_PJ_PIN_13_STATE__);
_PIN_CONFIGURATION(GPIOJ, 9, 14, __GPIO_PJ_PIN_14_NAME__, __GPIO_PJ_PIN_14_MODE__, __GPIO_PJ_PIN_14_SPEED__, __GPIO_PJ_PIN_14_AF__, __GPIO_PJ_PIN_14_STATE__);
_PIN_CONFIGURATION(GPIOJ, 9, 15, __GPIO_PJ_PIN_15_NAME__, __GPIO_PJ_PIN_15_MODE__, __GPIO_PJ_PIN_15_SPEED__, __GPIO_PJ_PIN_15_AF__, __GPIO_PJ_PIN_15_STATE__);
#endif

/** GPIOK pins configuration */
#if defined(RCC_AHBENR_GPIOKEN)
_PIN_CONFIGURATION(GPIOK, 10, 0, __GPIO_PK_PIN_0_NAME__, __GPIO_PK_PIN_0_MODE__, __GPIO_PK_PIN_0_SPEED__, __GPIO_PK_PIN_0_AF__, __GPIO_PK_PIN_0_STATE__);
_PIN_CONFIGURATION(GPIOK, 10, 1, __GPIO_PK_PIN_1_NAME__, __GPIO_PK_PIN_1_MODE__, __GPIO_PK_PIN_1_SPEED__, __GPIO_PK_PIN_1_AF__, __GPIO_PK_PIN_1_STATE__);
_PIN_CONFIGURATION(GPIOK, 10, 2, __GPIO_PK_PIN_2_NAME__, __GPIO_PK_PIN_2_MODE__, __GPIO_PK_PIN_2_SPEED__, __GPIO_PK_PIN_2_AF__, __GPIO_PK_PIN_2_STATE__);
_PIN_CONFIGURATION(GPIOK, 10, 3, __GPIO_PK_PIN_3_NAME__, __GPIO_PK_PIN_3_MODE__, __GPIO_PK_PIN_3_SPEED__, __GPIO_PK_PIN_3_AF__, __GPIO_PK_PIN_3_STATE__);
_PIN_CONFIGURATION(GPIOK, 10, 4, __GPIO_PK_PIN_4_NAME__, __GPIO_PK_PIN_4_MODE__, __GPIO_PK_PIN_4_SPEED__, __GPIO_PK_PIN_4_AF__, __GPIO_PK_PIN_4_STATE__);
_PIN_CONFIGURATION(GPIOK, 10, 5, __GPIO_PK_PIN_5_NAME__, __GPIO_PK_PIN_5_MODE__, __GPIO_PK_PIN_5_SPEED__, __GPIO_PK_PIN_5_AF__, __GPIO_PK_PIN_5_STATE__);
_PIN_CONFIGURATION(GPIOK, 10, 6, __GPIO_PK_PIN_6_NAME__, __GPIO_PK_PIN_6_MODE__, __GPIO_PK_PIN_6_SPEED__, __GPIO_PK_PIN_6_AF__, __GPIO_PK_PIN_6_STATE__);
_PIN_CONFIGURATION(GPIOK, 10, 7, __GPIO_PK_PIN_7_NAME__, __GPIO_PK_PIN_7_MODE__, __GPIO_PK_PIN_7_SPEED__, __GPIO_PK_PIN_7_AF__, __GPIO_PK_PIN_7_STATE__);
_PIN_CONFIGURATION(GPIOK, 10, 8, __GPIO_PK_PIN_8_NAME__, __GPIO_PK_PIN_8_MODE__, __GPIO_PK_PIN_8_SPEED__, __GPIO_PK_PIN_8_AF__, __GPIO_PK_PIN_8_STATE__);
_PIN_CONFIGURATION(GPIOK, 10, 9, __GPIO_PK_PIN_9_NAME__, __GPIO_PK_PIN_9_MODE__, __GPIO_PK_PIN_9_SPEED__, __GPIO_PK_PIN_9_AF__, __GPIO_PK_PIN_9_STATE__);
_PIN_CONFIGURATION(GPIOK, 10, 10, __GPIO_PK_PIN_10_NAME__, __GPIO_PK_PIN_10_MODE__, __GPIO_PK_PIN_10_SPEED__, __GPIO_PK_PIN_10_AF__, __GPIO_PK_PIN_10_STATE__);
_PIN_CONFIGURATION(GPIOK, 10, 11, __GPIO_PK_PIN_11_NAME__, __GPIO_PK_PIN_11_MODE__, __GPIO_PK_PIN_11_SPEED__, __GPIO_PK_PIN_11_AF__, __GPIO_PK_PIN_11_STATE__);
_PIN_CONFIGURATION(GPIOK, 10, 12, __GPIO_PK_PIN_12_NAME__, __GPIO_PK_PIN_12_MODE__, __GPIO_PK_PIN_12_SPEED__, __GPIO_PK_PIN_12_AF__, __GPIO_PK_PIN_12_STATE__);
_PIN_CONFIGURATION(GPIOK, 10, 13, __GPIO_PK_PIN_13_NAME__, __GPIO_PK_PIN_13_MODE__, __GPIO_PK_PIN_13_SPEED__, __GPIO_PK_PIN_13_AF__, __GPIO_PK_PIN_13_STATE__);
_PIN_CONFIGURATION(GPIOK, 10, 14, __GPIO_PK_PIN_14_NAME__, __GPIO_PK_PIN_14_MODE__, __GPIO_PK_PIN_14_SPEED__, __GPIO_PK_PIN_14_AF__, __GPIO_PK_PIN_14_STATE__);
_PIN_CONFIGURATION(GPIOK, 10, 15, __GPIO_PK_PIN_15_NAME__, __GPIO_PK_PIN_15_MODE__, __GPIO_PK_PIN_15_SPEED__, __GPIO_PK_PIN_15_AF__, __GPIO_PK_PIN_15_STATE__);
#endif

#define _PUPDR(_v)  (((_v) >> 0) & 3)
#define _OTYPER(_v) (((_v) >> 2) & 1)
#define _MODER(_v)  (((_v) >> 3) & 3)
#define _MODE(_m, _t, _p) ((((_m) & 3) << 3) | (((_t) & 1) << 2) | (((_p) & 3) << 0))

/*==============================================================================
  Exported types, enums definitions
==============================================================================*/
typedef enum {
        GPIO_MODE__PP       = _MODE(1, 0, 0),
        GPIO_MODE__OD       = _MODE(1, 1, 0),
        GPIO_MODE__OD_PU    = _MODE(1, 1, 1),
        GPIO_MODE__OD_PD    = _MODE(1, 1, 2),
        GPIO_MODE__AF_PP    = _MODE(2, 0, 0),
        GPIO_MODE__AF_PP_PU = _MODE(2, 0, 1),
        GPIO_MODE__AF_PP_PD = _MODE(2, 0, 2),
        GPIO_MODE__AF_OD    = _MODE(2, 1, 0),
        GPIO_MODE__AF_OD_PU = _MODE(2, 1, 1),
        GPIO_MODE__AF_OD_PD = _MODE(2, 1 ,2),
        GPIO_MODE__IN       = _MODE(0, 0, 0),
        GPIO_MODE__IN_PU    = _MODE(0, 0, 1),
        GPIO_MODE__IN_PD    = _MODE(0, 0, 2),
        GPIO_MODE__ANALOG   = _MODE(3, 0, 0),
} GPIO_mode_t;

/*==============================================================================
  Exported object declarations
==============================================================================*/

/*==============================================================================
  Exported function prototypes
==============================================================================*/

#endif /*ARCH_stm32f3*/

#ifdef __cplusplus
}
#endif

#endif /* _GPIO_CFG_H_ */
/*==============================================================================
  End of file
==============================================================================*/
