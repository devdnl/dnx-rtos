/*==============================================================================
File     stm32f4xx_vectors.c

Author   Daniel Zorychta

Brief    STM32F7xx family vectors.

         Copyright (C) 2020 Daniel Zorychta <daniel.zorychta@gmail.com>

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
WEAK_DEFAULT void SDMMC1_IRQHandler(void);
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
WEAK_DEFAULT void AES_CRYP_IRQHandler(void);
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
WEAK_DEFAULT void SAI2_IRQHandler(void);
WEAK_DEFAULT void QUADSPI_IRQHandler(void);
WEAK_DEFAULT void LPTIM1_IRQHandler(void);
WEAK_DEFAULT void CEC_IRQHandler(void);
WEAK_DEFAULT void I2C4_EV_IRQHandler(void);
WEAK_DEFAULT void I2C4_ER_IRQHandler(void);
WEAK_DEFAULT void SPDIF_RX_IRQHandler(void);
WEAK_DEFAULT void DSI_IRQHandler(void);
WEAK_DEFAULT void DFSDM1_FLT0_IRQHandler(void);
WEAK_DEFAULT void DFSDM1_FLT1_IRQHandler(void);
WEAK_DEFAULT void DFSDM1_FLT2_IRQHandler(void);
WEAK_DEFAULT void DFSDM1_FLT3_IRQHandler(void);
WEAK_DEFAULT void SDMMC2_IRQHandler(void);
WEAK_DEFAULT void CAN3_TX_IRQHandler(void);
WEAK_DEFAULT void CAN3_RX0_IRQHandler(void);
WEAK_DEFAULT void CAN3_RX1_IRQHandler(void);
WEAK_DEFAULT void CAN3_SCE_IRQHandler(void);
WEAK_DEFAULT void JPEG_IRQHandler(void);
WEAK_DEFAULT void MDIOS_IRQHandler(void);

/*==================================================================================================
Vector table
==================================================================================================*/
extern const char __main_stack_end;          /* imported main stack end (from linker script)      */

void Reset_Handler(void);                    /* import the address of Reset_Handler()             */

