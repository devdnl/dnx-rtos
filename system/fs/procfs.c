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
      COUNT_OF_TASK_FILE,
      TASK_FILE_NONE
};

struct procmem {
      list_t  *flist;
      u32_t    idcnt;
      mutex_t *mtx;
};

struct fileinfo {
      task_t *taskHdl;         /* task handle */
      u8_t    taskFile;        /* task info file */
};

/*==============================================================================
  Local function prototypes
==============================================================================*/
static stdret_t procfs_closedir_freedd(fsd_t fsd, dir_t *dir);
static stdret_t procfs_closedir_noop(fsd_t fsd, dir_t *dir);
static dirent_t procfs_readdir_root(fsd_t fsd, dir_t *dir);
static dirent_t procfs_readdir_taskname(fsd_t fsd, dir_t *dir);
static dirent_t procfs_readdir_taskid(fsd_t fsd, dir_t *dir);
static dirent_t procfs_readdir_taskid_n(fsd_t fsd, dir_t *dir);

/*==============================================================================
  Local object definitions
==============================================================================*/
static struct procmem *procmem;

/*==============================================================================
  Exported object definitions
==============================================================================*/

/*==============================================================================
  Function definitions
==============================================================================*/

//==============================================================================
/**
 * @brief Function initialize FS
 *
 * @param[in]  *srcPath         source path
 * @param[out] *fsd             file system descriptor
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
stdret_t procfs_init(const char *srcPath, fsd_t *fsd)
{
        (void) fsd;
        (void) srcPath;

        if (procmem) {
                return STD_RET_OK;
        }

        if ((procmem = calloc(1, sizeof(struct procmem))) != NULL) {
                procmem->flist = new_list();
                procmem->mtx   = new_mutex();

                if (!procmem->flist || !procmem->mtx) {
                        if (procmem->flist) {
                                delete_list(procmem->flist);
                        }

                        if (procmem->mtx) {
                                delete_mutex(procmem->mtx);
                        }

                        free(procmem);
                        procmem = NULL;
                        return STD_RET_ERROR;
                }

                return STD_RET_OK;
        }

        return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief Release file system
 *
 * @param[in] fsd           file system descriptor
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
stdret_t procfs_release(fsd_t fsd)
{
      (void)fsd;

      if (procmem) {
            while (lock_mutex(procmem->mtx, MTX_BLOCK_TIME) != MUTEX_LOCKED);
            enter_critical();
            unlock_mutex(procmem->mtx);
            delete_mutex(procmem->mtx);
            delete_list(procmem->flist);
            free(procmem);
            procmem = NULL;
            exit_critical();

            return STD_RET_OK;
      }

      return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief Function open selected file
 *
 * @param[in]   fsd          file system descriptor
 * @param[out] *fd           file descriptor
 * @param[out] *seek         file position
 * @param[in]  *path         file name
 * @param[in]  *mode         file mode
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
stdret_t procfs_open(fsd_t fsd, fd_t *fd, size_t *seek, const char *path, const char *mode)
{
        (void) fsd;

        struct taskstat  taskdata;
        struct fileinfo *fileInf;

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

                fileInf = calloc(1, sizeof(struct fileinfo));
                if (fileInf == NULL) {
                        return STD_RET_ERROR;
                }

                fileInf->taskHdl = taskHdl;

                if (strcmp((char*) path, TASK_FILE_NAME_STR) == 0) {
                        fileInf->taskFile = TASK_FILE_NAME;
                } else if (strcmp((char*) path, TASK_FILE_CPULOAD_STR)
                           == 0) {
                        fileInf->taskFile = TASK_FILE_CPULOAD;
                } else if (strcmp((char*) path, TASK_FILE_FREESTACK_STR)
                           == 0) {
                        fileInf->taskFile = TASK_FILE_FREESTACK;
                } else if (strcmp((char*) path, TASK_FILE_OPENFILES_STR)
                           == 0) {
                        fileInf->taskFile = TASK_FILE_OPENFILES;
                } else if (strcmp((char*) path, TASK_FILE_PRIO_STR)
                           == 0) {
                        fileInf->taskFile = TASK_FILE_PRIO;
                } else if (strcmp((char*) path, TASK_FILE_USEDMEM_STR)
                           == 0) {
                        fileInf->taskFile = TASK_FILE_USEDMEM;
                } else {
                        free(fileInf);
                        return STD_RET_ERROR;
                }

                while (lock_mutex(procmem->mtx, MTX_BLOCK_TIME) != MUTEX_LOCKED);

                if (list_add_item(procmem->flist, procmem->idcnt, fileInf) == 0) {
                        *fd   = procmem->idcnt++;
                        *seek = 0;

                        unlock_mutex(procmem->mtx);
                        return STD_RET_OK;
                }

                unlock_mutex(procmem->mtx);
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

                u16_t n = tskm_get_number_of_monitored_tasks();
                u16_t i = 0;

                while (n-- && tskm_get_ntask_stat(i++, &taskdata) == STD_RET_OK) {
                        if (strcmp(path, taskdata.task_name) != 0) {
                                continue;
                        }

                        fileInf = calloc(1, sizeof(struct fileinfo));
                        if (fileInf == NULL) {
                                return STD_RET_ERROR;
                        }

                        fileInf->taskHdl  = taskdata.task_handle;
                        fileInf->taskFile = TASK_FILE_NONE;

                        while (lock_mutex(procmem->mtx, MTX_BLOCK_TIME) != MUTEX_LOCKED);

                        if (list_add_item(procmem->flist,
                                        procmem->idcnt, fileInf) == 0) {

                                *fd = procmem->idcnt++;
                                *seek = 0;

                                unlock_mutex(procmem->mtx);
                                return STD_RET_OK;
                        }

                        unlock_mutex(procmem->mtx);
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
 * @param[in]  fsd          file system descriptor
 * @param[in] *fd           file descriptor
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
stdret_t procfs_close(fsd_t fsd, fd_t fd)
{
        (void) fsd;

        if (procmem) {
                while (lock_mutex(procmem->mtx, MTX_BLOCK_TIME) != MUTEX_LOCKED)
                        ;

                if (list_rm_iditem(procmem->flist, fd) == STD_RET_OK) {
                        unlock_mutex(procmem->mtx);
                        return STD_RET_OK;
                }

                unlock_mutex(procmem->mtx);
        }

        return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief Write data file
 *
 * @param[in]  fsd          file system descriptor
 * @param[in] *fd           file descriptor
 * @param[in] *src          data source
 * @param[in]  size         item size
 * @param[in]  nitems       item count
 * @param[in]  seek         file position
 *
 * @return written nitems
 */
