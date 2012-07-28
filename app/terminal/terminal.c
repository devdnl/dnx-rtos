/*=============================================================================================*//**
@file    terminal.c

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
#include "terminal.h"
#include "uart.h"
#include <string.h>
#include <stdarg.h>


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
APPLICATION(terminal, arg)
{
      stdio_t *stdio = (stdio_t*)arg;
      u32_t   cnt = 0;
      u8_t    *mem = NULL;

      sprint(stdio, "Hello world. I'm terminal\r\n");

      for (;;)
      {
            sprint(stdio, "%d: Heap free space: %d; stack free space: %d\r\n",
                   cnt++, GetFreeHeapSize(), TaskGetStackFreeSpace(THIS_TASK));

            if (cnt == 30)
            {
                  sprint(stdio, "Try to alloc 60000 bytes...\r\n");
                  mem = (u8_t*)Malloc(60000);

                  if (mem == NULL)
                        sprint(stdio, "Allocation failed :(\r\n");
            }
            else if (cnt == 40)
            {
                  if (mem)
                  {
                        sprint(stdio, "Freed allocated memory...\r\n");
                        Free(mem);
                  }
            }

            TaskDelay(1000);
      }

      TaskTerminate();
}


/*==================================================================================================
                                            End of file
==================================================================================================*/
