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

/**
 * @defgroup fs-h "fs/fs.h"
 *
 * This library is used by each file system and contains file system-specific
 * functions and @subpage sysfunc-h. This is main and only one library that shall
 * be used by file system code.
 *
 * @{
 */

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
#include <dnx/misc.h>
#include <string.h>
#include <errno.h>
#include "drivers/drvctrl.h"
#include "drivers/ioctl_requests.h"
#include "kernel/sysfunc.h"
#include "kernel/process.h"
#include "fs/pipe.h"

/*==============================================================================
  Exported symbolic constants/macros
==============================================================================*/
#ifndef DOXYGEN /* disabled for Doxygen documentation */
#undef errno
#undef _sys_calloc
#undef _sys_malloc
#undef _sys_free

#ifdef __cplusplus
inline void* operator new     (size_t size) {void *mem = NULL; _kmalloc(_MM_FS, size, &mem); return mem;}\
inline void* operator new[]   (size_t size) {void *mem = NULL; _kmalloc(_MM_FS, size, &mem); return mem;}\
inline void  operator delete  (void* ptr  ) {_sysfree(&ptr);}\
inline void  operator delete[](void* ptr  ) {_sysfree(&ptr);}
#define _FS_EXTERN_C extern "C"
#else
#define _FS_EXTERN_C
#endif
#endif /* DOXYGEN */

#ifdef DOXYGEN /* macros defined in vfs.h */
/**
 * @brief Read only flag.
 */
#define O_RDONLY                                00

/**
* @brief Write only flag.
*/
#define O_WRONLY                                01

/**
* @brief Read write flag.
*/
#define O_RDWR                                  02

/**
* @brief File create flag.
*/
#define O_CREAT                                 0100

/**
* @brief File execute flag.
*/
#define O_EXCL                                  0200

/**
* @brief File truncate flag.
*/
#define O_TRUNC                                 01000

/**
* @brief File append flag.
*/
#define O_APPEND                                02000
#endif /* DOXYGEN */

/**
 * @brief Macro creates unique name of initialization function.
 * @param fsname        file system name
 * @param fs_handle     [<b>void **</b>]        file system memory handler
 * @param src_path      [<b>const char *</b>]   source file path
 * @return One of @ref errno value.
 */
#ifdef DOXYGEN
#define API_FS_INIT(fsname, fs_handle, src_path)
#else
#define API_FS_INIT(fsname, ...)        _FS_EXTERN_C int _##fsname##_init(__VA_ARGS__)
#endif

/**
 * @brief Macro creates unique name of release function.
 * @param fsname        file system name
 * @param fs_handle     [<b>void *</b>]         file system memory handler
 * @return One of @ref errno value.
 */
#ifdef DOXYGEN
#define API_FS_RELEASE(fsname, fs_handle)
#else
#define API_FS_RELEASE(fsname, ...)     _FS_EXTERN_C int _##fsname##_release(__VA_ARGS__)
#endif

/**
 * @brief Macro creates unique name of file open function.
 * @param fsname        file system name
 * @param fs_handle     [<b>void *</b>]         file system memory handler
 * @param extra         [<b>void **</b>]        file extra data (user defined)
 * @param fd            [<b>fd_t *</b>]         file descriptor (user defined)
 * @param fpos          [<b>fpos_t *</b>]       file position indicator
 * @param path          [<b>const char *</b>]   file path
 * @param flags         [<b>u32_t</b>]          file flags (O_*)
 * @return One of @ref errno value.
 *
 * @see Flags: O_RDONLY, O_WRONLY, O_RDWR, O_CREAT, O_EXCL, O_TRUNC, O_APPEND
 */
#ifdef DOXYGEN
#define API_FS_OPEN(fsname, fs_handle, extra, fd, fpos, path, flags)
#else
#define API_FS_OPEN(fsname, ...)        _FS_EXTERN_C int _##fsname##_open(__VA_ARGS__)
#endif

/**
 * @brief Macro creates unique name of file close function.
 * @param fsname        file system name
 * @param fs_handle     [<b>void *</b>]         file system memory handler
 * @param extra         [<b>void *</b>]         file extra data
 * @param fd            [<b>fd_t</b>]           file descriptor
 * @param force         [<b>bool</b>]           force file close (system request)
 * @return One of @ref errno value.
 */
#ifdef DOXYGEN
#define API_FS_CLOSE(fsname, fs_handle, extra, fd, force)
#else
#define API_FS_CLOSE(fsname, ...)       _FS_EXTERN_C int _##fsname##_close(__VA_ARGS__)
#endif

