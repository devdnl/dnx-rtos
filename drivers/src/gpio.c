/*=============================================================================================*//**
@file    gpio.c

@author  Daniel Zorychta

@brief   This driver support GPIO. GPIO driver not provide any interface functions. All operations
         on ports should be made direct via definitions (much faster). When operation on a ports are
         needed please write own driver which controls pins directly and register it in the VFS if
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


*//*==============================================================================================*/

#ifdef __cplusplus
   extern "C" {
#endif

/*==================================================================================================
                                             Include files
==================================================================================================*/
#include "gpio.h"


/*==================================================================================================
                                   Local symbolic constants/macros
==================================================================================================*/
/** define pin configure size (CNF[1:0] + MODE[1:0]) */
#define GPIO_PIN_CFG_SIZE                 4U

/** define CRL configuration macro */
#define GPIO_SET_CRL(CFG, PIN)            ( CFG << (GPIO_PIN_CFG_SIZE * PIN) )

/** define CRH configuration macro */
#define GPIO_SET_CRH(CFG, PIN)            ( CFG << (GPIO_PIN_CFG_SIZE * (PIN - 8)) )

#if (GPIOA_EN != 0)
/** CRL register value for GPIOA */
#define GPIOA_CRL    ( GPIO_SET_CRL(GPIOA_PIN_00_MODE, 0 ) | GPIO_SET_CRL(GPIOA_PIN_01_MODE, 1 ) \
                     | GPIO_SET_CRL(GPIOA_PIN_02_MODE, 2 ) | GPIO_SET_CRL(GPIOA_PIN_03_MODE, 3 ) \
                     | GPIO_SET_CRL(GPIOA_PIN_04_MODE, 4 ) | GPIO_SET_CRL(GPIOA_PIN_05_MODE, 5 ) \
                     | GPIO_SET_CRL(GPIOA_PIN_06_MODE, 6 ) | GPIO_SET_CRL(GPIOA_PIN_07_MODE, 7 ) )

/** CRH register value for GPIOA */
#define GPIOA_CRH    ( GPIO_SET_CRH(GPIOA_PIN_08_MODE, 8 ) | GPIO_SET_CRH(GPIOA_PIN_09_MODE, 9 ) \
                     | GPIO_SET_CRH(GPIOA_PIN_10_MODE, 10) | GPIO_SET_CRH(GPIOA_PIN_11_MODE, 11) \
                     | GPIO_SET_CRH(GPIOA_PIN_12_MODE, 12) | GPIO_SET_CRH(GPIOA_PIN_13_MODE, 13) \
                     | GPIO_SET_CRH(GPIOA_PIN_14_MODE, 14) | GPIO_SET_CRH(GPIOA_PIN_15_MODE, 15) )

/** ODR register value for GPIOA */
#define GPIOA_ODR    ( GPIOA_PIN_00_DEFAULT_STATE | GPIOA_PIN_01_DEFAULT_STATE \
                     | GPIOA_PIN_02_DEFAULT_STATE | GPIOA_PIN_03_DEFAULT_STATE \
                     | GPIOA_PIN_04_DEFAULT_STATE | GPIOA_PIN_05_DEFAULT_STATE \
                     | GPIOA_PIN_06_DEFAULT_STATE | GPIOA_PIN_07_DEFAULT_STATE \
                     | GPIOA_PIN_08_DEFAULT_STATE | GPIOA_PIN_09_DEFAULT_STATE \
                     | GPIOA_PIN_10_DEFAULT_STATE | GPIOA_PIN_11_DEFAULT_STATE \
                     | GPIOA_PIN_12_DEFAULT_STATE | GPIOA_PIN_13_DEFAULT_STATE \
                     | GPIOA_PIN_14_DEFAULT_STATE | GPIOA_PIN_15_DEFAULT_STATE )
#endif

#if (GPIOB_EN != 0)
/** CRL register value for GPIOB */
#define GPIOB_CRL    ( GPIO_SET_CRL(GPIOB_PIN_00_MODE, 0 ) | GPIO_SET_CRL(GPIOB_PIN_01_MODE, 1 ) \
                     | GPIO_SET_CRL(GPIOB_PIN_02_MODE, 2 ) | GPIO_SET_CRL(GPIOB_PIN_03_MODE, 3 ) \
                     | GPIO_SET_CRL(GPIOB_PIN_04_MODE, 4 ) | GPIO_SET_CRL(GPIOB_PIN_05_MODE, 5 ) \
                     | GPIO_SET_CRL(GPIOB_PIN_06_MODE, 6 ) | GPIO_SET_CRL(GPIOB_PIN_07_MODE, 7 ) )

