/*=========================================================================*//**
@file    afio_cfg.h

@author  Daniel Zorychta

@brief   This driver support AFIO.

@note    Copyright (C) 2014  Daniel Zorychta <daniel.zorychta@gmail.com>

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

#ifndef _AFIO_CFG_H_
#define _AFIO_CFG_H_

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Include files
==============================================================================*/
#include "stm32f1/stm32f10x.h"

/*==============================================================================
  Exported symbolic constants/macros
==============================================================================*/
/**
 *  Enable (1) or disable (0) Event Output
 */
#define _AFIO_EVO_EN                    __AFIO_EVENT_OUT_ENABLE__

/**
 * Event Output pin selection: 0-15
 */
#define _AFIO_EVO_PIN                   __AFIO_EVENT_OUT_PIN__

/**
 * Event Output port selection: 0-4 for PA-E
 */
#define _AFIO_EVO_PORT                  __AFIO_EVENT_OUT_PORT__


/**
 * Enable (1) or disable (0) SPI1 remap
 */
#define _AFIO_REMAP_SPI1                __AFIO_REMAP_SPI1__

/**
 * Enable (1) or disable (0) I2C1 remap
 */
#define _AFIO_REMAP_I2C1                __AFIO_REMAP_I2C1__

/**
 * Enable (1) or disable (0) USART1 remap
 */
#define _AFIO_REMAP_USART1              __AFIO_REMAP_USART1__

/**
 * Enable (1) or disable (0) USART2 remap
 */
#define _AFIO_REMAP_USART2              __AFIO_REMAP_USART2__

/**
 * USART3 remap
 * 0: No remap (TX/PB10, RX/PB11, CK/PB12, CTS/PB13, RTS/PB14)
 * 1: Partial remap (TX/PC10, RX/PC11, CK/PC12, CTS/PB13, RTS/PB14)
 * 2: Full remap (TX/PD8, RX/PD9, CK/PD10, CTS/PD11, RTS/PD12)
 */
#define _AFIO_REMAP_USART3              __AFIO_REMAP_USART3__

/**
 * TIM1 remap
 * 0: No remap (ETR/PA12, CH1/PA8, CH2/PA9, CH3/PA10, CH4/PA11, BKIN/PB12, CH1N/PB13, CH2N/PB14, CH3N/PB15)
 * 1: Partial remap (ETR/PA12, CH1/PA8, CH2/PA9, CH3/PA10, CH4/PA11, BKIN/PA6, CH1N/PA7, CH2N/PB0, CH3N/PB1)
 * 2: Full remap (ETR/PE7, CH1/PE9, CH2/PE11, CH3/PE13, CH4/PE14, BKIN/PE15, CH1N/PE8, CH2N/PE10, CH3N/PE12)
 */
#define _AFIO_REMAP_TIM1                __AFIO_REMAP_TIM1__

/**
 * TIM2 remap
 * 0: No remap (CH1/ETR/PA0, CH2/PA1, CH3/PA2, CH4/PA3)
 * 1: Partial remap (CH1/ETR/PA15, CH2/PB3, CH3/PA2, CH4/PA3)
 * 2: Partial remap (CH1/ETR/PA0, CH2/PA1, CH3/PB10, CH4/PB11)
 * 3: Full remap (CH1/ETR/PA15, CH2/PB3, CH3/PB10, CH4/PB11)
 */
#define _AFIO_REMAP_TIM2                __AFIO_REMAP_TIM2__

/**
 * TIM3 remap
 * 0: No remap (CH1/PA6, CH2/PA7, CH3/PB0, CH4/PB1)
 * 1: Partial remap (CH1/PB4, CH2/PB5, CH3/PB0, CH4/PB1)
 * 2: Full remap (CH1/PC6, CH2/PC7, CH3/PC8, CH4/PC9)
 */
#define _AFIO_REMAP_TIM3                __AFIO_REMAP_TIM3__

/**
 * Enable (1) or disable (0) TIM4 remap
 */
#define _AFIO_REMAP_TIM4                __AFIO_REMAP_TIM4__

/**
 * CAN remap
 * 0: CANRX mapped to PA11, CANTX mapped to PA12
 * 1: CANRX mapped to PB8, CANTX mapped to PB9
 * 2: CANRX mapped to PD0, CANTX mapped to PD1
 */
#define _AFIO_REMAP_CAN                 __AFIO_REMAP_CAN__

