#ifndef _DRIVER_REGISTRATION_
#define _DRIVER_REGISTRATION_
/*=========================================================================*//**
@file    driver_registration.h

@author  Daniel Zorychta

@brief   This file is used to registration drivers

@note    Copyright (C) 2012, 2013 Daniel Zorychta <daniel.zorychta@gmail.com>

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
#include "drivers/driver_registration.h"

/*==============================================================================
  Exported symbolic constants/macros
==============================================================================*/
#define USE_MODULE(module_name)                 #module_name

#define USE_DRIVER_INTERFACE(_drvmodule, _drvname, _major, _minor)\
{.drv_name    = _drvname,\
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
  Exported types, enums definitions
==============================================================================*/
struct _driver_entry {
        const char               *drv_name;
        u8_t                      major;
        u8_t                      minor;
        stdret_t                (*drv_init   )(void **drvhdl, u8_t major, u8_t minor);
        stdret_t                (*drv_release)(void *drvhdl);
        struct vfs_drv_interface  drv_if;
};

/*==============================================================================
  Exported object declarations
==============================================================================*/
extern const char                 *_regdrv_module_name[];
extern const struct _driver_entry  _regdrv_driver_table[];
extern const uint                  _regdrv_driver_table_array_size;
extern const uint                  _regdrv_number_of_modules;

/*==============================================================================
  Exported function prototypes
==============================================================================*/

#ifdef __cplusplus
}
#endif

#endif /* _DRIVER_REGISTRATION_ */
/*==============================================================================
  End of file
==============================================================================*/
