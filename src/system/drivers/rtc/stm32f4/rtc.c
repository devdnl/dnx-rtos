/*==============================================================================
File    rtc.c

Author  Daniel Zorychta

Brief   Real-Time Clock Module

        Copyright (C) 2018 Daniel Zorychta <daniel.zorychta@gmail.com>

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

==============================================================================*/

/*==============================================================================
  Include files
==============================================================================*/
#include "drivers/driver.h"
#include "stm32f4/rtc_cfg.h"
#include "stm32f4/stm32f4xx.h"
#include "stm32f4/lib/stm32f4xx_rcc.h"
#include "../rtc_ioctl.h"

/*==============================================================================
  Local macros
==============================================================================*/
#undef RTC
#define RTCP                    ((RTC_TypeDef *) RTC_BASE)

#define RTC_WRITE_ATTEMPTS      65536
#define TIMEOUT_LSE             5000
#define HSE_RTCDIV              ((((__CLK_RTC_CLK_SRC__) >> 16) >= 2) ? ((__CLK_RTC_CLK_SRC__) >> 16) : 2)

/*==============================================================================
  Local object types
==============================================================================*/
typedef struct {
        u32_t apre;
        u32_t spre;
} pre_t;

/*==============================================================================
  Local function prototypes
==============================================================================*/
static void unlock_RTC(void);
static void lock_RTC(void);
static uint8_t BCD_to_byte(uint8_t value);
static uint8_t byte_to_BCD(uint8_t value);

/*==============================================================================
  Local objects
==============================================================================*/
MODULE_NAME(RTC);

