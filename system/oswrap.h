#ifndef OSWRAP_H_
#define OSWRAP_H_
/*=============================================================================================*//**
@file    oswrap.h

@author  Daniel Zorychta

@brief   Operating system wrapper

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


*//*==============================================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*==================================================================================================
                                            Include files
==================================================================================================*/
#include "basic_types.h"
#include "systypes.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"


/*==================================================================================================
                                  Exported symbolic constants/macros
==================================================================================================*/
#define MINIMAL_STACK_SIZE                configMINIMAL_STACK_SIZE
#define THIS_TASK                         NULL
#define EMPTY_TASK                        UINT32_MAX

/** TASK LEVEL DEFINITIONS */
#define TaskTerminate()                   TaskDelete(TaskGetCurrentTaskHandle())
#define TaskDelay(delay)                  vTaskDelay(delay)
#define TaskSuspend(taskID)               vTaskSuspend(taskID)
#define TaskResume(taskID)                vTaskResume(taskID)
#define TaskResumeFromISR(taskID)         xTaskResumeFromISR(taskID)
#define TaskYield()                       taskYIELD()
#define TaskEnterCritical()               taskENTER_CRITICAL()
#define TaskExitCritical()                taskEXIT_CRITICAL()
#define TaskDisableIRQ()                  taskDISABLE_INTERRUPTS()
#define TaskEnableIRQ()                   taskENABLE_INTERRUPTS()
#define TaskSuspendAll()                  vTaskSuspendAll()
#define TaskResumeAll()                   xTaskResumeAll()
#define TaskGetTickCount()                xTaskGetTickCount()
#define TaskGetName(taskhdl)              (ch_t*)pcTaskGetTaskName(taskhdl)
#define TaskGetCurrentTaskHandle()        xTaskGetCurrentTaskHandle()
#define TaskGetIdleTaskHandle()           xTaskGetIdleTaskHandle()
#define TaskGetPriority(taskhdl)          (i16_t)(uxTaskPriorityGet(taskhdl) - (configMAX_PRIORITIES / 2))
#define TaskGetStackFreeSpace(taskhdl)    uxTaskGetStackHighWaterMark(taskhdl)
#define TaskGetNumberOfTasks()            uxTaskGetNumberOfTasks()
#define TaskGetRunTimeStats(dst)          vTaskGetRunTimeStats((signed char*)dst)
#define TaskDelayUntil(lastTime, delay)   vTaskDelayUntil((portTickType*)lastTime, delay)
#define TaskSetTag(taskhdl, tag)          vTaskSetApplicationTaskTag(taskhdl, tag)
#define TaskGetTag(taskhdl)               xTaskGetApplicationTaskTag(taskhdl)

/** SEMAPHORES AND MUTEXES */
#define CreateSemCnt(maxCnt, intCnt)      xSemaphoreCreateCounting(maxCnt, intCnt)
#define CreateMutex()                     xSemaphoreCreateMutex()
#define CreateRecMutex()                  xSemaphoreCreateRecursiveMutex()
#define DeleteSemBin(sem)                 vSemaphoreDelete(sem)
#define DeleteSemCnt(sem)                 vSemaphoreDelete(sem)
#define DeleteMutex(mutex)                vSemaphoreDelete(mutex)
#define DeleteRecMutex(mutex)             vSemaphoreDelete(mutex)
#define TakeSemBin(sem, blocktime)        xSemaphoreTake(sem, (portTickType) blocktime)
#define TakeSemCnt(sem, blocktime)        xSemaphoreTake(sem, (portTickType) blocktime)
#define TakeMutex(mutex, blocktime)       xSemaphoreTake(mutex, (portTickType) blocktime)
#define TakeRecMutex(mutex, blocktime)    xSemaphoreTakeRecursive(mutex, (portTickType) blocktime)
#define GiveSemBin(sem)                   xSemaphoreGive(sem)
#define GiveSemCnt(sem)                   xSemaphoreGive(sem)
#define GiveMutex(mutex)                  xSemaphoreGive(mutex)
#define GiveRecMutex(mutex)               xSemaphoreGiveRecursive(mutex)
#define GiveSemBinFromISR(sem, woke)      xSemaphoreGiveFromISR(sem, woke)
#define GiveSemCntFromISR(sem, woke)      xSemaphoreGiveFromISR(sem, woke)

#define OS_OK                             pdTRUE
#define OS_NOT_OK                         pdFALSE


/*==================================================================================================
                                  Exported types, enums definitions
==================================================================================================*/
typedef xTaskHandle      task_t;
typedef pdTASK_CODE      taskCode_t;
typedef xSemaphoreHandle sem_t;
typedef xSemaphoreHandle mutex_t;


/*==================================================================================================
                                     Exported object declarations
==================================================================================================*/


/*==================================================================================================
                                     Exported function prototypes
==================================================================================================*/
extern int_t TaskCreate(taskCode_t taskCode, const ch_t *name, u16_t stack, void *argv, i8_t priority, task_t *taskHdl);
extern void  TaskDelete(task_t taskHdl);
extern sem_t CreateSemBin(void);

#ifdef __cplusplus
}
#endif

#endif /* OSWRAP_H_ */
/*==================================================================================================
                                             End of file
==================================================================================================*/
