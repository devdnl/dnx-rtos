/*=========================================================================*//**
@file    sysfunc.h

@author  Daniel Zorychta

@brief   System function that must be used in drivers (modules) and file systems.

@note    Copyright (C) 2015 Daniel Zorychta <daniel.zorychta@gmail.com>

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

#ifndef _SYSFUNC_H_
#define _SYSFUNC_H_

/*==============================================================================
  Include files
==============================================================================*/
#include <stdbool.h>
#include "dnx/misc.h"
#include "sys/types.h"
#include "lib/unarg.h"
#include "lib/conv.h"
#include "lib/llist.h"
#include "lib/vsnprintf.h"
#include "lib/vfprintf.h"
#include "lib/vsscanf.h"
#include "kernel/errno.h"
#include "kernel/printk.h"
#include "kernel/kwrapper.h"
#include "kernel/time.h"
#include "kernel/process.h"
#include "fs/vfs.h"
#include "drivers/drvctrl.h"
#include "portable/cpuctl.h"

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Exported macros
==============================================================================*/
// list foreach iterator
#define _sys_llist_foreach(type, element, _sys_llist_t__list)\
        _llist_foreach(type, element, _sys_llist_t__list)

// list reversed foreach iterator
#define _sys_llist_foreach_reverse(type, element, _sys_llist_t__list)\
        _llist_foreach_reverse(type, element, _sys_llist_t__list)

// list iterator
#define _sys_llist_foreach_iterator _iterator

/*==============================================================================
  Exported object types
==============================================================================*/
typedef struct {
        tid_t   tid;
        task_t *task;
} thread_t;

/*==============================================================================
  Exported objects
==============================================================================*/

/*==============================================================================
  Exported functions
==============================================================================*/

/*==============================================================================
  Exported inline functions
==============================================================================*/
//==============================================================================
/**
 * @brief Function convert string to double
 *
 * @param[in]  str            string
 * @param[out] end            the pointer to the character when conversion was finished
 *
 * @return converted value
 */
//==============================================================================
static inline double _sys_strtod(const char *str, char **end)
{
        return _strtod(str, end);
}

//==============================================================================
/**
 * @brief Function convert string to integer
 *
 * @param[in] str       string
 *
 * @return converted value
 */
//==============================================================================
static inline i32_t _sys_atoi(const char *str)
{
        return _atoi(str);
}

//==============================================================================
/**
 * @brief Function convert ASCII to the number
 * When function find any other character than number (depended of actual base)
 * immediately finished operation and return pointer when bad character was
 * found. If base is 0 then function recognize type of number used in string.
 * For hex values "0x" is recognized, for octal values "0" at the beginning of
 * string is recognized, for binary "0b" is recognized, and for decimals values
 * none above.
 *
 * @param[in]  string       string to decode
 * @param[in]  base         decode base
 * @param[out] value        pointer to result
 *
 * @return pointer in string when operation was finished
 */
//==============================================================================
static inline char *_sys_strtoi(const char *string, int base, i32_t *value)
{
        return _strtoi(string, base, value);
}

//==============================================================================
/**
 * @brief Function convert string to float
 *
 * @param[in] str      string
 *
 * @return converted value
 */
//==============================================================================
static inline double _sys_atof(const char *str)
{
        return _atof(str);
}

//==============================================================================
/**
 * @brief  List destructor
 * @param  list         list object
 * @return On success 1 is returned, otherwise 0
 */
//==============================================================================
static inline int _sys_llist_destroy(llist_t *list)
{
        return _llist_destroy(list);
}

//==============================================================================
/**
 * @brief  Checks if list is empty
 * @param  list         list object
 * @return If list is empty then true is returned, otherwise false
 */
//==============================================================================
static inline bool _sys_llist_empty(llist_t *list)
{
        return _llist_empty(list);
}

//==============================================================================
/**
 * @brief  Returns a number of elements of the list
 * @param  list         list object
 * @return Number of elements of the list or -1 on error
 */
//==============================================================================
static inline int _sys_llist_size(llist_t *list)
{
        return _llist_size(list);
}

//==============================================================================
/**
 * @brief  Pushes selected data to the front of the list. Creates a new object
 * @param  list         list object
 * @param  size         data size
 * @param  data         data source
 * @return On success allocated memory pointer is returned, otherwise NULL
 */
//==============================================================================
static inline void *_sys_llist_push_emplace_front(llist_t *list, size_t size, const void *data)
{
        return _llist_push_emplace_front(list, size, data);
}

//==============================================================================
/**
 * @brief  Pushes selected object to the front of the list
 * @param  list         list object
 * @param  object       object to push
 * @return On success pointer to the object is returned, otherwise NULL
 */
//==============================================================================
static inline void *_sys_llist_push_front(llist_t *list, void *object)
{
        return _llist_push_front(list, object);
}

//==============================================================================
/**
 * @brief  Delete first element of the list. This destroys an element
 * @param  list         list object
 * @return On success 1 is returned, otherwise 0
 */
//==============================================================================
static inline int _sys_llist_pop_front(llist_t *list)
{
        return _llist_pop_front(list);
}

//==============================================================================
/**
 * @brief  Push selected data to the back of the list. Creates a new object
 * @param  list         list object
 * @param  size         data size
 * @param  data         data source
 * @return On success allocated memory pointer is returned, otherwise NULL
 */
//==============================================================================
static inline void *_sys_llist_push_emplace_back(llist_t *list, size_t size, const void *data)
{
        return _llist_push_emplace_back(list, size, data);
}

//==============================================================================
/**
 * @brief  Push selected object to the back of the list
 * @param  list         list object
 * @return On success pointer to the object is returned, otherwise NULL
 */
//==============================================================================
static inline void *_sys_llist_push_back(llist_t *list, void *object)
{
        return _llist_push_back(list, object);
}

