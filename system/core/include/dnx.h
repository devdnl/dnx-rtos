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
#include "basic_types.h"
#include "systypes.h"
#include "memman.h"
#include "oswrap.h"
#include "io.h"
#include "progman.h"
#include "taskmoni.h"
#include "regfs.h"
#include "regdrv.h"
#include "oshooks.h"

/*==============================================================================
  Exported symbolic constants/macros
==============================================================================*/
/** MEMORY MANAGEMENT DEFINTIONS */
#define malloc(size)                      tskm_malloc(size)
#define calloc(nitems, isize)             tskm_calloc(nitems, isize)
#define free(mem)                         tskm_free(mem)

/** FILE ACCESS MANAGEMENT */
#define getmntentry(item, mntentPtr)      tskm_getmntentry(item, mntentPtr)
#define mknod(path, drv_cfgPtr)           tskm_mknod(path, drv_cfgPtr)
#define mkdir(path)                       tskm_mkdir(path)
#define opendir(path)                     tskm_opendir(path)
#define closedir(dir)                     tskm_closedir(dir)
#define readdir(dir)                      tskm_readdir(dir)
#define remove(path)                      tskm_remove(path)
#define rename(oldName, newName)          tskm_rename(oldName, newName)
#define chmod(path, mode)                 tskm_chmod(path, mode)
#define chown(path, owner, group)         tskm_chown(path, owner, group)
#define stat(path, statPtr)               tskm_stat(path, statPtr)
#define statfs(path, statfsPtr)           tskm_statfs(path, statfsPtr)
#define fopen(path, mode)                 tskm_fopen(path, mode)
#define fclose(file)                      tskm_fclose(file)
#define fwrite(ptr, isize, nitems, file)  tskm_fwrite(ptr, isize, nitems, file)
#define fread(ptr, isize, nitems, file)   tskm_fread(ptr, isize, nitems, file)
#define fseek(file, offset, mode)         tskm_fseek(file, offset, mode)
#define ftell(file)                       tskm_ftell(file)
#define ioctl(file, rq, data)             tskm_ioctl(file, rq, data)
#define fstat(file, statPtr)              tskm_fstat(file, stat)

/** ENVIRONMENT DEFINITIONS */
#define get_used_static_memory()          (CONFIG_RAM_SIZE - MEMMAN_HEAP_SIZE)
#define get_free_memory()                 (CONFIG_RAM_SIZE - get_used_static_memory() - memman_get_used_heap())
#define get_used_memory()                 (get_used_static_memory() + memman_get_used_heap())
#define get_memory_size()                 CONFIG_RAM_SIZE
#define get_uptime()                      get_uptime_counter()
#define get_task_stat(ntask, statPtr)     tskm_get_ntask_stat(ntask, statPtr)
#define get_number_of_monitored_tasks()   tskm_get_number_of_monitored_tasks()
#define get_total_CPU_usage()             tskm_get_total_CPU_usage()
#define clear_total_CPU_usage()           tskm_clear_total_CPU_usage()
#define reboot()                          cpuctl_restart_system()
#define get_OS_name()                     "dnx"
#define get_kernel_name()                 "FreeRTOS"
#define get_OS_version()                  "0.8.0"
#define get_kernel_version()              "7.3.0"
#define get_host_name()                   CONFIG_HOSTNAME
#define getcwd(buf, size)                 strncpy(buf, get_this_task_data()->f_cwd, size)

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
