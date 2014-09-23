/*=========================================================================*//**
@file    llist.h

@author  Kamil Zorychta
@author  Daniel Zorychta

@brief   C implementation of linked list

@note    Copyright (C) 2014 Kamil Zorychta <kamil.zorychta@gmail.com>
         Copyright (C) 2014 Daniel Zorychta <daniel.zorychta@gmail.com>

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

/*==============================================================================
  Include files
==============================================================================*/
#include "llist.h"
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

/*==============================================================================
  Local macros
==============================================================================*/
#ifndef const_cast
#define const_cast(t, v) (t)(v)
#endif

/*==============================================================================
  Local object types
==============================================================================*/
// list's item
typedef struct item {
        struct item *prev;
        struct item *next;
        void *data;
} item_t;

// list main object
struct llist {
        size_t   count;
        size_t   count_half;
        item_t  *head;
        item_t  *tail;
        item_t  *iterator;
        llist_t *self;
        uint32_t magic;
};

// iterator object
struct llist_iterator {
        llist_t           *list;
        item_t            *item;
        llist__iterator_t *self;
        uint32_t           magic;
};

/*==============================================================================
  Local function prototypes
==============================================================================*/
static bool  is_list_valid(llist_t *this);
static bool  is_iterator_valid(llist__iterator_t *this);
static int   insert_item(llist_t *this, int index, const void *data, int direction);
static void *at_item(llist_t *this, int index, int direction);
static int   remove_item(llist_t *this, int index, int direction, bool unlink);
static int   append_prepend(llist_t *this, const void *data, int direction);

/*==============================================================================
  Local objects
==============================================================================*/
static const uint32_t magic_number = 0x6D89B264;

/*==============================================================================
  Function definitions
==============================================================================*/

//==============================================================================
/**
 * @brief  List constructor
 * @param  None
 * @return On success list object is returned, otherwise NULL
 */
//==============================================================================
llist_t *llist__new(void)
{
        llist_t *this = calloc(1, sizeof(llist_t));
        if (this) {
                this->magic = magic_number;
                this->self  = this;
        }

        return this;
}


//==============================================================================
/**
 * @brief  List destructor
 * @param  this         list object
 * @return On success 1 is returned, otherwise 0
 */
//==============================================================================
int llist__delete(llist_t *this)
{
        if (is_list_valid(this)) {
                llist__clear(this);

                this->magic = 0;
                this->self  = NULL;

                free(this);

                return 1;
        }

        return 0;
}


//==============================================================================
/**
 * @brief  Append data to list
 * @param  this         list object
 * @param  data         data to append
 * @return On success 1 is returned, otherwise 0
 */
//==============================================================================
int llist__append(llist_t *this, const void *data)
{
        if (is_list_valid(this) && data) {
                return append_prepend(this, data, 1);
        } else {
                return 0;
        }
}


//==============================================================================
/**
 * @brief  Append data to list and allocate memory
 * @param  this         list object
 * @param  data         data source to append
 * @return Pointer to allocated memory or NULL on error
 */
//==============================================================================
void *llist__append_alloc(llist_t *this, const void *data, size_t size)
{
        if (is_list_valid(this) && data && size) {
                void *mem = malloc(size);
                if (mem) {
                        memcpy(mem, data, size);

                        if (append_prepend(this, mem, 1) >= 0) {
                                return mem;
                        } else {
                                free(mem);
                        }
                }
        }

        return NULL;
}


//==============================================================================
/**
 * @brief  Clear list
 * @param  this         list object
 * @return On success 1 is returned, otherwise 0
 */
//==============================================================================
int llist__clear(llist_t *this)
{
        if (is_list_valid(this)) {
                if (this->count == 0)
                        return 1;

                item_t *item = this->head;
                while (item != NULL) {
                        item_t *citem = item;
                        item = item->next;

                        if (citem->data) {
                                free(citem->data);
                        }

                        free(citem);
                }

                this->count      = 0;
                this->count_half = 0;
                this->head       = NULL;
                this->tail       = NULL;

                return 1;
        }

        return 0;
}


