#ifndef STDIO_H_
#define STDIO_H_
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

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Include files
==============================================================================*/
#include "config.h"
#include "core/sysmoni.h"

/*==============================================================================
  Exported macros
==============================================================================*/
/** stream values */
#define EOF                                     (-1)

/** stdio buffer size */
#define BUFSIZ                                  CONFIG_FSCANF_STREAM_BUFFER_SIZE


#define getmntentry(size_t__item, struct_vfs_mntent__pmntent)           sysm_getmntentry(size_t__item, struct_vfs_mntent__pmntent)
#define mknod(const_char__ppath, struct_vfs_drv_interface__pdrvif)      sysm_mknod(const_char__ppath, struct_vfs_drv_interface__pdrvif)
#define mkdir(const_char__ppath)                                        sysm_mkdir(const_char__ppath)
#define opendir(const_char__ppath)                                      sysm_opendir(const_char__ppath)
#define closedir(dir_t__pdir)                                           sysm_closedir(dir_t__pdir)
#define readdir(dir_t__pdir)                                            sysm_readdir(dir_t__pdir)
#define remove(const_char__ppath)                                       sysm_remove(const_char__ppath)
#define rename(const_char__pold_name, const_char__pnew_name)            sysm_rename(const_char__pold_name, const_char__pnew_name)
#define chmod(const_char__ppath, int__mode)                             sysm_chmod(const_char__ppath, int__mode)
#define chown(const_char__ppath, int__owner, int__group)                sysm_chown(const_char__ppath, int__owner, int__group)
#define stat(const_char__ppath, struct_vfs_stat__stat_ptr)              sysm_stat(const_char__ppath, struct_vfs_stat__stat_ptr)
#define statfs(const_char__ppath, struct_vfs_statfs__statfs_ptr)        sysm_statfs(const_char__ppath, struct_vfs_statfs__statfs_ptr)
#define fopen(const_char__ppath, const_char__pmode)                     sysm_fopen(const_char__ppath, const_char__pmode)
#define freopen(const_char__ppath, const_char__pmode, FILE__pfile)      sysm_freopen(const_char__ppath, const_char__pmode, FILE__pfile)
#define fclose(FILE__pfile)                                             sysm_fclose(FILE__pfile)
#define fwrite(void__ptr, size_t__isize, size_t__nitems, FILE__pfile)   sysm_fwrite(void__ptr, size_t__isize, size_t__nitems, FILE__pfile)
#define fread(void__ptr, size_t__isize, size_t__nitems, FILE__pfile)    sysm_fread(void__ptr, size_t__isize, size_t__nitems, FILE__pfile)
#define fseek(FILE__pfile, i64_t__offset, int__mode)                    sysm_fseek(FILE__pfile, i64_t__offset, int__mode)
#define ftell(FILE__pfile)                                              sysm_ftell(FILE__pfile)
#define ioctl(FILE__pfile, ...)                                         sysm_ioctl(FILE__pfile, __VA_ARGS__)
#define fstat(FILE__pfile, struct_vfs_stat__pstat)                      sysm_fstat(FILE__pfile, struct_vfs_stat__pstat)
#define fflush(FILE__pfile)                                             sysm_fflush(FILE__pfile)
#define feof(FILE__pfile)                                               sysm_feof(FILE__pfile)
#define rewind(FILE__pfile)                                             sysm_rewind(FILE__pfile)
#define ferror(FILE__pfile)                                             0

/*==============================================================================
  Exported object types
==============================================================================*/

/*==============================================================================
  Exported objects
==============================================================================*/

/*==============================================================================
  Exported functions
==============================================================================*/

#ifdef __cplusplus
}
#endif

#endif /* STDIO_H_ */
/*==============================================================================
  End of file
==============================================================================*/
