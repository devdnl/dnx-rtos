/*=========================================================================*//**
@file    sysmoni.c

@author  Daniel Zorychta

@brief   This module is used to monitoring system

@note    Copyright (C) 2012, 2013 Daniel Zorychta <daniel.zorychta@gmail.com>

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

/*==============================================================================
  Include files
==============================================================================*/
#include <errno.h>
#include <string.h>
#include <dnx/misc.h>
#include "core/sysmoni.h"
#include "core/llist.h"
#include "core/printx.h"
#include "core/progman.h"
#include "kernel/kwrapper.h"
#include "portable/cpuctl.h"

/*==============================================================================
  Local symbolic constants/macros
==============================================================================*/
#define TASK_MEMORY_SLOTS               24
#define TASK_FILE_SLOTS                 8
#define TASK_DIR_SLOTS                  4
#define MTX_BLOCK_TIME                  10

/*==============================================================================
  Local types, enums definitions
==============================================================================*/
struct kernel_panic_desc {
        uint32_t                       valid1;
        enum _kernel_panic_desc_cause  cause;
        char                           task_name[CONFIG_RTOS_TASK_NAME_LEN];
        uint32_t                       valid2;
};

#if (CONFIG_MONITOR_TASK_MEMORY_USAGE > 0)
typedef struct mem_slot_chain {
        int                    used_slots;
        void                  *mem_slot[TASK_MEMORY_SLOTS];
        struct mem_slot_chain *prev;
        struct mem_slot_chain *next;
} mem_slot_chain_t ;
#endif

/* task information */
struct task_monitor_data {
        #if (CONFIG_MONITOR_TASK_MEMORY_USAGE > 0)
        u32_t            used_memory;
        mem_slot_chain_t mem_chain;
        #endif

        #if (CONFIG_MONITOR_TASK_FILE_USAGE > 0)
        FILE  *file_slot[TASK_FILE_SLOTS];
        DIR   *dir_slot[TASK_DIR_SLOTS];
        uint   opened_files;
        #endif

        #if (CONFIG_MONITOR_TASK_MEMORY_USAGE == 0) && (CONFIG_MONITOR_TASK_FILE_USAGE == 0)
        int dummy; /* used only to create structure when above fields doesn't exist */
        #endif
};

/*==============================================================================
  Local function prototypes
==============================================================================*/

/*==============================================================================
  Local object definitions
==============================================================================*/
static struct kernel_panic_desc *kernel_panic_descriptor;

#if (CONFIG_MONITOR_TASK_MEMORY_USAGE > 0 || CONFIG_MONITOR_TASK_FILE_USAGE > 0 || CONFIG_MONITOR_CPU_LOAD > 0)
static llist_t *sysm_task_list;
static mutex_t *sysm_resource_mtx;
#endif

#if (CONFIG_MONITOR_CPU_LOAD > 0)
static bool  CPU_load_enabled = true;
static u32_t CPU_total_time;
#endif

#if (CONFIG_MONITOR_KERNEL_MEMORY_USAGE > 0)
static i32_t sysm_kernel_memory_usage;
#endif

#if (CONFIG_MONITOR_SYSTEM_MEMORY_USAGE > 0)
static i32_t sysm_system_memory_usage;
#endif

#if (CONFIG_MONITOR_MODULE_MEMORY_USAGE > 0)
static i32_t  sysm_modules_memory_usage;
static i32_t *sysm_module_memory_usage;
#endif

#if (CONFIG_MONITOR_TASK_MEMORY_USAGE > 0)
static i32_t sysm_programs_memory_usage;
#endif

#if ((CONFIG_MONITOR_NETWORK_MEMORY_USAGE > 0) && (CONFIG_NETWORK_ENABLE > 0))
static i32_t sysm_network_memory_usage;
#endif

/*==============================================================================
  Exported object definitions
==============================================================================*/

/*==============================================================================
  External object definitions
==============================================================================*/
#if (CONFIG_MONITOR_MODULE_MEMORY_USAGE > 0)
extern const uint _regdrv_number_of_modules;
#endif

/*==============================================================================
  Function definitions
==============================================================================*/

//==============================================================================
/**
 * @brief Force lock mutex
 *
 * @param mtx           mutex
 */
//==============================================================================
#if (CONFIG_MONITOR_TASK_MEMORY_USAGE > 0 || CONFIG_MONITOR_TASK_FILE_USAGE > 0 || CONFIG_MONITOR_CPU_LOAD > 0)
static inline void mutex_force_lock(mutex_t *mtx)
{
        while (_mutex_lock(mtx, MTX_BLOCK_TIME) != true);
}
#endif

//==============================================================================
/**
 * @brief  Function modify memory usage
 * @param  usage_variable       pointer to variable that is modified
 * @param  size                 size of allocated/freed memory
 * @return None
 */
//==============================================================================
#if (CONFIG_MONITOR_KERNEL_MEMORY_USAGE > 0)\
||  (CONFIG_MONITOR_SYSTEM_MEMORY_USAGE > 0)\
||  ((CONFIG_MONITOR_NETWORK_MEMORY_USAGE > 0) && (CONFIG_NETWORK_ENABLE > 0))
static void modify_memory_usage(void *usage_variable, i32_t size)
{
        *reinterpret_cast(i32_t*, usage_variable) += size;
}
#endif

//==============================================================================
/**
 * @brief  Function modify module's memory
 * @param  mod_no       module number (pointer must be casted to uint)
 * @param  size         size of allocated/freed memory
 * @return None
 */
