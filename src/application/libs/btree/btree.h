/*==============================================================================
File    btree.h

Author  Daniel Zorychta

Brief   BTree library.

        Copyright (C) 2017 Daniel Zorychta <daniel.zorychta@gmail.com>

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

==============================================================================*/

/**
@defgroup BTREE_H_ BTREE_H_

Detailed Doxygen description.
*/
/**@{*/

#ifndef _LIB_BTREE_H_
#define _LIB_BTREE_H_

/*==============================================================================
  Include files
==============================================================================*/
#include <lib/btree.h>
#include <stdlib.h>
#include <errno.h>

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Exported macros
==============================================================================*/
// btree foreach
#define btree_foreach(type, element, btree_t__list)\
        _builtinfunc(btree_foreach, type, element, btree_t__list)

// btree reversed foreach
#define btree_foreach_reverse(type, element, btree_t__list)\
        _builtinfunc(btree_foreach_reverse, type, element, btree_t__list)

/*==============================================================================
  Exported object types
==============================================================================*/

/*==============================================================================
  Exported objects
==============================================================================*/

/*==============================================================================
  Exported functions
==============================================================================*/
//==============================================================================
/**
 * @brief  BTree constructor.
 * @parma  size                 object size
 * @param  cmp_functor          compare functor
 * @param  obj_dtor             object destructor (can be NULL)
 * @return On success BTree object is returned, otherwise NULL.
 */
//==============================================================================
static inline btree_t *btree_new(size_t size, btree_cmp_functor_t cmp_functor, btree_obj_dtor_t obj_dtor)
{
        btree_t *btree = NULL;
        errno = _btree_create_usr(malloc, free, size, cmp_functor, obj_dtor, &btree);
        return btree;
}

//==============================================================================
/**
 * @brief  BTree destructor.
 * @param  btree         BTree object
 */
//==============================================================================
static inline void btree_delete(btree_t *btree)
{
        _builtinfunc(btree_destroy, btree);
}

//==============================================================================
/**
 * @brief  Function search selected object in BTree.
 *
 * @param  tree         BTree object
 * @param  key          object to find
 * @param  ret          found object
 *
 * @return On success 0 is returned.
 */
//==============================================================================
static inline int btree_search(btree_t *tree, void *key, void *ret)
{
        return _builtinfunc(btree_search, tree, key, ret);
}

//==============================================================================
/**
 * @brief  Function get object with minimum value.
 *
 * @param  tree         BTree object
 * @param  ret          return object
 *
 * @return On success 0 is returned.
 */
//==============================================================================
static inline int btree_minimum(btree_t *tree, void *ret)
{
        return _builtinfunc(btree_minimum, tree, ret);
}

//==============================================================================
/**
 * @brief  Function get object with maximum value.
 *
 * @param  tree         BTree object
 * @param  ret          return object
 *
 * @return On success 0 is returned.
 */
//==============================================================================
static inline int btree_maximum(btree_t *tree, void *ret)
{
        return _builtinfunc(btree_maximum, tree, ret);
}

//==============================================================================
/**
 * @brief  Function check if BTree is empty.
 *
 * @param  tree         BTree object
 *
 * @return Return true if BTree is empty, otherwise NULL.
 */
//==============================================================================
static inline bool btree_is_empty(btree_t *tree)
{
        return _builtinfunc(btree_is_empty, tree);
}

//==============================================================================
/**
 * @brief  Function return successor object relative to given key.
 *
 * @param  tree         BTree object
 * @param  key          key to find
 * @param  ret          successor object relative to key
 *
 * @return On success 0 is returned.
 */
//==============================================================================
static inline int btree_successor(btree_t *tree, void *key, void *ret)
{
        return _builtinfunc(btree_successor, tree, key, ret);
}

//==============================================================================
/**
 * @brief  Function return predecessor object relative to given key.
 *
 * @param  tree         BTree object
 * @param  key          key to find
 * @param  ret          predecessor object relative to key
 *
 * @return On success 0 is returned.
 */
//==============================================================================
static inline int btree_predecessor(btree_t *tree, void *key, void *ret)
{
        return _builtinfunc(btree_predecessor, tree, key, ret);
}

//==============================================================================
/**
 * @brief  Function insert object to BTree.
 *
 * @param  tree         BTree object
 * @param  data         object to insert to
 *
 * @return On success 0 is returned.
 */
//==============================================================================
static inline int btree_insert(btree_t *tree, void *data)
{
        return _builtinfunc(btree_insert, tree, data);
}

//==============================================================================
/**
 * @brief  Function delete object from BTree selected by key.
 *
 * @param  tree         BTree object
 * @param  key          object to delete
 *
 * @return On success 0 is returned.
 */
//==============================================================================
static inline int btree_remove(btree_t *tree, void *key)
{
        return _builtinfunc(btree_remove, tree, key);
}

#ifdef __cplusplus
}
#endif

#endif /* _LIB_BTREE_H_ */

/**@}*/
/*==============================================================================
  End of file
==============================================================================*/
