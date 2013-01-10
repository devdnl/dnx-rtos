#ifndef _DLIST_H_
#define _DLIST_H_
/*=============================================================================================*//**
@file    dlist.h

@author  Daniel Zorychta

@brief   This file support dynamically lists

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
#include "memman.h"


/*==================================================================================================
                                 Exported symbolic constants/macros
==================================================================================================*/
/* USER CFG: memory management */
#define DLIST_CALLOC(nmemb, msize)        memman_calloc(nmemb, msize)
#define DLIST_MALLOC(size)                memman_malloc(size)
#define DLIST_FREE(mem)                   memman_free(mem)


/*==================================================================================================
                                  Exported types, enums definitions
==================================================================================================*/
typedef struct list list_t;


/*==================================================================================================
                                     Exported function prototypes
==================================================================================================*/
extern list_t *ListCreate(void);
extern void    ListDelete(list_t *list);
extern i32_t   ListAddItem(list_t *list, u32_t id, void *data);
extern i32_t   ListInsItemBeforeNo(list_t *list, i32_t nitem, u32_t id, void *data);
extern i32_t   ListInsItemAfterNo(list_t *list, i32_t nitem, u32_t id, void *data);
extern i32_t   ListRmItemByNo(list_t *list, i32_t nitem);
extern i32_t   ListRmItemByID(list_t *list, u32_t id);
extern void    ListClear(list_t *list);
extern i32_t   ListSetItemDataByNo(list_t *list, i32_t nitem, void *data);
extern void   *ListGetItemDataByNo(list_t *list, i32_t nitem);
extern i32_t   ListSetItemDataByID(list_t *list, u32_t id, void *data);
extern void   *ListGetItemDataByID(list_t *list, u32_t id);
extern i32_t   ListUnlinkItemDataByNo(list_t *list, i32_t nitem);
extern i32_t   ListUnlinkItemDataByID(list_t *list, u32_t id);
extern i32_t   ListGetItemID(list_t *list, i32_t nitem, u32_t *itemid);
extern i32_t   ListGetItemNo(list_t *list, u32_t id, i32_t *nitem);
extern i32_t   ListGetItemCount(list_t *list);


#ifdef __cplusplus
}
#endif

#endif /* _DLIST_H_ */
/*==================================================================================================
                                            End of file
==================================================================================================*/
