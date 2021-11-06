/*=========================================================================*//**
@file    cpuctl.c

@author  Daniel Zorychta

@brief   This file support CPU control

@note    Copyright (C) 2020 Daniel Zorychta <daniel.zorychta@gmail.com>

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
#include "stm32f3/cpuctl.h"
#include "stm32f3/stm32f3xx.h"
#include "stm32f3/lib/stm32f3xx_ll_rcc.h"
#include "stm32f3/lib/misc.h"
#include "kernel/kwrapper.h"
#include "kernel/kpanic.h"
#include "kernel/sysfunc.h"

/*==============================================================================
  Local symbolic constants/macros
==============================================================================*/
/* Cortex System Control register address */
#define SCB_SysCtrl             (*((__IO uint32_t *)0xE000ED10))
#define SysCtrl_SLEEPDEEP       ((uint32_t)0x00000004)

#define SRAM_HEAP_START         ((void *)&__heap_start)
#define SRAM_HEAP_SIZE          ((size_t)&__heap_size)

#define CCM_START               ((void *)&__ccm_start)
#define CCM_SIZE                ((size_t)&__ccm_size)

// Memory Management Fault Status Register
#define NVIC_MFSR               (*(volatile unsigned char*)(0xE000ED28u))

// Bus Fault Status Register
#define NVIC_BFSR               (*(volatile unsigned char*)(0xE000ED29u))

// Usage Fault Status Register
#define NVIC_UFSR               (*(volatile unsigned short*)(0xE000ED2Au))

// Hard Fault Status Register
#define NVIC_HFSR               (*(volatile unsigned int*)(0xE000ED2Cu))

// Debug Fault Status Register
#define NVIC_DFSR               (*(volatile unsigned int*)(0xE000ED30u))

// MemManage Fault Address Register
#define NVIC_MFAR               (*(volatile unsigned int*)(0xE000ED34u))

// Bus Fault Manage Address Register
#define NVIC_BFAR               (*(volatile unsigned int*)(0xE000ED38u))

// Auxiliary Fault Status Register
#define NVIC_AFSR               (*(volatile unsigned int*)(0xE000ED3Cu))

/*==============================================================================
  Local types, enums definitions
==============================================================================*/
typedef struct {
        uint32_t R0;
        uint32_t R1;
        uint32_t R2;
        uint32_t R3;
        uint32_t R12;
        uint32_t LR;    // Link register.
        uint32_t PC;    // Program counter.
        uint32_t PSR;   // Program status register.
        uint32_t MFSR;  // Memory Management Fault Status Register.
        uint32_t BFSR;  // Bus Fault Status Register.
        uint32_t UFSR;  // Usage Fault Status Register.
        uint32_t HFSR;  // Hard Fault Status Register.
        uint32_t DFSR;  // Debug Fault Status Register.
        uint32_t BFAR;  // Bus Fault Manage Address Register.
        uint32_t AFSR;  // Auxiliary Fault Status Register.
        uint32_t MFAR;  // MemManage Fault Address Register
} reg_dump_t;

/*==============================================================================
  Local function prototypes
==============================================================================*/
void get_registers_from_stack(uint32_t *stack_address);

/*==============================================================================
  Local object definitions
==============================================================================*/
extern void *__heap_start;
extern void *__heap_size;
extern void *__ccm_start;
extern void *__ccm_size;

static _mm_region_t sram;
static _mm_region_t ccm;

static volatile reg_dump_t reg_dump __attribute__ ((section (".noinit")));

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

        /* enable FPU */
        SCB->CPACR |= ((3 << 20)|(3 << 22));

#if __CPU_DISABLE_INTER_OF_MCYCLE_INSTR__ == _YES_
        /* disable interrupting multi-cycle instructions */
        SCnSCB->ACTLR |= SCnSCB_ACTLR_DISMCYCINT_Msk;
#endif

        /* enable sleep on idle debug */
        SET_BIT(DBGMCU->CR, DBGMCU_CR_DBG_SLEEP);

        #if (__OS_MONITOR_CPU_LOAD__ > 0)
        _cpuctl_init_CPU_load_counter();
        #endif

        _mm_register_region(&sram, SRAM_HEAP_START, SRAM_HEAP_SIZE, _MM_FLAG__DMA_CAPABLE, "SRAM");

        if (CCM_SIZE > 0) {
                _mm_register_region(&ccm, CCM_START, CCM_SIZE, 0, _CPUCTL_FAST_MEM);
        }

