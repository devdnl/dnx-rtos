/*=============================================================================================*//**
@file    uptime.c

@author  Daniel Zorychta

@brief

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

/*==================================================================================================
                                            Include files
==================================================================================================*/
#include "clear.h"
#include <string.h>

#include "ds1307.h"

/* Begin of application section declaration */
APPLICATION(uptime)
APP_SEC_BEGIN

/*==================================================================================================
                                  Local symbolic constants/macros
==================================================================================================*/


/*==================================================================================================
                                   Local types, enums definitions
==================================================================================================*/


/*==================================================================================================
                                      Local object definitions
==================================================================================================*/


/*==================================================================================================
                                        Function definitions
==================================================================================================*/

//================================================================================================//
/**
 * @brief clear main function
 */
//================================================================================================//
stdRet_t appmain(ch_t *argv)
{
      (void) argv;

      bcdTime_t time = DS1307_GetTime();
      u32_t uptime   = SystemGetUptime();
      u32_t udays    = (uptime / (3600 * 24));
      u32_t uhrs     = (uptime / 3600) % 24;
      u32_t umins    = (uptime / 60) % 60;

      print("%x2:%x2:%x2, up %ud %u2:%u2\n",
            time.hours, time.minutes, time.seconds,
            udays, uhrs, umins);

      return STD_RET_OK;
}

/* End of application section declaration */
APP_SEC_END

/*==================================================================================================
                                            End of file
==================================================================================================*/
