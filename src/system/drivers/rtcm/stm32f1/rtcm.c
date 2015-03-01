/*=========================================================================*//**
@file    rtcm.c

@author  Daniel Zorychta

@brief   Real Time Clock Module

@note    Copyright (C) 2015 Daniel Zorychta <daniel.zorychta@gmail.com>

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
#include "stm32f1/rtcm_cfg.h"
#include "stm32f1/rtcm_def.h"
#include "stm32f1/rtcm_ioctl.h"
#include "stm32f1/stm32f10x.h"
#include "stm32f1/lib/stm32f10x_rcc.h"

/*==============================================================================
  Local macros
==============================================================================*/
#define RTC_WRITE_ATTEMPTS      10000
#define TIMEOUT_LSE             250

/*==============================================================================
  Local object types
==============================================================================*/

/*==============================================================================
  Local function prototypes
==============================================================================*/

/*==============================================================================
  Local objects
==============================================================================*/
MODULE_NAME(RTCM);

/*==============================================================================
  Exported objects
==============================================================================*/

/*==============================================================================
  External objects
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
API_MOD_INIT(RTCM, void **device_handle, u8_t major, u8_t minor)
{
        UNUSED_ARG(device_handle);
        UNUSED_ARG(major);
        UNUSED_ARG(minor);
        UNUSED_ARG(_module_name_);

        u32_t PRL = 1;
        switch (_RTCM_CFG__RTCCLK_SRC) {
        case RCC_RTCCLKSource_LSE:        PRL = 32768 - 1; break;
        case RCC_RTCCLKSource_LSI:        PRL = 40000 - 1; break;
        case RCC_RTCCLKSource_HSE_Div128: PRL = (CONFIG_CPU_OSC_FREQ / 128) - 1; break;
        }

        SET_BIT(RCC->APB1ENR, RCC_APB1ENR_PWREN | RCC_APB1ENR_BKPEN);
        SET_BIT(PWR->CR, PWR_CR_DBP);
        CLEAR_BIT(RCC->BDCR, RCC_BDCR_RTCEN);
        CLEAR_BIT(RCC->BDCR, RCC_BDCR_RTCSEL_1 | RCC_BDCR_RTCSEL_0);
        RCC_RTCCLKConfig(_RTCM_CFG__RTCCLK_SRC);
        SET_BIT(RCC->BDCR, RCC_BDCR_RTCEN);

        if (_RTCM_CFG__LSE_ON) {
                RCC_LSEConfig(_RTCM_CFG__LSE_ON);

                uint timer = _sys_time_get_reference();
                while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET) {
                        if (_sys_time_is_expired(timer, TIMEOUT_LSE)) {
                                errno = EIO;
                                return STD_RET_ERROR;
                        }
                }
        }

        _sys_critical_section_begin();
        {
                uint attempts = RTC_WRITE_ATTEMPTS;
                while (!(RTC->CRL & RTC_CRL_RTOFF)) {
                        if (--attempts == 0) {
                                _sys_critical_section_end();
                                errno = EIO;
                                return STD_RET_ERROR;
                        }
                }

                SET_BIT(RTC->CRL, RTC_CRL_CNF);
                WRITE_REG(RTC->PRLH, PRL >> 16);
                WRITE_REG(RTC->PRLL, PRL);
                CLEAR_BIT(RTC->CRL, RTC_CRL_CNF);
                while (!(RTC->CRL & RTC_CRL_RTOFF) && attempts--);

        }
        _sys_critical_section_end();

        return STD_RET_OK;
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
API_MOD_RELEASE(RTCM, void *device_handle)
{
        UNUSED_ARG(device_handle);

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
API_MOD_OPEN(RTCM, void *device_handle, vfs_open_flags_t flags)
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
API_MOD_CLOSE(RTCM, void *device_handle, bool force)
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
API_MOD_WRITE(RTCM, void *device_handle, const u8_t *src, size_t count, fpos_t *fpos, struct vfs_fattr fattr)
{
        UNUSED_ARG(device_handle);
        UNUSED_ARG(fattr);

        if (*fpos == 0 && count <= sizeof(time_t)) {

                time_t t = 0;
                memcpy(&t, src, count);

                _sys_critical_section_begin();
                {
                        uint attempts = RTC_WRITE_ATTEMPTS;
                        while (!(RTC->CRL & RTC_CRL_RTOFF)) {
                                if (--attempts == 0) {
                                        _sys_critical_section_end();
                                        errno = EIO;
                                        return 0;
                                }
                        }

                        SET_BIT(RTC->CRL, RTC_CRL_CNF);
                        WRITE_REG(RTC->CNTH, t >> 16);
                        WRITE_REG(RTC->CNTL, t);
                        CLEAR_BIT(RTC->CRL, RTC_CRL_CNF);
                        while (!(RTC->CRL & RTC_CRL_RTOFF) && attempts--);

                }
                _sys_critical_section_end();

                return count;
        } else {
                return 0;
        }
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
API_MOD_READ(RTCM, void *device_handle, u8_t *dst, size_t count, fpos_t *fpos, struct vfs_fattr fattr)
{
        UNUSED_ARG(device_handle);
        UNUSED_ARG(fattr);

        if (*fpos == 0 && count <= sizeof(time_t)) {
                _sys_critical_section_begin();
                u32_t cnt = (RTC->CNTH << 16) + RTC->CNTL;
                _sys_critical_section_end();

                memcpy(dst, &cnt, count);
                return count;
        } else {
                return 0;
        }
}

//==============================================================================
/**
 * @brief IO control
 *
 * @param[in ]          *device_handle          device allocated memory
 * @param[in ]           request                request
 * @param[in ][out]     *arg                    request's argument
 *
 * @return Value depends on request. To obtain more information see module's
 *         ioctl request definitions.
 */
//==============================================================================
API_MOD_IOCTL(RTCM, void *device_handle, int request, void *arg)
{
        UNUSED_ARG(device_handle);
        UNUSED_ARG(request);
        UNUSED_ARG(arg);

        errno = EBADRQC;

        return STD_RET_ERROR;
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
API_MOD_FLUSH(RTCM, void *device_handle)
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
API_MOD_STAT(RTCM, void *device_handle, struct vfs_dev_stat *device_stat)
{
        UNUSED_ARG(device_handle);

        device_stat->st_size  = sizeof(time_t);
        device_stat->st_major = 0;
        device_stat->st_minor = 0;

        return STD_RET_OK;
}

/*==============================================================================
  End of file
==============================================================================*/
