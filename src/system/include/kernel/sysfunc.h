/*=========================================================================*//**
@file    sysfunc.h

@author  Daniel Zorychta

@brief   System function that must be used in drivers (modules) and file systems.

@note    Copyright (C) 2015 Daniel Zorychta <daniel.zorychta@gmail.com>

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
 * @defgroup sysfunc-h System functions
 *
 * This library is used in the @ref fs-h and @ref driver-h headers. Header
 * shall not be included directly by drivers or file systems. Those functions
 * are accessible only from drivers and file systems.
 *
 * @{
 */

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
#include "lib/btree.h"
#include "lib/vsnprintf.h"
#include "lib/vfprintf.h"
#include "lib/vsscanf.h"
#include "lib/stropt.h"
#include "kernel/errno.h"
#include "kernel/printk.h"
#include "kernel/kwrapper.h"
#include "kernel/time.h"
#include "kernel/process.h"
#include "kernel/syscall.h"
#include "fs/vfs.h"
#include "drivers/drvctrl.h"
#include "cpu/cpuctl.h"

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Exported macros
==============================================================================*/
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
 * @brief List's @b foreach loop.
 *
 * Macro creates foreach loop of linked-list object.
 *
 * @note Macro can be used only by file system or driver code.
 *
 * @param type                  object type (in most cases pointer type)
 * @param element               element name of type @b type
 * @param list                  [<b>llist_t</b>] list object
 *
 * @b Example
 * @code
        // ...

        llist_t *list = NULL;
        if (sys_llist_create(NULL, NULL, &list) == ESUCC) {
                // ...

                int var = 1;
                sys_llist_push_emplace_back(list, sizeof(int), &var);

                var = 2;
                sys_llist_push_emplace_back(list, sizeof(int), &var);

                var = 3;
                sys_llist_push_emplace_back(list, sizeof(int), &var);

                // ...

                sys_llist_foreach(int *, var, list) {
                        print("Value: %d\n", *var);
                }

                // ...
        }

        // ...
   @endcode
 *
 * @see sys_llist_foreach_reverse()
 */
#ifdef DOXYGEN
#define sys_llist_foreach(type, element, list)
#else
#define sys_llist_foreach(type, element, _sys_llist_t__list)\
        _llist_foreach(type, element, _sys_llist_t__list)
#endif

/**
 * @brief List's reverse @b foreach loop.
 *
 * Macro creates reverse foreach loop of linked-list object.
 *
 * @note Macro can be used only by file system or driver code.
 *
 * @param type                  object type (in most cases pointer type)
 * @param element               element name of type @b type
 * @param list                  [<b>llist_t</b>] list object
 *
 * @b Example
 * @code
        // ...

        llist_t *list = NULL;
        if (sys_llist_create(NULL, NULL, &list) == ESUCC) {
                // ...

                int var = 1;
                sys_llist_push_emplace_back(list, sizeof(int), &var);

                var = 2;
                sys_llist_push_emplace_back(list, sizeof(int), &var);

                var = 3;
                sys_llist_push_emplace_back(list, sizeof(int), &var);

                // ...

                sys_llist_foreach_reverse(int *, var, list) {
                        print("Value: %d\n", *var);
                }

                // ...
        }

        // ...
   @endcode
 *
 * @see sys_llist_foreach()
 */
#ifdef DOXYGEN
#define sys_llist_foreach_reverse(type, element, list)
#else
#define sys_llist_foreach_reverse(type, element, _sys_llist_t__list)\
        _llist_foreach_reverse(type, element, _sys_llist_t__list)
#endif

/*==============================================================================
  Exported object types
==============================================================================*/
#ifdef DOXYGEN /* Type defined in lib/llist.h */
/**
 * @brief Linked list type.
 *
 * The type represents linked list object. Fields of object are private.
 */
typedef struct {} llist_t;
#endif

/**
 * @brief Memory region type used for dynamic memory allocation.
 */
typedef _mm_region_t mem_region_t;

/*==============================================================================
  Exported objects
==============================================================================*/

/*==============================================================================
  Exported functions
==============================================================================*/
#ifdef DOXYGEN /* Doxygen documentation only. Functions in fs.h and driver.h */
//==============================================================================
/**
 * @brief  Allocate memory.
 *
 * @note Function can be used only by file system or driver code.
 *
 * @param size             object size
 * @param mem              pointer to memory block pointer
 *
 * @return One of @ref errno value.
 */
//==============================================================================
static inline int sys_malloc(size_t size, void **mem);

//==============================================================================
/**
 * @brief  Allocate memory and clear content.
 *
 * @note Function can be used only by file system or driver code.
 *
 * @param size             object size
 * @param mem              pointer to memory block pointer
 *
 * @return One of @ref errno value.
 */
//==============================================================================
static inline int sys_zalloc(size_t size, void **mem);

//==============================================================================
/**
 * @brief  Free allocated memory.
 *
 * Function free selected memory block (by double pointer) and sets memory block
 * pointer to @ref NULL.
 *
 * @note Function can be used only by file system or driver code.
 *
 * @param mem           double pointer to memory block to free
 *
 * @return One of @ref errno value.
 */
//==============================================================================
static inline int sys_free(void **mem);

//==============================================================================
/**
 * @brief  Get module allocated memory for specified major and minor number.
 *
 * Function get memory instance of module that call this function. The major
 * and minor numbers specify the instance.
 *
 * @note Function can be used only by driver code.
 *
 * @param major         major number
 * @param minor         minor number
 * @param mem           driver instance memory
 *
 * @return One of @ref errno value.
 */
//==============================================================================
int sys_module_get_instance(u8_t major, u8_t minor, void **mem);

#endif

//==============================================================================
/**
 * @brief Function converts string to double.
 *
 * The function convert the initial portion of the string pointed to by <i>nptr</i>
 * to double representation.
 *
 * @note Function can be used only by file system or driver code.
 *
 * @param nptr          string to convert
 * @param endptr        points to first not converted character
 *
 * @return These functions return the converted value, if any.
 *
 * @b Example
 * @code
        // convert string to decimal value
        const char *str = "123.56";
        double      val = sys_strtod(str, NULL);
   @endcode
 *
 * @see sys_atoi(), sys_atof()
 */
//==============================================================================
static inline double sys_strtod(const char *nptr, char **endptr)
{
        return _strtod(nptr, endptr);
}

//==============================================================================
/**
 * @brief Function converts string to integer.
 *
 * The function converts the initial portion of the string pointed
 * to by <i>str</i> to int. The behavior is the same as sys_strtoi(nptr, NULL, 10);
 * except that sys_atoi() does not detect errors.
 *
 * @note Function can be used only by file system or driver code.
 *
 * @param str           string to convert
 *
 * @return The converted value.
 *
 * @b Example
 * @code
        // ...
        int val = sys_atoi("125");
        // ...
   @endcode
 *
 * @see sys_atof(), sys_strtod(), sys_strtoi(), sys_atof()
 */
//==============================================================================
static inline i32_t sys_atoi(const char *str)
{
        return _atoi(str);
}

//==============================================================================
/**
 * @brief Function convert ASCII to the number.
 *
 * When function find any other character than number (depended of actual base)
 * immediately finished operation and return pointer when bad character was
 * found. If base is 0 then function recognize type of number used in string.
 * For hex values "0x" is recognized, for octal values "0" at the beginning of
 * string is recognized, for binary "0b" is recognized, and for decimals values
 * none above.
 *
 * @note Function can be used only by file system or driver code.
 *
 * @param[in]  string       string to decode
 * @param[in]  base         decode base
 * @param[out] value        pointer to result
 *
 * @return Pointer in string when operation was finished.
 *
 * @b Example
 * @code
        // ...
        i32_t val = 0;
        char *ptr = sys_strtoi("0xDEADBEEF", 0, &val);
        // ...
   @endcode
 *
 * @see sys_atoi(), sys_strtod(), sys_atof()
 */
//==============================================================================
static inline char *sys_strtoi(const char *string, int base, i32_t *value)
{
        return _strtoi(string, base, value);
}

//==============================================================================
/**
 * @brief Function converts string to double.
 *
 * The function converts the initial portion of the string pointed
 * to by <i>nptr</i> to double.  The behavior is the same as
 * sys_strtod(nptr, NULL) except that sys_atof() does not detect errors.
 *
 * @note Function can be used only by file system or driver code.
 *
 * @param nptr          string to convert
 *
 * @return The converted value.
 *
 * @b Example
 * @code
        // convert string to decimal value
        const char *str = "123.56";
        double      val = sys_atof(str);
   @endcode
 *
 * @see sys_atoi(), sys_strtod(), sys_strtoi()
 */
//==============================================================================
static inline double sys_atof(const char *nptr)
{
        return _atof(nptr);
}

#ifdef DOXYGEN /* function documentation only */
//==============================================================================
/**
 * @brief  Linked list constructor function.
 *
 * @note Function can be used only by file system or driver code.
 *
 * @param  functor      compare functor (can be @ref NULL)
 * @param  obj_dtor     object destructor (can be @ref NULL, then free() is used as destructor)
 * @param  list         pointer to list object
 *
 * @return One of @ref errno value.
 *
 * @b Example
 * @code
        // ...
        llist_t *list = NULL;
        int      err  = sys_llist_create(NULL, NULL, &list);
        if (err != ESUCC) {
                // ...
        } else {
                // ...

                sys_llist_destroy(list);
        }
   @endcode
 *
 * @see sys_llist_destroy()
 */
//==============================================================================
static inline int sys_llist_create(llist_cmp_functor_t functor, llist_obj_dtor_t obj_dtor, llist_t **list);
#endif

//==============================================================================
/**
 * @brief  Linked list destructor function.
 *
 * @note Function can be used only by file system or driver code.
 *
 * @param  list         pointer to list object
 *
 * @return One of @ref errno value.
 *
 * @b Example
 * @code
        // ...
        llist_t *list = NULL;
        int      err  = sys_llist_create(NULL, NULL, &list);
        if (err != ESUCC) {
                // ...
        } else {
                // ...

                sys_llist_destroy(list);
        }
   @endcode
 *
 * @see sys_llist_create()
 */
//==============================================================================
static inline int sys_llist_destroy(llist_t *list)
{
        return _llist_destroy(list);
}

//==============================================================================
/**
 * @brief  Checks if list is empty.
 *
 * @note Function can be used only by file system or driver code.
 *
 * @param  list         list object
 *
 * @return If list is empty then @b true is returned, otherwise @b false.
 *
 * @b Example
 * @code
        // ...
        llist_t *list = NULL;
        int      err  = sys_llist_create(NULL, NULL, &list);
        if (err != ESUCC) {
                // ...
        } else {
                if (sys_llist_empty()) {
                        // ...
                }

                // ...

                sys_llist_destroy(list);
        }
   @endcode
 *
 * @see sys_llist_size(), sys_llist_erase(), sys_llist_clear()
 */
//==============================================================================
static inline bool sys_llist_empty(llist_t *list)
{
        return _llist_empty(list);
}

//==============================================================================
/**
 * @brief  Function returns a number of elements of the list.
 *
 * @note Function can be used only by file system or driver code.
 *
 * @param  list         list object
 *
 * @return Number of elements of the list or @b -1 on error.
 *
 * @b Example
 * @code
        // ...
        llist_t *list = NULL;
        int      err  = sys_llist_create(NULL, NULL, &list);
        if (err != ESUCC) {
                // ...
        } else {
                if (sys_llist_size() > 10) {
                        // ...
                } else {
                        // ...
                }

                sys_llist_destroy(list);
        }
   @endcode
 *
 * @see sys_llist_empty(), sys_llist_erase(), sys_llist_clear()
 */
//==============================================================================
static inline int sys_llist_size(llist_t *list)
{
        return _llist_size(list);
}

//==============================================================================
/**
 * @brief  Pushes selected data to the front of the list. Creates a new object.
 *
 * @note Function can be used only by file system or driver code.
 *
 * @param  list         list object
 * @param  size         data size
 * @param  data         data source
 *
 * @return On success allocated memory pointer is returned, otherwise @ref NULL.
 *
 * @b Example
 * @code
        // ...
        llist_t *list = NULL;
        int      err  = sys_llist_create(NULL, NULL, &list);
        if (err != ESUCC) {
                // ...
        } else {
                // ...

                int val = 100;
                if (sys_llist_push_emplace_front(list, sizeof(int), &val) != NULL) {
                        // success ...
                } else {
                        // error handling ...
                }

                sys_llist_destroy(list);
        }
   @endcode
 *
 * @see sys_llist_push_front(), sys_llist_push_emplace_back()
 */
//==============================================================================
static inline void *sys_llist_push_emplace_front(llist_t *list, size_t size, const void *data)
{
        return _llist_push_emplace_front(list, size, data);
}

//==============================================================================
/**
 * @brief  Pushes selected object to the front of the list.
 *
 * @note Function can be used only by file system or driver code.
 *
 * @param  list         list object
 * @param  object       object to push
 *
 * @return On success pointer to the object is returned, otherwise @ref NULL.
 *
 * @b Example
 * @code
        // ...
        llist_t *list = NULL;
        int      err  = sys_llist_create(NULL, NULL, &list);
        if (err != ESUCC) {
                // ...
        } else {
                // ...

                int *val = malloc(sizeof(int));
                if (!val) {
                        // no memory ...
                } else {
                        *val = 100;
                }

                if (sys_llist_push_front(list, val) != NULL) {
                        // success ...
                } else {
                        // error handling ...
                }

                sys_llist_destroy(list);
        }
   @endcode
 *
 * @see sys_llist_push_emplace_front(), sys_llist_push_back()
 */
//==============================================================================
static inline void *sys_llist_push_front(llist_t *list, void *object)
{
        return _llist_push_front(list, object);
}

//==============================================================================
/**
 * @brief  Deletes first element of the list. This destroys an element by using
 *         selected destructor function.
 *
 * @note Function can be used only by file system or driver code.
 *
 * @param  list         list object
 *
 * @return On success @b 1 is returned, otherwise @b 0.
 *
 * @b Example
 * @code
        // ...
        llist_t *list = NULL;
        int      err  = sys_llist_create(NULL, NULL, &list);
        if (err != ESUCC) {
                // ...
        } else {
                // ...

                int *val = malloc(sizeof(int));
                if (!val) {
                        // no memory ...
                } else {
                        *val = 100;
                }

                if (sys_llist_push_front(list, val) != NULL) {
                        // success ...
                        // ...

                        sys_llist_pop_front(list);

                        // ...
                } else {
                        // error handling ...
                }

                sys_llist_destroy(list);
        }
   @endcode
 *
 * @see sys_llist_push_front(), sys_llist_push_emplace_front()
 */
//==============================================================================
static inline int sys_llist_pop_front(llist_t *list)
{
        return _llist_pop_front(list);
}

//==============================================================================
/**
 * @brief  Push selected data to the back of the list. Creates a new object.
 *
 * @note Function can be used only by file system or driver code.
 *
 * @param  list         list object
 * @param  size         data size
 * @param  data         data source
 *
 * @return On success allocated memory pointer is returned, otherwise @ref NULL.
 *
 * @b Example
 * @code
        // ...
        llist_t *list = NULL;
        int      err  = sys_llist_create(NULL, NULL, &list);
        if (err != ESUCC) {
                // ...
        } else {
                // ...

                int val = 100;
                if (sys_llist_push_emplace_back(list, sizeof(int), &val) != NULL) {
                        // success ...
                } else {
                        // error handling ...
                }

                sys_llist_destroy(list);
        }
   @endcode
 *
 * @see sys_llist_push_emplace_front(), sys_llist_push_front(), sys_llist_push_back()
 */
//==============================================================================
static inline void *sys_llist_push_emplace_back(llist_t *list, size_t size, const void *data)
{
        return _llist_push_emplace_back(list, size, data);
}

//==============================================================================
/**
 * @brief  Push selected object to the back of the list.
 *
 * @note Function can be used only by file system or driver code.
 *
 * @param  list         list object
 * @param  object       object to push
 *
 * @return On success pointer to the object is returned, otherwise @ref NULL.
 */
//==============================================================================
static inline void *sys_llist_push_back(llist_t *list, void *object)
{
        return _llist_push_back(list, object);
}

//==============================================================================
/**
 * @brief  Delete the last element of the list. This destroys element.
 *
 * @note Function can be used only by file system or driver code.
 *
 * @param  list         list object
 *
 * @return On success 1 is returned, otherwise 0.
 */
//==============================================================================
static inline int sys_llist_pop_back(llist_t *list)
{
        return _llist_pop_back(list);
}

//==============================================================================
/**
 * @brief  Allocate and append data at selected position in the list.
 *
 * @note Function can be used only by file system or driver code.
 *
 * @param  list         list object
 * @param  position     element position
 * @param  size         element's size
 * @param  data         element's data
 *
 * @return On success pointer to the object is returned, otherwise @ref NULL.
 */
//==============================================================================
static inline void *sys_llist_emplace(llist_t *list, int position, size_t size, const void *data)
{
        return _llist_emplace(list, position, size, data);
}

//==============================================================================
/**
 * @brief  Insert an element to the list.
 *
 * @note Function can be used only by file system or driver code.
 *
 * @param  list         list object
 * @param  position     position to insert
 * @param  object       object to insert
 *
 * @return On success object is returned, otherwise @ref NULL.
 */
