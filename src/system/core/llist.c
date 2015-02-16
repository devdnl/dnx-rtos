/*=========================================================================*//**
@file    llist.c

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
#include "core/llist.h"
#include <string.h>

/*==============================================================================
  Local macros
==============================================================================*/
#ifndef const_cast
#define const_cast(t, v) ((t)(v))
#endif

#ifndef reinterpret_cast
#define reinterpret_cast(t, v) ((t)(v))
#endif

#ifndef static_cast
#define static_cast(t, v) ((t)(v))
#endif

/*==============================================================================
  Local object types
==============================================================================*/
// list's begin
typedef struct item {
        struct item *prev;
        struct item *next;
        void        *data;
} item_t;

// list main object
struct _llist {
        llist_malloc         malloc;
        llist_free           free;
        llist_cmp_functor_t  cmp_functor;
        llist_obj_dtor_t     obj_dtor;
        item_t              *head;
        item_t              *tail;
        llist_t             *self;
        size_t               count;
};

/*==============================================================================
  Local function prototypes
==============================================================================*/
static bool    is_iterator_valid(llist_iterator_t *this);
static bool    is_llist_valid   (llist_t *this);
static int     prepend          (llist_t *this, const void *data);
static int     append           (llist_t *this, const void *data);
static int     insert_item      (llist_t *this, int index, const void *data);
static item_t *get_item         (llist_t *this, int position);
static int     remove_item      (llist_t *this, item_t *item, bool unlink);
static void    quicksort        (llist_t *this, int left, int right);

/*==============================================================================
  Local objects
==============================================================================*/
static const uint32_t magic_number = 0x6D89B264;

/*==============================================================================
  Function definitions
==============================================================================*/

//==============================================================================
/**
 * @brief  Generic list constructor
 * @param  malloc               allocate memory function
 * @param  free                 free memory function
 * @param  cmp_functor          compare functor (can be NULL)
 * @param  obj_dtor             object destructor (can be NULL, then free() is destructor)
 * @return On success list object is returned, otherwise NULL
 */
