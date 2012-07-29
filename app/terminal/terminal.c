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

      print("Hello world. I'm terminal\n");

      for (;;)
      {
            print("%d: Heap free space: %d; stack free space: %d\n",
                  cnt++, GetFreeHeapSize(), TaskGetStackFreeSpace(THIS_TASK));

            if (cnt == 2)
            {
                  print("Try to allocate 60000 bytes...\n");
                  mem = (u8_t*)Malloc(60000);

                  if (mem == NULL)
                        print("Allocation failed :(\n");
            }
            else if (cnt == 4)
            {
                  if (mem)
                  {
                        print("Freed allocated memory...\n");
                        Free(mem);
                  }
            }
            else if (cnt == 6)
            {
                  clearSTDIN();

                  for (;;)
                  {
                        ch_t ch;

                        print("Exit success? [y/n]: ");
                        ch = getChar();
                        print("%c\n", ch);

                        if (ch == 'y')
                              Exit(STD_STATUS_OK);

                        print("Exit failure? [y/n]: ");
                        ch = getChar();
                        print("%c\n", ch);

                        if (ch == 'y')
                              Exit(STD_STATUS_ERROR);
                  }
            }

            Sleep(1000);
      }

      Exit(STD_STATUS_ERROR);
}


/*==================================================================================================
                                            End of file
==================================================================================================*/
