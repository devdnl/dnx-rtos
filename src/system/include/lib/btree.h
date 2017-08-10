/*==============================================================================
File     btree.h

Author   Daniel Zorychta

Brief    BTree library.

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

#ifndef _BTREE_H_
#define _BTREE_H_

/*==============================================================================
  Include files
==============================================================================*/
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "mm/mm.h"
#include "kernel/builtinfunc.h"

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Exported macros
==============================================================================*/
#define _btree_foreach(_type, _val, _btree) \
        for (int _err = 0; !_err;)\
                for (_type _val; !_err;)\
                        for (_err = _btree_minimum(_btree, &_val); !_err; _err = _btree_successor(_btree, &_val, &_val))

#define _btree_foreach_reverse(_type, _val, _btree) \
        for (int _err = 0; !_err;)\
                for (_type _val; !_err;)\
                        for (_err = _btree_maximum(_btree, &_val); !_err; _err = _btree_predecessor(_btree, &_val, &_val))

/*==============================================================================
  Exported object types
==============================================================================*/
typedef struct _btree_struct btree_t;
typedef int   (*btree_cmp_functor_t)(const void *a, const void *b);
typedef void  (*btree_obj_dtor_t)(void *obj);
typedef void *(*btree_malloc_t)(size_t size);
typedef void  (*btree_free_t)(void *mem);

/*==============================================================================
  Exported objects
==============================================================================*/

/*==============================================================================
  Exported functions
==============================================================================*/
extern int _btree_create_usr(btree_malloc_t      malloc,
                             btree_free_t        free,
                             size_t              size,
                             btree_cmp_functor_t cmp_functor,
                             btree_obj_dtor_t    obj_dtor,
                             btree_t             **btree);

extern int _btree_create_krn(enum _mm_mem        mem,
                             size_t              size,
                             btree_cmp_functor_t cmp_functor,
                             btree_obj_dtor_t    obj_dtor,
                             btree_t             **btree);

extern int _btree_create_mod(size_t              modid,
                             size_t              size,
                             btree_cmp_functor_t cmp_functor,
                             btree_obj_dtor_t    obj_dtor,
                             btree_t             **btree);

extern int  _btree_search(btree_t *tree, void *key, void *ret);
extern int  _btree_minimum(btree_t *tree, void *ret);
extern int  _btree_maximum(btree_t *tree, void *ret);
extern bool _btree_is_empty(btree_t *tree);
extern int  _btree_successor(btree_t *tree, void *key, void *ret);
extern int  _btree_predecessor(btree_t *tree, void *key, void *ret);
extern int  _btree_insert(btree_t *tree, void *data);
extern int  _btree_remove(btree_t *tree, void *data);
extern void _btree_destroy(btree_t *tree);

/*==============================================================================
  Exported inline functions
==============================================================================*/

#ifdef __cplusplus
}
#endif

#endif /* _BTREE_H_ */

/**@}*/
/*==============================================================================
  End of file
==============================================================================*/