//==============================================================================
#if (CONFIG_MONITOR_MODULE_MEMORY_USAGE > 0)
static void modify_module_memory_usage(void *mod_no, i32_t size)
{
        uint modno = reinterpret_cast(uint, mod_no);
        sysm_modules_memory_usage       += size;
        sysm_module_memory_usage[modno] += size;
}
#endif

//==============================================================================
/**
 * @brief Initialize module
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
int _sysm_init(void)
{
#if (CONFIG_MONITOR_SYSTEM_MEMORY_USAGE > 0)
        kernel_panic_descriptor = _sysm_sysmalloc(sizeof(struct kernel_panic_desc));
#else
        kernel_panic_descriptor = _memman_malloc(sizeof(struct kernel_panic_desc), NULL, NULL);
#endif

#if (CONFIG_MONITOR_SYSTEM_MEMORY_USAGE > 0)
        sysm_system_memory_usage = (i32_t)_MEMMAN_RAM_SIZE - (i32_t)_MEMMAN_HEAP_SIZE;
#endif

#if (CONFIG_MONITOR_TASK_MEMORY_USAGE > 0 || CONFIG_MONITOR_TASK_FILE_USAGE > 0 || CONFIG_MONITOR_CPU_LOAD > 0)
        sysm_task_list    = _llist_new(_sysm_sysmalloc, _sysm_sysfree, _llist_functor_cmp_pointers, NULL);
        sysm_resource_mtx = _mutex_new(MUTEX_RECURSIVE);
#endif

#if (CONFIG_MONITOR_CPU_LOAD > 0)
        _cpuctl_init_CPU_load_counter();
#endif

#if (CONFIG_MONITOR_MODULE_MEMORY_USAGE > 0)
        sysm_module_memory_usage = _sysm_syscalloc(_regdrv_number_of_modules, sizeof(i32_t));
#endif

#if (CONFIG_MONITOR_TASK_MEMORY_USAGE > 0 || CONFIG_MONITOR_TASK_FILE_USAGE > 0 || CONFIG_MONITOR_CPU_LOAD > 0)
#if (CONFIG_MONITOR_MODULE_MEMORY_USAGE > 0)
        if (!sysm_task_list || !sysm_resource_mtx || !sysm_module_memory_usage) {

                if (sysm_module_memory_usage) {
                        _sysm_sysfree(sysm_module_memory_usage);
                }
#else
        if (!sysm_task_list || !sysm_resource_mtx) {
#endif
                if (sysm_task_list) {
                        _llist_delete(sysm_task_list);
                }

                if (sysm_resource_mtx) {
                        _mutex_delete(sysm_resource_mtx);
                }

                return ENOMEM;
        }
#endif

        return ESUCC;
}

//==============================================================================
/**
 * @brief Function locks entire access to the system monitor to protect ongoing operations
 *
 * @param  None
 * @return None
 */
//==============================================================================
void _sysm_lock_access(void)
{
#if (CONFIG_MONITOR_TASK_MEMORY_USAGE > 0 || CONFIG_MONITOR_TASK_FILE_USAGE > 0 || CONFIG_MONITOR_CPU_LOAD > 0)
        mutex_force_lock(sysm_resource_mtx);
#endif
}

//==============================================================================
/**
 * @brief Function unlocks entire access to the system monitor
 *
 * @param  None
 * @return None
 */
//==============================================================================
void _sysm_unlock_access(void)
{
#if (CONFIG_MONITOR_TASK_MEMORY_USAGE > 0 || CONFIG_MONITOR_TASK_FILE_USAGE > 0 || CONFIG_MONITOR_CPU_LOAD > 0)
        _mutex_unlock(sysm_resource_mtx);
#endif
}

//==============================================================================
/**
 * @brief Function check if task is already in the monitor list
 *
 * @param *taskhdl      task handle
 *
 * @retval true         task exist
 * @retval false        task does not exist
 */
//==============================================================================
bool _sysm_is_task_exist(task_t *taskhdl)
{
#if (CONFIG_MONITOR_TASK_MEMORY_USAGE > 0 || CONFIG_MONITOR_TASK_FILE_USAGE > 0 || CONFIG_MONITOR_CPU_LOAD > 0)
        mutex_force_lock(sysm_resource_mtx);

        bool exist = _llist_find_begin(sysm_task_list, taskhdl) >= 0;

        if (!exist) {
                errno = ESRCH;
        }

        _mutex_unlock(sysm_resource_mtx);

        return exist;
#else
        if (taskhdl)
                return true;
        else
                return false;
#endif
}

