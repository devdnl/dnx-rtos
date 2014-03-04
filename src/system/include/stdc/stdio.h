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
 * The <b>fopen</b>() function opens the file whose name is the string pointed to by
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
 * @errors EINVAL, ENOMEM, ENOENT
 *
 * @return Upon successful completion <b>fopen</b>(), return a <b>FILE</b> pointer.
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
 * The <b>freopen</b>() function opens the file whose name is the string pointed to by
 * <i>path</i> and associates the stream pointed to by stream with it. The
 * original stream (if it exists) is closed. The <i>mode</i> argument is used just as
 * in the <b>fopen</b>() function.
 *
 * @param path          path to file
 * @param mode          file open mode
 * @param file          file to reopen
 *
 * @errors EINVAL, ENOMEM, ENOENT
 *
 * @return Upon successful completion <b>freopen</b>(), return a <b>FILE</b> pointer.
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
 * The <b>fclose</b>() function closes the created stream <i>file</i>.
 *
 * @param file          file to close
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
 * The function <b>fwrite</b>() writes <i>count</i> elements of data, each <i>size</i>
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
 * @return On success, <b>fwrite</b>() return the number of items written. This number
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
 * The function <b>fread</b>() reads <i>count</i> elements of data, each <i>size</i>
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
 * @return On success, <b>fread</b>() return the number of items read. This number
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
 * The <b>fsetpos</b>() function is alternate interfaces equivalent to <b>fseek</b>()
 * (with whence set to <b>SEEK_SET</b>), setting and storing the current value
 * of the file offset into the object referenced by <i>pos</i>.
 *
 * @param file          stream
 * @param pos           offset
 *
 * @errors EINVAL, ENOENT
 *
 * @return Upon successful completion, <b>fsetpos</b>() return 0. Otherwise, -1 is
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
 * The <b>fseek</b>() function sets the file position indicator for the stream
 * pointed to by <i>file</i>. The new position, measured in bytes, is obtained
 * by adding offset bytes to the position specified by whence. If whence is set
 * to <b>SEEK_SET</b>, <b>SEEK_CUR</b>, or <b>SEEK_END</b>, the offset is
 * relative to the start of the file, the current position indicator, or
 * end-of-file, respectively. A successful call to the <b>fseek</b>() function
 * clears the end-of-file indicator for the stream and undoes any effects of the
 * <b>ungetc</b>() function on the same stream.
 *
 * @param file          stream
 * @param offset        offset
 * @param mode          seek mode
 *
 * @errors EINVAL, ENOENT
 *
 * @return Upon successful completion, <b>fseek</b>() return 0. Otherwise, -1 is
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
 * The <b>rewind</b>() function sets the file position indicator for the stream
 * pointed to by <i>file</i> to the beginning of the file. It is equivalent to:
 * <pre>(void) fseek(stream, 0L, SEEK_SET)</pre>
 *
 * @param file          stream
 *
 * @errors EINVAL, ENOENT
 *
 * @return The <b>rewind</b>() function returns no value.
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
 * The <b>ftell</b>() function obtains the current value of the file position
 * indicator for the stream pointed to by <i>file</i>.
 *
 * @param file          stream
 *
 * @errors EINVAL
 *
 * @return Upon successful completion, <b>ftell</b>() returns the current offset.
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
 * The <b>fgetpos</b>() function is alternate interface equivalent to <b>ftell</b>(),
 * storing the current value of the file offset from the object referenced by
 * <i>pos</i>.
 *
 * @param file          stream
 * @param pos           position object
 *
 * @errors EINVAL
 *
 * @return Upon successful completion, <b>fgetpos</b>() return 0. Otherwise, -1
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
 * For output streams, <b>fflush</b>() forces a write of all buffered data for
 * the given output or update stream via the stream's underlying write function.
 * For input streams, <b>fflush</b>() discards any buffered data that has been
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
 * The function <b>feof</b>() tests the end-of-file indicator for the stream
 * pointed to by <i>file</i>, returning nonzero if it is set. The end-of-file
 * indicator can only be cleared by the function <b>clearerr</b>().
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

