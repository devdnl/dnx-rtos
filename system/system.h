#ifndef SYSTEM_H_
#define SYSTEM_H_
/*=============================================================================================*//**
@file    gpio.h

@author  Daniel Zorychta

@brief   System main header

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
#include "stm32f10x.h"

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"


/*==================================================================================================
                                  Exported symbolic constants/macros
==================================================================================================*/
#define MINIMAL_STACK_SIZE              configMINIMAL_STACK_SIZE
#define THIS_TASK                       NULL

#define TaskTerminate()                 vTaskDelete(NULL)
#define TaskDelete(taskID)              vTaskDelete(taskID)
#define TaskDelay(delay)                vTaskDelay(delay)
#define TaskSuspend(taskID)             vTaskSuspend(taskID)
#define TaskResume(taskID)              vTaskResume(taskID)
#define TaskResumeFromISR(taskID)       xTaskResumeFromISR(taskID)
#define TaskYield()                     taskYIELD()
#define TaskEnterCritical()             taskENTER_CRITICAL()
#define TaskExitCritical()              taskEXIT_CRITICAL()
#define TaskDisableIRQ()                taskDISABLE_INTERRUPTS()
#define TaskEnableIRQ()                 taskENABLE_INTERRUPTS()
#define TaskSuspendAll()                vTaskSuspendAll()
#define TaskResumeAll()                 xTaskResumeAll()
#define TaskGetTickCount()              xTaskGetTickCount()
#define YieldFromISR()                  portYIELD_FROM_ISR()
#define GetStackFreeSpace(taskID)       uxTaskGetStackHighWaterMark(taskID)
#define GetFreeHeapSize()               xPortGetFreeHeapSize()
#define Malloc(size)                    pvPortMalloc(size)
#define Free(pv)                        vPortFree(pv)

#define TaskCreate(pvTaskCode, pcName, usStackDepth, pvParameters, uxPriority, pvCreatedTask) \
        xTaskCreate(pvTaskCode, (signed char *)pcName, usStackDepth, pvParameters, uxPriority, pvCreatedTask)

#define TaskDelayUntil(pPreviousWakeTime, TimeIncrement) \
        vTaskDelayUntil(pPreviousWakeTime, TimeIncrement)


/*==================================================================================================
                                  Exported types, enums definitions
==================================================================================================*/


/*==================================================================================================
                                     Exported object declarations
==================================================================================================*/


/*==================================================================================================
                                     Exported function prototypes
==================================================================================================*/


#ifdef __cplusplus
   }
#endif

#endif /* SYSTEM_H_ */
/*==================================================================================================
                                             End of file
==================================================================================================*/