//==============================================================================
/**
 * @brief  Remove list item
 * @param  this         list object
 * @param  index        index to remove (negative values for backward indexing)
 * @return On success 1 is returned, otherwise 0
 */
//==============================================================================
int llist__remove(llist_t *this, int index)
{
        if (is_list_valid(this)) {
                if (abs(index) <= this->count_half) {
                        return remove_item(this, abs(index), (index < 0 ? -1 : 1), false);
                } else {
                        return remove_item(this, this->count - abs(index) - 1, (index < 0 ? 1 : -1), false);
                }
        }

        return 0;
}


//==============================================================================
/**
 * @brief  Remove list item without freeing item's data
 * @param  this         list object
 * @param  index        index to unlink (negative values for backward indexing)
 * @return On success 1 is returned, otherwise 0
 */
//==============================================================================
int llist__unlink(llist_t *this, int index)
{
        if (is_list_valid(this)) {
                if (abs(index) <= this->count_half) {
                        return remove_item(this, abs(index), (index < 0 ? -1 : 1), true);
                } else {
                        return remove_item(this, this->count - abs(index) - 1, (index < 0 ? 1 : -1), true);
                }
        }

        return 0;
}


//==============================================================================
/**
 * @brief  Insert an element to the list
 * @param  this         list object
 * @param  index        index to insert (negative values for backward indexing)
 * @return On success 1 is returned, otherwise 0
 */
//==============================================================================
int llist__insert(llist_t *this, int index, const void *data)
{
        if (is_list_valid(this) && data) {
                if (abs(index) <= this->count_half) {
                        return insert_item(this, abs(index), data, (index < 0 ? -1 : 1));
                } else {
                        return insert_item(this, this->count - abs(index), data, (index < 0 ? 1 : -1));
                }
        }

        return 0;
}


//==============================================================================
/**
 * @brief  Allocate memory and insert element to the list
 * @param  this         list object
 * @param  index        index to insert (negative values for backward indexing)
 * @param  data         data to insert
 * @param  size         size of data
 * @return Pointer to allocated memory or NULL on error
 */
//==============================================================================
void *llist__insert_alloc(llist_t *this, int index, const void *data, size_t size)
{
        if (is_list_valid(this) && data && size) {
                void *mem = malloc(size);
                if (mem) {
                        memcpy(mem, data, size);

                        if (llist__insert(this, index, mem) > 0) {
                                return mem;
                        } else {
                                free(mem);
                        }
                }
        }

        return NULL;
}


//==============================================================================
/**
 * @brief  Get element form list at selected index
 * @param  this         list object
 * @param  index        index (negative values for backward indexing)
 * @return Pointer to data
 */
//==============================================================================
void *llist__at(llist_t *this, int index)
{
        if (is_list_valid(this)) {
                if (abs(index) <= this->count_half) {
                        return at_item(this, index, (index < 0 ? -1 : 1));
                } else {
                        return at_item(this, this->count - index - 1, (index < 0 ? 1 : -1));
                }
        }

        return NULL;
}


//==============================================================================
/**
 * @brief  Get number of items of the list
 * @param  this         list object
 * @return On success number of items, on error -1
 */
//==============================================================================
int llist__count(llist_t *this)
{
        if (is_list_valid(this)) {
                return this->count;
        } else {
                return -1;
        }
}


//==============================================================================
/**
 * @brief  Find in the list that specified object exists
 * @param  this         list object
 * @param  data         data to find
 * @return Number of objects existing in the list
 */
//==============================================================================
int llist__contains(llist_t *this, const void *data)
{
        if (is_list_valid(this) && data) {
                int n = 0;

                for (item_t *item = this->head; item; item = item->next) {
                        if (item->data == data) {
                                n++;
                        }
                }

                return n;

        } else {
                return 0;
        }
}


