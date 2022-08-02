/*=========================================================================*//**
@file    stdio.h

@author  Daniel Zorychta

@brief   Standard IO library.

@note    Copyright (C) 2016 Daniel Zorychta <daniel.zorychta@gmail.com>

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
#include <libc/include/sys/types.h>
#include <limits.h>
#include <stddef.h>
#include <stdarg.h>
#include <libc/source/syscall.h>
#include <errno.h>
#include <libc/source/common.h>

/*==============================================================================
  Exported macros
==============================================================================*/
/**
 * @brief Buffer size used in print and scan functions.
 *
 * Buffer size is configurable from system configuration.
 */
#define BUFSIZ                  128

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

#ifndef ETX
/** @brief End Of Text value. */
#define ETX                     0x03
#endif

#ifndef EOT
/** @brief End Of Transfer value. */
#define EOT                     0x04
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

#ifndef O_DIRECTORY
#define O_DIRECTORY                             0200000
#endif

#ifndef O_SHARED
#define O_SHARED                                0400000
#endif

/* modes */
#define S_IRWXU                                 0000700    /* RWX mask for owner */
#define S_IRUSR                                 0000400    /* R for owner */
#define S_IWUSR                                 0000200    /* W for owner */
#define S_IXUSR                                 0000100    /* X for owner */

#define S_IRWXG                                 0000070    /* RWX mask for group */
#define S_IRGRP                                 0000040    /* R for group */
#define S_IWGRP                                 0000020    /* W for group */
#define S_IXGRP                                 0000010    /* X for group */

#define S_IRWXO                                 0000007    /* RWX mask for other */
#define S_IROTH                                 0000004    /* R for other */
#define S_IWOTH                                 0000002    /* W for other */
#define S_IXOTH                                 0000001    /* X for other */

#define S_ISUID                                 0004000    /* set user id on execution */
#define S_ISGID                                 0002000    /* set group id on execution */
#define S_ISVTX                                 0001000    /* save swapped text even after use */

#define S_IPMT(mode_t_m)                        ((mode_t_m) & 000777)
#define S_IFMT(mode_t_m)                        ((mode_t_m) & 070000)

#define S_ISREG(mode_t_m)                       (S_IFMT(mode_t_m) == S_IFREG)
#define S_ISDIR(mode_t_m)                       (S_IFMT(mode_t_m) == S_IFDIR)
#define S_ISDEV(mode_t_m)                       (S_IFMT(mode_t_m) == S_IFDEV)
#define S_ISLNK(mode_t_m)                       (S_IFMT(mode_t_m) == S_IFLNK)
#define S_ISPROG(mode_t_m)                      (S_IFMT(mode_t_m) == S_IFPROG)
#define S_ISFIFO(mode_t_m)                      (S_IFMT(mode_t_m) == S_IFIFO)

#define S_IFREG                                 0000000
#define S_IFDIR                                 0010000
#define S_IFDEV                                 0020000
#define S_IFLNK                                 0030000
#define S_IFPROG                                0040000
#define S_IFIFO                                 0050000




#define STACK_DEPTH_MINIMAL            (0)
#define STACK_DEPTH_VERY_LOW           (256)
#define STACK_DEPTH_LOW                (512)
#define STACK_DEPTH_MEDIUM             (768)
#define STACK_DEPTH_LARGE              (1024)
#define STACK_DEPTH_VERY_LARGE         (1280)
#define STACK_DEPTH_HUGE               (1545)
#define STACK_DEPTH_VERY_HUGE          (1792)


#define _GVAR_STRUCT_NAME               global_variables
#define GLOBAL_VARIABLES_SECTION        struct _GVAR_STRUCT_NAME
#define GLOBAL_VARIABLES_SECTION_BEGIN  struct _GVAR_STRUCT_NAME {
#define GLOBAL_VARIABLES_SECTION_END    };
#define GLOBAL_VARIABLES_TYPE(_type)    GLOBAL_VARIABLES_SECTION{_type _;}

#ifdef __cplusplus
#       include <stdlib.h>
#       include <stddef.h>
        inline void* operator new     (size_t size) {return malloc(size);}
        inline void* operator new[]   (size_t size) {return malloc(size);}
        inline void  operator delete  (void* ptr  ) {free(ptr);}
        inline void  operator delete[](void* ptr  ) {free(ptr);}
#       define _PROGMAN_CXX extern "C"
#       define _PROGMAN_EXTERN_C extern "C"
#else
#       define _PROGMAN_CXX
#       define _PROGMAN_EXTERN_C extern
#endif

#define PROGRAM_PARAMS(_name_, stack_depth)\
        static int main(int argc, char *argv[]);\
        _PROGMAN_CXX const size_t __builtin_app_##_name_##_gs__ = sizeof(struct _GVAR_STRUCT_NAME);\
        _PROGMAN_CXX const size_t __builtin_app_##_name_##_ss__ = stack_depth;\
        _PROGMAN_CXX int __builtin_app_##_name_##_main(int argc, char *argv[]) {return main(argc, argv);}

#define int_main(_name_, stack_depth, argc, argv)\
        _PROGMAN_CXX const size_t __builtin_app_##_name_##_gs__ = sizeof(struct _GVAR_STRUCT_NAME);\
        _PROGMAN_CXX const size_t __builtin_app_##_name_##_ss__ = stack_depth;\
        _PROGMAN_CXX int __builtin_app_##_name_##_main(argc, argv)

#define stdin  (((_libc_app_ctx_t*)(_libc_appctx_get()))->_stdin)
#define stdout (((_libc_app_ctx_t*)(_libc_appctx_get()))->_stdout)
#define stderr (((_libc_app_ctx_t*)(_libc_appctx_get()))->_stderr)
#define _getglobal ((struct _GVAR_STRUCT_NAME*const)_libc_global_get())
#define global _getglobal