//==============================================================================
static inline void *sys_llist_insert(llist_t *list, int position, void *object)
{
        return _llist_insert(list, position, object);
}

//==============================================================================
/**
 * @brief  Erase selected begin of the list. The element is destroyed.
 *
 * @note Function can be used only by file system or driver code.
 *
 * @param  list         list object
 * @param  position     position to remove
 *
 * @return On success 1 is returned, otherwise 0.
 */
//==============================================================================
static inline int sys_llist_erase(llist_t *list, int position)
{
        return _llist_erase(list, position);
}

//==============================================================================
/**
 * @brief  Return selected begin and remove from the list. The element is not destroyed.
 *
 * @note Function can be used only by file system or driver code.
 *
 * @param  list         list object
 * @param  position     position to take (unlink)
 *
 * @return On success taken object is returned, otherwise @ref NULL.
 */
//==============================================================================
static inline void *sys_llist_take(llist_t *list, int position)
{
        return _llist_take(list, position);
}

//==============================================================================
/**
 * @brief  Return first begin and remove from the list. The element is not destroyed.
 *
 * @note Function can be used only by file system or driver code.
 *
 * @param  list         list object
 *
 * @return On success taken object is returned, otherwise @ref NULL.
 */
//==============================================================================
static inline void *sys_llist_take_front(llist_t *list)
{
        return _llist_take_front(list);
}

//==============================================================================
/**
 * @brief  Return last begin and remove from the list. The element is not destroyed.
 *
 * @note Function can be used only by file system or driver code.
 *
 * @param  list         list object
 *
 * @return On success taken object is returned, otherwise @ref NULL.
 */
//==============================================================================
static inline void *sys_llist_take_back(llist_t *list)
{
        return _llist_take_back(list);
}

//==============================================================================
/**
 * @brief  Clear entire list (objects are destroyed).
 *
 * @note Function can be used only by file system or driver code.
 *
 * @param  list         list object

 * @return On success 1 is returned, otherwise 0.
 */
//==============================================================================
static inline int sys_llist_clear(llist_t *list)
{
        return _llist_clear(list);
}

//==============================================================================
/**
 * @brief  Swap 2 elements.
 *
 * @note Function can be used only by file system or driver code.
 *
 * @param  list         list object
 * @param  j            position of element a
 * @param  k            position of element b
 *
 * @return On success 1 is returned, otherwise 0.
 */
//==============================================================================
static inline int sys_llist_swap(llist_t *list, int j, int k)
{
        return _llist_swap(list, j, k);
}

//==============================================================================
/**
 * @brief  Quick sort elements of the list.
 *
 * @note Function can be used only by file system or driver code.
 *
 * @param  list         list object
 */
//==============================================================================
static inline void sys_llist_sort(llist_t *list)
{
        _llist_sort(list);
}

//==============================================================================
/**
 * @brief  Leave only an unique elements, all not unique are removed (are destroyed).
 *
 * @note Function can be used only by file system or driver code.
 *
 * @param  list         list object
 */
//==============================================================================
static inline void sys_llist_unique(llist_t *list)
{
        _llist_unique(list);
}

//==============================================================================
/**
 * @brief  Reverse entire table.
 *
 * @note Function can be used only by file system or driver code.
 *
 * @param  list         list object
 */
//==============================================================================
static inline void sys_llist_reverse(llist_t *list)
{
        _llist_reverse(list);
}

//==============================================================================
/**
 * @brief  Get element from the list at selected position.
 *
 * @note Function can be used only by file system or driver code.
 *
 * @param  list         list object
 * @param  position     begin position
 *
 * @return Pointer to data, or @ref NULL on error.
 */
//==============================================================================
static inline void *sys_llist_at(llist_t *list, int position)
{
        return _llist_at(list, position);
}

//==============================================================================
/**
 * @brief  Check if list contains selected object.
 *
 * @note Function can be used only by file system or driver code.
 *
 * @param  list         list object
 * @param  object       object to find
 *
 * @return Number of found objects, or -1 on error.
 */
//==============================================================================
static inline int sys_llist_contains(llist_t *list, const void *object)
{
        return _llist_contains(list, object);
}

//==============================================================================
/**
 * @brief  Find selected object in the list from the beginning.
 *
 * @note Function can be used only by file system or driver code.
 *
 * @param  list         list object
 * @param  object       object to find
 *
 * @return Object position, or -1 on error.
 */
//==============================================================================
static inline int sys_llist_find_begin(llist_t *list, const void *object)
{
        return _llist_find_begin(list, object);
}

//==============================================================================
/**
 * @brief  Find selected object in the list from the end.
 *
 * @note Function can be used only by file system or driver code.
 *
 * @param  list         list object
 * @param  object       object to find
 *
 * @return Object position, or -1 on error.
 */
//==============================================================================
static inline int sys_llist_find_end(llist_t *list, const void *object)
{
        return _llist_find_end(list, object);
}

//==============================================================================
/**
 * @brief  Access first element.
 *
 * @note Function can be used only by file system or driver code.
 *
 * @param  list         list object
 *
 * @return Pointer to data, or @ref NULL on error.
 */
//==============================================================================
static inline void *sys_llist_front(llist_t *list)
{
        return _llist_front(list);
}

//==============================================================================
/**
 * @brief  Access last element.
 *
 * @note Function can be used only by file system or driver code.
 *
 * @param  list         list object
 *
 * @return Pointer to data, or @ref NULL on error.
 */
//==============================================================================
static inline void *sys_llist_back(llist_t *list)
{
        return _llist_back(list);
}

//==============================================================================
/**
 * @brief  Create an iterator to the list.
 *
 * @note Function can be used only by file system or driver code.
 *
 * @param  list         list object
 *
 * @return Iterator object.
 *
 * @b Example
 * @code
        // ...
        llist_iterator_t it;

        // ...

        // iterate elements of the list from beginning
        it = sys_llist_iterator(list);
        for (void *obj = sys_llist_begin(&it); obj; obj = sys_llist_interator_next(&it)) {
                obj->...;
        }

        // ...

        // iterate elements of list for the end
        it = sys_llist_iterator(list);
        for (void *obj = sys_llist_end(&it); obj; obj = sys_llist_interator_prev(&it)) {
                obj->...;
        }

        // ...
   @endcode
 *
 * @see sys_llist_foreach(), sys_llist_foreach_reverse()
 */
//==============================================================================
static inline llist_iterator_t sys_llist_iterator(llist_t *list)
{
        return _llist_iterator(list);
}

//==============================================================================
/**
 * @brief  Return first object from list by using iterator.
 *
 * @note Function can be used only by file system or driver code.
 *
 * @param  iterator     iterator object
 *
 * @return Pointer to data object.
 *
 * @b Example
 * @code
        // ...
        llist_iterator_t it;

        // ...

        // iterate elements of the list from beginning
        it = sys_llist_iterator(list);
        for (void *obj = sys_llist_begin(&it); obj; obj = sys_llist_interator_next(&it)) {
                obj->...;
        }

        // ...

        // iterate elements of list for the end
        it = sys_llist_iterator(list);
        for (void *obj = sys_llist_end(&it); obj; obj = sys_llist_interator_prev(&it)) {
                obj->...;
        }

        // ...
   @endcode
 *
 * @see sys_llist_foreach(), sys_llist_foreach_reverse()
 */
//==============================================================================
static inline void *sys_llist_begin(llist_iterator_t *iterator)
{
        return _llist_begin(iterator);
}

//==============================================================================
/**
 * @brief  Return last object from list by using iterator.
 *
 * @note Function can be used only by file system or driver code.
 *
 * @param  iterator     iterator object
 *
 * @return Pointer to data object.
 *
 * @b Example
 * @code
        // ...
        llist_iterator_t it;

        // ...

        // iterate elements of the list from beginning
        it = sys_llist_iterator(list);
        for (void *obj = sys_llist_begin(&it); obj; obj = sys_llist_interator_next(&it)) {
                obj->...;
        }

        // ...

        // iterate elements of list for the end
        it = sys_llist_iterator(list);
        for (void *obj = sys_llist_end(&it); obj; obj = sys_llist_interator_prev(&it)) {
                obj->...;
        }

        // ...
   @endcode
 *
 * @see sys_llist_foreach(), sys_llist_foreach_reverse()
 */
//==============================================================================
static inline void *sys_llist_end(llist_iterator_t *iterator)
{
        return _llist_end(iterator);
}

//==============================================================================
/**
 * @brief  Return selected objects from list by using range iterator (forward).
 *
 * @note Function can be used only by file system or driver code.
 *
 * @param  iterator     iterator object
 * @param  begin        begin position
 * @param  end          end position
 *
 * @return Pointer to data object.
 *
 * @b Example
 * @code
        // ...

        // iterate list from object <5, 8>
        llist_iterator_t it = sys_llist_iterator(list);
        for (void *obj = sys_llist_range(&it, 5, 8); obj; obj = sys_llist_interator_next(&it)) {
                obj->...;
        }

        // ...
   @endcode
 *
 * @see sys_llist_foreach(), sys_llist_foreach_reverse()
 */
//==============================================================================
static inline void *sys_llist_range(llist_iterator_t *iterator, int begin, int end)
{
        return _llist_range(iterator, begin, end);
}

//==============================================================================
/**
 * @brief  Return next data object from list by using iterator.
 *
 * @note Function can be used only by file system or driver code.
 *
 * @param  iterator     iterator object
 *
 * @return Pointer to data object.
 *
 * @b Example
 * @code
        // ...
        llist_iterator_t it;

        // ...

        // iterate elements of the list from beginning
        it = sys_llist_iterator(list);
        for (void *obj = sys_llist_begin(&it); obj; obj = sys_llist_interator_next(&it)) {
                obj->...;
        }

        // ...

        // iterate elements of list for the end
        it = sys_llist_iterator(list);
        for (void *obj = sys_llist_end(&it); obj; obj = sys_llist_interator_prev(&it)) {
                obj->...;
        }

        // ...
   @endcode
 *
 * @see sys_llist_foreach(), sys_llist_foreach_reverse()
 */
//==============================================================================
static inline void *sys_llist_iterator_next(llist_iterator_t *iterator)
{
        return _llist_iterator_next(iterator);
}

//==============================================================================
/**
 * @brief  Return previous data object from list by using iterator.
 *
 * @note Function can be used only by file system or driver code.
 *
 * @param  iterator     iterator object
 *
 * @return Pointer to data object.
 *
 * @b Example
 * @code
        // ...
        llist_iterator_t it;

        // ...

        // iterate elements of the list from beginning
        it = sys_llist_iterator(list);
        for (void *obj = sys_llist_begin(&it); obj; obj = sys_llist_interator_next(&it)) {
                obj->...;
        }

        // ...

        // iterate elements of list for the end
        it = sys_llist_iterator(list);
        for (void *obj = sys_llist_end(&it); obj; obj = sys_llist_interator_prev(&it)) {
                obj->...;
        }

        // ...
   @endcode
 *
 * @see sys_llist_foreach(), sys_llist_foreach_reverse()
 */
//==============================================================================
static inline void *sys_llist_iterator_prev(llist_iterator_t *iterator)
{
        return _llist_iterator_prev(iterator);
}

//==============================================================================
/**
 * @brief  Erase selected begin of the list. The element is destroyed.
 *
 * @note Function can be used only by file system or driver code.
 *
 * @param  iterator     position to remove
 *
 * @return On success 1 is returned, otherwise 0.
 *
 * @b Example
 * @code
        // ...

        llist_iterator_t it = sys_llist_iterator(list);
        for (void *obj = sys_llist_begin(&it); obj; obj = sys_llist_interator_next(&it)) {
                if (obj->... > 0) {
                        sys_llist_erase_by_iterator(&it);
                }
        }

        // ...
   @endcode
 */
//==============================================================================
static inline int sys_llist_erase_by_iterator(llist_iterator_t *iterator)
{
        return _llist_erase_by_iterator(iterator);
}

//==============================================================================
/**
 * @brief  Compare functor that compares two pointers (not contents).
 *
 * @note Function can be used only by file system or driver code.
 *
 * @param  a    pointer a
 * @param  b    pointer b
 *
 * @retval a > b:  1
 * @retval a = b:  0
 * @retval a < b: -1
 */
//==============================================================================
static inline int sys_llist_functor_cmp_pointers(const void *a, const void *b)
{
        return _llist_functor_cmp_pointers(a, b);
}

//==============================================================================
/**
 * @brief  Compare functor that compares two strings (contents).
 *
 * @note Function can be used only by file system or driver code.
 *
 * @param  a    string a
 * @param  b    string b
 *
 * @retval a > b:  1
 * @retval a = b:  0
 * @retval a < b: -1
 */
//==============================================================================
static inline int sys_llist_functor_cmp_strings(const void *a, const void *b)
{
        return _llist_functor_cmp_strings(a, b);
}

#ifdef DOXYGEN /* function documentation only */
//==============================================================================
/**
 * @brief  BTree constructor.
 *
 * @note Function can be used only by file system or driver code.
 *
 * @param  size                 object size (element size)
 * @param  cmp_functor          compare functor (can be NULL)
 * @param  obj_dtor             object destructor (can be NULL, then free() is destructor)
 * @param  btree                created b-tree object
 *
 * @return One of @ref errno value.
 *
 * @b Example
 * @code
        // ...

        typedef struct {
                int value;
                ...
        } bt_obj_t;

        static int cmp(const void *a, const void *b)
        {
                if (a->value > b->value) {
                        return 1;
                } else if (a->value > b->value) {
                        return -1;
                } else {
                        return 0;
                }
        }

        btree_t *bt = NULL;
        int err = sys_btree_create(sizeof(bt_obj_t), cmp, NULL, &bt);
        if (!err) {

                bt_obj_t obj1 = {.value = 3};
                err = sys_btree_insert(bt, &obj1);
                if (!err) {
                        ...
                }

                bt_obj_t obj2 = {.value = 5};
                err = sys_btree_insert(bt, &obj2);
                if (!err) {
                        ...
                }

                ...

                bt_obj_t obj;
                err = sys_btree_search(bt, &obj1, &obj);
                if (!err) {
                        ...
                }

                ...

                sys_btree_destroy(bt);
        }


        // ...
   @endcode
 */
//==============================================================================
static inline int sys_btree_create(size_t size, btree_cmp_functor_t cmp_functor, btree_obj_dtor_t obj_dtor, btree_t **btree);
#endif

//==============================================================================
/**
 * @brief  Function search selected object in BTree.
 *
 * @note Function can be used only by file system or driver code.
 *
 * @param  tree         BTree object
 * @param  key          object to find
 * @param  ret          found object
 *
 * @return One of errno value.
 *
 * @b Example
 * @code
        // ...

        typedef struct {
                int value;
                ...
        } bt_obj_t;

        static int cmp(const void *a, const void *b)
        {
                if (a->value > b->value) {
                        return 1;
                } else if (a->value > b->value) {
                        return -1;
                } else {
                        return 0;
                }
        }

        btree_t *bt = NULL;
        int err = sys_btree_create(sizeof(bt_obj_t), cmp, NULL, &bt);
        if (!err) {

                bt_obj_t obj1 = {.value = 3};
                err = sys_btree_insert(bt, &obj1);
                if (!err) {
                        ...
                }

                bt_obj_t obj2 = {.value = 5};
                err = sys_btree_insert(bt, &obj2);
                if (!err) {
                        ...
                }

                ...

                bt_obj_t obj;
                err = sys_btree_search(bt, &obj1, &obj);
                if (!err) {
                        ...
                }

                ...

                sys_btree_destroy(bt);
        }


        // ...
   @endcode
 */
//==============================================================================
static inline int sys_btree_search(btree_t *tree, void *key, void *ret)
{
        return _btree_search(tree, key, ret);
}

//==============================================================================
/**
 * @brief  Function get object with minimum value.
 *
 * @note Function can be used only by file system or driver code.
 *
 * @param  tree         BTree object
 * @param  ret          return object
 *
 * @return One of errno value.
 *
 * @b Example
 * @code
        // ...

        typedef struct {
                int value;
                ...
        } bt_obj_t;

        static int cmp(const void *a, const void *b)
        {
                if (a->value > b->value) {
                        return 1;
                } else if (a->value > b->value) {
                        return -1;
                } else {
                        return 0;
                }
        }

        btree_t *bt = NULL;
        int err = sys_btree_create(sizeof(bt_obj_t), cmp, NULL, &bt);
        if (!err) {

                bt_obj_t obj1 = {.value = 3};
                err = sys_btree_insert(bt, &obj1);
                if (!err) {
                        ...
                }

                bt_obj_t obj2 = {.value = 5};
                err = sys_btree_insert(bt, &obj2);
                if (!err) {
                        ...
                }

                ...

                bt_obj_t obj;
                err = sys_btree_search(bt, &obj1, &obj);
                if (!err) {
                        ...
                }

                ...

                sys_btree_destroy(bt);
        }

        // ...
   @endcode
 */
//==============================================================================
static inline int sys_btree_minimum(btree_t *tree, void *ret)
{
        return _btree_minimum(tree, ret);
}

