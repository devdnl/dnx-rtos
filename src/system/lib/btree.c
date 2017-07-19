/*==============================================================================
File     btree.c

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

/*==============================================================================
  Include files
==============================================================================*/
#include <string.h>
#include "lib/btree.h"
#include "libc/errno.h"
#include "dnx/misc.h"

/*==============================================================================
  Local macros
==============================================================================*/
#define root(t)                 (t->root)
#define node_size(t)            (t->node_size)
#define elem_size(t)            (t->elem_size)

#define parent(n)               (n->parent)
#define left(n)                 (n->left)
#define right(n)                (n->right)

#define data(t,n)               (((char *)n) + node_size(t))
#define data_copy(t, d, s)      memcpy(d, s, elem_size(t))
#define data_compare(t, f, s)   ((t)->cmp)(f, s)

/*==============================================================================
  Local object types
==============================================================================*/
typedef void *(*malloc_t)(size_t size, void *allocctx);
typedef void  (*free_t)(void *mem, void *freectx);

struct _btree_struct {
        malloc_t            malloc;
        free_t              free;
        void               *allocctx;
        void               *freectx;
        struct node        *root;
        size_t              node_size;
        size_t              elem_size;
        btree_cmp_functor_t cmp;
        btree_obj_dtor_t    node_dtor;
};

typedef struct node {
        struct node *parent;
        struct node *left;
        struct node *right;
} btnode_t;

/*==============================================================================
  Local function prototypes
==============================================================================*/
static void      btree_init(btree_t *tree, size_t size, btree_cmp_functor_t cmp_functor, btree_obj_dtor_t obj_dtor);
static btnode_t *node_minimum(btnode_t*);
static btnode_t *node_maximum(btnode_t*);
static btnode_t *node_search(btree_t*, btnode_t*, void *);
static void      node_close(btree_t*, btnode_t*);
static btnode_t *node_successor(btnode_t*);
static btnode_t *node_make(btree_t *tree, void *data);
static void     *malloc_usr(size_t size, void *allocctx);
static void      free_usr(void *mem, void *freectx);
static void     *malloc_krn(size_t size, void *allocctx);
static void      free_krn(void *mem, void *freectx);
static void     *malloc_mod(size_t size, void *allocctx);
static void      free_mod(void *mem, void *freectx);

/*==============================================================================
  Local objects
==============================================================================*/

/*==============================================================================
  Exported objects
==============================================================================*/

/*==============================================================================
  External objects
==============================================================================*/

/*==============================================================================
  Function definitions
==============================================================================*/

//==============================================================================
/**
 * @brief  Btree constructor for userland.
 *
 * @param[in]  malloc               allocate memory function
 * @param[in]  free                 free memory function
 * @param[in]  size                 object size
 * @param[in]  cmp_functor          compare functor (can be NULL)
 * @param[in]  obj_dtor             object destructor (can be NULL, then free() is destructor)
 * @param[out] btree                pointer to pointer of btree handle
 *
 * @return One of errno value.
 */
//==============================================================================
int _btree_create_usr(btree_malloc_t      malloc,
                      btree_free_t        free,
                      size_t              size,
                      btree_cmp_functor_t cmp_functor,
                      btree_obj_dtor_t    obj_dtor,
                      btree_t             **btree)
{
        int err = EINVAL;

        if (malloc && free && btree) {
                *btree = malloc(sizeof(btree_t));
                if (*btree) {
                        (*btree)->malloc   = malloc_usr;
                        (*btree)->free     = free_usr;
                        (*btree)->allocctx = malloc;
                        (*btree)->freectx  = free;

                        btree_init(*btree, size, cmp_functor, obj_dtor);

                        err = ESUCC;
                } else {
                        err = ENOMEM;
                }
        }

        return err;
}

//==============================================================================
/**
 * @brief  Btree constructor for kernel space.
 *
 * @param[in]  mem                  memory group
 * @param[in]  size                 object size
 * @param[in]  cmp_functor          compare functor (can be NULL)
 * @param[in]  obj_dtor             object destructor (can be NULL, then free() is destructor)
 * @param[out] btree                pointer to pointer of btree handle
 *
 * @return One of errno value.
 */
