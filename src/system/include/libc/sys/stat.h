/*=========================================================================*//**
@file    stat.h

@author  Daniel Zorychta

@brief   File contains statistics functions and directory, FIFO, node create functions.

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

/**
\defgroup sys-stat-h <sys/stat.h>

The library is used to create device nodes, directories, and FIFO (pipe) devices.
Library provides also function used to manipulate file modes. The set of library
contains functions that read file statistics.

*/
/**@{*/

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
#ifdef DOXYGEN
/** @brief Read permission, owner. @see mode_t */
#define S_IRUSR
/** @brief Write permission, owner. @see mode_t */
#define S_IWUSR
/** @brief Execute permission, owner. @see mode_t */
#define S_IXUSR
/** @brief Read permission, group. @see mode_t */
#define S_IRGRO
/** @brief Write permission, group. @see mode_t */
#define S_IWGRO
/** @brief Execute permission, group. @see mode_t */
#define S_IXGRO
/** @brief Read permission, others. @see mode_t */
#define S_IROTH
/** @brief Write permission, others. @see mode_t */
#define S_IWOTH
/** @brief Execute permission, others. @see mode_t */
#define S_IXOTH
#endif

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
 * @brief Function creates device node (device file).
 *
 * The <b>mknod</b>() function creates a file system node (device special file) named
 * <i>pathname</i>. Node is connected to the driver from module <i>mod_name</i>
 * of <i>major</i> and <i>minor</i> address.
 *
 * @param pathname      node name
 * @param mod_name      module name
 * @param major         driver major number
 * @param minor         driver minor number
 *
 * @exception EINVAL    invalid argument
 * @exception ENOMEM    not enough memory to create node
 * @exception ...       error depends on file system
 *
 * @return On success, \b 0 is returned. On error, \b -1 is returned, and <b>errno</b>
 * is set appropriately.
 *
 * @b Example
 * @code
        // ...

        mknod("/dev/uart0", "UART", 0, 0);

        // ...
   @endcode
 */
//==============================================================================
static inline int mknod(const char *pathname, const char *mod_name, int major, int minor)
{
        int r = -1;
        syscall(SYSCALL_MKNOD, &r, pathname, mod_name, &major, &minor);
        return r;
}

//==============================================================================
/**
 * @brief Function creates new directory.
 *
 * The <b>mkdir</b>() attempts to create a directory named <i>pathname</i>. The
 * argument <i>mode</i> specifies the permissions to use.
 *
 * @param pathname      directory name
 * @param mode          directory permissions
 *
 * @exception EINVAL    invalid argument
 * @exception ENOMEM    not enough free memory to create directory
 * @exception EACCES    access denied
 * @exception EEXIST    directory already exists
 * @exception ENOENT    parent directory does not exists
 * @exception ENOSPC    not enough free space on disc
 * @exception ...       other errors returned by file system
 *
 * @return On success, \b 0 is returned. On error, \b -1 is returned, and <b>errno</b>
 * is set appropriately.
 *
 * @b Example
 * @code
        // ...

        mkdir("/dev", 0666);    // wr+rd access for all users, groups and others

        // ...

   @endcode
 */
//==============================================================================
static inline int mkdir(const char *pathname, mode_t mode)
{
        int r = -1;
        syscall(SYSCALL_MKDIR, &r, pathname, &mode);
        return r;
}

//==============================================================================
/**
 * @brief Function creates FIFO file.
 *
 * The <b>mkfifo</b>() makes a FIFO special file with name <i>pathname</i>. <i>mode</i>
 * specifies the FIFO's permissions. A FIFO special file is similar to pipe, but
 * is created in filesystem and is not an anonymous. Access to FIFO is the same
 * as to regular file, except that data can be read only one time. Not all
 * filesystems support this file type.
 *
 * @param pathname      FIFO name
 * @param mode          FIFO permissions
 *
 * @exception EINVAL    invalid argument
 * @exception ENOMEM    not enough free memory to create directory
 * @exception EACCES    access denied
 * @exception EEXIST    directory already exists
 * @exception ENOENT    parent directory does not exists
 * @exception ENOSPC    not enough free space on disc
 * @exception ...       other errors returned by file system
 *
 * @return On success, \b 0 is returned. On error, \b -1 is returned, and <b>errno</b>
 * is set appropriately.
 *
 * @b Example
 * @code
        // ...

        mkfifo("/dev/my_fifo", 0666);    // wr+rd access for all users, groups and others

        // ...

   @endcode
 */
