/*=========================================================================*//**
@file    cpuctl.c

@author  Daniel Zorychta

@brief   This file support CPU control

@note    Copyright (C) 2012 Daniel Zorychta <daniel.zorychta@gmail.com>

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

/*==============================================================================
  Include files
==============================================================================*/
#include "config.h"
#include "stm32f1/cpuctl.h"
#include "stm32f1/stm32f10x.h"
#include "stm32f1/lib/misc.h"
#include "stm32f1/lib/stm32f10x_rcc.h"
#include "kernel/kwrapper.h"

/*==============================================================================
  Local symbolic constants/macros
==============================================================================*/
/* Cortex System Control register address */
#define SCB_SysCtrl             (*((__IO uint32_t *)0xE000ED10))
#define SysCtrl_SLEEPDEEP       ((uint32_t)0x00000004)

/*==============================================================================
  Local types, enums definitions
==============================================================================*/

/*==============================================================================
  Local function prototypes
==============================================================================*/

/*==============================================================================
  Local object definitions
==============================================================================*/
#if (__OS_MONITOR_CPU_LOAD__ > 0)
static const u32_t timer_frequency = 1000000;
#endif

/*==============================================================================
  Function definitions
==============================================================================*/

//==============================================================================
/**
 * @brief  Basic (first) CPU/microcontroller configuration. This function is
 *         called before system start.
 *
 * @param  None
 *
 * @return None
 */
//==============================================================================
void _cpuctl_init(void)
{
        /* set interrupt vectors and NVIC priority */
        NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);
        NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);

        /* enable sleep on idle debug */
        SET_BIT(DBGMCU->CR, DBGMCU_CR_DBG_SLEEP);

        #if (__OS_MONITOR_CPU_LOAD__ > 0)
        _cpuctl_init_CPU_load_counter();
        #endif
}

//==============================================================================
/**
 * @brief  This function restart CPU.
 */
//==============================================================================
void _cpuctl_restart_system(void)
{
        NVIC_SystemReset();
}

//==============================================================================
/**
 * @brief  This function restart CPU.
 */
//==============================================================================
void _cpuctl_shutdown_system(void)
{
        // Note: implementation enters to deep sleep mode.

        /* Clear Wake-up flag */
        PWR->CR |= PWR_CR_CWUF;

        /* Select STANDBY mode */
        PWR->CR |= PWR_CR_PDDS;

        /* Set SLEEPDEEP bit of Cortex System Control Register */
        SCB_SysCtrl |= SysCtrl_SLEEPDEEP;

        /* Request Wait For Interrupt */
        __WFI();
}

//==============================================================================
/**
 * @brief  Start counter used for CPU load measurement. Timer should be set
 *         to at least 1MHz and should not overflow until 1 second.
 *
 * @param  None
 *
 * @return None
 */
//==============================================================================
#if (__OS_MONITOR_CPU_LOAD__ > 0)
void _cpuctl_init_CPU_load_counter(void)
{
        /* enable clock */
        RCC->APB1ENR  |= RCC_APB1ENR_TIM2EN;

        /* reset timer */
        RCC->APB1RSTR |=  RCC_APB1RSTR_TIM2RST;
        RCC->APB1RSTR &= ~RCC_APB1RSTR_TIM2RST;

        /* configure timer */
        RCC_ClocksTypeDef freq;
        RCC_GetClocksFreq(&freq);
        if (RCC->CFGR & RCC_CFGR_PPRE1_2) {
                freq.PCLK1_Frequency *= 2;
        }

        TIM2->PSC = (freq.PCLK1_Frequency/timer_frequency) - 1;
        TIM2->ARR = 0xFFFF;
        TIM2->CR1 = TIM_CR1_CEN;
}
#endif

//==============================================================================
/**
 * @brief  Function return valut that was counted from last call of this function.
 *         This function must reset timer after read. Function is called from
 *         IRQs.
 *
 * @param  None
 *
 * @return Timer value for last read (time delta).
 */
//==============================================================================
#if (__OS_MONITOR_CPU_LOAD__ > 0)
u32_t _cpuctl_get_CPU_load_counter_delta(void)
{
        u32_t diff = TIM2->CNT;
        TIM2->CNT  = 0;
        return diff;
}
#endif

//==============================================================================
/**
 * @brief  Function sleep CPU weakly. All IRQs must be able to wake up CPU.
 *
 * @param  None
 *
 * @return None
 */
//==============================================================================
void _cpuctl_sleep(void)
{
        __WFI();
}

//==============================================================================
/**
 * @brief  Function update all system clock after CPU frequency change.
 *         Function must update all devices which base on main clock oscillator.
 *         Function is called after clock/frequency change from clock management driver.
 *
 * @param  None
 *
 * @return None
 */
//==============================================================================
void _cpuctl_update_system_clocks(void)
{
        /* update CPU load timer frequency */
#if (__OS_MONITOR_CPU_LOAD__ > 0)
        _cpuctl_init_CPU_load_counter();
#endif

        /* update context switch counter frequency */
        _critical_section_begin();
        RCC_ClocksTypeDef freq;
        RCC_GetClocksFreq(&freq);
        SysTick_Config((freq.HCLK_Frequency / (u32_t)__OS_TASK_SCHED_FREQ__) - 1);
        _critical_section_end();
}

/*==============================================================================
  End of file
==============================================================================*/
