/*==============================================================================
File     stm32f10x_vect.c

Author   Daniel Zorychta

Brief    STM32F1xx family vectors.

         Copyright (C) 2016 Daniel Zorychta <daniel.zorychta@gmail.com>

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
WEAK_DEFAULT void TAMPER_IRQHandler(void);
WEAK_DEFAULT void RTC_IRQHandler(void);
WEAK_DEFAULT void FLASH_IRQHandler(void);
WEAK_DEFAULT void RCC_IRQHandler(void);
WEAK_DEFAULT void EXTI0_IRQHandler(void);
WEAK_DEFAULT void EXTI1_IRQHandler(void);
WEAK_DEFAULT void EXTI2_IRQHandler(void);
WEAK_DEFAULT void EXTI3_IRQHandler(void);
WEAK_DEFAULT void EXTI4_IRQHandler(void);
WEAK_DEFAULT void DMA1_Channel1_IRQHandler(void);
WEAK_DEFAULT void DMA1_Channel2_IRQHandler(void);
WEAK_DEFAULT void DMA1_Channel3_IRQHandler(void);
WEAK_DEFAULT void DMA1_Channel4_IRQHandler(void);
WEAK_DEFAULT void DMA1_Channel5_IRQHandler(void);
WEAK_DEFAULT void DMA1_Channel6_IRQHandler(void);
WEAK_DEFAULT void DMA1_Channel7_IRQHandler(void);

#ifdef STM32F10X_LD
WEAK_DEFAULT void ADC1_2_IRQHandler(void);
WEAK_DEFAULT void USB_HP_CAN1_TX_IRQHandler(void);
WEAK_DEFAULT void USB_LP_CAN1_RX0_IRQHandler(void);
WEAK_DEFAULT void CAN1_RX1_IRQHandler(void);
WEAK_DEFAULT void CAN1_SCE_IRQHandler(void);
WEAK_DEFAULT void EXTI9_5_IRQHandler(void);
WEAK_DEFAULT void TIM1_BRK_IRQHandler(void);
WEAK_DEFAULT void TIM1_UP_IRQHandler(void);
WEAK_DEFAULT void TIM1_TRG_COM_IRQHandler(void);
WEAK_DEFAULT void TIM1_CC_IRQHandler(void);
WEAK_DEFAULT void TIM2_IRQHandler(void);
WEAK_DEFAULT void TIM3_IRQHandler(void);
WEAK_DEFAULT void __Reserved_30_IRQHandler(void);
WEAK_DEFAULT void I2C1_EV_IRQHandler(void);
WEAK_DEFAULT void I2C1_ER_IRQHandler(void);
WEAK_DEFAULT void __Reserved_33_IRQHandler(void);
WEAK_DEFAULT void __Reserved_34_IRQHandler(void);
WEAK_DEFAULT void SPI1_IRQHandler(void);
WEAK_DEFAULT void __Reserved_36_IRQHandler(void);
WEAK_DEFAULT void USART1_IRQHandler(void);
WEAK_DEFAULT void USART2_IRQHandler(void);
WEAK_DEFAULT void __Reserved_39_IRQHandler(void);
WEAK_DEFAULT void EXTI15_10_IRQHandler(void);
WEAK_DEFAULT void RTCAlarm_IRQHandler(void);
WEAK_DEFAULT void USBWakeUp_IRQHandler(void);
#endif /* STM32F10X_LD */

#ifdef STM32F10X_LD_VL
WEAK_DEFAULT void ADC1_IRQHandler(void);
WEAK_DEFAULT void __Reserved_19_IRQHandler(void);
WEAK_DEFAULT void __Reserved_20_IRQHandler(void);
WEAK_DEFAULT void __Reserved_21_IRQHandler(void);
WEAK_DEFAULT void __Reserved_22_IRQHandler(void);
WEAK_DEFAULT void EXTI9_5_IRQHandler(void);
WEAK_DEFAULT void TIM1_BRK_TIM15_IRQHandler(void);
WEAK_DEFAULT void TIM1_UP_TIM16_IRQHandler(void);
WEAK_DEFAULT void TIM1_TRG_COM_TIM17_IRQHandler(void);
WEAK_DEFAULT void TIM1_CC_IRQHandler(void);
WEAK_DEFAULT void TIM2_IRQHandler(void);
WEAK_DEFAULT void TIM3_IRQHandler(void);
WEAK_DEFAULT void __Reserved_30_IRQHandler(void);
WEAK_DEFAULT void I2C1_EV_IRQHandler(void);
WEAK_DEFAULT void I2C1_ER_IRQHandler(void);
WEAK_DEFAULT void __Reserved_33_IRQHandler(void);
WEAK_DEFAULT void __Reserved_34_IRQHandler(void);
WEAK_DEFAULT void SPI1_IRQHandler(void);
WEAK_DEFAULT void __Reserved_36_IRQHandler(void);
WEAK_DEFAULT void USART1_IRQHandler(void);
WEAK_DEFAULT void USART2_IRQHandler(void);
WEAK_DEFAULT void __Reserved_39_IRQHandler(void);
WEAK_DEFAULT void EXTI15_10_IRQHandler(void);
WEAK_DEFAULT void RTCAlarm_IRQHandler(void);
WEAK_DEFAULT void CEC_IRQHandler(void);
WEAK_DEFAULT void __Reserved_43_IRQHandler(void);
WEAK_DEFAULT void __Reserved_44_IRQHandler(void);
WEAK_DEFAULT void __Reserved_45_IRQHandler(void);
WEAK_DEFAULT void __Reserved_46_IRQHandler(void);
WEAK_DEFAULT void __Reserved_47_IRQHandler(void);
WEAK_DEFAULT void __Reserved_48_IRQHandler(void);
WEAK_DEFAULT void __Reserved_49_IRQHandler(void);
WEAK_DEFAULT void __Reserved_50_IRQHandler(void);
WEAK_DEFAULT void __Reserved_51_IRQHandler(void);
WEAK_DEFAULT void __Reserved_52_IRQHandler(void);
WEAK_DEFAULT void __Reserved_53_IRQHandler(void);
WEAK_DEFAULT void TIM6_DAC_IRQHandler(void);
WEAK_DEFAULT void TIM7_IRQHandler(void);
#endif /* STM32F10X_LD_VL */

