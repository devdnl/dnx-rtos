/*=========================================================================*//**
@file    stdio.h

@author  Daniel Zorychta

@brief   Standard IO library.

@note    Copyright (C) 2016 Daniel Zorychta <daniel.zorychta@gmail.com>

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

/**
\defgroup stdio-h <stdio.h>

The library provides general purpose IO functions.

*/
/**@{*/

#ifndef _STDIO_H_
#define _STDIO_H_

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Include files
==============================================================================*/
#include <config.h>
#include <sys/types.h>
#include <limits.h>
#include <stddef.h>
#include <stdarg.h>
#include <kernel/process.h>
#include <kernel/syscall.h>
#include <kernel/builtinfunc.h>
#include <kernel/errno.h>
#include <lib/unarg.h>

/*==============================================================================
  Exported macros
==============================================================================*/
/**
 * @brief Buffer size used in print and scan functions.
 *
 * Buffer size is configurable from system configuration.
 */
#define BUFSIZ                  __OS_STREAM_BUFFER_LENGTH__

/** @brief Set stream to fully buffered.@see setvbuf() */
#define _IOFBF                  0

/** @brief Set stream to line buffered. @see setvbuf() */
#define _IOLBF                  1

/** @brief Set stream to unbuffered. @see setvbuf() */
#define _IONBF                  2

#ifndef NULL
/** @brief Zero pointer value. */
#define NULL                    0
#endif

#ifndef EOF
/** @brief End Of File value. */
#define EOF                     (-1)
#endif

#ifndef SEEK_SET
/** @brief Set file position to specified value. @see fseek() */
#define SEEK_SET                0
#endif

#ifndef SEEK_CUR
/** @brief Set file position to current position plus offset. @see fseek() */
#define SEEK_CUR                1
#endif

#ifndef SEEK_END
/** @brief Set file position at the end of file plus offset. @see fseek() */
#define SEEK_END                2
#endif

/** @brief Maximum length of file name. */
#define FILENAME_MAX            255

/** @brief Maximum opened files. */
#define FOPEN_MAX               255

/** @brief Maximum number of temporary files. */
#define TMP_MAX                 32

/** @brief Maximum name length of temporary files. */
#define L_tmpnam                32

/*==============================================================================
  Exported object types
==============================================================================*/
#ifndef __FILE_TYPE_DEFINED__
#   ifdef DOXYGEN
        /**
         * @brief File object
         *
         * The type represent file object. Fields are private.
         */
        typedef struct {} FILE;
#   else
        typedef struct vfs_file FILE;
#   endif
#endif

/*==============================================================================
  Exported objects
==============================================================================*/
/** @brief Standard input file (one for each application) */
extern FILE *stdin;

/** @brief Standard output file (one for each application) */
extern FILE *stdout;

/** @brief Standard error file (one for each application) */
extern FILE *stderr;

/*==============================================================================
  Exported functions
==============================================================================*/

/*==============================================================================
  Exported inline functions
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
 *
 * @b Example
 * @code
        // ...

        FILE *file = fopen("/foo/bar", "w+");
        if (!file) {
               perror(NULL);
               // error handling
               // ...
        }

        // file handling

        // ...
   @endcode
 *
 * @see fclose()
 */
