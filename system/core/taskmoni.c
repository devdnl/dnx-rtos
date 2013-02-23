/*=========================================================================*//**
@file    appmoni.c

@author  Daniel Zorychta

@brief   This module is used to monitoring all applications

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
#include "taskmoni.h"
#include "dlist.h"
#include "cpuctl.h"
#include "oswrap.h"
#include <string.h>

/*==============================================================================
  Local symbolic constants/macros
==============================================================================*/
#define calloc(nmemb, msize)        memman_calloc(nmemb, msize)
#define malloc(size)                memman_malloc(size)
#define free(mem)                   memman_free(mem)

#define MEM_BLOCK_COUNT             4
#define MEM_ADR_IN_BLOCK            7

#define FILE_BLOCK_COUNT            3
#define FILES_OPENED_IN_BLOCK       7

#define DIR_BLOCK_COUNT             1
#define DIRS_OPENED_IN_BLOCK        3

#define MTX_BLOCK_TIME              10

/*==============================================================================
  Local types, enums definitions
==============================================================================*/
/* task information */
struct taskData {
#if (TSK_MONITOR_MEMORY_USAGE > 0)
        struct memBlock {
                bool_t full;

                struct memSlot {
                        void *addr;
                        u32_t size;
                } mslot[MEM_ADR_IN_BLOCK];

        }*mblock[MEM_BLOCK_COUNT];
#endif

#if (TSK_MONITOR_FILE_USAGE > 0)
        struct fileBlock {
                bool_t full;

                struct fileSlot {
                        FILE_t *file;
                } fslot[FILES_OPENED_IN_BLOCK];

        }*fblock[FILE_BLOCK_COUNT];

        struct dirBlock {
                bool_t full;

                struct dirSlot {
                        DIR_t *dir;
                } dslot[DIRS_OPENED_IN_BLOCK];
        }*dblock[DIR_BLOCK_COUNT];
#endif
};

/* main structure */
struct tskm {
        list_t *tasks;  /* list with task informations */
        mutex_t mtx;    /* mutex */
};

/*==============================================================================
  Local function prototypes
==============================================================================*/
#if (  (TSK_MONITOR_MEMORY_USAGE > 0) \
    || (TSK_MONITOR_FILE_USAGE > 0  ) \
    || (TSK_MONITOR_CPU_LOAD > 0    ) )
static stdRet_t tskm_init(void);
#endif

/*==============================================================================
  Local object definitions
==============================================================================*/
#if (  (TSK_MONITOR_MEMORY_USAGE > 0) \
    || (TSK_MONITOR_FILE_USAGE > 0  ) \
    || (TSK_MONITOR_CPU_LOAD > 0    ) )
static struct tskm *tskm;
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
#if (  (TSK_MONITOR_MEMORY_USAGE > 0) \
    || (TSK_MONITOR_FILE_USAGE > 0  ) \
    || (TSK_MONITOR_CPU_LOAD > 0    ) )
static stdRet_t tskm_init(void)
{
        stdRet_t status = STD_RET_OK;

        if (tskm == NULL) {
                tskm = calloc(1, sizeof(struct tskm));

                if (tskm) {
                        tskm->tasks = new_list();
                        tskm->mtx = new_recursive_mutex();

                        if (!tskm->tasks || !tskm->mtx) {
                                if (tskm->tasks)
                                        delete_list(tskm->tasks);

                                if (tskm->mtx)
                                        delete_mutex_recursive(tskm->mtx);

                                free(tskm);
                                tskm = NULL;

                                status = STD_RET_ERROR;
                        } else {
                                cpuctl_init_CPU_load_timer();
                        }
                }
        }

        return status;
}
#endif

