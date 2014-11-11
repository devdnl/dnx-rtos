/*=========================================================================*//**
@file    mbus.h

@author  Daniel Zorychta

@brief   Message Bus Library

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

#ifndef _MBUS_H_
#define _MBUS_H_

/*==============================================================================
  Include files
==============================================================================*/
#include <sys/types.h>
#include <stddef.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Exported macros
==============================================================================*/

/*==============================================================================
  Exported object types
==============================================================================*/
/** mbus errors  */
typedef enum {
        MBUS_ERRNO__NO_ERROR,
        MBUS_ERRNO__INVALID_OBJECT_OR_ARGUMENT,
        MBUS_ERRNO__ACCESS_DENIED,
        MBUS_ERRNO__TIMEOUT,
        MBUS_ERRNO__DAEMON_IS_ALREADY_STARTED,
        MBUS_ERRNO__DAEMON_NOT_RUNNING,
        MBUS_ERRNO__NOT_ENOUGH_FREE_MEMORY,
        MBUS_ERRNO__INTERNAL_ERROR,
        MBUS_ERRNO__NO_ITEM,
        MBUS_ERRNO__MBOX_IS_FULL,
        MBUS_ERRNO__MBOX_EMPTY
} mbus_errno_t;

/** mbus signal permissions */
typedef enum {
        MBUS_PERM__PRIVATE,
        MBUS_PERM__READ,
        MBUS_PERM__WRITE,
        MBUS_PERM__INVALID
} mbus_sig_perm_t;

/** mbus signal type */
typedef enum {
        MBUS_SIG_TYPE__MBOX,
        MBUS_SIG_TYPE__VALUE,
        MBUS_SIG_TYPE__INVALID
} mbus_sig_type_t;

/** mbus signal information */
typedef struct {
        const char     *name;
        mbus_sig_perm_t permissions;
        mbus_sig_type_t type;
        size_t          size;
} mbus_sig_info_t;

/** mbus main object */
typedef struct mbus mbus_t;

/*==============================================================================
  Exported objects
==============================================================================*/

/*==============================================================================
  Exported functions
==============================================================================*/
extern mbus_errno_t mbus_daemon();
extern mbus_t      *mbus_new();
extern bool         mbus_delete(mbus_t*);
extern mbus_errno_t mbus_get_errno(mbus_t*);
extern int          mbus_get_number_of_signals(mbus_t*);
extern bool         mbus_get_signal_info(mbus_t*, size_t n, mbus_sig_info_t*);
extern bool         mbus_mbox_create(mbus_t*, const char*, size_t, mbus_sig_perm_t);
extern bool         mbus_mbox_delete(mbus_t*, const char*);
extern bool         mbus_mbox_send(mbus_t*, const char*, const void*);
extern bool         mbus_mbox_receive(mbus_t*, const char*, void*);
extern bool         mbus_value_create(mbus_t*, const char*, size_t, mbus_sig_perm_t);
extern bool         mbus_value_delete(mbus_t*, const char*);
extern bool         mbus_value_set(mbus_t*, const char*, const void*);
extern bool         mbus_value_get(mbus_t*, const char*, void*);

/*==============================================================================
  Exported inline functions
==============================================================================*/

#ifdef __cplusplus
}
#endif

#endif /* _MBUS_H_ */
/*==============================================================================
  End of file
==============================================================================*/
