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
#include "procfs.h"
#include "sysdrv.h"
#include "io.h"
#include "dlist.h"
#include "cpuctl.h"
#include <string.h>

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
#define TASK_FILE_CPULOAD_STR             "cpuload"

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
      TASK_FILE_CPULOAD,
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
static stdret_t procfs_closedir_freedd(void *fshdl, dir_t *dir);
static stdret_t procfs_closedir_noop(void *fshdl, dir_t *dir);
static dirent_t procfs_readdir_root(void *fshdl, dir_t *dir);
static dirent_t procfs_readdir_taskname(void *fshdl, dir_t *dir);
static dirent_t procfs_readdir_taskid(void *fshdl, dir_t *dir);
static dirent_t procfs_readdir_taskid_n(void *fshdl, dir_t *dir);

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
 * @param[out] **fshdl          pointer to allocated memory by file system
 * @param[in]  *src_path        file source path
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
stdret_t procfs_init(void **fshdl, const char *src_path)
{
        (void)src_path;
        struct procfs *procmem;

        if (!fshdl) {
                return STD_RET_ERROR;
        }

        if (!(procmem = kcalloc(1, sizeof(struct procfs)))) {
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

                kfree(procmem);
                return STD_RET_ERROR;
        } else {
                *fshdl = procmem;
                return STD_RET_OK;
        }
}

//==============================================================================
/**
 * @brief Function release file system
 *
 * @param[in] *fshdl            FS handle
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
stdret_t procfs_release(void *fshdl)
{
        struct procfs *procmem = fshdl;

        if (!procmem) {
                return STD_RET_ERROR;
        }

        while (lock_mutex(procmem->resource_mtx, MTX_BLOCK_TIME) != MUTEX_LOCKED);
        enter_critical();
        unlock_mutex(procmem->resource_mtx);
        delete_mutex(procmem->resource_mtx);
        delete_list(procmem->file_list);
        kfree(procmem);
        exit_critical();

        return STD_RET_OK;
}

//==============================================================================
/**
 * @brief Function open selected file
 *
 * @param[in]  *fshdl           FS handle
 * @param[out] *fd              file descriptor
 * @param[out] *seek            file position
 * @param[in]  *path            file path
 * @param[in]  *mode            file mode
 *
 * @retval STD_RET_OK           file opened/created
 * @retval STD_RET_ERROR        file not opened/created
 */
