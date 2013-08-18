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

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Include files
==============================================================================*/
#include "stm32f1/gpio.h"
#include "stm32f1/stm32f10x.h"

/*==============================================================================
  Local symbolic constants/macros
==============================================================================*/
/** define pin configure size (CNF[1:0] + MODE[1:0]) */
#define GPIO_PIN_CFG_SIZE                       4U

/** define CRL configuration macro */
#define GPIO_SET_CRL(CFG, PIN)                  ( (CFG) << ((GPIO_PIN_CFG_SIZE) * (PIN)) )

/** define CRH configuration macro */
#define GPIO_SET_CRH(CFG, PIN)                  ( (CFG) << ((GPIO_PIN_CFG_SIZE) * ((PIN) - 8)) )

/** CRL register value for GPIO */
#define GPIOx_CRL(port) ( GPIO_SET_CRL(port##_PIN_0_MODE, 0 ) | GPIO_SET_CRL(port##_PIN_1_MODE, 1 ) \
                        | GPIO_SET_CRL(port##_PIN_2_MODE, 2 ) | GPIO_SET_CRL(port##_PIN_3_MODE, 3 ) \
                        | GPIO_SET_CRL(port##_PIN_4_MODE, 4 ) | GPIO_SET_CRL(port##_PIN_5_MODE, 5 ) \
                        | GPIO_SET_CRL(port##_PIN_6_MODE, 6 ) | GPIO_SET_CRL(port##_PIN_7_MODE, 7 ) )

/** CRH register value for GPIO */
#define GPIOx_CRH(port) ( GPIO_SET_CRH(port##_PIN_8_MODE ,  8) | GPIO_SET_CRH(port##_PIN_9_MODE ,  9) \
                        | GPIO_SET_CRH(port##_PIN_10_MODE, 10) | GPIO_SET_CRH(port##_PIN_11_MODE, 11) \
                        | GPIO_SET_CRH(port##_PIN_12_MODE, 12) | GPIO_SET_CRH(port##_PIN_13_MODE, 13) \
                        | GPIO_SET_CRH(port##_PIN_14_MODE, 14) | GPIO_SET_CRH(port##_PIN_15_MODE, 15) )

/** ODR register value for GPIO */
#define GPIOx_ODR(port) ( (port##_PIN_0_STATE  <<  0) | (port##_PIN_1_STATE  <<  1) \
                        | (port##_PIN_2_STATE  <<  2) | (port##_PIN_3_STATE  <<  3) \
                        | (port##_PIN_4_STATE  <<  4) | (port##_PIN_5_STATE  <<  5) \
                        | (port##_PIN_6_STATE  <<  6) | (port##_PIN_7_STATE  <<  7) \
                        | (port##_PIN_8_STATE  <<  8) | (port##_PIN_9_STATE  <<  9) \
                        | (port##_PIN_10_STATE << 10) | (port##_PIN_11_STATE << 11) \
                        | (port##_PIN_12_STATE << 12) | (port##_PIN_13_STATE << 13) \
                        | (port##_PIN_14_STATE << 14) | (port##_PIN_15_STATE << 15) )

/*==============================================================================
  Local types, enums definitions
==============================================================================*/

/*==============================================================================
  Local function prototypes
==============================================================================*/
static void init_GPIOx(GPIO_t *gpio, u32_t crl, u32_t crh, u32_t odr);
static void init_AFIO(void);

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
 * @brief Initialize GPIO module
 */
//==============================================================================
MODULE__DEVICE_INIT(GPIO)
{
        UNUSED_ARG(device_handle);
        UNUSED_ARG(major);
        UNUSED_ARG(minor);

#if (_AFIO_EN > 0)
        init_AFIO();
#endif

#if (_GPIOA_EN > 0)
        init_GPIOx(GPIOA, GPIOx_CRL(GPIOA), GPIOx_CRH(GPIOA), GPIOx_ODR(GPIOA));
#endif

#if (_GPIOB_EN > 0)
        init_GPIOx(GPIOB, GPIOx_CRL(GPIOB), GPIOx_CRH(GPIOB), GPIOx_ODR(GPIOB));
#endif

#if (_GPIOC_EN > 0)
        init_GPIOx(GPIOC, GPIOx_CRL(GPIOC), GPIOx_CRH(GPIOC), GPIOx_ODR(GPIOC));
#endif

#if (_GPIOD_EN > 0)
        init_GPIOx(GPIOD, GPIOx_CRL(GPIOD), GPIOx_CRH(GPIOD), GPIOx_ODR(GPIOD));
#endif

#if (_GPIOE_EN > 0)
        init_GPIOx(GPIOE, GPIOx_CRL(GPIOE), GPIOx_CRH(GPIOE), GPIOx_ODR(GPIOE));
#endif

#if (_GPIOF_EN > 0)
        init_GPIOx(GPIOF, GPIOx_CRL(GPIOF), GPIOx_CRH(GPIOF), GPIOx_ODR(GPIOF));
#endif

#if (_GPIOG_EN > 0)
        init_GPIOx(GPIOG, GPIOx_CRL(GPIOG), GPIOx_CRH(GPIOG), GPIOx_ODR(GPIOG));
#endif

        return STD_RET_OK;
}

//==============================================================================
/**
 * @brief Release GPIO device
 */
//==============================================================================
MODULE__DEVICE_RELEASE(GPIO)
{
        UNUSED_ARG(device_handle);

        return STD_RET_OK;
}

//==============================================================================
/**
 * @brief Open device
 */
//==============================================================================
MODULE__DEVICE_OPEN(GPIO)
{
        UNUSED_ARG(device_handle);

        return STD_RET_OK;
}

//==============================================================================
/**
 * @brief Close device
 */
//==============================================================================
MODULE__DEVICE_CLOSE(GPIO)
{
        UNUSED_ARG(device_handle);

        return STD_RET_OK;
}

//==============================================================================
/**
 * @brief Write data to device
 */
//==============================================================================
MODULE__DEVICE_WRITE(GPIO)
{
        UNUSED_ARG(device_handle);
        UNUSED_ARG(src);
        UNUSED_ARG(item_size);
        UNUSED_ARG(lseek);
        UNUSED_ARG(n_items);

        return 0;
}

//==============================================================================
/**
 * @brief Read data from device
 */
//==============================================================================
MODULE__DEVICE_READ(GPIO)
{
        UNUSED_ARG(device_handle);
        UNUSED_ARG(dst);
        UNUSED_ARG(item_size);
        UNUSED_ARG(lseek);
        UNUSED_ARG(n_items);

        return 0;
}

//==============================================================================
/**
 * @brief IO control
 */
//==============================================================================
MODULE__DEVICE_IOCTL(GPIO)
{
        UNUSED_ARG(device_handle);
        UNUSED_ARG(args);

        switch (iorq) {
        default:
                return STD_RET_ERROR;
        }

        return STD_RET_OK;
}

//==============================================================================
/**
 * @brief Flush device
 */
//==============================================================================
MODULE__DEVICE_FLUSH(GPIO)
{
        UNUSED_ARG(device_handle);

        return STD_RET_OK;
}

//==============================================================================
/**
 * @brief Interface returns device information
 */
//==============================================================================
MODULE__DEVICE_INFO(GPIO)
{
        UNUSED_ARG(device_handle);

        device_info->st_size = 0;

        return STD_RET_OK;
}

//==============================================================================
/**
 * @brief Initialize GPIOx
 *
 * @param[in] *gpio             GPIO address
 * @param[in] crl               CRL register value
 * @param[in] crh               CRH register value
 * @param[in] odr               ODR register value
 */
//==============================================================================
static void init_GPIOx(GPIO_t *gpio, u32_t crl, u32_t crh, u32_t odr)
{
        /* enable peripherals */
        switch ((u32_t)gpio) {
#if (_GPIOA_EN > 0)
        case GPIOA_BASE:
                RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
                break;
#endif

#if (_GPIOB_EN > 0)
        case GPIOB_BASE:
                RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;
                break;
#endif

#if (_GPIOC_EN > 0)
        case GPIOC_BASE:
                RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;
                break;
#endif

#if (_GPIOD_EN > 0)
        case GPIOD_BASE:
                RCC->APB2ENR |= RCC_APB2ENR_IOPDEN;
                break;
#endif

#if (_GPIOE_EN > 0)
        case GPIOE_BASE:
                RCC->APB2ENR |= RCC_APB2ENR_IOPEEN;
                break;
#endif

#if (_GPIOF_EN > 0)
        case GPIOF_BASE:
                RCC->APB2ENR |= RCC_APB2ENR_IOPFEN;
                break;
#endif

#if (_GPIOG_EN > 0)
        case GPIOG_BASE:
                RCC->APB2ENR |= RCC_APB2ENR_IOPGEN;
                break;
#endif

        default:
                return;
        }

        /* pin 0-7 configuration */
        gpio->CRL = crl;

        /* pin 8-15 configuration */
        gpio->CRH = crh;

        /* configure outputs and inputs initial state */
        gpio->ODR = odr;
}

//==============================================================================
/**
 * @brief Initialize AFIO peripheral
 */
//==============================================================================
static void init_AFIO(void)
{
        RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;

#if (SPI3_REMAP > 0)
        AFIO->MAPR |= AFIO_MAPR_SPI3_REMAP;
#endif
}


#ifdef __cplusplus
}
#endif

/*==============================================================================
                                             End of file
==============================================================================*/
