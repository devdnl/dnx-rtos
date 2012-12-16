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
#include "print.h"
#include "oswrap.h"
#include "dlist.h"
#include "memman.h"


/*==================================================================================================
                                  Local symbolic constants/macros
==================================================================================================*/
#define calloc(nmemb, msize)        mm_calloc(nmemb, msize)
#define malloc(size)                mm_malloc(size)
#define free(mem)                   mm_free(mem)
#define fclose(file)                vfs_fclose(file)

#define MEM_ADR_IN_BLOCK            8
#define OPEN_FILE_IN_BLOCK          8

#define MTX_BLOCK_TIME              10

/*==================================================================================================
                                   Local types, enums definitions
==================================================================================================*/
/* structure that remember allocated memory */
struct memalloc {
      void  *addr[MEM_ADR_IN_BLOCK];
      u32_t  size[MEM_ADR_IN_BLOCK];
};


/* structure that remember opened files */
struct openfile {
      FILE_t *fileObj[OPEN_FILE_IN_BLOCK];
};


/* structure that contains task informations */
struct taskdata {
      PID_t   pid;            /* task ID */

#if (APP_MONITOR_MEMORY_USAGE > 0)
      list_t *memInfo;        /* list with allocated memory (struct memalloc) */
#endif

#if (APP_MONITOR_FILE_USAGE > 0)
      list_t *fileInfo;       /* list with opened files (struct openfile) */
#endif
};


/* main structure */
struct moni {
      list_t  *tasks;
      mutex_t  mtx;
      u32_t    cpuTicks;
};


