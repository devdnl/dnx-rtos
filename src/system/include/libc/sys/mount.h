/*=========================================================================*//**
@file    mount.h

@author  Daniel Zorychta

@brief   Library with mount file system tools

@note    Copyright (C) 2013 Daniel Zorychta <daniel.zorychta@gmail.com>

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
\defgroup sys-mount-h <sys/mount.h>

The library is used to control file systems mounting and drivers initialization.

*/
/**@{*/

#ifndef _MOUNT_H_
#define _MOUNT_H_

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Include files
==============================================================================*/
#include <kernel/syscall.h>
#include <drivers/drvctrl.h>
#include <sys/stat.h>
#include <kernel/errno.h>

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
 * @brief Function mount selected file system to selected path.
 *
 * The mount() function mounts file system name pointed by <i>FS_name</i>
 * from source file pointed by <i>src_path</i> to mount directory pointed by
 * <i>mount_point</i>.<p>
 *
 * File system name and source stream must exist in system to use it. If file
 * system not requires to use source file (e.g. procfs, ramfs, devfs) then
 * <i>src_path</i> shall be an empty string (<i>""</i>).
 *
 * Additional options can be passed to file system by using <i>options</i>
 * argument. It can be NULL or empty if no options given.
 *
 * @param FS_name       file system name
 * @param src_path      file system source file (e.g. /dev/sda1)
 * @param mount_point   file system mount directory
 * @param options       fiel system options (can be NULL)
 *
 * @exception | ...
 *
 * @return On success, <b>0</b> is returned. On error, <b>-1</b>
 * is returned, and <b>errno</b> is set appropriately.
 *
 * @b Example
 * @code
        // ...

        mkdir("/sdcard", 0666);

        errno = 0;
        if (mount("fatfs", "/dev/sda1", "/sdcard", NULL) == 0) {
                // file system mounted ...

        } else {
                // file system not mounted
                perror("Mount failure");
        }

        // ...

   @endcode
 *
 * @see umount()
 */
//==============================================================================
static inline int mount(const char *FS_name, const char *src_path,
                        const char *mount_point, const char *options)
{
        int r = -1;
        syscall(SYSCALL_MOUNT, &r, FS_name, src_path, mount_point, options);
        return r;
}

//==============================================================================
/**
 * @brief Function unmount earlier mounted file system.
 *
 * The umount() function unmount file system localized in path pointed by
 * <i>mount_point</i>. To unmount file system, all files of unmounting file
 * system shall be closed.
 *
 * @param mount_point   mount point directory
 *
 * @exception | @ref EINVAL
 * @exception | @ref EBUSY
 *
 * @return On success, <b>0</b> is returned. On error, <b>-1</b>
 * is returned, and <b>errno</b> is set appropriately.
 *
 * @b Example
 * @code
        // ...

        mkdir("/sdcard", 0666);

        errno = 0;
        if (mount("fatfs", "/dev/sda1", "/sdcard") == 0) {
                // file system mounted ...

                // operations on file system ...

                if (umount("/sdcard") == 0) {
                        // ...

                } else {
                        perror("Unmount failure");
                }
        } else {
                // file system not mounted
                perror("Mount failure");
        }

        // ...

   @endcode
 *
 * @see mount()
 */
//==============================================================================
static inline int umount(const char *mount_point)
{
        int r = -1;
        syscall(SYSCALL_UMOUNT, &r, mount_point);
        return r;
}

//==============================================================================
/**
 * @brief Function initializes driver.
 *
 * The driver_init() function initialize driver pointed by <i>mod_name</i>
 * and create file node pointed by <i>node_path</i>. If there is no need to
 * create node, then <i>node_path</i> can be <b>NULL</b>. Node can be created
 * later by using mknod() function.
 *
 * Driver must exist in system to perform initialization. Driver's nodes can
 * be created only on file system which support it.
 *
 * @param mod_name      module name
 * @param major         major driver number
 * @param minor         minor driver number
 * @param node_path     path where driver node should be created (or NULL)
 *
 * @exception | @ref EINVAL
 * @exception | @ref ENOMEM
 * @exception | @ref EADDRINUSE
 *
 * @return On success, driver ID is returned. On error, \b -1 is returned, and
 * <b>errno</b> is set appropriately.
 *
 * @b Example
 * @code
        // ...

        driver_init("UART", 0, 0, "/dev/uart0");   // UART0 as /dev/uart0
        driver_init("AFIO", 0, 0, NULL);           // driver without node

        // ...

   @endcode
 *
 * @see driver_release(), driver_release2()
 */
//==============================================================================
static inline dev_t driver_init(const char *mod_name, int major, int minor, const char *node_path)
{
        dev_t r = -1;
        syscall(SYSCALL_DRIVERINIT, &r, mod_name, &major, &minor, node_path);
        return r;
}

//==============================================================================
/**
 * @brief Function releases selected driver.
 *
 * The driver_release() function release driver pointed by <i>mod_name</i>.
 * If driver was released when node is created and pointed to driver then
 * node is not removed. From this time, device node is pointing to not
 * existing (initialized) device, resulting that user can't access to the file.
 *
 * @param mod_name      driver_name
 * @param major         major driver number
 * @param minor         minor driver number
 *
 * @exception | @ref EINVAL
 * @exception | ...
 *
 * @return On success, \b 0 is returned. On error, \b -1 is returned, and <b>errno</b>
 * is set appropriately.
 *
 * @b Example
 * @code
        // ...

        driver_release("UART", 0, 0);

        // ...

   @endcode
 *
 * @see driver_init(), driver_release2()
 */
//==============================================================================
static inline int driver_release(const char *mod_name, int major, int minor)
{
        int r = -1;
        syscall(SYSCALL_DRIVERRELEASE, &r, mod_name, &major, &minor);
        return r;
}

//==============================================================================
/**
 * @brief Function releases selected driver.
 *
 * The driver_release2() function release driver pointed by <i>path</i>.
 * If driver was released then node is not removed. From this time, device node
 * is pointing to not existing (initialized) device, resulting that user can't
 * access to the file.
 *
 * @param path          driver node
 *
 * @exception | @ref EINVAL
 * @exception | ...
 *
 * @return On success, \b 0 is returned. On error, \b -1 is returned, and <b>errno</b>
 * is set appropriately.
 *
 * @b Example
 * @code
        // ...

        driver_release2("/dev/UART");

        // ...

   @endcode
 *
 * @see driver_init(), driver_release()
 */
//==============================================================================
static inline int driver_release2(const char *path)
{
        int r = -1;

        struct stat buf;

        if (stat(path, &buf) == 0) {

                if (S_ISDEV(buf.st_mode)) {

                        u8_t  major = _dev_t__extract_major(buf.st_dev);
                        u8_t  minor = _dev_t__extract_minor(buf.st_dev);
                        u16_t modno = _dev_t__extract_modno(buf.st_dev);

                        const char *mod_name = _builtinfunc(module_get_name, modno);

                        syscall(SYSCALL_DRIVERRELEASE, &r, mod_name, &major, &minor);
                } else {
                        _errno = ENODEV;
                }
        }

        return r;
}

#ifdef __cplusplus
}
#endif

#endif /* _MOUNT_H_ */

/**@}*/
/*==============================================================================
  End of file
==============================================================================*/
