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

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Exported macros
==============================================================================*/
#define llist__foreach(voidp__data, llist__iterator_t__iterator)\
        for (void *voidp__data = llist__first(llist__iterator_t__iterator); voidp__data; voidp__data = llist__iterator_next(llist__iterator_t__iterator))

/*==============================================================================
  Exported object types
==============================================================================*/
typedef struct llist llist_t;
typedef struct llist_iterator llist__iterator_t;

/*==============================================================================
  Exported functions
==============================================================================*/
extern llist_t           *llist__new(void);
extern int                llist__delete(llist_t *this);
extern int                llist__append(llist_t *this, const void *data);
extern void              *llist__append_alloc(llist_t *this, const void *data, size_t size);
extern int                llist__insert(llist_t *this, int index, const void *data);
extern void              *llist__insert_alloc(llist_t *this, int index, const void *data, size_t size);
extern int                llist__remove(llist_t *this, int index);
extern int                llist__unlink(llist_t *this, int index);
extern int                llist__clear(llist_t *this);
extern void              *llist__at(llist_t *this, int index);
extern int                llist__count(llist_t *this);
extern int                llist__contains(llist_t *this, const void *data);
extern llist__iterator_t *llist__new_iterator(llist_t *this);
extern int                llist__delete_iterator(llist__iterator_t *iterator);
extern void              *llist__first(llist__iterator_t *iterator);
extern void              *llist__last(llist__iterator_t *iterator);
extern void              *llist__iterator_next(llist__iterator_t *iterator);
extern void              *llist__iterator_prev(llist__iterator_t *iterator);

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
