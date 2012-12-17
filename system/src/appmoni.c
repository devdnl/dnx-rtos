/*=============================================================================================*//**
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


*//*==============================================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*==================================================================================================
                                            Include files
==================================================================================================*/
#include "appmoni.h"
#include "dlist.h"
#include "memman.h"
#include <string.h>


/*==================================================================================================
                                  Local symbolic constants/macros
==================================================================================================*/
#define calloc(nmemb, msize)        mm_calloc(nmemb, msize)
#define malloc(size)                mm_malloc(size)
#define free(mem)                   mm_free(mem)
#define fclose(file)                vfs_fclose(file)

#define MEM_BLOCK_COUNT             4
#define MEM_ADR_IN_BLOCK            7

#define FLE_BLOCK_COUNT             3
#define FLE_OPN_IN_BLOCK            8

#define MTX_BLOCK_TIME              10

/*==================================================================================================
                                   Local types, enums definitions
==================================================================================================*/
/* task information */
struct taskData {
      task_t taskHdl;

#if (APP_MONITOR_MEMORY_USAGE > 0)
      struct memBlock {
            bool_t full;

            struct memSlot {
                void *addr;
                u32_t size;
            } mslot[MEM_ADR_IN_BLOCK];

      } *mblock[MEM_BLOCK_COUNT];
#endif

#if (APP_MONITOR_FILE_USAGE > 0)
      struct fileBlock {
            bool_t full;

            struct fileSlot {
                  FILE_t *file;
            } fslot[FLE_OPN_IN_BLOCK];

      } *fblock[FLE_BLOCK_COUNT];
#endif
};

/* main structure */
struct moni {
      list_t  *tasks;         /* list with task informations */
      mutex_t  mtx;           /* mutex */
};


/*==================================================================================================
                                      Local function prototypes
==================================================================================================*/


/*==================================================================================================
                                      Local object definitions
==================================================================================================*/
static struct moni *moni;


/*==================================================================================================
                                     Exported object definitions
==================================================================================================*/


/*==================================================================================================
                                        Function definitions
==================================================================================================*/

//================================================================================================//
/**
 * @brief Initialize module
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//================================================================================================//
#if ((APP_MONITOR_MEMORY_USAGE > 0) || (APP_MONITOR_FILE_USAGE > 0))
stdRet_t moni_Init(void)
{
      stdRet_t status = STD_RET_OK;

      if (moni == NULL) {
            moni = calloc(1, sizeof(struct moni));

            if (moni) {
                  moni->tasks = ListCreate();
                  CreateMutex(moni->mtx);

                  if (!moni->tasks || !moni->mtx) {
                        if (moni->tasks)
                              ListDestroy(moni->tasks);

                        if (moni->mtx)
                              DeleteMutex(moni->mtx);

                        free(moni);
                        moni = NULL;

                        status = STD_RET_ERROR;
                  } else {
                        cpuctl_CfgTimeStatCnt();
                  }
            }
      }

      return status;
}
#endif


//================================================================================================//
/**
 * @brief Function adds task to monitoring
 *
 * @param pid     task ID
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//================================================================================================//
#if ((APP_MONITOR_MEMORY_USAGE > 0) || (APP_MONITOR_FILE_USAGE > 0))
stdRet_t moni_AddTask(task_t taskHdl)
{
      stdRet_t status = STD_RET_ERROR;

      if (moni) {
            while (TakeMutex(moni->mtx, MTX_BLOCK_TIME) != OS_OK);

            /* find task that does not exist yet */
            i32_t n = ListGetItemCount(moni->tasks);

            for (i32_t i = 0; i < n; i++) {
                  struct taskData *task = ListGetItemDataByNo(moni->tasks, i);

                  if (task->taskHdl == taskHdl) {
                        status = STD_RET_OK;
                        goto moni_addTask_end;
                  }
            }

            /* task does not exist */
            struct taskData *task = calloc(1, sizeof(struct taskData));

            if (task) {
                  task->taskHdl = taskHdl;

                  if (ListAddItem(moni->tasks, task) >= 0) {
                        status = STD_RET_OK;
                  } else {
                        free(task);
                  }
            }

            moni_addTask_end:
            GiveMutex(moni->mtx);
      }

      return status;
}
#endif


//================================================================================================//
/**
 * @brief Function delete task monitoring
 *
 * @param pid     task ID
 */