//==============================================================================
/**
 * @brief  Function get object with maximum value.
 *
 * @note Function can be used only by file system or driver code.
 *
 * @param  tree         BTree object
 * @param  ret          return object
 *
 * @return One of errno value.
 *
 * @b Example
 * @code
        // ...

        typedef struct {
                int value;
                ...
        } bt_obj_t;

        static int cmp(const void *a, const void *b)
        {
                if (a->value > b->value) {
                        return 1;
                } else if (a->value > b->value) {
                        return -1;
                } else {
                        return 0;
                }
        }

        btree_t *bt = NULL;
        int err = sys_btree_create(sizeof(bt_obj_t), cmp, NULL, &bt);
        if (!err) {

                bt_obj_t obj1 = {.value = 3};
                err = sys_btree_insert(bt, &obj1);
                if (!err) {
                        ...
                }

                bt_obj_t obj2 = {.value = 5};
                err = sys_btree_insert(bt, &obj2);
                if (!err) {
                        ...
                }

                ...

                bt_obj_t obj;
                err = sys_btree_search(bt, &obj1, &obj);
                if (!err) {
                        ...
                }

                ...

                sys_btree_destroy(bt);
        }

        // ...
   @endcode
 */
//==============================================================================
static inline int sys_btree_maximum(btree_t *tree, void *ret)
{
        return _btree_maximum(tree, ret);
}

//==============================================================================
/**
 * @brief  Function check if BTree is empty.
 *
 * @note Function can be used only by file system or driver code.
 *
 * @param  tree         BTree object
 *
 * @return Return true if BTree is empty, otherwise NULL.
 *
 * @b Example
 * @code
        // ...

        typedef struct {
                int value;
                ...
        } bt_obj_t;

        static int cmp(const void *a, const void *b)
        {
                if (a->value > b->value) {
                        return 1;
                } else if (a->value > b->value) {
                        return -1;
                } else {
                        return 0;
                }
        }

        btree_t *bt = NULL;
        int err = sys_btree_create(sizeof(bt_obj_t), cmp, NULL, &bt);
        if (!err) {

                bt_obj_t obj1 = {.value = 3};
                err = sys_btree_insert(bt, &obj1);
                if (!err) {
                        ...
                }

                bt_obj_t obj2 = {.value = 5};
                err = sys_btree_insert(bt, &obj2);
                if (!err) {
                        ...
                }

                ...

                bt_obj_t obj;
                err = sys_btree_search(bt, &obj1, &obj);
                if (!err) {
                        ...
                }

                ...

                sys_btree_destroy(bt);
        }

        // ...
   @endcode
 */
//==============================================================================
static inline bool sys_btree_is_empty(btree_t *tree)
{
        return _btree_is_empty(tree);
}

//==============================================================================
/**
 * @brief  Function return successor object relative to given key.
 *
 * @note Function can be used only by file system or driver code.
 *
 * @param  tree         BTree object
 * @param  key          key to find
 * @param  ret          successor object relative to key
 *
 * @return One of errno value.
 *
 * @b Example
 * @code
        // ...

        typedef struct {
                int value;
                ...
        } bt_obj_t;

        static int cmp(const void *a, const void *b)
        {
                if (a->value > b->value) {
                        return 1;
                } else if (a->value > b->value) {
                        return -1;
                } else {
                        return 0;
                }
        }

        btree_t *bt = NULL;
        int err = sys_btree_create(sizeof(bt_obj_t), cmp, NULL, &bt);
        if (!err) {

                bt_obj_t obj1 = {.value = 3};
                err = sys_btree_insert(bt, &obj1);
                if (!err) {
                        ...
                }

                bt_obj_t obj2 = {.value = 5};
                err = sys_btree_insert(bt, &obj2);
                if (!err) {
                        ...
                }

                ...

                bt_obj_t obj;
                err = sys_btree_search(bt, &obj1, &obj);
                if (!err) {
                        ...
                }

                ...

                sys_btree_destroy(bt);
        }

        // ...
   @endcode
 */
//==============================================================================
static inline int sys_btree_successor(btree_t *tree, void *key, void *ret)
{
        return _btree_successor(tree, key, ret);
}

//==============================================================================
/**
 * @brief  Function return predecessor object relative to given key.
 *
 * @note Function can be used only by file system or driver code.
 *
 * @param  tree         BTree object
 * @param  key          key to find
 * @param  ret          predecessor object relative to key
 *
 * @return One of errno value.
 *
 * @b Example
 * @code
        // ...

        typedef struct {
                int value;
                ...
        } bt_obj_t;

        static int cmp(const void *a, const void *b)
        {
                if (a->value > b->value) {
                        return 1;
                } else if (a->value > b->value) {
                        return -1;
                } else {
                        return 0;
                }
        }

        btree_t *bt = NULL;
        int err = sys_btree_create(sizeof(bt_obj_t), cmp, NULL, &bt);
        if (!err) {

                bt_obj_t obj1 = {.value = 3};
                err = sys_btree_insert(bt, &obj1);
                if (!err) {
                        ...
                }

                bt_obj_t obj2 = {.value = 5};
                err = sys_btree_insert(bt, &obj2);
                if (!err) {
                        ...
                }

                ...

                bt_obj_t obj;
                err = sys_btree_search(bt, &obj1, &obj);
                if (!err) {
                        ...
                }

                ...

                sys_btree_destroy(bt);
        }

        // ...
   @endcode
 */
//==============================================================================
static inline int sys_btree_predecessor(btree_t *tree, void *key, void *ret)
{
        return _btree_predecessor(tree, key, ret);
}

//==============================================================================
/**
 * @brief  Function insert object to BTree.
 *
 * @note Function can be used only by file system or driver code.
 *
 * @param  tree         BTree object
 * @param  data         object to insert to
 *
 * @return One of errno value.
 *
 * @b Example
 * @code
        // ...

        typedef struct {
                int value;
                ...
        } bt_obj_t;

        static int cmp(const void *a, const void *b)
        {
                if (a->value > b->value) {
                        return 1;
                } else if (a->value > b->value) {
                        return -1;
                } else {
                        return 0;
                }
        }

        btree_t *bt = NULL;
        int err = sys_btree_create(sizeof(bt_obj_t), cmp, NULL, &bt);
        if (!err) {

                bt_obj_t obj1 = {.value = 3};
                err = sys_btree_insert(bt, &obj1);
                if (!err) {
                        ...
                }

                bt_obj_t obj2 = {.value = 5};
                err = sys_btree_insert(bt, &obj2);
                if (!err) {
                        ...
                }

                ...

                bt_obj_t obj;
                err = sys_btree_search(bt, &obj1, &obj);
                if (!err) {
                        ...
                }

                ...

                sys_btree_destroy(bt);
        }

        // ...
   @endcode
 */
//==============================================================================
static inline int sys_btree_insert(btree_t *tree, void *data)
{
        return _btree_insert(tree, data);
}

//==============================================================================
/**
 * @brief  Function delete object from BTree selected by key.
 *
 * @note Function can be used only by file system or driver code.
 *
 * @param  tree         BTree object
 * @param  key          object to delete
 *
 * @return One of errno value.
 *
 * @b Example
 * @code
        // ...

        typedef struct {
                int value;
                ...
        } bt_obj_t;

        static int cmp(const void *a, const void *b)
        {
                if (a->value > b->value) {
                        return 1;
                } else if (a->value > b->value) {
                        return -1;
                } else {
                        return 0;
                }
        }

        btree_t *bt = NULL;
        int err = sys_btree_create(sizeof(bt_obj_t), cmp, NULL, &bt);
        if (!err) {

                bt_obj_t obj1 = {.value = 3};
                err = sys_btree_insert(bt, &obj1);
                if (!err) {
                        ...
                }

                bt_obj_t obj2 = {.value = 5};
                err = sys_btree_insert(bt, &obj2);
                if (!err) {
                        ...
                }

                ...

                bt_obj_t obj;
                err = sys_btree_search(bt, &obj1, &obj);
                if (!err) {
                        ...
                }

                ...

                sys_btree_destroy(bt);
        }

        // ...
   @endcode
 */
//==============================================================================
static inline int sys_btree_remove(btree_t *tree, void *key)
{
        return _btree_remove(tree, key);
}

//==============================================================================
/**
 * @brief  Function destroy BTree.
 *
 * @note Function can be used only by file system or driver code.
 *
 * @param  tree         BTree object
 *
 * @b Example
 * @code
        // ...

        typedef struct {
                int value;
                ...
        } bt_obj_t;

        static int cmp(const void *a, const void *b)
        {
                if (a->value > b->value) {
                        return 1;
                } else if (a->value > b->value) {
                        return -1;
                } else {
                        return 0;
                }
        }

        btree_t *bt = NULL;
        int err = sys_btree_create(sizeof(bt_obj_t), cmp, NULL, &bt);
        if (!err) {

                bt_obj_t obj1 = {.value = 3};
                err = sys_btree_insert(bt, &obj1);
                if (!err) {
                        ...
                }

                bt_obj_t obj2 = {.value = 5};
                err = sys_btree_insert(bt, &obj2);
                if (!err) {
                        ...
                }

                ...

                bt_obj_t obj;
                err = sys_btree_search(bt, &obj1, &obj);
                if (!err) {
                        ...
                }

                ...

                sys_btree_destroy(bt);
        }

        // ...
   @endcode
 */
//==============================================================================
static inline void sys_btree_destroy(btree_t *tree)
{
        return _btree_destroy(tree);
}

//==============================================================================
/**
 * @brief Function creates device node (device file).
 *
 * Function creates a file system node (device special file) named
 * <i>path</i>. Node is connected to the device <i>dev</i>.
 *
 * @note Function can be used only by file system or driver code.
 *
 * @param path          node name
 * @param dev           device number
 *
 * @return One of @ref errno value.
 *
 * @b Example
 * @code
        // ...

        dev_t dev = _dev_t__create(_module_get_ID("UART"), 0, 0);
        sys_mknod("/dev/uart0", dev);

        // ...
   @endcode
 */
//==============================================================================
static inline int sys_mknod(const char *path, dev_t dev)
{
        struct vfs_path cpath;
        cpath.CWD  = NULL;
        cpath.PATH = path;

        return _vfs_mknod(&cpath, dev);
}

//==============================================================================
/**
 * @brief Function creates new directory.
 *
 * The function attempts to create a directory named <i>pathname</i>. The
 * argument <i>mode</i> specifies the permissions to use.
 *
 * @note Function can be used only by file system or driver code.
 *
 * @param pathname      directory name
 * @param mode          directory permissions
 *
 * @return One of @ref errno value.
 *
 * @b Example
 * @code
        // ...

        sys_mkdir("/dev", 0666);    // wr+rd access for all users, groups and others

        // ...

   @endcode
 */
//==============================================================================
static inline int sys_mkdir(const char *pathname, mode_t mode)
{
#if __OS_ENABLE_MKDIR__ == _YES_
        struct vfs_path path;
        path.CWD  = NULL;
        path.PATH = pathname;

        return _vfs_mkdir(&path, mode);
#else
        UNUSED_ARG2(pathname, mode);
        return ENOTSUP;
#endif
}

//==============================================================================
/**
 * @brief Function creates FIFO file.
 *
 * The mfunction makes a FIFO special file with name <i>pathname</i>. <i>mode</i>
 * specifies the FIFO's permissions. A FIFO special file is similar to pipe, but
 * is created in filesystem and is not an anonymous. Access to FIFO is the same
 * as to regular file, except that data can be read only one time. Not all
 * filesystems support this file type.
 *
 * @note Function can be used only by file system or driver code.
 *
 * @param pathname      FIFO name
 * @param mode          FIFO permissions
 *
 * @return One of @ref errno value.
 *
 * @b Example
 * @code
        // ...

        sys_mkfifo("/dev/my_fifo", 0666);    // wr+rd access for all users, groups and others

        // ...

   @endcode
 */
//==============================================================================
static inline int sys_mkfifo(const char *pathname, mode_t mode)
{
#if __OS_ENABLE_MKFIFO__ == _YES_
        struct vfs_path cpath;
        cpath.CWD  = NULL;
        cpath.PATH = pathname;

        return _vfs_mkfifo(&cpath, mode);
#else
        UNUSED_ARG2(pathname, mode);
        return ENOTSUP;
#endif
}

//==============================================================================
/**
 * @brief Function opens selected directory.
 *
 * Function opens a directory stream corresponding to the directory <i>path</i>, and
 * returns a pointer to the directory stream. The stream is positioned at the first
 * entry in the directory. Opened stream is passed by pointer <i>dir</i>.
 *
 * @note Function can be used only by file system or driver code.
 *
 * @param path          directory path
 * @param dir           pointer to stream object
 *
 * @return One of @ref errno value.
 *
 * @b Example
 * @code
        // ...

        DIR *dir = NULL;

        if (sys_opendir("/foo/bar", &dir) == ESUCC) {

                dirent_t *dirent = NULL;

                while (sys_readdir(dir, &dirent) == ESUCC) {
                        // ...
                }

                sys_closedir(dir);
        } else {
                // ...
        }

        // ...
   @endcode
 *
 * @see sys_closedir(), sys_readdir()
 */
//==============================================================================
extern int sys_opendir(const char *path, DIR **dir);

//==============================================================================
/**
 * @brief Function closes selected directory stream.
 *
 * Function closes the directory stream associated with <i>dir</i>. The directory
 * stream descriptor <i>dir</i> is not available after this call.
 *
 * @note Function can be used only by file system or driver code.
 *
 * @param dir           pinter to directory object
 *
 * @return One of @ref errno value.
 *
 * @b Example
 * @code
        // ...

        DIR *dir = NULL;

        if (sys_opendir("/foo/bar", &dir) == ESUCC) {

                dirent_t *dirent = NULL;

                while (sys_readdir(dir, &dirent) == ESUCC) {
                        // ...
                }

                sys_closedir(dir);
        } else {
                // ...
        }

        // ...
   @endcode
 *
 * @see sys_opendir(), sys_readdir()
 */
//==============================================================================
extern int sys_closedir(DIR *dir);

//==============================================================================
/**
 * @brief Function reads entry from opened directory stream.
 *
 * Function returns a pointer <i>dirent</i> to object <b>dirent_t</b> type
 * representing the next directory entry in the directory stream pointed to by
 * <i>dir</i>.<p>
 *
 * @note Function can be used only by file system or driver code.
 *
 * @param dir           directory object
 * @param dirent        directory entry
 *
 * @exception | @ref EINVAL
 * @exception | @ref ENOMEM
 * @exception | @ref EACCES
 * @exception | @ref ENOENT
 * @exception | ...
 *
 * @return One of @ref errno value.
 *
 * @b Example
 * @code
        // ...

        DIR *dir = NULL;

        if (sys_opendir("/foo/bar", &dir) == ESUCC) {

                dirent_t *dirent = NULL;

                while (sys_readdir(dir, &dirent) == ESUCC) {
                        // ...
                }

                sys_closedir(dir);
        } else {
                // ...
        }

        // ...
   @endcode
 *
 * @see sys_opendir(), sys_closedir()
 */
//==============================================================================
static inline int sys_readdir(DIR *dir, dirent_t **dirent)
{
        return _vfs_readdir(dir, dirent);
}

//==============================================================================
/**
 * @brief Function remove selected file.
 *
 * The function deletes a name from the file system. If the removed name was
 * the last link to a file and no processes have the file open, the file is
 * deleted and the space it was using is made available for reuse.<p>
 *
 * If the name referred to a FIFO, or device, the name is removed, but
 * processes which have the object open may continue to use it.
 *
 * @note Function can be used only by file system or driver code.
 *
 * @param path      path to file
 *
 * @return One of @ref errno value.
 *
 * @b Example
 * @code
        // ...

        int err = sys_remove("/foo/bar");

        // ...
   @endcode
 */
//==============================================================================
static inline int sys_remove(const char *path)
{
#if __OS_ENABLE_REMOVE__ == _YES_
        struct vfs_path cpath;
        cpath.CWD  = NULL;
        cpath.PATH = path;
        return _vfs_remove(&cpath);
#else
        UNUSED_ARG1(path);
        return ENOTSUP;
#endif
}

//==============================================================================
/**
 * @brief Function renames selected file.
 *
 * The function renames a file. In contrast to standard C library this function
 * don't move files between directories if <i>new_name</i> is localized on other
 * filesystem than <i>old_name</i>, otherwise it's depending on filesystem.
 *
 * @note Function can be used only by file system or driver code.
 *
 * @param old_name      old file name
 * @param new_name      new file name
 *
 * @return One of @ref errno value.
 *
 * @b Example
 * @code
        // ...

        int err = sys_rename("/foo/bar", "/foo/baz");

        // ...
   @endcode
 */
//==============================================================================
static inline int sys_rename(const char *old_name, const char *new_name)
{
#if __OS_ENABLE_RENAME__ == _YES_
        struct vfs_path cpath_old;
        cpath_old.CWD  = NULL;
        cpath_old.PATH = old_name;

        struct vfs_path cpath_new;
        cpath_new.CWD  = NULL;
        cpath_new.PATH = new_name;

        return _vfs_rename(&cpath_old, &cpath_new);
#else
        UNUSED_ARG2(old_name, new_name);
        return ENOTSUP;
#endif
}

//==============================================================================
/**
 * @brief Function changes file mode.
 *
 * The function change the permissions of a file specified by <i>path</i>.
 *
 * @note Function can be used only by file system or driver code.
 *
 * @param path          file to permission change
 * @param mode          new permissions
 *
 * @return One of @ref errno value.
 *
 * @b Example
 * @code
        // ...

        sys_chmod("/foo/bar", 0666);   // wr+rd access for all users, groups and others

        // ...

   @endcode
 */
