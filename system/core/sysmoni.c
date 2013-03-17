/*=========================================================================*//**
@file    sysmoni.c

@author  Daniel Zorychta

@brief   This module is used to monitoring system

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


*//*==========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Include files
==============================================================================*/
#include "sysmoni.h"
#include "dlist.h"
#include "cpuctl.h"
#include "oswrap.h"
#include "io.h"
#include <string.h>

/*==============================================================================
  Local symbolic constants/macros
==============================================================================*/
#define calloc(nmemb, msize)            memman_calloc(nmemb, msize, NULL)
#define malloc(size)                    memman_malloc(size, NULL)
#define free(mem)                       memman_free(mem)

#define MEM_BLOCK_COUNT                 4
#define MEM_ADR_IN_BLOCK                7

#define FILE_BLOCK_COUNT                3
#define FILES_OPENED_IN_BLOCK           7

#define DIR_BLOCK_COUNT                 1
#define DIRS_OPENED_IN_BLOCK            3

#define MTX_BLOCK_TIME                  10

#define force_lock_recursive_mutex(mtx) while (lock_recursive_mutex(mtx, MTX_BLOCK_TIME) != MUTEX_LOCKED)

/*==============================================================================
  Local types, enums definitions
==============================================================================*/
/* task information */
struct task_monitor_data {
    #if (SYSM_MONITOR_TASK_MEMORY_USAGE > 0) || (SYSM_MONITOR_FILE_USAGE > 0)
        bool_t fast_monitoring;
    #endif

    #if (SYSM_MONITOR_TASK_MEMORY_USAGE > 0)
        u32_t used_memory;

        struct memBlock {
                bool_t full;

                struct memSlot {
                        void *addr;
                        u32_t size;
                } mslot[MEM_ADR_IN_BLOCK];

        }*mblock[MEM_BLOCK_COUNT];
    #endif

    #if (SYSM_MONITOR_FILE_USAGE > 0)
        uint opened_files;

        struct fileBlock {
                bool_t full;

                struct fileSlot {
                        file_t *file;
                } fslot[FILES_OPENED_IN_BLOCK];

        }*fblock[FILE_BLOCK_COUNT];

        struct dirBlock {
                bool_t full;

                struct dirSlot {
                        dir_t *dir;
                } dslot[DIRS_OPENED_IN_BLOCK];
        }*dblock[DIR_BLOCK_COUNT];
    #endif
};

/*==============================================================================
  Local function prototypes
==============================================================================*/

/*==============================================================================
  Local object definitions
==============================================================================*/
#if (  (SYSM_MONITOR_TASK_MEMORY_USAGE > 0) \
    || (SYSM_MONITOR_FILE_USAGE > 0  ) \
    || (SYSM_MONITOR_CPU_LOAD > 0    ) )
static list_t  *sysm_task_list;
static mutex_t *sysm_resource_mtx;
#endif

#if (SYSM_MONITOR_KERNEL_MEMORY_USAGE > 0)
static i32_t sysm_kernel_memory_usage;
#endif

#if (SYSM_MONITOR_SYSTEM_MEMORY_USAGE > 0)
static i32_t sysm_system_memory_usage;
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
#if (  (SYSM_MONITOR_TASK_MEMORY_USAGE > 0) \
    || (SYSM_MONITOR_FILE_USAGE > 0  ) \
    || (SYSM_MONITOR_CPU_LOAD > 0    ) )