//================================================================================================//
#if ((APP_MONITOR_MEMORY_USAGE > 0) || (APP_MONITOR_FILE_USAGE > 0))
stdRet_t moni_DelTask(task_t taskHdl)
{
      stdRet_t status = STD_RET_ERROR;

      if (moni) {
            while (TakeMutex(moni->mtx, MTX_BLOCK_TIME) != OS_OK);

            i32_t taskTotal = ListGetItemCount(moni->tasks);

            for (i32_t task = 0; task < taskTotal; task++) {
                  struct taskData *taskInfo = ListGetItemDataByNo(moni->tasks, task);

                  if (taskInfo->taskHdl == taskHdl) {
                        #if (APP_MONITOR_MEMORY_USAGE > 0)
                        for (u32_t block = 0; block < MEM_BLOCK_COUNT; block++) {
                              if (taskInfo->mblock[block]) {
                                    for (u32_t slot = 0; slot < MEM_ADR_IN_BLOCK; slot++) {
                                          if (taskInfo->mblock[block]->mslot[slot].addr) {
                                                free(taskInfo->mblock[block]->mslot[slot].addr);
                                                taskInfo->mblock[block]->mslot[slot].size = 0;
                                          }
                                    }

                                    free(taskInfo->mblock[block]);
                                    taskInfo->mblock[block] = NULL;
                              }
                        }
                        #endif

                        #if (APP_MONITOR_FILE_USAGE > 0)
                        for (u32_t block = 0; block < FLE_BLOCK_COUNT; block++) {
                              if (taskInfo->fblock[block]) {
                                    for (u32_t slot = 0; slot < FLE_OPN_IN_BLOCK; slot++) {
                                          if (taskInfo->fblock[block]->fslot[slot].file) {
                                                fclose(taskInfo->fblock[block]->fslot[slot].file);
                                          }
                                    }

                                    free(taskInfo->fblock[block]);
                                    taskInfo->fblock[block] = NULL;
                              }
                        }
                        #endif

                        ListRmItemByNo(moni->tasks, task);

                        status = STD_RET_OK;
                        goto moni_DelTask_end;
                  }
            }

            moni_DelTask_end:
            GiveMutex(moni->mtx);
      }

      return status;
}
#endif


//================================================================================================//
/**
 * @brief Function gets task statistics
 *
 * @param  item   task item
 * @param *stat   task statistics
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//================================================================================================//
#if ((APP_MONITOR_MEMORY_USAGE > 0) || (APP_MONITOR_FILE_USAGE > 0))
stdRet_t moni_GetTaskStat(i32_t item, struct taskstat *stat)
{
      stdRet_t status = STD_RET_ERROR;

      if (moni && stat) {
            stat->memUsage      = 0;
            stat->fileUsage     = 0;

            while (TakeMutex(moni->mtx, MTX_BLOCK_TIME) != OS_OK);

            struct taskData *taskdata = ListGetItemDataByNo(moni->tasks, item);

            if (taskdata) {
                  #if (APP_MONITOR_MEMORY_USAGE > 0)
                  for (u32_t block = 0; block < MEM_BLOCK_COUNT; block++) {
                        if (taskdata->mblock[block]) {
                              for (u32_t slot = 0; slot < MEM_ADR_IN_BLOCK; slot++) {
                                    if (taskdata->mblock[block]->mslot[slot].addr) {
                                          stat->memUsage += taskdata->mblock[block]->mslot[slot].size;
                                    }
                              }
                        }
                  }
                  #endif

                  #if (APP_MONITOR_FILE_USAGE > 0)
                  for (u32_t block = 0; block < FLE_BLOCK_COUNT; block++) {
                        if (taskdata->fblock[block]) {
                              for (u32_t slot = 0; slot < FLE_OPN_IN_BLOCK; slot++) {
                                    if (taskdata->fblock[block]->fslot[slot].file) {
                                          stat->fileUsage++;
                                    }
                              }
                        }
                  }
                  #endif

                  status = STD_RET_OK;
            }

            GiveMutex(moni->mtx);

            if (status == STD_RET_OK) {
                  stat->taskHdl       = taskdata->taskHdl;
                  stat->taskName      = TaskGetName(taskdata->taskHdl);
                  stat->taskFreeStack = TaskGetStackFreeSpace(taskdata->taskHdl);
                  TaskSuspendAll();
                  stat->cpuUsage      = (u32_t)TaskGetTag(taskdata->taskHdl);
                  TaskSetTag(taskdata->taskHdl, (void*)0);
                  TaskResumeAll();
                  stat->cpuUsageTotal = cpuctl_GetCPUTotalTime();
                  stat->taskPriority  = TaskGetPriority(taskdata->taskHdl);

                  if (item == ListGetItemCount(moni->tasks) - 1) {
                        cpuctl_ClearCPUTotalTime();
                  }
            }
      }

      return status;
}
#endif


//================================================================================================//
/**
 * @brief Function return number of monitor tasks
 *
 * @return number of monitor tasks
 */
