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
WEAK_DEFAULT void FMC_FSMC_IRQHandler(void);
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
WEAK_DEFAULT void __Reserved_61_IRQHandler(void);
WEAK_DEFAULT void __Reserved_62_IRQHandler(void);
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
WEAK_DEFAULT void CAN3_TX_OTG_HS_EP1_OUT_IRQHandler(void);
WEAK_DEFAULT void CAN3_RX0_OTG_HS_EP1_IN_IRQHandler(void);
WEAK_DEFAULT void CAN3_RX1_OTG_HS_WKUP_IRQHandler(void);
WEAK_DEFAULT void CAN3_SCE_OTG_HS_IRQHandler(void);
WEAK_DEFAULT void DCMI_IRQHandler(void);
WEAK_DEFAULT void AES_CRYP_IRQHandler(void);
WEAK_DEFAULT void HASH_RNG_IRQHandler(void);
WEAK_DEFAULT void FPU_IRQHandler(void);
WEAK_DEFAULT void UART7_IRQHandler(void);
WEAK_DEFAULT void UART8_IRQHandler(void);
WEAK_DEFAULT void SPI4_IRQHandler(void);
WEAK_DEFAULT void SPI5_IRQHandler(void);
WEAK_DEFAULT void SPI6_IRQHandler(void);
WEAK_DEFAULT void SAI1_IRQHandler(void);
WEAK_DEFAULT void UART9_LTDC_IRQHandler(void);
WEAK_DEFAULT void UART10_LTDC_ER_IRQHandler(void);
WEAK_DEFAULT void DMA2D_IRQHandler(void);
WEAK_DEFAULT void SAI2_QUADSPI_IRQHandler(void);
WEAK_DEFAULT void QUADSPI_DSI_IRQHandler(void);
WEAK_DEFAULT void CEC_IRQHandler(void);
WEAK_DEFAULT void SPDIF_RX_IRQn(void);
WEAK_DEFAULT void FMPI2C1_EV_IRQn(void);
WEAK_DEFAULT void FMPI2C1_ER_IRQn(void);
WEAK_DEFAULT void LPTIM1_IRQn(void);
WEAK_DEFAULT void DFSDM2_FLT0_IRQn(void);
WEAK_DEFAULT void DFSDM2_FLT1_IRQn(void);
WEAK_DEFAULT void DFSDM2_FLT2_IRQn(void);
WEAK_DEFAULT void DFSDM2_FLT3_IRQn(void);

#if defined(STM32F407xx) || defined(STM32F417xx) || defined(STM32F427xx)\
 || defined(STM32F429xx) || defined(STM32F437xx) || defined(STM32F439xx)\
 || defined(STM32F469xx) || defined(STM32F479xx)
WEAK_DEFAULT void ETH_IRQHandler(void);
WEAK_DEFAULT void ETH_WKUP_IRQHandler(void);
#endif

