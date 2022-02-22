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
#include <libc/include/sys/types.h>

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

struct _libc_socket {
        int fd;
};

struct _libc_dir {
        int fd;
};

typedef struct {
        /** buffer used to store temporary time structure */
        struct tm _tmbuf;

        /** buffer used to store converted time to string */
        char _timestr[32];

        /** rand() seed */
        unsigned int _rand_seed;

        struct _libc_file *_stdin;
        struct _libc_file *_stdout;
        struct _libc_file *_stderr;
} _libc_app_ctx_t;

/*==============================================================================
  Exported objects
==============================================================================*/
extern int   *_libc_errno;
extern void **_libc_global;
extern void **_libc_app_ctx;

/*==============================================================================
  Exported functions
==============================================================================*/
extern void *_libc_malloc(size_t size);
extern void  _libc_free(void *mem);
extern int close(int fd);

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
