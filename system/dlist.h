#ifndef _DLIST_H_
#define _DLIST_H_
/*=============================================================================================*//**
@file    dlist.h

@author  Daniel Zorychta

@brief   This file support lists

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
#include "basic_types.h"


/*==================================================================================================
                                 Exported symbolic constants/macros
==================================================================================================*/
typedef struct list list_t;


/*==================================================================================================
                                  Exported types, enums definitions
==================================================================================================*/


/*==================================================================================================
                                     Exported function prototypes
==================================================================================================*/
extern list_t *ListCreate(void);
extern void    ListFree(list_t *list);
extern i32_t   ListAddItem(list_t *list);
extern size_t  ListRmItem(list_t *list, i32_t nitem);
extern void    ListSetItemDataPtr(list_t *list, i32_t nitem, void *ptr);
extern void   *ListGetItemDataPtr(list_t *list, i32_t nitem);
extern i32_t   ListGetItemCount(list_t *list);


#ifdef __cplusplus
}
#endif

#endif /* _DLIST_H_ */
/*==================================================================================================
                                            End of file
==================================================================================================*/
