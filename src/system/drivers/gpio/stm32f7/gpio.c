/*=========================================================================*//**
@file    gpio.c

@author  Daniel Zorychta

@brief   This driver support GPIO. GPIO driver not provide any interface
         functions. All operations on ports should be made direct via definitions
         (much faster). When operation on a ports are needed please write own
         driver which controls pins directly and register it in the VFS if
         needed.

@note    Copyright (C) 2017  Daniel Zorychta <daniel.zorychta@gmail.com>

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
#include "stm32f7/gpio_cfg.h"
#include "stm32f7/gpio_macros.h"
#include "stm32f7/stm32f7xx.h"
#include "../gpio_ioctl.h"

/*==============================================================================
  Local symbolic constants/macros
==============================================================================*/
/** define number of pins per port */
#define PINS_PER_PORT                           16

/** define CRL configuration macro */
#define GPIO_SET_AFRL(CFG, PIN)                 ( (CFG) << (4 * (PIN)) )

/** define CRH configuration macro */
#define GPIO_SET_AFRH(CFG, PIN)                 ( (CFG) << (4 * ((PIN) - 8)) )

/** AFRL register value for GPIO */
#define GPIOx_AFRL(port) \
        ( GPIO_SET_AFRL(_##port##_PIN_0_AF, 0) | GPIO_SET_AFRL(_##port##_PIN_1_AF, 1) \
        | GPIO_SET_AFRL(_##port##_PIN_2_AF, 2) | GPIO_SET_AFRL(_##port##_PIN_3_AF, 3) \
        | GPIO_SET_AFRL(_##port##_PIN_4_AF, 4) | GPIO_SET_AFRL(_##port##_PIN_5_AF, 5) \
        | GPIO_SET_AFRL(_##port##_PIN_6_AF, 6) | GPIO_SET_AFRL(_##port##_PIN_7_AF, 7) )

/** AFRH register value for GPIO */
#define GPIOx_AFRH(port) \
        ( GPIO_SET_AFRH(_##port##_PIN_8_AF,   8) | GPIO_SET_AFRH(_##port##_PIN_9_AF ,  9) \
        | GPIO_SET_AFRH(_##port##_PIN_10_AF, 10) | GPIO_SET_AFRH(_##port##_PIN_11_AF, 11) \
        | GPIO_SET_AFRH(_##port##_PIN_12_AF, 12) | GPIO_SET_AFRH(_##port##_PIN_13_AF, 13) \
        | GPIO_SET_AFRH(_##port##_PIN_14_AF, 14) | GPIO_SET_AFRH(_##port##_PIN_15_AF, 15) )

/** ODR register value for GPIO */
#define GPIOx_ODR(port) \
        ( (_##port##_PIN_0_STATE  <<  0) | (_##port##_PIN_1_STATE  <<  1) \
        | (_##port##_PIN_2_STATE  <<  2) | (_##port##_PIN_3_STATE  <<  3) \
        | (_##port##_PIN_4_STATE  <<  4) | (_##port##_PIN_5_STATE  <<  5) \
        | (_##port##_PIN_6_STATE  <<  6) | (_##port##_PIN_7_STATE  <<  7) \
        | (_##port##_PIN_8_STATE  <<  8) | (_##port##_PIN_9_STATE  <<  9) \
        | (_##port##_PIN_10_STATE << 10) | (_##port##_PIN_11_STATE << 11) \
        | (_##port##_PIN_12_STATE << 12) | (_##port##_PIN_13_STATE << 13) \
        | (_##port##_PIN_14_STATE << 14) | (_##port##_PIN_15_STATE << 15) )

/** TYPE register value for GPIO */
#define GPIOx_OTYPER(port) \
        ( (_##port##_PIN_0_TYPE  <<  0) | (_##port##_PIN_1_TYPE  <<  1) \
        | (_##port##_PIN_2_TYPE  <<  2) | (_##port##_PIN_3_TYPE  <<  3) \
        | (_##port##_PIN_4_TYPE  <<  4) | (_##port##_PIN_5_TYPE  <<  5) \
        | (_##port##_PIN_6_TYPE  <<  6) | (_##port##_PIN_7_TYPE  <<  7) \
        | (_##port##_PIN_8_TYPE  <<  8) | (_##port##_PIN_9_TYPE  <<  9) \
        | (_##port##_PIN_10_TYPE << 10) | (_##port##_PIN_11_TYPE << 11) \
        | (_##port##_PIN_12_TYPE << 12) | (_##port##_PIN_13_TYPE << 13) \
        | (_##port##_PIN_14_TYPE << 14) | (_##port##_PIN_15_TYPE << 15) )

/** MODE register value for GPIO */
#define GPIOx_MODER(port)\
        ( (_##port##_PIN_0_MODE  <<  0) | (_##port##_PIN_1_MODE  <<  2) \
        | (_##port##_PIN_2_MODE  <<  4) | (_##port##_PIN_3_MODE  <<  6) \
        | (_##port##_PIN_4_MODE  <<  8) | (_##port##_PIN_5_MODE  << 10) \
        | (_##port##_PIN_6_MODE  << 12) | (_##port##_PIN_7_MODE  << 14) \
        | (_##port##_PIN_8_MODE  << 16) | (_##port##_PIN_9_MODE  << 18) \
        | (_##port##_PIN_10_MODE << 20) | (_##port##_PIN_11_MODE << 22) \
        | (_##port##_PIN_12_MODE << 24) | (_##port##_PIN_13_MODE << 26) \
        | (_##port##_PIN_14_MODE << 28) | (_##port##_PIN_15_MODE << 30) )

/** OSPEED register value for GPIO */
#define GPIOx_OSPEEDR(port) \
        ( (_##port##_PIN_0_SPEED  <<  0) | (_##port##_PIN_1_SPEED  <<  2) \
        | (_##port##_PIN_2_SPEED  <<  4) | (_##port##_PIN_3_SPEED  <<  6) \
        | (_##port##_PIN_4_SPEED  <<  8) | (_##port##_PIN_5_SPEED  << 10) \
        | (_##port##_PIN_6_SPEED  << 12) | (_##port##_PIN_7_SPEED  << 14) \
        | (_##port##_PIN_8_SPEED  << 16) | (_##port##_PIN_9_SPEED  << 18) \
        | (_##port##_PIN_10_SPEED << 20) | (_##port##_PIN_11_SPEED << 22) \
        | (_##port##_PIN_12_SPEED << 24) | (_##port##_PIN_13_SPEED << 26) \
        | (_##port##_PIN_14_SPEED << 28) | (_##port##_PIN_15_SPEED << 30) )

/** PUPD register value for GPIO */
#define GPIOx_PUPDR(port) \
        ( (_##port##_PIN_0_PUPD  <<  0) | (_##port##_PIN_1_PUPD  <<  2) \
        | (_##port##_PIN_2_PUPD  <<  4) | (_##port##_PIN_3_PUPD  <<  6) \
        | (_##port##_PIN_4_PUPD  <<  8) | (_##port##_PIN_5_PUPD  << 10) \
        | (_##port##_PIN_6_PUPD  << 12) | (_##port##_PIN_7_PUPD  << 14) \
        | (_##port##_PIN_8_PUPD  << 16) | (_##port##_PIN_9_PUPD  << 18) \
        | (_##port##_PIN_10_PUPD << 20) | (_##port##_PIN_11_PUPD << 22) \
        | (_##port##_PIN_12_PUPD << 24) | (_##port##_PIN_13_PUPD << 26) \
        | (_##port##_PIN_14_PUPD << 28) | (_##port##_PIN_15_PUPD << 30) )

/*==============================================================================
  Local types, enums definitions
==============================================================================*/
struct GPIO_reg {
        GPIO_TypeDef *const     GPIO;
        uint32_t                IOPEN;
        uint32_t                MODER;
        uint32_t                OSPEEDR;
        uint32_t                PUPDR;
        uint32_t                AFRL;
        uint32_t                AFRH;
        uint16_t                OTYPER;
        uint16_t                ODR;
};

/*==============================================================================
  Local function prototypes
==============================================================================*/

/*==============================================================================
  Local object definitions
==============================================================================*/
static const struct GPIO_reg GPIOx[] = {
        #if defined(RCC_AHB1ENR_GPIOAEN)
        {.GPIO = GPIOA, .IOPEN = RCC_AHB1ENR_GPIOAEN, .MODER = GPIOx_MODER(GPIOA), .OSPEEDR = GPIOx_OSPEEDR(GPIOA), .PUPDR = GPIOx_PUPDR(GPIOA), .AFRL = GPIOx_AFRL(GPIOA), .AFRH = GPIOx_AFRH(GPIOA), .OTYPER = GPIOx_OTYPER(GPIOA), .ODR = GPIOx_ODR(GPIOA)},
        #endif
        #if defined(RCC_AHB1ENR_GPIOBEN)
        {.GPIO = GPIOB, .IOPEN = RCC_AHB1ENR_GPIOBEN, .MODER = GPIOx_MODER(GPIOB), .OSPEEDR = GPIOx_OSPEEDR(GPIOB), .PUPDR = GPIOx_PUPDR(GPIOB), .AFRL = GPIOx_AFRL(GPIOB), .AFRH = GPIOx_AFRH(GPIOB), .OTYPER = GPIOx_OTYPER(GPIOB), .ODR = GPIOx_ODR(GPIOB)},
        #endif
        #if defined(RCC_AHB1ENR_GPIOCEN)
        {.GPIO = GPIOC, .IOPEN = RCC_AHB1ENR_GPIOCEN, .MODER = GPIOx_MODER(GPIOC), .OSPEEDR = GPIOx_OSPEEDR(GPIOC), .PUPDR = GPIOx_PUPDR(GPIOC), .AFRL = GPIOx_AFRL(GPIOC), .AFRH = GPIOx_AFRH(GPIOC), .OTYPER = GPIOx_OTYPER(GPIOC), .ODR = GPIOx_ODR(GPIOC)},
        #endif
        #if defined(RCC_AHB1ENR_GPIODEN)
        {.GPIO = GPIOD, .IOPEN = RCC_AHB1ENR_GPIODEN, .MODER = GPIOx_MODER(GPIOD), .OSPEEDR = GPIOx_OSPEEDR(GPIOD), .PUPDR = GPIOx_PUPDR(GPIOD), .AFRL = GPIOx_AFRL(GPIOD), .AFRH = GPIOx_AFRH(GPIOD), .OTYPER = GPIOx_OTYPER(GPIOD), .ODR = GPIOx_ODR(GPIOD)},
        #endif
        #if defined(RCC_AHB1ENR_GPIOEEN)
        {.GPIO = GPIOE, .IOPEN = RCC_AHB1ENR_GPIOEEN, .MODER = GPIOx_MODER(GPIOE), .OSPEEDR = GPIOx_OSPEEDR(GPIOE), .PUPDR = GPIOx_PUPDR(GPIOE), .AFRL = GPIOx_AFRL(GPIOE), .AFRH = GPIOx_AFRH(GPIOE), .OTYPER = GPIOx_OTYPER(GPIOE), .ODR = GPIOx_ODR(GPIOE)},
        #endif
        #if defined(RCC_AHB1ENR_GPIOFEN)
        {.GPIO = GPIOF, .IOPEN = RCC_AHB1ENR_GPIOFEN, .MODER = GPIOx_MODER(GPIOF), .OSPEEDR = GPIOx_OSPEEDR(GPIOF), .PUPDR = GPIOx_PUPDR(GPIOF), .AFRL = GPIOx_AFRL(GPIOF), .AFRH = GPIOx_AFRH(GPIOF), .OTYPER = GPIOx_OTYPER(GPIOF), .ODR = GPIOx_ODR(GPIOF)},
        #endif
        #if defined(RCC_AHB1ENR_GPIOGEN)
        {.GPIO = GPIOG, .IOPEN = RCC_AHB1ENR_GPIOGEN, .MODER = GPIOx_MODER(GPIOG), .OSPEEDR = GPIOx_OSPEEDR(GPIOG), .PUPDR = GPIOx_PUPDR(GPIOG), .AFRL = GPIOx_AFRL(GPIOG), .AFRH = GPIOx_AFRH(GPIOG), .OTYPER = GPIOx_OTYPER(GPIOG), .ODR = GPIOx_ODR(GPIOG)},
        #endif
        #if defined(RCC_AHB1ENR_GPIOHEN)
        {.GPIO = GPIOH, .IOPEN = RCC_AHB1ENR_GPIOHEN, .MODER = GPIOx_MODER(GPIOH), .OSPEEDR = GPIOx_OSPEEDR(GPIOH), .PUPDR = GPIOx_PUPDR(GPIOH), .AFRL = GPIOx_AFRL(GPIOH), .AFRH = GPIOx_AFRH(GPIOH), .OTYPER = GPIOx_OTYPER(GPIOH), .ODR = GPIOx_ODR(GPIOH)},
        #endif
        #if defined(RCC_AHB1ENR_GPIOIEN)
        {.GPIO = GPIOI, .IOPEN = RCC_AHB1ENR_GPIOIEN, .MODER = GPIOx_MODER(GPIOI), .OSPEEDR = GPIOx_OSPEEDR(GPIOI), .PUPDR = GPIOx_PUPDR(GPIOI), .AFRL = GPIOx_AFRL(GPIOI), .AFRH = GPIOx_AFRH(GPIOI), .OTYPER = GPIOx_OTYPER(GPIOI), .ODR = GPIOx_ODR(GPIOI)},
        #endif
        #if defined(RCC_AHB1ENR_GPIOJEN)
        {.GPIO = GPIOJ, .IOPEN = RCC_AHB1ENR_GPIOJEN, .MODER = GPIOx_MODER(GPIOJ), .OSPEEDR = GPIOx_OSPEEDR(GPIOJ), .PUPDR = GPIOx_PUPDR(GPIOJ), .AFRL = GPIOx_AFRL(GPIOJ), .AFRH = GPIOx_AFRH(GPIOJ), .OTYPER = GPIOx_OTYPER(GPIOJ), .ODR = GPIOx_ODR(GPIOJ)},
        #endif
        #if defined(RCC_AHB1ENR_GPIOKEN)
        {.GPIO = GPIOK, .IOPEN = RCC_AHB1ENR_GPIOKEN, .MODER = GPIOx_MODER(GPIOK), .OSPEEDR = GPIOx_OSPEEDR(GPIOK), .PUPDR = GPIOx_PUPDR(GPIOK), .AFRL = GPIOx_AFRL(GPIOK), .AFRH = GPIOx_AFRH(GPIOK), .OTYPER = GPIOx_OTYPER(GPIOK), .ODR = GPIOx_ODR(GPIOK)},
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
                SET_BIT(RCC->AHB1ENR, GPIOx[major].IOPEN);

                GPIOx[major].GPIO->ODR     = GPIOx[major].ODR;
                GPIOx[major].GPIO->PUPDR   = GPIOx[major].PUPDR;
                GPIOx[major].GPIO->OSPEEDR = GPIOx[major].OSPEEDR;
                GPIOx[major].GPIO->OTYPER  = GPIOx[major].OTYPER;
                GPIOx[major].GPIO->AFR[0]  = GPIOx[major].AFRL;
                GPIOx[major].GPIO->AFR[1]  = GPIOx[major].AFRH;
                GPIOx[major].GPIO->MODER   = GPIOx[major].MODER;

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

        CLEAR_BIT(RCC->AHB1ENR, hdl->IOPEN);
        SET_BIT(RCC->AHB1RSTR, hdl->IOPEN);
        CLEAR_BIT(RCC->AHB1RSTR, hdl->IOPEN);

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
                                hdl->GPIO->BSRR = (1 << (pin + 16));
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
                                        hdl->GPIO->BSRR  = (1 << (pinstate->pin_idx + 16));
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
                                GPIOx[pin->port_idx].GPIO->BSRR = (1 << (pin->pin_idx + 16));
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
                                        GPIOx[pin->port_idx].GPIO->BSRR = (1 << (pin->pin_idx + 16));
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
                GPIOx[port_idx].GPIO->BSRR = (1 << (pin_idx + 16));
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

                CLEAR_BIT(GPIOx[port_idx].GPIO->PUPDR, (0x3 << (pin_idx*2)));
                SET_BIT(GPIOx[port_idx].GPIO->PUPDR, _PUPDR(mode) << (pin_idx * 2));

                CLEAR_BIT(GPIOx[port_idx].GPIO->OTYPER, (1 << pin_idx));
                SET_BIT(GPIOx[port_idx].GPIO->OTYPER, _OTYPER(mode) << pin_idx);

                CLEAR_BIT(GPIOx[port_idx].GPIO->MODER, (0x3 << (pin_idx*2)));
                SET_BIT(GPIOx[port_idx].GPIO->MODER, _MODER(mode) << (pin_idx * 2));

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

                *mode = _MODE((GPIOx[port_idx].GPIO->MODER >> (pin_idx * 2)) & 3,
                              (GPIOx[port_idx].GPIO->OTYPER >> pin_idx) & 1,
                              (GPIOx[port_idx].GPIO->PUPDR >> (pin_idx * 2)) & 3);

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
        if ((port_idx < ARRAY_SIZE(GPIOx)) && (pin_idx < 16) && (mux < 16)) {

                sys_ISR_disable();

                int reg = pin_idx / 8;

                CLEAR_BIT(GPIOx[port_idx].GPIO->AFR[reg], (0xF << (pin_idx*4)));
                SET_BIT(GPIOx[port_idx].GPIO->AFR[reg], ((mux & 0xF) << (pin_idx*4)));

                sys_ISR_enable();
        }
}

/*==============================================================================
  End of file
==============================================================================*/
