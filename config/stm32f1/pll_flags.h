#ifndef _PLL_FLAGS_H_
#define _PLL_FLAGS_H_

#define __PLL_LSI_ON__ ENABLE
#define __PLL_LSE_ON__ RCC_LSE_OFF
#define __PLL_HSE_ON__ RCC_HSE_ON
#define __PLL_RTC_CLK_SRC__ RCC_RTCCLKSource_LSI
#define __PLL_SYS_CLK_SRC__ RCC_SYSCLKSource_PLLCLK
#define __PLL_MCO_SRC__ RCC_MCO_NoClock
#define __PLL_I2S2_SRC__ RCC_I2S2CLKSource_SYSCLK
#define __PLL_I2S3_SRC__ RCC_I2S3CLKSource_SYSCLK
#define __PLL_PLL_ON__ ENABLE
#define __PLL_PLL_SRC__ RCC_PLLSource_PREDIV1
#define __PLL_PLL_MULL__ RCC_PLLMul_9
#define __PLL_USB_DIV__ RCC_USBCLKSource_PLLCLK_1Div5
#define __PLL_PREDIV1_SRC__ RCC_PREDIV1_Source_HSE
#define __PLL_PREDIV1_VAL__ RCC_PREDIV1_Div1
#define __PLL_PREDIV2_VAL__ RCC_PREDIV2_Div1
#define __PLL_PLL2_ON__ DISABLE
#define __PLL_PLL2_MULL__ RCC_PLL2Mul_8
#define __PLL_PLL3_ON__ ENABLE
#define __PLL_PLL3_MULL__ RCC_PLL3Mul_8
#define __PLL_AHB_PRE__ RCC_SYSCLK_Div1
#define __PLL_APB1_PRE__ RCC_HCLK_Div2
#define __PLL_APB2_PRE__ RCC_HCLK_Div1
#define __PLL_ADC_PRE__ RCC_PCLK2_Div8
#define __PLL_FLASH_LATENCY__ 2

#endif /* _PLL_FLAGS_H_ */
