#ifndef _DNXMODULE_H_
#define _DNXMODULE_H_
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
#include "core/drivers.h"
#include "kernel/kwrapper.h"

/*==============================================================================
  Exported symbolic constants/macros
==============================================================================*/
#ifdef _DNX_H_
#error "dnx.h and dnxmodule.h shall never included together!"
#endif

#undef  calloc
#define calloc(size_t__nmemb, size_t__msize)    sysm_modcalloc(size_t__nmemb, size_t__msize, _get_module_number(_module_name_))

#undef  malloc
#define malloc(size_t__size)                    sysm_modmalloc(size_t__size, _get_module_number(_module_name_))

#undef  free
#define free(void__pmem)                        sysm_modfree(void__pmem, _get_module_number(_module_name_))

#define STOP_IF(condition)                      _stop_if(condition)

#define _API_MOD_INIT(modname, ...)             stdret_t _##modname##_init(__VA_ARGS__)
#define API_MOD_INIT(modname, ...)              static const char *_module_name_ = #modname; _API_MOD_INIT(modname, __VA_ARGS__)
#define API_MOD_RELEASE(modname, ...)           stdret_t _##modname##_release(__VA_ARGS__)
#define API_MOD_OPEN(modname, ...)              stdret_t _##modname##_open(__VA_ARGS__)
#define API_MOD_CLOSE(modname, ...)             stdret_t _##modname##_close(__VA_ARGS__)
#define API_MOD_WRITE(modname, ...)             size_t _##modname##_write(__VA_ARGS__)
#define API_MOD_READ(modname, ...)              size_t _##modname##_read(__VA_ARGS__)
#define API_MOD_IOCTL(modname, ...)             stdret_t _##modname##_ioctl(__VA_ARGS__)
#define API_MOD_FLUSH(modname, ...)             stdret_t _##modname##_flush(__VA_ARGS__)
#define API_MOD_STAT(modname, ...)              stdret_t _##modname##_stat(__VA_ARGS__)

#define _IMPORT_MODULE(modname)                                          \
extern _API_MOD_INIT(modname, void**, u8_t, u8_t);                       \
extern API_MOD_RELEASE(modname, void*);                                  \
extern API_MOD_OPEN(modname, void*, int);                                \
extern API_MOD_CLOSE(modname, void*, bool, const task_t*);               \
extern API_MOD_WRITE(modname, void*, const u8_t*, size_t, u64_t*);       \
extern API_MOD_READ(modname, void*, u8_t*, size_t, u64_t*);              \
extern API_MOD_IOCTL(modname, void*, int, void*);                        \
extern API_MOD_FLUSH(modname, void*);                                    \
extern API_MOD_STAT(modname, void*, struct vfs_dev_stat*)

/*==============================================================================
  Exported types, enums definitions
==============================================================================*/

/*==============================================================================
  Exported object declarations
==============================================================================*/

/*==============================================================================
  Exported function prototypes
==============================================================================*/

/*==============================================================================
  Exported inline function
==============================================================================*/

#ifdef __cplusplus
}
#endif

#endif /* _DNXMODULE_H_ */
/*==============================================================================
  End of file
==============================================================================*/
