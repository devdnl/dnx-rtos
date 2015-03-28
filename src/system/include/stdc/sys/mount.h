/*=========================================================================*//**
@file    mount.h

@author  Daniel Zorychta

@brief   Library with mount file system tools

@note    Copyright (C) 2013 Daniel Zorychta <daniel.zorychta@gmail.com>

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

#ifndef _MOUNT_H_
#define _MOUNT_H_

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Include files
==============================================================================*/
#include "core/fsctrl.h"
#include "core/modctrl.h"
#include "core/syscall.h"

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
 * @brief int mount(const char *FS_name, const char *src_path, const char *mount_point)
 * The <b>mount</b>() function mounts file system name pointed by <i>FS_name</i>
 * from source file pointed by <i>src_path</i> to mount directory pointed by
 * <i>mount_point</i>.<p>
 *
 * File system name and driver must exist in system to use it. If file system
 * not require to use source file (e.g. procfs, lfs, devfs) then <i>src_path</i>
 * shall be an empty string (<i>""</i>).
 *
 * @param FS_name       file system name
 * @param src_path      file system source file (e.g. /dev/sda1)
 * @param mount_point   file system mount directory
 *
 * @errors EINVAL, ENOMEM, ...
 *
 * @return On success, <b>0</b> is returned. On error, <b>-1</b>
 * is returned, and <b>errno</b> is set appropriately.
 *
 * @example
 * // ...
 *
 * mkdir("/sdcard", 0666);
 * errno = 0;
 * if (mount("fatfs", "/dev/sda1", "/sdcard") == 0) {
 *         // file system mounted ...
 *
 * } else {
 *         // file system not mounted
 *         perror("Mount failure");
 * }
 *
 * // ...
 */
//==============================================================================
static inline int mount(const char *FS_name, const char *src_path, const char *mount_point)
{
        int r;
        _syscall(SYSCALL_MOUNT, &r, FS_name, src_path, mount_point);
        return r;
}

//==============================================================================
/**
 * @brief int umount(const char *mount_point)
 * The <b>umount<b>() function unmount file system localized in path pointed by
 * <i>mount_point</i>. To unmount file system, all files of unmounting file
 * system shall be closed.
 *
 * @param seconds   number of seconds to sleep
 *
 * @errors EINVAL, EBUSY
 *
 * @return On success, <b>0</b> is returned. On error, <b>-1</b>
 * is returned, and <b>errno</b> is set appropriately.
 *
 * @example
 * // ...
 *
 * mkdir("/sdcard", 0666);
 * errno = 0;
 * if (mount("fatfs", "/dev/sda1", "/sdcard") == 0) {
 *         // file system mounted ...
 *         // operations on file system ...
 *
 *         if (umount("/sdcard") == 0) {
 *                 // ...
 *
 *         } else {
 *                 perror("Unmount failure");
 *         }
 * } else {
 *         // file system not mounted
 *         perror("Mount failure");
 * }
 *
 * // ...
 */
//==============================================================================
static inline int umount(const char *mount_point)
{
        int r;
        _syscall(SYSCALL_UMOUNT, &r, mount_point);
        return r;
}

//==============================================================================
/**
 * @brief int driver_init(const char *drv_name, const char *node_path)
 * The <b>driver_init<b>() function initialize driver pointed by <i>drv_name</i>
 * and create file node pointed by <i>node_path</i>. If there is no need to
 * create node, then <i>node_path</i> can be <b>NULL</b>. Node can be created
 * later using <b>mknod</b>() function.<p>
 *
 * Driver must exist in system to perform initialization. Driver's nodes can
 * be created only on file system which support it.
 *
 * @param drv_name      driver_name
 * @param node_path     path to node which should be created (or NULL)
 *
 * @errors EINVAL, ENOMEM, EADDRINUSE
 *
 * @return On success, driver ID is returned. On error, -1 is returned, and
 * <b>errno</b> is set appropriately.
 *
 * @example
 * // ...
 *
 * driver_init("crc", "/dev/crc");   // with node
 * driver_init("afio", NULL);        // without node
 *
 * // ...
 */
//==============================================================================
static inline int driver_init(const char *drv_name, const char *node_path)
{
        int r;
        _syscall(SYSCALL_DRIVERINIT, &r, drv_name, node_path);
        return r;
}

//==============================================================================
/**
 * @brief int driver_release(const char *drv_name)
 * The <b>driver_release<b>() function release driver pointed by <i>drv_name</i>.
 * If driver was released when node is created and pointed to driver then
 * node is node removed. From this time, device node is pointing to not
 * existing (initialized) device, resulting that user can't access to file.
 *
 * @param drv_name      driver_name
 *
 * @errors EINVAL, ...
 *
 * @return On success, 0 is returned. On error, 1 is returned, and <b>errno</b>
 * is set appropriately.
 *
 * @example
 * // ...
 *
 * driver_release("/dev/crc");
 *
 * // ...
 */
//==============================================================================
static inline int driver_release(const char *drv_name)
{
        int r;
        _syscall(SYSCALL_DRIVERINIT, &r, drv_name);
        return r;
}

#ifdef __cplusplus
}
#endif

#endif /* _MOUNT_H_ */
/*==============================================================================
  End of file
==============================================================================*/
