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
#include "stm32f7/cpuctl.h"
#include "stm32f7/stm32f7xx.h"
#include "stm32f7/lib/stm32f7xx_ll_rcc.h"
#include "stm32f7/lib/misc.h"
#include "kernel/kwrapper.h"
#include "kernel/kpanic.h"
#include "kernel/sysfunc.h"

/*==============================================================================
  Local symbolic constants/macros
==============================================================================*/
/* Cortex System Control register address */
#define SCB_SysCtrl             (*((__IO uint32_t *)0xE000ED10))
#define SysCtrl_SLEEPDEEP       ((uint32_t)0x00000004)

#define SRAM1_HEAP_START        ((void *)&__heap_start)
#define SRAM1_HEAP_SIZE         ((size_t)&__heap_size)

#define SRAM2_START             ((void *)&__ram2_start)
#define SRAM2_SIZE              ((size_t)&__ram2_size)

#define DTCM_START              ((void *)&__dtcm_start)
#define DTCM_SIZE               ((size_t)&__dtcm_size)

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

/*
 * MPU Definitions
 */
#define MPU_HFNMI_PRIVDEF_NONE          ((uint32_t)0x00000000U)
#define MPU_HARDFAULT_NMI               ((uint32_t)0x00000002U)
#define MPU_PRIVILEGED_DEFAULT          ((uint32_t)0x00000004U)
#define MPU_HFNMI_PRIVDEF               ((uint32_t)0x00000006U)

#define MPU_REGION_ENABLE               ((uint8_t)0x01U)
#define MPU_REGION_DISABLE              ((uint8_t)0x00U)

#define MPU_INSTRUCTION_ACCESS_ENABLE   ((uint8_t)0x00U)
#define MPU_INSTRUCTION_ACCESS_DISABLE  ((uint8_t)0x01U)

#define MPU_ACCESS_SHAREABLE            ((uint8_t)0x01U)
#define MPU_ACCESS_NOT_SHAREABLE        ((uint8_t)0x00U)

#define MPU_ACCESS_CACHEABLE            ((uint8_t)0x01U)
#define MPU_ACCESS_NOT_CACHEABLE        ((uint8_t)0x00U)

#define MPU_ACCESS_BUFFERABLE           ((uint8_t)0x01U)
#define MPU_ACCESS_NOT_BUFFERABLE       ((uint8_t)0x00U)

#define MPU_TEX_LEVEL0                  ((uint8_t)0x00U)
#define MPU_TEX_LEVEL1                  ((uint8_t)0x01U)
#define MPU_TEX_LEVEL2                  ((uint8_t)0x02U)

#define MPU_REGION_SIZE_32B             ((uint8_t)0x04U)
#define MPU_REGION_SIZE_64B             ((uint8_t)0x05U)
#define MPU_REGION_SIZE_128B            ((uint8_t)0x06U)
#define MPU_REGION_SIZE_256B            ((uint8_t)0x07U)
#define MPU_REGION_SIZE_512B            ((uint8_t)0x08U)
#define MPU_REGION_SIZE_1KiB            ((uint8_t)0x09U)
#define MPU_REGION_SIZE_2KiB            ((uint8_t)0x0AU)
#define MPU_REGION_SIZE_4KiB            ((uint8_t)0x0BU)
#define MPU_REGION_SIZE_8KiB            ((uint8_t)0x0CU)
#define MPU_REGION_SIZE_16KiB           ((uint8_t)0x0DU)
#define MPU_REGION_SIZE_32KiB           ((uint8_t)0x0EU)
#define MPU_REGION_SIZE_64KiB           ((uint8_t)0x0FU)
#define MPU_REGION_SIZE_128KiB          ((uint8_t)0x10U)
#define MPU_REGION_SIZE_256KiB          ((uint8_t)0x11U)
#define MPU_REGION_SIZE_512KiB          ((uint8_t)0x12U)
#define MPU_REGION_SIZE_1MiB            ((uint8_t)0x13U)
#define MPU_REGION_SIZE_2MiB            ((uint8_t)0x14U)
#define MPU_REGION_SIZE_4MiB            ((uint8_t)0x15U)
#define MPU_REGION_SIZE_8MiB            ((uint8_t)0x16U)
#define MPU_REGION_SIZE_16MiB           ((uint8_t)0x17U)
#define MPU_REGION_SIZE_32MiB           ((uint8_t)0x18U)
#define MPU_REGION_SIZE_64MiB           ((uint8_t)0x19U)
#define MPU_REGION_SIZE_128MiB          ((uint8_t)0x1AU)
#define MPU_REGION_SIZE_256MiB          ((uint8_t)0x1BU)
#define MPU_REGION_SIZE_512MiB          ((uint8_t)0x1CU)
#define MPU_REGION_SIZE_1GiB            ((uint8_t)0x1DU)
#define MPU_REGION_SIZE_2GiB            ((uint8_t)0x1EU)
#define MPU_REGION_SIZE_4GiB            ((uint8_t)0x1FU)