#ifdef STM32F10X_MD
WEAK_DEFAULT void ADC1_2_IRQHandler(void);
WEAK_DEFAULT void USB_HP_CAN1_TX_IRQHandler(void);
WEAK_DEFAULT void USB_LP_CAN1_RX0_IRQHandler(void);
WEAK_DEFAULT void CAN1_RX1_IRQHandler(void);
WEAK_DEFAULT void CAN1_SCE_IRQHandler(void);
WEAK_DEFAULT void EXTI9_5_IRQHandler(void);
WEAK_DEFAULT void TIM1_BRK_IRQHandler(void);
WEAK_DEFAULT void TIM1_UP_IRQHandler(void);
WEAK_DEFAULT void TIM1_TRG_COM_IRQHandler(void);
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
WEAK_DEFAULT void RTCAlarm_IRQHandler(void);
WEAK_DEFAULT void USBWakeUp_IRQHandler(void);
#endif /* STM32F10X_MD */

#ifdef STM32F10X_MD_VL
WEAK_DEFAULT void ADC1_IRQHandler(void);
WEAK_DEFAULT void __Reserved_19_IRQHandler(void);
WEAK_DEFAULT void __Reserved_20_IRQHandler(void);
WEAK_DEFAULT void __Reserved_21_IRQHandler(void);
WEAK_DEFAULT void __Reserved_22_IRQHandler(void);
WEAK_DEFAULT void EXTI9_5_IRQHandler(void);
WEAK_DEFAULT void TIM1_BRK_TIM15_IRQHandler(void);
WEAK_DEFAULT void TIM1_UP_TIM16_IRQHandler(void);
WEAK_DEFAULT void TIM1_TRG_COM_TIM17_IRQHandler(void);
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
WEAK_DEFAULT void RTCAlarm_IRQHandler(void);
WEAK_DEFAULT void CEC_IRQHandler(void);
WEAK_DEFAULT void __Reserved_43_IRQHandler(void);
WEAK_DEFAULT void __Reserved_44_IRQHandler(void);
WEAK_DEFAULT void __Reserved_45_IRQHandler(void);
WEAK_DEFAULT void __Reserved_46_IRQHandler(void);
WEAK_DEFAULT void __Reserved_47_IRQHandler(void);
WEAK_DEFAULT void __Reserved_48_IRQHandler(void);
WEAK_DEFAULT void __Reserved_49_IRQHandler(void);
WEAK_DEFAULT void __Reserved_50_IRQHandler(void);
WEAK_DEFAULT void __Reserved_51_IRQHandler(void);
WEAK_DEFAULT void __Reserved_52_IRQHandler(void);
WEAK_DEFAULT void __Reserved_53_IRQHandler(void);
WEAK_DEFAULT void TIM6_DAC_IRQHandler(void);
WEAK_DEFAULT void TIM7_IRQHandler(void);
#endif /* STM32F10X_MD_VL */

#ifdef STM32F10X_HD
WEAK_DEFAULT void ADC1_2_IRQHandler(void);
WEAK_DEFAULT void USB_HP_CAN1_TX_IRQHandler(void);
WEAK_DEFAULT void USB_LP_CAN1_RX0_IRQHandler(void);
WEAK_DEFAULT void CAN1_RX1_IRQHandler(void);
WEAK_DEFAULT void CAN1_SCE_IRQHandler(void);
WEAK_DEFAULT void EXTI9_5_IRQHandler(void);
WEAK_DEFAULT void TIM1_BRK_IRQHandler(void);
WEAK_DEFAULT void TIM1_UP_IRQHandler(void);
WEAK_DEFAULT void TIM1_TRG_COM_IRQHandler(void);
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
WEAK_DEFAULT void RTCAlarm_IRQHandler(void);
WEAK_DEFAULT void USBWakeUp_IRQHandler(void);
WEAK_DEFAULT void TIM8_BRK_IRQHandler(void);
WEAK_DEFAULT void TIM8_UP_IRQHandler(void);
WEAK_DEFAULT void TIM8_TRG_COM_IRQHandler(void);
WEAK_DEFAULT void TIM8_CC_IRQHandler(void);
WEAK_DEFAULT void ADC3_IRQHandler(void);
WEAK_DEFAULT void FSMC_IRQHandler(void);
WEAK_DEFAULT void SDIO_IRQHandler(void);
WEAK_DEFAULT void TIM5_IRQHandler(void);
WEAK_DEFAULT void SPI3_IRQHandler(void);
WEAK_DEFAULT void UART4_IRQHandler(void);
WEAK_DEFAULT void UART5_IRQHandler(void);
WEAK_DEFAULT void TIM6_IRQHandler(void);
WEAK_DEFAULT void TIM7_IRQHandler(void);
WEAK_DEFAULT void DMA2_Channel1_IRQHandler(void);
WEAK_DEFAULT void DMA2_Channel2_IRQHandler(void);
WEAK_DEFAULT void DMA2_Channel3_IRQHandler(void);
WEAK_DEFAULT void DMA2_Channel4_5_IRQHandler(void);
#endif /* STM32F10X_HD */