/** CRH register value for GPIOB */
#define GPIOB_CRH    ( GPIO_SET_CRH(GPIOB_PIN_08_MODE, 8 ) | GPIO_SET_CRH(GPIOB_PIN_09_MODE, 9 ) \
                     | GPIO_SET_CRH(GPIOB_PIN_10_MODE, 10) | GPIO_SET_CRH(GPIOB_PIN_11_MODE, 11) \
                     | GPIO_SET_CRH(GPIOB_PIN_12_MODE, 12) | GPIO_SET_CRH(GPIOB_PIN_13_MODE, 13) \
                     | GPIO_SET_CRH(GPIOB_PIN_14_MODE, 14) | GPIO_SET_CRH(GPIOB_PIN_15_MODE, 15) )

/** ODR register value for GPIOB */
#define GPIOB_ODR    ( GPIOB_PIN_00_DEFAULT_STATE | GPIOB_PIN_01_DEFAULT_STATE \
                     | GPIOB_PIN_02_DEFAULT_STATE | GPIOB_PIN_03_DEFAULT_STATE \
                     | GPIOB_PIN_04_DEFAULT_STATE | GPIOB_PIN_05_DEFAULT_STATE \
                     | GPIOB_PIN_06_DEFAULT_STATE | GPIOB_PIN_07_DEFAULT_STATE \
                     | GPIOB_PIN_08_DEFAULT_STATE | GPIOB_PIN_09_DEFAULT_STATE \
                     | GPIOB_PIN_10_DEFAULT_STATE | GPIOB_PIN_11_DEFAULT_STATE \
                     | GPIOB_PIN_12_DEFAULT_STATE | GPIOB_PIN_13_DEFAULT_STATE \
                     | GPIOB_PIN_14_DEFAULT_STATE | GPIOB_PIN_15_DEFAULT_STATE )
#endif

#if (GPIOC_EN != 0)
/** CRL register value for GPIOC */
#define GPIOC_CRL    ( GPIO_SET_CRL(GPIOC_PIN_00_MODE, 0 ) | GPIO_SET_CRL(GPIOC_PIN_01_MODE, 1 ) \
                     | GPIO_SET_CRL(GPIOC_PIN_02_MODE, 2 ) | GPIO_SET_CRL(GPIOC_PIN_03_MODE, 3 ) \
                     | GPIO_SET_CRL(GPIOC_PIN_04_MODE, 4 ) | GPIO_SET_CRL(GPIOC_PIN_05_MODE, 5 ) \
                     | GPIO_SET_CRL(GPIOC_PIN_06_MODE, 6 ) | GPIO_SET_CRL(GPIOC_PIN_07_MODE, 7 ) )

/** CRH register value for GPIOC */
#define GPIOC_CRH    ( GPIO_SET_CRH(GPIOC_PIN_08_MODE, 8 ) | GPIO_SET_CRH(GPIOC_PIN_09_MODE, 9 ) \
                     | GPIO_SET_CRH(GPIOC_PIN_10_MODE, 10) | GPIO_SET_CRH(GPIOC_PIN_11_MODE, 11) \
                     | GPIO_SET_CRH(GPIOC_PIN_12_MODE, 12) | GPIO_SET_CRH(GPIOC_PIN_13_MODE, 13) \
                     | GPIO_SET_CRH(GPIOC_PIN_14_MODE, 14) | GPIO_SET_CRH(GPIOC_PIN_15_MODE, 15) )

