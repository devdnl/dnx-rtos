/*=========================================================================*//**
@file    printx.h

@author  Daniel Zorychta

@brief   Basic print functions

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

#ifndef _PRINTX_H_
#define _PRINTX_H_

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Include files
==============================================================================*/
#include <stdarg.h>
#include <stddef.h>
#include "fs/vfs.h"
#include "config.h"

/*==============================================================================
  Exported macros
==============================================================================*/

/*==============================================================================
  Exported object types
==============================================================================*/

/*==============================================================================
  Exported objects
==============================================================================*/

/*==============================================================================
  Exported functions
==============================================================================*/
extern int         _snprintf       (char*, size_t, const char*, ...);
extern int         _fprintf        (FILE*, const char*, ...);
extern int         _vfprintf       (FILE*, const char*, va_list);
extern int         _vsnprintf      (char*, size_t, const char*, va_list);
extern const char *_strerror       (int);
extern void        _perror         (const char*);
extern int         _fputc          (int, FILE*);
extern int         _f_puts         (const char*, FILE*, bool);
extern int         _getc           (FILE*);
extern char       *_fgets          (char*, int, FILE*);
extern char       *_ctime_r        (const time_t *timer, const struct tm *tm, char *buf);
extern size_t      _strftime       (char*, size_t, const char*, const struct tm*);

/*==============================================================================
  Exported inline functions
==============================================================================*/

#ifdef __cplusplus
}
#endif

#endif /* _PRINTX_H_ */
/*==============================================================================
  End of file
==============================================================================*/
