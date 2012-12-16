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

#include "appmoni.h"

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

      if (ParseArg(argv, "o", PARSE_AS_EXIST, NULL) == STD_RET_OK) {
            u32_t size   = 50 * SystemGetTaskCount();
            u8_t  divcnt = 10;
            ch_t  *buffer;

            if ( (buffer = calloc(size, sizeof(ch_t))) )
            {
                  while (ugetChar() != 'q')
                  {
                        Sleep(100);

                        if (divcnt >= 10)
                        {
                              SystemGetRunTimeStats(buffer);

                              printf("\x1B[2J\x1B[HPress q to quit\n");

                              printf("Total tasks: %u\n", SystemGetTaskCount());

                              printf("Memory:\t%u total,\t%u used,\t%u free\n\n",
                                     SystemGetMemSize(), SystemGetUsedMemSize(), SystemGetFreeMemSize());

                              printf("Name\t\tTime\t\tUsage\n%s\n", buffer);

                              divcnt = 0;
                        }
                        else
                        {
                              divcnt++;
                        }
                  }

                  status = STD_RET_OK;

                  free(buffer);
            }
            else
            {
                  printf("No enough free memory!\n");
            }
      } else {
            struct taskstat taskinfo;

            u8_t divcnt = 10;

            while (ugetChar() != 'q') {
                  Sleep(100);

                  if (divcnt >= 10) {
                        printf("\x1B[2J\x1B[HPress q to quit\n");

                        printf("Total tasks: %u\n", SystemGetTaskCount());

                        printf("Memory:\t%u total,\t%u used,\t%u free\n\n",
                               SystemGetMemSize(), SystemGetUsedMemSize(), SystemGetFreeMemSize());

                        printf("TaskHdl\t\tFreeStack\tMemUsage\tOpenFiles\t%%CPU\tName\n");

                        for (u16_t i = 0; ; i++) {
                              if (moni_GetTaskStat(i, &taskinfo) == STD_RET_OK) {
                                    printf("%x\t%u\t\t%u\t\t%u\t\t%u%%\t%s\n",
                                           taskinfo.taskHdl,
                                           taskinfo.taskFreeStack,
                                           taskinfo.memUsage,
                                           taskinfo.fileUsage,
                                           taskinfo.cpuUsage,
                                           taskinfo.taskName);
                              } else {
                                    break;
                              }
                        }

                        divcnt = 0;
                  } else {
                        divcnt++;
                  }

                  status = STD_RET_OK;
            }
      }

      return status;
}

/* End of application section declaration */
APP_SEC_END

/*==================================================================================================
                                            End of file
==================================================================================================*/