//==============================================================================
int _btree_create_krn(enum _mm_mem        mem,
                      size_t              size,
                      btree_cmp_functor_t cmp_functor,
                      btree_obj_dtor_t    obj_dtor,
                      btree_t             **btree)
{
        int err = EINVAL;

        if (btree && mem != _MM_MOD && mem < _MM_COUNT) {
                err = _kzalloc(mem, sizeof(btree_t), cast(void**, btree));
                if (!err) {
                        (*btree)->malloc   = malloc_krn;
                        (*btree)->free     = free_krn;
                        (*btree)->allocctx = cast(void*, mem);
                        (*btree)->freectx  = cast(void*, mem);

                        btree_init(*btree, size, cmp_functor, obj_dtor);

                        err = ESUCC;
                }
        }

        return err;
}

//==============================================================================
/**
 * @brief  Btree constructor for modules space.
 *
 * @param[in]  modid                module ID
 * @param[in]  size                 object size
 * @param[in]  cmp_functor          compare functor (can be NULL)
 * @param[in]  obj_dtor             object destructor (can be NULL, then free() is destructor)
 * @param[out] btree                pointer to pointer of btree handle
 *
 * @return One of errno value.
 */
//==============================================================================
int _btree_create_mod(size_t              modid,
                      size_t              size,
                      btree_cmp_functor_t cmp_functor,
                      btree_obj_dtor_t    obj_dtor,
                      btree_t             **btree)
{
        int err = EINVAL;

        if (btree) {
                err = _kzalloc(_MM_MOD, sizeof(btree_t), cast(void**, btree), modid);
                if (!err) {
                        (*btree)->malloc   = malloc_mod;
                        (*btree)->free     = free_mod;
                        (*btree)->allocctx = cast(void*, modid);
                        (*btree)->freectx  = cast(void*, modid);

                        btree_init(*btree, size, cmp_functor, obj_dtor);

                        err = ESUCC;
                }
        }

        return err;
}

//==============================================================================
/**
 * @brief  Function search selected object in BTree.
 *
 * @param  tree         BTree object
 * @param  key          object to find
 * @param  ret          found object
 *
 * @return One of errno value.
 */
//==============================================================================
int _btree_search(btree_t *tree, void *key, void *ret)
{
        btnode_t *node = root(tree);

        node = node_search(tree, node, key);

        if (node) {
                if (ret) {
                        data_copy(tree, ret, data(tree, node));
                }

                return ESUCC;
        } else {
                return ENOENT;
        }
}

//==============================================================================
/**
 * @brief  Function get object with minimum value.
 *
 * @param  tree         BTree object
 * @param  ret          return object
 *
 * @return One of errno value.
 */
//==============================================================================
int _btree_minimum(btree_t *tree, void *ret)
{
        btnode_t *node = node_minimum(root(tree));
        if (node) {
                data_copy(tree, ret, data(tree, node));
                return ESUCC;
        }

        return ENOENT;
}

//==============================================================================
/**
 * @brief  Function get object with maximum value.
 *
 * @param  tree         BTree object
 * @param  ret          return object
 *
 * @return One of errno value.
 */