/** ODR register value for GPIOC */
#define GPIOC_ODR    ( GPIOC_PIN_00_DEFAULT_STATE | GPIOC_PIN_01_DEFAULT_STATE \
                     | GPIOC_PIN_02_DEFAULT_STATE | GPIOC_PIN_03_DEFAULT_STATE \
                     | GPIOC_PIN_04_DEFAULT_STATE | GPIOC_PIN_05_DEFAULT_STATE \
                     | GPIOC_PIN_06_DEFAULT_STATE | GPIOC_PIN_07_DEFAULT_STATE \
                     | GPIOC_PIN_08_DEFAULT_STATE | GPIOC_PIN_09_DEFAULT_STATE \
                     | GPIOC_PIN_10_DEFAULT_STATE | GPIOC_PIN_11_DEFAULT_STATE \
                     | GPIOC_PIN_12_DEFAULT_STATE | GPIOC_PIN_13_DEFAULT_STATE \
                     | GPIOC_PIN_14_DEFAULT_STATE | GPIOC_PIN_15_DEFAULT_STATE )
#endif

#if (GPIOD_EN != 0)
/** CRL register value for GPIOD */
#define GPIOD_CRL    ( GPIO_SET_CRL(GPIOD_PIN_00_MODE, 0 ) | GPIO_SET_CRL(GPIOD_PIN_01_MODE, 1 ) \
                     | GPIO_SET_CRL(GPIOD_PIN_02_MODE, 2 ) | GPIO_SET_CRL(GPIOD_PIN_03_MODE, 3 ) \
                     | GPIO_SET_CRL(GPIOD_PIN_04_MODE, 4 ) | GPIO_SET_CRL(GPIOD_PIN_05_MODE, 5 ) \
                     | GPIO_SET_CRL(GPIOD_PIN_06_MODE, 6 ) | GPIO_SET_CRL(GPIOD_PIN_07_MODE, 7 ) )

/** CRH register value for GPIOD */
#define GPIOD_CRH    ( GPIO_SET_CRH(GPIOD_PIN_08_MODE, 8 ) | GPIO_SET_CRH(GPIOD_PIN_09_MODE, 9 ) \
                     | GPIO_SET_CRH(GPIOD_PIN_10_MODE, 10) | GPIO_SET_CRH(GPIOD_PIN_11_MODE, 11) \
                     | GPIO_SET_CRH(GPIOD_PIN_12_MODE, 12) | GPIO_SET_CRH(GPIOD_PIN_13_MODE, 13) \
                     | GPIO_SET_CRH(GPIOD_PIN_14_MODE, 14) | GPIO_SET_CRH(GPIOD_PIN_15_MODE, 15) )

/** ODR register value for GPIOD */
#define GPIOD_ODR    ( GPIOD_PIN_00_DEFAULT_STATE | GPIOD_PIN_01_DEFAULT_STATE \
                     | GPIOD_PIN_02_DEFAULT_STATE | GPIOD_PIN_03_DEFAULT_STATE \
                     | GPIOD_PIN_04_DEFAULT_STATE | GPIOD_PIN_05_DEFAULT_STATE \
                     | GPIOD_PIN_06_DEFAULT_STATE | GPIOD_PIN_07_DEFAULT_STATE \
                     | GPIOD_PIN_08_DEFAULT_STATE | GPIOD_PIN_09_DEFAULT_STATE \
                     | GPIOD_PIN_10_DEFAULT_STATE | GPIOD_PIN_11_DEFAULT_STATE \
                     | GPIOD_PIN_12_DEFAULT_STATE | GPIOD_PIN_13_DEFAULT_STATE \
                     | GPIOD_PIN_14_DEFAULT_STATE | GPIOD_PIN_15_DEFAULT_STATE )
#endif

#if (GPIOE_EN != 0)
/** CRL register value for GPIOE */
#define GPIOE_CRL    ( GPIO_SET_CRL(GPIOE_PIN_00_MODE, 0 ) | GPIO_SET_CRL(GPIOE_PIN_01_MODE, 1 ) \
                     | GPIO_SET_CRL(GPIOE_PIN_02_MODE, 2 ) | GPIO_SET_CRL(GPIOE_PIN_03_MODE, 3 ) \
                     | GPIO_SET_CRL(GPIOE_PIN_04_MODE, 4 ) | GPIO_SET_CRL(GPIOE_PIN_05_MODE, 5 ) \
                     | GPIO_SET_CRL(GPIOE_PIN_06_MODE, 6 ) | GPIO_SET_CRL(GPIOE_PIN_07_MODE, 7 ) )