#if defined(STM32F412xx) || defined(STM32F413xx) || defined(STM32F423xx)
WEAK_DEFAULT void DFSDM1_FLT0_IRQHandler(void);
WEAK_DEFAULT void DFSDM1_FLT1_IRQHandler(void);
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
        __Reserved_0x1C_IRQHandler,             // Reserved 0x1C
        __Reserved_0x20_IRQHandler,             // Reserved 0x20
        __Reserved_0x24_IRQHandler,             // Reserved 0x24
        __Reserved_0x28_IRQHandler,             // Reserved 0x28
        SVC_Handler,                            // System service call via SWI instruction
        DebugMon_Handler,                       // Debug monitor
        __Reserved_0x34_IRQHandler,             // Reserved 0x34
        PendSV_Handler,                         // Pendable request for system service
        SysTick_Handler,                        // System tick timer
        WWDG_IRQHandler,                        // 0:Window WatchDog
        PVD_IRQHandler,                         // 1:PVD through EXTI Line detection
        TAMP_STAMP_IRQHandler,                  // 2:Tamper and TimeStamps through the EXTI line
        RTC_WKUP_IRQHandler,                    // 3:RTC Wakeup through the EXTI line
        FLASH_IRQHandler,                       // 4:FLASH
        RCC_IRQHandler,                         // 5:RCC
        EXTI0_IRQHandler,                       // 6:EXTI Line0
        EXTI1_IRQHandler,                       // 7:EXTI Line1
        EXTI2_IRQHandler,                       // 8:EXTI Line2
        EXTI3_IRQHandler,                       // 9:EXTI Line3
        EXTI4_IRQHandler,                       // 10:EXTI Line4
        DMA1_Stream0_IRQHandler,                // 11:DMA1 Stream 0
        DMA1_Stream1_IRQHandler,                // 12:DMA1 Stream 1
        DMA1_Stream2_IRQHandler,                // 13:DMA1 Stream 2
        DMA1_Stream3_IRQHandler,                // 14:DMA1 Stream 3
        DMA1_Stream4_IRQHandler,                // 15:DMA1 Stream 4
        DMA1_Stream5_IRQHandler,                // 16:DMA1 Stream 5
        DMA1_Stream6_IRQHandler,                // 17:DMA1 Stream 6
        ADC_IRQHandler,                         // 18:ADC1, ADC2 and ADC3s
        CAN1_TX_IRQHandler,                     // 19:CAN1 TX
        CAN1_RX0_IRQHandler,                    // 20:CAN1 RX0
        CAN1_RX1_IRQHandler,                    // 21:CAN1 RX1
        CAN1_SCE_IRQHandler,                    // 22:CAN1 SCE
        EXTI9_5_IRQHandler,                     // 23:External Line[9:5]s
        TIM1_BRK_TIM9_IRQHandler,               // 24:TIM1 Break and TIM9
        TIM1_UP_TIM10_IRQHandler,               // 25:TIM1 Update and TIM10
        TIM1_TRG_COM_TIM11_IRQHandler,          // 26:TIM1 Trigger and Commutation and TIM11
        TIM1_CC_IRQHandler,                     // 27:TIM1 Capture Compare
        TIM2_IRQHandler,                        // 28:TIM2
        TIM3_IRQHandler,                        // 29:TIM3
        TIM4_IRQHandler,                        // 30:TIM4
        I2C1_EV_IRQHandler,                     // 31:I2C1 Event
        I2C1_ER_IRQHandler,                     // 32:I2C1 Error
        I2C2_EV_IRQHandler,                     // 33:I2C2 Event
        I2C2_ER_IRQHandler,                     // 34:I2C2 Error
        SPI1_IRQHandler,                        // 35:SPI1
        SPI2_IRQHandler,                        // 36:SPI2
        USART1_IRQHandler,                      // 37:USART1
        USART2_IRQHandler,                      // 38:USART2
        USART3_IRQHandler,                      // 39:USART3
        EXTI15_10_IRQHandler,                   // 40:External Line[15:10]s
        RTC_Alarm_IRQHandler,                   // 41:RTC Alarm (A and B) through EXTI Line
        OTG_FS_WKUP_IRQHandler,                 // 42:USB OTG FS Wakeup through EXTI line
        TIM8_BRK_TIM12_IRQHandler,              // 43:TIM8 Break and TIM12
        TIM8_UP_TIM13_IRQHandler,               // 44:TIM8 Update and TIM13
        TIM8_TRG_COM_TIM14_IRQHandler,          // 45:TIM8 Trigger and Commutation and TIM14
        TIM8_CC_IRQHandler,                     // 46:TIM8 Capture Compare
        DMA1_Stream7_IRQHandler,                // 47:DMA1 Stream7
        FMC_FSMC_IRQHandler,                    // 48:FMC, FSMC
        SDIO_IRQHandler,                        // 49:SDIO
        TIM5_IRQHandler,                        // 50:TIM5
        SPI3_IRQHandler,                        // 51:SPI3
        UART4_IRQHandler,                       // 52:UART4
        UART5_IRQHandler,                       // 53:UART5
        TIM6_DAC_IRQHandler,                    // 54:TIM6 and DAC1&2 underrun errors
        TIM7_IRQHandler,                        // 55:TIM7
        DMA2_Stream0_IRQHandler,                // 56:DMA2 Stream 0
        DMA2_Stream1_IRQHandler,                // 57:DMA2 Stream 1
        DMA2_Stream2_IRQHandler,                // 58:DMA2 Stream 2
        DMA2_Stream3_IRQHandler,                // 59:DMA2 Stream 3
        DMA2_Stream4_IRQHandler,                // 60:DMA2 Stream 4