#define MPU_REGION_NO_ACCESS            ((uint8_t)0x00U)
#define MPU_REGION_PRIV_RW              ((uint8_t)0x01U)
#define MPU_REGION_PRIV_RW_URO          ((uint8_t)0x02U)
#define MPU_REGION_FULL_ACCESS          ((uint8_t)0x03U)
#define MPU_REGION_PRIV_RO              ((uint8_t)0x05U)
#define MPU_REGION_PRIV_RO_URO          ((uint8_t)0x06U)

#define MPU_REGION_NUMBER0              ((uint8_t)0x00U)
#define MPU_REGION_NUMBER1              ((uint8_t)0x01U)
#define MPU_REGION_NUMBER2              ((uint8_t)0x02U)
#define MPU_REGION_NUMBER3              ((uint8_t)0x03U)
#define MPU_REGION_NUMBER4              ((uint8_t)0x04U)
#define MPU_REGION_NUMBER5              ((uint8_t)0x05U)
#define MPU_REGION_NUMBER6              ((uint8_t)0x06U)
#define MPU_REGION_NUMBER7              ((uint8_t)0x07U)

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

typedef struct
{
        uint8_t  enable;                /*!< Specifies the status of the region.*/
        uint8_t  number;                /*!< Specifies the number of the region to protect.*/
        uint32_t base_address;          /*!< Specifies the base address of the region to protect.*/
        uint8_t  size;                  /*!< Specifies the size of the region to protect.*/
        uint8_t  sub_region_disable;    /*!< Specifies the number of the subregion protection to disable.*/
        uint8_t  type_ext_field;        /*!< Specifies the TEX field level.*/
        uint8_t  access_permission;     /*!< Specifies the region access permission type.*/
        uint8_t  disable_exec;          /*!< Specifies the instruction access status.*/
        uint8_t  is_shareable;          /*!< Specifies the shareability status of the protected region.*/
        uint8_t  is_cacheable;          /*!< Specifies the cacheable status of the region protected.*/
        uint8_t  is_bufferable;         /*!< Specifies the bufferable status of the protected region.*/
} MPU_region_t;

/*==============================================================================
  Local function prototypes
==============================================================================*/
void get_registers_from_stack(uint32_t *stack_address);

#if __CPU_RAM1_RAM2_CACHE_DISABLE__ == _YES_
static void MPU_disable(void);
static void MPU_enable(uint32_t MPU_Control);
static void MPU_conf_region(MPU_region_t *mpu_region);
#endif

/*==============================================================================
  Local object definitions
==============================================================================*/
extern void *__heap_start;
extern void *__heap_size;
extern void *__ram2_start;
extern void *__ram2_size;
extern void *__dtcm_start;
extern void *__dtcm_size;

