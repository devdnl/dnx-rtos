/*=========================================================================*//**
@file    errno.h

@author  Daniel Zorychta

@brief   Error codes.

@note    Copyright (C) 2015 Daniel Zorychta <daniel.zorychta@gmail.com>

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


*//*==========================================================================*/

/**
\defgroup errno-h <errno.h>

The file provides all error codes.

*/
/**@{*/

/*==============================================================================
  Include files
==============================================================================*/

/*==============================================================================
  Exported macros
==============================================================================*/
#define errno           (*_libc_errno)

#define ESUCC            0      //!< Success
#define EPERM            1      //!< Operation not permitted
#define ENOENT           2      //!< No such file or directory
#define ESRCH            3      //!< No such process
#define EIO              4      //!< I/O error
#define ENXIO            5      //!< No such device or address
#define E2BIG            6      //!< Argument list too long
#define ENOEXEC          7      //!< Execute format error
#define EAGAIN           8      //!< Try again
#define ENOMEM           9      //!< Out of memory
#define EACCES          10      //!< Permission denied
#define EFAULT          11      //!< Bad address
#define EBUSY           12      //!< Device or resource busy
#define EEXIST          13      //!< File exists
#define ENODEV          14      //!< No such device
#define ENOTDIR         15      //!< Not a directory
#define EISDIR          16      //!< Is a directory
#define EINVAL          17      //!< Invalid argument
#define EMFILE          18      //!< Too many open files
#define EFBIG           19      //!< File too large
#define ENOSPC          20      //!< No space left on device
#define ESPIPE          21      //!< Illegal seek
#define EROFS           22      //!< Read-only file system
#define EDOM            23      //!< Math argument out of domain of function
#define ERANGE          24      //!< Math result not representable
#define EILSEQ          25      //!< Illegal byte sequence
#define ENAMETOOLONG    26      //!< File name too long
#define ENOTEMPTY       27      //!< Directory not empty
#define EBADRQC         28      //!< Invalid request code
#define ETIME           29      //!< Timer expired
#define ENONET          30      //!< Machine is not on the network
#define EUSERS          31      //!< Too many users
#define EADDRINUSE      32      //!< Address already in use
#define ENOMEDIUM       33      //!< No medium found
#define EMEDIUMTYPE     34      //!< Wrong medium type
#define ECANCELED       35      //!< Operation Canceled
#define ENOTSUP         36      //!< Not supported
#define ENOSYS          37      //!< Function not implemented
#define ECONNABORTED    38      //!< Connection aborted (POSIX.1)
#define ECONNREFUSED    39      //!< Connection refused (POSIX.1)
#define ECONNRESET      40      //!< Connection reset (POSIX.1)
#define EISCONN         41      //!< Socket is connected (POSIX.1)
#define EALREADY        42      //!< Connection already in progress
#ifndef DOXYGEN
#define _ENUMBER        43      //!< total supported errors
#endif
#define __ELASTERROR    2000    /* Users can add values starting here */

/*==============================================================================
  Exported object types
==============================================================================*/
typedef int error_t;

/*==============================================================================
  Exported objects
==============================================================================*/
/**
 * @brief Error number variable.
 *
 * This variable contains error number. Value can be cleared by writing 0.
 * Variable exists independently in each application and is set by miscellaneous
 * system functions.
 */
//extern int _errno;
extern int *_libc_errno;

/*==============================================================================
  Exported functions
==============================================================================*/

/*==============================================================================
  Exported inline functions
==============================================================================*/

/**@}*/
/*==============================================================================
  End of file
==============================================================================*/