//==============================================================================
/**
 * @brief void clearerr(FILE *file)
 * The function <b>clearerr</b>() clears the end-of-file and error indicators
 * for the stream pointed to by <i>file</i>.
 *
 * @param file          stream
 *
 * @errors EINVAL, ENOENT
 *
 * @return None
 *
 * @example
 * // ...
 * FILE *file = fopen("/foo/bar", "w+");
 * if (file) {
 *        // file operations...
 *
 *        if (ferror(file) {
 *                // ...
 *
 *                clearerr(file);
 *
 *                // ...
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
static inline void clearerr(FILE *file)
{
        return vfs_clearerr(file);
}

//==============================================================================
/**
 * @brief int ferror(FILE *file)
 * The function <b>ferror</b>() tests the error indicator for the stream pointed
 * to by <i>file</i>, returning nonzero if it is set.  The error indicator can
 * be reset only by the <b>clearerr</b>() function.
 *
 * @param file          stream
 *
 * @errors EINVAL, ENOENT
 *
 * @return Returns nonzero value if the file stream has errors occurred,
 * 0 otherwise.
 *
 * @example
 * // ...
 * FILE *file = fopen("/foo/bar", "w+");
 * if (file) {
 *        // file operations...
 *
 *        if (ferror(file) {
 *                // ...
 *
 *                clearerr(file);
 *
 *                // ...
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
static inline int ferror(FILE *file)
{
        return vfs_ferror(file);
}

//==============================================================================
/**
 * @brief void perror(const char *s)
 * The routine <b>perror</b>() produces a message on the standard error output,
 * describing the last error encountered during a call to a system or
 * library function.  First (if <i>s</i> is not <b>NULL</b> and <i>*s</i> is not
 * a null byte ('\0')) the argument string <i>s</i> is printed, followed by a
 * colon and a blank. Then the message and a new-line.
 *
 * @param s             string to print
 *
 * @errors EINVAL, ENOENT, ENOMEM, ...
 *
 * @return None
 *
 * @example
 * // ...
 * FILE *file = fopen("/foo/bar", "w+");
 * if (!file) {
 *        perror("/foo/bar");
 *
 *        // error handling
 * }
 *
 * // ...
 */
//==============================================================================
static inline void perror(const char *s)
{
        sys_perror(s);
}

//==============================================================================
/**
 * @brief void setbuf(FILE *file, char *buffer)
 * The routine exist in dnx RTOS only for compatible reasons. Function in this
 * case do nothing.
 *
 * @param file      stream
 * @param buffer    buffer
 *
 * @errors None
 *
 * @return None
 *
 * @example
 * // ...
 * FILE *file = fopen("/foo/bar", "r");
 * if (file) {
 *        char buffer[100];
 *        setbuf(file, buffer);
 *
 *        // ...
 * }
 * // ...
 */
//==============================================================================
static inline void setbuf(FILE *file, char *buffer)
{
        (void) file;
        (void) buffer;
}

//==============================================================================
/**
 * @brief int setvbuf(FILE *file, char *buffer, int mode, size_t size)
 * The routine exist in dnx RTOS only for compatible reasons. Function in this
 * case do nothing.
 *
 * @param file      stream
 * @param buffer    buffer
 * @param mode      buffer mode (<b>_IONBF</b> unbuffered, <b>_IOLBF</b> line buffered, <b>_IOFBF</b> fully buffered)
 *
 * @errors None
 *
 * @return None
 *
 * @example
 * // ...
 * FILE *file = fopen("/foo/bar", "r");
 * if (file) {
 *        char buffer[100];
 *        setvbuf(file, buffer, _IOFBF, 100);
 *
 *        // ...
 * }
 * // ...
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
 * @brief FILE *tmpfile(void)
 * The routine exist in dnx RTOS only for compatible reasons. Function in this
 * case do nothing.
 *
 * @param None
 *
 * @errors None
 *
 * @return The <b>tmpfile</b>() function returns a stream descriptor, or <b>NULL</b>
 * if a unique filename cannot be generated or the unique file cannot be opened.
 * In the latter case, <b>errno</b> is set to indicate the error.
 *
 * @example
 * // ...
 * FILE *file = tmpfile();
 * if (file) {
 *        // ...
 * }
 * // ...
 */
//==============================================================================
static inline FILE *tmpfile(void)
{
        return NULL;
}

//==============================================================================
/**
 * @brief char *tmpnam(char *str)
 * The routine exist in dnx RTOS only for compatible reasons. Function in this
 * case do nothing.
 *
 * @param str       temporary file name or automatic generated if <b>NULL</b>
 *
 * @errors None
 *
 * @return The <b>tmpnam</b>() function returns a pointer to a unique temporary
 * filename, or <b>NULL</b> if a unique name cannot be generated.
 *
 * @example
 * // ...
 * char *tmpname = tmpnam(NULL);
 * if (tmpname) {
 *        // ...
 * }
 * // ...
 */
