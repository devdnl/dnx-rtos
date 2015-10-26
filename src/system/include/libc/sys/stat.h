/*=========================================================================*//**
@file    stat.h

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

#ifndef _STAT_H_
#define _STAT_H_

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Include files
==============================================================================*/
#include <sys/types.h>
#include <kernel/syscall.h>

/*==============================================================================
  Exported macros
==============================================================================*/

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
 * @brief int mknod(const char *pathname, dev_t dev)
 * The <b>mknod</b>() function creates a filesystem node (device special file) named
 * <i>pathname</i>, with attributes specified by mode and <i>dev</i>. <i>dev</i>
 * determines the number of driver to use. This depends on system configuration.
 *
 * @param pathname      node name
 * @param mod_name      module name
 * @param major         driver major number
 * @param minor         driver minor number
 *
 * @errors EINVAL, ENOMEM, ...
 *
 * @return On success, 0 is returned. On error, -1 is returned, and <b>errno</b>
 * is set appropriately.
 *
 * @example
 * // ...
 *
 * // for example: 12 is crc driver
 * mknod("/dev/crc", 12);
 *
 * // ...
 */
//==============================================================================
static inline int mknod(const char *pathname, const char *mod_name, int major, int minor)
{
        int r;
        syscall(SYSCALL_MKNOD, &r, pathname, mod_name, &major, &minor);
        return r;
}

//==============================================================================
/**
 * @brief int mkdir(const char *pathname, mode_t mode)
 * The <b>mkdir</b>() attempts to create a directory named <i>pathname</i>. The
 * argument <i>mode</i> specifies the permissions to use.
 *
 * @param pathname      directory name
 * @param mode          directory permissions
 *
 * @errors EINVAL, ENOMEM, EACCES, EEXIST, ENOENT, ENOSPC, ...
 *
 * @return On success, 0 is returned. On error, -1 is returned, and <b>errno</b>
 * is set appropriately.
 *
 * @example
 * // ...
 *
 * mkdir("/dev", 0666);
 *
 * // ...
 */
//==============================================================================
static inline int mkdir(const char *pathname, mode_t mode)
{
        int r;
        syscall(SYSCALL_MKDIR, &r, pathname, &mode);
        return r;
}

//==============================================================================
/**
 * @brief int mkfifo(const char *pathname, mode_t mode)
 * The <b>mkfifo</b>() makes a FIFO special file with name <i>pathname</i>. <i>mode</i>
 * specifies the FIFO's permissions. A FIFO special file is similar to pipe, but
 * is created in filesystem and is not an anonymous. Access to FIFO is the same
 * as to regular file, except that data can be read only one time. Not all
 * filesystems support this file type.
 *
 * @param pathname      FIFO name
 * @param mode          FIFO permissions
 *
 * @errors EINVAL, ENOMEM, EACCES, EEXIST, ENOENT, ENOSPC, ...
 *
 * @return On success, 0 is returned. On error, -1 is returned, and <b>errno</b>
 * is set appropriately.
 *
 * @example
 * // ...
 *
 * mkfifo("/dev/my_fifo", 0666);
 *
 * // ...
 */
//==============================================================================
static inline int mkfifo(const char *pathname, mode_t mode)
{
        int r;
        syscall(SYSCALL_MKFIFO, &r, pathname, &mode);
        return r;
}

//==============================================================================
/**
 * @brief int chmod(const char *pathname, mode_t mode)
 * The <b>chmod</b>() system call change the permissions of a file.
 *
 * @param pathname      file to permission change
 * @param mode          new permissions
 *
 * @errors EINVAL, ENOMEM, EACCES, ENOENT, ...
 *
 * @return On success, 0 is returned. On error, -1 is returned, and <b>errno</b>
 * is set appropriately.
 *
 * @example
 * // ...
 *
 * chmod("/dev/foo", 0666);
 *
 * // ...
 */
//==============================================================================
static inline int chmod(const char *pathname, mode_t mode)
{
        int r;
        syscall(SYSCALL_CHMOD, &r, pathname, &mode);
        return r;
}

//==============================================================================
/**
 * @brief int stat(const char *pathname, struct stat *buf)
 * The <b>stat</b>() function return information about a file.<p>
 *
 * This function return a <b>stat</b> structure, which contains the following
 * fileds:
 * <pre>
 * struct stat {
 *         u64_t   st_size;     // total size, in bytes
 *         u32_t   st_dev;      // ID of device containing file
 *         u32_t   st_mode;     // protection
 *         u32_t   st_uid;      // user ID of owner
 *         u32_t   st_gid;      // group ID of owner
 *         u32_t   st_atime;    // time of last access
 *         u32_t   st_mtime;    // time of last modification
 *         tfile_t st_type;     // type of file
 * };
 * </pre>
 *
 * @param pathname      file to inspect
 * @param buf           file's information
 *
 * @errors EINVAL, ENOMEM, EACCES, ENOENT, ...
 *
 * @return On success, 0 is returned. On error, -1 is returned, and <b>errno</b>
 * is set appropriately.
 *
 * @example
 * // ...
 *
 * struct stat info;
 * errno = 0;
 * if (stat("/dev/foo", &info) == 0) {
 *         // ...
 * } else {
 *         perror("stat()");
 *         // ...
 * }
 *
 * // ...
 */
//==============================================================================
static inline int stat(const char *pathname, struct stat *buf)
{
        int r;
        syscall(SYSCALL_STAT, &r, pathname, buf);
        return r;
}

//==============================================================================
/**
 * @brief int fstat(FILE *file, struct stat *stat)
 * The <b>fstat</b>() function return information about a file pointed by <i>file</i>.
 *
 * @param file          file object
 * @param buf           file's information
 *
 * @errors EINVAL, ENOMEM, EACCES, ...
 *
 * @return On success, 0 is returned. On error, -1 is returned, and <b>errno</b>
 * is set appropriately.
 *
 * @example
 * // ...
 *
 * errno = 0;
 * FILE *file = fopen("/dev/foo", "r");
 * if (file) {
 *         struct stat info;
 *         if (fstat(file, &info) == 0) {
 *                 // ...
 *         } else {
 *                 perror("stat()");
 *                 // ...
 *         }
 *
 *         fclose(file);
 * } else {
 *         perror("fopen()");
 * }
 *
 * // ...
 */
//==============================================================================
static inline int fstat(FILE *file, struct stat *buf)
{
        int r;
        syscall(SYSCALL_FSTAT, &r, file, buf);
        return r;
}

#ifdef __cplusplus
}
#endif

#endif /* _STAT_H_ */
/*==============================================================================
  End of file
==============================================================================*/