//==============================================================================
/**
 * @brief Function start task monitoring
 *
 * @param *taskhdl      task handle
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
int _sysm_start_task_monitoring(task_t *taskhdl, size_t stack_size)
{
#if (CONFIG_MONITOR_TASK_MEMORY_USAGE > 0 || CONFIG_MONITOR_TASK_FILE_USAGE > 0 || CONFIG_MONITOR_CPU_LOAD > 0)
        mutex_force_lock(sysm_resource_mtx);

        int status = ESRCH;

        if (_sysm_is_task_exist(taskhdl) == false) {
                struct task_monitor_data *tmdata = _sysm_syscalloc(1, sizeof(struct task_monitor_data));
                if (tmdata) {
                        tmdata->used_memory = stack_size;

                        if (_llist_push_back(sysm_task_list, taskhdl)) {
                                _task_set_monitor_data(taskhdl, tmdata);
                                status = ESUCC;
                        } else {
                                _sysm_sysfree(tmdata);
                                _task_set_monitor_data(taskhdl, NULL);
                        }
                }
        }

        _mutex_unlock(sysm_resource_mtx);
        return status;
#else
        UNUSED_ARG(taskhdl);
        UNUSED_ARG(stack_size);
        return ESUCC;
#endif
}

//==============================================================================
/**
 * @brief Function stops task monitoring (free all resources)
 *
 * @param *taskhdl      task handle
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
int _sysm_stop_task_monitoring(task_t *taskhdl)
{
#if (CONFIG_MONITOR_TASK_MEMORY_USAGE > 0 || CONFIG_MONITOR_TASK_FILE_USAGE > 0 || CONFIG_MONITOR_CPU_LOAD > 0)
        mutex_force_lock(sysm_resource_mtx);

        if (_sysm_is_task_exist(taskhdl) == false) {
                goto exit_error;
        }

        struct task_monitor_data *task_monitor_data = _task_get_monitor_data(taskhdl);
        if (!task_monitor_data) {
                goto exit_error;
        }

#if (CONFIG_MONITOR_TASK_MEMORY_USAGE > 0)
        mem_slot_chain_t *chain;
        for (chain = &task_monitor_data->mem_chain; chain->next != NULL; chain = chain->next) {
                /* go to the last memory slot chain */
        }

        i32_t to_free = 0;
        do {
                if (chain->next) {
                        _memman_free(chain->next, NULL, NULL);
                }

                if (chain->used_slots > 0) {
                        for (uint i = 0; i < TASK_MEMORY_SLOTS; i++) {
                                if (chain->mem_slot[i]) {
                                        _memman_free(chain->mem_slot[i], modify_memory_usage, &to_free);
                                }
                        }
                }
        } while ((chain = chain->prev) != NULL);

        if (_task_get_data()->f_task_type != TASK_TYPE_RAW) {
                sysm_programs_memory_usage += to_free;
        }
#endif

#if (CONFIG_MONITOR_TASK_FILE_USAGE > 0)
        for (uint slot = 0; slot < TASK_FILE_SLOTS; slot++) {
                if (task_monitor_data->file_slot[slot]) {
                        _vfs_fclose_force(task_monitor_data->file_slot[slot]);
                }
        }

        for (uint slot = 0; slot < TASK_DIR_SLOTS; slot++) {
                if (task_monitor_data->dir_slot[slot]) {
                        _vfs_closedir(task_monitor_data->dir_slot[slot]);
                }
        }
#endif

        _sysm_sysfree(task_monitor_data);
        _task_set_monitor_data(taskhdl, NULL);

        int pos = _llist_find_begin(sysm_task_list, taskhdl);
        _llist_take(sysm_task_list, pos);

        _mutex_unlock(sysm_resource_mtx);
        return ESUCC;

exit_error:
        _mutex_unlock(sysm_resource_mtx);
        return ESRCH;

#else
        UNUSED_ARG(taskhdl);
        return ESUCC;
#endif
}

//==============================================================================
/**
 * @brief Function gets task status
 *
 * @param *taskHdl      task handle
 * @param *stat         status result
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
int _sysm_get_task_stat(task_t *taskhdl, struct _sysmoni_taskstat *stat)
{
        if (!stat)
                return EINVAL;

#if (CONFIG_MONITOR_TASK_MEMORY_USAGE > 0 || CONFIG_MONITOR_TASK_FILE_USAGE > 0 || CONFIG_MONITOR_CPU_LOAD > 0)
        mutex_force_lock(sysm_resource_mtx);

        if (_sysm_is_task_exist(taskhdl) == false) {
                goto exit_error;
        }

        struct task_monitor_data *tmdata = _task_get_monitor_data(taskhdl);
        if (!tmdata) {
                goto exit_error;
        }

        _critical_section_begin();
        stat->cpu_usage = _task_get_data_of(taskhdl)->f_cpu_usage;
        _task_get_data_of(taskhdl)->f_cpu_usage = 0;
        _critical_section_end();

        stat->free_stack   = _task_get_free_stack_of(taskhdl);

#if (CONFIG_MONITOR_TASK_MEMORY_USAGE > 0)
        stat->memory_usage = tmdata->used_memory;
#else
        stat->memory_usage = 0;
#endif

#if (CONFIG_MONITOR_TASK_FILE_USAGE > 0)
        stat->opened_files = tmdata->opened_files;
#else
        stat->opened_files = 0;
#endif

        stat->priority     = _task_get_priority_of(taskhdl);
        stat->task_handle  = taskhdl;
        stat->task_name    = _task_get_name_of(taskhdl);

        _mutex_unlock(sysm_resource_mtx);
        return ESUCC;

exit_error:
        _mutex_unlock(sysm_resource_mtx);
        return ESRCH;

#else
        stat->cpu_usage    = 0;
        stat->free_stack   = _task_get_free_stack_of(taskhdl);
        stat->memory_usage = 0;
        stat->opened_files = 0;
        stat->priority     = _task_get_priority_of(taskhdl);
        stat->task_handle  = taskhdl;
        stat->task_name    = _task_get_name_of(taskhdl);
        return ESUCC;
#endif
}

//==============================================================================
/**
 * @brief Function gets task statistics
 *
 * @param  item   task item
 * @param *stat   task statistics
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
int _sysm_get_ntask_stat(uint item, struct _sysmoni_taskstat *stat)
{
#if (CONFIG_MONITOR_TASK_MEMORY_USAGE > 0 || CONFIG_MONITOR_TASK_FILE_USAGE > 0 || CONFIG_MONITOR_CPU_LOAD > 0)
        mutex_force_lock(sysm_resource_mtx);

        int status = ESRCH;

        task_t *task = _llist_at(sysm_task_list, item);

        if (_sysm_get_task_stat(task, stat) == ESUCC) {
                status = ESUCC;
        }

        _mutex_unlock(sysm_resource_mtx);
        return status;
#else
        UNUSED_ARG(item);
        UNUSED_ARG(stat);
        return ESRCH;
#endif
}

//==============================================================================
/**
 * @brief Function gets information about used memory
 *
 * @param[out] *mem_info        memory information
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
int _sysm_get_used_memory(struct _sysmoni_used_memory *mem_info)
{
        if (!mem_info) {
                return EINVAL;
        }

#if (CONFIG_MONITOR_KERNEL_MEMORY_USAGE > 0)
        mem_info->used_kernel_memory   = sysm_kernel_memory_usage;
#else
        mem_info->used_kernel_memory   = 0;
#endif

#if (CONFIG_MONITOR_MODULE_MEMORY_USAGE > 0)
        mem_info->used_modules_memory  = sysm_modules_memory_usage;
#else
        mem_info->used_modules_memory  = 0;
#endif

#if (CONFIG_MONITOR_TASK_MEMORY_USAGE > 0)
        mem_info->used_programs_memory = sysm_programs_memory_usage;
#else
        mem_info->used_programs_memory = 0;
#endif

#if (CONFIG_MONITOR_SYSTEM_MEMORY_USAGE > 0)
        mem_info->used_system_memory   = sysm_system_memory_usage;
#else
        mem_info->used_system_memory   = 0;
#endif

#if ((CONFIG_MONITOR_NETWORK_MEMORY_USAGE > 0) && (CONFIG_NETWORK_ENABLE > 0))
        mem_info->used_network_memory  = sysm_network_memory_usage;
#else
        mem_info->used_network_memory  = 0;
#endif
        return ESUCC;
}

//==============================================================================
/**
 * @brief Function return number of monitor tasks
 *
 * @return number of monitor tasks
 */
