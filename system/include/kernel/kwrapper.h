#ifndef KWRAPPER_H_
#define KWRAPPER_H_
/*=========================================================================*//**
@file    kwrapper.h

@author  Daniel Zorychta

@brief   Kernel wrapper

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
#include "kernel/ktypes.h"
#include "core/systypes.h"
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
#define STACK_DEPTH_MINIMAL                             (1  * (CONFIG_RTOS_TASK_MIN_STACK_DEPTH))
#define STACK_DEPTH_VERY_LOW                            (2  * (CONFIG_RTOS_TASK_MIN_STACK_DEPTH))
#define STACK_DEPTH_LOW                                 (4  * (CONFIG_RTOS_TASK_MIN_STACK_DEPTH))
#define STACK_DEPTH_MEDIUM                              (6  * (CONFIG_RTOS_TASK_MIN_STACK_DEPTH))
#define STACK_DEPTH_LARGE                               (8  * (CONFIG_RTOS_TASK_MIN_STACK_DEPTH))
#define STACK_DEPTH_VERY_LARGE                          (10 * (CONFIG_RTOS_TASK_MIN_STACK_DEPTH))
#define STACK_DEPTH_HUGE                                (12 * (CONFIG_RTOS_TASK_MIN_STACK_DEPTH))
#define STACK_DEPTH_VERY_HUGE                           (14 * (CONFIG_RTOS_TASK_MIN_STACK_DEPTH))

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
#define MS2TICK(ms)                                     ms <= (configTICK_RATE_HZ/1000) ? 1 : ms/(configTICK_RATE_HZ/1000)

/** TASK LEVEL DEFINITIONS */
#define new_task(func, name, stack_depth, args)         kwrap_new_task(func, name, stack_depth, args)
#define delete_task(taskhdl)                            kwrap_delete_task(taskhdl)
#define task_exit()                                     kwrap_task_exit()
#define sleep_ms(msdelay)                               vTaskDelay(msdelay)
#define sleep(seconds)                                  vTaskDelay((seconds) * 1000UL)
#define prepare_sleep_until()                           long int __last_wake_time__ = get_tick_counter();
#define sleep_until(seconds)                            vTaskDelayUntil(&__last_wake_time__, (seconds) * 1000UL)
#define sleep_ms_until(msdelay)                         vTaskDelayUntil(&__last_wake_time__, msdelay)
#define suspend_task(taskhdl)                           vTaskSuspend(taskhdl)
#define suspend_this_task()                             vTaskSuspend(THIS_TASK)
#define resume_task(taskhdl)                            vTaskResume(taskhdl)
#define resume_task_from_ISR(taskhdl)                   xTaskResumeFromISR(taskhdl)
#define yield_task()                                    taskYIELD()
#define enter_critical()                                taskENTER_CRITICAL()
#define exit_critical()                                 taskEXIT_CRITICAL()
#define disable_ISR()                                   taskDISABLE_INTERRUPTS()
#define enable_ISR()                                    taskENABLE_INTERRUPTS()
#define get_tick_counter()                              xTaskGetTickCount()
#define get_task_name(taskhdl)                          (char*)pcTaskGetTaskName(taskhdl)
#define get_this_task_name()                            (char*)pcTaskGetTaskName(THIS_TASK)
#define get_task_handle()                               xTaskGetCurrentTaskHandle()
#define get_task_priority(taskhdl)                      (i16_t)(uxTaskPriorityGet(taskhdl) - (CONFIG_RTOS_TASK_MAX_PRIORITIES / 2))
#define set_task_priority(taskhdl, priority)            vTaskPrioritySet(taskhdl, PRIORITY(priority))
#define set_priority(priority)                          vTaskPrioritySet(THIS_TASK, PRIORITY(priority))
#define get_free_stack()                                uxTaskGetStackHighWaterMark(THIS_TASK)
#define get_task_free_stack(taskhdl)                    uxTaskGetStackHighWaterMark(taskhdl)
#define get_number_of_tasks()                           uxTaskGetNumberOfTasks()
#define _set_task_tag(taskhdl, tag)                     vTaskSetApplicationTaskTag(taskhdl, tag)
#define _get_task_tag(taskhdl)                          (void*)xTaskGetApplicationTaskTag(taskhdl)
#define _get_this_task_data()                           ((struct task_data*)_get_task_tag(THIS_TASK))
#define _get_task_data(taskhdl)                         ((struct task_data*)_get_task_tag(taskhdl))
#define get_parent_handle()                             _get_task_data(THIS_TASK)->f_parent_task
#define set_global_variables(ptr)                       _get_task_data(THIS_TASK)->f_global_vars = ptr
#define set_stdin(file)                                 _get_task_data(THIS_TASK)->f_stdin = file
#define set_stdout(file)                                _get_task_data(THIS_TASK)->f_stdout = file
#define set_stderr(file)                                _get_task_data(THIS_TASK)->f_stderr = file
#define set_user_data(ptr)                              _get_task_data(THIS_TASK)->f_user = ptr
#define get_user_data()                                 _get_task_data(THIS_TASK)->f_user
#define _set_task_monitor_data(taskhdl, ptr)            _get_task_data(taskhdl)->f_monitor = ptr
#define _get_task_monitor_data(taskhdl)                 _get_task_data(taskhdl)->f_monitor