/** CRH register value for GPIOE */
#define GPIOE_CRH    ( GPIO_SET_CRH(GPIOE_PIN_08_MODE, 8 ) | GPIO_SET_CRH(GPIOE_PIN_09_MODE, 9 ) \
                     | GPIO_SET_CRH(GPIOE_PIN_10_MODE, 10) | GPIO_SET_CRH(GPIOE_PIN_11_MODE, 11) \
                     | GPIO_SET_CRH(GPIOE_PIN_12_MODE, 12) | GPIO_SET_CRH(GPIOE_PIN_13_MODE, 13) \
                     | GPIO_SET_CRH(GPIOE_PIN_14_MODE, 14) | GPIO_SET_CRH(GPIOE_PIN_15_MODE, 15) )

/** ODR register value for GPIOE */
#define GPIOE_ODR    ( GPIOE_PIN_00_DEFAULT_STATE | GPIOE_PIN_01_DEFAULT_STATE \
                     | GPIOE_PIN_02_DEFAULT_STATE | GPIOE_PIN_03_DEFAULT_STATE \
                     | GPIOE_PIN_04_DEFAULT_STATE | GPIOE_PIN_05_DEFAULT_STATE \
                     | GPIOE_PIN_06_DEFAULT_STATE | GPIOE_PIN_07_DEFAULT_STATE \
                     | GPIOE_PIN_08_DEFAULT_STATE | GPIOE_PIN_09_DEFAULT_STATE \
                     | GPIOE_PIN_10_DEFAULT_STATE | GPIOE_PIN_11_DEFAULT_STATE \
                     | GPIOE_PIN_12_DEFAULT_STATE | GPIOE_PIN_13_DEFAULT_STATE \
                     | GPIOE_PIN_14_DEFAULT_STATE | GPIOE_PIN_15_DEFAULT_STATE )
#endif

#if (GPIOF_EN != 0)
/** CRL register value for GPIOF */
#define GPIOF_CRL    ( GPIO_SET_CRL(GPIOF_PIN_00_MODE, 0 ) | GPIO_SET_CRL(GPIOF_PIN_01_MODE, 1 ) \
                     | GPIO_SET_CRL(GPIOF_PIN_02_MODE, 2 ) | GPIO_SET_CRL(GPIOF_PIN_03_MODE, 3 ) \
                     | GPIO_SET_CRL(GPIOF_PIN_04_MODE, 4 ) | GPIO_SET_CRL(GPIOF_PIN_05_MODE, 5 ) \
                     | GPIO_SET_CRL(GPIOF_PIN_06_MODE, 6 ) | GPIO_SET_CRL(GPIOF_PIN_07_MODE, 7 ) )

/** CRH register value for GPIOF */
#define GPIOF_CRH    ( GPIO_SET_CRH(GPIOF_PIN_08_MODE, 8 ) | GPIO_SET_CRH(GPIOF_PIN_09_MODE, 9 ) \
                     | GPIO_SET_CRH(GPIOF_PIN_10_MODE, 10) | GPIO_SET_CRH(GPIOF_PIN_11_MODE, 11) \
                     | GPIO_SET_CRH(GPIOF_PIN_12_MODE, 12) | GPIO_SET_CRH(GPIOF_PIN_13_MODE, 13) \
                     | GPIO_SET_CRH(GPIOF_PIN_14_MODE, 14) | GPIO_SET_CRH(GPIOF_PIN_15_MODE, 15) )

/** ODR register value for GPIOF */
#define GPIOF_ODR    ( GPIOF_PIN_00_DEFAULT_STATE | GPIOF_PIN_01_DEFAULT_STATE \
                     | GPIOF_PIN_02_DEFAULT_STATE | GPIOF_PIN_03_DEFAULT_STATE \
                     | GPIOF_PIN_04_DEFAULT_STATE | GPIOF_PIN_05_DEFAULT_STATE \
                     | GPIOF_PIN_06_DEFAULT_STATE | GPIOF_PIN_07_DEFAULT_STATE \
                     | GPIOF_PIN_08_DEFAULT_STATE | GPIOF_PIN_09_DEFAULT_STATE \
                     | GPIOF_PIN_10_DEFAULT_STATE | GPIOF_PIN_11_DEFAULT_STATE \
                     | GPIOF_PIN_12_DEFAULT_STATE | GPIOF_PIN_13_DEFAULT_STATE \
                     | GPIOF_PIN_14_DEFAULT_STATE | GPIOF_PIN_15_DEFAULT_STATE )
