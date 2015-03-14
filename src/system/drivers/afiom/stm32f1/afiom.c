/*=========================================================================*//**
@file    afiom.c

@author  Daniel Zorychta

@brief   This driver support AFIO.

@note    Copyright (C) 2014  Daniel Zorychta <daniel.zorychta@gmail.com>

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
#include "core/module.h"
#include "stm32f1/afiom_cfg.h"
#include "stm32f1/afiom_def.h"
#include "stm32f1/stm32f10x.h"

/*==============================================================================
  Local symbolic constants/macros
==============================================================================*/
#define EXTICR(e3, e2, e1, e0) ( (((e3) & 0xF) << 12) | (((e2) & 0xF) << 8) | (((e1) & 0xF) << 4) | (((e0) & 0xF) << 0) )

/*==============================================================================
  Local types, enums definitions
==============================================================================*/

/*==============================================================================
  Local function prototypes
==============================================================================*/

/*==============================================================================
  Local object definitions
==============================================================================*/
static const uint32_t EVCR    = (_AFIOM_EVO_EN * AFIO_EVCR_EVOE) | (_AFIOM_EVO_PORT << 4) | (_AFIOM_EVO_PIN  << 0);

static const uint32_t MAPR    = (_AFIOM_REMAP_SPI1         * AFIO_MAPR_SPI1_REMAP        )
                              | (_AFIOM_REMAP_I2C1         * AFIO_MAPR_I2C1_REMAP        )
                              | (_AFIOM_REMAP_USART1       * AFIO_MAPR_USART1_REMAP      )
                              | (_AFIOM_REMAP_USART2       * AFIO_MAPR_USART2_REMAP      )
                              | (_AFIOM_REMAP_TIM4         * AFIO_MAPR_TIM4_REMAP        )
                              | (_AFIOM_REMAP_PD01         * AFIO_MAPR_PD01_REMAP        )
                              | (_AFIOM_REMAP_TIM5CH4      * AFIO_MAPR_TIM5CH4_IREMAP    )
                              | (_AFIOM_REMAP_ADC1_ETRGINJ * AFIO_MAPR_ADC1_ETRGINJ_REMAP)
                              | (_AFIOM_REMAP_ADC1_ETRGREG * AFIO_MAPR_ADC1_ETRGREG_REMAP)
                              | (_AFIOM_REMAP_ADC2_ETRGINJ * AFIO_MAPR_ADC2_ETRGINJ_REMAP)
                              | (_AFIOM_REMAP_ADC2_ETRGREG * AFIO_MAPR_ADC2_ETRGREG_REMAP)

                              #if   (_AFIOM_REMAP_USART3 == 0)
                              | (AFIO_MAPR_USART3_REMAP_NOREMAP)
                              #elif (_AFIOM_REMAP_USART3 == 1)
                              | (AFIO_MAPR_USART3_REMAP_PARTIALREMAP)
                              #elif (_AFIOM_REMAP_USART3 == 2)
                              | (AFIO_MAPR_USART3_REMAP_FULLREMAP)
                              #endif

                              #if   (_AFIOM_REMAP_TIM1 == 0)
                              | (AFIO_MAPR_TIM1_REMAP_NOREMAP)
                              #elif (_AFIOM_REMAP_TIM1 == 1)
                              | (AFIO_MAPR_TIM1_REMAP_PARTIALREMAP)
                              #elif (_AFIOM_REMAP_TIM1 == 2)
                              | (AFIO_MAPR_TIM1_REMAP_FULLREMAP)
                              #endif

                              #if   (_AFIOM_REMAP_TIM2 == 0)
                              | (AFIO_MAPR_TIM2_REMAP_NOREMAP)
                              #elif (_AFIOM_REMAP_TIM2 == 1)
                              | (AFIO_MAPR_TIM2_REMAP_PARTIALREMAP1)
                              #elif (_AFIOM_REMAP_TIM2 == 2)
                              | (AFIO_MAPR_TIM2_REMAP_PARTIALREMAP2)
                              #elif (_AFIOM_REMAP_TIM2 == 3)
                              | (AFIO_MAPR_TIM2_REMAP_FULLREMAP)
                              #endif

                              #if   (_AFIOM_REMAP_TIM3 == 0)
                              | (AFIO_MAPR_TIM3_REMAP_NOREMAP)
                              #elif (_AFIOM_REMAP_TIM3 == 1)
                              | (AFIO_MAPR_TIM3_REMAP_PARTIALREMAP)
                              #elif (_AFIOM_REMAP_TIM3 == 2)
                              | (AFIO_MAPR_TIM3_REMAP_FULLREMAP)
                              #endif

                              #if   (_AFIOM_REMAP_CAN == 0)
                              | (AFIO_MAPR_CAN_REMAP_REMAP1)
                              #elif (_AFIOM_REMAP_CAN == 1)
                              | (AFIO_MAPR_CAN_REMAP_REMAP2)
                              #elif (_AFIOM_REMAP_CAN == 2)
                              | (AFIO_MAPR_CAN_REMAP_REMAP3)
                              #endif

                              #if   (_AFIOM_REMAP_SWJ_CFG == 0)
                              | (AFIO_MAPR_SWJ_CFG_RESET)
                              #elif (_AFIOM_REMAP_SWJ_CFG == 1)
                              | (AFIO_MAPR_SWJ_CFG_NOJNTRST)
                              #elif (_AFIOM_REMAP_SWJ_CFG == 2)
                              | (AFIO_MAPR_SWJ_CFG_JTAGDISABLE)
                              #elif (_AFIOM_REMAP_SWJ_CFG == 3)
                              | (AFIO_MAPR_SWJ_CFG_DISABLE)
                              #endif

                              #ifdef STM32F10X_CL
                              | (_AFIOM_REMAP_ETH          * AFIO_MAPR_ETH_REMAP      )
                              | (_AFIOM_REMAP_CAN2         * AFIO_MAPR_CAN2_REMAP     )
                              | (_AFIOM_REMAP_MII_RMII_SEL * AFIO_MAPR_MII_RMII_SEL   )
                              | (_AFIOM_REMAP_SPI3         * AFIO_MAPR_SPI3_REMAP     )
                              | (_AFIOM_REMAP_TIM2ITR1     * AFIO_MAPR_TIM2ITR1_IREMAP)
                              | (_AFIOM_REMAP_PTP_PPS      * AFIO_MAPR_PTP_PPS_REMAP  )
                              #endif
                              ;

