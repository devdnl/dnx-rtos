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

#ifndef _LIBC_MOUNT_H_
#define _LIBC_MOUNT_H_

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Include files
==============================================================================*/
#include <libc/source/syscall.h>
#include <sys/stat.h>
#include <errno.h>

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
 * @param options       file system options (can be NULL)
 *
 * @exception | ...
 *
 * @return On success, <b>0</b> is returned. On error, <b> -1</b>
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
        int err = _libc_syscall(_LIBC_SYS_MOUNT, FS_name, src_path, mount_point, options);
        return err ? -1 : 0;
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
 * @return On success, <b>0</b> is returned. On error, <b> -1</b>
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
        int err = _libc_syscall(_LIBC_SYS_UMOUNT, mount_point);
        return err ? -1 : 0;
}

//==============================================================================
/**
 * @brief Function initializes driver with configuration argument.
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
 * @param config        configuration object (or NULL)
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

        static const TTY_config_t config = {
                .input_file   = "/dev/ttyS0",
                .output_file  = "/dev/ttyS0",
                .clear_screen = true,
        };
        driver_init2("TTY", 0, 0, "/dev/tty0", &config);        // TTY as /dev/tty0

        driver_init2("AFIO", 0, 0, NULL, NULL);                 // driver without node

        // ...

   @endcode
 *
 * @see driver_release(), driver_release2(), driver_init()
 */
//==============================================================================
static inline dev_t driver_init2(const char *mod_name, int major, int minor, const char *node_path, const void *config)
{
        int err = _libc_syscall(_LIBC_SYS_DRIVERINIT, mod_name, &major, &minor, node_path, config);
        return err ? -1 : 0;
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
 * @see driver_release(), driver_release2(), driver_init2()
 */
//==============================================================================
static inline dev_t driver_init(const char *mod_name, int major, int minor, const char *node_path)
{
        return driver_init2(mod_name, major, minor, node_path, NULL);
}

//==============================================================================
/**
 * @brief Function returns a name of selected driver.
 *
 * The function get_driver_name() return name of selected module by using
 * <i>modno</i> index.
 *
 * @param modno     module number
 *
 * @return On success return driver name reference. On error, NULL is returned.
 *
 * @b Example
 * @code
        #include <dnx/os.h>

        // ...

        printf("Module name: %s\n", get_driver_name(0));

        // ...

   @endcode
 */
//==============================================================================
static inline const char *get_driver_name(size_t modno)
{
        const char *name = NULL;
        int err = _libc_syscall(_LIBC_SYS_GETDRIVERNAME, &modno, &name);
        return err ? NULL : name;
}

//==============================================================================
/**
 * @brief Function returns an ID of selected module (by name).
 *
 * The function get_driver_ID() return module ID selected by
 * name pointed by <i>name</i>.
 *
 * @param name     module name
 *
 * @return On success, return module index (ID). On error, \b -1 is returned.
 *
 * @b Example
 * @code
        #include <dnx/os.h>

        // ...

        printf("Module ID: %d\n", get_driver_ID("crc"));

        // ...
   @endcode
 */
//==============================================================================
static inline int get_driver_ID(const char *name)
{
        int midx;
        int err = _libc_syscall(_LIBC_SYS_GETDRIVERID, name, &midx);
        return err ? -1 : midx;
}

//==============================================================================
/**
 * @brief Function returns an ID of selected module (by dev_t index).
 *
 * The function get_driver_ID2() return module ID stored in dev_t type.
 *
 * @param dev           device ID
 *
 * @return Return module ID.
 *
 * @b Example
 * @code
        #include <dnx/os.h>

        // ...

        printf("Module ID: %d\n", get_driver_ID2(dev));

        // ...

   @endcode
 */
//==============================================================================
static inline int get_driver_ID2(dev_t dev)
{
        return (dev >> 16) & 0x7FFF;
}

//==============================================================================
/**
 * @brief Function returns a major number of selected module.
 *
 * The function get_driver_major() return module major number stored in
 * dev_t type.
 *
 * @param dev           device ID
 *
 * @return Return module major number.
 *
 * @b Example
 * @code
        #include <dnx/os.h>

        // ...

        printf("Module ID: %d\n", get_driver_major(dev));

        // ...

   @endcode
 */
//==============================================================================
static inline int get_driver_major(dev_t dev)
{
        return (dev >> 8) & 0xFF;
}

//==============================================================================
/**
 * @brief Function returns a minor number of selected module.
 *
 * The function get_driver_minor() return module minor number stored in
 * dev_t type.
 *
 * @param dev           device ID
 *
 * @return Return module minor number.
 *
 * @b Example
 * @code
        #include <dnx/os.h>

        // ...

        printf("Module ID: %d\n", get_driver_minor(dev));

        // ...

   @endcode
 */
//==============================================================================
static inline int get_driver_minor(dev_t dev)
{
        return (dev >> 0) & 0xFF;
}

//==============================================================================
/**
 * @brief Function returns number of modules.
 *
 * The function get_number_of_drivers() return number of registered
 * modules in system.
 *
 * @return Return number of registered modules in system. On failure, 0 is
 * returned.
 *
 * @b Example
 * @code
        #include <dnx/os.h>

        // ...

        size_t number_of_modules = get_number_of_drivers();
        for (size_t i = 0; i < number_of_modules; i++) {
                ...
        }

        // ...

   @endcode
 */
//==============================================================================
static inline size_t get_number_of_drivers(void)
{
        size_t count;
        int err = _libc_syscall(_LIBC_SYS_GETDRIVERCOUNT, &count);
        return err ? 0 : count;
}

//==============================================================================
/**
 * @brief Function returns number of instances of selected module index (id).
 *
 * The function get_number_of_driver_instances() return number of instances
 * of selected module of index <i>n</i>.
 *
 * @param n             module index
 *
 * @return On success, number of instances is returned, otherwise \b -1 is returned.
 *
 * @b Example
 * @code
        #include <dnx/os.h>

        // ...

        ssize_t n = get_number_of_driver_instances(get_module_ID("TTY"));
        printf("Numer of TTY driver instances: %d\n", n);

        // ...

   @endcode
 */
//==============================================================================
static inline ssize_t get_number_of_driver_instances(size_t id)
{
        size_t count;
        int err = _libc_syscall(_LIBC_SYS_GETDRIVERINSTANCES, &id, &count);
        return err ? -1 : (ssize_t)count;
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
        int err = _libc_syscall(_LIBC_SYS_DRIVERRELEASE, mod_name, &major, &minor);
        return err ? -1 : 0;
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
        int err = -1;

        struct stat buf;
        if (stat(path, &buf) == 0) {

                if (S_ISDEV(buf.st_mode)) {

                        uint8_t  major = get_driver_major(buf.st_dev);
                        uint8_t  minor = get_driver_minor(buf.st_dev);
                        uint16_t modno = get_driver_ID2(buf.st_dev);

                        const char *mod_name = get_driver_name(modno);
                        err = driver_release(mod_name, major, minor);

                } else {
                        errno = ENODEV;
                }
        }

        return err;
}

#ifdef __cplusplus
}
#endif

#endif /* _LIBC_MOUNT_H_ */

/**@}*/
/*==============================================================================
  End of file
==============================================================================*/
