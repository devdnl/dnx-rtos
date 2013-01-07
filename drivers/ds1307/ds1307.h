#ifndef DS1307_H_
#define DS1307_H_
/*=============================================================================================*//**
@file    ds1307.h

@author  Daniel Zorychta

@brief   This file support DS1307

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


*//*==============================================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*==================================================================================================
                                            Include files
==================================================================================================*/
#include "ds1307_def.h"
#include "sysdrv.h"


/*==================================================================================================
                                  Exported symbolic constants/macros
==================================================================================================*/
#define DS1307_DEV_RTC        0
#define DS1307_DEV_NVM        1
#define DS1307_PART_NONE      0


/*==================================================================================================
                                  Exported types, enums definitions
==================================================================================================*/


/*==================================================================================================
                                     Exported object declarations
==================================================================================================*/


/*==================================================================================================
                                     Exported function prototypes
==================================================================================================*/
DRIVER_INTERFACE_CLASS(DS1307);


#ifdef __cplusplus
}
#endif

#endif /* DS1307_H_ */
/*==================================================================================================
                                             End of file
==================================================================================================*/
