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
#include "date.h"
#include "top.h"
#include "httpd.h"
#include "measd.h"
#include "cat.h"


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
static const regAppData_t appList[] = {
      {TERMINAL_NAME, terminal, TERMINAL_STACK_SIZE},
      {DATE_NAME    , date    , DATE_STACK_SIZE    },
      {TOP_NAME     , top     , TOP_STACK_SIZE     },
      {HTTPD_NAME   , httpd   , HTTPD_STACK_SIZE   },
      {MEASD_NAME   , measd   , MEASD_STACK_SIZE   },
      {CAT_NAME     , cat     , CAT_STACK_SIZE     },
};


/*==================================================================================================
                                     Exported object definitions
==================================================================================================*/


/*==================================================================================================
                                        Function definitions
==================================================================================================*/

//================================================================================================//
/**
 * @brief Function find in the application list selected application's parameters
 *
 * @param *appName            application name
 *
 * @return application informations needed to run
 */
//================================================================================================//
regAppData_t regapp_GetAppData(const ch_t *appName)
{
      regAppData_t appNULL = {NULL, NULL, 0};

      for (uint_t i = 0; i < ARRAY_SIZE(appList); i++) {
            if (strcmp(appList[i].appName, appName) == 0) {
                  appNULL = appList[i];
                  break;
            }
      }

      return appNULL;
}


//================================================================================================//
/**
 * @brief Function returns pointer to application list
 *
 * @return pointer to application list
 */
//================================================================================================//
regAppData_t *regapp_GetAppListPtr(void)
{
      return (regAppData_t*)appList;
}


//================================================================================================//
/**
 * @brief Function returns application count
 *
 * @return application count
 */
//================================================================================================//
int_t regapp_GetAppCount(void)
{
      return ARRAY_SIZE(appList);
}


#ifdef __cplusplus
}
#endif

/*==================================================================================================
                                            End of file
==================================================================================================*/
