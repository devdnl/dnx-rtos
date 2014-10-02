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

#ifndef _LLIST_H_
#define _LLIST_H_

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
#define llist_foreach(type, element, llist_t__list)\
        for (bool _tb = true; _tb; _tb = false)\
                for (llist_iterator_t _iterator = llist_iterator(llist_t__list); _tb;)\
                        for (type element = llist_begin(&_iterator);\
                             element;\
                             element = llist_iterator_next(&_iterator), _tb = false)

/*==============================================================================
  Exported object types
==============================================================================*/
typedef struct llist llist_t;
typedef int  (*llist_cmp_functor_t)(const void *a, const void *b);
typedef void (*llist_obj_dtor_t)(void *object);

// iterator object
typedef struct {
        llist_t  *list;
        void     *begin;
        void     *end;
        bool      fini;
        uint32_t  magic;
} llist_iterator_t;

/*==============================================================================
  Exported functions
==============================================================================*/
//==============================================================================
/**
 * @brief  List constructor
 * @param  cmp_functor  compare functor (can be NULL)
 * @param  obj_dtor     object's destructor (can be NULL, then free() is used as destructor)
 * @return On success list object is returned, otherwise NULL
 */
//==============================================================================
extern llist_t *llist_new(llist_cmp_functor_t functor, llist_obj_dtor_t obj_dtor);

//==============================================================================
/**
 * @brief  List destructor
 * @param  this         list object
 * @return On success 1 is returned, otherwise 0
 */
//==============================================================================
extern int llist_delete(llist_t *list);

//==============================================================================
/**
 * @brief  Checks if list is empty
 * @param  this         list object
 * @return If list is empty then true is returned, otherwise false
 */
//==============================================================================
extern bool llist_empty(llist_t *list);

//==============================================================================
/**
 * @brief  Returns a number of elements of the list
 * @param  this         list object
 * @return Number of elements of the list or -1 on error
 */
//==============================================================================
extern int llist_size(llist_t *list);

//==============================================================================
/**
 * @brief  Pushes selected data to the front of the list. Creates a new object
 * @param  this         list object
 * @param  size         data size
 * @param  data         data source
 * @return On success allocated memory pointer is returned, otherwise NULL
 */
//==============================================================================
extern void *llist_push_emplace_front(llist_t *list, size_t size, const void *data);

//==============================================================================
/**
 * @brief  Pushes selected object to the front of the list
 * @param  this         list object
 * @param  object       object to push
 * @return On success pointer to the object is returned, otherwise NULL
 */
//==============================================================================
extern void *llist_push_front(llist_t *list, void *object);

//==============================================================================
/**
 * @brief  Delete first element of the list. This destroys an element
 * @param  this         list object
 * @return On success 1 is returned, otherwise 0
 */
//==============================================================================
extern int llist_pop_front(llist_t *list);

//==============================================================================
/**
 * @brief  Push selected data to the back of the list. Creates a new object
 * @param  this         list object
 * @param  size         data size
 * @param  data         data source
 * @return On success allocated memory pointer is returned, otherwise NULL
 */
//==============================================================================
extern void *llist_push_emplace_back(llist_t *list, size_t size, const void *data);

//==============================================================================
/**
 * @brief  Push selected object to the back of the list
 * @param  this         list object
 * @return On success pointer to the object is returned, otherwise NULL
 */
//==============================================================================
extern void *llist_push_back(llist_t *list, void *object);

//==============================================================================
/**
 * @brief  Delete the last element of the list. This destroys element
 * @param  this         list object
 * @return On success 1 is returned, otherwise 0
 */
//==============================================================================
extern int llist_pop_back(llist_t *list);

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
extern void *llist_emplace(llist_t *list, int position, size_t size, const void *data);

//==============================================================================
/**
 * @brief  Insert an element to the list
 * @param  this         list object
 * @param  position     position to insert
 * @param  object       object to insert
 * @return On success object is returned, otherwise NULL
 */
//==============================================================================
extern void *llist_insert(llist_t *list, int position, void *object);

//==============================================================================
/**
 * @brief  Erase selected begin of the list. The element is destroyed
 * @param  this         list object
 * @param  position     position to remove
 * @return On success 1 is returned, otherwise 0
 */
//==============================================================================
extern int llist_erase(llist_t *list, int position);

//==============================================================================
/**
 * @brief  Return selected begin and remove from the list. The element is not destroyed
 * @param  this         list object
 * @param  position     position to take (unlink)
 * @return On success taken object is returned, otherwise NULL
 */
//==============================================================================
extern void *llist_take(llist_t *list, int position);

