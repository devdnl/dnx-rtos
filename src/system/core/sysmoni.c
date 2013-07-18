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

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Include files
==============================================================================*/
#include "core/sysmoni.h"
#include "core/list.h"
#include "core/io.h"
#include "drivers/driver_registration.h"
#include "kernel/kwrapper.h"
#include "portable/cpuctl.h"

/*==============================================================================
  Local symbolic constants/macros
==============================================================================*/
#define TASK_MEMORY_SLOTS               24
#define TASK_FILE_SLOTS                 8
#define TASK_DIR_SLOTS                  4

#define MTX_BLOCK_TIME                  10
#define force_lock_recursive_mutex(mtx) while (lock_recursive_mutex(mtx, MTX_BLOCK_TIME) != MUTEX_LOCKED)

/*==============================================================================
  Local types, enums definitions
==============================================================================*/
#if (SYSM_MONITOR_TASK_MEMORY_USAGE > 0)
typedef struct mem_slot_chain {
        int                    used_slots;
        void                  *mem_slot[TASK_MEMORY_SLOTS];
        struct mem_slot_chain *prev;
        struct mem_slot_chain *next;
} mem_slot_chain_t ;
#endif

/* task information */
struct task_monitor_data {
#if (SYSM_MONITOR_TASK_MEMORY_USAGE > 0)
        u32_t            used_memory;
        mem_slot_chain_t mem_chain;
#endif

#if (SYSM_MONITOR_TASK_FILE_USAGE > 0)
        uint   opened_files;
        FILE  *file_slot[TASK_FILE_SLOTS];
        dir_t *dir_slot[TASK_DIR_SLOTS];
#endif

#if (SYSM_MONITOR_TASK_MEMORY_USAGE == 0) && (SYSM_MONITOR_TASK_FILE_USAGE == 0)
        int dummy; /* used only to create structure when above fields doesn't exist */
#endif
};

/*==============================================================================
  Local function prototypes
==============================================================================*/

/*==============================================================================
  Local object definitions
==============================================================================*/
#if (  (SYSM_MONITOR_TASK_MEMORY_USAGE > 0) \
    || (SYSM_MONITOR_TASK_FILE_USAGE > 0  ) \
    || (SYSM_MONITOR_CPU_LOAD > 0    ) )
static list_t  *sysm_task_list;
static mutex_t *sysm_resource_mtx;
#endif

#if (SYSM_MONITOR_KERNEL_MEMORY_USAGE > 0)
static i32_t sysm_kernel_memory_usage;
#endif

#if (SYSM_MONITOR_SYSTEM_MEMORY_USAGE > 0)
static i32_t sysm_system_memory_usage = (i32_t)CONFIG_RAM_SIZE - (i32_t)CONFIG_HEAP_SIZE;
#endif

#if (SYSM_MONITOR_MODULE_MEMORY_USAGE > 0)
static i32_t  sysm_modules_memory_usage;
static i32_t *sysm_module_memory_usage;
#endif

#if (SYSM_MONITOR_TASK_MEMORY_USAGE > 0)
static i32_t sysm_programs_memory_usage;
#endif

/*==============================================================================
  Exported object definitions
==============================================================================*/

/*==============================================================================
  Function definitions
==============================================================================*/

//==============================================================================
/**
 * @brief Initialize module
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
#if (  (SYSM_MONITOR_TASK_MEMORY_USAGE > 0  ) \
    || (SYSM_MONITOR_TASK_FILE_USAGE > 0    ) \
    || (SYSM_MONITOR_CPU_LOAD > 0           ) )
stdret_t sysm_init(void)
{
        _cpuctl_init_CPU_load_timer();

        sysm_task_list    = new_list();
        sysm_resource_mtx = new_recursive_mutex();

#if (SYSM_MONITOR_MODULE_MEMORY_USAGE > 0)
        sysm_module_memory_usage = sysm_syscalloc(_regdrv_number_of_modules, sizeof(i32_t));
#endif

#if (SYSM_MONITOR_MODULE_MEMORY_USAGE > 0)
        if (!sysm_task_list || !sysm_resource_mtx || !sysm_module_memory_usage)
#else
        if (!sysm_task_list || !sysm_resource_mtx)
#endif
                return STD_RET_ERROR;
        else
                return STD_RET_OK;
}
#endif

//==============================================================================
/**
 * @brief Function check if task is already in the monitor list
 *
 * @param *taskhdl      task handle
 *
 * @retval TRUE         task exist
 * @retval FALSE        task does not exist
 */