//==============================================================================
static inline int sys_chmod(const char *path, mode_t mode)
{
#if __OS_ENABLE_CHMOD__ == _YES_
        struct vfs_path cpath;
        cpath.CWD  = NULL;
        cpath.PATH = path;
        return _vfs_chmod(&cpath, mode);
#else
        UNUSED_ARG2(path, mode);
        return ENOTSUP;
#endif
}

//==============================================================================
/**
 * @brief Function changes the ownership of file.
 *
 * The function changes the ownership of the file specified by <i>path</i>.
 *
 * @note Function can be used only by file system or driver code.
 *
 * @param path          path to file
 * @param owner         owner ID
 * @param group         group ID
 *
 * @return One of @ref errno value.
 *
 * @b Example
 * @code
        // ...

        int err = sys_chown("/foo/bar", 1000, 1000);

        // ...
   @endcode
 */
//==============================================================================
static inline int sys_chown(const char *path, uid_t owner, gid_t group)
{
#if __OS_ENABLE_CHOWN__ == _YES_
        struct vfs_path cpath;
        cpath.CWD  = NULL;
        cpath.PATH = path;
        return _vfs_chown(&cpath, owner, group);
#else
        UNUSED_ARG3(path, owner, group);
        return ENOTSUP;
#endif
}

//==============================================================================
/**
 * @brief Function gets file information.
 *
 * The function return information about a file specified by <i>path</i>.
 *
 * @note Function can be used only by file system or driver code.
 *
 * @param path          file to inspect
 * @param buf           file's information
 *
 * @return One of @ref errno value.
 *
 * @b Example
 * @code
        // ...

        struct stat info;

        if (sys_stat("/dev/foo", &info) == ESUCC) {
                // ...
        } else {
                // ...
        }

        // ...

   @endcode
 */
//==============================================================================
static inline int sys_stat(const char *path, struct stat *buf)
{
#if __OS_ENABLE_FSTAT__ == _YES_
        struct vfs_path cpath;
        cpath.CWD  = NULL;
        cpath.PATH = path;
        return _vfs_stat(&cpath, buf);
#else
        UNUSED_ARG2(path, buf);
        return ENOTSUP;
#endif
}

//==============================================================================
/**
 * @brief Function gets file system information.
 *
 * The function returns information about a mounted file system.
 * A <i>path</i> is directory of the mount point of file system.
 *
 * @note Function can be used only by file system or driver code.
 *
 * @param path          node name
 * @param statfs        file system information container
 *
 * @return One of @ref errno value.
 *
 * @b Example
 * @code
        // ...

        struct statfs info;
        if (sys_statfs("/proc", &info) == ESUCC) {
                // ...
        } else {
                // ...
        }

        // ...
   @endcode
 */
//==============================================================================
static inline int sys_statfs(const char *path, struct statfs *statfs)
{
#if __OS_ENABLE_STATFS__ == _YES_
        struct vfs_path cpath;
        cpath.CWD  = NULL;
        cpath.PATH = path;
        return _vfs_statfs(&cpath, statfs);
#else
        UNUSED_ARG2(path, statfs);
        return ENOTSUP;
#endif
}

//==============================================================================
/**
 * @brief Function opens file.
 *
 * The function opens the file whose name is the string pointed to by
 * <i>path</i> and associates a stream with it. The argument <i>mode</i> points
 * to a string beginning with one of the following sequences (possibly followed
 * by additional characters, as described below):<p>
 *
 * <b>r</b> - Open text file for reading. The stream is positioned at the
 * beginning of the file.<p>
 *
 * <b>r+</b> - Open for reading and writing. The stream is positioned at the
 * beginning of the file.<p>
 *
 * <b>w</b> - Truncate file to zero length or create text file for writing.
 * The stream is positioned at the beginning of the file.<p>
 *
 * <b>w+</b> - Open for reading and writing. The file is created if it does
 * not exist, otherwise it is truncated. The stream is positioned at the
 * beginning of the file.<p>
 *
 * <b>a</b> - Open for appending (writing at end of file). The file is
 * created if it does  not exist. The stream is positioned at the end of the
 * file.<p>
 *
 * <b>a+</b> - Open for reading and appending (writing at end of file). The
 * file is created if it does not exist. The initial file position for reading
 * is at the beginning of the file, but output is always appended to the end of
 * the file.<p>
 *
 * Pointer of opened stream is passed by <i>file</i> argument.
 *
 * @note Function can be used only by file system or driver code.
 *
 * @param path          path to file
 * @param mode          file open mode
 * @param file          pointer to file object
 *
 * @return One of @ref errno value.
 *
 * @b Example
 * @code
        // ...

        FILE *file = NULL;
        if (sys_fopen("/foo/bar", "w+", &file) == ESUCC) {
               // ...

               sys_fclose(file);
        } else {
               // ...
        }

        // ...
   @endcode
 *
 * @see sys_fclose()
 */
//==============================================================================
extern int sys_fopen(const char *path, const char *mode, FILE **file);

//==============================================================================
/**
 * @brief Function closes selected file.
 *
 * The function closes the created stream <i>file</i>.
 *
 * @note Function can be used only by file system or driver code.
 *
 * @param file          file to close
 *
 * @return One of @ref errno value.
 *
 * @b Example
 * @code
        // ...

        FILE *file = NULL;
        if (sys_fopen("/foo/bar", "w+", &file) == ESUCC) {
               // ...

               sys_fclose(file);
        } else {
               // ...
        }

        // ...
   @endcode
 *
 * @see sys_fopen()
 */
//==============================================================================
extern int sys_fclose(FILE *file);

//==============================================================================
/**
 * @brief Function writes data to stream.
 *
 * The function writes <i>size</i> bytes long buffer, to the stream pointed to
 * by <i>file</i>, obtaining them from the location given by <i>ptr</i>.
 * Number of wrote bytes is passed by <i>wrcnt</i> argument.
 *
 * @note Function can be used only by file system or driver code.
 *
 * @param ptr           data buffer
 * @param size          buffer size
 * @param wrcnt         number of wrote bytes
 * @param file          stream
 *
 * @return One of @ref errno value.
 *
 * @b Example
 * @code
        // ...

        FILE *file = NULL;
        if (sys_fopen("/foo/bar", "w+", &file) == ESUCC) {
               // ...

               const char buf[10] = {0,1,2,3,4,5,6,7,8,9};

               size_t wrcnt = 0;
               int err = sys_fwrite(buf, sizeof(buf), &wrcnt, file);

               // ...

               sys_fclose(file);
        } else {
               // ...
        }

        // ...
   @endcode
 *
 * @see sys_fread()
 */
//==============================================================================
static inline int sys_fwrite(const void *ptr, size_t size, size_t *wrcnt, FILE *file)
{
        return _vfs_fwrite(ptr, size, wrcnt, file);
}

//==============================================================================
/**
 * @brief Function reads data to stream.
 *
 * The function reads <i>size</i> bytes long buffer, from the stream pointed to
 * by <i>file</i>, storing them at the location given by <i>ptr</i>.
 * Number of read bytes is passed by <i>rdcnt</i> argument.
 *
 * @note Function can be used only by file system or driver code.
 *
 * @param ptr           data buffer
 * @param size          buffer size
 * @param rdcnt         number of read bytes
 * @param file          stream
 *
 * @return One of @ref errno value.
 *
 * @b Example
 * @code
        // ...

        FILE *file = NULL;
        if (sys_fopen("/foo/bar", "w+", &file) == ESUCC) {
               // ...

               char buf[10];

               size_t rdcnt = 0;
               int err = sys_fread(buf, sizeof(buf), &rdcnt, file);

               // ...

               sys_fclose(file);
        } else {
               // ...
        }

        // ...
   @endcode
 *
 * @see sys_fwrite()
 */
//==============================================================================
static inline int sys_fread(void *ptr, size_t size, size_t *rdcnt, FILE *file)
{
        return _vfs_fread(ptr, size, rdcnt, file);
}

//==============================================================================
/**
 * @brief Function sets file position indicator.
 *
 * The function sets the file position indicator for the stream
 * pointed to by <i>file</i>. The new position, measured in bytes, is obtained
 * by adding offset bytes to the position specified by whence. If whence is set
 * to @ref SEEK_SET, @ref SEEK_CUR, or @ref SEEK_END, the offset is
 * relative to the start of the file, the current position indicator, or
 * end-of-file, respectively. A successful call to the sys_fseek() function
 * clears the end-of-file indicator for the stream.
 *
 * @note Function can be used only by file system or driver code.
 *
 * @param file          stream
 * @param offset        offset
 * @param mode          seek mode
 *
 * @return One of @ref errno value.
 *
 * @b Example
 * @code
        // ...

        FILE *file = NULL;
        if (sys_fopen("/foo/bar", "w+", &file) == ESUCC) {
               // ...

               sys_fseek(file, 0, SEEK_SET);

               char buf[10];

               size_t rdcnt = 0;
               int err = sys_fread(buf, sizeof(buf), &rdcnt, file);

               // ...

               sys_fclose(file);
        } else {
               // ...
        }

        // ...
   @endcode
 *
 * @see sys_ftell()
 */
//==============================================================================
static inline int sys_fseek(FILE *file, i64_t offset, int mode)
{
        return _vfs_fseek(file, offset, mode);
}

//==============================================================================
/**
 * @brief Function returns file position indicator.
 *
 * The function obtains the current value of the file position
 * indicator pointed by <i>lseek</i> for the stream pointed to by <i>file</i>.
 *
 * @note Function can be used only by file system or driver code.
 *
 * @param file          stream
 * @param lseek         file position indicator
 *
 * @return One of @ref errno value.
 *
 * @b Example
 * @code
        // ...

        FILE *file = NULL;
        if (sys_fopen("/foo/bar", "w+", &file) == ESUCC) {
               // ...

               i64_t seek = 0;
               int err = sys_ftell(file, &seek);

               if (err == ESUCC && seek > 0) {
                       sys_fseek(file, 0, SEEK_SET);
               }

               char   buf[10];
               size_t rdcnt = 0;
               err = sys_fread(buf, sizeof(buf), &rdcnt, file);

               // ...

               sys_fclose(file);
        } else {
               // ...
        }

        // ...
   @endcode
 *
 * @see sys_fseek()
 */
//==============================================================================
static inline int sys_ftell(FILE *file, i64_t *lseek)
{
        return _vfs_ftell(file, lseek);
}

//==============================================================================
/**
 * @brief Function sends request to selected file to do non-standard operation.
 *
 * The function manipulates the file parameters. In particular, many
 * operating characteristics of character special files (e.g., drivers) may
 * be controlled with ioctl() requests.
 *
 * The second argument is a device-dependent request code. The third
 * argument is an untyped pointer to memory.
 *
 * @note Function can be used only by file system or driver code.
 *
 * @param file          stream to control
 * @param rq            request number (each driver has own requests)
 * @param ...           untyped pointer to memory (optional in some requests)
 *
 * @return One of @ref errno value.
 *
 * @b Example @b 1
 * @code
        // ...

        FILE *file = NULL;
        if (sys_fopen("/dev/tty0", "r", &file) == ESUCC) {
                int err = sys_ioctl(file, IOCTL_TTY__CLEAR_SCR);

                // ...
        } else {
                // ...
        }

        sys_fclose(file);

        // ...

   @endcode

 * @b Example @b 2
 * @code
        // ...

        FILE *file = NULL;
        if (sys_fopen("/dev/tty0", "r", &file) == ESUCC) {
                int row = -1;
                int err = sys_ioctl(file, IOCTL_TTY__GET_ROW, &row);

                // ...
        } else {
                // ...
        }

        sys_fclose(file);

        // ...

   @endcode
 *
 * @note
 * The names of all requests are constructed in the same way: @b IOCTL_<MODULE_NAME>__<REQUEST_NAME>.
 */
//==============================================================================
static inline int sys_ioctl(FILE *file, int rq, ...)
{
    va_list arg;
    va_start(arg, rq);
    int err = _vfs_vfioctl(file, rq, arg);
    va_end(arg);
    return err;
}

//==============================================================================
/**
 * @brief Function gets file information.
 *
 * The function return information about a file pointed by <i>file</i>.
 *
 * @note Function can be used only by file system or driver code.
 *
 * @param file          file object
 * @param buf           file's information
 *
 * @return One of @ref errno value.
 *
 * @b Example
 * @code
        // ...

        FILE *file = NULL;
        if (sys_fopen("/dev/tty0", "r", &file) == ESUCC) {
                // ...

                struct stat buf;
                int err = sys_fstat(file, &buf);

                // ...
        } else {
                // ...
        }

        sys_fclose(file);

        // ...

   @endcode
 */
//==============================================================================
static inline int sys_fstat(FILE *file, struct stat *buf)
{
        return _vfs_fstat(file, buf);
}

//==============================================================================
/**
 * @brief Function forces write buffers to stream.
 *
 * For output streams, sys_fflush() forces a write of all buffered data for
 * the given output or update stream via the stream's underlying write function.
 * For input streams, sys_fflush() discards any buffered data that has been
 * fetched from the underlying file. The open status of the stream is unaffected.
 *
 * @note Function can be used only by file system or driver code.
 *
 * @param file          stream
 *
 * @return One of @ref errno value.
 *
 * @b Example
 * @b Example
 * @code
        // ...

        FILE *file = NULL;
        if (sys_fopen("/foo/bar", "w+", &file) == ESUCC) {
               // ...

               const char buf[10] = {0,1,2,3,4,5,6,7,8,9};

               size_t wrcnt = 0;
               int err = sys_fwrite(buf, sizeof(buf), &wrcnt, file);

               err = sys_fflush(file);

               // ...

               sys_fclose(file);
        } else {
               // ...
        }

        // ...
   @endcode
 */
//==============================================================================
static inline int sys_fflush(FILE *file)
{
        return _vfs_fflush(file);
}

//==============================================================================
/**
 * @brief Function tests the end-of-file indicator.
 *
 * The function tests the end-of-file indicator for the stream
 * pointed to by <i>file</i>. The end-of-file
 * indicator can only be cleared by the function sys_clearerr().
 *
 * @note Function can be used only by file system or driver code.
 *
 * @param file          stream
 * @param eof           pointer to EOF indicator
 *
 * @return One of @ref errno value.
 *
 * @b Example
 * @code
        // ...

        FILE *file = NULL;
        if (sys_fopen("/foo/bar", "w+", &file) == ESUCC) {
               // file operations...

               int eof = 0;
               if (sys_feof(file) == ESUCC && eof) {
                       // end-of-file handling
               }

               // file operations...

               sys_fclose(file);
        }

        // ...
   @endcode
 *
 * @see sys_clearerr()
 */
//==============================================================================
static inline int sys_feof(FILE *file, int *eof)
{
        return _vfs_feof(file, eof);
}

//==============================================================================
/**
 * @brief Function clears end-of-file and error indicators.
 *
 * The function clears the end-of-file and error indicators
 * for the stream pointed to by <i>file</i>.
 *
 * @note Function can be used only by file system or driver code.
 *
 * @param file          stream
 *
 * @return One of @ref errno value.
 *
 * @b Example
 * @code
        // ...

        FILE *file = NULL;
        if (sys_fopen("/foo/bar", "w+", &file) == ESUCC) {
               // file operations...

               int error = 0;
               if (sys_ferror(file) == ESUCC && error) {
                       sys_clearerr();
               }

               // file operations...

               sys_fclose(file);
        }

        // ...
   @endcode
 *
 * @see sys_feof(), sys_ferror()
 */
//==============================================================================
static inline int sys_clearerr(FILE *file)
{
        return _vfs_clearerr(file);
}

//==============================================================================
/**
 * @brief Function tests error indicator.
 *
 * The function tests the error indicator for the stream pointed
 * to by <i>file</i>, returning nonzero if it is set.  The error indicator can
 * be reset only by the sys_clearerr() function.
 *
 * @note Function can be used only by file system or driver code.
 *
 * @param file          stream
 * @param error         pointer to error indicator
 *
 * @return One of @ref errno value.
 *
 * @b Example
 * @code
        // ...

        FILE *file = NULL;
        if (sys_fopen("/foo/bar", "w+", &file) == ESUCC) {
               // file operations...

               int error = 0;
               if (sys_ferror(file) == ESUCC && error) {
                       sys_clearerr();
               }

               // file operations...

               sys_fclose(file);
        }

        // ...
   @endcode
 *
 * @see sys_clearerr()
 */
//==============================================================================
static inline int sys_ferror(FILE *file, int *error)
{
        return _vfs_ferror(file, error);
}

//==============================================================================
/**
 * @brief Function sets file position indicator to the beginning of file.
 *
 * The function sets the file position indicator for the stream
 * pointed to by <i>file</i> to the beginning of the file. It is equivalent to:
 * <pre>sys_fseek(stream, 0L, SEEK_SET)</pre>
 *
 * @note Function can be used only by file system or driver code.
 *
 * @param file          stream
 *
 * @return One of @ref errno value.
 *
 * @b Example
 * @code
        // ...

        FILE *file = NULL;
        if (sys_fopen("/foo/bar", "w+", &file) == ESUCC) {
               // ...

               sys_rewind(file);

               char buf[10];

               size_t rdcnt = 0;
               int err = sys_fread(buf, sizeof(buf), &rdcnt, file);

               // ...

               sys_fclose(file);
        } else {
               // ...
        }

        // ...
   @endcode
 *
 * @see sys_fseek()
 */
