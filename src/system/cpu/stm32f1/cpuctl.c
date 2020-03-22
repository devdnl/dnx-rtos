/*=========================================================================*//**
@file    cpuctl.c

@author  Daniel Zorychta

@brief   This file support CPU control

@note    Copyright (C) 2017 Daniel Zorychta <daniel.zorychta@gmail.com>

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

/*==============================================================================
  Include files
==============================================================================*/
#include "config.h"
#include "stm32f1/cpuctl.h"
#include "stm32f1/stm32f10x.h"
#include "stm32f1/lib/misc.h"
#include "stm32f1/lib/stm32f10x_rcc.h"
#include "kernel/kwrapper.h"
#include "kernel/kpanic.h"

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
static void calculate_ticks_per_us(void);

/*==============================================================================
  Local object definitions
==============================================================================*/
static u32_t ticks_per_us;

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
        NVIC_SetVectorTable(NVIC_VectTab_FLASH, __CPU_VTOR_TAB_POSITION__);
        NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);

#if __CPU_DISABLE_INTER_OF_MCYCLE_INSTR__ == _YES_
        /* disable interrupting multi-cycle instructions */
        SCnSCB->ACTLR |= SCnSCB_ACTLR_DISMCYCINT_Msk;
#endif

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
        static uint32_t last;
        bool  ovf = SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk;
        u32_t now = SysTick->VAL;

        u32_t delta;
        if (ovf) {
                delta = ((SysTick->LOAD + 1) - now) + last;
        } else {
                delta = last - now;
        }

        last = now;

        return delta;
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

        calculate_ticks_per_us();

        _critical_section_end();
}

//==============================================================================
/**
 * @brief  Function delay code processing in microseconds.
 *
 * @note   Function should block CPU for specified amount of time.
 * @note   Function should work in critical section and interrupts.
 *
 * @param  microseconds         microsecond delay
 */
//==============================================================================
void _cpuctl_delay_us(u16_t microseconds)
{
        while (microseconds > 0) {

                u32_t ticks = ticks_per_us;

                while (ticks > 0) {
                        __asm volatile("nop");
                        ticks--;
                }

                microseconds--;
        }
}

//==============================================================================
/**
 * @brief  Function calculate number of loop needed to generate 1us delay.
 */
//==============================================================================
static void calculate_ticks_per_us(void)
{
        ticks_per_us = 0;

        u32_t ticks = ((u64_t)250 * SysTick->LOAD * __OS_TASK_SCHED_FREQ__) / 1000000;

        while (SysTick->VAL >= (SysTick->LOAD / 2));

        u32_t target = SysTick->VAL - ticks;

        while (SysTick->VAL > target) {
                ticks_per_us++;
        }

        ticks_per_us /= 185;

        if (ticks_per_us == 0) {
                ticks_per_us = 1;
        }
}

//==============================================================================
/**
 * @brief Hard Fault ISR
 */
//==============================================================================
void HardFault_Handler(void)
{
        _kernel_panic_report(_KERNEL_PANIC_DESC_CAUSE_SEGFAULT);
}

//==============================================================================
/**
 * @brief Memory Management failure ISR
 */
//==============================================================================
void MemManage_Handler(void)
{
        _kernel_panic_report(_KERNEL_PANIC_DESC_CAUSE_CPUFAULT);
}

//==============================================================================
/**
 * @brief Bus Fault ISR
 */
//==============================================================================
void BusFault_Handler(void)
{
        _kernel_panic_report(_KERNEL_PANIC_DESC_CAUSE_CPUFAULT);
}

//==============================================================================
/**
 * @brief Usage Fault ISR
 */
//==============================================================================
void UsageFault_Handler(void)
{
        _kernel_panic_report(_KERNEL_PANIC_DESC_CAUSE_CPUFAULT);
}

/*==============================================================================
  End of file
==============================================================================*/
