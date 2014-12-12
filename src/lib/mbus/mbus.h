/*=========================================================================*//**
@file    mbus.h

@author  Daniel Zorychta

@brief   Message Bus - user space IPC library.

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
        MBUS_ERRNO__NO_ERROR,                   //!< operation finished successfully
        MBUS_ERRNO__INVALID_OBJECT,             //!< invalid mbus object
        MBUS_ERRNO__INVALID_ARGUMENT,           //!< passed invalid argument
        MBUS_ERRNO__ACCESS_DENIED,              //!< no access to selected slot
        MBUS_ERRNO__TIMEOUT,                    //!< operation timeout
        MBUS_ERRNO__DAEMON_IS_ALREADY_STARTED,  //!< daemon is already started
        MBUS_ERRNO__DAEMON_NOT_RUNNING,         //!< daemon is not running
        MBUS_ERRNO__NOT_ENOUGH_FREE_MEMORY,     //!< not enough free memory
        MBUS_ERRNO__INTERNAL_ERROR,             //!< internal mbus error
        MBUS_ERRNO__SIGNAL_EXIST,               //!< signal exist
        MBUS_ERRNO__SIGNAL_NOT_EXIST,           //!< signal not exist
        MBUS_ERRNO__MBOX_IS_FULL,               //!< mbox is full, try again later
        MBUS_ERRNO__MBOX_EMPTY                  //!< mbox is empty, try again later
} mbus_errno_t;

/** mbus signal permissions */
typedef enum {
        MBUS_SIG_PERM__PRIVATE,                 //!< signal is private, can be used only by owner
        MBUS_SIG_PERM__READ,                    //!< signal can be read by others (owner has full access)
        MBUS_SIG_PERM__WRITE,                   //!< signal can be write by others (owner has full access)
        MBUS_SIG_PERM__READ_WRITE,              //!< signal can be read and write by others (owner has full access)
        MBUS_SIG_PERM__INVALID                  //!< invalid signal permissions
} mbus_sig_perm_t;

/** mbus signal type */
typedef enum {
        MBUS_SIG_TYPE__MBOX,                    //!< signal is mbox (queued mail-box) type
        MBUS_SIG_TYPE__VALUE,                   //!< signal is value type
        MBUS_SIG_TYPE__INVALID                  //!< invalid signal type
} mbus_sig_type_t;

/** mbus signal information */
typedef struct {
        const char     *name;                   //!< signal name reference
        mbus_sig_perm_t permissions;            //!< signal permissions
        mbus_sig_type_t type;                   //!< signal type
        size_t          size;                   //!< signal size (single item)
} mbus_sig_info_t;

/** mbus main object */
typedef struct mbus mbus_t;

/*==============================================================================
  Exported objects
==============================================================================*/

/*==============================================================================
  Exported functions
==============================================================================*/
//==============================================================================
/**
 * @brief  Main daemon function
 * @param  None
 * @return Function not return if daemon started correctly, otherwise specified
 *         error is returned.
 */
//==============================================================================
extern mbus_errno_t mbus_daemon();

//==============================================================================
/**
 * @brief  Create a new mbus connection. A new owner is created also.
 * @param  None
 * @return On success mbus object is returned, otherwise NULL.
 */
//==============================================================================
extern mbus_t *mbus_new();

//==============================================================================
/**
 * @brief  Delete created mbus connection
 * @param  mbus                 mbus connection
 * @param  delete_signals       delete all signals of this owner
 * @return On success true is returned. On error false and error number is set.
 */
//==============================================================================
extern bool mbus_delete(mbus_t *mbus, bool);

//==============================================================================
/**
 * @brief  Return last error number
 * @param  mbus                 mbus context
 * @return Last error number.
 */
//==============================================================================
extern mbus_errno_t mbus_get_errno(mbus_t *mbus);

//==============================================================================
/**
 * @brief  Return number of registered signals
 * @param  mbus                 mbus context
 * @return On success return number of signals, otherwise -1 and appropriate error
 *         number is set.
 */
//==============================================================================
extern int mbus_get_number_of_signals(mbus_t *mbus);

//==============================================================================
/**
 * @brief  Return information of selected signal
 * @param  mbus                 mbus context
 * @param  n                    n-element to get
 * @param  info                 pointer to the signal info object
 * @return On success true is returned. On error false and appropriate error
 *         number is set.
 */
//==============================================================================
extern bool mbus_get_signal_info(mbus_t *mbus, size_t n, mbus_sig_info_t *info);

//==============================================================================
/**
 * @brief  Create a new signal
 * @param  mbus                 mbus context
 * @param  name                 signal name
 * @param  size                 signal size
 * @param  type                 signal type
 * @param  permissions          signal permissions
 * @return On success true is returned. On error false and appropriate error
 *         number is set.
 */
//==============================================================================
extern bool mbus_signal_create(mbus_t *mbus, const char *name, size_t size,
                               mbus_sig_type_t type, mbus_sig_perm_t permissions);

//==============================================================================
/**
 * @brief  Delete signal
 * @param  mbus                 mbus context
 * @param  name                 name of signal to delete
 * @return On success true is returned. On error false and appropriate error
 *         number is set.
 */
//==============================================================================
extern bool mbus_signal_delete(mbus_t *mbus, const char *name);

//==============================================================================
/**
 * @brief  Force delete signal (even signal that caller is not owner)
 * @param  mbus                 mbus context
 * @param  name                 name of signal to delete
 * @return On success true is returned. On error false and appropriate error
 *         number is set.
 */
//==============================================================================
extern bool mbus_signal_force_delete(mbus_t *mbus, const char *name);

//==============================================================================
/**
 * @brief  Set selected signal
 * @param  mbus                 mbus context
 * @param  name                 name of signal
 * @param  data                 data to set
 * @return On success true is returned. On error false and appropriate error
 *         number is set.
 */
//==============================================================================
extern bool mbus_signal_set(mbus_t *mbus, const char *name, const void *data);

//==============================================================================
/**
 * @brief  Get data from signal
 * @param  mbus                 mbus context
 * @param  name                 name of signal
 * @param  data                 data destination
 * @return On success true is returned. On error false and appropriate error
 *         number is set.
 */
//==============================================================================
extern bool mbus_signal_get(mbus_t *mbus, const char *name, void *data);

//==============================================================================
/**
 * @brief  Check if signal exists
 * @param  mbus                 mbus context
 * @param  name                 signal name
 * @return If exists then 1 is returned.
 *         If not exists then 0 is returned.
 *         On error -1 is returned and appropriate error number is set.
 */
//==============================================================================
extern int mbus_signal_is_exist(mbus_t *mbus, const char *name);

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
