#ifndef _DNXMODULE_H_
#define _DNXMODULE_H_
/*=========================================================================*//**
@file    dnxmodule.h

@author  Daniel Zorychta

@brief   This function provide all required function needed to write modules.

@note    Copyright (C) 2012 Daniel Zorychta <daniel.zorychta@gmail.com>

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

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Include files
==============================================================================*/
#include "core/systypes.h"
#include "core/vfs.h"
#include "core/sysmoni.h"
#include "user/regdrv.h"
#include "kernel/kwrapper.h"

/*==============================================================================
  Exported symbolic constants/macros
==============================================================================*/
#ifdef _DNX_H_
#error "dnx.h and dnxmodule.h shall never included together!"
#endif

#undef  calloc
#define calloc(size_t__nmemb, size_t__msize)    sysm_modcalloc(size_t__nmemb, size_t__msize, regdrv_get_module_number(__module_name__))

#undef  malloc
#define malloc(size_t__size)                    sysm_modmalloc(size_t__size, regdrv_get_module_number(__module_name__))

#undef  free
#define free(void__pmem)                        sysm_modfree(void__pmem, regdrv_get_module_number(__module_name__))

#define STOP_IF(condition)                      _stop_if(condition)

//==============================================================================
/**
 * @brief Initialize device
 *
 * @param[out] **device_handle          memory region allocated by module
 * @param[in]    major                  device major number
 * @param[in]    minor                  device minor number
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
#define _MODULE__DEVICE_INIT(modname)           stdret_t _##modname##_init(void **device_handle, u8_t major, u8_t minor)
#define MODULE__DEVICE_INIT(modname)            const char *__module_name__ = #modname; _MODULE__DEVICE_INIT(modname)

//==============================================================================
/**
 * @brief Release device
 *
 * @param[in] *device_handle           memory region allocated by module
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
#define MODULE__DEVICE_RELEASE(modname)         stdret_t _##modname##_release(void *device_handle)

//==============================================================================
/**
 * @brief Open device
 *
 * @param[in] *device_handle           memory region allocated by module
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
#define MODULE__DEVICE_OPEN(modname)            stdret_t _##modname##_open(void *device_handle)

//==============================================================================
/**
 * @brief Close device
 *
 * @param[in] *device_handle           memory region allocated by module
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
#define MODULE__DEVICE_CLOSE(modname)           stdret_t _##modname##_close(void *device_handle)

//==============================================================================
/**
 * @brief Write data into a device
 *
 * @param[in] *device_handle            memory region allocated by module
 * @param[in] *src                      data source
 * @param[in]  item_size                size of item
 * @param[in]  n_items                  number of items
 * @param[in]  lseek                    position in file/device address
 *
 * @return number of written items
 */
//==============================================================================
#define MODULE__DEVICE_WRITE(modname)           size_t _##modname##_write(void *device_handle, const void *src, size_t item_size, size_t n_items, u64_t lseek)

//==============================================================================
/**
 * @brief Read data from a device
 *
 * @param[in]  *device_handle           memory region allocated by module
 * @param[out] *dst                     data destination
 * @param[in]   item_size               size of item
 * @param[in]   n_items                 number of items
 * @param[in]   lseek                   position in file/device address
 *
 * @return number of written items
 */
//==============================================================================
#define MODULE__DEVICE_READ(modname)            size_t _##modname##_read(void *device_handle, void *dst, size_t item_size, size_t n_items, u64_t lseek)

//==============================================================================
/**
 * @brief Device control
 *
 * @param[in]    *device_handle         memory region allocated by module
 * @param[in]     iorq                  control request
 * @param[in,out] args                  additional arguments
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
#define MODULE__DEVICE_IOCTL(modname)           stdret_t _##modname##_ioctl(void *device_handle, int iorq, va_list args)

//==============================================================================
/**
 * @brief Flush device memory/cache
 *
 * @param[in] *device_handle            memory region allocated by module
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
#define MODULE__DEVICE_FLUSH(modname)           stdret_t _##modname##_flush(void *device_handle)

//==============================================================================
/**
 * @brief Device information
 *
 * @param[in]  *device_handle           memory region allocated by module
 * @param[out] *device_info             device/file info
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
#define MODULE__DEVICE_INFO(modname)            stdret_t _##modname##_info(void *device_handle, struct vfs_dev_info *device_info)

/* module's external interface */
#define DRIVER_INTERFACE(modname)        \
extern _MODULE__DEVICE_INIT(modname);    \
extern  MODULE__DEVICE_RELEASE(modname); \
extern  MODULE__DEVICE_OPEN(modname);    \
extern  MODULE__DEVICE_CLOSE(modname);   \
extern  MODULE__DEVICE_WRITE(modname);   \
extern  MODULE__DEVICE_READ(modname);    \
extern  MODULE__DEVICE_IOCTL(modname);   \
extern  MODULE__DEVICE_FLUSH(modname);   \
extern  MODULE__DEVICE_INFO(modname)

/*==============================================================================
  Exported types, enums definitions
==============================================================================*/

/*==============================================================================
  Exported object declarations
==============================================================================*/

/*==============================================================================
  Exported function prototypes
==============================================================================*/

#ifdef __cplusplus
}
#endif

#endif /* _DNXMODULE_H_ */
/*==============================================================================
  End of file
==============================================================================*/
