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
#define _MODULE_NAME(module_name)               #module_name

#define _DRIVER_INTERFACE(_drvmodule, _drvname, _major, _minor)\
{.drv_name    = _drvname,\
 .mod_name    = #_drvmodule,\
 .major       = _major,\
 .minor       = _minor,\
 .interface   = &_regdrv_##_drvmodule##_interface}

#define _IMPORT_MODULE_INTERFACE(_modname)                                              \
extern API_MOD_INIT(_modname, void**, u8_t, u8_t);                                      \
extern API_MOD_RELEASE(_modname, void*);                                                \
extern API_MOD_OPEN(_modname, void*, vfs_open_flags_t);                                 \
extern API_MOD_CLOSE(_modname, void*, bool);                                            \
extern API_MOD_WRITE(_modname, void*, const u8_t*, size_t, fpos_t*, struct vfs_fattr);  \
extern API_MOD_READ(_modname, void*, u8_t*, size_t, fpos_t*, struct vfs_fattr);         \
extern API_MOD_IOCTL(_modname, void*, int, void*);                                      \
extern API_MOD_FLUSH(_modname, void*);                                                  \
extern API_MOD_STAT(_modname, void*, struct vfs_dev_stat*);                             \
static const struct _driver_if _regdrv_##_modname##_interface = {                       \
 .drv_init    = _##_modname##_init,                                                     \
 .drv_release = _##_modname##_release,                                                  \
 .drv_open    = _##_modname##_open,                                                     \
 .drv_close   = _##_modname##_close,                                                    \
 .drv_write   = _##_modname##_write,                                                    \
 .drv_read    = _##_modname##_read,                                                     \
 .drv_ioctl   = _##_modname##_ioctl,                                                    \
 .drv_stat    = _##_modname##_stat,                                                     \
 .drv_flush   = _##_modname##_flush}

/*==============================================================================
  Exported object types
==============================================================================*/
struct _driver_if {
        stdret_t (*drv_init   )(void **drvhdl, u8_t major, u8_t minor);
        stdret_t (*drv_release)(void *drvhdl);
        stdret_t (*drv_open   )(void *drvhdl, vfs_open_flags_t flags);
        stdret_t (*drv_close  )(void *drvhdl, bool force);
        ssize_t  (*drv_write  )(void *drvhdl, const u8_t *src, size_t count, fpos_t *fpos, struct vfs_fattr attr);
        ssize_t  (*drv_read   )(void *drvhdl, u8_t *dst, size_t count, fpos_t *fpos, struct vfs_fattr attr);
        int      (*drv_ioctl  )(void *drvhdl, int iorq, void *args);
        stdret_t (*drv_flush  )(void *drvhdl);
        stdret_t (*drv_stat   )(void *drvhdl, struct vfs_dev_stat *info);
};

struct _driver_entry {
        const struct _driver_if *interface;
        const char              *drv_name;
        const char              *mod_name;
        const u8_t               major;
        const u8_t               minor;
};

/*==============================================================================
  Exported objects
==============================================================================*/

/*==============================================================================
  Exported functions
==============================================================================*/
extern int         _driver_init                 (const char*, const char*);
extern int         _driver_release              (const char*);
extern stdret_t    _driver_open                 (dev_t, vfs_open_flags_t);
extern stdret_t    _driver_close                (dev_t, bool);
extern ssize_t     _driver_write                (dev_t, const u8_t*, size_t, fpos_t*, struct vfs_fattr);
extern ssize_t     _driver_read                 (dev_t, u8_t*, size_t, fpos_t*, struct vfs_fattr);
extern int         _driver_ioctl                (dev_t, int, void*);
extern stdret_t    _driver_flush                (dev_t);
extern stdret_t    _driver_stat                 (dev_t, struct vfs_dev_stat*);
extern const char *_get_module_name             (uint);
extern uint        _get_number_of_modules       (void);
extern int         _get_module_number           (const char*);
extern uint        _get_number_of_drivers       (void);
extern const char *_get_driver_name             (uint);
extern int         _get_driver_ID               (const char*);
extern const char *_get_driver_module_name      (uint);
extern bool        _is_driver_active            (uint);

#ifdef __cplusplus
}
#endif

#endif /* _MODCTRL_H_ */
/*==============================================================================
  End of file
==============================================================================*/