//==============================================================================
uint _sysm_get_number_of_monitored_tasks(void)
{
#if (CONFIG_MONITOR_TASK_MEMORY_USAGE > 0 || CONFIG_MONITOR_TASK_FILE_USAGE > 0 || CONFIG_MONITOR_CPU_LOAD > 0)
        mutex_force_lock(sysm_resource_mtx);

        int task_count = _llist_size(sysm_task_list);

        _mutex_unlock(sysm_resource_mtx);

        return task_count;
#else
        return 0;
#endif
}

//==============================================================================
/**
 * @brief Function monitor memory usage of kernel
 *
 * @param  size         block size
 *
 * @return pointer to allocated block or NULL if error
 */
//==============================================================================
void *_sysm_kmalloc(size_t size)
{
#if (CONFIG_MONITOR_KERNEL_MEMORY_USAGE > 0)
        return _memman_malloc(size, modify_memory_usage, &sysm_kernel_memory_usage);
#else
        return _memman_malloc(size, NULL, NULL);
#endif
}

//==============================================================================
/**
 * @brief Function monitor memory usage of kernel
 *
 * @param  count        count of items
 * @param  size         item size
 *
 * @return pointer to allocated block or NULL if error
 */
//==============================================================================
void *_sysm_kcalloc(size_t count, size_t size)
{
#if (CONFIG_MONITOR_KERNEL_MEMORY_USAGE > 0)
        return _memman_calloc(count, size, modify_memory_usage, &sysm_kernel_memory_usage);
#else
        return _memman_calloc(count, size, NULL, NULL);
#endif
}

//==============================================================================
/**
 * @brief Monitor memory freeing for kernel
 *
 * @param *mem          block to free
 */
//==============================================================================
void _sysm_kfree(void *mem)
{
#if (CONFIG_MONITOR_KERNEL_MEMORY_USAGE > 0)
        _memman_free(mem, modify_memory_usage, &sysm_kernel_memory_usage);
#else
        _memman_free(mem, NULL, NULL);
#endif
}

//==============================================================================
/**
 * @brief Function monitor memory usage of system
 *
 * @param  size         block size
 *
 * @return pointer to allocated block or NULL if error
 */
//==============================================================================
void *_sysm_sysmalloc(size_t size)
{
#if (CONFIG_MONITOR_SYSTEM_MEMORY_USAGE > 0)
        return _memman_malloc(size, modify_memory_usage, &sysm_system_memory_usage);
#else
        return _memman_malloc(size, NULL, NULL);
#endif
}

//==============================================================================
/**
 * @brief Function monitor memory usage of system
 *
 * @param  count        count of items
 * @param  size         item size
 *
 * @return pointer to allocated block or NULL if error
 */
//==============================================================================
void *_sysm_syscalloc(size_t count, size_t size)
{
#if (CONFIG_MONITOR_SYSTEM_MEMORY_USAGE > 0)
        return _memman_calloc(count, size, modify_memory_usage, &sysm_system_memory_usage);
#else
        return _memman_calloc(count, size, NULL, NULL);
#endif
}

//==============================================================================
/**
 * @brief Monitor memory freeing for system
 *
 * @param *mem          block to free
 */
