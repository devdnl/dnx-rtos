/*=========================================================================*//**
@file    dirent.h

@author  Daniel Zorychta

@brief

@note    Copyright (C) 2014 Daniel Zorychta <daniel.zorychta@gmail.com>

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

#ifndef _DIRENT_H_
#define _DIRENT_H_

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Include files
==============================================================================*/
#include "core/vfs.h"

/*==============================================================================
  Exported macros
==============================================================================*/

/*==============================================================================
  Exported object types
==============================================================================*/
#ifndef __DIR_TYPE_DEFINED__
typedef struct vfs_dir DIR;
#endif

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
 * @brief DIR *opendir(const char *name)
 * Function opens a directory stream corresponding to the directory <i>name</i>, and
 * returns a pointer to the directory stream. The stream is positioned at the first
 * entry in the directory.
 *
 * @param name         directory path
 *
 * @errors EINVAL, ENOENT, ...
 *
 * @return A pointer to the directory stream. On error, <b>NULL</b> is returned,
 * and <b>errno</b> is set appropriately.
 *
 * @example
 * // ...
 * DIR *dir = opendir("/foo");
 * if (dir) {
 *         // directory handling
 *         dirclose(dir);
 * }
 */
//==============================================================================
static inline DIR *opendir(const char *name)
{
        return _sysm_opendir(name);
}

//==============================================================================
/**
 * @brief int closedir(DIR *dir)
 * Function closes the directory stream associated with <i>dir</i>. The directory
 * stream descriptor <i>dir</i> is not available after this call.
 *
 * @param dir           pinter to directory object
 *
 * @errors EINVAL, ENOENT, ...
 *
 * @return Return 0 on success. On error, -1 is returned, and <b>errno</b> is set appropriately.
 *
 * @example
 * // ...
 *
 * DIR *dir = opendir("/foo");
 * if (dir) {
 *         // ...
 *         closedir(dir);
 * }
 *
 * // ...
 */
//==============================================================================
static inline int closedir(DIR *dir)
{
        return _sysm_closedir(dir);
}

//==============================================================================
/**
 * @brief dirent_t readdir(DIR *dir)
 * Function returns a object <b>dirent_t</b> type representing the next directory
 * entry in the directory stream pointed to by <i>dir</i>.<p>
 *
 * <b>dirent_t</b> structure:
 * <pre>
 * typedef struct dirent {
 *         char   *name;
 *         u64_t   size;
 *         tfile_t filetype;
 *         dev_t   dev;
 * } dirent_t;
 * </pre>
 *
 * @param[in] dir       directory object
 *
 * @errors EINVAL, ENOENT, ...
 *
 * @return On success, readdir() returns a pointer to a <b>dirent_t</b> type. If
 * the end of the directory stream is reached, field <b>name</b> of <b>dirent_t</b>
 * type is <b>NULL</b>. If an error occurs, NULL-object and <b>errno</b> is set
 * appropriately.
 *
 * @example
 * // ...
 *
 * DIR *dir = opendir(path);
 * if (dir) {
 *         errno = 0;
 *         dirent_t dirent = readdir(dir);
 *         while (dirent.name != NULL) {
 *                 // ...
 *         }
 *
 *         closedir(dir);
 * }
 *
 * // ...
 */
//==============================================================================
static inline dirent_t readdir(DIR *dir)
{
        return _vfs_readdir(dir);
}

#ifdef __cplusplus
}
#endif

#endif /* _DIRENT_H_ */
/*==============================================================================
  End of file
==============================================================================*/
