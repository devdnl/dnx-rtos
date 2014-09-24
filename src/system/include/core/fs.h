/*=========================================================================*//**
@file    fs.h

@author  Daniel Zorychta

@brief   This function provide all required function needed to write file systems.

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

#ifndef _DNXFS_H_
#define _DNXFS_H_

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Include files
==============================================================================*/
#include "config.h"
#include <sys/types.h>
#include "core/sysmoni.h"
#include "core/vfs.h"
#include "core/modctrl.h"
#include <errno.h>

/*==============================================================================
  Exported symbolic constants/macros
==============================================================================*/
#undef  calloc
#define calloc(size_t__nmemb, size_t__msize)    sysm_syscalloc(size_t__nmemb, size_t__msize)

#undef  malloc
#define malloc(size_t__size)                    sysm_sysmalloc(size_t__size)

#undef  free
#define free(void__pmem)                        sysm_sysfree(void__pmem)

#ifdef __cplusplus
        inline void* operator new     (size_t size) {return sysm_sysmalloc(size);}\
        inline void* operator new[]   (size_t size) {return sysm_sysmalloc(size);}\
        inline void  operator delete  (void* ptr  ) {sysm_sysfree(ptr);}\
        inline void  operator delete[](void* ptr  ) {sysm_sysfree(ptr);}
#       define _FS_EXTERN_C extern "C"
#else
#       define _FS_EXTERN_C
#endif

#define API_FS_INIT(fsname, ...)                _FS_EXTERN_C stdret_t _##fsname##_init(__VA_ARGS__)
#define API_FS_RELEASE(fsname, ...)             _FS_EXTERN_C stdret_t _##fsname##_release(__VA_ARGS__)
#define API_FS_OPEN(fsname, ...)                _FS_EXTERN_C stdret_t _##fsname##_open(__VA_ARGS__)
#define API_FS_CLOSE(fsname, ...)               _FS_EXTERN_C stdret_t _##fsname##_close(__VA_ARGS__)
#define API_FS_WRITE(fsname, ...)               _FS_EXTERN_C ssize_t _##fsname##_write(__VA_ARGS__)
#define API_FS_READ(fsname, ...)                _FS_EXTERN_C ssize_t _##fsname##_read(__VA_ARGS__)
#define API_FS_IOCTL(fsname, ...)               _FS_EXTERN_C stdret_t _##fsname##_ioctl(__VA_ARGS__)
#define API_FS_FSTAT(fsname, ...)               _FS_EXTERN_C stdret_t _##fsname##_fstat(__VA_ARGS__)
#define API_FS_FLUSH(fsname, ...)               _FS_EXTERN_C stdret_t _##fsname##_flush(__VA_ARGS__)
#define API_FS_MKDIR(fsname, ...)               _FS_EXTERN_C stdret_t _##fsname##_mkdir(__VA_ARGS__)
#define API_FS_MKFIFO(fsname, ...)              _FS_EXTERN_C stdret_t _##fsname##_mkfifo(__VA_ARGS__)
#define API_FS_MKNOD(fsname, ...)               _FS_EXTERN_C stdret_t _##fsname##_mknod(__VA_ARGS__)
#define API_FS_OPENDIR(fsname, ...)             _FS_EXTERN_C stdret_t _##fsname##_opendir(__VA_ARGS__)
#define API_FS_REMOVE(fsname, ...)              _FS_EXTERN_C stdret_t _##fsname##_remove(__VA_ARGS__)
#define API_FS_RENAME(fsname, ...)              _FS_EXTERN_C stdret_t _##fsname##_rename(__VA_ARGS__)
#define API_FS_CHMOD(fsname, ...)               _FS_EXTERN_C stdret_t _##fsname##_chmod(__VA_ARGS__)
#define API_FS_CHOWN(fsname, ...)               _FS_EXTERN_C stdret_t _##fsname##_chown(__VA_ARGS__)
#define API_FS_STAT(fsname, ...)                _FS_EXTERN_C stdret_t _##fsname##_stat(__VA_ARGS__)
#define API_FS_STATFS(fsname, ...)              _FS_EXTERN_C stdret_t _##fsname##_statfs(__VA_ARGS__)
#define API_FS_SYNC(fsname, ...)                _FS_EXTERN_C void     _##fsname##_sync(__VA_ARGS__)

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
 * @brief Function open selected driver
 *
 * @param id            module id
 * @param flags         flags
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
static inline stdret_t driver_open(dev_t id, vfs_open_flags_t flags)
{
        return _driver_open(id, flags);
}

//==============================================================================
/**
 * @brief Function close selected driver
 *
 * @param id            module id
 * @param force         force close request
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
static inline stdret_t driver_close(dev_t id, bool force)
{
        return _driver_close(id, force);
}

//==============================================================================
/**
 * @brief Function write data to driver
 *
 * @param id            module id
 * @param src           data source
 * @param count         buffer size
 * @param fpos          file position
 * @param fattr         file attributes
 *
 * @return number of written bytes, -1 on error
 */
//==============================================================================
static inline ssize_t driver_write(dev_t id, const u8_t *src, size_t count, fpos_t *fpos, struct vfs_fattr fattr)
{
        return _driver_write(id, src, count, fpos, fattr);
}

//==============================================================================
/**
 * @brief Function read data to driver
 *
 * @param id            module id
 * @param dst           data destination
 * @param count         buffer size
 * @param fpos          file position
 * @param fattr         file attributes
 *
 * @return number of read bytes, -1 on error
 */
//==============================================================================
static inline ssize_t driver_read(dev_t id, u8_t *dst, size_t count, fpos_t *fpos, struct vfs_fattr fattr)
{
        return _driver_read(id, dst, count, fpos, fattr);
}

//==============================================================================
/**
 * @brief IO control
 *
 * @param id            module id
 * @param request       io request
 * @param arg           argument
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
static inline stdret_t driver_ioctl(dev_t id, int request, void *arg)
{
        return _driver_ioctl(id, request, arg);
}

//==============================================================================
/**
 * @brief Flush device buffer (forces write)
 *
 * @param id            module id
 * @param request       io request
 * @param arg           argument
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
static inline stdret_t driver_flush(dev_t id)
{
        return _driver_flush(id);
}

//==============================================================================
/**
 * @brief Device information
 *
 * @param id            module id
 * @param stat          status object
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
static inline stdret_t driver_stat(dev_t id, struct vfs_dev_stat *stat)
{
        return _driver_stat(id, stat);
}

#ifdef __cplusplus
}
#endif

#endif /* _DNXFS_H_ */
/*==============================================================================
  End of file
==============================================================================*/
