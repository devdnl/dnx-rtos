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
#define TIMEOUT_MS                              250
#define LSE_TIMEOUT_MS                          5000

#if defined(PWR_SRDCR_VOS)
#define PWR_REGULATOR_VOLTAGE_SCALE0            (PWR_SRDCR_VOS_1 | PWR_SRDCR_VOS_0)
#define PWR_REGULATOR_VOLTAGE_SCALE1            (PWR_SRDCR_VOS_1)
#define PWR_REGULATOR_VOLTAGE_SCALE2            (PWR_SRDCR_VOS_0)
#define PWR_REGULATOR_VOLTAGE_SCALE3            (0U)
#else
#define PWR_REGULATOR_VOLTAGE_SCALE0            (0U)
#define PWR_REGULATOR_VOLTAGE_SCALE1            (PWR_D3CR_VOS_1 | PWR_D3CR_VOS_0)
#define PWR_REGULATOR_VOLTAGE_SCALE2            (PWR_D3CR_VOS_1)
#define PWR_REGULATOR_VOLTAGE_SCALE3            (PWR_D3CR_VOS_0)
#endif

#define FLASH_ACR_WRHIGHFREQ_MSK                (FLASH_ACR_WRHIGHFREQ_1 | FLASH_ACR_WRHIGHFREQ_0)
#define FLASH_ACR_WRHIGHFREQ_00                 ((0*FLASH_ACR_WRHIGHFREQ_1) | (0*FLASH_ACR_WRHIGHFREQ_0))
#define FLASH_ACR_WRHIGHFREQ_01                 ((0*FLASH_ACR_WRHIGHFREQ_1) | (1*FLASH_ACR_WRHIGHFREQ_0))
#define FLASH_ACR_WRHIGHFREQ_10                 ((1*FLASH_ACR_WRHIGHFREQ_1) | (0*FLASH_ACR_WRHIGHFREQ_0))

/** PWREx Supply configuration */
#define PWR_LDO_SUPPLY                          PWR_CR3_LDOEN                                                               /*!< Core domains are supplied from the LDO                                                                     */
#if defined (SMPS)
#define PWR_DIRECT_SMPS_SUPPLY                  PWR_CR3_SMPSEN                                                              /*!< Core domains are supplied from the SMPS only                                                               */
#define PWR_SMPS_1V8_SUPPLIES_LDO               (PWR_CR3_SMPSLEVEL_0 | PWR_CR3_SMPSEN    | PWR_CR3_LDOEN)                   /*!< The SMPS 1.8V output supplies the LDO which supplies the Core domains                                       */
#define PWR_SMPS_2V5_SUPPLIES_LDO               (PWR_CR3_SMPSLEVEL_1 | PWR_CR3_SMPSEN    | PWR_CR3_LDOEN)                   /*!< The SMPS 2.5V output supplies the LDO which supplies the Core domains                                       */
#define PWR_SMPS_1V8_SUPPLIES_EXT_AND_LDO       (PWR_CR3_SMPSLEVEL_0 | PWR_CR3_SMPSEXTHP | PWR_CR3_SMPSEN | PWR_CR3_LDOEN)  /*!< The SMPS 1.8V output supplies an external circuits and the LDO. The Core domains are supplied from the LDO */
#define PWR_SMPS_2V5_SUPPLIES_EXT_AND_LDO       (PWR_CR3_SMPSLEVEL_1 | PWR_CR3_SMPSEXTHP | PWR_CR3_SMPSEN | PWR_CR3_LDOEN)  /*!< The SMPS 2.5V output supplies an external circuits and the LDO. The Core domains are supplied from the LDO */
#define PWR_SMPS_1V8_SUPPLIES_EXT               (PWR_CR3_SMPSLEVEL_0 | PWR_CR3_SMPSEXTHP | PWR_CR3_SMPSEN | PWR_CR3_BYPASS) /*!< The SMPS 1.8V output supplies an external source which supplies the Core domains                            */
#define PWR_SMPS_2V5_SUPPLIES_EXT               (PWR_CR3_SMPSLEVEL_1 | PWR_CR3_SMPSEXTHP | PWR_CR3_SMPSEN | PWR_CR3_BYPASS) /*!< The SMPS 2.5V output supplies an external source which supplies the Core domains                            */
#endif /* defined (SMPS) */
#define PWR_EXTERNAL_SOURCE_SUPPLY              PWR_CR3_BYPASS                                                              /*!< The SMPS disabled and the LDO Bypass. The Core domains are supplied from an external source                 */

#if defined (SMPS)
#define PWR_SUPPLY_CONFIG_MASK                  (PWR_CR3_SMPSLEVEL | PWR_CR3_SMPSEXTHP | PWR_CR3_SMPSEN | PWR_CR3_LDOEN | PWR_CR3_BYPASS)
#else
#define PWR_SUPPLY_CONFIG_MASK                  (PWR_CR3_SCUEN | PWR_CR3_LDOEN | PWR_CR3_BYPASS)
#endif

/*==============================================================================
  Local types, enums definitions
==============================================================================*/
typedef struct {
        u8_t major;
        u8_t minor;
} clk_t;

/*==============================================================================
  Local function prototypes
==============================================================================*/
static u32_t get_pll_input_freq(void);
static void  get_pll_input_parameters(u32_t *pll_input_freq, u32_t *pll_input_range, u32_t *pll_ouput_range);
static int   configure_core_voltage_and_flash_latency(void);
static int   configure_power_supply(uint32_t supply_source);
static int   control_voltage_scaling(uint32_t voltage_scaling);

/*==============================================================================
  Local object definitions
==============================================================================*/
MODULE_NAME(CLK);

