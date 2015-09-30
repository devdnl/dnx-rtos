/*=========================================================================*//**
@file    module.h

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

#ifndef _DNXMODULE_H_
#define _DNXMODULE_H_

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Include files
==============================================================================*/
#include "config.h"
#include <sys/types.h>
#include <string.h>
#include "kernel/errno.h"
#include "kernel/sysfunc.h"
#include "drivers/drvctrl.h"
#include "lib/vt100.h"
#include "lib/cast.h"

/*==============================================================================
  Exported symbolic constants/macros
==============================================================================*/
#undef errno

#undef  _sys_zalloc
#define _sys_zalloc(size_t__size, void__ppmem)                    _kzalloc(_MM_MOD, size_t__size, void__ppmem, _get_module_number(_module_name_))

#undef  _sys_malloc
#define _sys_malloc(size_t__size, void__ppmem)                    _kmalloc(_MM_MOD, size_t__size, void__ppmem, _get_module_number(_module_name_))

#undef  _sys_free
#define _sys_free(void__ppmem)                                    _kfree(_MM_MOD, void__ppmem, _get_module_number(_module_name_))

#undef  _sys_llist_create
#define _sys_llist_create(llist_cmp_functor_t__functor, llist_obj_dtor_t__obj_dtor, llist_t__pplist)\
        _llist_create_mod(_get_module_number(_module_name_), llist_cmp_functor_t__functor, llist_obj_dtor_t__obj_dtor, llist_t__pplist);

#ifdef __cplusplus
#       define MODULE_NAME(modname) \
        static const char *_module_name_ = #modname;\
        inline void* operator new     (size_t size) {void *mem = NULL; _modmalloc(size, _get_module_number(_module_name_), &mem); return mem;}\
        inline void* operator new[]   (size_t size) {void *mem = NULL; _modmalloc(size, _get_module_number(_module_name_), &mem); return mem;}\
        inline void  operator delete  (void* ptr  ) {_modfree(&ptr, _get_module_number(_module_name_));}\
        inline void  operator delete[](void* ptr  ) {_modfree(&ptr, _get_module_number(_module_name_));}
#       define _MODULE_EXTERN_C extern "C"
#else
#       define MODULE_NAME(modname) \
        static const char *_module_name_ = #modname
#       define _MODULE_EXTERN_C
#endif

#define API_MOD_INIT(modname, ...)              _MODULE_EXTERN_C int _##modname##_init(__VA_ARGS__)
#define API_MOD_RELEASE(modname, ...)           _MODULE_EXTERN_C int _##modname##_release(__VA_ARGS__)
#define API_MOD_OPEN(modname, ...)              _MODULE_EXTERN_C int _##modname##_open(__VA_ARGS__)
#define API_MOD_CLOSE(modname, ...)             _MODULE_EXTERN_C int _##modname##_close(__VA_ARGS__)
#define API_MOD_WRITE(modname, ...)             _MODULE_EXTERN_C int _##modname##_write(__VA_ARGS__)
#define API_MOD_READ(modname, ...)              _MODULE_EXTERN_C int _##modname##_read(__VA_ARGS__)
#define API_MOD_IOCTL(modname, ...)             _MODULE_EXTERN_C int _##modname##_ioctl(__VA_ARGS__)
#define API_MOD_FLUSH(modname, ...)             _MODULE_EXTERN_C int _##modname##_flush(__VA_ARGS__)
#define API_MOD_STAT(modname, ...)              _MODULE_EXTERN_C int _##modname##_stat(__VA_ARGS__)

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
//==============================================================================
/**
 * @brief Function lock device for this process
 *
 * ERRNO: EBUSY
 *
 * @param *dev_lock     pointer to device lock object
 *
 * @return One of errno value (ESUCC for success)
 */
//==============================================================================
static inline int _sys_device_lock(dev_lock_t *dev_lock)
{
        return _lock_device(dev_lock);
}

//==============================================================================
/**
 * @brief Function unlock device locked by _sys_device_lock()
 *
 * @param *dev_lock     pointer to device lock object
 * @param  force        true: force unlock
 *
 * @return One of errno value (ESUCC for success)
 */
//==============================================================================
static inline int _sys_device_unlock(dev_lock_t *dev_lock, bool force)
{
        return _unlock_device(dev_lock, force);
}

//==============================================================================
/**
 * @brief Function get device access
 *
 * @param *dev_lock     pointer to device lock object
 *
 * @return One of errno value (ESUCC for success)
 */
//==============================================================================
static inline int _sys_device_get_access(dev_lock_t *dev_lock)
{
        return _get_access_to_device(dev_lock);
}

//==============================================================================
/**
 * @brief Function check that device is locked
 *
 * @param *dev_lock     pointer to device lock object
 *
 * @return true if locked, otherwise false
 */
//==============================================================================
static inline bool _sys_device_is_locked(dev_lock_t *dev_lock)
{
        return _is_device_locked(dev_lock);
}

//==============================================================================
/**
 * @brief Function check that device is locked
 *
 * @param *dev_lock     pointer to device lock object
 *
 * @return true if locked, otherwise false
 */
//==============================================================================
static inline bool _sys_device_is_unlocked(dev_lock_t *dev_lock)
{
        return !_is_device_locked(dev_lock);
}

#ifdef __cplusplus
}
#endif

#endif /* _DNXMODULE_H_ */
/*==============================================================================
  End of file
==============================================================================*/
