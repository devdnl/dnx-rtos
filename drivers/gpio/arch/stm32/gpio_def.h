#ifndef GPIO_DEF_H_
#define GPIO_DEF_H_
/*=============================================================================================*//**
@file    gpio_def.h

@author  Daniel Zorychta

@brief   This driver support GPIO definitions

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


/*==================================================================================================
                                  Exported symbolic constants/macros
==================================================================================================*/
#define GPIO_PART_NONE       0

/*==================================================================================================
                                  Exported types, enums definitions
==================================================================================================*/
/** port names */
enum GPIO_DEV_NUMBER_enum
{
   GPIO_DEV_NONE,
   GPIO_DEV_LAST
};

/*==================================================================================================
                                     Exported object declarations
==================================================================================================*/


/*==================================================================================================
                                     Exported function prototypes
==================================================================================================*/


#ifdef __cplusplus
}
#endif

#endif /* GPIO_DEF_H_ */
/*==================================================================================================
                                             End of file
==================================================================================================*/