//==============================================================================
void _sysm_sysfree(void *mem)
{
#if (CONFIG_MONITOR_SYSTEM_MEMORY_USAGE > 0)
        _memman_free(mem, modify_memory_usage, &sysm_system_memory_usage);
#else
        _memman_free(mem, NULL, NULL);
#endif
}

//==============================================================================
/**
 * @brief Function monitor memory usage of network
 *
 * @param  size         block size
 *
 * @return pointer to allocated block or NULL if error
 */
//==============================================================================
void *_sysm_netmalloc(size_t size)
{
#if ((CONFIG_MONITOR_NETWORK_MEMORY_USAGE > 0) && (CONFIG_NETWORK_ENABLE > 0))

        if (  CONFIG_MONITOR_NETWORK_MEMORY_USAGE_LIMIT == 0
           || sysm_network_memory_usage + size <= CONFIG_MONITOR_NETWORK_MEMORY_USAGE_LIMIT ) {
                return _memman_malloc(size, modify_memory_usage, &sysm_network_memory_usage);
        } else {
                return NULL;
        }
#else
        return _memman_malloc(size, NULL, NULL);
#endif
}

//==============================================================================
/**
 * @brief Function monitor memory usage of network
 *
 * @param  count        count of items
 * @param  size         item size
 *
 * @return pointer to allocated block or NULL if error
 */
//==============================================================================
void *_sysm_netcalloc(size_t count, size_t size)
{
#if ((CONFIG_MONITOR_NETWORK_MEMORY_USAGE > 0) && (CONFIG_NETWORK_ENABLE > 0))

        if (  CONFIG_MONITOR_NETWORK_MEMORY_USAGE_LIMIT == 0
           || sysm_network_memory_usage + size <= CONFIG_MONITOR_NETWORK_MEMORY_USAGE_LIMIT ) {
                return _memman_calloc(count, size, modify_memory_usage, &sysm_network_memory_usage);
        } else {
                return NULL;
        }
#else
        return _memman_calloc(count, size, NULL, NULL);
#endif
}

//==============================================================================
/**
 * @brief Monitor memory freeing for network
 *
 * @param *mem          block to free
 */
//==============================================================================
void _sysm_netfree(void *mem)
{
#if ((CONFIG_MONITOR_NETWORK_MEMORY_USAGE > 0) && (CONFIG_NETWORK_ENABLE > 0))
        _memman_free(mem, modify_memory_usage, &sysm_network_memory_usage);
#else
        _memman_free(mem, NULL, NULL);
#endif
}

//==============================================================================
/**
 * @brief Function monitor memory usage of modules
 *
 * @param  size                 block size
 * @param  module_number        module number
 *
 * @return pointer to allocated block or NULL if error
 */
//==============================================================================
void *_sysm_modmalloc(size_t size, uint module_number)
{
#if (CONFIG_MONITOR_MODULE_MEMORY_USAGE > 0)
        if (module_number < _regdrv_number_of_modules) {
                return _memman_malloc(size, modify_module_memory_usage, reinterpret_cast(void*, module_number));
        } else {
                return NULL;
        }
#else
        UNUSED_ARG(module_number);
        return _memman_malloc(size, NULL, NULL);
#endif
}

//==============================================================================
/**
 * @brief Function monitor memory usage of modules
 *
 * @param  count                count of items
 * @param  size                 item size
 * @param  module_number        module number
 *
 * @return pointer to allocated block or NULL if error
 */
//==============================================================================
void *_sysm_modcalloc(size_t count, size_t size, uint module_number)
{
#if (CONFIG_MONITOR_MODULE_MEMORY_USAGE > 0)
        if (module_number < _regdrv_number_of_modules) {
                return _memman_calloc(count, size, modify_module_memory_usage, reinterpret_cast(void*, module_number));
        } else {
                return NULL;
        }
#else
        UNUSED_ARG(module_number);
        return _memman_calloc(count, size, NULL, NULL);
#endif
}


//==============================================================================
/**
 * @brief Monitor memory freeing for modules
 *
 * @param *mem                  block to free
 * @param  module_number        module number
 */
//==============================================================================
void _sysm_modfree(void *mem, uint module_number)
{
#if (CONFIG_MONITOR_MODULE_MEMORY_USAGE > 0)
        if (module_number < _regdrv_number_of_modules) {
                _memman_free(mem, modify_module_memory_usage, reinterpret_cast(void*, module_number));
        }
#else
        UNUSED_ARG(module_number);
        _memman_free(mem, NULL, NULL);
#endif
}

//==============================================================================
/**
 * @brief Function return memory usage of selected driver
 *
 * @param  module_number        module number
 *
 * @return used memory by selected driver, or -1 on error
 */
//==============================================================================
i32_t _sysm_get_used_memory_by_module(uint module_number)
{
#if (CONFIG_MONITOR_MODULE_MEMORY_USAGE > 0)
        if (module_number >= _regdrv_number_of_modules)
                return -1;
        else
                return sysm_module_memory_usage[module_number];
#else
        UNUSED_ARG(module_number);
        return 0;
#endif
}

//==============================================================================
/**
 * @brief Monitor memory allocation for specified task
 *
 * @param *taskhdl      task handle
 * @param  size         block size
 *
 * @return pointer to allocated block or NULL if error
 */