//==============================================================================
llist_t *llist_new_generic(llist_malloc        malloc,
                           llist_free          free,
                           llist_cmp_functor_t cmp_functor,
                           llist_obj_dtor_t    obj_dtor)
{
        llist_t *this = NULL;

        if (malloc && free) {
                this = malloc(sizeof(llist_t));
                if (this) {
                        this->malloc      = malloc;
                        this->free        = free;
                        this->cmp_functor = cmp_functor;
                        this->obj_dtor    = obj_dtor;
                        this->head        = NULL;
                        this->tail        = NULL;
                        this->count       = 0;
                        this->self        = this;
                }
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
int llist_delete(llist_t *this)
{
        if (is_llist_valid(this)) {
                llist_clear(this);
                this->self = NULL;
                this->free(this);
                return 1;
        }

        return 0;
}

//==============================================================================
/**
 * @brief  Check if list is empty
 * @param  this         list object
 * @return If list is empty then true is returned, otherwise false
 */
//==============================================================================
bool llist_empty(llist_t *this)
{
        if (is_llist_valid(this)) {
                return this->count == 0;
        } else {
                return true;
        }
}

//==============================================================================
/**
 * @brief  Return number of elements of the list
 * @param  this         list object
 * @return Number of elements of the list or -1 on error
 */
//==============================================================================
int llist_size(llist_t *this)
{
        if (is_llist_valid(this)) {
                return this->count;
        } else {
                return -1;
        }
}

//==============================================================================
/**
 * @brief  Push selected data to the front of the list. Creates a new object
 * @param  this         list object
 * @param  size         data size
 * @param  data         data source
 * @return On success allocated memory pointer is returned, otherwise NULL
 */
//==============================================================================
void *llist_push_emplace_front(llist_t *this, size_t size, const void *data)
{
        if (is_llist_valid(this) && data && size) {
                void *mem = this->malloc(size);
                if (mem) {
                        memcpy(mem, data, size);

                        if (prepend(this, mem)) {
                                return mem;
                        } else {
                                this->free(mem);
                        }
                }
        }

        return NULL;
}

//==============================================================================
/**
 * @brief  Push selected object to the front of the list
 * @param  this         list object
 * @param  object       object to push
 * @return On success pointer to the object is returned, otherwise NULL
 */
//==============================================================================
void *llist_push_front(llist_t *this, void *object)
{
        if (is_llist_valid(this) && object) {
                if (prepend(this, object)) {
                        return object;
                }
        }

        return NULL;
}

//==============================================================================
/**
 * @brief  Delete first element of the list. This destroys element
 * @param  this         list object
 * @return On success 1 is returned, otherwise 0
 */
//==============================================================================
int llist_pop_front(llist_t *this)
{
        if (is_llist_valid(this)) {
                return remove_item(this, this->head, false);
        } else {
                return 0;
        }
}

//==============================================================================
/**
 * @brief  Push selected data to the back of the list. Creates a new object
 * @param  this         list object
 * @param  size         data size
 * @param  data         data source
 * @return On success allocated memory pointer is returned, otherwise NULL
 */
//==============================================================================
void *llist_push_emplace_back(llist_t *this, size_t size, const void *data)
{
        if (is_llist_valid(this) && data && size) {
                void *mem = this->malloc(size);
                if (mem) {
                        memcpy(mem, data, size);

                        if (append(this, mem)) {
                                return mem;
                        } else {
                                this->free(mem);
                        }
                }
        }

        return NULL;
}

//==============================================================================
/**
 * @brief  Push selected object to the back of the list
 * @param  this         list object
 * @return On success pointer to the object is returned, otherwise NULL
 */
//==============================================================================
void *llist_push_back(llist_t *this, void *object)
{
        if (is_llist_valid(this) && object) {
                if (append(this, object)) {
                        return object;
                }
        }

        return NULL;
}

//==============================================================================
/**
 * @brief  Delete the last element of the list. This destroys element
 * @param  this         list object
 * @return On success 1 is returned, otherwise 0
 */
//==============================================================================
int llist_pop_back(llist_t *this)
{
        if (is_llist_valid(this)) {
                return remove_item(this, this->tail, false);
        } else {
                return 0;
        }
}

//==============================================================================
/**
 * @brief  Allocate and append data at selected position in the list
 * @param  this         list object
 * @param  position     element position
 * @param  size         element's size
 * @param  data         element's data
 * @return On success pointer to the object is returned, otherwise NULL
 */
//==============================================================================
void *llist_emplace(llist_t *this, int position, size_t size, const void *data)
{
        if (is_llist_valid(this) && position >= 0 && data && size) {
                void *mem = this->malloc(size);
                if (mem) {
                        memcpy(mem, data, size);

                        if (insert_item(this, position, mem)) {
                                return mem;
                        } else {
                                this->free(mem);
                        }
                }
        }

        return NULL;
}

//==============================================================================
/**
 * @brief  Insert an element to the list
 * @param  this         list object
 * @param  position     position to insert
 * @param  object       object to insert
 * @return On success object is returned, otherwise NULL
 */
//==============================================================================
void *llist_insert(llist_t *this, int position, void *object)
{
        if (is_llist_valid(this) && position >= 0 && object) {
                return insert_item(this, position, object) ? object : NULL;
        }

        return NULL;
}

//==============================================================================
/**
 * @brief  Erase selected begin of the list. The element is destroyed
 * @param  this         list object
 * @param  position     position to remove
 * @return On success 1 is returned, otherwise 0
 */
//==============================================================================
int llist_erase(llist_t *this, int position)
{
        if (is_llist_valid(this)) {
                return remove_item(this, get_item(this, position), false);
        }

        return 0;
}

//==============================================================================
/**
 * @brief  Return selected begin and remove from the list. The element is not destroyed
 * @param  this         list object
 * @param  position     position to take (unlink)
 * @return On success taken object is returned, otherwise NULL
 */
//==============================================================================
void *llist_take(llist_t *this, int position)
{
        void *obj = NULL;

        if (is_llist_valid(this) && position >= 0) {
                item_t *item = get_item(this, position);
                obj = item->data;
                if (obj) {
                        remove_item(this, item, true);
                }
        }

        return obj;
}

//==============================================================================
/**
 * @brief  Return first begin and remove from the list. The element is not destroyed
 * @param  this         list object
 * @return On success taken object is returned, otherwise NULL
 */
//==============================================================================
void *llist_take_front(llist_t *this)
{
        void *obj = NULL;

        if (is_llist_valid(this)) {
                item_t *item = this->head;
                obj = item->data;
                if (obj) {
                        remove_item(this, item, true);
                }
        }

        return obj;
}

//==============================================================================
/**
 * @brief  Return last begin and remove from the list. The element is not destroyed
 * @param  this         list object
 * @return On success taken object is returned, otherwise NULL
 */
//==============================================================================
void *llist_take_back(llist_t *this)
{
        void *obj = NULL;

        if (is_llist_valid(this)) {
                item_t *item = this->tail;
                obj = item->data;
                if (obj) {
                        remove_item(this, item, true);
                }
        }

        return obj;
}

//==============================================================================
/**
 * @brief  Clear entire list (objects are destroyed)
 * @param  this         list object
 * @return On success 1 is returned, otherwise 0
 */
//==============================================================================
int llist_clear(llist_t *this)
{
        if (is_llist_valid(this)) {
                if (this->count == 0)
                        return 1;

                item_t *item = this->head;
                while (item) {
                        item_t *item_rm = item;
                        item = item->next;

                        remove_item(this, item_rm, false);
                }

                this->count = 0;
                this->head  = NULL;
                this->tail  = NULL;

                return 1;
        }

        return 0;
}

//==============================================================================
/**
 * @brief  Sort elements of the list
 * @param  this         list object
 * @return None
 */
//==============================================================================
void llist_sort(llist_t *this)
{
        if (is_llist_valid(this)) {
                if (this->cmp_functor) {
                        quicksort(this, 0, this->count - 1);
                }
        }
}

//==============================================================================
/**
 * @brief  Leave only unique elements, not unique are removed (are destroyed)
 * @param  this         list object
 * @return None
 */
//==============================================================================
void llist_unique(llist_t *this)
{
        if (is_llist_valid(this)) {
                if (this->cmp_functor) {
                        quicksort(this, 0, this->count - 1);

                        item_t *item = this->head;
                        while (item && item->next) {
                                item_t *item_rm = item;
                                item = item->next;

                                if (this->cmp_functor(item_rm->data, item_rm->next->data) == 0) {
                                        remove_item(this, item_rm, false);
                                }
                        }
                }
        }
}

//==============================================================================
/**
 * @brief  Reverse entire table
 * @param  this         list object
 * @return None
 */
//==============================================================================
void llist_reverse(llist_t *this)
{
    if (is_llist_valid(this)) {
            item_t *item_h = this->head;
            item_t *item_t = this->tail;
            int     n      = this->count / 2;

            while (n && item_h && item_t) {
                    void *data   = item_t->data;
                    item_t->data = item_h->data;
                    item_h->data = data;

                    item_h = item_h->next;
                    item_t = item_t->prev;

                    n--;
            }
    }
}

//==============================================================================
/**
 * @brief  Get element from the list at selected position
 * @param  this         list object
 * @param  position     begin position
 * @return Pointer to data, or NULL on error
 */
//==============================================================================
void *llist_at(llist_t *this, int position)
{
        if (is_llist_valid(this)) {
                item_t *item = get_item(this, position);
                if (item) {
                        return item->data;
                }
        }

        return NULL;
}

//==============================================================================
/**
 * @brief  Check if list contains selected object
 * @param  this         list object
 * @param  object       object to find
 * @return Number of found objects, or -1 on error
 */
//==============================================================================
int llist_contains(llist_t *this, const void *object)
{
        int cnt = -1;

        if (is_llist_valid(this) && object) {
                if (this->cmp_functor) {
                        cnt = 0;
                        for (item_t *item = this->head; item; item = item->next) {
                                if (this->cmp_functor(item->data, object) == 0) {
                                        cnt++;
                                }
                        }
                }
        }

        return cnt;
}

//==============================================================================
/**
 * @brief  Find selected object in the list from the beginning
 * @param  this         list object
 * @param  object       object to find
 * @return Object position, or -1 on error
 */
//==============================================================================
int llist_find_begin(llist_t *this, const void *object)
{
        if (is_llist_valid(this) && object) {
                if (this->cmp_functor) {
                        int cnt = 0;
                        for (item_t *item = this->head; item; item = item->next, cnt++) {
                                if (this->cmp_functor(item->data, object) == 0) {
                                        return cnt;
                                }
                        }
                }
        }

        return -1;
}

//==============================================================================
/**
 * @brief  Find selected object in the list from the end
 * @param  this         list object
 * @param  object       object to find
 * @return Object position, or -1 on error
 */
//==============================================================================
int llist_find_end(llist_t *this, const void *object)
{
        if (is_llist_valid(this) && object) {
                if (this->cmp_functor) {
                        int cnt = this->count - 1;
                        for (item_t *item = this->tail; item; item = item->prev, cnt--) {
                                if (this->cmp_functor(item->data, object) == 0) {
                                        return cnt;
                                }
                        }
                }
        }

        return -1;
}

//==============================================================================
/**
 * @brief  Access first element
 * @param  this         list object
 * @return Pointer to data, or NULL on error
 */
//==============================================================================
void *llist_front(llist_t *this)
{
        if (is_llist_valid(this)) {
                return this->head->data;
        }

        return NULL;
}

//==============================================================================
/**
 * @brief  Access last element
 * @param  this         list object
 * @return Pointer to data, or NULL on error
 */
//==============================================================================
void *llist_back(llist_t *this)
{
        if (is_llist_valid(this)) {
                return this->tail->data;
        }

        return NULL;
}

//==============================================================================
/**
 * @brief  Swap 2 elements
 * @param  this         list object
 * @param  j            position of element a
 * @param  k            position of element b
 * @return On success 1 is returned, otherwise 0
 */
//==============================================================================
int llist_swap(llist_t *this, int j, int k)
{
        if (is_llist_valid(this) && j >= 0 && k >= 0) {
                if (static_cast(size_t, j) < this->count && static_cast(size_t, k) < this->count) {

                        item_t *item_a = get_item(this, j);
                        item_t *item_b = get_item(this, k);

                        if (item_a && item_b) {
                                void *data   = item_b->data;
                                item_b->data = item_a->data;
                                item_a->data = data;

                                return 1;
                        }
                }
        }

        return 0;
}

//==============================================================================
/**
 * @brief  Create an iterator to the list
 * @param  this         list object
 * @return Iterator object
 */
//==============================================================================
llist_iterator_t llist_iterator(llist_t *this)
{
        llist_iterator_t iterator;
        memset(&iterator, 0, sizeof(llist_iterator_t));

        if (is_llist_valid(this)) {
                iterator.list  = this;
                iterator.magic = magic_number;
        }

        return iterator;
}

//==============================================================================
/**
 * @brief  Return first object from list by using iterator
 * @param  iterator     iterator object
 * @return Pointer to data object
 */
//==============================================================================
void *llist_begin(llist_iterator_t *iterator)
{
        if (is_iterator_valid(iterator)) {
                if (is_llist_valid(iterator->list)) {
                        iterator->current = iterator->list->head;

                        if (iterator->current) {
                                iterator->next = reinterpret_cast(item_t*, iterator->current)->next;
                                return reinterpret_cast(item_t*, iterator->current)->data;
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
void *llist_end(llist_iterator_t *iterator)
{
        if (is_iterator_valid(iterator)) {
                if (is_llist_valid(iterator->list)) {
                        iterator->current = iterator->list->tail;

                        if (iterator->current) {
                                iterator->next = reinterpret_cast(item_t*, iterator->current)->prev;
                                return reinterpret_cast(item_t*, iterator->current)->data;
                        }
                }
        }

        return NULL;
}

//==============================================================================
/**
 * @brief  Return selected objects from list by using range iterator (forward)
 * @param  iterator     iterator object
 * @param  begin        begin position
 * @param  end          end position
 * @return Pointer to data object
 */
//==============================================================================
void *llist_range(llist_iterator_t *iterator, int begin, int end)
{
        void *obj = NULL;

        if (is_iterator_valid(iterator) && begin >= 0 && end >= 0) {
                if (is_llist_valid(iterator->list)) {
                        int direction     = begin > end ? -1 : 1;
                        iterator->next    = get_item(iterator->list, begin + direction);
                        iterator->current = get_item(iterator->list, begin);
                        iterator->to      = get_item(iterator->list, end);

                        if (iterator->current) {
                                obj = reinterpret_cast(item_t*, iterator->current)->data;

                                if (iterator->current == iterator->to) {
                                        iterator->current = NULL;
                                }
                        }
                }
        }

        return obj;
}

//==============================================================================
/**
 * @brief  Return next data object from list by using iterator
 * @param  iterator     iterator object
 * @return Pointer to data object
 */
//==============================================================================
void *llist_iterator_next(llist_iterator_t *iterator)
{
        void *obj = NULL;

        if (is_iterator_valid(iterator)) {
                if (is_llist_valid(iterator->list) && iterator->current) {
                        iterator->current = iterator->next;

                        if (iterator->current) {
                                iterator->next = reinterpret_cast(item_t*, iterator->current)->next;
                                obj = reinterpret_cast(item_t*, iterator->current)->data;

                                if (iterator->current == iterator->to) {
                                        iterator->current = NULL;
                                }
                        }
                }
        }

        return obj;
}

//==============================================================================
/**
 * @brief  Return previous data object from list by using iterator
 * @param  iterator     iterator object
 * @return Pointer to data object
 */
//==============================================================================
void *llist_iterator_prev(llist_iterator_t *iterator)
{
        void *obj = NULL;

        if (is_iterator_valid(iterator)) {
                if (is_llist_valid(iterator->list) && iterator->current) {
                        iterator->current = iterator->next;

                        if (iterator->current) {
                                iterator->next = reinterpret_cast(item_t*, iterator->next)->prev;
                                obj = reinterpret_cast(item_t*, iterator->current)->data;

                                if (iterator->current == iterator->to) {
                                        iterator->current = NULL;
                                }
                        }
                }
        }

        return obj;
}

//==============================================================================
/**
 * @brief  Erase selected begin of the list. The element is destroyed
 * @param  iterator     position to remove
 * @return On success 1 is returned, otherwise 0
 */
//==============================================================================
int llist_erase_by_iterator(llist_iterator_t *iterator)
{
        if (is_iterator_valid(iterator)) {
                if (is_llist_valid(iterator->list) && iterator->current) {
                        return remove_item(iterator->list, iterator->current, false);
                }
        }

        return 0;
}

//==============================================================================
/**
 * @brief  Compare functor that compares two pointers (not contents)
 * @param  a    pointer a
 * @param  b    pointer b
 * @return a > b:  1
 *         a = b:  0
 *         a < b: -1
 */
//==============================================================================
int llist_functor_cmp_pointers(const void *a, const void *b)
{
        if (a > b)
                return 1;
        else if (a == b)
                return 0;
        else
                return -1;
}

//==============================================================================
/**
 * @brief  Compare functor that compares two strings (contents)
 * @param  a    string a
 * @param  b    string b
 * @return a > b:  1
 *         a = b:  0
 *         a < b: -1
 */
//==============================================================================
int llist_functor_cmp_strings(const void *a, const void *b)
{
        int d = strcmp(reinterpret_cast(char *, a), reinterpret_cast(char *, b));

        if (d)
                return 1;
        else if (d == 0)
                return 0;
        else
                return -1;
}

//==============================================================================
/**
 * @brief  Check if list object is valid
 * @param  this         list object to examine
 * @return If object is valid true is returned, otherwise false
 */
//==============================================================================
static bool is_llist_valid(llist_t *this)
{
        return this && this->self == this;
}

//==============================================================================
/**
 * @brief  Check if iterator object is valid
 * @param  this         iterator object to examine
 * @return If object is valid true is returned, otherwise false
 */
//==============================================================================
static bool is_iterator_valid(llist_iterator_t *this)
{
        return this && this->magic == magic_number;
}

//==============================================================================
/**
 * @brief  Return pointer to the element container
 * @param  this         list object
 * @param  position     begin's position
 * @return On success begin is returned, otherwise NULL
 */
//==============================================================================
static item_t *get_item(llist_t *this, int position)
{
        enum direction {DIR_FORWARD, DIR_BACKWARD};

        if (position < 0 || static_cast(size_t, position) >= this->count) {
                return NULL;

        } else {
                enum direction dir;
                item_t        *item;

                if (static_cast(size_t, position) <= this->count / 2) {
                        dir      = DIR_FORWARD;
                        item     = this->head;
                } else {
                        dir      = DIR_BACKWARD;
                        item     = this->tail;
                        position = this->count - position - 1;
                }

                while (item) {
                        if (position-- == 0) {
                                return item;
                        } else {
                                item = dir == DIR_FORWARD ? item->next : item->prev;
                        }
                }
        }

        return NULL;
}

//==============================================================================
/**
 * @brief  Remove begin of the list
 * @param  this         list object
 * @param  begin        begin to remove
 * @param  unlink       unlink data from list instead of remove
 * @return On success 1 is returned, otherwise 0
 */
//==============================================================================
static int remove_item(llist_t *this, item_t *item, bool unlink)
{
        if (item) {
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

                item->next = NULL;
                item->prev = NULL;

                if (item->data && !unlink) {
                        if (this->obj_dtor) {
                                this->obj_dtor(item->data);
                        } else {
                                this->free(item->data);
                        }

                        item->data = NULL;
                }

                this->free(item);

                this->count--;

                return 1;
        } else {
                return 0;
        }
}

//==============================================================================
/**
 * @brief  Insert begin to the list
 * @param  this         list object
 * @param  index        element index
 * @param  data         data inserted to the begin
 * @param  On success 1 is returned, otherwise 0
 */
//==============================================================================
static int insert_item(llist_t *this, int index, const void *data)
{
        if (index == 0) {
                return prepend(this, data);

        } else if (static_cast(size_t, index) == this->count) {
                return append(this, data);

        } else if (static_cast(size_t, index) > this->count) {
                return 0;

        } else {
                item_t *new_item = this->malloc(sizeof(item_t));
                if (new_item) {
                        new_item->data = const_cast(void*, data);

                        item_t *item = get_item(this, index);
                        if (item) {
                                new_item->prev   = item->prev;
                                new_item->next   = item;
                                item->prev->next = new_item;
                                item->prev       = new_item;

                                this->count++;

                                return 1;
                        }

                        this->free(new_item);
                }

        }

        return 0;
}

//==============================================================================
/**
 * @brief  Prepend a new begin to the list
 * @param  this         list object
 * @param  data         data to append
 * @return On success 1 is returned, otherwise 0
 */
//==============================================================================
static int prepend(llist_t *this, const void *data)
{
        item_t *new_item = this->malloc(sizeof(item_t));
        if (new_item) {
                new_item->data = const_cast(void*, data);

                if (this->head == NULL) {
                        new_item->next   = NULL;
                        new_item->prev   = NULL;
                        this->head       = new_item;
                        this->tail       = new_item;
                } else {
                        this->head->prev = new_item;
                        new_item->prev   = NULL;
                        new_item->next   = this->head;
                        this->head       = new_item;
                }

                this->count++;

                return 1;
        } else {
                return 0;
        }
}

//==============================================================================
/**
 * @brief  Append a new begin to the list
 * @param  this         list object
 * @param  data         data to append
 * @return On success 1 is returned, otherwise 0
 */
//==============================================================================
static int append(llist_t *this, const void *data)
{
        item_t *new_item = this->malloc(sizeof(item_t));
        if (new_item) {
                new_item->data = const_cast(void*, data);

                if (this->head == NULL) {
                        new_item->next   = NULL;
                        new_item->prev   = NULL;
                        this->head       = new_item;
                        this->tail       = new_item;
                } else {
                        this->tail->next = new_item;
                        new_item->prev   = this->tail;
                        new_item->next   = NULL;
                        this->tail       = new_item;
                }

                this->count++;

                return 1;
        } else {
                return 0;
        }
}

//==============================================================================
/**
 * @brief  Quicksort algorithm
 * @param  this         list object
 * @param  left         left position (sort border)
 * @param  right        right position (sort border)
 * @return None
 */
//==============================================================================
static void quicksort(llist_t *this, int left, int right)
{
        int     i, j;
        void   *pivot;
        item_t *item_i, *item_j, *item_r, *item_l;

        i = (left + right) / 2;

        item_i = get_item(this, i);
        item_r = get_item(this, right);
        item_l = get_item(this, left);
        pivot  = item_i->data;

        item_i->data = item_r->data;

        item_j = item_i = item_l;
        for (i = j = left; i < right; item_i = item_i->next, i++) {

                if (this->cmp_functor(item_i->data, pivot) < 0) {

                        void *data   = item_j->data;
                        item_j->data = item_i->data;
                        item_i->data = data;

                        item_j = item_j->next;
                        j++;
                }
        }

        item_r->data = item_j->data;
        item_j->data = pivot;

        if (left < j - 1) {
                quicksort(this, left, j - 1);
        }

        if (j + 1 < right) {
                quicksort(this, j + 1, right);
        }
}

/*==============================================================================
  End of file
==============================================================================*/