static const pre_t PRE[] = {
        {.apre = 127, .spre = 255},     // no clock
        {.apre = 127, .spre = 255},     // 32768 LSE
        {.apre = 127, .spre = 249},     // 32000 LSI
        {.apre = 127, .spre = __CPU_OSC_FREQ__ / 128 / HSE_RTCDIV} // HSE
};

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
 * @param[in ]            config               optional module configuration
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
API_MOD_INIT(RTC, void **device_handle, u8_t major, u8_t minor, const void *config)
{
        UNUSED_ARG3(device_handle, config, _module_name_);

        if ((major != 0) or (minor != 0)) {
                return ENODEV;
        }

        int err = ESUCC;

        SET_BIT(RCC->APB1ENR, RCC_APB1ENR_PWREN);
        SET_BIT(PWR->CR, PWR_CR_DBP);

        if (!(RCC->BDCR & RCC_BDCR_RTCEN)) {

                SET_BIT(RCC->BDCR, RCC_BDCR_RTCEN);

                sys_critical_section_begin();
                {
                        unlock_RTC();

                        SET_BIT(RTCP->ISR, RTC_ISR_INIT);

                        uint attempts = RTC_WRITE_ATTEMPTS;
                        while (not (RTCP->ISR & RTC_ISR_INITF)) {
                                if (--attempts == 0) {
                                        err = EIO;
                                        goto finish;
                                }
                        }

                        RTCP->CR = 0;

                        int osc = (RCC->BDCR & (RCC_BDCR_RTCSEL_1 | RCC_BDCR_RTCSEL_0))
                                  >> RCC_BDCR_RTCSEL_Pos;

                        // NOTE: separate write access needed
                        RTCP->PRER  = PRE[osc].spre;
                        RTCP->PRER |= (u32_t)PRE[osc].apre << RTC_PRER_PREDIV_A_Pos;

                        RTCP->TR = 0;
                        RTCP->DR = 0x002101;

                        CLEAR_BIT(RTCP->ISR, RTC_ISR_INIT);

                        finish:
                        lock_RTC();
                }
                sys_critical_section_end();
        } else {
                printk("RTC already initialized");
        }

        return err;
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

        int err = ESUCC;

        count = count > sizeof(time_t) ? sizeof(time_t) : count;

        time_t timer = 0;
        memcpy(&timer, src, count);

        sys_critical_section_begin();
        {
                unlock_RTC();

                SET_BIT(RTCP->ISR, RTC_ISR_INIT);

                uint attempts = RTC_WRITE_ATTEMPTS;
                while (not (RTCP->ISR & RTC_ISR_INITF)) {
                        if (--attempts == 0) {
                                err = EIO;
                                goto finish;
                        }
                }

                struct tm tm;
                sys_gmtime_r(&timer, &tm);

                u32_t TR = 0;
                TR |= ((byte_to_BCD(tm.tm_sec) & 0x7F) << 0);
                TR |= ((byte_to_BCD(tm.tm_min) & 0x7F) << 8);
                TR |= ((byte_to_BCD(tm.tm_hour) & 0x3F) << 16);

                u32_t DR = 0;
                DR |= ((byte_to_BCD(tm.tm_mday) & 0x3F) << 0);
                DR |= ((byte_to_BCD(tm.tm_mon + 1) & 0x1F) << 8);
                DR |= ((byte_to_BCD(tm.tm_wday + 1) & 0x07) << 13);
                DR |= ((byte_to_BCD(tm.tm_year - 100) & 0xFF) << 16);

                CLEAR_BIT(RTCP->CR, RTC_CR_FMT);
                RTCP->TR = TR;
                RTCP->DR = DR;

                CLEAR_BIT(RTCP->ISR, RTC_ISR_INIT);

                finish:
                lock_RTC();
        }
        sys_critical_section_end();

        *wrcnt = count;
        *fpos  = 0;

        return err;
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

        time_t timer = 0;

        sys_critical_section_begin();
        {
                // read time & date registers (according to reference manual)
                u32_t TR1 = RTCP->TR;
                u32_t DR1 = RTCP->DR;

                u32_t TR2 = RTCP->TR;
                u32_t DR2 = RTCP->DR;

                u32_t TR = (TR1 == TR2) ? TR1 : RTCP->TR;
                u32_t DR = (DR1 == DR2) ? DR1 : RTCP->DR;

                // convert BCD time to UNIX time stamp
                struct tm tm;
                tm.tm_isutc = true;
                tm.tm_sec   = BCD_to_byte((TR >>  0) & 0x7F);
                tm.tm_min   = BCD_to_byte((TR >>  8) & 0x7F);
                tm.tm_hour  = BCD_to_byte((TR >> 16) & 0x3F);
                tm.tm_mday  = BCD_to_byte((DR >>  0) & 0x3F);
                tm.tm_mon   = BCD_to_byte((DR >>  8) & 0x1F) - 1;
                tm.tm_year  = BCD_to_byte((DR >> 16) & 0xFF) + 100;

                timer = sys_mktime(&tm);
        }
        sys_critical_section_end();

        memcpy(dst, &timer, count);

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

        if (arg == NULL) {
                return EINVAL;
        }

        int err = EIO;

        switch (request) {
        case IOCTL_RTC__SET_ALARM: {
                sys_critical_section_begin();
                {
                        struct tm tm;
                        sys_gmtime_r(arg, &tm);

                        u32_t ALRMAR = 0;

                        ALRMAR |= ((byte_to_BCD(tm.tm_sec) & 0x7F) << 0);
                        ALRMAR |= ((byte_to_BCD(tm.tm_min) & 0x7F) << 8);
                        ALRMAR |= ((byte_to_BCD(tm.tm_hour) & 0x3F) << 16);
                        ALRMAR |= RTC_ALRMAR_MSK4;

                        unlock_RTC();

                        CLEAR_BIT(RTCP->CR, RTC_CR_ALRAE | RTC_CR_ALRAIE
                                          | RTC_CR_ALRBE | RTC_CR_ALRBIE);

                        RTCP->ISR = RTC_ISR_ALRAF | RTC_ISR_ALRBF;

                        uint attempts = RTC_WRITE_ATTEMPTS;
                        while (not (RTCP->ISR & RTC_ISR_ALRAWF)) {
                                if (--attempts == 0) {
                                        err = EIO;
                                        goto finish;
                                }
                        }

                        RTCP->ALRMASSR = 0;
                        RTCP->ALRMAR   = ALRMAR;

                        SET_BIT(RTCP->CR, RTC_CR_ALRAE | RTC_CR_ALRAIE);

                        err = ESUCC;

                        finish:
                        lock_RTC();
                }
                sys_critical_section_end();
                break;
        }

        default:
                err = EBADRQC;
                break;
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

        device_stat->st_size = sizeof(time_t);

        return ESUCC;
}

//==============================================================================
/**
 * @brief  Unlock RTC registers.
 */
//==============================================================================
static void unlock_RTC(void)
{
        RTCP->WPR = 0xCA;
        RTCP->WPR = 0x53;
}

//==============================================================================
/**
 * @brief  Lock RTC registers.
 */
//==============================================================================
static void lock_RTC(void)
{
        RTCP->WPR = 0xFF;
}

//==============================================================================
/**
 * @brief  Convert BCD to byte.
 *
 * @param  value        BCD value
 *
 * @return Byte value.
 */
//==============================================================================
static uint8_t BCD_to_byte(uint8_t value)
{
        return (((value & 0xF0) >> 0x4) * 10) + ((value & 0x0F));
}

//==============================================================================
/**
 * @brief  Convert byte to BCD.
 *
 * @param  value        byte value
 *
 * @return BCD value.
 */
//==============================================================================
static uint8_t byte_to_BCD(uint8_t value)
{
        return (value % 10) + ((value / 10) << 4);
}

/*==============================================================================
  End of file
==============================================================================*/