//==============================================================================
static inline int sys_rewind(FILE *file)
{
        return _vfs_fseek(file, 0, VFS_SEEK_SET);
}

//==============================================================================
/**
 * @brief Function synchronizes files buffers with file systems.
 *
 * @note Function can be used only by file system or driver code.
 *
 * @b Example
 * @code
        // ...

        sys_sync();

        // ...
   @endcode
 */
//==============================================================================
static inline void sys_sync(void)
{
        _vfs_sync();
}

//==============================================================================
/**
 * @brief Function put message to kernel log.
 *
 * @note Function can be used only by file system or driver code.
 *
 * The function produce output according to a <i>format</i> as described below.
 * The function write output to <b>kernel log buffer</b>.
 *
 * <b>%</b> - The flag starts interpreting of formatting. After character can be type a
 * numbers which determine e.g. buffer length, number of digits, etc. When
 * next character is <i>%</i> then per cent is printed.
 *
 * Supported flags:@n
 *   %%         - print % character
 *                @code printf("%%"); //=> % @endcode
 *
 *   %c         - print selected character (the \0 character is skipped)@n
 *                @code printf("_%c_", 'x');  //=> _x_ @endcode
 *                @code printf("_%c_", '\0'); //=> __ @endcode
 *
 *   %s         - print selected string
 *                @code printf("%s", "Foobar"); //=> Foobar @endcode
 *
 *   %.*s       - print selected string but only the length passed by argument
 *                @code printf("%.*s\n", 3, "Foobar"); //=> Foo @endcode
 *
 *   %.ns       - print selected string but only the n length
 *                @code printf("%.3s\n", "Foobar"); //=> Foo @endcode
 *
 *   %d, %i     - print decimal integer values
 *                @code printf("%d, %i", -5, 10); //=> -5, 10 @endcode
 *
 *   %u         - print unsigned decimal integer values
 *                @code printf("%u, %u", -1, 10); //=> 4294967295, 10 @endcode
 *
 *   %x, %X     - print hexadecimal values ('x' for lower characters, 'X' for upper characters)
 *                @code printf("0x%x, 0x%X", 0x5A, 0xfa); //=> 0x5a, 0xFA @endcode
 *
 *   %0x?       - print decimal (d, i, u) or hex (x, X) values with leading zeros.
 *                The number of characters (at least) is determined by x. The ?
 *                means d, i, u, x, or X value representations.
 *                @code printf("0x02X, 0x03X", 0x5, 0x1F43); //=> 0x05, 0x1F43 % @endcode
 *
 *   %f         - print float number. Note: make sure that input value is the float!
 *                @code printf("Foobar: %f", 1.0); //=> Foobar: 1.000000 @endcode
 *
 *   %l?        - print long long values, where ? means d, i, u, x, or X.
 *                @note Flag not supported.
 *
 *   %p         - print pointer
 *                @code printf("Pointer: %p", main); //=> Pointer: 0x4028B4 @endcode
 *
 * @note Function can be used only by file system or driver code.
 *
 * @param format        formatting string
 * @param ...           argument sequence
 *
 * @return None
 *
 * @b Example
 * @code
        // ...

        int foo = 12;
        int bar = 0x12;
        sys_printk("foo is %d; bar is 0x%x\n", foo, bar);

        // ...
   @endcode
 *
 * @see sys_vsnprintf(), sys_snprintf(), sys_vfprintf(), sys_fprintf()
 */
//==============================================================================
#ifndef DOXYGEN
#define printk(...) _printk(__VA_ARGS__)
#else
static inline void printk(const char *format, ...);
#endif

//==============================================================================
/**
 * @brief Function prints message according to format to buffer.
 *
 * The function produce output according to a <i>format</i> as described below.
 * The function write output pointed to by <i>buf</i> of size <i>size</i>.
 * An arguments are passed by list <i>args</i>.<p>
 *
 * Supported flags:@n
 *   %%         - print % character
 *                @code printf("%%"); //=> % @endcode
 *
 *   %c         - print selected character (the \0 character is skipped)@n
 *                @code printf("_%c_", 'x');  //=> _x_ @endcode
 *                @code printf("_%c_", '\0'); //=> __ @endcode
 *
 *   %s         - print selected string
 *                @code printf("%s", "Foobar"); //=> Foobar @endcode
 *
 *   %.*s       - print selected string but only the length passed by argument
 *                @code printf("%.*s\n", 3, "Foobar"); //=> Foo @endcode
 *
 *   %.ns       - print selected string but only the n length
 *                @code printf("%.3s\n", "Foobar"); //=> Foo @endcode
 *
 *   %d, %i     - print decimal integer values
 *                @code printf("%d, %i", -5, 10); //=> -5, 10 @endcode
 *
 *   %u         - print unsigned decimal integer values
 *                @code printf("%u, %u", -1, 10); //=> 4294967295, 10 @endcode
 *
 *   %x, %X     - print hexadecimal values ('x' for lower characters, 'X' for upper characters)
 *                @code printf("0x%x, 0x%X", 0x5A, 0xfa); //=> 0x5a, 0xFA @endcode
 *
 *   %0x?       - print decimal (d, i, u) or hex (x, X) values with leading zeros.
 *                The number of characters (at least) is determined by x. The ?
 *                means d, i, u, x, or X value representations.
 *                @code printf("0x02X, 0x03X", 0x5, 0x1F43); //=> 0x05, 0x1F43 % @endcode
 *
 *   %f         - print float number. Note: make sure that input value is the float!
 *                @code printf("Foobar: %f", 1.0); //=> Foobar: 1.000000 @endcode
 *
 *   %l?        - print long long values, where ? means d, i, u, x, or X.
 *                @note Flag not supported.
 *
 *   %p         - print pointer
 *                @code printf("Pointer: %p", main); //=> Pointer: 0x4028B4 @endcode
 *
 * @note Function can be used only by file system or driver code.
 *
 * @param buf           buffer which output was produced
 * @param size          buffer size
 * @param format        formatting string
 * @param args          argument list
 *
 * @return Upon successful return, these functions return the number of
 * characters printed (excluding the null byte used to end output to strings).
 * If an output error is encountered, a negative value is returned.
 *
 * @b Example
 * @code
        // ...

        va_list args;

        // ...

        char buffer[20];
        sys_vsnprintf(buffer, 20, "foo is %d; bar is 0x%x\n", args);

        // ...
   @endcode
 *
 * @see sys_snprintf(), sys_vfprintf(), sys_fprintf()
 */
//==============================================================================
static inline int sys_vsnprintf(char *buf, size_t size, const char *format, va_list args)
{
        return _vsnprintf(buf, size, format, args);
}

//==============================================================================
/**
 * @brief Function prints message according to format to buffer.
 *
 * The function produce output according to a <i>format</i> as described below.
 * The function write output pointed to by <i>bfr</i> of size <i>size</i>.<p>
 *
 * Supported flags:@n
 *   %%         - print % character
 *                @code printf("%%"); //=> % @endcode
 *
 *   %c         - print selected character (the \0 character is skipped)@n
 *                @code printf("_%c_", 'x');  //=> _x_ @endcode
 *                @code printf("_%c_", '\0'); //=> __ @endcode
 *
 *   %s         - print selected string
 *                @code printf("%s", "Foobar"); //=> Foobar @endcode
 *
 *   %.*s       - print selected string but only the length passed by argument
 *                @code printf("%.*s\n", 3, "Foobar"); //=> Foo @endcode
 *
 *   %.ns       - print selected string but only the n length
 *                @code printf("%.3s\n", "Foobar"); //=> Foo @endcode
 *
 *   %d, %i     - print decimal integer values
 *                @code printf("%d, %i", -5, 10); //=> -5, 10 @endcode
 *
 *   %u         - print unsigned decimal integer values
 *                @code printf("%u, %u", -1, 10); //=> 4294967295, 10 @endcode
 *
 *   %x, %X     - print hexadecimal values ('x' for lower characters, 'X' for upper characters)
 *                @code printf("0x%x, 0x%X", 0x5A, 0xfa); //=> 0x5a, 0xFA @endcode
 *
 *   %0x?       - print decimal (d, i, u) or hex (x, X) values with leading zeros.
 *                The number of characters (at least) is determined by x. The ?
 *                means d, i, u, x, or X value representations.
 *                @code printf("0x02X, 0x03X", 0x5, 0x1F43); //=> 0x05, 0x1F43 % @endcode
 *
 *   %f         - print float number. Note: make sure that input value is the float!
 *                @code printf("Foobar: %f", 1.0); //=> Foobar: 1.000000 @endcode
 *
 *   %l?        - print long long values, where ? means d, i, u, x, or X.
 *                @note Flag not supported.
 *
 *   %p         - print pointer
 *                @code printf("Pointer: %p", main); //=> Pointer: 0x4028B4 @endcode
 *
 * @note Function can be used only by file system or driver code.
 *
 * @param bfr           buffer where output was produced
 * @param size          buffer size
 * @param format        formatting string
 * @param ...           argument sequence
 *
 * @return Upon successful return, these functions return the number of
 * characters printed (excluding the null byte used to end output to strings).
 * If an output error is encountered, a negative value is returned.
 *
 * @b Example
 * @code
        // ...

        char buffer[20];
        int foo = 12;
        int bar = 0x12;
        sys_snprintf(buffer, 20, "foo is %d; bar is 0x%x\n", foo, bar);

        // ...
   @endcode
 *
 * @see sys_vsnprintf(), sys_vfprintf(), sys_fprintf()
 */
//==============================================================================
static inline int sys_snprintf(char *bfr, size_t size, const char *format, ...)
{
        va_list arg;
        va_start(arg, format);
        int r = _vsnprintf(bfr, size, format, arg);
        va_end(arg);
        return r;
}

//==============================================================================
/**
 * @brief Function prints message according to format to selected stream.
 *
 * The function produce output according to a <i>format</i> as described below.
 * The function write output to <i>file</i>.
 * An arguments are passed by list <i>args</i>.<p>
 *
 * Supported flags:@n
 *   %%         - print % character
 *                @code printf("%%"); //=> % @endcode
 *
 *   %c         - print selected character (the \0 character is skipped)@n
 *                @code printf("_%c_", 'x');  //=> _x_ @endcode
 *                @code printf("_%c_", '\0'); //=> __ @endcode
 *
 *   %s         - print selected string
 *                @code printf("%s", "Foobar"); //=> Foobar @endcode
 *
 *   %.*s       - print selected string but only the length passed by argument
 *                @code printf("%.*s\n", 3, "Foobar"); //=> Foo @endcode
 *
 *   %.ns       - print selected string but only the n length
 *                @code printf("%.3s\n", "Foobar"); //=> Foo @endcode
 *
 *   %d, %i     - print decimal integer values
 *                @code printf("%d, %i", -5, 10); //=> -5, 10 @endcode
 *
 *   %u         - print unsigned decimal integer values
 *                @code printf("%u, %u", -1, 10); //=> 4294967295, 10 @endcode
 *
 *   %x, %X     - print hexadecimal values ('x' for lower characters, 'X' for upper characters)
 *                @code printf("0x%x, 0x%X", 0x5A, 0xfa); //=> 0x5a, 0xFA @endcode
 *
 *   %0x?       - print decimal (d, i, u) or hex (x, X) values with leading zeros.
 *                The number of characters (at least) is determined by x. The ?
 *                means d, i, u, x, or X value representations.
 *                @code printf("0x02X, 0x03X", 0x5, 0x1F43); //=> 0x05, 0x1F43 % @endcode
 *
 *   %f         - print float number. Note: make sure that input value is the float!
 *                @code printf("Foobar: %f", 1.0); //=> Foobar: 1.000000 @endcode
 *
 *   %l?        - print long long values, where ? means d, i, u, x, or X.
 *                @note Flag not supported.
 *
 *   %p         - print pointer
 *                @code printf("Pointer: %p", main); //=> Pointer: 0x4028B4 @endcode
 *
 * @note Function can be used only by file system or driver code.
 *
 * @param file          output stream
 * @param format        formatting string
 * @param args          argument list
 *
 * @return Upon successful return, these functions return the number of
 * characters printed (excluding the null byte used to end output to strings).
 * If an output error is encountered, a negative value is returned.
 *
 * @b Example
 * @code
        // ...

        va_list arg;
        FILE   *file;

        // ...

        sys_vfprintf(file, "foo is %d; bar is 0x%x\n", arg);

        // ...
   @endcode
 *
 * @see sys_vsnprintf(), sys_snprintf(), sys_fprintf()
 */
//==============================================================================
static inline int sys_vfprintf(FILE *file, const char *format, va_list args)
{
        return _vfprintf(file, format, args);
}

//==============================================================================
/**
 * @brief Function prints message according to format to selected stream.
 *
 * The function produce output according to a <i>format</i> as described below.
 * The function write output to <i>file</i>.<p>
 *
 * Supported flags:@n
 *   %%         - print % character
 *                @code printf("%%"); //=> % @endcode
 *
 *   %c         - print selected character (the \0 character is skipped)@n
 *                @code printf("_%c_", 'x');  //=> _x_ @endcode
 *                @code printf("_%c_", '\0'); //=> __ @endcode
 *
 *   %s         - print selected string
 *                @code printf("%s", "Foobar"); //=> Foobar @endcode
 *
 *   %.*s       - print selected string but only the length passed by argument
 *                @code printf("%.*s\n", 3, "Foobar"); //=> Foo @endcode
 *
 *   %.ns       - print selected string but only the n length
 *                @code printf("%.3s\n", "Foobar"); //=> Foo @endcode
 *
 *   %d, %i     - print decimal integer values
 *                @code printf("%d, %i", -5, 10); //=> -5, 10 @endcode
 *
 *   %u         - print unsigned decimal integer values
 *                @code printf("%u, %u", -1, 10); //=> 4294967295, 10 @endcode
 *
 *   %x, %X     - print hexadecimal values ('x' for lower characters, 'X' for upper characters)
 *                @code printf("0x%x, 0x%X", 0x5A, 0xfa); //=> 0x5a, 0xFA @endcode
 *
 *   %0x?       - print decimal (d, i, u) or hex (x, X) values with leading zeros.
 *                The number of characters (at least) is determined by x. The ?
 *                means d, i, u, x, or X value representations.
 *                @code printf("0x02X, 0x03X", 0x5, 0x1F43); //=> 0x05, 0x1F43 % @endcode
 *
 *   %f         - print float number. Note: make sure that input value is the float!
 *                @code printf("Foobar: %f", 1.0); //=> Foobar: 1.000000 @endcode
 *
 *   %l?        - print long long values, where ? means d, i, u, x, or X.
 *                @note Flag not supported.
 *
 *   %p         - print pointer
 *                @code printf("Pointer: %p", main); //=> Pointer: 0x4028B4 @endcode
 *
 * @note Function can be used only by file system or driver code.
 *
 * @param file          output stream
 * @param format        formatting string
 * @param ...           argument sequence
 *
 * @return Upon successful return, these functions return the number of
 * characters printed (excluding the null byte used to end output to strings).
 * If an output error is encountered, a negative value is returned.
 *
 * @b Example
 * @code
        // ...

        FILE *file;

        int foo = 12;
        int bar = 0x12;
        sys_fprintf(file, "foo is %d; bar is 0x%x\n", foo, bar);

        // ...
   @endcode
 *
 * @see sys_vsnprintf(), sys_snprintf(), sys_vfprintf()
 */
//==============================================================================
static inline int sys_fprintf(FILE *file, const char *format, ...)
{
        va_list arg;
        va_start(arg, format);
        int r = _vfprintf(file, format, arg);
        va_end(arg);
        return r;
}

//==============================================================================
/**
 * @brief Function scans input according to format.
 *
 * The function scans input according to format as described below. This format
 * may contain conversion specifications; the results from such conversions,
 * if any, are stored in the locations pointed to by the pointer arguments that
 * follow format. Each pointer argument must be of a type that is appropriate
 * for the value returned by the corresponding conversion specification.<p>
 *
 * <b>%</b> - The flag starts interpreting of formatting. After character can be type a
 * numbers which determine e.g. buffer length, number of digits, etc.<p>
 *
 * <b>c</b> - Scans single character.<p>
 *
 * <b>s</b> - Scans string.<p>
 *
 * <b>d, i</b> - Scans signed <i>int</i> type number.<p>
 *
 * <b>u</b> - Scans unsigned type number.<p>
 *
 * <b>x, X</b> - Scans value in HEX formatting.<p>
 *
 * <b>o</b> - Scans value in Octal formatting.<p>
 *
 * <b>f, F, g, G</b> - Scans float value.
 *
 * @note Function can be used only by file system or driver code.
 *
 * @param str           input string (must be <i>null</i> terminated)
 * @param format        formatting string
 * @param ...           argument sequence list
 *
 * @return The function return the number of input items successfully matched
 * and assigned, which can be fewer than provided for, or even zero in the event
 * of an early matching failure.<p>
 *
 * The value @ref EOF is returned if the end of input is reached before either
 * the first successful conversion or a matching failure occurs. @ref EOF is
 * also returned if a read error occurs, in which case the error indicator for
 * the stream is set.
 *
 * @b Example
 * @code
        // ...

        char *buffer = "12, 1256";
        int foo, bar;
        sys_sscanf(buffer, "%i, %i", &foo, &bar);

        // ...
   @endcode
 *
 * @see sys_vsscanf()
 */
