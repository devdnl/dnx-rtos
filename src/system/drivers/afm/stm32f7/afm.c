/*=========================================================================*//**
@file    afm.c

@author  Daniel Zorychta

@brief   This driver support AFM.

@note    Copyright (C) 2014  Daniel Zorychta <daniel.zorychta@gmail.com>

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
#include "stm32f7/afm_cfg.h"
#include "stm32f7/stm32f7xx.h"
#include "../afm_ioctl.h"

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
API_MOD_INIT(AFM, void **device_handle, u8_t major, u8_t minor, const void *config)
{
        UNUSED_ARG2(device_handle, config);

        if (major == 0 && minor == 0) {

                if (!(RCC->APB2ENR & RCC_APB2ENR_SYSCFGEN)) {
                        SET_BIT(RCC->APB2ENR, RCC_APB2ENR_SYSCFGEN);

                        SYSCFG->MEMRMP |= __AFM_SWP_FMC__
                                        #ifdef SYSCFG_MEMRMP_SWP_FB_Pos
                                        | __AFM_SWP_FB__
                                        #endif
                                        | __AFM_MEM_BOOT__;

                        SYSCFG->PMC |= __AFM_MII_RMII_SEL__
                                     #ifdef SYSCFG_PMC_PB9_FMP_Pos
                                     | __AFM_PB9_FMP__
                                     #endif
                                     #ifdef SYSCFG_PMC_PB8_FMP_Pos
                                     | __AFM_PB8_FMP__
                                     #endif
                                     #ifdef SYSCFG_PMC_PB7_FMP_Pos
                                     | __AFM_PB7_FMP__
                                     #endif
                                     #ifdef SYSCFG_PMC_PB6_FMP_Pos
                                     | __AFM_PB6_FMP__
                                     #endif
                                     #ifdef SYSCFG_PMC_I2C4_FMP_Pos
                                     | __AFM_I2C4_FMP__
                                     #endif
                                     #ifdef SYSCFG_PMC_I2C3_FMP_Pos
                                     | __AFM_I2C3_FMP__
                                     #endif
                                     #ifdef SYSCFG_PMC_I2C2_FMP_Pos
                                     | __AFM_I2C2_FMP__
                                     #endif
                                     #ifdef SYSCFG_PMC_I2C1_FMP_Pos
                                     | __AFM_I2C1_FMP__
                                     #endif
                                     | __AFM_ADCxDC2__;

                        SYSCFG->EXTICR[0] |= __AFM_EXTI0_PORT__ | __AFM_EXTI1_PORT__
                                           | __AFM_EXTI2_PORT__ | __AFM_EXTI3_PORT__;

                        SYSCFG->EXTICR[1] |= __AFM_EXTI4_PORT__ | __AFM_EXTI5_PORT__
                                           | __AFM_EXTI6_PORT__ | __AFM_EXTI7_PORT__;

                        SYSCFG->EXTICR[2] |= __AFM_EXTI8_PORT__  | __AFM_EXTI9_PORT__
                                           | __AFM_EXTI10_PORT__ | __AFM_EXTI11_PORT__;

                        SYSCFG->EXTICR[3] |= __AFM_EXTI12_PORT__ | __AFM_EXTI13_PORT__
                                           | __AFM_EXTI14_PORT__ | __AFM_EXTI15_PORT__;

                        #if defined(SYSCFG_CBR_CLL) || defined(SYSCFG_CBR_PVDL)
                        SYSCFG->CBR = __AFM_PVDL__ | __AFM_CLL__;
                        #endif

                        SYSCFG->CMPCR = __AFM_CMP_PD__;

                        return ESUCC;
                } else {
                        return EADDRINUSE;
                }
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
API_MOD_RELEASE(AFM, void *device_handle)
{
        UNUSED_ARG1(device_handle);

        SET_BIT(RCC->APB2RSTR, RCC_APB2RSTR_SYSCFGRST);
        CLEAR_BIT(RCC->APB2RSTR, RCC_APB2RSTR_SYSCFGRST);
        CLEAR_BIT(RCC->APB2ENR, RCC_APB2ENR_SYSCFGEN);

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
API_MOD_OPEN(AFM, void *device_handle, u32_t flags)
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
API_MOD_CLOSE(AFM, void *device_handle, bool force)
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
API_MOD_WRITE(AFM,
              void            *device_handle,
              const u8_t      *src,
              size_t           count,
              fpos_t          *fpos,
              size_t          *wrcnt,
              struct vfs_fattr fattr)
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
API_MOD_READ(AFM,
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
API_MOD_IOCTL(AFM, void *device_handle, int request, void *arg)
{
        UNUSED_ARG2(device_handle, arg);

        switch (request) {
        default:
                return EBADRQC;
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
API_MOD_FLUSH(AFM, void *device_handle)
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
API_MOD_STAT(AFM, void *device_handle, struct vfs_dev_stat *device_stat)
{
        UNUSED_ARG2(device_handle, device_stat);

        return ESUCC;
}

/*==============================================================================
                                             End of file
==============================================================================*/
