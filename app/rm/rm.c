/*=============================================================================================*//**
@file    rm.c

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

/* Begin of application section declaration */
APPLICATION(rm)
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
      stdRet_t status = STD_RET_ERROR;

      if (argv)
      {
            /* parse directory and file name */
            ch_t *filename = strchr(argv + 1, '/');

            if (filename)
            {
                  *filename++ = '\0';

                  ch_t *dirname = argv;

                  DIR_t *dir = opendir(dirname);

                  if (dir)
                  {
                        dirent_t diren;

                        while ((diren = readdir(dir)).name != NULL)
                        {
                              if (strcmp(diren.name, filename) == 0)
                              {
                                    status = remove(&diren);
                                    break;
                              }
                        }

                        closedir(dir);
                  }
            }

            if (status != STD_RET_OK)
                  printf("Cannot remove specified file.\n");
      }
      else
      {
            printf("Enter correct filename.\n");
      }


      return status;
}

/* End of application section declaration */
APP_SEC_END

/*==================================================================================================
                                            End of file
==================================================================================================*/
