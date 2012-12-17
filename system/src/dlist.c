/*=============================================================================================*//**
@file    dlist.c

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
#include "dlist.h"


/*==================================================================================================
                                  Local symbolic constants/macros
==================================================================================================*/
#define calloc(nmemb, msize)              DLIST_CALLOC(nmemb, msize)
#define malloc(size)                      DLIST_MALLOC(size)
#define free(mem)                         DLIST_FREE(mem)


/*==================================================================================================
                                   Local types, enums definitions
==================================================================================================*/
struct listitem {
      void  *data;
      u32_t  id;
      struct listitem *next;
};

typedef struct list {
      struct listitem *head;
      struct listitem *tail;
      i32_t  itemcount;
} list_t;


/*==================================================================================================
                                      Local function prototypes
==================================================================================================*/
static void  GetItemAddrByNo(list_t *list, i32_t nitem, struct listitem **previtem, struct listitem **thisitem);
static i32_t GetItemAddrByID(list_t *list, u32_t id, struct listitem **previtem, struct listitem **thisitem);


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
void ListDelete(list_t *list)
{
      if (list) {
            struct listitem *item     = list->head;
            struct listitem *nextitem = NULL;

            while (item) {
                  nextitem = item->next;

                  if (item->data)
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
 * @param  id           item ID
 * @param *data         data pointer
 *
 * @return number of item in list, if failure function return -1
 */
//================================================================================================//
i32_t ListAddItem(list_t *list, u32_t id, void *data)
{
      i32_t n = -1;

      if (list) {
            struct listitem *newitem = calloc(1, sizeof(struct listitem));

            if (newitem) {
                  if (list->head == NULL) {
                        list->head = newitem;
                        list->tail = newitem;
                  } else {
                        list->tail->next = newitem;
                        list->tail = newitem;
                  }

                  list->itemcount++;

                  newitem->data = data;
                  newitem->id   = id;

                  n = list->itemcount - 1;
            }
      }

      return n;
}


//================================================================================================//
/**
 * @brief Function insert new item before selected place
 *
 * @param *list         pointer to list
 * @param  id           item ID
 * @param *data         data pointer
 *
 * @return 0 if ok, otherwise != 0
 */
//================================================================================================//
i32_t ListInsItemBeforeNo(list_t *list, i32_t nitem, u32_t id,  void *data)
{
      i32_t n = 1;

      if (list) {
            struct listitem *newitem = calloc(1, sizeof(struct listitem));

            if (newitem) {
                  struct listitem *item;
                  struct listitem *previtem;
                  GetItemAddrByNo(list, nitem, &previtem, &item);

                  if (item) {
                        /* check head pointer */
                        if (nitem == 0) {
                              newitem->next = list->head;
                              list->head    = newitem;
                        } else {
                              previtem->next = newitem;
                              newitem->next  = item;
                        }

                        list->itemcount++;

                        /* check tail pointer */
                        if (nitem == list->itemcount - 1) {
                              list->tail = newitem;
                        }

                        newitem->data = data;
                        newitem->id   = id;

                        n = 0;
                  }
            }
      }

      return n;
}


//================================================================================================//
/**
 * @brief Function insert new item after selected place
 *
 * @param *list         pointer to list
 * @param  id           item ID
 * @param *data         data pointer
 *
 * @return 0 if ok, otherwise != 0
 */
//================================================================================================//
i32_t ListInsItemAfterNo(list_t *list, i32_t nitem, u32_t id, void *data)
{
      i32_t n = 1;

      if (list) {
            struct listitem *newitem = calloc(1, sizeof(struct listitem));

            if (newitem) {
                  struct listitem *item;
                  struct listitem *previtem;
                  GetItemAddrByNo(list, nitem, &previtem, &item);

                  if (item) {
                        newitem->next = item->next;
                        item->next    = newitem;

                        list->itemcount++;

                        /* check tail pointer */
                        if (list->tail == item) {
                              list->tail = newitem;
                        }

                        newitem->data = data;
                        newitem->id   = id;

                        n = 0;
                  }
            }
      }

      return n;
}


//================================================================================================//
/**
 * @brief Function remove selected n item
 *
 * @param *list         pointer to list
 * @param nitem         item number
 *
 * @return 0 if ok, otherwise != 0
 */
//================================================================================================//
i32_t ListRmItemByNo(list_t *list, i32_t nitem)
{
      i32_t n = 1;

      if (list) {
            if ((nitem >= 0) && (list->itemcount - 1 >= nitem)) {
                  struct listitem *item;
                  struct listitem *previtem;
                  GetItemAddrByNo(list, nitem, &previtem, &item);

                  if (item) {
                        /* check head pointer */
                        if (nitem == 0) {
                              list->head = item->next;
                        }

                        /* check tail pointer */
                        if (list->itemcount - 1 == nitem) {
                              list->tail = previtem;
                        }

                        /* connect to previous item next item from current item */
                        if (previtem) {
                              previtem->next = item->next;
                        }

                        if (item->data)
                              free(item->data);

                        free(item);

                        list->itemcount--;

                        n = 0;
                  }
            }
      }

      return n;
}


//================================================================================================//
/**
 * @brief Function remove selected item by ID
 *
 * @param *list         pointer to list
 * @param  id           item ID
 *
 * @return 0 if ok, otherwise != 0
 */
//================================================================================================//
i32_t ListRmItemByID(list_t *list, u32_t id)
{
      i32_t n = 1;

      if (list) {
            struct listitem *item;
            struct listitem *previtem;
            i32_t nitem = GetItemAddrByID(list, id, &previtem, &item);

            if (item) {
                  /* check head pointer */
                  if (nitem == 0) {
                        list->head = item->next;
                  }

                  /* check tail pointer */
                  if (list->itemcount - 1 == nitem) {
                        list->tail = previtem;
                  }

                  /* connect to previous item next item from current item */
                  if (previtem) {
                        previtem->next = item->next;
                  }

                  if (item->data)
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
 * @brief Clear list
 *
 * Function free all data inserted in items. If data inserted in data pointer is not dynamically
 * allocated, please set this value to NULL before use this function.
 *
 * @param *list         pointer to list
 */
//================================================================================================//
void ListClear(list_t *list)
{
      if (list) {
            struct listitem *item     = list->head;
            struct listitem *nextitem = NULL;

            while (item) {
                  nextitem = item->next;

                  if (item->data)
                        free(item->data);

                  free(item);

                  item = nextitem;
            }

            list->head = NULL;
            list->tail = NULL;
            list->itemcount = 0;
      }
}


//================================================================================================//
/**
 * @brief Function set item data pointer
 * Function automatically free last data before use new data
 *
 * @param *list         pointer to list
 * @param nitem         item number
 * @param *data         data pointer
 *
 * @return 0 if ok, otherwise != 0
 */
//================================================================================================//
i32_t ListSetItemDataByNo(list_t *list, i32_t nitem, void *data)
{
      i32_t n = 1;

      if (list && (nitem >= 0)) {

            struct listitem *item;
            GetItemAddrByNo(list, nitem, NULL, &item);

            if (item) {
                  if (item->data)
                        free(item->data);

                  item->data = data;

                  n = 0;
            }
      }

      return n;
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
void *ListGetItemDataByNo(list_t *list, i32_t nitem)
{
      void *data = NULL;

      if (list && (nitem >= 0)) {
            struct listitem *item;
            GetItemAddrByNo(list, nitem, NULL, &item);

            if (item) {
                  data = item->data;
            }
      }

      return data;
}


//================================================================================================//
/**
 * @brief Function set item data pointer by id
 * Function automatically free last data before use new data
 *
 * @param *list         pointer to list
 * @param nitem         item number
 * @param *data         data pointer
 *
 * @return 0 if ok, otherwise != 0
 */
//================================================================================================//
i32_t ListSetItemDataByID(list_t *list, u32_t id, void *data)
{
      i32_t n = 1;

      if (list) {

            struct listitem *item;
            GetItemAddrByID(list, id, NULL, &item);

            if (item) {
                  if (item->data)
                        free(item->data);

                  item->data = data;

                  n = 0;
            }
      }

      return n;
}


//================================================================================================//
/**
 * @brief Function return item data pointer by ID
 *
 * @param *list         pointer to list
 * @param nitem         item number
 *
 * @return pointer to item user data
 */
//================================================================================================//
void *ListGetItemDataByID(list_t *list, u32_t id)
{
      void *data = NULL;

      if (list) {
            struct listitem *item;
            GetItemAddrByID(list, id, NULL, &item);

            if (item) {
                  data = item->data;
            }
      }

      return data;
}


//================================================================================================//
/**
 * @brief Function return item ID number
 *
 * @param *list         pointer to list
 * @param  nitem        item number
 * @param *itemid       item id
 *
 * @return 0 if ok, otherwise != 0
 */
//================================================================================================//
i32_t ListGetItemID(list_t *list, i32_t nitem, u32_t *itemid)
{
      i32_t n = 1;

      if (list && nitem >= 0 && itemid) {
            struct listitem *item;
            GetItemAddrByNo(list, nitem, NULL, &item);

            if (item) {
                  *itemid = item->id;
                  n = 0;
            }
      }

      return n;
}


//================================================================================================//
/**
 * @brief Function return item number
 *
 * @param *list         pointer to list
 * @param  id           item ID
 * @param *nitem        item number
 *
 * @return 0 if ok, otherwise != 0
 */
//================================================================================================//
i32_t ListGetItemNo(list_t *list, u32_t id, u32_t *nitem)
{
      i32_t n = 1;

      if (list && nitem) {
            struct listitem *item;
            i32_t itemno = GetItemAddrByID(list, id, NULL, &item);

            if (item) {
                  *nitem = itemno;
                  n = 0;
            }
      }

      return n;
}


//================================================================================================//
/**
 * @brief Function unlink data form item selected by id
 * Function automatically free last data before use new data
 *
 * @param *list         pointer to list
 * @param  nitem        item number
 * @param *data         data pointer
 *
 * @return 0 if ok, otherwise != 0
 */
//================================================================================================//
i32_t ListUnlinkItemDataByNo(list_t *list, i32_t nitem)
{
      i32_t n = 1;

      if (list) {

            struct listitem *item;
            GetItemAddrByNo(list, nitem, NULL, &item);

            if (item) {
                  item->data = NULL;

                  n = 0;
            }
      }

      return n;
}


//================================================================================================//
/**
 * @brief Function unlink data form item selected by id
 * Function automatically free last data before use new data
 *
 * @param *list         pointer to list
 * @param  id           item id
 * @param *data         data pointer
 *
 * @return 0 if ok, otherwise != 0
 */
//================================================================================================//
i32_t ListUnlinkItemDataByID(list_t *list, u32_t id)
{
      i32_t n = 1;

      if (list) {

            struct listitem *item;
            GetItemAddrByID(list, id, NULL, &item);

            if (item) {
                  item->data = NULL;

                  n = 0;
            }
      }

      return n;
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
 * @param *previtem     pointer to previous item
 * @param *thisitem     pointer to found item
 * @param  nitem        item number
 */
//================================================================================================//
static void GetItemAddrByNo(list_t *list, i32_t nitem, struct listitem **previtem, struct listitem **thisitem)
{
      struct listitem *titem = list->head;
      struct listitem *pitem = NULL;

      i32_t cnt = 0;

      while (cnt < nitem && titem) {
            pitem = titem;
            titem = titem->next;
            cnt++;
      }

      if (thisitem)
            *thisitem = titem;

      if (previtem)
            *previtem = pitem;
}


//================================================================================================//
/**
 * @brief Function find pointer to selected item's id
 *
 * @param *list         pointer to list
 * @param  id           item id
 * @param *previtem     pointer to previous item
 * @param *thisitem     pointer to found item
 *
 * @return item number
 */
//================================================================================================//
static i32_t GetItemAddrByID(list_t *list, u32_t id, struct listitem **previtem, struct listitem **thisitem)
{
      struct listitem *titem = list->head;
      struct listitem *pitem = NULL;

      i32_t cnt = 0;

      while (cnt < list->itemcount && titem && id != titem->id) {
            pitem = titem;
            titem = titem->next;
            cnt++;
      }

      if (thisitem)
            *thisitem = titem;

      if (previtem)
            *previtem = pitem;

      return cnt;
}


#ifdef __cplusplus
}
#endif

/*==================================================================================================
                                            End of file
==================================================================================================*/