//==============================================================================
static inline char *tmpnam(char *str)
{
        (void) str;

        return NULL;
}

//==============================================================================
/**
 * @brief int remove(const char *path)
 * <b>remove</b>() deletes a name from the filesystem. If the removed name was
 * the last link to a file and no processes have the file open, the file is
 * deleted and the space it was using is made available for reuse.<p>
 *
 * If the name referred to a FIFO, or device, the name is removed, but
 * processes which have the object open may continue to use it.
 *
 * @param path      path to file
 *
 * @errors EINVAL, ENOENT, ...
 *
 * @return On success, zero is returned. On error, -1 is returned, and
 * <b>errno</b> is set appropriately.
 *
 * @example
 * // ...
 * remove("/foo/bar");
 * // ...
 */
//==============================================================================
static inline int remove(const char *path)
{
        return vfs_remove(path);
}

//==============================================================================
/**
 * @brief int rename(const char *old_name, const char *new_name)
 * <b>rename</b>() renames a file. In contrast to standard C library this function
 * don't move files between directories if <i>new_name</i> is localized on other
 * filesystem than <i>old_name</i>, otherwise it's depending on filesystem.
 *
 * @param old_name      old file name
 * @param new_name      new file name
 *
 * @errors EINVAL, ENOENT, EPERM, ...
 *
 * @return On success, zero is returned. On error, -1 is returned, and <b>errno</b>
 * is set appropriately.
 *
 * @example
 * // ...
 * rename("/foo/bar", "/foo/baz");
 * // ...
 */
//==============================================================================
static inline int rename(const char *old_name, const char *new_name)
{
        return vfs_rename(old_name, new_name);
}

//==============================================================================
/**
 * @brief int printf(const char *format, ...)
 * The function produce output according to a <i>format</i> as described below.
 * The function write output to <b>stdout</b>, the standard output stream.<p>
 *
 * <b>%</b> - The flag starts interpreting of formatting. After character can be type a
 * numbers which determine e.g. buffer length, number of digits, etc. When
 * next character is <i>%</i> then per cent is printed.
 *
 * <b>c</b> - Prints single character.<p>
 *
 * <b>s</b> - Prints string. If string is <b>NULL</b> then <i>null</i> string is printed.<p>
 *
 * <b>d, i</b> - Prints signed <i>int</i> type number.<p>
 *
 * <b>u</b> - Prints unsigned type number.<p>
 *
 * <b>x, X</b> - Prints value in HEX formatting.<p>
 *
 * <b>f</b> - Prints float value.
 *
 * @param format        formatting string
 * @param ...           argument sequence
 *
 * @errors EINVAL, ENOMEM, ENOENT, ...
 *
 * @return Upon successful return, these functions return the number of
 * characters printed (excluding the null byte used to end output to strings).
 * If an output error is encountered, a negative value is returned.
 *
 * @example
 * // ...
 * int foo = 12;
 * int bar = 0x12;
 * printf("foo is %d; bar is 0x%x\n", foo, bar);
 * // ...
 */
//==============================================================================
static inline int printf(const char *format, ...)
{
        va_list arg;
        va_start(arg, format);
        int status = sys_vfprintf(stdout, format, arg);
        va_end(arg);
        return status;
}

//==============================================================================
/**
 * @brief int vprintf(const char *format, va_list arg)
 * The function produce output according to a <i>format</i> as described below.
 * The function write output to <b>stdout</b>, the standard output stream.
 * An arguments are passed by list <i>arg</i>.<p>
 *
 * <b>%</b> - The flag starts interpreting of formatting. After character can be type a
 * numbers which determine e.g. buffer length, number of digits, etc. When
 * next character is <i>%</i> then per cent is printed.<p>
 *
 * <b>c</b> - Prints single character.<p>
 *
 * <b>s</b> - Prints string. If string is <b>NULL</b> then <i>null</i> string is printed.<p>
 *
 * <b>d, i</b> - Prints signed <i>int</i> type number.<p>
 *
 * <b>u</b> - Prints unsigned type number.<p>
 *
 * <b>x, X</b> - Prints value in HEX formatting.<p>
 *
 * <b>f</b> - Prints float value.
 *
 * @param format        formatting string
 * @param arg           argument sequence list
 *
 * @errors EINVAL, ENOMEM, ENOENT, ...
 *
 * @return Upon successful return, these functions return the number of
 * characters printed (excluding the null byte used to end output to strings).
 * If an output error is encountered, a negative value is returned.
 *
 * @example
 * // ...
 * va_list arg;
 * // ...
 * vprintf("foo is %d; bar is 0x%x\n", arg);
 * // ...
 */
