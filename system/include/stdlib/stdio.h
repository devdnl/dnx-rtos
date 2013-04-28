#ifndef STDIO_H_
#define STDIO_H_
/*=========================================================================*//**
@file    dnxio.h

@author  Daniel Zorychta

@brief

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

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Include files
==============================================================================*/
#include "config.h"
#include "core/sysmoni.h"

/*==============================================================================
  Exported macros
==============================================================================*/
/** stream values */
#define EOF                                     (-1)

/** stdio buffer size */
#define BUFSIZ                                  CONFIG_FSCANF_STREAM_BUFFER_SIZE


#define getmntentry(item, mntentPtr)            sysm_getmntentry(item, mntentPtr)
#define mknod(path, drv_cfgPtr)                 sysm_mknod(path, drv_cfgPtr)
#define mkdir(path)                             sysm_mkdir(path)
#define opendir(path)                           sysm_opendir(path)
#define closedir(dir)                           sysm_closedir(dir)
#define readdir(dir)                            sysm_readdir(dir)
#define remove(path)                            sysm_remove(path)
#define rename(oldName, newName)                sysm_rename(oldName, newName)
#define chmod(path, mode)                       sysm_chmod(path, mode)
#define chown(path, owner, group)               sysm_chown(path, owner, group)
#define stat(path, statPtr)                     sysm_stat(path, statPtr)
#define statfs(path, statfsPtr)                 sysm_statfs(path, statfsPtr)
#define fopen(path, mode)                       sysm_fopen(path, mode)
#define freopen(path, mode, file)               sysm_freopen(path, mode, file)
#define fclose(file)                            sysm_fclose(file)
#define fwrite(ptr, isize, nitems, file)        sysm_fwrite(ptr, isize, nitems, file)
#define fread(ptr, isize, nitems, file)         sysm_fread(ptr, isize, nitems, file)
#define fseek(file, offset, mode)               sysm_fseek(file, offset, mode)
#define ftell(file)                             sysm_ftell(file)
#define ioctl(file, ...)                        sysm_ioctl(file, __VA_ARGS__)
#define fstat(file, statPtr)                    sysm_fstat(file, stat)
#define fflush(file)                            sysm_fflush(file)
#define feof(file)                              sysm_feof(file)
#define rewind(file)                            sysm_rewind(file)
#define ferror(file)                            0

/*==============================================================================
  Exported object types
==============================================================================*/

/*==============================================================================
  Exported objects
==============================================================================*/

/*==============================================================================
  Exported functions
==============================================================================*/

#ifdef __cplusplus
}
#endif

#endif /* STDIO_H_ */
/*==============================================================================
  End of file
==============================================================================*/
