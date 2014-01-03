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
#include "core/systypes.h"
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
static inline FILE *fopen(const char *path, const char *mode)
{
        return sysm_fopen(path, mode);
}

static inline FILE *freopen(const char *path, const char *mode, FILE *file)
{
        return sysm_freopen(path, mode, file);
}

static inline int fclose(FILE *file)
{
        return sysm_fclose(file);
}

static inline size_t fwrite(const void *ptr, size_t size, size_t count, FILE *file)
{
        return vfs_fwrite(ptr, size, count, file);
}

static inline size_t fread(void *ptr, size_t size, size_t count, FILE *file)
{
        return vfs_fread(ptr, size, count, file);
}

static inline int fsetpos(FILE *file, const fpos_t *pos)
{
        if (pos) {
                return vfs_fseek(file, *pos, SEEK_SET);
        } else {
                return EOF;
        }
}

static inline int fseek(FILE *file, i64_t offset, int mode)
{
        return vfs_fseek(file, offset, mode);
}

static inline i64_t ftell(FILE *file)
{
        return vfs_ftell(file);
}

static inline int fgetpos(FILE *file, fpos_t *pos)
{
        if (pos) {
                *pos = (fpos_t)vfs_ftell(file);
                return 0;
        } else {
                return EOF;
        }
}

static inline int fflush(FILE *file)
{
        return vfs_fflush(file);
}

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

static inline int rewind(FILE *file)
{
        return vfs_rewind(file);
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
        return sys_vfprintf(stdout, format, arg);
}

static inline int vprintf(const char *format, va_list arg)
{
        return sys_vfprintf(stdout, format, arg);
}

static inline int fprintf(FILE *stream, const char *format, ...)
{
        va_list arg;
        va_start(arg, format);
        return sys_vfprintf(stream, format, arg);
}

static inline int vfprintf(FILE *stream, const char *format, va_list arg)
{
        return sys_vfprintf(stream, format, arg);
}

static inline int snprintf(char *s, size_t n, const char *format, ...)
{
        va_list arg;
        va_start(arg, format);
        return sys_vsnprintf(s, n, format, arg);
}

static inline int vsnprintf(char *bfr, size_t size, const char *format, va_list args)
{
        return sys_vsnprintf(bfr, size, format, args);
}

static inline int sprintf(char *s, const char *format, ...)
{
        va_list arg;
        va_start(arg, format);
        return sys_vsnprintf(s, UINT16_MAX, format, arg);
}

static inline int vsprintf(char *s, const char *format, va_list arg)
{
        return sys_vsnprintf(s, UINT16_MAX, format, arg);
}

static inline int scanf(const char *format, ...)
{
        va_list arg;
        va_start(arg, format);
        return sys_vfscanf(stdin, format, arg);
}

static inline int vscanf(const char *format, va_list arg)
{
        return sys_vfscanf(stdin, format, arg);
}

static inline int fscanf(FILE *stream, const char *format, ...)
{
        va_list arg;
        va_start(arg, format);
        return sys_vfscanf(stream, format, arg);
}

static inline int vfscanf(FILE *stream, const char *format, va_list arg)
{
        return sys_vfscanf(stream, format, arg);
}

static inline int sscanf(const char *s, const char *format, ...)
{
        va_list arg;
        va_start(arg, format);
        return sys_vsscanf(s, format, arg);
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