//================================================================================================//
#if ((APP_MONITOR_MEMORY_USAGE > 0) || (APP_MONITOR_FILE_USAGE > 0))
u16_t moni_GetTaskCount(void)
{
      return ListGetItemCount(moni->tasks);
}
#endif


//================================================================================================//
/**
 * @brief Monitor memory allocation
 *
 * @param size          block size
 *
 * @return pointer to allocated block or NULL if error
 */
//================================================================================================//
#if (APP_MONITOR_MEMORY_USAGE > 0)
void *moni_malloc(u32_t size)
{
      void *mem = NULL;

      if (moni) {
            while (TakeMutex(moni->mtx, MTX_BLOCK_TIME) != OS_OK);

            task_t taskHdl  = TaskGetCurrentTaskHandle();
            i32_t taskCount = ListGetItemCount(moni->tasks);

            /* find task */
            for (i32_t task = 0; task < taskCount; task++) {
                  struct taskData *taskInfo = ListGetItemDataByNo(moni->tasks, task);

                  if (taskInfo == NULL)
                        goto moni_malloc_end;

                  if (taskInfo->taskHdl == taskHdl) {
                        /* find empty address slot */
                        u32_t block = 0;

                        while (block < MEM_BLOCK_COUNT) {
                              if (taskInfo->mblock[block]) {

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

                                                if (slot == MEM_ADR_IN_BLOCK - 1)
                                                      taskInfo->mblock[block]->full = TRUE;

                                                goto moni_malloc_end;
                                          }
                                    }

                                    block++;
                              } else {
                                    /* create new block */
                                    taskInfo->mblock[block] = calloc(1, sizeof(struct memBlock));

                                    if (taskInfo->mblock[block] == NULL) {
                                          block++;
                                    }
                              }
                        }
                  }
            }

            moni_malloc_end:
            GiveMutex(moni->mtx);
      }

      return mem;
}
#endif


//================================================================================================//
/**
 * @brief Monitor memory allocation
 *
 * @param nmemb         n members
 * @param msize         member size
 *
 * @return pointer to allocated block or NULL if error
 */
//================================================================================================//
#if (APP_MONITOR_MEMORY_USAGE > 0)
void *moni_calloc(u32_t nmemb, u32_t msize)
{
      void *ptr = moni_malloc(nmemb * msize);

      if (ptr)
            memset(ptr, 0, nmemb * msize);

      return ptr;
}
#endif


//================================================================================================//
/**
 * @brief Monitor memory freeing
 *
 * @param *mem          block to free
 */
//================================================================================================//
#if (APP_MONITOR_MEMORY_USAGE > 0)
void moni_free(void *mem)
{
      if (moni) {
            while (TakeMutex(moni->mtx, MTX_BLOCK_TIME) != OS_OK);

            task_t taskHdl  = TaskGetCurrentTaskHandle();
            i32_t taskTotal = ListGetItemCount(moni->tasks);

            /* find task */
            for (i32_t task = 0; task < taskTotal; task++) {
                  struct taskData *taskInfo = ListGetItemDataByNo(moni->tasks, task);

                  if (taskInfo == NULL)
                        goto moni_free_end;

                  if (taskInfo->taskHdl == taskHdl) {
                        /* find address slot */
                        for (u8_t block = 0; block < MEM_BLOCK_COUNT; block++) {

                              if (taskInfo->mblock[block]) {

                                    /* find empty address slot in block */
                                    for (u8_t slot = 0; slot < MEM_ADR_IN_BLOCK; slot++) {

                                          struct memSlot *mslot = &taskInfo->mblock[block]->mslot[slot];

                                          if (mslot->addr == mem) {
                                                free(mslot->addr);
                                                mslot->addr = NULL;
                                                mslot->size = 0;

                                                taskInfo->mblock[block]->full = FALSE;

                                                /* check if block is empty */
                                                if (block != 0) {
                                                      for (u8_t i = 0; i < MEM_ADR_IN_BLOCK; i++) {
                                                            if (taskInfo->mblock[block]->mslot[i].addr != NULL)
                                                                  goto moni_free_end;
                                                      }

                                                      /* block is empty - freeing memory */
                                                      free(taskInfo->mblock[block]);
                                                      taskInfo->mblock[block] = NULL;
                                                }

                                                goto moni_free_end;
                                          }
                                    }
                              }
                        }

                        /* error: application try to free freed memory */
                        /* signal code can be added here */
                  }
            }

            moni_free_end:
            GiveMutex(moni->mtx);
      }
}
#endif


