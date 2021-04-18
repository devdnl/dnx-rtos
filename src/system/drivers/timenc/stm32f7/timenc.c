/*==============================================================================
File    timenc.c

Author  Daniel Zorychta

Brief   Timer driven AB encoder

        Copyright (C) 2021 Daniel Zorychta <daniel.zorychta@gmail.com>

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
#include "stm32f7/timenc_cfg.h"
#include "gpio/gpio_ddi.h"
#include "../timenc_ioctl.h"
#include "stm32f7/lib/stm32f7xx_ll_rcc.h"
#include "stm32f7/stm32f7xx.h"

/*==============================================================================
  Local macros
==============================================================================*/
#define TIMER           TIM7
#define TIM_NULL        {.TIM = NULL, .APBENR = NULL, .APBENR_TIMEN = 0, .APBRSTR = NULL, .APBRSTR_TIMRST}

/*==============================================================================
  Local object types
==============================================================================*/
typedef struct {
        TIM_TypeDef   *TIM;
        __IO uint32_t *APBENR;
        __IO uint32_t *APBRSTR;
        uint32_t       APBENR_TIMEN;
        uint32_t       APBRSTR_TIMRST;
        uint32_t       IRQn;
} TIM_addr_t;

struct base;

typedef struct {
        u8_t major;
        u8_t minor;
        u8_t last_state;
        int32_t counter;
        GPIO_pin_in_port_t pin_A;
        GPIO_pin_in_port_t pin_B;
        struct base *base;
} TIMENC_t;

typedef struct base {
        u8_t      encoders;
        TIMENC_t *encoder[__TIMENC_MAX_ENCODERS__];
} BASE_t;

/*==============================================================================
  Local function prototypes
==============================================================================*/
static u32_t get_timer_frequency(void);