//==============================================================================
/**
 * @brief  Return first begin and remove from the list. The element is not destroyed
 * @param  this         list object
 * @return On success taken object is returned, otherwise NULL
 */
//==============================================================================
extern void *llist_take_front(llist_t *list);

//==============================================================================
/**
 * @brief  Return last begin and remove from the list. The element is not destroyed
 * @param  this         list object
 * @return On success taken object is returned, otherwise NULL
 */
//==============================================================================
extern void *llist_take_back(llist_t *list);

//==============================================================================
/**
 * @brief  Clear entire list (objects are destroyed)
 * @param  this         list object
 * @return On success 1 is returned, otherwise 0
 */
//==============================================================================
extern int llist_clear(llist_t *list);

//==============================================================================
/**
 * @brief  Swap 2 elements
 * @param  this         list object
 * @param  j            position of element a
 * @param  k            position of element b
 * @return On success 1 is returned, otherwise 0
 */
//==============================================================================
extern int llist_swap(llist_t *list, int j, int k);

//==============================================================================
/**
 * @brief  Sort elements of the list
 * @param  this         list object
 * @return None
 */
//==============================================================================
extern void llist_sort(llist_t *list);

//==============================================================================
/**
 * @brief  Leave only an unique elements, all not unique are removed (are destroyed)
 * @param  this         list object
 * @return None
 */
//==============================================================================
extern void llist_unique(llist_t *list);

//==============================================================================
/**
 * @brief  Reverse entire table
 * @param  this         list object
 * @return None
 */
//==============================================================================
extern void llist_reverse(llist_t *list);

//==============================================================================
/**
 * @brief  Get element from the list at selected position
 * @param  this         list object
 * @param  position     begin position
 * @return Pointer to data, or NULL on error
 */
//==============================================================================
extern void *llist_at(llist_t *list, int position);

//==============================================================================
/**
 * @brief  Check if list contains selected object
 * @param  this         list object
 * @param  object       object to find
 * @return Number of found objects, or -1 on error
 */
//==============================================================================
extern int llist_contains(llist_t *list, const void *object);

//==============================================================================
/**
 * @brief  Find selected object in the list from the beginning
 * @param  this         list object
 * @param  object       object to find
 * @return Object position, or -1 on error
 */
//==============================================================================
extern int llist_find_begin(llist_t *list, const void *object);

//==============================================================================
/**
 * @brief  Find selected object in the list from the end
 * @param  this         list object
 * @param  object       object to find
 * @return Object position, or -1 on error
 */
//==============================================================================
extern int   llist_find_end(llist_t *list, const void *object);

//==============================================================================
/**
 * @brief  Access first element
 * @param  this         list object
 * @return Pointer to data, or NULL on error
 */
//==============================================================================
extern void *llist_front(llist_t *list);

//==============================================================================
/**
 * @brief  Access last element
 * @param  this         list object
 * @return Pointer to data, or NULL on error
 */
//==============================================================================
extern void *llist_back(llist_t *list);

//==============================================================================
/**
 * @brief  Create an iterator to the list
 * @param  this         list object
 * @return Iterator object
 */
//==============================================================================
extern llist_iterator_t llist_iterator(llist_t *this);

//==============================================================================
/**
 * @brief  Return first object from list by using iterator
 * @param  iterator     iterator object
 * @return Pointer to data object
 */
//==============================================================================
extern void *llist_begin(llist_iterator_t *iterator);

//==============================================================================
/**
 * @brief  Return last object from list by using iterator
 * @param  iterator     iterator object
 * @return Pointer to data object
 */
//==============================================================================
extern void *llist_end(llist_iterator_t *iterator);

//==============================================================================
/**
 * @brief  Return selected objects from list by using range iterator (forward)
 * @param  iterator     iterator object
 * @param  begin        begin position
 * @param  end          end position
 * @return Pointer to data object
 */
//==============================================================================
extern void *llist_range(llist_iterator_t *iterator, int begin, int end);

//==============================================================================
/**
 * @brief  Return next data object from list by using iterator
 * @param  iterator     iterator object
 * @return Pointer to data object
 */
//==============================================================================
extern void *llist_iterator_next(llist_iterator_t *iterator);

//==============================================================================
/**
 * @brief  Return previous data object from list by using iterator
 * @param  iterator     iterator object
 * @return Pointer to data object
 */
//==============================================================================
extern void *llist_iterator_prev(llist_iterator_t *iterator);

/*==============================================================================
  Exported inline functions
==============================================================================*/

#ifdef __cplusplus
}
#endif

#endif /* _LLIST_H_ */
/*==============================================================================
  End of file
==============================================================================*/