//==============================================================================
/**
 * @brief Function adds task to monitoring
 *
 * @param pid     task ID
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
#if (  (TSK_MONITOR_MEMORY_USAGE > 0) \
    || (TSK_MONITOR_FILE_USAGE > 0  ) \
    || (TSK_MONITOR_CPU_LOAD > 0    ) )
stdRet_t tskm_add_task(task_t taskHdl)
{
        stdRet_t status = STD_RET_ERROR;

        if (tskm == NULL) {
                tskm_init();
        }

        if (tskm) {
                while (mutex_recursive_lock(tskm->mtx, MTX_BLOCK_TIME) != OS_OK);

                struct taskData *task = list_get_iditem_data(tskm->tasks,
                                                            (u32_t) taskHdl);

                /* task does not exist */
                if (task == NULL) {
                        task = calloc(1, sizeof(struct taskData));

                        if (task) {
                                if (list_add_item(tskm->tasks, (u32_t) taskHdl,
                                                task) >= 0) {
                                        status = STD_RET_OK;
                                } else {
                                        free(task);
                                }
                        }
                }

                mutex_recursive_unlock(tskm->mtx);
        }

        return status;
}
#endif

//==============================================================================
/**
 * @brief Function delete task monitoring
 *
 * @param pid     task ID
 */
//==============================================================================
#if (  (TSK_MONITOR_MEMORY_USAGE > 0) \
    || (TSK_MONITOR_FILE_USAGE > 0  ) \
    || (TSK_MONITOR_CPU_LOAD > 0    ) )
stdRet_t tskm_remove_task(task_t taskHdl)
{
        if (tskm == NULL) {
                return STD_RET_ERROR;
        }

        while (mutex_recursive_lock(tskm->mtx, MTX_BLOCK_TIME) != OS_OK);

        struct taskData *taskInfo = list_get_iditem_data(tskm->tasks, (u32_t)taskHdl);

        if (taskInfo == NULL) {
                return STD_RET_ERROR;
        }

#if (TSK_MONITOR_MEMORY_USAGE > 0)
        for (u32_t block = 0; block < MEM_BLOCK_COUNT; block++) {
                if (taskInfo->mblock[block] == NULL) {
                        continue;
                }

                for (u32_t slot = 0; slot < MEM_ADR_IN_BLOCK; slot++) {
                        struct memSlot *mslot = &taskInfo->mblock[block]->mslot[slot];

                        if (mslot->addr) {
                                free(mslot->addr);
                                mslot->size = 0;
                        }
                }

                free(taskInfo->mblock[block]);
                taskInfo->mblock[block] = NULL;
        }
#endif

#if (TSK_MONITOR_FILE_USAGE > 0)
        for (u32_t block = 0; block < FILE_BLOCK_COUNT; block++) {
                if (taskInfo->fblock[block] == NULL) {
                        continue;
                }

                for (u32_t slot = 0; slot < FILES_OPENED_IN_BLOCK; slot++) {
                        struct fileSlot *fslot = &taskInfo->fblock[block]->fslot[slot];

                        if (fslot->file) {
                                vfs_fclose(fslot->file);
                        }
                }

                free(taskInfo->fblock[block]);
                taskInfo->fblock[block] = NULL;
        }

        for (u32_t block = 0; block < DIR_BLOCK_COUNT; block++) {
                if (taskInfo->dblock[block] == NULL) {
                        continue;
                }

                for (u32_t slot = 0; slot < DIRS_OPENED_IN_BLOCK; slot++) {
                        struct dirSlot *dslot = &taskInfo->dblock[block]->dslot[slot];

                        if (dslot->dir) {
                                vfs_closedir(dslot->dir);
                        }
                }

                free(taskInfo->fblock[block]);
                taskInfo->fblock[block] = NULL;
        }
#endif

        list_rm_iditem(tskm->tasks, (u32_t) taskHdl);

        mutex_recursive_unlock(tskm->mtx);

        return STD_RET_OK;
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
#if (  (TSK_MONITOR_MEMORY_USAGE > 0) \
    || (TSK_MONITOR_FILE_USAGE > 0  ) \
    || (TSK_MONITOR_CPU_LOAD > 0    ) )
