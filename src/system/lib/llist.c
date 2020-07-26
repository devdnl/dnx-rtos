/*=========================================================================*//**
@file    llist.c

@author  Kamil Zorychta
@author  Daniel Zorychta

@brief   C implementation of linked list

@note    Copyright (C) 2014 Kamil Zorychta <kamil.zorychta@gmail.com>
         Copyright (C) 2014 Daniel Zorychta <daniel.zorychta@gmail.com>

         This program is free software; you can redistribute it and/or modify
         it under the terms of the GNU General Public License as published by
         the Free Software Foundation and modified by the dnx RTOS exception.

         NOTE: The modification  to the GPL is  included to allow you to
               distribute a combined work that includes dnx RTOS without
               being obliged to provide the source  code for proprietary
               components outside of the dnx RTOS.

         The dnx RTOS  is  distributed  in the hope  that  it will be useful,
         but WITHOUT  ANY  WARRANTY;  without  even  the implied  warranty of
         MERCHANTABILITY  or  FITNESS  FOR  A  PARTICULAR  PURPOSE.  See  the
         GNU General Public License for more details.

         Full license text is available on the following file: doc/license.txt.


*//*==========================================================================*/

/*==============================================================================
  Include files
==============================================================================*/
#include "lib/llist.h"
#include <string.h>
#include "libc/errno.h"

/*==============================================================================
  Local macros
==============================================================================*/
#ifndef const_cast
#define const_cast(t, v) ((t)(v))
#endif

#ifndef cast
#define cast(t, v) ((t)(v))
#endif

/*==============================================================================
  Local object types
==============================================================================*/
typedef void *(*malloc_t)(size_t size, void *allocctx);
typedef void  (*free_t)(void *mem, void *freectx);

// list's begin
typedef struct item {
        struct item *prev;
        struct item *next;
        void        *data;
} item_t;

