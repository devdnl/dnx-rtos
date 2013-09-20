#ifndef _DRIVERS_H_
#define _DRIVERS_H_
/*=========================================================================*//**
@file    drivers.h

@author  Daniel Zorychta

@brief   Drivers support.

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
 .drv_if      = {.handle    = NULL,\
                 .drv_open  = _##_drvmodule##_open,\
                 .drv_close = _##_drvmodule##_close,\
                 .drv_write = _##_drvmodule##_write,\
                 .drv_read  = _##_drvmodule##_read,\
                 .drv_ioctl = _##_drvmodule##_ioctl,\
                 .drv_stat  = _##_drvmodule##_stat,\
                 .drv_flush = _##_drvmodule##_flush}}

/*==============================================================================
  Exported object types
==============================================================================*/
struct _driver_entry {
        const char               *drv_name;
        const char               *mod_name;
        u8_t                      major;
        u8_t                      minor;
        stdret_t                (*drv_init   )(void **drvhdl, u8_t major, u8_t minor);
        stdret_t                (*drv_release)(void *drvhdl);
        struct vfs_drv_interface  drv_if;
};

/*==============================================================================
  Exported objects
==============================================================================*/

/*==============================================================================
  Exported functions
==============================================================================*/
extern stdret_t    init_driver            (const char *drvName, const char *nodeName);
extern stdret_t    release_driver         (const char *drvName);
extern const char *_get_module_name       (int module_number);
extern int         _get_module_number     (const char *module_name);

#ifdef __cplusplus
}
#endif

#endif /* _DRIVERS_H_ */
/*==============================================================================
  End of file
==============================================================================*/
