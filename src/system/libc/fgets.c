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
 * @brief Function gets number of bytes from file
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
        if (!str || size < 2 || !stream) {
                return NULL;
        }

        struct stat file_stat;
        if (fstat(stream, &file_stat) == 0) {
                if (file_stat.st_type == FILE_TYPE_PIPE || file_stat.st_type == FILE_TYPE_DRV) {
                        int n = 0;
                        for (int i = 0; i < size - 1; i++) {
                                int m = fread(str + i, sizeof(char), 1, stream);
                                if (m == 0) {
                                        str[i] = '\0';
                                        return str;
                                } else {
                                        n += m;
                                }

                                if (ferror(stream) || feof(stream)) {
                                        if (n == 0) {
                                                return NULL;
                                        } else {
                                                str[i + 1] = '\0';
                                                return str;
                                        }
                                }

                                if (str[i] == '\n') {
                                        str[i + 1] = '\0';
                                        break;
                                }
                        }

                        return str;
                } else {
                        u64_t fpos = ftell(stream);

                        int n;
                        while ((n = fread(str, sizeof(char), size - 1, stream)) == 0) {
                                if (ferror(stream) || feof(stream)) {
                                        return NULL;
                                }
                        }

                        char *end;
                        if ((end = strchr(str, '\n'))) {
                                end++;
                                *end = '\0';
                        } else {
                                str[n] = '\0';
                        }

                        int len = strlen(str);

                        if (len != 0 && len < n && feof(stream))
                                clearerr(stream);

                        if (len == 0)
                                len = 1;

                        fseek(stream, fpos + len, SEEK_SET);

                        return str;
                }
        }
#else
        UNUSED_ARG1(str);
        UNUSED_ARG1(size);
        UNUSED_ARG1(stream);
#endif
        return NULL;
}

/*==============================================================================
  End of file
==============================================================================*/
