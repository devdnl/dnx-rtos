/*=========================================================================*//**
@file    errno.h

@author  Daniel Zorychta

@brief

@note    Copyright (C) 2013 Daniel Zorychta <daniel.zorychta@gmail.com>

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

#ifndef _ERRNO_H_
#define _ERRNO_H_

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Include files
==============================================================================*/

/*==============================================================================
  Exported macros
==============================================================================*/
#define EPERM           1       /* Operation not permitted */
#define EOFIL           2       /* End of file */
#define ENOENT          3       /* No such file or directory */
#define ESRCH           4       /* No such process */
#define EIO             5       /* I/O error */
#define ENXIO           6       /* No such device or address */
#define E2BIG           7       /* Argument list too long */
#define ENOEXEC         8       /* Exec format error */
#define EAGAIN          9       /* Try again */
#define ENOMEM         10       /* Out of memory */
#define EACCES         11       /* Permission denied */
#define EFAULT         12       /* Bad address */
#define EBUSY          13       /* Device or resource busy */
#define EEXIST         14       /* File exists */
#define ENODEV         15       /* No such device */
#define ENOTDIR        16       /* Not a directory */
#define EISDIR         17       /* Is a directory */
#define EINVAL         18       /* Invalid argument */
#define EMFILE         19       /* Too many open files */
#define EFBIG          20       /* File too large */
#define ENOSPC         21       /* No space left on device */
#define ESPIPE         22       /* Illegal seek */
#define EROFS          23       /* Read-only file system */
#define EDOM           24       /* Math argument out of domain of func */
#define ERANGE         25       /* Math result not representable */
#define EILSEQ         26       /* Illegal byte sequence */
#define ENAMETOOLONG   27       /* File name too long */
#define ENOTEMPTY      28       /* Directory not empty */
#define EBADRQC        29       /* Invalid request code */
#define ETIME          30       /* Timer expired */
#define ENONET         31       /* Machine is not on the network */
#define EUSERS         32       /* Too many users */
#define EADDRINUSE     33       /* Address already in use */
#define ENOMEDIUM      34       /* No medium found */
#define EMEDIUMTYPE    35       /* Wrong medium type */
#define ECANCELED      36       /* Operation Canceled */
#define _ENUMBER       37       /* total supported errors */

/*==============================================================================
  Exported object types
==============================================================================*/
typedef int errno_t;

/*==============================================================================
  Exported objects
==============================================================================*/

/*==============================================================================
  Exported functions
==============================================================================*/

/*==============================================================================
  Exported inline functions
==============================================================================*/

#ifdef __cplusplus
}
#endif

#endif /* _ERRNO_H_ */
/*==============================================================================
  End of file
==============================================================================*/