//==============================================================================
static inline int vprintf(const char *format, va_list arg)
{
        return sys_vfprintf(stdout, format, arg);
}

//==============================================================================
/**
 * @brief int fprintf(FILE *stream, const char *format, ...)
 * The function produce output according to a <i>format</i> as described below.
 * The function write output to <b>stream</b>.<p>
 *
 * <b>%</b> - The flag starts interpreting of formatting. After character can be type a
 * numbers which determine e.g. buffer length, number of digits, etc. When
 * next character is <i>%</i> then per cent is printed.<p>
 *
 * <b>c</b> - Prints single character.<p>
 *
 * <b>s</b> - Prints string. If string is <b>NULL</b> then <i>null</i> string is printed.<p>
 *
 * <b>d, i</b> - Prints signed <i>int</i> type number.<p>
 *
 * <b>u</b> - Prints unsigned type number.<p>
 *
 * <b>x, X</b> - Prints value in HEX formatting.<p>
 *
 * <b>f</b> - Prints float value.
 *
 * @param stream        output stream
 * @param format        formatting string
 * @param ...           argument sequence
 *
 * @errors EINVAL, ENOMEM, ENOENT, ...
 *
 * @return Upon successful return, these functions return the number of
 * characters printed (excluding the null byte used to end output to strings).
 * If an output error is encountered, a negative value is returned.
 *
 * @example
 * // ...
 * int foo = 12;
 * int bar = 0x12;
 * fprintf(stdout, "foo is %d; bar is 0x%x\n", foo, bar);
 * // ...
 */
//==============================================================================
static inline int fprintf(FILE *stream, const char *format, ...)
{
        va_list arg;
        va_start(arg, format);
        int status = sys_vfprintf(stream, format, arg);
        va_end(arg);
        return status;
}

//==============================================================================
/**
 * @brief int vfprintf(FILE *stream, const char *format, va_list arg)
 * The function produce output according to a <i>format</i> as described below.
 * The function write output to <b>stream</b>.
 * An arguments are passed by list <i>arg</i>.<p>
 *
 * <b>%</b> - The flag starts interpreting of formatting. After character can be type a
 * numbers which determine e.g. buffer length, number of digits, etc. When
 * next character is <i>%</i> then per cent is printed.<p>
 *
 * <b>c</b> - Prints single character.<p>
 *
 * <b>s</b> - Prints string. If string is <b>NULL</b> then <i>null</i> string is printed.<p>
 *
 * <b>d, i</b> - Prints signed <i>int</i> type number.<p>
 *
 * <b>u</b> - Prints unsigned type number.<p>
 *
 * <b>x, X</b> - Prints value in HEX formatting.<p>
 *
 * <b>f</b> - Prints float value.
 *
 * @param stream        output stream
 * @param format        formatting string
 * @param arg           argument list
 *
 * @errors EINVAL, ENOMEM, ENOENT, ...
 *
 * @return Upon successful return, these functions return the number of
 * characters printed (excluding the null byte used to end output to strings).
 * If an output error is encountered, a negative value is returned.
 *
 * @example
 * // ...
 * va_list arg;
 * // ...
 * vfprintf(stdout, "foois %d; bar is 0x%x\n", arg);
 * // ...
 */
//==============================================================================
static inline int vfprintf(FILE *stream, const char *format, va_list arg)
{
        return sys_vfprintf(stream, format, arg);
}

//==============================================================================
/**
 * @brief int snprintf(char *s, size_t n, const char *format, ...)
 * The function produce output according to a <i>format</i> as described below.
 * The function write output pointed to by <i>s</i> of size <i>n</i>.<p>
 *
 * <b>%</b> - The flag starts interpreting of formatting. After character can be type a
 * numbers which determine e.g. buffer length, number of digits, etc. When
 * next character is <i>%</i> then per cent is printed.<p>
 *
 * <b>c</b> - Prints single character.<p>
 *
 * <b>s</b> - Prints string. If string is <b>NULL</b> then <i>null</i> string is printed.<p>
 *
 * <b>d, i</b> - Prints signed <i>int</i> type number.<p>
 *
 * <b>u</b> - Prints unsigned type number.<p>
 *
 * <b>x, X</b> - Prints value in HEX formatting.<p>
 *
 * <b>f</b> - Prints float value.
 *
 * @param s             buffer where output was produced
 * @param n             buffer size
 * @param format        formatting string
 * @param ...           argument sequence
 *
 * @errors EINVAL, ENOMEM, ENOENT, ...
 *
 * @return Upon successful return, these functions return the number of
 * characters printed (excluding the null byte used to end output to strings).
 * If an output error is encountered, a negative value is returned.
 *
 * @example
 * // ...
 * char buffer[20];
 * int foo = 12;
 * int bar = 0x12;
 * snprintf(buffer, 20, "foo is %d; bar is 0x%x\n", foo, bar);
 * // ...
 */
