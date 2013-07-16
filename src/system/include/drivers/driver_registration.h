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

/*==============================================================================
  Exported symbolic constants/macros
==============================================================================*/
/*
 * USER CONFIGURATION: this determine how many driver modules are used in the
 *                     system. Driver module example: UART, TTY, I2C, etc.
 *                     One module can support many devices e.g.: UART can support
 *                     uart1, uart2, etc, so here type only number of modules not
 *                     supported devices.
 */
#define REGDRV_NUMBER_OF_REGISTERED_MODULES             5

/*==============================================================================
  Exported types, enums definitions
==============================================================================*/

/*==============================================================================
  Exported object declarations
==============================================================================*/

/*==============================================================================
  Exported function prototypes
==============================================================================*/
extern stdret_t    init_driver(const char *drvName, const char *nodeName);
extern stdret_t    release_driver(const char *drvName);
extern const char *regdrv_get_module_name(uint module_number);
extern int         regdrv_get_module_number(const char *module_name);

#ifdef __cplusplus
}
#endif

#endif /* _DRIVER_REGISTRATION_ */
/*==============================================================================
  End of file
==============================================================================*/
