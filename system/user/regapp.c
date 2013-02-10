/*=========================================================================*//**
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


*//*==========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Include files
==============================================================================*/
#include "regapp.h"
#include <string.h>

/* include here applications headers */
#include "terminal.h"
#include "date.h"
#include "top.h"
#if !defined(ARCH_posix) /* DNLFIXME this must be changed!! LwIP correction needed! */
#include "httpd.h"
#endif
#include "measd.h"
#include "cat.h"
#include "lwipd.h"

/*==============================================================================
  Local symbolic constants/macros
==============================================================================*/
#define IMPORT_APPLICATION(name)        {.appName   = #name, \
                                         .appPtr    = name,  \
                                         .stackSize = &name##_stack_size}

/*==============================================================================
  Local types, enums definitions
==============================================================================*/

/*==============================================================================
  Local function prototypes
==============================================================================*/

/*==============================================================================
  Local object definitions
==============================================================================*/
static const regAppData_t appList[] = {
        IMPORT_APPLICATION(term),
        IMPORT_APPLICATION(date),
        IMPORT_APPLICATION(top),
#if !defined(ARCH_posix)
        IMPORT_APPLICATION(httpd),
#endif
        IMPORT_APPLICATION(measd),
        IMPORT_APPLICATION(cat),
        IMPORT_APPLICATION(lwipd),
};

/*==============================================================================
  Exported object definitions
==============================================================================*/

/*==============================================================================
  Function definitions
==============================================================================*/

//==============================================================================
/**
 * @brief Function find in the application list selected application's parameters
 *
 * @param *appName            application name
 *
 * @return application informations needed to run
 */
//==============================================================================
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

//==============================================================================
/**
 * @brief Function returns pointer to application list
 *
 * @return pointer to application list
 */
//==============================================================================
regAppData_t *regapp_GetAppListPtr(void)
{
        return (regAppData_t*)appList;
}

//==============================================================================
/**
 * @brief Function returns application count
 *
 * @return application count
 */
//==============================================================================
int_t regapp_GetAppCount(void)
{
        return ARRAY_SIZE(appList);
}

#ifdef __cplusplus
}
#endif

/*==============================================================================
  End of file
==============================================================================*/