//==============================================================================
/**
 * @brief  Delete the last element of the list. This destroys element
 * @param  list         list object
 * @return On success 1 is returned, otherwise 0
 */
//==============================================================================
static inline int _sys_llist_pop_back(llist_t *list)
{
        return _llist_pop_back(list);
}

//==============================================================================
/**
 * @brief  Allocate and append data at selected position in the list
 * @param  list         list object
 * @param  position     element position
 * @param  size         element's size
 * @param  data         element's data
 * @return On success pointer to the object is returned, otherwise NULL
 */
//==============================================================================
static inline void *_sys_llist_emplace(llist_t *list, int position, size_t size, const void *data)
{
        return _llist_emplace(list, position, size, data);
}

//==============================================================================
/**
 * @brief  Insert an element to the list
 * @param  list         list object
 * @param  position     position to insert
 * @param  object       object to insert
 * @return On success object is returned, otherwise NULL
 */
//==============================================================================
static inline void *_sys_llist_insert(llist_t *list, int position, void *object)
{
        return _llist_insert(list, position, object);
}

//==============================================================================
/**
 * @brief  Erase selected begin of the list. The element is destroyed
 * @param  list         list object
 * @param  position     position to remove
 * @return On success 1 is returned, otherwise 0
 */
//==============================================================================
static inline int _sys_llist_erase(llist_t *list, int position)
{
        return _llist_erase(list, position);
}

//==============================================================================
/**
 * @brief  Return selected begin and remove from the list. The element is not destroyed
 * @param  list         list object
 * @param  position     position to take (unlink)
 * @return On success taken object is returned, otherwise NULL
 */
//==============================================================================
static inline void *_sys_llist_take(llist_t *list, int position)
{
        return _llist_take(list, position);
}

//==============================================================================
/**
 * @brief  Return first begin and remove from the list. The element is not destroyed
 * @param  list         list object
 * @return On success taken object is returned, otherwise NULL
 */
//==============================================================================
static inline void *_sys_llist_take_front(llist_t *list)
{
        return _llist_take_front(list);
}

//==============================================================================
/**
 * @brief  Return last begin and remove from the list. The element is not destroyed
 * @param  list         list object
 * @return On success taken object is returned, otherwise NULL
 */
//==============================================================================
static inline void *_sys_llist_take_back(llist_t *list)
{
        return _llist_take_back(list);
}

//==============================================================================
/**
 * @brief  Clear entire list (objects are destroyed)
 * @param  list         list object
 * @return On success 1 is returned, otherwise 0
 */
//==============================================================================
static inline int _sys_llist_clear(llist_t *list)
{
        return _llist_clear(list);
}

//==============================================================================
/**
 * @brief  Swap 2 elements
 * @param  list         list object
 * @param  j            position of element a
 * @param  k            position of element b
 * @return On success 1 is returned, otherwise 0
 */
//==============================================================================
static inline int _sys_llist_swap(llist_t *list, int j, int k)
{
        return _llist_swap(list, j, k);
}

//==============================================================================
/**
 * @brief  Sort elements of the list
 * @param  list         list object
 * @return None
 */
//==============================================================================
static inline void _sys_llist_sort(llist_t *list)
{
        _llist_sort(list);
}

//==============================================================================
/**
 * @brief  Leave only an unique elements, all not unique are removed (are destroyed)
 * @param  list         list object
 * @return None
 */
//==============================================================================
static inline void _sys_llist_unique(llist_t *list)
{
        _llist_unique(list);
}

//==============================================================================
/**
 * @brief  Reverse entire table
 * @param  list         list object
 * @return None
 */
//==============================================================================
static inline void _sys_llist_reverse(llist_t *list)
{
        _llist_reverse(list);
}

//==============================================================================
/**
 * @brief  Get element from the list at selected position
 * @param  list         list object
 * @param  position     begin position
 * @return Pointer to data, or NULL on error
 */
//==============================================================================
static inline void *_sys_llist_at(llist_t *list, int position)
{
        return _llist_at(list, position);
}

//==============================================================================
/**
 * @brief  Check if list contains selected object
 * @param  list         list object
 * @param  object       object to find
 * @return Number of found objects, or -1 on error
 */
//==============================================================================
static inline int _sys_llist_contains(llist_t *list, const void *object)
{
        return _llist_contains(list, object);
}

//==============================================================================
/**
 * @brief  Find selected object in the list from the beginning
 * @param  list         list object
 * @param  object       object to find
 * @return Object position, or -1 on error
 */
//==============================================================================
static inline int _sys_llist_find_begin(llist_t *list, const void *object)
{
        return _llist_find_begin(list, object);
}

//==============================================================================
/**
 * @brief  Find selected object in the list from the end
 * @param  list         list object
 * @param  object       object to find
 * @return Object position, or -1 on error
 */
//==============================================================================
static inline int _sys_llist_find_end(llist_t *list, const void *object)
{
        return _llist_find_end(list, object);
}

//==============================================================================
/**
 * @brief  Access first element
 * @param  list         list object
 * @return Pointer to data, or NULL on error
 */
//==============================================================================
static inline void *_sys_llist_front(llist_t *list)
{
        return _llist_front(list);
}

//==============================================================================
/**
 * @brief  Access last element
 * @param  list         list object
 * @return Pointer to data, or NULL on error
 */
//==============================================================================
static inline void *_sys_llist_back(llist_t *list)
{
        return _llist_back(list);
}

//==============================================================================
/**
 * @brief  Create an iterator to the list
 * @param  list         list object
 * @return Iterator object
 */
//==============================================================================
static inline llist_iterator_t _sys_llist_iterator(llist_t *list)
{
        return _llist_iterator(list);
}

//==============================================================================
/**
 * @brief  Return first object from list by using iterator
 * @param  iterator     iterator object
 * @return Pointer to data object
 */
//==============================================================================
static inline void *_sys_llist_begin(llist_iterator_t *iterator)
{
        return _llist_begin(iterator);
}