/**
 * Enable (1) or disable (0) PD01 remap
 */
#define _AFIO_REMAP_PD01                __AFIO_REMAP_PD01__

/**
 * Enable (1) or disable (0) TIM5CH4 remap
 */
#define _AFIO_REMAP_TIM5CH4             __AFIO_REMAP_TIM5CH4__

/**
 * Enable (1) or disable (0) ADC1_ENTRGINJ remap
 */
#define _AFIO_REMAP_ADC1_ETRGINJ        __AFIO_REMAP_ADC1_ETRGINJ__

/**
 * Enable (1) or disable (0) ADC1_ETRGREG remap
 */
#define _AFIO_REMAP_ADC1_ETRGREG        __AFIO_REMAP_ADC1_ETRGREG__

/**
 * Enable (1) or disable (0) ADC2_ETRGINJ remap
 */
#define _AFIO_REMAP_ADC2_ETRGINJ        __AFIO_REMAP_ADC2_ETRGINJ__

/**
 * Enable (1) or disable (0) ADC2_ETRGREG remap
 */
#define _AFIO_REMAP_ADC2_ETRGREG        __AFIO_REMAP_ADC2_ETRGREG__

/**
 * SWJ_CFG configuration
 * 0: Full SWJ (JTAG-DP + SW-DP) : Reset State
 * 1: Full SWJ (JTAG-DP + SW-DP) but without JNTRST
 * 2: JTAG-DP Disabled and SW-DP Enabled
 * 3: JTAG-DP Disabled and SW-DP Disabled
 */
#define _AFIO_REMAP_SWJ_CFG             __AFIO_REMAP_SWJ_CFG__

#ifdef STM32F10X_CL
/**
 * Enable (1) or disable (0) ETH remap
 */
#define _AFIO_REMAP_ETH                 __AFIO_REMAP_ETH__

/**
 * Enable (1) or disable (0) CAN2 remap
 */
#define _AFIO_REMAP_CAN2                __AFIO_REMAP_CAN2__

/**
 * Enable (1) or disable (0) MII_RMII_SEL remap
 */
#define _AFIO_REMAP_MII_RMII_SEL        __AFIO_REMAP_MII_RMII_SEL__

/**
 * Enable (1) or disable (0) SPI3 remap
 */
#define _AFIO_REMAP_SPI3                __AFIO_REMAP_SPI3__

/**
 * Enable (1) or disable (0) TIM2ITR1 remap
 */
#define _AFIO_REMAP_TIM2ITR1            __AFIO_REMAP_TIM2ITR1__

/**
 * Enable (1) or disable (0) PTP_PPS remap
 */
#define _AFIO_REMAP_PTP_PPS             __AFIO_REMAP_PTP_PPS__
#endif

#if defined(STM32F10X_LD_VL) || defined(STM32F10X_MD_VL) || defined(STM32F10X_HD_VL)
/**
 * Enable (1) or disable (0) TIM15 remap
 */
#define _AFIO_REMAP_TIM15               __AFIO_REMAP_TIM15__

/**
 * Enable (1) or disable (0) TIM16 remap
 */
#define _AFIO_REMAP_TIM16               __AFIO_REMAP_TIM16__

/**
 * Enable (1) or disable (0) TIM17 remap
 */
#define _AFIO_REMAP_TIM17               __AFIO_REMAP_TIM17__

/**
 * Enable (1) or disable (0) CEC remap
 */
#define _AFIO_REMAP_CEC                 __AFIO_REMAP_CEC__

/**
 * Enable (1) or disable (0) TIM1 remap
 */
#define _AFIO_REMAP_TIM1_DMA            __AFIO_REMAP_TIM1_DMA__
#endif

#ifdef STM32F10X_HD_VL
/**
 * Enable (1) or disable (0) TIM13 remap
 */
#define _AFIO_REMAP_TIM13               __AFIO_REMAP_TIM13__

/**
 * Enable (1) or disable (0) TIM14 remap
 */
#define _AFIO_REMAP_TIM14               __AFIO_REMAP_TIM14__

/**
 * Enable (1) or disable (0) FSMC_NADV remap
 */
#define _AFIO_REMAP_FSMC_NADV           __AFIO_REMAP_FSMC_NADV__

/**
 * Enable (1) or disable (0) TIM67_DAC_DMA remap
 */