//==============================================================================
#if (  (SYSM_MONITOR_TASK_MEMORY_USAGE > 0) \
    || (SYSM_MONITOR_TASK_FILE_USAGE > 0  ) \
    || (SYSM_MONITOR_CPU_LOAD > 0         ) )
bool sysm_is_task_exist(task_t *taskhdl)
{
        i32_t item  = -1;
        bool  exist = FALSE;

        force_lock_recursive_mutex(sysm_resource_mtx);

        if (taskhdl) {
                if (list_get_iditem_No(sysm_task_list, (u32_t)taskhdl, &item) == STD_RET_OK) {
                        if (item >= 0) {
                                exist = TRUE;
                        }
                }
        }

        unlock_recursive_mutex(sysm_resource_mtx);

        return exist;
}
#endif

//==============================================================================
/**
 * @brief Function start task monitoring
 *
 * @param *taskhdl      task handle
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
#if (  (SYSM_MONITOR_TASK_MEMORY_USAGE > 0) \
    || (SYSM_MONITOR_TASK_FILE_USAGE > 0  ) \
    || (SYSM_MONITOR_CPU_LOAD > 0         ) )
stdret_t sysm_start_task_monitoring(task_t *taskhdl)
{
        struct task_monitor_data *tmdata;

        force_lock_recursive_mutex(sysm_resource_mtx);

        if (sysm_is_task_exist(taskhdl) == TRUE) {
                goto exit_error;
        }

        if ((tmdata = sysm_syscalloc(1, sizeof(struct task_monitor_data)))) {

                if (list_add_item(sysm_task_list, (u32_t)taskhdl, NULL) < 0) {
                        sysm_sysfree(tmdata);
                        _set_task_monitor_data(taskhdl, NULL);
                        goto exit_error;
                } else {
                        _set_task_monitor_data(taskhdl, tmdata);
                        unlock_recursive_mutex(sysm_resource_mtx);
                        return STD_RET_OK;
                }
        }

        exit_error:
        unlock_recursive_mutex(sysm_resource_mtx);
        return STD_RET_ERROR;
}
#endif

//==============================================================================
/**
 * @brief Function stops task monitoring (free all resources)
 *
 * @param *taskhdl      task handle
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
#if (  (SYSM_MONITOR_TASK_MEMORY_USAGE > 0) \
    || (SYSM_MONITOR_TASK_FILE_USAGE > 0  ) \
    || (SYSM_MONITOR_CPU_LOAD > 0         ) )
stdret_t sysm_stop_task_monitoring(task_t *taskhdl)
{
        struct task_monitor_data *task_monitor_data;

        force_lock_recursive_mutex(sysm_resource_mtx);

        if (sysm_is_task_exist(taskhdl) == FALSE) {
                goto exit_error;
        }

        if (!(task_monitor_data = _get_task_monitor_data(taskhdl))) {
                goto exit_error;
        }

#if (SYSM_MONITOR_TASK_MEMORY_USAGE > 0)
        mem_slot_chain_t *chain;
        for (chain = &task_monitor_data->mem_chain; chain->next != NULL; chain = chain->next) {
                /* go to the last memory slot chain */
        }

        int to_free = 0;
        do {
                if (chain->next) {
                        memman_free(chain->next);
                }

                if (chain->used_slots > 0) {
                        for (uint i = 0; i < TASK_MEMORY_SLOTS; i++) {
                                if (chain->mem_slot[i]) {
                                        to_free += memman_free(chain->mem_slot[i]);
                                }
                        }
                }
        } while ((chain = chain->prev) != NULL);

        if (_get_this_task_data()->f_program) {
                sysm_programs_memory_usage -= to_free;
        }
#endif