/**
 * @brief Macro creates unique name of file write function.
 * @param fsname        file system name
 * @param fs_handle     [<b>void *</b>]         file system memory handler
 * @param extra         [<b>void *</b>]         file extra data
 * @param fd            [<b>fd_t</b>]           file descriptor
 * @param src           [<b>const u8_t *</b>]   source buffer
 * @param count         [<b>size_t</b>]         bytes to write
 * @param fpos          [<b>fpos_t *</b>]       file position indicator (can be modified)
 * @param wrcnt         [<b>size_t *</b>]       number of wrote bytes
 * @param fattr         [<b>struct vfs_fattr</b>] file access attributes
 * @return One of @ref errno value.
 *
 * @see struct vfs_fattr
 */
#ifdef DOXYGEN
#define API_FS_WRITE(fsname, fs_handle, extra, fd, src, count, fpos, wrcnt, fattr)
#else
#define API_FS_WRITE(fsname, ...)       _FS_EXTERN_C int _##fsname##_write(__VA_ARGS__)
#endif

/**
 * @brief Macro creates unique name of file read function.
 * @param fsname        file system name
 * @param fs_handle     [<b>void *</b>]         file system memory handler
 * @param extra         [<b>void *</b>]         file extra data
 * @param fd            [<b>fd_t</b>]           file descriptor
 * @param dst           [<b>u8_t *</b>]         destination buffer
 * @param count         [<b>size_t</b>]         bytes to read
 * @param fpos          [<b>fpos_t *</b>]       file position indicator (can be modified)
 * @param rdcnt         [<b>size_t *</b>]       number of read bytes
 * @param fattr         [<b>struct vfs_fattr</b>] file access attributes
 * @return One of @ref errno value.
 *
 * @see struct vfs_fattr
 */
#ifdef DOXYGEN
#define API_FS_READ(fsname, fs_handle, extra, fd, dst, count, fpos, rdcnt, fattr)
#else
#define API_FS_READ(fsname, ...)        _FS_EXTERN_C int _##fsname##_read(__VA_ARGS__)
#endif

/**
 * @brief Macro creates unique name of file ioctl function.
 * @param fsname        file system name
 * @param fs_handle     [<b>void *</b>]         file system memory handler
 * @param extra         [<b>void *</b>]         file extra data
 * @param fd            [<b>fd_t</b>]           file descriptor
 * @param request       [<b>int</b>]            ioctl request
 * @param arg           [<b>void *</b>]         ioctl argument
 * @return One of @ref errno value.
 *
 * @see ioctl()
 */
#ifdef DOXYGEN
#define API_FS_IOCTL(fsname, fs_handle, extra, fd, request, arg)
#else
#define API_FS_IOCTL(fsname, ...)       _FS_EXTERN_C int _##fsname##_ioctl(__VA_ARGS__)
#endif

/**
 * @brief Macro creates unique name of file buffer flush function.
 * @param fsname        file system name
 * @param fs_handle     [<b>void *</b>]         file system memory handler
 * @param extra         [<b>void *</b>]         file extra data
 * @param fd            [<b>fd_t</b>]           file descriptor
 * @return One of @ref errno value.
 *
 * @see sync()
 */
#ifdef DOXYGEN
#define API_FS_FLUSH(fsname, fs_handle, extra)
#else
#define API_FS_FLUSH(fsname, ...)       _FS_EXTERN_C int _##fsname##_flush(__VA_ARGS__)
#endif

/**
 * @brief Macro creates unique name of directory create function.
 * @param fsname        file system name
 * @param fs_handle     [<b>void *</b>]         file system memory handler
 * @param path          [<b>const char *</b>]   directory path
 * @param mode          [<b>mode_t</b>]         directory mode (permissions)
 * @return One of @ref errno value.
 *
 * @see mode_t
 */
#ifdef DOXYGEN
#define API_FS_MKDIR(fsname, fs_handle, path, mode)
#else
#define API_FS_MKDIR(fsname, ...)       _FS_EXTERN_C int _##fsname##_mkdir(__VA_ARGS__)
#endif

/**
 * @brief Macro creates unique name of FIFO create function.
 * @param fsname        file system name
 * @param fs_handle     [<b>void *</b>]         file system memory handler
 * @param path          [<b>const char *</b>]   FIFO path
 * @param mode          [<b>mode_t</b>]         FIFO mode (permissions)
 * @return One of @ref errno value.
 *
 * @see mode_t
 */
#ifdef DOXYGEN
#define API_FS_MKFIFO(fsname, fs_handle, path, mode)
#else
#define API_FS_MKFIFO(fsname, ...)      _FS_EXTERN_C int _##fsname##_mkfifo(__VA_ARGS__)
#endif

