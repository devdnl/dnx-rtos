/*=========================================================================*//**
@file    c

@author  Daniel Zorychta

@brief   This module support process

@note    Copyright (C) 2013 Daniel Zorychta <daniel.zorychta@gmail.com>

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
#include "systypes.h"
#include "process.h"
#include "dlist.h"

/*==============================================================================
  Local symbolic constants/macros
==============================================================================*/
#define PROCESS_DEFAULT_PRIORITY        0

/*==============================================================================
  Local types, enums definitions
==============================================================================*/

/*==============================================================================
  Local function prototypes
==============================================================================*/
static i32_t add_process_to_list(task_t *task);
static void  delete_process_from_list(pid_t pid);
static void  lock_write_to_process_list(void);
static void  unlock_write_to_process_list(void);

/*==============================================================================
  Local object definitions
==============================================================================*/
static list_t  *list;
static mutex_t *list_mtx;
static uint     read_counter;
static pid_t    pid_counter;

/*==============================================================================
  Exported object definitions
==============================================================================*/

/*==============================================================================
  Function definitions
==============================================================================*/

//==============================================================================
/**
 * @brief Function initialize module
 */
//==============================================================================
stdRet_t proc_init(void)
{
        list     = new_list();
        list_mtx = new_mutex();

        if (!list || !list_mtx) {
                return STD_RET_ERROR;
        }

        return STD_RET_OK;
}

//==============================================================================
/**
 * @brief Function start new process
 *
 * @param[in ]  func          process code
 * @param[in ] *name          process name
 * @param[in ]  stack         stack deep
 * @param[in ] *argv          argument pointer
 *
 * @return PID number, otherwise -1 if error
 */
//==============================================================================
pid_t proc_new_process(void (*func)(void*), char *name, uint stack, void *argv)
{
        task_t *task;
        pid_t   pid  = -1;

        task = osw_new_task(func, name, stack, argv, PROCESS_DEFAULT_PRIORITY);
        if (task) {
                if (add_process_to_list(task) >= 0) {
                        pid = pid_counter++;
                } else {
                        osw_delete_task(task);
                }
        }

        return pid;
}

//==============================================================================
/**
 * @brief Function find and kill running process
 *
 * @param pid           process PID
 */
//==============================================================================
void proc_kill_process(pid_t pid)
{
        task_t *task = proc_get_process_task_handle(pid);

        delete_process_from_list(pid);

        if (task) {
                osw_delete_task(task);
        }
}

//==============================================================================
/**
 * @brief Function returns task handle based on PID
 *
 * @param pid           process PID
 *
 * @return task handle object, otherwise NULL if error
 */
//==============================================================================
task_t *proc_get_process_task_handle(pid_t pid)
{
        lock_write_to_process_list();
        task_t *task = list_get_iditem_data(list, pid);
        unlock_write_to_process_list();

        return task;
}

//==============================================================================
/**
 * @brief Function returns current process PID
 *
 * @return PID, otherwise -1 if error
 */
//==============================================================================
pid_t proc_get_pid(void)
{
        task_t *current_task = osw_get_task_handle();
        task_t *task;
        int     total_process;
        pid_t   pid = -1;

        lock_write_to_process_list();

        total_process = list_get_item_count(list);

        for (int item = 0; item < total_process; item++) {
                task = list_get_nitem_data(list, item);

                if (task == current_task) {
                        list_get_nitem_ID(list, item, (u32_t*)&pid);
                        break;
                }
        }

        unlock_write_to_process_list();

        return pid;
}

//==============================================================================
/**
 * @brief Function terminate process
 */
//==============================================================================
void proc_process_terminate(void)
{
        delete_process_from_list(proc_get_pid());
        osw_delete_task(THIS_TASK);
}

//==============================================================================
/**
 * @brief Function adds process to list
 *
 * @param *task         task handle
 *
 * @return list item number
 */
//==============================================================================
static i32_t add_process_to_list(task_t *task)
{
        i32_t item = -1;

        if (task == NULL) {
                return -1;
        }

        while (TRUE) {
                if (mutex_lock(list_mtx, 1000) == MUTEX_LOCKED) {
                        if (read_counter == 0) {
                                item = list_add_item(list,
                                                     pid_counter,
                                                     task);

                                mutex_unlock(list_mtx);
                                break;
                        }

                        mutex_unlock(list_mtx);
                }
        }

        return item;
}

//==============================================================================
/**
 * @brief Function adds process to list
 *
 * @param pid           PID
 *
 * @return list item number
 */
//==============================================================================
static void delete_process_from_list(pid_t pid)
{
        while (TRUE) {
                if (mutex_lock(list_mtx, 1000) == MUTEX_LOCKED) {
                        if (read_counter == 0) {
                                list_unlink_iditem_data(list, (u32_t)pid);
                                list_rm_iditem(list, (u32_t)pid);
                                mutex_unlock(list_mtx);
                                break;
                        }

                        mutex_unlock(list_mtx);
                }
        }
}

//==============================================================================
/**
 * @brief Function increase read counter of the process list
 *
 * @retval STD_RET_OK           counter increased
 * @retval STD_RET_ERROR        no operation
 */
//==============================================================================
static void lock_write_to_process_list(void)
{
        while (mutex_lock(list_mtx, 1000) != MUTEX_LOCKED);
        read_counter++;
        mutex_unlock(list_mtx);
}

//==============================================================================
/**
 * @brief Function decrease read counter of the process list
 *
 * @retval STD_RET_OK           counter increased
 * @retval STD_RET_ERROR        no operation
 */
//==============================================================================
static void unlock_write_to_process_list(void)
{
        while (mutex_lock(list_mtx, 1000) != MUTEX_LOCKED);
        read_counter--;
        mutex_unlock(list_mtx);
}

#ifdef __cplusplus
}
#endif

/*==============================================================================
  End of file
==============================================================================*/