//==============================================================================
void *_sysm_tskmalloc_as(task_t *taskhdl, size_t size)
{
#if (CONFIG_MONITOR_TASK_MEMORY_USAGE > 0)
        void *mem = NULL;

        mutex_force_lock(sysm_resource_mtx);

        if (size == 0) {
                goto exit;
        }

        if (_sysm_is_task_exist(taskhdl) == false) {
                goto exit;
        }

        struct task_monitor_data *task_monitor_data = _task_get_monitor_data(taskhdl);
        if (!task_monitor_data) {
                goto exit;
        }

        mem_slot_chain_t *chain = &task_monitor_data->mem_chain;
        do {
                if (chain->used_slots < TASK_MEMORY_SLOTS) {
                        for (uint i = 0; i < TASK_MEMORY_SLOTS; i++) {
                                if (chain->mem_slot[i] != NULL)
                                        continue;

                                i32_t allocated = 0;
                                if ((mem = _memman_malloc(size, modify_memory_usage, &allocated))) {
                                        chain->mem_slot[i] = mem;
                                        chain->used_slots++;
                                        task_monitor_data->used_memory += allocated;
                                        sysm_programs_memory_usage     += allocated;
                                }

                                goto exit;
                        }
                } else if (chain->next == NULL) {
                        chain->next = _memman_calloc(1, sizeof(mem_slot_chain_t), NULL, NULL);
                        if (chain->next) {
                                chain->next->prev = chain;
                        }
                }
        } while ((chain = chain->next) != NULL);

        _printk("%s: malloc(): cannot create next memory slot chain!\n", _task_get_name());

exit:
        _mutex_unlock(sysm_resource_mtx);
        return mem;
#else
        UNUSED_ARG(taskhdl);
        return _memman_malloc(size, NULL, NULL);
#endif
}

//==============================================================================
/**
 * @brief Monitor memory allocation
 *
 * @param size          block size
 *
 * @return pointer to allocated block or NULL if error
 */
//==============================================================================
void *_sysm_tskmalloc(size_t size)
{
#if (CONFIG_MONITOR_TASK_MEMORY_USAGE > 0)
        return _sysm_tskmalloc_as(_task_get_handle(), size);
#else
        return _memman_malloc(size, NULL, NULL);
#endif
}

//==============================================================================
/**
 * @brief Monitor memory allocation for specified task
 *
 * @param *taskhdl      task handle
 * @param  nmemb        n members
 * @param  msize        member size
 *
 * @return pointer to allocated block or NULL if error
 */
//==============================================================================
void *_sysm_tskcalloc_as(task_t *taskhdl, size_t nmemb, size_t msize)
{
#if (CONFIG_MONITOR_TASK_MEMORY_USAGE > 0)
        void *ptr = _sysm_tskmalloc_as(taskhdl, nmemb * msize);

        if (ptr) {
                memset(ptr, 0, nmemb * msize);
        }

        return ptr;
#else
        UNUSED_ARG(taskhdl);
        return _memman_calloc(nmemb, msize, NULL, NULL);
#endif
}

//==============================================================================
/**
 * @brief Monitor memory allocation
 *
 * @param nmemb         n members
 * @param msize         member size
 *
 * @return pointer to allocated block or NULL if error
 */
//==============================================================================
void *_sysm_tskcalloc(size_t nmemb, size_t msize)
{
#if (CONFIG_MONITOR_TASK_MEMORY_USAGE > 0)
        void *ptr = _sysm_tskmalloc_as(_task_get_handle(), nmemb * msize);

        if (ptr) {
                memset(ptr, 0, nmemb * msize);
        }

        return ptr;
#else
        return _memman_calloc(nmemb, msize, NULL, NULL);
#endif
}

//==============================================================================
/**
 * @brief Monitor memory freeing for specified task
 *
 * @param *taskhdl      task handle
 * @param *mem          block to free
 */
//==============================================================================
void _sysm_tskfree_as(task_t *taskhdl, void *mem)
{
#if (CONFIG_MONITOR_TASK_MEMORY_USAGE > 0)
        mutex_force_lock(sysm_resource_mtx);

        if (!mem) {
                goto exit;
        }

        if (_sysm_is_task_exist(taskhdl) == false) {
                goto exit;
        }

        struct task_monitor_data *task_monitor_data = _task_get_monitor_data(taskhdl);
        if (!task_monitor_data) {
                goto exit;
        }

        mem_slot_chain_t *chain = &task_monitor_data->mem_chain;
        do {
                for (uint i = 0; i < TASK_MEMORY_SLOTS; i++) {
                        if (chain->mem_slot[i] == mem) {
                                size_t freed = 0;
                                _memman_free(mem, modify_memory_usage, &freed);
                                chain->mem_slot[i] = NULL;
                                task_monitor_data->used_memory += freed;
                                sysm_programs_memory_usage     += freed;
                                chain->used_slots--;

                                if (chain->used_slots == 0 && chain->prev != NULL) {
                                        chain->prev->next = chain->next;

                                        if (chain->next)
                                                chain->next->prev = chain->prev;

                                        _memman_free(chain, NULL, NULL);
                                }

                                goto exit;
                        }
                }
        } while ((chain = chain->next) != NULL);

        /* block not found */
        _fprintf(stdout, "*** Error in %s: double free or corruption: 0x%x ***\n", _task_get_name(), mem);
        _mutex_unlock(sysm_resource_mtx);
        _abort();

exit:
        _mutex_unlock(sysm_resource_mtx);
#else
        UNUSED_ARG(taskhdl);
        _memman_free(mem, NULL, NULL);
#endif
}

//==============================================================================
/**
 * @brief Monitor memory freeing
 *
 * @param *mem          block to free
 */
