/*=========================================================================*//**
@file    llist.h

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

#ifndef _APP_LLIST_H_
#define _APP_LLIST_H_

/*==============================================================================
  Include files
==============================================================================*/
#include <lib/llist.h>
#include <stdlib.h>
#include <errno.h>

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Exported macros
==============================================================================*/
// list foreach iterator
#define llist_foreach(type, element, llist_t__list) _builtinfunc(llist_foreach, type, element, llist_t__list)

// list reversed foreach iterator
#define llist_foreach_reverse(type, element, llist_t__list) _builtinfunc(llist_foreach_reverse, type, element, llist_t__list)

// list iterator
#define llist_foreach_iterator _iterator

/*==============================================================================
  Exported object types
==============================================================================*/

/*==============================================================================
  Exported functions
==============================================================================*/
//==============================================================================
/**
 * @brief  List constructor
 * @param  cmp_functor          compare functor (can be NULL)
 * @param  obj_dtor             object destructor (can be NULL, then free() is destructor)
 * @return On success list object is returned, otherwise NULL
 */
//==============================================================================
static inline llist_t *llist_new(llist_cmp_functor_t functor, llist_obj_dtor_t obj_dtor)
{
        llist_t *list = NULL;
        errno = _builtinfunc(llist_create_usr, malloc, free, functor, obj_dtor, &list);
        return list;
}

//==============================================================================
/**
 * @brief  List destructor
 * @param  list         list object
 * @return On success 1 is returned, otherwise 0
 */
//==============================================================================
static inline int llist_delete(llist_t *list)
{
        return _builtinfunc(llist_destroy, list);
}

//==============================================================================
/**
 * @brief  Checks if list is empty
 * @param  list         list object
 * @return If list is empty then true is returned, otherwise false
 */
//==============================================================================
static inline bool llist_empty(llist_t *list)
{
        return _builtinfunc(llist_empty, list);
}

//==============================================================================
/**
 * @brief  Returns a number of elements of the list
 * @param  list         list object
 * @return Number of elements of the list or -1 on error
 */
//==============================================================================
static inline int llist_size(llist_t *list)
{
        return _builtinfunc(llist_size, list);
}

//==============================================================================
/**
 * @brief  Pushes selected data to the front of the list. Creates a new object
 * @param  list         list object
 * @param  size         data size
 * @param  data         data source
 * @return On success allocated memory pointer is returned, otherwise NULL
 */
//==============================================================================
static inline void *llist_push_emplace_front(llist_t *list, size_t size, const void *data)
{
        return _builtinfunc(llist_push_emplace_front, list, size, data);
}

//==============================================================================
/**
 * @brief  Pushes selected object to the front of the list
 * @param  list         list object
 * @param  object       object to push
 * @return On success pointer to the object is returned, otherwise NULL
 */
//==============================================================================
static inline void *llist_push_front(llist_t *list, void *object)
{
        return _builtinfunc(llist_push_front, list, object);
}

//==============================================================================
/**
 * @brief  Delete first element of the list. This destroys an element
 * @param  list         list object
 * @return On success 1 is returned, otherwise 0
 */
//==============================================================================
static inline int llist_pop_front(llist_t *list)
{
        return _builtinfunc(llist_pop_front, list);
}

//==============================================================================
/**
 * @brief  Push selected data to the back of the list. Creates a new object
 * @param  list         list object
 * @param  size         data size
 * @param  data         data source
 * @return On success allocated memory pointer is returned, otherwise NULL
 */
//==============================================================================
static inline void *llist_push_emplace_back(llist_t *list, size_t size, const void *data)
{
        return _builtinfunc(llist_push_emplace_back, list, size, data);
}

//==============================================================================
/**
 * @brief  Push selected object to the back of the list
 * @param  list         list object
 * @return On success pointer to the object is returned, otherwise NULL
 */
//==============================================================================
static inline void *llist_push_back(llist_t *list, void *object)
{
        return _builtinfunc(llist_push_back, list, object);
}

