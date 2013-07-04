#ifndef DNXMODULE_H_
#define DNXMODULE_H_
/*=========================================================================*//**
@file    dnxmodule.h

@author  Daniel Zorychta

@brief   This function provide all required function needed to write modules.

@note    Copyright (C) 2012 Daniel Zorychta <daniel.zorychta@gmail.com>

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
#include "core/systypes.h"
#include "core/vfs.h"
#include "core/sysmoni.h"
#include "user/regdrv.h"
#include "kernel/kwrapper.h"

/*==============================================================================
  Exported symbolic constants/macros
==============================================================================*/
#ifdef DNX_H_
#error "dnx.h and dnxmodule.h shall never included together!"
#endif

#define MODULE_NAME(const_char__pmodule_name)                           static const char *__module_name__ = #const_char__pmodule_name

#undef  calloc
#define calloc(size_t__nmemb, size_t__msize)                            sysm_modcalloc(size_t__nmemb, size_t__msize, regdrv_get_module_number(__module_name__))

#undef  malloc
#define malloc(size_t__size)                                            sysm_modmalloc(size_t__size, regdrv_get_module_number(__module_name__))

#undef  free
#define free(void__pmem)                                                sysm_modfree(void__pmem, regdrv_get_module_number(__module_name__))

#undef  mknod
#define mknod(const_char__ppath, struct_vfs_drv_interface__pdrvif)      vfs_mknod(const_char__ppath, struct_vfs_drv_interface__pdrvif)

#undef  mkdir
#define mkdir(const_char__ppath)                                        vfs_mkdir(const_char__ppath)

#undef  opendir
#define opendir(const_char__ppath)                                      vfs_opendir(const_char__ppath)

#undef  closedir
#define closedir(dir_t__pdir)                                           vfs_closedir(dir_t__pdir)

#undef  readdir
#define readdir(dir_t__pdir)                                            vfs_readdir(dir_t__pdir)

#undef  remove
#define remove(const_char__ppath)                                       vfs_remove(const_char__ppath)

#undef  rename
#define rename(const_char__pold_name, const_char__pnew_name)            vfs_rename(const_char__pold_name, const_char__pnew_name)

#undef  chmod
#define chmod(const_char__ppath, int__mode)                             vfs_chmod(const_char__ppath, int__mode)

#undef  chown
#define chown(const_char__ppath, int__owner, int__group)                vfs_chown(const_char__ppath, int__owner, int__group)

#undef  stat
#define stat(const_char__ppath, struct_vfs_stat__pstat)                 vfs_stat(const_char__ppath, struct_vfs_stat__pstat)

#undef  statfs
#define statfs(const_char__ppath, struct_vfs_statfs__pstatfs)           vfs_statfs(const_char__ppath, struct_vfs_statfs__pstatfs)

#undef  fopen
#define fopen(const_char__ppath, const_char__pmode)                     vfs_fopen(const_char__ppath, const_char__pmode)

#undef  fclose
#define fclose(FILE__pfile)                                             vfs_fclose(FILE__pfile)

#undef  fwrite
#define fwrite(void__ptr, size_t__isize, size_t__nitems, FILE__pfile)   vfs_fwrite(void__ptr, size_t__isize, size_t__nitems, FILE__pfile)

#undef  fread
#define fread(void__ptr, size_t__isize, size_t__nitems, FILE__pfile)    vfs_fread(void__ptr, size_t__isize, size_t__nitems, FILE__pfile)

#undef  fseek
#define fseek(FILE__pfile, i64_t__offset, int__mode)                    vfs_fseek(FILE__pfile, i64_t__offset, int__mode)

#undef  ftell
#define ftell(FILE__pfile)                                              vfs_ftell(FILE__pfile)

#undef  ioctl
#define ioctl(FILE__pfile, ...)                                         vfs_ioctl(FILE__pfile, __VA_ARGS__)

#undef  fflush
#define fflush(FILE__pfile)                                             vfs_fflush(FILE__pfile)

#undef  fstat
#define fstat(FILE__pfile, struct_vfs_stat__pstat)                      vfs_fstat(FILE__pfile, struct_vfs_stat__pstat)

#undef  fflush
#define fflush(FILE__pfile)                                             vfs_fflush(FILE__pfile)

#undef  feof
#define feof(FILE__pfile)                                               vfs_feof(FILE__pfile)

#undef  rewind
#define rewind(FILE__pfile)                                             vfs_rewind(FILE__pfile)

#define DRIVER_INTERFACE(modname)                                               \
extern stdret_t modname##_init   (void**, uint, uint);                          \
extern stdret_t modname##_release(void*);                                       \
extern stdret_t modname##_open   (void*);                                       \
extern stdret_t modname##_close  (void*);                                       \
extern size_t   modname##_write  (void*, const void*, size_t, size_t, u64_t);   \
extern size_t   modname##_read   (void*, void*, size_t, size_t, u64_t);         \
extern stdret_t modname##_ioctl  (void*, int, va_list);                         \
extern stdret_t modname##_flush  (void*);                                       \
extern stdret_t modname##_info   (void*, struct vfs_dev_info*);

/*==============================================================================
  Exported types, enums definitions
==============================================================================*/

/*==============================================================================
  Exported object declarations
==============================================================================*/

/*==============================================================================
  Exported function prototypes
==============================================================================*/

#ifdef __cplusplus
}
#endif

#endif /* DNXMODULE_H_ */
/*==============================================================================
  End of file
==============================================================================*/
