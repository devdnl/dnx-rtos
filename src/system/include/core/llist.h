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

#ifndef _CORE_LIST_H_
#define _CORE_LIST_H_

/*==============================================================================
  Include files
==============================================================================*/
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Exported macros
==============================================================================*/
// list foreach iterator
#define _llist_foreach(type, element, _llist_t__list)\
        for (bool __loop = true; __loop && _llist_size(_llist_t__list); __loop = false)\
                for (_llist_iterator_t _iterator = _llist_iterator(_llist_t__list); __loop; __loop = false)\
                        for (type element = _llist_begin(&_iterator);\
                             element;\
                             element = _llist_iterator_next(&_iterator), __loop = false)

// list reversed foreach iterator
#define _llist_foreach_reverse(type, element, _llist_t__list)\
        for (bool __loop = true; __loop && _llist_size(_llist_t__list); __loop = false)\
                for (_llist_iterator_t _iterator = _llist_iterator(_llist_t__list); __loop; __loop = false)\
                        for (type element = _llist_end(&_iterator);\
                             element;\
                             element = _llist_iterator_prev(&_iterator), __loop = false)

// list iterator
#define _llist_foreach_iterator _iterator

/*==============================================================================
  Exported object types
==============================================================================*/
typedef struct _list _llist_t;
typedef int   (*_llist_cmp_functor_t)(const void *a, const void *b);
typedef void  (*_llist_obj_dtor_t)   (void *object);
typedef void *(*_llist_malloc)       (size_t size);
typedef void  (*_llist_free)         (void *mem);

// iterator object
typedef struct {
        _llist_t  *list;
        void     *current;
        void     *next;
        void     *to;
        uint32_t  magic;
} _llist_iterator_t;

/*==============================================================================
  Exported functions
==============================================================================*/
//==============================================================================
/**
 * @brief  List constructor
 * @param  malloc               allocate memory function
 * @param  free                 free memory function
 * @param  cmp_functor          compare functor (can be NULL)
 * @param  obj_dtor             object destructor (can be NULL, then free() is destructor)
 * @return On success list object is returned, otherwise NULL
 */
//==============================================================================
extern _llist_t *_llist_new(_llist_malloc malloc, _llist_free free, _llist_cmp_functor_t functor, _llist_obj_dtor_t obj_dtor);

//==============================================================================
/**
 * @brief  List destructor
 * @param  list         list object
 * @return On success 1 is returned, otherwise 0
 */
//==============================================================================
extern int _llist_delete(_llist_t *list);

//==============================================================================
/**
 * @brief  Checks if list is empty
 * @param  list         list object
 * @return If list is empty then true is returned, otherwise false
 */
//==============================================================================
extern bool _llist_empty(_llist_t *list);

//==============================================================================
/**
 * @brief  Returns a number of elements of the list
 * @param  list         list object
 * @return Number of elements of the list or -1 on error
 */
//==============================================================================
extern int _llist_size(_llist_t *list);

//==============================================================================
/**
 * @brief  Pushes selected data to the front of the list. Creates a new object
 * @param  list         list object
 * @param  size         data size
 * @param  data         data source
 * @return On success allocated memory pointer is returned, otherwise NULL
 */
//==============================================================================
extern void *_llist_push_emplace_front(_llist_t *list, size_t size, const void *data);

//==============================================================================
/**
 * @brief  Pushes selected object to the front of the list
 * @param  list         list object
 * @param  object       object to push
 * @return On success pointer to the object is returned, otherwise NULL
 */
//==============================================================================
extern void *_llist_push_front(_llist_t *list, void *object);

//==============================================================================
/**
 * @brief  Delete first element of the list. This destroys an element
 * @param  list         list object
 * @return On success 1 is returned, otherwise 0
 */
//==============================================================================
extern int _llist_pop_front(_llist_t *list);

//==============================================================================
/**
 * @brief  Push selected data to the back of the list. Creates a new object
 * @param  list         list object
 * @param  size         data size
 * @param  data         data source
 * @return On success allocated memory pointer is returned, otherwise NULL
 */
//==============================================================================
extern void *_llist_push_emplace_back(_llist_t *list, size_t size, const void *data);

//==============================================================================
/**
 * @brief  Push selected object to the back of the list
 * @param  list         list object
 * @return On success pointer to the object is returned, otherwise NULL
 */
//==============================================================================
extern void *_llist_push_back(_llist_t *list, void *object);

//==============================================================================
/**
 * @brief  Delete the last element of the list. This destroys element
 * @param  list         list object
 * @return On success 1 is returned, otherwise 0
 */
//==============================================================================
extern int _llist_pop_back(_llist_t *list);

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
extern void *_llist_emplace(_llist_t *list, int position, size_t size, const void *data);

//==============================================================================
/**
 * @brief  Insert an element to the list
 * @param  list         list object
 * @param  position     position to insert
 * @param  object       object to insert
 * @return On success object is returned, otherwise NULL
 */
//==============================================================================
extern void *_llist_insert(_llist_t *list, int position, void *object);

//==============================================================================
/**
 * @brief  Erase selected begin of the list. The element is destroyed
 * @param  list         list object
 * @param  position     position to remove
 * @return On success 1 is returned, otherwise 0
 */
//==============================================================================
extern int _llist_erase(_llist_t *list, int position);

