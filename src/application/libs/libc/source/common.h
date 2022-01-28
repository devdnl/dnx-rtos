/*==============================================================================
File     common.h

Author   Daniel Zorychta

Brief    Common header file.

	 Copyright (C) 2022 Daniel Zorychta <daniel.zorychta@gmail.com>

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

==============================================================================*/

/**
@defgroup _LIBC_SOURCE_COMMON_H_ _LIBC_SOURCE_COMMON_H_

Detailed Doxygen description.
*/
/**@{*/

#pragma once

/*==============================================================================
  Include files
==============================================================================*/
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Exported macros
==============================================================================*/
#ifndef errno
#define errno           (*_libc_errno)
#endif

/*==============================================================================
  Exported object types
==============================================================================*/
struct _libc_file {
        int fd;
        char *tmppath;

        struct {
                bool eof:1;
                bool error:1;
        } flag;
};

/*==============================================================================
  Exported objects
==============================================================================*/
extern struct _libc_file *_libc_stdin;
extern struct _libc_file *_libc_stdout;
extern struct _libc_file *_libc_stderr;
extern int   *_libc_errno;
extern void **_libc_global;

/*==============================================================================
  Exported functions
==============================================================================*/

/*==============================================================================
  Exported inline functions
==============================================================================*/

#ifdef __cplusplus
}
#endif

/**@}*/
/*==============================================================================
  End of file
==============================================================================*/
