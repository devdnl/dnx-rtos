/*=========================================================================*//**
@file    pll.c

@author  Daniel Zorychta

@brief   File support PLL

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

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Include files
==============================================================================*/
#include "system/dnxmodule.h"
#include "stm32f1/pll_cfg.h"
#include "stm32f1/pll_def.h"
#include "stm32f1/stm32f10x.h"
#include "stm32f1/lib/stm32f10x_rcc.h"
#include "stm32f1/cpuctl.h"
#include "system/timer.h"

/*==============================================================================
  Local symbolic constants/macros
==============================================================================*/
#define TIMEOUT_MS                      100

/*==============================================================================
  Local types, enums definitions
==============================================================================*/

/*==============================================================================
  Local function prototypes
==============================================================================*/
static void     set_flash_latency       (u32_t latency);
static void     enable_prefetch_buffer  (void);
static stdret_t wait_for_flag           (u32_t flag);
static bool     is_APB1_divided         (void);
static bool     is_APB2_divided         (void);

/*==============================================================================
  Local object definitions
==============================================================================*/

/*==============================================================================
  Exported object definitions
==============================================================================*/

/*==============================================================================
  Function definitions
==============================================================================*/

//==============================================================================
/**
 * @brief Initialize device
 *
 * @param[out]          **device_handle        device allocated memory
 * @param[in ]            major                major device number
 * @param[in ]            minor                minor device number
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
API_MOD_INIT(PLL, void **device_handle, u8_t major, u8_t minor)
{
        UNUSED_ARG(device_handle);
        UNUSED_ARG(major);
        UNUSED_ARG(minor);
        UNUSED_ARG(_module_name_);

        RCC_DeInit();

        set_flash_latency(_PLL_CFG__FLASH_LATENCY);
        enable_prefetch_buffer();

        if (_PLL_CFG__LSI_ON) {
                RCC_LSICmd(_PLL_CFG__LSI_ON);
                if (wait_for_flag(RCC_FLAG_LSIRDY) == STD_RET_ERROR)
                        goto exit_error;
        }

        if (_PLL_CFG__LSE_ON) {
                RCC_LSEConfig(_PLL_CFG__LSE_ON);
                if (wait_for_flag(RCC_FLAG_LSERDY) == STD_RET_ERROR)
                        goto exit_error;
        }

        if (_PLL_CFG__HSE_ON) {
                RCC_HSEConfig(_PLL_CFG__HSE_ON);
                if (wait_for_flag(RCC_FLAG_HSERDY) == STD_RET_ERROR)
                        goto exit_error;
        }

        RCC_RTCCLKConfig(_PLL_CFG__RTCCLK_SRC);

#ifdef STM32F10X_CL
        RCC_PREDIV2Config(_PLL_CFG__PLL_PREDIV2_VAL);

        if (_PLL_CFG__PLL2_ON) {
                RCC_PLL2Config(_PLL_CFG__PLL2_MUL);
                RCC_PLL2Cmd(_PLL_CFG__PLL2_ON);
                if (wait_for_flag(RCC_FLAG_PLL2RDY) == STD_RET_ERROR)
                        goto exit_error;
        }

        if (_PLL_CFG__PLL3_ON) {
                RCC_PLL3Config(_PLL_CFG__PLL3_MUL);
                RCC_PLL3Cmd(_PLL_CFG__PLL3_ON);
                if (wait_for_flag(RCC_FLAG_PLL3RDY) == STD_RET_ERROR)
                        goto exit_error;
        }

        RCC_I2S2CLKConfig(_PLL_CFG__I2S2_SRC);
        RCC_I2S3CLKConfig(_PLL_CFG__I2S3_SRC);

        RCC_PREDIV1Config(_PLL_CFG__PREDIV1_SRC, _PLL_CFG__PREDIV1_VAL);
        RCC_OTGFSCLKConfig(_PLL_CFG__USB_DIV);
#else
        RCC_USBCLKConfig(_PLL_CFG__USB_DIV);
#endif

        RCC_PLLConfig(_PLL_CFG__PLL_SRC, _PLL_CFG__PLL_MUL);
        RCC_PLLCmd(_PLL_CFG__PLL_ON);
        if (wait_for_flag(RCC_FLAG_PLLRDY) == STD_RET_ERROR)
                goto exit_error;

        RCC_ADCCLKConfig(_PLL_CFG__ADC_PRE);
        RCC_PCLK2Config(_PLL_CFG__APB2_PRE);
        RCC_PCLK1Config(_PLL_CFG__APB1_PRE);
        RCC_HCLKConfig(_PLL_CFG__AHB_PRE);

        RCC_SYSCLKConfig(_PLL_CFG__SYSCLK_SRC);
        RCC_MCOConfig(_PLL_CFG__MCO_SRC);

        _cpuctl_update_system_clocks();

        return STD_RET_OK;

exit_error:
        errno = EIO;
        return STD_RET_ERROR;

//        (void) _module_name_;
//
//        u32_t wait;
//
//        /* turn on HSE oscillator */
//        RCC->CR |= RCC_CR_HSEON;
//
//        /* waiting for HSE ready */
//        wait = UINT32_MAX;
//        while (!(RCC->CR & RCC_CR_HSERDY) && wait) {
//                wait--;
//        }
//
//        if (wait == 0) {
//                errno = ETIME;
//                return STD_RET_ERROR;
//        }
//
//#define CONFIG_CPU_TARGET_FREQ 72000000
//        /* wait states */
//        if (CONFIG_CPU_TARGET_FREQ <= 24000000UL)
//                FLASH->ACR |= (0x00 & FLASH_ACR_LATENCY);
//        else if (CONFIG_CPU_TARGET_FREQ <= 48000000UL)
//                FLASH->ACR |= (0x01 & FLASH_ACR_LATENCY);
//        else if (CONFIG_CPU_TARGET_FREQ <= 72000000UL)
//                FLASH->ACR |= (0x02 & FLASH_ACR_LATENCY);
//        else
//                FLASH->ACR |= (0x03 & FLASH_ACR_LATENCY);
//
//        /* AHB prescaler  configuration (/1) */
//        RCC->CFGR |= RCC_CFGR_HPRE_DIV1;
//
//        /* APB1 prescaler configuration (/2) */
//        RCC->CFGR |= RCC_CFGR_PPRE1_DIV2;
//
//        /* APB2 prescaler configuration (/1) */
//        RCC->CFGR |= RCC_CFGR_PPRE2_DIV1;
//
//        /* FCLK cortex free running clock */
//        SysTick->CTRL |= SysTick_CTRL_CLKSOURCE;
//
//        /* PLL source - HSE; PREDIV1 = 1; PLL x9 */
//        RCC->CFGR2 |= RCC_CFGR2_PREDIV1SRC_HSE | RCC_CFGR2_PREDIV1_DIV1;
//        RCC->CFGR  |= RCC_CFGR_PLLSRC_PREDIV1  | RCC_CFGR_PLLMULL9;
//
//        /* OTG USB set to 48 MHz (72*2 / 3)*/
//        RCC->CFGR &= ~RCC_CFGR_OTGFSPRE;
//
//        /* I2S3 and I2S2 from SYSCLK */
//        RCC->CFGR2 &= ~(RCC_CFGR2_I2S3SRC | RCC_CFGR2_I2S2SRC);
//
//        /* enable PLL */
//        RCC->CR |= RCC_CR_PLLON;
//
//        /* waiting for PLL ready */
//        wait = UINT32_MAX;
//        while (!(RCC->CR & RCC_CR_PLLRDY) && wait) {
//                wait--;
//        }
//
//        if (wait == 0) {
//                errno = ETIME;
//                return STD_RET_ERROR;
//        }
//
//        /* set PLL as system clock */
//        RCC->CFGR |= RCC_CFGR_SW_PLL;
//
//        wait = UINT32_MAX;
//        while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL) {
//                wait--;
//        }
//
//        if (wait == 0) {
//                errno = ETIME;
//                return STD_RET_ERROR;
//        }
//
//        _cpuctl_update_system_clocks();
//
//        return STD_RET_OK;
}

