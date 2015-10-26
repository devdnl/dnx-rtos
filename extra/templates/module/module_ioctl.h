/*=========================================================================*//**
@file    <!module_name!>_ioctl.h

@author  <!author!>

@brief   <!module_description!>

@note    Copyright (C) <!year!> <!author!> <<!email!>>

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

#ifndef _<!MODULE_NAME!>_IOCTL_H_
#define _<!MODULE_NAME!>_IOCTL_H_

/*==============================================================================
  Include files
==============================================================================*/
#include "drivers/ioctl_macros.h"

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Exported macros
==============================================================================*/
/**
 * @brief  ...
 * @param  ...
 * @return ...
 */
#define IOCTL_<!MODULE_NAME!>__EX1           _IO(<!MODULE_NAME!>, 0x00)

/**
 * @brief  ...
 * @param  ...
 * @return ...
 */
#define IOCTL_<!MODULE_NAME!>__EX2           _IOR(<!MODULE_NAME!>, 0x01, int*)

/**
 * @brief  ...
 * @param  ...
 * @return ...
 */
#define IOCTL_<!MODULE_NAME!>__EX3           _IOW(<!MODULE_NAME!>, 0x02, int*)

/**
 * @brief  ...
 * @param  ...
 * @return ...
 */
#define IOCTL_<!MODULE_NAME!>__EX4           _IOWR(<!MODULE_NAME!>, 0x03, int*)


/*==============================================================================
  Exported object types
==============================================================================*/

/*==============================================================================
  Exported objects
==============================================================================*/

/*==============================================================================
  Exported functions
==============================================================================*/

/*==============================================================================
  Exported inline functions
==============================================================================*/

#ifdef __cplusplus
}
#endif

#endif /* _<!MODULE_NAME!>_IOCTL_H_ */
/*==============================================================================
  End of file
==============================================================================*/