/**
 * @brief Macro creates unique name of device node create function.
 * @param fsname        file system name
 * @param fs_handle     [<b>void *</b>]         file system memory handler
 * @param path          [<b>const char *</b>]   device node path
 * @param dev           [<b>dev_t</b>]          device ID
 * @return One of @ref errno value.
 *
 * @see dev_t
 */
#ifdef DOXYGEN
#define API_FS_MKNOD(fsname, fs_handle, path, dev)
#else
#define API_FS_MKNOD(fsname, ...)       _FS_EXTERN_C int _##fsname##_mknod(__VA_ARGS__)
#endif

/**
 * @brief Macro creates unique name of open directory function.
 * @param fsname        file system name
 * @param fs_handle     [<b>void *</b>]         file system memory handler
 * @param path          [<b>const char *</b>]   directory path
 * @param dir           [<b>DIR *</b>]          directory object (already created)
 * @return One of @ref errno value.
 *
 * @see DIR
 */
#ifdef DOXYGEN
#define API_FS_OPENDIR(fsname, fs_handle, path, dir)
#else
#define API_FS_OPENDIR(fsname, ...)     _FS_EXTERN_C int _##fsname##_opendir(__VA_ARGS__)
#endif

/**
 * @brief Macro creates unique name of file remove function.
 * @param fsname        file system name
 * @param fs_handle     [<b>void *</b>]         file system memory handler
 * @param path          [<b>const char *</b>]   file path
 * @return One of @ref errno value.
 */
#ifdef DOXYGEN
#define API_FS_REMOVE(fsname, fs_handle, path)
#else
#define API_FS_REMOVE(fsname, ...)      _FS_EXTERN_C int _##fsname##_remove(__VA_ARGS__)
#endif

/**
 * @brief Macro creates unique name of file rename function.
 * @param fsname        file system name
 * @param fs_handle     [<b>void *</b>]         file system memory handler
 * @param old_name      [<b>const char *</b>]   old file path (name)
 * @param new_name      [<b>const char *</b>]   new file path (name)
 * @return One of @ref errno value.
 */
#ifdef DOXYGEN
#define API_FS_RENAME(fsname, fs_handle, old_name, new_name)
#else
#define API_FS_RENAME(fsname, ...)      _FS_EXTERN_C int _##fsname##_rename(__VA_ARGS__)
#endif

/**
 * @brief Macro creates unique name of file change mode function.
 * @param fsname        file system name
 * @param fs_handle     [<b>void *</b>]         file system memory handler
 * @param path          [<b>const char *</b>]   file path
 * @param mode          [<b>mode_t</b>]         new file mode (permissions)
 * @return One of @ref errno value.
 *
 * @see mode_t
 */
#ifdef DOXYGEN
#define API_FS_CHMOD(fsname, fs_handle, path, mode)
#else
#define API_FS_CHMOD(fsname, ...)       _FS_EXTERN_C int _##fsname##_chmod(__VA_ARGS__)
#endif

/**
 * @brief Macro creates unique name of file change owner function.
 * @param fsname        file system name
 * @param fs_handle     [<b>void *</b>]         file system memory handler
 * @param path          [<b>const char *</b>]   file path
 * @param owner         [<b>uid_t</b>]          owner ID
 * @param group         [<b>gid_t</b>]          group ID
 * @return One of @ref errno value.
 *
 * @see uid_t, gid_t
 */
#ifdef DOXYGEN
#define API_FS_CHOWN(fsname, fs_handle, path, owner, group)
#else
#define API_FS_CHOWN(fsname, ...)       _FS_EXTERN_C int _##fsname##_chown(__VA_ARGS__)
#endif

/**
 * @brief Macro creates unique name of file statistics function.
 * @param fsname        file system name
 * @param fs_handle     [<b>void *</b>]         file system memory handler
 * @param extra         [<b>void *</b>]         file extra data
 * @param fd            [<b>fd_t</b>]           file descriptor
 * @param stat          [<b>struct stat *</b>]  file information
 * @return One of @ref errno value.
 *
 * @see struct stat
 */
#ifdef DOXYGEN
#define API_FS_FSTAT(fsname, fs_handle, extra, fd, stat)
#else
#define API_FS_FSTAT(fsname, ...)       _FS_EXTERN_C int _##fsname##_fstat(__VA_ARGS__)
#endif

/**
 * @brief Macro creates unique name of file statistics function.
 * @param fsname        file system name
 * @param fs_handle     [<b>void *</b>]         file system memory handler
 * @param extra         [<b>void *</b>]         file extra data
 * @param path          [<b>const char *</b>]   file path
 * @param stat          [<b>struct stat *</b>]  file information
 * @return One of @ref errno value.
 *
 * @see struct stat
 */
