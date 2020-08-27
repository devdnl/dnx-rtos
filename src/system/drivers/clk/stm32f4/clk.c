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
#include "stm32f4/clk_cfg.h"
#include "stm32f4/stm32f4xx.h"
#include "stm32f4/lib/stm32f4xx_rcc.h"
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

        //----------------------------------------------------------------------
        // MCOx clock sources and prescalers
        //----------------------------------------------------------------------
        RCC_MCO1Config(_CLK_CFG__MCO1_SRC, _CLK_CFG__MCO1_DIV);
        RCC_MCO2Config(_CLK_CFG__MCO2_SRC, _CLK_CFG__MCO2_DIV);

        //----------------------------------------------------------------------
        // LSI OSCILLATOR
        //----------------------------------------------------------------------
        if (_CLK_CFG__LSI_ON) {
                RCC_LSICmd(_CLK_CFG__LSI_ON);
                err = wait_for_flag(RCC_FLAG_LSIRDY, TIMEOUT_MS);
                if (err) {
                        printk("CLK: LSI timeout");
                        return err;
                }
        }

        //----------------------------------------------------------------------
        // LSE OSCILLATOR
        //----------------------------------------------------------------------
        if (_CLK_CFG__LSE_ON) {

                SET_BIT(RCC->APB1ENR, RCC_APB1ENR_PWREN);
                SET_BIT(PWR->CR, PWR_CR_DBP);

                if (!(RCC->BDCR & RCC_BDCR_LSERDY)) {

                        RCC_LSEConfig(_CLK_CFG__LSE_ON);

                        if (_CLK_CFG__LSE_ON != RCC_LSE_Bypass) {
                                // this oscillator not causes an error because is not a main osc.
                                if (wait_for_flag(RCC_FLAG_LSERDY, LSE_TIMEOUT_MS) != ESUCC) {
                                        // this oscillator not causes an error because is not a main osc.
                                        printk("CLK: LSE timeout");
                                }
                        }
                }
        }

        //----------------------------------------------------------------------
        // HSE OSCILLATOR
        //----------------------------------------------------------------------
        if (_CLK_CFG__HSE_ON) {
                RCC_HSEConfig(_CLK_CFG__HSE_ON);
                err = wait_for_flag(RCC_FLAG_HSERDY, TIMEOUT_MS);
                if (err) {
                        printk("CLK: HSE timeout");
                        return err;
                }
        }

        //----------------------------------------------------------------------
        // RTC clock selection
        //----------------------------------------------------------------------
        if ( (  (_CLK_CFG__RTCCLK_SRC == RCC_RTCCLKSource_LSE && _CLK_CFG__LSE_ON)
             || (_CLK_CFG__RTCCLK_SRC == RCC_RTCCLKSource_LSI && _CLK_CFG__LSI_ON)
             || (IS_RCC_RTCCLK_SOURCE(_CLK_CFG__RTCCLK_SRC)   && _CLK_CFG__HSE_ON) )

           && (RCC->BDCR & RCC_BDCR_RTCEN) == 0
           && (RCC->BDCR & (RCC_BDCR_RTCSEL_1 | RCC_BDCR_RTCSEL_0)) == 0) {

                SET_BIT(RCC->APB1ENR, RCC_APB1ENR_PWREN);
                SET_BIT(PWR->CR, PWR_CR_DBP);
                RCC_RTCCLKConfig(_CLK_CFG__RTCCLK_SRC);
        }

        //----------------------------------------------------------------------
        // SAI clock source
        //----------------------------------------------------------------------
#if defined(STM32F427_437xx) || defined(STM32F429_439xx) || defined(STM32F446xx) || defined(STM32F469_479xx)
        RCC_SAIPLLSAIClkDivConfig(_CLK_CFG__SAI_PLLSAI_CLK_DIV);
        RCC_SAIPLLI2SClkDivConfig(_CLK_CFG__SAI_PLLI2S_CLK_DIV);
#endif

#if defined(STM32F413_423xx)
        RCC_SAIPLLI2SRClkDivConfig(_CLK_CFG__SAI_PLLI2S_CLK_DIVR);
        RCC_SAIPLLRClkDivConfig(_CLK_CFG__SAI_PLL_CLK_DIVR);
#endif

#if defined(STM32F446xx)
        RCC_SAICLKConfig(RCC_SAIInstance_SAI1, _CLK_CFG__SAI1_CLK_SRC);
        RCC_SAICLKConfig(RCC_SAIInstance_SAI2, _CLK_CFG__SAI2_CLK_SRC);
#endif