//==============================================================================
void _sysm_tskfree(void *mem)
{
#if (CONFIG_MONITOR_TASK_MEMORY_USAGE > 0)
        _sysm_tskfree_as(_task_get_handle(), mem);
#else
        _memman_free(mem, NULL, NULL);
#endif
}

//==============================================================================
/**
 * @brief Function open selected file
 *
 * @param *name         file path
 * @param *mode         file mode
 *
 * @retval NULL if file can't be created
 */
//==============================================================================
FILE *_sysm_fopen(const char *path, const char *mode)
{
#if (CONFIG_MONITOR_TASK_FILE_USAGE > 0)
        FILE *file = NULL;

        mutex_force_lock(sysm_resource_mtx);

        task_t *task = _task_get_handle();

        if (_sysm_is_task_exist(task) == false) {
                goto exit;
        }

        struct task_monitor_data *task_monitor_data = _task_get_monitor_data(task);
        if (!task_monitor_data) {
                errno = ESRCH;
                goto exit;
        }

        for (uint slot = 0; slot < TASK_FILE_SLOTS; slot++) {
                if (task_monitor_data->file_slot[slot] == NULL) {
                        file = _vfs_fopen(path, mode);

                        if (file) {
                                task_monitor_data->file_slot[slot] = file;
                                task_monitor_data->opened_files++;
                        }

                        goto exit;
                }
        }

        errno = EMFILE;

exit:
        _mutex_unlock(sysm_resource_mtx);
        return file;
#else
        return _vfs_fopen(path, mode);
#endif
}

//==============================================================================
/**
 * @brief Function reopen selected file
 *
 * @param *name         file path
 * @param *mode         file mode
 * @param *file         old file
 *
 * @retval NULL if file can't be created
 */
//==============================================================================
FILE *_sysm_freopen(const char *path, const char *mode, FILE *file)
{
#if (CONFIG_MONITOR_TASK_FILE_USAGE > 0)
        if (!path || !mode || !file) {
                errno = EINVAL;
                return NULL;
        }

        if (_sysm_fclose(file) == ESUCC) {
                return _sysm_fopen(path, mode);
        } else {
                return NULL;
        }
#else
        return _vfs_freopen(path, mode, file);
#endif
}

//==============================================================================
/**
 * @brief Function close opened file
 *
 * @param *file               pinter to file
 *
 * @retval 0                  file closed successfully
 * @retval -1                 file not closed
 */
//==============================================================================
int _sysm_fclose(FILE *file)
{
#if (CONFIG_MONITOR_TASK_FILE_USAGE > 0)
        if (!file) {
                errno = EINVAL;
                return EOF;
        }

        int                  status = EOF;
        task_t                   *task;
        struct task_monitor_data *task_monitor_data;

        mutex_force_lock(sysm_resource_mtx);

        task = _task_get_handle();

        if (_sysm_is_task_exist(task) == false) {
                goto exit;
        }

        if (!(task_monitor_data = _task_get_monitor_data(task))) {
                errno = ESRCH;
                goto exit;
        }

        for (uint slot = 0; slot < TASK_FILE_SLOTS; slot++) {
                if (task_monitor_data->file_slot[slot] == file) {
                        status = _vfs_fclose(file);

                        if (status == ESUCC) {
                                task_monitor_data->file_slot[slot] = NULL;
                                task_monitor_data->opened_files--;
                        }

                        goto exit;
                }
        }

        errno = ENOENT;

exit:
        _mutex_unlock(sysm_resource_mtx);
        return status;
#else
        return _vfs_fclose(file);
#endif
}

//==============================================================================
/**
 * @brief Function open selected directory
 *
 * @param *name         directory path
 *
 * @retval NULL if file can't be created
 */
//==============================================================================
DIR *_sysm_opendir(const char *path)
{
#if (CONFIG_MONITOR_TASK_FILE_USAGE > 0)
        DIR *dir = NULL;

        mutex_force_lock(sysm_resource_mtx);

        task_t *task = _task_get_handle();

        if (_sysm_is_task_exist(task) == false) {
                goto exit;
        }

        struct task_monitor_data *task_monitor_data;
        if (!(task_monitor_data = _task_get_monitor_data(task))) {
                errno = ESRCH;
                goto exit;
        }

        for (uint slot = 0; slot < TASK_DIR_SLOTS; slot++) {
                if (task_monitor_data->dir_slot[slot] == NULL) {
                        dir = _vfs_opendir(path);

                        if (dir) {
                                task_monitor_data->dir_slot[slot] = dir;
                                task_monitor_data->opened_files++;
                        }

                        goto exit;
                }
        }

        errno = EMFILE;

exit:
        _mutex_unlock(sysm_resource_mtx);
        return dir;
#else
        return _vfs_opendir(path);
#endif
}

//==============================================================================
/**
 * @brief Function close opened directory
 *
 * @param *DIR                pinter to directory
 *
 * @retval 0                  file closed successfully
 * @retval -1                 file not closed
 */