#define _AFIO_REMAP_TIM76_DAC_DMA       __AFIO_REMAP_TIM76_DAC_DMA__

/**
 * Enable (1) or disable (0) TIM12 remap
 */
#define _AFIO_REMAP_TIM12               __AFIO_REMAP_TIM12__

/**
 * Enable (1) or disable (0) MISC remap
 */
#define _AFIO_REMAP_MISC                __AFIO_REMAP_MISC__
#endif

#ifdef STM32F10X_XL
/**
 * Enable (1) or disable (0) TIM9 remap
 */
#define _AFIO_REMAP_TIM9                __AFIO_REMAP_TIM9__

/**
 * Enable (1) or disable (0) TIM10 remap
 */
#define _AFIO_REMAP_TIM10               __AFIO_REMAP_TIM10__

/**
 * Enable (1) or disable (0) TIM11 remap
 */
#define _AFIO_REMAP_TIM11               __AFIO_REMAP_TIM11__

/**
 * Enable (1) or disable (0) TIM13 remap
 */
#define _AFIO_REMAP_TIM13               __AFIO_REMAP_TIM13__

/**
 * Enable (1) or disable (0) TIM14 remap
 */
#define _AFIO_REMAP_TIM14               __AFIO_REMAP_TIM14__

/**
 * Enable (1) or disable (0) FSMC_NADV remap
 */
#define _AFIO_REMAP_FSMC_NADV           __AFIO_REMAP_FSMC_NADV__
#endif

/**
 * EXTI0 PORT SELECTION: 0-6 for PA-PG
 */
#define _AFIO_EXTI0_PORT                __AFIO_EXTI0_PORT__

/**
 * EXTI1 PORT SELECTION: 0-6 for PA-PG
 */
#define _AFIO_EXTI1_PORT                __AFIO_EXTI1_PORT__

/**
 * EXTI2 PORT SELECTION: 0-6 for PA-PG
 */
#define _AFIO_EXTI2_PORT                __AFIO_EXTI2_PORT__

/**
 * EXTI3 PORT SELECTION: 0-6 for PA-PG
 */
#define _AFIO_EXTI3_PORT                __AFIO_EXTI3_PORT__

/**
 * EXTI4 PORT SELECTION: 0-6 for PA-PG
 */
#define _AFIO_EXTI4_PORT                __AFIO_EXTI4_PORT__

/**
 * EXTI5 PORT SELECTION: 0-6 for PA-PG
 */
#define _AFIO_EXTI5_PORT                __AFIO_EXTI5_PORT__

/**
 * EXTI6 PORT SELECTION: 0-6 for PA-PG
 */
#define _AFIO_EXTI6_PORT                __AFIO_EXTI6_PORT__

/**
 * EXTI7 PORT SELECTION: 0-6 for PA-PG
 */
#define _AFIO_EXTI7_PORT                __AFIO_EXTI7_PORT__

/**
 * EXTI8 PORT SELECTION: 0-6 for PA-PG
 */
#define _AFIO_EXTI8_PORT                __AFIO_EXTI8_PORT__

/**
 * EXTI9 PORT SELECTION: 0-6 for PA-PG
 */
#define _AFIO_EXTI9_PORT                __AFIO_EXTI9_PORT__

/**
 * EXTI10 PORT SELECTION: 0-6 for PA-PG
 */
#define _AFIO_EXTI10_PORT               __AFIO_EXTI10_PORT__

/**
 * EXTI11 PORT SELECTION: 0-6 for PA-PG
 */
#define _AFIO_EXTI11_PORT               __AFIO_EXTI11_PORT__

/**
 * EXTI12 PORT SELECTION: 0-6 for PA-PG
 */
#define _AFIO_EXTI12_PORT               __AFIO_EXTI12_PORT__

/**
 * EXTI13 PORT SELECTION: 0-6 for PA-PG
 */
#define _AFIO_EXTI13_PORT               __AFIO_EXTI13_PORT__

/**
 * EXTI14 PORT SELECTION: 0-6 for PA-PG
 */
#define _AFIO_EXTI14_PORT               __AFIO_EXTI14_PORT__

/**
 * EXTI15 PORT SELECTION: 0-6 for PA-PG
 */
#define _AFIO_EXTI15_PORT               __AFIO_EXTI15_PORT__

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

#endif /* _AFIO_CFG_H_ */
/*==============================================================================
  End of file
==============================================================================*/
