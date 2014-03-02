/*=========================================================================*//**
@file    stdio.h

@author  Daniel Zorychta

@brief

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

#ifndef _STDIO_H_
#define _STDIO_H_

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Include files
==============================================================================*/
#include "config.h"
#include <sys/types.h>
#include "core/sysmoni.h"
#include "core/printx.h"
#include "core/scanx.h"
#include "core/progman.h"
#include "core/conv.h"
#include "kernel/kwrapper.h"
#include <limits.h>

/*==============================================================================
  Exported macros
==============================================================================*/
/** stdio buffer size */
#define BUFSIZ                  CONFIG_STREAM_BUFFER_LENGTH

#define _IOFBF                  0               /* setvbuf should set fully buffered */
#define _IOLBF                  1               /* setvbuf should set line buffered */
#define _IONBF                  2               /* setvbuf should set unbuffered */

#ifndef NULL
#define NULL                    0
#endif

#ifndef EOF
#define EOF                     (-1)
#endif

#ifndef SEEK_SET
#define SEEK_SET                VFS_SEEK_SET
#endif

#ifndef SEEK_CUR
#define SEEK_CUR                VFS_SEEK_CUR
#endif

#ifndef SEEK_END
#define SEEK_END                VFS_SEEK_END
#endif

/*==============================================================================
  Exported object types
==============================================================================*/

/*==============================================================================
  Exported objects
==============================================================================*/

/*==============================================================================
  Exported functions
==============================================================================*/

/*==============================================================================
  Exported inline functions
==============================================================================*/
//==============================================================================
/**
 * @brief FILE *fopen(const char *path, const char *mode)
 * The <b>fopen()</b> function opens the file whose name is the string pointed to by
 * <i>path</i> and associates a stream with it. The argument <i>mode</i> points
 * to a string beginning with one of the following sequences (possibly followed
 * by additional characters, as described below):<p>
 *
 * <b>r</b><br>
 * Open text file for reading. The stream is positioned at the beginning of the
 * file.<p>
 *
 * <b>r+</b><br>
 * Open for reading and writing. The stream is positioned at the beginning of
 * the file.<p>
 *
 * <b>w</b><br>
 * Truncate file to zero length or create text file for writing. The stream is
 * positioned at the beginning of the file.<p>
 *
 * <b>w+</b><br>
 * Open for reading and writing. The file is created if it does not exist,
 * otherwise it is truncated. The stream is positioned at the beginning of the
 * file.<p>
 *
 * <b>a</b><br>
 * Open for appending (writing at end of file). The file is created if it does
 * not exist. The stream is positioned at the end of the file.<p>
 *
 * <b>a+</b><br>
 * Open for reading and appending (writing at end of file). The file is created
 * if it does not exist. The initial file position for reading is at the
 * beginning of the file, but output is always appended to the end of the file.
 *
 * @param path          path to file
 * @param mode          file open mode
 *
 * @errors EINVAL, ENOMEM, ENOENT
 *
 * @return Upon successful completion <b>fopen()</b>, return a <b>FILE</b> pointer.
 * Otherwise, <b>NULL</b> is returned and <b>errno</b> is set to indicate the
 * error.
 *
 * @example
 * // ...
 * FILE *file = fopen("/foo/bar", "w+");
 * if (!file) {
 *        perror(NULL);
 *        // error handling
 *        // ...
 * }
 *
 * // file handling
 * // ...
 */
//==============================================================================
static inline FILE *fopen(const char *path, const char *mode)
{
        return sysm_fopen(path, mode);
}

