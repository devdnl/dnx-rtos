/*=========================================================================*//**
@file    procfs.c

@author  Daniel Zorychta

@brief   This file support process file system (procfs)

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
#include "fs/procfs.h"
#include "core/io.h"
#include "core/list.h"

/*==============================================================================
  Local symbolic constants/macros
==============================================================================*/
/* task ID string length (8B name + \0). ID is 32b hex number converted to string */
#define TASK_ID_STR_LEN                   9

#define DIR_TASKID_STR                    "taskid"
#define DIR_TASKNAME_STR                  "taskname"
#define TASK_FILE_NAME_STR                "name"
#define TASK_FILE_PRIO_STR                "priority"
#define TASK_FILE_FREESTACK_STR           "freestack"
#define TASK_FILE_USEDMEM_STR             "usedmem"
#define TASK_FILE_OPENFILES_STR           "openfiles"

#define MTX_BLOCK_TIME                    10

/*==============================================================================
  Local types, enums definitions
==============================================================================*/
enum taskInfFile {
      TASK_FILE_NAME,
      TASK_FILE_PRIO,
      TASK_FILE_FREESTACK,
      TASK_FILE_USEDMEM,
      TASK_FILE_OPENFILES,
      TASK_FILE_COUNT,
      TASK_FILE_NONE
};

struct procfs {
      list_t  *file_list;
      u32_t    ID_counter;
      mutex_t *resource_mtx;
};

struct file_info {
      task_t *taskhdl;         /* task handle */
      u8_t    task_file;       /* task info file */
};

/*==============================================================================
  Local function prototypes
==============================================================================*/
static stdret_t procfs_closedir_freedd  (void *fs_handle, dir_t *dir);
static stdret_t procfs_closedir_noop    (void *fs_handle, dir_t *dir);
static dirent_t procfs_readdir_root     (void *fs_handle, dir_t *dir);
static dirent_t procfs_readdir_taskname (void *fs_handle, dir_t *dir);
static dirent_t procfs_readdir_taskid   (void *fs_handle, dir_t *dir);
static dirent_t procfs_readdir_taskid_n (void *fs_handle, dir_t *dir);

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
 * @brief Initialize file system
 *
 * @param[out] **fs_handle      pointer to allocated memory by file system
 * @param[in]  *src_path        file source path
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
stdret_t procfs_init(void **fs_handle, const char *src_path)
{
        UNUSED_ARG(src_path);

        STOP_IF(!fs_handle);

        struct procfs *procmem;
        if (!(procmem = calloc(1, sizeof(struct procfs)))) {
                return STD_RET_ERROR;
        }

        procmem->file_list    = new_list();
        procmem->resource_mtx = new_mutex();

        if (!procmem->file_list || !procmem->resource_mtx) {
                if (procmem->file_list) {
                        delete_list(procmem->file_list);
                }

                if (procmem->resource_mtx) {
                        delete_mutex(procmem->resource_mtx);
                }

                free(procmem);
                return STD_RET_ERROR;
        } else {
                *fs_handle = procmem;
                return STD_RET_OK;
        }
}

//==============================================================================
/**
 * @brief Function release file system
 *
 * @param[in] *fs_handle            FS handle
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
stdret_t procfs_release(void *fs_handle)
{
        STOP_IF(!fs_handle);

        struct procfs *procmem = fs_handle;

        while (lock_mutex(procmem->resource_mtx, MTX_BLOCK_TIME) != MUTEX_LOCKED);
        enter_critical_section();
        unlock_mutex(procmem->resource_mtx);
        delete_mutex(procmem->resource_mtx);
        delete_list(procmem->file_list);
        free(procmem);
        exit_critical_section();

        return STD_RET_OK;
}

//==============================================================================
/**
 * @brief Function open selected file
 *
 * @param[in]  *fs_handle       FS handle
 * @param[out] *extra           file extra data
 * @param[out] *fd              file descriptor
 * @param[out] *lseek           file position
 * @param[in]  *path            file path
 * @param[in]   flags           file open flags
 *
 * @retval STD_RET_OK           file opened/created
 * @retval STD_RET_ERROR        file not opened/created
 */
