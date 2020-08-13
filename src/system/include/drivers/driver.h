/*=========================================================================*//**
@file    driver.h

@author  Daniel Zorychta

@brief   This function provide all required function needed to write modules.

@note    Copyright (C) 2012 Daniel Zorychta <daniel.zorychta@gmail.com>

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
 * @defgroup driver-h "drivers/driver.h"
 *
 * This library is used by each driver and contains driver-specific
 * functions and @subpage sysfunc-h. This is main system library that shall
 * be used by drivers.
 *
 * @{
 */

#ifndef _DRIVER_H_
#define _DRIVER_H_

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

#ifndef DOXYGEN /* Doxygen documentation in sysfunc.h */
#undef  sys_zalloc
#define sys_zalloc(size_t__size, void__ppmem)                    _kzalloc(_MM_MOD, size_t__size, void__ppmem, _module_get_ID(_module_name_))

#undef  sys_malloc
#define sys_malloc(size_t__size, void__ppmem)                    _kmalloc(_MM_MOD, size_t__size, void__ppmem, _module_get_ID(_module_name_))

#undef  sys_free
#define sys_free(void__ppmem)                                    _kfree(_MM_MOD, void__ppmem, _module_get_ID(_module_name_))

#undef sys_get_driver_instance
#define sys_module_get_instance(u8_t__major, u8_t__minor, void_pp__mem) _module_get_instance(_module_name_, u8_t__major, u8_t__minor, void_pp__mem)

#undef  sys_llist_create
#define sys_llist_create(llist_cmp_functor_t__functor, llist_obj_dtor_t__obj_dtor, llist_t__pplist)\
        _llist_create_mod(_module_get_ID(_module_name_), llist_cmp_functor_t__functor, llist_obj_dtor_t__obj_dtor, llist_t__pplist);

#undef sys_btree_create
#define sys_btree_create(size_t__size, btree_cmp_functor_t__functor, btree_obj_dtor_t__obj_dtor, btree_t__pptree)\
        _btree_create_mod(_module_get_ID(_module_name_), size_t__size, btree_cmp_functor_t__functor, btree_obj_dtor_t__obj_dtor, btree_t__pptree)
#endif /* DOXYGEN */

#ifndef DOXYGEN
  #ifdef __cplusplus
    #define MODULE_NAME(modname) \
    static const char *_module_name_ = #modname;\
    inline void* operator new     (size_t size) {void *mem = NULL; _modmalloc(size, _module_get_ID(_module_name_), &mem); return mem;}\
    inline void* operator new[]   (size_t size) {void *mem = NULL; _modmalloc(size, _module_get_ID(_module_name_), &mem); return mem;}\
    inline void  operator delete  (void* ptr  ) {_modfree(&ptr, _module_get_ID(_module_name_));}\
    inline void  operator delete[](void* ptr  ) {_modfree(&ptr, _module_get_ID(_module_name_));}
    #define _MODULE_EXTERN_C extern "C"
  #else
    #define MODULE_NAME(modname) static const char *_module_name_ = #modname
    #define GET_MODULE_NAME() _module_name_
    #define _MODULE_EXTERN_C
  #endif /* __cplusplus */
#else /* defined(DOXYGEN) */
  /**
   * @brief Macro set name of module.
   *
   * Macro is used in each source file of driver to set module name.
   *
   * @note Macro can be used only by driver code.
   *
   * @param modname       module name
   *
   * @b Example
   * @code
          // ...
          MODULE_NAME(UART);
          // ...
     @endcode
   */
  #define MODULE_NAME(modname)
#endif /* DOXYGEN */

#ifdef DOXYGEN
/**
 * @brief Macro creates unique name of driver initialize function.
 *
 * Function created by this macro is called by system when driver is initialized.
 * Each driver with major and minor number can create own memory region.
 *
 * @note Macro can be used only by driver code.
 *
 * @param modname       module name
 * @param device_handle [<b>void **</b>]        driver memory (output)
 * @param major         [<b>u8_t</b>]           device major number
 * @param minor         [<b>u8_t</b>]           device minor number
 * @return One of @ref errno value.
 */
#define API_MOD_INIT(modname, device_handle, major, minor)
#else
#define API_MOD_INIT(modname, ...)              _MODULE_EXTERN_C int _##modname##_init(__VA_ARGS__)
#endif

#ifdef DOXYGEN
/**
 * @brief Macro creates unique name of driver release function.
 *
 * Function created by this macro is called by system when driver is released.
 *
 * @note Macro can be used only by driver code.
 *
 * @param modname       module name
 * @param device_handle [<b>void *</b>]         memory region allocated by driver
 * @return One of @ref errno value.
 */
