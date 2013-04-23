/*=========================================================================*//**
@file    kwrapper.c

@author  Daniel Zorychta

@brief   Kernel wrapper

@note    Copyright (C) 2012, 2013 Daniel Zorychta <daniel.zorychta@gmail.com>

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
#include "kernel/kwrapper.h"
#include "core/sysmoni.h"

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
 * Function by default allocate memory for task data (localized in task tag)
 * which is used to cpu load calculation and standard IO and etc.
 *
 * @param[in ] *func            task code
 * @param[in ] *name            task name
 * @param[in ]  stack_depth     stack deep
 * @param[in ] *argv            argument pointer
 *
 * @return task object pointer or NULL if error
 */
//==============================================================================
task_t *kwrap_new_task(void (*func)(void*), const char *name, u16_t stack_depth, void *argv)
{
        task_t           *task          = NULL;
        uint             child_priority = PRIORITY(0);
        struct task_data *data;

        if (!(data = sysm_kcalloc(1, sizeof(struct task_data)))) {
                return NULL;
        }

        data->f_parent_task = xTaskGetCurrentTaskHandle();

        if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED) {
                child_priority = uxTaskPriorityGet(THIS_TASK);
        }

        taskENTER_CRITICAL();
        if (xTaskCreate(func, (signed char *)name, stack_depth,
                        argv, child_priority, &task) == OS_OK) {

                vTaskSuspend(task);
                taskEXIT_CRITICAL();
                vTaskSetApplicationTaskTag(task, (void *)data);

                if (sysm_start_task_monitoring(task) == STD_RET_OK) {
                        vTaskResume(task);
                } else {
                        vTaskDelete(task);
                        sysm_kfree(data);
                        task = NULL;
                }
        } else {
                taskEXIT_CRITICAL();
                sysm_kfree(data);
        }

        return task;
}

//==============================================================================
/**
 * @brief Function delete task
 * Function remove task from monitoring list, and next delete the task from OS
 * list. Function resume the parent task before delete.
 *
 * @param *taskHdl       task handle
 */
//==============================================================================
void kwrap_delete_task(task_t *taskHdl)
{
        struct task_data *data;

        if (sysm_is_task_exist(taskHdl)) {
                (void)sysm_stop_task_monitoring(taskHdl);

                taskENTER_CRITICAL();
                if ((data = (void *)xTaskGetApplicationTaskTag(taskHdl))) {

                        vTaskSetApplicationTaskTag(taskHdl, NULL);

                        taskYIELD();
                        if (data->f_parent_task) {
                                vTaskResume(data->f_parent_task);
                        }

                        sysm_kfree(data);
                }

                vTaskDelete(taskHdl);
                taskEXIT_CRITICAL();
        }
}

//==============================================================================
/**
 * @brief Function create binary semaphore
 *
 * @return binary semaphore object
 */
//==============================================================================
sem_t *kwrap_create_binary_semaphore(void)
{
        sem_t *sem = NULL;
        vSemaphoreCreateBinary(sem);
        return sem;
}

//==============================================================================
/**
 * @brief Function wait for task exit
 */
//==============================================================================
void kwrap_task_exit(void)
{
        do {
                /* request to delete task */
                delete_task(get_task_handle());

                /* wait for exit */
                for (;;) {}
        } while(0);
}

#ifdef __cplusplus
}
#endif

/*==============================================================================
  End of file
==============================================================================*/