static _mm_region_t sram1;
static _mm_region_t sram2;
static _mm_region_t dtcm;

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

        #if __CPU_RAM1_RAM2_CACHE_DISABLE__ == _YES_
        MPU_disable();
        MPU_region_t mpu;
        mpu.enable = MPU_REGION_ENABLE;
        mpu.access_permission = MPU_REGION_FULL_ACCESS;
        mpu.base_address = 0x20010000; // SRAM1 start
        mpu.disable_exec = MPU_INSTRUCTION_ACCESS_ENABLE;
        mpu.type_ext_field = MPU_TEX_LEVEL1;
        mpu.is_bufferable = MPU_ACCESS_NOT_BUFFERABLE;
        mpu.is_cacheable = MPU_ACCESS_NOT_CACHEABLE;
        mpu.is_shareable = MPU_ACCESS_SHAREABLE;
        mpu.number = MPU_REGION_NUMBER0;
        mpu.size = MPU_REGION_SIZE_512KiB;
        mpu.sub_region_disable = 0;
        MPU_conf_region(&mpu);
        MPU_enable(MPU_PRIVILEGED_DEFAULT);
        #endif

        u32_t ram1_ram2_flags = (__CPU_RAM1_RAM2_CACHE_DISABLE__ ? 0 : _MM_FLAG__CACHEABLE)
                              | _MM_FLAG__DMA_CAPABLE;

        _mm_register_region(&sram1, SRAM1_HEAP_START, SRAM1_HEAP_SIZE, ram1_ram2_flags, "SRAM1");
        _mm_register_region(&sram2, SRAM2_START, SRAM2_SIZE, ram1_ram2_flags, "SRAM2");
        _mm_register_region(&dtcm, DTCM_START, DTCM_SIZE, _MM_FLAG__DMA_CAPABLE, _CPUCTL_FAST_MEM);

        if (__CPU_ICACHE_ENABLE__) {
                _ISR_disable();
                SCB_InvalidateICache();
                SCB_EnableICache();
                _ISR_enable();
                printk("CPU: ICACHE enabled");
        }

        if (__CPU_DCACHE_ENABLE__) {
                _ISR_disable();
                SCB_InvalidateDCache();
                SCB_EnableDCache();
                _ISR_enable();
                printk("CPU: DCACHE enabled");
        }

        static const char *FPU_TYPE[] = {"none", "single", "double"};
        printk("CPU: FPU type %s", FPU_TYPE[SCB_GetFPUType()]);
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
        PWR->CR2 |= PWR_CR2_CWUPF1 | PWR_CR2_CWUPF2 | PWR_CR2_CWUPF3
                  | PWR_CR2_CWUPF4 | PWR_CR2_CWUPF5 | PWR_CR2_CWUPF6;

        /* Select STANDBY mode */
        PWR->CR1 |= PWR_CR1_PDDS;

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
 *
 * @param  file         destination file
 */
