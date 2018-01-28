/*==============================================================================
File     efr32xx_vectors.c

Author   Daniel Zorychta

Brief    EFR32xx family vectors.

         Copyright (C) 2017 Daniel Zorychta <daniel.zorychta@gmail.com>

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

==============================================================================*/
#include "config.h"

/** define weak assign to the default function */
#define WEAK_DEFAULT __attribute__ ((interrupt, weak, alias("__Default_Handler")))

/*============================================================================*/
/**
* \brief Default interrupt handler.
* \details Default interrupt handler, used for interrupts that don't have their
* own handler defined.
*/
/*============================================================================*/
static void __Default_Handler(void) __attribute__ ((interrupt));
static void __Default_Handler(void)
{
	while (1);
}

/*==============================================================================
Assign all unused interrupts to the default handler
==============================================================================*/
WEAK_DEFAULT void NMI_Handler(void);
WEAK_DEFAULT void HardFault_Handler(void);
WEAK_DEFAULT void MemManage_Handler(void);
WEAK_DEFAULT void BusFault_Handler(void);
WEAK_DEFAULT void UsageFault_Handler(void);
WEAK_DEFAULT void __Reserved_0x1C_Handler(void);
WEAK_DEFAULT void __Reserved_0x20_Handler(void);
WEAK_DEFAULT void __Reserved_0x24_Handler(void);
WEAK_DEFAULT void __Reserved_0x28_Handler(void);
WEAK_DEFAULT void SVC_Handler(void);
WEAK_DEFAULT void DebugMon_Handler(void);
WEAK_DEFAULT void __Reserved_0x34_Handler(void);
WEAK_DEFAULT void PendSV_Handler(void);
WEAK_DEFAULT void SysTick_Handler(void);

/* EFR32 specific Interrupt Numbers *******************************************/
#if defined(EFR32MG1B732F256IM32) ||  defined(EFR32MG1B131F256GM32)\
||  defined(EFR32MG1B732F256GM32) ||  defined(EFR32MG1B632F256IM32)\
||  defined(EFR32MG1B632F256GM32) ||  defined(EFR32MG1B232F256IM48)\
||  defined(EFR32MG1B232F256GM48) ||  defined(EFR32MG1B232F256GM32)\
||  defined(EFR32MG1B232F256GJ43) ||  defined(EFR32MG1B231F256GM48)\
||  defined(EFR32MG1B231F256GM32) ||  defined(EFR32MG1B132F256GM48)\
||  defined(EFR32MG1B132F256GM32) ||  defined(EFR32MG1B132F256GJ43)\
||  defined(EFR32MG1B131F256GM48) ||  defined(EFR32MG1P732F256IM32)\
||  defined(EFR32MG1P131F256GM48) ||  defined(EFR32MG1P732F256GM32)\
||  defined(EFR32MG1P632F256IM32) ||  defined(EFR32MG1P632F256GM32)\
||  defined(EFR32MG1P233F256GM48) ||  defined(EFR32MG1P232F256GM48)\
||  defined(EFR32MG1P232F256GM32) ||  defined(EFR32MG1P232F256GJ43)\
||  defined(EFR32MG1P231F256GM48) ||  defined(EFR32MG1P133F256GM48)\
||  defined(EFR32MG1P132F256IM32) ||  defined(EFR32MG1P132F256GM48)\
||  defined(EFR32MG1P132F256GM32) ||  defined(EFR32MG1P132F256GJ43)\
||  defined(EFR32MG1V132F256GM48) ||  defined(EFR32MG1V131F256GM32)\
||  defined(EFR32MG1V132F256GM32) ||  defined(EFR32MG1V131F256GM48)
WEAK_DEFAULT void EMU_Handler(void);
WEAK_DEFAULT void __Reserved_1_Handler(void);
WEAK_DEFAULT void WDOG0_Handler(void);
WEAK_DEFAULT void __Reserved_3_Handler(void);
WEAK_DEFAULT void __Reserved_4_Handler(void);
WEAK_DEFAULT void __Reserved_5_Handler(void);
WEAK_DEFAULT void __Reserved_6_Handler(void);
WEAK_DEFAULT void __Reserved_7_Handler(void);
WEAK_DEFAULT void LDMA_Handler(void);
WEAK_DEFAULT void GPIO_EVEN_Handler(void);
WEAK_DEFAULT void TIMER0_Handler(void);
WEAK_DEFAULT void USART0_RX_Handler(void);
WEAK_DEFAULT void USART0_TX_Handler(void);
WEAK_DEFAULT void ACMP0_Handler(void);
WEAK_DEFAULT void ADC0_Handler(void);
WEAK_DEFAULT void IDAC0_Handler(void);
WEAK_DEFAULT void I2C0_Handler(void);
WEAK_DEFAULT void GPIO_ODD_Handler(void);
WEAK_DEFAULT void TIMER1_Handler(void);
WEAK_DEFAULT void USART1_RX_Handler(void);
WEAK_DEFAULT void USART1_TX_Handler(void);
WEAK_DEFAULT void LEUART0_Handler(void);
WEAK_DEFAULT void PCNT0_Handler(void);
WEAK_DEFAULT void CMU_Handler(void);
WEAK_DEFAULT void MSC_Handler(void);
WEAK_DEFAULT void CRYPTO_Handler(void);
WEAK_DEFAULT void LETIMER0_Handler(void);
WEAK_DEFAULT void __Reserved_27_Handler(void);
WEAK_DEFAULT void __Reserved_28_Handler(void);
WEAK_DEFAULT void RTCC_Handler(void);
WEAK_DEFAULT void __Reserved_30_Handler(void);
WEAK_DEFAULT void CRYOTIMER_Handler(void);
WEAK_DEFAULT void __Reserved_32_Handler(void);
WEAK_DEFAULT void FPUEH_Handler(void);
#endif