#endif

#if (GPIOG_EN != 0)
/** CRL register value for GPIOG */
#define GPIOG_CRL    ( GPIO_SET_CRL(GPIOG_PIN_00_MODE, 0 ) | GPIO_SET_CRL(GPIOG_PIN_01_MODE, 1 ) \
                     | GPIO_SET_CRL(GPIOG_PIN_02_MODE, 2 ) | GPIO_SET_CRL(GPIOG_PIN_03_MODE, 3 ) \
                     | GPIO_SET_CRL(GPIOG_PIN_04_MODE, 4 ) | GPIO_SET_CRL(GPIOG_PIN_05_MODE, 5 ) \
                     | GPIO_SET_CRL(GPIOG_PIN_06_MODE, 6 ) | GPIO_SET_CRL(GPIOG_PIN_07_MODE, 7 ) )

/** CRH register value for GPIOG */
#define GPIOG_CRH    ( GPIO_SET_CRH(GPIOG_PIN_08_MODE, 8 ) | GPIO_SET_CRH(GPIOG_PIN_09_MODE, 9 ) \
                     | GPIO_SET_CRH(GPIOG_PIN_10_MODE, 10) | GPIO_SET_CRH(GPIOG_PIN_11_MODE, 11) \
                     | GPIO_SET_CRH(GPIOG_PIN_12_MODE, 12) | GPIO_SET_CRH(GPIOG_PIN_13_MODE, 13) \
                     | GPIO_SET_CRH(GPIOG_PIN_14_MODE, 14) | GPIO_SET_CRH(GPIOG_PIN_15_MODE, 15) )

/** ODR register value for GPIOG */
#define GPIOG_ODR    ( GPIOG_PIN_00_DEFAULT_STATE | GPIOG_PIN_01_DEFAULT_STATE \
                     | GPIOG_PIN_02_DEFAULT_STATE | GPIOG_PIN_03_DEFAULT_STATE \
                     | GPIOG_PIN_04_DEFAULT_STATE | GPIOG_PIN_05_DEFAULT_STATE \
                     | GPIOG_PIN_06_DEFAULT_STATE | GPIOG_PIN_07_DEFAULT_STATE \
                     | GPIOG_PIN_08_DEFAULT_STATE | GPIOG_PIN_09_DEFAULT_STATE \
                     | GPIOG_PIN_10_DEFAULT_STATE | GPIOG_PIN_11_DEFAULT_STATE \
                     | GPIOG_PIN_12_DEFAULT_STATE | GPIOG_PIN_13_DEFAULT_STATE \
                     | GPIOG_PIN_14_DEFAULT_STATE | GPIOG_PIN_15_DEFAULT_STATE )
#endif


/*==================================================================================================
                                   Local types, enums definitions
==================================================================================================*/


/*==================================================================================================
                                      Local function prototypes
==================================================================================================*/
static void InitGPIOx(GPIO_t *gpio_p, u32_t crl_u32, u32_t crh_u32, u32_t odr_u32);


/*==================================================================================================
                                      Local object definitions
==================================================================================================*/


/*==================================================================================================
                                     Exported object definitions
==================================================================================================*/


/*==================================================================================================
                                         Function definitions
==================================================================================================*/

//================================================================================================//
/**
 * @brief Initialise all GPIOs
 */
//================================================================================================//
stdStatus_t GPIO_Init(void)
{
      #if (GPIOA_EN != 0)
           InitGPIOx(GPIOA, GPIOA_CRL, GPIOA_CRH, GPIOA_ODR);
      #endif

      #if (GPIOB_EN != 0)
           InitGPIOx(GPIOB, GPIOB_CRL, GPIOB_CRH, GPIOB_ODR);
      #endif

      #if (GPIOC_EN != 0)
           InitGPIOx(GPIOC, GPIOC_CRL, GPIOC_CRH, GPIOC_ODR);
      #endif

      #if (GPIOD_EN != 0)
           InitGPIOx(GPIOD, GPIOD_CRL, GPIOD_CRH, GPIOD_ODR);
      #endif

      #if (GPIOE_EN != 0)
           InitGPIOx(GPIOE, GPIOE_CRL, GPIOE_CRH, GPIOE_ODR);
      #endif

      #if (GPIOF_EN != 0)
            InitGPIOx(GPIOF, GPIOF_CRL, GPIOF_CRH, GPIOF_ODR);
      #endif

      #if (GPIOG_EN != 0)
            InitGPIOx(GPIOG, GPIOG_CRL, GPIOG_CRH, GPIOG_ODR);
      #endif

      return STD_STATUS_OK;
}