// list main object
struct _llist {
        malloc_t             malloc;
        free_t               free;
        void                *allocctx;
        void                *freectx;
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
static void   *usrmalloc        (size_t size, void *allocctx);
static void    usrfree          (void *mem, void *freectx);
static void   *krnmalloc        (size_t size, void *allocctx);
static void    krnfree          (void *mem, void *freectx);
static void   *modmalloc        (size_t size, void *allocctx);
static void    modfree          (void *mem, void *freectx);


/*==============================================================================
  Local objects
==============================================================================*/
static const uint32_t magic_number = 0x6D89B264;

/*==============================================================================
  Function definitions
==============================================================================*/

//==============================================================================
/**
 * @brief  List constructor for userland
 *
 * @param[in]  malloc               allocate memory function
 * @param[in]  free                 free memory function
 * @param[in]  cmp_functor          compare functor (can be NULL)
 * @param[in]  obj_dtor             object destructor (can be NULL, then free() is destructor)
 * @param[out] list                 pointer to pointer of list handle
 *
 * @return One of errno value.
 */
//==============================================================================
int _llist_create_usr(llist_malloc_t      malloc,
                      llist_free_t        free,
                      llist_cmp_functor_t cmp_functor,
                      llist_obj_dtor_t    obj_dtor,
                      llist_t             **list)
{
        int err = EINVAL;

        if (malloc && free && list) {
                *list = malloc(sizeof(llist_t));
                if (*list) {
                        (*list)->malloc      = usrmalloc;
                        (*list)->free        = usrfree;
                        (*list)->allocctx    = malloc;
                        (*list)->freectx     = free;
                        (*list)->cmp_functor = cmp_functor;
                        (*list)->obj_dtor    = obj_dtor;
                        (*list)->head        = NULL;
                        (*list)->tail        = NULL;
                        (*list)->count       = 0;
                        (*list)->self        = *list;

                        err = ESUCC;
                } else {
                        err = ENOMEM;
                }
        }

        return err;
}

//==============================================================================
/**
 * @brief  List constructor for userland
 *
 * @param[in]  mem                  memory allocation selection
 * @param[in]  cmp_functor          compare functor (can be NULL)
 * @param[in]  obj_dtor             object destructor (can be NULL, then free() is destructor)
 * @param[out] list                 pointer to pointer of list handle
 *
 * @return One of errno value.
 */
//==============================================================================
int _llist_create_krn(enum _mm_mem        mem,
                      llist_cmp_functor_t cmp_functor,
                      llist_obj_dtor_t    obj_dtor,
                      llist_t             **list)
{
        int err = EINVAL;

        if (list && mem != _MM_MOD && mem < _MM_COUNT) {
                err = _kzalloc(mem, sizeof(llist_t), cast(void*, list));
                if (!err) {
                        (*list)->malloc      = krnmalloc;
                        (*list)->free        = krnfree;
                        (*list)->allocctx    = cast(void*, mem);
                        (*list)->freectx     = cast(void*, mem);
                        (*list)->cmp_functor = cmp_functor;
                        (*list)->obj_dtor    = obj_dtor;
                        (*list)->head        = NULL;
                        (*list)->tail        = NULL;
                        (*list)->count       = 0;
                        (*list)->self        = *list;

                        err = ESUCC;
                }
        }

        return err;
}

//==============================================================================
/**
 * @brief  List constructor for userland
 *
 * @param[in]  modid                module ID
 * @param[in]  cmp_functor          compare functor (can be NULL)
 * @param[in]  obj_dtor             object destructor (can be NULL, then free() is destructor)
 * @param[out] list                 pointer to pointer of list handle
 *
 * @return One of errno value.
 */
//==============================================================================
int _llist_create_mod(size_t              modid,
                      llist_cmp_functor_t cmp_functor,
                      llist_obj_dtor_t    obj_dtor,
                      llist_t             **list)
{
        int err = EINVAL;

        if (list) {
                err = _kzalloc(_MM_MOD, sizeof(llist_t), cast(void*, list), modid);
                if (!err) {
                        (*list)->malloc      = modmalloc;
                        (*list)->free        = modfree;
                        (*list)->allocctx    = cast(void*, modid);
                        (*list)->freectx     = cast(void*, modid);
                        (*list)->cmp_functor = cmp_functor;
                        (*list)->obj_dtor    = obj_dtor;
                        (*list)->head        = NULL;
                        (*list)->tail        = NULL;
                        (*list)->count       = 0;
                        (*list)->self        = *list;

                        err = ESUCC;
                }
        }

        return err;
}

//==============================================================================
/**
 * @brief  List destructor
 *
 * @param  this         list object
 *
 * @return One of errno value.
 */
//==============================================================================
int _llist_destroy(llist_t *this)
{
        if (is_llist_valid(this)) {
                _llist_clear(this);
                this->self = NULL;
                this->free(this, this->freectx);
                return ESUCC;
        }

        return EINVAL;
}

//==============================================================================
/**
 * @brief  Check if list is empty
 * @param  this         list object
 * @return If list is empty then true is returned, otherwise false
 */
//==============================================================================
bool _llist_empty(llist_t *this)
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
int _llist_size(llist_t *this)
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
void *_llist_push_emplace_front(llist_t *this, size_t size, const void *data)
{
        if (is_llist_valid(this) && data && size) {
                void *mem = this->malloc(size, this->allocctx);
                if (mem) {
                        memcpy(mem, data, size);

                        if (prepend(this, mem)) {
                                return mem;
                        } else {
                                this->free(mem, this->freectx);
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
void *_llist_push_front(llist_t *this, void *object)
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
int _llist_pop_front(llist_t *this)
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
void *_llist_push_emplace_back(llist_t *this, size_t size, const void *data)
{
        if (is_llist_valid(this) && data && size) {
                void *mem = this->malloc(size, this->allocctx);
                if (mem) {
                        memcpy(mem, data, size);

                        if (append(this, mem)) {
                                return mem;
                        } else {
                                this->free(mem, this->freectx);
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
void *_llist_push_back(llist_t *this, void *object)
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
int _llist_pop_back(llist_t *this)
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
void *_llist_emplace(llist_t *this, int position, size_t size, const void *data)
{
        if (is_llist_valid(this) && position >= 0 && data && size) {
                void *mem = this->malloc(size, this->allocctx);
                if (mem) {
                        memcpy(mem, data, size);

                        if (insert_item(this, position, mem)) {
                                return mem;
                        } else {
                                this->free(mem, this->freectx);
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
void *_llist_insert(llist_t *this, int position, void *object)
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
int _llist_erase(llist_t *this, int position)
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
void *_llist_take(llist_t *this, int position)
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
void *_llist_take_front(llist_t *this)
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
void *_llist_take_back(llist_t *this)
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
int _llist_clear(llist_t *this)
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
void _llist_sort(llist_t *this)
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
void _llist_unique(llist_t *this)
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
void _llist_reverse(llist_t *this)
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
void *_llist_at(llist_t *this, int position)
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
int _llist_contains(llist_t *this, const void *object)
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
int _llist_find_begin(llist_t *this, const void *object)
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
int _llist_find_end(llist_t *this, const void *object)
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
void *_llist_front(llist_t *this)
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
void *_llist_back(llist_t *this)
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
int _llist_swap(llist_t *this, int j, int k)
{
        if (is_llist_valid(this) && (j >= 0) && (k >= 0) && (j != k)) {
                if (cast(size_t, j) < this->count && cast(size_t, k) < this->count) {

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
llist_iterator_t _llist_iterator(llist_t *this)
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
void *_llist_begin(llist_iterator_t *iterator)
{
        if (is_iterator_valid(iterator)) {
                if (is_llist_valid(iterator->list)) {
                        iterator->current = iterator->list->head;

                        if (iterator->current) {
                                iterator->next = cast(item_t*, iterator->current)->next;
                                return cast(item_t*, iterator->current)->data;
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
void *_llist_end(llist_iterator_t *iterator)
{
        if (is_iterator_valid(iterator)) {
                if (is_llist_valid(iterator->list)) {
                        iterator->current = iterator->list->tail;

                        if (iterator->current) {
                                iterator->next = cast(item_t*, iterator->current)->prev;
                                return cast(item_t*, iterator->current)->data;
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
void *_llist_range(llist_iterator_t *iterator, int begin, int end)
{
        void *obj = NULL;

        if (is_iterator_valid(iterator) && begin >= 0 && end >= 0) {
                if (is_llist_valid(iterator->list)) {
                        int direction     = begin > end ? -1 : 1;
                        iterator->next    = get_item(iterator->list, begin + direction);
                        iterator->current = get_item(iterator->list, begin);
                        iterator->to      = get_item(iterator->list, end);

                        if (iterator->current) {
                                obj = cast(item_t*, iterator->current)->data;

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
void *_llist_iterator_next(llist_iterator_t *iterator)
{
        void *obj = NULL;

        if (is_iterator_valid(iterator)) {
                if (is_llist_valid(iterator->list) && iterator->current) {
                        iterator->current = iterator->next;

                        if (iterator->current) {
                                iterator->next = cast(item_t*, iterator->current)->next;
                                obj = cast(item_t*, iterator->current)->data;

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
void *_llist_iterator_prev(llist_iterator_t *iterator)
{
        void *obj = NULL;

        if (is_iterator_valid(iterator)) {
                if (is_llist_valid(iterator->list) && iterator->current) {
                        iterator->current = iterator->next;

                        if (iterator->current) {
                                iterator->next = cast(item_t*, iterator->next)->prev;
                                obj = cast(item_t*, iterator->current)->data;

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
int _llist_erase_by_iterator(llist_iterator_t *iterator)
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
int _llist_functor_cmp_pointers(const void *a, const void *b)
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
int _llist_functor_cmp_strings(const void *a, const void *b)
{
        int d = strcmp(cast(char *, a), cast(char *, b));

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

        if (position < 0 || cast(size_t, position) >= this->count) {
                return NULL;

        } else {
                enum direction dir;
                item_t        *item;

                if (cast(size_t, position) <= this->count / 2) {
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
                                this->free(item->data, this->freectx);
                        }

                        item->data = NULL;
                }

                this->free(item, this->freectx);

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

        } else if (cast(size_t, index) == this->count) {
                return append(this, data);

        } else if (cast(size_t, index) > this->count) {
                return 0;

        } else {
                item_t *new_item = this->malloc(sizeof(item_t), this->allocctx);
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

                        this->free(new_item, this->freectx);
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
        item_t *new_item = this->malloc(sizeof(item_t), this->allocctx);
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
        item_t *new_item = this->malloc(sizeof(item_t), this->allocctx);
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

//==============================================================================
/**
 * @brief  Allocate memory in user space
 *
 * @param  size         block size to allocate
 * @param  allocctx     allocation context (user's malloc function)
 *
 * @return On success pointer to allocated block, otherwise NULL
 */
//==============================================================================
static void *usrmalloc(size_t size, void *allocctx)
{
        llist_malloc_t alloc = allocctx;
        return alloc(size);
}

//==============================================================================
/**
 * @brief  Free allocated memory in user space
 *
 * @param  mem          block to free
 * @param  freectx      deallocation context (user's free function)
 *
 * @return None
 */
//==============================================================================
static void usrfree(void *mem, void *freectx)
{
        llist_free_t free = freectx;
        free(mem);
}

//==============================================================================
/**
 * @brief  Allocate memory in user space
 *
 * @param  size         block size to allocate
 * @param  allocctx     allocation context (kernel memory index)
 *
 * @return On success pointer to allocated block, otherwise NULL
 */
//==============================================================================
static void *krnmalloc(size_t size, void *allocctx)
{
        void *mem = NULL;
        _kmalloc(cast(enum _mm_mem, allocctx), size, &mem);
        return mem;
}

//==============================================================================
/**
 * @brief  Free allocated memory in user space
 *
 * @param  mem          block to free
 * @param  freectx      deallocation context (kernel memory index)
 *
 * @return None
 */
//==============================================================================
static void krnfree(void *mem, void *freectx)
{
        _kfree(cast(enum _mm_mem, freectx), &mem);
}

//==============================================================================
/**
 * @brief  Allocate memory in user space
 *
 * @param  size         block size to allocate
 * @param  allocctx     allocation context (module index)
 *
 * @return On success pointer to allocated block, otherwise NULL
 */
//==============================================================================
static void *modmalloc(size_t size, void *allocctx)
{
        void *mem = NULL;
        _kmalloc(_MM_MOD, size, &mem, cast(size_t, allocctx));
        return mem;
}

//==============================================================================
/**
 * @brief  Free allocated memory in user space
 *
 * @param  mem          block to free
 * @param  freectx      deallocation context (module index)
 *
 * @return None
 */
//==============================================================================
static void modfree(void *mem, void *freectx)
{
        _kfree(_MM_MOD, &mem, cast(size_t, freectx));
}

/*==============================================================================
  End of file
==============================================================================*/