//==============================================================================
static inline FILE *fopen(const char *path, const char *mode)
{
        FILE *f = NULL;
        syscall(SYSCALL_FOPEN, &f, path, mode);
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
 *
 * @b Example
 * @code
        // ...

        FILE *file = fopen("/foo/bar", "w+");
        if (!file) {
               perror(NULL);
               // error handling
               // ...
        }

        // file handling

        // ...
   @endcode
 *
 * @see fopen()
 */
//==============================================================================
static inline int fclose(FILE *file)
{
        int r = EOF;
        syscall(SYSCALL_FCLOSE, &r, file);
        return r;
}

//==============================================================================
/**
 * @brief Function reopen already opened file to another.
 *
 * The freopen() function opens the file whose name is the string pointed to by
 * <i>path</i> and associates the stream pointed to by stream with it. The
 * original stream (if it exists) is closed. The <i>mode</i> argument is used just as
 * in the fopen() function.
 *
 * @param path          path to file
 * @param mode          file open mode
 * @param file          file to reopen
 *
 * @exception | @ref EINVAL
 * @exception | @ref ENOMEM
 * @exception | @ref EACCES
 * @exception | @ref ENOENT
 * @exception | @ref EFAULT
 *
 * @return Upon successful completion freopen(), return a <b>FILE</b> pointer.
 * Otherwise, @ref NULL is returned and @ref errno is set to indicate the
 * error.
 *
 * @b Example
 * @code
        // ...
        FILE *file = fopen("/foo/bar", "r");

        // ...

        file = freopen("/foo/bar", "w+", file);
        if (!file) {
               perror(NULL);
               // error handling
               // ...
        }

        // file handling

        // ...
   @endcode
 */
//==============================================================================
static inline FILE *freopen(const char *path, const char *mode, FILE *file)
{
        fclose(file);
        return fopen(path, mode);
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
 *
 * @b Example
 * @code
        #include <stdio.h>

        // ...

        FILE *file = fopen("/foo/bar", "w+");
        if (file) {
               char buffer[4] = {0, 1, 2, 3};
               if (fwrite(buffer, 1, sizeof(buffer), file) != 4) {
                       // error handling
               }

               fclose(file);
        } else {
               perror(NULL);
        }

        // ...
   @endcode
 *
 * @see fread()
 */
//==============================================================================
static inline size_t fwrite(const void *ptr, size_t size, size_t count, FILE *file)
{
        size_t s = 0;
        syscall(SYSCALL_FWRITE, &s, ptr, &size, &count, file);
        return s;
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
 *
 * @b Example
 * @code
        #include <stdio.h>

        // ...

        FILE *file = fopen("/foo/bar", "w+");
        if (file) {
               char buffer[4];
               if (fread(buffer, 1, sizeof(buffer), file) != 4) {
                       // error handling
               }

               fclose(file);
        }

        // ...
   @endcode
 *
 * @see fwrite()
 */
//==============================================================================
static inline size_t fread(void *ptr, size_t size, size_t count, FILE *file)
{
        size_t s = 0;
        syscall(SYSCALL_FREAD, &s, ptr, &size, &count, file);
        return s;
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
 *
 * @b Example
 * @code
        #include <stdio.h>

        // ...

        FILE *file = fopen("/foo/bar", "w+");
        if (file) {
               if (fseek(file, 100, SEEK_SET) != 0) {
                       // error handling
               }

               // file operations...

               fclose(file);
        }

        // ...
   @endcode
 *
 * @see fsetpos(), fgetpos(), ftell()
 */
//==============================================================================
static inline int fseek(FILE *file, i64_t offset, int mode)
{
        size_t r = 1;
        syscall(SYSCALL_FSEEK, &r, file, &offset, &mode);
        return r;
}

//==============================================================================
/**
 * @brief Function sets file position indicator.
 *
 * The fsetpos() function is alternate interfaces equivalent to fseek()
 * (with whence set to @ref SEEK_SET), setting and storing the current value
 * of the file offset into the object referenced by <i>pos</i>.
 *
 * @param file          stream
 * @param pos           offset
 *
 * @exception | @ref ENOENT
 * @exception | @ref EINVAL
 *
 * @return Upon successful completion, fsetpos() return 0. Otherwise, -1 is
 * returned and @ref errno is set to indicate the error.
 *
 * @b Example
 * @code
        #include <stdio.h>

        // ...

        FILE *file = fopen("/foo/bar", "w+");
        if (file) {
               static const fpos_t pos = 100;
               if (fsetpos(file, &pos) != 0) {
                       // error handling
               }

               // file operations...

               fclose(file);
        }

        // ...
   @endcode
 *
 * @see fseek(), fgetpos(), ftell()
 */
//==============================================================================
static inline int fsetpos(FILE *file, const fpos_t *pos)
{
        if (pos) {
                return fseek(file, *pos, SEEK_SET);
        } else {
                return EOF;
        }
}

//==============================================================================
/**
 * @brief Function sets file position indicator to the beginning of file.
 *
 * The rewind() function sets the file position indicator for the stream
 * pointed to by <i>file</i> to the beginning of the file. It is equivalent to:
 * <pre>(void) fseek(stream, 0L, SEEK_SET)</pre>
 *
 * @param file          stream
 *
 * @exception | @ref ENOENT
 * @exception | @ref EINVAL
 *
 * @return The rewind() function returns no value.
 *
 * @b Example
 * @code
        #include <stdio.h>

        // ...

        FILE *file = fopen("/foo/bar", "w+");
        if (file) {
               // file operations...

               rewind(file);

               // file operations...

               fclose(file);
        }

        // ...
   @endcode
 *
 * @see fseek(), fsetpos()
 */
//==============================================================================
static inline void rewind(FILE *file)
{
        fseek(file, 0, SEEK_SET);
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
 *
 * @b Example
 * @code
        #include <stdio.h>

        // ...

        FILE *file = fopen("/foo/bar", "w+");
        if (file) {
               // file operations...

               i64_t pos = ftell(file);
               if (pos == -1) {
                       // error handling
               }

               // file operations...

               fclose(file);
        }

        // ...
   @endcode
 *
 * @see fseek(), fsetpos(), fgetpos()
 */
//==============================================================================
static inline i64_t ftell(FILE *file)
{
        i64_t lseek = 0;
        _errno = _builtinfunc(vfs_ftell, file, &lseek);
        return _errno ? -1 : 0;
}

//==============================================================================
/**
 * @brief Function returns file position indicator.
 *
 * The fgetpos() function is alternate interface equivalent to ftell(),
 * storing the current value of the file offset from the object referenced by
 * <i>pos</i>.
 *
 * @param file          stream
 * @param pos           position object
 *
 * @exception | @ref EINVAL
 *
 * @return Upon successful completion, fgetpos() return 0. Otherwise, -1
 * is returned and @ref errno is set to indicate the error.
 *
 * @b Example
 * @code
        #include <stdio.h>

        // ...

        FILE *file = fopen("/foo/bar", "w+");
        if (file) {
               // file operations...

               fpos_t pos;
               if (fgetpos(file, &pos) == -1) {
                       // error handling
               }

               // file operations...

               fclose(file);
        }

        // ...
   @endcode
 *
 * @see fsetpos(), fseek(), ftell()
 */
//==============================================================================
static inline int fgetpos(FILE *file, fpos_t *pos)
{
        if (pos) {
                *pos = ftell(file);
                return (i64_t)*pos < 0 ? EOF : 0;
        } else {
                return EOF;
        }
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
 *
 * @b Example
 * @code
        #include <stdio.h>

        // ...

        FILE *file = fopen("/foo/bar", "w+");
        if (file) {
               // file operations...

               if (fflush(file) == EOF) {
                       // error handling
               }

               // file operations...

               fclose(file);
        }

        // ...
   @endcode
 */
//==============================================================================
static inline int fflush(FILE *file)
{
        int r = EOF;
        syscall(SYSCALL_FFLUSH, &r, file);
        return r;
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
 *
 * @b Example
 * @code
        #include <stdio.h>

        // ...

        FILE *file = fopen("/foo/bar", "w+");
        if (file) {
               // file operations...

               if (feof(file) != 0) {
                       // end-of-file handling
               }

               // file operations...

               fclose(file);
        }

        // ...
   @endcode
 *
 * @see clearerr()
 */
//==============================================================================
static inline int feof(FILE *file)
{
        int eof = 0;
        _errno = _builtinfunc(vfs_feof, file, &eof);
        return _errno | eof;
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
 *
 * @b Example
 * @code
        #include <stdio.h>

        // ...

        FILE *file = fopen("/foo/bar", "w+");
        if (file) {
               // file operations...

               if (ferror(file) {
                       // ...

                       clearerr(file);

                       // ...
               }

               // file operations...

               fclose(file);
        }

        // ...
   @endcode
 *
 * @see feof(), ferror()
 */
//==============================================================================
static inline void clearerr(FILE *file)
{
        _builtinfunc(vfs_clearerr, file);
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
 *
 * @b Example
 * @code
        #include <stdio.h>

        // ...

        FILE *file = fopen("/foo/bar", "w+");
        if (file) {
               // file operations...

               if (ferror(file) {
                       // ...

                       clearerr(file);

                       // ...
               }

               // file operations...

               fclose(file);
        }

        // ...
   @endcode
 *
 * @see clearerr()
 */
//==============================================================================
static inline int ferror(FILE *file)
{
        int err = 0;
        _errno = _builtinfunc(vfs_ferror, file, &err);
        return _errno | err;
}

//==============================================================================
/**
 * @brief Function produces message on the stdout.
 *
 * The routine perror() produces a message on the standard error output,
 * describing the last error encountered during a call to a system or
 * library function.  First (if <i>s</i> is not @ref NULL and <i>*s</i> is not
 * a null byte ('\0')) the argument string <i>s</i> is printed, followed by a
 * colon and a blank. Then the message and a new-line.
 *
 * @param s             string to print
 *
 * @exception | @ref ENOENT
 * @exception | @ref ENOMEM
 * @exception | @ref EINVAL
 *
 * @b Example
 * @code
        #include <stdio.h>

        // ...

        FILE *file = fopen("/foo/bar", "w+");
        if (!file) {
               perror("/foo/bar");

               // error handling
        }

        // ...
   @endcode
 *
 * @see strerror(), errno
 */
//==============================================================================
extern void perror(const char *s);

//==============================================================================
/**
 * @brief Function sets stream buffer mode.
 *
 * The routine exist in dnx RTOS only for compatible reasons. Function in this
 * case do nothing.
 *
 * @note Function not supported.
 *
 * @param file      stream
 * @param buffer    buffer
 *
 * @b Example
 * @code
        #include <stdio.h>

        // ...

        FILE *file = fopen("/foo/bar", "r");
        if (file) {
               char buffer[100];
               setbuf(file, buffer);

               // ...
        }
        // ...
   @endcode
 *
 * @see setvbuf()
 */
//==============================================================================
static inline void setbuf(FILE *file, char *buffer)
{
        (void) file;
        (void) buffer;
}

//==============================================================================
/**
 * @brief Function sets stream buffer mode.
 *
 * The routine exist in dnx RTOS only for compatible reasons. Function in this
 * case do nothing.
 *
 * @note Function not supported.
 *
 * @param file      stream
 * @param buffer    buffer
 * @param mode      buffer mode (@ref _IONBF, @ref _IOLBF, @ref _IOFBF)
 * @param size      buffer size
 *
 * @b Example
 * @code
        #include <stdio.h>

        // ...

        FILE *file = fopen("/foo/bar", "r");
        if (file) {
               char buffer[100];
               setvbuf(file, buffer, _IOFBF, 100);

               // ...
        }
        // ...
   @endcode
 *
 * @see setbuf()
 */
//==============================================================================
static inline int setvbuf(FILE *file, char *buffer, int mode, size_t size)
{
        (void) file;
        (void) buffer;
        (void) mode;
        (void) size;

        return 0;
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
 *
 * @b Example
 * @code
        #include <stdio.h>

        // ...

        FILE *file = tmpfile();
        if (file) {
               // ...
        }
        // ...
   @endcode
 */
//==============================================================================
static inline FILE *tmpfile(void)
{
        return NULL;
}

//==============================================================================
/**
 * @brief Function sets name of temporary file.
 *
 * The routine exist in dnx RTOS only for compatible reasons. Function in this
 * case do nothing.
 *
 * @note Function not supported.
 *
 * @param str       temporary file name or automatic generated if @ref NULL
 *
 * @return The tmpnam() function returns a pointer to a unique temporary
 * filename, or @ref NULL if a unique name cannot be generated.
 *
 * @b Example
 * @code
        #include <stdio.h>

        // ...

        char *tmpname = tmpnam(NULL);
        if (tmpname) {
               // ...
        }
        // ...
   @endcode
 */
//==============================================================================
static inline char *tmpnam(char *str)
{
        (void) str;

        return NULL;
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
 *
 * @b Example
 * @code
        #include <stdio.h>

        // ...

        remove("/foo/bar");

        // ...
   @endcode
 */
//==============================================================================
static inline int remove(const char *path)
{
#if __OS_ENABLE_REMOVE__ == _YES_
        int r = EOF;
        syscall(SYSCALL_REMOVE, &r, path);
        return r;
#else
        UNUSED_ARG1(path);
        _errno = ENOTSUP;
        return -1;
#endif
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
 *
 * @b Example
 * @code
        #include <stdio.h>

        // ...

        rename("/foo/bar", "/foo/baz");

        // ...
   @endcode
 */
//==============================================================================
static inline int rename(const char *old_name, const char *new_name)
{
#if __OS_ENABLE_RENAME__ == _YES_
        int r = EOF;
        syscall(SYSCALL_RENAME, &r, old_name, new_name);
        return r;
#else
        UNUSED_ARG2(old_name, new_name);
        _errno = ENOTSUP;
        return -1;
#endif
}

//==============================================================================
/**
 * @brief Function prints message according to format to selected stream.
 *
 * The function produce output according to a <i>format</i> as described below.
 * The function write output to <b>stream</b>.
 * An arguments are passed by list <i>arg</i>.<p>
 *
 * Supported flags:@n
 *   %%         - print % character
 *                @code printf("%%"); //=> % @endcode
 *
 *   %c         - print selected character (the \0 character is skipped)@n
 *                @code printf("_%c_", 'x');  //=> _x_ @endcode
 *                @code printf("_%c_", '\0'); //=> __ @endcode
 *
 *   %s         - print selected string
 *                @code printf("%s", "Foobar"); //=> Foobar @endcode
 *
 *   %.*s       - print selected string but only the length passed by argument
 *                @code printf("%.*s\n", 3, "Foobar"); //=> Foo @endcode
 *
 *   %.ns       - print selected string but only the n length
 *                @code printf("%.3s\n", "Foobar"); //=> Foo @endcode
 *
 *   %d, %i     - print decimal integer values
 *                @code printf("%d, %i", -5, 10); //=> -5, 10 @endcode
 *
 *   %u         - print unsigned decimal integer values
 *                @code printf("%u, %u", -1, 10); //=> 4294967295, 10 @endcode
 *
 *   %x, %X     - print hexadecimal values ('x' for lower characters, 'X' for upper characters)
 *                @code printf("0x%x, 0x%X", 0x5A, 0xfa); //=> 0x5a, 0xFA @endcode
 *
 *   %0x?       - print decimal (d, i, u) or hex (x, X) values with leading zeros.
 *                The number of characters (at least) is determined by x. The ?
 *                means d, i, u, x, or X value representations.
 *                @code printf("0x02X, 0x03X", 0x5, 0x1F43); //=> 0x05, 0x1F43 % @endcode
 *
 *   %f         - print float number. Note: make sure that input value is the float!
 *                @code printf("Foobar: %f", 1.0); //=> Foobar: 1.000000 @endcode
 *
 *   %l?        - print long long values, where ? means d, i, u, x, or X.
 *                @note Flag not supported.
 *
 *   %p         - print pointer
 *                @code printf("Pointer: %p", main); //=> Pointer: 0x4028B4 @endcode
 *
 * @param stream        output stream
 * @param format        formatting string
 * @param arg           argument list
 *
 * @exception | @ref ENOENT
 * @exception | @ref ENOMEM
 * @exception | @ref EINVAL
 *
 * @return Upon successful return, these functions return the number of
 * characters printed (excluding the null byte used to end output to strings).
 * If an output error is encountered, a negative value is returned.
 *
 * @b Example
 * @code
        #include <stdio.h>

        // ...

        va_list arg;

        // ...

        vfprintf(stdout, "foois %d; bar is 0x%x\n", arg);

        // ...
   @endcode
 *
 * @see printf(), vprintf(), fprintf(), vsprintf(), vsnprintf(), snprintf(), sprintf()
 */
//==============================================================================
extern int vfprintf(FILE *stream, const char *format, va_list arg);

//==============================================================================
/**
 * @brief Function prints message according to format to @ref stdout.
 *
 * The function produce output according to a <i>format</i> as described below.
 * The function write output to <b>stdout</b>, the standard output stream.<p>
 *
 * <b>%</b> - The flag starts interpreting of formatting. After character can be type a
 * numbers which determine e.g. buffer length, number of digits, etc. When
 * next character is <i>%</i> then per cent is printed.
 *
 * Supported flags:@n
 *   %%         - print % character
 *                @code printf("%%"); //=> % @endcode
 *
 *   %c         - print selected character (the \0 character is skipped)@n
 *                @code printf("_%c_", 'x');  //=> _x_ @endcode
 *                @code printf("_%c_", '\0'); //=> __ @endcode
 *
 *   %s         - print selected string
 *                @code printf("%s", "Foobar"); //=> Foobar @endcode
 *
 *   %.*s       - print selected string but only the length passed by argument
 *                @code printf("%.*s\n", 3, "Foobar"); //=> Foo @endcode
 *
 *   %.ns       - print selected string but only the n length
 *                @code printf("%.3s\n", "Foobar"); //=> Foo @endcode
 *
 *   %d, %i     - print decimal integer values
 *                @code printf("%d, %i", -5, 10); //=> -5, 10 @endcode
 *
 *   %u         - print unsigned decimal integer values
 *                @code printf("%u, %u", -1, 10); //=> 4294967295, 10 @endcode
 *
 *   %x, %X     - print hexadecimal values ('x' for lower characters, 'X' for upper characters)
 *                @code printf("0x%x, 0x%X", 0x5A, 0xfa); //=> 0x5a, 0xFA @endcode
 *
 *   %0x?       - print decimal (d, i, u) or hex (x, X) values with leading zeros.
 *                The number of characters (at least) is determined by x. The ?
 *                means d, i, u, x, or X value representations.
 *                @code printf("0x02X, 0x03X", 0x5, 0x1F43); //=> 0x05, 0x1F43 % @endcode
 *
 *   %f         - print float number. Note: make sure that input value is the float!
 *                @code printf("Foobar: %f", 1.0); //=> Foobar: 1.000000 @endcode
 *
 *   %l?        - print long long values, where ? means d, i, u, x, or X.
 *                @note Flag not supported.
 *
 *   %p         - print pointer
 *                @code printf("Pointer: %p", main); //=> Pointer: 0x4028B4 @endcode
 *
 * @param format        formatting string
 * @param ...           argument sequence
 *
 * @exception | @ref ENOENT
 * @exception | @ref ENOMEM
 * @exception | @ref EINVAL
 *
 * @return Upon successful return, these functions return the number of
 * characters printed (excluding the null byte used to end output to strings).
 * If an output error is encountered, a negative value is returned.
 *
 * @b Example
 * @code
        #include <stdio.h>

        // ...

        int foo = 12;
        int bar = 0x12;
        printf("foo is %d; bar is 0x%x\n", foo, bar);

        // ...
   @endcode
 *
 * @see vfprintf(), vprintf(), fprintf(), vsprintf(), vsnprintf(), snprintf(), sprintf()
 */
//==============================================================================
static inline int printf(const char *format, ...)
{
        va_list arg;
        va_start(arg, format);
        int status = vfprintf(stdout, format, arg);
        va_end(arg);
        return status;
}

//==============================================================================
/**
 * @brief Function prints message according to format to @ref stdout.
 *
 * The function produce output according to a <i>format</i> as described below.
 * The function write output to <b>stdout</b>, the standard output stream.
 * An arguments are passed by list <i>arg</i>.<p>
 *
 * <b>%</b> - The flag starts interpreting of formatting. After character can be type a
 * numbers which determine e.g. buffer length, number of digits, etc. When
 * next character is <i>%</i> then per cent is printed.<p>
 *
 * Supported flags:@n
 *   %%         - print % character
 *                @code printf("%%"); //=> % @endcode
 *
 *   %c         - print selected character (the \0 character is skipped)@n
 *                @code printf("_%c_", 'x');  //=> _x_ @endcode
 *                @code printf("_%c_", '\0'); //=> __ @endcode
 *
 *   %s         - print selected string
 *                @code printf("%s", "Foobar"); //=> Foobar @endcode
 *
 *   %.*s       - print selected string but only the length passed by argument
 *                @code printf("%.*s\n", 3, "Foobar"); //=> Foo @endcode
 *
 *   %.ns       - print selected string but only the n length
 *                @code printf("%.3s\n", "Foobar"); //=> Foo @endcode
 *
 *   %d, %i     - print decimal integer values
 *                @code printf("%d, %i", -5, 10); //=> -5, 10 @endcode
 *
 *   %u         - print unsigned decimal integer values
 *                @code printf("%u, %u", -1, 10); //=> 4294967295, 10 @endcode
 *
 *   %x, %X     - print hexadecimal values ('x' for lower characters, 'X' for upper characters)
 *                @code printf("0x%x, 0x%X", 0x5A, 0xfa); //=> 0x5a, 0xFA @endcode
 *
 *   %0x?       - print decimal (d, i, u) or hex (x, X) values with leading zeros.
 *                The number of characters (at least) is determined by x. The ?
 *                means d, i, u, x, or X value representations.
 *                @code printf("0x02X, 0x03X", 0x5, 0x1F43); //=> 0x05, 0x1F43 % @endcode
 *
 *   %f         - print float number. Note: make sure that input value is the float!
 *                @code printf("Foobar: %f", 1.0); //=> Foobar: 1.000000 @endcode
 *
 *   %l?        - print long long values, where ? means d, i, u, x, or X.
 *                @note Flag not supported.
 *
 *   %p         - print pointer
 *                @code printf("Pointer: %p", main); //=> Pointer: 0x4028B4 @endcode
 *
 * @param format        formatting string
 * @param arg           argument sequence list
 *
 * @exception | @ref ENOENT
 * @exception | @ref ENOMEM
 * @exception | @ref EINVAL
 *
 * @return Upon successful return, these functions return the number of
 * characters printed (excluding the null byte used to end output to strings).
 * If an output error is encountered, a negative value is returned.
 *
 * @b Example
 * @code
        #include <stdio.h>

        // ...

        va_list arg;

        // ...

        vprintf("foo is %d; bar is 0x%x\n", arg);

        // ...
   @endcode
 *
 * @see vfprintf(), printf(), fprintf(), vsprintf(), vsnprintf(), snprintf(), sprintf()
 */
//==============================================================================
static inline int vprintf(const char *format, va_list arg)
{
        return vfprintf(stdout, format, arg);
}

//==============================================================================
/**
 * @brief Function prints message according to format to selected stream.
 *
 * The function produce output according to a <i>format</i> as described below.
 * The function write output to <i>stream</i>.<p>
 *
 * Supported flags:@n
 *   %%         - print % character
 *                @code printf("%%"); //=> % @endcode
 *
 *   %c         - print selected character (the \0 character is skipped)@n
 *                @code printf("_%c_", 'x');  //=> _x_ @endcode
 *                @code printf("_%c_", '\0'); //=> __ @endcode
 *
 *   %s         - print selected string
 *                @code printf("%s", "Foobar"); //=> Foobar @endcode
 *
 *   %.*s       - print selected string but only the length passed by argument
 *                @code printf("%.*s\n", 3, "Foobar"); //=> Foo @endcode
 *
 *   %.ns       - print selected string but only the n length
 *                @code printf("%.3s\n", "Foobar"); //=> Foo @endcode
 *
 *   %d, %i     - print decimal integer values
 *                @code printf("%d, %i", -5, 10); //=> -5, 10 @endcode
 *
 *   %u         - print unsigned decimal integer values
 *                @code printf("%u, %u", -1, 10); //=> 4294967295, 10 @endcode
 *
 *   %x, %X     - print hexadecimal values ('x' for lower characters, 'X' for upper characters)
 *                @code printf("0x%x, 0x%X", 0x5A, 0xfa); //=> 0x5a, 0xFA @endcode
 *
 *   %0x?       - print decimal (d, i, u) or hex (x, X) values with leading zeros.
 *                The number of characters (at least) is determined by x. The ?
 *                means d, i, u, x, or X value representations.
 *                @code printf("0x02X, 0x03X", 0x5, 0x1F43); //=> 0x05, 0x1F43 % @endcode
 *
 *   %f         - print float number. Note: make sure that input value is the float!
 *                @code printf("Foobar: %f", 1.0); //=> Foobar: 1.000000 @endcode
 *
 *   %l?        - print long long values, where ? means d, i, u, x, or X.
 *                @note Flag not supported.
 *
 *   %p         - print pointer
 *                @code printf("Pointer: %p", main); //=> Pointer: 0x4028B4 @endcode
 *
 * @param stream        output stream
 * @param format        formatting string
 * @param ...           argument sequence
 *
 * @exception | @ref ENOENT
 * @exception | @ref ENOMEM
 * @exception | @ref EINVAL
 *
 * @return Upon successful return, these functions return the number of
 * characters printed (excluding the null byte used to end output to strings).
 * If an output error is encountered, a negative value is returned.
 *
 * @b Example
 * @code
        #include <stdio.h>

        // ...

        int foo = 12;
        int bar = 0x12;
        fprintf(stdout, "foo is %d; bar is 0x%x\n", foo, bar);

        // ...
   @endcode
 *
 * @see vfprintf(), printf(), vprintf(), vsprintf(), vsnprintf(), snprintf(), sprintf()
 */
//==============================================================================
static inline int fprintf(FILE *stream, const char *format, ...)
{
        va_list arg;
        va_start(arg, format);
        int status = vfprintf(stream, format, arg);
        va_end(arg);
        return status;
}

//==============================================================================
/**
 * @brief Function prints message according to format to buffer.
 *
 * The function produce output according to a <i>format</i> as described below.
 * The function write output pointed to by <i>bfr</i> of size <i>size</i>.
 * An arguments are passed by list <i>args</i>.<p>
 *
 * Supported flags:@n
 *   %%         - print % character
 *                @code printf("%%"); //=> % @endcode
 *
 *   %c         - print selected character (the \0 character is skipped)@n
 *                @code printf("_%c_", 'x');  //=> _x_ @endcode
 *                @code printf("_%c_", '\0'); //=> __ @endcode
 *
 *   %s         - print selected string
 *                @code printf("%s", "Foobar"); //=> Foobar @endcode
 *
 *   %.*s       - print selected string but only the length passed by argument
 *                @code printf("%.*s\n", 3, "Foobar"); //=> Foo @endcode
 *
 *   %.ns       - print selected string but only the n length
 *                @code printf("%.3s\n", "Foobar"); //=> Foo @endcode
 *
 *   %d, %i     - print decimal integer values
 *                @code printf("%d, %i", -5, 10); //=> -5, 10 @endcode
 *
 *   %u         - print unsigned decimal integer values
 *                @code printf("%u, %u", -1, 10); //=> 4294967295, 10 @endcode
 *
 *   %x, %X     - print hexadecimal values ('x' for lower characters, 'X' for upper characters)
 *                @code printf("0x%x, 0x%X", 0x5A, 0xfa); //=> 0x5a, 0xFA @endcode
 *
 *   %0x?       - print decimal (d, i, u) or hex (x, X) values with leading zeros.
 *                The number of characters (at least) is determined by x. The ?
 *                means d, i, u, x, or X value representations.
 *                @code printf("0x02X, 0x03X", 0x5, 0x1F43); //=> 0x05, 0x1F43 % @endcode
 *
 *   %f         - print float number. Note: make sure that input value is the float!
 *                @code printf("Foobar: %f", 1.0); //=> Foobar: 1.000000 @endcode
 *
 *   %l?        - print long long values, where ? means d, i, u, x, or X.
 *                @note Flag not supported.
 *
 *   %p         - print pointer
 *                @code printf("Pointer: %p", main); //=> Pointer: 0x4028B4 @endcode
 *
 * @param bfr           buffer which output was produced
 * @param size          buffer size
 * @param format        formatting string
 * @param args          argument list
 *
 * @exception | @ref ENOENT
 * @exception | @ref ENOMEM
 * @exception | @ref EINVAL
 *
 * @return Upon successful return, these functions return the number of
 * characters printed (excluding the null byte used to end output to strings).
 * If an output error is encountered, a negative value is returned.
 *
 * @b Example
 * @code
        #include <stdio.h>

        // ...

        va_list args;

        // ...

        char buffer[20];
        vsnprintf(buffer, 20, "foo is %d; bar is 0x%x\n", args);

        // ...
   @endcode
 *
 * @see vfprintf(), printf(), vprintf(), fprintf(), vsprintf(), snprintf(), sprintf()
 */
//==============================================================================
extern int vsnprintf(char *bfr, size_t size, const char *format, va_list args);

//==============================================================================
/**
 * @brief Function prints message according to format to buffer.
 *
 * The function produce output according to a <i>format</i> as described below.
 * The function write output pointed to by <i>s</i> of size <i>n</i>.<p>
 *
 * Supported flags:@n
 *   %%         - print % character
 *                @code printf("%%"); //=> % @endcode
 *
 *   %c         - print selected character (the \0 character is skipped)@n
 *                @code printf("_%c_", 'x');  //=> _x_ @endcode
 *                @code printf("_%c_", '\0'); //=> __ @endcode
 *
 *   %s         - print selected string
 *                @code printf("%s", "Foobar"); //=> Foobar @endcode
 *
 *   %.*s       - print selected string but only the length passed by argument
 *                @code printf("%.*s\n", 3, "Foobar"); //=> Foo @endcode
 *
 *   %.ns       - print selected string but only the n length
 *                @code printf("%.3s\n", "Foobar"); //=> Foo @endcode
 *
 *   %d, %i     - print decimal integer values
 *                @code printf("%d, %i", -5, 10); //=> -5, 10 @endcode
 *
 *   %u         - print unsigned decimal integer values
 *                @code printf("%u, %u", -1, 10); //=> 4294967295, 10 @endcode
 *
 *   %x, %X     - print hexadecimal values ('x' for lower characters, 'X' for upper characters)
 *                @code printf("0x%x, 0x%X", 0x5A, 0xfa); //=> 0x5a, 0xFA @endcode
 *
 *   %0x?       - print decimal (d, i, u) or hex (x, X) values with leading zeros.
 *                The number of characters (at least) is determined by x. The ?
 *                means d, i, u, x, or X value representations.
 *                @code printf("0x02X, 0x03X", 0x5, 0x1F43); //=> 0x05, 0x1F43 % @endcode
 *
 *   %f         - print float number. Note: make sure that input value is the float!
 *                @code printf("Foobar: %f", 1.0); //=> Foobar: 1.000000 @endcode
 *
 *   %l?        - print long long values, where ? means d, i, u, x, or X.
 *                @note Flag not supported.
 *
 *   %p         - print pointer
 *                @code printf("Pointer: %p", main); //=> Pointer: 0x4028B4 @endcode
 *
 * @param s             buffer where output was produced
 * @param n             buffer size
 * @param format        formatting string
 * @param ...           argument sequence
 *
 * @exception | @ref ENOENT
 * @exception | @ref ENOMEM
 * @exception | @ref EINVAL
 *
 * @return Upon successful return, these functions return the number of
 * characters printed (excluding the null byte used to end output to strings).
 * If an output error is encountered, a negative value is returned.
 *
 * @b Example
 * @code
        #include <stdio.h>

        // ...

        char buffer[20];
        int foo = 12;
        int bar = 0x12;
        snprintf(buffer, 20, "foo is %d; bar is 0x%x\n", foo, bar);

        // ...
   @endcode
 *
 * @see vfprintf(), printf(), vprintf(), vsprintf(), fprintf(), vsnprintf(), sprintf()
 */
//==============================================================================
static inline int snprintf(char *s, size_t n, const char *format, ...)
{
        va_list arg;
        va_start(arg, format);
        int status = vsnprintf(s, n, format, arg);
        va_end(arg);
        return status;
}

//==============================================================================
/**
 * @brief Function prints message according to format to buffer.
 *
 * The function produce output according to a <i>format</i> as described below.
 * The function write output pointed to by <i>s</i>. Buffer must be enough long
 * to store produced data.<p>
 *
 * Supported flags:@n
 *   %%         - print % character
 *                @code printf("%%"); //=> % @endcode
 *
 *   %c         - print selected character (the \0 character is skipped)@n
 *                @code printf("_%c_", 'x');  //=> _x_ @endcode
 *                @code printf("_%c_", '\0'); //=> __ @endcode
 *
 *   %s         - print selected string
 *                @code printf("%s", "Foobar"); //=> Foobar @endcode
 *
 *   %.*s       - print selected string but only the length passed by argument
 *                @code printf("%.*s\n", 3, "Foobar"); //=> Foo @endcode
 *
 *   %.ns       - print selected string but only the n length
 *                @code printf("%.3s\n", "Foobar"); //=> Foo @endcode
 *
 *   %d, %i     - print decimal integer values
 *                @code printf("%d, %i", -5, 10); //=> -5, 10 @endcode
 *
 *   %u         - print unsigned decimal integer values
 *                @code printf("%u, %u", -1, 10); //=> 4294967295, 10 @endcode
 *
 *   %x, %X     - print hexadecimal values ('x' for lower characters, 'X' for upper characters)
 *                @code printf("0x%x, 0x%X", 0x5A, 0xfa); //=> 0x5a, 0xFA @endcode
 *
 *   %0x?       - print decimal (d, i, u) or hex (x, X) values with leading zeros.
 *                The number of characters (at least) is determined by x. The ?
 *                means d, i, u, x, or X value representations.
 *                @code printf("0x02X, 0x03X", 0x5, 0x1F43); //=> 0x05, 0x1F43 % @endcode
 *
 *   %f         - print float number. Note: make sure that input value is the float!
 *                @code printf("Foobar: %f", 1.0); //=> Foobar: 1.000000 @endcode
 *
 *   %l?        - print long long values, where ? means d, i, u, x, or X.
 *                @note Flag not supported.
 *
 *   %p         - print pointer
 *                @code printf("Pointer: %p", main); //=> Pointer: 0x4028B4 @endcode
 *
 * @param s             buffer which output was produced
 * @param format        formatting string
 * @param ...           argument sequence
 *
 * @exception | @ref ENOENT
 * @exception | @ref ENOMEM
 * @exception | @ref EINVAL
 *
 * @return Upon successful return, these functions return the number of
 * characters printed (excluding the null byte used to end output to strings).
 * If an output error is encountered, a negative value is returned.
 *
 * @b Example
 * @code
        #include <stdio.h>

        // ...

        int foo = 12;
        int bar = 0x12;
        char buffer[20];
        sprintf(buffer, "foo is %d; bar is 0x%x\n", foo, bar);

        // ...
   @endcode
 *
 * @see vfprintf(), printf(), vprintf(), vsprintf(), fprintf(), vsnprintf(), snprintf()
 */
//==============================================================================
static inline int sprintf(char *s, const char *format, ...)
{
        va_list arg;
        va_start(arg, format);
        int status = vsnprintf(s, UINT16_MAX, format, arg);
        va_end(arg);
        return status;
}

//==============================================================================
/**
 * @brief Function prints message according to format to buffer.
 *
 * The function produce output according to a <i>format</i> as described below.
 * The function write output pointed to by <i>s</i>. Buffer must be enough long
 * to store produced data. An arguments are passed by list <i>arg</i>.<p>
 *
 * Supported flags:@n
 *   %%         - print % character
 *                @code printf("%%"); //=> % @endcode
 *
 *   %c         - print selected character (the \0 character is skipped)@n
 *                @code printf("_%c_", 'x');  //=> _x_ @endcode
 *                @code printf("_%c_", '\0'); //=> __ @endcode
 *
 *   %s         - print selected string
 *                @code printf("%s", "Foobar"); //=> Foobar @endcode
 *
 *   %.*s       - print selected string but only the length passed by argument
 *                @code printf("%.*s\n", 3, "Foobar"); //=> Foo @endcode
 *
 *   %.ns       - print selected string but only the n length
 *                @code printf("%.3s\n", "Foobar"); //=> Foo @endcode
 *
 *   %d, %i     - print decimal integer values
 *                @code printf("%d, %i", -5, 10); //=> -5, 10 @endcode
 *
 *   %u         - print unsigned decimal integer values
 *                @code printf("%u, %u", -1, 10); //=> 4294967295, 10 @endcode
 *
 *   %x, %X     - print hexadecimal values ('x' for lower characters, 'X' for upper characters)
 *                @code printf("0x%x, 0x%X", 0x5A, 0xfa); //=> 0x5a, 0xFA @endcode
 *
 *   %0x?       - print decimal (d, i, u) or hex (x, X) values with leading zeros.
 *                The number of characters (at least) is determined by x. The ?
 *                means d, i, u, x, or X value representations.
 *                @code printf("0x02X, 0x03X", 0x5, 0x1F43); //=> 0x05, 0x1F43 % @endcode
 *
 *   %f         - print float number. Note: make sure that input value is the float!
 *                @code printf("Foobar: %f", 1.0); //=> Foobar: 1.000000 @endcode
 *
 *   %l?        - print long long values, where ? means d, i, u, x, or X.
 *                @note Flag not supported.
 *
 *   %p         - print pointer
 *                @code printf("Pointer: %p", main); //=> Pointer: 0x4028B4 @endcode
 *
 * @param s             buffer which output was produced
 * @param format        formatting string
 * @param arg           argument sequence list
 *
 * @exception | @ref ENOENT
 * @exception | @ref ENOMEM
 * @exception | @ref EINVAL
 *
 * @return Upon successful return, these functions return the number of
 * characters printed (excluding the null byte used to end output to strings).
 * If an output error is encountered, a negative value is returned.
 *
 * @b Example
 * @code
        #include <stdio.h>

        // ...

        va_list args;

        // ...

        char buffer[20];
        vsprintf(buffer, "foo is %d; bar is 0x%x\n", args);

        // ...
   @endcode
 *
 * @see vfprintf(), printf(), vprintf(), fprintf(), vsnprintf(), snprintf(), sprintf()
 */
//==============================================================================
static inline int vsprintf(char *s, const char *format, va_list arg)
{
        return vsnprintf(s, UINT16_MAX, format, arg);
}

//==============================================================================
/**
 * @brief Function scans input according to format.
 *
 * The function scans input according to format as described below. This format
 * may contain conversion specifications; the results from such conversions,
 * if any, are stored in the locations pointed to by the pointer arguments that
 * follow format. Each pointer argument must be of a type that is appropriate
 * for the value returned by the corresponding conversion specification.
 * An arguments are passed by list <i>args</i>.<p>
 *
 * <b>%</b> - The flag starts interpreting of formatting. After character can be type a
 * numbers which determine e.g. buffer length, number of digits, etc.<p>
 *
 * <b>c</b> - Scans single character.<p>
 *
 * <b>s</b> - Scans string.<p>
 *
 * <b>d, i</b> - Scans signed <i>int</i> type number.<p>
 *
 * <b>u</b> - Scans unsigned type number.<p>
 *
 * <b>x, X</b> - Scans value in HEX formatting.<p>
 *
 * <b>o</b> - Scans value in Octal formatting.<p>
 *
 * <b>f, F, g, G</b> - Scans float value.
 *
 * @param s             input string (must be <i>null</i> terminated)
 * @param format        formatting string
 * @param args          argument sequence list
 *
 * @exception | @ref ENOMEM
 * @exception | @ref EINVAL
 *
 * @return The function return the number of input items successfully matched
 * and assigned, which can be fewer than provided for, or even zero in the event
 * of an early matching failure.<p>
 *
 * The value @ref EOF is returned if the end of input is reached before either
 * the first successful conversion or a matching failure occurs. @ref EOF is
 * also returned if a read error occurs, in which case the error indicator for
 * the stream is set, and @ref errno is set indicate the error.
 *
 * @b Example
 * @code
        #include <stdio.h>

        // ...

        char *buffer = "12, 1256";
        va_list arg;
        vsscanf("%i, %i", arg);

        // ...
   @endcode
 *
 * @see vfscanf(), scanf(), vscanf(), fscanf(), sscanf()
 */
//==============================================================================
extern int vsscanf(const char *s, const char *format, va_list args);

//==============================================================================
/**
 * @brief Function scans input according to format.
 *
 * The function scans input according to format as described below. This format
 * may contain conversion specifications; the results from such conversions,
 * if any, are stored in the locations pointed to by the pointer arguments that
 * follow format. Each pointer argument must be of a type that is appropriate
 * for the value returned by the corresponding conversion specification.
 * An arguments are passed by list <i>arg</i>.<p>
 *
 * <b>%</b> - The flag starts interpreting of formatting. After character can be type a
 * numbers which determine e.g. buffer length, number of digits, etc.<p>
 *
 * <b>c</b> - Scans single character.<p>
 *
 * <b>s</b> - Scans string.<p>
 *
 * <b>d, i</b> - Scans signed <i>int</i> type number.<p>
 *
 * <b>u</b> - Scans unsigned type number.<p>
 *
 * <b>x, X</b> - Scans value in HEX formatting.<p>
 *
 * <b>o</b> - Scans value in Octal formatting.<p>
 *
 * <b>f, F, g, G</b> - Scans float value.
 *
 * @param stream        input file
 * @param format        formatting string
 * @param arg           argument sequence list
 *
 * @exception | @ref ENOMEM
 * @exception | @ref EINVAL
 *
 * @return The function return the number of input items successfully matched
 * and assigned, which can be fewer than provided for, or even zero in the event
 * of an early matching failure.<p>
 *
 * The value @ref EOF is returned if the end of input is reached before either
 * the first successful conversion or a matching failure occurs. @ref EOF is
 * also returned if a read error occurs, in which case the error indicator for
 * the stream is set, and @ref errno is set indicate the error.
 *
 * @b Example
 * @code
        #include <stdio.h>

        // ...

        va_list arg;

        // ...

        vfscanf(stdin, "%i%i", arg);

        // ...
   @endcode
 *
 * @see vsscanf(), scanf(), vscanf(), fscanf(), sscanf()
 */
//==============================================================================
extern int vfscanf(FILE *stream, const char *format, va_list arg);

//==============================================================================
/**
 * @brief Function scans input according to format.
 *
 * The function scans input according to format as described below. This format
 * may contain conversion specifications; the results from such conversions,
 * if any, are stored in the locations pointed to by the pointer arguments that
 * follow format. Each pointer argument must be of a type that is appropriate
 * for the value returned by the corresponding conversion specification.<p>
 *
 * <b>%</b> - The flag starts interpreting of formatting. After character can be type a
 * numbers which determine e.g. buffer length, number of digits, etc.<p>
 *
 * <b>c</b> - Scans single character.<p>
 *
 * <b>s</b> - Scans string.<p>
 *
 * <b>d, i</b> - Scans signed <i>int</i> type number.<p>
 *
 * <b>u</b> - Scans unsigned type number.<p>
 *
 * <b>x, X</b> - Scans value in HEX formatting.<p>
 *
 * <b>o</b> - Scans value in Octal formatting.<p>
 *
 * <b>f, F, g, G</b> - Scans float value.
 *
 * @param format        formatting string
 * @param ...           argument sequence list
 *
 * @exception | @ref ENOMEM
 * @exception | @ref EINVAL
 *
 * @return The function return the number of input items successfully matched
 * and assigned, which can be fewer than provided for, or even zero in the event
 * of an early matching failure.<p>
 *
 * The value @ref EOF is returned if the end of input is reached before either
 * the first successful conversion or a matching failure occurs. @ref EOF is
 * also returned if a read error occurs, in which case the error indicator for
 * the stream is set, and @ref errno is set indicate the error.
 *
 * @b Example
 * @code
        #include <stdio.h>

        // ...

        int foo, bar;
        scanf("%i%i", &foo, &bar);

        // ...
   @endcode
 *
 * @see vsscanf(), vfscanf(), vscanf(), fscanf(), sscanf()
 */
//==============================================================================
static inline int scanf(const char *format, ...)
{
        va_list arg;
        va_start(arg, format);
        int status = vfscanf(stdin, format, arg);
        va_end(arg);
        return status;
}

//==============================================================================
/**
 * @brief Function scans input according to format.
 *
 * The function scans input according to format as described below. This format
 * may contain conversion specifications; the results from such conversions,
 * if any, are stored in the locations pointed to by the pointer arguments that
 * follow format. Each pointer argument must be of a type that is appropriate
 * for the value returned by the corresponding conversion specification.
 * An arguments are passed by list <i>arg</i>.<p>
 *
 * <b>%</b> - The flag starts interpreting of formatting. After character can be type a
 * numbers which determine e.g. buffer length, number of digits, etc.<p>
 *
 * <b>c</b> - Scans single character.<p>
 *
 * <b>s</b> - Scans string.<p>
 *
 * <b>d, i</b> - Scans signed <i>int</i> type number.<p>
 *
 * <b>u</b> - Scans unsigned type number.<p>
 *
 * <b>x, X</b> - Scans value in HEX formatting.<p>
 *
 * <b>o</b> - Scans value in Octal formatting.<p>
 *
 * <b>f, F, g, G</b> - Scans float value.
 *
 * @param format        formatting string
 * @param arg           argument sequence list
 *
 * @exception | @ref ENOMEM
 * @exception | @ref EINVAL
 *
 * @return The function return the number of input items successfully matched
 * and assigned, which can be fewer than provided for, or even zero in the event
 * of an early matching failure.<p>
 *
 * The value @ref EOF is returned if the end of input is reached before either
 * the first successful conversion or a matching failure occurs. @ref EOF is
 * also returned if a read error occurs, in which case the error indicator for
 * the stream is set, and @ref errno is set indicate the error.
 *
 * @b Example
 * @code
        #include <stdio.h>

        // ...

        va_list arg;

        // ...

        vscanf("%i%i", arg);

        // ...
   @endcode
 *
 * @see vsscanf(), vfscanf(), scanf(), fscanf(), sscanf()
 */
//==============================================================================
static inline int vscanf(const char *format, va_list arg)
{
        return vfscanf(stdin, format, arg);
}

//==============================================================================
/**
 * @brief Function scans input according to format.
 *
 * The function scans input according to format as described below. This format
 * may contain conversion specifications; the results from such conversions,
 * if any, are stored in the locations pointed to by the pointer arguments that
 * follow format. Each pointer argument must be of a type that is appropriate
 * for the value returned by the corresponding conversion specification.<p>
 *
 * <b>%</b> - The flag starts interpreting of formatting. After character can be type a
 * numbers which determine e.g. buffer length, number of digits, etc.<p>
 *
 * <b>c</b> - Scans single character.<p>
 *
 * <b>s</b> - Scans string.<p>
 *
 * <b>d, i</b> - Scans signed <i>int</i> type number.<p>
 *
 * <b>u</b> - Scans unsigned type number.<p>
 *
 * <b>x, X</b> - Scans value in HEX formatting.<p>
 *
 * <b>o</b> - Scans value in Octal formatting.<p>
 *
 * <b>f, F, g, G</b> - Scans float value.
 *
 * @param stream        input file
 * @param format        formatting string
 * @param ...           argument sequence list
 *
 * @exception | @ref ENOMEM
 * @exception | @ref EINVAL
 *
 * @return The function return the number of input items successfully matched
 * and assigned, which can be fewer than provided for, or even zero in the event
 * of an early matching failure.<p>
 *
 * The value @ref EOF is returned if the end of input is reached before either
 * the first successful conversion or a matching failure occurs. @ref EOF is
 * also returned if a read error occurs, in which case the error indicator for
 * the stream is set, and @ref errno is set indicate the error.
 *
 * @b Example
 * @code
        #include <stdio.h>

        // ...

        int foo, bar;
        fscanf(stdin, "%i%i", &foo, &bar);

        // ...
   @endcode
 *
 * @see vsscanf(), vfscanf(), scanf(), vscanf(), sscanf()
 */
//==============================================================================
static inline int fscanf(FILE *stream, const char *format, ...)
{
        va_list arg;
        va_start(arg, format);
        int status = vfscanf(stream, format, arg);
        va_end(arg);
        return status;
}

//==============================================================================
/**
 * @brief Function scans input according to format.
 *
 * The function scans input according to format as described below. This format
 * may contain conversion specifications; the results from such conversions,
 * if any, are stored in the locations pointed to by the pointer arguments that
 * follow format. Each pointer argument must be of a type that is appropriate
 * for the value returned by the corresponding conversion specification.<p>
 *
 * <b>%</b> - The flag starts interpreting of formatting. After character can be type a
 * numbers which determine e.g. buffer length, number of digits, etc.<p>
 *
 * <b>c</b> - Scans single character.<p>
 *
 * <b>s</b> - Scans string.<p>
 *
 * <b>d, i</b> - Scans signed <i>int</i> type number.<p>
 *
 * <b>u</b> - Scans unsigned type number.<p>
 *
 * <b>x, X</b> - Scans value in HEX formatting.<p>
 *
 * <b>o</b> - Scans value in Octal formatting.<p>
 *
 * <b>f, F, g, G</b> - Scans float value.
 *
 * @param s             input string (must be <i>null</i> terminated)
 * @param format        formatting string
 * @param ...           argument sequence list
 *
 * @exception | @ref ENOMEM
 * @exception | @ref EINVAL
 *
 * @return The function return the number of input items successfully matched
 * and assigned, which can be fewer than provided for, or even zero in the event
 * of an early matching failure.<p>
 *
 * The value @ref EOF is returned if the end of input is reached before either
 * the first successful conversion or a matching failure occurs. @ref EOF is
 * also returned if a read error occurs, in which case the error indicator for
 * the stream is set, and @ref errno is set indicate the error.
 *
 * @b Example
 * @code
        #include <stdio.h>

        // ...

        char *buffer = "12, 1256";
        int foo, bar;
        sscanf(buffer, "%i, %i", &foo, &bar);

        // ...
   @endcode
 *
 * @see vsscanf(), vfscanf(), scanf(), vscanf(), fscanf()
 */
//==============================================================================
static inline int sscanf(const char *s, const char *format, ...)
{
        va_list arg;
        va_start(arg, format);
        int status = vsscanf(s, format, arg);
        va_end(arg);
        return status;
}

//==============================================================================
/**
 * @brief Function writes character to stream.
 *
 * fputc() writes the character <i>c</i>, cast to an unsigned char, to
 * <i>stream</i>.
 *
 * @param c         character to put
 * @param stream    destination stream
 *
 * @exception | @ref ENOMEM
 * @exception | @ref EINVAL
 *
 * @return Return the character written as an unsigned char cast to an int or
 * @ref EOF on error.
 *
 * @b Example
 * @code
        #include <stdio.h>

        // ...

        fputc('f', stdout);

        // ...
   @endcode
 *
 * @see putc(), putchar()
 */
//==============================================================================
extern int fputc(int c, FILE *stream);

//==============================================================================
/**
 * @brief Function writes character to stream.
 *
 * putc() is equivalent to fputc().
 *
 * @param c         character to put
 * @param stream    destination stream
 *
 * @exception | @ref ENOMEM
 * @exception | @ref EINVAL
 *
 * @return Return the character written as an unsigned char cast to an int or
 * @ref EOF on error.
 *
 * @b Example
 * @code
        #include <stdio.h>

        // ...

        putc('f', stdout);

        // ...
   @endcode
 *
 * @see fputc(), putchar()
 */
//==============================================================================
static inline int putc(int c, FILE *stream)
{
        return fputc(c, stream);
}

//==============================================================================
/**
 * @brief Function writes character to @ref stdout stream.
 *
 * <b>putchar</b>(c); is equivalent to <b>putc</b>(c, stdout).
 *
 * @param c         character to put
 *
 * @exception | @ref ENOMEM
 * @exception | @ref EINVAL
 *
 * @return Return the character written as an unsigned char cast to an int or
 * @ref EOF on error.
 *
 * @b Example
 * @code
        #include <stdio.h>

        // ...

        putchar('f');

        // ...
   @endcode
 *
 * @see fputc(), putc()
 */
//==============================================================================
static inline int putchar(int c)
{
        return fputc(c, stdout);
}

//==============================================================================
/**
 * @brief Function writes string to stream.
 *
 * fputs() writes the string <i>s</i> to <i>stream</i>, without its
 * terminating null byte ('\0').
 *
 * @param s         string to put
 * @param stream    destination stream
 *
 * @exception | @ref ENOMEM
 * @exception | @ref EINVAL
 *
 * @return Return a nonnegative number on success, or @ref EOF on error.
 *
 * @b Example
 * @code
        #include <stdio.h>

        // ...

        fputs("foo bar", stdout);

        // ...
   @endcode
 *
 * @see puts()
 */
//==============================================================================
extern int fputs(const char *s, FILE *stream);

//==============================================================================
/**
 * @brief Function writes character to @ref stdout stream.
 *
 * puts() writes the string <i>s</i> to <b>stdout</b>, without its
 * terminating null byte ('\0').
 *
 * @param s         string to put
 *
 * @exception | @ref ENOMEM
 * @exception | @ref EINVAL
 *
 * @return Return a nonnegative number on success, or @ref EOF on error.
 *
 * @b Example
 * @code
        #include <stdio.h>

        // ...

        puts("foo bar");

        // ...
   @endcode
 *
 * @see fputs()
 */
//==============================================================================
extern int puts(const char *s);

//==============================================================================
/**
 * @brief Function reads character from stream.
 *
 * getc() is equivalent to fgetc().
 *
 * @param stream        input stream
 *
 * @exception | @ref ENOMEM
 * @exception | @ref EINVAL
 *
 * @return Return the character read as an unsigned char cast to an int or
 * @ref EOF on end of file or error.
 *
 * @b Example
 * @code
        #include <stdio.h>

        // ...

        char c = getc(stdin);

        // ...
   @endcode
 *
 * @see getchar(), fgetc()
 */
//==============================================================================
extern int getc(FILE *stream);

//==============================================================================
/**
 * @brief Function reads character from @ref stdin stream.
 *
 * getchar() is equivalent to <b>getc</b>(stdin).
 *
 * @exception | @ref ENOMEM
 * @exception | @ref EINVAL
 *
 * @return Return the character read as an unsigned char cast to an int or
 * @ref EOF on end of file or error.
 *
 * @b Example
 * @code
        #include <stdio.h>

        // ...

        char c = getchar();

        // ...
   @endcode
 *
 * @see getc(), fgetc()
 */
//==============================================================================
static inline int getchar(void)
{
        return getc(stdin);
}

//==============================================================================
/**
 * @brief Function reads character from stream.
 *
 * fgetc() reads the next character from <i>stream</i> and returns it
 * as an unsigned char cast to an int, or @ref EOF on end of file or error.
 *
 * @param stream        input stream
 *
 * @exception | @ref ENOMEM
 * @exception | @ref EINVAL
 *
 * @return Return the character read as an unsigned char cast to an int or
 * @ref EOF on end of file or error.
 *
 * @b Example
 * @code
        #include <stdio.h>

        // ...

        char c = fgetc(stdin);

        // ...
   @endcode
 *
 * @see getc(), getchar()
 */
//==============================================================================
static inline int fgetc(FILE *stream)
{
        return getc(stream);
}

//==============================================================================
/**
 * @brief Function back to stream a character.
 *
 * ungetc() pushes <i>c</i> back to stream, cast to unsigned char, where
 * it is available for subsequent read operations. Pushed-back characters will
 * be returned in reverse order; only one pushback is guaranteed.<p>
 *
 * Calls to the functions described here can be mixed with each other and with
 * calls to other input functions from the stdio library for the same input
 * stream.<p>
 *
 * @note Function not supported.
 *
 * @param c             output stream
 * @param stream        input stream
 *
 * @exception | @ref ENOMEM
 * @exception | @ref EINVAL
 *
 * @return Return the character read as an unsigned char cast to an int or
 * @ref EOF on end of file or error. Function is not supported by dnx RTOS
 * and always @ref EOF is returned.
 *
 * @b Example
 * @code
        #include <stdio.h>

        // ...

        ungetc(c, stdin);

        // ...
   @endcode
 *
 * @see getc(), getchar(), fgetc()
 */
//==============================================================================
static inline int ungetc(int c, FILE *stream)
{
        (void) c;
        (void) stream;

        return EOF;
}

//==============================================================================
/**
 * @brief Function reads string from stream.
 *
 * fgets() reads in at most one less than size characters from stream
 * and stores them into the buffer pointed to by <i>str</i>. Reading stops after
 * an @ref EOF, a newline or buffer end. If a newline is read, it is stored into the
 * buffer. A terminating null byte ('\0') is stored after the last character in
 * the buffer.
 *
 * @param str           destination buffer
 * @param size          buffer size
 * @param stream        input stream
 *
 * @exception | @ref ENOMEM
 * @exception | @ref EINVAL
 *
 * @return On success return the pointer to the buffer <i>str</i>, on error
 * @ref NULL is returned.
 *
 * @b Example
 * @code
        #include <stdio.h>

        // ...

        char *buffer[100];
        fgets(buffer, 100, stdin);

        // ...
   @endcode
 *
 * @see fputs(), puts()
 */
//==============================================================================
extern char *fgets(char *str, int size, FILE *stream);

#ifdef __cplusplus
}
#endif

#endif /* _STDIO_H_ */

/**@}*/
/*==============================================================================
  End of file
==============================================================================*/