#if (SYSM_MONITOR_TASK_FILE_USAGE > 0)
        for (uint slot = 0; slot < TASK_FILE_SLOTS; slot++) {
                if (task_monitor_data->file_slot[slot]) {
                        vfs_fclose(task_monitor_data->file_slot[slot]);
                }
        }

        for (uint slot = 0; slot < TASK_DIR_SLOTS; slot++) {
                if (task_monitor_data->dir_slot[slot]) {
                        vfs_closedir(task_monitor_data->dir_slot[slot]);
                }
        }
#endif

        sysm_sysfree(task_monitor_data);
        _set_task_monitor_data(taskhdl, NULL);
        list_rm_iditem(sysm_task_list, (u32_t)taskhdl);
        unlock_recursive_mutex(sysm_resource_mtx);
        return STD_RET_OK;

exit_error:
        unlock_recursive_mutex(sysm_resource_mtx);
        return STD_RET_ERROR;
}
#endif

//==============================================================================
/**
 * @brief Function gets task status
 *
 * @param *taskHdl      task handle
 * @param *stat         status result
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
#if (  (SYSM_MONITOR_TASK_MEMORY_USAGE > 0) \
    || (SYSM_MONITOR_TASK_FILE_USAGE > 0  ) \
    || (SYSM_MONITOR_CPU_LOAD > 0         ) )
stdret_t sysm_get_task_stat(task_t *taskhdl, struct sysmoni_taskstat *stat)
{
        struct task_monitor_data *tmdata;

        force_lock_recursive_mutex(sysm_resource_mtx);

        if (!stat) {
                goto exit_error;
        }

        if (sysm_is_task_exist(taskhdl) == FALSE) {
                goto exit_error;
        }

        if (!(tmdata = _get_task_monitor_data(taskhdl))) {
                goto exit_error;
        }

        enter_critical_section();
        stat->cpu_usage = _get_task_data(taskhdl)->f_cpu_usage;
        _get_task_data(taskhdl)->f_cpu_usage = 0;
        exit_critical_section();

        stat->free_stack   = get_task_free_stack(taskhdl);
#if (SYSM_MONITOR_TASK_MEMORY_USAGE > 0)
        stat->memory_usage = tmdata->used_memory;
#else
        stat->memory_usage = 0;
#endif
#if (SYSM_MONITOR_TASK_FILE_USAGE > 0)
        stat->opened_files = tmdata->opened_files;
#else
        stat->opened_files = 0;
#endif
        stat->priority     = get_task_priority(taskhdl);
        stat->task_handle  = taskhdl;
        stat->task_name    = get_task_name(taskhdl);

        unlock_recursive_mutex(sysm_resource_mtx);
        return STD_RET_OK;

        exit_error:
        unlock_recursive_mutex(sysm_resource_mtx);
        return STD_RET_ERROR;
}
#endif

//==============================================================================
/**
 * @brief Function gets task statistics
 *
 * @param  item   task item
 * @param *stat   task statistics
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
#if (  (SYSM_MONITOR_TASK_MEMORY_USAGE > 0) \
    || (SYSM_MONITOR_TASK_FILE_USAGE > 0  ) \
    || (SYSM_MONITOR_CPU_LOAD > 0         ) )
stdret_t sysm_get_ntask_stat(i32_t item, struct sysmoni_taskstat *stat)
{
        task_t *task;

        force_lock_recursive_mutex(sysm_resource_mtx);

        if (list_get_nitem_ID(sysm_task_list, item, (u32_t *)&task) != STD_RET_OK) {
                goto exit_error;
        }

        if (sysm_get_task_stat(task, stat) != STD_RET_OK) {
                goto exit_error;
        }

        unlock_recursive_mutex(sysm_resource_mtx);
        return STD_RET_OK;

        exit_error:
        unlock_recursive_mutex(sysm_resource_mtx);
        return STD_RET_ERROR;
}
#endif

//==============================================================================
/**
 * @brief Function gets information about used memory
 *
 * @param[out] *mem_info        memory information
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
#if (  (SYSM_MONITOR_TASK_MEMORY_USAGE > 0) \
    || (SYSM_MONITOR_TASK_FILE_USAGE > 0  ) \
    || (SYSM_MONITOR_CPU_LOAD > 0         ) )
stdret_t sysm_get_used_memory(struct sysmoni_used_memory *mem_info)
{
        if (!mem_info)
                return STD_RET_ERROR;

        mem_info->used_kernel_memory   = sysm_kernel_memory_usage;
        mem_info->used_modules_memory  = sysm_modules_memory_usage;
        mem_info->used_programs_memory = sysm_programs_memory_usage;
        mem_info->used_system_memory   = sysm_system_memory_usage;
        return STD_RET_OK;
}
#endif

//==============================================================================
/**
 * @brief Function return number of monitor tasks
 *
 * @return number of monitor tasks
 */