//==============================================================================
/**
 * @brief FILE *freopen(const char *path, const char *mode, FILE *file)
 * The <b>freopen()</b> function opens the file whose name is the string pointed to by
 * <i>path</i> and associates the stream pointed to by stream with it. The
 * original stream (if it exists) is closed. The <i>mode</i> argument is used just as
 * in the <b>fopen()</b> function.
 *
 * @param path          path to file
 * @param mode          file open mode
 * @param file          file to reopen
 *
 * @errors EINVAL, ENOMEM, ENOENT
 *
 * @return Upon successful completion <b>freopen()</b>, return a <b>FILE</b> pointer.
 * Otherwise, <b>NULL</b> is returned and <b>errno</b> is set to indicate the
 * error.
 *
 * @example
 * // ...
 * FILE *file = freopen("/foo/bar", "w+", file);
 * if (!file) {
 *        perror(NULL);
 *        // error handling
 *        // ...
 * }
 *
 * // file handling
 * // ...
 */
//==============================================================================
static inline FILE *freopen(const char *path, const char *mode, FILE *file)
{
        return sysm_freopen(path, mode, file);
}

//==============================================================================
/**
 * @brief int fclose(FILE *file)
 * The <b>fclose()</b> function closes the created stream <i>file</i>.
 *
 * @param file          file to reopen
 *
 * @errors EINVAL, ENOENT, ESRCH
 *
 * @return Upon successful completion 0 is returned. Otherwise, <b>EOF</b> is
 * returned and <b>errno</b> is set to indicate the error. In either case any
 * further access (including another call to fclose()) to the stream results
 * in undefined behavior.
 *
 * @example
 * // ...
 * FILE *file = fopen("/foo/bar", "w+");
 * if (file) {
 *        // file operations...
 *
 *        fclose(file);
 * }
 *
 * // ...
 */
//==============================================================================
static inline int fclose(FILE *file)
{
        return sysm_fclose(file);
}

//==============================================================================
/**
 * @brief size_t fwrite(const void *ptr, size_t size, size_t count, FILE *file)
 * The function <b>fwrite()</b> writes <i>count</i> elements of data, each <i>size</i>
 * bytes long, to the stream pointed to by <i>file</i>, obtaining them from the
 * location given by <i>ptr</i>.
 *
 * @param ptr           pointer to data
 * @param size          element size
 * @param count         number of elements
 * @param file          stream
 *
 * @errors EINVAL, ENOENT, ENOMEM, ...
 *
 * @return On success, <b>fwrite()</b> return the number of items written. This number
 * equals the number of bytes transferred only when <i>size</i> is 1. If an error
 * occurs, or the end of the file is reached, the return value is a short item
 * count (or zero).
 *
 * @example
 * // ...
 * FILE *file = fopen("/foo/bar", "w+");
 * if (file) {
 *        char buffer[4] = {0, 1, 2, 3};
 *        if (fwrite(buffer, 1, sizeof(buffer), file) != 4) {
 *                // error handling
 *        }
 *
 *        fclose(file);
 * }
 *
 * // ...
 */
//==============================================================================
static inline size_t fwrite(const void *ptr, size_t size, size_t count, FILE *file)
{
        return vfs_fwrite(ptr, size, count, file);
}

//==============================================================================
/**
 * @brief size_t fread(void *ptr, size_t size, size_t count, FILE *file)
 * The function <b>fread()</b> reads <i>count</i> elements of data, each <i>size</i>
 * bytes long, from the stream pointed to by <i>file</i>, storing them at the
 * location given by <i>ptr</i>.
 *
 * @param ptr           pointer to data
 * @param size          element size
 * @param count         number of elements
 * @param file          stream
 *
 * @errors EINVAL, ENOENT, ENOMEM, ...
 *
 * @return On success, <b>fread()</b> return the number of items read. This number
 * equals the number of bytes transferred only when <i>size</i> is 1. If an error
 * occurs, or the end of the file is reached, the return value is a short item
 * count (or zero).
 *
 * @example
 * // ...
 * FILE *file = fopen("/foo/bar", "w+");
 * if (file) {
 *        char buffer[4];
 *        if (fread(buffer, 1, sizeof(buffer), file) != 4) {
 *                // error handling
 *        }
 *
 *        fclose(file);
 * }
 *
 * // ...
 */
