/*=========================================================================*//**
@file    errno.h

@author  Daniel Zorychta

@brief

@note    Copyright (C) 2015 Daniel Zorychta <daniel.zorychta@gmail.com>

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

#ifndef _KERNEL_ERRNO_H_
#define _KERNEL_ERRNO_H_

/*==============================================================================
  Include files
==============================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Exported macros
==============================================================================*/
#define ESUCC            0      /* Success */
#define EPERM            1      /* Operation not permitted */
#define ENOENT           2      /* No such file or directory */
#define ESRCH            3      /* No such process */
#define EIO              4      /* I/O error */
#define ENXIO            5      /* No such device or address */
#define E2BIG            6      /* Argument list too long */
#define ENOEXEC          7      /* Exec format error */
#define EAGAIN           8      /* Try again */
#define ENOMEM           9      /* Out of memory */
#define EACCES          10      /* Permission denied */
#define EFAULT          11      /* Bad address */
#define EBUSY           12      /* Device or resource busy */
#define EEXIST          13      /* File exists */
#define ENODEV          14      /* No such device */
#define ENOTDIR         15      /* Not a directory */
#define EISDIR          16      /* Is a directory */
#define EINVAL          17      /* Invalid argument */
#define EMFILE          18      /* Too many open files */
#define EFBIG           19      /* File too large */
#define ENOSPC          20      /* No space left on device */
#define ESPIPE          21      /* Illegal seek */
#define EROFS           22      /* Read-only file system */
#define EDOM            23      /* Math argument out of domain of func */
#define ERANGE          24      /* Math result not representable */
#define EILSEQ          25      /* Illegal byte sequence */
#define ENAMETOOLONG    26      /* File name too long */
#define ENOTEMPTY       27      /* Directory not empty */
#define EBADRQC         28      /* Invalid request code */
#define ETIME           29      /* Timer expired */
#define ENONET          30      /* Machine is not on the network */
#define EUSERS          31      /* Too many users */
#define EADDRINUSE      32      /* Address already in use */
#define ENOMEDIUM       33      /* No medium found */
#define EMEDIUMTYPE     34      /* Wrong medium type */
#define ECANCELED       35      /* Operation Canceled */
#define ENOTSUP         36      /* Not supported */
#define ENOSYS          37      /* Function not implemented */
#define _ENUMBER        38      /* total supported errors */

/*==============================================================================
  Exported object types
==============================================================================*/

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

#endif /* _KERNEL_ERRNO_H_ */
/*==============================================================================
  End of file
==============================================================================*/
