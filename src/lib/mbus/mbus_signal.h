/*=========================================================================*//**
@file    mbus_friends.h

@author  Daniel Zorychta

@brief   Message Bus Library. Signal class.

@note    Copyright (C) 2014 Daniel Zorychta <daniel.zorychta@gmail.com>

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

#ifndef _MBUS_FRIENDS_H_
#define _MBUS_FRIENDS_H_

/*==============================================================================
  Include files
==============================================================================*/
#include <stdbool.h>
#include <mbus.h>
#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Exported macros
==============================================================================*/

/*==============================================================================
  Exported object types
==============================================================================*/
typedef u32_t _mbus_owner_ID_t;
typedef struct _mbus_signal _mbus_signal_t;

/*==============================================================================
  Exported objects
==============================================================================*/

/*==============================================================================
  Exported functions
==============================================================================*/
extern _mbus_signal_t  *_mbus_signal_new(const char*, _mbus_owner_ID_t, size_t, size_t, mbus_sig_perm_t, mbus_sig_type_t);
extern  void            _mbus_signal_delete(_mbus_signal_t*);
extern  char           *_mbus_signal_get_name(_mbus_signal_t*);
extern  mbus_sig_type_t _mbus_signal_get_type(_mbus_signal_t*);
extern  size_t          _mbus_signal_get_size(_mbus_signal_t*);
extern _mbus_owner_ID_t _mbus_signal_get_owner(_mbus_signal_t*);
extern  mbus_sig_perm_t _mbus_signal_get_permissions(_mbus_signal_t*);
extern  mbus_errno_t    _mbus_signal_set_data(_mbus_signal_t*, const void*);
extern  mbus_errno_t    _mbus_signal_get_data(_mbus_signal_t*, void**);
extern  int             _mbus_signal_compare(_mbus_signal_t*, _mbus_signal_t*);

/*==============================================================================
  Exported inline functions
==============================================================================*/

#ifdef __cplusplus
}
#endif

#endif /* _MBUS_FRIENDS_H_ */
/*==============================================================================
  End of file
==============================================================================*/