//==============================================================================
/**
 * @brief  Return last object from list by using iterator
 * @param  iterator     iterator object
 * @return Pointer to data object
 */
//==============================================================================
static inline void *_sys_llist_end(llist_iterator_t *iterator)
{
        return _llist_end(iterator);
}

//==============================================================================
/**
 * @brief  Return selected objects from list by using range iterator (forward)
 * @param  iterator     iterator object
 * @param  begin        begin position
 * @param  end          end position
 * @return Pointer to data object
 */
//==============================================================================
static inline void *_sys_llist_range(llist_iterator_t *iterator, int begin, int end)
{
        return _llist_range(iterator, begin, end);
}

//==============================================================================
/**
 * @brief  Return next data object from list by using iterator
 * @param  iterator     iterator object
 * @return Pointer to data object
 */
//==============================================================================
static inline void *_sys_llist_iterator_next(llist_iterator_t *iterator)
{
        return _llist_iterator_next(iterator);
}

//==============================================================================
/**
 * @brief  Return previous data object from list by using iterator
 * @param  iterator     iterator object
 * @return Pointer to data object
 */
//==============================================================================
static inline void *_sys_llist_iterator_prev(llist_iterator_t *iterator)
{
        return _llist_iterator_prev(iterator);
}

//==============================================================================
/**
 * @brief  Erase selected begin of the list. The element is destroyed
 * @param  iterator     position to remove
 * @return On success 1 is returned, otherwise 0
 */
//==============================================================================
static inline int _sys_llist_erase_by_iterator(llist_iterator_t *iterator)
{
        return _llist_erase_by_iterator(iterator);
}

//==============================================================================
/**
 * @brief  Compare functor that compares two pointers (not contents)
 * @param  a    pointer a
 * @param  b    pointer b
 * @return a > b:  1
 *         a = b:  0
 *         a < b: -1
 */
//==============================================================================
static inline int _sys_llist_functor_cmp_pointers(const void *a, const void *b)
{
        return _llist_functor_cmp_pointers(a, b);
}

//==============================================================================
/**
 * @brief  Compare functor that compares two strings (contents)
 * @param  a    string a
 * @param  b    string b
 * @return a > b:  1
 *         a = b:  0
 *         a < b: -1
 */
//==============================================================================
static inline int _sys_llist_functor_cmp_strings(const void *a, const void *b)
{
        return _llist_functor_cmp_strings(a, b);
}

//==============================================================================
/**
 * @brief Function create node for driver file
 *
 * @param[in] path              path when driver-file shall be created
 * @param[in] dev               device number
 *
 * @return One of errno values
 */
//==============================================================================
static inline int _sys_mknod(const char *path, dev_t dev)
{
        return _vfs_mknod(path, dev);
}

//==============================================================================
/**
 * @brief Create directory
 *
 * @param[in] path              path to new directory
 * @param[in] mode              directory mode
 *
 * @return One of errno values
 */
//==============================================================================
static inline int _sys_mkdir(const char *path, mode_t mode)
{
        return _vfs_mkdir(path, mode);
}

//==============================================================================
/**
 * @brief Create pipe
 *
 * @param[in] path              path to pipe
 * @param[in] mode              directory mode
 *
 * @return One of errno values
 */
//==============================================================================
static inline int _sys_mkfifo(const char *path, mode_t mode)
{
        return _vfs_mkfifo(path, mode);
}

//==============================================================================
/**
 * @brief Function open directory
 *
 * @param[in]  path                 directory path
 * @param[out] dir                  pointer to dir pointer
 *
 * @return One of errno values
 */
//==============================================================================
static inline int _sys_opendir(const char *path, DIR **dir)
{
        return _vfs_opendir(path, dir);
}

//==============================================================================
/**
 * @brief Function close opened directory
 *
 * @param[in] *dir                  directory object
 *
 * @return One of errno values
 */
//==============================================================================
static inline int _sys_closedir(DIR *dir)
{
        return _vfs_closedir(dir);
}

//==============================================================================
/**
 * @brief Function read next item of opened directory
 *
 * @param[in]  *dir                  directory object
 * @param[out] **dirent              pointer to direntry pointer
 *
 * @return One of errno values
 */
//==============================================================================
static inline int _sys_readdir(DIR *dir, dirent_t **dirent)
{
        return _vfs_readdir(dir, dirent);
}

//==============================================================================
/**
 * @brief Remove file
 * Removes file or directory. Removes directory if is not a mount point.
 *
 * @param[in] *path                localization of file/directory
 *
 * @return One of errno values
 */
//==============================================================================
static inline int _sys_remove(const char *path)
{
        return _vfs_remove(path);
}

//==============================================================================
/**
 * @brief Rename file name
 * The implementation of rename can move files only if external FS provide
 * functionality. Local VFS cannot do this. Cross FS move is also not possible.
 *
 * @param[in] *old_name                  old file name
 * @param[in] *new_name                  new file name
 *
 * @return One of errno values
 */
//==============================================================================
static inline int _sys_rename(const char *old_name, const char *new_name)
{
        return _vfs_rename(old_name, new_name);
}

//==============================================================================
/**
 * @brief Function change file mode
 *
 * @param[in] *path         file path
 * @param[in]  mode         file mode
 *
 * @return One of errno values
 */
//==============================================================================
static inline int _sys_chmod(const char *path, mode_t mode)
{
        return _vfs_chmod(path, mode);
}

//==============================================================================
/**
 * @brief Function change file owner and group
 *
 * @param[in] *path         file path
 * @param[in]  owner        file owner
 * @param[in]  group        file group
 *
 * @return One of errno values
 */
//==============================================================================
static inline int _sys_chown(const char *path, uid_t owner, gid_t group)
{
        return _vfs_chown(path, owner, group);
}

