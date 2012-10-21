/*=============================================================================================*//**
@file    top.c

@author  Daniel Zorychta

@brief   Application show CPU load

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
#include "top.h"
#include "regapp.h"
#include <string.h>

/* Begin of application section declaration */
APPLICATION(top)
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

      stdRet_t status = STD_RET_ERROR;

      u32_t size    = 50 * SystemGetTaskCount();
      ch_t  *buffer = Calloc(size, sizeof(ch_t));

      if (buffer)
      {
            SystemGetRunTimeStats(buffer);

            while (ugetChar() != 'q')
            {
                  Sleep(1000);
                  SystemGetRunTimeStats(buffer);
                  printf("\x1B[2J\x1B[HName\t\tTime\t\tUsage\n%s\nPress q to quit\n", buffer);
                  memset(buffer, 0, size);
            }

            status = STD_RET_OK;
      }
      else
      {
            printf("No enough free memory!\n");
      }

      return status;
}

/* End of application section declaration */
APP_SEC_END

/*==================================================================================================
                                            End of file
==================================================================================================*/