//==============================================================================
/**
 * @brief Release device
 *
 * @param[in ]          *device_handle          device allocated memory
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
API_MOD_RELEASE(PLL, void *device_handle)
{
        UNUSED_ARG(device_handle);

        return STD_RET_OK;
}

//==============================================================================
/**
 * @brief Open device
 *
 * @param[in ]          *device_handle          device allocated memory
 * @param[in ]           flags                  file operation flags (O_RDONLY, O_WRONLY, O_RDWR)
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
API_MOD_OPEN(PLL, void *device_handle, int flags)
{
        UNUSED_ARG(device_handle);
        UNUSED_ARG(flags);

        return STD_RET_OK;
}

//==============================================================================
/**
 * @brief Close device
 *
 * @param[in ]          *device_handle          device allocated memory
 * @param[in ]           force                  device force close (true)
 * @param[in ]          *opened_by_task         task with opened this device (valid only if force is true)
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
API_MOD_CLOSE(PLL, void *device_handle, bool force, const task_t *opened_by_task)
{
        UNUSED_ARG(device_handle);
        UNUSED_ARG(force);
        UNUSED_ARG(opened_by_task);

        return STD_RET_OK;
}

//==============================================================================
/**
 * @brief Write data to device
 *
 * @param[in ]          *device_handle          device allocated memory
 * @param[in ]          *src                    data source
 * @param[in ]           count                  number of bytes to write
 * @param[in ][out]     *fpos                   file position
 *
 * @return number of written bytes, -1 if error
 */
//==============================================================================
API_MOD_WRITE(PLL, void *device_handle, const u8_t *src, size_t count, u64_t *fpos)
{
        UNUSED_ARG(device_handle);
        UNUSED_ARG(src);
        UNUSED_ARG(count);
        UNUSED_ARG(fpos);

        errno = EPERM;

        return -1;
}