//==============================================================================
/**
 * @brief  Delete the last element of the list. This destroys element
 * @param  list         list object
 * @return On success 1 is returned, otherwise 0
 */
//==============================================================================
static inline int llist_pop_back(llist_t *list)
{
        return _builtinfunc(llist_pop_back, list);
}

//==============================================================================
/**
 * @brief  Allocate and append data at selected position in the list
 * @param  list         list object
 * @param  position     element position
 * @param  size         element's size
 * @param  data         element's data
 * @return On success pointer to the object is returned, otherwise NULL
 */
//==============================================================================
static inline void *llist_emplace(llist_t *list, int position, size_t size, const void *data)
{
        return _builtinfunc(llist_emplace, list, position, size, data);
}

//==============================================================================
/**
 * @brief  Insert an element to the list
 * @param  list         list object
 * @param  position     position to insert
 * @param  object       object to insert
 * @return On success object is returned, otherwise NULL
 */
//==============================================================================
static inline void *llist_insert(llist_t *list, int position, void *object)
{
        return _builtinfunc(llist_insert, list, position, object);
}

//==============================================================================
/**
 * @brief  Erase selected begin of the list. The element is destroyed
 * @param  list         list object
 * @param  position     position to remove
 * @return On success 1 is returned, otherwise 0
 */
//==============================================================================
static inline int llist_erase(llist_t *list, int position)
{
        return _builtinfunc(llist_erase, list, position);
}

//==============================================================================
/**
 * @brief  Return selected begin and remove from the list. The element is not destroyed
 * @param  list         list object
 * @param  position     position to take (unlink)
 * @return On success taken object is returned, otherwise NULL
 */
//==============================================================================
static inline void *llist_take(llist_t *list, int position)
{
        return _builtinfunc(llist_take, list, position);
}

//==============================================================================
/**
 * @brief  Return first begin and remove from the list. The element is not destroyed
 * @param  list         list object
 * @return On success taken object is returned, otherwise NULL
 */
//==============================================================================
static inline void *llist_take_front(llist_t *list)
{
        return _builtinfunc(llist_take_front, list);
}

//==============================================================================
/**
 * @brief  Return last begin and remove from the list. The element is not destroyed
 * @param  list         list object
 * @return On success taken object is returned, otherwise NULL
 */
//==============================================================================
static inline void *llist_take_back(llist_t *list)
{
        return _builtinfunc(llist_take_back, list);
}

//==============================================================================
/**
 * @brief  Clear entire list (objects are destroyed)
 * @param  list         list object
 * @return On success 1 is returned, otherwise 0
 */
//==============================================================================
static inline int llist_clear(llist_t *list)
{
        return _builtinfunc(llist_clear, list);
}

//==============================================================================
/**
 * @brief  Swap 2 elements
 * @param  list         list object
 * @param  j            position of element a
 * @param  k            position of element b
 * @return On success 1 is returned, otherwise 0
 */
//==============================================================================
static inline int llist_swap(llist_t *list, int j, int k)
{
        return _builtinfunc(llist_swap, list, j, k);
}

//==============================================================================
/**
 * @brief  Sort elements of the list
 * @param  list         list object
 * @return None
 */
//==============================================================================
static inline void llist_sort(llist_t *list)
{
        _builtinfunc(llist_sort, list);
}

//==============================================================================
/**
 * @brief  Leave only an unique elements, all not unique are removed (are destroyed)
 * @param  list         list object
 * @return None
 */
//==============================================================================
static inline void llist_unique(llist_t *list)
{
        _builtinfunc(llist_unique, list);
}

//==============================================================================
/**
 * @brief  Reverse entire table
 * @param  list         list object
 * @return None
 */
//==============================================================================
static inline void llist_reverse(llist_t *list)
{
        _builtinfunc(llist_reverse, list);
}

//==============================================================================
/**
 * @brief  Get element from the list at selected position
 * @param  list         list object
 * @param  position     begin position
 * @return Pointer to data, or NULL on error
 */