/*==============================================================================
  Local object
==============================================================================*/
MODULE_NAME(TIMENC);
static BASE_t *BASE;

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
API_MOD_INIT(TIMENC, void **device_handle, u8_t major, u8_t minor, const void *config)
{
        int err = EFAULT;

        if ((major == 0) && (minor < __TIMENC_MAX_ENCODERS__)) {

                if (BASE == NULL) {
                        err = sys_zalloc2(sizeof(*BASE), NULL, 0, 0, cast(void**, &BASE));
                        if (!err) {
                                SET_BIT(RCC->APB1ENR, RCC_APB1ENR_TIM7EN);
                                SET_BIT(RCC->APB1RSTR, RCC_APB1RSTR_TIM7RST);
                                CLEAR_BIT(RCC->APB1RSTR, RCC_APB1RSTR_TIM7RST);

                                u32_t freq = get_timer_frequency();
                                if (freq >= 1000000UL) {
                                        TIMER->ARR = (1000000UL / __TIMENC_FREQ_HZ__);
                                        TIMER->PSC = (freq / 1000000UL) - 1; // 1us base
                                        TIMER->CNT = 0;
                                        SET_BIT(TIMER->DIER, TIM_DIER_UIE);
                                        SET_BIT(TIMER->CR1, TIM_CR1_CEN);

                                        NVIC_ClearPendingIRQ(TIM7_IRQn);
                                        NVIC_EnableIRQ(TIM7_IRQn);
                                        NVIC_SetPriority(TIM7_IRQn, _CPU_IRQ_SAFE_PRIORITY_);

                                } else {
                                        printk("%s-%u: Too low peripheral frequency!", GET_MODULE_NAME(), major);
                                }
                        }

                } else {
                        err = 0;
                }

                if (!err) {
                        TIMENC_t *hdl;
                        err = sys_zalloc2(sizeof(*hdl), NULL, 0, 0, cast(void**, &hdl));
                        if (!err) {
                                *device_handle = hdl;
                                hdl->major = major;
                                hdl->minor = minor;
                                hdl->base  = BASE;

                                if (config) {
                                        const TIMENC_config_t *conf = config;
                                        hdl->pin_A = conf->pin_A;
                                        hdl->pin_B = conf->pin_B;
                                }

                                hdl->base->encoder[hdl->base->encoders++] = hdl;

                        } else {
                                if (BASE->encoders == 0) {
                                        SET_BIT(RCC->APB1RSTR, RCC_APB1RSTR_TIM7RST);
                                        CLEAR_BIT(RCC->APB1RSTR, RCC_APB1RSTR_TIM7RST);
                                        CLEAR_BIT(RCC->APB1ENR, RCC_APB1ENR_TIM7EN);

                                        NVIC_ClearPendingIRQ(TIM7_IRQn);
                                        NVIC_DisableIRQ(TIM7_IRQn);
                                        NVIC_SetPriority(TIM7_IRQn, _CPU_IRQ_SAFE_PRIORITY_);

                                        sys_free(cast(void**, &BASE));
                                }
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
API_MOD_RELEASE(TIMENC, void *device_handle)
{
        sys_critical_section_begin();

        int err = sys_free(&device_handle);
        if (!err) {
                if (--BASE->encoders == 0) {
                        SET_BIT(RCC->APB1RSTR, RCC_APB1RSTR_TIM7RST);
                        CLEAR_BIT(RCC->APB1RSTR, RCC_APB1RSTR_TIM7RST);
                        CLEAR_BIT(RCC->APB1ENR, RCC_APB1ENR_TIM7EN);

                        NVIC_ClearPendingIRQ(TIM7_IRQn);
                        NVIC_DisableIRQ(TIM7_IRQn);

                        sys_free(cast(void**, &BASE));
                }
        }

        sys_critical_section_end();

        return err;
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
API_MOD_OPEN(TIMENC, void *device_handle, u32_t flags)
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
API_MOD_CLOSE(TIMENC, void *device_handle, bool force)
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
 * @param[out]          *wrctr                  number of written bytes
 * @param[in ]           fattr                  file attributes
 *
 * @return One of errno value (errno.h).
 */
//==============================================================================
API_MOD_WRITE(TIMENC,
              void             *device_handle,
              const u8_t       *src,
              size_t            count,
              fpos_t           *fpos,
              size_t           *wrctr,
              struct vfs_fattr  fattr)
{
        UNUSED_ARG2(fpos, fattr);

        TIMENC_t *hdl = device_handle;

        int err = EINVAL;

        if (count >= sizeof(hdl->counter)) {
                hdl->counter = *cast(i32_t*, src);
                *wrctr = sizeof(hdl->counter);
                err = ESUCC;
        }

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
 * @param[out]          *rdctr                  number of read bytes
 * @param[in ]           fattr                  file attributes
 *
 * @return One of errno value (errno.h).
 */
//==============================================================================
API_MOD_READ(TIMENC,
             void            *device_handle,
             u8_t            *dst,
             size_t           count,
             fpos_t          *fpos,
             size_t          *rdctr,
             struct vfs_fattr fattr)
{
        UNUSED_ARG2(fpos, fattr);

        TIMENC_t *hdl = device_handle;

        int err = EINVAL;

        if (count >= sizeof(hdl->counter)) {
                *cast(i32_t*, dst) = hdl->counter;
                *rdctr = sizeof(hdl->counter);
                err = ESUCC;
        }

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
API_MOD_IOCTL(TIMENC, void *device_handle, int request, void *arg)
{
        TIMENC_t *hdl = device_handle;

        int err = EBADRQC;

        switch (request) {
        case IOCTL_TIMENC__CONFIGURE:
                hdl->pin_A = cast(const TIMENC_config_t*, arg)->pin_A;
                hdl->pin_B = cast(const TIMENC_config_t*, arg)->pin_B;
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
API_MOD_FLUSH(TIMENC, void *device_handle)
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
API_MOD_STAT(TIMENC, void *device_handle, struct vfs_dev_stat *device_stat)
{
        TIMENC_t *hdl = device_handle;

        device_stat->st_size = sizeof(hdl->counter);

        return ESUCC;
}

//==============================================================================
/**
 * @brief  Function return source frequency of selected timer.
 *
 * @return Source frequency in Hz.
 */
//==============================================================================
static u32_t get_timer_frequency(void)
{
        LL_RCC_ClocksTypeDef freq;
        LL_RCC_GetSystemClocksFreq(&freq);

        if (freq.SYSCLK_Frequency != freq.PCLK1_Frequency) {
                return freq.PCLK1_Frequency * 2;
        } else {
                return freq.PCLK1_Frequency;
        }
}

//==============================================================================
/**
 * @brief  Timer interrupt handler.
 */
//==============================================================================
void TIM7_IRQHandler(void)
{
        inline i8_t idx(u8_t last, u8_t new) {return ( (((last) & 3) << 2) | (((new) & 3) << 0) );}

        static const i8_t TRUTH_TAB[16] = {0,-1,1,0, 1,0,0,-1, -1,0,0,1, 0,1,-1,0};

        for (size_t i = 0; i < BASE->encoders; i++) {
                TIMENC_t *hdl = BASE->encoder[i];

                u8_t state = 0;
                state |= (_GPIO_DDI_get_pin(hdl->pin_A.port_idx, hdl->pin_A.pin_idx) << 0);
                state |= (_GPIO_DDI_get_pin(hdl->pin_B.port_idx, hdl->pin_B.pin_idx) << 1);

                int step = TRUTH_TAB[idx(hdl->last_state, state)];
                hdl->counter += step;

                hdl->last_state = (state & 3);
        }

        TIMER->SR = 0;
}

/*==============================================================================
  End of file
==============================================================================*/
