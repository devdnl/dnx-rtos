/*=============================================================================================*//**
@file    ls.c

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
#include "ls.h"
#include "regapp.h"
#include <string.h>

/* Begin of application section declaration */
APPLICATION(ls)
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
      const ch_t fileType[] = {'-', 'd', 'c', 'l'};

      stdRet_t status = STD_RET_ERROR;

      DIR_t *dir = opendir(argv);

      if (dir)
      {
            dirent_t dirent;

            ch_t *fcolor = "\x1B[36m";
            ch_t *dcolor = "\x1B[33m";

            printf("Total %u\n", dir->items);

            while ((dirent = readdir(dir)).name != NULL)
            {
                  printf("%s%c %u\t%s\x1B[0m\n",
                         dirent.filetype != FILE_TYPE_DIR? fcolor : dcolor,
                         fileType[dirent.filetype],
                         dirent.size,
                         dirent.name);
            }

            closedir(dir);

            status = STD_RET_OK;
      }
      else
      {
            printf("No such directory\n");
      }

      return status;
}

/* End of application section declaration */
APP_SEC_END

/*==================================================================================================
                                            End of file
==================================================================================================*/
