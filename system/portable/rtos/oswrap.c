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
 *
 * @return task object pointer or NULL if error
 */
//==============================================================================
task_t *osw_new_task(taskCode_t taskCode, const char *name, u16_t stackDeep,
                     void *argv, i8_t priority)
{
        vTaskSuspendAll();

        task_t *task = NULL;

        if (xTaskCreate(taskCode, (signed char *)name, stackDeep, argv,
                        PRIORITY(priority), &task) == OS_OK) {

                tskm_add_task(task);
        }

        xTaskResumeAll();

        return task;
}

//==============================================================================
/**
 * @brief Function delete task and if enabled remove from monitor list
 *
 * @param *taskHdl       task handle
 */
//==============================================================================
void osw_delete_task(task_t *taskHdl)
{
        tskm_remove_task(taskHdl);
        vTaskDelete(taskHdl);
}

//==============================================================================
/**
 * @brief Function create binary semaphore
 *
 * @return binary semaphore object
 */
//==============================================================================
sem_t *osw_create_binary_semaphore(void)
{
        sem_t *sem = NULL;

        vSemaphoreCreateBinary(sem);

        return sem;
}

#ifdef __cplusplus
}
#endif

/*==============================================================================
  End of file
==============================================================================*/
