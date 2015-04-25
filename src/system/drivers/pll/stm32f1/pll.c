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

/*==============================================================================
  Include files
==============================================================================*/
#include "drivers/driver.h"
#include "stm32f1/pll_cfg.h"
#include "stm32f1/pll_def.h"
#include "stm32f1/stm32f10x.h"
#include "stm32f1/lib/stm32f10x_rcc.h"

/*==============================================================================
  Local symbolic constants/macros
==============================================================================*/
#define TIMEOUT_MS                      250
#define LSE_TIMEOUT_MS                  5000

/*==============================================================================
  Local types, enums definitions
==============================================================================*/

/*==============================================================================
  Local function prototypes
==============================================================================*/
static void set_flash_latency       (u32_t latency);
static void enable_prefetch_buffer  (void);
static int  wait_for_flag           (u32_t flag, uint timeout);
static bool is_APB1_divided         (void);
static bool is_APB2_divided         (void);

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
 * @return One of errno value (errno.h)
 */
//==============================================================================
API_MOD_INIT(PLL, void **device_handle, u8_t major, u8_t minor)
{
        UNUSED_ARG(device_handle);
        UNUSED_ARG(major);
        UNUSED_ARG(minor);

        RCC_DeInit();

        set_flash_latency(_PLL_CFG__FLASH_LATENCY);
        enable_prefetch_buffer();

        int status;

        if (_PLL_CFG__LSI_ON) {
                RCC_LSICmd(_PLL_CFG__LSI_ON);
                status = wait_for_flag(RCC_FLAG_LSIRDY, TIMEOUT_MS);
                if (status != ESUCC)
                        return status;
        }

        if (_PLL_CFG__LSE_ON) {
                SET_BIT(RCC->APB1ENR, RCC_APB1ENR_PWREN | RCC_APB1ENR_BKPEN);
                SET_BIT(PWR->CR, PWR_CR_DBP);

                if (!(RCC->BDCR & RCC_BDCR_LSERDY)) {
                        RCC_LSEConfig(_PLL_CFG__LSE_ON);
                        wait_for_flag(RCC_FLAG_LSERDY, LSE_TIMEOUT_MS);
                        // this oscillator not causes an error because is not a main osc.
                }
        }

        if (_PLL_CFG__HSE_ON) {
                RCC_HSEConfig(_PLL_CFG__HSE_ON);
                status = wait_for_flag(RCC_FLAG_HSERDY, TIMEOUT_MS);
                if (status != ESUCC)
                        return status;
        }

        if (  (  (_PLL_CFG__RTCCLK_SRC == RCC_RTCCLKSource_LSE        && _PLL_CFG__LSE_ON)
              || (_PLL_CFG__RTCCLK_SRC == RCC_RTCCLKSource_LSI        && _PLL_CFG__LSI_ON)
              || (_PLL_CFG__RTCCLK_SRC == RCC_RTCCLKSource_HSE_Div128 && _PLL_CFG__HSE_ON) )
           && (RCC->BDCR & RCC_BDCR_RTCEN) == 0
           && (RCC->BDCR & (RCC_BDCR_RTCSEL_1 | RCC_BDCR_RTCSEL_0)) == RCC_BDCR_RTCSEL_NOCLOCK) {

                SET_BIT(RCC->APB1ENR, RCC_APB1ENR_PWREN | RCC_APB1ENR_BKPEN);
                SET_BIT(PWR->CR, PWR_CR_DBP);
                RCC_RTCCLKConfig(_PLL_CFG__RTCCLK_SRC);
        }

#ifdef STM32F10X_CL
        RCC_PREDIV2Config(_PLL_CFG__PLL_PREDIV2_VAL);

        if (_PLL_CFG__PLL2_ON) {
                RCC_PLL2Config(_PLL_CFG__PLL2_MUL);
                RCC_PLL2Cmd(_PLL_CFG__PLL2_ON);
                status = wait_for_flag(RCC_FLAG_PLL2RDY, TIMEOUT_MS);
                if (status != ESUCC)
                        return status;
        }

        if (_PLL_CFG__PLL3_ON) {
                RCC_PLL3Config(_PLL_CFG__PLL3_MUL);
                RCC_PLL3Cmd(_PLL_CFG__PLL3_ON);
                status = wait_for_flag(RCC_FLAG_PLL3RDY, TIMEOUT_MS);
                if (status != ESUCC)
                        return status;
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
        status = wait_for_flag(RCC_FLAG_PLLRDY, TIMEOUT_MS);
        if (status != ESUCC)
                return status;

        RCC_ADCCLKConfig(_PLL_CFG__ADC_PRE);
        RCC_PCLK2Config(_PLL_CFG__APB2_PRE);
        RCC_PCLK1Config(_PLL_CFG__APB1_PRE);
        RCC_HCLKConfig(_PLL_CFG__AHB_PRE);

        RCC_SYSCLKConfig(_PLL_CFG__SYSCLK_SRC);
        RCC_MCOConfig(_PLL_CFG__MCO_SRC);

        _sys_update_system_clocks();

        return ESUCC;
}

//==============================================================================
/**
 * @brief Release device
 *
 * @param[in ]          *device_handle          device allocated memory
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
API_MOD_RELEASE(PLL, void *device_handle)
{
        UNUSED_ARG(device_handle);

        RCC_DeInit();
        _sys_update_system_clocks();

        return ESUCC;
}

//==============================================================================
/**
 * @brief Open device
 *
 * @param[in ]          *device_handle          device allocated memory
 * @param[in ]           flags                  file operation flags (O_RDONLY, O_WRONLY, O_RDWR)
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
API_MOD_OPEN(PLL, void *device_handle, u32_t flags)
{
        UNUSED_ARG(device_handle);
        UNUSED_ARG(flags);

        return ESUCC;
}

//==============================================================================
/**
 * @brief Close device
 *
 * @param[in ]          *device_handle          device allocated memory
 * @param[in ]           force                  device force close (true)
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
API_MOD_CLOSE(PLL, void *device_handle, bool force)
{
        UNUSED_ARG(device_handle);
        UNUSED_ARG(force);

        return ESUCC;
}

//==============================================================================
/**
 * @brief Write data to device
 *
 * @param[in ]          *device_handle          device allocated memory
 * @param[in ]          *src                    data source
 * @param[in ]           count                  number of bytes to write
 * @param[in ][out]     *fpos                   file position
 * @param[out]          *wrcnt                  number of written bytes
 * @param[in ]           fattr                  file attributes
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
API_MOD_WRITE(PLL,
              void             *device_handle,
              const u8_t       *src,
              size_t            count,
              fpos_t           *fpos,
              size_t           *wrcnt,
              struct vfs_fattr  fattr)
{
        UNUSED_ARG(device_handle);
        UNUSED_ARG(src);
        UNUSED_ARG(count);
        UNUSED_ARG(fpos);
        UNUSED_ARG(wrcnt);
        UNUSED_ARG(fattr);

        return ENOTSUP;
}

//==============================================================================
/**
 * @brief Read data from device
 *
 * @param[in ]          *device_handle          device allocated memory
 * @param[out]          *dst                    data destination
 * @param[in ]           count                  number of bytes to read
 * @param[in ][out]     *fpos                   file position
 * @param[out]          *rdcnt                  number of read bytes
 * @param[in ]           fattr                  file attributes
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
API_MOD_READ(PLL,
             void            *device_handle,
             u8_t            *dst,
             size_t           count,
             fpos_t          *fpos,
             size_t          *rdcnt,
             struct vfs_fattr fattr)
{
        UNUSED_ARG(device_handle);
        UNUSED_ARG(dst);
        UNUSED_ARG(count);
        UNUSED_ARG(fpos);
        UNUSED_ARG(rdcnt);
        UNUSED_ARG(fattr);

        return ENOTSUP;
}

//==============================================================================
/**
 * @brief IO control
 *
 * @param[in ]          *device_handle          device allocated memory
 * @param[in ]           request                request
 * @param[in ][out]     *arg                    request's argument
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
API_MOD_IOCTL(PLL, void *device_handle, int request, void *arg)
{
        UNUSED_ARG(device_handle);

        int status = ESUCC;

        if (arg) {
                RCC_ClocksTypeDef freq;
                RCC_GetClocksFreq(&freq);

                switch (request) {
                case IOCTL_PLL__GET_SYSCLK_FREQ:
                        *(u32_t *)arg = freq.SYSCLK_Frequency;
                        break;

                case IOCTL_PLL__GET_HCLK_FREQ:
                        *(u32_t *)arg = freq.HCLK_Frequency;
                        break;

                case IOCTL_PLL__GET_PCLK1_FREQ:
                        *(u32_t *)arg = freq.PCLK1_Frequency;
                        break;

                case IOCTL_PLL__GET_PCLK2_FREQ:
                        *(u32_t *)arg = freq.PCLK2_Frequency;
                        break;

                case IOCTL_PLL__GET_ADCCLK_FREQ:
                        *(u32_t *)arg = freq.ADCCLK_Frequency;
                        break;

                case IOCTL_PLL__GET_PCLK1_TIM_FREQ:
                        if (is_APB1_divided()) {
                                *(u32_t *)arg = freq.PCLK1_Frequency * 2;
                        } else {
                                *(u32_t *)arg = freq.PCLK1_Frequency;
                        }
                        break;

                case IOCTL_PLL__GET_PCLK2_TIM_FREQ:
                        if (is_APB2_divided()) {
                                *(u32_t *)arg = freq.PCLK2_Frequency * 2;
                        } else {
                                *(u32_t *)arg = freq.PCLK2_Frequency;
                        }
                        break;

                default:
                        status = EBADRQC;
                        break;
                }
        } else {
                status = EINVAL;
        }

        return status;
}

//==============================================================================
/**
 * @brief Flush device
 *
 * @param[in ]          *device_handle          device allocated memory
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
API_MOD_FLUSH(PLL, void *device_handle)
{
        UNUSED_ARG(device_handle);

        return ESUCC;
}

//==============================================================================
/**
 * @brief Device information
 *
 * @param[in ]          *device_handle          device allocated memory
 * @param[out]          *device_stat            device status
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
API_MOD_STAT(PLL, void *device_handle, struct vfs_dev_stat *device_stat)
{
        UNUSED_ARG(device_handle);

        device_stat->st_size  = 0;
        device_stat->st_major = 0;
        device_stat->st_minor = 0;

        return ESUCC;
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
 * ERRNO: EIO
 *
 * @param flag          flag
 *
 * @return ESUCC if success, ETIME on error
 */
//==============================================================================
static int wait_for_flag(u32_t flag, uint timeout)
{
        uint timer = _sys_time_get_reference();
        while (RCC_GetFlagStatus(flag) == RESET) {
                if (_sys_time_is_expired(timer, timeout)) {
                        return ETIME;
                }
        }

        return ESUCC;
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

/*==============================================================================
  End of file
==============================================================================*/