//================================================================================================//
/**
 * @brief Function open selected file
 *
 * @param *name         file path
 * @param *mode         file mode
 *
 * @retval NULL if file can't be created
 */
//================================================================================================//
#if (APP_MONITOR_FILE_USAGE > 0)
FILE_t *moni_fopen(const ch_t *path, const ch_t *mode)
{
      void *file = NULL;

      if (moni) {
            while (TakeMutex(moni->mtx, MTX_BLOCK_TIME) != OS_OK);

            task_t taskHdl  = TaskGetCurrentTaskHandle();
            i32_t taskCount = ListGetItemCount(moni->tasks);

            /* find task */
            for (i32_t task = 0; task < taskCount; task++) {
                  struct taskData *taskInfo = ListGetItemDataByNo(moni->tasks, task);

                  if (taskInfo == NULL)
                        goto moni_fopen_end;

                  if (taskInfo->taskHdl == taskHdl) {
                        /* find empty file slot */
                        u32_t block = 0;

                        while (block < FLE_BLOCK_COUNT) {
                              if (taskInfo->fblock[block]) {

                                    /* find empty file slot in block */
                                    for (u8_t slot = 0; slot < FLE_OPN_IN_BLOCK; slot++) {

                                          if (taskInfo->fblock[block]->full == TRUE) {
                                                break;
                                          }

                                          struct fileSlot *fslot = &taskInfo->fblock[block]->fslot[slot];

                                          if (fslot->file == NULL) {
                                                fslot->file = vfs_fopen(path, mode);

                                                if (fslot->file) {
                                                      file = fslot->file;
                                                }

                                                if (slot == FLE_OPN_IN_BLOCK - 1)
                                                      taskInfo->fblock[block]->full = TRUE;

                                                goto moni_fopen_end;
                                          }
                                    }

                                    block++;
                              } else {
                                    /* create new block */
                                    taskInfo->fblock[block] = calloc(1, sizeof(struct fileBlock));

                                    if (taskInfo->fblock[block] == NULL) {
                                          block++;
                                    }
                              }
                        }
                  }
            }

            moni_fopen_end:
            GiveMutex(moni->mtx);
      }

      return file;
}
#endif


//================================================================================================//
/**
 * @brief Function close opened file
 *
 * @param *file               pinter to file
 *
 * @retval STD_RET_OK         file closed successfully
 * @retval STD_RET_ERROR      file not closed
 */
//================================================================================================//
#if (APP_MONITOR_FILE_USAGE > 0)
stdRet_t moni_fclose(FILE_t *file)
{
      stdRet_t status = STD_RET_ERROR;

      if (moni) {
            while (TakeMutex(moni->mtx, MTX_BLOCK_TIME) != OS_OK);

            task_t taskHdl  = TaskGetCurrentTaskHandle();
            i32_t taskCount = ListGetItemCount(moni->tasks);

            /* find task */
            for (i32_t task = 0; task < taskCount; task++) {
                  struct taskData *taskInfo = ListGetItemDataByNo(moni->tasks, task);

                  if (taskInfo == NULL)
                        goto moni_fclose_end;

                  if (taskInfo->taskHdl == taskHdl) {
                        /* find empty file slot */
                        u32_t block = 0;

                        while (block < FLE_BLOCK_COUNT) {
                              if (taskInfo->fblock[block]) {

                                    /* find opened file */
                                    for (u8_t slot = 0; slot < FLE_OPN_IN_BLOCK; slot++) {

                                          struct fileSlot *fslot = &taskInfo->fblock[block]->fslot[slot];

                                          if (fslot->file == file) {
                                                status = vfs_fclose(file);

                                                if (status == STD_RET_OK) {
                                                      fslot->file = NULL;

                                                      taskInfo->fblock[block]->full = FALSE;

                                                      if (block != 0) {
                                                            /* check if block is empty */
                                                            for (u8_t i = 0; i < MEM_ADR_IN_BLOCK; i++) {
                                                                  if (taskInfo->fblock[block]->fslot[i].file != NULL)
                                                                        goto moni_fclose_end;
                                                            }

                                                            /* block is empty - freeing memory */
                                                            free(taskInfo->fblock[block]);
                                                            taskInfo->fblock[block] = NULL;
                                                      }
                                                }

                                                goto moni_fclose_end;
                                          }
                                    }

                                    block++;
                              }
                        }
                  }
            }

            moni_fclose_end:
            GiveMutex(moni->mtx);
      }

      return status;
}
#endif


#ifdef __cplusplus
}
#endif

/*==================================================================================================
                                            End of file
==================================================================================================*/
