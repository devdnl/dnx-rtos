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
#include <stdbool.h>
#include <string.h>
#include "common.h"

/*==============================================================================
  Local macros
==============================================================================*/
#ifndef O_RDONLY
#define O_RDONLY                                00
#endif

#ifndef O_WRONLY
#define O_WRONLY                                01
#endif

#ifndef O_RDWR
#define O_RDWR                                  02
#endif

#ifndef O_CREAT
#define O_CREAT                                 0100
#endif

#ifndef O_EXCL
#define O_EXCL                                  0200
#endif

#ifndef O_TRUNC
#define O_TRUNC                                 01000
#endif

#ifndef O_APPEND
#define O_APPEND                                02000
#endif

#ifndef O_NONBLOCK
#define O_NONBLOCK                              04000
#endif

/*==============================================================================
  Local object types
==============================================================================*/

/*==============================================================================
  Local function prototypes
==============================================================================*/
int _libc_fclose(FILE *file);
int unlink(const char *pathname);

/*==============================================================================
  Local objects
==============================================================================*/

/*==============================================================================
  Exported objects
==============================================================================*/

/*==============================================================================
  External objects
==============================================================================*/
extern void *_libc_malloc(size_t size);
extern void _libc_free(void *ptr);

/*==============================================================================
  Function definitions
==============================================================================*/

//==============================================================================
/**
 * @brief Function convert file open mode string to flags
 *
 * @param[in]  str      file open mode string
 *
 * @return Flags.
 */
//==============================================================================
static u32_t parse_flags(const char *str)
{
        if (strcmp("r", str) == 0 || strcmp("rb", str) == 0) {
                return O_RDONLY;
        }

        if (strcmp("r+", str) == 0 || strcmp("rb+", str) == 0 || strcmp("r+b", str) == 0) {
                return O_RDWR;
        }

        if (strcmp("w", str) == 0 || strcmp("wb", str) == 0) {
                return O_WRONLY | O_CREAT | O_TRUNC;
        }

        if (strcmp("w+", str) == 0 || strcmp("wb+", str) == 0 || strcmp("w+b", str) == 0) {
                return O_RDWR | O_CREAT | O_TRUNC;
        }

        if (strcmp("a", str) == 0 || strcmp("ab", str) == 0) {
                return O_WRONLY | O_CREAT | O_APPEND;
        }

        if (strcmp("a+", str) == 0 || strcmp("ab+", str) == 0 || strcmp("a+b", str) == 0) {
                return O_RDWR | O_CREAT | O_APPEND;
        }

        return O_RDONLY;
}

//==============================================================================
/**
 * @brief  Function open file as file descriptor.
 *
 * @param  path         file path
 * @param  flags        file open flags
 *
 * @return On success file descriptor is returned (fd >= 0), otherwise -1.
 */
//==============================================================================
int open(const char *path, int flags)
{
        int fd;
        int err = _libc_syscall(_LIBC_SYS_OPEN, &fd, path, &flags);
        return err ? -1 : fd;
}

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
        FILE *f = _libc_malloc(sizeof(*f));
        if (f) {
                f->tmppath = NULL;
                f->flag.eof = false;
                f->flag.error = false;
                f->fd = open(path, parse_flags(mode));
                if (f->fd >= 0) {
                        return f;
                }

                _libc_free(f);
        }

        return NULL;
}

//==============================================================================
/**
 * @brief  Function open file by using selected descriptor.
 *
 * @param  fd           file descriptor
 * @param  mode         file mode
 *
 * @return Upon successful completion fdopen(), return a <b>FILE</b> pointer.
 * Otherwise, @ref NULL is returned and @ref errno is set to indicate the
 * error.
 */
//==============================================================================
FILE *_libc_fdopen(int fd, const char *mode)
{
        (void)mode;

        if (fd < 0) {
                return NULL;
        }

        FILE *f = _libc_malloc(sizeof(*f));
        if (!f) {
                return NULL;
        }

        f->tmppath = NULL;
        f->flag.eof = false;
        f->flag.error = false;
        f->fd = fd;
        return f;
}

//==============================================================================
/**
 * @brief Function creates temporary file.
 *
 * The routine exist in dnx RTOS only for compatible reasons. Function in this
 * case do nothing.
 *
 * @note Function not supported.
 *
 * @return The tmpfile() function returns a stream descriptor, or @ref NULL
 * if a unique filename cannot be generated or the unique file cannot be opened.
 * In the latter case, @ref errno is set to indicate the error.
 */
