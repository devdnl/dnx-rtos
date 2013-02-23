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
#define MINIMAL_STACK_SIZE                      CONFIG_RTOS_TASK_MIN_STACK_SIZE
#define THIS_TASK                               NULL
#define EMPTY_TASK                              UINT32_MAX
#define OS_OK                                   pdTRUE
#define OS_NOT_OK                               pdFALSE

/** OS kernel control functions */
#define OSStartScheduler()                      vTaskStartScheduler()

/** TASK LEVEL DEFINITIONS */
#define TaskTerminate()                         delete_task(TaskGetCurrentTaskHandle())
#define TaskDelay(ms_delay)                     vTaskDelay(ms_delay)
#define TaskSuspend(taskID)                     vTaskSuspend(taskID)
#define TaskResume(taskID)                      vTaskResume(taskID)
#define TaskResumeFromISR(taskID)               xTaskResumeFromISR(taskID)
#define TaskYield()                             taskYIELD()
#define TaskEnterCritical()                     taskENTER_CRITICAL()
#define TaskExitCritical()                      taskEXIT_CRITICAL()
#define TaskDisableIRQ()                        taskDISABLE_INTERRUPTS()
#define TaskEnableIRQ()                         taskENABLE_INTERRUPTS()
#define TaskSuspendAll()                        vTaskSuspendAll()
#define TaskResumeAll()                         xTaskResumeAll()
#define TaskGetTickCount()                      xTaskGetTickCount()
#define TaskGetName(taskhdl)                    (ch_t*)pcTaskGetTaskName(taskhdl)
#define TaskGetCurrentTaskHandle()              xTaskGetCurrentTaskHandle()
#define TaskGetIdleTaskHandle()                 xTaskGetIdleTaskHandle()
#define TaskGetPriority(taskhdl)                (i16_t)(uxTaskPriorityGet(taskhdl) - (CONFIG_RTOS_TASK_MAX_PRIORITIES / 2))
#define TaskGetStackFreeSpace(taskhdl)          uxTaskGetStackHighWaterMark(taskhdl)
#define TaskGetNumberOfTasks()                  uxTaskGetNumberOfTasks()
#define TaskGetRunTimeStats(dst)                vTaskGetRunTimeStats((signed char*)dst)
#define TaskDelayUntil(lastTime, delay)         vTaskDelayUntil((portTickType*)lastTime, delay)
#define TaskSetTag(taskhdl, tag)                vTaskSetApplicationTaskTag(taskhdl, tag)
#define TaskGetTag(taskhdl)                     xTaskGetApplicationTaskTag(taskhdl)

/** SEMAPHORES AND MUTEXES */
#define new_semaphore()                         create_semaphore()
#define new_semaphore_counting(maxCnt, intCnt)  xSemaphoreCreateCounting(maxCnt, intCnt)
#define new_mutex()                             xSemaphoreCreateMutex()
#define new_recursive_mutex()                   xSemaphoreCreateRecursiveMutex()
#define delete_semaphore(sem)                   vSemaphoreDelete(sem)
#define delete_semaphore_counting(sem)          vSemaphoreDelete(sem)
#define delete_mutex(mutex)                     vSemaphoreDelete(mutex)
#define delete_mutex_recursive(mutex)           vSemaphoreDelete(mutex)
#define semaphore_take(sem, blocktime)          xSemaphoreTake(sem, (portTickType) blocktime)
#define semaphore_counting_take(sem, blocktime) xSemaphoreTake(sem, (portTickType) blocktime)
#define semaphore_give(sem)                     xSemaphoreGive(sem)
#define semaphore_counting_give(sem)            xSemaphoreGive(sem)
#define semaphore_take_from_ISR(sem, woke)      xSemaphoreTakeFromISR(sem, woke)
#define semaphore_counting_take_from_ISR(sem, woke) xSemaphoreTakeFromISR(sem, woke)
#define semaphore_give_from_ISR(sem, woke)      xSemaphoreGiveFromISR(sem, woke)
#define semaphore_counting_give_from_ISR(sem, woke) xSemaphoreGiveFromISR(sem, woke)
#define mutex_lock(mutex, blocktime)            xSemaphoreTake(mutex, (portTickType) blocktime)
#define mutex_recursive_lock(mutex, blocktime)  xSemaphoreTakeRecursive(mutex, (portTickType) blocktime)
#define mutex_unlock(mutex)                     xSemaphoreGive(mutex)
#define mutex_recursive_unlock(mutex)           xSemaphoreGiveRecursive(mutex)

/*==============================================================================
  Exported types, enums definitions
==============================================================================*/

/*==============================================================================
  Exported object declarations
==============================================================================*/

/*==============================================================================
  Exported function prototypes
==============================================================================*/
extern int_t new_task(taskCode_t taskCode, const ch_t *name, u16_t stack,
                        void *argv, i8_t priority, task_t *taskHdl);
extern void  delete_task(task_t taskHdl);
extern sem_t create_semaphore(void);

#ifdef __cplusplus
}
#endif

#endif /* OSWRAP_H_ */
/*==============================================================================
  End of file
==============================================================================*/
