/*=========================================================================*//**
@file    afm_cfg.h

@author  Daniel Zorychta

@brief   This driver support AFM.

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

#ifndef _AFM_CFG_H_
#define _AFM_CFG_H_

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
#define _AFM_EVO_EN                    __AFM_EVENT_OUT_ENABLE__

/**
 * Event Output pin selection: 0-15
 */
#define _AFM_EVO_PIN                   __AFM_EVENT_OUT_PIN__

/**
 * Event Output port selection: 0-4 for PA-E
 */
#define _AFM_EVO_PORT                  __AFM_EVENT_OUT_PORT__


/**
 * Enable (1) or disable (0) SPI1 remap
 */
#define _AFM_REMAP_SPI1                __AFM_REMAP_SPI1__

/**
 * Enable (1) or disable (0) I2C1 remap
 */
#define _AFM_REMAP_I2C1                __AFM_REMAP_I2C1__

/**
 * Enable (1) or disable (0) USART1 remap
 */
#define _AFM_REMAP_USART1              __AFM_REMAP_USART1__

/**
 * Enable (1) or disable (0) USART2 remap
 */
#define _AFM_REMAP_USART2              __AFM_REMAP_USART2__

/**
 * USART3 remap
 * 0: No remap (TX/PB10, RX/PB11, CK/PB12, CTS/PB13, RTS/PB14)
 * 1: Partial remap (TX/PC10, RX/PC11, CK/PC12, CTS/PB13, RTS/PB14)
 * 2: Full remap (TX/PD8, RX/PD9, CK/PD10, CTS/PD11, RTS/PD12)
 */
#define _AFM_REMAP_USART3              __AFM_REMAP_USART3__

/**
 * TIM1 remap
 * 0: No remap (ETR/PA12, CH1/PA8, CH2/PA9, CH3/PA10, CH4/PA11, BKIN/PB12, CH1N/PB13, CH2N/PB14, CH3N/PB15)
 * 1: Partial remap (ETR/PA12, CH1/PA8, CH2/PA9, CH3/PA10, CH4/PA11, BKIN/PA6, CH1N/PA7, CH2N/PB0, CH3N/PB1)
 * 2: Full remap (ETR/PE7, CH1/PE9, CH2/PE11, CH3/PE13, CH4/PE14, BKIN/PE15, CH1N/PE8, CH2N/PE10, CH3N/PE12)
 */
#define _AFM_REMAP_TIM1                __AFM_REMAP_TIM1__

/**
 * TIM2 remap
 * 0: No remap (CH1/ETR/PA0, CH2/PA1, CH3/PA2, CH4/PA3)
 * 1: Partial remap (CH1/ETR/PA15, CH2/PB3, CH3/PA2, CH4/PA3)
 * 2: Partial remap (CH1/ETR/PA0, CH2/PA1, CH3/PB10, CH4/PB11)
 * 3: Full remap (CH1/ETR/PA15, CH2/PB3, CH3/PB10, CH4/PB11)
 */
#define _AFM_REMAP_TIM2                __AFM_REMAP_TIM2__

/**
 * TIM3 remap
 * 0: No remap (CH1/PA6, CH2/PA7, CH3/PB0, CH4/PB1)
 * 1: Partial remap (CH1/PB4, CH2/PB5, CH3/PB0, CH4/PB1)
 * 2: Full remap (CH1/PC6, CH2/PC7, CH3/PC8, CH4/PC9)
 */
#define _AFM_REMAP_TIM3                __AFM_REMAP_TIM3__

/**
 * Enable (1) or disable (0) TIM4 remap
 */
#define _AFM_REMAP_TIM4                __AFM_REMAP_TIM4__

/**
 * CAN remap
 * 0: CANRX mapped to PA11, CANTX mapped to PA12
 * 1: CANRX mapped to PB8, CANTX mapped to PB9
 * 2: CANRX mapped to PD0, CANTX mapped to PD1
 */
#define _AFM_REMAP_CAN                 __AFM_REMAP_CAN__

/**
 * Enable (1) or disable (0) PD01 remap
 */
#define _AFM_REMAP_PD01                __AFM_REMAP_PD01__

/**
 * Enable (1) or disable (0) TIM5CH4 remap
 */
#define _AFM_REMAP_TIM5CH4             __AFM_REMAP_TIM5CH4__

/**
 * Enable (1) or disable (0) ADC1_ENTRGINJ remap
 */
#define _AFM_REMAP_ADC1_ETRGINJ        __AFM_REMAP_ADC1_ETRGINJ__

/**
 * Enable (1) or disable (0) ADC1_ETRGREG remap
 */
#define _AFM_REMAP_ADC1_ETRGREG        __AFM_REMAP_ADC1_ETRGREG__

/**
 * Enable (1) or disable (0) ADC2_ETRGINJ remap
 */
