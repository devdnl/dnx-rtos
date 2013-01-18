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
APPLICATION(top, 3)
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

      u8_t divcnt = 10;

      while (ugetchar() != 'q') {
            Sleep(100);

            if (divcnt >= 10) {
                  u8_t n = SystemGetMoniTaskCount();

                  printf("\x1B[2J\x1B[HPress q to quit\n");

                  printf("Total tasks: %u\n", n);

                  printf("Memory:\t%u total,\t%u used,\t%u free\n\n",
                         SystemGetMemSize(), SystemGetUsedMemSize(), SystemGetFreeMemSize());

                  printf("\x1B[30;47m TSKHDL   PR    FRSTK   MEM     OPFI    %%CPU    NAME \x1B[0m\n");

                  for (u16_t i = 0; i < n; i++) {
                        struct taskstat taskinfo;

                        if (SystemGetTaskStat(i, &taskinfo) == STD_RET_OK) {
                              printf("%x  %d\t%u\t%u\t%u\t%u.%u%%\t%s\n",
                                     taskinfo.handle,
                                     taskinfo.priority,
                                     taskinfo.freeStack,
                                     taskinfo.memUsage,
                                     taskinfo.openFiles,
                                     ( taskinfo.cpuUsage * 100)  / taskinfo.cpuUsageTotal,
                                     ((taskinfo.cpuUsage * 1000) / taskinfo.cpuUsageTotal) % 10,
                                     taskinfo.name);
                        } else {
                              break;
                        }
                  }

                  divcnt = 0;
            } else {
                  divcnt++;
            }
      }

      return STD_RET_OK;
}

/* End of application section declaration */
APP_SEC_END

/*==================================================================================================
                                            End of file
==================================================================================================*/