#if defined(STM32F413_423xx) || defined(STM32F427_437xx) || defined(STM32F429_439xx) || defined(STM32F469_479xx)
        RCC_SAIBlockACLKConfig(_CLK_CFG__SAI_BLOCK_A_CLK_SRC);
        RCC_SAIBlockBCLKConfig(_CLK_CFG__SAI_BLOCK_B_CLK_SRC);
#endif

        //----------------------------------------------------------------------
        // LTDC clock source
        //----------------------------------------------------------------------
#if defined(STM32F427_437xx) || defined(STM32F429_439xx) || defined(STM32F469_479xx)
        RCC_LTDCCLKDivConfig(_CLK_CFG__LTDC_CLK_DIV);
#endif

        //----------------------------------------------------------------------
        // DFSDM clock source
        //----------------------------------------------------------------------
#if defined(STM32F412xG) || defined(STM32F413_423xx)
        RCC_DFSDM1CLKConfig(_CLK_CFG__DFSDM1_CLK_SRC);
        RCC_DFSDM1ACLKConfig(_CLK_CFG__DFSDM1_ACLK_SRC);

#if defined(STM32F413_423xx)
        RCC_DFSDM2ACLKConfig(_CLK_CFG__DFSDM2_ACLK_SRC);
#endif
#endif

        //----------------------------------------------------------------------
        // DSI clock source
        //----------------------------------------------------------------------
#if defined(STM32F469_479xx)
        RCC_DSIClockSourceConfig(_CLK_CFG__DSI_CLK_SRC);
#endif

        //----------------------------------------------------------------------
        // DFSDM clock source
        //----------------------------------------------------------------------
#if defined(STM32F412xG) || defined(STM32F413_423xx) || defined(STM32F446xx) || defined(STM32F469_479xx)
        RCC_48MHzClockSourceConfig(_CLK_CFG__48MHZ_CLK_SRC);
#endif

        //----------------------------------------------------------------------
        // SDIO clock source
        //----------------------------------------------------------------------
#if defined(STM32F412xG) || defined(STM32F413_423xx) || defined(STM32F446xx) || defined(STM32F469_479xx)
        RCC_SDIOClockSourceConfig(_CLK_CFG__SDIO_CLK_SRC);
#endif

        //----------------------------------------------------------------------
        // SPDIF clock source
        //----------------------------------------------------------------------
#if defined(STM32F446xx)
        RCC_SPDIFRXClockSourceConfig(_CLK_CFG__SPDIF_CLK_SRC);
#endif

        //----------------------------------------------------------------------
        // CEC clock source
        //----------------------------------------------------------------------
#if defined(STM32F446xx)
        RCC_CECClockSourceConfig(_CLK_CFG__CEC_CLK_SRC);
#endif

        //----------------------------------------------------------------------
        // FMPI2C clock source
        //----------------------------------------------------------------------
#if defined(STM32F410xx) || defined(STM32F412xG) || defined(STM32F413_423xx) || defined(STM32F446xx)
        RCC_FMPI2C1ClockSourceConfig(_CLK_CFG__FMPI2C1_CLK_SRC);
#endif

        //----------------------------------------------------------------------
        // Main PLL configuration
        //----------------------------------------------------------------------
#if defined(STM32F410xx) || defined(STM32F412xG) || defined(STM32F413_423xx) || defined(STM32F446xx) || defined(STM32F469_479xx)
        RCC_PLLConfig(_CLK_CFG__PLL_SRC,
                      _CLK_CFG__PLL_SRC_DIV_M,
                      _CLK_CFG__PLL_N,
                      _CLK_CFG__PLL_P,
                      _CLK_CFG__PLL_Q,
                      _CLK_CFG__PLL_R);
#endif

#if defined(STM32F40_41xxx) || defined(STM32F427_437xx) || defined(STM32F429_439xx) || defined(STM32F401xx) || defined(STM32F411xE)
        RCC_PLLConfig(_CLK_CFG__PLL_SRC,
                      _CLK_CFG__PLL_SRC_DIV_M,
                      _CLK_CFG__PLL_N,
                      _CLK_CFG__PLL_P,
                      _CLK_CFG__PLL_Q);
#endif

        RCC_PLLCmd(_CLK_CFG__PLL_ON);
        if (_CLK_CFG__PLL_ON) {
                err = wait_for_flag(RCC_FLAG_PLLRDY, TIMEOUT_MS);
                if (err) {
                        printk("CLK: PLL timeout");
                        return err;
                }
        }

        //----------------------------------------------------------------------
        // I2S PLL configuration
        //----------------------------------------------------------------------
#if defined(STM32F40_41xxx) || defined(STM32F401xx)
        RCC_PLLI2SConfig(_CLK_CFG__PLLI2S_N, _CLK_CFG__PLLI2S_R);
