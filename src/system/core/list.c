/*=========================================================================*//**
@file    list.c

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


*//*==========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Include files
==============================================================================*/
#include "core/list.h"
#include "core/sysmoni.h"

/*==============================================================================
  Local symbolic constants/macros
==============================================================================*/
#define calloc(nmemb, msize)              sysm_syscalloc(nmemb, msize)
#define malloc(size)                      sysm_sysmalloc(size)
#define free(mem)                         sysm_sysfree(mem)

/*==============================================================================
  Local types, enums definitions
==============================================================================*/
struct list_item {
        void  *data;
        u32_t  id;
        struct list_item *next;
};

struct list {
        struct list_item *head;
        struct list_item *tail;
        i32_t  item_count;
};

/*==============================================================================
  Local function prototypes
==============================================================================*/
static void  get_iaddr_by_No(list_t *list, i32_t nitem, struct list_item **previtem,
                             struct list_item **thisitem);
static i32_t get_iaddr_by_ID(list_t *list, u32_t id, struct list_item **previtem,
                             struct list_item **thisitem);

/*==============================================================================
  Local object definitions
==============================================================================*/

/*==============================================================================
  Function definitions
==============================================================================*/
//==============================================================================
/**
 * @brief Function create new list object
 *
 * @return pointer to new list object
 */
//==============================================================================
list_t *list_new(void)
{
      return calloc(1, sizeof(list_t));
}

//==============================================================================
/**
 * @brief Delete list object
 *
 * Function free list object and all data inserted in data pointer. If data
 * inserted in data pointer is not dynamically allocated, please set this value
 * to NULL before use this function.
 *
 * @param *list         pointer to list
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
stdret_t list_delete(list_t *list)
{
        if (list) {
                struct list_item *item      = list->head;
                struct list_item *next_item = NULL;

                while (item) {
                        next_item = item->next;

                        if (item->data) {
                                free(item->data);
                        }

                        free(item);

                        item = next_item;
                }

                free(list);

                return STD_RET_OK;
        }

        return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief Function add item to end of list
 *
 * @param *list         pointer to list
 * @param  id           item ID
 * @param *data         data pointer
 *
 * @return number of item in list, if failure function return -1
 */
//==============================================================================
i32_t list_add_item(list_t *list, u32_t id, void *data)
{
        if (list) {
                struct list_item *new_item = calloc(1, sizeof(struct list_item));

                if (new_item) {
                        if (list->head == NULL) {
                                list->head = new_item;
                                list->tail = new_item;
                        } else {
                                list->tail->next = new_item;
                                list->tail = new_item;
                        }

                        list->item_count++;

                        new_item->data = data;
                        new_item->id   = id;

                        return list->item_count - 1;
                }
        }

        return -1;
}