//==============================================================================
size_t procfs_write(fsd_t fsd, fd_t fd, void *src, size_t size, size_t nitems, size_t seek)
{
        (void) fsd;
        (void) fd;
        (void) src;
        (void) size;
        (void) nitems;
        (void) seek;

        return 0;
}

//==============================================================================
/**
 * @brief Read data files
 *
 * @param[in]   fsd          file system descriptor
 * @param[in]  *fd           file descriptor
 * @param[out] *dst          data destination
 * @param[in]   size         item size
 * @param[in]   nitems       item count
 * @param[in]   seek         file position
 *
 * @retval read nitems
 */
//==============================================================================
size_t procfs_read(fsd_t fsd, fd_t fd, void *dst, size_t size, size_t nitems, size_t seek)
{
        (void) fsd;

        struct fileinfo *fileInf;
        struct taskstat  taskInfo;
        size_t           n = 0;
        u32_t            total_cpu_usage;

        if (!dst || !procmem) {
                return 0;
        }

        while (lock_mutex(procmem->mtx, MTX_BLOCK_TIME) != MUTEX_LOCKED);
        fileInf = list_get_iditem_data(procmem->flist, fd);
        unlock_mutex(procmem->mtx);

        if (fileInf == NULL) {
                return 0;
        }

        if (fileInf->taskFile >= COUNT_OF_TASK_FILE) {
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
        if (tskm_get_task_stat(fileInf->taskHdl, &taskInfo) != STD_RET_OK) {
                return 0;
        }

        switch (fileInf->taskFile) {
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
 * @brief Control file
 *
 * @param[in]      fsd          file system descriptor
 * @param[in]      fd           file descriptor
 * @param[in]      iorq         request
 * @param[in,out] *data         data
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
stdret_t procfs_ioctl(fsd_t fsd, fd_t fd, iorq_t iorq, void *data)
{
        (void) fsd;
        (void) fd;
        (void) iorq;
        (void) data;

        return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief Statistics of opened file
 *
 * @param[in]   fsd          file system descriptor
 * @param[in]  *fd           file descriptor
 * @param[out] *stat         output statistics
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
stdret_t procfs_fstat(fsd_t fsd, fd_t fd, struct vfs_statf *stat)
{
        (void) fsd;

        struct fileinfo *fileInf;
        struct taskstat  taskInfo;
        u32_t total_cpu_usage;

        if (!stat || !procmem) {
                return STD_RET_ERROR;
        }

        while (lock_mutex(procmem->mtx, MTX_BLOCK_TIME) != MUTEX_LOCKED);
        fileInf = list_get_iditem_data(procmem->flist, fd);
        unlock_mutex(procmem->mtx);

        if (fileInf == NULL) {
                return STD_RET_ERROR;
        }

        if (fileInf->taskFile >= COUNT_OF_TASK_FILE) {
                return STD_RET_ERROR;
        }

        if (tskm_get_task_stat(fileInf->taskHdl, &taskInfo) != STD_RET_OK) {
                return STD_RET_ERROR;
        }

        stat->st_dev   = 0;
        stat->st_mode  = 0444;
        stat->st_mtime = 0;
        stat->st_rdev  = 0;
        stat->st_size  = 0;
        stat->st_gid   = 0;
        stat->st_uid   = 0;

        char data[12] = {0};

        switch (fileInf->taskFile) {
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
 * @param[in]  fsd          file system descriptor
 * @param[in] *path         directory path
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
stdret_t procfs_mkdir(fsd_t fsd, const char *path)
{
        (void) fsd;
        (void) path;

        return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief Create device node
 *
 * @param[in]  fsd          file system descriptor
 * @param[in] *path         node path
 * @param[in] *dcfg         device configuration
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
stdret_t procfs_mknod(fsd_t fsd, const char *path, struct vfs_drv_interface *dcfg)
{
        (void) fsd;
        (void) path;
        (void) dcfg;

        return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief Opens directory
 *
 * @param[in]   fsd          file system descriptor
 * @param[in]  *path         directory path
 * @param[out] *dir          directory object to fill
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
stdret_t procfs_opendir(fsd_t fsd, const char *path, dir_t *dir)
{
        (void) fsd;

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
                dir->dd    = calloc(TASK_ID_STR_LEN, sizeof(char));
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
                        dir->items = COUNT_OF_TASK_FILE;
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
 * @param[in]   fsd          file system descriptor
 * @param[out] *dir          directory object
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
static stdret_t procfs_closedir_freedd(fsd_t fsd, dir_t *dir)
{
        (void) fsd;

        if (dir) {
                if (dir->dd) {
                        free(dir->dd);
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
 * @param[in]   fsd          file system descriptor
 * @param[out] *dir          directory object
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
static stdret_t procfs_closedir_noop(fsd_t fsd, dir_t *dir)
{
        (void) fsd;
        (void) dir;

        return STD_RET_OK;
}

//==============================================================================
/**
 * @brief Remove file
 *
 * @param[in]   fsd          file system descriptor
 * @param[out] *path         file path
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
stdret_t procfs_remove(fsd_t fsd, const char *path)
{
        (void) fsd;
        (void) path;

        return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief Rename file
 *
 * @param[in]  fsd          file system descriptor
 * @param[in] *oldName      old file name
 * @param[in] *newName      new file name
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
stdret_t procfs_rename(fsd_t fsd, const char *oldName, const char *newName)
{
        (void) fsd;
        (void) oldName;
        (void) newName;

        return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief Change file mode
 *
 * @param[in]  fsd          file system descriptor
 * @param[in] *path         file path
 * @param[in]  mode         new mode
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
stdret_t procfs_chmod(fsd_t fsd, const char *path, u32_t mode)
{
        (void) fsd;
        (void) path;
        (void) mode;

        return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief Change file owner and group
 *
 * @param[in]  fsd          file system descriptor
 * @param[in] *path         file path
 * @param[in]  owner        owner
 * @param[in]  group        group
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
stdret_t procfs_chown(fsd_t fsd, const char *path, u16_t owner, u16_t group)
{
        (void) fsd;
        (void) path;
        (void) owner;
        (void) group;

        return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief File statistics
 *
 * @param[in]   fsd          file system descriptor
 * @param[in]  *path         file path
 * @param[out] *stat         file statistics
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
stdret_t procfs_stat(fsd_t fsd, const char *path, struct vfs_statf *stat)
{
      (void)fsd;

      if (path && stat) {
            stat->st_dev   = 0;
            stat->st_gid   = 0;
            stat->st_mode  = 0444;
            stat->st_mtime = 0;
            stat->st_rdev  = 0;
            stat->st_size  = 0;
            stat->st_uid   = 0;

            return STD_RET_OK;
      }

      return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief File system statistics
 *
 * @param[in]   fsd          file system descriptor
 * @param[out] *statfs       FS statistics
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
stdret_t procfs_statfs(fsd_t fsd, struct vfs_statfs *statfs)
{
        (void) fsd;

        if (statfs) {
                statfs->f_bfree = 0;
                statfs->f_blocks = 0;
                statfs->f_ffree = 0;
                statfs->f_files = 0;
                statfs->f_type = 1;
                statfs->fsname = "procfs";

                return STD_RET_OK;
        }

        return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief Read item from opened directory
 *
 * @param[in]   fsd          file system descriptor
 * @param[out] *dir          directory object
 *
 * @return directory entry
 */
//==============================================================================
static dirent_t procfs_readdir_root(fsd_t fsd, dir_t *dir)
{
        (void) fsd;

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
 * @param[in]   fsd          file system descriptor
 * @param[out] *dir          directory object
 *
 * @return directory entry
 */
//==============================================================================
static dirent_t procfs_readdir_taskname(fsd_t fsd, dir_t *dir)
{
        (void) fsd;

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
 * @param[in]   fsd          file system descriptor
 * @param[out] *dir          directory object
 *
 * @return directory entry
 */
//==============================================================================
static dirent_t procfs_readdir_taskid(fsd_t fsd, dir_t *dir)
{
        (void) fsd;

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
 * @param[in]   fsd          file system descriptor
 * @param[out] *dir          directory object
 *
 * @return directory entry
 */
//==============================================================================
static dirent_t procfs_readdir_taskid_n(fsd_t fsd, dir_t *dir)
{
        (void) fsd;

        struct taskstat taskdata;
        u32_t total_cpu_usage;

        dirent_t dirent;
        dirent.name = NULL;
        dirent.size = 0;

        if (dir == NULL) {
                return dirent;
        }

        if (dir->seek >= COUNT_OF_TASK_FILE) {
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
