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
#include "stm32f1/gpio_cfg.h"
#include "stm32f1/gpio_def.h"
#include "stm32f1/gpio_macros.h"
#include "stm32f1/stm32f10x.h"

/*==============================================================================
  Local symbolic constants/macros
==============================================================================*/
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
struct gpio_reg_val {
        GPIO_t  *const GPIO;
        const uint32_t CRL;
        const uint32_t CRH;
        const uint16_t ODR;
};

/*==============================================================================
  Local function prototypes
==============================================================================*/

/*==============================================================================
  Local object definitions
==============================================================================*/
static const struct gpio_reg_val GPIOx[] = {
#if _GPIOA_EN
        {.GPIO = GPIOA, .CRL = GPIOx_CRL(GPIOA), .CRH = GPIOx_CRH(GPIOA), .ODR = GPIOx_ODR(GPIOA)},
#endif
#if _GPIOB_EN
        {.GPIO = GPIOB, .CRL = GPIOx_CRL(GPIOB), .CRH = GPIOx_CRH(GPIOB), .ODR = GPIOx_ODR(GPIOB)},
#endif
#if _GPIOC_EN
        {.GPIO = GPIOC, .CRL = GPIOx_CRL(GPIOC), .CRH = GPIOx_CRH(GPIOC), .ODR = GPIOx_ODR(GPIOC)},
#endif
#if _GPIOD_EN
        {.GPIO = GPIOD, .CRL = GPIOx_CRL(GPIOD), .CRH = GPIOx_CRH(GPIOD), .ODR = GPIOx_ODR(GPIOD)},
#endif
#if _GPIOE_EN
        {.GPIO = GPIOE, .CRL = GPIOx_CRL(GPIOE), .CRH = GPIOx_CRH(GPIOE), .ODR = GPIOx_ODR(GPIOE)},
#endif
#if _GPIOF_EN
        {.GPIO = GPIOF, .CRL = GPIOx_CRL(GPIOF), .CRH = GPIOx_CRH(GPIOF), .ODR = GPIOx_ODR(GPIOF)},
#endif
#if _GPIOG_EN
        {.GPIO = GPIOG, .CRL = GPIOx_CRL(GPIOG), .CRH = GPIOx_CRH(GPIOG), .ODR = GPIOx_ODR(GPIOG)},
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
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
API_MOD_INIT(GPIO, void **device_handle, u8_t major, u8_t minor)
{
        UNUSED_ARG(device_handle);
        UNUSED_ARG(major);
        UNUSED_ARG(minor);

#if (_GPIOA_EN > 0)
        SET_BIT(RCC->APB2ENR, RCC_APB2ENR_IOPAEN);
#endif
#if (_GPIOB_EN > 0)
        SET_BIT(RCC->APB2ENR, RCC_APB2ENR_IOPBEN);
#endif
#if (_GPIOC_EN > 0)
        SET_BIT(RCC->APB2ENR, RCC_APB2ENR_IOPCEN);
#endif
#if (_GPIOD_EN > 0)
        SET_BIT(RCC->APB2ENR, RCC_APB2ENR_IOPDEN);
#endif
#if (_GPIOE_EN > 0)
        SET_BIT(RCC->APB2ENR, RCC_APB2ENR_IOPEEN);
#endif
#if (_GPIOF_EN > 0)
        SET_BIT(RCC->APB2ENR, RCC_APB2ENR_IOPFEN);
#endif
#if (_GPIOG_EN > 0)
        SET_BIT(RCC->APB2ENR, RCC_APB2ENR_IOPGEN);
#endif

        for (uint i = 0; i < ARRAY_SIZE(GPIOx); i++) {
                GPIOx[i].GPIO->ODR = GPIOx[i].ODR;
                GPIOx[i].GPIO->CRL = GPIOx[i].CRL;
                GPIOx[i].GPIO->CRH = GPIOx[i].CRH;
        }

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
API_MOD_RELEASE(GPIO, void *device_handle)
{
        UNUSED_ARG(device_handle);

#if (_GPIOA_EN > 0)
        CLEAR_BIT(RCC->APB2ENR, RCC_APB2ENR_IOPAEN);
        SET_BIT(RCC->APB2RSTR, RCC_APB2RSTR_IOPARST);
        CLEAR_BIT(RCC->APB2RSTR, RCC_APB2RSTR_IOPARST);
#endif
#if (_GPIOB_EN > 0)
        CLEAR_BIT(RCC->APB2ENR, RCC_APB2ENR_IOPBEN);
        SET_BIT(RCC->APB2RSTR, RCC_APB2RSTR_IOPBRST);
        CLEAR_BIT(RCC->APB2RSTR, RCC_APB2RSTR_IOPBRST);
#endif
#if (_GPIOC_EN > 0)
        CLEAR_BIT(RCC->APB2ENR, RCC_APB2ENR_IOPCEN);
        SET_BIT(RCC->APB2RSTR, RCC_APB2RSTR_IOPCRST);
        CLEAR_BIT(RCC->APB2RSTR, RCC_APB2RSTR_IOPCRST);
#endif
#if (_GPIOD_EN > 0)
        CLEAR_BIT(RCC->APB2ENR, RCC_APB2ENR_IOPDEN);
        SET_BIT(RCC->APB2RSTR, RCC_APB2RSTR_IOPDRST);
        CLEAR_BIT(RCC->APB2RSTR, RCC_APB2RSTR_IOPDRST);
#endif
#if (_GPIOE_EN > 0)
        CLEAR_BIT(RCC->APB2ENR, RCC_APB2ENR_IOPEEN);
        SET_BIT(RCC->APB2RSTR, RCC_APB2RSTR_IOPERST);
        CLEAR_BIT(RCC->APB2RSTR, RCC_APB2RSTR_IOPERST);
#endif
#if (_GPIOF_EN > 0)
        CLEAR_BIT(RCC->APB2ENR, RCC_APB2ENR_IOPFEN);
        SET_BIT(RCC->APB2RSTR, RCC_APB2RSTR_IOPFRST);
        CLEAR_BIT(RCC->APB2RSTR, RCC_APB2RSTR_IOPFRST);
#endif
#if (_GPIOG_EN > 0)
        CLEAR_BIT(RCC->APB2ENR, RCC_APB2ENR_IOPGEN);
        SET_BIT(RCC->APB2RSTR, RCC_APB2RSTR_IOPGRST);
        CLEAR_BIT(RCC->APB2RSTR, RCC_APB2RSTR_IOPGRST);
#endif

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
API_MOD_CLOSE(GPIO, void *device_handle, bool force)
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
API_MOD_WRITE(GPIO,
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
API_MOD_READ(GPIO,
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
 * @return On success return 0. On error, -1 is returned.
 */
//==============================================================================
API_MOD_IOCTL(GPIO, void *device_handle, int request, void *arg)
{
        UNUSED_ARG(device_handle);

        if (arg) {
                switch (request) {
                case IOCTL_GPIO__SET_PIN: {
                        GPIO_pin_t *io = arg;
                        if (io->port_index == _GPIO_IOCTL_NONE) break;
                        GPIOx[io->port_index].GPIO->BSRR = (1 << io->pin_number);
                        break;
                }

                case IOCTL_GPIO__CLEAR_PIN: {
                        GPIO_pin_t *io = arg;
                        if (io->port_index == _GPIO_IOCTL_NONE) break;
                        GPIOx[io->port_index].GPIO->BRR = (1 << io->pin_number);
                        break;
                }

                case IOCTL_GPIO__TOGGLE_PIN: {
                        GPIO_pin_t *io = arg;
                        if (io->port_index == _GPIO_IOCTL_NONE) break;
                        GPIOx[io->port_index].GPIO->ODR ^= (1 << io->pin_number);
                        break;
                }

                case IOCTL_GPIO__GET_PIN: {
                        GPIO_pin_state_t *io = arg;
                        if (io->pin->port_index == _GPIO_IOCTL_NONE) break;
                        io->state = (GPIOx[io->pin->port_index].GPIO->IDR & (1 << io->pin->pin_number)) ? true : false;
                        break;
                }

                default:
                        return EBADRQC;
                }

                return ESUCC;
        } else {
                return EINVAL;
        }
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
API_MOD_STAT(GPIO, void *device_handle, struct vfs_dev_stat *device_stat)
{
        UNUSED_ARG(device_handle);

        device_stat->st_size  = 0;
        device_stat->st_major = 0;
        device_stat->st_minor = 0;

        return ESUCC;
}

/*==============================================================================
                                             End of file
==============================================================================*/