//==============================================================================
#if (  (SYSM_MONITOR_TASK_MEMORY_USAGE > 0) \
    || (SYSM_MONITOR_TASK_FILE_USAGE > 0  ) \
    || (SYSM_MONITOR_CPU_LOAD > 0         ) )
int sysm_get_number_of_monitored_tasks(void)
{
        int task_count;

        force_lock_recursive_mutex(sysm_resource_mtx);
        task_count = list_get_item_count(sysm_task_list);
        unlock_recursive_mutex(sysm_resource_mtx);

        return task_count;
}
#endif

//==============================================================================
/**
 * @brief Function monitor memory usage of kernel
 *
 * @param  size         block size
 *
 * @return pointer to allocated block or NULL if error
 */
//==============================================================================
#if (SYSM_MONITOR_KERNEL_MEMORY_USAGE > 0)
void *sysm_kmalloc(size_t size)
{
      size_t allocated;
      void *p = memman_malloc(size, &allocated);
      sysm_kernel_memory_usage += allocated;
      return p;
}
#endif

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
#if (SYSM_MONITOR_KERNEL_MEMORY_USAGE > 0)
void *sysm_kcalloc(size_t count, size_t size)
{
        size_t allocated;
        void *p = memman_calloc(count, size, &allocated);
        sysm_kernel_memory_usage += allocated;
        return p;
}
#endif

//==============================================================================
/**
 * @brief Monitor memory freeing for kernel
 *
 * @param *mem          block to free
 */
//==============================================================================
#if (SYSM_MONITOR_KERNEL_MEMORY_USAGE > 0)
void sysm_kfree(void *mem)
{
        sysm_kernel_memory_usage -= memman_free(mem);
}
#endif

//==============================================================================
/**
 * @brief Function monitor memory usage of system
 *
 * @param  size         block size
 *
 * @return pointer to allocated block or NULL if error
 */
//==============================================================================
#if (SYSM_MONITOR_SYSTEM_MEMORY_USAGE > 0)
void *sysm_sysmalloc(size_t size)
{
      size_t allocated;
      void *p = memman_malloc(size, &allocated);
      sysm_system_memory_usage += allocated;
      return p;
}
#endif

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
#if (SYSM_MONITOR_SYSTEM_MEMORY_USAGE > 0)
void *sysm_syscalloc(size_t count, size_t size)
{
        size_t allocated;
        void *p = memman_calloc(count, size, &allocated);
        sysm_system_memory_usage += allocated;
        return p;
}
#endif

//==============================================================================
/**
 * @brief Monitor memory freeing for system
 *
 * @param *mem          block to free
 */
//==============================================================================
#if (SYSM_MONITOR_SYSTEM_MEMORY_USAGE > 0)
void sysm_sysfree(void *mem)
{
        sysm_system_memory_usage -= memman_free(mem);
}
#endif

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
#if (SYSM_MONITOR_MODULE_MEMORY_USAGE > 0)
void *sysm_modmalloc(size_t size, uint module_number)
{
        size_t allocated;
        void   *p = NULL;

        if (module_number < _regdrv_number_of_modules) {
                p = memman_malloc(size, &allocated);
                sysm_modules_memory_usage += allocated;
                sysm_module_memory_usage[module_number] += allocated;
        }

        return p;
}
#endif

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
#if (SYSM_MONITOR_MODULE_MEMORY_USAGE > 0)
void *sysm_modcalloc(size_t count, size_t size, uint module_number)
{
        size_t allocated;
        void   *p = NULL;

        if (module_number < _regdrv_number_of_modules) {
                p = memman_calloc(count, size, &allocated);
                sysm_modules_memory_usage += allocated;
                sysm_module_memory_usage[module_number] += allocated;
        }

        return p;
}
#endif