//==============================================================================
/**
 * @brief Function insert new item before selected place
 *
 * @param *list         pointer to list
 * @param  id           item ID
 * @param *data         data pointer
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
stdret_t list_insert_item_before_n(list_t *list, i32_t nitem, u32_t id,  void *data)
{
        if (list) {
                struct list_item *new_item = calloc(1, sizeof(struct list_item));

                if (new_item) {
                        struct list_item *item;
                        struct list_item *prev_item;
                        get_iaddr_by_No(list, nitem, &prev_item, &item);

                        if (item) {
                                /* check head pointer */
                                if (nitem == 0) {
                                        new_item->next = list->head;
                                        list->head    = new_item;
                                } else {
                                        prev_item->next = new_item;
                                        new_item->next  = item;
                                }

                                list->item_count++;

                                /* check tail pointer */
                                if (nitem == list->item_count - 1) {
                                        list->tail = new_item;
                                }

                                new_item->data = data;
                                new_item->id   = id;

                                return STD_RET_OK;
                        }
                }
        }

        return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief Function insert new item after selected place
 *
 * @param *list         pointer to list
 * @param  id           item ID
 * @param *data         data pointer
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
stdret_t list_insert_item_after_n(list_t *list, i32_t nitem, u32_t id, void *data)
{
        if (list) {
                struct list_item *new_item = calloc(1, sizeof(struct list_item));

                if (new_item) {
                        struct list_item *item;
                        struct list_item *prev_item;
                        get_iaddr_by_No(list, nitem, &prev_item, &item);

                        if (item) {
                                new_item->next = item->next;
                                item->next    = new_item;

                                list->item_count++;

                                /* check tail pointer */
                                if (list->tail == item) {
                                        list->tail = new_item;
                                }

                                new_item->data = data;
                                new_item->id   = id;

                                return STD_RET_OK;
                        }
                }
        }

        return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief Function remove selected n item
 *
 * @param *list         pointer to list
 * @param nitem         item number
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
stdret_t list_rm_nitem(list_t *list, i32_t nitem)
{
        if (list) {
                if ((nitem >= 0) && (list->item_count - 1 >= nitem)) {
                        struct list_item *item;
                        struct list_item *prev_item;
                        get_iaddr_by_No(list, nitem, &prev_item, &item);

                        if (item) {
                                /* check head pointer */
                                if (nitem == 0) {
                                        list->head = item->next;
                                }

                                /* check tail pointer */
                                if (list->item_count - 1 == nitem) {
                                        list->tail = prev_item;
                                }

                                /* connect to previous item the next item */
                                if (prev_item) {
                                        prev_item->next = item->next;
                                }

                                if (item->data) {
                                        free(item->data);
                                }

                                free(item);

                                list->item_count--;

                                return STD_RET_OK;
                        }
                }
        }

        return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief Function remove selected item by ID
 *
 * @param *list         pointer to list
 * @param  id           item ID
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
stdret_t list_rm_iditem(list_t *list, u32_t id)
{
        if (list) {
                struct list_item *item;
                struct list_item *prev_item;
                i32_t nitem = get_iaddr_by_ID(list, id, &prev_item, &item);

                if (item) {
                        /* check head pointer */
                        if (nitem == 0) {
                                list->head = item->next;
                        }

                        /* check tail pointer */
                        if (list->item_count - 1 == nitem) {
                                list->tail = prev_item;
                        }

                        /* connect to previous item next item from current item */
                        if (prev_item) {
                                prev_item->next = item->next;
                        }

                        if (item->data) {
                                free(item->data);
                        }

                        free(item);

                        list->item_count--;

                        return STD_RET_OK;
                }
        }

        return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief Function set item data pointer
 * Function automatically free last data before use new data
 *
 * @param *list         pointer to list
 * @param nitem         item number
 * @param *data         data pointer
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
stdret_t list_set_nitem_data(list_t *list, i32_t nitem, void *data)
{
        if (list && (nitem >= 0)) {

                struct list_item *item;
                get_iaddr_by_No(list, nitem, NULL, &item);

                if (item) {
                        if (item->data) {
                                free(item->data);
                        }

                        item->data = data;

                        return STD_RET_OK;
                }
        }

        return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief Function return item data pointer
 *
 * @param *list         pointer to list
 * @param  nitem        item number
 *
 * @return pointer to item user data
 */
//==============================================================================
void *list_get_nitem_data(list_t *list, i32_t nitem)
{
        if (list && (nitem >= 0)) {
                struct list_item *item;
                get_iaddr_by_No(list, nitem, NULL, &item);

                if (item) {
                        return item->data;
                }
        }

        return NULL;
}

//==============================================================================
/**
 * @brief Function set item data pointer by id
 * Function automatically free last data before use new data
 *
 * @param *list         pointer to list
 * @param nitem         item number
 * @param *data         data pointer
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
stdret_t list_set_iditem_data(list_t *list, u32_t id, void *data)
{
        if (list) {
                struct list_item *item;
                get_iaddr_by_ID(list, id, NULL, &item);

                if (item) {
                        if (item->data) {
                                free(item->data);
                        }

                        item->data = data;

                        return STD_RET_OK;
                }
        }

        return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief Function return item data pointer by ID
 *
 * @param *list         pointer to list
 * @param  id           item ID
 *
 * @return pointer to item user data
 */
//==============================================================================
void *list_get_iditem_data(list_t *list, u32_t id)
{
        if (list) {
                struct list_item *item;
                get_iaddr_by_ID(list, id, NULL, &item);

                if (item) {
                        return item->data;
                }
        }

        return NULL;
}

//==============================================================================
/**
 * @brief Function return item ID number
 *
 * @param *list         pointer to list
 * @param  nitem        item number
 * @param *itemid       item id
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
stdret_t list_get_nitem_ID(list_t *list, i32_t nitem, u32_t *itemid)
{
        if (list && nitem >= 0 && itemid) {
                struct list_item *item;
                get_iaddr_by_No(list, nitem, NULL, &item);

                if (item) {
                        *itemid = item->id;
                        return STD_RET_OK;
                }
        }

        return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief Function return item number
 *
 * Function returns -1 in nitem pointer if item does not exist.
 *
 * @param *list         pointer to list
 * @param  id           item ID
 * @param *nitem        item number
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
stdret_t list_get_iditem_No(list_t *list, u32_t id, i32_t *nitem)
{
        if (list && nitem) {
                struct list_item *item;
                i32_t itemno = get_iaddr_by_ID(list, id, NULL, &item);

                if (item) {
                        *nitem = itemno;
                } else {
                        *nitem = -1;
                }

                return STD_RET_OK;
        }

        return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief Function unlink data form item selected by id
 * Function automatically free last data before use new data
 *
 * @param *list         pointer to list
 * @param  nitem        item number
 * @param *data         data pointer
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
stdret_t list_unlink_nitem_data(list_t *list, i32_t nitem)
{
        if (list) {
                struct list_item *item;
                get_iaddr_by_No(list, nitem, NULL, &item);

                if (item) {
                        item->data = NULL;

                        return STD_RET_OK;
                }
        }

        return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief Function unlink data form item selected by id
 * Function automatically free last data before use new data
 *
 * @param *list         pointer to list
 * @param  id           item id
 * @param *data         data pointer
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
stdret_t list_unlink_iditem_data(list_t *list, u32_t id)
{
        if (list) {
                struct list_item *item;
                get_iaddr_by_ID(list, id, NULL, &item);

                if (item) {
                        item->data = NULL;

                        return STD_RET_OK;
                }
        }

        return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief Function return the number of list's item
 *
 * @param *list         pointer to list
 *
 * @return number of list's item, if error -1 is returned
 */
//==============================================================================
i32_t list_get_item_count(list_t *list)
{
        if (list) {
                return list->item_count;
        }

        return -1;
}

//==============================================================================
/**
 * @brief Function find pointer to selected item
 *
 * @param *list         pointer to list
 * @param *previtem     pointer to previous item
 * @param *thisitem     pointer to found item
 * @param  nitem        item number
 */
//==============================================================================
static void get_iaddr_by_No(list_t *list, i32_t nitem, struct list_item **previtem,
                            struct list_item **thisitem)
{
        struct list_item *titem = list->head;
        struct list_item *pitem = NULL;

        i32_t cnt = 0;

        while (cnt < nitem && titem) {
                pitem = titem;
                titem = titem->next;
                cnt++;
        }

        if (thisitem) {
                *thisitem = titem;
        }

        if (previtem) {
                *previtem = pitem;
        }
}

//==============================================================================
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
//==============================================================================
static i32_t get_iaddr_by_ID(list_t *list, u32_t id, struct list_item **previtem,
                             struct list_item **thisitem)
{
        struct list_item *titem = list->head;
        struct list_item *pitem = NULL;

        i32_t cnt = 0;

        while (cnt < list->item_count && titem && id != titem->id) {
                pitem = titem;
                titem = titem->next;
                cnt++;
        }

        if (thisitem) {
                *thisitem = titem;
        }

        if (previtem) {
                *previtem = pitem;
        }

        return cnt;
}

#ifdef __cplusplus
}
#endif

/*==============================================================================
  End of file
==============================================================================*/