//==============================================================================
/**
 * @brief Function returns file/dir status
 *
 * @param[in]  *path            file/dir path
 * @param[out] *stat            pointer to structure
 *
 * @return One of errno values
 */
//==============================================================================
static inline int _sys_stat(const char *path, struct stat *stat)
{
        return _vfs_stat(path, stat);
}

//==============================================================================
/**
 * @brief Function returns file system status
 *
 * @param[in]  *path            fs path
 * @param[out] *statfs          pointer to FS status structure
 *
 * @return One of errno values
 */
//==============================================================================
static inline int _sys_statfs(const char *path, struct statfs *statfs)
{
        return _vfs_statfs(path, statfs);
}

//==============================================================================
/**
 * @brief Function open selected file
 *
 * @param[in]  *name             file path
 * @param[in]  *mode             file mode
 * @param[out] **file            pointer to file pointer
 *
 * @return One of errno values
 */
//==============================================================================
static inline int _sys_fopen(const char *name, const char *mode, FILE **file)
{
        return _vfs_fopen(name, mode, file);
}

//==============================================================================
/**
 * @brief Function close opened file
 *
 * @param[in] file              pinter to file
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
static inline int _sys_fclose(FILE *file)
{
        return _vfs_fclose(file, false);
}

//==============================================================================
/**
 * @brief Function write data to file
 *
 * @param[in]  ptr              address to data (src)
 * @param[in]  count            number of items
 * @param[out] wrcnt            number of written elements
 * @param[in]  file             pointer to file object
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
static inline int _sys_fwrite(const void *ptr, size_t size, size_t *wrcnt, FILE *file)
{
        return _vfs_fwrite(ptr, size, wrcnt, file);
}

//==============================================================================
/**
 * @brief Function read data from file
 *
 * @param[out] ptr              address to data (dst)
 * @param[in]  size             item size
 * @param[out] rdcnt            number of read bytes
 * @param[in]  file             pointer to file object
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
static inline int _sys_fread(void *ptr, size_t size, size_t *rdcnt, FILE *file)
{
        return _vfs_fread(ptr, size, rdcnt, file);
}

//==============================================================================
/**
 * @brief Function set seek value
 *
 * @param[in] *file             file object
 * @param[in]  offset           seek value
 * @param[in]  mode             seek mode
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
static inline int _sys_fseek(FILE *file, i64_t offset, int mode)
{
        return _vfs_fseek(file, offset, mode);
}

//==============================================================================
/**
 * @brief Function returns seek value
 *
 * @param[in]  file             file object
 * @param[out] lseek            file seek
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
static inline int _sys_ftell(FILE *file, i64_t *lseek)
{
        return _vfs_ftell(file, lseek);
}

//==============================================================================
/**
 * @brief Function support not standard operations on devices
 *
 * @param[in]     *file         file
 * @param[in]      rq           request
 * @param[in,out]  ...          argument (non or one, depends on request)
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
static inline int _sys_ioctl(FILE *file, int rq, ...)
{
    va_list arg;
    va_start(arg, rq);
    int result = _vfs_vfioctl(file, rq, arg);
    va_end(arg);
    return result;
}

//==============================================================================
/**
 * @brief Function returns file/dir status
 *
 * @param[in]  *file            file object
 * @param[out] *stat            pointer to stat structure
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
static inline int _sys_fstat(FILE *file, struct stat *stat)
{
        return _vfs_fstat(file, stat);
}

//==============================================================================
/**
 * @brief Function flush file data
 *
 * @param[in] *file     file to flush
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
static inline int _sys_fflush(FILE *file)
{
        return _vfs_fflush(file);
}

//==============================================================================
/**
 * @brief Function check end of file
 *
 * @param[in]  *file    file
 * @param[out] *eof     EOF indicator (EOF or 0)
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
static inline int _sys_feof(FILE *file, int *eof)
{
        return _vfs_feof(file, eof);
}

//==============================================================================
/**
 * @brief Function clear file's error
 *
 * @param[in] *file     file
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
static inline int _sys_clearerr(FILE *file)
{
        return _vfs_clearerr(file);
}

//==============================================================================
/**
 * @brief Function check that file has no errors
 *
 * @param[in]  file     file
 * @param[out] error    error indicator (1 for error, 0 no error)
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
static inline int _sys_ferror(FILE *file, int *error)
{
        return _vfs_ferror(file, error);
}

//==============================================================================
/**
 * @brief Function rewind file
 *
 * @param[in] *file     file
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
static inline int _sys_rewind(FILE *file)
{
        return _vfs_fseek(file, 0, VFS_SEEK_SET);
}

//==============================================================================
/**
 * @brief Synchronize internal buffers of mounted file systems
 *
 * @param None
 *
 * @return None
 */
//==============================================================================
static inline void _sys_sync()
{
        _vfs_sync();
}

//==============================================================================
/**
 * @brief Function send kernel message on terminal
 *
 * @param *format             formated text
 * @param ...                 format arguments
 */
//==============================================================================
#define _sys_printk(...) _printk(__VA_ARGS__)

//==============================================================================
/**
 * @brief Function convert arguments to stream
 *
 * @param[in] *buf           buffer for stream
 * @param[in]  size          buffer size
 * @param[in] *format        message format
 * @param[in]  args          argument list
 *
 * @return number of printed characters
 */
//==============================================================================
static inline int _sys_vsnprintf(char *buf, size_t size, const char *format, va_list args)
{
        return _vsnprintf(buf, size, format, args);
}

//==============================================================================
/**
 * @brief Function send to buffer formated output string
 *
 * @param *bfr                output buffer
 * @param  size               buffer size
 * @param *format             formated text
 * @param  ...                format arguments
 *
 * @retval number of written characters
 */
//==============================================================================
static inline int _sys_snprintf(char *bfr, size_t size, const char *format, ...)
{
        va_list arg;
        va_start(arg, format);
        int r = _vsnprintf(bfr, size, format, arg);
        va_end(arg);
        return r;
}

