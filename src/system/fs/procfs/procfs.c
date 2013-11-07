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
#include "system/dnxfs.h"
#include "core/printx.h"
#include "core/conv.h"
#include "core/list.h"
#include "core/progman.h"

/*==============================================================================
  Local symbolic constants/macros
==============================================================================*/
/* task ID string length (8B name + \0). ID is 32b hex number converted to string */
#define TASK_ID_STR_LEN                   9

#define DIR_TASKID_STR                    "taskid"
#define DIR_TASKNAME_STR                  "taskname"
#define DIR_BIN_STR                       "bin"
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
      mutex_t *resource_mtx;
      u32_t    ID_counter;
};

struct file_info {
      task_t *taskhdl;         /* task handle */
      u8_t    task_file;       /* task info file */
};

/*==============================================================================
  Local function prototypes
==============================================================================*/
static stdret_t procfs_closedir_freedd  (void *fs_handle, DIR *dir);
static stdret_t procfs_closedir_generic (void *fs_handle, DIR *dir);
static dirent_t procfs_readdir_root     (void *fs_handle, DIR *dir);
static dirent_t procfs_readdir_taskname (void *fs_handle, DIR *dir);
static dirent_t procfs_readdir_bin      (void *fs_handle, DIR *dir);
static dirent_t procfs_readdir_taskid   (void *fs_handle, DIR *dir);
static dirent_t procfs_readdir_taskid_n (void *fs_handle, DIR *dir);

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
 * @param[out]          **fs_handle             file system allocated memory
 * @param[in ]           *src_path              file source path
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
API_FS_INIT(procfs, void **fs_handle, const char *src_path)
{
        UNUSED_ARG(src_path);

        STOP_IF(!fs_handle);

        struct procfs *procfs    = calloc(1, sizeof(struct procfs));
        list_t        *file_list = list_new();
        mutex_t       *mtx       = mutex_new();

        if (procfs && file_list && mtx) {
                procfs->file_list    = file_list;
                procfs->resource_mtx = mtx;
                procfs->ID_counter   = 0;

                *fs_handle = procfs;
                return STD_RET_OK;
        }

        if (file_list) {
                list_delete(file_list);
        }

        if (mtx) {
                mutex_delete(mtx);
        }

        if (procfs) {
                free(procfs);
        }

        return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief Release file system
 *
 * @param[in ]          *fs_handle              file system allocated memory
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
API_FS_RELEASE(procfs, void *fs_handle)
{
        STOP_IF(!fs_handle);

        struct procfs *procfs = fs_handle;

        if (mutex_lock(procfs->resource_mtx, 100) == MUTEX_LOCKED) {
                if (list_get_item_count(procfs->file_list) != 0) {
                        mutex_unlock(procfs->resource_mtx);
                        return STD_RET_ERROR;
                }

                critical_section_begin();
                mutex_unlock(procfs->resource_mtx);

                list_delete(procfs->file_list);
                mutex_delete(procfs->resource_mtx);
                free(procfs);

                critical_section_end();
                return STD_RET_OK;
        }

        return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief Open file
 *
 * @param[in ]          *fs_handle              file system allocated memory
 * @param[out]          *extra                  file extra data
 * @param[out]          *fd                     file descriptor
 * @param[out]          *fpos                   file position
 * @param[in]           *path                   file path
 * @param[in]            flags                  file open flags (see vfs.h)
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
API_FS_OPEN(procfs, void *fs_handle, void **extra, fd_t *fd, u64_t *lseek, const char *path, int flags)
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
                path = sys_strtoi((char*)path, 16, (i32_t*)&taskHdl);

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

                while (mutex_lock(procmem->resource_mtx, MTX_BLOCK_TIME) != MUTEX_LOCKED);

                if (list_add_item(procmem->file_list, procmem->ID_counter, fileInf) == 0) {
                        *fd    = procmem->ID_counter++;
                        *lseek = 0;

                        mutex_unlock(procmem->resource_mtx);
                        return STD_RET_OK;
                }

                mutex_unlock(procmem->resource_mtx);
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

                        while (mutex_lock(procmem->resource_mtx, MTX_BLOCK_TIME) != MUTEX_LOCKED);

                        if (list_add_item(procmem->file_list,
                                        procmem->ID_counter, fileInf) == 0) {

                                *fd = procmem->ID_counter++;
                                *lseek = 0;

                                mutex_unlock(procmem->resource_mtx);
                                return STD_RET_OK;
                        }

                        mutex_unlock(procmem->resource_mtx);
                        free(fileInf);
                        return STD_RET_ERROR;
                }
        }

        return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief Close file
 *
 * @param[in ]          *fs_handle              file system allocated memory
 * @param[in ]          *extra                  file extra data
 * @param[in ]           fd                     file descriptor
 * @param[in ]           force                  force close
 * @param[in ]          *file_owner             task which opened file (valid if force is true)
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
API_FS_CLOSE(procfs, void *fs_handle, void *extra, fd_t fd, bool force, task_t *file_owner)
{
        UNUSED_ARG(extra);
        UNUSED_ARG(force);
        UNUSED_ARG(file_owner);

        STOP_IF(!fs_handle);

        struct procfs *procmem = fs_handle;
        if (procmem) {
                while (mutex_lock(procmem->resource_mtx, MTX_BLOCK_TIME) != MUTEX_LOCKED);

                if (list_rm_iditem(procmem->file_list, fd) == STD_RET_OK) {
                        mutex_unlock(procmem->resource_mtx);
                        return STD_RET_OK;
                }

                mutex_unlock(procmem->resource_mtx);
        }

        return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief Write data to the file
 *
 * @param[in ]          *fs_handle              file system allocated memory
 * @param[in ]          *extra                  file extra data
 * @param[in ]           fd                     file descriptor
 * @param[in ]          *src                    data source
 * @param[in ]           count                  number of bytes to write
 * @param[in ]          *fpos                   position in file

 * @return number of written bytes, -1 if error
 */
//==============================================================================
API_FS_WRITE(procfs, void *fs_handle,void *extra, fd_t fd, const u8_t *src, size_t count, u64_t *fpos)
{
        UNUSED_ARG(fs_handle);
        UNUSED_ARG(extra);
        UNUSED_ARG(fd);
        UNUSED_ARG(src);
        UNUSED_ARG(count);
        UNUSED_ARG(fpos);

        return 0;
}

//==============================================================================
/**
 * @brief Read data from file
 *
 * @param[in ]          *fs_handle              file system allocated memory
 * @param[in ]          *extra                  file extra data
 * @param[in ]           fd                     file descriptor
 * @param[out]          *dst                    data destination
 * @param[in ]           count                  number of bytes to read
 * @param[in ]          *fpos                   position in file

 * @return number of read bytes, -1 if error
 */
//==============================================================================
API_FS_READ(procfs, void *fs_handle, void *extra, fd_t fd, u8_t *dst, size_t count, u64_t *fpos)
{
        UNUSED_ARG(extra);

        STOP_IF(!fs_handle);
        STOP_IF(!dst);
        STOP_IF(!count);
        STOP_IF(!fpos);

        struct procfs *procmem = fs_handle;

        while (mutex_lock(procmem->resource_mtx, MTX_BLOCK_TIME) != MUTEX_LOCKED);
        struct file_info *fileInf = list_get_iditem_data(procmem->file_list, fd);
        mutex_unlock(procmem->resource_mtx);

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
                dataSize = sys_snprintf(data, dataSize, "%u\n", taskInfo.free_stack);
                break;

        case TASK_FILE_NAME:
                dataSize = sys_snprintf(data, dataSize, "%s\n", taskInfo.task_name);
                break;

        case TASK_FILE_OPENFILES:
                dataSize = sys_snprintf(data, dataSize, "%u\n", taskInfo.opened_files);
                break;

        case TASK_FILE_PRIO:
                dataSize = sys_snprintf(data, dataSize, "%d\n", taskInfo.priority);
                break;

        case TASK_FILE_USEDMEM:
                dataSize = sys_snprintf(data, dataSize, "%u\n", taskInfo.memory_usage);
                break;
        }

        size_t n;
        size_t seek = *fpos > SIZE_MAX ? SIZE_MAX : *fpos;
        if (seek > dataSize) {
                n = 0;
        } else {
                if (dataSize - seek <= count) {
                        n = dataSize - seek;
                        strncpy((char*)dst, data + seek, n);
                } else {
                        n = count;
                        strncpy((char *)dst, data + seek, n);
                }
        }

        free(data);
        return n;
}

