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
PROGRAM(top, 3)
PROG_SEC_BEGIN

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
stdRet_t appmain(ch_t *argv[], int_t argc)
{
        (void) argv;

        u8_t divcnt = 10;

        while (TRUE) {
                ch_t chr = EOF;
                fread(&chr, sizeof(ch_t), 1, stdin);

                if (chr == 'q') {
                        break;
                }

                milisleep(100);

                if (divcnt < 10) {
                        divcnt++;
                        continue;
                }

                u8_t n = SystemGetMoniTaskCount();

                printf("\x1B[2J\x1B[HPress q to quit\n");

                printf("Total tasks: %u\n", n);

                printf("Memory:\t%u total,\t%u used,\t%u free\n\n",
                       SystemGetMemSize(),
                       SystemGetUsedMemSize(),
                       SystemGetFreeMemSize());

                printf("\x1B[30;47m TSKHDL   PR    FRSTK   MEM     OPFI    %%CPU    NAME \x1B[0m\n");

                for (int_t i = 0; i < n; i++) {
                        struct taskstat taskinfo;

                        if (SystemGetTaskStat(i, &taskinfo) == STD_RET_OK) {
                                printf("%x  %d\t%u\t%u\t%u\t%u.%u%%\t%s\n",
                                taskinfo.task_handle,
                                taskinfo.priority,
                                taskinfo.free_stack,
                                taskinfo.memory_usage,
                                taskinfo.opened_files,
                                ( taskinfo.cpu_usage * 100)  / taskinfo.cpu_usage_total,
                                ((taskinfo.cpu_usage * 1000) / taskinfo.cpu_usage_total) % 10,
                                taskinfo.task_name);
                        } else {
                                break;
                        }
                }

                divcnt = 0;
        }

        return STD_RET_OK;
}

/* End of application section declaration */
PROG_SEC_END

/*==================================================================================================
                                            End of file
==================================================================================================*/