//==============================================================================
/**
 * @brief Function write to file formatted string
 *
 * @param file                file
 * @param format              formated text
 * @param args                arguments
 *
 * @retval number of written characters
 */
//==============================================================================
static inline int _sys_vfprintf(FILE *file, const char *format, va_list args)
{
        return _vfprintf(file, format, args);
}

//==============================================================================
/**
 * @brief Function write to file formatted string
 *
 * @param *file               file
 * @param *format             formated text
 * @param ...                 format arguments
 *
 * @retval number of written characters
 */
//==============================================================================
static inline int _sys_fprintf(FILE *file, const char *format, ...)
{
        va_list arg;
        va_start(arg, format);
        int r = _vfprintf(file, format, arg);
        va_end(arg);
        return r;
}

//==============================================================================
/**
 * @brief Function scan arguments defined by format (multiple argument version)
 *
 * @param[in]  *str           data buffer
 * @param[in]  *format        scan format
 * @param[out]  ...           output
 *
 * @return number of scanned elements
 */
//==============================================================================
static inline int _sys_sscanf(const char *str, const char *format, ...)
{
        va_list arg;
        va_start(arg, format);
        int n = _vsscanf(str, format, arg);
        va_end(arg);
        return n;
}

//==============================================================================
/**
 * @brief Function scan arguments defined by format (argument list version)
 *
 * @param[in]  *str           data buffer
 * @param[in]  *format        scan format
 * @param[out]  args          output
 *
 * @return number of scanned elements
 */
//==============================================================================
static inline int _sys_vsscanf(const char *str, const char *format, va_list args)
{
        return _vsscanf(str, format, args);
}

//==============================================================================
/**
 * @brief Function get time reference
 *
 * @param None
 *
 * @return Synchronized timer object
 */
//==============================================================================
static inline uint _sys_time_get_reference()
{
        return _kernel_get_time_ms();
}

//==============================================================================
/**
 * @brief Check if time expired
 *
 * @param time_ref      time reference
 * @param time          time to check
 *
 * @return If time expired then true is returned, otherwise false.
 */
//==============================================================================
static inline bool _sys_time_is_expired(uint time_ref, uint time)
{
        return (_kernel_get_time_ms() - time_ref >= time);
}

//==============================================================================
/**
 * @brief Set time reference as expired
 *
 * @param None
 *
 * @return Timer object with expired value.
 */
//==============================================================================
static inline uint _sys_time_set_expired()
{
        return 0;
}

//==============================================================================
/**
 * @brief Calculate difference between time 1 and time 2
 *
 * @param time1        time reference 1
 * @param time2        time reference 2
 *
 * @return Returns difference between timer1 and timer2.
 */
//==============================================================================
static inline int _sys_time_diff(uint time1, uint time2)
{
        return time1 - time2;
}

//==============================================================================
/**
 * @brief Function create binary semaphore
 *
 * @param[in]  cnt_max          max count value (1 for binary)
 * @param[in]  cnt_init         initial value (0 or 1 for binary)
 * @param[out] sem              created semaphore handle
 *
 * @return One of errno values.
 */
//==============================================================================
static inline int _sys_semaphore_create(const uint cnt_max, const uint cnt_init, sem_t **sem)
{
        return _semaphore_create(cnt_max, cnt_init, sem);
}

//==============================================================================
/**
 * @brief Function delete semaphore
 *
 * @param[in] *sem      semaphore object
 */
//==============================================================================
static inline int _sys_semaphore_destroy(sem_t *sem)
{
        return _semaphore_destroy(sem);
}

//==============================================================================
/**
 * @brief Function wait for semaphore
 *
 * @param[in] *sem              semaphore object
 * @param[in]  blocktime_ms     semaphore polling time
 *
 * @return One of errno values.
 */
//==============================================================================
static inline int _sys_semaphore_wait(sem_t *sem, const uint timeout)
{
        return _semaphore_wait(sem, timeout);
}

//==============================================================================
/**
 * @brief Function signal semaphore
 *
 * @param[in] *sem      semaphore object
 *
 * @return One of errno values.
 */
//==============================================================================
static inline int _sys_semaphore_signal(sem_t *sem)
{
        return _semaphore_signal(sem);
}

//==============================================================================
/**
 * @brief Function wait for semaphore from ISR
 *
 * @param[in]  *sem              semaphore object
 * @param[out] *task_woken       true if higher priority task woken, otherwise false (can be NULL)
 *
 * @return One of errno values.
 */
//==============================================================================
static inline int _sys_semaphore_wait_from_ISR(sem_t *sem, bool *task_woken)
{
        return _semaphore_wait_from_ISR(sem, task_woken);
}

//==============================================================================
/**
 * @brief Function signal semaphore from ISR
 *
 * @param[in]  *sem              semaphore object
 * @param[out] *task_woken       true if higher priority task woken, otherwise false (can be NULL)
 *
 * @return One of errno values.
 */
//==============================================================================
static inline bool _sys_semaphore_signal_from_ISR(sem_t *sem, bool *task_woken)
{
        return _semaphore_signal_from_ISR(sem, task_woken);
}

//==============================================================================
/**
 * @brief Function create new mutex
 *
 * @param[in]  type     mutex type
 * @param[out] mtx      created mutex handle
 *
 * @return One of errno values.
 */
//==============================================================================
static inline int _sys_mutex_create(enum mutex_type type, mutex_t **mtx)
{
        return _mutex_create(type, mtx);
}

//==============================================================================
/**
 * @brief Function destroy mutex
 *
 * @param[in] *mutex    mutex object
 *
 * @return One of errno values.
 */
//==============================================================================
static inline int _sys_mutex_destroy(mutex_t *mutex)
{
        return _mutex_destroy(mutex);
}

