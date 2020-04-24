/*==============================================================================
File    pwm.c

Author  Daniel Zorychta

Brief   PWM driver

        Copyright (C) 2018 Daniel Zorychta <daniel.zorychta@gmail.com>

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

==============================================================================*/

/*==============================================================================
  Include files
==============================================================================*/
#include "drivers/driver.h"
#include "../pwm_ioctl.h"
#include "../stm32fx/pwm_cfg.h"

#if defined(ARCH_stm32f1)
#include "stm32f1/lib/stm32f10x_rcc.h"
#include "stm32f1/stm32f10x.h"
#elif defined(ARCH_stm32f4)
#include "stm32f4/lib/stm32f4xx_rcc.h"
#include "stm32f4/stm32f4xx.h"
#endif

/*==============================================================================
  Local macros
==============================================================================*/
#define TIM_NULL        {.reg = NULL, .APBENR = NULL, .APBENR_TIMEN = 0}

/*==============================================================================
  Local object types
==============================================================================*/
#if defined(ARCH_stm32f1)
typedef TIM_t TIM_TypeDef;
#elif defined(ARCH_stm32f4)
#endif

typedef struct {
        TIM_TypeDef   *reg;
        __IO uint32_t *APBENR;
        uint32_t       APBENR_TIMEN;
} TIM_addr_t;

typedef struct {
        u8_t timer;
        u8_t channel;
} PWM_t;

/*==============================================================================
  Local function prototypes
==============================================================================*/
static u32_t get_timer_frequency(PWM_t *hdl);

/*==============================================================================
  Local object
==============================================================================*/
MODULE_NAME(PWM);

static const TIM_addr_t TIM[] = {
#ifdef RCC_APB2ENR_TIM1EN
        {.reg = TIM1,  .APBENR = &RCC->APB2ENR, .APBENR_TIMEN = RCC_APB2ENR_TIM1EN},
#else
        TIM_NULL,
#endif
#ifdef RCC_APB1ENR_TIM2EN
        {.reg = TIM2,  .APBENR = &RCC->APB1ENR, .APBENR_TIMEN = RCC_APB1ENR_TIM2EN},
#else
        TIM_NULL,
#endif
#ifdef RCC_APB1ENR_TIM3EN
        {.reg = TIM3,  .APBENR = &RCC->APB1ENR, .APBENR_TIMEN = RCC_APB1ENR_TIM3EN},
#else
        TIM_NULL,
#endif
#ifdef RCC_APB1ENR_TIM4EN
        {.reg = TIM4,  .APBENR = &RCC->APB1ENR, .APBENR_TIMEN = RCC_APB1ENR_TIM4EN},
#else
        TIM_NULL,
#endif
#ifdef RCC_APB1ENR_TIM5EN
        {.reg = TIM5,  .APBENR = &RCC->APB1ENR, .APBENR_TIMEN = RCC_APB1ENR_TIM5EN},
#else
        TIM_NULL,
#endif
#ifdef RCC_APB1ENR_TIM6EN
        {.reg = TIM6,  .APBENR = &RCC->APB1ENR, .APBENR_TIMEN = RCC_APB1ENR_TIM6EN},
#else
        TIM_NULL,
#endif
#ifdef RCC_APB1ENR_TIM7EN
        {.reg = TIM7,  .APBENR = &RCC->APB1ENR, .APBENR_TIMEN = RCC_APB1ENR_TIM7EN},
#else
        TIM_NULL,
#endif
#ifdef RCC_APB2ENR_TIM8EN
        {.reg = TIM8,  .APBENR = &RCC->APB2ENR, .APBENR_TIMEN = RCC_APB2ENR_TIM8EN},
#else
        TIM_NULL,
#endif
#ifdef RCC_APB2ENR_TIM9EN
        {.reg = TIM9,  .APBENR = &RCC->APB2ENR, .APBENR_TIMEN = RCC_APB2ENR_TIM9EN},
#else
        TIM_NULL,
#endif
#ifdef RCC_APB2ENR_TIM10EN
        {.reg = TIM10, .APBENR = &RCC->APB2ENR, .APBENR_TIMEN = RCC_APB2ENR_TIM10EN},
#else
        TIM_NULL,
#endif
#ifdef RCC_APB2ENR_TIM11EN
        {.reg = TIM11, .APBENR = &RCC->APB2ENR, .APBENR_TIMEN = RCC_APB2ENR_TIM11EN},
#else
        TIM_NULL,
#endif
#ifdef RCC_APB1ENR_TIM12EN
        {.reg = TIM12, .APBENR = &RCC->APB1ENR, .APBENR_TIMEN = RCC_APB1ENR_TIM12EN},
#else
        TIM_NULL,
#endif
#ifdef RCC_APB1ENR_TIM13EN
        {.reg = TIM13, .APBENR = &RCC->APB1ENR, .APBENR_TIMEN = RCC_APB1ENR_TIM13EN},
#else
        TIM_NULL,
#endif
#ifdef RCC_APB1ENR_TIM14EN
        {.reg = TIM14, .APBENR = &RCC->APB1ENR, .APBENR_TIMEN = RCC_APB1ENR_TIM14EN},
#else
        TIM_NULL,
#endif
};