//==============================================================================
stdret_t procfs_open(void *fs_handle, void **extra, fd_t *fd, u64_t *lseek, const char *path, int flags)
{
        UNUSED_ARG(extra);

        STOP_IF(!fs_handle);
        STOP_IF(!fd);
        STOP_IF(!lseek);
        STOP_IF(!path);

        struct procfs          *procmem = fs_handle;
        struct sysmoni_taskstat taskdata;
        struct file_info       *fileInf;

        if (flags != O_RDONLY) {
                return STD_RET_ERROR;
        }

        if (strncmp(path, "/"DIR_TASKID_STR"/", strlen(DIR_TASKID_STR) + 2) == 0) {
                path = strchr(path + 1, '/') + 1;

                if (path == NULL) {
                        return STD_RET_ERROR;
                }

                task_t *taskHdl = NULL;
                path = strtoi((char*)path, 16, (i32_t*)&taskHdl);

                if (sysm_get_task_stat(taskHdl, &taskdata) != STD_RET_OK) {
                        return STD_RET_ERROR;
                }

                if ((path = strrchr(path, '/')) == NULL) {
                        return STD_RET_ERROR;
                } else {
                        path++;
                }

                fileInf = calloc(1, sizeof(struct file_info));
                if (fileInf == NULL) {
                        return STD_RET_ERROR;
                }

                fileInf->taskhdl = taskHdl;

                if (strcmp((char*) path, TASK_FILE_NAME_STR) == 0) {
                        fileInf->task_file = TASK_FILE_NAME;
                } else if (strcmp((char*) path, TASK_FILE_FREESTACK_STR) == 0) {
                        fileInf->task_file = TASK_FILE_FREESTACK;
                } else if (strcmp((char*) path, TASK_FILE_OPENFILES_STR) == 0) {
                        fileInf->task_file = TASK_FILE_OPENFILES;
                } else if (strcmp((char*) path, TASK_FILE_PRIO_STR) == 0) {
                        fileInf->task_file = TASK_FILE_PRIO;
                } else if (strcmp((char*) path, TASK_FILE_USEDMEM_STR) == 0) {
                        fileInf->task_file = TASK_FILE_USEDMEM;
                } else {
                        free(fileInf);
                        return STD_RET_ERROR;
                }

                while (lock_mutex(procmem->resource_mtx, MTX_BLOCK_TIME) != MUTEX_LOCKED);

                if (list_add_item(procmem->file_list, procmem->ID_counter, fileInf) == 0) {
                        *fd    = procmem->ID_counter++;
                        *lseek = 0;

                        unlock_mutex(procmem->resource_mtx);
                        return STD_RET_OK;
                }

                unlock_mutex(procmem->resource_mtx);
                free(fileInf);
                return STD_RET_ERROR;

        } else if (strncmp(path, "/"DIR_TASKNAME_STR"/",
                           strlen(DIR_TASKNAME_STR) + 2) == 0) {

                path = strrchr(path, '/');

                if (path == NULL) {
                        return STD_RET_ERROR;
                } else {
                        path++;
                }

                u16_t n = sysm_get_number_of_monitored_tasks();
                u16_t i = 0;

                while (n-- && sysm_get_ntask_stat(i++, &taskdata) == STD_RET_OK) {
                        if (strcmp(path, taskdata.task_name) != 0) {
                                continue;
                        }

                        fileInf = calloc(1, sizeof(struct file_info));
                        if (fileInf == NULL) {
                                return STD_RET_ERROR;
                        }

                        fileInf->taskhdl   = taskdata.task_handle;
                        fileInf->task_file = TASK_FILE_NONE;

                        while (lock_mutex(procmem->resource_mtx, MTX_BLOCK_TIME) != MUTEX_LOCKED);

                        if (list_add_item(procmem->file_list,
                                        procmem->ID_counter, fileInf) == 0) {

                                *fd = procmem->ID_counter++;
                                *lseek = 0;

                                unlock_mutex(procmem->resource_mtx);
                                return STD_RET_OK;
                        }

                        unlock_mutex(procmem->resource_mtx);
                        free(fileInf);
                        return STD_RET_ERROR;
                }
        }

        return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief Function close file in LFS
 *
 * @param[in] *fs_handle        FS handle
 * @param[in] *extra            file extra data (useful in FS wrappers)
 * @param[in]  fd               file descriptor
 * @param[in]  forced           force close
 * @param[in] *task             task which opened file
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
stdret_t procfs_close(void *fs_handle, void *extra, fd_t fd, bool forced, task_t *task)
{
        UNUSED_ARG(extra);
        UNUSED_ARG(forced);
        UNUSED_ARG(task);

        STOP_IF(!fs_handle);

        struct procfs *procmem = fs_handle;
        if (procmem) {
                while (lock_mutex(procmem->resource_mtx, MTX_BLOCK_TIME) != MUTEX_LOCKED);

                if (list_rm_iditem(procmem->file_list, fd) == STD_RET_OK) {
                        unlock_mutex(procmem->resource_mtx);
                        return STD_RET_OK;
                }

                unlock_mutex(procmem->resource_mtx);
        }

        return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief Function write data to the file
 *
 * @param[in] *fs_handle        FS handle
 * @param[in] *extra            file extra data (useful in FS wrappers)v
 * @param[in]  fd               file descriptor
 * @param[in] *src              data source
 * @param[in]  size             item size
 * @param[in]  nitems           number of items
 * @param[in]  lseek            position in file
 *
 * @return number of written items
 */
//==============================================================================
size_t procfs_write(void *fs_handle,void *extra, fd_t fd, const void *src, size_t size, size_t nitems, u64_t lseek)
{
        UNUSED_ARG(fs_handle);
        UNUSED_ARG(extra);
        UNUSED_ARG(fd);
        UNUSED_ARG(src);
        UNUSED_ARG(size);
        UNUSED_ARG(nitems);
        UNUSED_ARG(lseek);

        return 0;
}

//==============================================================================
/**
 * @brief Function read from file data
 *
 * @param[in]  *fs_handle       FS handle
 * @param[in]  *extra           file extra data (useful in FS wrappers)
 * @param[in]   fd              file descriptor
 * @param[out] *dst             data destination
 * @param[in]   size            item size
 * @param[in]   nitems          number of items
 * @param[in]   lseek           position in file
 *
 * @return number of read items
 */
//==============================================================================
size_t procfs_read(void *fs_handle, void *extra, fd_t fd, void *dst, size_t size, size_t nitems, u64_t lseek)
{
        UNUSED_ARG(extra);

        STOP_IF(!fs_handle);
        STOP_IF(!dst);
        STOP_IF(!size);
        STOP_IF(!nitems);

        struct procfs *procmem = fs_handle;

        while (lock_mutex(procmem->resource_mtx, MTX_BLOCK_TIME) != MUTEX_LOCKED);
        struct file_info *fileInf = list_get_iditem_data(procmem->file_list, fd);
        unlock_mutex(procmem->resource_mtx);

        if (fileInf == NULL) {
                return 0;
        }

        if (fileInf->task_file >= TASK_FILE_COUNT) {
                return 0;
        }

        struct sysmoni_taskstat taskInfo;
        taskInfo.cpu_usage    = 1000;
        taskInfo.free_stack   = 0;
        taskInfo.task_handle  = 0;
        taskInfo.memory_usage = 0;
        taskInfo.task_name    = NULL;
        taskInfo.opened_files = 0;
        taskInfo.priority     = 0;


        if (sysm_get_task_stat(fileInf->taskhdl, &taskInfo) != STD_RET_OK) {
                return 0;
        }

        u8_t dataSize;
        if (CONFIG_RTOS_TASK_NAME_LEN > 12) {
                dataSize = CONFIG_RTOS_TASK_NAME_LEN + 1;
        } else {
                dataSize = 12;
        }

        char *data = calloc(dataSize + 1, 1);
        if (!data) {
                return 0;
        }

        switch (fileInf->task_file) {
        case TASK_FILE_FREESTACK:
                dataSize = snprintf(data, dataSize, "%u\n", taskInfo.free_stack);
                break;

        case TASK_FILE_NAME:
                dataSize = snprintf(data, dataSize, "%s\n", taskInfo.task_name);
                break;

        case TASK_FILE_OPENFILES:
                dataSize = snprintf(data, dataSize, "%u\n", taskInfo.opened_files);
                break;

        case TASK_FILE_PRIO:
                dataSize = snprintf(data, dataSize, "%d\n", taskInfo.priority);
                break;

        case TASK_FILE_USEDMEM:
                dataSize = snprintf(data, dataSize, "%u\n", taskInfo.memory_usage);
                break;
        }

        size_t n;
        size_t seek = lseek > SIZE_MAX ? SIZE_MAX : lseek;
        if (seek > dataSize) {
                n = 0;
        } else {
                if (dataSize - seek <= size * nitems) {
                        n = dataSize - seek;
                        strncpy(dst, data + seek, n);
                } else {
                        n = size * nitems;
                        strncpy(dst, data + seek, n);
                }
        }

        free(data);
        return n;
}

//==============================================================================
/**
 * @brief IO operations on files
 *
 * @param[in]     *fs_handle    FS handle
 * @param[in]     *extra        file extra data (useful in FS wrappers)
 * @param[in]      fd           file descriptor
 * @param[in]      iorq         request
 * @param[in,out]  args         additional arguments
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
stdret_t procfs_ioctl(void *fs_handle, void *extra, fd_t fd, int iorq, va_list args)
{
        UNUSED_ARG(fs_handle);
        UNUSED_ARG(extra);
        UNUSED_ARG(fd);
        UNUSED_ARG(iorq);
        UNUSED_ARG(args);

        return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief Function flush file data
 *
 * @param[in]     *fs_handle    FS handle
 * @param[in]     *extra        file extra data (useful in FS wrappers)
 * @param[in]      fd           file descriptor
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
stdret_t procfs_flush(void *fs_handle, void *extra, fd_t fd)
{
        UNUSED_ARG(fs_handle);
        UNUSED_ARG(extra);
        UNUSED_ARG(fd);

        return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief Function returns file status
 *
 * @param[in]  *fs_handle            FS handle
 * @param[in]  *extra                file extra data (useful in FS wrappers)
 * @param[in]   fd                   file descriptor
 * @param[out] *stat                 pointer to status structure
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
stdret_t procfs_fstat(void *fs_handle, void *extra, fd_t fd, struct vfs_stat *stat)
{
        UNUSED_ARG(extra);

        STOP_IF(!fs_handle);
        STOP_IF(!stat);

        struct procfs *procmem = fs_handle;

        while (lock_mutex(procmem->resource_mtx, MTX_BLOCK_TIME) != MUTEX_LOCKED);
        struct file_info *fileInf = list_get_iditem_data(procmem->file_list, fd);
        unlock_mutex(procmem->resource_mtx);

        if (fileInf == NULL) {
                return STD_RET_ERROR;
        }

        if (fileInf->task_file >= TASK_FILE_COUNT) {
                return STD_RET_ERROR;
        }

        struct sysmoni_taskstat taskInfo;
        if (sysm_get_task_stat(fileInf->taskhdl, &taskInfo) != STD_RET_OK) {
                return STD_RET_ERROR;
        }

        stat->st_dev   = 0;
        stat->st_mode  = OWNER_MODE(MODE_R) | GROUP_MODE(MODE_R) | OTHER_MODE(MODE_R);
        stat->st_mtime = 0;
        stat->st_size  = 0;
        stat->st_gid   = 0;
        stat->st_uid   = 0;

        char data[12] = {0};

        switch (fileInf->task_file) {
        case TASK_FILE_FREESTACK:
                stat->st_size = snprintf(data, sizeof(data), "%u\n", taskInfo.free_stack);
                break;

        case TASK_FILE_NAME:
                stat->st_size = strlen(taskInfo.task_name) + 1;
                break;

        case TASK_FILE_OPENFILES:
                stat->st_size = snprintf(data, sizeof(data), "%u\n", taskInfo.opened_files);
                break;

        case TASK_FILE_PRIO:
                stat->st_size = snprintf(data, sizeof(data), "%d\n", taskInfo.priority);
                break;

        case TASK_FILE_USEDMEM:
                stat->st_size = snprintf(data, sizeof(data), "%u\n", taskInfo.memory_usage);
                break;
        }

        return STD_RET_OK;
}

//==============================================================================
/**
 * @brief Create directory
 *
 * @param[in] *fs_handle        FS handle
 * @param[in] *path             path to new directory
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
stdret_t procfs_mkdir(void *fs_handle, const char *path)
{
        UNUSED_ARG(fs_handle);
        UNUSED_ARG(path);

        return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief Function create node for driver file
 *
 * @param[in] *fs_handle        FS handle
 * @param[in] *path             path when driver-file shall be created
 * @param[in] *drv_if           pointer to driver interface
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
stdret_t procfs_mknod(void *fs_handle, const char *path, struct vfs_drv_interface *drv_if)
{
        UNUSED_ARG(fs_handle);
        UNUSED_ARG(path);
        UNUSED_ARG(drv_if);

        return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief Function open directory
 *
 * @param[in]  *fs_handle       FS handle
 * @param[in]  *path            directory path
 * @param[out] *dir             directory info
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
stdret_t procfs_opendir(void *fs_handle, const char *path, dir_t *dir)
{
        UNUSED_ARG(fs_handle);

        STOP_IF(!path);
        STOP_IF(!dir);

        dir->f_seek = 0;

        if (strcmp(path, "/") == 0) {
                dir->f_dd       = NULL;
                dir->f_items    = 2;
                dir->f_readdir  = procfs_readdir_root;
                dir->f_closedir = procfs_closedir_noop;
                return STD_RET_OK;
        } else if (strcmp(path, "/"DIR_TASKNAME_STR"/") == 0) {
                dir->f_dd       = NULL;
                dir->f_items    = sysm_get_number_of_monitored_tasks();
                dir->f_readdir  = procfs_readdir_taskname;
                dir->f_closedir = procfs_closedir_noop;
                return STD_RET_OK;
        } else if (strcmp(path, "/"DIR_TASKID_STR"/") == 0) {
                dir->f_dd       = calloc(TASK_ID_STR_LEN, sizeof(char));
                dir->f_items    = sysm_get_number_of_monitored_tasks();
                dir->f_readdir  = procfs_readdir_taskid;
                dir->f_closedir = procfs_closedir_freedd;
                return STD_RET_OK;
        } else if (strncmp(path, "/"DIR_TASKID_STR"/", strlen(DIR_TASKID_STR) + 2) == 0) {

                path = strchr(path + 1, '/') + 1;

                if (path == NULL) {
                        return STD_RET_ERROR;
                }

                task_t *taskHdl = NULL;
                path = strtoi((char*)path, 16, (i32_t*)&taskHdl);

                if (!((*path == '/' && *(path + 1) == '\0') || *path == '\0')) {
                        return STD_RET_ERROR;
                }

                struct sysmoni_taskstat taskdata;
                if (sysm_get_task_stat(taskHdl, &taskdata) == STD_RET_OK) {
                        dir->f_dd       = (void*)taskHdl;
                        dir->f_items    = TASK_FILE_COUNT;
                        dir->f_readdir  = procfs_readdir_taskid_n;
                        dir->f_closedir = procfs_closedir_noop;
                        return STD_RET_OK;
                }
        }

        return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief Function close opened dir (is used when dd contains pointer to
 *        allocated block)
 *
 * @param[in]  *fs_handle    file system data
 * @param[out] *dir          directory object
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
static stdret_t procfs_closedir_freedd(void *fs_handle, dir_t *dir)
{
        UNUSED_ARG(fs_handle);

        STOP_IF(!dir);

        if (dir->f_dd) {
                free(dir->f_dd);
                dir->f_dd = NULL;
                return STD_RET_OK;
        }

        return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief Function close opened dir (is used when dd contains data which cannot
 *        be freed)
 *
 * @param[in]  *fs_handle    file system data
 * @param[out] *dir          directory object
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
static stdret_t procfs_closedir_noop(void *fs_handle, dir_t *dir)
{
        UNUSED_ARG(fs_handle);
        UNUSED_ARG(dir);

        return STD_RET_OK;
}

//==============================================================================
/**
 * @brief Remove file
 *
 * @param[in] *fs_handle        FS handle
 * @param[in] *patch            localization of file/directory
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
stdret_t procfs_remove(void *fs_handle, const char *path)
{
        UNUSED_ARG(fs_handle);
        UNUSED_ARG(path);

        return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief Rename file name
 *
 * @param[in] *fs_handle            FS handle
 * @param[in] *oldName              old file name
 * @param[in] *newName              new file name
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
stdret_t procfs_rename(void *fs_handle, const char *old_name, const char *new_name)
{
        UNUSED_ARG(fs_handle);
        UNUSED_ARG(old_name);
        UNUSED_ARG(new_name);

        return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief Function change file mode
 *
 * @param[in] *fs_handle            FS handle
 * @param[in] *path                 path
 * @param[in]  mode                 file mode
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
stdret_t procfs_chmod(void *fs_handle, const char *path, int mode)
{
        UNUSED_ARG(fs_handle);
        UNUSED_ARG(path);
        UNUSED_ARG(mode);

        return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief Function change file owner and group
 *
 * @param[in] *fs_handle            FS handle
 * @param[in] *path                 path
 * @param[in]  owner                file owner
 * @param[in]  group                file group
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
stdret_t procfs_chown(void *fs_handle, const char *path, int owner, int group)
{
        UNUSED_ARG(fs_handle);
        UNUSED_ARG(path);
        UNUSED_ARG(owner);
        UNUSED_ARG(group);

        return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief Function returns file/dir status
 *
 * @param[in]  *fs_handle            FS handle
 * @param[in]  *path                 file/dir path
 * @param[out] *stat                 pointer to stat structure
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
stdret_t procfs_stat(void *fs_handle, const char *path, struct vfs_stat *stat)
{
        UNUSED_ARG(fs_handle);
        UNUSED_ARG(path);

        STOP_IF(!stat);

        stat->st_dev   = 0;
        stat->st_gid   = 0;
        stat->st_mode  = OWNER_MODE(MODE_R) | GROUP_MODE(MODE_R) | OTHER_MODE(MODE_R);
        stat->st_mtime = 0;
        stat->st_size  = 0;
        stat->st_uid   = 0;

        return STD_RET_OK;
}

//==============================================================================
/**
 * @brief Function returns FS status
 *
 * @param[in]  *fs_handle           FS handle
 * @param[out] *statfs              pointer to status structure
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
stdret_t procfs_statfs(void *fs_handle, struct vfs_statfs *statfs)
{
        UNUSED_ARG(fs_handle);

        STOP_IF(!statfs);

        statfs->f_bfree  = 0;
        statfs->f_blocks = 0;
        statfs->f_ffree  = 0;
        statfs->f_files  = 0;
        statfs->f_type   = 1;
        statfs->fsname   = "procfs";

        return STD_RET_OK;
}

//==============================================================================
/**
 * @brief Read item from opened directory
 *
 * @param[in]  *fs_handle    file system data
 * @param[out] *dir          directory object
 *
 * @return directory entry
 */
//==============================================================================
static dirent_t procfs_readdir_root(void *fs_handle, dir_t *dir)
{
        UNUSED_ARG(fs_handle);

        STOP_IF(!dir);

        dirent_t dirent;
        dirent.name     = NULL;
        dirent.size     = 0;
        dirent.filetype = FILE_TYPE_DIR;
        dirent.size     = 0;

        switch (dir->f_seek++) {
        case 0:
                dirent.name = DIR_TASKID_STR;
                break;
        case 1:
                dirent.name = DIR_TASKNAME_STR;
                break;
        default:
                break;
        }

        return dirent;
}

//==============================================================================
/**
 * @brief Read item from opened directory
 *
 * @param[in]  *fs_handle    file system data
 * @param[out] *dir          directory object
 *
 * @return directory entry
 */
//==============================================================================
static dirent_t procfs_readdir_taskname(void *fs_handle, dir_t *dir)
{
        UNUSED_ARG(fs_handle);

        STOP_IF(!dir);

        dirent_t dirent;
        dirent.name = NULL;
        dirent.size = 0;

        struct sysmoni_taskstat taskdata;
        if (sysm_get_ntask_stat(dir->f_seek, &taskdata) == STD_RET_OK) {
                dirent.filetype = FILE_TYPE_REGULAR;
                dirent.name     = taskdata.task_name;
                dirent.size     = 0;

                dir->f_seek++;
        }

        return dirent;
}

//==============================================================================
/**
 * @brief Read item from opened directory
 *
 * @param[in]  *fs_handle    file system data
 * @param[out] *dir          directory object
 *
 * @return directory entry
 */
//==============================================================================
static dirent_t procfs_readdir_taskid(void *fs_handle, dir_t *dir)
{
        UNUSED_ARG(fs_handle);

        STOP_IF(!dir);

        dirent_t dirent;
        dirent.name = NULL;
        dirent.size = 0;

        if (dir->f_dd && dir->f_seek < (size_t)sysm_get_number_of_monitored_tasks()) {
                struct sysmoni_taskstat taskdata;
                if (sysm_get_ntask_stat(dir->f_seek, &taskdata) == STD_RET_OK) {
                        snprintf(dir->f_dd, TASK_ID_STR_LEN, "%x", (int)taskdata.task_handle);

                        dirent.filetype = FILE_TYPE_DIR;
                        dirent.name     = dir->f_dd;
                        dirent.size     = 0;

                        dir->f_seek++;
                }
        }

        return dirent;
}

//==============================================================================
/**
 * @brief Read item from opened directory
 *
 * @param[in]  *fs_handle    file system data
 * @param[out] *dir          directory object
 *
 * @return directory entry
 */
//==============================================================================
static dirent_t procfs_readdir_taskid_n(void *fs_handle, dir_t *dir)
{
        UNUSED_ARG(fs_handle);

        STOP_IF(!dir);

        dirent_t dirent;
        dirent.name = NULL;
        dirent.size = 0;

        if (dir->f_seek >= TASK_FILE_COUNT) {
                return dirent;
        }

        struct sysmoni_taskstat taskdata;
        if (sysm_get_task_stat((task_t*)dir->f_dd, &taskdata) != STD_RET_OK) {
                return dirent;
        }

        char data[12]   = {0};
        dirent.filetype = FILE_TYPE_REGULAR;

        switch (dir->f_seek) {
        case TASK_FILE_NAME:
                dirent.name = TASK_FILE_NAME_STR;
                dirent.size = strlen(taskdata.task_name) + 1;
                break;

        case TASK_FILE_PRIO:
                dirent.name = TASK_FILE_PRIO_STR;
                dirent.size = snprintf(data, ARRAY_SIZE(data), "%d\n", taskdata.priority);
                break;

        case TASK_FILE_FREESTACK:
                dirent.name = TASK_FILE_FREESTACK_STR;
                dirent.size = snprintf(data, ARRAY_SIZE(data), "%u\n", taskdata.free_stack);
                break;

        case TASK_FILE_USEDMEM:
                dirent.name = TASK_FILE_USEDMEM_STR;
                dirent.size = snprintf(data, ARRAY_SIZE(data), "%d\n", taskdata.memory_usage);
                break;

        case TASK_FILE_OPENFILES:
                dirent.name = TASK_FILE_OPENFILES_STR;
                dirent.size = snprintf(data, ARRAY_SIZE(data), "%d\n", taskdata.opened_files);
                break;
        }

        dir->f_seek++;
        return dirent;
}

#ifdef __cplusplus
}
#endif

/*==============================================================================
  End of file
==============================================================================*/