//==============================================================================
/**
 * @brief Monitor memory freeing for modules
 *
 * @param *mem                  block to free
 * @param  module_number        module number
 */
//==============================================================================
#if (SYSM_MONITOR_MODULE_MEMORY_USAGE > 0)
void sysm_modfree(void *mem, uint module_number)
{
        if (module_number < _regdrv_number_of_modules) {
                size_t freed = memman_free(mem);
                sysm_modules_memory_usage -= freed;
                sysm_module_memory_usage[module_number] -= freed;
        }
}
#endif

//==============================================================================
/**
 * @brief Function return memory usage of selected driver
 *
 * @param  module_number        module number
 *
 * @return used memory by selected driver
 */
//==============================================================================
#if (SYSM_MONITOR_MODULE_MEMORY_USAGE > 0)
i32_t sysm_get_used_memory_by_module(uint module_number)
{
        if (module_number >= _regdrv_number_of_modules)
                return 0;
        else
                return sysm_module_memory_usage[module_number];
}
#endif


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
#if (SYSM_MONITOR_TASK_MEMORY_USAGE > 0)
void *sysm_tskmalloc_as(task_t *taskhdl, size_t size)
{
        void                     *mem = NULL;
        struct task_monitor_data *task_monitor_data;
        mem_slot_chain_t         *chain;
        size_t                   allocated;

        force_lock_recursive_mutex(sysm_resource_mtx);

        if (size == 0) {
                goto exit;
        }

        if (sysm_is_task_exist(taskhdl) == FALSE) {
                goto exit;
        }

        if (!(task_monitor_data = _get_task_monitor_data(taskhdl))) {
                goto exit;
        }

        chain = &task_monitor_data->mem_chain;
        do {
                if (chain->used_slots < TASK_MEMORY_SLOTS) {
                        for (uint i = 0; i < TASK_MEMORY_SLOTS; i++) {
                                if (chain->mem_slot[i] != NULL)
                                        continue;

                                if ((mem = memman_malloc(size, &allocated))) {
                                        chain->mem_slot[i] = mem;
                                        chain->used_slots++;
                                        task_monitor_data->used_memory += allocated;
                                        sysm_programs_memory_usage     += allocated;
                                }

                                goto exit;
                        }
                } else if (chain->next == NULL) {
                        chain->next = memman_calloc(1, sizeof(mem_slot_chain_t), NULL);
                        if (chain->next) {
                                chain->next->prev = chain;
                        }
                }
        } while ((chain = chain->next) != NULL);

        printk("%s: malloc(): cannot create the next memory slot chain!\n", get_this_task_name());

        exit:
        unlock_recursive_mutex(sysm_resource_mtx);
        return mem;
}
#endif

//==============================================================================
/**
 * @brief Monitor memory allocation
 *
 * @param size          block size
 *
 * @return pointer to allocated block or NULL if error
 */
//==============================================================================
#if (SYSM_MONITOR_TASK_MEMORY_USAGE > 0)
void *sysm_tskmalloc(size_t size)
{
        return sysm_tskmalloc_as(get_task_handle(), size);
}
#endif

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
#if (SYSM_MONITOR_TASK_MEMORY_USAGE > 0)
void *sysm_tskcalloc_as(task_t *taskhdl, size_t nmemb, size_t msize)
{
        void *ptr = sysm_tskmalloc_as(taskhdl, nmemb * msize);

        if (ptr) {
                memset(ptr, 0, nmemb * msize);
        }

        return ptr;
}
#endif

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
#if (SYSM_MONITOR_TASK_MEMORY_USAGE > 0)
void *sysm_tskcalloc(size_t nmemb, size_t msize)
{
        void *ptr = sysm_tskmalloc_as(get_task_handle(), nmemb * msize);

        if (ptr) {
                memset(ptr, 0, nmemb * msize);
        }

        return ptr;
}
#endif

//==============================================================================
/**
 * @brief Monitor memory freeing for specified task
 *
 * @param *taskhdl      task handle
 * @param *mem          block to free
 */