#ifdef DOXYGEN
#define API_FS_STAT(fsname, fs_handle, extra, path, stat)
#else
#define API_FS_STAT(fsname, ...)        _FS_EXTERN_C int _##fsname##_stat(__VA_ARGS__)
#endif

/**
 * @brief Macro creates unique name of file system statistics function.
 * @param fsname        file system name
 * @param fs_handle     [<b>void *</b>]          file system memory handler
 * @param statfs        [<b>struct statfs *</b>] file system information
 * @return One of @ref errno value.
 *
 * @see struct statfs
 */
#ifdef DOXYGEN
#define API_FS_STATFS(fsname, fs_handle, statfs)
#else
#define API_FS_STATFS(fsname, ...)      _FS_EXTERN_C int _##fsname##_statfs(__VA_ARGS__)
#endif

/**
 * @brief Macro creates unique name of file system synchronize function.
 * @param fsname        file system name
 * @param fs_handle     [<b>void *</b>]         file system memory handler
 * @return One of @ref errno value.
 */
#ifdef DOXYGEN
#define API_FS_SYNC(fsname, fs_handle)
#else
#define API_FS_SYNC(fsname, ...)        _FS_EXTERN_C int _##fsname##_sync(__VA_ARGS__)
#endif

/*==============================================================================
  Exported types, enums definitions
==============================================================================*/
#ifdef DOXYGEN
/**
 * @brief Structure indicate file access mode.
 */
struct vfs_fattr {
        bool non_blocking_rd;         //!< Non-blocking file read access
        bool non_blocking_wr;         //!< Non-blocking file write access
};

/**
 * @brief Structure describe built-in program data.
 * @see   _sys_get_programs_table()
 */
 struct _prog_data {
        const char     *name;           //!< Program name
        const size_t   *globals_size;   //!< Size of program global variables
        const size_t   *stack_depth;    //!< Stack depth
        process_func_t  main;           //!< Program main function
};
#endif

/*==============================================================================
  Exported object declarations
==============================================================================*/

/*==============================================================================
  Exported function prototypes
==============================================================================*/

/*==============================================================================
  Exported inline function
==============================================================================*/
#ifndef DOXYGEN /* functions described general in sysfunc.h header */
//==============================================================================
/**
 * @brief  Allocate memory
 *
 * @param size             object size
 * @param mem              pointer to memory block pointer
 *
 * @return One of @ref errno value.
 */
//==============================================================================
static inline int _sys_malloc(size_t size, void **mem)
{
        return _kmalloc(_MM_FS, size, mem);
}

//==============================================================================
/**
 * @brief  Allocate memory and clear content
 *
 * @param size             object size
 * @param mem              pointer to memory block pointer
 *
 * @return One of @ref errno value.
 */
//==============================================================================
static inline int _sys_zalloc(size_t size, void **mem)
{
        return _kzalloc(_MM_FS, size, mem);
}

//==============================================================================
/**
 * @brief  Free allocated memory
 *
 * @param mem           pointer to memory block to free
 *
 * @return One of @ref errno value.
 */
//==============================================================================
static inline int _sys_free(void **mem)
{
        return _kfree(_MM_FS, mem);
}

//==============================================================================
/**
 * @brief  List constructor (for FS only)
 * @param  cmp_functor          compare functor (can be NULL)
 * @param  obj_dtor             object destructor (can be NULL, then free() is destructor)
 * @return One of @ref errno value.
 */
//==============================================================================
static inline int _sys_llist_create(llist_cmp_functor_t functor, llist_obj_dtor_t obj_dtor, llist_t **list)
{
        return _llist_create_krn(_MM_FS, functor, obj_dtor, list);
}

#endif /* DOXYGEN */

//==============================================================================
/**
 * @brief Function open selected driver
 *
 * @param id            module id
 * @param flags         flags
 *
 * @return One of @ref errno value.
 */
//==============================================================================
static inline int _sys_driver_open(dev_t id, u32_t flags)
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
 * @return One of @ref errno value.
 */
//==============================================================================
static inline int _sys_driver_close(dev_t id, bool force)
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
 * @param wrcnt         number of written bytes
 * @param fattr         file attributes
 *
 * @return One of @ref errno value.
 */
//==============================================================================
static inline int _sys_driver_write(dev_t            id,
                                    const u8_t      *src,
                                    size_t           count,
                                    fpos_t          *fpos,
                                    size_t          *wrcnt,
                                    struct vfs_fattr fattr)
{
        return _driver_write(id, src, count, fpos, wrcnt, fattr);
}