#if __CPU_SHOW_AND_CLEAR_RESET_SOURCE__
        u32_t CSR = RCC->CSR;
        SET_BIT(RCC->CSR, RCC_CSR_RMVF);
        printk("Reset source:");
        if (CSR & RCC_CSR_LPWRRSTF) printk("  Low power reset");
        if (CSR & RCC_CSR_WWDGRSTF) printk("  Window watchdog reset");
        if (CSR & RCC_CSR_IWDGRSTF) printk("  Watchdog reset");
        if (CSR & RCC_CSR_SFTRSTF)  printk("  Software reset");
        if (CSR & RCC_CSR_PORRSTF)  printk("  POR reset");
        if (CSR & RCC_CSR_PINRSTF)  printk("  Pin reset");
        if (CSR & RCC_CSR_OBLRSTF)  printk("  OBL reset");
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

        /* enable power module */
        RCC->APB1ENR |= RCC_APB1ENR_PWREN;

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
        LL_RCC_ClocksTypeDef freq;
        LL_RCC_GetSystemClocksFreq(&freq);
        SysTick_Config((freq.HCLK_Frequency / (u32_t)__OS_TASK_SCHED_FREQ__) - 1);
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
        u32_t ticks = ((u64_t)microseconds * SysTick->LOAD * __OS_TASK_SCHED_FREQ__) / 1000000;

        while (ticks > 0) {
                i32_t now = SysTick->VAL;

                if (now - ticks > 0) {
                        while (SysTick->VAL > (u32_t)(now - ticks));
                        ticks = 0;
                } else {
                        while (SysTick->VAL <= (u32_t)now);
                        ticks -= now;
                }
        }
}

//==============================================================================
/**
 * @brief  Function printout dumped registers.
 */
//==============================================================================
void _cpuctl_print_exception(void)
{
        printk("R0   : %08xh\n", reg_dump.R0);
        printk("R1   : %08xh\n", reg_dump.R1);
        printk("R2   : %08xh\n", reg_dump.R2);
        printk("R3   : %08xh\n", reg_dump.R3);
        printk("R12  : %08xh\n", reg_dump.R12);
        printk("LR   : %08xh\n", reg_dump.LR);
        printk("PC   : %08xh\n", reg_dump.PC);
        printk("PSR  : %08xh\n", reg_dump.PSR);
        printk("MFSR : %08xh\n", reg_dump.MFSR);
        printk("BFSR : %08xh\n", reg_dump.BFSR);
        printk("UFSR : %08xh\n", reg_dump.UFSR);
        printk("HFSR : %08xh\n", reg_dump.HFSR);
        printk("DFSR : %08xh\n", reg_dump.DFSR);
        printk("BFAR : %08xh\n", reg_dump.BFAR);
        printk("AFSR : %08xh\n", reg_dump.AFSR);
        printk("MFAR : %08xh\n", reg_dump.MFAR);
}

//==============================================================================
/**
 * @brief Hard Fault ISR
 */
//==============================================================================
void HardFault_Handler(void)
{
        __asm volatile
        (
                " tst lr, #4                                                \n"
                " ite eq                                                    \n"
                " mrseq r0, msp                                             \n"
                " mrsne r0, psp                                             \n"
                " ldr r1, [r0, #24]                                         \n"
                " b get_registers_from_stack                                \n"
        );
}

//==============================================================================
/**
 * @brief Memory Management failure ISR
 */
//==============================================================================
void MemManage_Handler(void)
{
        _kernel_panic_report_from_ISR(_KERNEL_PANIC_DESC_CAUSE_CPUFAULT);
}

//==============================================================================
/**
 * @brief Bus Fault ISR
 */
//==============================================================================
void BusFault_Handler(void)
{
        _kernel_panic_report_from_ISR(_KERNEL_PANIC_DESC_CAUSE_CPUFAULT);
}

//==============================================================================
/**
 * @brief Usage Fault ISR
 */
//==============================================================================
void UsageFault_Handler(void)
{
        _kernel_panic_report_from_ISR(_KERNEL_PANIC_DESC_CAUSE_CPUFAULT);
}

//==============================================================================
/**
 * @brief  Function dump registers stored in stack.
 *
 * @param  stack_address        stack address
 */
//==============================================================================
void get_registers_from_stack(uint32_t *stack_address)
{
        reg_dump.R0   = stack_address[0];
        reg_dump.R1   = stack_address[1];
        reg_dump.R2   = stack_address[2];
        reg_dump.R3   = stack_address[3];
        reg_dump.R12  = stack_address[4];
        reg_dump.LR   = stack_address[5];
        reg_dump.PC   = stack_address[6];
        reg_dump.PSR  = stack_address[7];
        reg_dump.MFSR = NVIC_MFSR;
        reg_dump.BFSR = NVIC_BFSR;
        reg_dump.UFSR = NVIC_UFSR;
        reg_dump.HFSR = NVIC_HFSR;
        reg_dump.DFSR = NVIC_DFSR;
        reg_dump.BFAR = NVIC_BFAR;
        reg_dump.AFSR = NVIC_AFSR;
        reg_dump.MFAR = NVIC_MFAR;

        _kernel_panic_report_from_ISR(_KERNEL_PANIC_DESC_CAUSE_SEGFAULT);
}

/*==============================================================================
  End of file
==============================================================================*/