//==============================================================================
int _btree_maximum(btree_t *tree, void *ret)
{
        btnode_t *node = node_maximum(root(tree));
        if (node) {
                data_copy(tree, ret, data(tree, node));
                return ESUCC;
        }

        return ENOENT;
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
bool _btree_is_empty(btree_t *tree)
{
        return root(tree) == NULL;
}

//==============================================================================
/**
 * @brief  Function return successor object relative to given key.
 *
 * @param  tree         BTree object
 * @param  key          key to find
 * @param  ret          successor object relative to key
 *
 * @return One of errno value.
 */
//==============================================================================
int _btree_successor(btree_t *tree, void *key, void *ret)
{
        if (!root(tree)) {
                return ENOENT;
        }

        btnode_t *node = node_search(tree, root(tree), key);
        if (!node) {
                return ENOENT;
        }

        node = node_successor(node);
        if (node) {
                data_copy(tree, ret, data(tree, node));
                return ESUCC;
        }

        return ENOENT;
}

//==============================================================================
/**
 * @brief  Function return predecessor object relative to given key.
 *
 * @param  tree         BTree object
 * @param  key          key to find
 * @param  ret          predecessor object relative to key
 *
 * @return One of errno value.
 */
//==============================================================================
int _btree_predecessor(btree_t *tree, void *key, void *ret)
{
        if (!root(tree)) {
                return ENOENT;
        }

        btnode_t *node = node_search(tree, root(tree), key);
        if (!node) {
                return ENOENT;
        }

        if (left(node)) {
                node = node_maximum(left(node));
        } else {
                btnode_t *node2 = parent(node);
                while (node2 && node == left(node2)) {
                        node  = node2;
                        node2 = parent(node);
                }
                node = node2;
        }

        if (node) {
                data_copy(tree, ret, data(tree, node));
                return ESUCC;
        }

        return ENOENT;
}

//==============================================================================
/**
 * @brief  Function insert object to BTree.
 *
 * @param  tree         BTree object
 * @param  data         object to insert to
 *
 * @return One of errno value.
 */
//==============================================================================
int _btree_insert(btree_t *tree, void *data)
{
        btnode_t *newnode = node_make(tree, data);
        if (!newnode) {
                return ENOMEM;
        }

        btnode_t *parent = NULL;
        btnode_t *node   = root(tree);

        while (node) {
                parent = node;
                if (data_compare(tree, data, data(tree, node)) < 0) {
                        node = left(node);
                } else {
                        node = right(node);
                }
        }

        parent(newnode) = parent;

        if (!parent) {
                root(tree) = newnode;
        } else {
                if (data_compare(tree, data, data(tree, parent)) < 0) {
                        left(parent) = newnode;
                } else {
                        right(parent) = newnode;
                }
        }

        return ESUCC;
}

//==============================================================================
/**
 * @brief  Function delete object from BTree selected by key.
 *
 * @param  tree         BTree object
 * @param  key          object to delete
 *
 * @return One of errno value.
 */
//==============================================================================
int _btree_delete(btree_t *tree, void *key)
{
        if (!root(tree)) {
                return ENOENT;
        }

        btnode_t *remove;
        btnode_t *other;
        btnode_t *node = node_search(tree, root(tree), key);

        if (!node) {
                return ENOENT;
        }

        if (!left(node) || !right(node)) {
                remove = node;
        } else {
                remove = node_successor(node);
        }

        if (left(remove)) {
                other = left(remove);
        } else {
                other = right(remove);
        }

        if (other) {
                parent(other) = parent(remove);
        }

        if (!parent(remove)) {
                root(tree) = other;
        } else {
                if (remove == left(parent(remove))) {
                        left(parent(remove)) = other;
                } else {
                        right(parent(remove)) = other;
                }
        }

        if (node != remove) {
                data_copy(tree, data(tree, node), data(tree, remove));
        }

        if (tree->node_dtor) {
                tree->node_dtor(data(tree, node));
        }

        tree->free(node, tree->freectx);

        return ESUCC;
}

//==============================================================================
/**
 * @brief  Function destroy BTree.
 *
 * @param  tree         BTree object
 */
//==============================================================================
void _btree_destroy(btree_t *tree)
{
        if (root(tree)) {
                node_close(tree, root(tree));
        }

        tree->free(tree, tree->freectx);
}

//==============================================================================
/**
 * @brief  Function initialize BTree.
 *
 * @param  tree         BTree object
 * @param  size         object size
 * @param  cmp_functor  compare functor
 * @param  obj_dtor     object destructor (can be NULL)
 */
//==============================================================================
static void btree_init(btree_t            *tree,
                       size_t              size,
                       btree_cmp_functor_t cmp_functor,
                       btree_obj_dtor_t    obj_dtor)
{
        tree->root      = NULL;
        tree->node_size = sizeof(struct node);
        tree->elem_size = size;
        tree->cmp       = cmp_functor;
        tree->node_dtor = obj_dtor;
}

//==============================================================================
/**
 * @brief  Function get successor node.
 *
 * @param  node         node
 *
 * @return Successor node.
 */
//==============================================================================
static btnode_t *node_successor(btnode_t *node)
{
        btnode_t *node2;

        if (right(node)) {
                node = node_minimum(right(node));
        } else {
                node2 = parent(node);
                while (node2 && node == right(node2)) {
                        node  = node2;
                        node2 = parent(node);
                }
                node = node2;
        }

        return node;
}

//==============================================================================
/**
 * @brief  Function return node with maximum value.
 *
 * @param  node         node
 *
 * @return Maximum node.
 */
//==============================================================================
static btnode_t *node_maximum(btnode_t *node)
{
        if (node) {
                while (right(node)) {
                        node = right(node);
                }
        }

        return node;
}

//==============================================================================
/**
 * @brief  Function return node with minimum value.
 *
 * @param  node         node
 *
 * @return Minimum node.
 */
//==============================================================================
static btnode_t *node_minimum(btnode_t *node)
{
        if (node) {
                while (left(node)) {
                        node = left(node);
                }
        }

        return node;
}

//==============================================================================
/**
 * @brief  Function search node according to key.
 *
 * @param  tree         BTree object
 * @param  node         start node
 * @param  key          key to find
 *
 * @return Found node.
 */
//==============================================================================
static btnode_t *node_search(btree_t *tree, btnode_t *node, void *key)
{
        int result;

        if (node) {
                while (node && (result = data_compare(tree, data(tree, node), key))) {
                        if (result > 0) {
                                node = left(node);
                        } else {
                                node = right(node);
                        }
                }
        }

        return node;
}

//==============================================================================
/**
 * @brief  Function close selected node (delete).
 *
 * @param  tree         BTree object
 * @param  node         node to delete
 */
//==============================================================================
static void node_close(btree_t *tree, btnode_t *node)
{
        if (left(node)) {
                node_close(tree, left(node));
        }

        if (right(node)) {
                node_close(tree, right(node));
        }

        if (tree->node_dtor) {
                tree->node_dtor(data(tree, node));
        }

        tree->free(node, tree->freectx);
}

//==============================================================================
/**
 * @brief  Function create new node.
 *
 * @param  tree         BTree object
 * @param  data         node data
 *
 * @return New node object.
 */
//==============================================================================
static btnode_t *node_make(btree_t *tree, void *data)
{
        btnode_t *node = tree->malloc(node_size(tree) + elem_size(tree), tree->allocctx);

        if (node) {
                data_copy(tree, data(tree, node), data);
                parent(node) = left(node) = right(node) = NULL;
        }

        return node;
}

//==============================================================================
/**
 * @brief  Allocate memory in user space.
 *
 * @param  size         block size to allocate
 * @param  allocctx     allocation context (user's malloc function)
 *
 * @return On success pointer to allocated block, otherwise NULL.
 */
//==============================================================================
static void *malloc_usr(size_t size, void *allocctx)
{
        btree_malloc_t alloc = allocctx;
        return alloc(size);
}

//==============================================================================
/**
 * @brief  Free allocated memory in user space.
 *
 * @param  mem          block to free
 * @param  freectx      deallocation context (user's free function)
 */
//==============================================================================
static void free_usr(void *mem, void *freectx)
{
        btree_free_t free = freectx;
        free(mem);
}

//==============================================================================
/**
 * @brief  Allocate memory in kernel space.
 *
 * @param  size         block size to allocate
 * @param  allocctx     allocation context (kernel memory index)
 *
 * @return On success pointer to allocated block, otherwise NULL.
 */
//==============================================================================
static void *malloc_krn(size_t size, void *allocctx)
{
        void *mem = NULL;
        _kmalloc(cast(enum _mm_mem, allocctx), size, &mem);
        return mem;
}

//==============================================================================
/**
 * @brief  Free allocated memory in kernel space.
 *
 * @param  mem          block to free
 * @param  freectx      deallocation context (kernel memory index)
 */
//==============================================================================
static void free_krn(void *mem, void *freectx)
{
        _kfree(cast(enum _mm_mem, freectx), &mem);
}

//==============================================================================
/**
 * @brief  Allocate memory in user space.
 *
 * @param  size         block size to allocate
 * @param  allocctx     allocation context (module index)
 *
 * @return On success pointer to allocated block, otherwise NULL.
 */
//==============================================================================
static void *malloc_mod(size_t size, void *allocctx)
{
        void *mem = NULL;
        _kmalloc(_MM_MOD, size, &mem, cast(size_t, allocctx));
        return mem;
}

//==============================================================================
/**
 * @brief  Free allocated memory in user space.
 *
 * @param  mem          block to free
 * @param  freectx      deallocation context (module index)
 */
//==============================================================================
static void free_mod(void *mem, void *freectx)
{
        _kfree(_MM_MOD, &mem, cast(size_t, freectx));
}

/*==============================================================================
  End of file
==============================================================================*/