#if defined(STM32F407xx) || defined(STM32F417xx) || defined(STM32F427xx)\
 || defined(STM32F429xx) || defined(STM32F437xx) || defined(STM32F439xx)\
 || defined(STM32F469xx) || defined(STM32F479xx)
        ETH_IRQHandler,                         // 61:Ethernet
        ETH_WKUP_IRQHandler,                    // 62:Ethernet Wakeup through EXTI line
#elif defined(STM32F412xx) || defined(STM32F413xx) || defined(STM32F423xx)
        DFSDM1_FLT0_IRQHandler,                 // 61:DFSM_FLT0
        DFSDM1_FLT1_IRQHandler,                 // 62:DFSM_FLT
#else
        __Reserved_61_IRQHandler,               // 61:Reserved
        __Reserved_62_IRQHandler,               // 62:Reserved
#endif
        CAN2_TX_IRQHandler,                     // 63:CAN2 TX
        CAN2_RX0_IRQHandler,                    // 64:CAN2 RX0
        CAN2_RX1_IRQHandler,                    // 65:CAN2 RX1
        CAN2_SCE_IRQHandler,                    // 66:CAN2 SCE
        OTG_FS_IRQHandler,                      // 67:USB OTG FS
        DMA2_Stream5_IRQHandler,                // 68:DMA2 Stream 5
        DMA2_Stream6_IRQHandler,                // 69:DMA2 Stream 6
        DMA2_Stream7_IRQHandler,                // 70:DMA2 Stream 7
        USART6_IRQHandler,                      // 71:USART6
        I2C3_EV_IRQHandler,                     // 72:I2C3 event
        I2C3_ER_IRQHandler,                     // 73:I2C3 error
        CAN3_TX_OTG_HS_EP1_OUT_IRQHandler,      // 74:USB OTG HS End Point 1 Out
        CAN3_RX0_OTG_HS_EP1_IN_IRQHandler,      // 75:USB OTG HS End Point 1 In
        CAN3_RX1_OTG_HS_WKUP_IRQHandler,        // 76:USB OTG HS Wakeup through EXTI
        CAN3_SCE_OTG_HS_IRQHandler,             // 77:USB OTG HS
        DCMI_IRQHandler,                        // 78:DCMI
        AES_CRYP_IRQHandler,                    // 79:AES, CRYP crypto
        HASH_RNG_IRQHandler,                    // 80:Hash and Rng
        FPU_IRQHandler,                         // 81:FPU
        UART7_IRQHandler,                       // 82:UART7 global interrupt
        UART8_IRQHandler,                       // 83:UART8 global interrupt
        SPI4_IRQHandler,                        // 84:SPI4 global Interrupt
        SPI5_IRQHandler,                        // 85:SPI5 global Interrupt
        SPI6_IRQHandler,                        // 86:SPI6 global Interrupt
        SAI1_IRQHandler,                        // 87:SAI1 global Interrupt
        UART9_LTDC_IRQHandler,                  // 88:UART9, LTDC global Interrupt
        UART10_LTDC_ER_IRQHandler,              // 89:UART10, LTDC Error global Interrupt
        DMA2D_IRQHandler,                       // 90:DMA2D global Interrupt
        SAI2_QUADSPI_IRQHandler,                // 91:SAI2, QUADSPI global Interrupt
        QUADSPI_DSI_IRQHandler,                 // 92:QUADSPI, DSI global Interrupt
        CEC_IRQHandler,                         // 93:CEC
        SPDIF_RX_IRQn,                          // 94:SPDIF_RX_IRQn
        FMPI2C1_EV_IRQn,                        // 95:FMPI2C1_EV_IRQn
        FMPI2C1_ER_IRQn,                        // 96:FMPI2C1_ER_IRQn
        LPTIM1_IRQn,                            // 97:LPTIM1_IRQn
        DFSDM2_FLT0_IRQn,                       // 98:DFSDM2_FLT0_IRQn
        DFSDM2_FLT1_IRQn,                       // 99:DFSDM2_FLT1_IRQn
        DFSDM2_FLT2_IRQn,                       // 100:DFSDM2_FLT2_IRQn
        DFSDM2_FLT3_IRQn,                       // 101:DFSDM2_FLT3_IRQn
};

/***************************************************************************************************
END OF FILE
***************************************************************************************************/