#endif

#if defined(STM32F411xE)
        RCC_PLLI2SConfig(_CLK_CFG__PLLI2S_N, _CLK_CFG__PLLI2S_R, _CLK_CFG__PLLI2S_M);
#endif

#if defined(STM32F427_437xx) || defined(STM32F429_439xx) || defined(STM32F469_479xx)
        RCC_PLLI2SConfig(_CLK_CFG__PLLI2S_N, _CLK_CFG__PLLI2S_Q, _CLK_CFG__PLLI2S_R);
#endif

#if defined(STM32F412xG ) || defined(STM32F413_423xx) || defined(STM32F446xx)
        RCC_PLLI2SConfig(_CLK_CFG__PLLI2S_M, _CLK_CFG__PLLI2S_N, _CLK_CFG__PLLI2S_P,
                         _CLK_CFG__PLLI2S_Q, _CLK_CFG__PLLI2S_R);
#endif

#if defined(STM32F40_41xxx) || defined(STM32F401xx) || defined(STM32F411xE) || defined(STM32F427_437xx) || defined(STM32F429_439xx) || defined(STM32F469_479xx) || defined(STM32F412xG ) || defined(STM32F413_423xx) || defined(STM32F446xx)
        RCC_PLLI2SCmd(_CLK_CFG__PLLI2S_ON);
        if (_CLK_CFG__PLLI2S_ON) {
                err = wait_for_flag(RCC_FLAG_PLLI2SRDY, TIMEOUT_MS);
                if (err) {
                        printk("CLK: PLLI2S timeout");
                        return err;
                }
        }
#endif

        //----------------------------------------------------------------------
        // SAI PLL configuration
        //----------------------------------------------------------------------
#if defined(STM32F469_479xx)
        RCC_PLLSAIConfig(_CLK_CFG__PLLSAI_N, _CLK_CFG__PLLSAI_P, _CLK_CFG__PLLSAI_Q, _CLK_CFG__PLLSAI_R);
#endif

#if defined(STM32F446xx)
        RCC_PLLSAIConfig(_CLK_CFG__PLLSAI_M, _CLK_CFG__PLLSAI_N, _CLK_CFG__PLLSAI_P, _CLK_CFG__PLLSAI_Q);
#endif

#if defined(STM32F427_437xx) || defined(STM32F429_439xx)
        RCC_PLLSAIConfig(_CLK_CFG__PLLSAI_N, _CLK_CFG__PLLSAI_Q, _CLK_CFG__PLLSAI_R);
#endif

#if defined(STM32F469_479xx) || defined(STM32F446xx) || defined(STM32F427_437xx) || defined(STM32F429_439xx)
        RCC_PLLSAICmd(_CLK_CFG__PLLSAI_ON);
        if (_CLK_CFG__PLLSAI_ON) {
                err = wait_for_flag(RCC_FLAG_PLLSAIRDY, TIMEOUT_MS);
                if (err) {
                        printk("CLK: PLLSAI timeout");
                        return err;
                }
        }
#endif

        //----------------------------------------------------------------------
        // Peripheral clock sources and prescalers
        //----------------------------------------------------------------------
        RCC_PCLK2Config(_CLK_CFG__APB2_PRE);
        RCC_PCLK1Config(_CLK_CFG__APB1_PRE);
        RCC_HCLKConfig(_CLK_CFG__AHB_PRE);
        RCC_SYSCLKConfig(_CLK_CFG__SYSCLK_SRC);

        //----------------------------------------------------------------------
        // I2S clock source
        //----------------------------------------------------------------------
#if defined(STM32F412xG) || defined(STM32F413_423xx) || defined(STM32F446xx)
        RCC_I2SCLKConfig(RCC_I2SBus_APB1, _CLK_CFG__I2SAPB1_CLK_SRC);
        RCC_I2SCLKConfig(RCC_I2SBus_APB2, _CLK_CFG__I2SAPB2_CLK_SRC);
#endif

#if defined(STM32F410xx) || defined(STM32F40_41xxx)
        RCC_I2SCLKConfig(_CLK_CFG__I2S_CLK_SRC);
#endif

#if defined(STM32F40_41xxx) || defined(STM32F427_437xx) || defined(STM32F429_439xx) || defined(STM32F401xx) || defined(STM32F411xE) || defined(STM32F469_479xx)
        RCC_I2SCLKConfig(_CLK_CFG__I2S_CLK_SRC);
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
        SET_BIT(FLASH->ACR, FLASH_ACR_PRFTEN | FLASH_ACR_ICEN | FLASH_ACR_DCEN);
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