//==============================================================================
void _cpuctl_print_exception(void *file)
{
        sys_fprintf(file, "R0   : %08xh\n", reg_dump.R0);
        sys_fprintf(file, "R1   : %08xh\n", reg_dump.R1);
        sys_fprintf(file, "R2   : %08xh\n", reg_dump.R2);
        sys_fprintf(file, "R3   : %08xh\n", reg_dump.R3);
        sys_fprintf(file, "R12  : %08xh\n", reg_dump.R12);
        sys_fprintf(file, "LR   : %08xh\n", reg_dump.LR);
        sys_fprintf(file, "PC   : %08xh\n", reg_dump.PC);
        sys_fprintf(file, "PSR  : %08xh\n", reg_dump.PSR);
        sys_fprintf(file, "MFSR : %08xh\n", reg_dump.MFSR);
        sys_fprintf(file, "BFSR : %08xh\n", reg_dump.BFSR);
        sys_fprintf(file, "UFSR : %08xh\n", reg_dump.UFSR);
        sys_fprintf(file, "HFSR : %08xh\n", reg_dump.HFSR);
        sys_fprintf(file, "DFSR : %08xh\n", reg_dump.DFSR);
        sys_fprintf(file, "BFAR : %08xh\n", reg_dump.BFAR);
        sys_fprintf(file, "AFSR : %08xh\n", reg_dump.AFSR);
        sys_fprintf(file, "MFAR : %08xh\n", reg_dump.MFAR);
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
                " ldr r2, handler2_address_const                            \n"
                " bx r2                                                     \n"
                " handler2_address_const: .word get_registers_from_stack    \n"
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

//==============================================================================
/**
 * @brief  Function clean/flush DCACHE.
 */
//==============================================================================
void _cpuctl_clean_dcache(void)
{
#if __DCACHE_PRESENT && __CPU_DCACHE_ENABLE__ && !__CPU_RAM1_RAM2_CACHE_DISABLE__
       SCB_CleanDCache();
#endif
}

//==============================================================================
/**
 * @brief  Function clean/flush and invalidate/reset DCACHE.
 */
//==============================================================================
void _cpuctl_clean_invalidate_dcache(void)
{
#if __DCACHE_PRESENT && __CPU_DCACHE_ENABLE__ && !__CPU_RAM1_RAM2_CACHE_DISABLE__
        SCB_CleanInvalidateDCache();
#endif
}

//==============================================================================
/**
 * @brief  Function invalidate/reset DCACHE.
 */
//==============================================================================
void _cpuctl_invalidate_dcache(void)
{
#if __DCACHE_PRESENT && __CPU_DCACHE_ENABLE__ && !__CPU_RAM1_RAM2_CACHE_DISABLE__
        SCB_InvalidateDCache();
#endif
}

//==============================================================================
/**
 * @brief  Function invalidate/reset DCACHE by address.
 *
 * @param  addr         address (must be aligned to 32 bytes)
 * @param  size         size in bytes
 */
//==============================================================================
void _cpuctl_invalidate_dcache_by_addr(u32_t *addr, u32_t size)
{
#if __DCACHE_PRESENT && __CPU_DCACHE_ENABLE__ && !__CPU_RAM1_RAM2_CACHE_DISABLE__
        SCB_InvalidateDCache_by_Addr(addr, size);
#else
        (void)addr;
        (void)size;
#endif
}

//==============================================================================
/**
 * @brief  Function clean/flush DCACHE by address.
 *
 * @param  addr         address (must be aligned to 32 bytes)
 * @param  size         size in bytes
 */
//==============================================================================
void _cpuctl_clean_dcache_by_addr(u32_t *addr, u32_t size)
{
#if __DCACHE_PRESENT && __CPU_DCACHE_ENABLE__ && !__CPU_RAM1_RAM2_CACHE_DISABLE__
        SCB_CleanDCache_by_Addr(addr, size);
#else
        (void)addr;
        (void)size;
#endif
}

//==============================================================================
/**
 * @brief  Function clean/flush and invalidate/reset DCACHE by address.
 *
 * @param  addr         address (must be aligned to 32 bytes)
 * @param  size         size in bytes
 */
//==============================================================================
void _cpuctl_clean_invalidate_dcache_by_addr(u32_t *addr, u32_t size)
{
#if __DCACHE_PRESENT && __CPU_DCACHE_ENABLE__ && !__CPU_RAM1_RAM2_CACHE_DISABLE__
        SCB_CleanInvalidateDCache_by_Addr(addr, size);
#else
        (void)addr;
        (void)size;
#endif
}

#if __CPU_RAM1_RAM2_CACHE_DISABLE__ == _YES_
//==============================================================================
/**
 * @brief  Disables the MPU
 */
//==============================================================================
static void MPU_disable(void)
{
        /* Make sure outstanding transfers are done */
        __DMB();

        /* Disable fault exceptions */
        SCB->SHCSR &= ~SCB_SHCSR_MEMFAULTENA_Msk;

        /* Disable the MPU and clear the control register*/
        MPU->CTRL = 0;
}

//==============================================================================
/**
 * @brief  Enables the MPU
 * @param  MPU_Control Specifies the control mode of the MPU during hard fault,
 *         NMI, FAULTMASK and privileged access to the default memory
 *         This parameter can be one of the following values:
 *         @arg MPU_HFNMI_PRIVDEF_NONE
 *         @arg MPU_HARDFAULT_NMI
 *         @arg MPU_PRIVILEGED_DEFAULT
 *         @arg MPU_HFNMI_PRIVDEF
 * @retval None
 */
//==============================================================================
static void MPU_enable(uint32_t MPU_Control)
{
        /* Enable the MPU */
        MPU->CTRL = MPU_Control | MPU_CTRL_ENABLE_Msk;

        /* Enable fault exceptions */
        SCB->SHCSR |= SCB_SHCSR_MEMFAULTENA_Msk;

        /* Ensure MPU setting take effects */
        __DSB();
        __ISB();
}

//==============================================================================
/**
 * @brief  Initializes and configures the Region and the memory to be protected.
 * @param  MPU_Init Pointer to a MPU_Region_InitTypeDef structure that contains
 *                the initialization and configuration information.
 */
//==============================================================================
static void MPU_conf_region(MPU_region_t *mpu_region)
{
        MPU->RNR = mpu_region->number;

        if (mpu_region->enable) {
                MPU->RBAR = mpu_region->base_address;
                MPU->RASR = ((uint32_t)mpu_region->disable_exec << MPU_RASR_XN_Pos)
                          | ((uint32_t)mpu_region->access_permission << MPU_RASR_AP_Pos)
                          | ((uint32_t)mpu_region->type_ext_field << MPU_RASR_TEX_Pos)
                          | ((uint32_t)mpu_region->is_shareable << MPU_RASR_S_Pos)
                          | ((uint32_t)mpu_region->is_cacheable << MPU_RASR_C_Pos)
                          | ((uint32_t)mpu_region->is_bufferable << MPU_RASR_B_Pos)
                          | ((uint32_t)mpu_region->sub_region_disable << MPU_RASR_SRD_Pos)
                          | ((uint32_t)mpu_region->size << MPU_RASR_SIZE_Pos)
                          | ((uint32_t)mpu_region->enable << MPU_RASR_ENABLE_Pos);
        } else {
                MPU->RBAR = 0x00;
                MPU->RASR = 0x00;
        }
}
#endif

/*==============================================================================
  End of file
==============================================================================*/