//==============================================================================
/**
 * @brief  Create new iterator
 * @param  this         list object
 * @return On success returns iterator object, otherwise NULL
 */
//==============================================================================
llist__iterator_t *llist__new_iterator(llist_t *this)
{
        if (is_list_valid(this)) {
                llist__iterator_t *iterator = calloc(1, sizeof(llist__iterator_t));
                if (iterator) {
                        iterator->list  = this;
                        iterator->item  = this->head;
                        iterator->magic = magic_number;
                        iterator->self  = iterator;

                        return iterator;
                }
        }

        return NULL;
}


//==============================================================================
/**
 * @brief  Delete created iterator
 * @param  iterator     iterator object
 * @return On success 1 is returned, otherwise 0
 */
//==============================================================================
int llist__delete_iterator(llist__iterator_t *iterator)
{
        if (is_iterator_valid(iterator)) {
                if (is_list_valid(iterator->list)) {
                        iterator->magic = 0;
                        iterator->self  = NULL;
                        free(iterator);
                        return 1;
                }
        }

        return 0;
}


//==============================================================================
/**
 * @brief  Return first object from list by using iterator
 * @param  iterator     iterator object
 * @return Pointer to data object
 */
//==============================================================================
void *llist__first(llist__iterator_t *iterator)
{
        if (is_iterator_valid(iterator)) {
                if (is_list_valid(iterator->list)) {
                        iterator->item = iterator->list->head;

                        if (iterator->item) {
                                return iterator->item->data;
                        }
                }
        }

        return NULL;
}


//==============================================================================
/**
 * @brief  Return last object from list by using iterator
 * @param  iterator     iterator object
 * @return Pointer to data object
 */
//==============================================================================
void *llist__last(llist__iterator_t *iterator)
{
        if (is_iterator_valid(iterator)) {
                if (is_list_valid(iterator->list)) {
                        iterator->item = iterator->list->tail;

                        if (iterator->item) {
                                return iterator->item->data;
                        }
                }
        }

        return NULL;
}


//==============================================================================
/**
 * @brief  Return next data object from list by using iterator
 * @param  iterator     iterator object
 * @return Pointer to data object
 */
//==============================================================================
void *llist__iterator_next(llist__iterator_t *iterator)
{
        if (is_iterator_valid(iterator)) {
                if (is_list_valid(iterator->list) && iterator->item) {
                        iterator->item = iterator->item->next;

                        if (iterator->item) {
                                return iterator->item->data;
                        }
                }
        }

        return NULL;
}


//==============================================================================
/**
 * @brief  Return previous data object from list by using iterator
 * @param  iterator     iterator object
 * @return Pointer to data object
 */
//==============================================================================
void *llist__iterator_prev(llist__iterator_t *iterator)
{
        if (is_iterator_valid(iterator)) {
                if (is_list_valid(iterator->list) && iterator->item) {
                        iterator->item = iterator->item->prev;

                        if (iterator->item) {
                                return iterator->item->data;
                        }
                }
        }

        return NULL;
}


//==============================================================================
/**
 * @brief  Check if list object is valid
 * @param  this         list object to examine
 * @return If object is valid true is returned, otherwise false
 */
//==============================================================================
static bool is_list_valid(llist_t *this)
{
        if (this) {
                if (this->magic == magic_number && this->self == this) {
                        return true;
                }
        }

        return false;
}


//==============================================================================
/**
 * @brief  Check if iterator object is valid
 * @param  this         iterator object to examine
 * @return If object is valid true is returned, otherwise false
 */
//==============================================================================
static bool is_iterator_valid(llist__iterator_t *this)
{
        if (this) {
                if (this->magic == magic_number && this->self == this) {
                        return true;
                }
        }

        return false;
}


//==============================================================================
/**
 * @brief  Remove list item
 * @param  this         list object
 * @param  index        index of item to delete
 * @param  unlink       unlink data from list instead of remove
 * @param  direction    item find direction
 * @return On success 1 is returned, otherwise 0
 */
