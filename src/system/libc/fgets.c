/*=========================================================================*//**
@file    fgets.h

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

/*==============================================================================
  Include files
==============================================================================*/
#include <config.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include "lib/unarg.h"

/*==============================================================================
  Local macros
==============================================================================*/

/*==============================================================================
  Local object types
==============================================================================*/

/*==============================================================================
  Local function prototypes
==============================================================================*/

/*==============================================================================
  Local objects
==============================================================================*/

/*==============================================================================
  Exported objects
==============================================================================*/

/*==============================================================================
  External objects
==============================================================================*/

/*==============================================================================
  Function definitions
==============================================================================*/

//==============================================================================
/**
 * @brief Function receive string from selected file.
 *
 * @param[out] *str          buffer with string
 * @param[in]   size         buffer size
 * @param[in]  *stream       source stream
 *
 * @retval NULL if error, otherwise pointer to str
 */
//==============================================================================
char *fgets(char *str, int size, FILE *stream)
{
#if (__OS_PRINTF_ENABLE__ > 0)
        if (!str || size < 1 || !stream) {
                return NULL;
        }

        struct stat file_stat;
        if (fstat(stream, &file_stat) == 0) {
                if (file_stat.st_type == FILE_TYPE_PIPE || file_stat.st_type == FILE_TYPE_DRV) {

                        int   c = EOF;
                        char *p = str;

                        for (p = str, size--; size > 0; size--) {
                                if ((c = fgetc(stream)) == EOF)
                                        break;
                                *p++ = c;
                                if (c == '\n') {
                                        break;
                                }
                        }

                        *p = '\0';
                        if (p == str || c == EOF)
                                return NULL;
                        else
                                return str;

                } else {
                        i64_t fpos = ftell(stream);

                        int   l = 0;
                        char *p = str;
                        size--;

                        while (true) {
                                char cache[16];
                                int n = fread(cache, 1, sizeof(cache), stream);

                                for (int i = 0; size > 0 && i < n; size--, i++) {
                                        int c = cache[i];

                                        *p++ = c;
                                        fpos++;
                                        if (c == '\n') {
                                                goto finish;
                                        }
                                }

                                if (ferror(stream) || feof(stream)) {
                                        if (l == 0) {
                                                str = NULL;
                                        }

                                        break;
                                }
                        }

                        finish:
                        *p = '\0';

                        fseek(stream, fpos, SEEK_SET);

                        return str;
                }
        }
#else
        UNUSED_ARG3(str, size, stream);
#endif
        return NULL;
}

/*==============================================================================
  End of file
==============================================================================*/