//==============================================================================
static inline int snprintf(char *s, size_t n, const char *format, ...)
{
        va_list arg;
        va_start(arg, format);
        int status = sys_vsnprintf(s, n, format, arg);
        va_end(arg);
        return status;
}

//==============================================================================
/**
 * @brief int vsnprintf(char *bfr, size_t size, const char *format, va_list args)
 * The function produce output according to a <i>format</i> as described below.
 * The function write output pointed to by <i>bfr</i> of size <i>size</i>.
 * An arguments are passed by list <i>args</i>.<p>
 *
 * <b>%</b> - The flag starts interpreting of formatting. After character can be type a
 * numbers which determine e.g. buffer length, number of digits, etc. When
 * next character is <i>%</i> then per cent is printed.<p>
 *
 * <b>c</b> - Prints single character.<p>
 *
 * <b>s</b> - Prints string. If string is <b>NULL</b> then <i>null</i> string is printed.<p>
 *
 * <b>d, i</b> - Prints signed <i>int</i> type number.<p>
 *
 * <b>u</b> - Prints unsigned type number.<p>
 *
 * <b>x, X</b> - Prints value in HEX formatting.<p>
 *
 * <b>f</b> - Prints float value.
 *
 * @param bfr           buffer which output was produced
 * @param size          buffer size
 * @param format        formatting string
 * @param args          argument list
 *
 * @errors EINVAL, ENOMEM, ENOENT, ...
 *
 * @return Upon successful return, these functions return the number of
 * characters printed (excluding the null byte used to end output to strings).
 * If an output error is encountered, a negative value is returned.
 *
 * @example
 * // ...
 * va_list args;
 * // ...
 * char buffer[20];
 * snprintf(buffer, 20, "foo is %d; bar is 0x%x\n", args);
 * // ...
 */
//==============================================================================
static inline int vsnprintf(char *bfr, size_t size, const char *format, va_list args)
{
        return sys_vsnprintf(bfr, size, format, args);
}

//==============================================================================
/**
 * @brief int sprintf(char *s, const char *format, ...)
 * The function produce output according to a <i>format</i> as described below.
 * The function write output pointed to by <i>s</i>. Buffer must be enough long
 * to store produced data.<p>
 *
 * <b>%</b> - The flag starts interpreting of formatting. After character can be type a
 * numbers which determine e.g. buffer length, number of digits, etc. When
 * next character is <i>%</i> then per cent is printed.<p>
 *
 * <b>c</b> - Prints single character.<p>
 *
 * <b>s</b> - Prints string. If string is <b>NULL</b> then <i>null</i> string is printed.<p>
 *
 * <b>d, i</b> - Prints signed <i>int</i> type number.<p>
 *
 * <b>u</b> - Prints unsigned type number.<p>
 *
 * <b>x, X</b> - Prints value in HEX formatting.<p>
 *
 * <b>f</b> - Prints float value.
 *
 * @param s             buffer which output was produced
 * @param format        formatting string
 * @param ...           argument sequence
 *
 * @errors EINVAL, ENOMEM, ENOENT, ...
 *
 * @return Upon successful return, these functions return the number of
 * characters printed (excluding the null byte used to end output to strings).
 * If an output error is encountered, a negative value is returned.
 *
 * @example
 * // ...
 * int foo = 12;
 * int bar = 0x12;
 * char buffer[20];
 * sprintf(buffer, "foo is %d; bar is 0x%x\n", foo, bar);
 * // ...
 */
//==============================================================================
static inline int sprintf(char *s, const char *format, ...)
{
        va_list arg;
        va_start(arg, format);
        int status = sys_vsnprintf(s, UINT16_MAX, format, arg);
        va_end(arg);
        return status;
}

