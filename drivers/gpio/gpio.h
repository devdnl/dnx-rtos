#ifndef GPIO_H_
#define GPIO_H_
/*=============================================================================================*//**
@file    gpio.h

@author  Daniel Zorychta

@brief   This driver support GPIO.

@note    Copyright (C) 2012  Daniel Zorychta <daniel.zorychta@gmail.com>

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
#include "gpio_cfg.h"
#include "gpio_def.h"
#include "sysdrv.h"


/*==================================================================================================
                                  Exported symbolic constants/macros
==================================================================================================*/


/*==================================================================================================
                                  Exported types, enums definitions
==================================================================================================*/
#define GPIO_PART_NONE       0

/** port names */
enum GPIO_DEV_NUMBER_enum
{
   GPIO_DEV_NONE,
   GPIO_DEV_LAST
};


/*==================================================================================================
                                     Exported function prototypes
==================================================================================================*/
DRIVER_INTERFACE_CLASS(GPIO);

#ifdef __cplusplus
}
#endif

#endif /* GPIO_H_ */
/*==================================================================================================
                                             End of file
==================================================================================================*/