//==============================================================================
static inline int sys_sscanf(const char *str, const char *format, ...)
{
        va_list arg;
        va_start(arg, format);
        int n = _vsscanf(str, format, arg);
        va_end(arg);
        return n;
}

//==============================================================================
/**
 * @brief Function scans input according to format.
 *
 * The function scans input according to format as described below. This format
 * may contain conversion specifications; the results from such conversions,
 * if any, are stored in the locations pointed to by the pointer arguments that
 * follow format. Each pointer argument must be of a type that is appropriate
 * for the value returned by the corresponding conversion specification.
 * An arguments are passed by list <i>args</i>.<p>
 *
 * <b>%</b> - The flag starts interpreting of formatting. After character can be type a
 * numbers which determine e.g. buffer length, number of digits, etc.<p>
 *
 * <b>c</b> - Scans single character.<p>
 *
 * <b>s</b> - Scans string.<p>
 *
 * <b>d, i</b> - Scans signed <i>int</i> type number.<p>
 *
 * <b>u</b> - Scans unsigned type number.<p>
 *
 * <b>x, X</b> - Scans value in HEX formatting.<p>
 *
 * <b>o</b> - Scans value in Octal formatting.<p>
 *
 * <b>f, F, g, G</b> - Scans float value.
 *
 * @note Function can be used only by file system or driver code.
 *
 * @param str           input string (must be <i>null</i> terminated)
 * @param format        formatting string
 * @param args          argument sequence list
 *
 * @return The function return the number of input items successfully matched
 * and assigned, which can be fewer than provided for, or even zero in the event
 * of an early matching failure.<p>
 *
 * The value @ref EOF is returned if the end of input is reached before either
 * the first successful conversion or a matching failure occurs. @ref EOF is
 * also returned if a read error occurs, in which case the error indicator for
 * the stream is set.
 *
 * @b Example
 * @code
        // ...

        char *buffer = "12, 1256";
        va_list arg;
        sys_vsscanf("%i, %i", arg);

        // ...
   @endcode
 *
 * @see sys_sscanf()
 */
//==============================================================================
static inline int sys_vsscanf(const char *str, const char *format, va_list args)
{
        return _vsscanf(str, format, args);
}

//==============================================================================
/**
 * @brief Function gets time reference.
 *
 * @note Function can be used only by file system or driver code.
 *
 * @return System timer value.
 *
 * @b Example
 * @code
        // ...

        u64_t tref = sys_time_get_reference();

        while (!sys_time_is_expired(tref, 2000)) {
                // ...
        }

        // ...
   @endcode
 *
 * @see sys_time_is_expired(), sys_time_set_expired(), sys_time_diff()
 */
//==============================================================================
static inline u64_t sys_time_get_reference(void)
{
        return _kernel_get_time_ms();
}

//==============================================================================
/**
 * @brief Check if time expired.
 *
 * @note Function can be used only by file system or driver code.
 *
 * @param time_ref      time reference
 * @param time          time to check
 *
 * @return If time expired then @b true is returned, otherwise @b false.
 *
 * @b Example
 * @code
        // ...

        u64_t tref = sys_time_get_reference();

        while (!sys_time_is_expired(tref, 2000)) {
                // ...
        }

        // ...
   @endcode
 *
 * @see sys_time_get_reference(), sys_time_set_expired(), sys_time_diff()
 */
//==============================================================================
static inline bool sys_time_is_expired(u64_t time_ref, u64_t time)
{
        return (_kernel_get_time_ms() - time_ref >= time);
}

//==============================================================================
/**
 * @brief Set time reference as expired.
 *
 * @note Function can be used only by file system or driver code.
 *
 * @return Expired time value.
 *
 * @b Example
 * @code
        // ...

        u64_t tref = sys_time_set_expired();

        while (!sys_time_is_expired(tref, 2000)) {
                // this code will not be executed because time expired
                // ...
        }

        // ...
   @endcode
 *
 * @see sys_time_get_reference(), sys_time_is_expired(), sys_time_diff()
 */
//==============================================================================
static inline u64_t sys_time_set_expired(void)
{
        return UINT64_MAX;
}

//==============================================================================
/**
 * @brief Calculate difference between <i>time1</i> and <i>time2</i>.
 *
 * @note Function can be used only by file system or driver code.
 *
 * @param time1        time reference 1
 * @param time2        time reference 2
 *
 * @return Returns difference between timer1 and timer2 (in ticks).
 *
 * @see sys_time_get_reference(), sys_time_set_expired(), sys_time_set_expired()
 */
//==============================================================================
static inline int sys_time_diff(u32_t time1, u32_t time2)
{
        return time1 - time2;
}

//==============================================================================
/**
 * @brief Function creates binary semaphore.
 *
 * @note Function can be used only by file system or driver code.
 *
 * @param cnt_max          max count value (1 for binary)
 * @param cnt_init         initial value (0 or 1 for binary)
 * @param sem              created semaphore handle
 *
 * @return One of @ref errno value.
 *
 * @b Example
 * @code
        // ...

        sem_t *sem = NULL;
        if (sys_semaphore_create(1, 0, &sem) == ESUCC) {

                // ...

                sys_semaphore_destroy(sem);
        }

        // ...
   @endcode
 *
 * @see sys_semaphore_destroy()
 */
//==============================================================================
extern int sys_semaphore_create(const size_t cnt_max, const size_t cnt_init, sem_t **sem);

//==============================================================================
/**
 * @brief Function deletes semaphore.
 *
 * @note Function can be used only by file system or driver code.
 *
 * @param sem       semaphore object
 *
 * @return One of @ref errno value.
 *
 * @b Example
 * @code
        // ...

        sem_t *sem = NULL;
        if (sys_semaphore_create(1, 0, &sem) == ESUCC) {

                // ...

                int err = sys_semaphore_destroy(sem);
        }

        // ...
   @endcode
 *
 * @see sys_semaphore_create()
 */
//==============================================================================
extern int sys_semaphore_destroy(sem_t *sem);

//==============================================================================
/**
 * @brief Function wait for semaphore.
 *
 * The function waits for semaphore signal pointed by
 * <i>sem</i> by <i>timeout</i> milliseconds. If semaphore was signaled then
 * ESUCC is returned, otherwise (timeout) ETIME. When <i>timeout</i>
 * value is set to 0 then semaphore is polling without timeout.
 *
 * @note Function can be used only by file system or driver code.
 *
 * @param sem           semaphore object pointer
 * @param timeout       timeout value in milliseconds
 *
 * @return One of @ref errno value.
 *
 * @b Example
 * @code
        // ...

        void thread2(void *arg)
        {
                while (true) {
                        // this task will wait for semaphore signal
                        sys_semaphore_wait(sem, MAX_DELAY_MS);

                        // ...
                }

                // ...
        }

        void thread1(void *arg)
        {
                while (true) {
                       // ...

                       // this task signal to thread2 that can execute part of code
                       sys_semaphore_signal(sem);
                }

                // ...
        }

        // ...

   @endcode
 *
 * @see sys_semaphore_signal()
 */
//==============================================================================
static inline int sys_semaphore_wait(sem_t *sem, const u32_t timeout)
{
        return _semaphore_wait(sem, timeout);
}

//==============================================================================
/**
 * @brief Function get counter value of semaphore.
 *
 * The function get value of semaphore pointed by <i>sem</i>. The counter value
 * is modified by sys_semaphore_wait() and sys_semaphore_signal() family functions.
 *
 * @note Function can be used only by file system or driver code.
 *
 * @param sem           semaphore object pointer
 * @param value         counter value
 *
 * @return One of @ref errno value.
 *
 * @b Example
 * @code
        // ...

        size_t value = 0;

        int err = sys_semaphore_get_value(sem, &value);
        if (!err && value > 0) {
                // ...
        }

        // ...

   @endcode
 *
 * @see sys_semaphore_signal(), sys_semaphore_wait()
 */
//==============================================================================
static inline int sys_semaphore_get_value(sem_t *sem, size_t *value)
{
        return _semaphore_get_value(sem, value);
}

//==============================================================================
/**
 * @brief Function signal semaphore.
 *
 * The function signals semaphore pointed by <i>sem</i>.
 *
 * @note Function can be used only by file system or driver code.
 *
 * @param sem           semaphore object pointer
 *
 * @return One of @ref errno value.
 *
 * @b Example
 * @code
        // ...

        void thread2(void *arg)
        {
                while (true) {
                        // this task will wait for semaphore signal
                        sys_semaphore_wait(sem, MAX_DELAY_MS);

                        // ...
                }

                // ...
        }

        void thread1(void *arg)
        {
                while (true) {
                       // ...

                       // this task signal to thread2 that can execute part of code
                       sys_semaphore_signal(sem);
                }

                // ...
        }

        // ...

   @endcode
 *
 * @see sys_semaphore_wait()
 */
//==============================================================================
static inline int sys_semaphore_signal(sem_t *sem)
{
        return _semaphore_signal(sem);
}

//==============================================================================
/**
 * @brief Function wait for semaphore from ISR.
 *
 * @note Function can be used only by file system or driver code.
 *
 * @param sem              semaphore object
 * @param task_woken       true if higher priority task woken, otherwise false (can be @ref NULL)
 *
 * @return One of @ref errno value.
 *
 * @b Example
 * @code
        // ...

        void ISR(void)
        {
                bool woken = false;
                if (sys_semaphore_wait_from_ISR(sem, &woken) == ESUCC) {
                        // ...
                } else {
                        // ...
                }

                sys_thread_yield_from_ISR(woken);
        }

        // ...

   @endcode
 *
 * @see sys_semaphore_signal_from_ISR()
 */
//==============================================================================
static inline int sys_semaphore_wait_from_ISR(sem_t *sem, bool *task_woken)
{
        return _semaphore_wait_from_ISR(sem, task_woken);
}

//==============================================================================
/**
 * @brief Function signal semaphore from ISR.
 *
 * @note Function can be used only by file system or driver code.
 *
 * @param sem              semaphore object
 * @param task_woken       true if higher priority task woken, otherwise false (can be @ref NULL)
 *
 * @return One of @ref errno value.
 *
 * @b Example
 * @code
        // ...

        void thread(void *arg)
        {
                while (true) {
                        // this task will wait for semaphore signal
                        sys_semaphore_wait(sem, MAX_DELAY_MS);

                        // ...
                }

                // ...
        }

        void ISR(void)
        {
               // ...

               bool woken = false;
               sys_semaphore_signal_from_ISR(sem, &woken);

               // ...

               if (woken) {
                       sys_thread_yield_from_ISR();
               }
        }

        // ...

   @endcode
 *
 * @see sys_semaphore_wait_from_ISR()
 */
//==============================================================================
static inline int sys_semaphore_signal_from_ISR(sem_t *sem, bool *task_woken)
{
        return _semaphore_signal_from_ISR(sem, task_woken);
}

//==============================================================================
/**
 * @brief Function create new mutex.
 *
 * @note Function can be used only by file system or driver code.
 *
 * @param type     mutex type
 * @param mtx      created mutex handle
 *
 * @return One of @ref errno value.
 *
 * @b Example
 * @code
        // ...

        // resource that be accessed from many threads
        int resource;


        // create mutex instance
        mutex_t *mtx = NULL;
        int      err = sys_mutex_create(MUTEX_TYPE_NORMAL, &mtx);
        if (err != ESUCC) {
                return err;
        }

        // ...


        void thread1()
        {
                // protected access to resource
                if (sys_mutex_lock(mtx, MAX_DELAY_MS) == ESUCC) {
                        // write to buffer is allowed
                        resource = ...;

                        // ...

                        sys_mutex_unlock(mtx);
                }
        }

        void thread2()
        {
                // protected access to resource
                if (sys_mutex_lock(mtx, MAX_DELAY_MS) == ESUCC) {
                        // write to buffer is allowed
                        resource = ...;

                        // ...

                        sys_mutex_unlock(mtx);
                }
        }


        // ...

        // destroy created mutex (if not used anymore)
        if (sys_mutex_destroy(mtx) == ESUCC) {
                // ...
        } else {
                // ...
        }

        // ...

   @endcode
 *
 * @see sys_mutex_destroy()
 */
//==============================================================================
extern int sys_mutex_create(enum mutex_type type, mutex_t **mtx);

//==============================================================================
/**
 * @brief Function creates flags. Flag is similar to semaphore except single
 *        object contains more than one synchronization point.
 *
 * @note Function can be used only by file system or driver code.
 *
 * @param flag            created flag handle
 *
 * @return One of @ref errno value.
 *
 * @b Example
 * @code
        // ...

        flag_t *flag = NULL;
        if (sys_flag_create(&flag) == ESUCC) {

                // ...

                sys_flag_destroy(flag);
        }

        // ...
   @endcode
 *
 * @see sys_flag_destroy()
 */
//==============================================================================
extern int sys_flag_create(flag_t **flag);

//==============================================================================
/**
 * @brief Function deletes flag.
 *
 * @note Function can be used only by file system or driver code.
 *
 * @param flag      flag object
 *
 * @return One of @ref errno value.
 *
 * @b Example
 * @code
        // ...

        flag_t *flag = NULL;
        if (sys_flag_create(&flag) == ESUCC) {

                // ...

                sys_flag_destroy(flag);
        }

        // ...
   @endcode
 *
 * @see sys_flag_create()
 */
//==============================================================================
extern int sys_flag_destroy(flag_t *flag);

//==============================================================================
/**
 * @brief Function wait for selected flags.
 *
 * @note Function can be used only by file system or driver code.
 *
 * @param flag          flag object
 * @param bits          bits to wait for (logical AND)
 * @param blocktime_ms  timeout in milliseconds
 *
 * @return One of @ref errno value.
 *
 * @b Example
 * @code
        // ...

        flag_t *flag = NULL;
        if (sys_flag_create(&flag) == ESUCC) {

                // ...

                sys_flag_wait(flag, (1<<0), 100);

                // ...

                sys_flag_destroy(flag);
        }

        // ...

        ISR()
        {
                sys_flag_set_from_ISR(flag, (1<<0));
        }
   @endcode
 *
 * @see sys_flag_set(), sys_flag_clear()
 */
//==============================================================================
static inline int sys_flag_wait(flag_t *flag, u32_t bits, const u32_t blocktime_ms)
{
        return _flag_wait(flag, bits, blocktime_ms);
}

//==============================================================================
/**
 * @brief Function set selected flags.
 *
 * @note Function can be used only by file system or driver code.
 *
 * @param flag          flag object
 * @param bits          bits to set
 *
 * @return One of @ref errno value.
 *
 * @b Example
 * @code
        // ...

        flag_t *flag = NULL;
        if (sys_flag_create(&flag) == ESUCC) {

                // ...

                sys_flag_wait(flag, (1<<0), 100);

                // ...

                sys_flag_destroy(flag);
        }

        // ...

        ISR()
        {
                sys_flag_set_from_ISR(flag, (1<<0));
        }
   @endcode
 *
 * @see sys_flag_clear()
 */
//==============================================================================
static inline int sys_flag_set(flag_t *flag, u32_t bits)
{
        return _flag_set(flag, bits);
}

//==============================================================================
/**
 * @brief Function clear selected flags.
 *
 * @note Function can be used only by file system or driver code.
 *
 * @param flag          flag object
 * @param bits          bits to clear
 *
 * @return One of @ref errno value.
 *
 * @b Example
 * @code
        // ...

        flag_t *flag = NULL;
        if (sys_flag_create(&flag) == ESUCC) {

                // ...

                sys_flag_wait(flag, (1<<0), 100);

                // ...

                sys_flag_destroy(flag);
        }

        // ...

        ISR()
        {
                sys_flag_set_from_ISR(flag, (1<<0));
        }
   @endcode
 *
 * @see sys_flag_set()
 */
//==============================================================================
static inline int sys_flag_clear(flag_t *flag, u32_t bits)
{
        return _flag_clear(flag, bits);
}

//==============================================================================
/**
 * @brief Function get flags.
 *
 * @note Function can be used only by file system or driver code.
 *
 * @param flag          flag object
 *
 * @b Example
 * @code
        // ...

        flag_t *flag = NULL;
        if (sys_flag_create(&flag) == ESUCC) {

                // ...

                sys_flag_wait(flag, (1<<0), 100);

                // ...

                sys_flag_destroy(flag);
        }

        // ...

        ISR()
        {
                sys_flag_set_from_ISR(flag, (1<<0));
        }
   @endcode
 *
 * @see sys_flag_get_from_ISR()
 */
//==============================================================================
static inline u32_t sys_flag_get(flag_t *flag)
{
        return _flag_get(flag);
}

//==============================================================================
/**
 * @brief Function get flags from interrupt.
 *
 * @note Function can be used only by file system or driver code.
 *
 * @param flag          flag object
 *
 * @b Example
 * @code
        // ...

        flag_t *flag = NULL;
        if (sys_flag_create(&flag) == ESUCC) {

                // ...

                sys_flag_wait(flag, (1<<0), 100);

                // ...

                sys_flag_destroy(flag);
        }

        // ...

        ISR()
        {
                sys_flag_set_from_ISR(flag, (1<<0));
        }
   @endcode
 *
 * @see sys_flag_get()
 */
//==============================================================================
static inline u32_t sys_flag_get_from_ISR(flag_t *flag)
{
        return _flag_get_from_ISR(flag);
}

