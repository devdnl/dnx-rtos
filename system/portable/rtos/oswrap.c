/*=========================================================================*//**
@file    oswrap.c

@author  Daniel Zorychta

@brief   Operating system wrapper

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
#include "oswrap.h"
#include "taskmoni.h"

/*==============================================================================
  Local symbolic constants/macros
==============================================================================*/
#define Priority(prio)                    (prio + (configMAX_PRIORITIES / 2))

/*==============================================================================
  Local types, enums definitions
==============================================================================*/

/*==============================================================================
  Local function prototypes
==============================================================================*/

/*==============================================================================
  Local object definitions
==============================================================================*/

/*==============================================================================
  Exported object definitions
==============================================================================*/

/*==============================================================================
  Function definitions
==============================================================================*/

//==============================================================================
/**
 * @brief Function create new task and if enabled add to monitor list
 *
 * @param[in ]  taskCode      task code
 * @param[in ] *name          task name
 * @param[in ]  stackDeep     stack deep
 * @param[in ] *argv          argument pointer
 * @param[in ]  priority      task priority (calculated to FreeRTOS priority level)
 * @param[out] *taskHdl       task handle
 *
 * @retval OS_OK
 * @retval OS_NOT_OK
 */
//==============================================================================
int_t TaskCreate(taskCode_t taskCode, const ch_t *name, u16_t stackDeep,
                 void *argv, i8_t priority, task_t *taskHdl)
{
        TaskSuspendAll();

        task_t task;

        int_t status = xTaskCreate(taskCode,
                                   (signed char *)name,
                                   stackDeep,
                                   argv,
                                   Priority(priority),
                                   &task);

        if (taskHdl) {
                *taskHdl = task;
        }

        if (status == OS_OK) {
                moni_AddTask(task);
        }

        TaskResumeAll();

        return status;
}

//==============================================================================
/**
 * @brief Function delete task and if enabled remove from monitor list
 *
 * @param taskHdl       task handle
 */
//==============================================================================
void TaskDelete(task_t taskHdl)
{
        moni_DelTask(taskHdl);
        vTaskDelete(taskHdl);
}

//==============================================================================
/**
 * @brief Function create binary semaphore
 *
 * @return binary semaphore object
 */
//==============================================================================
sem_t CreateSemBin(void)
{
        sem_t sem = NULL;

        vSemaphoreCreateBinary(sem);

        return sem;
}

#ifdef __cplusplus
}
#endif

/*==============================================================================
  End of file
==============================================================================*/