//==============================================================================
/**
 * @brief Function read data to driver
 *
 * @param id            module id
 * @param dst           data destination
 * @param count         buffer size
 * @param fpos          file position
 * @param rdcnt         number of read byes
 * @param fattr         file attributes
 *
 * @return One of @ref errno value.
 */
//==============================================================================
static inline int _sys_driver_read(dev_t            id,
                                   u8_t            *dst,
                                   size_t           count,
                                   fpos_t          *fpos,
                                   size_t          *rdcnt,
                                   struct vfs_fattr fattr)
{
        return _driver_read(id, dst, count, fpos, rdcnt, fattr);
}

//==============================================================================
/**
 * @brief IO control
 *
 * @param id            module id
 * @param request       io request
 * @param arg           argument
 *
 * @return One of @ref errno value.
 */
//==============================================================================
static inline int _sys_driver_ioctl(dev_t id, int request, void *arg)
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
 * @return One of @ref errno value.
 */
//==============================================================================
static inline int _sys_driver_flush(dev_t id)
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
 * @return One of @ref errno value.
 */
//==============================================================================
static inline int _sys_driver_stat(dev_t id, struct vfs_dev_stat *stat)
{
        return _driver_stat(id, stat);
}

//==============================================================================
/**
 * @brief Create pipe object
 *
 * @param pipe     pointer to pointer of pipe handle
 *
 * @return One of @ref errno value.
 */
//==============================================================================
static inline int _sys_pipe_create(pipe_t **pipe)
{
        return _pipe_create(pipe);
}

//==============================================================================
/**
 * @brief Destroy pipe object
 *
 * @param pipe          a pipe object
 *
 * @return One of @ref errno value.
 */
//==============================================================================
static inline int _sys_pipe_destroy(pipe_t *pipe)
{
        return _pipe_destroy(pipe);
}

//==============================================================================
/**
 * @brief Return length of pipe
 *
 * @param pipe          a pipe object
 * @param len           a pipe length
 *
 * @return One of @ref errno value.
 */
//==============================================================================
static inline int _sys_pipe_get_length(pipe_t *pipe, size_t *len)
{
        return _pipe_get_length(pipe, len);
}

//==============================================================================
/**
 * @brief Read data from pipe
 *
 * @param pipe          a pipe object
 * @param buf           a destination buffer
 * @param count         a count of bytes to read
 * @param rdcnt         a number of read bytes
 * @param non_blocking  a non-blocking access mode
 *
 * @return One of @ref errno value.
 */
//==============================================================================
static inline int _sys_pipe_read(pipe_t *pipe, u8_t *buf, size_t count, size_t *rdcnt, bool non_blocking)
{
        return _pipe_read(pipe, buf, count, rdcnt, non_blocking);
}

//==============================================================================
/**
 * @brief Read data from pipe
 *
 * @param pipe          a pipe object
 * @param buf           a destination buffer
 * @param count         a count of bytes to read
 * @param wrcnt         a number of written bytes
 * @param non_blocking  a non-blocking access mode
 *
 * @return One of @ref errno value.
 */
//==============================================================================
static inline int _sys_pipe_write(pipe_t *pipe, const u8_t *buf, size_t count, size_t *wrcnt, bool non_blocking)
{
        return _pipe_write(pipe, buf, count, wrcnt, non_blocking);
}

//==============================================================================
/**
 * @brief Close pipe
 *
 * @param pipe          a pipe object
 *
 * @return One of @ref errno value.
 */
//==============================================================================
static inline int _sys_pipe_close(pipe_t *pipe)
{
        return _pipe_close(pipe);
}

//==============================================================================
/**
 * @brief  Clear pipe
 *
 * @param  pipe         a pipe object
 *
 * @return One of @ref errno value.
 */
//==============================================================================
static inline int _sys_pipe_clear(pipe_t *pipe)
{
        return _pipe_clear(pipe);
}

//==============================================================================
/**
 * @brief  Function return size of programs table (number of programs)
 *
 * @return Return number of programs
 */
//==============================================================================
static inline int _sys_get_programs_table_size()
{
        return _get_programs_table_size();
}

//==============================================================================
/**
 * @brief  Function return pointer to beginning of programs table
 *
 * @return Return pointer to programs table
 */
//==============================================================================
static inline const struct _prog_data *_sys_get_programs_table()
{
        return _get_programs_table();
}

#ifdef __cplusplus
}
#endif

#endif /* _DNXFS_H_ */
/**@}*/
/*==============================================================================
  End of file
==============================================================================*/
