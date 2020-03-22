/*=========================================================================*//**
@file    rtc.c

@author  Daniel Zorychta

@brief   Real-Time Clock Module

@note    Copyright (C) 2015 Daniel Zorychta <daniel.zorychta@gmail.com>

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
#include "stm32f1/rtc_cfg.h"
#include "stm32f1/stm32f10x.h"
#include "stm32f1/lib/stm32f10x_rcc.h"
#include "../rtc_ioctl.h"

/*==============================================================================
  Local macros
==============================================================================*/
#undef RTC
#define RTCP                    ((RTC_t *) RTC_BASE)

#define RTC_WRITE_ATTEMPTS      10000
#define TIMEOUT_LSE             5000

/*==============================================================================
  Local object types
==============================================================================*/
static const u32_t PRE[] = {1, 32767, 39999, (__CPU_OSC_FREQ__ / 128) - 1};

/*==============================================================================
  Local function prototypes
==============================================================================*/

/*==============================================================================
  Local objects
==============================================================================*/
MODULE_NAME(RTC);

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
 * @return One of errno value (errno.h)
 */
//==============================================================================
API_MOD_INIT(RTC, void **device_handle, u8_t major, u8_t minor)
{
        UNUSED_ARG1(device_handle);
        UNUSED_ARG1(major);
        UNUSED_ARG1(minor);
        UNUSED_ARG1(_module_name_);

        SET_BIT(RCC->APB1ENR, RCC_APB1ENR_PWREN | RCC_APB1ENR_BKPEN);
        SET_BIT(PWR->CR, PWR_CR_DBP);

        if (!(RCC->BDCR & RCC_BDCR_RTCEN)) {

                sys_critical_section_begin();
                {
                        uint attempts = RTC_WRITE_ATTEMPTS;
                        while (!(RTCP->CRL & RTC_CRL_RTOFF)) {
                                if (--attempts == 0) {
                                        sys_critical_section_end();
                                        return EIO;
                                }
                        }

                        SET_BIT(RTCP->CRL, RTC_CRL_CNF);

                        int osc = (RCC->BDCR & (RCC_BDCR_RTCSEL_1 | RCC_BDCR_RTCSEL_0)) >> 8;

                        WRITE_REG(RTCP->PRLH, PRE[osc] >> 16);
                        WRITE_REG(RTCP->PRLL, PRE[osc]);

                        CLEAR_BIT(RTCP->CRL, RTC_CRL_CNF);

                        while (!(RTCP->CRL & RTC_CRL_RTOFF) && attempts--);
                }

                SET_BIT(RCC->BDCR, RCC_BDCR_RTCEN);

                sys_critical_section_end();
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
API_MOD_RELEASE(RTC, void *device_handle)
{
        UNUSED_ARG1(device_handle);

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
API_MOD_OPEN(RTC, void *device_handle, u32_t flags)
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
API_MOD_CLOSE(RTC, void *device_handle, bool force)
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
API_MOD_WRITE(RTC,
              void             *device_handle,
              const u8_t       *src,
              size_t            count,
              fpos_t           *fpos,
              size_t           *wrcnt,
              struct vfs_fattr  fattr)
{
        UNUSED_ARG2(device_handle, fattr);

        count = count > sizeof(time_t) ? sizeof(time_t) : count;

        time_t t = 0;
        memcpy(&t, src, count);

        sys_critical_section_begin();
        {
                uint attempts = RTC_WRITE_ATTEMPTS;
                while (!(RTCP->CRL & RTC_CRL_RTOFF)) {
                        if (--attempts == 0) {
                                sys_critical_section_end();
                                return EIO;
                        }
                }

                SET_BIT(RTCP->CRL, RTC_CRL_CNF);
                WRITE_REG(RTCP->CNTH, t >> 16);
                WRITE_REG(RTCP->CNTL, t);
                CLEAR_BIT(RTCP->CRL, RTC_CRL_CNF);
                while (!(RTCP->CRL & RTC_CRL_RTOFF) && attempts--);
        }
        sys_critical_section_end();

        *wrcnt = count;
        *fpos  = 0;

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
API_MOD_READ(RTC,
             void            *device_handle,
             u8_t            *dst,
             size_t           count,
             fpos_t          *fpos,
             size_t          *rdcnt,
             struct vfs_fattr fattr)
{
        UNUSED_ARG2(device_handle, fattr);

        count = count > sizeof(time_t) ? sizeof(time_t) : count;

        sys_critical_section_begin();
        u32_t cnt = (RTCP->CNTH << 16) + RTCP->CNTL;
        sys_critical_section_end();

        memcpy(dst, &cnt, count);

        *rdcnt = count;
        *fpos  = 0;

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
 * @return One of errno value (errno.h)
 */
//==============================================================================
API_MOD_IOCTL(RTC, void *device_handle, int request, void *arg)
{
        UNUSED_ARG1(device_handle);

        int err = EINVAL;

        if (arg) {
                err = EBADRQC;

                switch (request) {
                case IOCTL_RTC__SET_ALARM: {
                        sys_critical_section_begin();
                        {
                                u16_t attempts = RTC_WRITE_ATTEMPTS;
                                while (!(RTCP->CRL & RTC_CRL_RTOFF)) {
                                        if (--attempts == 0) {
                                                sys_critical_section_end();
                                                err = EIO;
                                        }
                                }

                                if (attempts) {
                                        time_t *time = arg;

                                        SET_BIT(RTCP->CRL, RTC_CRL_CNF);
                                        WRITE_REG(RTCP->ALRH, (*time) >> 16);
                                        WRITE_REG(RTCP->ALRL, (*time));
                                        CLEAR_BIT(RTCP->CRL, RTC_CRL_CNF);
                                        while (!(RTCP->CRL & RTC_CRL_RTOFF) && attempts--);
                                        err = ESUCC;
                                }
                        }
                        sys_critical_section_end();
                        break;
                }

                default:
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
API_MOD_FLUSH(RTC, void *device_handle)
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
API_MOD_STAT(RTC, void *device_handle, struct vfs_dev_stat *device_stat)
{
        UNUSED_ARG1(device_handle);

        device_stat->st_size  = sizeof(time_t);
        device_stat->st_major = 0;
        device_stat->st_minor = 0;

        return ESUCC;
}

//==============================================================================
/**
 * @brief RTC Alarm IRQ
 */
//==============================================================================
void RTCAlarm_IRQHandler(void)
{
}

/*==============================================================================
  End of file
==============================================================================*/
