/*=========================================================================*//**
@file    clk.c

@author  Daniel Zorychta

@brief   File support system clokc (PLL).

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
#include "drivers/driver.h"
#include "stm32f1/clk_cfg.h"
#include "stm32f1/stm32f10x.h"
#include "stm32f1/lib/stm32f10x_rcc.h"
#include "../clk_ioctl.h"

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
static int  wait_for_flag           (u32_t flag, u32_t timeout);
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
 * @param[in ]            config               optional module configuration
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
API_MOD_INIT(CLK, void **device_handle, u8_t major, u8_t minor, const void *config)
{
        UNUSED_ARG4(device_handle, major, minor, config);

        RCC_DeInit();

        set_flash_latency(_CLK_CFG__FLASH_LATENCY);
        enable_prefetch_buffer();

        int err = ESUCC;

        if (_CLK_CFG__LSI_ON) {
                RCC_LSICmd(_CLK_CFG__LSI_ON);
                err = wait_for_flag(RCC_FLAG_LSIRDY, TIMEOUT_MS);
                if (err) {
                        printk("CLK: LSI timeout");
                        return err;
                }
        }

        if (_CLK_CFG__LSE_ON) {
                SET_BIT(RCC->APB1ENR, RCC_APB1ENR_PWREN | RCC_APB1ENR_BKPEN);
                SET_BIT(PWR->CR, PWR_CR_DBP);

                if (!(RCC->BDCR & RCC_BDCR_LSERDY)) {

                        RCC_LSEConfig(_CLK_CFG__LSE_ON);

                        if (_CLK_CFG__LSE_ON != RCC_LSE_Bypass) {
                                if (wait_for_flag(RCC_FLAG_LSERDY, LSE_TIMEOUT_MS) != ESUCC) {
                                        // this oscillator not causes an error because is not a main osc.
                                        printk("CLK: LSE timeout");
                                }
                        }
                }
        }

        if (_CLK_CFG__HSE_ON) {
                RCC_HSEConfig(_CLK_CFG__HSE_ON);
                err = wait_for_flag(RCC_FLAG_HSERDY, TIMEOUT_MS);
                if (err) {
                        printk("CLK: HSE timeout");
                        return err;
                }
        }

        if (  (  (_CLK_CFG__RTCCLK_SRC == RCC_RTCCLKSource_LSE        && _CLK_CFG__LSE_ON)
              || (_CLK_CFG__RTCCLK_SRC == RCC_RTCCLKSource_LSI        && _CLK_CFG__LSI_ON)
              || (_CLK_CFG__RTCCLK_SRC == RCC_RTCCLKSource_HSE_Div128 && _CLK_CFG__HSE_ON) )
           && (RCC->BDCR & RCC_BDCR_RTCEN) == 0
           && (RCC->BDCR & (RCC_BDCR_RTCSEL_1 | RCC_BDCR_RTCSEL_0)) == RCC_BDCR_RTCSEL_NOCLOCK) {

                SET_BIT(RCC->APB1ENR, RCC_APB1ENR_PWREN | RCC_APB1ENR_BKPEN);
                SET_BIT(PWR->CR, PWR_CR_DBP);
                RCC_RTCCLKConfig(_CLK_CFG__RTCCLK_SRC);
        }

#ifdef STM32F10X_CL
        RCC_PREDIV2Config(_CLK_CFG__PLL_PREDIV2_VAL);

        if (_CLK_CFG__PLL2_ON) {
                RCC_PLL2Config(_CLK_CFG__PLL2_MUL);
                RCC_PLL2Cmd(_CLK_CFG__PLL2_ON);
                err = wait_for_flag(RCC_FLAG_PLL2RDY, TIMEOUT_MS);
                if (err) {
                        printk("CLK: PLL2 timeout");
                        return err;
                }
        }

        if (_CLK_CFG__PLL3_ON) {
                RCC_PLL3Config(_CLK_CFG__PLL3_MUL);
                RCC_PLL3Cmd(_CLK_CFG__PLL3_ON);
                err = wait_for_flag(RCC_FLAG_PLL3RDY, TIMEOUT_MS);
                if (err) {
                        printk("CLK: PLL3 timeout");
                        return err;
                }
        }

        RCC_I2S2CLKConfig(_CLK_CFG__I2S2_SRC);
        RCC_I2S3CLKConfig(_CLK_CFG__I2S3_SRC);

        RCC_PREDIV1Config(_CLK_CFG__PREDIV1_SRC, _CLK_CFG__PREDIV1_VAL);
        RCC_OTGFSCLKConfig(_CLK_CFG__USB_DIV);
#else
        RCC_USBCLKConfig(_CLK_CFG__USB_DIV);
#endif

        RCC_PLLConfig(_CLK_CFG__PLL_SRC, _CLK_CFG__PLL_MUL);
        RCC_PLLCmd(_CLK_CFG__PLL_ON);
        if (_CLK_CFG__PLL_ON) {
                err = wait_for_flag(RCC_FLAG_PLLRDY, TIMEOUT_MS);
                if (err)  {
                        printk("CLK: PLL timeout");
                }
        }

        RCC_ADCCLKConfig(_CLK_CFG__ADC_PRE);
        RCC_PCLK2Config(_CLK_CFG__APB2_PRE);
        RCC_PCLK1Config(_CLK_CFG__APB1_PRE);
        RCC_HCLKConfig(_CLK_CFG__AHB_PRE);

        RCC_SYSCLKConfig(_CLK_CFG__SYSCLK_SRC);
        RCC_MCOConfig(_CLK_CFG__MCO_SRC);

        sys_update_system_clocks();

        return err;
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
API_MOD_RELEASE(CLK, void *device_handle)
{
        UNUSED_ARG1(device_handle);

        RCC_DeInit();
        sys_update_system_clocks();

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
API_MOD_OPEN(CLK, void *device_handle, u32_t flags)
{
        UNUSED_ARG1(device_handle);
        UNUSED_ARG1(flags);

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
API_MOD_CLOSE(CLK, void *device_handle, bool force)
{
        UNUSED_ARG1(device_handle);
        UNUSED_ARG1(force);

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
API_MOD_WRITE(CLK,
              void             *device_handle,
              const u8_t       *src,
              size_t            count,
              fpos_t           *fpos,
              size_t           *wrcnt,
              struct vfs_fattr  fattr)
{
        UNUSED_ARG1(device_handle);
        UNUSED_ARG1(src);
        UNUSED_ARG1(count);
        UNUSED_ARG1(fpos);
        UNUSED_ARG1(wrcnt);
        UNUSED_ARG1(fattr);

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
API_MOD_READ(CLK,
             void            *device_handle,
             u8_t            *dst,
             size_t           count,
             fpos_t          *fpos,
             size_t          *rdcnt,
             struct vfs_fattr fattr)
{
        UNUSED_ARG1(device_handle);
        UNUSED_ARG1(dst);
        UNUSED_ARG1(count);
        UNUSED_ARG1(fpos);
        UNUSED_ARG1(rdcnt);
        UNUSED_ARG1(fattr);

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
API_MOD_IOCTL(CLK, void *device_handle, int request, void *arg)
{
        UNUSED_ARG1(device_handle);

        int err = EINVAL;

        if (arg) {
                if (request == IOCTL_CLK__GET_CLK_INFO) {
                        RCC_ClocksTypeDef freq;
                        RCC_GetClocksFreq(&freq);

                        CLK_info_t *clkinf = arg;

                        switch (clkinf->iterator) {
                        case 0:
                                clkinf->freq_Hz = freq.SYSCLK_Frequency;
                                clkinf->name = "SYSCLK";
                                break;

                        case 1:
                                clkinf->freq_Hz = freq.HCLK_Frequency;
                                clkinf->name = "HCLK";
                                break;

                        case 2:
                                clkinf->freq_Hz = freq.PCLK1_Frequency;
                                clkinf->name = "PCLK1";
                                break;

                        case 3:
                                clkinf->freq_Hz = freq.PCLK2_Frequency;
                                clkinf->name = "PCLK2";
                                break;

                        case 4:
                                clkinf->freq_Hz = freq.ADCCLK_Frequency;
                                clkinf->name = "ADCCLK";
                                break;

                        case 5:
                                if (is_APB1_divided()) {
                                        clkinf->freq_Hz = freq.PCLK1_Frequency * 2;
                                } else {
                                        clkinf->freq_Hz = freq.PCLK1_Frequency;
                                }
                                clkinf->name = "PCLK1_TIM";
                                break;

                        case 6:
                                if (is_APB2_divided()) {
                                        clkinf->freq_Hz = freq.PCLK2_Frequency * 2;
                                } else {
                                        clkinf->freq_Hz = freq.PCLK2_Frequency;
                                }
                                clkinf->name = "PCLK2_TIM";
                                break;

                        default:
                                clkinf->freq_Hz = 0;
                                clkinf->name = NULL;
                                break;
                        }

                        clkinf->iterator++;

                        err = ESUCC;
                } else {
                        err = EBADRQC;
                }
        }

        return err;
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
API_MOD_FLUSH(CLK, void *device_handle)
{
        UNUSED_ARG1(device_handle);

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
API_MOD_STAT(CLK, void *device_handle, struct vfs_dev_stat *device_stat)
{
        UNUSED_ARG1(device_handle);

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
static int wait_for_flag(u32_t flag, u32_t timeout)
{
        u32_t timer = sys_time_get_reference();
        while (RCC_GetFlagStatus(flag) == RESET) {
                if (sys_time_is_expired(timer, timeout)) {
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