// makes dev_dbg() happy
static const clk_t dummy_hdl = {.major = 0, .minor = 0};
static const clk_t *const hdl = &dummy_hdl;

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

        int err = configure_power_supply(__CLK_SUPPLY_SOURCE__);
        if (err) {
                goto finish;
        }

        LL_RCC_DeInit();

        //----------------------------------------------------------------------
        // LSI OSCILLATOR
        //----------------------------------------------------------------------
        if (__CLK_LSI_ON__) {
                LL_RCC_LSI_Enable();

                clock_t tref = sys_get_uptime_ms();
                while (not sys_is_time_expired(tref, TIMEOUT_MS)) {
                        if (LL_RCC_LSI_IsReady()) {
                                break;
                        }
                }

                if (sys_is_time_expired(tref, TIMEOUT_MS)) {
                        printk("CLK: LSI timeout");
                }
        }

        //----------------------------------------------------------------------
        // LSE OSCILLATOR
        //----------------------------------------------------------------------
        if (__CLK_LSE_ON__) {

                SET_BIT(PWR->CR1, PWR_CR1_DBP);

                if (!(RCC->BDCR & RCC_BDCR_LSERDY)) {

                        if (__CLK_LSE_ON__ == _CLK_BAYPASS) {
                                LL_RCC_LSE_EnableBypass();

                        } else {
                                LL_RCC_LSE_Enable();

                                clock_t tref = sys_get_uptime_ms();
                                while (not sys_is_time_expired(tref, TIMEOUT_MS)) {
                                        if (LL_RCC_LSE_IsReady()) {
                                                break;
                                        }
                                }

                                if (sys_is_time_expired(tref, TIMEOUT_MS)) {
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

                        clock_t tref = sys_get_uptime_ms();
                        while (not sys_is_time_expired(tref, TIMEOUT_MS)) {
                                if (LL_RCC_HSE_IsReady()) {
                                        break;
                                }
                        }

                        if (sys_is_time_expired(tref, TIMEOUT_MS)) {
                                printk("CLK: HSE timeout");
                        }
                }
        }

        //----------------------------------------------------------------------
        // Core voltage regulator setup and FLASH latency
        //----------------------------------------------------------------------
        err = configure_core_voltage_and_flash_latency();
        if (err) {
                goto finish;
        }

        //----------------------------------------------------------------------
        // Setup PLLs clock source
        //----------------------------------------------------------------------
        LL_RCC_PLL_SetSource(__CLK_PLL_SRC__);
        u32_t pllinputfreq = 0;
        u32_t pll_input_range;
        u32_t pll_ouput_range;
        get_pll_input_parameters(&pllinputfreq, &pll_input_range, &pll_ouput_range);

        //----------------------------------------------------------------------
        // PLL1
        //----------------------------------------------------------------------
        if (__CLK_PLL1_ON__) {
                (__CLK_PLL1_P_ENABLE__ == _YES_) ? LL_RCC_PLL1P_Enable() : LL_RCC_PLL1P_Disable();
                (__CLK_PLL1_Q_ENABLE__ == _YES_) ? LL_RCC_PLL1Q_Enable() : LL_RCC_PLL1Q_Disable();
                (__CLK_PLL1_R_ENABLE__ == _YES_) ? LL_RCC_PLL1R_Enable() : LL_RCC_PLL1R_Disable();

                LL_RCC_PLL1_SetP(__CLK_PLL1_P__);
                LL_RCC_PLL1_SetQ(__CLK_PLL1_Q__);
                LL_RCC_PLL1_SetR(__CLK_PLL1_R__);
                LL_RCC_PLL1_SetM(__CLK_PLL1_M__);
                LL_RCC_PLL1_SetN(__CLK_PLL1_N__);
                LL_RCC_PLL1_SetVCOOutputRange(pll_ouput_range);
                LL_RCC_PLL1_SetVCOInputRange(pll_input_range);
                LL_RCC_PLL1_Enable();

                clock_t tref = sys_get_uptime_ms();
                while (not LL_RCC_PLL1_IsReady()) {
                        if (sys_is_time_expired(tref, TIMEOUT_MS)) {
                                printk("CLK: PLL1 timeout");
                                break;
                        }

                        sys_sleep_ms(1);
                }
        }

        //----------------------------------------------------------------------
        // PLL2
        //----------------------------------------------------------------------
        if (__CLK_PLL2_ON__) {
                (__CLK_PLL2_P_ENABLE__ == _YES_) ? LL_RCC_PLL2P_Enable() : LL_RCC_PLL2P_Disable();
                (__CLK_PLL2_Q_ENABLE__ == _YES_) ? LL_RCC_PLL2Q_Enable() : LL_RCC_PLL2Q_Disable();
                (__CLK_PLL2_R_ENABLE__ == _YES_) ? LL_RCC_PLL2R_Enable() : LL_RCC_PLL2R_Disable();

                LL_RCC_PLL2_SetP(__CLK_PLL2_P__);
                LL_RCC_PLL2_SetQ(__CLK_PLL2_Q__);
                LL_RCC_PLL2_SetR(__CLK_PLL2_R__);
                LL_RCC_PLL2_SetM(__CLK_PLL2_M__);
                LL_RCC_PLL2_SetN(__CLK_PLL2_N__);
                LL_RCC_PLL2_SetVCOOutputRange(pll_ouput_range);
                LL_RCC_PLL2_SetVCOInputRange(pll_input_range);
                LL_RCC_PLL2_Enable();

                clock_t tref = sys_get_uptime_ms();
                while (not LL_RCC_PLL2_IsReady()) {
                        if (sys_is_time_expired(tref, TIMEOUT_MS)) {
                                printk("CLK: PLL2 timeout");
                                break;
                        }

                        sys_sleep_ms(1);
                }
        }

        //----------------------------------------------------------------------
        // PLL3
        //----------------------------------------------------------------------
        if (__CLK_PLL3_ON__) {
                (__CLK_PLL3_P_ENABLE__ == _YES_) ? LL_RCC_PLL3P_Enable() : LL_RCC_PLL3P_Disable();
                (__CLK_PLL3_Q_ENABLE__ == _YES_) ? LL_RCC_PLL3Q_Enable() : LL_RCC_PLL3Q_Disable();
                (__CLK_PLL3_R_ENABLE__ == _YES_) ? LL_RCC_PLL3R_Enable() : LL_RCC_PLL3R_Disable();

                LL_RCC_PLL3_SetP(__CLK_PLL3_P__);
                LL_RCC_PLL3_SetQ(__CLK_PLL3_Q__);
                LL_RCC_PLL3_SetR(__CLK_PLL3_R__);
                LL_RCC_PLL3_SetM(__CLK_PLL3_M__);
                LL_RCC_PLL3_SetN(__CLK_PLL3_N__);
                LL_RCC_PLL3_SetVCOOutputRange(pll_ouput_range);
                LL_RCC_PLL3_SetVCOInputRange(pll_input_range);
                LL_RCC_PLL3_Enable();

                clock_t tref = sys_get_uptime_ms();
                while (not LL_RCC_PLL3_IsReady()) {
                        if (sys_is_time_expired(tref, TIMEOUT_MS)) {
                                printk("CLK: PLL3 timeout");
                                break;
                        }

                        sys_sleep_ms(1);
                }
        }

        //----------------------------------------------------------------------
        // Clock prescalers
        //----------------------------------------------------------------------
        LL_RCC_SetSysPrescaler(__CLK_SYSCLK_DIV__);
        LL_RCC_SetAHBPrescaler(__CLK_AHB_DIV__);
        LL_RCC_SetAPB1Prescaler(__CLK_APB1_DIV__);
        LL_RCC_SetAPB2Prescaler(__CLK_APB2_DIV__);
        LL_RCC_SetAPB3Prescaler(__CLK_APB3_DIV__);
        LL_RCC_SetAPB4Prescaler(__CLK_APB4_DIV__);

        //----------------------------------------------------------------------
        // Clock sources
        //----------------------------------------------------------------------
        LL_RCC_SetSysClkSource(__CLK_SYSCLK_SRC__);
        while (LL_RCC_GetSysClkSource() != __CLK_SYSCLK_SRC_STATUS__);

        LL_RCC_SetUSARTClockSource(__CLK_SRC_UART16__);
        LL_RCC_SetUSARTClockSource(__CLK_SRC_UART234578__);
        LL_RCC_SetLPUARTClockSource(__CLK_SRC_LPUART1__);
        LL_RCC_SetI2CClockSource(__CLK_SRC_I2C123__);
        LL_RCC_SetI2CClockSource(__CLK_SRC_I2C4__);
        LL_RCC_SetLPTIMClockSource(__CLK_SRC_LPTIM1__);
        LL_RCC_SetLPTIMClockSource(__CLK_SRC_LPTIM2__);
        LL_RCC_SetLPTIMClockSource(__CLK_SRC_LPTIM345__);
        LL_RCC_SetCLKPClockSource(__CLK_SRC_CLKP__);
        LL_RCC_SetSPDIFClockSource(__CLK_SRC_SPDIF__);
#if defined(RCC_D2CCIP1R_SAI1SEL)
        LL_RCC_SetSAIClockSource(__CLK_SRC_SAI1__);
#endif
#if defined(SAI3)
        LL_RCC_SetSAIClockSource(__CLK_SRC_SAI23__);
#endif
#if defined(RCC_CDCCIP1R_SAI2ASEL)
        LL_RCC_SetSAIClockSource(__CLK_SRC_SAI2A__);
#endif
#if defined(RCC_CDCCIP1R_SAI2BSEL)
        LL_RCC_SetSAIClockSource(__CLK_SRC_SAI2B__);
#endif
#if defined(SAI4_Block_A)
        LL_RCC_SetSAIClockSource(__CLK_SRC_SAI4A__);
#endif
#if defined(SAI4_Block_B)
        LL_RCC_SetSAIClockSource(__CLK_SRC_SAI4B__);
#endif
        LL_RCC_SetSDMMCClockSource(__CLK_SRC_SDMMC__);
        LL_RCC_SetRNGClockSource(__CLK_SRC_RNG__);
        LL_RCC_SetUSBClockSource(__CLK_SRC_USB__);
        LL_RCC_SetCECClockSource(__CLK_SRC_CEC__);
        LL_RCC_SetDFSDMClockSource(__CLK_SRC_DFSDM1__);
#if defined(DFSDM2_BASE)
        LL_RCC_SetDFSDM2ClockSource(__CLK_SRC_DFSDM2__);
#endif
        LL_RCC_SetFMCClockSource(__CLK_SRC_FMC__);
        LL_RCC_SetQSPIClockSource(__CLK_SRC_QSPI__);
#if defined(OCTOSPI1) || defined(OCTOSPI2)
        LL_RCC_SetOSPIClockSource(__CLK_SRC_OSPI__);
#endif
#if defined(DSI)
        LL_RCC_SetDSIClockSource(__CLK_SRC_DSI__);
#endif
        LL_RCC_SetSPIClockSource(__CLK_SRC_SPI123__);
        LL_RCC_SetSPIClockSource(__CLK_SRC_SPI45__);
        LL_RCC_SetSPIClockSource(__CLK_SRC_SPI6__);
        LL_RCC_SetFDCANClockSource(__CLK_SRC_FDCAN__);
        LL_RCC_SetSWPClockSource(__CLK_SRC_SWP__);
        LL_RCC_SetADCClockSource(__CLK_SRC_ADC__);

        //----------------------------------------------------------------------
        // Update OS clocks
        //----------------------------------------------------------------------
        sys_update_system_clocks();

        finish:
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
                                clkinf->name = "CPUCLK";
                                break;
                        }

                        case 1: {
                                LL_RCC_ClocksTypeDef freq;
                                LL_RCC_GetSystemClocksFreq(&freq);
                                clkinf->freq_Hz = freq.SYSCLK_Frequency;
                                clkinf->name = "SYSCLK";
                                break;
                        }

                        case 2: {
                                LL_RCC_ClocksTypeDef freq;
                                LL_RCC_GetSystemClocksFreq(&freq);
                                clkinf->freq_Hz = freq.HCLK_Frequency;
                                clkinf->name = "HCLK";
                                break;
                        }

                        case 3: {
                                LL_RCC_ClocksTypeDef freq;
                                LL_RCC_GetSystemClocksFreq(&freq);
                                clkinf->freq_Hz = freq.PCLK1_Frequency;
                                clkinf->name = "PCLK1";
                                break;
                        }

                        case 4: {
                                LL_RCC_ClocksTypeDef freq;
                                LL_RCC_GetSystemClocksFreq(&freq);
                                clkinf->freq_Hz = freq.PCLK2_Frequency;
                                clkinf->name = "PCLK2";
                                break;
                        }

                        case 5: {
                                LL_RCC_ClocksTypeDef freq;
                                LL_RCC_GetSystemClocksFreq(&freq);
                                clkinf->freq_Hz = freq.PCLK3_Frequency;
                                clkinf->name = "PCLK3";
                                break;
                        }

                        case 6: {
                                LL_RCC_ClocksTypeDef freq;
                                LL_RCC_GetSystemClocksFreq(&freq);
                                clkinf->freq_Hz = freq.PCLK4_Frequency;
                                clkinf->name = "PCLK4";
                                break;
                        }

                        case 7: {
                                LL_PLL_ClocksTypeDef pll;
                                LL_RCC_GetPLL1ClockFreq(&pll);
                                clkinf->freq_Hz = pll.PLL_P_Frequency;
                                clkinf->name    = "PLL1P";
                                break;
                        }

                        case 8: {
                                LL_PLL_ClocksTypeDef pll;
                                LL_RCC_GetPLL1ClockFreq(&pll);
                                clkinf->freq_Hz = pll.PLL_Q_Frequency;
                                clkinf->name    = "PLL1Q";
                                break;
                        }

                        case 9: {
                                LL_PLL_ClocksTypeDef pll;
                                LL_RCC_GetPLL1ClockFreq(&pll);
                                clkinf->freq_Hz = pll.PLL_R_Frequency;
                                clkinf->name    = "PLL1R";
                                break;
                        }

                        case 10: {
                                LL_PLL_ClocksTypeDef pll;
                                LL_RCC_GetPLL2ClockFreq(&pll);
                                clkinf->freq_Hz = pll.PLL_P_Frequency;
                                clkinf->name    = "PLL2P";
                                break;
                        }

                        case 11: {
                                LL_PLL_ClocksTypeDef pll;
                                LL_RCC_GetPLL2ClockFreq(&pll);
                                clkinf->freq_Hz = pll.PLL_Q_Frequency;
                                clkinf->name    = "PLL2Q";
                                break;
                        }

                        case 12: {
                                LL_PLL_ClocksTypeDef pll;
                                LL_RCC_GetPLL2ClockFreq(&pll);
                                clkinf->freq_Hz = pll.PLL_R_Frequency;
                                clkinf->name    = "PLL2R";
                                break;
                        }

                        case 13: {
                                LL_PLL_ClocksTypeDef pll;
                                LL_RCC_GetPLL3ClockFreq(&pll);
                                clkinf->freq_Hz = pll.PLL_P_Frequency;
                                clkinf->name    = "PLL3P";
                                break;
                        }

                        case 14: {
                                LL_PLL_ClocksTypeDef pll;
                                LL_RCC_GetPLL3ClockFreq(&pll);
                                clkinf->freq_Hz = pll.PLL_Q_Frequency;
                                clkinf->name    = "PLL3Q";
                                break;
                        }

                        case 15: {
                                LL_PLL_ClocksTypeDef pll;
                                LL_RCC_GetPLL3ClockFreq(&pll);
                                clkinf->freq_Hz = pll.PLL_R_Frequency;
                                clkinf->name    = "PLL3R";
                                break;
                        }

                        case 16:
                                clkinf->freq_Hz = LL_RCC_GetUSARTClockFreq(LL_RCC_USART16_CLKSOURCE);
                                clkinf->name = "USART1";
                                break;

                        case 17:
                                clkinf->freq_Hz = LL_RCC_GetUSARTClockFreq(LL_RCC_USART234578_CLKSOURCE);
                                clkinf->name = "USART2";
                                break;

                        case 18:
                                clkinf->freq_Hz = LL_RCC_GetUSARTClockFreq(LL_RCC_USART234578_CLKSOURCE);
                                clkinf->name = "USART3";
                                break;

                        case 19:
                                clkinf->freq_Hz = LL_RCC_GetUSARTClockFreq(LL_RCC_USART234578_CLKSOURCE);
                                clkinf->name = "UART4";
                                break;

                        case 20:
                                clkinf->freq_Hz = LL_RCC_GetUSARTClockFreq(LL_RCC_USART234578_CLKSOURCE);
                                clkinf->name = "UART5";
                                break;

                        case 21:
                                clkinf->freq_Hz = LL_RCC_GetUSARTClockFreq(LL_RCC_USART16_CLKSOURCE);
                                clkinf->name = "USART6";
                                break;

                        case 22:
                                clkinf->freq_Hz = LL_RCC_GetUSARTClockFreq(LL_RCC_USART234578_CLKSOURCE);
                                clkinf->name = "UART7";
                                break;

                        case 23:
                                clkinf->freq_Hz = LL_RCC_GetUSARTClockFreq(LL_RCC_USART234578_CLKSOURCE);
                                clkinf->name = "UART8";
                                break;

                        case 24:
                                clkinf->freq_Hz = LL_RCC_GetUSARTClockFreq(LL_RCC_USART16_CLKSOURCE);
                                clkinf->name = "UART9";
                                break;

                        case 25:
                                clkinf->freq_Hz = LL_RCC_GetUSARTClockFreq(LL_RCC_USART16_CLKSOURCE);
                                clkinf->name = "USART10";
                                break;

                        case 26:
                                clkinf->freq_Hz = LL_RCC_GetLPUARTClockFreq(LL_RCC_LPUART1_CLKSOURCE);
                                clkinf->name = "LPUART1";
                                break;

                        case 27:
                                clkinf->freq_Hz = LL_RCC_GetI2CClockFreq(LL_RCC_I2C123_CLKSOURCE);
                                clkinf->name = "I2C1";
                                break;

                        case 28:
                                clkinf->freq_Hz = LL_RCC_GetI2CClockFreq(LL_RCC_I2C123_CLKSOURCE);
                                clkinf->name = "I2C2";
                                break;

                        case 29:
                                clkinf->freq_Hz = LL_RCC_GetI2CClockFreq(LL_RCC_I2C123_CLKSOURCE);
                                clkinf->name = "I2C3";
                                break;

                        case 30:
                                clkinf->freq_Hz = LL_RCC_GetI2CClockFreq(LL_RCC_I2C4_CLKSOURCE);
                                clkinf->name = "I2C4";
                                break;

                        case 31:
                                clkinf->freq_Hz = LL_RCC_GetI2CClockFreq(LL_RCC_I2C123_CLKSOURCE);
                                clkinf->name = "I2C5";
                                break;

                        case 32:
                                clkinf->freq_Hz = LL_RCC_GetLPTIMClockFreq(LL_RCC_LPTIM1_CLKSOURCE);
                                clkinf->name = "LPTIM1";
                                break;

                        case 33:
                                clkinf->freq_Hz = LL_RCC_GetLPTIMClockFreq(LL_RCC_LPTIM2_CLKSOURCE);
                                clkinf->name = "LPTIM2";
                                break;

                        case 34:
                                clkinf->freq_Hz = LL_RCC_GetLPTIMClockFreq(LL_RCC_LPTIM345_CLKSOURCE);
                                clkinf->name = "LPTIM3";
                                break;

                        case 35:
                                clkinf->freq_Hz = LL_RCC_GetLPTIMClockFreq(LL_RCC_LPTIM345_CLKSOURCE);
                                clkinf->name = "LPTIM4";
                                break;

                        case 36:
                                clkinf->freq_Hz = LL_RCC_GetLPTIMClockFreq(LL_RCC_LPTIM345_CLKSOURCE);
                                clkinf->name = "LPTIM5";
                                break;

                        case 37:
                                clkinf->freq_Hz = LL_RCC_GetADCClockFreq(LL_RCC_ADC_CLKSOURCE);
                                clkinf->name = "ADC";
                                break;

                        case 38:
                                clkinf->freq_Hz = LL_RCC_GetSDMMCClockFreq(LL_RCC_SDMMC_CLKSOURCE);
                                clkinf->name = "SDMMC";
                                break;

                        case 39:
                                clkinf->freq_Hz = LL_RCC_GetRNGClockFreq(LL_RCC_RNG_CLKSOURCE);
                                clkinf->name = "RNG";
                                break;

                        case 40:
                                clkinf->freq_Hz = LL_RCC_GetCECClockFreq(LL_RCC_CEC_CLKSOURCE);
                                clkinf->name = "CEC";
                                break;

                        case 41:
                                clkinf->freq_Hz = LL_RCC_GetUSBClockFreq(LL_RCC_USB_CLKSOURCE);
                                clkinf->name = "USB";
                                break;

                        case 42:
                                clkinf->freq_Hz = LL_RCC_GetDFSDMClockFreq(LL_RCC_DFSDM1_CLKSOURCE);
                                clkinf->name = "DFSDM1";
                                break;

                        case 43:
                                clkinf->freq_Hz = LL_RCC_GetSPIClockFreq(LL_RCC_SPI123_CLKSOURCE);
                                clkinf->name = "SPI1";
                                break;

                        case 44:
                                clkinf->freq_Hz = LL_RCC_GetSPIClockFreq(LL_RCC_SPI123_CLKSOURCE);
                                clkinf->name = "SPI2";
                                break;

                        case 45:
                                clkinf->freq_Hz = LL_RCC_GetSPIClockFreq(LL_RCC_SPI123_CLKSOURCE);
                                clkinf->name = "SPI3";
                                break;

                        case 46:
                                clkinf->freq_Hz = LL_RCC_GetSPIClockFreq(LL_RCC_SPI45_CLKSOURCE);
                                clkinf->name = "SPI4";
                                break;

                        case 47:
                                clkinf->freq_Hz = LL_RCC_GetSPIClockFreq(LL_RCC_SPI45_CLKSOURCE);
                                clkinf->name = "SPI5";
                                break;

                        case 48:
                                clkinf->freq_Hz = LL_RCC_GetSPIClockFreq(LL_RCC_SPI6_CLKSOURCE);
                                clkinf->name = "SPI6";
                                break;

                        case 49:
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
 * @brief  Function get PLL input frequency (the same for PLL1, PLL2, PLL3).
 *
 * @return PLL input frequency.
 */
//==============================================================================
static u32_t get_pll_input_freq(void)
{
        u32_t pllinputfreq = 0;

        switch (LL_RCC_PLL_GetSource()) {
        case LL_RCC_PLLSOURCE_HSI:
                if (LL_RCC_HSI_IsReady() != 0U) {
                        pllinputfreq = HSI_VALUE >> (LL_RCC_HSI_GetDivider() >> RCC_CR_HSIDIV_Pos);
                }
                break;

        case LL_RCC_PLLSOURCE_CSI:
                if (LL_RCC_CSI_IsReady() != 0U) {
                        pllinputfreq = CSI_VALUE;
                }
                break;

        case LL_RCC_PLLSOURCE_HSE:
                if (LL_RCC_HSE_IsReady() != 0U) {
                        pllinputfreq = HSE_VALUE;
                }
                break;
        }

        return pllinputfreq;
}

//==============================================================================
/**
 * @brief  Function return PLL input parameters.
 *
 * @param  pllinputfreq         PLL input frequency
 * @param  pll_input_range      PLL input range
 * @param  pll_output_range     PLL output range
 */
//==============================================================================
static void get_pll_input_parameters(u32_t *pll_input_freq, u32_t *pll_input_range, u32_t *pll_ouput_range)
{
        *pll_input_freq = get_pll_input_freq();

        if (*pll_input_freq >= 8000000) {
                *pll_input_range = LL_RCC_PLLINPUTRANGE_8_16;
                *pll_ouput_range = LL_RCC_PLLVCORANGE_WIDE;
        } else if (*pll_input_freq >= 4000000) {
                *pll_input_range = LL_RCC_PLLINPUTRANGE_4_8;
                *pll_ouput_range = LL_RCC_PLLVCORANGE_WIDE;
        } else if (*pll_input_freq >= 2000000) {
                *pll_input_range = LL_RCC_PLLINPUTRANGE_2_4;
                *pll_ouput_range = LL_RCC_PLLVCORANGE_WIDE;
        } else {
                *pll_input_range = LL_RCC_PLLINPUTRANGE_1_2;
                *pll_ouput_range = LL_RCC_PLLVCORANGE_MEDIUM;
        }
}

//==============================================================================
/**
 * @brief  Wait for selected flag in the selected register.
 *
 * @param  reg          pointer to register
 * @param  mask         bit mask
 * @param  timeout_ms   timeout in milliseconds
 * @param  msg          timeout message
 *
 * @return One of errno value.
 */
//==============================================================================
static int wait_for_flag(volatile uint32_t *reg, uint32_t mask, uint32_t timeout_ms, const char *msg)
{
        clock_t tref = sys_get_uptime_ms();
        while (not (*reg & mask)) {
                if (sys_is_time_expired(tref, timeout_ms)) {
                        dev_dbg(hdl, "%s", msg);
                        return ETIME;
                }
                sys_sleep_ms(1);
        }

        return ESUCC;
}

//==============================================================================
/**
 * @brief  Function calculate core voltage regulator parameters and flash wait
 *         states according to target frequency.
 *
 * @return One of errno value.
 */
//==============================================================================
static int configure_core_voltage_and_flash_latency(void)
{
        int   err  = 0;

        u32_t hclk = __CLK_HCLK_FREQ__ / 1000000;

        u32_t FLASH_ACR = FLASH_ACR_WRHIGHFREQ_10 | FLASH_ACR_LATENCY_7WS;
        u32_t VOS = PWR_REGULATOR_VOLTAGE_SCALE3;

        if (hclk <= 45) {
                VOS = PWR_REGULATOR_VOLTAGE_SCALE3;
                FLASH_ACR = FLASH_ACR_LATENCY_0WS | FLASH_ACR_WRHIGHFREQ_00;

        } else if (hclk <= 55) {
                VOS = PWR_REGULATOR_VOLTAGE_SCALE2;
                FLASH_ACR = FLASH_ACR_LATENCY_0WS | FLASH_ACR_WRHIGHFREQ_00;

        } else if (hclk <= 70) {
                VOS = PWR_REGULATOR_VOLTAGE_SCALE1;
                FLASH_ACR = FLASH_ACR_LATENCY_0WS | FLASH_ACR_WRHIGHFREQ_00;

        } else if ((hclk >= 45) && (hclk <= 90)) {
                VOS = PWR_REGULATOR_VOLTAGE_SCALE3;
                FLASH_ACR = FLASH_ACR_LATENCY_1WS | FLASH_ACR_WRHIGHFREQ_01;

        } else if ((hclk >= 55) && (hclk <= 110)) {
                VOS = PWR_REGULATOR_VOLTAGE_SCALE2;
                FLASH_ACR = FLASH_ACR_LATENCY_1WS | FLASH_ACR_WRHIGHFREQ_01;

        } else if ((hclk >= 70) && (hclk <= 140)) {
                VOS = PWR_REGULATOR_VOLTAGE_SCALE1;
                FLASH_ACR = FLASH_ACR_LATENCY_1WS | FLASH_ACR_WRHIGHFREQ_01;

        } else if ((hclk >= 90) && (hclk <= 135)) {
                VOS = PWR_REGULATOR_VOLTAGE_SCALE3;
                FLASH_ACR = FLASH_ACR_LATENCY_2WS | FLASH_ACR_WRHIGHFREQ_01;

        } else if ((hclk >= 110) && (hclk <= 165)) {
                VOS = PWR_REGULATOR_VOLTAGE_SCALE2;
                FLASH_ACR = FLASH_ACR_LATENCY_2WS | FLASH_ACR_WRHIGHFREQ_01;

        } else if ((hclk >= 135) && (hclk <= 180)) {
                VOS = PWR_REGULATOR_VOLTAGE_SCALE3;
                FLASH_ACR = FLASH_ACR_LATENCY_3WS | FLASH_ACR_WRHIGHFREQ_10;

        } else if ((hclk >= 140) && (hclk <= 185)) {
                VOS = PWR_REGULATOR_VOLTAGE_SCALE1;
                FLASH_ACR = FLASH_ACR_LATENCY_2WS | FLASH_ACR_WRHIGHFREQ_01;

        } else if ((hclk >= 165) && (hclk <= 225)) {
                VOS = PWR_REGULATOR_VOLTAGE_SCALE2;
                FLASH_ACR = FLASH_ACR_LATENCY_3WS | FLASH_ACR_WRHIGHFREQ_10;

        } else if ((hclk >= 180) && (hclk <= 225)) {
                VOS = PWR_REGULATOR_VOLTAGE_SCALE3;
                FLASH_ACR = FLASH_ACR_LATENCY_4WS | FLASH_ACR_WRHIGHFREQ_10;

        } else if ((hclk >= 185) && (hclk <= 210)) {
                VOS = PWR_REGULATOR_VOLTAGE_SCALE1;
                FLASH_ACR = FLASH_ACR_LATENCY_2WS | FLASH_ACR_WRHIGHFREQ_10;

        } else if ((hclk >= 210) && (hclk <= 225)) {
                VOS = PWR_REGULATOR_VOLTAGE_SCALE1;
                FLASH_ACR = FLASH_ACR_LATENCY_3WS | FLASH_ACR_WRHIGHFREQ_10;

        } else {
                VOS = PWR_REGULATOR_VOLTAGE_SCALE0;
                FLASH_ACR = FLASH_ACR_LATENCY_4WS | FLASH_ACR_WRHIGHFREQ_10;
        }

        if (  (VOS == PWR_REGULATOR_VOLTAGE_SCALE3)
           || (VOS == PWR_REGULATOR_VOLTAGE_SCALE2)
           || (VOS == PWR_REGULATOR_VOLTAGE_SCALE1)
           || (VOS == PWR_REGULATOR_VOLTAGE_SCALE0) ) {

                err = control_voltage_scaling(PWR_REGULATOR_VOLTAGE_SCALE3);
                if (err) {
                        dev_dbg(hdl, "VOS3 switch error");
                        return err;
                }

                err = wait_for_flag(&PWR->D3CR, PWR_D3CR_VOSRDY, TIMEOUT_MS, "VOS3 switch timeout");
                if (err) {
                        return err;
                }

                err = wait_for_flag(&PWR->CSR1, PWR_CSR1_ACTVOSRDY, TIMEOUT_MS, "VOS3 switch timeout");
                if (err) {
                        return err;
                }
        }

        if (  (VOS == PWR_REGULATOR_VOLTAGE_SCALE2)
           || (VOS == PWR_REGULATOR_VOLTAGE_SCALE1)
           || (VOS == PWR_REGULATOR_VOLTAGE_SCALE0) ) {

                err = control_voltage_scaling(PWR_REGULATOR_VOLTAGE_SCALE2);
                if (err) {
                        dev_dbg(hdl, "VOS2 switch error");
                        return err;
                }

                err = wait_for_flag(&PWR->D3CR, PWR_D3CR_VOSRDY, TIMEOUT_MS, "VOS2 switch timeout");
                if (err) {
                        return err;
                }

                err = wait_for_flag(&PWR->CSR1, PWR_CSR1_ACTVOSRDY, TIMEOUT_MS, "VOS2 switch timeout");
                if (err) {
                        return err;
                }
        }

        if (  (VOS == PWR_REGULATOR_VOLTAGE_SCALE1)
           || (VOS == PWR_REGULATOR_VOLTAGE_SCALE0) ) {

                err = control_voltage_scaling(PWR_REGULATOR_VOLTAGE_SCALE1);
                if (err) {
                        dev_dbg(hdl, "VOS1 switch error");
                        return err;
                }

                err = wait_for_flag(&PWR->D3CR, PWR_D3CR_VOSRDY, TIMEOUT_MS, "VOS1 switch timeout");
                if (err) {
                        return err;
                }

                err = wait_for_flag(&PWR->CSR1, PWR_CSR1_ACTVOSRDY, TIMEOUT_MS, "VOS1 switch timeout");
                if (err) {
                        return err;
                }
        }

        if (VOS == PWR_REGULATOR_VOLTAGE_SCALE0) {

                err = control_voltage_scaling(PWR_REGULATOR_VOLTAGE_SCALE0);
                if (err) {
                        dev_dbg(hdl, "VOS0 switch error");
                        return err;
                }

                err = wait_for_flag(&PWR->D3CR, PWR_D3CR_VOSRDY, TIMEOUT_MS, "VOS0 switch timeout");
                if (err) {
                        return err;
                }

                err = wait_for_flag(&PWR->CSR1, PWR_CSR1_ACTVOSRDY, TIMEOUT_MS, "VOS0 switch timeout");
                if (err) {
                        return err;
                }
        }

        // flash latency and programming delay
        if (!err) {
                clock_t tref = sys_get_uptime_ms();
                while (not sys_is_time_expired(tref, TIMEOUT_MS)) {
                        WRITE_REG(FLASH->ACR, FLASH_ACR);
                        if ((FLASH->ACR & FLASH_ACR_LATENCY_Msk) == FLASH_ACR) {
                                break;
                        }
                }
        }

        return err;
}

//==============================================================================
/**
 * @brief Configure the system Power Supply.
 * @param  SupplySource : Specifies the Power Supply source to set after a
 *                        system startup.
 *         This parameter can be one of the following values :
 *            @arg PWR_DIRECT_SMPS_SUPPLY : The SMPS supplies the Vcore Power
 *                                          Domains. The LDO is Bypassed.
 *            @arg PWR_SMPS_1V8_SUPPLIES_LDO : The SMPS 1.8V output supplies
 *                                             the LDO. The Vcore Power Domains
 *                                             are supplied from the LDO.
 *            @arg PWR_SMPS_2V5_SUPPLIES_LDO : The SMPS 2.5V output supplies
 *                                             the LDO. The Vcore Power Domains
 *                                             are supplied from the LDO.
 *            @arg PWR_SMPS_1V8_SUPPLIES_EXT_AND_LDO : The SMPS 1.8V output
 *                                                     supplies external
 *                                                     circuits and the LDO.
 *                                                     The Vcore Power Domains
 *                                                     are supplied from the
 *                                                     LDO.
 *            @arg PWR_SMPS_2V5_SUPPLIES_EXT_AND_LDO : The SMPS 2.5V output
 *                                                     supplies external
 *                                                     circuits and the LDO.
 *                                                     The Vcore Power Domains
 *                                                     are supplied from the
 *                                                     LDO.
 *            @arg PWR_SMPS_1V8_SUPPLIES_EXT : The SMPS 1.8V output supplies
 *                                             external circuits. The LDO is
 *                                             Bypassed. The Vcore Power
 *                                             Domains are supplied from
 *                                             external source.
 *            @arg PWR_SMPS_2V5_SUPPLIES_EXT : The SMPS 2.5V output supplies
 *                                             external circuits. The LDO is
 *                                             Bypassed. The Vcore Power
 *                                             Domains are supplied from
 *                                             external source.
 *            @arg PWR_LDO_SUPPLY : The LDO regulator supplies the Vcore Power
 *                                  Domains. The SMPS regulator is Bypassed.
 *            @arg PWR_EXTERNAL_SOURCE_SUPPLY : The SMPS and the LDO are
 *                                              Bypassed. The Vcore Power
 *                                              Domains are supplied from
 *                                              external source.
 * @note   The PWR_LDO_SUPPLY and PWR_EXTERNAL_SOURCE_SUPPLY are used by all
 *         H7 lines.
 *         The PWR_DIRECT_SMPS_SUPPLY, PWR_SMPS_1V8_SUPPLIES_LDO,
 *         PWR_SMPS_2V5_SUPPLIES_LDO, PWR_SMPS_1V8_SUPPLIES_EXT_AND_LDO,
 *         PWR_SMPS_2V5_SUPPLIES_EXT_AND_LDO, PWR_SMPS_1V8_SUPPLIES_EXT and
 *         PWR_SMPS_2V5_SUPPLIES_EXT are used only for lines that supports SMPS
 *         regulator.
 * @retval HAL status.
 */
//==============================================================================
static int configure_power_supply(uint32_t supply_source)
{
        /* Check if supply source was configured */
#if defined (PWR_CR3_SCUEN)
        if ((PWR->CR3 & PWR_CR3_SCUEN) == 0U)
#else
        if ((PWR->CR3 & (PWR_CR3_SMPSEN | PWR_CR3_LDOEN | PWR_CR3_BYPASS)) != (PWR_CR3_SMPSEN | PWR_CR3_LDOEN))
#endif
        {
                if ((PWR->CR3 & PWR_SUPPLY_CONFIG_MASK) != supply_source) {
                        /* Supply configuration update locked, can't apply a new supply config */
                        dev_dbg(hdl, "supply configuration update locked");
                        return EIO;
                } else {
                        /*
                          Supply configuration update locked, but new supply configuration
                          matches with old supply configuration : nothing to do
                         */
                        return ESUCC;
                }
        }

        MODIFY_REG (PWR->CR3, PWR_SUPPLY_CONFIG_MASK, supply_source);

        int err = wait_for_flag(&PWR->CSR1, PWR_CSR1_ACTVOSRDY, TIMEOUT_MS,
                                "configure supply timeout");
        if (err) {
                return err;
        }

#if defined (SMPS)
        /* When the SMPS supplies external circuits verify that SDEXTRDY flag is set */
        if ((supply_source == PWR_SMPS_1V8_SUPPLIES_EXT_AND_LDO) ||
            (supply_source == PWR_SMPS_2V5_SUPPLIES_EXT_AND_LDO) ||
            (supply_source == PWR_SMPS_1V8_SUPPLIES_EXT)         ||
            (supply_source == PWR_SMPS_2V5_SUPPLIES_EXT)) {

                err = wait_for_flag(&PWR->CR3, PWR_CR3_SMPSEXTRDY, TIMEOUT_MS,
                                    "run SMPS timeout");
                if (err) {
                        return err;
                }
        }
#endif

        return ESUCC;
}

//==============================================================================
/**
 * @brief Configure the main internal regulator output voltage.
 * @param  VoltageScaling : Specifies the regulator output voltage to achieve
 *                          a tradeoff between performance and power
 *                          consumption.
 *          This parameter can be one of the following values :
 *            @arg PWR_REGULATOR_VOLTAGE_SCALE0 : Regulator voltage output
 *                                                Scale 0 mode.
 *            @arg PWR_REGULATOR_VOLTAGE_SCALE1 : Regulator voltage output
 *                                                range 1 mode.
 *            @arg PWR_REGULATOR_VOLTAGE_SCALE2 : Regulator voltage output
 *                                                range 2 mode.
 *            @arg PWR_REGULATOR_VOLTAGE_SCALE3 : Regulator voltage output
 *                                                range 3 mode.
 * @note   For STM32H74x and STM32H75x lines, configuring Voltage Scale 0 is
 *         only possible when Vcore is supplied from LDO (Low DropOut). The
 *         SYSCFG Clock must be enabled through __HAL_RCC_SYSCFG_CLK_ENABLE()
 *         macro before configuring Voltage Scale 0.
 *         To enter low power mode , and if current regulator voltage is
 *         Voltage Scale 0 then first switch to Voltage Scale 1 before entering
 *         low power mode.
 * @retval HAL Status
 */
//==============================================================================
int control_voltage_scaling(uint32_t voltage_scaling)
{
        /* Get the voltage scaling  */
        if ((PWR->CSR1 & PWR_CSR1_ACTVOS) == voltage_scaling) {
                return ESUCC;
        }

#if defined (PWR_SRDCR_VOS)
        /* Set the voltage range */
        MODIFY_REG (PWR->SRDCR, PWR_SRDCR_VOS, voltage_scaling);
#else
#if defined(SYSCFG_PWRCR_ODEN) /* STM32H74xxx and STM32H75xxx lines */
        if (voltage_scaling == PWR_REGULATOR_VOLTAGE_SCALE0) {

                if ((PWR->CR3 & PWR_CR3_LDOEN) == PWR_CR3_LDOEN) {
                        MODIFY_REG (PWR->D3CR, PWR_D3CR_VOS, PWR_REGULATOR_VOLTAGE_SCALE1);

                        int err = wait_for_flag(&PWR->CSR1, PWR_CSR1_ACTVOSRDY, TIMEOUT_MS,
                                                "voltage scale switch timeout");
                        if (err) {
                                return err;
                        }

                        SET_BIT(RCC->APB4ENR, RCC_APB4ENR_SYSCFGEN);
                        SET_BIT(SYSCFG->PWRCR, SYSCFG_PWRCR_ODEN);
                        dev_dbg(hdl, "enabled power overdrive");
                } else {
                        dev_dbg(hdl, "VOS0 is only possible when LDO is enabled");
                        return EIO;
                }
        } else {
                if ((PWR->CSR1 & PWR_CSR1_ACTVOS) == PWR_REGULATOR_VOLTAGE_SCALE1) {

                        if ((SYSCFG->PWRCR & SYSCFG_PWRCR_ODEN) != 0U) {
                                CLEAR_BIT(SYSCFG->PWRCR, SYSCFG_PWRCR_ODEN);

                                int err = wait_for_flag(&PWR->CSR1, PWR_CSR1_ACTVOSRDY, TIMEOUT_MS,
                                                        "voltage scale switch timeout");
                                if (err) {
                                        return err;
                                }
                        }
                }

                /* Set the voltage range */
                MODIFY_REG (PWR->D3CR, PWR_D3CR_VOS, voltage_scaling);
        }
#else  /* STM32H72xxx and STM32H73xxx lines */
        MODIFY_REG(PWR->D3CR, PWR_D3CR_VOS, voltage_scaling);
#endif /* defined (SYSCFG_PWRCR_ODEN) */
#endif /* defined (PWR_SRDCR_VOS) */

        int err = wait_for_flag(&PWR->CSR1, PWR_CSR1_ACTVOSRDY, TIMEOUT_MS,
                                "voltage scale switch timeout");
        if (err) {
                return err;
        }

        return ESUCC;
}

/*==============================================================================
  End of file
==============================================================================*/