//==============================================================================
/**
 * @brief Function lock mutex
 *
 * @param[in] mutex             mutex object
 * @param[in] blocktime_ms      polling time
 *
 * @return One of errno values.
 */
//==============================================================================
static inline int _sys_mutex_lock(mutex_t *mutex, const uint timeout)
{
        return _mutex_lock(mutex, timeout);
}

//==============================================================================
/**
 * @brief Function lock mutex
 *
 * @param[in] mutex             mutex object
 *
 * @return One of errno values.
 */
//==============================================================================
static inline int _sys_mutex_trylock(mutex_t *mutex)
{
        return _mutex_lock(mutex, 0);
}

//==============================================================================
/**
 * @brief Function unlock mutex
 *
 * @param[in] *mutex            mutex object
 *
 * @return One of errno values.
 */
//==============================================================================
static inline int _sys_mutex_unlock(mutex_t *mutex)
{
        return _mutex_unlock(mutex);
}

//==============================================================================
/**
 * @brief Function create new queue
 *
 * @param[in]  length           queue length
 * @param[in]  item_size        queue item size
 * @param[out] queue            created queue
 *
 * @return One of errno values.
 */
//==============================================================================
static inline int _sys_queue_create(const uint length, const uint item_size, queue_t **queue)
{
        return _queue_create(length, item_size, queue);
}

//==============================================================================
/**
 * @brief Function delete queue
 *
 * @param[in] *queue            queue object
 *
 * @return One of errno values.
 */
//==============================================================================
static inline int _sys_queue_destroy(queue_t *queue)
{
        return _queue_destroy(queue);
}

//==============================================================================
/**
 * @brief Function reset queue
 *
 * @param[in] *queue            queue object
 *
 * @return One of errno values.
 */
//==============================================================================
static inline int _sys_queue_reset(queue_t *queue)
{
        return _queue_reset(queue);
}

//==============================================================================
/**
 * @brief Function send queue
 *
 * @param[in] *queue            queue object
 * @param[in] *item             item
 * @param[in]  waittime_ms      wait time
 *
 * @return One of errno values.
 */
//==============================================================================
static inline int _sys_queue_send(queue_t *queue, const void *item, const uint waittime_ms)
{
        return _queue_send(queue, item, waittime_ms);
}

//==============================================================================
/**
 * @brief Function send queue
 *
 * @param[in]  *queue            queue object
 * @param[in]  *item             item
 * @param[out] *task_woken       1 if higher priority task woken, otherwise 0 (can be NULL)
 *
 * @return One of errno values.
 */
//==============================================================================
static inline int _sys_queue_send_from_ISR(queue_t *queue, const void *item, bool *task_woken)
{
        return _queue_send_from_ISR(queue, item, task_woken);
}

//==============================================================================
/**
 * @brief Function send queue
 *
 * @param[in]  *queue            queue object
 * @param[out] *item             item
 * @param[in]   waittime_ms      wait time
 *
 * @return One of errno values.
 */
//==============================================================================
static inline int _sys_queue_receive(queue_t *queue, void *item, const uint waittime_ms)
{
        return _queue_receive(queue, item, waittime_ms);
}

//==============================================================================
/**
 * @brief Function receive queue from ISR
 *
 * @param[in]  queue            queue object
 * @param[out] item             item
 * @param[out] task_woken       true if higher priority task woke, otherwise false (can be NULL)
 *
 * @return One of errno values.
 */
//==============================================================================
static inline int _sys_queue_receive_from_ISR(queue_t *queue, void *item, bool *task_woken)
{
        return _queue_receive_from_ISR(queue, item, task_woken);
}

//==============================================================================
/**
 * @brief Function peek queue
 *
 * @param[in]  *queue            queue object
 * @param[out] *item             item
 * @param[in]   waittime_ms      wait time
 *
 * @return One of errno values.
 */
//==============================================================================
static inline int _sys_queue_receive_peek(queue_t *queue, void *item, const uint waittime_ms)
{
        return _queue_receive_peek(queue, item, waittime_ms);
}

//==============================================================================
/**
 * @brief Function gets number of items in queue
 *
 * @param[in]  queue            queue object
 * @param[out] items            number of items in queue
 *
 * @return One of errno values.
 */
//==============================================================================
static inline int _sys_queue_get_number_of_items(queue_t *queue, size_t *items)
{
        return _queue_get_number_of_items(queue, items);
}

//==============================================================================
/**
 * @brief Function gets number of items in queue from ISR
 *
 * @param[in]  queue            queue object
 * @param[out] items            number of items in queue
 *
 * @return One of errno values.
 */
//==============================================================================
static inline int _sys_queue_get_number_of_items_from_ISR(queue_t *queue, size_t *items)
{
        return _queue_get_number_of_items_from_ISR(queue, items);
}

//==============================================================================
/**
 * @brief Function gets number of free items in queue
 *
 * @param[in]  queue            queue object
 * @param[out] items            number of items in queue
 *
 * @return One of errno values.
 */
//==============================================================================
static inline int _sys_queue_get_space_available(queue_t *queue, size_t *items)
{
        return _queue_get_space_available(queue, items);
}

//==============================================================================
/**
 * @brief Function return OS time in milliseconds
 *
 * @return a OS time in milliseconds
 */
//==============================================================================
static inline uint _sys_get_time_ms()
{
        return _kernel_get_time_ms();
}

//==============================================================================
/**
 * @brief Function return tick counter
 *
 * @return a tick counter value
 */
//==============================================================================
static inline uint _sys_get_tick_counter()
{
        return _kernel_get_tick_counter();
}

//==============================================================================
/**
 * @brief Function return a number of task
 *
 * @return a number of tasks
 */
//==============================================================================
static inline int _sys_get_number_of_tasks()
{
        return _kernel_get_number_of_tasks();
}