//==============================================================================
static inline size_t fread(void *ptr, size_t size, size_t count, FILE *file)
{
        return vfs_fread(ptr, size, count, file);
}

//==============================================================================
/**
 * @brief int fsetpos(FILE *file, const fpos_t *pos)
 * The <b>fsetpos()</b> function is alternate interfaces equivalent to <b>fseek()</b>
 * (with whence set to <b>SEEK_SET</b>), setting and storing the current value
 * of the file offset into the object referenced by <i>pos</i>.
 *
 * @param file          stream
 * @param pos           offset
 *
 * @errors EINVAL, ENOENT
 *
 * @return Upon successful completion, <b>fsetpos()</b> return 0. Otherwise, -1 is
 * returned and <b>errno</b> is set to indicate the error.
 *
 * @example
 * // ...
 * FILE *file = fopen("/foo/bar", "w+");
 * if (file) {
 *        if (fsetpos(file, 100) != 0) {
 *                // error handling
 *        }
 *
 *        // file operations...
 *
 *        fclose(file);
 * }
 *
 * // ...
 */
//==============================================================================
static inline int fsetpos(FILE *file, const fpos_t *pos)
{
        if (pos) {
                return vfs_fseek(file, *pos, SEEK_SET);
        } else {
                return EOF;
        }
}

//==============================================================================
/**
 * @brief int fseek(FILE *file, i64_t offset, int mode)
 * The <b>fseek()</b> function sets the file position indicator for the stream
 * pointed to by <i>file</i>. The new position, measured in bytes, is obtained
 * by adding offset bytes to the position specified by whence. If whence is set
 * to <b>SEEK_SET</b>, <b>SEEK_CUR</b>, or <b>SEEK_END</b>, the offset is
 * relative to the start of the file, the current position indicator, or
 * end-of-file, respectively. A successful call to the <b>fseek()</b> function
 * clears the end-of-file indicator for the stream and undoes any effects of the
 * <b>ungetc()</b> function on the same stream.
 *
 * @param file          stream
 * @param offset        offset
 * @param mode          seek mode
 *
 * @errors EINVAL, ENOENT
 *
 * @return Upon successful completion, <b>fseek()</b> return 0. Otherwise, -1 is
 * returned and <b>errno</b> is set to indicate the error.
 *
 * @example
 * // ...
 * FILE *file = fopen("/foo/bar", "w+");
 * if (file) {
 *        if (fseek(file, 100, SEEK_SET) != 0) {
 *                // error handling
 *        }
 *
 *        // file operations...
 *
 *        fclose(file);
 * }
 *
 * // ...
 */
//==============================================================================
static inline int fseek(FILE *file, i64_t offset, int mode)
{
        return vfs_fseek(file, offset, mode);
}

//==============================================================================
/**
 * @brief int rewind(FILE *file)
 * The <b>rewind()</b> function sets the file position indicator for the stream
 * pointed to by <i>file</i> to the beginning of the file. It is equivalent to:
 * <pre>(void) fseek(stream, 0L, SEEK_SET)</pre>
 *
 * @param file          stream
 *
 * @errors EINVAL, ENOENT
 *
 * @return The <b>rewind()</b> function returns no value.
 *
 * @example
 * // ...
 * FILE *file = fopen("/foo/bar", "w+");
 * if (file) {
 *        // file operations...
 *
 *        rewind(file);
 *
 *        // file operations...
 *
 *        fclose(file);
 * }
 *
 * // ...
 */
//==============================================================================
static inline int rewind(FILE *file)
{
        return vfs_rewind(file);
}

//==============================================================================
/**
 * @brief i64_t ftell(FILE *file)
 * The <b>ftell()</b> function obtains the current value of the file position
 * indicator for the stream pointed to by <i>file</i>.
 *
 * @param file          stream
 *
 * @errors EINVAL
 *
 * @return Upon successful completion, <b>ftell()</b> returns the current offset.
 * Otherwise, -1 is returned and <b>errno</b> is set to indicate the error.
 *
 * @example
 * // ...
 * FILE *file = fopen("/foo/bar", "w+");
 * if (file) {
 *        // file operations...
 *
 *        i64_t pos = ftell(file);
 *        if (pos == -1) {
 *                // error handling
 *        }
 *
 *        // file operations...
 *
 *        fclose(file);
 * }
 *
 * // ...
 */