static const uint32_t MAPR2   = 0
                              #if defined(STM32F10X_LD_VL) || defined(STM32F10X_MD_VL) || defined(STM32F10X_HD_VL)
                              | (_AFIOM_REMAP_TIM15    * AFIO_MAPR2_TIM15_REMAP   )
                              | (_AFIOM_REMAP_TIM16    * AFIO_MAPR2_TIM16_REMAP   )
                              | (_AFIOM_REMAP_TIM17    * AFIO_MAPR2_TIM17_REMAP   )
                              | (_AFIOM_REMAP_CEC      * AFIO_MAPR2_CEC_REMAP     )
                              | (_AFIOM_REMAP_TIM1_DMA * AFIO_MAPR2_TIM1_DMA_REMAP)
                              #endif

                              #ifdef STM32F10X_HD_VL
                              | (_AFIOM_REMAP_TIM13         * AFIO_MAPR2_TIM13_REMAP        )
                              | (_AFIOM_REMAP_TIM14         * AFIO_MAPR2_TIM14_REMAP        )
                              | (_AFIOM_REMAP_FSMC_NADV     * AFIO_MAPR2_FSMC_NADV_REMAP    )
                              | (_AFIOM_REMAP_TIM76_DAC_DMA * AFIO_MAPR2_TIM67_DAC_DMA_REMAP)
                              | (_AFIOM_REMAP_TIM12         * AFIO_MAPR2_TIM12_REMAP        )
                              | (_AFIOM_REMAP_MISC          * AFIO_MAPR2_MISC_REMAP         )
                              #endif

                              #ifdef STM32F10X_XL
                              | (_AFIOM_REMAP_TIM9      * AFIO_MAPR2_TIM9_REMAP     )
                              | (_AFIOM_REMAP_TIM10     * AFIO_MAPR2_TIM10_REMAP    )
                              | (_AFIOM_REMAP_TIM11     * AFIO_MAPR2_TIM11_REMAP    )
                              | (_AFIOM_REMAP_TIM13     * AFIO_MAPR2_TIM13_REMAP    )
                              | (_AFIOM_REMAP_TIM14     * AFIO_MAPR2_TIM14_REMAP    )
                              | (_AFIOM_REMAP_FSMC_NADV * AFIO_MAPR2_FSMC_NADV_REMAP)
                              #endif
                              ;

static const uint32_t EXTICR1 = EXTICR(_AFIOM_EXTI3_PORT,  _AFIOM_EXTI2_PORT,  _AFIOM_EXTI1_PORT,  _AFIOM_EXTI0_PORT );
static const uint32_t EXTICR2 = EXTICR(_AFIOM_EXTI7_PORT,  _AFIOM_EXTI6_PORT,  _AFIOM_EXTI5_PORT,  _AFIOM_EXTI4_PORT );
static const uint32_t EXTICR3 = EXTICR(_AFIOM_EXTI11_PORT, _AFIOM_EXTI10_PORT, _AFIOM_EXTI9_PORT,  _AFIOM_EXTI8_PORT );
static const uint32_t EXTICR4 = EXTICR(_AFIOM_EXTI15_PORT, _AFIOM_EXTI14_PORT, _AFIOM_EXTI13_PORT, _AFIOM_EXTI12_PORT);

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
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
API_MOD_INIT(AFIOM, void **device_handle, u8_t major, u8_t minor)
{
        UNUSED_ARG(device_handle);
        UNUSED_ARG(major);
        UNUSED_ARG(minor);

        if (!(RCC->APB2ENR & RCC_APB2ENR_AFIOEN)) {
                SET_BIT(RCC->APB2ENR, RCC_APB2ENR_AFIOEN);

                AFIO->EVCR      = EVCR;
                AFIO->MAPR      = MAPR;
                AFIO->MAPR2     = MAPR2;
                AFIO->EXTICR[0] = EXTICR1;
                AFIO->EXTICR[1] = EXTICR2;
                AFIO->EXTICR[2] = EXTICR3;
                AFIO->EXTICR[3] = EXTICR4;

                return STD_RET_OK;
        } else {
                return STD_RET_ERROR;
        }
}