//==============================================================================
/**
 * @brief Function create new task and if enabled add to monitor list
 *
 * Function by default allocate memory for task data (localized in task tag)
 * which is used to cpu load calculation and standard IO and etc.
 *
 * @param[in ] func             task code
 * @param[in ] name             task name
 * @param[in ] stack_depth      stack deep
 * @param[in ] argv             argument pointer (can be NULL)
 * @param[in ] tag              user's tag (can be NULL)
 * @param[out] task             task handle (can be NULL)
 *
 * @return On of errno value.
 */
//==============================================================================
static inline int _sys_thread_create(thread_func_t func, const thread_attr_t *attr, void *arg, thread_t *thread)
{
        int result = EINVAL;

        if (thread) {
                _process_t *proc = _process_get_container_by_task(_THIS_TASK, NULL);
                result = _process_thread_create(proc, func, attr, arg, &thread->tid, &thread->task);
        }

        return result;

} // TODO
//static inline int _sys_task_create(void (*func)(void*), const char *name, const uint stack_depth, void *argv, void *tag, task_t **task)
//{
//        return _task_create(func, name, stack_depth, argv, tag, task);
//}

//==============================================================================
/**
 * @brief Function delete task
 * Function remove task from monitoring list, and next delete the task from OS
 * list. Function resume the parent task before delete.
 *
 * @param *taskHdl       task handle
 */
//==============================================================================
static inline int _sys_thread_destroy(thread_t *thread)
{
        int result = EINVAL;

        if (thread) {
                _process_t *proc = _process_get_container_by_task(_THIS_TASK, NULL);
                _thread_t  *thr  = _process_thread_get_container(proc, thread->tid);
                result           = _process_release_resource(proc,
                                                             static_cast(res_header_t*, thr),
                                                             RES_TYPE_THREAD);
                if (result == ESUCC) {
                        thread->task = NULL;
                        thread->tid  = 0;
                }
        }

        return result;
}
//TODO
//static inline void _sys_task_destroy(task_t *taskhdl)
//{
//        _task_destroy(taskhdl);
//}

//==============================================================================
/**
 * @brief  ?
 * @param  ?
 * @return ?
 */
//==============================================================================
static inline bool _sys_thread_is_valid(thread_t *thread)
{
        return thread && thread->tid && thread->task;
}

//==============================================================================
/**
 * @brief Function wait for task exit
 */
//==============================================================================
//static inline void _sys_task_exit() TODO
//{
//        _task_exit();
//}

//==============================================================================
/**
 * @brief Function suspend selected task
 *
 * @param[in] *taskhdl          task handle
 */
//==============================================================================
static inline void _sys_thread_suspend(thread_t *thread)
{
        if (thread) {
                _task_suspend(thread->task);
        }
}
//static inline void _sys_task_suspend(task_t *taskhdl) TODO
//{
//        return _task_suspend(taskhdl);
//}

//==============================================================================
/**
 * @brief Function suspend current task
 */
//==============================================================================
static inline void _sys_thread_suspend_now()
{
        _task_suspend(_THIS_TASK);
}

//==============================================================================
/**
 * @brief Function resume selected task
 *
 * @param[in] *taskhdl          task handle
 */
//==============================================================================
static inline void _sys_thread_resume(thread_t *thread)
{
        if (thread) {
                _task_resume(thread->task);
        }
}

//==============================================================================
/**
 * @brief Function resume selected task from ISR
 *
 * @param[in] *taskhdl          task handle
 *
 * @retval true                 if yield required
 * @retval false                if yield not required
 */
//==============================================================================
static inline bool _sys_thread_resume_from_ISR(thread_t *thread)
{
        if (thread) {
                return _task_resume_from_ISR(thread->task);
        } else {
                return false;
        }
}

//==============================================================================
/**
 * @brief Function yield task
 */
//==============================================================================
static inline void _sys_thread_yield()
{
        _task_yield();
}

//==============================================================================
/**
 * @brief  ?
 * @param  ?
 * @return ?
 */
//==============================================================================
static inline int _sys_thread_self(thread_t *thread)
{
        int result = EINVAL;

        if (thread) {
                _thread_t *thr = _task_get_tag(_THIS_TASK);
                if (thr && reinterpret_cast(res_header_t*, thr)->type == RES_TYPE_THREAD) {
                        thread->task = _process_thread_get_task(thr);
                        thread->tid  = _process_thread_get_tid(thr);
                        result       = ESUCC;
                } else {
                        result = ESRCH;
                }
        }

        return result;
}

//==============================================================================
/**
 * @brief Function yield task from ISR
 */
//==============================================================================
static inline void _sys_thread_yield_from_ISR()
{
        _task_yield_from_ISR();
}

//==============================================================================
/**
 * @brief Function set priority of current task
 *
 * @param[in]  priority         priority
 */
//==============================================================================
static inline void _sys_thread_set_priority(const int priority)
{
        _task_set_priority(_THIS_TASK, priority);
}

//==============================================================================
/**
 * @brief Function return priority of current task
 *
 * @return current task priority
 */
//==============================================================================
static inline int _sys_thread_get_priority()
{
        return _task_get_priority(_THIS_TASK);
}

//==============================================================================
/**
 * @brief Function return a free stack level of current task
 *
 * @return free stack level
 */
//==============================================================================
static inline int _sys_thread_get_free_stack()
{
        return _task_get_free_stack(_THIS_TASK);
}

//==============================================================================
/**
 * @brief Function enter to critical section
 */
//==============================================================================
static inline void _sys_critical_section_begin()
{
        _critical_section_begin();
}

//==============================================================================
/**
 * @brief Function exit from critical section
 */
//==============================================================================
static inline void _sys_critical_section_end()
{
        _critical_section_end();
}

//==============================================================================
/**
 * @brief Function disable interrupts
 */
//==============================================================================
static inline void _sys_ISR_disable()
{
        _ISR_disable();
}

