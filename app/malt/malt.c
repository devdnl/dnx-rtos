/*=============================================================================================*//**
@file    malt.c

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
#include "malt.h"
#include <string.h>

#include "utils.h"

/* Begin of application section declaration */
APPLICATION(malt)
APP_SEC_BEGIN

/*==================================================================================================
                                  Local symbolic constants/macros
==================================================================================================*/


/*==================================================================================================
                                   Local types, enums definitions
==================================================================================================*/
typedef struct testptr
{
      u8_t *ptr;
      u16_t size;
      u8_t  patt;
} test_t;


/*==================================================================================================
                                      Local object definitions
==================================================================================================*/


/*==================================================================================================
                                        Function definitions
==================================================================================================*/

u16_t GetRandom(void)
{
      u16_t rand = 0;

      while ((rand = UTL_GetRandom() & 0x1FF) == 0);

      return rand & 0x1FF;
}


//================================================================================================//
/**
 * @brief clear main function
 */
//================================================================================================//
stdRet_t appmain(ch_t *argv)
{
      print("Program used to test malloc() and free() functions in random size block.\n");
//      print("HEAP address: %x - %x\n", GetHeapBegin(), GetHeapEnd());
      print("Press (s) to start or (e) to exit.\n");

      while (TRUE)
      {
            ch_t ch = getChar();

            if (ch == 'e')
            {
                  break;
            }

            if (ch == 's')
            {
                  u32_t count = 0;

                  while (ugetChar() != 'e')
                  {
                        test_t test[64];

                        /* alloc */
                        for (u8_t i = 0; i < ARRAY_SIZE(test); i++)
                        {
                              test[i].size = GetRandom();
                              test[i].ptr  = Malloc(test[i].size);

                              if (test[i].ptr == NULL)
                              {
                                    fontYellow();
                                    print("\nptr%d error: NULL; alloc size %d; free mem %d",
                                          i, test[i].size, SystemGetFreeMemSize());
                                    resetAttr();
                              }
//                              else if (test[i].ptr < (u8_t*)GetHeapBegin() && test[i].ptr > (u8_t*)GetHeapEnd())
//                              {
//                                    fontRed();
//                                    print("\nptr%d error: out of range %x", i, test[i]);
//                                    resetAttr();
//                              }
                        }

                        /* pattern write */
                        for (u8_t i = 0; i < ARRAY_SIZE(test); i++)
                        {
                              if (test[i].ptr != NULL && test[i].size != 0)
                              {
                                    test[i].patt = (u8_t)GetRandom();
                                    memset(test[i].ptr, test[i].patt, test[i].size);
                              }
                        }

                        /* pattern test */
                        for (u8_t i = 0; i < ARRAY_SIZE(test); i++)
                        {
                              u8_t *ptr = test[i].ptr;

                              while (test[i].size-- && test[i].ptr)
                              {
                                    if (*(ptr++) != test[i].patt)
                                    {
                                          fontBlue();
                                          print("\nptr%d error: bad pattern", i);
                                          resetAttr();

                                          break;
                                    }
                              }
                        }

                        /* free */
                        for (u8_t i = 0; i < ARRAY_SIZE(test); i++)
                        {
                              if (test[i].ptr)
                              {
                                    Free(test[i].ptr);
                              }
                        }

                        /* progress */
                        if (count % 1000 == 0)
                        {
                              print("\r%d tests", count / 1000);
                        }

                        count += sizeof(test);
                  }

                  break;
            }
      }

      if (ParseArg(argv, "stack", PARSE_AS_EXIST, NULL) == STD_RET_OK)
      {
            print("\nFree stack: %d levels", SystemGetStackFreeSpace());
      }

      print("\nExiting...\n");

      return STD_RET_OK;
}

/* End of application section declaration */
APP_SEC_END

/*==================================================================================================
                                            End of file
==================================================================================================*/
