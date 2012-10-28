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
#include "clear.h"
#include "freemem.h"
#include "date.h"
#include "ls.h"
#include "malt.h"
#include "uptime.h"
#include "top.h"
#include "httpd.h"
#include "touch.h"
#include "rm.h"


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
static const regAppData_t appList[] =
{
      {TERMINAL_NAME, terminal, TERMINAL_STACK_SIZE},
      {CLEAR_NAME   , clear   , CLEAR_STACK_SIZE   },
      {FREEMEM_NAME,  freemem,  FREEMEM_STACK_SIZE },
      {DATE_NAME,     date,     DATE_STACK_SIZE    },
      {LS_NAME,       ls,       LS_STACK_SIZE      },
      {MALT_NAME,     malt,     MALT_STACK_SIZE    },
      {UPTIME_NAME,   uptime,   UPTIME_STACK_SIZE  },
      {TOP_NAME,      top,      TOP_STACK_SIZE     },
      {HTTPD_NAME,    httpd,    HTTPD_STACK_SIZE   },
      {TOUCH_NAME,    touch,    TOUCH_STACK_SIZE   },
      {RM_NAME,       rm,       RM_STACK_SIZE      },
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
regAppData_t GetAppData(const ch_t *appName)
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


//================================================================================================//
/**
 * @brief Send application list
 *
 * @param *nameList           list of exist application
 *
 * @return number of exist application
 */
//================================================================================================//
u32_t GetAppList(ch_t *nameList, u32_t size)
{
      if (ARRAY_SIZE(appList) == 0)
            return 0;

      if (size)
            size--;

      u32_t app;
      for (app = 0; app < ARRAY_SIZE(appList); app++)
      {
            u32_t i;
            for (i = 0; i < strlen(appList[app].appName); i++)
            {
                  if (size)
                  {
                        *(nameList++) = appList[app].appName[i];
                        size--;
                  }
                  else
                  {
                        *(nameList) = ASCII_NULL;
                        return app;
                  }

                  size--;
            }

            if (size--)
                  *(nameList++) = ASCII_CR;

            if (size--)
                  *(nameList++) = ASCII_LF;
      }

      return app;
}



//================================================================================================//
/**
 * @brief Function open driver directory
 *
 * @param *dir          directory
 *
 * @return number of items
 */
//================================================================================================//
void REGAPP_opendir(DIR_t *dir)
{
      dir->readdir = REGAPP_readdir;
      dir->seek    = 0;
      dir->items   = ARRAY_SIZE(appList);
}


//================================================================================================//
/**
 * @brief Function read selected item
 *
 * @param seek          nitem
 * @return file attributes
 */
//================================================================================================//
dirent_t REGAPP_readdir(size_t seek)
{
      dirent_t direntry;
      direntry.name = NULL;
      direntry.size = 0;
      direntry.fd   = 0;

      if (seek < ARRAY_SIZE(appList))
      {
            direntry.name   = (ch_t*)appList[seek].appName;
            direntry.size   = appList[seek].stackSize * sizeof(void *)
                            + strlen(direntry.name)
                            + sizeof(regAppData_t);
            direntry.isfile = TRUE;
      }

      return direntry;
}


#ifdef __cplusplus
}
#endif

/*==================================================================================================
                                            End of file
==================================================================================================*/
