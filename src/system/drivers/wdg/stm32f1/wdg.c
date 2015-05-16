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
#include "drivers/driver.h"
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
MODULE_NAME(WDG);

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
API_MOD_INIT(WDG, void **device_handle, u8_t major, u8_t minor)
{
        if (major == _WDG_MAJOR_NUMBER && minor == _WDG_MINOR_NUMBER) {

                int result = _sys_zalloc(sizeof(WDG_t), device_handle);
                if (result == ESUCC) {
                        configure_wdg();
                        start_wdg();
                        reset_wdg();
                }

                return result;

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
API_MOD_RELEASE(WDG, void *device_handle)
{
        UNUSED_ARG1(device_handle);

        return EPERM;
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
API_MOD_OPEN(WDG, void *device_handle, u32_t flags)
{
        UNUSED_ARG1(flags);

        WDG_t *hdl = device_handle;

        if (_WDG_CFG_OPEN_LOCK) {
                return _sys_device_lock(&hdl->file_lock);
        } else {
                return ESUCC;
        }
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
API_MOD_CLOSE(WDG, void *device_handle, bool force)
{
        WDG_t *hdl = device_handle;

        if (_WDG_CFG_OPEN_LOCK) {
                return _sys_device_unlock(&hdl->file_lock, force);
        } else {
                return ESUCC;
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
 * @param[out]          *wrcnt                  number of written bytes
 * @param[in ]           fattr                  file attributes
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
API_MOD_WRITE(WDG,
              void             *device_handle,
              const u8_t       *src,
              size_t            count,
              fpos_t           *fpos,
              size_t           *wrcnt,
              struct vfs_fattr  fattr)
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
API_MOD_READ(WDG,
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
API_MOD_IOCTL(WDG, void *device_handle, int request, void *arg)
{
        UNUSED_ARG1(arg);

        WDG_t *hdl = device_handle;

        int result = _sys_device_access(&hdl->file_lock);
        if (result == ESUCC) {
                switch (request) {
                case IOCTL_WDG__RESET:
                        reset_wdg();
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
API_MOD_FLUSH(WDG, void *device_handle)
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
API_MOD_STAT(WDG, void *device_handle, struct vfs_dev_stat *device_stat)
{
        UNUSED_ARG1(device_handle);

        device_stat->st_major = _WDG_MAJOR_NUMBER;
        device_stat->st_minor = _WDG_MINOR_NUMBER;
        device_stat->st_size  = 0;

        return ESUCC;
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