stdRet_t tskm_get_ntask_stat(i32_t item, struct taskstat *stat)
{
        if (!tskm || !stat) {
                return STD_RET_ERROR;
        }

        stat->memory_usage = 0;
        stat->opened_files = 0;

        while (mutex_recursive_lock(tskm->mtx, MTX_BLOCK_TIME) != OS_OK);

        struct taskData *taskdata = list_get_nitem_data(tskm->tasks, item);
        if (taskdata == NULL) {
                return STD_RET_ERROR;
        }

#if (TSK_MONITOR_MEMORY_USAGE > 0)
        for (u32_t block = 0; block < MEM_BLOCK_COUNT; block++) {
                if (taskdata->mblock[block] == NULL) {
                        continue;
                }

                for (u32_t slot = 0; slot < MEM_ADR_IN_BLOCK; slot++) {
                        if (taskdata->mblock[block]->mslot[slot].addr) {
                                stat->memory_usage += taskdata->mblock[block]->mslot[slot].size;
                        }
                }
        }
#endif

#if (TSK_MONITOR_FILE_USAGE > 0)
        for (u32_t block = 0; block < FILE_BLOCK_COUNT; block++) {
                if (taskdata->fblock[block] == NULL) {
                        continue;
                }

                for (u32_t slot = 0; slot < FILES_OPENED_IN_BLOCK; slot++) {
                        if (taskdata->fblock[block]->fslot[slot].file) {
                                stat->opened_files++;
                        }
                }
        }

        for (u32_t block = 0; block < DIR_BLOCK_COUNT; block++) {
                if (taskdata->dblock[block] == NULL) {
                        continue;
                }

                for (u32_t slot = 0; slot < DIRS_OPENED_IN_BLOCK; slot++) {
                        if (taskdata->dblock[block]->dslot[slot].dir) {
                                stat->opened_files++;
                        }
                }
        }
#endif

        mutex_recursive_unlock(tskm->mtx);

        task_t taskHdl = 0;
        list_get_nitem_ID(tskm->tasks, item, (task_t) &taskHdl);

        stat->task_handle     = taskHdl;
        stat->task_name       = get_task_name(taskHdl);
        stat->free_stack      = get_task_free_stack(taskHdl);
        suspend_all_tasks();
        stat->cpu_usage       = (u32_t)get_task_tag(taskHdl);
        set_task_tag(taskHdl, (void*)0);
        resume_all_tasks();
        stat->cpu_usage_total = cpuctl_get_CPU_total_time();
        stat->priority        = get_task_priority(taskHdl);

        if (item == list_get_item_count(tskm->tasks) - 1) {
                cpuctl_clear_CPU_total_time();
        }

        return STD_RET_OK;
}
#endif

