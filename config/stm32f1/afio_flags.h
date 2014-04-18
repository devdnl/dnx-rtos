/*=========================================================================*//**
@file    afio_flags.h

@author  Daniel Zorychta

@brief   AFIO module configuration.

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
#ifndef _AFIO_FLAGS_H_
#define _AFIO_FLAGS_H_

// AFIO_EVCR
#define __AFIO_EVENT_OUT_ENABLE__ __NO__
#define __AFIO_EVENT_OUT_PORT__ 0
#define __AFIO_EVENT_OUT_PIN__ 0

// AFIO_MAPR
#define __AFIO_REMAP_SPI1__ __NO__
#define __AFIO_REMAP_I2C1__ __NO__
#define __AFIO_REMAP_USART1__ __NO__
#define __AFIO_REMAP_USART2__ __NO__
#define __AFIO_REMAP_USART3__ 0
#define __AFIO_REMAP_TIM1__ 0
#define __AFIO_REMAP_TIM2__ 0
#define __AFIO_REMAP_TIM3__ 0
#define __AFIO_REMAP_TIM4__ __NO__
#define __AFIO_REMAP_CAN__ 0
#define __AFIO_REMAP_PD01__ __NO__
#define __AFIO_REMAP_TIM5CH4__ __NO__
#define __AFIO_REMAP_ADC1_ETRGINJ__ __NO__
#define __AFIO_REMAP_ADC1_ETRGREG__ __NO__
#define __AFIO_REMAP_ADC2_ETRGINJ__ __NO__
#define __AFIO_REMAP_ADC2_ETRGREG__ __NO__
#define __AFIO_REMAP_SWJ_CFG__ 0
#define __AFIO_REMAP_ETH__ __NO__
#define __AFIO_REMAP_CAN2__ __NO__
#define __AFIO_REMAP_MII_RMII_SEL__ __NO__
#define __AFIO_REMAP_SPI3__ __YES__
#define __AFIO_REMAP_TIM2ITR1__ __NO__
#define __AFIO_REMAP_PTP_PPS__ __NO__

// AFIO_MAPR2
#define __AFIO_REMAP_CEC__ __NO__
#define __AFIO_REMAP_TIM1_DMA__ __NO__
#define __AFIO_REMAP_TIM9__ __NO__
#define __AFIO_REMAP_TIM10__ __NO__
#define __AFIO_REMAP_TIM11__ __NO__
#define __AFIO_REMAP_TIM12__ __NO__
#define __AFIO_REMAP_TIM13__ __NO__
#define __AFIO_REMAP_TIM14__ __NO__
#define __AFIO_REMAP_TIM15__ __NO__
#define __AFIO_REMAP_TIM16__ __NO__
#define __AFIO_REMAP_TIM17__ __NO__
#define __AFIO_REMAP_FSMC_NADV__ __NO__
#define __AFIO_REMAP_TIM76_DAC_DMA__ __NO__
#define __AFIO_REMAP_MISC__ __NO__

// AFIO_EXTI
#define __AFIO_EXTI0_PORT__ 0
#define __AFIO_EXTI1_PORT__ 0
#define __AFIO_EXTI2_PORT__ 0
#define __AFIO_EXTI3_PORT__ 0
#define __AFIO_EXTI4_PORT__ 0
#define __AFIO_EXTI5_PORT__ 0
#define __AFIO_EXTI6_PORT__ 0
#define __AFIO_EXTI7_PORT__ 0
#define __AFIO_EXTI8_PORT__ 0
#define __AFIO_EXTI9_PORT__ 0
#define __AFIO_EXTI10_PORT__ 0
#define __AFIO_EXTI11_PORT__ 0
#define __AFIO_EXTI12_PORT__ 0
#define __AFIO_EXTI13_PORT__ 0
#define __AFIO_EXTI14_PORT__ 0
#define __AFIO_EXTI15_PORT__ 0

#endif /* _AFIO_FLAGS_H_ */
/*==============================================================================
  End of file
==============================================================================*/