/*==============================================================================
  Exported object types
==============================================================================*/
typedef struct _libc_file FILE;

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
        extern FILE *_libc_fopen(const char *path, const char *mode);
        return _libc_fopen(path, mode);
}

//==============================================================================
/**
 * @brief  Function open file by using selected descriptor.
 *
 * @param  fd           file descriptor
 * @param  mode         file mode (not used, mode the same as open() does)
 *
 * @return Upon successful completion fdopen(), return a <b>FILE</b> pointer.
 * Otherwise, @ref NULL is returned and @ref errno is set to indicate the
 * error.
 */
//==============================================================================
static inline FILE *fdopen(int fd, const char *mode)
{
        extern FILE *_libc_fdopen(int fd, const char *mode);
        return _libc_fdopen(fd, mode);
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
        extern int _libc_fclose(FILE *file);
        return _libc_fclose(file);
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
        extern size_t _libc_fwrite(const void *ptr, size_t size, size_t count, FILE *file);
        return _libc_fwrite(ptr, size, count, file);
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
        extern size_t _libc_fread(void *ptr, size_t size, size_t count, FILE *file);
        return _libc_fread(ptr, size, count, file);
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
static inline int fseek(FILE *file, int64_t offset, int mode)
{
        extern int _libc_fseek(FILE *file, int64_t offset, int mode);
        return _libc_fseek(file, offset, mode);
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
static inline int64_t ftell(FILE *file)
{
        extern int64_t _libc_ftell(FILE *file);
        return _libc_ftell(file);
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
                return (int64_t)*pos < 0 ? EOF : 0;
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
        extern int _libc_fflush(FILE *file);
        return _libc_fflush(file);
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
        extern int _libc_feof(FILE *file);
        return _libc_feof(file);
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
        extern void _libc_clearerr(FILE *file);
        return _libc_clearerr(file);
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
        extern int _libc_ferror(FILE *file);
        return _libc_ferror(file);
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
 * @brief Function creates temporary file in "/tmp" directory.
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
        extern FILE *_libc_tmpfile(void);
        return _libc_tmpfile();
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
 *
 * @b Example
 * @code
        #include <stdio.h>

        // ...

        FILE *file = fopen("file", "r+");
        if (file) {
                fd_t fd = fileno(file);

               // ...
        }
        // ...
   @endcode
 */
//==============================================================================
static inline int fileno(FILE *fp)
{
        extern int _libc_fileno(FILE *fp);
        return _libc_fileno(fp);
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
        extern int _libc_remove(const char *path);
        return _libc_remove(path);
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
        extern int _libc_rename(const char *old_name, const char *new_name);
        return _libc_rename(old_name, new_name);
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
 *   %l?        - print long values, where ? means d, i, u, x, or X.
 *                printf("32-bit value: %lu\n", 4561); => 32-bit value: 4561
 *
 *   %ll?       - print long long values, where ? means d, i, u, x, or X.
 *                printf("64-bit value: %llu\n", 4561); => 64-bit value: 4561
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
 *   %l?        - print long values, where ? means d, i, u, x, or X.
 *                printf("32-bit value: %lu\n", 4561); => 32-bit value: 4561
 *
 *   %ll?       - print long long values, where ? means d, i, u, x, or X.
 *                printf("64-bit value: %llu\n", 4561); => 64-bit value: 4561
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
#define printf _libc_printf
extern int _libc_printf(const char *format, ...);

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
 *   %l?        - print long values, where ? means d, i, u, x, or X.
 *                printf("32-bit value: %lu\n", 4561); => 32-bit value: 4561
 *
 *   %ll?       - print long long values, where ? means d, i, u, x, or X.
 *                printf("64-bit value: %llu\n", 4561); => 64-bit value: 4561
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
 *   %l?        - print long values, where ? means d, i, u, x, or X.
 *                printf("32-bit value: %lu\n", 4561); => 32-bit value: 4561
 *
 *   %ll?       - print long long values, where ? means d, i, u, x, or X.
 *                printf("64-bit value: %llu\n", 4561); => 64-bit value: 4561
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
#define fprintf _libc_fprintf
extern int _libc_fprintf(FILE *stream, const char *format, ...);

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
 *   %l?        - print long values, where ? means d, i, u, x, or X.
 *                printf("32-bit value: %lu\n", 4561); => 32-bit value: 4561
 *
 *   %ll?       - print long long values, where ? means d, i, u, x, or X.
 *                printf("64-bit value: %llu\n", 4561); => 64-bit value: 4561
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
 *   %l?        - print long values, where ? means d, i, u, x, or X.
 *                printf("32-bit value: %lu\n", 4561); => 32-bit value: 4561
 *
 *   %ll?       - print long long values, where ? means d, i, u, x, or X.
 *                printf("64-bit value: %llu\n", 4561); => 64-bit value: 4561
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
#define snprintf _libc_snprintf
extern int _libc_snprintf(char *s, size_t n, const char *format, ...);

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
 *   %l?        - print long values, where ? means d, i, u, x, or X.
 *                printf("32-bit value: %lu\n", 4561); => 32-bit value: 4561
 *
 *   %ll?       - print long long values, where ? means d, i, u, x, or X.
 *                printf("64-bit value: %llu\n", 4561); => 64-bit value: 4561
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
#define sprintf _libc_sprintf
extern int _libc_sprintf(char *s, const char *format, ...);

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
 *   %l?        - print long values, where ? means d, i, u, x, or X.
 *                printf("32-bit value: %lu\n", 4561); => 32-bit value: 4561
 *
 *   %ll?       - print long long values, where ? means d, i, u, x, or X.
 *                printf("64-bit value: %llu\n", 4561); => 64-bit value: 4561
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