//==============================================================================
/**
 * @brief IO operations on files
 *
 * @param[in ]          *fs_handle              file system allocated memory
 * @param[in ]          *extra                  file extra data
 * @param[in ]           fd                     file descriptor
 * @param[in ]           request                request
 * @param[in ][out]     *arg                    request's argument
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
API_FS_IOCTL(procfs, void *fs_handle, void *extra, fd_t fd, int request, void *arg)
{
        UNUSED_ARG(fs_handle);
        UNUSED_ARG(extra);
        UNUSED_ARG(fd);
        UNUSED_ARG(request);
        UNUSED_ARG(arg);

        return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief Flush file data
 *
 * @param[in ]          *fs_handle              file system allocated memory
 * @param[in ]          *extra                  file extra data
 * @param[in ]           fd                     file descriptor
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
API_FS_FLUSH(procfs, void *fs_handle, void *extra, fd_t fd)
{
        UNUSED_ARG(fs_handle);
        UNUSED_ARG(extra);
        UNUSED_ARG(fd);

        return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief Return file status
 *
 * @param[in ]          *fs_handle              file system allocated memory
 * @param[in ]          *extra                  file extra data
 * @param[in ]           fd                     file descriptor
 * @param[out]          *stat                   file status
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
API_FS_FSTAT(procfs, void *fs_handle, void *extra, fd_t fd, struct vfs_stat *stat)
{
        UNUSED_ARG(extra);

        STOP_IF(!fs_handle);
        STOP_IF(!stat);

        struct procfs *procmem = fs_handle;

        while (mutex_lock(procmem->resource_mtx, MTX_BLOCK_TIME) != MUTEX_LOCKED);
        struct file_info *fileInf = list_get_iditem_data(procmem->file_list, fd);
        mutex_unlock(procmem->resource_mtx);

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
        stat->st_mode  = S_IRUSR | S_IRGRO | S_IROTH;
        stat->st_mtime = 0;
        stat->st_size  = 0;
        stat->st_gid   = 0;
        stat->st_uid   = 0;

        char data[12] = {0};

        switch (fileInf->task_file) {
        case TASK_FILE_FREESTACK:
                stat->st_size = sys_snprintf(data, sizeof(data), "%u\n", taskInfo.free_stack);
                break;

        case TASK_FILE_NAME:
                stat->st_size = strlen(taskInfo.task_name) + 1;
                break;

        case TASK_FILE_OPENFILES:
                stat->st_size = sys_snprintf(data, sizeof(data), "%u\n", taskInfo.opened_files);
                break;

        case TASK_FILE_PRIO:
                stat->st_size = sys_snprintf(data, sizeof(data), "%d\n", taskInfo.priority);
                break;

        case TASK_FILE_USEDMEM:
                stat->st_size = sys_snprintf(data, sizeof(data), "%u\n", taskInfo.memory_usage);
                break;
        }

        return STD_RET_OK;
}

//==============================================================================
/**
 * @brief Create directory
 *
 * @param[in ]          *fs_handle              file system allocated memory
 * @param[in ]          *path                   name of created directory
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
API_FS_MKDIR(procfs, void *fs_handle, const char *path)
{
        UNUSED_ARG(fs_handle);
        UNUSED_ARG(path);

        return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief Create node for driver file
 *
 * @param[in ]          *fs_handle              file system allocated memory
 * @param[in ]          *path                   name of created node
 * @param[in ]          *drv_if                 driver interface
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
API_FS_MKNOD(procfs, void *fs_handle, const char *path, const struct vfs_drv_interface *drv_if)
{
        UNUSED_ARG(fs_handle);
        UNUSED_ARG(path);
        UNUSED_ARG(drv_if);

        return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief Open directory
 *
 * @param[in ]          *fs_handle              file system allocated memory
 * @param[in ]          *path                   name of opened directory
 * @param[in ]          *dir                    directory object
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
API_FS_OPENDIR(procfs, void *fs_handle, const char *path, DIR *dir)
{
        UNUSED_ARG(fs_handle);

        STOP_IF(!path);
        STOP_IF(!dir);

        dir->f_seek = 0;

        if (strcmp(path, "/") == 0) {
                dir->f_dd       = NULL;
                dir->f_items    = 3;
                dir->f_readdir  = procfs_readdir_root;
                dir->f_closedir = procfs_closedir_generic;
                return STD_RET_OK;
        } else if (strcmp(path, "/"DIR_TASKNAME_STR"/") == 0) {
                dir->f_dd       = NULL;
                dir->f_items    = sysm_get_number_of_monitored_tasks();
                dir->f_readdir  = procfs_readdir_taskname;
                dir->f_closedir = procfs_closedir_generic;
                return STD_RET_OK;
        } else if (strcmp(path, "/"DIR_TASKID_STR"/") == 0) {
                dir->f_dd       = calloc(TASK_ID_STR_LEN, sizeof(char));
                dir->f_items    = sysm_get_number_of_monitored_tasks();
                dir->f_readdir  = procfs_readdir_taskid;
                dir->f_closedir = procfs_closedir_freedd;
                return STD_RET_OK;
        } else if (strcmp(path, "/"DIR_BIN_STR"/") == 0) {
                dir->f_dd       = NULL;
                dir->f_items    = _get_programs_table_size();
                dir->f_readdir  = procfs_readdir_bin;
                dir->f_closedir = procfs_closedir_generic;
                return STD_RET_OK;
        } else if (strncmp(path, "/"DIR_TASKID_STR"/", strlen(DIR_TASKID_STR) + 2) == 0) {

                path = strchr(path + 1, '/') + 1;

                if (path == NULL) {
                        return STD_RET_ERROR;
                }

                task_t *taskHdl = NULL;
                path = sys_strtoi((char*)path, 16, (i32_t*)&taskHdl);

                if (!((*path == '/' && *(path + 1) == '\0') || *path == '\0')) {
                        return STD_RET_ERROR;
                }

                struct sysmoni_taskstat taskdata;
                if (sysm_get_task_stat(taskHdl, &taskdata) == STD_RET_OK) {
                        dir->f_dd       = (void*)taskHdl;
                        dir->f_items    = TASK_FILE_COUNT;
                        dir->f_readdir  = procfs_readdir_taskid_n;
                        dir->f_closedir = procfs_closedir_generic;
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
static stdret_t procfs_closedir_freedd(void *fs_handle, DIR *dir)
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
static stdret_t procfs_closedir_generic(void *fs_handle, DIR *dir)
{
        UNUSED_ARG(fs_handle);
        UNUSED_ARG(dir);

        return STD_RET_OK;
}

//==============================================================================
/**
 * @brief Remove file/directory
 *
 * @param[in ]          *fs_handle              file system allocated memory
 * @param[in ]          *path                   name of removed file/directory
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
API_FS_REMOVE(procfs, void *fs_handle, const char *path)
{
        UNUSED_ARG(fs_handle);
        UNUSED_ARG(path);

        return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief Rename file/directory
 *
 * @param[in ]          *fs_handle              file system allocated memory
 * @param[in ]          *old_name               old object name
 * @param[in ]          *new_name               new object name
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
API_FS_RENAME(procfs, void *fs_handle, const char *old_name, const char *new_name)
{
        UNUSED_ARG(fs_handle);
        UNUSED_ARG(old_name);
        UNUSED_ARG(new_name);

        return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief Change file's mode
 *
 * @param[in ]          *fs_handle              file system allocated memory
 * @param[in ]          *path                   file path
 * @param[in ]           mode                   new file mode
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
API_FS_CHMOD(procfs, void *fs_handle, const char *path, int mode)
{
        UNUSED_ARG(fs_handle);
        UNUSED_ARG(path);
        UNUSED_ARG(mode);

        return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief Change file's owner and group
 *
 * @param[in ]          *fs_handle              file system allocated memory
 * @param[in ]          *path                   file path
 * @param[in ]           owner                  new file owner
 * @param[in ]           group                  new file group
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
API_FS_CHOWN(procfs, void *fs_handle, const char *path, int owner, int group)
{
        UNUSED_ARG(fs_handle);
        UNUSED_ARG(path);
        UNUSED_ARG(owner);
        UNUSED_ARG(group);

        return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief Return file/dir status
 *
 * @param[in ]          *fs_handle              file system allocated memory
 * @param[in ]          *path                   file path
 * @param[out]          *stat                   file status
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
API_FS_STAT(procfs, void *fs_handle, const char *path, struct vfs_stat *stat)
{
        UNUSED_ARG(fs_handle);
        UNUSED_ARG(path);

        STOP_IF(!stat);

        stat->st_dev   = 0;
        stat->st_gid   = 0;
        stat->st_mode  = S_IRUSR | S_IRGRO | S_IROTH;
        stat->st_mtime = 0;
        stat->st_size  = 0;
        stat->st_uid   = 0;

        return STD_RET_OK;
}

//==============================================================================
/**
 * @brief Return file system status
 *
 * @param[in ]          *fs_handle              file system allocated memory
 * @param[out]          *statfs                 file system status
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
API_FS_STATFS(procfs, void *fs_handle, struct vfs_statfs *statfs)
{
        UNUSED_ARG(fs_handle);

        STOP_IF(!statfs);

        statfs->f_bfree  = 0;
        statfs->f_blocks = 0;
        statfs->f_ffree  = 0;
        statfs->f_files  = 0;
        statfs->f_type   = 1;
        statfs->f_fsname = "procfs";

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
static dirent_t procfs_readdir_root(void *fs_handle, DIR *dir)
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
        case 2: dirent.name = DIR_BIN_STR;
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
static dirent_t procfs_readdir_taskname(void *fs_handle, DIR *dir)
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
static dirent_t procfs_readdir_bin(void *fs_handle, DIR *dir)
{
        UNUSED_ARG(fs_handle);

        STOP_IF(!dir);

        dirent_t dirent;
        dirent.name = NULL;
        dirent.size = 0;

        if (dir->f_seek < (size_t)_get_programs_table_size()) {
                dirent.filetype = FILE_TYPE_PROGRAM;
                dirent.name     = _get_programs_table()[dir->f_seek].program_name;
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
static dirent_t procfs_readdir_taskid(void *fs_handle, DIR *dir)
{
        UNUSED_ARG(fs_handle);

        STOP_IF(!dir);

        dirent_t dirent;
        dirent.name = NULL;
        dirent.size = 0;

        if (dir->f_dd && dir->f_seek < (size_t)sysm_get_number_of_monitored_tasks()) {
                struct sysmoni_taskstat taskdata;
                if (sysm_get_ntask_stat(dir->f_seek, &taskdata) == STD_RET_OK) {
                        sys_snprintf(dir->f_dd, TASK_ID_STR_LEN, "%x", (int)taskdata.task_handle);

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
static dirent_t procfs_readdir_taskid_n(void *fs_handle, DIR *dir)
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
                dirent.size = sys_snprintf(data, ARRAY_SIZE(data), "%d\n", taskdata.priority);
                break;

        case TASK_FILE_FREESTACK:
                dirent.name = TASK_FILE_FREESTACK_STR;
                dirent.size = sys_snprintf(data, ARRAY_SIZE(data), "%u\n", taskdata.free_stack);
                break;

        case TASK_FILE_USEDMEM:
                dirent.name = TASK_FILE_USEDMEM_STR;
                dirent.size = sys_snprintf(data, ARRAY_SIZE(data), "%d\n", taskdata.memory_usage);
                break;

        case TASK_FILE_OPENFILES:
                dirent.name = TASK_FILE_OPENFILES_STR;
                dirent.size = sys_snprintf(data, ARRAY_SIZE(data), "%d\n", taskdata.opened_files);
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