#define _AFM_REMAP_ADC2_ETRGINJ        __AFM_REMAP_ADC2_ETRGINJ__

/**
 * Enable (1) or disable (0) ADC2_ETRGREG remap
 */
#define _AFM_REMAP_ADC2_ETRGREG        __AFM_REMAP_ADC2_ETRGREG__

/**
 * SWJ_CFG configuration
 * 0: Full SWJ (JTAG-DP + SW-DP) : Reset State
 * 1: Full SWJ (JTAG-DP + SW-DP) but without JNTRST
 * 2: JTAG-DP Disabled and SW-DP Enabled
 * 3: JTAG-DP Disabled and SW-DP Disabled
 */
#define _AFM_REMAP_SWJ_CFG             __AFM_REMAP_SWJ_CFG__

#ifdef STM32F10X_CL
/**
 * Enable (1) or disable (0) ETH remap
 */
#define _AFM_REMAP_ETH                 __AFM_REMAP_ETH__

/**
 * Enable (1) or disable (0) CAN2 remap
 */
#define _AFM_REMAP_CAN2                __AFM_REMAP_CAN2__

/**
 * Enable (1) or disable (0) MII_RMII_SEL remap
 */
#define _AFM_REMAP_MII_RMII_SEL        __AFM_REMAP_MII_RMII_SEL__

/**
 * Enable (1) or disable (0) SPI3 remap
 */
#define _AFM_REMAP_SPI3                __AFM_REMAP_SPI3__

/**
 * Enable (1) or disable (0) TIM2ITR1 remap
 */
#define _AFM_REMAP_TIM2ITR1            __AFM_REMAP_TIM2ITR1__

/**
 * Enable (1) or disable (0) PTP_PPS remap
 */
#define _AFM_REMAP_PTP_PPS             __AFM_REMAP_PTP_PPS__
#endif

#if defined(STM32F10X_LD_VL) || defined(STM32F10X_MD_VL) || defined(STM32F10X_HD_VL)
/**
 * Enable (1) or disable (0) TIM15 remap
 */
#define _AFM_REMAP_TIM15               __AFM_REMAP_TIM15__

/**
 * Enable (1) or disable (0) TIM16 remap
 */
#define _AFM_REMAP_TIM16               __AFM_REMAP_TIM16__

/**
 * Enable (1) or disable (0) TIM17 remap
 */
#define _AFM_REMAP_TIM17               __AFM_REMAP_TIM17__

/**
 * Enable (1) or disable (0) CEC remap
 */
#define _AFM_REMAP_CEC                 __AFM_REMAP_CEC__

/**
 * Enable (1) or disable (0) TIM1 remap
 */
#define _AFM_REMAP_TIM1_DMA            __AFM_REMAP_TIM1_DMA__
#endif

#ifdef STM32F10X_HD_VL
/**
 * Enable (1) or disable (0) TIM13 remap
 */
#define _AFM_REMAP_TIM13               __AFM_REMAP_TIM13__

/**
 * Enable (1) or disable (0) TIM14 remap
 */
#define _AFM_REMAP_TIM14               __AFM_REMAP_TIM14__

/**
 * Enable (1) or disable (0) FSMC_NADV remap
 */
#define _AFM_REMAP_FSMC_NADV           __AFM_REMAP_FSMC_NADV__

/**
 * Enable (1) or disable (0) TIM67_DAC_DMA remap
 */
#define _AFM_REMAP_TIM76_DAC_DMA       __AFM_REMAP_TIM76_DAC_DMA__

/**
 * Enable (1) or disable (0) TIM12 remap
 */
#define _AFM_REMAP_TIM12               __AFM_REMAP_TIM12__

/**
 * Enable (1) or disable (0) MISC remap
 */
#define _AFM_REMAP_MISC                __AFM_REMAP_MISC__
#endif

#ifdef STM32F10X_XL
/**
 * Enable (1) or disable (0) TIM9 remap
 */
#define _AFM_REMAP_TIM9                __AFM_REMAP_TIM9__

/**
 * Enable (1) or disable (0) TIM10 remap
 */
#define _AFM_REMAP_TIM10               __AFM_REMAP_TIM10__

/**
 * Enable (1) or disable (0) TIM11 remap
 */
#define _AFM_REMAP_TIM11               __AFM_REMAP_TIM11__

/**
 * Enable (1) or disable (0) TIM13 remap
 */
#define _AFM_REMAP_TIM13               __AFM_REMAP_TIM13__

/**
 * Enable (1) or disable (0) TIM14 remap
 */
#define _AFM_REMAP_TIM14               __AFM_REMAP_TIM14__

/**
 * Enable (1) or disable (0) FSMC_NADV remap
 */
#define _AFM_REMAP_FSMC_NADV           __AFM_REMAP_FSMC_NADV__
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

#endif /* _AFM_CFG_H_ */
/*==============================================================================
  End of file
==============================================================================*/