//==============================================================================
#if (SYSM_MONITOR_TASK_MEMORY_USAGE > 0)
void sysm_tskfree_as(task_t *taskhdl, void *mem)
{
        struct task_monitor_data *task_monitor_data;
        mem_slot_chain_t         *chain;
        size_t                   freed;

        force_lock_recursive_mutex(sysm_resource_mtx);

        if (!mem) {
                goto exit;
        }

        if (sysm_is_task_exist(taskhdl) == FALSE) {
                goto exit;
        }

        if (!(task_monitor_data = _get_task_monitor_data(taskhdl))) {
                goto exit;
        }

        chain = &task_monitor_data->mem_chain;
        do {
                for (uint i = 0; i < TASK_MEMORY_SLOTS; i++) {
                        if (chain->mem_slot[i] == mem) {
                                freed = memman_free(mem);
                                chain->mem_slot[i] = NULL;
                                task_monitor_data->used_memory -= freed;
                                sysm_programs_memory_usage     -= freed;
                                chain->used_slots--;

                                if (chain->used_slots == 0 && chain->prev != NULL) {
                                        chain->prev->next = chain->next;

                                        if (chain->next)
                                                chain->next->prev = chain->prev;

                                        memman_free(chain);
                                }

                                goto exit;
                        }
                }
        } while ((chain = chain->next) != NULL);

        printk("%s: free(): address does not exist!\n", get_this_task_name());

        exit:
        unlock_recursive_mutex(sysm_resource_mtx);
}
#endif

//==============================================================================
/**
 * @brief Monitor memory freeing
 *
 * @param *mem          block to free
 */
//==============================================================================
#if (SYSM_MONITOR_TASK_MEMORY_USAGE > 0)
void sysm_tskfree(void *mem)
{
        sysm_tskfree_as(get_task_handle(), mem);
}
#endif

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
#if (SYSM_MONITOR_TASK_FILE_USAGE > 0)
FILE *sysm_fopen(const char *path, const char *mode)
{
        FILE *file = NULL;
        task_t *task;
        struct task_monitor_data *task_monitor_data;

        force_lock_recursive_mutex(sysm_resource_mtx);

        task = get_task_handle();

        if (sysm_is_task_exist(task) == FALSE) {
                goto exit;
        }

        if (!(task_monitor_data = _get_task_monitor_data(task))) {
                goto exit;
        }

        for (uint slot = 0; slot < TASK_FILE_SLOTS; slot++) {
                if (task_monitor_data->file_slot[slot] == NULL) {
                        file = vfs_fopen(path, mode);

                        if (file) {
                                task_monitor_data->file_slot[slot] = file;
                                task_monitor_data->opened_files++;
                        }

                        goto exit;
                }
        }

        printk("%s: Not enough free slots to open file!\n", get_this_task_name());

        exit:
        unlock_recursive_mutex(sysm_resource_mtx);
        return file;
}
#endif

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
#if (SYSM_MONITOR_TASK_FILE_USAGE > 0)
FILE *sysm_freopen(const char *path, const char *mode, FILE *file)
{
        if (sysm_fclose(file) == STD_RET_OK) {
                return sysm_fopen(path, mode);
        } else {
                return NULL;
        }
}
#endif

//==============================================================================
/**
 * @brief Function close opened file
 *
 * @param *file               pinter to file
 *
 * @retval STD_RET_OK         file closed successfully
 * @retval STD_RET_ERROR      file not closed
 */
//==============================================================================
#if (SYSM_MONITOR_TASK_FILE_USAGE > 0)
stdret_t sysm_fclose(FILE *file)
{
        stdret_t status = STD_RET_ERROR;
        task_t *task;
        struct task_monitor_data *task_monitor_data;

        force_lock_recursive_mutex(sysm_resource_mtx);

        task = get_task_handle();

        if (sysm_is_task_exist(task) == FALSE) {
                goto exit;
        }

        if (!(task_monitor_data = _get_task_monitor_data(task))) {
                goto exit;
        }

        for (uint slot = 0; slot < TASK_FILE_SLOTS; slot++) {
                if (task_monitor_data->file_slot[slot] == file) {
                        status = vfs_fclose(file);

                        if (status == STD_RET_OK) {
                                task_monitor_data->file_slot[slot] = NULL;
                                task_monitor_data->opened_files--;
                        }

                        goto exit;
                }
        }

        printk("%s: File does not exist or closed!\n", get_this_task_name());

        exit:
        unlock_recursive_mutex(sysm_resource_mtx);
        return status;
}
#endif