//==============================================================================
static inline i64_t ftell(FILE *file)
{
        return vfs_ftell(file);
}

//==============================================================================
/**
 * @brief int fgetpos(FILE *file, fpos_t *pos)
 * The <b>fgetpos()</b> function is alternate interface equivalent to <b>ftell()</b>,
 * storing the current value of the file offset from the object referenced by
 * <i>pos</i>.
 *
 * @param file          stream
 * @param pos           position object
 *
 * @errors EINVAL
 *
 * @return Upon successful completion, <b>fgetpos()</b> return 0. Otherwise, -1
 * is returned and <b>errno</b> is set to indicate the error.
 *
 * @example
 * // ...
 * FILE *file = fopen("/foo/bar", "w+");
 * if (file) {
 *        // file operations...
 *
 *        fpos_t pos;
 *        if (fgetpos(file, &pos) == -1) {
 *                // error handling
 *        }
 *
 *        // file operations...
 *
 *        fclose(file);
 * }
 *
 * // ...
 */
//==============================================================================
static inline int fgetpos(FILE *file, fpos_t *pos)
{
        if (pos) {
                *pos = (fpos_t)vfs_ftell(file);
                return 0;
        } else {
                return EOF;
        }
}

//==============================================================================
/**
 * @brief int fflush(FILE *file)
 * For output streams, <b>fflush()</b> forces a write of all buffered data for
 * the given output or update stream via the stream's underlying write function.
 * For input streams, <b>fflush()</b> discards any buffered data that has been
 * fetched from the underlying file. The open status of the stream is unaffected.
 *
 * @param file          stream
 *
 * @errors EINVAL, ENOENT, ...
 *
 * @return Upon successful completion 0 is returned. Otherwise, <b>EOF</b> is
 * returned and <b>errno</b> is set to indicate the error.
 *
 * @example
 * // ...
 * FILE *file = fopen("/foo/bar", "w+");
 * if (file) {
 *        // file operations...
 *
 *        if (fflush(file) == EOF) {
 *                // error handling
 *        }
 *
 *        // file operations...
 *
 *        fclose(file);
 * }
 *
 * // ...
 */
//==============================================================================
static inline int fflush(FILE *file)
{
        return vfs_fflush(file);
}

//==============================================================================
/**
 * @brief int feof(FILE *file)
 * The function <b>feof()</b> tests the end-of-file indicator for the stream
 * pointed to by <i>file</i>, returning nonzero if it is set. The end-of-file
 * indicator can only be cleared by the function <b>clearerr()</b>.
 *
 * @param file          stream
 *
 * @errors EINVAL, ENOENT, ...
 *
 * @return Returns nonzero if <b>EOF</b> indicator is set, otherwise 0 is
 * returned.
 *
 * @example
 * // ...
 * FILE *file = fopen("/foo/bar", "w+");
 * if (file) {
 *        // file operations...
 *
 *        if (feof(file) != 0) {
 *                // end-of-file handling
 *        }
 *
 *        // file operations...
 *
 *        fclose(file);
 * }
 *
 * // ...
 */
//==============================================================================
static inline int feof(FILE *file)
{
        return vfs_feof(file);
}

static inline void clearerr(FILE *file)
{
        return vfs_clearerr(file);
}

static inline int ferror(FILE *file)
{
        return vfs_ferror(file);
}

static inline void perror(const char *s)
{
        sys_perror(s);
}

static inline void setbuf(FILE *stream, char *buffer)
{
        (void) stream;
        (void) buffer;
}

static inline int setvbuf(FILE *stream, char *buffer, int mode, size_t size)
{
        (void) stream;
        (void) buffer;
        (void) mode;
        (void) size;

        return 0;
}

