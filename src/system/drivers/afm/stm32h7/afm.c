/*=========================================================================*//**
@file    afm.c

@author  Daniel Zorychta

@brief   This driver support AFM.

@note    Copyright (C) 2022  Daniel Zorychta <daniel.zorychta@gmail.com>

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
#include "stm32h7/stm32h7xx.h"
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

                SET_BIT(RCC->APB4ENR, RCC_APB4ENR_SYSCFGEN);
                __IO uint32_t tmpreg = READ_BIT(RCC->APB4ENR, RCC_APB4ENR_SYSCFGEN);
                UNUSED_ARG1(tmpreg);

                SYSCFG->PMCR = __AFM_PC3SO__
                             | __AFM_PC2SO__
                             | __AFM_PA1SO__
                             | __AFM_PA0SO__
                             | __AFM_MII_RMII_SEL__
                             | __AFM_BOOSTVDDSEL__
                             | __AFM_BOOSTEREN__
                             | __AFM_PB9_FMP__
                             | __AFM_PB8_FMP__
                             | __AFM_PB7_FMP__
                             | __AFM_PB6_FMP__
                             | __AFM_I2C4_FMP__
                             | __AFM_I2C3_FMP__
                             | __AFM_I2C2_FMP__
                             | __AFM_I2C1_FMP__;

                SYSCFG->CFGR = __AFM_AXISRAML__
                             | __AFM_ITCML__
                             | __AFM_DTCML__
                             | __AFM_SRAM1L__
                             | __AFM_SRAM2L__
                             | __AFM_SRAM3L__
                             | __AFM_SRAM4L__
                             | __AFM_BKRAML__
                             | __AFM_CM7L__
                             | __AFM_FLASHL__
                             | __AFM_PVDL__;

                SYSCFG->CCCR  = (__AFM_PCC__ << SYSCFG_CCCR_PCC_Pos)
                              | (__AFM_NCC__ << SYSCFG_CCCR_NCC_Pos);

                if (__AFM_CMP_EN__) {
                        SET_BIT(RCC->CR, RCC_CR_CSION);

                        SYSCFG->CCCSR = __AFM_HSLV__
                                      | __AFM_CS__
                                      | __AFM_CMP_EN__;
                }

                SYSCFG->UR13 = __AFM_D1SBRST__;
                SYSCFG->UR14 = __AFM_D1STPRST__;

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
API_MOD_RELEASE(AFM, void *device_handle)
{
        UNUSED_ARG1(device_handle);

        SET_BIT(RCC->APB4RSTR, RCC_APB4RSTR_SYSCFGRST);
        CLEAR_BIT(RCC->APB4RSTR, RCC_APB4RSTR_SYSCFGRST);
        CLEAR_BIT(RCC->APB4ENR, RCC_APB4ENR_SYSCFGEN);

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