void (*const vectors[])(void) __attribute__ ((section(".vectors"))) =
{
        (void (*)(void))&__main_stack_end,      // Main stack end address
        Reset_Handler,                          // -15:Reset
        NMI_Handler,                            // -14:Non-maskable interrupt (RCC clock security system)
        HardFault_Handler,                      // -13:All class of fault
        MemManage_Handler,                      // -12:Memory management
        BusFault_Handler,                       // -11:Pre-fetch fault, memory access fault
        UsageFault_Handler,                     // -10:Undefined instruction or illegal state
        __Reserved_0x1C_IRQHandler,             // -9:Reserved 0x1C
        __Reserved_0x20_IRQHandler,             // -8:Reserved 0x20
        __Reserved_0x24_IRQHandler,             // -7:Reserved 0x24
        __Reserved_0x28_IRQHandler,             // -6:Reserved 0x28
        SVC_Handler,                            // -5:System service call via SWI instruction
        DebugMon_Handler,                       // -4:Debug monitor
        __Reserved_0x34_IRQHandler,             // -3:Reserved 0x34
        PendSV_Handler,                         // -2:Pendable request for system service
        SysTick_Handler,                        // -1:System tick timer
        WWDG_IRQHandler,                        // 0:Window WatchDog Interrupt
        PVD_IRQHandler,                         // 1:PVD through EXTI Line detection Interrupt
        TAMP_STAMP_IRQHandler,                  // 2:Tamper and TimeStamp interrupts through the EXTI line
        RTC_WKUP_IRQHandler,                    // 3:RTC Wakeup interrupt through the EXTI line
        FLASH_IRQHandler,                       // 4:FLASH global Interrupt
        RCC_IRQHandler,                         // 5:RCC global Interrupt
        EXTI0_IRQHandler,                       // 6:EXTI Line0 Interrupt
        EXTI1_IRQHandler,                       // 7:EXTI Line1 Interrupt
        EXTI2_IRQHandler,                       // 8:EXTI Line2 Interrupt
        EXTI3_IRQHandler,                       // 9:EXTI Line3 Interrupt
        EXTI4_IRQHandler,                       // 10:EXTI Line4 Interrupt
        DMA1_Stream0_IRQHandler,                // 11:DMA1 Stream 0 global Interrupt
        DMA1_Stream1_IRQHandler,                // 12:DMA1 Stream 1 global Interrupt
        DMA1_Stream2_IRQHandler,                // 13:DMA1 Stream 2 global Interrupt
        DMA1_Stream3_IRQHandler,                // 14:DMA1 Stream 3 global Interrupt
        DMA1_Stream4_IRQHandler,                // 15:DMA1 Stream 4 global Interrupt
        DMA1_Stream5_IRQHandler,                // 16:DMA1 Stream 5 global Interrupt
        DMA1_Stream6_IRQHandler,                // 17:DMA1 Stream 6 global Interrupt
        ADC_IRQHandler,                         // 18:ADC1, ADC2 and ADC3 global Interrupts
        CAN1_TX_IRQHandler,                     // 19:CAN1 TX Interrupt
        CAN1_RX0_IRQHandler,                    // 20:CAN1 RX0 Interrupt
        CAN1_RX1_IRQHandler,                    // 21:CAN1 RX1 Interrupt
        CAN1_SCE_IRQHandler,                    // 22:CAN1 SCE Interrupt
        EXTI9_5_IRQHandler,                     // 23:External Line[9:5] Interrupts
        TIM1_BRK_TIM9_IRQHandler,               // 24:TIM1 Break interrupt and TIM9 global interrupt
        TIM1_UP_TIM10_IRQHandler,               // 25:TIM1 Update Interrupt and TIM10 global interrupt
        TIM1_TRG_COM_TIM11_IRQHandler,          // 26:TIM1 Trigger and Commutation Interrupt and TIM11 global interrupt
        TIM1_CC_IRQHandler,                     // 27:TIM1 Capture Compare Interrupt
        TIM2_IRQHandler,                        // 28:TIM2 global Interrupt
        TIM3_IRQHandler,                        // 29:TIM3 global Interrupt
        TIM4_IRQHandler,                        // 30:TIM4 global Interrupt
        I2C1_EV_IRQHandler,                     // 31:I2C1 Event Interrupt
        I2C1_ER_IRQHandler,                     // 32:I2C1 Error Interrupt
        I2C2_EV_IRQHandler,                     // 33:I2C2 Event Interrupt
        I2C2_ER_IRQHandler,                     // 34:I2C2 Error Interrupt
        SPI1_IRQHandler,                        // 35:SPI1 global Interrupt
        SPI2_IRQHandler,                        // 36:SPI2 global Interrupt
        USART1_IRQHandler,                      // 37:USART1 global Interrupt
        USART2_IRQHandler,                      // 38:USART2 global Interrupt
        USART3_IRQHandler,                      // 39:USART3 global Interrupt
        EXTI15_10_IRQHandler,                   // 40:External Line[15:10] Interrupts
        RTC_Alarm_IRQHandler,                   // 41:RTC Alarm (A and B) through EXTI Line Interrupt
        OTG_FS_WKUP_IRQHandler,                 // 42:USB OTG FS Wakeup through EXTI line interrupt
        TIM8_BRK_TIM12_IRQHandler,              // 43:TIM8 Break Interrupt and TIM12 global interrupt
        TIM8_UP_TIM13_IRQHandler,               // 44:TIM8 Update Interrupt and TIM13 global interrupt
        TIM8_TRG_COM_TIM14_IRQHandler,          // 45:TIM8 Trigger and Commutation Interrupt and TIM14 global interrupt
        TIM8_CC_IRQHandler,                     // 46:TIM8 Capture Compare Interrupt
        DMA1_Stream7_IRQHandler,                // 47:DMA1 Stream7 Interrupt
        FMC_IRQHandler,                         // 48:FMC global Interrupt
        SDMMC1_IRQHandler,                      // 49:SDMMC1 global Interrupt
        TIM5_IRQHandler,                        // 50:TIM5 global Interrupt
        SPI3_IRQHandler,                        // 51:SPI3 global Interrupt
        UART4_IRQHandler,                       // 52:UART4 global Interrupt
        UART5_IRQHandler,                       // 53:UART5 global Interrupt
        TIM6_DAC_IRQHandler,                    // 54:TIM6 global and DAC1&2 underrun error  interrupts
        TIM7_IRQHandler,                        // 55:TIM7 global interrupt
        DMA2_Stream0_IRQHandler,                // 56:DMA2 Stream 0 global Interrupt
        DMA2_Stream1_IRQHandler,                // 57:DMA2 Stream 1 global Interrupt
        DMA2_Stream2_IRQHandler,                // 58:DMA2 Stream 2 global Interrupt
        DMA2_Stream3_IRQHandler,                // 59:DMA2 Stream 3 global Interrupt
        DMA2_Stream4_IRQHandler,                // 60:DMA2 Stream 4 global Interrupt
        ETH_IRQHandler,                         // 61:Ethernet global Interrupt
        ETH_WKUP_IRQHandler,                    // 62:Ethernet Wakeup through EXTI line Interrupt
        CAN2_TX_IRQHandler,                     // 63:CAN2 TX Interrupt
        CAN2_RX0_IRQHandler,                    // 64:CAN2 RX0 Interrupt
        CAN2_RX1_IRQHandler,                    // 65:CAN2 RX1 Interrupt
        CAN2_SCE_IRQHandler,                    // 66:CAN2 SCE Interrupt
        OTG_FS_IRQHandler,                      // 67:USB OTG FS global Interrupt
        DMA2_Stream5_IRQHandler,                // 68:DMA2 Stream 5 global interrupt
        DMA2_Stream6_IRQHandler,                // 69:DMA2 Stream 6 global interrupt
        DMA2_Stream7_IRQHandler,                // 70:DMA2 Stream 7 global interrupt
        USART6_IRQHandler,                      // 71:USART6 global interrupt
        I2C3_EV_IRQHandler,                     // 72:I2C3 event interrupt
        I2C3_ER_IRQHandler,                     // 73:I2C3 error interrupt
        OTG_HS_EP1_OUT_IRQHandler,              // 74:USB OTG HS End Point 1 Out global interrupt
        OTG_HS_EP1_IN_IRQHandler,               // 75:USB OTG HS End Point 1 In global interrupt
        OTG_HS_WKUP_IRQHandler,                 // 76:USB OTG HS Wakeup through EXTI interrupt
        OTG_HS_IRQHandler,                      // 77:USB OTG HS global interrupt
        DCMI_IRQHandler,                        // 78:DCMI global interrupt
        AES_CRYP_IRQHandler,                    // 79:AES/CRYP crypto global interrupt
        HASH_RNG_IRQHandler,                    // 80:Hash and Rng global interrupt
        FPU_IRQHandler,                         // 81:FPU global interrupt
        UART7_IRQHandler,                       // 82:UART7 global interrupt
        UART8_IRQHandler,                       // 83:UART8 global interrupt
        SPI4_IRQHandler,                        // 84:SPI4 global Interrupt
        SPI5_IRQHandler,                        // 85:SPI5 global Interrupt
        SPI6_IRQHandler,                        // 86:SPI6 global Interrupt
        SAI1_IRQHandler,                        // 87:SAI1 global Interrupt
        LTDC_IRQHandler,                        // 88:LTDC global Interrupt
        LTDC_ER_IRQHandler,                     // 89:LTDC Error global Interrupt
        DMA2D_IRQHandler,                       // 90:DMA2D global Interrupt
        SAI2_IRQHandler,                        // 91:SAI2 global Interrupt
        QUADSPI_IRQHandler,                     // 92:Quad SPI global interrupt
        LPTIM1_IRQHandler,                      // 93:LP TIM1 interrupt
        CEC_IRQHandler,                         // 94:HDMI-CEC global Interrupt
        I2C4_EV_IRQHandler,                     // 95:I2C4 Event Interrupt
        I2C4_ER_IRQHandler,                     // 96:I2C4 Error Interrupt
        SPDIF_RX_IRQHandler,                    // 97:SPDIF-RX global Interrupt
        DSI_IRQHandler,                         // 98:DSI global Interrupt
        DFSDM1_FLT0_IRQHandler,                 // 99:DFSDM1 Filter 0 global Interrupt
        DFSDM1_FLT1_IRQHandler,                 // 100:DFSDM1 Filter 1 global Interrupt
        DFSDM1_FLT2_IRQHandler,                 // 101:DFSDM1 Filter 2 global Interrupt
        DFSDM1_FLT3_IRQHandler,                 // 102:DFSDM1 Filter 3 global Interrupt
        SDMMC2_IRQHandler,                      // 103:SDMMC2 global Interrupt
        CAN3_TX_IRQHandler,                     // 104:CAN3 TX Interrupt
        CAN3_RX0_IRQHandler,                    // 105:CAN3 RX0 Interrupt
        CAN3_RX1_IRQHandler,                    // 106:CAN3 RX1 Interrupt
        CAN3_SCE_IRQHandler,                    // 107:CAN3 SCE Interrupt
        JPEG_IRQHandler,                        // 108:JPEG global Interrupt
        MDIOS_IRQHandler,                       // 109:MDIO Slave global Interrupt
};

/***************************************************************************************************
END OF FILE
***************************************************************************************************/
