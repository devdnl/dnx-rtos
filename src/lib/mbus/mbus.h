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

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Exported macros
==============================================================================*/

/*==============================================================================
  Exported object types
==============================================================================*/
/** the mbus statuses  */
typedef enum {
        MBUS_STATUS_SUCCESS             = 0,    //!< An operation finished successfully
        MBUS_STATUS_SLOT_EXIST          = 1,    //!< A slot with the same name exist
        MBUS_STATUS_SLOT_NOT_EXIST      = 2,    //!< A slot not exist
        MBUS_STATUS_SLOT_EMPTY          = 3,    //!< A slot is empty (created but has no data)
        MBUS_STATUS_SLOT_HAS_MSG        = 4,    //!< A slot contains a object
        MBUS_STATUS_INVALID_OBJECT      = 5,    //!< An used object is invalid
        MBUS_STATUS_INVALID_ARGUMENTS   = 6,    //!< Passed arguments are not correct
        MBUS_STATUS_ERROR               = 7,    //!< A general operation error (e.g. not enough free memory)
        MBUS_STATUS_DAEMON_IS_RUNNING   = 8     //!< The daemon is already started and is not possible to start a new one
} mbus_status_t;

/** a slot object */
typedef struct mbus_slot mbus_slot_t;

/** a mbus main object */
typedef struct mbus mbus_t;

/** a slot ID type */
typedef u32_t mbus_slot_ID_t;

/** a slot object */
typedef struct mbus_slot {
        mbus_slot_ID_t  ID;
        u32_t           magic;
} mbus_slot_t;

/*==============================================================================
  Exported objects
==============================================================================*/

/*==============================================================================
  Exported functions
==============================================================================*/
extern mbus_status_t mbus_daemon                 ();
extern mbus_t       *mbus_bus_new                ();
extern mbus_status_t mbus_bus_delete             (mbus_t *mbus);
extern mbus_status_t mbus_bus_get_number_of_slots(mbus_t *mbus, uint *number);
extern mbus_status_t mbus_bus_get_slot_name      (mbus_t *mbus, uint n, char *name, size_t buf_len);
extern mbus_status_t mbus_slot_create            (mbus_t *mbus, const char *name, size_t msg_size, mbus_slot_t *slot);
extern mbus_status_t mbus_slot_destroy           (mbus_t *mbus, const char *name);
extern mbus_status_t mbus_slot_connect           (mbus_t *mbus, const char *name, mbus_slot_t *slot);
extern mbus_status_t mbus_slot_disconnect        (mbus_t *mbus, mbus_slot_t *slot);
extern mbus_status_t mbus_slot_has_msg           (mbus_t *mbus, mbus_slot_t *slot);
extern mbus_status_t mbus_slot_clear             (mbus_t *mbus, mbus_slot_t *slot);
extern mbus_status_t mbus_msg_send               (mbus_t *mbus, mbus_slot_t *slot, const void *msg);
extern mbus_status_t mbus_msg_receive            (mbus_t *mbus, mbus_slot_t *slot, void *msg);

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