//==============================================================================
/**
 * @brief Read data from device
 *
 * @param[in ]          *device_handle          device allocated memory
 * @param[out]          *dst                    data destination
 * @param[in ]           count                  number of bytes to read
 * @param[in ][out]     *fpos                   file position
 *
 * @return number of read bytes, -1 if error
 */
//==============================================================================
API_MOD_READ(PLL, void *device_handle, u8_t *dst, size_t count, u64_t *fpos)
{
        UNUSED_ARG(device_handle);
        UNUSED_ARG(dst);
        UNUSED_ARG(count);
        UNUSED_ARG(fpos);

        errno = EPERM;

        return -1;
}

//==============================================================================
/**
 * @brief IO control
 *
 * @param[in ]          *device_handle          device allocated memory
 * @param[in ]           request                request
 * @param[in ][out]     *arg                    request's argument
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
API_MOD_IOCTL(PLL, void *device_handle, int request, void *arg)
{
        STOP_IF(!device_handle);

        stdret_t status = STD_RET_OK;

        if (arg) {
                RCC_ClocksTypeDef freq;
                RCC_GetClocksFreq(&freq);

                switch (request) {
                case PLL_IORQ_GET_SYSCLK_FREQ:
                        *(u32_t *)arg = freq.SYSCLK_Frequency;
                        break;

                case PLL_IORQ_GET_HCLK_FREQ:
                        *(u32_t *)arg = freq.HCLK_Frequency;
                        break;

                case PLL_IORQ_GET_PCLK1_FREQ:
                        *(u32_t *)arg = freq.PCLK1_Frequency;
                        break;

                case PLL_IORQ_GET_PCLK2_FREQ:
                        *(u32_t *)arg = freq.PCLK2_Frequency;
                        break;

                case PLL_IORQ_GET_ADCCLK_FREQ:
                        *(u32_t *)arg = freq.ADCCLK_Frequency;
                        break;

                case PLL_IORQ_GET_PCLK1_TIM_FREQ:
                        if (is_APB1_divided()) {
                                *(u32_t *)arg = freq.PCLK1_Frequency * 2;
                        } else {
                                *(u32_t *)arg = freq.PCLK1_Frequency;
                        }
                        break;

                case PLL_IORQ_GET_PCLK2_TIM_FREQ:
                        if (is_APB2_divided()) {
                                *(u32_t *)arg = freq.PCLK2_Frequency * 2;
                        } else {
                                *(u32_t *)arg = freq.PCLK2_Frequency;
                        }
                        break;

                default:
                        errno  = EBADRQC;
                        status = STD_RET_ERROR;
                        break;
                }
        } else {
                errno  = EINVAL;
                status = STD_RET_ERROR;
        }

        return status;
}

//==============================================================================
/**
 * @brief Flush device
 *
 * @param[in ]          *device_handle          device allocated memory
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
API_MOD_FLUSH(PLL, void *device_handle)
{
        UNUSED_ARG(device_handle);

        return STD_RET_OK;
}

//==============================================================================
/**
 * @brief Device information
 *
 * @param[in ]          *device_handle          device allocated memory
 * @param[out]          *device_stat            device status
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
API_MOD_STAT(PLL, void *device_handle, struct vfs_dev_stat *device_stat)
{
        UNUSED_ARG(device_handle);

        device_stat->st_size  = 0;
        device_stat->st_major = 0;
        device_stat->st_minor = 0;

        return STD_RET_OK;
}

//==============================================================================
/**
 * @brief Function set flash latency (wait states)
 *
 * @param latency
 */
//==============================================================================
static void set_flash_latency(u32_t latency)
{
        CLEAR_BIT(FLASH->ACR, FLASH_ACR_LATENCY);
        SET_BIT(FLASH->ACR, latency & FLASH_ACR_LATENCY);
}

//==============================================================================
/**
 * @brief Function enable prefetch buffer
 */
//==============================================================================
static void enable_prefetch_buffer(void)
{
        SET_BIT(FLASH->ACR, FLASH_ACR_PRFTBE);
}

//==============================================================================
/**
 * @brief Wait for flag set
 *
 * @param flag          flag
 *
 * @return STD_RET_OK if success, STD_RET_ERROR on error
 */
//==============================================================================
static stdret_t wait_for_flag(u32_t flag)
{
        timer_t timer = timer_reset();
        while (RCC_GetFlagStatus(flag) == RESET) {
                if (timer_is_expired(timer, TIMEOUT_MS))
                        return STD_RET_ERROR;
        }

        return STD_RET_OK;
}

//==============================================================================
/**
 * @brief Function return APB1 divider
 *
 * @return true if APB1 is divided, false if not
 */
//==============================================================================
static bool is_APB1_divided(void)
{
        return (RCC->CFGR & RCC_CFGR_PPRE1_2);
}

//==============================================================================
/**
 * @brief Function return APB2 divider
 *
 * @return true if APB2 is divided, false if not
 */
//==============================================================================
static bool is_APB2_divided(void)
{
        return (RCC->CFGR & RCC_CFGR_PPRE2_2);
}

#ifdef __cplusplus
}
#endif

/*==============================================================================
  End of file
==============================================================================*/
