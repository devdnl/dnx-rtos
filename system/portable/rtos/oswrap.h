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

/** STANDART STACK SIZES */
#define STACK_MINIMAL_SIZE                              (1  * (CONFIG_RTOS_TASK_MIN_STACK_SIZE))
#define STACK_LOW_SIZE                                  (2  * (CONFIG_RTOS_TASK_MIN_STACK_SIZE))
#define STACK_MEDIUM_SIZE                               (4  * (CONFIG_RTOS_TASK_MIN_STACK_SIZE))
#define STACK_LARGE_SIZE                                (8  * (CONFIG_RTOS_TASK_MIN_STACK_SIZE))
#define STACK_HUGE_SIZE                                 (16 * (CONFIG_RTOS_TASK_MIN_STACK_SIZE))

/** OS BASIC DEFINITIONS */
#define THIS_TASK                                       NULL
#define OS_OK                                           pdTRUE
#define OS_NOT_OK                                       pdFALSE

/** MUTEX AND SEMAPHORES DEFINITIONS */
#define MUTEX_LOCKED                                    OS_OK
#define SEMAPHORE_TAKEN                                 OS_OK

/** OS kernel control functions */
#define start_task_scheduler()                          vTaskStartScheduler()

/** CALCULATIONS */
#define PRIORITY(prio)                                  (prio + (configMAX_PRIORITIES / 2))

/** TASK LEVEL DEFINITIONS */
#define milisleep(msdelay)                              vTaskDelay(msdelay)
#define sleep(seconds)                                  vTaskDelay((seconds) * 1000UL)
#define prepare_sleep_until()                           long int __last_wake_time__ = get_tick_counter();
#define sleep_until(seconds)                            vTaskDelayUntil(&__last_wake_time__, (seconds) * 1000UL)
#define milisleep_until(msdelay)                        vTaskDelayUntil(&__last_wake_time__, msdelay)
#define osw_suspend_task(taskhdl)                       vTaskSuspend(taskhdl)
#define suspend_this_process()                          vTaskSuspend(THIS_TASK)
//#define resume_task(taskhdl)                            vTaskResume(taskhdl)
#define osw_resume_task_from_ISR(taskhdl)               xTaskResumeFromISR(taskhdl)
#define suspend_all_process()                           vTaskSuspendAll()
#define resume_all_process()                            xTaskResumeAll()
#define yield_process()                                 taskYIELD()
#define enter_critical()                                taskENTER_CRITICAL()
#define exit_critical()                                 taskEXIT_CRITICAL()
#define disable_ISR()                                   taskDISABLE_INTERRUPTS()
#define enable_ISR()                                    taskENABLE_INTERRUPTS()
#define get_tick_counter()                              xTaskGetTickCount()
#define osw_get_task_name(taskhdl)                      (char*)pcTaskGetTaskName(taskhdl)
#define get_this_process_name()                         (char*)pcTaskGetTaskName(THIS_TASK)
#define osw_get_task_handle()                           xTaskGetCurrentTaskHandle()
#define osw_get_task_priority(taskhdl)                  (i16_t)(uxTaskPriorityGet(taskhdl) - (CONFIG_RTOS_TASK_MAX_PRIORITIES / 2))
#define get_free_stack()                                uxTaskGetStackHighWaterMark(THIS_TASK)
#define osw_get_task_free_stack(taskhdl)                uxTaskGetStackHighWaterMark(taskhdl)
#define get_number_of_process()                         uxTaskGetNumberOfTasks()
#define set_task_tag(taskhdl, tag)                      vTaskSetApplicationTaskTag(taskhdl, tag)
#define get_task_tag(taskhdl)                           xTaskGetApplicationTaskTag(taskhdl)

/** SEMAPHORES AND MUTEXES */
#define new_semaphore()                                 osw_create_binary_semaphore()
#define new_semaphore_counting(maxCnt, intCnt)          xSemaphoreCreateCounting(maxCnt, intCnt)
#define new_mutex()                                     xSemaphoreCreateMutex()
#define new_recursive_mutex()                           xSemaphoreCreateRecursiveMutex()
#define delete_semaphore(sem)                           vSemaphoreDelete(sem)
#define delete_semaphore_counting(sem)                  vSemaphoreDelete(sem)
#define delete_mutex(mutex)                             vSemaphoreDelete(mutex)
#define delete_mutex_recursive(mutex)                   vSemaphoreDelete(mutex)
#define semaphore_take(sem, blocktime)                  xSemaphoreTake(sem, (portTickType) blocktime)
#define semaphore_counting_take(sem, blocktime)         xSemaphoreTake(sem, (portTickType) blocktime)
#define semaphore_give(sem)                             xSemaphoreGive(sem)
#define semaphore_counting_give(sem)                    xSemaphoreGive(sem)
#define semaphore_take_from_ISR(sem, woke)              xSemaphoreTakeFromISR(sem, woke)
#define semaphore_counting_take_from_ISR(sem, woke)     xSemaphoreTakeFromISR(sem, woke)
#define semaphore_give_from_ISR(sem, woke)              xSemaphoreGiveFromISR(sem, woke)
#define semaphore_counting_give_from_ISR(sem, woke)     xSemaphoreGiveFromISR(sem, woke)
#define mutex_lock(mutex, blocktime)                    xSemaphoreTake(mutex, (portTickType) blocktime)
#define mutex_recursive_lock(mutex, blocktime)          xSemaphoreTakeRecursive(mutex, (portTickType) blocktime)
#define mutex_unlock(mutex)                             xSemaphoreGive(mutex)
#define mutex_recursive_unlock(mutex)                   xSemaphoreGiveRecursive(mutex)

/*==============================================================================
  Exported types, enums definitions
==============================================================================*/

/*==============================================================================
  Exported object declarations
==============================================================================*/

/*==============================================================================
  Exported function prototypes
==============================================================================*/
extern task_t *osw_new_task(taskCode_t, const char*, u16_t, void*, i8_t);
extern void    osw_delete_task(task_t *taskHdl);
extern sem_t  *osw_create_binary_semaphore(void);

#ifdef __cplusplus
}
#endif

#endif /* OSWRAP_H_ */
/*==============================================================================
  End of file
==============================================================================*/