stdret_t sysm_init(void)
{
        cpuctl_init_CPU_load_timer();

        sysm_task_list    = new_list();
        sysm_resource_mtx = new_recursive_mutex();

        if (!sysm_task_list || !sysm_resource_mtx)
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
bool_t sysm_is_task_exist(task_t *taskhdl)
{
        i32_t  item  = -1;
        bool_t exist = FALSE;

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
    || (SYSM_MONITOR_FILE_USAGE > 0  ) \
    || (SYSM_MONITOR_CPU_LOAD > 0    ) )
stdret_t sysm_start_task_monitoring(task_t *taskhdl)
{
        struct task_monitor_data *tmdata;

        force_lock_recursive_mutex(sysm_resource_mtx);

        if (sysm_is_task_exist(taskhdl) == TRUE) {
                goto exit_error;
        }

        if ((tmdata = calloc(1, sizeof(struct task_monitor_data)))) {

                if (list_add_item(sysm_task_list, (u32_t)taskhdl, NULL) < 0) {
                        free(tmdata);
                        set_task_monitor_data(taskhdl, NULL);
                        goto exit_error;
                } else {
                        set_task_monitor_data(taskhdl, tmdata);
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
 * @brief Function stops task monitoring
 *
 * @param *taskhdl      task handle
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
#if (  (SYSM_MONITOR_TASK_MEMORY_USAGE > 0) \
    || (SYSM_MONITOR_FILE_USAGE > 0  ) \
    || (SYSM_MONITOR_CPU_LOAD > 0    ) )
stdret_t sysm_stop_task_monitoring(task_t *taskhdl)
{
        struct task_monitor_data *task_monitor_data;
        struct memSlot           *mslot;
        struct fileSlot          *fslot;
        struct dirSlot           *dslot;

        force_lock_recursive_mutex(sysm_resource_mtx);

        if (sysm_is_task_exist(taskhdl) == FALSE) {
                goto exit_error;
        }

        if (!(task_monitor_data = get_task_monitor_data(taskhdl))) {
                goto exit_error;
        }

    #if (SYSM_MONITOR_TASK_MEMORY_USAGE > 0)
        for (u32_t block = 0; block < MEM_BLOCK_COUNT; block++) {
                if (task_monitor_data->mblock[block] == NULL) {
                        continue;
                }

                for (u32_t slot = 0; slot < MEM_ADR_IN_BLOCK; slot++) {
                        mslot = &task_monitor_data->mblock[block]->mslot[slot];

                        if (mslot->addr) {
                                free(mslot->addr);
                                mslot->size = 0;
                        }
                }

                free(task_monitor_data->mblock[block]);
                task_monitor_data->mblock[block] = NULL;
        }
    #endif

    #if (SYSM_MONITOR_FILE_USAGE > 0)
        for (u32_t block = 0; block < FILE_BLOCK_COUNT; block++) {
                if (task_monitor_data->fblock[block] == NULL) {
                        continue;
                }

                for (u32_t slot = 0; slot < FILES_OPENED_IN_BLOCK; slot++) {
                        fslot = &task_monitor_data->fblock[block]->fslot[slot];

                        if (fslot->file) {
                                vfs_fclose(fslot->file);
                        }
                }

                free(task_monitor_data->fblock[block]);
                task_monitor_data->fblock[block] = NULL;
        }

        for (u32_t block = 0; block < DIR_BLOCK_COUNT; block++) {
                if (task_monitor_data->dblock[block] == NULL) {
                        continue;
                }

                for (u32_t slot = 0; slot < DIRS_OPENED_IN_BLOCK; slot++) {
                        dslot = &task_monitor_data->dblock[block]->dslot[slot];

                        if (dslot->dir) {
                                vfs_closedir(dslot->dir);
                        }
                }

                free(task_monitor_data->fblock[block]);
                task_monitor_data->fblock[block] = NULL;
        }
   #endif

        free(task_monitor_data);
        set_task_monitor_data(taskhdl, NULL);
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
 * @brief Function returns the CPU total time (used to calculate CPU load)
 *
 * @return CPU total time
 */
//==============================================================================
#if (  (SYSM_MONITOR_TASK_MEMORY_USAGE > 0) \
    || (SYSM_MONITOR_FILE_USAGE > 0  ) \
    || (SYSM_MONITOR_CPU_LOAD > 0    ) )
u32_t sysm_get_total_CPU_usage(void)
{
        return cpuctl_get_CPU_total_time();
}
#endif

//==============================================================================
/**
 * @brief Function clears the CPU total time
 */
//==============================================================================
#if (  (SYSM_MONITOR_TASK_MEMORY_USAGE > 0) \
    || (SYSM_MONITOR_FILE_USAGE > 0  ) \
    || (SYSM_MONITOR_CPU_LOAD > 0    ) )
void sysm_clear_total_CPU_usage(void)
{
        cpuctl_clear_CPU_total_time();
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
    || (SYSM_MONITOR_FILE_USAGE > 0  ) \
    || (SYSM_MONITOR_CPU_LOAD > 0    ) )
stdret_t sysm_get_task_stat(task_t *taskhdl, struct taskstat *stat)
{
        struct task_monitor_data *tmdata;

        force_lock_recursive_mutex(sysm_resource_mtx);

        if (!stat) {
                goto exit_error;
        }

        if (sysm_is_task_exist(taskhdl) == FALSE) {
                goto exit_error;
        }

        if (!(tmdata = get_task_monitor_data(taskhdl))) {
                goto exit_error;
        }

        enter_critical();
        stat->cpu_usage = get_task_data(taskhdl)->f_cpu_usage;
        get_task_data(taskhdl)->f_cpu_usage = 0;
        exit_critical();

        stat->free_stack   = get_task_free_stack(taskhdl);
        stat->memory_usage = tmdata->used_memory;
        stat->opened_files = tmdata->opened_files;
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
    || (SYSM_MONITOR_FILE_USAGE > 0  ) \
    || (SYSM_MONITOR_CPU_LOAD > 0    ) )
stdret_t sysm_get_ntask_stat(i32_t item, struct taskstat *stat)
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
 * @brief Function return number of monitor tasks
 *
 * @return number of monitor tasks
 */
//==============================================================================
#if (  (SYSM_MONITOR_TASK_MEMORY_USAGE > 0) \
    || (SYSM_MONITOR_FILE_USAGE > 0  ) \
    || (SYSM_MONITOR_CPU_LOAD > 0    ) )
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
 * @brief Function enable fast memory monitoring (not remember allocation addresses)
 *
 * @param *taskhdl      task handle
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
#if (SYSM_MONITOR_TASK_MEMORY_USAGE > 0)
stdret_t sysm_enable_fast_memory_monitoring(task_t *taskhdl)
{
        stdret_t status = STD_RET_ERROR;
        struct task_monitor_data *task_monitor_data;

        force_lock_recursive_mutex(sysm_resource_mtx);

        if ((task_monitor_data = get_task_monitor_data(taskhdl))) {
                task_monitor_data->fast_monitoring = TRUE;
                status = STD_RET_OK;
        }

        unlock_recursive_mutex(sysm_resource_mtx);

        return status;
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
void *sysm_smalloc(size_t size)
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
void *sysm_scalloc(size_t count, size_t size)
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
void sysm_sfree(void *mem)
{
        sysm_system_memory_usage -= memman_free(mem);
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
void *sysm_malloc_as(task_t *taskhdl, u32_t size)
{
        void   *mem   = NULL;
        uint    block = 0;
        struct task_monitor_data *task_monitor_data;
        struct memSlot           *mslot;

        force_lock_recursive_mutex(sysm_resource_mtx);

        if (size == 0) {
                goto exit;
        }

        if (sysm_is_task_exist(taskhdl) == FALSE) {
                goto exit;
        }

        if (!(task_monitor_data = get_task_monitor_data(taskhdl))) {
                goto exit;
        }

        if (task_monitor_data->fast_monitoring) {
                mem = malloc(size);
                if (mem) {
                        task_monitor_data->used_memory += size;
                }

                goto exit;
        }

        /* find empty address slot */
        while (block < MEM_BLOCK_COUNT) {
                if (task_monitor_data->mblock[block] == NULL) {
                        task_monitor_data->mblock[block] = calloc(1, sizeof(struct memBlock));

                        if (task_monitor_data->mblock[block] == NULL) {
                                block++;
                                continue;
                        }
                }

                /* find empty address slot in block */
                for (u8_t slot = 0; slot < MEM_ADR_IN_BLOCK; slot++) {
                        if (task_monitor_data->mblock[block]->full == TRUE) {
                                break;
                        }

                        mslot = &task_monitor_data->mblock[block]->mslot[slot];

                        if (mslot->addr == NULL) {
                                if ((mslot->addr = malloc(size))) {
                                        mem = mslot->addr;
                                        mslot->size = size;
                                        task_monitor_data->used_memory += size;
                                }

                                if (slot == MEM_ADR_IN_BLOCK - 1) {
                                        task_monitor_data->mblock[block]->full = TRUE;
                                }

                                goto exit;
                        }
                }

                block++;
        }

        printk("%s: Not enough free slots to allocate memory!\n", get_this_task_name());

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
void *sysm_malloc(u32_t size)
{
        return sysm_malloc_as(get_task_handle(), size);
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
void *sysm_calloc_as(task_t *taskhdl, u32_t nmemb, u32_t msize)
{
        void *ptr = sysm_malloc_as(taskhdl, nmemb * msize);

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
void *sysm_calloc(u32_t nmemb, u32_t msize)
{
        void *ptr = sysm_malloc_as(get_task_handle(), nmemb * msize);

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
void sysm_free_as(task_t *taskhdl, void *mem)
{
        struct task_monitor_data *task_monitor_data;
        struct memSlot           *mslot;

        force_lock_recursive_mutex(sysm_resource_mtx);

        if (!mem) {
                goto exit;
        }

        if (sysm_is_task_exist(taskhdl) == FALSE) {
                goto exit;
        }

        if (!(task_monitor_data = get_task_monitor_data(taskhdl))) {
                goto exit;
        }

        if (task_monitor_data->fast_monitoring) {
                free(mem);
                goto exit;
        }

        /* find address slot */
        for (u8_t block = 0; block < MEM_BLOCK_COUNT; block++) {
                if (task_monitor_data->mblock[block] == NULL) {
                        continue;
                }

                /* find empty address slot in block */
                for (u8_t slot = 0; slot < MEM_ADR_IN_BLOCK;slot++) {

                        mslot = &task_monitor_data->mblock[block]->mslot[slot];

                        if (mslot->addr != mem) {
                                continue;
                        }

                        free(mslot->addr);
                        mslot->addr = NULL;

                        task_monitor_data->used_memory -= mslot->size;
                        mslot->size = 0;

                        task_monitor_data->mblock[block]->full = FALSE;

                        /* first block cannot be freed */
                        if (block == 0) {
                                goto exit;
                        }

                        /* check if block is empty */
                        for (u8_t i = 0; i < MEM_ADR_IN_BLOCK; i++) {
                                if (task_monitor_data->mblock[block]->mslot[i].addr != NULL) {
                                        goto exit;
                                }
                        }

                        /* block is empty - freeing whole block */
                        free(task_monitor_data->mblock[block]);
                        task_monitor_data->mblock[block] = NULL;

                        goto exit;
                }
        }

        printk("%s: Address to free does not exist!\n", get_this_task_name());

        exit:
        unlock_recursive_mutex(sysm_resource_mtx);
}
#endif

//==============================================================================
/**
 * @brief Monitor memory freeing for specified task
 *
 * @param *taskhdl      task handle
 * @param *mem          block to free
 * @param  size         freed block size
 */
//==============================================================================
#if (SYSM_MONITOR_TASK_MEMORY_USAGE > 0)
void sysm_freemem_as(task_t *taskhdl, void *mem, u32_t size)
{
        struct task_monitor_data *task_monitor_data;

        if (size && mem && taskhdl) {
                force_lock_recursive_mutex(sysm_resource_mtx);

                if (sysm_is_task_exist(taskhdl)) {
                        if ((task_monitor_data = get_task_monitor_data(taskhdl))) {
                                if (task_monitor_data->fast_monitoring) {
                                        task_monitor_data->used_memory -= size;
                                }

                                sysm_free_as(taskhdl, mem);
                        }
                }

                unlock_recursive_mutex(sysm_resource_mtx);
        }
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
void sysm_free(void *mem)
{
        sysm_free_as(get_task_handle(), mem);
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
#if (SYSM_MONITOR_FILE_USAGE > 0)
file_t *sysm_fopen(const char *path, const char *mode)
{
        file_t *file  = NULL;
        uint    block = 0;
        task_t *task;
        struct task_monitor_data *task_monitor_data;
        struct fileSlot          *fslot;

        force_lock_recursive_mutex(sysm_resource_mtx);

        task = get_task_handle();

        if (sysm_is_task_exist(task) == FALSE) {
                goto exit;
        }

        if (!(task_monitor_data = get_task_monitor_data(task))) {
                goto exit;
        }

        /* find empty file slot */
        while (block < FILE_BLOCK_COUNT) {
                if (task_monitor_data->fblock[block] == NULL) {
                        task_monitor_data->fblock[block] = calloc(1, sizeof(struct fileBlock));

                        if (task_monitor_data->fblock[block] == NULL) {
                                block++;
                                continue;
                        }
                }

                /* find empty file slot in block */
                for (u8_t slot = 0; slot < FILES_OPENED_IN_BLOCK; slot++) {
                        if (task_monitor_data->fblock[block]->full == TRUE) {
                                break;
                        }

                        fslot = &task_monitor_data->fblock[block]->fslot[slot];

                        if (fslot->file == NULL) {
                                fslot->file = vfs_fopen(path, mode);

                                if (fslot->file) {
                                        file = fslot->file;
                                        task_monitor_data->opened_files++;
                                }

                                if (slot == FILES_OPENED_IN_BLOCK - 1) {
                                        task_monitor_data->fblock[block]->full = TRUE;
                                }

                                goto exit;
                        }
                }

                block++;
        }

        printk("%s: Not enough free slots to open file!\n", get_this_task_name());

        exit:
        unlock_recursive_mutex(sysm_resource_mtx);
        return file;
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
#if (SYSM_MONITOR_FILE_USAGE > 0)
stdret_t sysm_fclose(file_t *file)
{
        stdret_t status = STD_RET_ERROR;
        task_t  *task;
        struct task_monitor_data *task_monitor_data;
        struct fileSlot          *fslot;

        force_lock_recursive_mutex(sysm_resource_mtx);

        task = get_task_handle();

        if (sysm_is_task_exist(task) == FALSE) {
                goto exit;
        }

        if (!(task_monitor_data = get_task_monitor_data(task))) {
                goto exit;
        }

        /* find empty file slot */
        for (u8_t block = 0; block < FILE_BLOCK_COUNT; block++) {
                if (task_monitor_data->fblock[block] == NULL) {
                        continue;
                }

                /* find opened file */
                for (u8_t slot = 0; slot < FILES_OPENED_IN_BLOCK; slot++) {
                        fslot = &task_monitor_data->fblock[block]->fslot[slot];

                        if (fslot->file != file) {
                                continue;
                        }

                        if ((status = vfs_fclose(file)) != STD_RET_OK) {
                                goto exit;
                        }

                        fslot->file = NULL;
                        task_monitor_data->opened_files--;
                        task_monitor_data->fblock[block]->full = FALSE;

                        if (block == 0) {
                                goto exit;
                        }

                        /* check if block is empty */
                        for (u8_t i = 0; i < MEM_ADR_IN_BLOCK; i++) {
                                if (task_monitor_data->fblock[block]->fslot[i].file != NULL) {
                                        goto exit;
                                }
                        }

                        /* block is empty - freeing memory */
                        free(task_monitor_data->fblock[block]);
                        task_monitor_data->fblock[block] = NULL;

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
#if (SYSM_MONITOR_FILE_USAGE > 0)
dir_t *sysm_opendir(const char *path)
{
        dir_t  *dir   = NULL;
        u32_t   block = 0;
        task_t *task;
        struct task_monitor_data *task_monitor_data;
        struct dirSlot           *dslot;

        force_lock_recursive_mutex(sysm_resource_mtx);

        task = get_task_handle();

        if (sysm_is_task_exist(task) == FALSE) {
                goto exit;
        }

        if (!(task_monitor_data = get_task_monitor_data(task))) {
                goto exit;
        }

        /* find empty file slot */
        while (block < DIR_BLOCK_COUNT) {
                if (task_monitor_data->dblock[block] == NULL) {
                        task_monitor_data->dblock[block] = calloc(1, sizeof(struct dirBlock));

                        if (task_monitor_data->dblock[block] == NULL) {
                                block++;
                                continue;
                        }
                }

                /* find empty dir slot in block */
                for (u8_t slot = 0; slot < DIRS_OPENED_IN_BLOCK; slot++) {
                        if (task_monitor_data->dblock[block]->full == TRUE) {
                                break;
                        }

                        dslot = &task_monitor_data->dblock[block]->dslot[slot];

                        if (dslot->dir == NULL) {
                                dslot->dir = vfs_opendir(path);

                                if (dslot->dir) {
                                        dir = dslot->dir;
                                        task_monitor_data->opened_files++;
                                }

                                if (slot == DIRS_OPENED_IN_BLOCK - 1) {
                                        task_monitor_data->dblock[block]->full = TRUE;
                                }

                                goto exit;
                        }
                }

                block++;
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
#if (SYSM_MONITOR_FILE_USAGE > 0)
extern stdret_t sysm_closedir(dir_t *dir)
{
        stdret_t status = STD_RET_ERROR;
        task_t *task;
        struct task_monitor_data *task_monitor_data;
        struct dirSlot           *dslot;

        force_lock_recursive_mutex(sysm_resource_mtx);

        task = get_task_handle();

        if (sysm_is_task_exist(task) == FALSE) {
                goto exit;
        }

        if (!(task_monitor_data = get_task_monitor_data(task))) {
                goto exit;
        }

        /* find empty file slot */
        for (u8_t block = 0; block < DIR_BLOCK_COUNT; block++) {
                if (task_monitor_data->dblock[block] == NULL) {
                        continue;
                }

                /* find opened file */
                for (u8_t slot = 0; slot < DIRS_OPENED_IN_BLOCK; slot++) {
                        dslot = &task_monitor_data->dblock[block]->dslot[slot];

                        if (dslot->dir != dir) {
                                continue;
                        }

                        if ((status = vfs_closedir(dir)) != STD_RET_OK) {
                                goto exit;
                        }

                        dslot->dir = NULL;
                        task_monitor_data->opened_files--;
                        task_monitor_data->dblock[block]->full = FALSE;

                        if (block == 0) {
                                goto exit;
                        }

                        /* check if block is empty */
                        for (u8_t i = 0; i < MEM_ADR_IN_BLOCK; i++) {
                                if (task_monitor_data->dblock[block]->dslot[i].dir != NULL) {
                                        goto exit;
                                }
                        }

                        /* block is empty - freeing memory */
                        free(task_monitor_data->dblock[block]);
                        task_monitor_data->dblock[block] = NULL;

                        goto exit;
                }
        }

        printk("%s: Dir does not exist or closed!\n", get_this_task_name());

        exit:
        unlock_recursive_mutex(sysm_resource_mtx);
        return status;
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
        cpuctl_clear_CPU_load_timer();
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
        struct task_data *tdata = get_this_task_data();
        u32_t             cnt   = cpuctl_get_CPU_load_timer();

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