//==============================================================================
int _sysm_closedir(DIR *dir)
{
#if (CONFIG_MONITOR_TASK_FILE_USAGE > 0)
        int status = EOF;

        mutex_force_lock(sysm_resource_mtx);

        task_t *task = _task_get_handle();

        if (_sysm_is_task_exist(task) == false) {
                goto exit;
        }

        struct task_monitor_data *task_monitor_data = _task_get_monitor_data(task);
        if (!task_monitor_data) {
                errno = ESRCH;
                goto exit;
        }

        for (uint slot = 0; slot < TASK_DIR_SLOTS; slot++) {
                if (task_monitor_data->dir_slot[slot] == dir) {
                        status = _vfs_closedir(dir);

                        if (status == ESUCC) {
                                task_monitor_data->dir_slot[slot] = NULL;
                                task_monitor_data->opened_files--;
                        }

                        goto exit;
                }
        }

        errno = ENOENT;

exit:
        _mutex_unlock(sysm_resource_mtx);
        return status;
#else
        return _vfs_closedir(dir);
#endif
}

//==============================================================================
/**
 * @brief Function returns the CPU total time (used to calculate CPU load)
 *
 * @return CPU total time
 */
//==============================================================================
u32_t _sysm_get_total_CPU_usage(void)
{
#if (CONFIG_MONITOR_CPU_LOAD > 0)
        u32_t time     = CPU_total_time;
        CPU_total_time = 0;
        return time;
#else
        return 0;
#endif
}

//==============================================================================
/**
 * @brief Function disable CPU load measurement
 */
//==============================================================================
void _sysm_disable_CPU_load_measurement(void)
{
#if (CONFIG_MONITOR_CPU_LOAD > 0)
        CPU_load_enabled = false;
#endif
}

//==============================================================================
/**
 * @brief Function enable CPU load measurement
 */
//==============================================================================
void _sysm_enable_CPU_load_measurement(void)
{
#if (CONFIG_MONITOR_CPU_LOAD > 0)
        CPU_load_enabled = true;
#endif
}

//==============================================================================
/**
 * @brief Function called after task go to ready state
 */
//==============================================================================
void _sysm_task_switched_in(void)
{
#if (CONFIG_MONITOR_CPU_LOAD > 0)
        _cpuctl_reset_CPU_load_counter();
#endif
}

//==============================================================================
/**
 * @brief Function called when task go out ready state
 */
//==============================================================================
void _sysm_task_switched_out(void)
{
#if (CONFIG_MONITOR_CPU_LOAD > 0)
        if (CPU_load_enabled) {
                struct _task_data *tdata = _task_get_data();
                u32_t              cnt   = _cpuctl_get_CPU_load_counter_value();
                CPU_total_time          += cnt;

                if (tdata) {
                        tdata->f_cpu_usage += cnt;
                }
        }
#endif
}

//==============================================================================
/**
 * @brief  Function check if the kernel panic occurred in the last session
 * @param  show_msg     true: show error message
 * @return If kernel panic occured in the last session then true is returned,
 *         otherwise false.
 */
//==============================================================================
bool _sysm_kernel_panic_detect(bool show_msg)
{
#if ((CONFIG_SYSTEM_MSG_ENABLE > 0) && (CONFIG_PRINTF_ENABLE > 0))
        static const char *cause[] = {
               "SEGFAULT",
               "STACKOVF",
               "CPUFAULT",
               "UNKNOWN"
        };
#endif

        bool occurred = (  kernel_panic_descriptor->valid1 == _KERNEL_PANIC_DESC_VALID1
                       && kernel_panic_descriptor->valid2 == _KERNEL_PANIC_DESC_VALID2 );

        if (occurred) {
                if (show_msg) {
                        if (kernel_panic_descriptor->cause > _KERNEL_PANIC_DESC_CAUSE_UNKNOWN) {
                                kernel_panic_descriptor->cause = _KERNEL_PANIC_DESC_CAUSE_UNKNOWN;
                        }

                        kernel_panic_descriptor->task_name[CONFIG_RTOS_TASK_NAME_LEN - 1] = '\0';
                        if (strlen(kernel_panic_descriptor->task_name) == 0) {
                                strncpy(kernel_panic_descriptor->task_name, "<defected>", CONFIG_RTOS_TASK_NAME_LEN);
                        }

#if ((CONFIG_SYSTEM_MSG_ENABLE > 0) && (CONFIG_PRINTF_ENABLE > 0))
                        _printk(FONT_COLOR_RED"*** KERNEL PANIC ***"RESET_ATTRIBUTES"\n");
                        _printk("Cause: %s\n", cause[kernel_panic_descriptor->cause]);
                        _printk("Task : %s\n\n", kernel_panic_descriptor->task_name);
#endif
                        _sleep(2);
                }

                kernel_panic_descriptor->valid1 = 0;
                kernel_panic_descriptor->valid2 = 0;
        }

        return occurred;
}

//==============================================================================
/**
 * @brief  Function report kernel panic
 * @param  suggest_cause        suggested cause (STACKOVF has priority)
 * @return None
 */
//==============================================================================
void _sysm_kernel_panic_report(const char *task_name, enum _kernel_panic_desc_cause suggest_cause)
{
        strncpy(kernel_panic_descriptor->task_name, task_name, CONFIG_RTOS_TASK_NAME_LEN);

        if (suggest_cause == _KERNEL_PANIC_DESC_CAUSE_STACKOVF || _task_get_free_stack() == 0) {
                kernel_panic_descriptor->cause = _KERNEL_PANIC_DESC_CAUSE_STACKOVF;
        } else {
                kernel_panic_descriptor->cause = suggest_cause;
        }

        kernel_panic_descriptor->valid1 = _KERNEL_PANIC_DESC_VALID1;
        kernel_panic_descriptor->valid2 = _KERNEL_PANIC_DESC_VALID2;

        _cpuctl_restart_system();
}

/*==============================================================================
  End of file
==============================================================================*/