#define API_MOD_RELEASE(modname, device_handle)
#else
#define API_MOD_RELEASE(modname, ...)           _MODULE_EXTERN_C int _##modname##_release(__VA_ARGS__)
#endif

#ifdef DOXYGEN
/**
 * @brief Macro creates unique name of driver open function.
 *
 * Function created by this macro is called by system when corresponding device
 * has to be opened.
 *
 * @note Macro can be used only by driver code.
 *
 * @param modname       module name
 * @param device_handle [<b>void *</b>]         memory region allocated by driver
 * @param flags         [<b>u32_t</b>]          flags
 * @return One of @ref errno value.
 *
 * @see O_RDONLY, O_WRONLY, O_RDWR
 */
#define API_MOD_OPEN(modname, device_handle, flags)
#else
#define API_MOD_OPEN(modname, ...)              _MODULE_EXTERN_C int _##modname##_open(__VA_ARGS__)
#endif

#ifdef DOXYGEN
/**
 * @brief Macro creates unique name of driver close function.
 *
 * Function created by this macro is called by system when corresponding device
 * has to be closed. System can force close by setting <i>force</i> flag to
 * true. In this case driver shall close device safe and immediately.
 *
 * @note Macro can be used only by driver code.
 *
 * @param modname       module name
 * @param device_handle [<b>void *</b>]         memory region allocated by driver
 * @param force         [<b>bool</b>]           force close driver (system request)
 * @return One of @ref errno value.
 */
#define API_MOD_CLOSE(modname, device_handle, force)
#else
#define API_MOD_CLOSE(modname, ...)             _MODULE_EXTERN_C int _##modname##_close(__VA_ARGS__)
#endif

#ifdef DOXYGEN
/**
 * @brief Macro creates unique name of driver write function.
 *
 * Function created by this macro is called by system when corresponding device
 * has to be written.
 *
 * @note Macro can be used only by driver code.
 *
 * @param modname       module name
 * @param device_handle [<b>void *</b>]         memory region allocated by driver
 * @param src           [<b>const u8_t *</b>]   source buffer
 * @param count         [<b>size_t</b>]         bytes to write
 * @param fpos          [<b>fpos_t *</b>]       file position indicator (can be modified)
 * @param wrcnt         [<b>size_t *</b>]       number of wrote bytes
 * @param fattr         [<b>struct vfs_fattr</b>] file access attributes
 * @return One of @ref errno value.
 *
 * @see struct vfs_fattr
 */
#define API_MOD_WRITE(modname, device_handle, src, count, fpos, wrcnt, fattr)
#else
#define API_MOD_WRITE(modname, ...)             _MODULE_EXTERN_C int _##modname##_write(__VA_ARGS__)
#endif

#ifdef DOXYGEN
/**
 * @brief Macro creates unique name of driver read function.
 *
 * Function created by this macro is called by system when corresponding device
 * has to be read.
 *
 * @note Macro can be used only by driver code.
 *
 * @param modname       module name
 * @param device_handle [<b>void *</b>]         memory region allocated by driver
 * @param dst           [<b>u8_t *</b>]         destination buffer
 * @param count         [<b>size_t</b>]         bytes to read
 * @param fpos          [<b>fpos_t *</b>]       file position indicator (can be modified)
 * @param rdcnt         [<b>size_t *</b>]       number of read bytes
 * @param fattr         [<b>struct vfs_fattr</b>] file access attributes
 * @return One of @ref errno value.
 *
 * @see struct vfs_fattr
 */
#define API_MOD_READ(modname, device_handle, dst, count, fpos, rdcnt, fattr)
#else
#define API_MOD_READ(modname, ...)              _MODULE_EXTERN_C int _##modname##_read(__VA_ARGS__)
#endif

#ifdef DOXYGEN
/**
 * @brief Macro creates unique name of driver IO control function.
 *
 * Function created by this macro is called by system when corresponding device
 * has to be controlled in non-standard way.
 *
 * @note Macro can be used only by driver code.
 *
 * @param modname       module name
 * @param device_handle [<b>void *</b>]         memory region allocated by driver
 * @param request       [<b>int</b>]            request
 * @param arg           [<b>void *</b>]         argument pointer
 * @return One of @ref errno value.
 */
#define API_MOD_IOCTL(modname, device_handle, request, arg)
#else
#define API_MOD_IOCTL(modname, ...)             _MODULE_EXTERN_C int _##modname##_ioctl(__VA_ARGS__)
#endif

#ifdef DOXYGEN
/**
 * @brief Macro creates unique name of driver flush function.
 *
 * Function created by this macro is called by system when buffers of corresponding
 * device has to be flushed to the device.
 *
 * @note Macro can be used only by driver code.
 *
 * @param modname       module name
 * @param device_handle [<b>void *</b>]         memory region allocated by driver
 * @return One of @ref errno value.
 */