//==============================================================================
/**
 * @brief Function enable interrupts
 */
//==============================================================================
static inline void _sys_ISR_enable()
{
        _sys_ISR_enable();
}

//==============================================================================
/**
 * @brief Function put to sleep task in milliseconds
 *
 * @param[in] milliseconds
 */
//==============================================================================
static inline void _sys_sleep_ms(const uint milliseconds)
{
        _sleep_ms(milliseconds);
}

//==============================================================================
/**
 * @brief Function put to sleep task in seconds
 *
 * @param[in] seconds
 */
//==============================================================================
static inline void _sys_sleep(const uint seconds)
{
        _sleep(seconds);
}

//==============================================================================
/**
 * @brief Function sleep task in regular periods (reference argument)
 *
 * @param milliseconds          milliseconds
 * @param ref_time_ticks        reference time in OS ticks
 */
//==============================================================================
static inline void _sys_sleep_until_ms(const uint milliseconds, int *ref_time_ticks)
{
        _sleep_until_ms(milliseconds, ref_time_ticks);
}

//==============================================================================
/**
 * @brief Function sleep task in regular periods (reference argument)
 *
 * @param seconds       seconds
 * @param ref_time_ticks        reference time in OS ticks
 */
//==============================================================================
static inline void _sys_sleep_until(const uint seconds, int *ref_time_ticks)
{
        _sleep_until(seconds, ref_time_ticks);
}

//==============================================================================
/**
 * @brief Function update all system clock after CPU frequency change
 *
 * Function shall update all devices which base on main clock oscillator.
 * Function is called after clock/frequency change from clock management driver.
 */
//==============================================================================
static inline void _sys_update_system_clocks()
{
        _cpuctl_update_system_clocks();
}

//==============================================================================
/**
 * @brief  Convert tm structure to time_t
 *
 * This function performs the reverse translation that localtime does.
 * The values of the members tm_wday and tm_yday of timeptr are ignored, and
 * the values of the other members are interpreted even if out of their valid
 * ranges (see struct tm). For example, tm_mday may contain values above 31,
 * which are interpreted accordingly as the days that follow the last day of
 * the selected month.
 * A call to this function automatically adjusts the values of the members of
 * timeptr if they are off-range or -in the case of tm_wday and tm_yday- if they
 * have values that do not match the date described by the other members.
 *
 * @param  timeptr      Pointer to a tm structure that contains a calendar time
 *                      broken down into its components (see struct tm)
 *
 * @return A time_t value corresponding to the calendar time passed as argument.
 *         If the calendar time cannot be represented, a value of -1 is returned.
 */
//==============================================================================
static inline time_t _sys_mktime(struct tm *timeptr)
{
        return _mktime(timeptr);
}

//==============================================================================
/**
 * @brief  Get current time
 *
 * The function returns this value, and if the argument is not a null pointer,
 * it also sets this value to the object pointed by timer.
 * The value returned generally represents the number of seconds since 00:00
 * hours, Jan 1, 1970 UTC (i.e., the current unix timestamp). Although libraries
 * may use a different representation of time: Portable programs should not use
 * the value returned by this function directly, but always rely on calls to
 * other elements of the standard library to translate them to portable types
 * (such as localtime, gmtime or difftime).
 *
 * @param  timer        Pointer to an object of type time_t, where the time
 *                      value is stored.
 *                      Alternatively, this parameter can be a null pointer,
 *                      in which case the parameter is not used (the function
 *                      still returns a value of type time_t with the result).
 *
 * @return One of errno value.
 */
//==============================================================================
static inline int _sys_gettime(time_t *timer)
{
        return _gettime(timer);
}

//==============================================================================
/**
 * @brief  Set system's time
 *
 * stime() sets the system's idea of the time and date. The time, pointed to by
 * timer, is measured in seconds since the Epoch, 1970-01-01 00:00:00 +0000 (UTC).
 *
 * @param  timer        pointer to an object of type time_t, where the time
 *                      value is stored.
 *
 * @return One of errno value.
 */
//==============================================================================
static inline int _sys_settime(time_t *timer)
{
        return _settime(timer);
}

//==============================================================================
/**
 * @brief  Convert time_t to tm as UTC time
 *
 * Uses the value pointed by timer to fill a tm structure with the values that
 * represent the corresponding time, expressed as a UTC time (i.e., the time
 * at the GMT timezone).
 *
 * @param[in]  timer    Pointer to an object of type time_t that contains a time value.
 *                      time_t is an alias of a fundamental arithmetic type
 *                      capable of representing times as returned by function time.
 *
 * @param[out] tm       Pointer to an object of type struct tm that will contains
 *                      converted timer value to time structure.
 *
 * @return A pointer to a tm structure with its members filled with the values
 *         that correspond to the UTC time representation of timer.
 */
//==============================================================================
static inline struct tm *_sys_gmtime_r(const time_t *timer, struct tm *tm)
{
        return _gmtime_r(timer, tm);
}

//==============================================================================
/**
 * @brief  Convert time_t to tm as local time
 *
 * Uses the value pointed by timer to fill a tm structure with the values that
 * represent the corresponding time, expressed for the local timezone.
 *
 * @param[in]  timer    Pointer to an object of type time_t that contains a time value.
 *                      time_t is an alias of a fundamental arithmetic type
 *                      capable of representing times as returned by function time.
 *
 * @param[out] tm       Pointer to an object of type struct tm that will contains
 *                      converted timer value to time structure.
 *
 * @return A pointer to a tm structure with its members filled with the values
 *         that correspond to the local time representation of timer.
 */
//==============================================================================
static inline struct tm *_sys_localtime_r(const time_t *timer, struct tm *tm)
{
        return _localtime_r(timer, tm);
}

#ifdef __cplusplus
}
#endif

#endif /* _SYSFUNC_H_ */
/*==============================================================================
  End of file
==============================================================================*/
