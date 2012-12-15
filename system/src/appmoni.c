/*=============================================================================================*//**
@file    appmoni.c

@author  Daniel Zorychta

@brief   This module is used to monitoring all applications

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

#ifdef __cplusplus
extern "C" {
#endif

/*==================================================================================================
                                            Include files
==================================================================================================*/
#include "appmoni.h"
#include "print.h"
#include "oswrap.h"


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
 * @brief Monitor memory allocation
 *
 * @param size          block size
 *
 * @return pointer to allocated block or NULL if error
 */
//================================================================================================//
#if (APP_MONITORING_ENABLE > 0)
void *moni_malloc(u32_t size)
{
      void *ptr = mm_malloc(size);

      kprint("%s: malloc: %u -> 0x%x\n", TaskGetName(NULL), size, ptr);

      return ptr;
}
#endif


//================================================================================================//
/**
 * @brief Monitor memory allocation
 *
 * @param nmemb         n members
 * @param msize         member size
 *
 * @return pointer to allocated block or NULL if error
 */
//================================================================================================//
#if (APP_MONITORING_ENABLE > 0)
void *moni_calloc(u32_t nmemb, u32_t msize)
{
      void *ptr = mm_calloc(nmemb, msize);

      kprint("%s: calloc: %u * %u -> 0x%x\n", TaskGetName(NULL), nmemb, msize, ptr);

      return ptr;
}
#endif


//================================================================================================//
/**
 * @brief Monitor memory freeing
 *
 * @param *mem          block to free
 */
//================================================================================================//
#if (APP_MONITORING_ENABLE > 0)
void moni_free(void *mem)
{
      mm_free(mem);

      kprint("%s: freeing: 0x%x\n", TaskGetName(NULL), mem);
}
#endif


//================================================================================================//
/**
 * @brief Function open selected file
 *
 * @param *name         file path
 * @param *mode         file mode
 *
 * @retval NULL if file can't be created
 */
//================================================================================================//
FILE_t *moni_fopen(const ch_t *path, const ch_t *mode)
{
      FILE_t *file = vfs_fopen(path, mode);

      return file;
}


//================================================================================================//
/**
 * @brief Function close opened file
 *
 * @param *file               pinter to file
 *
 * @retval STD_RET_OK         file closed successfully
 * @retval STD_RET_ERROR      file not closed
 */
//================================================================================================//
stdRet_t moni_fclose(FILE_t *file)
{
      stdRet_t status = vfs_fclose(file);

      return status;
}

#ifdef __cplusplus
}
#endif

/*==================================================================================================
                                            End of file
==================================================================================================*/
