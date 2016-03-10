/*=========================================================================*//**
@file    crc.c

@author  Daniel Zorychta

@brief   CRC driver (CRCM - CRC Module)

@note    Copyright (C) 2014 Daniel Zorychta <daniel.zorychta@gmail.com>

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
#include "stm32f1/crc_cfg.h"
#include "stm32f1/stm32f10x.h"
#include "../crc_ioctl.h"

/*==============================================================================
  Local macros
==============================================================================*/
#undef CRC
#define CRCP    ((CRC_t *) CRC_BASE)

/*==============================================================================
  Local object types
==============================================================================*/
typedef struct CRCM {
        dev_lock_t          file_lock;
        enum CRC_input_mode input_mode;
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
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
API_MOD_INIT(CRC, void **device_handle, u8_t major, u8_t minor)
{
        if (major != 0 || minor != 0) {
                return ENODEV;
        }

        if (RCC->AHBENR & RCC_AHBENR_CRCEN) {
                return EADDRINUSE;
        }

        int result = sys_zalloc(sizeof(CRCM), device_handle);
        if (result == ESUCC) {
                CRCM *hdl = *device_handle;

                SET_BIT(RCC->AHBENR, RCC_AHBENR_CRCEN);
                hdl->input_mode = CRC_INPUT_MODE__32BIT;
        }

        return result;
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

        int status;

        sys_critical_section_begin();
        {
                if (sys_device_is_unlocked(&hdl->file_lock)) {
                        CLEAR_BIT(RCC->AHBENR, RCC_AHBENR_CRCEN);
                        sys_free(cast(void**, &hdl));
                        status = ESUCC;
                } else {
                        status = EBUSY;
                }
        }
        sys_critical_section_end();

        return status;
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

        int result = sys_device_get_access(&hdl->file_lock);

        if (result == ESUCC) {
                result = sys_device_unlock(&hdl->file_lock, force);
        }

        return result;
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

        int result = sys_device_get_access(&hdl->file_lock);
        if (result == ESUCC) {
                reset_CRC();

                size_t n = 0;

                if (hdl->input_mode == CRC_INPUT_MODE__8BIT) {

                        for (n = 0; n < (size_t)count; n++) {
                                CRCP->DR = src[n];
                        }

                } else if (hdl->input_mode == CRC_INPUT_MODE__16BIT) {

                        size_t len = count / sizeof(u16_t);
                        u16_t *ptr = (u16_t *)src;

                        for (size_t i = 0; i < len; i++) {
                                CRCP->DR = *ptr++;
                        }

                        n = len * sizeof(u16_t);

                } else {

                        size_t len = count / sizeof(u32_t);
                        u32_t *ptr = (u32_t *)src;

                        for (size_t i = 0; i < len; i++) {
                                CRCP->DR = *ptr++;
                        }

                        n = len * sizeof(u32_t);
                }

                *wrcnt = n;
        }

        return result;
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

        int result = sys_device_get_access(&hdl->file_lock);
        if (result == ESUCC) {

                u8_t  crc[4] = {CRCP->DR, CRCP->DR >> 8, CRCP->DR >> 16, CRCP->DR >> 24};

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

        return result;
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
        CRCM *hdl = device_handle;

        int result = sys_device_get_access(&hdl->file_lock);
        if (result == ESUCC) {
                switch (request) {
                case IOCTL_CRC__SET_INPUT_MODE:
                        if (arg) {
                                enum CRC_input_mode mode = *(enum CRC_input_mode *)arg;
                                if (mode <= CRC_INPUT_MODE__32BIT) {
                                        hdl->input_mode = mode;
                                        return ESUCC;
                                }
                        }
                        return EINVAL;
                        break;

                case IOCTL_CRC__GET_INPUT_MODE:
                        if (arg) {
                                *(enum CRC_input_mode *)arg = hdl->input_mode;
                                return ESUCC;
                        } else {
                                return EINVAL;
                        }
                        break;

                default:
                        return EBADRQC;
                }
        }

        return result;
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
