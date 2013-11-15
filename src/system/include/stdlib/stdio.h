/*=========================================================================*//**
@file    dnxio.h

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

/*==============================================================================
  Exported macros
==============================================================================*/
/* MEMORY MANAGEMENT DEFINTIONS */
#ifndef malloc
#define malloc(size_t__size)                                    sysm_tskmalloc(size_t__size)
#endif

#ifndef calloc
#define calloc(size_t__nitems, size_t__isize)                   sysm_tskcalloc(size_t__nitems, size_t__isize)
#endif

#ifndef free
#define free(void__pmem)                                        sysm_tskfree(void__pmem)
#endif

/** stdio buffer size */
#define BUFSIZ                                                  CONFIG_FSCANF_STREAM_BUFFER_SIZE

/** function-like macros */
#define printf(...)                                             sys_fprintf(stdout, __VA_ARGS__)
#define fprintf(FILE__stream, ...)                              sys_fprintf(FILE__stream, __VA_ARGS__)
#define snprintf(char__bfr, size_t__size, ...)                  sys_snprintf(char__bfr, size_t__size, __VA_ARGS__)
#define sprintf(char__bfr, ...)                                 sys_snprintf(char__bfr, UINT16_MAX, __VA_ARGS__)

#define scanf(const_char__format, ...)                          sys_fscanf(stdin, const_char__format, __VA_ARGS__)
#define fscanf(FILE__stream, const_char__format, ...)           sys_fscanf(FILE__stream, const_char__format, __VA_ARGS__)
#define sscanf(const_char__str, const_char__format, ...)        sys_sscanf(const_char__str, const_char__format, __VA_ARGS__)

#define strerror(int__errnum)                                   sys_strerror(int__errnum)

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

static inline size_t fwrite(void *ptr, size_t size, size_t nitems, FILE *file)
{
        return vfs_fwrite(ptr, size, nitems, file);
}

static inline size_t fread(void *ptr, size_t size, size_t nitems, FILE *file)
{
        return vfs_fread(ptr, size, nitems, file);
}

static inline int fseek(FILE *file, i64_t offset, int mode)
{
        return vfs_fseek(file, offset, mode);
}

static inline i64_t ftell(FILE *file)
{
        return vfs_ftell(file);
}

static inline int fstat(FILE *file, struct vfs_stat *stat)
{
        return vfs_fstat(file, stat);
}

static inline int fflush(FILE *file)
{
        return vfs_fflush(file);
}

static inline int feof(FILE *file)
{
        return vfs_feof(file);
}

static inline void clearerr(FILE *file) {
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

static inline stdret_t getmntentry(size_t item, struct vfs_mntent *mntent)
{
        return vfs_getmntentry(item, mntent);
}

static inline int mknod(const char *path, struct vfs_drv_interface *drvif)
{
        return vfs_mknod(path, drvif);
}

static inline int mkdir(const char *path)
{
        return vfs_mkdir(path);
}

static inline DIR *opendir(const char *path)
{
        return sysm_opendir(path);
}

static inline int closedir(DIR *dir)
{
        return sysm_closedir(dir);
}

static inline dirent_t readdir(DIR *dir)
{
        return vfs_readdir(dir);
}

static inline int remove(const char *path)
{
        return vfs_remove(path);
}

static inline int rename(const char *old_name, const char *new_name)
{
        return vfs_rename(old_name, new_name);
}

static inline int chmod(const char *path, int mode)
{
        return vfs_chmod(path, mode);
}

static inline int chown(const char *path, int owner, int group)
{
        return vfs_chown(path, owner, group);
}

static inline int stat(const char *path, struct vfs_stat *stat)
{
        return vfs_stat(path, stat);
}

static inline int statfs(const char *path, struct vfs_statfs *statfs)
{
        return vfs_statfs(path, statfs);
}

static inline int vsnprintf(char *bfr, size_t size, const char *format, va_list args)
{
        return sys_vsnprintf(bfr, size, format, args);
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

static inline char *fgets(char *str, int size, FILE *stream)
{
        return sys_fgets(str, size, stream);
}

static inline int atoi(const char *str)
{
        return sys_atoi(str);
}

static inline char *strtoi(const char *str, int base, i32_t *result)
{
        return sys_strtoi(str, base, result);
}

static inline double atof(const char *str)
{
        return sys_atof(str);
}

static inline double strtod(const char *str, char **end)
{
        return sys_strtod(str, end);
}

#ifdef __cplusplus
}
#endif

#endif /* _STDIO_H_ */
/*==============================================================================
  End of file
==============================================================================*/
