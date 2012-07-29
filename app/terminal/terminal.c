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
APPLICATION(terminal)
{
      InitSTDIO();
      InitArgvAs(u8_t);

      u32_t   cnt = 0;
      u8_t    *mem = NULL;

      sprint(stdio, "Hello world. I'm terminal\n");

      for (;;)
      {
            sprint(stdio, "%d: Heap free space: %d; stack free space: %d\n",
                   cnt++, GetFreeHeapSize(), TaskGetStackFreeSpace(THIS_TASK));

            if (cnt == 2)
            {
                  sprint(stdio, "Try to alloc 60000 bytes...\n");
                  mem = (u8_t*)Malloc(60000);

                  if (mem == NULL)
                        sprint(stdio, "Allocation failed :(\n");
            }
            else if (cnt == 4)
            {
                  if (mem)
                  {
                        sprint(stdio, "Freed allocated memory...\n");
                        Free(mem);
                  }
            }
            else if (cnt == 6)
            {
                  ClearStdin(stdio);

                  for (;;)
                  {
                        sprint(stdio, "Czy zakonczyc terminal? [t/n]: ");
                        ch_t ch = GetChar(stdio);
                        PutChar(stdio, ch);
                        sprint(stdio, "\n");

                        if (ch == 't')
                              Exit(STD_STATUS_OK);
                  }
            }

            TaskDelay(1000);
      }

      Exit(STD_STATUS_ERROR);
}


/*==================================================================================================
                                            End of file
==================================================================================================*/
