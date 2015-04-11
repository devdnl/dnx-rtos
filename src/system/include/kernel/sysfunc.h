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
#include <sys/types.h>
#include <dnx/misc.h>
#include <errno.h>
#include <stdbool.h>
#include "lib/conv.h"
#include "lib/llist.h"
#include "kernel/printk.h"
#include "fs/vfs.h"
#include "drivers/drvctrl.h"
#include "lib/printx.h"
#include "lib/scanx.h"
#include "kernel/env.h"
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
static inline int _sys_llist_delete(llist_t *list)
{
        return _llist_delete(list);
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
 * @return zero on success. On error, -1 is returned
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
 * @return 0 on success. On error, -1 is returned
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
 * @return 0 on success. On error, -1 is returned
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
 * @param[in] *path                 directory path
 *
 * @return directory object
 */
//==============================================================================
static inline DIR *_sys_opendir(const char *path)
{
//        return _vfs_opendir(path); // TODO
}

//==============================================================================
/**
 * @brief Function close opened directory
 *
 * @param[in] *dir                  directory object
 *
 * @return 0 on success. On error, -1 is returned
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
 * @param[in] *dir                  directory object
 *
 * @return element attributes
 */
//==============================================================================
static inline dirent_t *_sys_readdir(DIR *dir)
{
//        return _vfs_readdir(dir); // TODO
}

//==============================================================================
/**
 * @brief Remove file
 * Removes file or directory. Removes directory if is not a mount point.
 *
 * @param[in] *path                localization of file/directory
 *
 * @return 0 on success. On error, -1 is returned
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
 * @return 0 on success. On error, -1 is returned
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
 * @return 0 on success. On error, -1 is returned
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
 * @return 0 on success. On error, -1 is returned
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
 * @return 0 on success. On error, -1 is returned
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
 * @return 0 on success. On error, -1 is returned
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
 * @param[in] *name             file path
 * @param[in] *mode             file mode
 *
 * @retval NULL if file can't be created
 */
//==============================================================================
static inline FILE *_sys_fopen(const char *name, const char *mode)
{
//        return _vfs_fopen(name, mode); // TODO
}

//==============================================================================
/**
 * @brief Function close old stream and open new
 *
 * @param[in] *name             file path
 * @param[in] *mode             file mode
 * @param[in] *file             old stream
 *
 * @retval NULL if file can't be created
 */
//==============================================================================
static inline FILE *_sys_freopen(const char *name, const char *mode, FILE *file)
{
//        return _vfs_freopen(name, mode, file); // TODO
}

//==============================================================================
/**
 * @brief Function close opened file
 *
 * @param[in] *file             pinter to file
 *
 * @return 0 on success. On error, -1 is returned
 */
//==============================================================================
static inline int _sys_fclose(FILE *file)
{
//        return _vfs_fclose(file); // TODO
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
//        return _vfs_fread(ptr, size, rdcnt, file);
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
 * @param[in] *file             file object
 *
 * @return -1 if error, otherwise correct value
 */
//==============================================================================
static inline i64_t _sys_ftell(FILE *file)
{
//        return _vfs_ftell(file); // TODO
}

//==============================================================================
/**
 * @brief Function support not standard operations on devices
 *
 * @param[in]     *file         file
 * @param[in]      rq           request
 * @param[in,out] ...           additional function arguments
 *
 * @return 0 on success. On error, different from 0 is returned
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
 * @return 0 on success. On error, -1 is returned
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
 * @return 0 on success. On error, -1 is returned
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
 * @param[in] *file     file
 *
 * @return 0 if there is not a file end, otherwise greater than 0
 */
//==============================================================================
static inline int _sys_feof(FILE *file)
{
//        return _vfs_feof(file); // TODO
}

//==============================================================================
/**
 * @brief Function clear file's error
 *
 * @param[in] *file     file
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
 * @param[in] *file     file
 *
 * @return nonzero value if the file stream has errors occurred, 0 otherwise
 */
//==============================================================================
static inline int _sys_ferror(FILE *file)
{
//        return _vfs_ferror(file); // TODO
}

//==============================================================================
/**
 * @brief Function rewind file
 *
 * @param[in] *file     file
 *
 * @return 0 on success. On error, -1 is returned
 */
//==============================================================================
static inline int _sys_rewind(FILE *file)
{
        return _vfs_rewind(file);
}

//==============================================================================
/**
 * @brief Synchronize internal buffers of mounted file systems
 *
 * @param None
 *
 * @errors None
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
 * @brief Enable printk functionality
 *
 * @param filename      path to file used to write kernel log
 */
//==============================================================================
static inline void _sys_printk_enable(char *filename)
{
        _printk_enable(filename);
}

//==============================================================================
/**
 * @brief Disable printk functionality
 */
//==============================================================================
static inline void _sys_printk_disable()
{
        _printk_disable();
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
#define _sys_snprintf(char__bfr, size_t__size, ...) _snprintf(char__bfr, size_t__size, __VA_ARGS__)

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
#define _sys_fprintf(FILE__file, ...) _fprintf(FILE__file, __VA_ARGS__)

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
 * @brief Function returns error string
 *
 * @param errnum        error number
 *
 * @return error number string
 */
//==============================================================================
static inline const char *_sys_strerror(int errnum)
{
        return _strerror(errnum);
}

//==============================================================================
/**
 * @brief Function put character into file
 *
 * @param  c                   character
 * @param *stream              file
 *
 * @retval c if OK otherwise EOF
 */
//==============================================================================
static inline int _sys_fputc(int c, FILE *stream)
{
        return _fputc(c, stream);
}

//==============================================================================
/**
 * @brief Function puts string to selected file
 *
 * @param[in] *s        string
 * @param[in] *file     file
 *
 * @return number of characters written to the stream
 */
//==============================================================================
static inline int _sys_fputs(const char *s, FILE *file)
{
        return _f_puts(s, file, false);
}

//==============================================================================
/**
 * @brief Function get character from file
 *
 * @param *stream            source file
 *
 * @retval character
 */
//==============================================================================
static inline int _sys_getc(FILE *stream)
{
        return _getc(stream);
}

//==============================================================================
/**
 * @brief Function gets number of bytes from file
 *
 * @param[out] *str          buffer with string
 * @param[in]   size         buffer size
 * @param[in]  *stream       source stream
 *
 * @retval NULL if error, otherwise pointer to str
 */
//==============================================================================
static inline char *_sys_fgets(char *str, int size, FILE *stream)
{
        return _fgets(str, size, stream);
}

//==============================================================================
/**
 * @brief Function scan stream
 *
 * @param[in]  *stream        file
 * @param[in]  *format        message format
 * @param[out]  ...           output
 *
 * @return number of scanned elements
 */
//==============================================================================
#define _sys_fscanf(FILE__stream, ...) _fscanf(FILE__stream, __VA_ARGS__)

//==============================================================================
/**
 * @brief Function scan stream
 *
 * @param[in]  *stream        file
 * @param[in]  *format        message format
 * @param[out]  args          output arguments
 *
 * @return number of scanned elements
 */
//==============================================================================
static inline int _sys_vfscanf(FILE *stream, const char *format, va_list args)
{
        return _vfscanf(stream, format, args);
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
#define _sys_sscanf(const_char__str, ...) _sscanf(const_char__str, __VA_ARGS__)

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
static inline int _sys_task_create(void (*func)(void*), const char *name, const uint stack_depth, void *argv, void *tag, task_t **task)
{
        return _task_create(func, name, stack_depth, argv, tag, task);
}

//==============================================================================
/**
 * @brief Function delete task
 * Function remove task from monitoring list, and next delete the task from OS
 * list. Function resume the parent task before delete.
 *
 * @param *taskHdl       task handle
 */
//==============================================================================
static inline int _sys_task_destroy(task_t *taskhdl)
{
        return _task_destroy(taskhdl);
}

//==============================================================================
/**
 * @brief Function wait for task exit
 */
//==============================================================================
static inline void _sys_task_exit()
{
        _task_exit();
}

//==============================================================================
/**
 * @brief Function suspend selected task
 *
 * @param[in] *taskhdl          task handle
 */
//==============================================================================
static inline void _sys_task_suspend(task_t *taskhdl)
{
        return _task_suspend(taskhdl);
}

//==============================================================================
/**
 * @brief Function resume selected task
 *
 * @param[in] *taskhdl          task handle
 */
//==============================================================================
static inline void _sys_task_resume(task_t *taskhdl)
{
        return _task_resume(taskhdl);
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
static inline bool _sys_task_resume_from_ISR(task_t *taskhdl)
{
        return _task_resume_from_ISR(taskhdl);
}

//==============================================================================
/**
 * @brief Function create binary semaphore
 *
 * @param cnt_max       max count value (1 for binary)
 * @param cnt_init      initial value (0 or 1 for binary)
 *
 * @return binary semaphore object
 */
//==============================================================================
static inline sem_t *_sys_semaphore_new(const uint cnt_max, const uint cnt_init)
{
        return _semaphore_new(cnt_max, cnt_init);
}

//==============================================================================
/**
 * @brief Function delete semaphore
 *
 * @param[in] *sem      semaphore object
 */
//==============================================================================
static inline void _sys_semaphore_delete(sem_t *sem)
{
        return _semaphore_delete(sem);
}

//==============================================================================
/**
 * @brief Function take semaphore
 *
 * @param[in] *sem              semaphore object
 * @param[in]  timeout          semaphore polling time
 *
 * @retval true         semaphore taken
 * @retval false        semaphore not taken
 */
//==============================================================================
static inline bool _sys_semaphore_wait(sem_t *sem, const uint timeout)
{
        return _semaphore_wait(sem, timeout);
}

//==============================================================================
/**
 * @brief Function give semaphore
 *
 * @param[in] *sem      semaphore object
 *
 * @retval true         semaphore given
 * @retval false        semaphore not given
 */
//==============================================================================
static inline bool _sys_semaphore_signal(sem_t *sem)
{
        return _semaphore_signal(sem);
}

//==============================================================================
/**
 * @brief Function take semaphore from ISR
 *
 * @param[in]  *sem              semaphore object
 * @param[out] *task_woken       true if higher priority task woken, otherwise false (can be NULL)
 *
 * @retval true         semaphore taken
 * @retval false        semaphore not taken
 */
//==============================================================================
static inline bool _sys_semaphore_wait_from_ISR(sem_t *sem, bool *task_woken)
{
        return _semaphore_wait_from_ISR(sem, task_woken);
}

//==============================================================================
/**
 * @brief Function give semaphore from ISR
 *
 * @param[in]  *sem              semaphore object
 * @param[out] *task_woken       true if higher priority task woken, otherwise false (can be NULL)
 *
 * @retval true         semaphore taken
 * @retval false        semaphore not taken
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
 * @param type          mutex type
 *
 * @return pointer to mutex object, otherwise NULL if error
 */
//==============================================================================
static inline mutex_t *_sys_mutex_new(enum mutex_type type)
{
        return _mutex_new(type);
}

//==============================================================================
/**
 * @brief Function delete mutex
 *
 * @param[in] *mutex    mutex object
 */
//==============================================================================
static inline void _sys_mutex_delete(mutex_t *mutex)
{
        return _mutex_delete(mutex);
}

//==============================================================================
/**
 * @brief Function lock mutex
 *
 * @param[in] mutex             mutex object
 * @param[in] timeout           polling time
 *
 * @retval true                 mutex locked
 * @retval false                mutex not locked
 */
//==============================================================================
static inline bool _sys_mutex_lock(mutex_t *mutex, const uint timeout)
{
        return _mutex_lock(mutex, timeout);
}

//==============================================================================
/**
 * @brief Function unlock mutex
 *
 * @param[in] *mutex            mutex object
 *
 * @retval true         mutex unlocked
 * @retval false        mutex still locked
 */
//==============================================================================
static inline bool _sys_mutex_unlock(mutex_t *mutex)
{
        return _mutex_unlock(mutex);
}

//==============================================================================
/**
 * @brief Function create new queue
 *
 * @param[in] length            queue length
 * @param[in] item_size         queue item size
 *
 * @return pointer to queue object, otherwise NULL if error
 */
//==============================================================================
static inline queue_t*_sys_queue_new(const uint length, const uint item_size)
{
        return _queue_new(length, item_size);
}

//==============================================================================
/**
 * @brief Function delete queue
 *
 * @param[in] *queue            queue object
 */
//==============================================================================
static inline void _sys_queue_delete(queue_t *queue)
{
        return _queue_delete(queue);
}

//==============================================================================
/**
 * @brief Function reset queue
 *
 * @param[in] *queue            queue object
 */
//==============================================================================
static inline void _sys_queue_reset(queue_t *queue)
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
 * @retval true         item posted
 * @retval false        item not posted
 */
//==============================================================================
static inline bool _sys_queue_send(queue_t *queue, const void *item, const uint waittime_ms)
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
 * @retval true         item posted
 * @retval false        item not posted
 */
//==============================================================================
static inline bool _sys_queue_send_from_ISR(queue_t *queue, const void *item, bool *task_woken)
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
 * @retval true         item received
 * @retval false        item not received
 */
//==============================================================================
static inline bool _sys_queue_receive(queue_t *queue, void *item, const uint waittime_ms)
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
 * @retval true                 item received
 * @retval false                item not received
 */
//==============================================================================
static inline bool _sys_queue_receive_from_ISR(queue_t *queue, void *item, bool *task_woken)
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
 * @retval true         item received
 * @retval false        item not received
 */
//==============================================================================
static inline bool _sys_queue_receive_peek(queue_t *queue, void *item, const uint waittime_ms)
{
        return _queue_receive_peek(queue, item, waittime_ms);
}

//==============================================================================
/**
 * @brief Function gets number of items in queue
 *
 * @param[in] *queue            queue object
 *
 * @return a number of items in queue, -1 if error
 */
//==============================================================================
static inline int _sys_queue_get_number_of_items(queue_t *queue)
{
        return _queue_get_number_of_items(queue);
}

//==============================================================================
/**
 * @brief Function gets number of items in queue from ISR
 *
 * @param[in] *queue            queue object
 *
 * @return a number of items in queue, -1 if error
 */
//==============================================================================
static inline int _sys_queue_get_number_of_items_from_ISR(queue_t *queue)
{
        return _queue_get_number_of_items_from_ISR(queue);
}

//==============================================================================
/**
 * @brief Function gets number of items in queue from ISR
 *
 * @param[in] *queue            queue object
 *
 * @return a number of items in queue, -1 if error
 */
//==============================================================================
static inline int _sys_queue_get_space_available(queue_t *queue)
{
        return _queue_get_space_available(queue);
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
 * @brief Function suspend current task
 */
//==============================================================================
static inline void _sys_task_suspend_now()
{
        _task_suspend_now();
}

//==============================================================================
/**
 * @brief Function yield task
 */
//==============================================================================
static inline void _sys_task_yield()
{
        _task_yield();
}

//==============================================================================
/**
 * @brief Function yield task from ISR
 */
//==============================================================================
static inline void _sys_task_yield_from_ISR()
{
        _task_yield_from_ISR();
}

//==============================================================================
/**
 * @brief Function return name of current task
 *
 * @return name of current task
 */
//==============================================================================
static inline char *_sys_task_get_name()
{
        return _task_get_name();
}

//==============================================================================
/**
 * @brief Function return current task handle object address
 *
 * @return current task handle
 */
//==============================================================================
static inline task_t *_sys_task_get_handle()
{
        return _task_get_handle();
}

//==============================================================================
/**
 * @brief Function set priority of current task
 *
 * @param[in]  priority         priority
 */
//==============================================================================
static inline void _sys_task_set_priority(const int priority)
{
        _task_set_priority(priority);
}

//==============================================================================
/**
 * @brief Function return priority of current task
 *
 * @return current task priority
 */
//==============================================================================
static inline int _sys_task_get_priority()
{
        return _task_get_priority();
}

//==============================================================================
/**
 * @brief Function return a free stack level of current task
 *
 * @return free stack level
 */
//==============================================================================
static inline int _sys_task_get_free_stack()
{
        return _task_get_free_stack();
}

//==============================================================================
/**
 * @brief Function return parent task handle
 *
 * @return parent task handle
 */
//==============================================================================
static inline task_t *_sys_task_get_parent_handle()
{
//        return _task_get_parent_handle(); // TODO
}

//==============================================================================
/**
 * @brief Function set global variables address
 *
 * @param[in] *mem
 */
//==============================================================================
static inline void _sys_task_set_memory_address(void *mem)
{
//        _task_set_memory_address(mem); // TODO
}

//==============================================================================
/**
 * @brief Function set stdin file
 *
 * @param[in] *file
 */
//==============================================================================
static inline void _sys_task_set_stdin(FILE *file)
{
//        _task_set_stdin(file); // TODO
}

//==============================================================================
/**
 * @brief Function set stdout file
 *
 * @param[in] *file
 */
//==============================================================================
static inline void _sys_task_set_stdout(FILE *file)
{
//        _task_set_stdout(file); // TODO
}

//==============================================================================
/**
 * @brief Function set stderr file
 *
 * @param[in] *file
 */
//==============================================================================
static inline void _sys_task_set_stderr(FILE *file)
{
//        _task_set_stderr(file); // TODO
}

//==============================================================================
/**
 * @brief Function set cwd path
 *
 * @param str           cwd string
 */
//==============================================================================
static inline void _sys_task_set_cwd(const char *str)
{
//        _task_set_cwd(str); // TODO
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
 * @return The current calendar time as a time_t object.
 *         If the argument is not a null pointer, the return value is the same
 *         as the one stored in the location pointed by argument timer.
 *         If the function could not retrieve the calendar time, it returns
 *         a value of -1.
 */
//==============================================================================
static inline time_t _sys_time(time_t *timer)
{
        return _time(timer);
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
 * @return On success 0 is returned.
 *         On error -1 is returned.
 */
//==============================================================================
static inline int _sys_stime(time_t *timer)
{
        return _stime(timer);
}

//==============================================================================
/**
 * @brief  Setup time zone by setting difference between UTC and local time
 *
 * @param  tdiff        time difference in seconds (can be negative)
 *
 * @return None
 */
//==============================================================================
static inline void _sys_tzset(int tdiff)
{
        _ltimeoff = tdiff;
}

//==============================================================================
/**
 * @brief  Return difference in seconds between UTC and local time
 *
 * @param  None
 *
 * @return Difference between UTC and local time in seconds.
 */
//==============================================================================
static inline int _sys_timezone()
{
        return _ltimeoff;
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

//==============================================================================
/**
 * @brief  Format time as string
 *
 * Copies into ptr the content of format, expanding its format specifiers into
 * the corresponding values that represent the time described in timeptr, with
 * a limit of maxsize characters.
 *
 * @param  ptr          Pointer to the destination array where the resulting
 *                      C string is copied.
 * @param  maxsize      Maximum number of characters to be copied to ptr,
 *                      including the terminating null-character.
 * @param  format       C string containing any combination of regular characters
 *                      and special format specifiers. These format specifiers
 *                      are replaced by the function to the corresponding values
 *                      to represent the time specified in timeptr.
 * @param timeptr       Pointer to a tm structure that contains a calendar time
 *                      broken down into its components (see struct tm).
 *
 * @return If the length of the resulting C string, including the terminating
 *         null-character, doesn't exceed maxsize, the function returns the
 *         total number of characters copied to ptr (not including the terminating
 *         null-character).
 *         Otherwise, it returns zero, and the contents of the array pointed by
 *         ptr are indeterminate.
 *
 * @note Supported flags:
 *       % - % character
 *       n - new line
 *       H - Hour in 24h format (00-23)
 *       I - Hour in 12h format (01-12)
 *       M - Minute (00-59)
 *       S - Second (00-61)
 *       A - Full weekday name
 *       a - Abbreviated weekday name
 *       B - Full month name
 *       b - Abbreviated month name
 *       h - Abbreviated month name
 *       C - Year divided by 100 and truncated to integer (00-99) (century)
 *       y - Year, last two digits (00-99)
 *       Y - Year
 *       d - Day of the month, zero-padded (01-31)
 *       p - AM or PM designation
 *       j - Day of the year (001-366)
 *       m - Month as a decimal number (01-12)
 *       X - Time representation                                14:55:02
 *       F - Short YYYY-MM-DD date, equivalent to %Y-%m-%d      2001-08-23
 *       D - Short MM/DD/YY date, equivalent to %m/%d/%y        08/23/01
 *       x - Short MM/DD/YY date, equivalent to %m/%d/%y        08/23/01
 *       z - ISO 8601 offset from UTC in timezone (1 minute=1, 1 hour=100) +0100, -1230
 */
//==============================================================================
static inline size_t _sys_strftime(char *ptr, size_t maxsize, const char *format, const struct tm *timeptr)
{
        return _strftime(ptr, maxsize, format, timeptr);
}

#ifdef __cplusplus
}
#endif

#endif /* _SYSFUNC_H_ */
/*==============================================================================
  End of file
==============================================================================*/
