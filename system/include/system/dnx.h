#ifndef DNX_H_
#define DNX_H_
/*=========================================================================*//**
@file    dnx.h

@author  Daniel Zorychta

@brief   dnx system main header

@note    Copyright (C) 2012  Daniel Zorychta <daniel.zorychta@gmail.com>

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
#include "core/systypes.h"
#include "core/memman.h"
#include "core/progman.h"
#include "core/sysmoni.h"
#include "core/io.h"
#include "user/regfs.h"
#include "user/regdrv.h"
#include "kernel/kwrapper.h"
#include "kernel/khooks.h"
#include "portable/cpuctl.h"
#include <string.h>
#include <stdarg.h>

/*==============================================================================
  Exported symbolic constants/macros
==============================================================================*/
/** MEMORY MANAGEMENT DEFINTIONS */
#define malloc(size)                      sysm_tskmalloc(size)
#define calloc(nitems, isize)             sysm_tskcalloc(nitems, isize)
#define free(mem)                         sysm_tskfree(mem)

/** FILE ACCESS MANAGEMENT */
#define getmntentry(item, mntentPtr)      sysm_getmntentry(item, mntentPtr)
#define mknod(path, drv_cfgPtr)           sysm_mknod(path, drv_cfgPtr)
#define mkdir(path)                       sysm_mkdir(path)
#define opendir(path)                     sysm_opendir(path)
#define closedir(dir)                     sysm_closedir(dir)
#define readdir(dir)                      sysm_readdir(dir)
#define remove(path)                      sysm_remove(path)
#define rename(oldName, newName)          sysm_rename(oldName, newName)
#define chmod(path, mode)                 sysm_chmod(path, mode)
#define chown(path, owner, group)         sysm_chown(path, owner, group)
#define stat(path, statPtr)               sysm_stat(path, statPtr)
#define statfs(path, statfsPtr)           sysm_statfs(path, statfsPtr)
#define fopen(path, mode)                 sysm_fopen(path, mode)
#define fclose(file)                      sysm_fclose(file)
#define fwrite(ptr, isize, nitems, file)  sysm_fwrite(ptr, isize, nitems, file)
#define fread(ptr, isize, nitems, file)   sysm_fread(ptr, isize, nitems, file)
#define fseek(file, offset, mode)         sysm_fseek(file, offset, mode)
#define ftell(file)                       sysm_ftell(file)
#define ioctl(file, ...)                  sysm_ioctl(file, __VA_ARGS__)
#define fstat(file, statPtr)              sysm_fstat(file, stat)
#define fflush(file)                      sysm_fflush(file)

/** ENVIRONMENT DEFINITIONS */
#define get_used_static_memory()          (CONFIG_RAM_SIZE - CONFIG_HEAP_SIZE)
#define get_free_memory()                 memman_get_free_heap()
#define get_used_memory()                 (get_used_static_memory() + (CONFIG_HEAP_SIZE - memman_get_free_heap()))
#define get_memory_size()                 CONFIG_RAM_SIZE
#define get_uptime()                      get_uptime_counter()
#define get_task_stat(ntask, statPtr)     sysm_get_ntask_stat(ntask, statPtr)
#define get_number_of_monitored_tasks()   sysm_get_number_of_monitored_tasks()
#define get_total_CPU_usage()             sysm_get_total_CPU_usage()
#define clear_total_CPU_usage()           sysm_clear_total_CPU_usage()
#define reboot()                          cpuctl_restart_system()
#define get_platform_name()               CPUCTL_PLATFORM_NAME
#define get_OS_name()                     "dnx"
#define get_OS_version()                  "0.9.7"
#define get_kernel_name()                 "FreeRTOS"
#define get_kernel_version()              tskKERNEL_VERSION_NUMBER
#define get_host_name()                   CONFIG_HOSTNAME
#define getcwd(buf, size)                 strncpy(buf, _get_this_task_data()->f_cwd, size)
#define get_used_memory_by_kernel()       sysm_get_used_kernel_memory()
#define get_used_memory_by_system()       sysm_get_used_system_memory()
#define get_used_memory_by_modules()      sysm_get_used_modules_memory()
#define get_used_memory_by_programs()     sysm_get_used_program_memory()
#define get_module_memory_usage(modid)    sysm_get_module_used_memory(modid)
#define get_module_name(modid)            regdrv_get_module_name(modid)
#define get_module_count()                REGDRV_NUMBER_OF_REGISTERED_MODULES

/*==============================================================================
  Exported types, enums definitions
==============================================================================*/

/*==============================================================================
  Exported object declarations
==============================================================================*/

/*==============================================================================
  Exported function prototypes
==============================================================================*/
extern void dnx_init(void);

#ifdef __cplusplus
}
#endif

#endif /* DNX_H_ */
/*==============================================================================
  End of file
==============================================================================*/