//==============================================================================
static inline void *llist_at(llist_t *list, int position)
{
        return _builtinfunc(llist_at, list, position);
}

//==============================================================================
/**
 * @brief  Check if list contains selected object
 * @param  list         list object
 * @param  object       object to find
 * @return Number of found objects, or -1 on error
 */
//==============================================================================
static inline int llist_contains(llist_t *list, const void *object)
{
        return _builtinfunc(llist_contains, list, object);
}

//==============================================================================
/**
 * @brief  Find selected object in the list from the beginning
 * @param  list         list object
 * @param  object       object to find
 * @return Object position, or -1 on error
 */
//==============================================================================
static inline int llist_find_begin(llist_t *list, const void *object)
{
        return _builtinfunc(llist_find_begin, list, object);
}

//==============================================================================
/**
 * @brief  Find selected object in the list from the end
 * @param  list         list object
 * @param  object       object to find
 * @return Object position, or -1 on error
 */
//==============================================================================
static inline int llist_find_end(llist_t *list, const void *object)
{
        return _builtinfunc(llist_find_end, list, object);
}

//==============================================================================
/**
 * @brief  Access first element
 * @param  list         list object
 * @return Pointer to data, or NULL on error
 */
//==============================================================================
static inline void *llist_front(llist_t *list)
{
        return _builtinfunc(llist_front, list);
}

//==============================================================================
/**
 * @brief  Access last element
 * @param  list         list object
 * @return Pointer to data, or NULL on error
 */
//==============================================================================
static inline void *llist_back(llist_t *list)
{
        return _builtinfunc(llist_back, list);
}

//==============================================================================
/**
 * @brief  Create an iterator to the list
 * @param  list         list object
 * @return Iterator object
 */
//==============================================================================
static inline llist_iterator_t llist_iterator(llist_t *list)
{
        return _builtinfunc(llist_iterator, list);
}

//==============================================================================
/**
 * @brief  Return first object from list by using iterator
 * @param  iterator     iterator object
 * @return Pointer to data object
 */
//==============================================================================
static inline void *llist_begin(llist_iterator_t *iterator)
{
        return _builtinfunc(llist_begin, iterator);
}

//==============================================================================
/**
 * @brief  Return last object from list by using iterator
 * @param  iterator     iterator object
 * @return Pointer to data object
 */
//==============================================================================
static inline void *llist_end(llist_iterator_t *iterator)
{
        return _builtinfunc(llist_end, iterator);
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
static inline void *llist_range(llist_iterator_t *iterator, int begin, int end)
{
        return _builtinfunc(llist_range, iterator, begin, end);
}

//==============================================================================
/**
 * @brief  Return next data object from list by using iterator
 * @param  iterator     iterator object
 * @return Pointer to data object
 */
//==============================================================================
static inline void *llist_iterator_next(llist_iterator_t *iterator)
{
        return _builtinfunc(llist_iterator_next, iterator);
}

//==============================================================================
/**
 * @brief  Return previous data object from list by using iterator
 * @param  iterator     iterator object
 * @return Pointer to data object
 */
//==============================================================================
static inline void *llist_iterator_prev(llist_iterator_t *iterator)
{
        return _builtinfunc(llist_iterator_prev, iterator);
}

//==============================================================================
/**
 * @brief  Erase selected begin of the list. The element is destroyed
 * @param  iterator     position to remove
 * @return On success 1 is returned, otherwise 0
 */
//==============================================================================
static inline int llist_erase_by_iterator(llist_iterator_t *iterator)
{
        return _builtinfunc(llist_erase_by_iterator, iterator);
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
static inline int llist_functor_cmp_pointers(const void *a, const void *b)
{
        return _builtinfunc(llist_functor_cmp_pointers, a, b);
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
static inline int llist_functor_cmp_strings(const void *a, const void *b)
{
        return _builtinfunc(llist_functor_cmp_strings, a, b);
}

#ifdef __cplusplus
}
#endif

#endif /* _APP_LLIST_H_ */
/*==============================================================================
  End of file
==============================================================================*/
