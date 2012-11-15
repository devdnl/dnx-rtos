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
struct listitem {
      void  *data;
      u32_t  usrAttr;
      struct listitem *prev;
      struct listitem *next;
};

typedef struct list {
      struct listitem *firstitem;
      struct listitem *lastitem;
      i32_t  itemcount;
} list_t;


/*==================================================================================================
                                      Local function prototypes
==================================================================================================*/
static struct listitem *GetItemAddr(list_t *list, i32_t nitem);


/*==================================================================================================
                                      Local object definitions
==================================================================================================*/


/*==================================================================================================
                                        Function definitions
==================================================================================================*/
//================================================================================================//
/**
 * @brief Function create new list object
 *
 * @return pointer to new list object
 */
//================================================================================================//
list_t *ListCreate(void)
{
      list_t *newlist = calloc(1, sizeof(list_t));

      return newlist;
}


//================================================================================================//
/**
 * @brief Free list object
 *
 * Function free list object and all data inserted in data pointer. If data inserted in data pointer
 * is not dynamically allocated, please set this value to NULL before use this function.
 *
 * @param *list         pointer to list
 */
//================================================================================================//
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


//================================================================================================//
/**
 * @brief Function add item to end of list
 *
 * @param *list         pointer to list
 *
 * @return number of item in list, if failure function return -1
 */
//================================================================================================//
i32_t ListAddItem(list_t *list)
{
      size_t n = -1;

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

            n = list->itemcount - 1;
      }

      return n;
}


//================================================================================================//
/**
 * @brief Function remove selected item
 *
 * @param *list         pointer to list
 * @param nitem         item number
 *
 * @return 0 if ok, otherwise != 0
 */
//================================================================================================//
size_t ListRmItem(list_t *list, i32_t nitem)
{
      size_t n = 1;

      if (list && (list->itemcount - 1 >= nitem)) {

            struct listitem *item = GetItemAddr(list, nitem);

            if (item) {
                  if (item->prev) {
                        item->prev->next = item->next;
                  } else {
                        list->firstitem = item->next;
                  }

                  if (item->next == NULL) {
                        list->lastitem = item->prev;
                  }

                  free(item->data);
                  free(item);

                  list->itemcount--;

                  n = 0;
            }
      }

      return n;
}


//================================================================================================//
/**
 * @brief Function set item data pointer
 * Function automatically free last data before use new data
 *
 * @param *list         pointer to list
 * @param nitem         item number
 * @param *ptr          data pointer
 */
//================================================================================================//
void ListSetItemData(list_t *list, i32_t nitem, void *ptr)
{
      if (list && (nitem > -1)) {

            struct listitem *item = GetItemAddr(list, nitem);

            if (item) {
                  free(item->data);
                  item->data = ptr;
            }
      }
}


//================================================================================================//
/**
 * @brief Function set item attribute
 *
 * @param *list         pointer to list
 * @param nitem         item number
 * @param attr          item attribute
 */
//================================================================================================//
void ListSetItemAttr(list_t *list, i32_t nitem, u32_t attr)
{
      if (list && (nitem > -1)) {

            struct listitem *item = GetItemAddr(list, nitem);

            if (item) {
                  item->usrAttr = attr;
            }
      }
}


//================================================================================================//
/**
 * @brief Function return item data pointer
 *
 * @param *list         pointer to list
 * @param nitem         item number
 *
 * @return pointer to item user data
 */
//================================================================================================//
void *ListGetItemData(list_t *list, i32_t nitem)
{
      void *usrdata = NULL;

      if (list && (nitem > -1)) {

            struct listitem *item = GetItemAddr(list, nitem);

            if (item) {
                  usrdata = item->data;
            }
      }

      return usrdata;
}


//================================================================================================//
/**
 * @brief Function return item's user attribute
 *
 * @param *list         pointer to list
 * @param nitem         item number
 *
 * @return item's user attribute
 */
//================================================================================================//
u32_t ListGetItemAttr(list_t *list, i32_t nitem)
{
      u32_t usrattr = 0;

      if (list && (nitem > -1)) {

            struct listitem *item = GetItemAddr(list, nitem);

            if (item) {
                  usrattr = item->usrAttr;
            }
      }

      return usrattr;
}


//================================================================================================//
/**
 * @brief Function return the number of list's item
 *
 * @param *list         pointer to list
 *
 * @return number of list's item, if error -1 is returned
 */
//================================================================================================//
i32_t ListGetItemCount(list_t *list)
{
      i32_t n = -1;

      if (list) {
            n = list->itemcount;
      }

      return n;
}


//================================================================================================//
/**
 * @brief Function find pointer to selected item
 *
 * @param *list         pointer to list
 * @param nitem         item number
 *
 * @return pointer to item
 */
//================================================================================================//
static struct listitem *GetItemAddr(list_t *list, i32_t nitem)
{
      struct listitem *item = list->firstitem;

      i32_t cnt = 0;

      while (cnt < nitem && item) {
            item = item->next;
            cnt++;
      }

      return item;
}


#ifdef __cplusplus
}
#endif

/*==================================================================================================
                                            End of file
==================================================================================================*/