#define API_MOD_FLUSH(modname, device_handle)
#else
#define API_MOD_FLUSH(modname, ...)             _MODULE_EXTERN_C int _##modname##_flush(__VA_ARGS__)
#endif

#ifdef DOXYGEN
/**
 * @brief Macro creates unique name of driver statistics function.
 *
 * Function created by this macro is called by system when device statistics
 * has to be read.
 *
 * @note Macro can be used only by driver code.
 *
 * @param modname       module name
 * @param device_handle [<b>void *</b>]         memory region allocated by driver
 * @param device_stat   [<b>struct vfs_dev_stat *</b>]    device statistics
 * @return One of @ref errno value.
 *
 * @see struct vfs_dev_stat
 */
#define API_MOD_STAT(modname, device_handle, device_stat)
#else
#define API_MOD_STAT(modname, ...)              _MODULE_EXTERN_C int _##modname##_stat(__VA_ARGS__)
#endif

/*==============================================================================
  Exported object types
==============================================================================*/
#ifdef DOXYGEN /* type in vfs.h */
/**
 * @brief Device statistic type.
 *
 * The type represents device information e.g. size and address.
 */
struct vfs_dev_stat {
        u64_t st_size;                  /*!< Total size, in bytes.*/
        u8_t  st_major;                 /*!< Device major number.*/
        u8_t  st_minor;                 /*!< Device minor number.*/
};

/**
 * @brief Device lock type.
 *
 * The type represents device lock object. Object is used to lock device access
 * from other processes. Type is a reference to other object address (with unique
 * address for each process). Object address stored in type is used only as
 * unique number that identify process.
 *
 * @note Type can be used only by driver code.
 */
typedef void* dev_lock_t;
#endif /* DOXYGEN*/

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
 * @brief Function locks device for this process.
 *
 * Function is used to lock access to the device opened by this thread.
 *
 * @note Function can be used only by driver code.
 *
 * @param dev_lock      pointer to device lock object
 *
 * @return If device is successfully locked then @ref ESUCC is returned,
 *         otherwise @ref EBUSY.
 *
 * @see sys_device_unlock()
 */
//==============================================================================
static inline int sys_device_lock(dev_lock_t *dev_lock)
{
        return _device_lock(dev_lock);
}

//==============================================================================
/**
 * @brief Function unlocks locked device.
 *
 * @note Function can be used only by driver code.
 *
 * @param dev_lock      pointer to device lock object
 * @param force         true: force unlock
 *
 * @return If device is successfully unlocked then @ref ESUCC is returned,
 *         otherwise @ref EBUSY (locked by other process).
 *
 * @see sys_device_lock()
 */
//==============================================================================
static inline int sys_device_unlock(dev_lock_t *dev_lock, bool force)
{
        return _device_unlock(dev_lock, force);
}

//==============================================================================
/**
 * @brief Function gets device access.
 *
 * Function gets device access when was locked by this process.
 *
 * @note Function can be used only by driver code.
 *
 * @param dev_lock      pointer to device lock object
 *
 * @return If device is successfully locked by this process then @ref ESUCC is
 *         returned, otherwise @ref EBUSY (locked by other process).
 *
 * @see sys_device_lock(), sys_device_unlock()
 */
//==============================================================================
static inline int sys_device_get_access(dev_lock_t *dev_lock)
{
        return _device_get_access(dev_lock);
}

//==============================================================================
/**
 * @brief Function checks that device is locked.
 *
 * @note Function can be used only by driver code.
 *
 * @param dev_lock      pointer to device lock object
 *
 * @return If device is unlocked then @b true is returned, otherwise @b false.
 *
 * @see sys_device_is_unlocked(), sys_device_lock(), sys_device_unlock(), sys_device_get_access()
 */
//==============================================================================
static inline bool sys_device_is_locked(dev_lock_t *dev_lock)
{
        return _device_is_locked(dev_lock);
}

//==============================================================================
/**
 * @brief Function checks that device is unlocked.
 *
 * @note Function can be used only by driver code.
 *
 * @param dev_lock      pointer to device lock object
 *
 * @return If device is unlocked then @b true is returned, otherwise @b false.
 *
 * @see sys_device_is_locked(), sys_device_lock(), sys_device_unlock(), sys_device_get_access()
 */
//==============================================================================
static inline bool sys_device_is_unlocked(dev_lock_t *dev_lock)
{
        return !_device_is_locked(dev_lock);
}

#ifdef __cplusplus
}
#endif

#endif /* _DRIVER_H_ */
/**@}*/
/*==============================================================================
  End of file
==============================================================================*/