//==============================================================================
FILE *_libc_tmpfile(void)
{
        uint64_t time_ms = 0;
        int err = _libc_syscall(_LIBC_SYS_GETUPTIMEMS, &time_ms);
        if (err) {
                return NULL;
        }

        char path[64];
        snprintf(path, sizeof(path), "/tmp/%llu", time_ms);

        FILE *f = _libc_fopen(path, "r+");
        if (!f) {
                f = _libc_fopen(path, "w");
                if (f) {
                        _libc_fclose(f);

                        f = _libc_fopen(path, "r+");
                        if (f) {
                                f->tmppath = strdup(path);
                                if (f->tmppath) {
                                        return f;
                                }

                                _libc_fclose(f);
                        }
                }

        } else {
                _libc_fclose(f);
        }

        return NULL;
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
int close(int fd)
{
        int err = _libc_syscall(_LIBC_SYS_CLOSE, &fd);
        return err ? -1 : 0;
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
        if (file) {
                int err = close(file->fd);
                if (!err) {

                        if (file->tmppath) {
                                unlink(file->tmppath);
                                _libc_free(file->tmppath);
                        }

                        _libc_free(file);
                        return 0;
                }
        } else {
                errno = EINVAL;
        }

        return EOF;
}

//==============================================================================
/**
 * @brief  Function read file.
 *
 * @param  fd           file descriptor
 * @param  buf          buffer
 * @param  count        bytes to read
 *
 * @return On success number of written bytes is returned, otherwise -1.
 */
//==============================================================================
ssize_t write(int fd, const void *buf, size_t count)
{
        size_t n = 0;
        int err = _libc_syscall(_LIBC_SYS_WRITE, &fd, buf, &count, &n);
        return err ? -1 : (ssize_t)n;
}

//==============================================================================
/**
 * @brief Function writes data to stream.
 *
 * The function fwrite() writes <i>count</i> elements of data, each <i>size</i>
 * bytes long, to the stream pointed to by <i>file</i>, obtaining them from the
 * location given by <i>ptr</i>.
 *
 * @param ptr           pointer to data
 * @param size          element size
 * @param count         number of elements
 * @param file          stream
 *
 * @exception | @ref EINVAL
 * @exception | @ref EACCES
 * @exception | @ref ENOENT
 * @exception | @ref EFAULT
 *
 * @return On success, fwrite() return the number of items written. This number
 * equals the number of bytes transferred only when <i>size</i> is 1. If an error
 * occurs, or the end of the file is reached, the return value is a short item
 * count (or zero).
 */
//==============================================================================
size_t _libc_fwrite(const void *ptr, size_t size, size_t count, FILE *file)
{
        if (file) {
                ssize_t n = write(file->fd, ptr, count * size);
                if (n < 0) {
                        file->flag.error = true;
                        return 0;
                }

                return n / size;
        } else {
                errno = EINVAL;
                return 0;
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
 * @return On success number of read bytes is returned, otherwise -1 value.
 */
//==============================================================================
ssize_t read(int fd, void *buf, size_t count)
{
        size_t n = 0;
        int err = _libc_syscall(_LIBC_SYS_READ, &fd, buf, &count, &n);
        return err ? -1 : (ssize_t)n;
}

//==============================================================================
/**
 * @brief Function reads data from stream.
 *
 * The function fread() reads <i>count</i> elements of data, each <i>size</i>
 * bytes long, from the stream pointed to by <i>file</i>, storing them at the
 * location given by <i>ptr</i>.
 *
 * @param ptr           pointer to data
 * @param size          element size
 * @param count         number of elements
 * @param file          stream
 *
 * @exception | @ref ENOENT
 * @exception | @ref ENOMEM
 * @exception | @ref EINVAL
 *
 * @return On success, fread() return the number of items read. This number
 * equals the number of bytes transferred only when <i>size</i> is 1. If an error
 * occurs, or the end of the file is reached, the return value is a short item
 * count (or zero).
 */
//==============================================================================
size_t _libc_fread(void *ptr, size_t size, size_t count, FILE *file)
{
        if (file) {
                ssize_t n = read(file->fd, ptr, count * size);
                if (n < 0) {
                        file->flag.error = true;
                        return 0;

                } else if (n < (ssize_t)(count * size)) {
                        file->flag.eof = true;

                } else {
                        file->flag.eof = false;
                }

                return n / size;
        } else {
                errno = EINVAL;
                return 0;
        }
}

//==============================================================================
/**
 * @brief  Function set file position.
 *
 * @param  fd           file descriptor
 * @param  offset       offset
 * @param  whence       seek mode (SEEK_SET, SEEK_CUR, SEEK_END)
 *
 * @return On success return file offset after operation, otherwise negative value
 *         on error.
 */
//==============================================================================
int64_t lseek64(int fd, int64_t offset, int whence)
{
        i64_t seek = offset;
        int err = _libc_syscall(_LIBC_SYS_SEEK64, &fd, &seek, &whence);
        return err ? -1 : seek;
}

//==============================================================================
/**
 * @brief  Function set file position.
 *
 * @param  fd           file descriptor
 * @param  offset       offset
 * @param  whence       seek mode (SEEK_SET, SEEK_CUR, SEEK_END)
 *
 * @return On success return file offset after operation, otherwise negative value
 *         on error.
 */
//==============================================================================
off_t lseek(int fd, off_t offset, int whence)
{
        return lseek64(fd, offset, whence);
}

//==============================================================================
/**
 * @brief Function sets file position indicator.
 *
 * The fseek() function sets the file position indicator for the stream
 * pointed to by <i>file</i>. The new position, measured in bytes, is obtained
 * by adding offset bytes to the position specified by whence. If whence is set
 * to @ref SEEK_SET, @ref SEEK_CUR, or @ref SEEK_END, the offset is
 * relative to the start of the file, the current position indicator, or
 * end-of-file, respectively. A successful call to the fseek() function
 * clears the end-of-file indicator for the stream and undoes any effects of the
 * ungetc() function on the same stream.
 *
 * @param file          stream
 * @param offset        offset
 * @param mode          seek mode
 *
 * @exception | @ref ENOENT
 * @exception | @ref EINVAL
 *
 * @return Upon successful completion, fseek() return 0. Otherwise, -1 is
 * returned and @ref errno is set to indicate the error.
 */
//==============================================================================
int _libc_fseek(FILE *file, int64_t offset, int mode)
{
        if (file) {
                int64_t seek = lseek64(file->fd, offset, mode);
                if (seek < 0) {
                        return -1;
                } else {
                        return 0;
                }

        } else {
                errno = EINVAL;
                return -1;
        }
}

//==============================================================================
/**
 * @brief Function returns file position indicator.
 *
 * The ftell() function obtains the current value of the file position
 * indicator for the stream pointed to by <i>file</i>.
 *
 * @param file          stream
 *
 * @exception | @ref EINVAL
 *
 * @return Upon successful completion, ftell() returns the current offset.
 * Otherwise, -1 is returned and @ref errno is set to indicate the error.
 */
//==============================================================================
int64_t _libc_ftell(FILE *file)
{
        if (file) {
                int64_t seek = lseek64(file->fd, 0, SEEK_CUR);
                if (seek < 0) {
                        return -1;
                } else {
                        return seek;
                }

        } else {
                errno = EINVAL;
                return -1;
        }
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
        int err = _libc_syscall(_LIBC_SYS_REMOVE, pathname);
        return err ? -1 : 0;
}

//==============================================================================
/**
 * @brief Function forces write buffers to stream.
 *
 * For output streams, fflush() forces a write of all buffered data for
 * the given output or update stream via the stream's underlying write function.
 * For input streams, fflush() discards any buffered data that has been
 * fetched from the underlying file. The open status of the stream is unaffected.
 *
 * @param file          stream
 *
 * @exception | @ref ENOENT
 * @exception | @ref EINVAL
 *
 * @return Upon successful completion 0 is returned. Otherwise, @ref EOF is
 * returned and @ref errno is set to indicate the error.
 */
//==============================================================================
int _libc_fflush(FILE *file)
{
        if (file) {
                int err = _libc_syscall(_LIBC_SYS_FLUSH, &file->fd);
                if (!err) {
                        return 0;
                }

        } else {
                errno = EINVAL;
        }

        return EOF;
}

//==============================================================================
/**
 * @brief Function tests the end-of-file indicator.
 *
 * The function feof() tests the end-of-file indicator for the stream
 * pointed to by <i>file</i>, returning nonzero if it is set. The end-of-file
 * indicator can only be cleared by the function clearerr().
 *
 * @param file          stream
 *
 * @exception | @ref ENOENT
 * @exception | @ref EINVAL
 *
 * @return Returns nonzero if @ref EOF indicator is set, otherwise 0 is
 * returned.
 */
//==============================================================================
int _libc_feof(FILE *file)
{
        if (file) {
                return file->flag.eof;

        } else {
                errno = EINVAL;
        }

        return 0;
}

//==============================================================================
/**
 * @brief Function clears end-of-file and error indicators.
 *
 * The function clearerr() clears the end-of-file and error indicators
 * for the stream pointed to by <i>file</i>.
 *
 * @param file          stream
 *
 * @exception | @ref ENOENT
 * @exception | @ref EINVAL
 */
//==============================================================================
void _libc_clearerr(FILE *file)
{
        if (file) {
                file->flag.eof = false;
                file->flag.error = false;
        }
}

//==============================================================================
/**
 * @brief Function tests error indicator.
 *
 * The function ferror() tests the error indicator for the stream pointed
 * to by <i>file</i>, returning nonzero if it is set.  The error indicator can
 * be reset only by the clearerr() function.
 *
 * @param file          stream
 *
 * @exception | @ref ENOENT
 * @exception | @ref EINVAL
 *
 * @return Returns nonzero value if the file stream has errors occurred,
 * 0 otherwise.
 */
//==============================================================================
int _libc_ferror(FILE *file)
{
        if (file) {
                if (file->flag.error) {
                        return 1;
                } else {
                        return 0;
                }
        } else {
                errno = EINVAL;
        }

        return 1;
}

//==============================================================================
/**
 * @brief Function return file number descriptor.
 *
 * This function returns the file descriptor associated with the stream
 * <i>stream</i>. If an error is detected (for example, if the stream is not
 * valid) or if stream does not do I/O to a file, fileno returns -1.
 *
 * @param fp    FILE pointer
 *
 * @return The fileno() function returns a file descriptor, or -1 on error.
 */
//==============================================================================
int _libc_fileno(FILE *fp)
{
        if (fp) {
                return fp->fd;
        } else {
                errno = EINVAL;
                return -1;
        }
}

//==============================================================================
/**
 * @brief Function remove selected file.
 *
 * remove() deletes a name from the file system. If the removed name was
 * the last link to a file and no processes have the file open, the file is
 * deleted and the space it was using is made available for reuse.<p>
 *
 * If the name referred to a FIFO, or device, the name is removed, but
 * processes which have the object open may continue to use it.
 *
 * @param path      path to file
 *
 * @exception | @ref ENOENT
 * @exception | @ref EINVAL
 *
 * @return On success, zero is returned. On error, -1 is returned, and
 * @ref errno is set appropriately.
 */
//==============================================================================
int _libc_remove(const char *path)
{
        int err = _libc_syscall(_LIBC_SYS_REMOVE, path);
        return err ? -1 : 0;
}

//==============================================================================
/**
 * @brief Function renames selected file.
 *
 * rename() renames a file. In contrast to standard C library this function
 * don't move files between directories if <i>new_name</i> is localized on other
 * filesystem than <i>old_name</i>, otherwise it's depending on filesystem.
 *
 * @param old_name      old file name
 * @param new_name      new file name
 *
 * @exception | @ref ENOENT
 * @exception | @ref EPERM
 * @exception | @ref EINVAL
 *
 * @return On success, zero is returned. On error, -1 is returned, and @ref errno
 * is set appropriately.
 */
//==============================================================================
int _libc_rename(const char *old_name, const char *new_name)
{
        int err = _libc_syscall(_LIBC_SYS_RENAME, old_name, new_name);
        return err ? -1 : 0;
}

//==============================================================================
/**
 * @brief Function gets file information.
 *
 * The fstat() function return information about a descriptor <i>fd</i>.
 *
 * @param fd            file descriptor
 * @param buf           file's information
 *
 * @exception | @ref EINVAL
 * @exception | @ref ENOMEM
 * @exception | @ref EACCES
 * @exception | @ref EEXIST
 * @exception | @ref ENOENT
 * @exception | @ref ENOSPC
 * @exception | ...
 *
 * @return On success, \b 0 is returned. On error, \b -1 is returned, and <b>errno</b>
 * is set appropriately.
 */
//==============================================================================
int _libc_fstat(int fd, struct stat *buf)
{
        if (fd >= 0) {
                int err = _libc_syscall(_LIBC_SYS_FSTAT, &fd, buf);
                return err ? -1 : 0;
        } else {
                errno = EINVAL;
                return -1;
        }
}

/*==============================================================================
  End of file
==============================================================================*/
