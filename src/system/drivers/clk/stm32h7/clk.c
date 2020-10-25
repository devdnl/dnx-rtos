/*=========================================================================*//**
@file    clk.c

@author  Daniel Zorychta

@brief   File support system clock (PLL).

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
#include "drivers/driver.h"
#include "stm32h7/clk_cfg.h"
#include "stm32h7/stm32h7xx.h"
#include "stm32h7/lib/stm32h7xx_ll_rcc.h"
#include "../clk_ioctl.h"

/*==============================================================================
  Local symbolic constants/macros
==============================================================================*/

/*==============================================================================
  Local types, enums definitions
==============================================================================*/

/*==============================================================================
  Local function prototypes
==============================================================================*/

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
                        CLK_info_t *clkinf = arg;

                        switch (clkinf->iterator) {
                        case 0: {
                                LL_RCC_ClocksTypeDef freq;
                                LL_RCC_GetSystemClocksFreq(&freq);
                                clkinf->freq_Hz = freq.SYSCLK_Frequency;
                                clkinf->name = "SYSCLK";
                                break;
                        }

                        case 1: {
                                LL_RCC_ClocksTypeDef freq;
                                LL_RCC_GetSystemClocksFreq(&freq);
                                clkinf->freq_Hz = freq.HCLK_Frequency;
                                clkinf->name = "HCLK";
                                break;
                        }

                        case 2: {
                                LL_RCC_ClocksTypeDef freq;
                                LL_RCC_GetSystemClocksFreq(&freq);
                                clkinf->freq_Hz = freq.PCLK1_Frequency;
                                clkinf->name = "PCLK1";
                                break;
                        }

                        case 3: {
                                LL_RCC_ClocksTypeDef freq;
                                LL_RCC_GetSystemClocksFreq(&freq);
                                clkinf->freq_Hz = freq.PCLK2_Frequency;
                                clkinf->name = "PCLK2";
                                break;
                        }

                        case 4: {
                                LL_RCC_ClocksTypeDef freq;
                                LL_RCC_GetSystemClocksFreq(&freq);
                                clkinf->freq_Hz = freq.PCLK3_Frequency;
                                clkinf->name = "PCLK3";
                                break;
                        }

                        case 5: {
                                LL_RCC_ClocksTypeDef freq;
                                LL_RCC_GetSystemClocksFreq(&freq);
                                clkinf->freq_Hz = freq.PCLK4_Frequency;
                                clkinf->name = "PCLK4";
                                break;
                        }

                        case 6: {
                                LL_PLL_ClocksTypeDef pll;
                                LL_RCC_GetPLL1ClockFreq(&pll);
                                clkinf->freq_Hz = pll.PLL_P_Frequency;
                                clkinf->name    = "PLL1P";
                                break;
                        }

                        case 7: {
                                LL_PLL_ClocksTypeDef pll;
                                LL_RCC_GetPLL1ClockFreq(&pll);
                                clkinf->freq_Hz = pll.PLL_Q_Frequency;
                                clkinf->name    = "PLL1Q";
                                break;
                        }

                        case 8: {
                                LL_PLL_ClocksTypeDef pll;
                                LL_RCC_GetPLL1ClockFreq(&pll);
                                clkinf->freq_Hz = pll.PLL_R_Frequency;
                                clkinf->name    = "PLL1R";
                                break;
                        }

                        case 9: {
                                LL_PLL_ClocksTypeDef pll;
                                LL_RCC_GetPLL2ClockFreq(&pll);
                                clkinf->freq_Hz = pll.PLL_P_Frequency;
                                clkinf->name    = "PLL2P";
                                break;
                        }

                        case 10: {
                                LL_PLL_ClocksTypeDef pll;
                                LL_RCC_GetPLL2ClockFreq(&pll);
                                clkinf->freq_Hz = pll.PLL_Q_Frequency;
                                clkinf->name    = "PLL2Q";
                                break;
                        }

                        case 11: {
                                LL_PLL_ClocksTypeDef pll;
                                LL_RCC_GetPLL2ClockFreq(&pll);
                                clkinf->freq_Hz = pll.PLL_R_Frequency;
                                clkinf->name    = "PLL2R";
                                break;
                        }

                        case 12: {
                                LL_PLL_ClocksTypeDef pll;
                                LL_RCC_GetPLL3ClockFreq(&pll);
                                clkinf->freq_Hz = pll.PLL_P_Frequency;
                                clkinf->name    = "PLL3P";
                                break;
                        }

                        case 13: {
                                LL_PLL_ClocksTypeDef pll;
                                LL_RCC_GetPLL3ClockFreq(&pll);
                                clkinf->freq_Hz = pll.PLL_Q_Frequency;
                                clkinf->name    = "PLL3Q";
                                break;
                        }

                        case 14: {
                                LL_PLL_ClocksTypeDef pll;
                                LL_RCC_GetPLL3ClockFreq(&pll);
                                clkinf->freq_Hz = pll.PLL_R_Frequency;
                                clkinf->name    = "PLL3R";
                                break;
                        }

                        case 15:
                                clkinf->freq_Hz = LL_RCC_GetUSARTClockFreq(LL_RCC_USART16_CLKSOURCE);
                                clkinf->name = "USART1";
                                break;

                        case 16:
                                clkinf->freq_Hz = LL_RCC_GetUSARTClockFreq(LL_RCC_USART234578_CLKSOURCE);
                                clkinf->name = "USART2";
                                break;

                        case 17:
                                clkinf->freq_Hz = LL_RCC_GetUSARTClockFreq(LL_RCC_USART234578_CLKSOURCE);
                                clkinf->name = "USART3";
                                break;

                        case 18:
                                clkinf->freq_Hz = LL_RCC_GetUSARTClockFreq(LL_RCC_USART234578_CLKSOURCE);
                                clkinf->name = "UART4";
                                break;

                        case 19:
                                clkinf->freq_Hz = LL_RCC_GetUSARTClockFreq(LL_RCC_USART234578_CLKSOURCE);
                                clkinf->name = "UART5";
                                break;

                        case 20:
                                clkinf->freq_Hz = LL_RCC_GetUSARTClockFreq(LL_RCC_USART16_CLKSOURCE);
                                clkinf->name = "USART6";
                                break;

                        case 21:
                                clkinf->freq_Hz = LL_RCC_GetUSARTClockFreq(LL_RCC_USART234578_CLKSOURCE);
                                clkinf->name = "UART7";
                                break;

                        case 22:
                                clkinf->freq_Hz = LL_RCC_GetUSARTClockFreq(LL_RCC_USART234578_CLKSOURCE);
                                clkinf->name = "UART8";
                                break;

                        case 23:
                                clkinf->freq_Hz = LL_RCC_GetUSARTClockFreq(LL_RCC_USART16_CLKSOURCE);
                                clkinf->name = "UART9";
                                break;

                        case 24:
                                clkinf->freq_Hz = LL_RCC_GetUSARTClockFreq(LL_RCC_USART16_CLKSOURCE);
                                clkinf->name = "USART10";
                                break;

                        case 25:
                                clkinf->freq_Hz = LL_RCC_GetLPUARTClockFreq(LL_RCC_LPUART1_CLKSOURCE);
                                clkinf->name = "LPUART1";
                                break;

                        case 26:
                                clkinf->freq_Hz = LL_RCC_GetI2CClockFreq(LL_RCC_I2C123_CLKSOURCE);
                                clkinf->name = "I2C1";
                                break;

                        case 27:
                                clkinf->freq_Hz = LL_RCC_GetI2CClockFreq(LL_RCC_I2C123_CLKSOURCE);
                                clkinf->name = "I2C2";
                                break;

                        case 28:
                                clkinf->freq_Hz = LL_RCC_GetI2CClockFreq(LL_RCC_I2C123_CLKSOURCE);
                                clkinf->name = "I2C3";
                                break;

                        case 29:
                                clkinf->freq_Hz = LL_RCC_GetI2CClockFreq(LL_RCC_I2C4_CLKSOURCE);
                                clkinf->name = "I2C4";
                                break;

                        case 30:
                                clkinf->freq_Hz = LL_RCC_GetI2CClockFreq(LL_RCC_I2C123_CLKSOURCE);
                                clkinf->name = "I2C5";
                                break;

                        case 31:
                                clkinf->freq_Hz = LL_RCC_GetLPTIMClockFreq(LL_RCC_LPTIM1_CLKSOURCE);
                                clkinf->name = "LPTIM1";
                                break;

                        case 32:
                                clkinf->freq_Hz = LL_RCC_GetLPTIMClockFreq(LL_RCC_LPTIM2_CLKSOURCE);
                                clkinf->name = "LPTIM2";
                                break;

                        case 33:
                                clkinf->freq_Hz = LL_RCC_GetLPTIMClockFreq(LL_RCC_LPTIM345_CLKSOURCE);
                                clkinf->name = "LPTIM3";
                                break;

                        case 34:
                                clkinf->freq_Hz = LL_RCC_GetLPTIMClockFreq(LL_RCC_LPTIM345_CLKSOURCE);
                                clkinf->name = "LPTIM4";
                                break;

                        case 35:
                                clkinf->freq_Hz = LL_RCC_GetLPTIMClockFreq(LL_RCC_LPTIM345_CLKSOURCE);
                                clkinf->name = "LPTIM5";
                                break;

                        case 36:
                                clkinf->freq_Hz = LL_RCC_GetADCClockFreq(LL_RCC_ADC_CLKSOURCE);
                                clkinf->name = "ADC";
                                break;

                        case 37:
                                clkinf->freq_Hz = LL_RCC_GetSDMMCClockFreq(LL_RCC_SDMMC_CLKSOURCE);
                                clkinf->name = "SDMMC";
                                break;

                        case 38:
                                clkinf->freq_Hz = LL_RCC_GetRNGClockFreq(LL_RCC_RNG_CLKSOURCE);
                                clkinf->name = "RNG";
                                break;

                        case 39:
                                clkinf->freq_Hz = LL_RCC_GetCECClockFreq(LL_RCC_CEC_CLKSOURCE);
                                clkinf->name = "CEC";
                                break;

                        case 40:
                                clkinf->freq_Hz = LL_RCC_GetUSBClockFreq(LL_RCC_USB_CLKSOURCE);
                                clkinf->name = "USB";
                                break;

                        case 41:
                                clkinf->freq_Hz = LL_RCC_GetDFSDMClockFreq(LL_RCC_DFSDM1_CLKSOURCE);
                                clkinf->name = "DFSDM1";
                                break;

                        case 42:
                                clkinf->freq_Hz = LL_RCC_GetSPIClockFreq(LL_RCC_SPI123_CLKSOURCE);
                                clkinf->name = "SPI1";
                                break;

                        case 43:
                                clkinf->freq_Hz = LL_RCC_GetSPIClockFreq(LL_RCC_SPI123_CLKSOURCE);
                                clkinf->name = "SPI2";
                                break;

                        case 44:
                                clkinf->freq_Hz = LL_RCC_GetSPIClockFreq(LL_RCC_SPI123_CLKSOURCE);
                                clkinf->name = "SPI3";
                                break;

                        case 45:
                                clkinf->freq_Hz = LL_RCC_GetSPIClockFreq(LL_RCC_SPI45_CLKSOURCE);
                                clkinf->name = "SPI4";
                                break;

                        case 46:
                                clkinf->freq_Hz = LL_RCC_GetSPIClockFreq(LL_RCC_SPI45_CLKSOURCE);
                                clkinf->name = "SPI5";
                                break;

                        case 47:
                                clkinf->freq_Hz = LL_RCC_GetSPIClockFreq(LL_RCC_SPI6_CLKSOURCE);
                                clkinf->name = "SPI6";
                                break;

                        case 48:
                                clkinf->freq_Hz = LL_RCC_GetFMCClockFreq(LL_RCC_FMC_CLKSOURCE);
                                clkinf->name = "FMC";
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

/*==============================================================================
  End of file
==============================================================================*/