/** SEMAPHORES AND MUTEXES */
#define new_semaphore()                                 kwrap_create_binary_semaphore()
#define new_counting_semaphore(maxCnt, intCnt)          xSemaphoreCreateCounting(maxCnt, intCnt)
#define new_mutex()                                     xSemaphoreCreateMutex()
#define new_recursive_mutex()                           xSemaphoreCreateRecursiveMutex()
#define delete_semaphore(sem)                           vSemaphoreDelete(sem)
#define delete_counting_semaphore(sem)                  vSemaphoreDelete(sem)
#define delete_mutex(mutex)                             vSemaphoreDelete(mutex)
#define delete_recursive_mutex(mutex)                   vSemaphoreDelete(mutex)
#define take_semaphore(sem, blocktime)                  xSemaphoreTake(sem, (portTickType) blocktime)
#define give_semaphore(sem)                             xSemaphoreGive(sem)
#define take_semaphore_from_ISR(sem, woke)              xSemaphoreTakeFromISR(sem, woke)
#define give_semaphore_from_ISR(sem, woke)              xSemaphoreGiveFromISR(sem, woke)
#define take_counting_semaphore(sem, blocktime)         xSemaphoreTake(sem, (portTickType) blocktime)
#define give_counting_semaphore(sem)                    xSemaphoreGive(sem)
#define take_counting_semaphore_from_ISR(sem, woke)     xSemaphoreTakeFromISR(sem, woke)
#define give_counting_semaphore_from_ISR(sem, woke)     xSemaphoreGiveFromISR(sem, woke)
#define lock_mutex(mutex, blocktime)                    xSemaphoreTake(mutex, (portTickType) blocktime)
#define unlock_mutex(mutex)                             xSemaphoreGive(mutex)
#define lock_recursive_mutex(mutex, blocktime)          xSemaphoreTakeRecursive(mutex, (portTickType) blocktime)
#define unlock_recursive_mutex(mutex)                   xSemaphoreGiveRecursive(mutex)

/*==============================================================================
  Exported types, enums definitions
==============================================================================*/
struct task_data {
        file_t *f_stdin;        /* stdin file                         */
        file_t *f_stdout;       /* stdout file                        */
        file_t *f_stderr;       /* stderr file                        */
        char   *f_cwd;          /* current working path               */
        void   *f_global_vars;  /* address to global variables        */
        void   *f_user;         /* pointer to user data               */
        void   *f_monitor;      /* pointer to task monitor data       */
        task_t *f_parent_task;  /* program's parent task              */
        u32_t   f_cpu_usage;    /* counter used to calculate CPU load */
};

/*==============================================================================
  Exported object declarations
==============================================================================*/

/*==============================================================================
  Exported function prototypes
==============================================================================*/
extern task_t *kwrap_new_task(void (*func)(void*), const char*, u16_t, void*);
extern void    kwrap_delete_task(task_t *taskHdl);
extern sem_t  *kwrap_create_binary_semaphore(void);
extern void    kwrap_task_exit(void);

#ifdef __cplusplus
}
#endif

#endif /* KWRAPPER_H_ */
/*==============================================================================
  End of file
==============================================================================*/