/*==============================================================================
  Exported object
==============================================================================*/

/*==============================================================================
  Function definitions
==============================================================================*/

//==============================================================================
/**
 * @brief Initialize device.
 *
 * @param[out]          **device_handle        device allocated memory
 * @param[in ]            major                major device number
 * @param[in ]            minor                minor device number
 * @param[in ]            config               optional module configuration
 *
 * @return One of errno value (errno.h).
 */
//==============================================================================
API_MOD_INIT(PWM, void **device_handle, u8_t major, u8_t minor, const void *config)
{
        int err = EFAULT;

        if ((major < ARRAY_SIZE(TIM)) && (minor < 4) && TIM[major].reg) {

                err = sys_zalloc(sizeof(PWM_t), device_handle);
                if (!err) {
                        PWM_t *hdl   = *device_handle;
                        hdl->timer   = major;
                        hdl->channel = minor;

                        SET_BIT(*TIM[hdl->timer].APBENR, TIM[hdl->timer].APBENR_TIMEN);

                        switch (hdl->channel) {
                        case 0:
                                TIM[hdl->timer].reg->CCMR1 |= TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_1;
                                TIM[hdl->timer].reg->CCER  |= TIM_CCER_CC1E;
                                TIM[hdl->timer].reg->CCR1   = 0;
                                break;
                        case 1:
                                TIM[hdl->timer].reg->CCMR1 |= TIM_CCMR1_OC2M_2 | TIM_CCMR1_OC2M_1;
                                TIM[hdl->timer].reg->CCER  |= TIM_CCER_CC2E;
                                TIM[hdl->timer].reg->CCR2   = 0;
                                break;
                        case 2:
                                TIM[hdl->timer].reg->CCMR2 |= TIM_CCMR2_OC3M_2 | TIM_CCMR2_OC3M_1;
                                TIM[hdl->timer].reg->CCER  |= TIM_CCER_CC3E;
                                TIM[hdl->timer].reg->CCR3   = 0;
                                break;
                        case 3:
                                TIM[hdl->timer].reg->CCMR2 |= TIM_CCMR2_OC4M_2 | TIM_CCMR2_OC4M_1;
                                TIM[hdl->timer].reg->CCER  |= TIM_CCER_CC4E;
                                TIM[hdl->timer].reg->CCR4   = 0;
                                break;
                        }

                        TIM[hdl->timer].reg->BDTR  |= TIM_BDTR_MOE;
                        TIM[hdl->timer].reg->CR1   |= TIM_CR1_CEN;

                        if (config) {
                                const PWM_config_t *conf = config;

                                TIM[hdl->timer].reg->PSC = conf->prescaler > 0
                                                         ? conf->prescaler - 1
                                                         : conf->prescaler;
                                TIM[hdl->timer].reg->ARR = conf->reload;
                                TIM[hdl->timer].reg->CNT = 0;
                        }
                }
        }

        return err;
}

//==============================================================================
/**
 * @brief Release device.
 *
 * @param[in ]          *device_handle          device allocated memory
 *
 * @return One of errno value (errno.h).
 */
//==============================================================================
API_MOD_RELEASE(PWM, void *device_handle)
{
        PWM_t *hdl = device_handle;

        switch (hdl->channel) {
        case 0:
                CLEAR_BIT(TIM[hdl->timer].reg->CCMR1, TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_1);
                CLEAR_BIT(TIM[hdl->timer].reg->CCER, TIM_CCER_CC1E);
                TIM[hdl->timer].reg->CCR1 = 0;
                break;
        case 1:
                CLEAR_BIT(TIM[hdl->timer].reg->CCMR1, TIM_CCMR1_OC2M_2 | TIM_CCMR1_OC2M_1);
                CLEAR_BIT(TIM[hdl->timer].reg->CCER, TIM_CCER_CC2E);
                TIM[hdl->timer].reg->CCR2 = 0;
                break;
        case 2:
                CLEAR_BIT(TIM[hdl->timer].reg->CCMR2, TIM_CCMR2_OC3M_2 | TIM_CCMR2_OC3M_1);
                CLEAR_BIT(TIM[hdl->timer].reg->CCER, TIM_CCER_CC3E);
                TIM[hdl->timer].reg->CCR3 = 0;
                break;
        case 3:
                CLEAR_BIT(TIM[hdl->timer].reg->CCMR2, TIM_CCMR2_OC4M_2 | TIM_CCMR2_OC4M_1);
                CLEAR_BIT(TIM[hdl->timer].reg->CCER, TIM_CCER_CC4E);
                TIM[hdl->timer].reg->CCR4 = 0;
                break;
        }

        if ( (TIM[hdl->timer].reg->CCER & ( TIM_CCER_CC1E
                                          | TIM_CCER_CC2E
                                          | TIM_CCER_CC3E
                                          | TIM_CCER_CC4E) ) == 0) {

                CLEAR_BIT(TIM[hdl->timer].reg->BDTR, TIM_BDTR_MOE);
                CLEAR_BIT(TIM[hdl->timer].reg->CR1, TIM_CR1_CEN);
                CLEAR_BIT(*TIM[hdl->timer].APBENR, TIM[hdl->timer].APBENR_TIMEN);
        }

        return sys_free(&device_handle);
}

