/*=========================================================================*//**
@file    modctrl.h

@author  Daniel Zorychta

@brief   Modules support.

@note    Copyright (C) 2013 Daniel Zorychta <daniel.zorychta@gmail.com>

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

#ifndef _MODCTRL_H_
#define _MODCTRL_H_

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Include files
==============================================================================*/
#include "core/vfs.h"

/*==============================================================================
  Exported macros
==============================================================================*/
#define _USE_MODULE(module_name)                 #module_name

#define _USE_DRIVER_INTERFACE(_drvmodule, _drvname, _major, _minor)\
{.drv_name    = _drvname,\
 .mod_name    = #_drvmodule,\
 .major       = _major,\
 .minor       = _minor,\
 .drv_init    = _##_drvmodule##_init,\
 .drv_release = _##_drvmodule##_release,\
 .drv_open    = _##_drvmodule##_open,\
 .drv_close   = _##_drvmodule##_close,\
 .drv_write   = _##_drvmodule##_write,\
 .drv_read    = _##_drvmodule##_read,\
 .drv_ioctl   = _##_drvmodule##_ioctl,\
 .drv_stat    = _##_drvmodule##_stat,\
 .drv_flush   = _##_drvmodule##_flush}

/*==============================================================================
  Exported object types
==============================================================================*/
struct _driver_entry {
        const char       *drv_name;
        const char       *mod_name;
        u8_t              major;
        u8_t              minor;
        stdret_t        (*drv_init   )(void **drvhdl, u8_t major, u8_t minor);
        stdret_t        (*drv_release)(void *drvhdl);
        stdret_t        (*drv_open   )(void *drvhdl, int flags);
        stdret_t        (*drv_close  )(void *drvhdl, bool force);
        ssize_t         (*drv_write  )(void *drvhdl, const u8_t *src, size_t count, u64_t *fpos, struct vfs_fattr attr);
        ssize_t         (*drv_read   )(void *drvhdl, u8_t *dst, size_t count, u64_t *fpos, struct vfs_fattr attr);
        stdret_t        (*drv_ioctl  )(void *drvhdl, int iorq, void *args);
        stdret_t        (*drv_flush  )(void *drvhdl);
        stdret_t        (*drv_stat   )(void *drvhdl, struct vfs_dev_stat *info);
};

/*==============================================================================
  Exported objects
==============================================================================*/

/*==============================================================================
  Exported functions
==============================================================================*/
extern int         _driver_init                 (const char*, const char*);
extern int         _driver_release              (const char*);
extern stdret_t    _driver_open                 (dev_t, int);
extern stdret_t    _driver_close                (dev_t, bool);
extern ssize_t     _driver_write                (dev_t, const u8_t*, size_t, u64_t*, struct vfs_fattr);
extern ssize_t     _driver_read                 (dev_t, u8_t*, size_t, u64_t*, struct vfs_fattr);
extern stdret_t    _driver_ioctl                (dev_t, int, void*);
extern stdret_t    _driver_flush                (dev_t);
extern stdret_t    _driver_stat                 (dev_t, struct vfs_dev_stat*);
extern const char *_get_module_name             (uint);
extern int         _get_number_of_modules       (void);
extern int         _get_module_number           (const char*);
extern int         _get_number_of_drivers       (void);
extern const char *_get_driver_name             (uint);
extern const char *_get_driver_module_name      (uint);
extern bool        _is_driver_active            (uint);

#ifdef __cplusplus
}
#endif

#endif /* _MODCTRL_H_ */
/*==============================================================================
  End of file
==============================================================================*/