//==============================================================================
/**
 * @brief int vsprintf(char *s, const char *format, va_list arg)
 * The function produce output according to a <i>format</i> as described below.
 * The function write output pointed to by <i>s</i>. Buffer must be enough long
 * to store produced data. An arguments are passed by list <i>arg</i>.<p>
 *
 * <b>%</b> - The flag starts interpreting of formatting. After character can be type a
 * numbers which determine e.g. buffer length, number of digits, etc. When
 * next character is <i>%</i> then per cent is printed.<p>
 *
 * <b>c</b> - Prints single character.<p>
 *
 * <b>s</b> - Prints string. If string is <b>NULL</b> then <i>null</i> string is printed.<p>
 *
 * <b>d, i</b> - Prints signed <i>int</i> type number.<p>
 *
 * <b>u</b> - Prints unsigned type number.<p>
 *
 * <b>x, X</b> - Prints value in HEX formatting.<p>
 *
 * <b>f</b> - Prints float value.
 *
 * @param s             buffer which output was produced
 * @param format        formatting string
 * @param arg           argument sequence list
 *
 * @errors EINVAL, ENOMEM, ENOENT, ...
 *
 * @return Upon successful return, these functions return the number of
 * characters printed (excluding the null byte used to end output to strings).
 * If an output error is encountered, a negative value is returned.
 *
 * @example
 * // ...
 * va_list args;
 * // ...
 * char buffer[20];
 * vsprintf(buffer, "foo is %d; bar is 0x%x\n", args);
 * // ...
 */
//==============================================================================
static inline int vsprintf(char *s, const char *format, va_list arg)
{
        return sys_vsnprintf(s, UINT16_MAX, format, arg);
}

//==============================================================================
/**
 * @brief int scanf(const char *format, ...)
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
 * @errors EINVAL, ENOMEM
 *
 * @return The function return the number of input items successfully matched
 * and assigned, which can be fewer than provided for, or even zero in the event
 * of an early matching failure.<p>
 *
 * The value <b>EOF</b> is returned if the end of input is reached before either
 * the first successful conversion or a matching failure occurs. <b>EOF</b> is
 * also returned if a read error occurs, in which case the error indicator for
 * the stream is set, and <b>errno</b> is set indicate the error.
 *
 * @example
 * // ...
 * int foo, bar;
 * scanf("%i%i", &foo, &bar);
 * // ...
 */
//==============================================================================
static inline int scanf(const char *format, ...)
{
        va_list arg;
        va_start(arg, format);
        int status = sys_vfscanf(stdin, format, arg);
        va_end(arg);
        return status;
}

//==============================================================================
/**
 * @brief int vscanf(const char *format, va_list arg)
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
 * @errors EINVAL, ENOMEM
 *
 * @return The function return the number of input items successfully matched
 * and assigned, which can be fewer than provided for, or even zero in the event
 * of an early matching failure.<p>
 *
 * The value <b>EOF</b> is returned if the end of input is reached before either
 * the first successful conversion or a matching failure occurs. <b>EOF</b> is
 * also returned if a read error occurs, in which case the error indicator for
 * the stream is set, and <b>errno</b> is set indicate the error.
 *
 * @example
 * // ...
 * va_list arg;
 * // ...
 * vscanf("%i%i", arg);
 * // ...
 */
//==============================================================================
static inline int vscanf(const char *format, va_list arg)
{
        return sys_vfscanf(stdin, format, arg);
}

//==============================================================================
/**
 * @brief int fscanf(FILE *stream, const char *format, ...)
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
 * @errors EINVAL, ENOMEM
 *
 * @return The function return the number of input items successfully matched
 * and assigned, which can be fewer than provided for, or even zero in the event
 * of an early matching failure.<p>
 *
 * The value <b>EOF</b> is returned if the end of input is reached before either
 * the first successful conversion or a matching failure occurs. <b>EOF</b> is
 * also returned if a read error occurs, in which case the error indicator for
 * the stream is set, and <b>errno</b> is set indicate the error.
 *
 * @example
 * // ...
 * int foo, bar;
 * fscanf(stdin, "%i%i", &foo, &bar);
 * // ...
 */
//==============================================================================
static inline int fscanf(FILE *stream, const char *format, ...)
{
        va_list arg;
        va_start(arg, format);
        int status = sys_vfscanf(stream, format, arg);
        va_end(arg);
        return status;
}

//==============================================================================
/**
 * @brief int vfscanf(FILE *stream, const char *format, va_list arg)
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
 * @errors EINVAL, ENOMEM
 *
 * @return The function return the number of input items successfully matched
 * and assigned, which can be fewer than provided for, or even zero in the event
 * of an early matching failure.<p>
 *
 * The value <b>EOF</b> is returned if the end of input is reached before either
 * the first successful conversion or a matching failure occurs. <b>EOF</b> is
 * also returned if a read error occurs, in which case the error indicator for
 * the stream is set, and <b>errno</b> is set indicate the error.
 *
 * @example
 * // ...
 * va_list arg;
 * // ...
 * vfscanf(stdin, "%i%i", arg);
 * // ...
 */