//==============================================================================
/**
 * @brief Function gets task status
 *
 * @param  taskHdl      task handle
 * @param *stat         status result
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
#if (  (TSK_MONITOR_MEMORY_USAGE > 0) \
    || (TSK_MONITOR_FILE_USAGE > 0  ) \
    || (TSK_MONITOR_CPU_LOAD > 0    ) )
stdRet_t tskm_get_task_stat(task_t taskHdl, struct taskstat *stat)
{
        if (!taskHdl || !tskm) {
                return STD_RET_ERROR;
        }

        i32_t item = -1;

        if (list_get_iditem_No(tskm->tasks, (u32_t) taskHdl, &item) == STD_RET_OK) {
                return tskm_get_ntask_stat(item, stat);
        }

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
#if (  (TSK_MONITOR_MEMORY_USAGE > 0) \
    || (TSK_MONITOR_FILE_USAGE > 0  ) \
    || (TSK_MONITOR_CPU_LOAD > 0    ) )
u16_t tskm_get_task_count(void)
{
        if (!tskm) {
                return -1;
        }

        return list_get_item_count(tskm->tasks);
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
#if (TSK_MONITOR_MEMORY_USAGE > 0)
void *tskm_malloc(u32_t size)
{
        void   *mem   = NULL;
        uint    block = 0;
        struct taskData *taskInfo;

        if (!tskm || size == 0) {
                return NULL;
        }

        while (mutex_recursive_lock(tskm->mtx, MTX_BLOCK_TIME) != OS_OK);

        taskInfo = list_get_iditem_data(tskm->tasks, (u32_t)get_task_handle());
        if (taskInfo == NULL) {
                return NULL;
        }

        /* find empty address slot */
        while (block < MEM_BLOCK_COUNT) {
                if (taskInfo->mblock[block] == NULL) {
                        taskInfo->mblock[block] = calloc(1, sizeof(struct memBlock));

                        if (taskInfo->mblock[block] == NULL) {
                                block++;
                                continue;
                        }
                }

                /* find empty address slot in block */
                for (u8_t slot = 0; slot < MEM_ADR_IN_BLOCK; slot++) {
                        if (taskInfo->mblock[block]->full == TRUE) {
                                break;
                        }

                        struct memSlot *mslot = &taskInfo->mblock[block]->mslot[slot];

                        if (mslot->addr == NULL) {
                                mslot->addr = malloc(size);

                                if (mslot->addr) {
                                        mem = mslot->addr;
                                        mslot->size = size;
                                }

                                if (slot == MEM_ADR_IN_BLOCK - 1) {
                                        taskInfo->mblock[block]->full = TRUE;
                                }

                                goto moni_malloc_end;
                        }
                }

                block++;
        }

        moni_malloc_end:
        mutex_recursive_unlock(tskm->mtx);

        return mem;
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
#if (TSK_MONITOR_MEMORY_USAGE > 0)
void *tskm_calloc(u32_t nmemb, u32_t msize)
{
        void *ptr = tskm_malloc(nmemb * msize);

        if (ptr) {
                memset(ptr, 0, nmemb * msize);
        }

        return ptr;
}
#endif

//==============================================================================
/**
 * @brief Monitor memory freeing
 *
 * @param *mem          block to free
 */
