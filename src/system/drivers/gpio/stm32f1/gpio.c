/*=========================================================================*//**
@file    gpio.c

@author  Daniel Zorychta

@brief   This driver support GPIO. GPIO driver not provide any interface
         functions. All operations on ports should be made direct via definitions
         (much faster). When operation on a ports are needed please write own
         driver which controls pins directly and register it in the VFS if
         needed.

@note    Copyright (C) 2012  Daniel Zorychta <daniel.zorychta@gmail.com>

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
#include "stm32f1/gpio_cfg.h"
#include "stm32f1/gpio_macros.h"
#include "stm32f1/stm32f10x.h"
#include "../gpio_ioctl.h"

/*==============================================================================
  Local symbolic constants/macros
==============================================================================*/
/** define number of pins per port */
#define PINS_PER_PORT   16

/** define CRL configuration macro */
#define GPIO_SET_CRL(CFG, PIN)                  ( (CFG) << (4 * (PIN)) )

/** define CRH configuration macro */
#define GPIO_SET_CRH(CFG, PIN)                  ( (CFG) << (4 * ((PIN) - 8)) )

/** CRL register value for GPIO */
#define GPIOx_CRL(port) ( GPIO_SET_CRL(_##port##_PIN_0_MODE, 0 ) | GPIO_SET_CRL(_##port##_PIN_1_MODE, 1 ) \
                        | GPIO_SET_CRL(_##port##_PIN_2_MODE, 2 ) | GPIO_SET_CRL(_##port##_PIN_3_MODE, 3 ) \
                        | GPIO_SET_CRL(_##port##_PIN_4_MODE, 4 ) | GPIO_SET_CRL(_##port##_PIN_5_MODE, 5 ) \
                        | GPIO_SET_CRL(_##port##_PIN_6_MODE, 6 ) | GPIO_SET_CRL(_##port##_PIN_7_MODE, 7 ) )

/** CRH register value for GPIO */
#define GPIOx_CRH(port) ( GPIO_SET_CRH(_##port##_PIN_8_MODE ,  8) | GPIO_SET_CRH(_##port##_PIN_9_MODE ,  9) \
                        | GPIO_SET_CRH(_##port##_PIN_10_MODE, 10) | GPIO_SET_CRH(_##port##_PIN_11_MODE, 11) \
                        | GPIO_SET_CRH(_##port##_PIN_12_MODE, 12) | GPIO_SET_CRH(_##port##_PIN_13_MODE, 13) \
                        | GPIO_SET_CRH(_##port##_PIN_14_MODE, 14) | GPIO_SET_CRH(_##port##_PIN_15_MODE, 15) )

/** ODR register value for GPIO */
#define GPIOx_ODR(port) ( (_##port##_PIN_0_STATE  <<  0) | (_##port##_PIN_1_STATE  <<  1) \
                        | (_##port##_PIN_2_STATE  <<  2) | (_##port##_PIN_3_STATE  <<  3) \
                        | (_##port##_PIN_4_STATE  <<  4) | (_##port##_PIN_5_STATE  <<  5) \
                        | (_##port##_PIN_6_STATE  <<  6) | (_##port##_PIN_7_STATE  <<  7) \
                        | (_##port##_PIN_8_STATE  <<  8) | (_##port##_PIN_9_STATE  <<  9) \
                        | (_##port##_PIN_10_STATE << 10) | (_##port##_PIN_11_STATE << 11) \
                        | (_##port##_PIN_12_STATE << 12) | (_##port##_PIN_13_STATE << 13) \
                        | (_##port##_PIN_14_STATE << 14) | (_##port##_PIN_15_STATE << 15) )

/*==============================================================================
  Local types, enums definitions
==============================================================================*/
struct GPIO_reg {
        GPIO_t  *const GPIO;
        uint32_t       IOPEN;
        uint32_t       CRL;
        uint32_t       CRH;
        uint16_t       ODR;
};

/*==============================================================================
  Local function prototypes
==============================================================================*/

/*==============================================================================
  Local object definitions
==============================================================================*/
static const struct GPIO_reg GPIOx[] = {
        #if defined(RCC_APB2ENR_IOPAEN)
        {.GPIO = GPIOA, .IOPEN = RCC_APB2ENR_IOPAEN, .CRL = GPIOx_CRL(GPIOA), .CRH = GPIOx_CRH(GPIOA), .ODR = GPIOx_ODR(GPIOA)},
        #endif
        #if defined(RCC_APB2ENR_IOPBEN)
        {.GPIO = GPIOB, .IOPEN = RCC_APB2ENR_IOPBEN, .CRL = GPIOx_CRL(GPIOB), .CRH = GPIOx_CRH(GPIOB), .ODR = GPIOx_ODR(GPIOB)},
        #endif
        #if defined(RCC_APB2ENR_IOPCEN)
        {.GPIO = GPIOC, .IOPEN = RCC_APB2ENR_IOPCEN, .CRL = GPIOx_CRL(GPIOC), .CRH = GPIOx_CRH(GPIOC), .ODR = GPIOx_ODR(GPIOC)},
        #endif
        #if defined(RCC_APB2ENR_IOPDEN)
        {.GPIO = GPIOD, .IOPEN = RCC_APB2ENR_IOPDEN, .CRL = GPIOx_CRL(GPIOD), .CRH = GPIOx_CRH(GPIOD), .ODR = GPIOx_ODR(GPIOD)},
        #endif
        #if defined(RCC_APB2ENR_IOPEEN)
        {.GPIO = GPIOE, .IOPEN = RCC_APB2ENR_IOPEEN, .CRL = GPIOx_CRL(GPIOE), .CRH = GPIOx_CRH(GPIOE), .ODR = GPIOx_ODR(GPIOE)},
        #endif
        #if defined(RCC_APB2ENR_IOPFEN)
        {.GPIO = GPIOF, .IOPEN = RCC_APB2ENR_IOPFEN, .CRL = GPIOx_CRL(GPIOF), .CRH = GPIOx_CRH(GPIOF), .ODR = GPIOx_ODR(GPIOF)},
        #endif
        #if defined(RCC_APB2ENR_IOPGEN)
        {.GPIO = GPIOG, .IOPEN = RCC_APB2ENR_IOPGEN, .CRL = GPIOx_CRL(GPIOG), .CRH = GPIOx_CRH(GPIOG), .ODR = GPIOx_ODR(GPIOG)},
        #endif
};

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
API_MOD_INIT(GPIO, void **device_handle, u8_t major, u8_t minor, const void *config)
{
        UNUSED_ARG1(config);

        if (major < ARRAY_SIZE(GPIOx) && minor == 0) {
                SET_BIT(RCC->APB2ENR, GPIOx[major].IOPEN);

                GPIOx[major].GPIO->ODR = GPIOx[major].ODR;
                GPIOx[major].GPIO->CRL = GPIOx[major].CRL;
                GPIOx[major].GPIO->CRH = GPIOx[major].CRH;

                *device_handle = const_cast(void*, &GPIOx[major]);

                return ESUCC;
        } else {
                return ENODEV;
        }
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
API_MOD_RELEASE(GPIO, void *device_handle)
{
        const struct GPIO_reg *hdl = device_handle;

        CLEAR_BIT(RCC->APB2ENR, hdl->IOPEN);
        SET_BIT(RCC->APB2RSTR, hdl->IOPEN);
        CLEAR_BIT(RCC->APB2RSTR, hdl->IOPEN);

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
API_MOD_OPEN(GPIO, void *device_handle, u32_t flags)
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
API_MOD_CLOSE(GPIO, void *device_handle, bool force)
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
API_MOD_WRITE(GPIO,
              void             *device_handle,
              const u8_t       *src,
              size_t            count,
              fpos_t           *fpos,
              size_t           *wrcnt,
              struct vfs_fattr  fattr)
{
        UNUSED_ARG2(fpos, fattr);

        const struct GPIO_reg *hdl = device_handle;

        if (count >= 2) {
                for (size_t i = 0; i < count / 2; i++) {
                        hdl->GPIO->ODR = cast(u16_t*, src)[i];
                }

                *wrcnt = count & ~(1);
        }

        return ESUCC;
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
API_MOD_READ(GPIO,
             void            *device_handle,
             u8_t            *dst,
             size_t           count,
             fpos_t          *fpos,
             size_t          *rdcnt,
             struct vfs_fattr fattr)
{
        UNUSED_ARG2(fpos, fattr);

        const struct GPIO_reg *hdl = device_handle;

        if (count >= 2) {
                for (size_t i = 0; i < count / 2; i++) {
                        cast(u16_t*, dst)[i] = hdl->GPIO->IDR;
                }

                *rdcnt = count & ~(1);
        }

        return ESUCC;
}

//==============================================================================
/**
 * @brief IO control
 *
 * @param[in ]          *device_handle          device allocated memory
 * @param[in ]           request                request
 * @param[in ][out]     *arg                    request's argument
 *
 * @return On success return 0. On error, -1 is returned.
 */
//==============================================================================
API_MOD_IOCTL(GPIO, void *device_handle, int request, void *arg)
{
        int err = EINVAL;

        const struct GPIO_reg *hdl = device_handle;

        if (arg) {
                switch (request) {
                case IOCTL_GPIO__SET_PIN: {
                        u8_t pin = *cast(u8_t*, arg);
                        if (pin < PINS_PER_PORT) {
                                hdl->GPIO->BSRR = (1 << pin);
                                err = ESUCC;
                        }
                        break;
                }

                case IOCTL_GPIO__CLEAR_PIN: {
                        u8_t pin = *cast(u8_t*, arg);
                        if (pin < PINS_PER_PORT) {
                                hdl->GPIO->BRR = (1 << pin);
                                err = ESUCC;
                        }
                        break;
                }

                case IOCTL_GPIO__TOGGLE_PIN: {
                        u8_t pin = *cast(u8_t*, arg);
                        if (pin < PINS_PER_PORT) {
                                hdl->GPIO->ODR ^= (1 << pin);
                                err = ESUCC;
                        }
                        break;
                }

                case IOCTL_GPIO__SET_PIN_STATE: {
                        GPIO_pin_state_t *pinstate = arg;
                        if (pinstate->pin_idx < PINS_PER_PORT) {
                                if (pinstate->state) {
                                        hdl->GPIO->BSRR = (1 << pinstate->pin_idx);
                                } else {
                                        hdl->GPIO->BRR  = (1 << pinstate->pin_idx);
                                }
                                err = ESUCC;
                        }
                        break;
                }

                case IOCTL_GPIO__GET_PIN_STATE: {
                        GPIO_pin_state_t *pinstate = arg;
                        if (pinstate->pin_idx < PINS_PER_PORT) {
                                pinstate->state = (hdl->GPIO->IDR & (1 << pinstate->pin_idx)) ? 1 : 0;
                                err = ESUCC;
                        }
                        break;
                }

                case IOCTL_GPIO__SET_PIN_IN_PORT: {
                        GPIO_pin_in_port_t *pin = arg;
                        if ((pin->port_idx < ARRAY_SIZE(GPIOx)) && (pin->pin_idx < 16)) {
                                GPIOx[pin->port_idx].GPIO->BSRR = (1 << pin->pin_idx);
                                err = ESUCC;
                        }
                        break;
                }

                case IOCTL_GPIO__CLEAR_PIN_IN_PORT: {
                        GPIO_pin_in_port_t *pin = arg;
                        if ((pin->port_idx < ARRAY_SIZE(GPIOx)) && (pin->pin_idx < 16)) {
                                GPIOx[pin->port_idx].GPIO->BRR = (1 << pin->pin_idx);
                                err = ESUCC;
                        }
                        break;
                }

                case IOCTL_GPIO__TOGGLE_PIN_IN_PORT: {
                        GPIO_pin_in_port_t *pin = arg;
                        if ((pin->port_idx < ARRAY_SIZE(GPIOx)) && (pin->pin_idx < 16)) {
                                GPIOx[pin->port_idx].GPIO->ODR ^= (1 << pin->pin_idx);
                                err = ESUCC;
                        }
                        break;
                }

                case IOCTL_GPIO__SET_PIN_STATE_IN_PORT: {
                        GPIO_pin_in_port_state_t *pin = arg;
                        if ((pin->port_idx < ARRAY_SIZE(GPIOx)) && (pin->pin_idx < 16)) {
                                if (pin->state) {
                                        GPIOx[pin->port_idx].GPIO->BSRR = (1 << pin->pin_idx);
                                } else {
                                        GPIOx[pin->port_idx].GPIO->BRR = (1 << pin->pin_idx);
                                }
                                err = ESUCC;
                        }
                        break;
                }

                case IOCTL_GPIO__GET_PIN_STATE_IN_PORT: {
                        GPIO_pin_in_port_state_t *pin = arg;
                        if ((pin->port_idx < ARRAY_SIZE(GPIOx)) && (pin->pin_idx < 16)) {
                                pin->state = (GPIOx[pin->port_idx].GPIO->IDR & (1 << pin->pin_idx)) ? 1 : 0;
                                err = ESUCC;
                        }
                        break;
                }

                default:
                        err = EBADRQC;
                        break;
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
API_MOD_FLUSH(GPIO, void *device_handle)
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
API_MOD_STAT(GPIO, void *device_handle, struct vfs_dev_stat *device_stat)
{
        const struct GPIO_reg *hdl = device_handle;

        device_stat->st_size  = 2;
        device_stat->st_major = (hdl->IOPEN >> 4);
        device_stat->st_minor = 0;

        return ESUCC;
}

//==============================================================================
/**
 * @brief Function set pin to 1.
 * @param port_idx      Port index
 * @param pin_idx       Pin index
 */
//==============================================================================
void _GPIO_DDI_set_pin(u8_t port_idx, u8_t pin_idx)
{
        if ((port_idx < ARRAY_SIZE(GPIOx)) && (pin_idx < 16)) {
                GPIOx[port_idx].GPIO->BSRR = (1 << pin_idx);
        }
}

//==============================================================================
/**
 * @brief Function clear pin to 0.
 * @param port_idx      Port index.
 * @param pin_idx       Pin index.
 */
//==============================================================================
void _GPIO_DDI_clear_pin(u8_t port_idx, u8_t pin_idx)
{
        if ((port_idx < ARRAY_SIZE(GPIOx)) && (pin_idx < 16)) {
                GPIOx[port_idx].GPIO->BRR = (1 << pin_idx);
        }
}

//==============================================================================
/**
 * @brief  Function get pin state.
 * @param  port_idx      Port index.
 * @param  pin_idx       Pin index.
 * @return Pin value. On error -1.
 */
//==============================================================================
i8_t _GPIO_DDI_get_pin(u8_t port_idx, u8_t pin_idx)
{
        if ((port_idx < ARRAY_SIZE(GPIOx)) && (pin_idx < 16)) {
                return GPIOx[port_idx].GPIO->IDR & (1 << pin_idx) ? 1 : 0;
        } else {
                return -1;
        }
}

//==============================================================================
/**
 * @brief  Function set pin mode.
 * @param  port_idx      Port index.
 * @param  pin_idx       Pin index.
 * @param  mode          Pin mode
 */
//==============================================================================
void _GPIO_DDI_set_pin_mode(u8_t port_idx, u8_t pin_idx, int mode)
{
        if ((port_idx < ARRAY_SIZE(GPIOx)) && (pin_idx < 16)) {
                sys_ISR_disable();

                volatile u32_t *CR = (pin_idx < 8) ? &GPIOx[port_idx].GPIO->CRL
                                                   : &GPIOx[port_idx].GPIO->CRH;

                u8_t shift = 4 * ((pin_idx < 8) ? pin_idx : pin_idx - 8);

                if (_PXODR(mode) <= 1) {
                        if (_PXODR(mode)) {
                                SET_BIT(GPIOx[port_idx].GPIO->ODR, (1 << pin_idx));
                        } else {
                                CLEAR_BIT(GPIOx[port_idx].GPIO->ODR, (1 << pin_idx));
                        }
                }

                CLEAR_BIT(*CR, (GPIO_CRL_MODE0 | GPIO_CRL_CNF0 << shift));
                SET_BIT(*CR, ((_CNF(mode) << 2) | (_MODER(mode) << 0) << shift));

                sys_ISR_enable();
        }
}

//==============================================================================
/**
 * @brief  Function get pin mode.
 * @param  port_idx      Port index.
 * @param  pin_idx       Pin index.
 * @param  mode          Pin mode
 * @return On success 0 is returned.
 */
//==============================================================================
int _GPIO_DDI_get_pin_mode(u8_t port_idx, u8_t pin_idx, int *mode)
{
        if ((port_idx < ARRAY_SIZE(GPIOx)) && (pin_idx < 16) && mode) {
                sys_ISR_disable();

                volatile u32_t *CR = (pin_idx < 8) ? &GPIOx[port_idx].GPIO->CRL
                                                   : &GPIOx[port_idx].GPIO->CRH;

                u8_t shift = 4 * ((pin_idx < 8) ? pin_idx : pin_idx - 8);

                *CR >>= shift;

                *mode = ((((*CR >> 2) & 3) << 4)) // CNF
                      | ((((*CR >> 0) & 3) << 2)) // MODE
                      | (((GPIOx[port_idx].GPIO->ODR >> pin_idx) & 1) << 0);

                sys_ISR_enable();

                return 0;
        } else {
                return EINVAL;
        }
}

//==============================================================================
/**
 * @brief  Function set pin multiplexer (internal peripheral connection).
 * @param  port_idx      Port index.
 * @param  pin_idx       Pin index.
 * @param  mux           Pin multiplexer
 */
//==============================================================================
void _GPIO_DDI_set_pin_mux(u8_t port_idx, u8_t pin_idx, int mux)
{
        UNUSED_ARG3(port_idx, pin_idx, mux);
}

/*==============================================================================
  End of file
==============================================================================*/
