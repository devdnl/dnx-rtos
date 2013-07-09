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
#include "user/regdrv.h"
#include "kernel/kwrapper.h"

/*==============================================================================
  Exported symbolic constants/macros
==============================================================================*/
#ifdef DNX_H_
#error "dnx.h and dnxmodule.h shall never included together!"
#endif

#define MODULE_NAME(const_char__pmodule_name)   static const char *__module_name__ = #const_char__pmodule_name

#undef  calloc
#define calloc(size_t__nmemb, size_t__msize)    sysm_modcalloc(size_t__nmemb, size_t__msize, regdrv_get_module_number(__module_name__))

#undef  malloc
#define malloc(size_t__size)                    sysm_modmalloc(size_t__size, regdrv_get_module_number(__module_name__))

#undef  free
#define free(void__pmem)                        sysm_modfree(void__pmem, regdrv_get_module_number(__module_name__))

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

#endif /* _DNXMODULE_H_ */
/*==============================================================================
  End of file
==============================================================================*/