#if defined(EFR32MG12P433F1024GM48)  || defined(EFR32MG12P132F1024GL125)\
||  defined(EFR32MG12P433F1024GL125) || defined(EFR32MG12P432F1024GM48)\
||  defined(EFR32MG12P432F1024GL125) || defined(EFR32MG12P332F1024GM48)\
||  defined(EFR32MG12P332F1024GL125) || defined(EFR32MG12P232F1024GM48)\
||  defined(EFR32MG12P232F1024GL125) || defined(EFR32MG12P132F1024GM48)
WEAK_DEFAULT void EMU_Handler(void);
WEAK_DEFAULT void __Reserved_1_Handler(void);
WEAK_DEFAULT void WDOG0_Handler(void);
WEAK_DEFAULT void WDOG1_Handler(void);
WEAK_DEFAULT void __Reserved_4_Handler(void);
WEAK_DEFAULT void __Reserved_5_Handler(void);
WEAK_DEFAULT void __Reserved_6_Handler(void);
WEAK_DEFAULT void __Reserved_7_Handler(void);
WEAK_DEFAULT void __Reserved_8_Handler(void);
WEAK_DEFAULT void LDMA_Handler(void);
WEAK_DEFAULT void GPIO_EVEN_Handler(void);
WEAK_DEFAULT void TIMER0_Handler(void);
WEAK_DEFAULT void USART0_RX_Handler(void);
WEAK_DEFAULT void USART0_TX_Handler(void);
WEAK_DEFAULT void ACMP0_Handler(void);
WEAK_DEFAULT void ADC0_Handler(void);
WEAK_DEFAULT void IDAC0_Handler(void);
WEAK_DEFAULT void I2C0_Handler(void);
WEAK_DEFAULT void GPIO_ODD_Handler(void);
WEAK_DEFAULT void TIMER1_Handler(void);
WEAK_DEFAULT void USART1_RX_Handler(void);
WEAK_DEFAULT void USART1_TX_Handler(void);
WEAK_DEFAULT void LEUART0_Handler(void);
WEAK_DEFAULT void PCNT0_Handler(void);
WEAK_DEFAULT void CMU_Handler(void);
WEAK_DEFAULT void MSC_Handler(void);
WEAK_DEFAULT void CRYPTO0_Handler(void);
WEAK_DEFAULT void LETIMER0_Handler(void);
WEAK_DEFAULT void __Reserved_28_Handler(void);
WEAK_DEFAULT void __Reserved_29_Handler(void);
WEAK_DEFAULT void RTCC_Handler(void);
WEAK_DEFAULT void __Reserved_31_Handler(void);
WEAK_DEFAULT void CRYOTIMER_Handler(void);
WEAK_DEFAULT void __Reserved_66_Handler(void);
WEAK_DEFAULT void FPUEH_Handler(void);
WEAK_DEFAULT void SMU_Handler(void);
WEAK_DEFAULT void WTIMER0_Handler(void);
WEAK_DEFAULT void WTIMER1_Handler(void);
WEAK_DEFAULT void PCNT1_Handler(void);
WEAK_DEFAULT void PCNT2_Handler(void);
WEAK_DEFAULT void USART2_RX_Handler(void);
WEAK_DEFAULT void USART2_TX_Handler(void);
WEAK_DEFAULT void I2C1_Handler(void);
WEAK_DEFAULT void USART3_RX_Handler(void);
WEAK_DEFAULT void USART3_TX_Handler(void);
WEAK_DEFAULT void VDAC0_Handler(void);
WEAK_DEFAULT void CSEN_Handler(void);
WEAK_DEFAULT void LESENSE_Handler(void);
WEAK_DEFAULT void CRYPTO1_Handler(void);
WEAK_DEFAULT void TRNG0_Handler(void);
#endif

