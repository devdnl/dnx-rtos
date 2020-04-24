/*=========================================================================*//**
@file    crc.c

@author  Daniel Zorychta

@brief   CRC driver (CRCM - CRC Module)

@note    Copyright (C) 2014 Daniel Zorychta <daniel.zorychta@gmail.com>

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
#include "../crc_ioctl.h"
#include "../stm32fx/crc_cfg.h"

#if defined(ARCH_stm32f1)
#include "stm32f1/stm32f10x.h"
#elif defined(ARCH_stm32f4)
#include "stm32f4/stm32f4xx.h"
#endif

/*==============================================================================
  Local macros
==============================================================================*/
#undef CRC

#if defined(ARCH_stm32f1)
#define CRCP                    ((CRC_TypeDef *) CRC_BASE)
#define AHBxENR                 AHBENR
#define RCC_AHBxENR_CRCEN       RCC_AHBENR_CRCEN
#elif defined(ARCH_stm32f4)
#define CRCP                     ((CRC_TypeDef *) CRC_BASE)
#define AHBxENR                 AHB1ENR
#define RCC_AHBxENR_CRCEN       RCC_AHB1ENR_CRCEN
#endif

/*==============================================================================
  Local object types
==============================================================================*/
typedef struct CRCM {
        dev_lock_t          file_lock;
} CRCM;

/*==============================================================================
  Local function prototypes
==============================================================================*/
static inline void reset_CRC();

/*==============================================================================
  Local objects
==============================================================================*/
MODULE_NAME(CRC);

/*==============================================================================
  Exported objects
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
API_MOD_INIT(CRC, void **device_handle, u8_t major, u8_t minor, const void *config)
{
        UNUSED_ARG1(config);

        if (major != 0 || minor != 0) {
                return ENODEV;
        }

        if (RCC->AHBxENR & RCC_AHBxENR_CRCEN) {
                return EADDRINUSE;
        }

        int err = sys_zalloc(sizeof(CRCM), device_handle);
        if (err == ESUCC) {
                SET_BIT(RCC->AHBxENR, RCC_AHBxENR_CRCEN);
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
API_MOD_RELEASE(CRC, void *device_handle)
{
        CRCM *hdl = device_handle;

        int err = sys_device_lock(&hdl->file_lock);
        if (!err) {
                CLEAR_BIT(RCC->AHBxENR, RCC_AHBxENR_CRCEN);
                sys_free(cast(void**, &hdl));
        }

        return err;
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
API_MOD_OPEN(CRC, void *device_handle, u32_t flags)
{
        UNUSED_ARG1(flags);

        CRCM *hdl = device_handle;

        return sys_device_lock(&hdl->file_lock);
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
API_MOD_CLOSE(CRC, void *device_handle, bool force)
{
        CRCM *hdl = device_handle;

        int err = sys_device_get_access(&hdl->file_lock);

        if (err == ESUCC) {
                err = sys_device_unlock(&hdl->file_lock, force);
        }

        return err;
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
API_MOD_WRITE(CRC,
              void             *device_handle,
              const u8_t       *src,
              size_t            count,
              fpos_t           *fpos,
              size_t           *wrcnt,
              struct vfs_fattr  fattr)
{
        UNUSED_ARG2(fpos, fattr);

        CRCM *hdl = device_handle;

        int err = sys_device_get_access(&hdl->file_lock);
        if (err == ESUCC) {
                reset_CRC();

                size_t len = count / sizeof(u32_t);
                u32_t *ptr = cast(u32_t*, src);

                for (size_t i = 0; i < len; i++) {
                        CRCP->DR = *ptr++;
                }

                *wrcnt = len * sizeof(u32_t);
        }

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
API_MOD_READ(CRC,
             void            *device_handle,
             u8_t            *dst,
             size_t           count,
             fpos_t          *fpos,
             size_t          *rdcnt,
             struct vfs_fattr fattr)
{
        UNUSED_ARG2(fpos, fattr);

        CRCM *hdl = device_handle;

        int err = sys_device_get_access(&hdl->file_lock);
        if (err == ESUCC) {

                u8_t crc[4] = {CRCP->DR, CRCP->DR >> 8, CRCP->DR >> 16, CRCP->DR >> 24};

                size_t n   = 0;
                size_t pos = *fpos;
                for (size_t i = 0; i < count && sizeof(u32_t); i++) {
                        if (pos + n < sizeof(u32_t)) {
                                *(dst++) = crc[n + pos];
                                n++;
                        } else {
                                break;
                        }
                }

                *rdcnt = n;
        }

        return err;
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
API_MOD_IOCTL(CRC, void *device_handle, int request, void *arg)
{
        UNUSED_ARG1(arg);

        CRCM *hdl = device_handle;

        int err = sys_device_get_access(&hdl->file_lock);
        if (err == ESUCC) {
                switch (request) {
                case IOCTL_CRC__SET_INITIAL_VALUE:
                        err = ENOTSUP;
                        break;

                case IOCTL_CRC__SET_POLYNOMIAL:
                        err = ENOTSUP;
                        break;

                default:
                        return EBADRQC;
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
API_MOD_FLUSH(CRC, void *device_handle)
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
API_MOD_STAT(CRC, void *device_handle, struct vfs_dev_stat *device_stat)
{
        UNUSED_ARG1(device_handle);

        device_stat->st_major = 0;
        device_stat->st_minor = 0;
        device_stat->st_size  = 4;

        return ESUCC;
}

//==============================================================================
/**
 * @brief Reset CRC value register
 */
//==============================================================================
static inline void reset_CRC()
{
        CRCP->CR = CRC_CR_RESET;
}

/*==============================================================================
  End of file
==============================================================================*/