//==============================================================================
static inline int vfscanf(FILE *stream, const char *format, va_list arg)
{
        return sys_vfscanf(stream, format, arg);
}

//==============================================================================
/**
 * @brief int sscanf(const char *s, const char *format, ...)
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
 * @errors EINVAL, ENOMEM
 *
 * @return The function return the number of input items successfully matched
 * and assigned, which can be fewer than provided for, or even zero in the event
 * of an early matching failure.<p>
 *
 * The value <b>EOF</b> is returned if the end of input is reached before either
 * the first successful conversion or a matching failure occurs. <b>EOF</b> is
 * also returned if a read error occurs, in which case the error indicator for
 * the stream is set, and <b>errno</b> is set indicate the error.
 *
 * @example
 * // ...
 * char *buffer = "12, 1256";
 * int foo, bar;
 * sscanf(buffer, "%i, %i", &foo, &bar);
 * // ...
 */
//==============================================================================
static inline int sscanf(const char *s, const char *format, ...)
{
        va_list arg;
        va_start(arg, format);
        int status = sys_vsscanf(s, format, arg);
        va_end(arg);
        return status;
}

//==============================================================================
/**
 * @brief int vsscanf(const char *str, const char *format, va_list args)
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
 * @param s             input string (must be <i>null</i> terminated)
 * @param format        formatting string
 * @param args          argument sequence list
 *
 * @errors EINVAL, ENOMEM
 *
 * @return The function return the number of input items successfully matched
 * and assigned, which can be fewer than provided for, or even zero in the event
 * of an early matching failure.<p>
 *
 * The value <b>EOF</b> is returned if the end of input is reached before either
 * the first successful conversion or a matching failure occurs. <b>EOF</b> is
 * also returned if a read error occurs, in which case the error indicator for
 * the stream is set, and <b>errno</b> is set indicate the error.
 *
 * @example
 * // ...
 * char *buffer = "12, 1256";
 * va_list arg;
 * vsscanf("%i, %i", arg);
 * // ...
 */
//==============================================================================
static inline int vsscanf(const char *s, const char *format, va_list args)
{
        return sys_vsscanf(s, format, args);
}

//==============================================================================
/**
 * @brief int putc(int c, FILE *stream)
 * <b>putc</b>() is equivalent to <b>fputc</b>().
 *
 * @param c         character to put
 * @param stream    destination stream
 *
 * @errors EINVAL, ENOMEM, ...
 *
 * @return Return the character written as an unsigned char cast to an int or
 * <b>EOF</b> on error.
 *
 * @example
 * // ...
 * putc('f', stdout);
 * // ...
 */
//==============================================================================
static inline int putc(int c, FILE *stream)
{
        return sys_fputc(c, stream);
}

//==============================================================================
/**
 * @brief int putchar(int c)
 * <b>putchar</b>(c); is equivalent to <b>putc</b>(c, stdout).
 *
 * @param c         character to put
 *
 * @errors EINVAL, ENOMEM, ...
 *
 * @return Return the character written as an unsigned char cast to an int or
 * <b>EOF</b> on error.
 *
 * @example
 * // ...
 * putchar('f');
 * // ...
 */
//==============================================================================
static inline int putchar(int c)
{
        return sys_fputc(c, stdout);
}

//==============================================================================
/**
 * @brief int fputc(int c, FILE *stream)
 * <b>fputc</b>() writes the character <i>c</i>, cast to an unsigned char, to
 * <i>stream</i>.
 *
 * @param c         character to put
 * @param stream    destination stream
 *
 * @errors EINVAL, ENOMEM, ...
 *
 * @return Return the character written as an unsigned char cast to an int or
 * <b>EOF</b> on error.
 *
 * @example
 * // ...
 * fputc('f', stdout);
 * // ...
 */
//==============================================================================
static inline int fputc(int c, FILE *stream)
{
        return sys_fputc(c, stream);
}

//==============================================================================
/**
 * @brief int fputs(const char *s, FILE *stream)
 * <b>fputs</b>() writes the string <i>s</i> to <i>stream</i>, without its
 * terminating null byte ('\0').
 *
 * @param s         string to put
 * @param stream    destination stream
 *
 * @errors EINVAL, ENOMEM, ...
 *
 * @return Return a nonnegative number on success, or <b>EOF</b> on error.
 *
 * @example
 * // ...
 * fputs("foo bar", stdout);
 * // ...
 */