//==============================================================================
static inline int mkfifo(const char *pathname, mode_t mode)
{
        int r = -1;
        syscall(SYSCALL_MKFIFO, &r, pathname, &mode);
        return r;
}

//==============================================================================
/**
 * @brief Function changes file mode.
 *
 * The <b>chmod</b>() system call change the permissions of a file.
 *
 * @param pathname      file to permission change
 * @param mode          new permissions
 *
 * @exception EINVAL    invalid argument
 * @exception ENOMEM    not enough free memory to create directory
 * @exception EACCES    access denied
 * @exception EEXIST    directory already exists
 * @exception ENOENT    parent directory does not exists
 * @exception ENOSPC    not enough free space on disc
 * @exception ...       other errors returned by file system
 *
 * @return On success, \b 0 is returned. On error, \b -1 is returned, and <b>errno</b>
 * is set appropriately.
 *
 * @b Example
 * @code
        // ...

        chmod("/foo/bar", 0666);   // wr+rd access for all users, groups and others

        // ...

   @endcode
 */
//==============================================================================
static inline int chmod(const char *pathname, mode_t mode)
{
        int r = -1;
        syscall(SYSCALL_CHMOD, &r, pathname, &mode);
        return r;
}

//==============================================================================
/**
 * @brief Function gets file information.
 *
 * The <b>stat</b>() function return information about a file.
 *
 * @param pathname      file to inspect
 * @param buf           file's information
 *
 * @exception EINVAL    invalid argument
 * @exception ENOMEM    not enough free memory to create directory
 * @exception EACCES    access denied
 * @exception EEXIST    directory already exists
 * @exception ENOENT    parent directory does not exists
 * @exception ENOSPC    not enough free space on disc
 * @exception ...       other errors returned by file system
 *
 * @return On success, \b 0 is returned. On error, \b -1 is returned, and <b>errno</b>
 * is set appropriately.
 *
 * @b Example
 * @code
        // ...

        struct stat info;
        errno = 0;
        if (stat("/dev/foo", &info) == 0) {
                // ...
        } else {
                perror("stat()");
                // ...
        }

        // ...

   @endcode
 */
//==============================================================================
static inline int stat(const char *pathname, struct stat *buf)
{
        int r = -1;
        syscall(SYSCALL_STAT, &r, pathname, buf);
        return r;
}

//==============================================================================
/**
 * @brief Function gets file information.
 *
 * The <b>fstat</b>() function return information about a file pointed by <i>file</i>.
 *
 * @param file          file object
 * @param buf           file's information
 *
 * @exception EINVAL    invalid argument
 * @exception ENOMEM    not enough free memory to create directory
 * @exception EACCES    access denied
 * @exception EEXIST    directory already exists
 * @exception ENOENT    parent directory does not exists
 * @exception ENOSPC    not enough free space on disc
 * @exception ...       other errors returned by file system
 *
 * @return On success, \b 0 is returned. On error, \b -1 is returned, and <b>errno</b>
 * is set appropriately.
 *
 * @b Example
 * @code
        // ...

        errno = 0;
        FILE *file = fopen("/foo/bar", "r");
        if (file) {
                struct stat info;
                if (fstat(file, &info) == 0) {
                        // ...
                } else {
                        perror("stat()");
                        // ...
                }

                fclose(file);
        } else {
                perror("fopen()");
        }

        // ...

   @endcode
 */
//==============================================================================
static inline int fstat(FILE *file, struct stat *buf)
{
        int r = -1;
        syscall(SYSCALL_FSTAT, &r, file, buf);
        return r;
}

#ifdef __cplusplus
}
#endif

/**@}*/

#endif /* _STAT_H_ */
/*==============================================================================
  End of file
==============================================================================*/