//==============================================================================
static int remove_item(llist_t *this, int index, int direction, bool unlink)
{
        if (this->count == 0 || index < 0 || index >= this->count) {
                return 0;
        }

        item_t *item = direction >= 0 ? this->head : this->tail;

        while (item != NULL) {
                if (index-- == 0) {
                        if (item->prev == NULL) {
                                this->head = item->next;
                        } else {
                                item->prev->next = item->next;
                        }

                        if (item->next == NULL) {
                                this->tail = item->prev;
                        } else {
                                item->next->prev = item->prev;
                        }

                        if (item->data && !unlink) {
                                free(item->data);
                        }

                        free(item);

                        this->count--;
                        this->count_half = this->count / 2;

                        return 1;
                }

                item = direction >= 0 ? item->next : item->prev;
        }

        return 0;
}


//==============================================================================
/**
 * @brief  Insert item to the list
 * @param  this         list object
 * @param  index        element index
 * @param  data         data inserted to the item
 * @param  direction    insert direction
 * @param  On success 1 is returned, otherwise 0
 */
//==============================================================================
static int insert_item(llist_t *this, int index, const void *data, int direction)
{
        if (index <= 0) {
                return append_prepend(this, data, (direction >= 0 ? -1 : 1));
        } else if (index >= this->count) {
                return 0;
        }

        item_t *new_item = calloc(1, sizeof(item_t));
        if (new_item) {
                new_item->data = const_cast(void*, data);

                index = direction >= 0 ? index : index - 1;

                item_t *item = direction >= 0 ? this->head : this->tail;
                if (item == NULL) {
                        this->head = new_item;
                        this->tail = new_item;
                        this->count++;
                        this->count_half = this->count / 2;

                        return 1;
                }

                while (item) {
                        if (index-- == 0) {
                                if (item->prev == NULL) {
                                        this->head = new_item;
                                } else {
                                        item->prev->next = new_item;
                                }

                                new_item->prev = item->prev;
                                new_item->next = item;
                                item->prev     = new_item;

                                this->count++;
                                this->count_half = this->count / 2;

                                return 1;
                        }

                        item = direction >= 0 ? item->next : item->prev;
                }

                free(new_item);
        }

        return 0;
}


//==============================================================================
/**
 * @brief  Get indexed element from list
 * @param  this         list object
 * @param  index        item index
 * @param  direction    find direction
 * @return Pointer to item's data
 */
//==============================================================================
static void *at_item(llist_t *this, int index, int direction)
{
        if (index < 0 || index >= this->count) {
                return NULL;
        }

        item_t *item = direction >= 0 ? this->head : this->tail;
        while (item) {
                if (index-- == 0) {
                        return item->data;
                }

                item = direction >= 0 ? item->next : item->prev;
        }

        return NULL;
}


//==============================================================================
/**
 * @brief  Append data to the list
 * @param  this         list object
 * @param  data         data to append
 * @param  direction    append direction
 * @return On success 1 is returned, otherwise 0
 */
//==============================================================================
static int append_prepend(llist_t *this, const void *data, int direction)
{
        item_t* new_item = calloc(1, sizeof(item_t));
        if (new_item) {
                new_item->data = const_cast(void*, data);

                if (this->head == NULL) {
                        this->head = new_item;
                        this->tail = new_item;
                } else {
                        if (direction >= 0) {
                                item_t *prev_item = this->tail;
                                prev_item->next   = new_item;
                                new_item->prev    = prev_item;
                                new_item->next    = NULL;
                                this->tail        = new_item;
                        } else {
                                item_t* next_item = this->head;
                                next_item->prev   = new_item;
                                new_item->prev    = NULL;
                                new_item->next    = next_item;
                                this->head        = new_item;
                        }
                }

                this->count++;
                this->count_half = this->count / 2;

                return 1;
        }

        return 0;
}

/*==============================================================================
  End of file
==============================================================================*/
