/*=========================================================================*//**
File     unistd.c

Author   Daniel Zorychta

Brief    unistd function implementation.

         Copyright (C) 2020 Daniel Zorychta <daniel.zorychta@gmail.com>

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
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

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
 * @brief  Function open file as file descriptor.
 *
 * @param  path         file path
 * @param  flags        file open flags
 * @param  mode         optional mode
 *
 * @return On success file descriptor is returned (fd >= 0), otherwise -1.
 */
//==============================================================================
fd_t open(const char *path, int flags, ...)
{
        const char *flag = "";

        if (flags == O_RDONLY) {
                flag = "r";
        } else if (flags == (O_WRONLY|O_CREAT|O_TRUNC)) {
                flag = "w";
        } else if (flags == (O_WRONLY|O_CREAT|O_APPEND)) {
                flag = "a";
        } else if (flags == O_RDWR) {
                flag = "r+";
        } else if (flags == (O_RDWR|O_CREAT|O_TRUNC)) {
                flag = "w+";
        } else if (flags == (O_RDWR|O_CREAT|O_APPEND)) {
                flag = "a+";
        }

        FILE *f = NULL;
        syscall(SYSCALL_FOPEN, &f, path, flag);

        return (f == NULL) ? -1 : (fd_t)f;
}

//==============================================================================
/**
 * @brief  Function close file descriptor.
 *
 * @param  fd           file descriptor
 *
 * @return On success 0 is returned.
 */
//==============================================================================
int close(fd_t fd)
{
        if (fd != -1) {
                FILE *f = (FILE*)fd;

                if (fd == 0) {
                        f = stdin;
                } else if (fd == 1) {
                        f = stdout;
                } else if (fd == 2) {
                        f = stderr;
                }

                int r = EOF;
                syscall(SYSCALL_FCLOSE, &r, f);
                return r;
        }

        return -1;
}

//==============================================================================
/**
 * @brief  Function read file.
 *
 * @param  fd           file descriptor
 * @param  buf          buffer
 * @param  count        bytes to read
 *
 * @return On success number of read bytes is returned, otherwise negative value.
 */
//==============================================================================
ssize_t read(fd_t fd, void *buf, size_t count)
{
        if (fd != -1) {
                FILE *f = (FILE*)fd;

                if (fd == 0) {
                        f = stdin;
                } else if (fd == 1) {
                        f = stdout;
                } else if (fd == 2) {
                        f = stderr;
                }

                size_t n = 0;
                size_t size = 1;
                syscall(SYSCALL_FREAD, &n, buf, &size, &count, f);

                int iserr = 0;
                int err = _builtinfunc(vfs_ferror, f, &iserr);
                return (iserr || err) ? _errno : (ssize_t)n;

        } else {
                return -1;
        }
}

//==============================================================================
/**
 * @brief  Function read file.
 *
 * @param  fd           file descriptor
 * @param  buf          buffer
 * @param  count        bytes to read
 *
 * @return On success number of written bytes is returned, otherwise negative value.
 */
//==============================================================================
ssize_t write(fd_t fd, const void *buf, size_t count)
{
        if (fd != -1) {
                FILE *f = (FILE*)fd;

                if (fd == 0) {
                        f = stdin;
                } else if (fd == 1) {
                        f = stdout;
                } else if (fd == 2) {
                        f = stderr;
                }

                size_t n = 0;
                size_t size = 1;
                syscall(SYSCALL_FWRITE, &n, buf, &size, &count, f);

                int iserr = 0;
                int err = _builtinfunc(vfs_ferror, f, &iserr);
                return (iserr || err) ? _errno : (ssize_t)n;

        } else {
                return -1;
        }
}

//==============================================================================
/**
 * @brief  Function set file position.
 *
 * @param  fd           file descriptor
 * @param  offset       offset
 * @param  whence       seek mode
 *
 * @return On success return file offset after operation, otherwise negative value
 *         on error.
 */
//==============================================================================
off_t lseek(fd_t fd, off_t offset, int whence)
{
        if (fd != -1) {
                FILE *f = (FILE*)fd;

                if (fd == 0) {
                        f = stdin;
                } else if (fd == 1) {
                        f = stdout;
                } else if (fd == 2) {
                        f = stderr;
                }

                size_t r = 1;
                i64_t seek = offset;
                syscall(SYSCALL_FSEEK, &r, f, &seek, &whence);

                if (!r) {
                        i64_t lseek = 0;
                        if (_builtinfunc(vfs_ftell, f, &lseek) == 0) {
                                return lseek;
                        }
                }
        }

        return -1;
}

//==============================================================================
/**
 * @brief  Function remove selected file.
 *
 * @param  pathname     file to remove
 *
 * @return On success 0 is returned.
 */
//==============================================================================
int unlink(const char *pathname)
{
        int r = EOF;
        syscall(SYSCALL_REMOVE, &r, pathname);
        return r;
}

/*==============================================================================
  End of file
==============================================================================*/