#ifdef STM32F10X_HD_VL
WEAK_DEFAULT void ADC1_IRQHandler(void);
WEAK_DEFAULT void __Reserved_19_IRQHandler(void);
WEAK_DEFAULT void __Reserved_20_IRQHandler(void);
WEAK_DEFAULT void __Reserved_21_IRQHandler(void);
WEAK_DEFAULT void __Reserved_22_IRQHandler(void);
WEAK_DEFAULT void EXTI9_5_IRQHandler(void);
WEAK_DEFAULT void TIM1_BRK_TIM15_IRQHandler(void);
WEAK_DEFAULT void TIM1_UP_TIM16_IRQHandler(void);
WEAK_DEFAULT void TIM1_TRG_COM_TIM17_IRQHandler(void);
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
WEAK_DEFAULT void RTCAlarm_IRQHandler(void);
WEAK_DEFAULT void CEC_IRQHandler(void);
WEAK_DEFAULT void TIM12_IRQHandler(void);
WEAK_DEFAULT void TIM13_IRQHandler(void);
WEAK_DEFAULT void TIM14_IRQHandler(void);
WEAK_DEFAULT void __Reserved_46_IRQHandler(void);
WEAK_DEFAULT void __Reserved_47_IRQHandler(void);
WEAK_DEFAULT void __Reserved_48_IRQHandler(void);
WEAK_DEFAULT void __Reserved_49_IRQHandler(void);
WEAK_DEFAULT void TIM5_IRQHandler(void);
WEAK_DEFAULT void SPI3_IRQHandler(void);
WEAK_DEFAULT void UART4_IRQHandler(void);
WEAK_DEFAULT void UART5_IRQHandler(void);
WEAK_DEFAULT void TIM6_DAC_IRQHandler(void);
WEAK_DEFAULT void TIM7_IRQHandler(void);
WEAK_DEFAULT void DMA2_Channel1_IRQHandler(void);
WEAK_DEFAULT void DMA2_Channel2_IRQHandler(void);
WEAK_DEFAULT void DMA2_Channel3_IRQHandler(void);
WEAK_DEFAULT void DMA2_Channel4_5_IRQHandler(void);
WEAK_DEFAULT void DMA2_Channel5_IRQHandler(void);
#endif /* STM32F10X_HD_VL */

#ifdef STM32F10X_XL
WEAK_DEFAULT void ADC1_2_IRQHandler(void);
WEAK_DEFAULT void USB_HP_CAN1_TX_IRQHandler(void);
WEAK_DEFAULT void USB_LP_CAN1_RX0_IRQHandler(void);
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
WEAK_DEFAULT void RTCAlarm_IRQHandler(void);
WEAK_DEFAULT void USBWakeUp_IRQHandler(void);
WEAK_DEFAULT void TIM8_BRK_TIM12_IRQHandler(void);
WEAK_DEFAULT void TIM8_UP_TIM13_IRQHandler(void);
WEAK_DEFAULT void TIM8_TRG_COM_TIM14_IRQHandler(void);
WEAK_DEFAULT void TIM8_CC_IRQHandler(void);
WEAK_DEFAULT void ADC3_IRQHandler(void);
WEAK_DEFAULT void FSMC_IRQHandler(void);
WEAK_DEFAULT void SDIO_IRQHandler(void);
WEAK_DEFAULT void TIM5_IRQHandler(void);
WEAK_DEFAULT void SPI3_IRQHandler(void);
WEAK_DEFAULT void UART4_IRQHandler(void);
WEAK_DEFAULT void UART5_IRQHandler(void);
WEAK_DEFAULT void TIM6_IRQHandler(void);
WEAK_DEFAULT void TIM7_IRQHandler(void);
WEAK_DEFAULT void DMA2_Channel1_IRQHandler(void);
WEAK_DEFAULT void DMA2_Channel2_IRQHandler(void);
WEAK_DEFAULT void DMA2_Channel3_IRQHandler(void);
WEAK_DEFAULT void DMA2_Channel4_5_IRQHandler(void);
#endif /* STM32F10X_XL */

#ifdef STM32F10X_CL
WEAK_DEFAULT void ADC1_2_IRQHandler(void);
WEAK_DEFAULT void CAN1_TX_IRQHandler(void);
WEAK_DEFAULT void CAN1_RX0_IRQHandler(void);
WEAK_DEFAULT void CAN1_RX1_IRQHandler(void);
WEAK_DEFAULT void CAN1_SCE_IRQHandler(void);
WEAK_DEFAULT void EXTI9_5_IRQHandler(void);
WEAK_DEFAULT void TIM1_BRK_IRQHandler(void);
WEAK_DEFAULT void TIM1_UP_IRQHandler(void);
WEAK_DEFAULT void TIM1_TRG_COM_IRQHandler(void);
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
WEAK_DEFAULT void RTCAlarm_IRQHandler(void);
WEAK_DEFAULT void OTG_FS_WKUP_IRQHandler(void);
WEAK_DEFAULT void __Reserved_43_IRQHandler(void);
WEAK_DEFAULT void __Reserved_44_IRQHandler(void);
WEAK_DEFAULT void __Reserved_45_IRQHandler(void);
WEAK_DEFAULT void __Reserved_46_IRQHandler(void);
WEAK_DEFAULT void __Reserved_47_IRQHandler(void);
WEAK_DEFAULT void __Reserved_48_IRQHandler(void);
WEAK_DEFAULT void __Reserved_49_IRQHandler(void);
WEAK_DEFAULT void TIM5_IRQHandler(void);
WEAK_DEFAULT void SPI3_IRQHandler(void);
WEAK_DEFAULT void UART4_IRQHandler(void);
WEAK_DEFAULT void UART5_IRQHandler(void);
WEAK_DEFAULT void TIM6_IRQHandler(void);
WEAK_DEFAULT void TIM7_IRQHandler(void);
WEAK_DEFAULT void DMA2_Channel1_IRQHandler(void);
WEAK_DEFAULT void DMA2_Channel2_IRQHandler(void);
WEAK_DEFAULT void DMA2_Channel3_IRQHandler(void);
WEAK_DEFAULT void DMA2_Channel4_IRQHandler(void);
WEAK_DEFAULT void DMA2_Channel5_IRQHandler(void);
WEAK_DEFAULT void ETH_IRQHandler(void);
WEAK_DEFAULT void ETH_WKUP_IRQHandler(void);
WEAK_DEFAULT void CAN2_TX_IRQHandler(void);
WEAK_DEFAULT void CAN2_RX0_IRQHandler(void);
WEAK_DEFAULT void CAN2_RX1_IRQHandler(void);
WEAK_DEFAULT void CAN2_SCE_IRQHandler(void);
WEAK_DEFAULT void OTG_FS_IRQHandler(void);
#endif /* STM32F10X_CL */

