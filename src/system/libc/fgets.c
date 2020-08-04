/*=========================================================================*//**
@file    fgets.h

@author  Daniel Zorychta

@brief   String get functions.

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
 * @brief Function receive string from selected file using buffer 'buf' of size
 *        'buflen'.
 *
 * @param[out] *str          buffer with string
 * @param[in]   size         buffer size
 * @param[in]  *stream       source stream
 * @param[in]  *buf          buffer
 * @param[in]   buflen       buffer length
 *
 * @retval NULL if error, otherwise pointer to str.
 */
//==============================================================================
char *fgets_buffered(char *str, int size, FILE *stream, char *buf, size_t buflen)
{
#if (__OS_PRINTF_ENABLE__ > 0)
        if (!str || size < 1 || !stream || !buf || !buflen) {
                return NULL;
        }

        struct stat file_stat;
        if (fstat(stream, &file_stat) == 0) {

                char *p = str;
                int   c = EOF;

                size--;

                if (S_ISFIFO(file_stat.st_mode) || S_ISDEV(file_stat.st_mode)) {

                        while ((c != '\n') && size--) {

                                c = fgetc(stream);
                                if (c == EOF) {
                                        break;
                                } else if (c == ETX) {
                                        str = NULL;
                                        break;
                                } else {
                                        *p++ = c;
                                }
                        }

                } else {
                        int n = 0;

                        i64_t fpos = ftell(stream);

                        fseek(stream, 0, SEEK_END);
                        i64_t end = ftell(stream);

                        fseek(stream, fpos, SEEK_SET);

                        while ((c != '\n') && (size > 0) && !(ferror(stream) || feof(stream))) {

                                n = fread(buf, 1, buflen, stream);

                                for (int i = 0; c != '\n' && size > 0 && i < n; size--, i++) {
                                        c    = buf[i];
                                        *p++ = c;
                                        fpos++;
                                }
                        }

                        if ((c == '\n' || size == 0) && fpos < end) {
                                fseek(stream, fpos, SEEK_SET);
                        }
                }

                *p = '\0';

                if (ferror(stream)) {
                        str = NULL;

                } else if (feof(stream)) {
                        str = (p == str) ? NULL : str;
                }

                return str;
        }
#else
        UNUSED_ARG3(str, size, stream);
#endif
        return NULL;
}

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
        char chunk[16];
        return fgets_buffered(str, size, stream, chunk, sizeof(chunk));
}

/*==============================================================================
  End of file
==============================================================================*/