#if defined(EFR32MG13P932F512GM48) || defined(EFR32MG13P932F512IM48)\
||  defined(EFR32MG13P832F512IM48) || defined(EFR32MG13P832F512GM48)\
||  defined(EFR32MG13P733F512GM48) || defined(EFR32MG13P732F512GM48)\
||  defined(EFR32MG13P732F512GM32) || defined(EFR32MG13P632F512GM48)\
||  defined(EFR32MG13P632F512GM32)
WEAK_DEFAULT void EMU_Handler(void);
WEAK_DEFAULT void __Reserved_1_Handler(void);
WEAK_DEFAULT void WDOG0_Handler(void);
WEAK_DEFAULT void WDOG1_Handler(void);
WEAK_DEFAULT void __Reserved_4_Handler(void);
WEAK_DEFAULT void __Reserved_5_Handler(void);
WEAK_DEFAULT void __Reserved_6_Handler(void);
WEAK_DEFAULT void __Reserved_7_Handler(void);
WEAK_DEFAULT void __Reserved_8_Handler(void);
WEAK_DEFAULT void LDMA_Handler(void);
WEAK_DEFAULT void GPIO_EVEN_Handler(void);
WEAK_DEFAULT void TIMER0_Handler(void);
WEAK_DEFAULT void USART0_RX_Handler(void);
WEAK_DEFAULT void USART0_TX_Handler(void);
WEAK_DEFAULT void ACMP0_Handler(void);
WEAK_DEFAULT void ADC0_Handler(void);
WEAK_DEFAULT void IDAC0_Handler(void);
WEAK_DEFAULT void I2C0_Handler(void);
WEAK_DEFAULT void GPIO_ODD_Handler(void);
WEAK_DEFAULT void TIMER1_Handler(void);
WEAK_DEFAULT void USART1_RX_Handler(void);
WEAK_DEFAULT void USART1_TX_Handler(void);
WEAK_DEFAULT void LEUART0_Handler(void);
WEAK_DEFAULT void PCNT0_Handler(void);
WEAK_DEFAULT void CMU_Handler(void);
WEAK_DEFAULT void MSC_Handler(void);
WEAK_DEFAULT void CRYPTO0_Handler(void);
WEAK_DEFAULT void LETIMER0_Handler(void);
WEAK_DEFAULT void __Reserved_28_Handler(void);
WEAK_DEFAULT void __Reserved_29_Handler(void);
WEAK_DEFAULT void __Reserved_30_Handler(void);
WEAK_DEFAULT void RTCC_Handler(void);
WEAK_DEFAULT void __Reserved_32_Handler(void);
WEAK_DEFAULT void CRYOTIMER_Handler(void);
WEAK_DEFAULT void __Reserved_34_Handler(void);
WEAK_DEFAULT void FPUEH_Handler(void);
WEAK_DEFAULT void SMU_Handler(void);
WEAK_DEFAULT void WTIMER0_Handler(void);
WEAK_DEFAULT void USART2_RX_Handler(void);
WEAK_DEFAULT void USART2_TX_Handler(void);
WEAK_DEFAULT void I2C1_Handler(void);
WEAK_DEFAULT void VDAC0_Handler(void);
WEAK_DEFAULT void CSEN_Handler(void);
WEAK_DEFAULT void LESENSE_Handler(void);
WEAK_DEFAULT void CRYPTO1_Handler(void);
WEAK_DEFAULT void TRNG0_Handler(void);
#endif

