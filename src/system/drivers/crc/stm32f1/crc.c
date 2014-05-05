/*=========================================================================*//**
@file    crc.c

@author  Daniel Zorychta

@brief   CRC driver (CRCCU - CRC Calculation Unit)

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
#include "core/module.h"
#include <dnx/thread.h>
#include <dnx/misc.h>
#include "stm32f1/crc_cfg.h"
#include "stm32f1/crc_def.h"
#include "stm32f1/stm32f10x.h"

/*==============================================================================
  Local macros
==============================================================================*/

/*==============================================================================
  Local object types
==============================================================================*/
typedef struct CRCCU
{
        dev_lock_t              file_lock;
        enum CRC_input_mode     input_mode;
} CRCCU;

/*==============================================================================
  Local function prototypes
==============================================================================*/
static inline void reset_CRC();

/*==============================================================================
  Local objects
==============================================================================*/
MODULE_NAME("CRCCU");

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
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
API_MOD_INIT(CRCCU, void **device_handle, u8_t major, u8_t minor)
{
        if (  major != _CRC_MAJOR_NUMBER
           || minor != _CRC_MINOR_NUMBER
           || (RCC->AHBENR & RCC_AHBENR_CRCEN) ) {

                return STD_RET_ERROR;
        }

        CRCCU *hdl = calloc(1, sizeof(CRCCU));
        if (hdl) {
                SET_BIT(RCC->AHBENR, RCC_AHBENR_CRCEN);

                hdl->input_mode = CRC_INPUT_MODE_WORD;
                *device_handle  = hdl;

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
API_MOD_RELEASE(CRCCU, void *device_handle)
{
        CRCCU *hdl = device_handle;

        critical_section_begin();

        stdret_t status = STD_RET_ERROR;

        if (device_is_unlocked(hdl->file_lock)) {
                CLEAR_BIT(RCC->AHBENR, RCC_AHBENR_CRCEN);
                free(hdl);
                status = STD_RET_OK;
        } else {
                errno = EBUSY;
        }

        critical_section_end();

        return status;
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
API_MOD_OPEN(CRCCU, void *device_handle, vfs_open_flags_t flags)
{
        UNUSED_ARG(flags);

        CRCCU *hdl = device_handle;

        return device_lock(&hdl->file_lock) ? STD_RET_OK : STD_RET_ERROR;
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
API_MOD_CLOSE(CRCCU, void *device_handle, bool force)
{
        CRCCU *hdl = device_handle;

        if (device_is_access_granted(&hdl->file_lock) || force) {
                device_unlock(&hdl->file_lock, force);
                return STD_RET_OK;
        } else {
                errno = EBUSY;
                return STD_RET_ERROR;
        }
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
API_MOD_WRITE(CRCCU, void *device_handle, const u8_t *src, size_t count, fpos_t *fpos, struct vfs_fattr fattr)
{
        UNUSED_ARG(fpos);
        UNUSED_ARG(fattr);

        CRCCU *hdl = device_handle;

        ssize_t n = -1;

        if (device_is_access_granted(&hdl->file_lock)) {
                reset_CRC();

                if (hdl->input_mode == CRC_INPUT_MODE_BYTE) {

                        for (n = 0; n < (ssize_t)count; n++) {
                                CRC->DR = src[n];
                        }

                } else if (hdl->input_mode == CRC_INPUT_MODE_HALF_WORD) {

                        size_t len = count / sizeof(u16_t);
                        u16_t *ptr = (u16_t *)src;

                        for (size_t i = 0; i < len; i++) {
                                CRC->DR = *ptr++;
                        }

                        n = len * sizeof(u16_t);

                } else {

                        size_t len = count / sizeof(u32_t);
                        u32_t *ptr = (u32_t *)src;

                        for (size_t i = 0; i < len; i++) {
                                CRC->DR = *ptr++;
                        }

                        n = len * sizeof(u32_t);
                }
        } else {
                errno = EACCES;
        }

        return n;
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
API_MOD_READ(CRCCU, void *device_handle, u8_t *dst, size_t count, fpos_t *fpos, struct vfs_fattr fattr)
{
        UNUSED_ARG(fpos);
        UNUSED_ARG(fattr);

        CRCCU *hdl = device_handle;

        ssize_t n = -1;

        if (device_is_access_granted(&hdl->file_lock)) {

                u8_t  crc[4] = {CRC->DR, CRC->DR >> 8, CRC->DR >> 16, CRC->DR >> 24};

                n = 0;
                size_t pos = *fpos;
                for (size_t i = 0; i < count && sizeof(u32_t); i++) {
                        if (pos + n < sizeof(u32_t)) {
                                *(dst++) = crc[n + pos];
                                n++;
                        } else {
                                break;
                        }
                }
        } else {
                errno = EACCES;
        }

        return n;
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
API_MOD_IOCTL(CRCCU, void *device_handle, int request, void *arg)
{
        CRCCU *hdl = device_handle;

        if (device_is_access_granted(&hdl->file_lock)) {
                switch (request) {
                case IOCTL_CRC__SET_INPUT_MODE:
                        if (arg) {
                                enum CRC_input_mode mode = *(enum CRC_input_mode *)arg;
                                if (mode <= CRC_INPUT_MODE_WORD) {
                                        hdl->input_mode = mode;
                                        return STD_RET_OK;
                                }
                        }
                        errno = EINVAL;
                        break;

                case IOCTL_CRC__GET_INPUT_MODE:
                        if (arg) {
                                *(enum CRC_input_mode *)arg = hdl->input_mode;
                                return STD_RET_OK;
                        } else {
                                errno = EINVAL;
                        }
                        break;

                default:
                        errno = EBADRQC;
                        break;
                }
        } else {
                errno = EACCES;
        }

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
API_MOD_FLUSH(CRCCU, void *device_handle)
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
API_MOD_STAT(CRCCU, void *device_handle, struct vfs_dev_stat *device_stat)
{
        UNUSED_ARG(device_handle);

        device_stat->st_major = _CRC_MAJOR_NUMBER;
        device_stat->st_minor = _CRC_MINOR_NUMBER;
        device_stat->st_size  = 4;

        return STD_RET_OK;
}

//==============================================================================
/**
 * @brief Reset CRC value register
 */
//==============================================================================
static inline void reset_CRC()
{
        CRC->CR = CRC_CR_RESET;
}

/*==============================================================================
  End of file
==============================================================================*/
