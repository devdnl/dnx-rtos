/*=========================================================================*//**
@file    wdg.c

@author  Daniel Zorychta

@brief   WDG driver

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
#include "stm32f1/wdg_cfg.h"
#include "stm32f1/wdg_def.h"
#include "stm32f1/stm32f10x.h"

/*==============================================================================
  Local macros
==============================================================================*/
#define KEY_START               0xCCCC
#define KEY_RELOAD              0xAAAA
#define KEY_UNLOCK              0x5555

/*==============================================================================
  Local object types
==============================================================================*/
typedef struct {
        dev_lock_t file_lock;
} WDG_t;

/*==============================================================================
  Local function prototypes
==============================================================================*/
static inline void configure_wdg();
static inline void start_wdg();
static inline void reset_wdg();

/*==============================================================================
  Local objects
==============================================================================*/
MODULE_NAME("WDG");

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
API_MOD_INIT(WDG, void **device_handle, u8_t major, u8_t minor)
{
        if (major == _WDG_MAJOR_NUMBER || minor == _WDG_MINOR_NUMBER) {

                WDG_t *hdl = calloc(1, sizeof(WDG_t));
                if (hdl) {
                        configure_wdg();
                        start_wdg();
                        reset_wdg();

                        *device_handle = hdl;

                        return STD_RET_OK;
                } else {
                        return STD_RET_ERROR;
                }

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
API_MOD_RELEASE(WDG, void *device_handle)
{
        UNUSED_ARG(device_handle);

        errno = EPERM;
        return STD_RET_ERROR;
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
API_MOD_OPEN(WDG, void *device_handle, vfs_open_flags_t flags)
{
        UNUSED_ARG(flags);

        WDG_t *hdl = device_handle;

        if (_WDG_CFG_OPEN_LOCK) {
                return device_lock(&hdl->file_lock) ? STD_RET_OK : STD_RET_ERROR;
        } else {
                return STD_RET_OK;
        }
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
API_MOD_CLOSE(WDG, void *device_handle, bool force)
{
        WDG_t *hdl = device_handle;

        if (_WDG_CFG_OPEN_LOCK) {
                if (device_is_access_granted(&hdl->file_lock) || force) {
                        device_unlock(&hdl->file_lock, force);
                        return STD_RET_OK;
                } else {
                        errno = EBUSY;
                        return STD_RET_ERROR;
                }
        } else {
                return STD_RET_OK;
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
API_MOD_WRITE(WDG, void *device_handle, const u8_t *src, size_t count, u64_t *fpos, struct vfs_fattr fattr)
{
        UNUSED_ARG(device_handle);
        UNUSED_ARG(src);
        UNUSED_ARG(count);
        UNUSED_ARG(fpos);
        UNUSED_ARG(fattr);

        errno = EPERM;
        return -1;
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
API_MOD_READ(WDG, void *device_handle, u8_t *dst, size_t count, u64_t *fpos, struct vfs_fattr fattr)
{
        UNUSED_ARG(device_handle);
        UNUSED_ARG(dst);
        UNUSED_ARG(count);
        UNUSED_ARG(fpos);
        UNUSED_ARG(fattr);

        errno = EPERM;
        return -1;
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
API_MOD_IOCTL(WDG, void *device_handle, int request, void *arg)
{
        UNUSED_ARG(arg);

        WDG_t *hdl = device_handle;

        if (device_is_access_granted(&hdl->file_lock)) {
                switch (request) {
                case IOCTL_WDG__RESET:
                        reset_wdg();
                        return STD_RET_OK;

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
API_MOD_FLUSH(WDG, void *device_handle)
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
API_MOD_STAT(WDG, void *device_handle, struct vfs_dev_stat *device_stat)
{
        UNUSED_ARG(device_handle);

        device_stat->st_major = _WDG_MAJOR_NUMBER;
        device_stat->st_minor = _WDG_MINOR_NUMBER;
        device_stat->st_size  = 0;

        return STD_RET_OK;
}

//==============================================================================
/**
 * @brief Configure Watchdog
 */
//==============================================================================
static inline void configure_wdg()
{
        /* default LSI / 4 */
        u8_t divider;
        switch (_WDG_CFG_DIVIDER) {
        case 8  : divider = IWDG_PR_PR_0; break;
        case 16 : divider = IWDG_PR_PR_1; break;
        case 32 : divider = IWDG_PR_PR_1 | IWDG_PR_PR_0; break;
        case 64 : divider = IWDG_PR_PR_2; break;
        case 128: divider = IWDG_PR_PR_2 | IWDG_PR_PR_0; break;
        case 256: divider = IWDG_PR_PR_2 | IWDG_PR_PR_1; break;
        default : divider = 0; break;
        }

        /* reload value */
        u16_t reload = _WDG_CFG_RELOAD & 0xFFF;

        /* enable IWDG */
        IWDG->KR = KEY_UNLOCK;

        while (IWDG->SR & IWDG_SR_PVU);
        IWDG->PR = divider;

        while (IWDG->SR & IWDG_SR_RVU);
        IWDG->RLR = reload;

        if (_WDG_CFG_DISABLE_ON_DEBUG) {
                SET_BIT(DBGMCU->CR, DBGMCU_CR_DBG_IWDG_STOP);
        }
}

//==============================================================================
/**
 * @brief Start Watchdog
 */
//==============================================================================
static inline void start_wdg()
{
        IWDG->KR = KEY_START;
}

//==============================================================================
/**
 * @brief Reload Watchdog
 */
//==============================================================================
static inline void reset_wdg()
{
        IWDG->KR = KEY_RELOAD;
}

/*==============================================================================
  End of file
==============================================================================*/
