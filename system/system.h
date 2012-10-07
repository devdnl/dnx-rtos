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
#include "systypes.h"
#include "stm32f10x.h"
#include "pll_cfg.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "printf.h"
#include "appruntime.h"
#include "netconf.h"


/*==================================================================================================
                                  Exported symbolic constants/macros
==================================================================================================*/
#define MINIMAL_STACK_SIZE                configMINIMAL_STACK_SIZE
#define THIS_TASK                         NULL
#define EMPTY_TASK                        UINT32_MAX

/** TASK LEVEL DEFINITIONS */
#define TaskTerminate()                   vTaskDelete(NULL)
#define TaskDelete(taskID)                vTaskDelete(taskID)
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
#define TaskGetPID()                      xTaskGetPID()
#define TaskGetCurrentTaskHandle()        xTaskGetCurrentTaskHandle()
#define TaskGetStackFreeSpace(taskID)     uxTaskGetStackHighWaterMark(taskID)
#define GetFreeHeapSize()                 xPortGetFreeHeapSize()
#define Malloc(size)                      pvPortMalloc(size)
#define Free(pv)                          vPortFree(pv)

#define TaskCreate(pvTaskCode, pcName, usStackDepth, pvParameters, uxPriority, pvCreatedTask) \
        xTaskCreate(pvTaskCode, (signed char *)pcName, usStackDepth, pvParameters, uxPriority, pvCreatedTask)

#define TaskDelayUntil(pPreviousWakeTime, TimeIncrement) \
        vTaskDelayUntil(pPreviousWakeTime, TimeIncrement)


/** APPLICATION LEVEL DEFINITIONS */
#define Sleep(delay)                      vTaskDelay(delay)
#define SystemGetTickCount()              xTaskGetTickCount()
#define SystemGetStackFreeSpace()         uxTaskGetStackHighWaterMark(THIS_TASK)
#define SystemEnterCritical()             taskENTER_CRITICAL()
#define SystemExitCritical()              taskEXIT_CRITICAL()
#define SystemDisableIRQ()                taskDISABLE_INTERRUPTS()
#define SystemEnableIRQ()                 taskENABLE_INTERRUPTS()
#define SystemGetPID()                    xTaskGetPID()
#define SystemGetAppHandle()              xTaskGetCurrentTaskHandle()
#define SystemAppSuspend()                vTaskSuspend(NULL)
#define SystemLockContent()               vTaskSuspendAll()
#define SystemUnlockContent()             xTaskResumeAll()
#define SystemGetFreeMemSize()            xPortGetFreeHeapSize()
#define SystemGetMemSize()                configTOTAL_HEAP_SIZE
#define SystemReboot()                    NVIC_SystemReset()
#define SystemGetHostname()               LwIP_GetHostname()


/** application preamble */
#define APPLICATION(name)                 void name(void *appArgument)
#define APP_SEC_BEGIN                     { InitApp(); clearSTDIN();
#define APP_SEC_END                       Exit(appmain(argv));}

#define InitApp()                         stdioFIFO_t *stdin  = ((appArgs_t *)appArgument)->stdin; \
                                          stdioFIFO_t *stdout = ((appArgs_t *)appArgument)->stdout;\
                                          ch_t        *argv   = ((appArgs_t *)appArgument)->arg

/** array element count */
#define ARRAY_SIZE(array)                 (sizeof(array)/sizeof(array[0]))


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
