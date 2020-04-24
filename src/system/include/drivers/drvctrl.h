/*=========================================================================*//**
@file    drvctrl.h

@author  Daniel Zorychta

@brief   Modules support.

@note    Copyright (C) 2013 Daniel Zorychta <daniel.zorychta@gmail.com>

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

#ifndef _DRVCTRL_H_
#define _DRVCTRL_H_

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Include files
==============================================================================*/
#include <stdbool.h>
#include "fs/vfs.h"

/*==============================================================================
  Exported macros
==============================================================================*/
#define _MODULE_NAME(module_name)               #module_name

#define _MODULE_INTERFACE(_modname)\
{.name = #_modname,\
 .IF   = {.drv_init    = _##_modname##_init,\
          .drv_release = _##_modname##_release,\
          .drv_open    = _##_modname##_open,\
          .drv_close   = _##_modname##_close,\
          .drv_write   = _##_modname##_write,\
          .drv_read    = _##_modname##_read,\
          .drv_ioctl   = _##_modname##_ioctl,\
          .drv_stat    = _##_modname##_stat,\
          .drv_flush   = _##_modname##_flush}}

#define _IMPORT_MODULE_INTERFACE(_modname)\
extern API_MOD_INIT(_modname, void**, u8_t, u8_t, const void *config);\
extern API_MOD_RELEASE(_modname, void*);\
extern API_MOD_OPEN(_modname, void*, u32_t);\
extern API_MOD_CLOSE(_modname, void*, bool);\
extern API_MOD_WRITE(_modname, void*, const u8_t*, size_t, fpos_t*, size_t*, struct vfs_fattr);\
extern API_MOD_READ(_modname, void*, u8_t*, size_t, fpos_t*, size_t*,  struct vfs_fattr);\
extern API_MOD_IOCTL(_modname, void*, int, void*);\
extern API_MOD_FLUSH(_modname, void*);\
extern API_MOD_STAT(_modname, void*, struct vfs_dev_stat*)

/*==============================================================================
  Exported object types
==============================================================================*/
struct _module_if {
        int (*drv_init   )(void **drvhdl, u8_t major, u8_t minor, const void *config);
        int (*drv_release)(void *drvhdl);
        int (*drv_open   )(void *drvhdl, u32_t flags);
        int (*drv_close  )(void *drvhdl, bool force);
        int (*drv_write  )(void *drvhdl, const u8_t *src, size_t count, fpos_t *fpos, size_t *wrcnt, struct vfs_fattr attr);
        int (*drv_read   )(void *drvhdl, u8_t *dst, size_t count, fpos_t *fpos, size_t *rdcnt, struct vfs_fattr attr);
        int (*drv_ioctl  )(void *drvhdl, int iorq, void *arg);
        int (*drv_flush  )(void *drvhdl);
        int (*drv_stat   )(void *drvhdl, struct vfs_dev_stat *info);
};

struct _module_entry {
        const char              *name;
        const struct _module_if  IF;
};

/*
 * To lock device, system uses PID of syscall's client.
 * Doxygen documentation in drivers/driver.h.
 */
typedef pid_t dev_lock_t;

/*==============================================================================
  Exported objects
==============================================================================*/

/*==============================================================================
  Exported functions
==============================================================================*/
extern int         _driver_init                   (const char*, u8_t, u8_t, const char*, const void*, dev_t*);
extern int         _driver_release                (dev_t);
extern int         _driver_open                   (dev_t, u32_t);
extern int         _driver_close                  (dev_t, bool);
extern int         _driver_write                  (dev_t, const u8_t*, size_t, fpos_t*, size_t*, struct vfs_fattr);
extern int         _driver_read                   (dev_t, u8_t*, size_t, fpos_t*, size_t*, struct vfs_fattr);
extern int         _driver_ioctl                  (dev_t, int, void*);
extern int         _driver_flush                  (dev_t);
extern int         _driver_stat                   (dev_t, struct vfs_dev_stat*);
extern int         _module_get_instance           (const char*, u8_t, u8_t, void**);
extern const char *_module_get_name               (size_t);
extern size_t      _module_get_count              (void);
extern i32_t       _module_get_ID                 (const char*);
extern ssize_t     _module_get_number_of_instances(size_t);
extern int         _device_lock                   (dev_lock_t*);
extern int         _device_unlock                 (dev_lock_t*, bool);
extern int         _device_get_access             (dev_lock_t*);
extern bool        _device_is_locked              (dev_lock_t*);

/*==============================================================================
  Exported inline functions
==============================================================================*/
//==============================================================================
/**
 * @brief  Create dev_t-type value by using module id, major and minor numbers
 *
 * @param  modid        module id
 * @param  major        module major number
 * @param  minor        module minor number
 *
 * @return Device ID
 */
//==============================================================================
static inline dev_t _dev_t__create(u16_t modid, u8_t major, u8_t minor)
{
        return ((modid & 0x7FFF) << 16) | ((major & 0xFF) << 8) | (minor & 0xFF);
}

//==============================================================================
/**
 * @brief  Extract module id from dev_t-type value
 *
 * @param  devid        device id
 *
 * @return Module ID
 */
//==============================================================================
static inline u16_t _dev_t__extract_modno(dev_t devid)
{
        return (devid >> 16) & 0x7FFF;
}

//==============================================================================
/**
 * @brief  Extract major number from dev_t-type value
 *
 * @param  devid        device id
 *
 * @return Major number
 */
//==============================================================================
static inline u8_t _dev_t__extract_major(dev_t devid)
{
        return (devid >> 8) & 0xFF;
}

//==============================================================================
/**
 * @brief  Extract minor number from dev_t-type value
 *
 * @param  devid        device id
 *
 * @return Minor number
 */
//==============================================================================
static inline u8_t _dev_t__extract_minor(dev_t devid)
{
        return (devid >> 0) & 0xFF;
}

#ifdef __cplusplus
}
#endif

#endif /* _DRVCTRL_H_ */
/*==============================================================================
  End of file
==============================================================================*/
