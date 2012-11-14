/*=============================================================================================*//**
@file    dlist.c

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
#include "dlist.h"
#include "system.h"


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
                                        Function definitions
==================================================================================================*/
list_t *ListCreate(void)
{
      list_t *newlist = calloc(1, sizeof(list_t));

      return newlist;
}


void ListFree(list_t *list)
{
      if (list) {
            struct listitem *item     = list->firstitem;
            struct listitem *nextitem = NULL;

            while (item) {
                  nextitem = item->next;

                  free(item->data);
                  free(item);

                  item = nextitem;
            }

            free(list);
      }
}


size_t ListAddItem(list_t *list)
{
      struct listitem *newitem = calloc(1, sizeof(struct listitem));

      if (newitem) {
            if (list->firstitem == NULL) {
                  list->firstitem = newitem;
                  list->lastitem  = newitem;
            } else {
                  list->lastitem->next = newitem;
                  list->lastitem = newitem;
            }

            list->itemcount++;
      }
}


void ListRmItem(list_t *list, size_t number)
{

}


void ListSetItemData(list_t *list, void *ptr)
{

}


void ListSetItemAttr(list_t *list, u32_t attr)
{

}


void *ListGetItemData(list_t *list, size_t number)
{

}


u32_t *ListGetItemAttr(list_t *list, size_t number)
{

}


size_t ListGetItemCount(list_t *list)
{

}


#ifdef __cplusplus
}
#endif

/*==================================================================================================
                                            End of file
==================================================================================================*/