//==============================================================================
/**
 * @brief Open device.
 *
 * @param[in ]          *device_handle          device allocated memory
 * @param[in ]           flags                  file operation flags (O_RDONLY, O_WRONLY, O_RDWR)
 *
 * @return One of errno value (errno.h).
 */
//==============================================================================
API_MOD_OPEN(PWM, void *device_handle, u32_t flags)
{
        UNUSED_ARG2(device_handle, flags);
        return ESUCC;
}

//==============================================================================
/**
 * @brief Close device.
 *
 * @param[in ]          *device_handle          device allocated memory
 * @param[in ]           force                  device force close (true)
 *
 * @return One of errno value (errno.h).
 */
//==============================================================================
API_MOD_CLOSE(PWM, void *device_handle, bool force)
{
        UNUSED_ARG2(device_handle, force);
        return ESUCC;
}

//==============================================================================
/**
 * @brief Write data to device.
 *
 * @param[in ]          *device_handle          device allocated memory
 * @param[in ]          *src                    data source
 * @param[in ]           count                  number of bytes to write
 * @param[in ][out]     *fpos                   file position
 * @param[out]          *wrcnt                  number of written bytes
 * @param[in ]           fattr                  file attributes
 *
 * @return One of errno value (errno.h).
 */
//==============================================================================
API_MOD_WRITE(PWM,
              void             *device_handle,
              const u8_t       *src,
              size_t            count,
              fpos_t           *fpos,
              size_t           *wrcnt,
              struct vfs_fattr  fattr)
{
        UNUSED_ARG1(fattr);

        PWM_t   *hdl = device_handle;
        int      err = ESUCC;
        uint16_t val = (count == 1) ? *src : *cast(u16_t*, src);

        if (count > 0) {
                switch (hdl->channel) {
                case 0: TIM[hdl->timer].reg->CCR1 = val; break;
                case 1: TIM[hdl->timer].reg->CCR2 = val; break;
                case 2: TIM[hdl->timer].reg->CCR3 = val; break;
                case 3: TIM[hdl->timer].reg->CCR4 = val; break;
                }

                *wrcnt = 2;
        }

        *fpos = 0;

        return err;
}

//==============================================================================
/**
 * @brief Read data from device.
 *
 * @param[in ]          *device_handle          device allocated memory
 * @param[out]          *dst                    data destination
 * @param[in ]           count                  number of bytes to read
 * @param[in ][out]     *fpos                   file position
 * @param[out]          *rdcnt                  number of read bytes
 * @param[in ]           fattr                  file attributes
 *
 * @return One of errno value (errno.h).
 */
//==============================================================================
API_MOD_READ(PWM,
             void            *device_handle,
             u8_t            *dst,
             size_t           count,
             fpos_t          *fpos,
             size_t          *rdcnt,
             struct vfs_fattr fattr)
{
        UNUSED_ARG1(fattr);

        PWM_t   *hdl = device_handle;
        int      err = ESUCC;
        uint16_t val = 0;

        if (count > 0) {
                switch (hdl->channel) {
                case 0: val = TIM[hdl->timer].reg->CCR1; break;
                case 1: val = TIM[hdl->timer].reg->CCR2; break;
                case 2: val = TIM[hdl->timer].reg->CCR3; break;
                case 3: val = TIM[hdl->timer].reg->CCR4; break;
                }

                if (count > 0) dst[0] = val & 0xFF;
                if (count > 1) dst[1] = val >> 8;

                *rdcnt = min(count, 2);
        }

        *fpos = 0;

        return err;
}

//==============================================================================
/**
 * @brief IO control.
 *
 * @param[in ]          *device_handle          device allocated memory
 * @param[in ]           request                request
 * @param[in ][out]     *arg                    request's argument
 *
 * @return One of errno value (errno.h).
 */
