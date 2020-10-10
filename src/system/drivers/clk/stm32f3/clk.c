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
#include "stm32f3/clk_cfg.h"
#include "stm32f3/stm32f3xx.h"
#include "stm32f3/lib/stm32f3xx_ll_rcc.h"
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

        int err = ESUCC;

        LL_RCC_DeInit();

        set_flash_latency(__CLK_FLASH_LATENCY__);
        enable_prefetch_buffer();

        //----------------------------------------------------------------------
        // MCOx clock sources and prescalers
        //----------------------------------------------------------------------
        LL_RCC_ConfigMCO(__CLK_MCO1_SRC__, __CLK_MCO1_DIV__);

        //----------------------------------------------------------------------
        // LSI OSCILLATOR
        //----------------------------------------------------------------------
        if (__CLK_LSI_ON__) {
                LL_RCC_LSI_Enable();

                u64_t tref = sys_time_get_reference();
                while (not sys_time_is_expired(tref, TIMEOUT_MS)) {
                        if (LL_RCC_LSI_IsReady()) {
                                break;
                        }
                }

                if (sys_time_is_expired(tref, TIMEOUT_MS)) {
                        printk("CLK: LSI timeout");
                }
        }

        //----------------------------------------------------------------------
        // LSE OSCILLATOR
        //----------------------------------------------------------------------
        if (__CLK_LSE_ON__) {

                SET_BIT(RCC->APB1ENR, RCC_APB1ENR_PWREN);
                SET_BIT(PWR->CR, PWR_CR_DBP);

                if (!(RCC->BDCR & RCC_BDCR_LSERDY)) {

                        if (__CLK_LSE_ON__ == _CLK_BAYPASS) {
                                LL_RCC_LSE_EnableBypass();

                        } else {
                                LL_RCC_LSE_Enable();

                                u64_t tref = sys_time_get_reference();
                                while (not sys_time_is_expired(tref, TIMEOUT_MS)) {
                                        if (LL_RCC_LSE_IsReady()) {
                                                break;
                                        }
                                }

                                if (sys_time_is_expired(tref, TIMEOUT_MS)) {
                                        printk("CLK: LSE timeout");
                                }
                        }
                }
        }

        //----------------------------------------------------------------------
        // HSE OSCILLATOR
        //----------------------------------------------------------------------
        if (__CLK_HSE_ON__) {

                if (__CLK_HSE_ON__ == _CLK_BAYPASS) {
                        LL_RCC_HSE_Enable();
                        LL_RCC_HSE_EnableBypass();

                } else {
                        LL_RCC_HSE_Enable();

                        u64_t tref = sys_time_get_reference();
                        while (not sys_time_is_expired(tref, TIMEOUT_MS)) {
                                if (LL_RCC_HSE_IsReady()) {
                                        break;
                                }
                        }

                        if (sys_time_is_expired(tref, TIMEOUT_MS)) {
                                printk("CLK: HSE timeout");
                        }
                }
        }

        //----------------------------------------------------------------------
        // RTC clock selection
        //----------------------------------------------------------------------
        if ( (  (__CLK_RTC_SRC__ == LL_RCC_RTC_CLKSOURCE_LSE && __CLK_LSE_ON__)
             || (__CLK_RTC_SRC__ == LL_RCC_RTC_CLKSOURCE_LSI && __CLK_LSI_ON__)
             || (__CLK_RTC_SRC__ == LL_RCC_RTC_CLKSOURCE_HSE_DIV32 && __CLK_HSE_ON__) )

           && (RCC->BDCR & RCC_BDCR_RTCEN) == 0
           && (RCC->BDCR & (RCC_BDCR_RTCSEL_1 | RCC_BDCR_RTCSEL_0)) == 0) {

                SET_BIT(RCC->APB1ENR, RCC_APB1ENR_PWREN);
                SET_BIT(PWR->CR, PWR_CR_DBP);
                LL_RCC_SetRTCClockSource(__CLK_RTC_SRC__);
        }

        //----------------------------------------------------------------------
        // Main PLL configuration
        //----------------------------------------------------------------------
        if (__CLK_PLL_ON__) {

                LL_RCC_PLL_ConfigDomain_SYS(__CLK_PLL_SRC__,
                                            __CLK_PLL_N__, __CLK_PLL_SRC_DIV_M__);

                LL_RCC_PLL_Enable();

                u64_t tref = sys_time_get_reference();
                while (not sys_time_is_expired(tref, TIMEOUT_MS)) {
                        if (LL_RCC_PLL_IsReady()) {
                                break;
                        }
                }

                if (sys_time_is_expired(tref, TIMEOUT_MS)) {
                        printk("CLK: PLL timeout");
                }
        }

        //----------------------------------------------------------------------
        // Peripheral clock sources and prescalers
        //----------------------------------------------------------------------
        LL_RCC_SetAHBPrescaler(__CLK_AHB_PRE__);
        LL_RCC_SetAPB1Prescaler(__CLK_APB1_PRE__);
        LL_RCC_SetAPB2Prescaler(__CLK_APB2_PRE__);
        LL_RCC_SetSysClkSource(__CLK_SYS_CLK_SRC__);

        //----------------------------------------------------------------------
        // USART/UART clock source
        //----------------------------------------------------------------------
        LL_RCC_SetUSARTClockSource(__CLK_USART1_SRC__);
        LL_RCC_SetUSARTClockSource(__CLK_USART2_SRC__);
        LL_RCC_SetUSARTClockSource(__CLK_USART3_SRC__);
        LL_RCC_SetUARTClockSource(__CLK_UART4_SRC__);
        LL_RCC_SetUARTClockSource(__CLK_UART5_SRC__);

        //----------------------------------------------------------------------
        // I2C clock source
        //----------------------------------------------------------------------
        LL_RCC_SetI2CClockSource(__CLK_I2C1_SRC__);
        LL_RCC_SetI2CClockSource(__CLK_I2C2_SRC__);

        //----------------------------------------------------------------------
        // HDMI-CEC clock source
        //----------------------------------------------------------------------
        #if defined(CEC)
        LL_RCC_SetCECClockSource(__CLK_CEC_SRC__);
        #endif

        //----------------------------------------------------------------------
        // USB & RND clock source
        //----------------------------------------------------------------------
        #if defined(USB)
        LL_RCC_SetUSBClockSource(__CLK_USB_DIV__);
        #endif

        //----------------------------------------------------------------------
        // I2S clock source
        //----------------------------------------------------------------------
        #if defined(RCC_CFGR_I2SSRC)
        LL_RCC_SetI2SClockSource(__CLK_I2S_SRC__);
        #endif

        //----------------------------------------------------------------------
        // ADC clock source
        //----------------------------------------------------------------------
        #if defined(RCC_CFGR2_ADCPRE12)
        LL_RCC_SetADCClockSource(__CLK_ADC12_SRC__);
        #endif
        #if defined(RCC_CFGR2_ADCPRE34)
        LL_RCC_SetADCClockSource(__CLK_ADC34_SRC__);
        #endif

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

        LL_RCC_DeInit();
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
        UNUSED_ARG2(device_handle, flags);
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
        UNUSED_ARG2(device_handle, force);
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
        UNUSED_ARG6(device_handle, src, count, fpos, wrcnt, fattr);
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
        UNUSED_ARG6(device_handle, dst, count, fpos, rdcnt, fattr);
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
                        LL_RCC_ClocksTypeDef freq;
                        LL_RCC_GetSystemClocksFreq(&freq);

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
                                if (is_APB1_divided()) {
                                        clkinf->freq_Hz = freq.PCLK1_Frequency * 2;
                                } else {
                                        clkinf->freq_Hz = freq.PCLK1_Frequency;
                                }
                                clkinf->name = "PCLK1_TIM";
                                break;

                        case 5:
                                if (is_APB2_divided()) {
                                        clkinf->freq_Hz = freq.PCLK2_Frequency * 2;
                                } else {
                                        clkinf->freq_Hz = freq.PCLK2_Frequency;
                                }
                                clkinf->name = "PCLK2_TIM";
                                break;

                        case 6:
                                clkinf->freq_Hz = LL_RCC_GetUSARTClockFreq(LL_RCC_USART1_CLKSOURCE);
                                clkinf->name = "USART1";
                                break;

                        case 7:
                                clkinf->freq_Hz = LL_RCC_GetUSARTClockFreq(LL_RCC_USART2_CLKSOURCE);
                                clkinf->name = "USART2";
                                break;

                        case 8:
                                clkinf->freq_Hz = LL_RCC_GetUSARTClockFreq(LL_RCC_USART3_CLKSOURCE);
                                clkinf->name = "USART3";
                                break;

                        case 9:
                                clkinf->freq_Hz = LL_RCC_GetUARTClockFreq(LL_RCC_UART4_CLKSOURCE);
                                clkinf->name = "UART4";
                                break;

                        case 10:
                                clkinf->freq_Hz = LL_RCC_GetUARTClockFreq(LL_RCC_UART5_CLKSOURCE);
                                clkinf->name = "UART5";
                                break;

                        case 11:
                                clkinf->freq_Hz = LL_RCC_GetI2CClockFreq(LL_RCC_I2C1_CLKSOURCE);
                                clkinf->name = "I2C1";
                                break;

                        case 12:
                                clkinf->freq_Hz = LL_RCC_GetI2CClockFreq(LL_RCC_I2C2_CLKSOURCE);
                                clkinf->name = "I2C2";
                                break;

                        case 13:
                                #if defined(CEC)
                                clkinf->freq_Hz = LL_RCC_GetCECClockFreq(LL_RCC_CEC_CLKSOURCE);
                                #else
                                clkinf->freq_Hz = 0;
                                #endif
                                clkinf->name = "CEC";
                                break;

                        case 14:
                                #if defined(USB)
                                clkinf->freq_Hz = LL_RCC_GetUSBClockFreq(LL_RCC_USB_CLKSOURCE);
                                #else
                                clkinf->freq_Hz = 0;
                                #endif
                                clkinf->name = "USB";
                                break;

                        case 15:
                                #if defined(RCC_CFGR_I2SSRC)
                                clkinf->freq_Hz = LL_RCC_GetI2SClockFreq(LL_RCC_I2S_CLKSOURCE);
                                #else
                                clkinf->freq_Hz = 0;
                                #endif
                                clkinf->name = "I2S";
                                break;

                        case 16:
                                #if defined(RCC_CFGR_I2SSRC)
                                clkinf->freq_Hz = LL_RCC_GetI2SClockFreq(LL_RCC_I2S_CLKSOURCE);
                                #else
                                clkinf->freq_Hz = 0;
                                #endif
                                clkinf->name = "I2S";
                                break;

                        case 17:
                                #if defined(RCC_CFGR2_ADCPRE12)
                                clkinf->freq_Hz = LL_RCC_GetADCClockFreq(LL_RCC_ADC12_CLKSOURCE);
                                #else
                                clkinf->freq_Hz = 0;
                                #endif
                                clkinf->name = "ADC12";
                                break;

                        case 18:
                                #if defined(RCC_CFGR2_ADCPRE34)
                                clkinf->freq_Hz = LL_RCC_GetADCClockFreq(LL_RCC_ADC34_CLKSOURCE);
                                #else
                                clkinf->freq_Hz = 0;
                                #endif
                                clkinf->name = "ADC34";
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
        UNUSED_ARG2(device_handle, device_stat);
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