//==============================================================================
/**
 * @brief Function open selected directory
 *
 * @param *name         directory path
 *
 * @retval NULL if file can't be created
 */
//==============================================================================
#if (SYSM_MONITOR_TASK_FILE_USAGE > 0)
dir_t *sysm_opendir(const char *path)
{
        dir_t  *dir = NULL;
        task_t *task;
        struct task_monitor_data *task_monitor_data;

        force_lock_recursive_mutex(sysm_resource_mtx);

        task = get_task_handle();

        if (sysm_is_task_exist(task) == FALSE) {
                goto exit;
        }

        if (!(task_monitor_data = _get_task_monitor_data(task))) {
                goto exit;
        }

        for (uint slot = 0; slot < TASK_DIR_SLOTS; slot++) {
                if (task_monitor_data->dir_slot[slot] == NULL) {
                        dir = vfs_opendir(path);

                        if (dir) {
                                task_monitor_data->dir_slot[slot] = dir;
                                task_monitor_data->opened_files++;
                        }

                        goto exit;
                }
        }

        printk("%s: Not enough free slots to open directory!\n", get_this_task_name());

        exit:
        unlock_recursive_mutex(sysm_resource_mtx);
        return dir;
}
#endif

//==============================================================================
/**
 * @brief Function close opened directory
 *
 * @param *DIR                pinter to directory
 *
 * @retval STD_RET_OK         file closed successfully
 * @retval STD_RET_ERROR      file not closed
 */
//==============================================================================
#if (SYSM_MONITOR_TASK_FILE_USAGE > 0)
extern stdret_t sysm_closedir(dir_t *dir)
{
        stdret_t status = STD_RET_ERROR;
        task_t *task;
        struct task_monitor_data *task_monitor_data;

        force_lock_recursive_mutex(sysm_resource_mtx);

        task = get_task_handle();

        if (sysm_is_task_exist(task) == FALSE) {
                goto exit;
        }

        if (!(task_monitor_data = _get_task_monitor_data(task))) {
                goto exit;
        }

        for (uint slot = 0; slot < TASK_DIR_SLOTS; slot++) {
                if (task_monitor_data->dir_slot[slot] == dir) {
                        status = vfs_closedir(dir);

                        if (status == STD_RET_OK) {
                                task_monitor_data->dir_slot[slot] = NULL;
                                task_monitor_data->opened_files--;
                        }

                        goto exit;
                }
        }

        printk("%s: Directory does not exist or closed!\n", get_this_task_name());

        exit:
        unlock_recursive_mutex(sysm_resource_mtx);
        return status;
}
#endif

//==============================================================================
/**
 * @brief Function returns the CPU total time (used to calculate CPU load)
 *
 * @return CPU total time
 */
//==============================================================================
#if (SYSM_MONITOR_CPU_LOAD > 0)
u32_t sysm_get_total_CPU_usage(void)
{
        return _cpuctl_get_CPU_total_time();
}
#endif

//==============================================================================
/**
 * @brief Function clears the CPU total time
 */
//==============================================================================
#if (SYSM_MONITOR_CPU_LOAD > 0)
void sysm_clear_total_CPU_usage(void)
{
        _cpuctl_clear_CPU_total_time();
}
#endif

//==============================================================================
/**
 * @brief Function called after task go to ready state
 */
//==============================================================================
#if (SYSM_MONITOR_CPU_LOAD > 0)
void sysm_task_switched_in(void)
{
        _cpuctl_clear_CPU_load_timer();
}
#endif

//==============================================================================
/**
 * @brief Function called when task go out ready state
 */
//==============================================================================
#if (SYSM_MONITOR_CPU_LOAD > 0)
void sysm_task_switched_out(void)
{
        struct task_data *tdata = _get_this_task_data();
        u32_t             cnt   = _cpuctl_get_CPU_load_timer();

        if (tdata) {
                tdata->f_cpu_usage += cnt;
        }
}
#endif

#ifdef __cplusplus
}
#endif

/*==============================================================================
  End of file
==============================================================================*/
