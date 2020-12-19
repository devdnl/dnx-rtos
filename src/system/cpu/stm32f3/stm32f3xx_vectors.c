/*==============================================================================
File     stm32f3xx_vectors.c

Author   Daniel Zorychta

Brief    STM32F3xx family vectors.

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
WEAK_DEFAULT void EXTI2_TSC_IRQHandler(void);
WEAK_DEFAULT void EXTI3_IRQHandler(void);
WEAK_DEFAULT void EXTI4_IRQHandler(void);
WEAK_DEFAULT void DMA1_Channel7_IRQHandler(void);
WEAK_DEFAULT void DMA1_Channel1_IRQHandler(void);
WEAK_DEFAULT void DMA1_Channel2_IRQHandler(void);
WEAK_DEFAULT void DMA1_Channel3_IRQHandler(void);
WEAK_DEFAULT void DMA1_Channel4_IRQHandler(void);
WEAK_DEFAULT void DMA1_Channel5_IRQHandler(void);
WEAK_DEFAULT void DMA1_Channel6_IRQHandler(void);
WEAK_DEFAULT void ADC1_2_IRQHandler(void);
WEAK_DEFAULT void USB_HP_CAN_TX_IRQHandler(void);
WEAK_DEFAULT void USB_LP_CAN_RX0_IRQHandler(void);
WEAK_DEFAULT void CAN_RX1_IRQHandler(void);
WEAK_DEFAULT void CAN_SCE_IRQHandler(void);
WEAK_DEFAULT void EXTI9_5_IRQHandler(void);
WEAK_DEFAULT void TIM1_BRK_TIM15_IRQHandler(void);
WEAK_DEFAULT void TIM1_UP_TIM16_IRQHandler(void);
WEAK_DEFAULT void TIM14_TIM8_TRG_COM_IRQHandler(void);
WEAK_DEFAULT void ADC3_IRQHandler(void);
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
WEAK_DEFAULT void COMP1_2_3_IRQHandler(void);
WEAK_DEFAULT void TIM1_CC_TIM18_DAC2_IRQHandler(void);
WEAK_DEFAULT void TIM12_TIM8_BRK_IRQHandler(void);
WEAK_DEFAULT void TIM17_TIM1_TRG_COM_TIM17_IRQHandler(void);
WEAK_DEFAULT void TIM8_CC_IRQHandler(void);
WEAK_DEFAULT void TIM20_TRG_COM_IRQHandler(void);
WEAK_DEFAULT void TIM20_CC_IRQHandler(void);
WEAK_DEFAULT void TIM5_IRQHandler(void);
WEAK_DEFAULT void SPI3_IRQHandler(void);
WEAK_DEFAULT void UART4_IRQHandler(void);
WEAK_DEFAULT void UART5_IRQHandler(void);
WEAK_DEFAULT void TIM6_DAC1_IRQHandler(void);
WEAK_DEFAULT void TIM7_DAC2_IRQHandler(void);
WEAK_DEFAULT void USB_LP_IRQHandler(void);
WEAK_DEFAULT void DMA2_Channel5_IRQHandler(void);
WEAK_DEFAULT void DMA2_Channel1_IRQHandler(void);
WEAK_DEFAULT void DMA2_Channel2_IRQHandler(void);
WEAK_DEFAULT void DMA2_Channel3_IRQHandler(void);
WEAK_DEFAULT void DMA2_Channel4_IRQHandler(void);
WEAK_DEFAULT void __Reserved_49_IRQHandler(void);
WEAK_DEFAULT void __Reserved_82_IRQHandler(void);
WEAK_DEFAULT void __Reserved_83_IRQHandler(void);
WEAK_DEFAULT void TIM13_TIM8_UP_IRQHandler(void);
WEAK_DEFAULT void HRTIM1_Master_IRQHandler(void);
WEAK_DEFAULT void HRTIM1_TIMA_IRQHandler(void);
WEAK_DEFAULT void HRTIM1_TIMB_IRQHandler(void);
WEAK_DEFAULT void HRTIM1_TIMC_IRQHandler(void);
WEAK_DEFAULT void HRTIM1_TIMD_IRQHandler(void);
WEAK_DEFAULT void USB_HP_IRQHandler(void);
WEAK_DEFAULT void TIM20_BRK_IRQHandler(void);
WEAK_DEFAULT void TIM19_TIM20_UP_IRQHandler(void);
WEAK_DEFAULT void HRTIM1_TIME_I2C3_EV_IRQHandler(void);
WEAK_DEFAULT void HRTIM1_FLT_I2C3_ER_IRQHandler(void);
WEAK_DEFAULT void FPU_IRQHandler(void);
WEAK_DEFAULT void SPI4_IRQHandler(void);
WEAK_DEFAULT void USBWakeUp_RMP_IRQHandler(void);
WEAK_DEFAULT void USBWakeUp_CEC_IRQHandler(void);
WEAK_DEFAULT void COMP7_IRQHandler(void);
WEAK_DEFAULT void ADC4_SDADC1_IRQHandler(void);
WEAK_DEFAULT void SDADC2_IRQHandler(void);
WEAK_DEFAULT void SDADC3_IRQHandler(void);
WEAK_DEFAULT void COMP4_5_6_IRQHandler(void);
WEAK_DEFAULT void FMC_IRQHandler(void);

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
        WWDG_IRQHandler,                        // 0:Window WatchDog Interrupt
        PVD_IRQHandler,                         // 1:PVD through EXTI Line detection Interrupt
        TAMP_STAMP_IRQHandler,                  // 2:Tamper and TimeStamp interrupts through the EXTI line 19
        RTC_WKUP_IRQHandler,                    // 3:RTC Wakeup interrupt through the EXTI line 20
        FLASH_IRQHandler,                       // 4:FLASH global Interrupt
        RCC_IRQHandler,                         // 5:RCC global Interrupt
        EXTI0_IRQHandler,                       // 6:EXTI Line0 Interrupt
        EXTI1_IRQHandler,                       // 7:EXTI Line1 Interrupt
        EXTI2_TSC_IRQHandler,                   // 8:EXTI Line2 Interrupt and Touch Sense Controller Interrupt
        EXTI3_IRQHandler,                       // 9:EXTI Line3 Interrupt
        EXTI4_IRQHandler,                       // 10:EXTI Line4 Interrupt
        DMA1_Channel1_IRQHandler,               // 11:DMA1 Channel 1 Interrupt
        DMA1_Channel2_IRQHandler,               // 12:DMA1 Channel 2 Interrupt
        DMA1_Channel3_IRQHandler,               // 13:DMA1 Channel 3 Interrupt
        DMA1_Channel4_IRQHandler,               // 14:DMA1 Channel 4 Interrupt
        DMA1_Channel5_IRQHandler,               // 15:DMA1 Channel 5 Interrupt
        DMA1_Channel6_IRQHandler,               // 16:DMA1 Channel 6 Interrupt
        DMA1_Channel7_IRQHandler,               // 17:DMA1 Channel 7 Interrupt
        ADC1_2_IRQHandler,                      // 18:ADC1 & ADC2 Interrupts
        USB_HP_CAN_TX_IRQHandler,               // 19:USB Device High Priority or CAN TX Interrupts
        USB_LP_CAN_RX0_IRQHandler,              // 20:CAN RX0 Interrupt
        CAN_RX1_IRQHandler,                     // 21:CAN RX1 Interrupt
        CAN_SCE_IRQHandler,                     // 22:CAN SCE Interrupt
        EXTI9_5_IRQHandler,                     // 23:External Line[9:5] Interrupts
        TIM1_BRK_TIM15_IRQHandler,              // 24:TIM1 Break and TIM15 Interrupts
        TIM1_UP_TIM16_IRQHandler,               // 25:TIM1 Update and TIM16 Interrupts
        TIM17_TIM1_TRG_COM_TIM17_IRQHandler,    // 26:TIM1 Trigger and Commutation and TIM17 Interrupt
        TIM1_CC_TIM18_DAC2_IRQHandler,          // 27:TIM1 Capture Compare Interrupt
        TIM2_IRQHandler,                        // 28:TIM2 global Interrupt
        TIM3_IRQHandler,                        // 29:TIM3 global Interrupt
        TIM4_IRQHandler,                        // 30:TIM4 global Interrupt
        I2C1_EV_IRQHandler,                     // 31:I2C1 Event Interrupt & EXTI Line23 Interrupt (I2C1 wakeup)
        I2C1_ER_IRQHandler,                     // 32:I2C1 Error Interrupt
        I2C2_EV_IRQHandler,                     // 33:I2C2 Event Interrupt & EXTI Line24 Interrupt (I2C2 wakeup)
        I2C2_ER_IRQHandler,                     // 34:I2C2 Error Interrupt
        SPI1_IRQHandler,                        // 35:SPI1 global Interrupt
        SPI2_IRQHandler,                        // 36:SPI2 global Interrupt
        USART1_IRQHandler,                      // 37:USART1 global Interrupt & EXTI Line25 Interrupt (USART1 wakeup)
        USART2_IRQHandler,                      // 38:USART2 global Interrupt & EXTI Line26 Interrupt (USART2 wakeup)
        USART3_IRQHandler,                      // 39:USART3 global Interrupt & EXTI Line28 Interrupt (USART3 wakeup)
        EXTI15_10_IRQHandler,                   // 40:External Line[15:10] Interrupts
        RTC_Alarm_IRQHandler,                   // 41:RTC Alarm (A and B) through EXTI Line 17 Interrupt
        USBWakeUp_CEC_IRQHandler,               // 42:CEC Interrupt & EXTI Line27 Interrupt (CEC wakeup)
        TIM12_TIM8_BRK_IRQHandler,              // 43:TIM8 Break Interrupt
        TIM13_TIM8_UP_IRQHandler,               // 44:TIM8 Update Interrupt
        TIM14_TIM8_TRG_COM_IRQHandler,          // 45:TIM8 Trigger and Commutation Interrupt
        TIM8_CC_IRQHandler,                     // 46:TIM8 Capture Compare Interrupt
        ADC3_IRQHandler,                        // 47:ADC3 global Interrupt
        FMC_IRQHandler,                         // 48:FMC global Interrupt
        __Reserved_49_IRQHandler,               // 49:Reserved
        TIM5_IRQHandler,                        // 50:TIM5 global Interrupt
        SPI3_IRQHandler,                        // 51:SPI3 global Interrupt
        UART4_IRQHandler,                       // 52:UART4 global Interrupt & EXTI Line34 Interrupt (UART4 wakeup)
        UART5_IRQHandler,                       // 53:UART5 global Interrupt & EXTI Line35 Interrupt (UART5 wakeup)
        TIM6_DAC1_IRQHandler,                   // 54:TIM6 global and DAC1 underrun error Interrupts
        TIM7_DAC2_IRQHandler,                   // 55:TIM7 global and DAC2 channel1 underrun error Interrupt
        DMA2_Channel1_IRQHandler,               // 56:DMA2 Channel 1 global Interrupt
        DMA2_Channel2_IRQHandler,               // 57:DMA2 Channel 2 global Interrupt
        DMA2_Channel3_IRQHandler,               // 58:DMA2 Channel 3 global Interrupt
        DMA2_Channel4_IRQHandler,               // 59:DMA2 Channel 4 global Interrupt
        DMA2_Channel5_IRQHandler,               // 60:DMA2 Channel 5 global Interrupt
        ADC4_SDADC1_IRQHandler,                 // 61:ADC Sigma Delta 1 global Interrupt
        SDADC2_IRQHandler,                      // 62:ADC Sigma Delta 2 global Interrupt
        SDADC3_IRQHandler,                      // 63:ADC Sigma Delta 1 global Interrupt
        COMP1_2_3_IRQHandler,                   // 64:COMP1, COMP2 and COMP3 global Interrupt via EXTI Line21, 22 and 29
        COMP4_5_6_IRQHandler,                   // 65:COMP4, COMP5 and COMP6 global Interrupt via EXTI Line30, 31 and 32
        COMP7_IRQHandler,                       // 66:COMP7 global Interrupt via EXTI Line33
        HRTIM1_Master_IRQHandler,               // 67:HRTIM Master Timer global Interrupts
        HRTIM1_TIMA_IRQHandler,                 // 68:HRTIM Timer A global Interrupt
        HRTIM1_TIMB_IRQHandler,                 // 69:HRTIM Timer B global Interrupt
        HRTIM1_TIMC_IRQHandler,                 // 70:HRTIM Timer C global Interrupt
        HRTIM1_TIMD_IRQHandler,                 // 71:HRTIM Timer D global Interrupt
        HRTIM1_TIME_I2C3_EV_IRQHandler,         // 72:I2C3 Event Interrupt & EXTI Line27 Interrupt (I2C3 wakeup)
        HRTIM1_FLT_I2C3_ER_IRQHandler,          // 73:I2C3 Error Interrupt
        USB_HP_IRQHandler,                      // 74:USB High Priority global Interrupt
        USB_LP_IRQHandler,                      // 75:USB Low Priority global Interrupt
        USBWakeUp_RMP_IRQHandler,               // 76:USB Wakeup Interrupt remap
        TIM20_BRK_IRQHandler,                   // 77:TIM20 Break Interrupt
        TIM19_TIM20_UP_IRQHandler,              // 78:TIM20 Update Interrupt
        TIM20_TRG_COM_IRQHandler,               // 79:TIM20 Trigger and Commutation Interrupt
        TIM20_CC_IRQHandler,                    // 80:TIM20 Capture Compare Interrupt
        FPU_IRQHandler,                         // 81:Floating point Interrupt
        __Reserved_82_IRQHandler,               // 82:Reserved
        __Reserved_83_IRQHandler,               // 83:Reserved
        SPI4_IRQHandler,                        // 84:SPI4 global Interrupt
};

/***************************************************************************************************
END OF FILE
***************************************************************************************************/