//==============================================================================
/**
 * @brief  Return selected begin and remove from the list. The element is not destroyed
 * @param  list         list object
 * @param  position     position to take (unlink)
 * @return On success taken object is returned, otherwise NULL
 */
//==============================================================================
extern void *_llist_take(_llist_t *list, int position);

//==============================================================================
/**
 * @brief  Return first begin and remove from the list. The element is not destroyed
 * @param  list         list object
 * @return On success taken object is returned, otherwise NULL
 */
//==============================================================================
extern void *_llist_take_front(_llist_t *list);

//==============================================================================
/**
 * @brief  Return last begin and remove from the list. The element is not destroyed
 * @param  list         list object
 * @return On success taken object is returned, otherwise NULL
 */
//==============================================================================
extern void *_llist_take_back(_llist_t *list);

//==============================================================================
/**
 * @brief  Clear entire list (objects are destroyed)
 * @param  list         list object
 * @return On success 1 is returned, otherwise 0
 */
//==============================================================================
extern int _llist_clear(_llist_t *list);

//==============================================================================
/**
 * @brief  Swap 2 elements
 * @param  list         list object
 * @param  j            position of element a
 * @param  k            position of element b
 * @return On success 1 is returned, otherwise 0
 */
//==============================================================================
extern int _llist_swap(_llist_t *list, int j, int k);

//==============================================================================
/**
 * @brief  Sort elements of the list
 * @param  list         list object
 * @return None
 */
//==============================================================================
extern void _llist_sort(_llist_t *list);

//==============================================================================
/**
 * @brief  Leave only an unique elements, all not unique are removed (are destroyed)
 * @param  list         list object
 * @return None
 */
//==============================================================================
extern void _llist_unique(_llist_t *list);

//==============================================================================
/**
 * @brief  Reverse entire table
 * @param  list         list object
 * @return None
 */
//==============================================================================
extern void _llist_reverse(_llist_t *list);

//==============================================================================
/**
 * @brief  Get element from the list at selected position
 * @param  list         list object
 * @param  position     begin position
 * @return Pointer to data, or NULL on error
 */
//==============================================================================
extern void *_llist_at(_llist_t *list, int position);

//==============================================================================
/**
 * @brief  Check if list contains selected object
 * @param  list         list object
 * @param  object       object to find
 * @return Number of found objects, or -1 on error
 */
//==============================================================================
extern int _llist_contains(_llist_t *list, const void *object);

//==============================================================================
/**
 * @brief  Find selected object in the list from the beginning
 * @param  list         list object
 * @param  object       object to find
 * @return Object position, or -1 on error
 */
//==============================================================================
extern int _llist_find_begin(_llist_t *list, const void *object);

//==============================================================================
/**
 * @brief  Find selected object in the list from the end
 * @param  list         list object
 * @param  object       object to find
 * @return Object position, or -1 on error
 */
//==============================================================================
extern int _llist_find_end(_llist_t *list, const void *object);

//==============================================================================
/**
 * @brief  Access first element
 * @param  list         list object
 * @return Pointer to data, or NULL on error
 */
//==============================================================================
extern void *_llist_front(_llist_t *list);

//==============================================================================
/**
 * @brief  Access last element
 * @param  list         list object
 * @return Pointer to data, or NULL on error
 */
//==============================================================================
extern void *_llist_back(_llist_t *list);

//==============================================================================
/**
 * @brief  Create an iterator to the list
 * @param  list         list object
 * @return Iterator object
 */
//==============================================================================
extern _llist_iterator_t _llist_iterator(_llist_t *list);

//==============================================================================
/**
 * @brief  Return first object from list by using iterator
 * @param  iterator     iterator object
 * @return Pointer to data object
 */
//==============================================================================
extern void *_llist_begin(_llist_iterator_t *iterator);

//==============================================================================
/**
 * @brief  Return last object from list by using iterator
 * @param  iterator     iterator object
 * @return Pointer to data object
 */
//==============================================================================
extern void *_llist_end(_llist_iterator_t *iterator);

//==============================================================================
/**
 * @brief  Return selected objects from list by using range iterator (forward)
 * @param  iterator     iterator object
 * @param  begin        begin position
 * @param  end          end position
 * @return Pointer to data object
 */
//==============================================================================
extern void *_llist_range(_llist_iterator_t *iterator, int begin, int end);

//==============================================================================
/**
 * @brief  Return next data object from list by using iterator
 * @param  iterator     iterator object
 * @return Pointer to data object
 */
//==============================================================================
extern void *_llist_iterator_next(_llist_iterator_t *iterator);

//==============================================================================
/**
 * @brief  Return previous data object from list by using iterator
 * @param  iterator     iterator object
 * @return Pointer to data object
 */
//==============================================================================
extern void *_llist_iterator_prev(_llist_iterator_t *iterator);

//==============================================================================
/**
 * @brief  Erase selected begin of the list. The element is destroyed
 * @param  iterator     position to remove
 * @return On success 1 is returned, otherwise 0
 */
//==============================================================================
extern int _llist_erase_by_iterator(_llist_iterator_t *iterator);

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
extern int _llist_functor_cmp_pointers(const void *a, const void *b);

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
extern int _llist_functor_cmp_strings(const void *a, const void *b);

/*==============================================================================
  Exported inline functions
==============================================================================*/

#ifdef __cplusplus
}
#endif

#endif /* _CORE_LIST_H_ */
/*==============================================================================
  End of file
==============================================================================*/