/*==================================================================================================
Vector table
==================================================================================================*/
extern const char __main_stack_end;          /* imported main stack end (from linker script)      */

void Reset_Handler(void);                    /* import the address of Reset_Handler()             */

void (*const vectors[])(void) __attribute__ ((section(".vectors"))) =
{
   (void (*)(void))&__main_stack_end,        /* Main stack end address                            */
   Reset_Handler,                            /* Reset                                             */
   NMI_Handler,                              /* Non-maskable interrupt (RCC clock security system)*/
   HardFault_Handler,                        /* All class of fault                                */
   MemManage_Handler,                        /* Memory management                                 */
   BusFault_Handler,                         /* Pre-fetch fault, memory access fault              */
   UsageFault_Handler,                       /* Undefined instruction or illegal state            */
   __Reserved_0x1C_IRQHandler,
   __Reserved_0x20_IRQHandler,
   __Reserved_0x24_IRQHandler,
   __Reserved_0x28_IRQHandler,
   SVC_Handler,                              /* System service call via SWI instruction           */
   DebugMon_Handler,                         /* Debug monitor                                     */
   __Reserved_0x34_IRQHandler,
   PendSV_Handler,                           /* Pendable request for system service               */
   SysTick_Handler,                          /* System tick timer                                 */

   /* STM32 specific Interrupt Numbers ************************************************************/
   WWDG_IRQHandler,                          /* Window watchdog interrupt                         */
   PVD_IRQHandler,                           /* PVD through EXTI line detection interrupt         */
   TAMPER_IRQHandler,                        /* Tamper interrupt                                  */
   RTC_IRQHandler,                           /* RTC global interrupt                              */
   FLASH_IRQHandler,                         /* Flash global interrupt                            */
   RCC_IRQHandler,                           /* RCC global interrupt                              */
   EXTI0_IRQHandler,                         /* EXTI Line0 interrupt                              */
   EXTI1_IRQHandler,                         /* EXTI Line1 interrupt                              */
   EXTI2_IRQHandler,                         /* EXTI Line2 interrupt                              */
   EXTI3_IRQHandler,                         /* EXTI Line3 interrupt                              */
   EXTI4_IRQHandler,                         /* EXTI Line4 interrupt                              */
   DMA1_Channel1_IRQHandler,                 /* DMA1 Channel1 global interrupt                    */
   DMA1_Channel2_IRQHandler,                 /* DMA1 Channel2 global interrupt                    */
   DMA1_Channel3_IRQHandler,                 /* DMA1 Channel3 global interrupt                    */
   DMA1_Channel4_IRQHandler,                 /* DMA1 Channel4 global interrupt                    */
   DMA1_Channel5_IRQHandler,                 /* DMA1 Channel5 global interrupt                    */
   DMA1_Channel6_IRQHandler,                 /* DMA1 Channel6 global interrupt                    */
   DMA1_Channel7_IRQHandler,                 /* DMA1 Channel7 global interrupt                    */

#ifdef STM32F10X_LD
   ADC1_2_IRQHandler,                        /* ADC1 and ADC2 global Interrupt                    */
   USB_HP_CAN1_TX_IRQHandler,                /* USB Device High Priority or CAN1 TX Interrupts    */
   USB_LP_CAN1_RX0_IRQHandler,               /* USB Device Low Priority or CAN1 RX0 Interrupts    */
   CAN1_RX1_IRQHandler,                      /* CAN1 RX1 Interrupt                                */
   CAN1_SCE_IRQHandler,                      /* CAN1 SCE Interrupt                                */
   EXTI9_5_IRQHandler,                       /* External Line[9:5] Interrupts                     */
   TIM1_BRK_IRQHandler,                      /* TIM1 Break Interrupt                              */
   TIM1_UP_IRQHandler,                       /* TIM1 Update Interrupt                             */
   TIM1_TRG_COM_IRQHandler,                  /* TIM1 Trigger and Commutation Interrupt            */
   TIM1_CC_IRQHandler,                       /* TIM1 Capture Compare Interrupt                    */
   TIM2_IRQHandler,                          /* TIM2 global Interrupt                             */
   TIM3_IRQHandler,                          /* TIM3 global Interrupt                             */
   __Reserved_30_IRQHandler,
   I2C1_EV_IRQHandler,                       /* I2C1 Event Interrupt                              */
   I2C1_ER_IRQHandler,                       /* I2C1 Error Interrupt                              */
   __Reserved_33_IRQHandler,
   __Reserved_34_IRQHandler,
   SPI1_IRQHandler,                          /* SPI1 global Interrupt                             */
   __Reserved_36_IRQHandler,
   USART1_IRQHandler,                        /* USART1 global Interrupt                           */
   USART2_IRQHandler,                        /* USART2 global Interrupt                           */
   __Reserved_39_IRQHandler,
   EXTI15_10_IRQHandler,                     /* External Line[15:10] Interrupts                   */
   RTCAlarm_IRQHandler,                      /* RTC Alarm through EXTI Line Interrupt             */
   USBWakeUp_IRQHandler                      /* USB Device WakeUp from suspend through EXTI Line  */
#endif /* STM32F10X_LD */

#ifdef STM32F10X_LD_VL
   ADC1_IRQHandler,                         /* ADC1 global Interrupt                              */
   __Reserved_19_IRQHandler,
   __Reserved_20_IRQHandler,
   __Reserved_21_IRQHandler,
   __Reserved_22_IRQHandler,
   EXTI9_5_IRQHandler,                      /* External Line[9:5] Interrupts                      */
   TIM1_BRK_TIM15_IRQHandler,               /* TIM1 Break Interrupt                               */
   TIM1_UP_TIM16_IRQHandler,                /* TIM1 Update Interrupt                              */
   TIM1_TRG_COM_TIM17_IRQHandler,           /* TIM1 Trigger and Commutation Interrupt             */
   TIM1_CC_IRQHandler,                      /* TIM1 Capture Compare Interrupt                     */
   TIM2_IRQHandler,                         /* TIM2 global Interrupt                              */
   TIM3_IRQHandler,                         /* TIM3 global Interrupt                              */
   __Reserved_30_IRQHandler,
   I2C1_EV_IRQHandler,                      /* I2C1 Event Interrupt                               */
   I2C1_ER_IRQHandler,                      /* I2C1 Error Interrupt                               */
   __Reserved_33_IRQHandler,
   __Reserved_34_IRQHandler,
   SPI1_IRQHandler,                         /* SPI1 global Interrupt                              */
   __Reserved_36_IRQHandler,
   USART1_IRQHandler,                       /* USART1 global Interrupt                            */
   USART2_IRQHandler,                       /* USART2 global Interrupt                            */
   __Reserved_39_IRQHandler,
   EXTI15_10_IRQHandler,                    /* External Line[15:10] Interrupts                    */
   RTCAlarm_IRQHandler,                     /* RTC Alarm through EXTI Line Interrupt              */
   CEC_IRQHandler,                          /* CEC global interrupt                               */
   __Reserved_43_IRQHandler,
   __Reserved_44_IRQHandler,
   __Reserved_45_IRQHandler,
   __Reserved_46_IRQHandler,
   __Reserved_47_IRQHandler,
   __Reserved_48_IRQHandler,
   __Reserved_49_IRQHandler,
   __Reserved_50_IRQHandler,
   __Reserved_51_IRQHandler,
   __Reserved_52_IRQHandler,
   __Reserved_53_IRQHandler,
   TIM6_DAC_IRQHandler,                     /* TIM6 and DAC underrun Interrupt                    */
   TIM7_IRQHandler                          /* TIM7 Interrupt                                     */
#endif /* STM32F10X_LD_VL */

#ifdef STM32F10X_MD
   ADC1_2_IRQHandler,                       /* ADC1 and ADC2 global Interrupt                     */
   USB_HP_CAN1_TX_IRQHandler,               /* USB Device High Priority or CAN1 TX Interrupts     */
   USB_LP_CAN1_RX0_IRQHandler,              /* USB Device Low Priority or CAN1 RX0 Interrupts     */
   CAN1_RX1_IRQHandler,                     /* CAN1 RX1 Interrupt                                 */
   CAN1_SCE_IRQHandler,                     /* CAN1 SCE Interrupt                                 */
   EXTI9_5_IRQHandler,                      /* External Line[9:5] Interrupts                      */
   TIM1_BRK_IRQHandler,                     /* TIM1 Break Interrupt                               */
   TIM1_UP_IRQHandler,                      /* TIM1 Update Interrupt                              */
   TIM1_TRG_COM_IRQHandler,                 /* TIM1 Trigger and Commutation Interrupt             */
   TIM1_CC_IRQHandler,                      /* TIM1 Capture Compare Interrupt                     */
   TIM2_IRQHandler,                         /* TIM2 global Interrupt                              */
   TIM3_IRQHandler,                         /* TIM3 global Interrupt                              */
   TIM4_IRQHandler,                         /* TIM4 global Interrupt                              */
   I2C1_EV_IRQHandler,                      /* I2C1 Event Interrupt                               */
   I2C1_ER_IRQHandler,                      /* I2C1 Error Interrupt                               */
   I2C2_EV_IRQHandler,                      /* I2C2 Event Interrupt                               */
   I2C2_ER_IRQHandler,                      /* I2C2 Error Interrupt                               */
   SPI1_IRQHandler,                         /* SPI1 global Interrupt                              */
   SPI2_IRQHandler,                         /* SPI2 global Interrupt                              */
   USART1_IRQHandler,                       /* USART1 global Interrupt                            */
   USART2_IRQHandler,                       /* USART2 global Interrupt                            */
   USART3_IRQHandler,                       /* USART3 global Interrupt                            */
   EXTI15_10_IRQHandler,                    /* External Line[15:10] Interrupts                    */
   RTCAlarm_IRQHandler,                     /* RTC Alarm through EXTI Line Interrupt              */
   USBWakeUp_IRQHandler                     /* USB Device WakeUp from suspend through EXTI Line   */
#endif /* STM32F10X_MD */

#ifdef STM32F10X_MD_VL
   ADC1_IRQHandler,                         /* ADC1 global Interrupt                              */
   __Reserved_19_IRQHandler,
   __Reserved_20_IRQHandler,
   __Reserved_21_IRQHandler,
   __Reserved_22_IRQHandler,
   EXTI9_5_IRQHandler,                      /* External Line[9:5] Interrupts                      */
   TIM1_BRK_TIM15_IRQHandler,               /* TIM1 Break and TIM15 Interrupts                    */
   TIM1_UP_TIM16_IRQHandler,                /* TIM1 Update and TIM16 Interrupts                   */
   TIM1_TRG_COM_TIM17_IRQHandler,           /* TIM1 Trigger and Commutation and TIM17 Interrupt   */
   TIM1_CC_IRQHandler,                      /* TIM1 Capture Compare Interrupt                     */
   TIM2_IRQHandler,                         /* TIM2 global Interrupt                              */
   TIM3_IRQHandler,                         /* TIM3 global Interrupt                              */
   TIM4_IRQHandler,                         /* TIM4 global Interrupt                              */
   I2C1_EV_IRQHandler,                      /* I2C1 Event Interrupt                               */
   I2C1_ER_IRQHandler,                      /* I2C1 Error Interrupt                               */
   I2C2_EV_IRQHandler,                      /* I2C2 Event Interrupt                               */
   I2C2_ER_IRQHandler,                      /* I2C2 Error Interrupt                               */
   SPI1_IRQHandler,                         /* SPI1 global Interrupt                              */
   SPI2_IRQHandler,                         /* SPI2 global Interrupt                              */
   USART1_IRQHandler,                       /* USART1 global Interrupt                            */
   USART2_IRQHandler,                       /* USART2 global Interrupt                            */
   USART3_IRQHandler,                       /* USART3 global Interrupt                            */
   EXTI15_10_IRQHandler,                    /* External Line[15:10] Interrupts                    */
   RTCAlarm_IRQHandler,                     /* RTC Alarm through EXTI Line Interrupt              */
   CEC_IRQHandler,                          /* HDMI-CEC Interrupt                                 */
   __Reserved_43_IRQHandler,
   __Reserved_44_IRQHandler,
   __Reserved_45_IRQHandler,
   __Reserved_46_IRQHandler,
   __Reserved_47_IRQHandler,
   __Reserved_48_IRQHandler,
   __Reserved_49_IRQHandler,
   __Reserved_50_IRQHandler,
   __Reserved_51_IRQHandler,
   __Reserved_52_IRQHandler,
   __Reserved_53_IRQHandler,
   TIM6_DAC_IRQHandler,                     /* TIM6 and DAC underrun Interrupt                    */
   TIM7_IRQHandler                          /* TIM7 Interrupt                                     */
#endif /* STM32F10X_MD_VL */

#ifdef STM32F10X_HD
   ADC1_2_IRQHandler,                       /* ADC1 and ADC2 global Interrupt                     */
   USB_HP_CAN1_TX_IRQHandler,               /* USB Device High Priority or CAN1 TX Interrupts     */
   USB_LP_CAN1_RX0_IRQHandler,              /* USB Device Low Priority or CAN1 RX0 Interrupts     */
   CAN1_RX1_IRQHandler,                     /* CAN1 RX1 Interrupt                                 */
   CAN1_SCE_IRQHandler,                     /* CAN1 SCE Interrupt                                 */
   EXTI9_5_IRQHandler,                      /* External Line[9:5] Interrupts                      */
   TIM1_BRK_IRQHandler,                     /* TIM1 Break Interrupt                               */
   TIM1_UP_IRQHandler,                      /* TIM1 Update Interrupt                              */
   TIM1_TRG_COM_IRQHandler,                 /* TIM1 Trigger and Commutation Interrupt             */
   TIM1_CC_IRQHandler,                      /* TIM1 Capture Compare Interrupt                     */
   TIM2_IRQHandler,                         /* TIM2 global Interrupt                              */
   TIM3_IRQHandler,                         /* TIM3 global Interrupt                              */
   TIM4_IRQHandler,                         /* TIM4 global Interrupt                              */
   I2C1_EV_IRQHandler,                      /* I2C1 Event Interrupt                               */
   I2C1_ER_IRQHandler,                      /* I2C1 Error Interrupt                               */
   I2C2_EV_IRQHandler,                      /* I2C2 Event Interrupt                               */
   I2C2_ER_IRQHandler,                      /* I2C2 Error Interrupt                               */
   SPI1_IRQHandler,                         /* SPI1 global Interrupt                              */
   SPI2_IRQHandler,                         /* SPI2 global Interrupt                              */
   USART1_IRQHandler,                       /* USART1 global Interrupt                            */
   USART2_IRQHandler,                       /* USART2 global Interrupt                            */
   USART3_IRQHandler,                       /* USART3 global Interrupt                            */
   EXTI15_10_IRQHandler,                    /* External Line[15:10] Interrupts                    */
   RTCAlarm_IRQHandler,                     /* RTC Alarm through EXTI Line Interrupt              */
   USBWakeUp_IRQHandler,                    /* USB Device WakeUp from suspend through EXTI Line   */
   TIM8_BRK_IRQHandler,                     /* TIM8 Break Interrupt                               */
   TIM8_UP_IRQHandler,                      /* TIM8 Update Interrupt                              */
   TIM8_TRG_COM_IRQHandler,                 /* TIM8 Trigger and Commutation Interrupt             */
   TIM8_CC_IRQHandler,                      /* TIM8 Capture Compare Interrupt                     */
   ADC3_IRQHandler,                         /* ADC3 global Interrupt                              */
   FSMC_IRQHandler,                         /* FSMC global Interrupt                              */
   SDIO_IRQHandler,                         /* SDIO global Interrupt                              */
   TIM5_IRQHandler,                         /* TIM5 global Interrupt                              */
   SPI3_IRQHandler,                         /* SPI3 global Interrupt                              */
   UART4_IRQHandler,                        /* UART4 global Interrupt                             */
   UART5_IRQHandler,                        /* UART5 global Interrupt                             */
   TIM6_IRQHandler,                         /* TIM6 global Interrupt                              */
   TIM7_IRQHandler,                         /* TIM7 global Interrupt                              */
   DMA2_Channel1_IRQHandler,                /* DMA2 Channel 1 global Interrupt                    */
   DMA2_Channel2_IRQHandler,                /* DMA2 Channel 2 global Interrupt                    */
   DMA2_Channel3_IRQHandler,                /* DMA2 Channel 3 global Interrupt                    */
   DMA2_Channel4_5_IRQHandler               /* DMA2 Channel 4 and Channel 5 global Interrupt      */
#endif /* STM32F10X_HD */

#ifdef STM32F10X_HD_VL
   ADC1_IRQHandler,                         /* ADC1 global Interrupt                              */
   __Reserved_19_IRQHandler,
   __Reserved_20_IRQHandler,
   __Reserved_21_IRQHandler,
   __Reserved_22_IRQHandler,
   EXTI9_5_IRQHandler,                      /* External Line[9:5] Interrupts                      */
   TIM1_BRK_TIM15_IRQHandler,               /* TIM1 Break and TIM15 Interrupts                    */
   TIM1_UP_TIM16_IRQHandler,                /* TIM1 Update and TIM16 Interrupts                   */
   TIM1_TRG_COM_TIM17_IRQHandler,           /* TIM1 Trigger and Commutation and TIM17 Interrupt   */
   TIM1_CC_IRQHandler,                      /* TIM1 Capture Compare Interrupt                     */
   TIM2_IRQHandler,                         /* TIM2 global Interrupt                              */
   TIM3_IRQHandler,                         /* TIM3 global Interrupt                              */
   TIM4_IRQHandler,                         /* TIM4 global Interrupt                              */
   I2C1_EV_IRQHandler,                      /* I2C1 Event Interrupt                               */
   I2C1_ER_IRQHandler,                      /* I2C1 Error Interrupt                               */
   I2C2_EV_IRQHandler,                      /* I2C2 Event Interrupt                               */
   I2C2_ER_IRQHandler,                      /* I2C2 Error Interrupt                               */
   SPI1_IRQHandler,                         /* SPI1 global Interrupt                              */
   SPI2_IRQHandler,                         /* SPI2 global Interrupt                              */
   USART1_IRQHandler,                       /* USART1 global Interrupt                            */
   USART2_IRQHandler,                       /* USART2 global Interrupt                            */
   USART3_IRQHandler,                       /* USART3 global Interrupt                            */
   EXTI15_10_IRQHandler,                    /* External Line[15:10] Interrupts                    */
   RTCAlarm_IRQHandler,                     /* RTC Alarm through EXTI Line Interrupt              */
   CEC_IRQHandler,                          /* HDMI-CEC Interrupt                                 */
   TIM12_IRQHandler,                        /* TIM12 global Interrupt                             */
   TIM13_IRQHandler,                        /* TIM13 global Interrupt                             */
   TIM14_IRQHandler,                        /* TIM14 global Interrupt                             */
   __Reserved_46_IRQHandler,
   __Reserved_47_IRQHandler,
   __Reserved_48_IRQHandler,
   __Reserved_49_IRQHandler,
   TIM5_IRQHandler,                         /* TIM5 global Interrupt                              */
   SPI3_IRQHandler,                         /* SPI3 global Interrupt                              */
   UART4_IRQHandler,                        /* UART4 global Interrupt                             */
   UART5_IRQHandler,                        /* UART5 global Interrupt                             */
   TIM6_DAC_IRQHandler,                     /* TIM6 and DAC underrun Interrupt                    */
   TIM7_IRQHandler,                         /* TIM7 Interrupt                                     */
   DMA2_Channel1_IRQHandler,                /* DMA2 Channel 1 global Interrupt                    */
   DMA2_Channel2_IRQHandler,                /* DMA2 Channel 2 global Interrupt                    */
   DMA2_Channel3_IRQHandler,                /* DMA2 Channel 3 global Interrupt                    */
   DMA2_Channel4_5_IRQHandler,              /* DMA2 Channel 4 and Channel 5 global Interrupt      */
   DMA2_Channel5_IRQHandler                 /* DMA2 Channel 5 global Interrupt (DMA2 Channel 5
                                               mapped at position 60 only if the MISC_REMAP bit in
                                               the AFIO_MAPR2 register is set)                    */
#endif /* STM32F10X_HD_VL */

#ifdef STM32F10X_XL
   ADC1_2_IRQHandler,                       /* ADC1 and ADC2 global Interrupt                     */
   USB_HP_CAN1_TX_IRQHandler,               /* USB Device High Priority or CAN1 TX Interrupts     */
   USB_LP_CAN1_RX0_IRQHandler,              /* USB Device Low Priority or CAN1 RX0 Interrupts     */
   CAN1_RX1_IRQHandler,                     /* CAN1 RX1 Interrupt                                 */
   CAN1_SCE_IRQHandler,                     /* CAN1 SCE Interrupt                                 */
   EXTI9_5_IRQHandler,                      /* External Line[9:5] Interrupts                      */
   TIM1_BRK_TIM9_IRQHandler,                /* TIM1 Break Interrupt and TIM9 global Interrupt     */
   TIM1_UP_TIM10_IRQHandler,                /* TIM1 Update Interrupt and TIM10 global Interrupt   */
   TIM1_TRG_COM_TIM11_IRQHandler,           /* TIM1 Trigger and Commutation Interrupt and TIM11   */
   TIM1_CC_IRQHandler,                      /* TIM1 Capture Compare Interrupt                     */
   TIM2_IRQHandler,                         /* TIM2 global Interrupt                              */
   TIM3_IRQHandler,                         /* TIM3 global Interrupt                              */
   TIM4_IRQHandler,                         /* TIM4 global Interrupt                              */
   I2C1_EV_IRQHandler,                      /* I2C1 Event Interrupt                               */
   I2C1_ER_IRQHandler,                      /* I2C1 Error Interrupt                               */
   I2C2_EV_IRQHandler,                      /* I2C2 Event Interrupt                               */
   I2C2_ER_IRQHandler,                      /* I2C2 Error Interrupt                               */
   SPI1_IRQHandler,                         /* SPI1 global Interrupt                              */
   SPI2_IRQHandler,                         /* SPI2 global Interrupt                              */
   USART1_IRQHandler,                       /* USART1 global Interrupt                            */
   USART2_IRQHandler,                       /* USART2 global Interrupt                            */
   USART3_IRQHandler,                       /* USART3 global Interrupt                            */
   EXTI15_10_IRQHandler,                    /* External Line[15:10] Interrupts                    */
   RTCAlarm_IRQHandler,                     /* RTC Alarm through EXTI Line Interrupt              */
   USBWakeUp_IRQHandler,                    /* USB Device WakeUp from suspend through EXTI Line   */
   TIM8_BRK_TIM12_IRQHandler,               /* TIM8 Break Interrupt and TIM12 global Interrupt    */
   TIM8_UP_TIM13_IRQHandler,                /* TIM8 Update Interrupt and TIM13 global Interrupt   */
   TIM8_TRG_COM_TIM14_IRQHandler,           /* TIM8 Trigger and Commutation Interrupt and TIM14   */
   TIM8_CC_IRQHandler,                      /* TIM8 Capture Compare Interrupt                     */
   ADC3_IRQHandler,                         /* ADC3 global Interrupt                              */
   FSMC_IRQHandler,                         /* FSMC global Interrupt                              */
   SDIO_IRQHandler,                         /* SDIO global Interrupt                              */
   TIM5_IRQHandler,                         /* TIM5 global Interrupt                              */
   SPI3_IRQHandler,                         /* SPI3 global Interrupt                              */
   UART4_IRQHandler,                        /* UART4 global Interrupt                             */
   UART5_IRQHandler,                        /* UART5 global Interrupt                             */
   TIM6_IRQHandler,                         /* TIM6 global Interrupt                              */
   TIM7_IRQHandler,                         /* TIM7 global Interrupt                              */
   DMA2_Channel1_IRQHandler,                /* DMA2 Channel 1 global Interrupt                    */
   DMA2_Channel2_IRQHandler,                /* DMA2 Channel 2 global Interrupt                    */
   DMA2_Channel3_IRQHandler,                /* DMA2 Channel 3 global Interrupt                    */
   DMA2_Channel4_5_IRQHandler               /* DMA2 Channel 4 and Channel 5 global Interrupt      */
#endif /* STM32F10X_XL */

#ifdef STM32F10X_CL
   ADC1_2_IRQHandler,                       /* ADC1 and ADC2 global Interrupt                     */
   CAN1_TX_IRQHandler,                      /* USB Device High Priority or CAN1 TX Interrupts     */
   CAN1_RX0_IRQHandler,                     /* USB Device Low Priority or CAN1 RX0 Interrupts     */
   CAN1_RX1_IRQHandler,                     /* CAN1 RX1 Interrupt                                 */
   CAN1_SCE_IRQHandler,                     /* CAN1 SCE Interrupt                                 */
   EXTI9_5_IRQHandler,                      /* External Line[9:5] Interrupts                      */
   TIM1_BRK_IRQHandler,                     /* TIM1 Break Interrupt                               */
   TIM1_UP_IRQHandler,                      /* TIM1 Update Interrupt                              */
   TIM1_TRG_COM_IRQHandler,                 /* TIM1 Trigger and Commutation Interrupt             */
   TIM1_CC_IRQHandler,                      /* TIM1 Capture Compare Interrupt                     */
   TIM2_IRQHandler,                         /* TIM2 global Interrupt                              */
   TIM3_IRQHandler,                         /* TIM3 global Interrupt                              */
   TIM4_IRQHandler,                         /* TIM4 global Interrupt                              */
   I2C1_EV_IRQHandler,                      /* I2C1 Event Interrupt                               */
   I2C1_ER_IRQHandler,                      /* I2C1 Error Interrupt                               */
   I2C2_EV_IRQHandler,                      /* I2C2 Event Interrupt                               */
   I2C2_ER_IRQHandler,                      /* I2C2 Error Interrupt                               */
   SPI1_IRQHandler,                         /* SPI1 global Interrupt                              */
   SPI2_IRQHandler,                         /* SPI2 global Interrupt                              */
   USART1_IRQHandler,                       /* USART1 global Interrupt                            */
   USART2_IRQHandler,                       /* USART2 global Interrupt                            */
   USART3_IRQHandler,                       /* USART3 global Interrupt                            */
   EXTI15_10_IRQHandler,                    /* External Line[15:10] Interrupts                    */
   RTCAlarm_IRQHandler,                     /* RTC Alarm through EXTI Line Interrupt              */
   OTG_FS_WKUP_IRQHandler,                  /* USB OTG FS WakeUp from suspend through EXTI Line   */
   __Reserved_43_IRQHandler,
   __Reserved_44_IRQHandler,
   __Reserved_45_IRQHandler,
   __Reserved_46_IRQHandler,
   __Reserved_47_IRQHandler,
   __Reserved_48_IRQHandler,
   __Reserved_49_IRQHandler,
   TIM5_IRQHandler,                         /* TIM5 global Interrupt                              */
   SPI3_IRQHandler,                         /* SPI3 global Interrupt                              */
   UART4_IRQHandler,                        /* UART4 global Interrupt                             */
   UART5_IRQHandler,                        /* UART5 global Interrupt                             */
   TIM6_IRQHandler,                         /* TIM6 global Interrupt                              */
   TIM7_IRQHandler,                         /* TIM7 global Interrupt                              */
   DMA2_Channel1_IRQHandler,                /* DMA2 Channel 1 global Interrupt                    */
   DMA2_Channel2_IRQHandler,                /* DMA2 Channel 2 global Interrupt                    */
   DMA2_Channel3_IRQHandler,                /* DMA2 Channel 3 global Interrupt                    */
   DMA2_Channel4_IRQHandler,                /* DMA2 Channel 4 global Interrupt                    */
   DMA2_Channel5_IRQHandler,                /* DMA2 Channel 5 global Interrupt                    */
   ETH_IRQHandler,                          /* Ethernet global Interrupt                          */
   ETH_WKUP_IRQHandler,                     /* Ethernet Wakeup through EXTI line Interrupt        */
   CAN2_TX_IRQHandler,                      /* CAN2 TX Interrupt                                  */
   CAN2_RX0_IRQHandler,                     /* CAN2 RX0 Interrupt                                 */
   CAN2_RX1_IRQHandler,                     /* CAN2 RX1 Interrupt                                 */
   CAN2_SCE_IRQHandler,                     /* CAN2 SCE Interrupt                                 */
   OTG_FS_IRQHandler                        /* USB OTG FS global Interrupt                        */
#endif /* STM32F10X_CL */
};

/***************************************************************************************************
END OF FILE
***************************************************************************************************/