//==============================================================================
/**
 * @brief Release device
 *
 * @param[in ]          *device_handle          device allocated memory
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
API_MOD_RELEASE(AFIOM, void *device_handle)
{
        UNUSED_ARG(device_handle);

        SET_BIT(RCC->APB2RSTR, RCC_APB2RSTR_AFIORST);
        CLEAR_BIT(RCC->APB2RSTR, RCC_APB2RSTR_AFIORST);
        CLEAR_BIT(RCC->APB2ENR, RCC_APB2ENR_AFIOEN);

        return STD_RET_OK;
}

//==============================================================================
/**
 * @brief Open device
 *
 * @param[in ]          *device_handle          device allocated memory
 * @param[in ]           flags                  file operation flags (O_RDONLY, O_WRONLY, O_RDWR)
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
API_MOD_OPEN(AFIOM, void *device_handle, vfs_open_flags_t flags)
{
        UNUSED_ARG(device_handle);
        UNUSED_ARG(flags);

        return STD_RET_OK;
}

//==============================================================================
/**
 * @brief Close device
 *
 * @param[in ]          *device_handle          device allocated memory
 * @param[in ]           force                  device force close (true)
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
API_MOD_CLOSE(AFIOM, void *device_handle, bool force)
{
        UNUSED_ARG(device_handle);
        UNUSED_ARG(force);

        return STD_RET_OK;
}

//==============================================================================
/**
 * @brief Write data to device
 *
 * @param[in ]          *device_handle          device allocated memory
 * @param[in ]          *src                    data source
 * @param[in ]           count                  number of bytes to write
 * @param[in ][out]     *fpos                   file position
 * @param[in ]           fattr                  file attributes
 *
 * @return number of written bytes, -1 if error
 */
//==============================================================================
API_MOD_WRITE(AFIOM, void *device_handle, const u8_t *src, size_t count, fpos_t *fpos, struct vfs_fattr fattr)
{
        UNUSED_ARG(device_handle);
        UNUSED_ARG(src);
        UNUSED_ARG(count);
        UNUSED_ARG(fpos);
        UNUSED_ARG(fattr);

        errno = EPERM;

        return 0;
}

//==============================================================================
/**
 * @brief Read data from device
 *
 * @param[in ]          *device_handle          device allocated memory
 * @param[out]          *dst                    data destination
 * @param[in ]           count                  number of bytes to read
 * @param[in ][out]     *fpos                   file position
 * @param[in ]           fattr                  file attributes
 *
 * @return number of read bytes, -1 if error
 */
//==============================================================================
API_MOD_READ(AFIOM, void *device_handle, u8_t *dst, size_t count, fpos_t *fpos, struct vfs_fattr fattr)
{
        UNUSED_ARG(device_handle);
        UNUSED_ARG(dst);
        UNUSED_ARG(count);
        UNUSED_ARG(fpos);
        UNUSED_ARG(fattr);

        errno = EPERM;

        return 0;
}

//==============================================================================
/**
 * @brief IO control
 *
 * @param[in ]          *device_handle          device allocated memory
 * @param[in ]           request                request
 * @param[in ][out]     *arg                    request's argument
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
API_MOD_IOCTL(AFIOM, void *device_handle, int request, void *arg)
{
        UNUSED_ARG(device_handle);
        UNUSED_ARG(arg);

        switch (request) {
        default:
                errno = EBADRQC;
                return STD_RET_ERROR;
        }

        return STD_RET_OK;
}

//==============================================================================
/**
 * @brief Flush device
 *
 * @param[in ]          *device_handle          device allocated memory
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
API_MOD_FLUSH(AFIOM, void *device_handle)
{
        UNUSED_ARG(device_handle);

        return STD_RET_OK;
}

//==============================================================================
/**
 * @brief Device information
 *
 * @param[in ]          *device_handle          device allocated memory
 * @param[out]          *device_stat            device status
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
API_MOD_STAT(AFIOM, void *device_handle, struct vfs_dev_stat *device_stat)
{
        UNUSED_ARG(device_handle);

        device_stat->st_size  = 0;
        device_stat->st_major = 0;
        device_stat->st_minor = 0;

        return STD_RET_OK;
}

/*==============================================================================
                                             End of file
==============================================================================*/
