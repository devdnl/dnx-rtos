/*=============================================================================================*//**
@file    regapp.c

@author  Daniel Zorychta

@brief   This file is used to registration applications

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
#include "regapp.h"
#include <string.h>

/* include here applications headers */
#include "terminal.h"


/*==================================================================================================
                                  Local symbolic constants/macros
==================================================================================================*/


/*==================================================================================================
                                   Local types, enums definitions
==================================================================================================*/


/*==================================================================================================
                                      Local function prototypes
==================================================================================================*/


/*==================================================================================================
                                      Local object definitions
==================================================================================================*/
const regAppData_t appList[] =
{
      {TERMINAL_NAME, terminal, TERMINAL_STACK_SIZE},
};


/*==================================================================================================
                                     Exported object definitions
==================================================================================================*/


/*==================================================================================================
                                        Function definitions
==================================================================================================*/

//================================================================================================//
/**
 * @brief
 */
//================================================================================================//
regAppData_t REGAPP_GetAppData(const ch_t *appName)
{
      u32_t i;

      for (i = 0; i < ARRAY_SIZE(appList); i++)
      {
            if (strcmp(appList[i].appName, appName) == 0)
            {
                  return appList[i];
            }
      }

      regAppData_t appNULL = {NULL, NULL, 0};

      return appNULL;
}


#ifdef __cplusplus
}
#endif

/*==================================================================================================
                                            End of file
==================================================================================================*/