//==============================================================================
/**
 * @brief Function destroy mutex.
 *
 * @note Function can be used only by file system or driver code.
 *
 * @param mutex    mutex object
 *
 * @return One of @ref errno value.
 *
 * @b Example
 * @code
        // ...

        // resource that be accessed from many threads
        int resource;


        // create mutex instance
        mutex_t *mtx = NULL;
        int      err = sys_mutex_create(MUTEX_TYPE_NORMAL, &mtx);
        if (err != ESUCC) {
                return err;
        }

        // ...


        void thread1()
        {
                // protected access to resource
                if (sys_mutex_lock(mtx, MAX_DELAY_MS) == ESUCC) {
                        // write to buffer is allowed
                        resource = ...;

                        // ...

                        sys_mutex_unlock(mtx);
                }
        }

        void thread2()
        {
                // protected access to resource
                if (sys_mutex_lock(mtx, MAX_DELAY_MS) == ESUCC) {
                        // write to buffer is allowed
                        resource = ...;

                        // ...

                        sys_mutex_unlock(mtx);
                }
        }


        // ...

        // destroy created mutex (if not used anymore)
        if (sys_mutex_destroy(mtx) == ESUCC) {
                // ...
        } else {
                // ...
        }

        // ...

   @endcode
 *
 * @see sys_mutex_create()
 */
//==============================================================================
extern int sys_mutex_destroy(mutex_t *mutex);

//==============================================================================
/**
 * @brief Function lock mutex.
 *
 * @note Function can be used only by file system or driver code.
 *
 * @param mutex             mutex object
 * @param timeout           polling time
 *
 * @return One of @ref errno value.
 *
 * @b Example
 * @code
        // ...

        // resource that be accessed from many threads
        int resource;


        // create mutex instance
        mutex_t *mtx = NULL;
        int      err = sys_mutex_create(MUTEX_TYPE_NORMAL, &mtx);
        if (err != ESUCC) {
                return err;
        }

        // ...


        void thread1()
        {
                // protected access to resource
                if (sys_mutex_lock(mtx, MAX_DELAY_MS) == ESUCC) {
                        // write to buffer is allowed
                        resource = ...;

                        // ...

                        sys_mutex_unlock(mtx);
                }
        }

        void thread2()
        {
                // protected access to resource
                if (sys_mutex_lock(mtx, MAX_DELAY_MS) == ESUCC) {
                        // write to buffer is allowed
                        resource = ...;

                        // ...

                        sys_mutex_unlock(mtx);
                }
        }


        // ...

        // destroy created mutex (if not used anymore)
        if (sys_mutex_destroy(mtx) == ESUCC) {
                // ...
        } else {
                // ...
        }

        // ...

   @endcode
 *
 * @see sys_mutex_trylock(), sys_mutex_unlock()
 */
//==============================================================================
static inline int sys_mutex_lock(mutex_t *mutex, const u32_t timeout)
{
        return _mutex_lock(mutex, timeout);
}

//==============================================================================
/**
 * @brief Function try lock mutex.
 *
 * The function try to lock mutex. If mutex is free then is immediately locked,
 * if not then error is returned. Function is equivalent to:
 * sys_mutex_lock(mtx, 0) call.
 *
 * @note Function can be used only by file system or driver code.
 *
 * @param mutex             mutex object
 *
 * @return One of @ref errno value.
 *
 * @b Example
 * @code
        // ...

        // resource that be accessed from many threads
        int resource;


        // create mutex instance
        mutex_t *mtx = NULL;
        int      err = sys_mutex_create(MUTEX_TYPE_NORMAL, &mtx);
        if (err != ESUCC) {
                return err;
        }

        // ...


        void thread1()
        {
                // protected access to resource
                if (sys_mutex_trylock(mtx) == ESUCC) {
                        // write to buffer is allowed
                        resource = ...;

                        // ...

                        sys_mutex_unlock(mtx);
                }
        }

        void thread2()
        {
                // protected access to resource
                if (sys_mutex_trylock(mtx) == ESUCC) {
                        // write to buffer is allowed
                        resource = ...;

                        // ...

                        sys_mutex_unlock(mtx);
                }
        }


        // ...

        // destroy created mutex (if not used anymore)
        if (sys_mutex_destroy(mtx) == ESUCC) {
                // ...
        } else {
                // ...
        }

        // ...

   @endcode
 *
 * @see sys_mutex_lock(), sys_mutex_unlock()
 */
//==============================================================================
static inline int sys_mutex_trylock(mutex_t *mutex)
{
        return _mutex_lock(mutex, 0);
}

//==============================================================================
/**
 * @brief Function unlock mutex.
 *
 * @note Function can be used only by file system or driver code.
 *
 * @param *mutex            mutex object
 *
 * @return One of @ref errno value.
 *
 * @b Example
 * @code
        // ...

        // resource that be accessed from many threads
        int resource;


        // create mutex instance
        mutex_t *mtx = NULL;
        int      err = sys_mutex_create(MUTEX_TYPE_NORMAL, &mtx);
        if (err != ESUCC) {
                return err;
        }

        // ...


        void thread1()
        {
                // protected access to resource
                if (sys_mutex_lock(mtx, MAX_DELAY_MS) == ESUCC) {
                        // write to buffer is allowed
                        resource = ...;

                        // ...

                        sys_mutex_unlock(mtx);
                }
        }

        void thread2()
        {
                // protected access to resource
                if (sys_mutex_lock(mtx, MAX_DELAY_MS) == ESUCC) {
                        // write to buffer is allowed
                        resource = ...;

                        // ...

                        sys_mutex_unlock(mtx);
                }
        }


        // ...

        // destroy created mutex (if not used anymore)
        if (sys_mutex_destroy(mtx) == ESUCC) {
                // ...
        } else {
                // ...
        }

        // ...

   @endcode
 *
 * @see sys_mutex_trylock(), sys_mutex_lock()
 */
//==============================================================================
static inline int sys_mutex_unlock(mutex_t *mutex)
{
        return _mutex_unlock(mutex);
}

//==============================================================================
/**
 * @brief Function create new queue.
 *
 * @note Function can be used only by file system or driver code.
 *
 * @param length           queue length
 * @param item_size        queue item size
 * @param queue            created queue
 *
 * @return One of @ref errno value.
 */
//==============================================================================
extern int sys_queue_create(const size_t length, const size_t item_size, queue_t **queue);

//==============================================================================
/**
 * @brief Function delete queue.
 *
 * @note Function can be used only by file system or driver code.
 *
 * @param *queue            queue object
 *
 * @return One of @ref errno value.
 */
//==============================================================================
extern int sys_queue_destroy(queue_t *queue);

//==============================================================================
/**
 * @brief Function reset queue.
 *
 * @note Function can be used only by file system or driver code.
 *
 * @param *queue            queue object
 *
 * @return One of @ref errno value.
 */
//==============================================================================
static inline int sys_queue_reset(queue_t *queue)
{
        return _queue_reset(queue);
}

//==============================================================================
/**
 * @brief Function send queue.
 *
 * @note Function can be used only by file system or driver code.
 *
 * @param queue            queue object
 * @param item             item
 * @param waittime_ms      wait time
 *
 * @return One of @ref errno value.
 */
//==============================================================================
static inline int sys_queue_send(queue_t *queue, const void *item, const u32_t waittime_ms)
{
        return _queue_send(queue, item, waittime_ms);
}

//==============================================================================
/**
 * @brief Function send queue.
 *
 * @note Function can be used only by file system or driver code.
 *
 * @param *queue            queue object
 * @param *item             item
 * @param *task_woken       true if higher priority task woken, otherwise false (can be @ref NULL)
 *
 * @return One of @ref errno value.
 */
//==============================================================================
static inline int sys_queue_send_from_ISR(queue_t *queue, const void *item, bool *task_woken)
{
        return _queue_send_from_ISR(queue, item, task_woken);
}

//==============================================================================
/**
 * @brief Function send queue.
 *
 * @note Function can be used only by file system or driver code.
 *
 * @param queue            queue object
 * @param item             item
 * @param waittime_ms      wait time
 *
 * @return One of @ref errno value.
 */
//==============================================================================
static inline int sys_queue_receive(queue_t *queue, void *item, const u32_t waittime_ms)
{
        return _queue_receive(queue, item, waittime_ms);
}

//==============================================================================
/**
 * @brief Function receive queue from ISR.
 *
 * @note Function can be used only by file system or driver code.
 *
 * @param queue            queue object
 * @param item             item
 * @param task_woken       true if higher priority task woke, otherwise false (can be @ref NULL)
 *
 * @return One of @ref errno value.
 */
//==============================================================================
static inline int sys_queue_receive_from_ISR(queue_t *queue, void *item, bool *task_woken)
{
        return _queue_receive_from_ISR(queue, item, task_woken);
}

//==============================================================================
/**
 * @brief Function receive item from the top of the queue and not delete it.
 *
 * @note Function can be used only by file system or driver code.
 *
 * @param queue            queue object
 * @param item             item
 * @param waittime_ms      wait time
 *
 * @return One of @ref errno value.
 */
//==============================================================================
static inline int sys_queue_receive_peek(queue_t *queue, void *item, const u32_t waittime_ms)
{
        return _queue_receive_peek(queue, item, waittime_ms);
}

//==============================================================================
/**
 * @brief Function gets number of items in queue.
 *
 * @note Function can be used only by file system or driver code.
 *
 * @param queue            queue object
 * @param items            number of items in queue
 *
 * @return One of @ref errno value.
 */
//==============================================================================
static inline int sys_queue_get_number_of_items(queue_t *queue, size_t *items)
{
        return _queue_get_number_of_items(queue, items);
}

//==============================================================================
/**
 * @brief Function gets number of items in queue from ISR.
 *
 * @note Function can be used only by file system or driver code.
 *
 * @param queue            queue object
 * @param items            number of items in queue
 *
 * @return One of @ref errno value.
 */
//==============================================================================
static inline int sys_queue_get_number_of_items_from_ISR(queue_t *queue, size_t *items)
{
        return _queue_get_number_of_items_from_ISR(queue, items);
}

//==============================================================================
/**
 * @brief Function gets number of free items in queue.
 *
 * @note Function can be used only by file system or driver code.
 *
 * @param queue            queue object
 * @param items            number of items in queue
 *
 * @return One of @ref errno value.
 */
//==============================================================================
static inline int sys_queue_get_space_available(queue_t *queue, size_t *items)
{
        return _queue_get_space_available(queue, items);
}

//==============================================================================
/**
 * @brief  Function return free memory in bytes.
 *
 * @note Function can be used only by file system or driver code.
 *
 * @return Free memory value.
 *
 * @see sys_get_used_mem(), sys_get_mem_size()
 */
//==============================================================================
static inline size_t sys_get_free_mem(void)
{
        return _mm_get_mem_free();
}

//==============================================================================
/**
 * @brief  Function return used memory in bytes.
 *
 * @note Function can be used only by file system or driver code.
 *
 * @return Used memory value.
 *
 * @see sys_get_free_mem(), sys_get_mem_size()
 */
//==============================================================================
static inline size_t sys_get_used_mem(void)
{
        return _mm_get_mem_usage();
}

//==============================================================================
/**
 * @brief  Function return memory size (RAM) in bytes.
 *
 * @note Function can be used only by file system or driver code.
 *
 * @return Memory size.
 *
 * @see sys_get_free_mem(), sys_get_used_mem()
 */
//==============================================================================
static inline size_t sys_get_mem_size(void)
{
        return _mm_get_mem_size();
}

//==============================================================================
/**
 * @brief Function return OS time in milliseconds.
 *
 * @note Function can be used only by file system or driver code.
 *
 * @return OS time in milliseconds.
 *
 * @see sys_get_tick_counter()
 */
//==============================================================================
static inline u64_t sys_get_uptime_ms(void)
{
        return _kernel_get_time_ms();
}

//==============================================================================
/**
 * @brief Function return tick counter.
 *
 * The tick counter is incremented every context switch interrupt. If context switch
 * frequency is set to 1000Hz then counter is incremented every 1ms. To get value
 * of system time in milliseconds use sys_get_time_ms() function.
 *
 * @note Function can be used only by file system or driver code.
 *
 * @return Tick counter value.
 *
 * @see sys_gettime_ms(), sys_sleep_until(), sys_sleep_until_ms()
 */
//==============================================================================
static inline u64_t sys_get_tick_counter(void)
{
        return _kernel_get_tick_counter();
}

//==============================================================================
/**
 * @brief Function return a number of tasks.
 *
 * Task is the basic unit of CPU time. Each process has at least 1 task (called
 * main thread), each additional thread is a new task.
 *
 * @note Function can be used only by file system or driver code.
 *
 * @return Number of tasks.
 */
//==============================================================================
static inline int sys_get_number_of_tasks(void)
{
        return _kernel_get_number_of_tasks();
}

//==============================================================================
/**
 * @brief  Function return collected process statistics.
 *
 * @note Function can be used only by file system or driver code.
 *
 * @param  pid      PID
 * @param  stat     process statistics
 *
 * @return One of @ref errno value.
 *
 * @see sys_process_get_stat_seek()
 */
//==============================================================================
static inline int sys_process_get_stat_pid(pid_t pid, process_stat_t *stat)
{
        return _process_get_stat_pid(pid, stat);
}

//==============================================================================
/**
 * @brief  Function return collected process statistics
 *
 * @note Function can be used only by file system or driver code.
 *
 * @param  seek     process seek (start from 0)
 * @param  stat     process statistics
 *
 * @return One of @ref errno value.
 *
 * @see sys_process_get_count(), sys_process_get_stat_pid()
 */
//==============================================================================
static inline int sys_process_get_stat_seek(size_t seek, process_stat_t *stat)
{
        return _process_get_stat_seek(seek, stat);
}

//==============================================================================
/**
 * @brief  Function return number of processes.
 *
 * @note Function can be used only by file system or driver code.
 *
 * @return Number of processes.
 */
//==============================================================================
static inline size_t sys_process_get_count(void)
{
        return _process_get_count();
}

//==============================================================================
/**
 * @brief Function create new thread (task), and if enabled, add to monitor list.
 *
 * Function by default allocate memory for task data (localized in task tag)
 * which is used to CPU load calculation/ standard IO, and etc.
 *
 * @note Function can be used only by file system or driver code.
 *
 * @param func             thread code
 * @param attr             thread attributes
 * @param arg              thread argument
 * @param tid              thread ID
 *
 * @return One of @ref errno value.
 */
//==============================================================================
extern int sys_thread_create(thread_func_t func, const thread_attr_t *attr, void *arg, tid_t *tid);

//==============================================================================
/**
 * @brief Function delete thread.
 *
 * Function clear all allocated resources by thread and remove it from execution
 * list.
 *
 * @note Function can be used only by file system or driver code.
 *
 * @param tid        thread ID
 *
 * @return One of @ref errno value.
 */
//==============================================================================
extern int sys_thread_destroy(tid_t tid);

//==============================================================================
/**
 * @brief  Function checks if selected thread handle is valid.
 *
 * @note Function can be used only by file system or driver code.
 *
 * @param  tid       thread ID
 *
 * @return If thread object is valid the @b true is returned otherwise @b false.
 */
//==============================================================================
static inline bool sys_thread_is_valid(tid_t tid)
{
        return ((tid >= 1) && (tid < __OS_TASK_MAX_SYSTEM_THREADS__));
}

//==============================================================================
/**
 * @brief Function suspend selected thread.
 *
 * @note Function can be used only by file system or driver code.
 *
 * @param  tid       thread ID
 */
//==============================================================================
static inline void sys_thread_suspend(tid_t tid)
{
        if (sys_thread_is_valid(tid)) {
                task_t *task = _process_thread_get_task(_kworker_proc, tid);
                if (task) {
                        _task_suspend(task);
                }
        }
}

//==============================================================================
/**
 * @brief Function suspend current thread.
 *
 * @note Function can be used only by file system or driver code.
 */
//==============================================================================
static inline void sys_thread_suspend_now(void)
{
        _task_suspend(_THIS_TASK);
}

//==============================================================================
/**
 * @brief Function resume selected thread.
 *
 * @note Function can be used only by file system or driver code.
 *
 * @param  tid       thread ID
 */
//==============================================================================
static inline void sys_thread_resume(tid_t tid)
{
        if (sys_thread_is_valid(tid)) {
                task_t *task = _process_thread_get_task(_kworker_proc, tid);
                if (task) {
                        _task_resume(task);
                }
        }
}

//==============================================================================
/**
 * @brief Function resume selected thread from ISR.
 *
 * @note Function can be used only by file system or driver code.
 *
 * @param tid           thread ID
 * @param task_woken    true if higher priority task woke, otherwise false (can be @ref NULL)
 *
 * @return One of @ref errno value.
 */
//==============================================================================
static inline int sys_thread_resume_from_ISR(tid_t tid, bool *task_woken)
{
        if (sys_thread_is_valid(tid)) {
                task_t *task = _process_thread_get_task(_kworker_proc, tid);
                if (task) {
                        bool woken = _task_resume_from_ISR(task);
                        if (task_woken) {
                                *task_woken = woken;
                        }

                        return ESUCC;
                }

                return EINVAL;
        } else {
                return ESRCH;
        }
}

//==============================================================================
/**
 * @brief Function yield thread.
 *
 * @note Function can be used only by file system or driver code.
 */
//==============================================================================
static inline void sys_thread_yield(void)
{
        _task_yield();
}