//==============================================================================
static inline int fputs(const char *s, FILE *stream)
{
        return sys_f_puts(s, stream, false);
}

//==============================================================================
/**
 * @brief int puts(const char *s)
 * <b>puts</b>() writes the string <i>s</i> to <b>stdout</b>, without its
 * terminating null byte ('\0').
 *
 * @param s         string to put
 *
 * @errors EINVAL, ENOMEM, ...
 *
 * @return Return a nonnegative number on success, or <b>EOF</b> on error.
 *
 * @example
 * // ...
 * puts("foo bar");
 * // ...
 */
//==============================================================================
static inline int puts(const char *s)
{
        return sys_f_puts(s, stdout, true);
}

//==============================================================================
/**
 * @brief int getchar(void)
 * <b>getchar</b>() is equivalent to <b>getc</b>(stdin).
 *
 * @param None
 *
 * @errors EINVAL, ENOMEM, ...
 *
 * @return Return the character read as an unsigned char cast to an int or
 * <b>EOF</b> on end of file or error.
 *
 * @example
 * // ...
 * char c = getchar();
 * // ...
 */
//==============================================================================
static inline int getchar(void)
{
        return sys_getc(stdin);
}

//==============================================================================
/**
 * @brief int getc(FILE *stream)
 * <b>getc</b>() is equivalent to <b>fgetc</b>().
 *
 * @param stream        input stream
 *
 * @errors EINVAL, ENOMEM, ...
 *
 * @return Return the character read as an unsigned char cast to an int or
 * <b>EOF</b> on end of file or error.
 *
 * @example
 * // ...
 * char c = getc(stdin);
 * // ...
 */
//==============================================================================
static inline int getc(FILE *stream)
{
        return sys_getc(stream);
}

//==============================================================================
/**
 * @brief int fgetc(FILE *stream)
 * <b>fgetc</b>() reads the next character from <i>stream</i> and returns it
 * as an unsigned char cast to an int, or <b>EOF</b> on end of file or error.
 *
 * @param stream        input stream
 *
 * @errors EINVAL, ENOMEM, ...
 *
 * @return Return the character read as an unsigned char cast to an int or
 * <b>EOF</b> on end of file or error.
 *
 * @example
 * // ...
 * char c = fgetc(stdin);
 * // ...
 */
//==============================================================================
static inline int fgetc(FILE *stream)
{
        return sys_getc(stream);
}

//==============================================================================
/**
 * @brief int ungetc(int c, FILE *stream)
 * <b>ungetc</b>() pushes <i>c</i> back to stream, cast to unsigned char, where
 * it is available for subsequent read operations. Pushed-back characters will
 * be returned in reverse order; only one pushback is guaranteed.<p>
 *
 * Calls to the functions described here can be mixed with each other and with
 * calls to other input functions from the stdio library for the same input
 * stream.<p>
 *
 * The routine exist in dnx RTOS only for compatible reasons. Function in no
 * supported.
 *
 * @param c             output stream
 * @param stream        input stream
 *
 * @errors EINVAL, ENOMEM, ...
 *
 * @return Return the character read as an unsigned char cast to an int or
 * <b>EOF</b> on end of file or error. Function is not supported by dnx RTOS
 * and always <b>EOF</b> is returned.
 *
 * @example
 * // ...
 * ungetc(c, stdin);
 * // ...
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
 * @brief char *fgets(char *str, int size, FILE *stream)
 * <b>fgets</b>() reads in at most one less than size characters from stream
 * and stores them into the buffer pointed to by <i>str</i>. Reading stops after
 * an <b>EOF</b>, a newline or buffer end. If a newline is read, it is stored into the
 * buffer. A terminating null byte ('\0') is stored after the last character in
 * the buffer.
 *
 * @param str           destination buffer
 * @param size          buffer size
 * @param stream        input stream
 *
 * @errors EINVAL, ENOMEM, ...
 *
 * @return Return the character read as an unsigned char cast to an int or
 * <b>EOF</b> on end of file or error. Function is not supported by dnx RTOS
 * and always returns <b>EOF</b>.
 *
 * @example
 * // ...
 * char *buffer[100];
 * fgets(buffer, 100, stdin);
 * // ...
 */
//==============================================================================
static inline char *fgets(char *str, int size, FILE *stream)
{
        return sys_fgets(str, size, stream);
}

#ifdef __cplusplus
}
#endif

#endif /* _STDIO_H_ */
/*==============================================================================
  End of file
==============================================================================*/