//==============================================================================
API_MOD_IOCTL(PWM, void *device_handle, int request, void *arg)
{
        PWM_t *hdl = device_handle;

        int err = EBADRQC;

        switch (request) {
        case IOCTL_PWM__SET_CONFIGURATION:
                if (arg) {
                        const PWM_config_t *conf = arg;

                        TIM[hdl->timer].reg->PSC = conf->prescaler > 0
                                                 ? conf->prescaler - 1
                                                 : conf->prescaler;
                        TIM[hdl->timer].reg->ARR = conf->reload;
                        TIM[hdl->timer].reg->CNT = 0;

                        err = ESUCC;
                } else {
                        err = EINVAL;
                }
                break;

        case IOCTL_PWM__AUTO_ADJUST:
                if (arg) {
                        const PWM_auto_adjust_t *adj = arg;

                        u32_t freq = get_timer_frequency(hdl);
                        u32_t pre  = (freq / (adj->reload + 1) / adj->frequency);

                        TIM[hdl->timer].reg->ARR = adj->reload;
                        TIM[hdl->timer].reg->PSC = pre > 0 ? pre - 1 : pre;
                        TIM[hdl->timer].reg->CNT = 0;

                        err = ESUCC;

                } else {
                        err = EINVAL;
                }
                break;

        case IOCTL_PWM__GET_BASE_FREQUENCY:
                if (arg) {
                        *cast(u32_t*,arg) = get_timer_frequency(hdl);
                        err = ESUCC;

                } else {
                        err = EINVAL;
                }
                break;

        case IOCTL_PWM__SET_VALUE:
                if (arg) {
                        u16_t val = *cast(u16_t*, arg);

                        switch (hdl->channel) {
                        case 0: TIM[hdl->timer].reg->CCR1 = val; break;
                        case 1: TIM[hdl->timer].reg->CCR2 = val; break;
                        case 2: TIM[hdl->timer].reg->CCR3 = val; break;
                        case 3: TIM[hdl->timer].reg->CCR4 = val; break;
                        }

                        err = ESUCC;
                } else {
                        err = EINVAL;
                }
                break;

        case IOCTL_PWM__GET_VALUE:
                if (arg) {
                        u16_t val = 0;
                        switch (hdl->channel) {
                        case 0: val = TIM[hdl->timer].reg->CCR1; break;
                        case 1: val = TIM[hdl->timer].reg->CCR2; break;
                        case 2: val = TIM[hdl->timer].reg->CCR3; break;
                        case 3: val = TIM[hdl->timer].reg->CCR4; break;
                        }

                        *cast(u16_t*, arg) = val;

                        err = ESUCC;
                } else {
                        err = EINVAL;
                }
                break;

        case IOCTL_PWM__GET_RELOAD:
                if (arg) {
                        *cast(u16_t*, arg) = TIM[hdl->timer].reg->ARR;

                        err = ESUCC;
                } else {
                        err = EINVAL;
                }
                break;
        }

        return err;
}

//==============================================================================
/**
 * @brief Flush device.
 *
 * @param[in ]          *device_handle          device allocated memory
 *
 * @return One of errno value (errno.h).
 */
//==============================================================================
API_MOD_FLUSH(PWM, void *device_handle)
{
        UNUSED_ARG1(device_handle);
        return ESUCC;
}

//==============================================================================
/**
 * @brief Device information.
 *
 * @param[in ]          *device_handle          device allocated memory
 * @param[out]          *device_stat            device status
 *
 * @return One of errno value (errno.h).
 */
//==============================================================================
API_MOD_STAT(PWM, void *device_handle, struct vfs_dev_stat *device_stat)
{
        UNUSED_ARG1(device_handle);

        device_stat->st_size = 2;

        return ESUCC;
}

//==============================================================================
/**
 * @brief  Function return source frequency of selected timer.
 *
 * @param  hdl          device handle
 *
 * @return Source frequency in Hz.
 */
//==============================================================================
static u32_t get_timer_frequency(PWM_t *hdl)
{
        RCC_ClocksTypeDef freq;
        RCC_GetClocksFreq(&freq);

        if (TIM[hdl->timer].APBENR == &RCC->APB1ENR) {

                if (freq.SYSCLK_Frequency != freq.PCLK1_Frequency) {
                        return freq.PCLK1_Frequency * 2;
                } else {
                        return freq.PCLK1_Frequency;
                }
        } else {
                if (freq.SYSCLK_Frequency != freq.PCLK2_Frequency) {
                        return freq.PCLK2_Frequency * 2;
                } else {
                        return freq.PCLK2_Frequency;
                }
        }
}

/*==============================================================================
  End of file
==============================================================================*/