/*==================================================================================================
Vector table
==================================================================================================*/
extern const char __main_stack_end;          /* imported main stack end (from linker script)      */

void Reset_Handler(void);                    /* import the address of Reset_Handler()             */

void (*const vectors[])(void) __attribute__ ((section(".vectors"))) =
{
        (void (*)(void))&__main_stack_end,      // Main stack end address
        Reset_Handler,                          // Reset
        NMI_Handler,                            // Non-maskable interrupt (RCC clock security system)
        HardFault_Handler,                      // All class of fault
        MemManage_Handler,                      // Memory management
        BusFault_Handler,                       // Pre-fetch fault, memory access fault
        UsageFault_Handler,                     // Undefined instruction or illegal state
        __Reserved_0x1C_Handler,                // Reserved 0x1C
        __Reserved_0x20_Handler,                 // Reserved 0x20
        __Reserved_0x24_Handler,                // Reserved 0x24
        __Reserved_0x28_Handler,                // Reserved 0x28
        SVC_Handler,                            // System service call via SWI instruction
        DebugMon_Handler,                       // Debug monitor
        __Reserved_0x34_Handler,                // Reserved 0x34
        PendSV_Handler,                         // Pendable request for system service
        SysTick_Handler,                        // System tick timer

#if defined(EFR32MG1B732F256IM32) ||  defined(EFR32MG1B131F256GM32)\
||  defined(EFR32MG1B732F256GM32) ||  defined(EFR32MG1B632F256IM32)\
||  defined(EFR32MG1B632F256GM32) ||  defined(EFR32MG1B232F256IM48)\
||  defined(EFR32MG1B232F256GM48) ||  defined(EFR32MG1B232F256GM32)\
||  defined(EFR32MG1B232F256GJ43) ||  defined(EFR32MG1B231F256GM48)\
||  defined(EFR32MG1B231F256GM32) ||  defined(EFR32MG1B132F256GM48)\
||  defined(EFR32MG1B132F256GM32) ||  defined(EFR32MG1B132F256GJ43)\
||  defined(EFR32MG1B131F256GM48) ||  defined(EFR32MG1P732F256IM32)\
||  defined(EFR32MG1P131F256GM48) ||  defined(EFR32MG1P732F256GM32)\
||  defined(EFR32MG1P632F256IM32) ||  defined(EFR32MG1P632F256GM32)\
||  defined(EFR32MG1P233F256GM48) ||  defined(EFR32MG1P232F256GM48)\
||  defined(EFR32MG1P232F256GM32) ||  defined(EFR32MG1P232F256GJ43)\
||  defined(EFR32MG1P231F256GM48) ||  defined(EFR32MG1P133F256GM48)\
||  defined(EFR32MG1P132F256IM32) ||  defined(EFR32MG1P132F256GM48)\
||  defined(EFR32MG1P132F256GM32) ||  defined(EFR32MG1P132F256GJ43)\
||  defined(EFR32MG1V132F256GM48) ||  defined(EFR32MG1V131F256GM32)\
||  defined(EFR32MG1V132F256GM32) ||  defined(EFR32MG1V131F256GM48)
        EMU_Handler,                            // 0,  EFR32 EMU Interrupt
        __Reserved_1_Handler,                   // 1,  reserved Interrupt
        WDOG0_Handler,                          // 2,  EFR32 WDOG0 Interrupt
        __Reserved_3_Handler,                   // 3,  reserved Interrupt
        __Reserved_4_Handler,                   // 4,  reserved Interrupt
        __Reserved_5_Handler,                   // 5,  reserved Interrupt
        __Reserved_6_Handler,                   // 6,  reserved Interrupt
        __Reserved_7_Handler,                   // 7,  reserved Interrupt
        LDMA_Handler,                           // 8,  EFR32 LDMA Interrupt
        GPIO_EVEN_Handler,                      // 9,  EFR32 GPIO_EVEN Interrupt
        TIMER0_Handler,                         // 10, EFR32 TIMER0 Interrupt
        USART0_RX_Handler,                      // 11, EFR32 USART0_RX Interrupt
        USART0_TX_Handler,                      // 12, EFR32 USART0_TX Interrupt
        ACMP0_Handler,                          // 13, EFR32 ACMP0 Interrupt
        ADC0_Handler,                           // 14, EFR32 ADC0 Interrupt
        IDAC0_Handler,                          // 15, EFR32 IDAC0 Interrupt
        I2C0_Handler,                           // 16, EFR32 I2C0 Interrupt
        GPIO_ODD_Handler,                       // 17, EFR32 GPIO_ODD Interrupt
        TIMER1_Handler,                         // 18, EFR32 TIMER1 Interrupt
        USART1_RX_Handler,                      // 19, EFR32 USART1_RX Interrupt
        USART1_TX_Handler,                      // 20, EFR32 USART1_TX Interrupt
        LEUART0_Handler,                        // 21, EFR32 LEUART0 Interrupt
        PCNT0_Handler,                          // 22, EFR32 PCNT0 Interrupt
        CMU_Handler,                            // 23, EFR32 CMU Interrupt
        MSC_Handler,                            // 24, EFR32 MSC Interrupt
        CRYPTO_Handler,                         // 25, EFR32 CRYPTO Interrupt
        LETIMER0_Handler,                       // 26, EFR32 LETIMER0 Interrupt
        __Reserved_27_Handler,                  // 27, reserved Interrupt
        __Reserved_28_Handler,                  // 28, reserved Interrupt
        RTCC_Handler,                           // 29, EFR32 RTCC Interrupt
        __Reserved_30_Handler,                  // 30, reserved Interrupt
        CRYOTIMER_Handler,                      // 31, EFR32 CRYOTIMER Interrupt
        __Reserved_32_Handler,                  // 32, reserved Interrupt
        FPUEH_Handler,                          // 33, EFR32 FPUEH Interrupt
#endif

#if defined(EFR32MG12P433F1024GM48)  || defined(EFR32MG12P132F1024GL125)\
||  defined(EFR32MG12P433F1024GL125) || defined(EFR32MG12P432F1024GM48)\
||  defined(EFR32MG12P432F1024GL125) || defined(EFR32MG12P332F1024GM48)\
||  defined(EFR32MG12P332F1024GL125) || defined(EFR32MG12P232F1024GM48)\
||  defined(EFR32MG12P232F1024GL125) || defined(EFR32MG12P132F1024GM48)
        EMU_Handler,                            // 0  EFR32 EMU Interrupt
        __Reserved_1_Handler,                   // 1, reserved Interrupt
        WDOG0_Handler,                          // 2  EFR32 WDOG0 Interrupt
        WDOG1_Handler,                          // 3  EFR32 WDOG1 Interrupt
        __Reserved_4_Handler,                   // 4, reserved Interrupt
        __Reserved_5_Handler,                   // 5, reserved Interrupt
        __Reserved_6_Handler,                   // 6, reserved Interrupt
        __Reserved_7_Handler,                   // 7, reserved Interrupt
        __Reserved_8_Handler,                   // 8, reserved Interrupt
        LDMA_Handler,                           // 9  EFR32 LDMA Interrupt
        GPIO_EVEN_Handler,                      // 10 EFR32 GPIO_EVEN Interrupt
        TIMER0_Handler,                         // 11 EFR32 TIMER0 Interrupt
        USART0_RX_Handler,                      // 12 EFR32 USART0_RX Interrupt
        USART0_TX_Handler,                      // 13 EFR32 USART0_TX Interrupt
        ACMP0_Handler,                          // 14 EFR32 ACMP0 Interrupt
        ADC0_Handler,                           // 15 EFR32 ADC0 Interrupt
        IDAC0_Handler,                          // 16 EFR32 IDAC0 Interrupt
        I2C0_Handler,                           // 17 EFR32 I2C0 Interrupt
        GPIO_ODD_Handler,                       // 18 EFR32 GPIO_ODD Interrupt
        TIMER1_Handler,                         // 19 EFR32 TIMER1 Interrupt
        USART1_RX_Handler,                      // 20 EFR32 USART1_RX Interrupt
        USART1_TX_Handler,                      // 21 EFR32 USART1_TX Interrupt
        LEUART0_Handler,                        // 22 EFR32 LEUART0 Interrupt
        PCNT0_Handler,                          // 23 EFR32 PCNT0 Interrupt
        CMU_Handler,                            // 24 EFR32 CMU Interrupt
        MSC_Handler,                            // 25 EFR32 MSC Interrupt
        CRYPTO0_Handler,                        // 26 EFR32 CRYPTO0 Interrupt
        LETIMER0_Handler,                       // 27 EFR32 LETIMER0 Interrupt
        __Reserved_28_Handler,                  // 28,reserved Interrupt
        __Reserved_29_Handler,                  // 29,reserved Interrupt
        RTCC_Handler,                           // 30 EFR32 RTCC Interrupt
        __Reserved_31_Handler,                  // 31,reserved Interrupt
        CRYOTIMER_Handler,                      // 32 EFR32 CRYOTIMER Interrupt
        __Reserved_66_Handler,                  // 33 reserved Interrupt
        FPUEH_Handler,                          // 34 EFR32 FPUEH Interrupt
        SMU_Handler,                            // 35 EFR32 SMU Interrupt
        WTIMER0_Handler,                        // 36 EFR32 WTIMER0 Interrupt
        WTIMER1_Handler,                        // 37 EFR32 WTIMER1 Interrupt
        PCNT1_Handler,                          // 38 EFR32 PCNT1 Interrupt
        PCNT2_Handler,                          // 39 EFR32 PCNT2 Interrupt
        USART2_RX_Handler,                      // 40 EFR32 USART2_RX Interrupt
        USART2_TX_Handler,                      // 41 EFR32 USART2_TX Interrupt
        I2C1_Handler,                           // 42 EFR32 I2C1 Interrupt
        USART3_RX_Handler,                      // 43 EFR32 USART3_RX Interrupt
        USART3_TX_Handler,                      // 44 EFR32 USART3_TX Interrupt
        VDAC0_Handler,                          // 45 EFR32 VDAC0 Interrupt
        CSEN_Handler,                           // 46 EFR32 CSEN Interrupt
        LESENSE_Handler,                        // 47 EFR32 LESENSE Interrupt
        CRYPTO1_Handler,                        // 48 EFR32 CRYPTO1 Interrupt
        TRNG0_Handler,                          // 49 EFR32 TRNG0 Interrupt
#endif

#if defined(EFR32MG13P932F512GM48) || defined(EFR32MG13P932F512IM48)\
||  defined(EFR32MG13P832F512IM48) || defined(EFR32MG13P832F512GM48)\
||  defined(EFR32MG13P733F512GM48) || defined(EFR32MG13P732F512GM48)\
||  defined(EFR32MG13P732F512GM32) || defined(EFR32MG13P632F512GM48)\
||  defined(EFR32MG13P632F512GM32)
        EMU_Handler,                            // 0  EFR32 EMU Interrupt
        __Reserved_1_Handler,                   // 1  reserved Interrupt
        WDOG0_Handler,                          // 2  EFR32 WDOG0 Interrupt
        WDOG1_Handler,                          // 3  EFR32 WDOG1 Interrupt
        __Reserved_4_Handler,                   // 4  reserved Interrupt
        __Reserved_5_Handler,                   // 5  reserved Interrupt
        __Reserved_6_Handler,                   // 6  reserved Interrupt
        __Reserved_7_Handler,                   // 7  reserved Interrupt
        __Reserved_8_Handler,                   // 8  reserved Interrupt
        LDMA_Handler,                           // 9  EFR32 LDMA Interrupt
        GPIO_EVEN_Handler,                      // 10 EFR32 GPIO_EVEN Interrupt
        TIMER0_Handler,                         // 11 EFR32 TIMER0 Interrupt
        USART0_RX_Handler,                      // 12 EFR32 USART0_RX Interrupt
        USART0_TX_Handler,                      // 13 EFR32 USART0_TX Interrupt
        ACMP0_Handler,                          // 14 EFR32 ACMP0 Interrupt
        ADC0_Handler,                           // 15 EFR32 ADC0 Interrupt
        IDAC0_Handler,                          // 16 EFR32 IDAC0 Interrupt
        I2C0_Handler,                           // 17 EFR32 I2C0 Interrupt
        GPIO_ODD_Handler,                       // 18 EFR32 GPIO_ODD Interrupt
        TIMER1_Handler,                         // 19 EFR32 TIMER1 Interrupt
        USART1_RX_Handler,                      // 20 EFR32 USART1_RX Interrupt
        USART1_TX_Handler,                      // 21 EFR32 USART1_TX Interrupt
        LEUART0_Handler,                        // 22 EFR32 LEUART0 Interrupt
        PCNT0_Handler,                          // 23 EFR32 PCNT0 Interrupt
        CMU_Handler,                            // 24 EFR32 CMU Interrupt
        MSC_Handler,                            // 25 EFR32 MSC Interrupt
        CRYPTO0_Handler,                        // 26 EFR32 CRYPTO0 Interrupt
        LETIMER0_Handler,                       // 27 EFR32 LETIMER0 Interrupt
        __Reserved_28_Handler,                  // 28 reserved Interrupt
        __Reserved_29_Handler,                  // 29 reserved Interrupt
        __Reserved_30_Handler,                  // 30 reserved Interrupt
        RTCC_Handler,                           // 31 EFR32 RTCC Interrupt
        __Reserved_32_Handler,                  // 32 reserved Interrupt
        CRYOTIMER_Handler,                      // 33 EFR32 CRYOTIMER Interrupt
        __Reserved_34_Handler,                  // 34 reserved Interrupt
        FPUEH_Handler,                          // 35 EFR32 FPUEH Interrupt
        SMU_Handler,                            // 36 EFR32 SMU Interrupt
        WTIMER0_Handler,                        // 37 EFR32 WTIMER0 Interrupt
        USART2_RX_Handler,                      // 38 EFR32 USART2_RX Interrupt
        USART2_TX_Handler,                      // 39 EFR32 USART2_TX Interrupt
        I2C1_Handler,                           // 40 EFR32 I2C1 Interrupt
        VDAC0_Handler,                          // 41 EFR32 VDAC0 Interrupt
        CSEN_Handler,                           // 42 EFR32 CSEN Interrupt
        LESENSE_Handler,                        // 43 EFR32 LESENSE Interrupt
        CRYPTO1_Handler,                        // 44 EFR32 CRYPTO1 Interrupt
        TRNG0_Handler,                          // 45 EFR32 TRNG0 Interrupt
#endif
};

/***************************************************************************************************
END OF FILE
***************************************************************************************************/
