#ifndef OSWRAP_H_
#define OSWRAP_H_
/*=========================================================================*//**
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


*//*==========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Include files
==============================================================================*/
#include "basic_types.h"
#include "ostypes.h"
#include "systypes.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "config.h"

/*==============================================================================
  Exported symbolic constants/macros
==============================================================================*/
/** UNDEFINE MEMORY MANAGEMENT DEFINITIONS LOCALIZED IN FreeRTOS.h file (IMPORTANT!) */
#undef free
#undef malloc

/** OS BASIC DEFINITIONS */
#define MINIMAL_STACK_SIZE                CONFIG_RTOS_TASK_MIN_STACK_SIZE
#define THIS_TASK                         NULL
#define EMPTY_TASK                        UINT32_MAX
#define OS_OK                             pdTRUE
#define OS_NOT_OK                         pdFALSE

/** OS kernel control functions */
#define OSStartScheduler()                vTaskStartScheduler()

/** TASK LEVEL DEFINITIONS */
#define TaskTerminate()                   TaskDelete(TaskGetCurrentTaskHandle())
#define TaskDelay(ms_delay)               vTaskDelay(ms_delay)
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
#define TaskGetPriority(taskhdl)          (i16_t)(uxTaskPriorityGet(taskhdl) - (CONFIG_RTOS_TASK_MAX_PRIORITIES / 2))
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

/*==============================================================================
  Exported types, enums definitions
==============================================================================*/

/*==============================================================================
  Exported object declarations
==============================================================================*/

/*==============================================================================
  Exported function prototypes
==============================================================================*/
extern int_t TaskCreate(taskCode_t taskCode, const ch_t *name, u16_t stack,
                        void *argv, i8_t priority, task_t *taskHdl);
extern void  TaskDelete(task_t taskHdl);
extern sem_t CreateSemBin(void);

#ifdef __cplusplus
}
#endif

#endif /* OSWRAP_H_ */
/*==============================================================================
  End of file
==============================================================================*/
