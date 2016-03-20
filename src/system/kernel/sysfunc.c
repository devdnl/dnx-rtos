/*=========================================================================*//**
@file    sysfunc.c

@author  Daniel Zorychta

@brief

@note    Copyright (C) 2015 Daniel Zorychta <daniel.zorychta@gmail.com>

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

/*==============================================================================
  Include files
==============================================================================*/
#include "kernel/sysfunc.h"

/*==============================================================================
  Local macros
==============================================================================*/

/*==============================================================================
  Local object types
==============================================================================*/

/*==============================================================================
  Local function prototypes
==============================================================================*/

/*==============================================================================
  Local objects
==============================================================================*/

/*==============================================================================
  Exported objects
==============================================================================*/

/*==============================================================================
  External objects
==============================================================================*/

/*==============================================================================
  Function definitions
==============================================================================*/

//==============================================================================
/**
 * @brief Function open directory
 *
 * @param[in]  path                 directory path
 * @param[out] dir                  pointer to dir pointer
 *
 * @return One of errno values
 */
//==============================================================================
int sys_opendir(const char *path, DIR **dir)
{
        int r = _vfs_opendir(path, dir);
        if (r == ESUCC) {
                _process_register_resource(_process_get_container_by_task(_THIS_TASK, NULL),
                                           cast(res_header_t*, *dir));
        }
        return r;
}

//==============================================================================
/**
 * @brief Function close opened directory
 *
 * @param[in] *dir                  directory object
 *
 * @return One of errno values
 */
//==============================================================================
int sys_closedir(DIR *dir)
{
        return _process_release_resource(_process_get_container_by_task(_THIS_TASK, NULL),
                                         cast(res_header_t *, dir),
                                         RES_TYPE_DIR);
}

//==============================================================================
/**
 * @brief Function open selected file
 *
 * @param[in]  *name             file path
 * @param[in]  *mode             file mode
 * @param[out] **file            pointer to file pointer
 *
 * @return One of errno values
 */
//==============================================================================
int sys_fopen(const char *name, const char *mode, FILE **file)
{
        int r = _vfs_fopen(name, mode, file);
        if (r == ESUCC) {
                _process_register_resource(_process_get_container_by_task(_THIS_TASK, NULL),
                                           cast(res_header_t*, *file));
        }
        return r;
}

//==============================================================================
/**
 * @brief Function close opened file
 *
 * @param[in] file              pinter to file
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
int sys_fclose(FILE *file)
{
        return _process_release_resource(_process_get_container_by_task(_THIS_TASK, NULL),
                                         cast(res_header_t *, file),
                                         RES_TYPE_FILE);
}

//==============================================================================
/**
 * @brief Function create binary semaphore
 *
 * @param[in]  cnt_max          max count value (1 for binary)
 * @param[in]  cnt_init         initial value (0 or 1 for binary)
 * @param[out] sem              created semaphore handle
 *
 * @return One of errno values.
 */
//==============================================================================
int sys_semaphore_create(const size_t cnt_max, const size_t cnt_init, sem_t **sem)
{
        int r = _semaphore_create(cnt_max, cnt_init, sem);
        if (r == ESUCC) {
                _process_register_resource(_process_get_container_by_task(_THIS_TASK, NULL),
                                           cast(res_header_t*, *sem));
        }
        return r;
}

//==============================================================================
/**
 * @brief Function delete semaphore
 *
 * @param[in] *sem      semaphore object
 */
//==============================================================================
int sys_semaphore_destroy(sem_t *sem)
{
        return _process_release_resource(_process_get_container_by_task(_THIS_TASK, NULL),
                                         cast(res_header_t *, sem),
                                         RES_TYPE_SEMAPHORE);
}

//==============================================================================
/**
 * @brief Function create new mutex
 *
 * @param[in]  type     mutex type
 * @param[out] mtx      created mutex handle
 *
 * @return One of errno values.
 */
//==============================================================================
int sys_mutex_create(enum mutex_type type, mutex_t **mtx)
{
        int r = _mutex_create(type, mtx);
        if (r == ESUCC) {
                _process_register_resource(_process_get_container_by_task(_THIS_TASK, NULL),
                                           cast(res_header_t*, *mtx));
        }
        return r;
}

//==============================================================================
/**
 * @brief Function destroy mutex
 *
 * @param[in] *mutex    mutex object
 *
 * @return One of errno values.
 */
//==============================================================================
int sys_mutex_destroy(mutex_t *mutex)
{
        return _process_release_resource(_process_get_container_by_task(_THIS_TASK, NULL),
                                         cast(res_header_t *, mutex),
                                         RES_TYPE_MUTEX);
}

//==============================================================================
/**
 * @brief Function create new queue
 *
 * @param[in]  length           queue length
 * @param[in]  item_size        queue item size
 * @param[out] queue            created queue
 *
 * @return One of errno values.
 */
//==============================================================================
int sys_queue_create(const uint length, const uint item_size, queue_t **queue)
{
        int r = _queue_create(length, item_size, queue);
        if (r == ESUCC) {
                _process_register_resource(_process_get_container_by_task(_THIS_TASK, NULL),
                                           cast(res_header_t*, *queue));
        }
        return r;
}

//==============================================================================
/**
 * @brief Function delete queue
 *
 * @param[in] *queue            queue object
 *
 * @return One of errno values.
 */
//==============================================================================
int sys_queue_destroy(queue_t *queue)
{
        return _process_release_resource(_process_get_container_by_task(_THIS_TASK, NULL),
                                         cast(res_header_t *, queue),
                                         RES_TYPE_QUEUE);
}

//==============================================================================
/**
 * @brief Function create new task and if enabled add to monitor list
 *
 * Function by default allocate memory for task data (localized in task tag)
 * which is used to cpu load calculation and standard IO and etc.
 *
 * @param[in ] func             thread code
 * @param[in ] attr             thread attribute (NULL for default)
 * @param[in ] arg              argument pointer (can be NULL)
 * @param[in ] thread           thread parameters (task and ID) (can be NULL)
 * @param[out] task             task handle
 *
 * @return On of errno value.
 */
//==============================================================================
int sys_thread_create(thread_func_t func, const thread_attr_t *attr, void *arg, thread_t *thread)
{
        int result = EINVAL;
        if (thread) {
                result = _process_thread_create(_process_get_container_by_task(_THIS_TASK, NULL),
                                                func, attr, true, arg, &thread->tid, &thread->task);
        }
        return result;
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
int sys_thread_destroy(thread_t *thread)
{
        int result = EINVAL;

        if (thread) {
                _process_t *proc = _process_get_container_by_task(_THIS_TASK, NULL);
                _thread_t  *thr  = _process_thread_get_container(proc, thread->tid);
                result           = _process_release_resource(proc,
                                                             cast(res_header_t*, thr),
                                                             RES_TYPE_THREAD);
                if (result == ESUCC) {
                        thread->task = NULL;
                        thread->tid  = 0;
                }
        }

        return result;
}

//==============================================================================
/**
 * @brief  Function return this thread object
 *
 * @param[out] thread   thread information
 *
 * @return One of errno value
 */
//==============================================================================
int sys_thread_self(thread_t *thread)
{
        int result = EINVAL;

        if (thread) {
                _thread_t *thr = _task_get_tag(_THIS_TASK);
                if (thr && cast(res_header_t*, thr)->type == RES_TYPE_THREAD) {
                        result  = _process_thread_get_task(thr, &thread->task);
                        result |= _process_thread_get_tid(thr, &thread->tid);
                } else {
                        result = ESRCH;
                }
        }

        return result;
}

/*==============================================================================
  End of file
==============================================================================*/
