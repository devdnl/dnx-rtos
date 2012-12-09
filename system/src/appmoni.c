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
#include "memman.h"
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
 */
//================================================================================================//
void *moni_malloc(u32_t size)
{
      void *ptr = mm_malloc(size);

      kprint("%s: malloc: %u -> 0x%x\n", TaskGetName(NULL), size, ptr);

      return ptr;
}


//================================================================================================//
/**
 * @brief Monitor memory allocation
 */
//================================================================================================//
void *moni_calloc(u32_t nmemb, u32_t msize)
{
      void *ptr = mm_calloc(nmemb, msize);

      kprint("%s: calloc: %u * %u -> 0x%x\n", TaskGetName(NULL), nmemb, msize, ptr);

      return ptr;
}


//================================================================================================//
/**
 * @brief Monitor memory freeing
 */
//================================================================================================//
void moni_free(void *mem)
{
      mm_free(mem);

      kprint("%s: freeing: 0x%x\n", TaskGetName(NULL), mem);
}


#ifdef __cplusplus
}
#endif

/*==================================================================================================
                                            End of file
==================================================================================================*/