/*==================================================================================================
                                      Local function prototypes
==================================================================================================*/
static void addMemInfo(PID_t pid, void *addr, u32_t size);
static void delMemInfo(PID_t pid, void *addr);


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
stdRet_t moni_AddTask(PID_t pid)
{
      stdRet_t status = STD_RET_ERROR;

      if (moni) {
            while (TakeMutex(moni->mtx, MTX_BLOCK_TIME) != OS_OK);

            /* find task that does not exist yet */
            i32_t n = ListGetItemCount(moni->tasks);

            for (i32_t i = 0; i < n; i++) {
                  struct taskdata *task = ListGetItemDataByNo(moni->tasks, i);

                  if (task->pid == pid) {
                        status = STD_RET_OK;
                        goto moni_addTask_end;
                  }
            }

            /* task does not exist */
            struct taskdata *task = calloc(1, sizeof(struct taskdata));

            if (task) {
                  bool_t createSuccess = TRUE;

                  /* create file and memory lists */
                  #if (APP_MONITOR_FILE_USAGE > 0)
                  task->fileInfo = ListCreate();
                  #endif

                  #if (APP_MONITOR_MEMORY_USAGE > 0)
                  task->memInfo = ListCreate();
                  #endif

                  /* check if lists was created */
                  #if (APP_MONITOR_FILE_USAGE > 0)
                  if (task->fileInfo == NULL)
                        createSuccess = FALSE;
                  #endif

                  #if (APP_MONITOR_MEMORY_USAGE > 0)
                  if (task->memInfo == NULL)
                        createSuccess = FALSE;
                  #endif

                  /* finalize task data */
                  if (createSuccess == TRUE) {
                        task->pid = pid;

                        if (ListAddItem(moni->tasks, task) < 0) {
                              createSuccess = FALSE;
                        }
                  }

                  /* free if task cannot be created */
                  if (createSuccess == FALSE) {
                        #if (APP_MONITOR_FILE_USAGE > 0)
                        ListDestroy(task->fileInfo);
                        #endif

                        #if (APP_MONITOR_MEMORY_USAGE > 0)
                        ListDestroy(task->memInfo);
                        #endif

                        free(task);

                        status = STD_RET_ERROR;
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
stdRet_t moni_DelTask(PID_t pid)
{
      stdRet_t status = STD_RET_ERROR;

      if (moni) {
            while (TakeMutex(moni->mtx, MTX_BLOCK_TIME) != OS_OK);

            i32_t taskTotal = ListGetItemCount(moni->tasks);

            for (i32_t task = 0; task < taskTotal; task++) {
                  struct taskdata *taskInfo = ListGetItemDataByNo(moni->tasks, task);

                  if (taskInfo->pid == pid) {
                        i32_t blockTotal;

                        #if (APP_MONITOR_MEMORY_USAGE > 0)
                              blockTotal = ListGetItemCount(taskInfo->memInfo);

                              for (i32_t blockNo = 0; blockNo < blockTotal; blockNo++) {
                                    struct memalloc *bmem = ListGetItemDataByNo(taskInfo->memInfo, blockNo);

                                    if (bmem) {
                                          for (u8_t slot = 0; slot < MEM_ADR_IN_BLOCK; slot++) {
                                                if (bmem->addr[slot]) {
                                                      free(bmem->addr[slot]);
                                                      bmem->size[slot] = 0;
                                                }
                                          }

                                          free(bmem);
                                          bmem = NULL;
                                    }
                              }

                              ListDestroy(taskInfo->memInfo);
                              taskInfo->memInfo = NULL;
                        #endif

                        #if (APP_MONITOR_FILE_USAGE > 0)
                              blockTotal = ListGetItemCount(taskInfo->fileInfo);

                              for (i32_t blockNo = 0; blockNo < blockTotal; blockNo++) {
                                    struct openfile *bfile = ListGetItemDataByNo(taskInfo->fileInfo, blockNo);

                                    if (bfile) {
                                          for (u8_t slot = 0; slot < OPEN_FILE_IN_BLOCK; slot++) {
                                                if (bfile->fileObj[slot])
                                                      fclose(bfile->fileObj[slot]);
                                          }

                                          free(bfile);
                                          bfile = NULL;
                                    }
                              }

                              ListDestroy(taskInfo->fileInfo);
                              taskInfo->fileInfo = NULL;
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
            stat->cpuUsage      = 0;
            stat->cpuUsageTotal = 0;
            stat->memUsage      = 0;
            stat->fileUsage     = 0;

            while (TakeMutex(moni->mtx, MTX_BLOCK_TIME) != OS_OK);

            struct taskdata *taskdata = ListGetItemDataByNo(moni->tasks, item);

            if (taskdata) {
                  i32_t blockTotal;

                  #if (APP_MONITOR_MEMORY_USAGE > 0)
                        blockTotal = ListGetItemCount(taskdata->memInfo);

                        for (i32_t blockNo = 0; blockNo < blockTotal; blockNo++) {
                              struct memalloc *mem = ListGetItemDataByNo(taskdata->memInfo, blockNo);

                              if (mem) {
                                    for (u8_t m = 0; m < MEM_ADR_IN_BLOCK; m++) {
                                          if (mem->addr[m] != NULL) {
                                                stat->memUsage += mem->size[m];
                                          }
                                    }
                              }
                        }
                  #endif

                  #if (APP_MONITOR_FILE_USAGE > 0)
                        blockTotal = ListGetItemCount(taskdata->fileInfo);

                        for (i32_t blockNo = 0; blockNo < blockTotal; blockNo++) {
                              struct openfile *file = ListGetItemDataByNo(taskdata->fileInfo, blockNo);

                              if (file) {
                                    for (u8_t f = 0; f < OPEN_FILE_IN_BLOCK; f++) {
                                          if (file->fileObj[f] != NULL)
                                                stat->fileUsage++;
                                    }
                              }
                        }
                  #endif

                  status = STD_RET_OK;
            }

            GiveMutex(moni->mtx);
      }

      return status;
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
      void *ptr = mm_malloc(size);

      if (ptr)
            addMemInfo(TaskGetPID(), ptr, size);

      kprint("%s: malloc: %u -> 0x%x\n", TaskGetName(NULL), size, ptr);

      return ptr;
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
      void *ptr = mm_calloc(nmemb, msize);

      if (ptr)
            addMemInfo(TaskGetPID(), ptr, nmemb * msize);

      kprint("%s: calloc: %u * %u -> 0x%x\n", TaskGetName(NULL), nmemb, msize, ptr);

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
      mm_free(mem);

      if (mem)
            delMemInfo(TaskGetPID(), mem);

      kprint("%s: freeing: 0x%x\n", TaskGetName(NULL), mem);
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
      FILE_t *file = vfs_fopen(path, mode);

      if (file) {
            /* DNLTODO */
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
      stdRet_t status = vfs_fclose(file);

      if (status == STD_RET_OK) {
            /* DNLTODO */
      }

      return status;
}
#endif


//================================================================================================//
/**
 * @brief Function add to specified task's list information about allocated memory
 *
 * @param  pid    task id
 * @param *addr   address pointer
 * @param  size   allocated memory size
 */
//================================================================================================//
static void addMemInfo(PID_t pid, void *addr, u32_t size)
{
      if (moni) {
            while (TakeMutex(moni->mtx, MTX_BLOCK_TIME) != OS_OK);

            i32_t taskCount = ListGetItemCount(moni->tasks);

            /* find task */
            for (i32_t task = 0; task < taskCount; task++) {
                  struct taskdata *taskInfo = ListGetItemDataByNo(moni->tasks, task);

                  if (taskInfo == NULL)
                        goto addMem_end;

                  if (taskInfo->pid == pid) {
                        struct memalloc *mem;

                        i32_t block      = 0;
                        i32_t blockTotal = ListGetItemCount(taskInfo->memInfo);

                        while (TRUE) {
                              /* find empty address slot */
                              while (block < blockTotal) {
                                    mem = ListGetItemDataByNo(taskInfo->memInfo, block);

                                    if (mem)
                                          goto addMem_end;

                                    /* find empty address slot in block */
                                    for (i32_t slot = 0; slot < MEM_ADR_IN_BLOCK; slot++) {
                                          if (mem->addr[slot] == NULL) {
                                                mem->addr[slot] = addr;
                                                mem->size[slot] = size;

                                                goto addMem_end;
                                          }
                                    }

                                    block++;
                              }

                              /* block is full - create new block */
                              mem = calloc(1, sizeof(struct memalloc));

                              if (mem) {
                                    if (ListAddItem(taskInfo->memInfo, mem) < 0) {
                                          free(mem);
                                          goto addMem_end;
                                    } else {
                                          blockTotal++;
                                    }
                              } else {
                                    goto addMem_end;
                              }
                        }
                  }
            }

            addMem_end:
            GiveMutex(moni->mtx);
      }
}


//================================================================================================//
/**
 * @brief Function remove allocated memory from selected task
 *
 * @param  pid    task ID
 * @param *addr   allocated block
 */
//================================================================================================//
static void delMemInfo(PID_t pid, void *addr)
{
      if (moni) {
            while (TakeMutex(moni->mtx, MTX_BLOCK_TIME) != OS_OK);

            i32_t taskTotal = ListGetItemCount(moni->tasks);

            /* find task */
            for (i32_t task = 0; task < taskTotal; task++) {
                  struct taskdata *taskInfo = ListGetItemDataByNo(moni->tasks, task);

                  if (taskInfo == NULL)
                        goto delMem_end;

                  if (taskInfo->pid == pid) {
                        i32_t blockTotal = ListGetItemCount(taskInfo->memInfo);

                        /* find address slot */
                        for (i32_t blockNo = 0; blockNo < blockTotal; blockNo++) {
                              struct memalloc *mem = ListGetItemDataByNo(taskInfo->memInfo, blockNo);

                              if (mem)
                                    goto delMem_end;

                              /* find empty address slot in block */
                              for (i32_t slot = 0; slot < MEM_ADR_IN_BLOCK; slot++) {
                                    if (mem->addr[slot] == addr) {
                                          mem->addr[slot] = NULL;
                                          mem->size[slot] = 0;

                                          /* check if block is empty */
                                          for (u8_t i = 0; i < MEM_ADR_IN_BLOCK; i++) {
                                                if (mem->addr[i] != NULL)
                                                      goto delMem_end;
                                          }

                                          /* block is empty - freeing memory */
                                          ListRmItemByNo(taskInfo->memInfo, blockNo);
                                          goto delMem_end;
                                    }
                              }
                        }

                  }
            }

            delMem_end:
            GiveMutex(moni->mtx);
      }
}


#ifdef __cplusplus
}
#endif

/*==================================================================================================
                                            End of file
==================================================================================================*/
