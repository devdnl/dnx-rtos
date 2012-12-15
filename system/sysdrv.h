#ifndef SYSDRV_H_
#define SYSDRV_H_
/*=============================================================================================*//**
@file    sysdrv.h

@author  Daniel Zorychta

@brief   This function provide all required function needed to write drivers.

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


*//*==============================================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*==================================================================================================
                                            Include files
==================================================================================================*/
#include "basic_types.h"
#include "systypes.h"
#include "memman.h"
#include "oswrap.h"
#include "vfs.h"


/*==================================================================================================
                                 Exported symbolic constants/macros
==================================================================================================*/
#ifdef SYSTEM_H_
#error "system.h and sysdrv.h shall never included together!"
#endif

#ifndef calloc
#define calloc(nmemb, msize)              mm_calloc(nmemb, msize)
#endif

#ifndef malloc
#define malloc(size)                      mm_malloc(size)
#endif

#ifndef free
#define free(mem)                         mm_free(mem)
#endif

#define mount(path, fs_cfgPtr)            vfs_mount(path, fs_cfgPtr)
#define umount(path)                      vfs_umount(path)
#define getmntentry(item, mntentPtr)      vfs_getmntentry(item, mntentPtr)
#define mknod(path, drv_cfgPtr)           vfs_mknod(path, drv_cfgPtr)
#define mkdir(path)                       vfs_mkdir(path)
#define opendir(path)                     vfs_opendir(path)
#define closedir(dir)                     vfs_closedir(dir)
#define readdir(dir)                      vfs_readdir(dir)
#define remove(path)                      vfs_remove(path)
#define rename(oldName, newName)          vfs_rename(oldName, newName)
#define chmod(path, mode)                 vfs_chmod(path, mode)
#define chown(path, owner, group)         vfs_chown(path, owner, group)
#define stat(path, statPtr)               vfs_stat(path, statPtr)
#define statfs(path, statfsPtr)           vfs_statfs(path, statfsPtr)
#define fopen(path, mode)                 vfs_fopen(path, mode)
#define fclose(file)                      vfs_fclose(file)
#define fwrite(ptr, isize, nitems, file)  vfs_fwrite(ptr, isize, nitems, file)
#define fread(ptr, isize, nitems, file)   vfs_fread(ptr, isize, nitems, file)
#define fseek(file, offset, mode)         vfs_fseek(file, offset, mode)
#define ftell(file)                       vfs_ftell(file)
#define ioctl(file, rq, data)             vfs_ioctl(file, rq, data)
#define fstat(file, statPtr)              vfs_fstat(file, stat)


/*==================================================================================================
                                  Exported types, enums definitions
==================================================================================================*/


/*==================================================================================================
                                     Exported object declarations
==================================================================================================*/


/*==================================================================================================
                                     Exported function prototypes
==================================================================================================*/


#ifdef __cplusplus
}
#endif

#endif /* SYSDRV_H_ */
/*==================================================================================================
                                            End of file
==================================================================================================*/
