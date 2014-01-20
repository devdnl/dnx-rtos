/*=========================================================================*//**
@file    list.h

@author  Daniel Zorychta

@brief   This file support dynamically lists

@note    Copyright (C) 2012 Daniel Zorychta <daniel.zorychta@gmail.com>

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

#ifndef _LIST_H_
#define _LIST_H_

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Include files
==============================================================================*/
#include <sys/types.h>

/*==============================================================================
  Exported symbolic constants/macros
==============================================================================*/

/*==============================================================================
  Exported types, enums definitions
==============================================================================*/
typedef struct list list_t;

/*==============================================================================
  Exported function prototypes
==============================================================================*/
extern list_t  *list_new(void);
extern stdret_t list_delete(list_t *list);
extern i32_t    list_add_item(list_t *list, u32_t id, void *data);
extern stdret_t list_insert_item_before_n(list_t *list, i32_t nitem, u32_t id, void *data);
extern stdret_t list_insert_item_after_n(list_t *list, i32_t nitem, u32_t id, void *data);
extern stdret_t list_rm_nitem(list_t *list, i32_t nitem);
extern stdret_t list_rm_iditem(list_t *list, u32_t id);
extern stdret_t list_set_nitem_data(list_t *list, i32_t nitem, void *data);
extern void    *list_get_nitem_data(list_t *list, i32_t nitem);
extern stdret_t list_set_iditem_data(list_t *list, u32_t id, void *data);
extern void    *list_get_iditem_data(list_t *list, u32_t id);
extern stdret_t list_unlink_nitem_data(list_t *list, i32_t nitem);
extern stdret_t list_unlink_iditem_data(list_t *list, u32_t id);
extern stdret_t list_get_nitem_ID(list_t *list, i32_t nitem, u32_t *itemid);
extern stdret_t list_get_iditem_No(list_t *list, u32_t id, i32_t *nitem);
extern i32_t    list_get_item_count(list_t *list);

#ifdef __cplusplus
}
#endif

#endif /* _LIST_H_ */
/*==============================================================================
  End of file
==============================================================================*/