//================================================================================================//
/**
 * @brief Open device
 *
 * @param dev     device number
 *
 * @retval STD_STATUS_OK
 */
//================================================================================================//
stdStatus_t GPIO_Open(dev_t dev)
{
      (void)dev;

      return STD_STATUS_OK;
}


//================================================================================================//
/**
 * @brief Close device
 *
 * @param dev     device number
 *
 * @retval STD_STATUS_OK
 */
//================================================================================================//
stdStatus_t GPIO_Close(dev_t dev)
{
      (void)dev;

      return STD_STATUS_OK;
}


//================================================================================================//
/**
 * @brief Write to the device
 *
 * @param dev     device number
 * @param *src    source
 * @param size    size
 * @param seek    seek
 *
 * @retval STD_STATUS_OK
 */
//================================================================================================//
stdStatus_t GPIO_Write(dev_t dev, void *src, size_t size, size_t seek)
{
      (void)dev;
      (void)src;
      (void)size;
      (void)seek;

      return STD_STATUS_OK;
}


//================================================================================================//
/**
 * @brief Read from device
 *
 * @param dev     device number
 * @param *dst    destination
 * @param size    size
 * @param seek    seek
 *
 * @retval STD_STATUS_OK
 */
//================================================================================================//
stdStatus_t GPIO_Read(dev_t dev, void *dst, size_t size, size_t seek)
{
      (void)dev;
      (void)dst;
      (void)size;
      (void)seek;

      return STD_STATUS_OK;
}


//================================================================================================//
/**
 * @brief IO control
 *
 * @param[in]     dev     device number
 * @param[in]     ioRq    IO reqest
 * @param[in,out] data    data pointer
 *
 * @retval STD_STATUS_OK
 */
//================================================================================================//
stdStatus_t GPIO_IOCtl(dev_t dev, IORq_t ioRq, void *data)
{
      (void)dev;
      (void)ioRq;
      (void)data;

      return STD_STATUS_OK;
}


//================================================================================================//
/**
 * @brief Initialize GPIOx
 *
 * @param[in] *gpio_p      GPIO address
 * @param[in] crl_u32      CRL register value
 * @param[in] crh_u32      CRH register value
 * @param[in] odr_u32      ODR register value
 */
//================================================================================================//
static void InitGPIOx(GPIO_t *gpio_p, u32_t crl_u32, u32_t crh_u32, u32_t odr_u32)
{
      /* enable peripherals */
      switch ((u32_t)gpio_p)
      {
            #if (GPIOA_EN)
                  case GPIOA_BASE: RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
            #endif

            #if (GPIOB_EN)
                  case GPIOB_BASE: RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;
            #endif

            #if (GPIOC_EN)
                  case GPIOC_BASE: RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;
            #endif

            #if (GPIOD_EN)
                  case GPIOD_BASE: RCC->APB2ENR |= RCC_APB2ENR_IOPDEN;
            #endif

            #if (GPIOE_EN)
                  case GPIOE_BASE: RCC->APB2ENR |= RCC_APB2ENR_IOPEEN;
            #endif

            #if (GPIOF_EN)
                  case GPIOF_BASE: RCC->APB2ENR |= RCC_APB2ENR_IOPFEN;
            #endif

            #if (GPIOG_EN)
                  case GPIOG_BASE: RCC->APB2ENR |= RCC_APB2ENR_IOPGEN;
            #endif

            default:
                  return;
      }

      /* pin 0-7 configuration */
      gpio_p->CRL = crl_u32;

      /* pin 8-15 configuration */
      gpio_p->CRH = crh_u32;

      /* configure outputs and inputs initial state */
      gpio_p->ODR = odr_u32;
}


#ifdef __cplusplus
   }
#endif

/*==================================================================================================
                                             End of file
==================================================================================================*/
