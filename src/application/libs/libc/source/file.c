/*=========================================================================*//**
@file    file.c

@author  Daniel Zorychta

@brief   File handling functions.

@note    Copyright (C) 2021 Daniel Zorychta <daniel.zorychta@gmail.com>

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
#include <stdio.h>

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
 * @brief Function opens file.
 *
 * The fopen() function opens the file whose name is the string pointed to by
 * <i>path</i> and associates a stream with it. The argument <i>mode</i> points
 * to a string beginning with one of the following sequences (possibly followed
 * by additional characters, as described below):<p>
 *
 * <b>r</b> - Open text file for reading. The stream is positioned at the
 * beginning of the file.<p>
 *
 * <b>r+</b> - Open for reading and writing. The stream is positioned at the
 * beginning of the file.<p>
 *
 * <b>w</b> - Truncate file to zero length or create text file for writing.
 * The stream is positioned at the beginning of the file.<p>
 *
 * <b>w+</b> - Open for reading and writing. The file is created if it does
 * not exist, otherwise it is truncated. The stream is positioned at the
 * beginning of the file.<p>
 *
 * <b>a</b> - Open for appending (writing at end of file). The file is
 * created if it does  not exist. The stream is positioned at the end of the
 * file.<p>
 *
 * <b>a+</b> - Open for reading and appending (writing at end of file). The
 * file is created if it does not exist. The initial file position for reading
 * is at the beginning of the file, but output is always appended to the end of
 * the file.
 *
 * @param path          path to file
 * @param mode          file open mode
 *
 * @exception | @ref EINVAL
 * @exception | @ref ENOMEM
 * @exception | @ref EACCES
 * @exception | @ref EISDIR
 * @exception | @ref ENOENT
 *
 * @return Upon successful completion fopen(), return a <b>FILE</b> pointer.
 * Otherwise, @ref NULL is returned and @ref errno is set to indicate the
 * error.
 */
//==============================================================================
FILE *_libc_fopen(const char *path, const char *mode)
{
        FILE *f = NULL;
        _libc_syscall(_LIBC_SYS_FOPEN, &f, path, mode);
        return f;
}

//==============================================================================
/**
 * @brief Function closes selected file.
 *
 * The fclose() function closes the created stream <i>file</i>.
 *
 * @param file          file to close
 *
 * @exception | @ref EINVAL
 * @exception | @ref ENOENT
 * @exception | @ref EFAULT
 *
 * @return Upon successful completion \b 0 is returned. Otherwise, @ref EOF is
 * returned and @ref errno is set to indicate the error. In either case any
 * further access (including another call to fclose()) to the stream results
 * in undefined behavior.
 */
//==============================================================================
int _libc_fclose(FILE *file)
{
        int r = EOF;
        _libc_syscall(_LIBC_SYS_FCLOSE, &r, file);
        return r;
}

/*==============================================================================
  End of file
==============================================================================*/
