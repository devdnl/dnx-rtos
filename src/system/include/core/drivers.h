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

/*==============================================================================
  Exported macros
==============================================================================*/

/*==============================================================================
  Exported object types
==============================================================================*/

/*==============================================================================
  Exported objects
==============================================================================*/

/*==============================================================================
  Exported functions
==============================================================================*/
extern stdret_t    init_driver            (const char *drvName, const char *nodeName);
extern stdret_t    release_driver         (const char *drvName);
extern const char *_get_module_name       (uint module_number);
extern int         _get_module_number     (const char *module_name);

#ifdef __cplusplus
}
#endif

#endif /* _DRIVERS_H_ */
/*==============================================================================
  End of file
==============================================================================*/