//==============================================================================
stdret_t procfs_open(void *fshdl, fd_t *fd, size_t *seek, const char *path, const char *mode)
{
        struct procfs    *procmem = fshdl;
        struct taskstat  taskdata;
        struct file_info *fileInf;

        if (!path || !mode || !procmem) {
                return STD_RET_ERROR;
        }

        if (strncmp(mode, "r", 2) != 0) {
                return STD_RET_ERROR;
        }

        if (strncmp(path, "/"DIR_TASKID_STR"/", strlen(DIR_TASKID_STR) + 2) == 0) {
                path = strchr(path + 1, '/') + 1;

                if (path == NULL) {
                        return STD_RET_ERROR;
                }

                task_t *taskHdl = NULL;
                path = atoi((char*)path, 16, (i32_t*)&taskHdl);

                if (tskm_get_task_stat(taskHdl, &taskdata) != STD_RET_OK) {
                        return STD_RET_ERROR;
                }

                if ((path = strrchr(path, '/')) == NULL) {
                        return STD_RET_ERROR;
                } else {
                        path++;
                }

                fileInf = kcalloc(1, sizeof(struct file_info));
                if (fileInf == NULL) {
                        return STD_RET_ERROR;
                }

                fileInf->taskhdl = taskHdl;

                if (strcmp((char*) path, TASK_FILE_NAME_STR) == 0) {
                        fileInf->task_file = TASK_FILE_NAME;
                } else if (strcmp((char*) path, TASK_FILE_CPULOAD_STR)
                           == 0) {
                        fileInf->task_file = TASK_FILE_CPULOAD;
                } else if (strcmp((char*) path, TASK_FILE_FREESTACK_STR)
                           == 0) {
                        fileInf->task_file = TASK_FILE_FREESTACK;
                } else if (strcmp((char*) path, TASK_FILE_OPENFILES_STR)
                           == 0) {
                        fileInf->task_file = TASK_FILE_OPENFILES;
                } else if (strcmp((char*) path, TASK_FILE_PRIO_STR)
                           == 0) {
                        fileInf->task_file = TASK_FILE_PRIO;
                } else if (strcmp((char*) path, TASK_FILE_USEDMEM_STR)
                           == 0) {
                        fileInf->task_file = TASK_FILE_USEDMEM;
                } else {
                        kfree(fileInf);
                        return STD_RET_ERROR;
                }

                while (lock_mutex(procmem->resource_mtx, MTX_BLOCK_TIME) != MUTEX_LOCKED);

                if (list_add_item(procmem->file_list, procmem->ID_counter, fileInf) == 0) {
                        *fd   = procmem->ID_counter++;
                        *seek = 0;

                        unlock_mutex(procmem->resource_mtx);
                        return STD_RET_OK;
                }

                unlock_mutex(procmem->resource_mtx);
                kfree(fileInf);
                return STD_RET_ERROR;

        } else if (strncmp(path, "/"DIR_TASKNAME_STR"/",
                           strlen(DIR_TASKNAME_STR) + 2) == 0) {

                path = strrchr(path, '/');

                if (path == NULL) {
                        return STD_RET_ERROR;
                } else {
                        path++;
                }

                u16_t n = tskm_get_number_of_monitored_tasks();
                u16_t i = 0;

                while (n-- && tskm_get_ntask_stat(i++, &taskdata) == STD_RET_OK) {
                        if (strcmp(path, taskdata.task_name) != 0) {
                                continue;
                        }

                        fileInf = kcalloc(1, sizeof(struct file_info));
                        if (fileInf == NULL) {
                                return STD_RET_ERROR;
                        }

                        fileInf->taskhdl  = taskdata.task_handle;
                        fileInf->task_file = TASK_FILE_NONE;

                        while (lock_mutex(procmem->resource_mtx, MTX_BLOCK_TIME) != MUTEX_LOCKED);

                        if (list_add_item(procmem->file_list,
                                        procmem->ID_counter, fileInf) == 0) {

                                *fd = procmem->ID_counter++;
                                *seek = 0;

                                unlock_mutex(procmem->resource_mtx);
                                return STD_RET_OK;
                        }

                        unlock_mutex(procmem->resource_mtx);
                        kfree(fileInf);
                        return STD_RET_ERROR;
                }
        }

        return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief Function close file in LFS
 *
 * @param[in] *fshdl            FS handle
 * @param[in] fd                file descriptor
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
stdret_t procfs_close(void *fshdl, fd_t fd)
{
        struct procfs *procmem = fshdl;

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
 * @param[in] *fshdl            FS handle
 * @param[in]  fd               file descriptor
 * @param[in] *src              data source
 * @param[in]  size             item size
 * @param[in]  nitems           number of items
 * @param[in]  seek             position in file
 *
 * @return number of written items
 */
//==============================================================================
size_t procfs_write(void *fshdl, fd_t fd, void *src, size_t size, size_t nitems, size_t seek)
{
        (void)fshdl;
        (void)fd;
        (void)src;
        (void)size;
        (void)nitems;
        (void)seek;

        return 0;
}

//==============================================================================
/**
 * @brief Function read from file data
 *
 * @param[in]  *fshdl           FS handle
 * @param[in]   fd              file descriptor
 * @param[out] *dst             data destination
 * @param[in]   size            item size
 * @param[in]   nitems          number of items
 * @param[in]   seek            position in file
 *
 * @return number of read items
 */
//==============================================================================
size_t procfs_read(void *fshdl, fd_t fd, void *dst, size_t size, size_t nitems, size_t seek)
{
        struct procfs    *procmem = fshdl;
        struct file_info *fileInf;
        struct taskstat  taskInfo;
        size_t           n = 0;
        u32_t            total_cpu_usage;

        if (!dst || !procmem) {
                return 0;
        }

        while (lock_mutex(procmem->resource_mtx, MTX_BLOCK_TIME) != MUTEX_LOCKED);
        fileInf = list_get_iditem_data(procmem->file_list, fd);
        unlock_mutex(procmem->resource_mtx);

        if (fileInf == NULL) {
                return 0;
        }

        if (fileInf->task_file >= TASK_FILE_COUNT) {
                return 0;
        }

        taskInfo.cpu_usage    = 1000;
        taskInfo.free_stack   = 0;
        taskInfo.task_handle  = 0;
        taskInfo.memory_usage = 0;
        taskInfo.task_name    = NULL;
        taskInfo.opened_files = 0;
        taskInfo.priority     = 0;

        char  data[12] = {0};
        char *dataPtr  = data;
        u8_t  dataSize = 0;

        /* DNLFIXME here is bug, cpuUsage always is 0 (why?) */
        if (tskm_get_task_stat(fileInf->taskhdl, &taskInfo) != STD_RET_OK) {
                return 0;
        }

        switch (fileInf->task_file) {
        case TASK_FILE_CPULOAD:
                total_cpu_usage = tskm_get_total_CPU_usage();
                dataSize = snprintf(data, ARRAY_SIZE(data), "%u.%u",
                                    ( taskInfo.cpu_usage *  100) / total_cpu_usage,
                                    ((taskInfo.cpu_usage * 1000) / total_cpu_usage) % 10);
                cpuctl_clear_CPU_total_time();
                break;

        case TASK_FILE_FREESTACK:
                dataSize = snprintf(data, ARRAY_SIZE(data), "%u",
                                    taskInfo.free_stack);
                break;

        case TASK_FILE_NAME:
                dataSize = strlen(taskInfo.task_name);
                dataPtr  = taskInfo.task_name;
                break;

        case TASK_FILE_OPENFILES:
                dataSize = snprintf(data, ARRAY_SIZE(data), "%u",
                                    taskInfo.opened_files);
                break;

        case TASK_FILE_PRIO:
                dataSize = snprintf(data, ARRAY_SIZE(data), "%d",
                                    taskInfo.priority);
                break;

        case TASK_FILE_USEDMEM:
                dataSize = snprintf(data, ARRAY_SIZE(data), "%u",
                                    taskInfo.memory_usage);
                break;
        }

        if (seek > dataSize) {
                n = 0;
        } else {
                if (dataSize - seek < size * nitems) {
                        n = dataSize - seek;
                        strncpy(dst, dataPtr + seek, n);
                } else {
                        n = size * nitems;
                        strncpy(dst, dataPtr + seek, n);
                }
        }

        return n;
}

//==============================================================================
/**
 * @brief IO operations on files
 *
 * @param[in]     *fshdl        FS handle
 * @param[in]      fd           file descriptor
 * @param[in]      iorq         request
 * @param[in,out] *data         data pointer
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
stdret_t procfs_ioctl(void *fshdl, fd_t fd, iorq_t iorq, void *data)
{
        (void)fshdl;
        (void)fd;
        (void)iorq;
        (void)data;

        return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief Function returns file status
 *
 * @param[in]  *fshdl                FS handle
 * @param[in]  fd                    file descriptor
 * @param[out] *stat                 pointer to status structure
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
stdret_t procfs_fstat(void *fshdl, fd_t fd, struct vfs_statf *stat)
{
        struct procfs    *procmem = fshdl;
        struct file_info *fileInf;
        struct taskstat  taskInfo;
        u32_t            total_cpu_usage;

        if (!stat || !procmem) {
                return STD_RET_ERROR;
        }

        while (lock_mutex(procmem->resource_mtx, MTX_BLOCK_TIME) != MUTEX_LOCKED);
        fileInf = list_get_iditem_data(procmem->file_list, fd);
        unlock_mutex(procmem->resource_mtx);

        if (fileInf == NULL) {
                return STD_RET_ERROR;
        }

        if (fileInf->task_file >= TASK_FILE_COUNT) {
                return STD_RET_ERROR;
        }

        if (tskm_get_task_stat(fileInf->taskhdl, &taskInfo) != STD_RET_OK) {
                return STD_RET_ERROR;
        }

        stat->st_dev   = 0;
        stat->st_mode  = 0444;
        stat->st_mtime = 0;
        stat->st_size  = 0;
        stat->st_gid   = 0;
        stat->st_uid   = 0;

        char data[12] = {0};

        switch (fileInf->task_file) {
        case TASK_FILE_CPULOAD:
                total_cpu_usage = tskm_get_total_CPU_usage();
                stat->st_size = snprintf(data, sizeof(data), "%u.%u",
                                         ( taskInfo.cpu_usage *  100) / total_cpu_usage,
                                         ((taskInfo.cpu_usage * 1000) / total_cpu_usage) % 10);
                break;

        case TASK_FILE_FREESTACK:
                stat->st_size = snprintf(data, sizeof(data), "%u",
                                         taskInfo.free_stack);
                break;

        case TASK_FILE_NAME:
                stat->st_size = strlen(taskInfo.task_name);
                break;

        case TASK_FILE_OPENFILES:
                stat->st_size = snprintf(data, sizeof(data), "%u",
                                         taskInfo.opened_files);
                break;

        case TASK_FILE_PRIO:
                stat->st_size = snprintf(data, sizeof(data), "%d",
                                         taskInfo.priority);
                break;

        case TASK_FILE_USEDMEM:
                stat->st_size = snprintf(data, sizeof(data), "%u",
                                         taskInfo.memory_usage);
                break;
        }

        return STD_RET_OK;
}

//==============================================================================
/**
 * @brief Create directory
 *
 * @param[in] *fshdl            FS handle
 * @param[in] *path             path to new directory
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
stdret_t procfs_mkdir(void *fshdl, const char *path)
{
        (void)fshdl;
        (void)path;

        return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief Function create node for driver file
 *
 * @param[in] *fshdl            FS handle
 * @param[in] *path             path when driver-file shall be created
 * @param[in] *drv_if           pointer to driver interface
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
stdret_t procfs_mknod(void *fshdl, const char *path, struct vfs_drv_interface *drv_if)
{
        (void)fshdl;
        (void)path;
        (void)drv_if;

        return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief Function open directory
 *
 * @param[in]  *fshdl           FS handle
 * @param[in]  *path            directory path
 * @param[out] *dir             directory info
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
stdret_t procfs_opendir(void *fshdl, const char *path, dir_t *dir)
{
        (void)fshdl;

        if (!path || !dir) {
                return STD_RET_ERROR;
        }

        dir->seek = 0;

        if (strcmp(path, "/") == 0) {
                dir->dd    = NULL;
                dir->items = 2;
                dir->rddir = procfs_readdir_root;
                dir->cldir = procfs_closedir_noop;
                return STD_RET_OK;
        } else if (strcmp(path, "/"DIR_TASKNAME_STR"/") == 0) {
                dir->dd    = NULL;
                dir->items = tskm_get_number_of_monitored_tasks();
                dir->rddir = procfs_readdir_taskname;
                dir->cldir = procfs_closedir_noop;
                return STD_RET_OK;
        } else if (strcmp(path, "/"DIR_TASKID_STR"/") == 0) {
                dir->dd    = kcalloc(TASK_ID_STR_LEN, sizeof(char));
                dir->items = tskm_get_number_of_monitored_tasks();
                dir->rddir = procfs_readdir_taskid;
                dir->cldir = procfs_closedir_freedd;
                return STD_RET_OK;
        } else if (strncmp(path, "/"DIR_TASKID_STR"/",
                           strlen(DIR_TASKID_STR) + 2) == 0) {

                path = strchr(path + 1, '/') + 1;

                if (path == NULL) {
                        return STD_RET_ERROR;
                }

                task_t *taskHdl = NULL;
                path = atoi((char*)path, 16, (i32_t*)&taskHdl);

                if (!((*path == '/' && *(path + 1) == '\0') || *path == '\0')) {
                        return STD_RET_ERROR;
                }

                struct taskstat taskdata;

                if (tskm_get_task_stat(taskHdl, &taskdata) == STD_RET_OK) {
                        dir->dd    = (void*)taskHdl;
                        dir->items = TASK_FILE_COUNT;
                        dir->rddir = procfs_readdir_taskid_n;
                        dir->cldir = procfs_closedir_noop;
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
 * @param[in]  *fshdl        file system data
 * @param[out] *dir          directory object
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
static stdret_t procfs_closedir_freedd(void *fshdl, dir_t *dir)
{
        (void)fshdl;

        if (dir) {
                if (dir->dd) {
                        kfree(dir->dd);
                        dir->dd = NULL;
                        return STD_RET_OK;
                }
        }

        return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief Function close opened dir (is used when dd contains data which cannot
 *        be freed)
 *
 * @param[in]  *fshdl        file system data
 * @param[out] *dir          directory object
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
static stdret_t procfs_closedir_noop(void *fshdl, dir_t *dir)
{
        (void)fshdl;
        (void)dir;

        return STD_RET_OK;
}

//==============================================================================
/**
 * @brief Remove file
 *
 * @param[in] *fshdl            FS handle
 * @param[in] *patch            localization of file/directory
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
stdret_t procfs_remove(void *fshdl, const char *path)
{
        (void)fshdl;
        (void)path;

        return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief Rename file name
 *
 * @param[in] *fshdl                FS handle
 * @param[in] *oldName              old file name
 * @param[in] *newName              new file name
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
stdret_t procfs_rename(void *fshdl, const char *old_name, const char *new_name)
{
        (void)fshdl;
        (void)old_name;
        (void)new_name;

        return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief Function change file mode
 *
 * @param[in] *fshdl                FS handle
 * @param[in] *path                 path
 * @param[in]  mode                 file mode
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
stdret_t procfs_chmod(void *fshdl, const char *path, u32_t mode)
{
        (void)fshdl;
        (void)path;
        (void)mode;

        return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief Function change file owner and group
 *
 * @param[in] *fshdl                FS handle
 * @param[in] *path                 path
 * @param[in]  owner                file owner
 * @param[in]  group                file group
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
stdret_t procfs_chown(void *fshdl, const char *path, u16_t owner, u16_t group)
{
        (void)fshdl;
        (void)path;
        (void)owner;
        (void)group;

        return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief Function returns file/dir status
 *
 * @param[in]  *fshdl                FS handle
 * @param[in]  *path                 file/dir path
 * @param[out] *stat                 pointer to stat structure
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
stdret_t procfs_stat(void *fshdl, const char *path, struct vfs_statf *stat)
{
      (void)fshdl;

      if (path && stat) {
            stat->st_dev   = 0;
            stat->st_gid   = 0;
            stat->st_mode  = 0444;
            stat->st_mtime = 0;
            stat->st_size  = 0;
            stat->st_uid   = 0;

            return STD_RET_OK;
      }

      return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief Function returns FS status
 *
 * @param[in]  *fshdl               FS handle
 * @param[out] *statfs              pointer to status structure
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
stdret_t procfs_statfs(void *fshdl, struct vfs_statfs *statfs)
{
        (void)fshdl;

        if (statfs) {
                statfs->f_bfree  = 0;
                statfs->f_blocks = 0;
                statfs->f_ffree  = 0;
                statfs->f_files  = 0;
                statfs->f_type   = 1;
                statfs->fsname   = "procfs";

                return STD_RET_OK;
        }

        return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief Read item from opened directory
 *
 * @param[in]  *fshdl        file system data
 * @param[out] *dir          directory object
 *
 * @return directory entry
 */
//==============================================================================
static dirent_t procfs_readdir_root(void *fshdl, dir_t *dir)
{
        (void)fshdl;

        dirent_t dirent;
        dirent.name = NULL;
        dirent.size = 0;

        if (dir) {
                dirent.filetype = FILE_TYPE_DIR;
                dirent.size = 0;

                switch (dir->seek++) {
                case 0:
                        dirent.name = DIR_TASKID_STR;
                        break;
                case 1:
                        dirent.name = DIR_TASKNAME_STR;
                        break;
                default:
                        break;
                }
        }

        return dirent;
}

//==============================================================================
/**
 * @brief Read item from opened directory
 *
 * @param[in]  *fshdl        file system data
 * @param[out] *dir          directory object
 *
 * @return directory entry
 */
//==============================================================================
static dirent_t procfs_readdir_taskname(void *fshdl, dir_t *dir)
{
        (void)fshdl;

        dirent_t dirent;
        dirent.name = NULL;
        dirent.size = 0;

        struct taskstat taskdata;

        if (dir) {
                if (tskm_get_ntask_stat(dir->seek, &taskdata) == STD_RET_OK) {
                        dirent.filetype = FILE_TYPE_REGULAR;
                        dirent.name     = taskdata.task_name;
                        dirent.size     = 0;

                        dir->seek++;
                }
        }

        return dirent;
}

//==============================================================================
/**
 * @brief Read item from opened directory
 *
 * @param[in]  *fshdl        file system data
 * @param[out] *dir          directory object
 *
 * @return directory entry
 */
//==============================================================================
static dirent_t procfs_readdir_taskid(void *fshdl, dir_t *dir)
{
        (void)fshdl;

        dirent_t dirent;
        dirent.name = NULL;
        dirent.size = 0;

        struct taskstat taskdata;

        if (dir == NULL) {
                return dirent;
        }

        if (dir->dd && dir->seek < (size_t)tskm_get_number_of_monitored_tasks()) {
                if (tskm_get_ntask_stat(dir->seek, &taskdata) == STD_RET_OK) {
                        snprintf(dir->dd, TASK_ID_STR_LEN,
                                 "%x", (int)taskdata.task_handle);

                        dirent.filetype = FILE_TYPE_DIR;
                        dirent.name     = dir->dd;
                        dirent.size     = 0;

                        dir->seek++;
                }
        }

        return dirent;
}

//==============================================================================
/**
 * @brief Read item from opened directory
 *
 * @param[in]  *fshdl        file system data
 * @param[out] *dir          directory object
 *
 * @return directory entry
 */
//==============================================================================
static dirent_t procfs_readdir_taskid_n(void *fshdl, dir_t *dir)
{
        (void)fshdl;

        struct taskstat taskdata;
        u32_t total_cpu_usage;

        dirent_t dirent;
        dirent.name = NULL;
        dirent.size = 0;

        if (dir == NULL) {
                return dirent;
        }

        if (dir->seek >= TASK_FILE_COUNT) {
                return dirent;
        }

        if (tskm_get_task_stat((task_t*)dir->dd, &taskdata) != STD_RET_OK) {
                return dirent;
        }

        char data[12]   = {0};
        dirent.filetype = FILE_TYPE_REGULAR;

        switch (dir->seek) {
        case TASK_FILE_NAME:
                dirent.name = TASK_FILE_NAME_STR;
                dirent.size = strlen(taskdata.task_name);
                break;

        case TASK_FILE_PRIO:
                dirent.name = TASK_FILE_PRIO_STR;
                dirent.size = snprintf(data, ARRAY_SIZE(data), "%d",
                                       taskdata.priority);
                break;

        case TASK_FILE_FREESTACK:
                dirent.name = TASK_FILE_FREESTACK_STR;
                dirent.size = snprintf(data, ARRAY_SIZE(data), "%u",
                                       taskdata.free_stack);
                break;

        case TASK_FILE_USEDMEM:
                dirent.name = TASK_FILE_USEDMEM_STR;
                dirent.size = snprintf(data, ARRAY_SIZE(data), "%d",
                                       taskdata.memory_usage);
                break;

        case TASK_FILE_OPENFILES:
                dirent.name = TASK_FILE_OPENFILES_STR;
                dirent.size = snprintf(data, ARRAY_SIZE(data), "%d",
                                       taskdata.opened_files);
                break;

        case TASK_FILE_CPULOAD:
                total_cpu_usage = tskm_get_total_CPU_usage();
                dirent.name = TASK_FILE_CPULOAD_STR;
                dirent.size = snprintf(data, ARRAY_SIZE(data), "%u.%u",
                                       ( taskdata.cpu_usage *  100) / total_cpu_usage,
                                       ((taskdata.cpu_usage * 1000) / total_cpu_usage) % 10);
                break;
        }

        dir->seek++;
        return dirent;
}

#ifdef __cplusplus
}
#endif

/*==============================================================================
  End of file
==============================================================================*/