//==============================================================================
/**
 * @brief Function return thread object information.
 *
 * @note Function can be used only by file system or driver code.
 *
 * @param tid           thread ID
 *
 * @return One of @ref errno value.
 */
//==============================================================================
static inline void sys_thread_self(tid_t *tid)
{
        _task_get_process_container(_THIS_TASK, NULL, tid);
}

//==============================================================================
/**
 * @brief Function yield thread from ISR.
 *
 * @note Function can be used only by file system or driver code.
 *
 * @param yield         thread yield
 */
//==============================================================================
static inline void sys_thread_yield_from_ISR(bool yield)
{
        _task_yield_from_ISR(yield);
}

//==============================================================================
/**
 * @brief Function set priority of current thread.
 *
 * @note Function can be used only by file system or driver code.
 *
 * @param priority         priority
 */
//==============================================================================
static inline void sys_thread_set_priority(const int priority)
{
        _task_set_priority(_THIS_TASK, priority);
}

//==============================================================================
/**
 * @brief Function return priority of current thread.
 *
 * @note Function can be used only by file system or driver code.
 *
 * @return Thread priority.
 */
//==============================================================================
static inline int sys_thread_get_priority(void)
{
        return _task_get_priority(_THIS_TASK);
}

//==============================================================================
/**
 * @brief Function return a free stack level of current thread.
 *
 * @note Function can be used only by file system or driver code.
 *
 * @return Free stack level.
 */
//==============================================================================
static inline int sys_thread_get_free_stack(void)
{
        return _task_get_free_stack(_THIS_TASK);
}

//==============================================================================
/**
 * @brief Function enter to critical section.
 *
 * @note Function can be used only by file system or driver code.
 *
 * @see sys_critical_section_end()
 */
//==============================================================================
static inline void sys_critical_section_begin(void)
{
        _critical_section_begin();
}

//==============================================================================
/**
 * @brief Function exit from critical section.
 *
 * @note Function can be used only by file system or driver code.
 *
 * @see sys_critical_section_begin()
 */
//==============================================================================
static inline void sys_critical_section_end(void)
{
        _critical_section_end();
}

//==============================================================================
/**
 * @brief Function disable interrupts.
 *
 * @note Function can be used only by file system or driver code.
 *
 * @see sys_ISR_enable()
 */
//==============================================================================
static inline void sys_ISR_disable(void)
{
        _ISR_disable();
}

//==============================================================================
/**
 * @brief Function enable interrupts.
 *
 * @note Function can be used only by file system or driver code.
 *
 * @see sys_ISR_disable()
 */
//==============================================================================
static inline void sys_ISR_enable(void)
{
        _ISR_enable();
}

//==============================================================================
/**
 * @brief  Function lock context switch.
 *
 * @note Function can be used only by file system or driver code.
 *
 * @see sys_context_switch_unlock()
 */
//==============================================================================
static inline void sys_context_switch_lock(void)
{
        _kernel_scheduler_lock();
}

//==============================================================================
/**
 * @brief  Function unlock context switch.
 *
 * @note Function can be used only by file system or driver code.
 *
 * @see sys_context_switch_lock()
 */
//==============================================================================
static inline void sys_context_switch_unlock(void)
{
        _kernel_scheduler_unlock();
}

//==============================================================================
/**
 * @brief Function put to sleep thread for microseconds.
 *
 * @note Function can sleep longer that declared because of context switch
 *       settings.
 *
 * @note Function work in critical section.
 *
 * @note Function can be used only by file system or driver code.
 *
 * @param microseconds          number of microseconds of sleep
 *
 * @see sys_sleep(), sys_sleep_until(), sys_sleep_until_ms()
 */
//==============================================================================
static inline void sys_sleep_us(const u32_t microseconds)
{
        _cpuctl_delay_us(microseconds);
}

//==============================================================================
/**
 * @brief Function put to sleep thread for milliseconds.
 *
 * @note Function can sleep longer that declared because of context switch
 *       settings. Context switch can be longer than 1ms.
 *
 * @note Function can be used only by file system or driver code.
 *
 * @param milliseconds          number of milliseconds of sleep
 *
 * @see sys_sleep(), sys_sleep_until(), sys_sleep_until_ms()
 */
//==============================================================================
static inline void sys_sleep_ms(const u32_t milliseconds)
{
        _sleep_ms(milliseconds);
}

//==============================================================================
/**
 * @brief Function put to sleep thread for seconds.
 *
 * @note Function can be used only by file system or driver code.
 *
 * @param seconds               number of seconds of sleep
 *
 * @see sys_sleep_ms(), sys_sleep_until(), sys_sleep_until_ms()
 */
//==============================================================================
static inline void sys_sleep(const u32_t seconds)
{
        _sleep(seconds);
}

//==============================================================================
/**
 * @brief Function sleep thread in regular periods (reference argument).
 *
 * @note Function can be used only by file system or driver code.
 *
 * @param milliseconds          milliseconds
 * @param ref_time_ticks        reference time in OS ticks
 *
 * @b Example
 * @code
        // ...

        u32_t tick_ref = sys_get_tick_counter();

        // ...

        sys_sleep_until_ms(250, &tick_ref);

        // ...
   @endcode
 *
 * @see sys_get_tick_counter(), sys_sleep_ms(), sys_sleep_until(), sys_sleep()
 */
//==============================================================================
static inline void sys_sleep_until_ms(const u32_t milliseconds, u32_t *ref_time_ticks)
{
        _sleep_until_ms(milliseconds, ref_time_ticks);
}

//==============================================================================
/**
 * @brief Function sleep task in regular periods (reference argument)
 *
 * @note Function can be used only by file system or driver code.
 *
 * @param seconds               seconds
 * @param ref_time_ticks        reference time in OS ticks
 *
 * @b Example
 * @code
        // ...

        u32_t tick_ref = sys_get_tick_counter();

        // ...

        sys_sleep_until(2, &tick_ref);

        // ...
   @endcode
 *
 * @see sys_get_tick_counter(), sys_sleep(), sys_sleep_until_ms(), sys_sleep_ms()
 */
//==============================================================================
static inline void sys_sleep_until(const u32_t seconds, u32_t *ref_time_ticks)
{
        _sleep_until(seconds, ref_time_ticks);
}

//==============================================================================
/**
 * @brief Function update all system clock after CPU frequency change.
 *
 * Function shall update all devices which base on main clock oscillator.
 * Function is called after clock/frequency change from clock management driver
 * (e.g. PLL).
 *
 * @note Function can be used only by file system or driver code.
 */
//==============================================================================
static inline void sys_update_system_clocks(void)
{
        _cpuctl_update_system_clocks();
}

//==============================================================================
/**
 * @brief  Convert tm structure to time_t.
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
 * @note Function can be used only by file system or driver code.
 *
 * @param  timeptr      Pointer to a tm structure that contains a calendar time
 *                      broken down into its components (see struct tm)
 *
 * @return A time_t value corresponding to the calendar time passed as argument.
 *         If the calendar time cannot be represented, a value of -1 is returned.
 *
 * @see sys_gettime()
 */
//==============================================================================
static inline time_t sys_mktime(struct tm *timeptr)
{
#if __OS_ENABLE_TIMEMAN__ == _YES_
        return _mktime(timeptr);
#else
        UNUSED_ARG1(timeptr);
        return _kernel_get_time_ms() / 1000;
#endif
}

//==============================================================================
/**
 * @brief  Get current time.
 *
 * The function returns this value, and if the argument is not a @ref NULL pointer,
 * it also sets this value to the object pointed by timer.
 * The value returned generally represents the number of seconds since 00:00
 * hours, Jan 1, 1970 UTC (i.e., the current unix timestamp). Although libraries
 * may use a different representation of time: Portable programs should not use
 * the value returned by this function directly, but always rely on calls to
 * other elements of the standard library to translate them to portable types
 * (such as localtime, gmtime or difftime).
 *
 * @note Function can be used only by file system or driver code.
 *
 * @param  timer        Pointer to an object of type time_t, where the time
 *                      value is stored.
 *                      Alternatively, this parameter can be a @ref NULL pointer,
 *                      in which case the parameter is not used (the function
 *                      still returns a value of type time_t with the result).
 *
 * @return One of @ref errno value.
 *
 * @see sys_settime()
 */
//==============================================================================
static inline int sys_gettime(time_t *timer)
{
#if __OS_ENABLE_TIMEMAN__ == _YES_
        struct timeval timeval;
        int err = _gettime(&timeval);
        if (!err) {
                if (timer) {
                        *timer = timeval.tv_sec;
                } else {
                        err = EINVAL;
                }
        }

        return err;
#else
        UNUSED_ARG1(timer);
        return ENOTSUP;
#endif
}

//==============================================================================
/**
 * @brief  Set system's time.
 *
 * stime() sets the system's idea of the time and date. The time, pointed to by
 * timer, is measured in seconds since the Epoch, 1970-01-01 00:00:00 +0000 (UTC).
 *
 * @note Function can be used only by file system or driver code.
 *
 * @param  timer        pointer to an object of type time_t, where the time
 *                      value is stored.
 *
 * @return One of @ref errno value.
 *
 * @see sys_gettime()
 */
//==============================================================================
static inline int sys_settime(time_t *timer)
{
        return _settime(timer);
}

//==============================================================================
/**
 * @brief  Convert time_t to tm as UTC time.
 *
 * Uses the value pointed by timer to fill a tm structure with the values that
 * represent the corresponding time, expressed as a UTC time (i.e., the time
 * at the GMT timezone).
 *
 * @note Function can be used only by file system or driver code.
 *
 * @param timer    Pointer to an object of type time_t that contains a time value.
 *                      time_t is an alias of a fundamental arithmetic type
 *                      capable of representing times as returned by function time.
 *
 * @param tm       Pointer to an object of type struct tm that will contains
 *                      converted timer value to time structure.
 *
 * @return A pointer to a tm structure with its members filled with the values
 *         that correspond to the UTC time representation of timer.
 *
 * @see sys_localtime_r()
 */
//==============================================================================
static inline struct tm *sys_gmtime_r(const time_t *timer, struct tm *tm)
{
#if __OS_ENABLE_TIMEMAN__ == _YES_
        return _gmtime_r(timer, tm);
#else
        UNUSED_ARG1(timer);
        memset(tm, 0, sizeof(struct tm));
        return tm;
#endif
}

//==============================================================================
/**
 * @brief  Convert time_t to tm as local time.
 *
 * Uses the value pointed by timer to fill a tm structure with the values that
 * represent the corresponding time, expressed for the local timezone.
 *
 * @note Function can be used only by file system or driver code.
 *
 * @param timer    Pointer to an object of type time_t that contains a time value.
 *                      time_t is an alias of a fundamental arithmetic type
 *                      capable of representing times as returned by function time.
 *
 * @param tm       Pointer to an object of type struct tm that will contains
 *                      converted timer value to time structure.
 *
 * @return A pointer to a tm structure with its members filled with the values
 *         that correspond to the local time representation of timer.
 *
 * @see sys_gmtime_r()
 */
//==============================================================================
static inline struct tm *sys_localtime_r(const time_t *timer, struct tm *tm)
{
#if __OS_ENABLE_TIMEMAN__ == _YES_
        return _localtime_r(timer, tm);
#else
        UNUSED_ARG1(timer);
        return tm;
#endif
}

//==============================================================================
/**
 * @brief  Function register new memory region. The region object should be
 *         visible during entire system runtime. There is no possibility to
 *         remove added region.
 *
 * @note Function can be used only by driver code.
 *
 * @param  region       region object (initialized by system)
 * @param  start        region start address
 * @param  size         region size
 *
 * @return One of errno value.
 *
 * @b Example
 * @code
        // ...

        mem_region_t ram2;

        int err = sys_memory_register(&ram2, 0x20001000, 16384);
        if (!err) {
                // ...
        }

        // ...
   @endcode
 *
 */
//==============================================================================
static inline int sys_memory_register(mem_region_t *region, void *start, size_t size)
{
        return _mm_register_region(region, start, size);
}

//==============================================================================
/**
 * @brief  Function return integer value from given configuration.
 *
 * Function found expression: VAR=VAL. If variable is not found then <i>defval</i>
 * is returned.
 *
 * @param opts          options to analyze
 * @param var           variable to search
 * @param defval        value used if variable does not found
 *
 * @return Integer (on error or when variable does not found the default value).
 */
//==============================================================================
static inline int sys_stropt_get_int(const char *opts, const char *var, int defval)
{
        return _stropt_get_int(opts, var, defval);
}

//==============================================================================
/**
 * @brief  Function return string from given configuration.
 *
 * Function found expression: VAR=STR.
 *
 * @param opts          options to analyze
 * @param var           variable to search
 * @param len           string length
 *
 * @return On success string pointer and length passed by len pointer, otherwise NULL.
 */
//==============================================================================
static inline const char *sys_stropt_get_string_ref(const char *opts, const char *var, size_t *len)
{
        return _stropt_get_string_ref(opts, var, len);
}

//==============================================================================
/**
 * @brief  Function copy string from option.
 *
 * Function found expression: VAR=STR.
 *
 * @param opts          options to analyze
 * @param var           variable to search
 * @param buf           destination buffer
 * @param buflen        destination buffer length
 *
 * @return On success string pointer and length passed by len pointer, otherwise NULL.
 */
//==============================================================================
static inline size_t sys_stropt_get_string_copy(const char *opts, const char *var, char *buf, size_t buflen)
{
        return _stropt_get_string_copy(opts, var, buf, buflen);
}

//==============================================================================
/**
 * @brief  Function return integer value from given configuration.
 *
 * Function found expression: VAR=BOOL(false, true, on, off, enable, disable, yes, no).
 * If none of given values is found, then function return <i>defval</i> defined
 * by user.
 *
 * @param opts          options to analyze
 * @param var           variable to search
 * @param defval        default value
 *
 * @return On success return 1 or 0
 */
//==============================================================================
static inline int sys_stropt_get_bool(const char *opts, const char *var, int defval)
{
        return _stropt_get_bool(opts, var, defval);
}

//==============================================================================
/**
 * @brief  Function check if flag exist.
 *
 * @param opts          options to analyze
 * @param flag          flag to find
 *
 * @return On success return true, otherwise false.
 */
//==============================================================================
static inline bool sys_stropt_is_flag(const char *opts, const char *flag)
{
        return _stropt_is_flag(opts, flag);
}

//==============================================================================
/**
 * @brief Function find driver name and then initialize device
 *
 * @note Function can be used only by driver code.
 *
 * @param [IN]  module              module name
 * @param [IN]  major               major number
 * @param [IN]  minor               minor number
 * @param [IN]  config              module configuration object (optional, can be NULL)
 * @param [OUT] id                  module id (can be NULL)
 *
 * @return One of error code (errno)
 */
//==============================================================================
static inline int sys_driver_init(const char *module, u8_t major, u8_t minor, const void *config, dev_t *id)
{
        return _driver_init(module, major, minor, NULL, config, id);
}

//==============================================================================
/**
 * @brief Function release selected device by using device ID
 *
 * @note Function can be used only by driver code.
 *
 * @param id                   device id
 *
 * @return One of error code (errno)
 */
//==============================================================================
static inline int sys_driver_release(dev_t id)
{
        return _driver_release(id);
}

//==============================================================================
/**
 * @brief Function open selected driver
 *
 * @note Function can be used only by file system or driver code.
 *
 * @param id            module id
 * @param flags         flags
 *
 * @return One of @ref errno value.
 */
//==============================================================================
static inline int sys_driver_open(dev_t id, u32_t flags)
{
        return _driver_open(id, flags);
}

//==============================================================================
/**
 * @brief Function close selected driver
 *
 * @note Function can be used only by file system or driver code.
 *
 * @param id            module id
 * @param force         force close request
 *
 * @return One of @ref errno value.
 */
//==============================================================================
static inline int sys_driver_close(dev_t id, bool force)
{
        return _driver_close(id, force);
}

//==============================================================================
/**
 * @brief Function write data to driver
 *
 * @note Function can be used only by file system or driver code.
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
static inline int sys_driver_write(dev_t            id,
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
 * @note Function can be used only by file system or driver code.
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
static inline int sys_driver_read(dev_t            id,
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
 * @note Function can be used only by file system or driver code.
 *
 * @param id            module id
 * @param request       io request
 * @param arg           argument
 *
 * @return One of @ref errno value.
 */
//==============================================================================
static inline int sys_driver_ioctl(dev_t id, int request, void *arg)
{
        return _driver_ioctl(id, request, arg);
}

//==============================================================================
/**
 * @brief Flush device buffer (forces write)
 *
 * @note Function can be used only by file system or driver code.
 *
 * @param id            module id
 *
 * @return One of @ref errno value.
 */
//==============================================================================
static inline int sys_driver_flush(dev_t id)
{
        return _driver_flush(id);
}

//==============================================================================
/**
 * @brief Device information
 *
 * @note Function can be used only by file system or driver code.
 *
 * @param id            module id
 * @param stat          status object
 *
 * @return One of @ref errno value.
 */
//==============================================================================
static inline int sys_driver_stat(dev_t id, struct vfs_dev_stat *stat)
{
        return _driver_stat(id, stat);
}

#ifdef __cplusplus
}
#endif

#endif /* _SYSFUNC_H_ */
/**@}*/
/*==============================================================================
  End of file
==============================================================================*/