//==============================================================================
#if (TSK_MONITOR_MEMORY_USAGE > 0)
void tskm_free(void *mem)
{
        struct taskData *taskInfo;

        if (tskm == NULL) {
                return;
        }

        while (mutex_recursive_lock(tskm->mtx, MTX_BLOCK_TIME) != OS_OK);

        taskInfo = list_get_iditem_data(tskm->tasks, (u32_t)get_task_handle());
        if (taskInfo == NULL) {
                return;
        }

        /* find address slot */
        for (u8_t block = 0; block < MEM_BLOCK_COUNT; block++) {
                if (taskInfo->mblock[block] == NULL) {
                        continue;
                }

                /* find empty address slot in block */
                for (u8_t slot = 0; slot < MEM_ADR_IN_BLOCK;slot++) {

                        struct memSlot *mslot = &taskInfo->mblock[block]->mslot[slot];

                        if (mslot->addr != mem) {
                                continue;
                        }

                        free(mslot->addr);
                        mslot->addr = NULL;
                        mslot->size = 0;

                        taskInfo->mblock[block]->full = FALSE;

                        /* first block cannot be freed */
                        if (block == 0) {
                                goto moni_free_end;
                        }

                        /* check if block is empty */
                        for (u8_t i = 0; i < MEM_ADR_IN_BLOCK; i++) {
                                if (taskInfo->mblock[block]->mslot[i].addr != NULL) {
                                        goto moni_free_end;
                                }
                        }

                        /* block is empty - freeing whole block */
                        free(taskInfo->mblock[block]);
                        taskInfo->mblock[block] = NULL;

                        goto moni_free_end;
                }
        }

        /* error: application try to free freed memory */
        /* signal code can be added here */

        moni_free_end:
        mutex_recursive_unlock(tskm->mtx);
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
#if (TSK_MONITOR_FILE_USAGE > 0)
FILE_t *tskm_fopen(const ch_t *path, const ch_t *mode)
{
        FILE_t *file  = NULL;
        uint    block = 0;
        struct taskData *taskInfo;

        if (tskm == NULL) {
                return NULL;
        }

        while (mutex_recursive_lock(tskm->mtx, MTX_BLOCK_TIME) != OS_OK);

        taskInfo = list_get_iditem_data(tskm->tasks, (u32_t)get_task_handle());
        if (taskInfo == NULL) {
                return NULL;
        }

        /* find empty file slot */
        while (block < FILE_BLOCK_COUNT) {
                if (taskInfo->fblock[block] == NULL) {
                        taskInfo->fblock[block] = calloc(1, sizeof(struct fileBlock));

                        if (taskInfo->fblock[block] == NULL) {
                                block++;
                                continue;
                        }
                }

                /* find empty file slot in block */
                for (u8_t slot = 0; slot < FILES_OPENED_IN_BLOCK; slot++) {
                        if (taskInfo->fblock[block]->full == TRUE) {
                                break;
                        }

                        struct fileSlot *fslot = &taskInfo->fblock[block]->fslot[slot];

                        if (fslot->file == NULL) {
                                fslot->file = vfs_fopen(path, mode);

                                if (fslot->file) {
                                        file = fslot->file;
                                }

                                if (slot == FILES_OPENED_IN_BLOCK - 1) {
                                        taskInfo->fblock[block]->full = TRUE;
                                }

                                goto moni_fopen_end;
                        }
                }

                block++;
        }

        moni_fopen_end:
        mutex_recursive_unlock(tskm->mtx);

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
#if (TSK_MONITOR_FILE_USAGE > 0)
stdRet_t tskm_fclose(FILE_t *file)
{
        stdRet_t status = STD_RET_ERROR;
        struct taskData *taskInfo;

        if (tskm == NULL) {
                return STD_RET_ERROR;
        }

        while (mutex_recursive_lock(tskm->mtx, MTX_BLOCK_TIME) != OS_OK);

        taskInfo = list_get_iditem_data(tskm->tasks, (u32_t)get_task_handle());
        if (taskInfo == NULL) {
                return STD_RET_ERROR;
        }

        /* find empty file slot */
        for (u8_t block = 0; block < FILE_BLOCK_COUNT; block++) {
                if (taskInfo->fblock[block] == NULL) {
                        continue;
                }

                /* find opened file */
                for (u8_t slot = 0; slot < FILES_OPENED_IN_BLOCK; slot++) {
                        struct fileSlot *fslot = &taskInfo->fblock[block]->fslot[slot];

                        if (fslot->file != file) {
                                continue;
                        }

                        if ((status = vfs_fclose(file)) != STD_RET_OK) {
                                goto moni_fclose_end;
                        }

                        fslot->file = NULL;
                        taskInfo->fblock[block]->full = FALSE;

                        if (block == 0) {
                                goto moni_fclose_end;
                        }

                        /* check if block is empty */
                        for (u8_t i = 0; i < MEM_ADR_IN_BLOCK; i++) {
                                if (taskInfo->fblock[block]->fslot[i].file != NULL) {
                                        goto moni_fclose_end;
                                }
                        }

                        /* block is empty - freeing memory */
                        free(taskInfo->fblock[block]);
                        taskInfo->fblock[block] = NULL;

                        goto moni_fclose_end;
                }
        }

        moni_fclose_end:
        mutex_recursive_unlock(tskm->mtx);

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
#if (TSK_MONITOR_FILE_USAGE > 0)
DIR_t *tskm_opendir(const ch_t *path)
{
        DIR_t *dir   = NULL;
        u32_t  block = 0;
        struct taskData *taskInfo;

        if (tskm == NULL) {
                return NULL;
        }

        while (mutex_recursive_lock(tskm->mtx, MTX_BLOCK_TIME) != OS_OK);

        taskInfo = list_get_iditem_data(tskm->tasks, (u32_t)get_task_handle());
        if (taskInfo == NULL) {
                return NULL;
        }

        /* find empty file slot */
        while (block < DIR_BLOCK_COUNT) {
                if (taskInfo->dblock[block] == NULL) {
                        taskInfo->dblock[block] = calloc(1, sizeof(struct dirBlock));

                        if (taskInfo->dblock[block] == NULL) {
                                block++;
                                continue;
                        }
                }

                /* find empty dir slot in block */
                for (u8_t slot = 0; slot < DIRS_OPENED_IN_BLOCK; slot++) {
                        if (taskInfo->dblock[block]->full == TRUE) {
                                break;
                        }

                        struct dirSlot *dslot = &taskInfo->dblock[block]->dslot[slot];

                        if (dslot->dir == NULL) {
                                dslot->dir = vfs_opendir(path);

                                if (dslot->dir) {
                                        dir = dslot->dir;
                                }

                                if (slot == DIRS_OPENED_IN_BLOCK - 1) {
                                        taskInfo->dblock[block]->full = TRUE;
                                }

                                goto moni_opendir_end;
                        }
                }

                block++;
        }

        moni_opendir_end:
        mutex_recursive_unlock(tskm->mtx);

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
#if (TSK_MONITOR_FILE_USAGE > 0)
extern stdRet_t tskm_closedir(DIR_t *dir)
{
        stdRet_t status = STD_RET_ERROR;
        struct taskData *taskInfo;

        if (tskm == NULL) {
                return STD_RET_ERROR;
        }

        while (mutex_recursive_lock(tskm->mtx, MTX_BLOCK_TIME) != OS_OK);

        taskInfo = list_get_iditem_data(tskm->tasks, (u32_t)get_task_handle());
        if (taskInfo == NULL) {
                return STD_RET_ERROR;
        }

        /* find empty file slot */
        for (u8_t block = 0; block < DIR_BLOCK_COUNT; block++) {
                if (taskInfo->dblock[block] == NULL) {
                        continue;
                }

                /* find opened file */
                for (u8_t slot = 0; slot < DIRS_OPENED_IN_BLOCK; slot++) {
                        struct dirSlot *dslot = &taskInfo->dblock[block]->dslot[slot];

                        if (dslot->dir != dir) {
                                continue;
                        }

                        if ((status = vfs_closedir(dir)) != STD_RET_OK) {
                                goto moni_closedir_end;
                        }

                        dslot->dir = NULL;
                        taskInfo->dblock[block]->full = FALSE;

                        if (block == 0) {
                                goto moni_closedir_end;
                        }

                        /* check if block is empty */
                        for (u8_t i = 0; i < MEM_ADR_IN_BLOCK; i++) {
                                if (taskInfo->dblock[block]->dslot[i].dir != NULL) {
                                        goto moni_closedir_end;
                                }
                        }

                        /* block is empty - freeing memory */
                        free(taskInfo->dblock[block]);
                        taskInfo->dblock[block] = NULL;

                        goto moni_closedir_end;
                }
        }

        moni_closedir_end:
        mutex_recursive_unlock(tskm->mtx);

        return status;
}
#endif

//==============================================================================
/**
 * @brief Function called after task go to ready state
 */
//==============================================================================
#if (TSK_MONITOR_CPU_LOAD > 0)
void tskm_task_switched_in(void)
{
        cpuctl_clear_CPU_load_timer();
}
#endif

//==============================================================================
/**
 * @brief Function called when task go out ready state
 */
//==============================================================================
#if (TSK_MONITOR_CPU_LOAD > 0)
void tskm_task_switched_out(void)
{
        u16_t  cnt     = cpuctl_get_CPU_load_timer();
        task_t taskhdl = get_task_handle();
        u32_t  tmp     = (u32_t)get_task_tag(taskhdl) + cnt;
        set_task_tag(taskhdl, (void*)tmp);
}
#endif

#ifdef __cplusplus
}
#endif

/*==============================================================================
  End of file
==============================================================================*/