static inline FILE *tmpfile(void)
{
        return NULL;
}

static inline char *tmpnam(char *str)
{
        (void) str;

        return NULL;
}

static inline int remove(const char *path)
{
        return vfs_remove(path);
}

static inline int rename(const char *old_name, const char *new_name)
{
        return vfs_rename(old_name, new_name);
}

static inline int printf(const char *format, ...)
{
        va_list arg;
        va_start(arg, format);
        int status = sys_vfprintf(stdout, format, arg);
        va_end(arg);
        return status;
}

static inline int vprintf(const char *format, va_list arg)
{
        return sys_vfprintf(stdout, format, arg);
}

static inline int fprintf(FILE *stream, const char *format, ...)
{
        va_list arg;
        va_start(arg, format);
        int status = sys_vfprintf(stream, format, arg);
        va_end(arg);
        return status;
}

static inline int vfprintf(FILE *stream, const char *format, va_list arg)
{
        return sys_vfprintf(stream, format, arg);
}

static inline int snprintf(char *s, size_t n, const char *format, ...)
{
        va_list arg;
        va_start(arg, format);
        int status = sys_vsnprintf(s, n, format, arg);
        va_end(arg);
        return status;
}

static inline int vsnprintf(char *bfr, size_t size, const char *format, va_list args)
{
        return sys_vsnprintf(bfr, size, format, args);
}

static inline int sprintf(char *s, const char *format, ...)
{
        va_list arg;
        va_start(arg, format);
        int status = sys_vsnprintf(s, UINT16_MAX, format, arg);
        va_end(arg);
        return status;
}

static inline int vsprintf(char *s, const char *format, va_list arg)
{
        return sys_vsnprintf(s, UINT16_MAX, format, arg);
}

static inline int scanf(const char *format, ...)
{
        va_list arg;
        va_start(arg, format);
        int status = sys_vfscanf(stdin, format, arg);
        va_end(arg);
        return status;
}

static inline int vscanf(const char *format, va_list arg)
{
        return sys_vfscanf(stdin, format, arg);
}

static inline int fscanf(FILE *stream, const char *format, ...)
{
        va_list arg;
        va_start(arg, format);
        int status = sys_vfscanf(stream, format, arg);
        va_end(arg);
        return status;
}

static inline int vfscanf(FILE *stream, const char *format, va_list arg)
{
        return sys_vfscanf(stream, format, arg);
}

static inline int sscanf(const char *s, const char *format, ...)
{
        va_list arg;
        va_start(arg, format);
        int status = sys_vsscanf(s, format, arg);
        va_end(arg);
        return status;
}

static inline int vsscanf(const char *str, const char *format, va_list args)
{
        return sys_vsscanf(str, format, args);
}

static inline int putc(int c, FILE *stream)
{
        return sys_fputc(c, stream);
}

static inline int fputc(int c, FILE *stream)
{
        return sys_fputc(c, stream);
}

static inline int fputs(const char *s, FILE *stream)
{
        return sys_f_puts(s, stream, false);
}

static inline int puts(const char *s)
{
        return sys_f_puts(s, stdout, true);
}

static inline int putchar(int c)
{
        return sys_fputc(c, stdout);
}

static inline int getchar(void)
{
        return sys_getc(stdin);
}

static inline int fgetc(FILE *stream)
{
        return sys_getc(stream);
}

static inline int getc(FILE *stream)
{
        return sys_getc(stream);
}

static inline int ungetc(int c, FILE *stream)
{
        (void) c;
        (void) stream;

        return EOF;
}

static inline char *fgets(char *str, int size, FILE *stream)
{
        return sys_fgets(str, size, stream);
}

static inline char *gets(char *str)
{
        return sys_fgets(str, INT_MAX, stdin);
}

#ifdef __cplusplus
}
#endif

#endif /* _STDIO_H_ */
/*==============================================================================
  End of file
==============================================================================*/
