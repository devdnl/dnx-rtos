/*=========================================================================*//**
@file    sysfunc.c

@author  Daniel Zorychta

@brief   System functions.

@note    Copyright (C) 2015 Daniel Zorychta <daniel.zorychta@gmail.com>

         This program is free software; you can redistribute it and/or modify
         it under the terms of the GNU General Public License as published by
         the Free Software Foundation and modified by the dnx RTOS exception.

         NOTE: The modification  to the GPL is  included to allow you to
               distribute a combined work that includes dnx RTOS without
               being obliged to provide the source  code for proprietary
               components outside of the dnx RTOS.

         The dnx RTOS  is  distributed  in the hope  that  it will be useful,
         but WITHOUT  ANY  WARRANTY;  without  even  the implied  warranty of
         MERCHANTABILITY  or  FITNESS  FOR  A  PARTICULAR  PURPOSE.  See  the
         GNU General Public License for more details.

         Full license text is available on the following file: doc/license.txt.


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
int sys_opendir(const char *path, kdir_t **dir)
{
        struct vfs_path cpath;
        cpath.CWD  = NULL;
        cpath.PATH = path;

        int r = _vfs_opendir(&cpath, dir);
        if (r == ESUCC) {
                _process_register_resource(_kworker_proc, cast(res_header_t*, *dir));
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
int sys_closedir(kdir_t *dir)
{
        return _process_release_resource(_kworker_proc,
                                         cast(res_header_t *, dir),
                                         RES_TYPE_DIR);
}

//==============================================================================
/**
 * @brief Function open selected file
 *
 * @param[in]  *name             file path
 * @param[in]   flags            flags
 * @param[out] **file            pointer to file pointer
 *
 * @return One of errno values
 */
//==============================================================================
int sys_fopen(const char *name, int flags, kfile_t **file)
{
        struct vfs_path cpath;
        cpath.CWD  = NULL;
        cpath.PATH = name;

        int r = _vfs_fopen(&cpath, flags, file);
        if (r == ESUCC) {
                _process_register_resource(_kworker_proc, cast(res_header_t*, *file));
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
int sys_fclose(kfile_t *file)
{
        return _process_release_resource(_kworker_proc,
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
int sys_semaphore_create(const size_t cnt_max, const size_t cnt_init, ksem_t **sem)
{
        int r = _semaphore_create(cnt_max, cnt_init, sem);
        if (r == ESUCC) {
                _process_register_resource(_kworker_proc, cast(res_header_t*, *sem));
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
int sys_semaphore_destroy(ksem_t *sem)
{
        return _process_release_resource(_kworker_proc,
                                         cast(res_header_t *, sem),
                                         RES_TYPE_SEMAPHORE);
}

//==============================================================================
/**
 * @brief Function creates flags. Flag is similar to semaphore except single
 *        object contains more than one synchronization point.
 *
 * @note Function can be used only by file system or driver code.
 *
 * @param flag            created flag handle
 *
 * @return One of @ref errno value.
 */
//==============================================================================
int sys_flag_create(kflag_t **flag)
{
        int r = _flag_create(flag);
        if (r == ESUCC) {
                _process_register_resource(_kworker_proc, cast(res_header_t*, *flag));
        }
        return r;
}

//==============================================================================
/**
 * @brief Function deletes flag.
 *
 * @note Function can be used only by file system or driver code.
 *
 * @param flag      flag object
 *
 * @return One of @ref errno value.
 */
//==============================================================================
int sys_flag_destroy(kflag_t *flag)
{
        return _process_release_resource(_kworker_proc,
                                         cast(res_header_t *, flag),
                                         RES_TYPE_FLAG);
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
int sys_mutex_create(enum kmtx_type type, kmtx_t **mtx)
{
        int r = _mutex_create(type, mtx);
        if (r == ESUCC) {
                _process_register_resource(_kworker_proc, cast(res_header_t*, *mtx));
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
int sys_mutex_destroy(kmtx_t *mutex)
{
        return _process_release_resource(_kworker_proc,
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
int sys_queue_create(size_t length, size_t item_size, kqueue_t **queue)
{
        int r = _queue_create(length, item_size, queue);
        if (r == ESUCC) {
                _process_register_resource(_kworker_proc, cast(res_header_t*, *queue));
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
int sys_queue_destroy(kqueue_t *queue)
{
        return _process_release_resource(_kworker_proc,
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
int sys_thread_create(thread_func_t func, const _thread_attr_t *attr, void *arg, tid_t *tid)
{
        return _process_thread_create(_kworker_proc, func, attr, arg, tid);
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
int sys_thread_destroy(tid_t tid)
{
        return _process_thread_kill(_kworker_proc, tid);
}

/*==============================================================================
  End of file
==============================================================================*/
