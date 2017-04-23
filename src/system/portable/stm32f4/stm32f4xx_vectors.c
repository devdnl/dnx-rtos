/*==============================================================================
File     stm32f4xx_vectors.c

Author   Daniel Zorychta

Brief    STM32F4xx family vectors.

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
WEAK_DEFAULT void __Reserved_0x1C_IRQHandler(void);
WEAK_DEFAULT void __Reserved_0x20_IRQHandler(void);
WEAK_DEFAULT void __Reserved_0x24_IRQHandler(void);
WEAK_DEFAULT void __Reserved_0x28_IRQHandler(void);
WEAK_DEFAULT void SVC_Handler(void);
WEAK_DEFAULT void DebugMon_Handler(void);
WEAK_DEFAULT void __Reserved_0x34_IRQHandler(void);
WEAK_DEFAULT void PendSV_Handler(void);
WEAK_DEFAULT void SysTick_Handler(void);

/* STM32 specific Interrupt Numbers *******************************************/
WEAK_DEFAULT void WWDG_IRQHandler(void);
WEAK_DEFAULT void PVD_IRQHandler(void);
WEAK_DEFAULT void TAMP_STAMP_IRQHandler(void);
WEAK_DEFAULT void RTC_WKUP_IRQHandler(void);
WEAK_DEFAULT void FLASH_IRQHandler(void);
WEAK_DEFAULT void RCC_IRQHandler(void);
WEAK_DEFAULT void EXTI0_IRQHandler(void);
WEAK_DEFAULT void EXTI1_IRQHandler(void);
WEAK_DEFAULT void EXTI2_IRQHandler(void);
WEAK_DEFAULT void EXTI3_IRQHandler(void);
WEAK_DEFAULT void EXTI4_IRQHandler(void);
WEAK_DEFAULT void DMA1_Stream0_IRQHandler(void);
WEAK_DEFAULT void DMA1_Stream1_IRQHandler(void);
WEAK_DEFAULT void DMA1_Stream2_IRQHandler(void);
WEAK_DEFAULT void DMA1_Stream3_IRQHandler(void);
WEAK_DEFAULT void DMA1_Stream4_IRQHandler(void);
WEAK_DEFAULT void DMA1_Stream5_IRQHandler(void);
WEAK_DEFAULT void DMA1_Stream6_IRQHandler(void);
WEAK_DEFAULT void ADC_IRQHandler(void);
WEAK_DEFAULT void CAN1_TX_IRQHandler(void);
WEAK_DEFAULT void CAN1_RX0_IRQHandler(void);
WEAK_DEFAULT void CAN1_RX1_IRQHandler(void);
WEAK_DEFAULT void CAN1_SCE_IRQHandler(void);
WEAK_DEFAULT void EXTI9_5_IRQHandler(void);
WEAK_DEFAULT void TIM1_BRK_TIM9_IRQHandler(void);
WEAK_DEFAULT void TIM1_UP_TIM10_IRQHandler(void);
WEAK_DEFAULT void TIM1_TRG_COM_TIM11_IRQHandler(void);
WEAK_DEFAULT void TIM1_CC_IRQHandler(void);
WEAK_DEFAULT void TIM2_IRQHandler(void);
WEAK_DEFAULT void TIM3_IRQHandler(void);
WEAK_DEFAULT void TIM4_IRQHandler(void);
WEAK_DEFAULT void I2C1_EV_IRQHandler(void);
WEAK_DEFAULT void I2C1_ER_IRQHandler(void);
WEAK_DEFAULT void I2C2_EV_IRQHandler(void);
WEAK_DEFAULT void I2C2_ER_IRQHandler(void);
WEAK_DEFAULT void SPI1_IRQHandler(void);
WEAK_DEFAULT void SPI2_IRQHandler(void);
WEAK_DEFAULT void USART1_IRQHandler(void);
WEAK_DEFAULT void USART2_IRQHandler(void);
WEAK_DEFAULT void USART3_IRQHandler(void);
WEAK_DEFAULT void EXTI15_10_IRQHandler(void);
WEAK_DEFAULT void RTC_Alarm_IRQHandler(void);
WEAK_DEFAULT void OTG_FS_WKUP_IRQHandler(void);
WEAK_DEFAULT void TIM8_BRK_TIM12_IRQHandler(void);
WEAK_DEFAULT void TIM8_UP_TIM13_IRQHandler(void);
WEAK_DEFAULT void TIM8_TRG_COM_TIM14_IRQHandler(void);
WEAK_DEFAULT void TIM8_CC_IRQHandler(void);
WEAK_DEFAULT void DMA1_Stream7_IRQHandler(void);
WEAK_DEFAULT void FMC_IRQHandler(void);
WEAK_DEFAULT void SDIO_IRQHandler(void);
WEAK_DEFAULT void TIM5_IRQHandler(void);
WEAK_DEFAULT void SPI3_IRQHandler(void);
WEAK_DEFAULT void UART4_IRQHandler(void);
WEAK_DEFAULT void UART5_IRQHandler(void);
WEAK_DEFAULT void TIM6_DAC_IRQHandler(void);
WEAK_DEFAULT void TIM7_IRQHandler(void);
WEAK_DEFAULT void DMA2_Stream0_IRQHandler(void);
WEAK_DEFAULT void DMA2_Stream1_IRQHandler(void);
WEAK_DEFAULT void DMA2_Stream2_IRQHandler(void);
WEAK_DEFAULT void DMA2_Stream3_IRQHandler(void);
WEAK_DEFAULT void DMA2_Stream4_IRQHandler(void);
WEAK_DEFAULT void ETH_IRQHandler(void);
WEAK_DEFAULT void ETH_WKUP_IRQHandler(void);
WEAK_DEFAULT void CAN2_TX_IRQHandler(void);
WEAK_DEFAULT void CAN2_RX0_IRQHandler(void);
WEAK_DEFAULT void CAN2_RX1_IRQHandler(void);
WEAK_DEFAULT void CAN2_SCE_IRQHandler(void);
WEAK_DEFAULT void OTG_FS_IRQHandler(void);
WEAK_DEFAULT void DMA2_Stream5_IRQHandler(void);
WEAK_DEFAULT void DMA2_Stream6_IRQHandler(void);
WEAK_DEFAULT void DMA2_Stream7_IRQHandler(void);
WEAK_DEFAULT void USART6_IRQHandler(void);
WEAK_DEFAULT void I2C3_EV_IRQHandler(void);
WEAK_DEFAULT void I2C3_ER_IRQHandler(void);
WEAK_DEFAULT void OTG_HS_EP1_OUT_IRQHandler(void);
WEAK_DEFAULT void OTG_HS_EP1_IN_IRQHandler(void);
WEAK_DEFAULT void OTG_HS_WKUP_IRQHandler(void);
WEAK_DEFAULT void OTG_HS_IRQHandler(void);
WEAK_DEFAULT void DCMI_IRQHandler(void);
WEAK_DEFAULT void CRYP_IRQHandler(void);
WEAK_DEFAULT void HASH_RNG_IRQHandler(void);
WEAK_DEFAULT void FPU_IRQHandler(void);
WEAK_DEFAULT void UART7_IRQHandler(void);
WEAK_DEFAULT void UART8_IRQHandler(void);
WEAK_DEFAULT void SPI4_IRQHandler(void);
WEAK_DEFAULT void SPI5_IRQHandler(void);
WEAK_DEFAULT void SPI6_IRQHandler(void);
WEAK_DEFAULT void SAI1_IRQHandler(void);
WEAK_DEFAULT void LTDC_IRQHandler(void);
WEAK_DEFAULT void LTDC_ER_IRQHandler(void);
WEAK_DEFAULT void DMA2D_IRQHandler(void);
WEAK_DEFAULT void QUADSPI_IRQHandler(void);
WEAK_DEFAULT void DSI_IRQHandler(void);

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
        __Reserved_0x1C_IRQHandler,             // Reserved 0x1C
        __Reserved_0x20_IRQHandler,             // Reserved 0x20
        __Reserved_0x24_IRQHandler,             // Reserved 0x24
        __Reserved_0x28_IRQHandler,             // Reserved 0x28
        SVC_Handler,                            // System service call via SWI instruction
        DebugMon_Handler,                       // Debug monitor
        __Reserved_0x34_IRQHandler,             // Reserved 0x34
        PendSV_Handler,                         // Pendable request for system service
        SysTick_Handler,                        // System tick timer
        WWDG_IRQHandler,                        // Window WatchDog
        PVD_IRQHandler,                         // PVD through EXTI Line detection
        TAMP_STAMP_IRQHandler,                  // Tamper and TimeStamps through the EXTI line
        RTC_WKUP_IRQHandler,                    // RTC Wakeup through the EXTI line
        FLASH_IRQHandler,                       // FLASH
        RCC_IRQHandler,                         // RCC
        EXTI0_IRQHandler,                       // EXTI Line0
        EXTI1_IRQHandler,                       // EXTI Line1
        EXTI2_IRQHandler,                       // EXTI Line2
        EXTI3_IRQHandler,                       // EXTI Line3
        EXTI4_IRQHandler,                       // EXTI Line4
        DMA1_Stream0_IRQHandler,                // DMA1 Stream 0
        DMA1_Stream1_IRQHandler,                // DMA1 Stream 1
        DMA1_Stream2_IRQHandler,                // DMA1 Stream 2
        DMA1_Stream3_IRQHandler,                // DMA1 Stream 3
        DMA1_Stream4_IRQHandler,                // DMA1 Stream 4
        DMA1_Stream5_IRQHandler,                // DMA1 Stream 5
        DMA1_Stream6_IRQHandler,                // DMA1 Stream 6
        ADC_IRQHandler,                         // ADC1, ADC2 and ADC3s
        CAN1_TX_IRQHandler,                     // CAN1 TX
        CAN1_RX0_IRQHandler,                    // CAN1 RX0
        CAN1_RX1_IRQHandler,                    // CAN1 RX1
        CAN1_SCE_IRQHandler,                    // CAN1 SCE
        EXTI9_5_IRQHandler,                     // External Line[9:5]s
        TIM1_BRK_TIM9_IRQHandler,               // TIM1 Break and TIM9
        TIM1_UP_TIM10_IRQHandler,               // TIM1 Update and TIM10
        TIM1_TRG_COM_TIM11_IRQHandler,          // TIM1 Trigger and Commutation and TIM11
        TIM1_CC_IRQHandler,                     // TIM1 Capture Compare
        TIM2_IRQHandler,                        // TIM2
        TIM3_IRQHandler,                        // TIM3
        TIM4_IRQHandler,                        // TIM4
        I2C1_EV_IRQHandler,                     // I2C1 Event
        I2C1_ER_IRQHandler,                     // I2C1 Error
        I2C2_EV_IRQHandler,                     // I2C2 Event
        I2C2_ER_IRQHandler,                     // I2C2 Error
        SPI1_IRQHandler,                        // SPI1
        SPI2_IRQHandler,                        // SPI2
        USART1_IRQHandler,                      // USART1
        USART2_IRQHandler,                      // USART2
        USART3_IRQHandler,                      // USART3
        EXTI15_10_IRQHandler,                   // External Line[15:10]s
        RTC_Alarm_IRQHandler,                   // RTC Alarm (A and B) through EXTI Line
        OTG_FS_WKUP_IRQHandler,                 // USB OTG FS Wakeup through EXTI line
        TIM8_BRK_TIM12_IRQHandler,              // TIM8 Break and TIM12
        TIM8_UP_TIM13_IRQHandler,               // TIM8 Update and TIM13
        TIM8_TRG_COM_TIM14_IRQHandler,          // TIM8 Trigger and Commutation and TIM14
        TIM8_CC_IRQHandler,                     // TIM8 Capture Compare
        DMA1_Stream7_IRQHandler,                // DMA1 Stream7
        FMC_IRQHandler,                        // FSMC
        SDIO_IRQHandler,                        // SDIO
        TIM5_IRQHandler,                        // TIM5
        SPI3_IRQHandler,                        // SPI3
        UART4_IRQHandler,                       // UART4
        UART5_IRQHandler,                       // UART5
        TIM6_DAC_IRQHandler,                    // TIM6 and DAC1&2 underrun errors
        TIM7_IRQHandler,                        // TIM7
        DMA2_Stream0_IRQHandler,                // DMA2 Stream 0
        DMA2_Stream1_IRQHandler,                // DMA2 Stream 1
        DMA2_Stream2_IRQHandler,                // DMA2 Stream 2
        DMA2_Stream3_IRQHandler,                // DMA2 Stream 3
        DMA2_Stream4_IRQHandler,                // DMA2 Stream 4
        ETH_IRQHandler,                         // Ethernet
        ETH_WKUP_IRQHandler,                    // Ethernet Wakeup through EXTI line
        CAN2_TX_IRQHandler,                     // CAN2 TX
        CAN2_RX0_IRQHandler,                    // CAN2 RX0
        CAN2_RX1_IRQHandler,                    // CAN2 RX1
        CAN2_SCE_IRQHandler,                    // CAN2 SCE
        OTG_FS_IRQHandler,                      // USB OTG FS
        DMA2_Stream5_IRQHandler,                // DMA2 Stream 5
        DMA2_Stream6_IRQHandler,                // DMA2 Stream 6
        DMA2_Stream7_IRQHandler,                // DMA2 Stream 7
        USART6_IRQHandler,                      // USART6
        I2C3_EV_IRQHandler,                     // I2C3 event
        I2C3_ER_IRQHandler,                     // I2C3 error
        OTG_HS_EP1_OUT_IRQHandler,              // USB OTG HS End Point 1 Out
        OTG_HS_EP1_IN_IRQHandler,               // USB OTG HS End Point 1 In
        OTG_HS_WKUP_IRQHandler,                 // USB OTG HS Wakeup through EXTI
        OTG_HS_IRQHandler,                      // USB OTG HS
        DCMI_IRQHandler,                        // DCMI
        CRYP_IRQHandler,                        // CRYP crypto
        HASH_RNG_IRQHandler,                    // Hash and Rng
        FPU_IRQHandler,                         // FPU
        UART7_IRQHandler,                       // UART7 global interrupt
        UART8_IRQHandler,                       // UART8 global interrupt
        SPI4_IRQHandler,                        // SPI4 global Interrupt
        SPI5_IRQHandler,                        // SPI5 global Interrupt
        SPI6_IRQHandler,                        // SPI6 global Interrupt
        SAI1_IRQHandler,                        // SAI1 global Interrupt
        LTDC_IRQHandler,                        // LTDC global Interrupt
        LTDC_ER_IRQHandler,                     // LTDC Error global Interrupt
        DMA2D_IRQHandler,                       // DMA2D global Interrupt
        QUADSPI_IRQHandler,                     // QUADSPI global Interrupt
        DSI_IRQHandler                          // DSI global Interrupt
};

/***************************************************************************************************
END OF FILE
***************************************************************************************************/
