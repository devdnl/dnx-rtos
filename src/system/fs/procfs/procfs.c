/*=========================================================================*//**
@file    procfs.c

@author  Daniel Zorychta

@brief   This file support process file system (procfs)

@note    Copyright (C) 2012-2015 Daniel Zorychta <daniel.zorychta@gmail.com>

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
#include "fs/fs.h"

#if defined(ARCH_stm32f1)
#include "stm32f1/lib/stm32f10x_rcc.h"
#endif

/*==============================================================================
  Local symbolic constants/macros
==============================================================================*/
/* task ID string length (8B name + \0). ID is 32b hex number converted to string */
#define TASK_ID_STR_LEN                 9

#define FILE_DATA_SIZE                  256

#define DIR_TASKID_STR                  "taskid"
#define DIR_TASKNAME_STR                "taskname"
#define DIR_BIN_STR                     "bin"
#define FILE_CPUINFO_STR                "cpuinfo"
#define FILE_TASK_NAME_STR              "name"
#define FILE_TASK_PRIO_STR              "priority"
#define FILE_TASK_FREESTACK_STR         "freestack"
#define FILE_TASK_USEDMEM_STR           "usedmem"
#define FILE_TASK_OPENFILES_STR         "openfiles"

#define MTX_BLOCK_TIME                  10

#define SECOND_CHARACTER(_s)            _s[1]

/*==============================================================================
  Local types, enums definitions
==============================================================================*/
struct procfs {
      llist_t *file_list;
      mutex_t *resource_mtx;
};

struct file_info {
      task_t *taskhdl;

      enum file_content {
              FILE_CONTENT_TASK_NAME,
              FILE_CONTENT_TASK_PRIO,
              FILE_CONTENT_TASK_FREESTACK,
              FILE_CONTENT_TASK_USEDMEM,
              FILE_CONTENT_TASK_OPENFILES,
              FILE_CONTENT_CPUINFO,
              FILE_CONTENT_COUNT,
              FILE_CONTENT_NONE
      } file_content;
};

/*==============================================================================
  Local function prototypes
==============================================================================*/
static int         procfs_closedir_freedd  (void *fs_handle, DIR *dir);
static int         procfs_closedir_generic (void *fs_handle, DIR *dir);
static int         procfs_readdir_root     (void *fs_handle, DIR *dir, dirent_t **dirent);
static int         procfs_readdir_taskname (void *fs_handle, DIR *dir, dirent_t **dirent);
static int         procfs_readdir_bin      (void *fs_handle, DIR *dir, dirent_t **dirent);
static int         procfs_readdir_taskid   (void *fs_handle, DIR *dir, dirent_t **dirent);
static int         procfs_readdir_taskid_n (void *fs_handle, DIR *dir, dirent_t **dirent);
static inline void mutex_force_lock        (mutex_t *mtx);
static int         add_file_info_to_list   (struct procfs *procmem, task_t *taskhdl, enum file_content file_content, fd_t *fd);
static uint        get_file_content        (struct file_info *file_info, char *buff, uint size);

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
 * @return One of errno value (errno.h)
 */
//==============================================================================
API_FS_INIT(procfs, void **fs_handle, const char *src_path)
{
        UNUSED_ARG(src_path);

        int result = _sys_zalloc(sizeof(struct procfs), fs_handle);
        if (result == ESUCC) {
                struct procfs *procfs = *fs_handle;

                result = _sys_llist_create(_sys_llist_functor_cmp_pointers, NULL, &procfs->file_list);
                if (result != ESUCC)
                        goto finish;

                result = _sys_mutex_create(MUTEX_TYPE_NORMAL, &procfs->resource_mtx);
                if (result != ESUCC)
                        goto finish;

                finish:
                if (result != ESUCC) {
                        if (procfs->file_list)
                                _sys_llist_destroy(procfs->file_list);

                        if (procfs->resource_mtx)
                                _sys_mutex_destroy(procfs->resource_mtx);

                        _sys_free(fs_handle);
                }
        }

        return result;
}

//==============================================================================
/**
 * @brief Release file system
 *
 * @param[in ]          *fs_handle              file system allocated memory
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
API_FS_RELEASE(procfs, void *fs_handle)
{
        struct procfs *procfs = fs_handle;

        if (_sys_mutex_lock(procfs->resource_mtx, 100) == ESUCC) {
                if (_sys_llist_size(procfs->file_list) != 0) {
                        _sys_mutex_unlock(procfs->resource_mtx);
                        return EBUSY;
                }

                _sys_critical_section_begin();
                {
                        _sys_mutex_unlock(procfs->resource_mtx);
                        _sys_mutex_destroy(procfs->resource_mtx);
                        _sys_llist_destroy(procfs->file_list);
                        _sys_free(&fs_handle);
                }
                _sys_critical_section_end();

                return ESUCC;

        } else {
                return EBUSY;
        }
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
 * @param[in]            flags                  file open flags
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
API_FS_OPEN(procfs, void *fs_handle, void **extra, fd_t *fd, fpos_t *fpos, const char *path, u32_t flags)
{
        UNUSED_ARG(extra);

        struct procfs *procmem = fs_handle;

        if (flags != O_RDONLY) {
                return EROFS;
        }

        *fpos = 0;

        if (strncmp(path, "/"DIR_TASKID_STR"/", strlen("/"DIR_TASKID_STR"/")) == 0) {
                path += strlen("/"DIR_TASKID_STR"/");

                task_t *taskhdl = NULL;
                path = _strtoi((char*)path, 16, (i32_t*)&taskhdl);

//                struct _sysmoni_taskstat task_data; TODO _sys_ functions
//                if (_sysm_get_task_stat(taskhdl, &task_data) != ESUCC) {
                        return ENOENT;
//                }

                path = strrchr(path, '/');
                if (path == NULL) {
                        return ENOENT;
                } else {
                        path++;
                }

                enum file_content file_content;

                if (strcmp((char*) path, FILE_TASK_NAME_STR) == 0) {
                        file_content = FILE_CONTENT_TASK_NAME;
                } else if (strcmp((char*) path, FILE_TASK_FREESTACK_STR) == 0) {
                        file_content = FILE_CONTENT_TASK_FREESTACK;
                } else if (strcmp((char*) path, FILE_TASK_OPENFILES_STR) == 0) {
                        file_content = FILE_CONTENT_TASK_OPENFILES;
                } else if (strcmp((char*) path, FILE_TASK_PRIO_STR) == 0) {
                        file_content = FILE_CONTENT_TASK_PRIO;
                } else if (strcmp((char*) path, FILE_TASK_USEDMEM_STR) == 0) {
                        file_content = FILE_CONTENT_TASK_USEDMEM;
                } else {
                        return ENOENT;
                }

                return add_file_info_to_list(procmem, taskhdl, file_content, fd);

        } else if (strncmp(path, "/"DIR_TASKNAME_STR"/", strlen("/"DIR_TASKNAME_STR"/")) == 0) {

                path += strlen("/"DIR_TASKNAME_STR"/");

//                u16_t n = _sysm_get_number_of_monitored_tasks();TODO _sys_ functions
//                u16_t i = 0;

//                struct _sysmoni_taskstat task_data;TODO _sys_ functions
//                while (n-- && _sysm_get_ntask_stat(i++, &task_data) == ESUCC) {
//                        if (strcmp(path, task_data.task_name) == 0) {
//                                return add_file_info_to_list(procmem,
//                                                             task_data.task_handle,
//                                                             FILE_CONTENT_NONE, fd);
//                        }
//                }

                return ENOENT;

        } else if (strcmp(path, "/"FILE_CPUINFO_STR) == 0) {
                return add_file_info_to_list(procmem, NULL, FILE_CONTENT_CPUINFO, fd);

        } else if (strncmp(path, "/"DIR_BIN_STR"/", strlen("/"DIR_BIN_STR"/")) == 0) {
                path += strlen("/"DIR_BIN_STR"/");

                for (int i = 0; i < _sys_get_programs_table_size(); i++) {
                        if (strcmp(path, _sys_get_programs_table()[i].program_name) == 0) {
                                return add_file_info_to_list(procmem, NULL, FILE_CONTENT_NONE, fd);
                        }
                }

                return ENOENT;
        } else {
                return ENOENT;
        }
}

//==============================================================================
/**
 * @brief Close file
 *
 * @param[in ]          *fs_handle              file system allocated memory
 * @param[in ]          *extra                  file extra data
 * @param[in ]           fd                     file descriptor
 * @param[in ]           force                  force close
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
API_FS_CLOSE(procfs, void *fs_handle, void *extra, fd_t fd, bool force)
{
        UNUSED_ARG(extra);
        UNUSED_ARG(force);

        struct procfs *procmem = fs_handle;

        mutex_force_lock(procmem->resource_mtx);

        int pos    = _sys_llist_find_begin(procmem->file_list, reinterpret_cast(void *, fd));
        int result = _sys_llist_erase(procmem->file_list, pos) ? ESUCC : ENOENT;

        _sys_mutex_unlock(procmem->resource_mtx);

        return result;
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
 * @param[out]          *wrcnt                  number of written bytes
 * @param[in ]           fattr                  file attributes
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
API_FS_WRITE(procfs,
             void            *fs_handle,
             void            *extra,
             fd_t             fd,
             const u8_t      *src,
             size_t           count,
             fpos_t          *fpos,
             size_t          *wrcnt,
             struct vfs_fattr fattr)
{
        UNUSED_ARG(fs_handle);
        UNUSED_ARG(extra);
        UNUSED_ARG(fd);
        UNUSED_ARG(src);
        UNUSED_ARG(count);
        UNUSED_ARG(wrcnt);
        UNUSED_ARG(fpos);
        UNUSED_ARG(fattr);

        return EROFS;
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
 * @param[out]          *rdcnt                  number of read bytes
 * @param[in ]           fattr                  file attributes
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
API_FS_READ(procfs,
            void            *fs_handle,
            void            *extra,
            fd_t             fd,
            u8_t            *dst,
            size_t           count,
            fpos_t          *fpos,
            size_t          *rdcnt,
            struct vfs_fattr fattr)
{
        UNUSED_ARG2(extra, fattr);

        struct procfs *procmem = fs_handle;

        mutex_force_lock(procmem->resource_mtx);
        struct file_info *file_info = reinterpret_cast(struct file_info*, fd);
        _sys_mutex_unlock(procmem->resource_mtx);

        if (file_info == NULL) {
                return ENOENT;
        }

        if (file_info->file_content == FILE_CONTENT_NONE) {
                *rdcnt = 0;
                return ESUCC;
        }

        if (file_info->file_content >= FILE_CONTENT_COUNT) {
                return ENOENT;
        }

        char *data;
        int result = _sys_zalloc(FILE_DATA_SIZE, static_cast(void**, &data));
        if (result == ESUCC) {
                uint data_size = get_file_content(file_info, data, FILE_DATA_SIZE);

                size_t seek = *fpos > SIZE_MAX ? SIZE_MAX : *fpos;
                if (seek > data_size) {
                        *rdcnt = 0;
                } else {
                        size_t n = (data_size - seek <= count) ? data_size - seek : count;
                        strncpy((char *)dst, data + seek, n);
                        *rdcnt = n;
                }

                _sys_free(static_cast(void**, &data));
        }

        return result;
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
 * @return One of errno value (errno.h)
 */
//==============================================================================
API_FS_IOCTL(procfs, void *fs_handle, void *extra, fd_t fd, int request, void *arg)
{
        UNUSED_ARG(fs_handle);
        UNUSED_ARG(extra);
        UNUSED_ARG(fd);
        UNUSED_ARG(request);
        UNUSED_ARG(arg);

        return ENOTSUP;
}

//==============================================================================
/**
 * @brief Flush file data
 *
 * @param[in ]          *fs_handle              file system allocated memory
 * @param[in ]          *extra                  file extra data
 * @param[in ]           fd                     file descriptor
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
API_FS_FLUSH(procfs, void *fs_handle, void *extra, fd_t fd)
{
        UNUSED_ARG(fs_handle);
        UNUSED_ARG(extra);
        UNUSED_ARG(fd);

        return ESUCC;
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
 * @return One of errno value (errno.h)
 */
//==============================================================================
API_FS_FSTAT(procfs, void *fs_handle, void *extra, fd_t fd, struct stat *stat)
{
        UNUSED_ARG(extra);

        struct procfs *procmem = fs_handle;

        mutex_force_lock(procmem->resource_mtx);
        struct file_info *file_info = reinterpret_cast(struct file_info*, fd);
        _sys_mutex_unlock(procmem->resource_mtx);

        if (file_info == NULL) {
                return ENOENT;
        }

        if (file_info->file_content >= FILE_CONTENT_COUNT) {
                return ENOENT;
        }

        stat->st_dev   = 0;
        stat->st_mode  = S_IRUSR | S_IRGRO | S_IROTH;
        stat->st_mtime = 0;
        stat->st_size  = 0;
        stat->st_gid   = 0;
        stat->st_uid   = 0;
        stat->st_type  = FILE_TYPE_REGULAR;

        char *data;
        int result = _sys_zalloc(FILE_DATA_SIZE, static_cast(void**, &data));
        if (result == ESUCC) {
                stat->st_size = get_file_content(file_info, data, FILE_DATA_SIZE);
                _sys_free(static_cast(void**, &data));
        }

        return result;
}

//==============================================================================
/**
 * @brief Create directory
 *
 * @param[in ]          *fs_handle              file system allocated memory
 * @param[in ]          *path                   name of created directory
 * @param[in ]           mode                   dir mode
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
API_FS_MKDIR(procfs, void *fs_handle, const char *path, mode_t mode)
{
        UNUSED_ARG(fs_handle);
        UNUSED_ARG(path);
        UNUSED_ARG(mode);

        return EROFS;
}

//==============================================================================
/**
 * @brief Create pipe
 *
 * @param[in ]          *fs_handle              file system allocated memory
 * @param[in ]          *path                   name of created pipe
 * @param[in ]           mode                   pipe mode
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
API_FS_MKFIFO(procfs, void *fs_handle, const char *path, mode_t mode)
{
        UNUSED_ARG(fs_handle);
        UNUSED_ARG(path);
        UNUSED_ARG(mode);

        return ENOTSUP;
}

//==============================================================================
/**
 * @brief Create node for driver file
 *
 * @param[in ]          *fs_handle              file system allocated memory
 * @param[in ]          *path                   name of created node
 * @param[in ]           dev                    driver number
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
API_FS_MKNOD(procfs, void *fs_handle, const char *path, const dev_t dev)
{
        UNUSED_ARG(fs_handle);
        UNUSED_ARG(path);
        UNUSED_ARG(dev);

        return ENOTSUP;
}

//==============================================================================
/**
 * @brief Open directory
 *
 * @param[in ]          *fs_handle              file system allocated memory
 * @param[in ]          *path                   name of opened directory
 * @param[in ]          *dir                    directory object
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
API_FS_OPENDIR(procfs, void *fs_handle, const char *path, DIR *dir)
{
        UNUSED_ARG(fs_handle);

        dir->f_seek = 0;

        int result = ENOENT;

        if (strcmp(path, "/") == 0) {
                dir->f_dd       = NULL;
                dir->f_items    = 4;
                dir->f_readdir  = procfs_readdir_root;
                dir->f_closedir = procfs_closedir_generic;
                result = ESUCC;

        } else if (strcmp(path, "/"DIR_TASKNAME_STR"/") == 0) {
                dir->f_dd       = NULL;
//                dir->f_items    = _sysm_get_number_of_monitored_tasks(); // TODO _sys function needed
                dir->f_readdir  = procfs_readdir_taskname;
                dir->f_closedir = procfs_closedir_generic;
                result = ESUCC;

        } else if (strcmp(path, "/"DIR_TASKID_STR"/") == 0) {
                _sys_zalloc(TASK_ID_STR_LEN, &dir->f_dd);
//                dir->f_items    = _sysm_get_number_of_monitored_tasks(); // TODO _sys function needed
                dir->f_readdir  = procfs_readdir_taskid;
                dir->f_closedir = procfs_closedir_freedd;
                result = ESUCC;

        } else if (strcmp(path, "/"DIR_BIN_STR"/") == 0) {
                dir->f_dd       = NULL;
                dir->f_items    = _sys_get_programs_table_size();
                dir->f_readdir  = procfs_readdir_bin;
                dir->f_closedir = procfs_closedir_generic;
                result = ESUCC;

        } else if (strncmp(path, "/"DIR_TASKID_STR"/", strlen("/"DIR_TASKID_STR"/")) == 0) {
                path += strlen("/"DIR_TASKID_STR"/");

                i32_t taskval = 0;
                path = _strtoi((char*)path, 16, &taskval);

                if (FIRST_CHARACTER(path) == '/' && SECOND_CHARACTER(path) == '\0') {
//                        struct _sysmoni_taskstat taskdata; // TODO _sys function needed
//                        task_t                 *taskHdl = (task_t *)taskval; // TODO _sys function needed
//                        result = _sysm_get_task_stat(taskHdl, &taskdata); // TODO _sys function needed
                        if (result == ESUCC) {
//                                dir->f_dd       = taskHdl; // TODO _sys function needed
                                dir->f_items    = FILE_CONTENT_COUNT;
                                dir->f_readdir  = procfs_readdir_taskid_n;
                                dir->f_closedir = procfs_closedir_generic;
                        }
                }
        }

        return result;
}

//==============================================================================
/**
 * @brief Function close opened dir (is used when dd contains pointer to
 *        allocated block)
 *
 * @param[in]  *fs_handle    file system data
 * @param[out] *dir          directory object
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
static int procfs_closedir_freedd(void *fs_handle, DIR *dir)
{
        UNUSED_ARG(fs_handle);

        if (dir->f_dd) {
                _sys_free(&dir->f_dd);
                dir->f_dd = NULL;
                return ESUCC;
        } else {
                return ENOENT;
        }
}

//==============================================================================
/**
 * @brief Function close opened dir (is used when dd contains data which cannot
 *        be freed)
 *
 * @param[in]  *fs_handle    file system data
 * @param[out] *dir          directory object
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
static int procfs_closedir_generic(void *fs_handle, DIR *dir)
{
        UNUSED_ARG(fs_handle);
        UNUSED_ARG(dir);

        return ESUCC;
}

//==============================================================================
/**
 * @brief Remove file/directory
 *
 * @param[in ]          *fs_handle              file system allocated memory
 * @param[in ]          *path                   name of removed file/directory
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
API_FS_REMOVE(procfs, void *fs_handle, const char *path)
{
        UNUSED_ARG(fs_handle);
        UNUSED_ARG(path);

        return EROFS;
}

//==============================================================================
/**
 * @brief Rename file/directory
 *
 * @param[in ]          *fs_handle              file system allocated memory
 * @param[in ]          *old_name               old object name
 * @param[in ]          *new_name               new object name
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
API_FS_RENAME(procfs, void *fs_handle, const char *old_name, const char *new_name)
{
        UNUSED_ARG(fs_handle);
        UNUSED_ARG(old_name);
        UNUSED_ARG(new_name);

        return EROFS;
}

//==============================================================================
/**
 * @brief Change file's mode
 *
 * @param[in ]          *fs_handle              file system allocated memory
 * @param[in ]          *path                   file path
 * @param[in ]           mode                   new file mode
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
API_FS_CHMOD(procfs, void *fs_handle, const char *path, mode_t mode)
{
        UNUSED_ARG(fs_handle);
        UNUSED_ARG(path);
        UNUSED_ARG(mode);

        return EROFS;
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
 * @return One of errno value (errno.h)
 */
//==============================================================================
API_FS_CHOWN(procfs, void *fs_handle, const char *path, uid_t owner, gid_t group)
{
        UNUSED_ARG(fs_handle);
        UNUSED_ARG(path);
        UNUSED_ARG(owner);
        UNUSED_ARG(group);

        return EROFS;
}

//==============================================================================
/**
 * @brief Return file/dir status
 *
 * @param[in ]          *fs_handle              file system allocated memory
 * @param[in ]          *path                   file path
 * @param[out]          *stat                   file status
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
API_FS_STAT(procfs, void *fs_handle, const char *path, struct stat *stat)
{
        UNUSED_ARG(fs_handle);
        UNUSED_ARG(path);

        stat->st_dev   = 0;
        stat->st_gid   = 0;
        stat->st_mode  = S_IRUSR | S_IRGRO | S_IROTH;
        stat->st_mtime = 0;
        stat->st_size  = 0;
        stat->st_uid   = 0;
        stat->st_type  = FILE_TYPE_REGULAR;

        return ESUCC;
}

//==============================================================================
/**
 * @brief Return file system status
 *
 * @param[in ]          *fs_handle              file system allocated memory
 * @param[out]          *statfs                 file system status
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
API_FS_STATFS(procfs, void *fs_handle, struct statfs *statfs)
{
        UNUSED_ARG(fs_handle);

        statfs->f_bfree  = 0;
        statfs->f_blocks = 0;
        statfs->f_ffree  = 0;
        statfs->f_files  = 0;
        statfs->f_type   = 1;
        statfs->f_fsname = "procfs";

        return ESUCC;
}

//==============================================================================
/**
 * @brief Synchronize all buffers to a medium
 *
 * @param[in ]          *fs_handle              file system allocated memory
 *
 * @return None
 */
//==============================================================================
API_FS_SYNC(procfs, void *fs_handle)
{
        UNUSED_ARG(fs_handle);
        return ESUCC;
}

//==============================================================================
/**
 * @brief Read directory
 *
 * @param[in ]          *fs_handle              file system allocated memory
 * @param[in ]          *dir                    directory object
 * @param[out]          **dirent                directory entry
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
static int procfs_readdir_root(void *fs_handle, DIR *dir, dirent_t **dirent)
{
        UNUSED_ARG(fs_handle);

        int result = ENOENT;

        *dirent = &dir->dirent;

        dir->dirent.filetype = FILE_TYPE_DIR;
        dir->dirent.size     = 0;

        switch (dir->f_seek++) {
        case 0:
                dir->dirent.name = DIR_TASKID_STR;
                result = ESUCC;
                break;

        case 1:
                dir->dirent.name = DIR_TASKNAME_STR;
                result = ESUCC;
                break;

        case 2:
                dir->dirent.name = DIR_BIN_STR;
                result = ESUCC;
                break;

        case 3: {
                char *data;
                result = _sys_zalloc(FILE_DATA_SIZE, static_cast(void**, &data));
                if (result == ESUCC) {
                        struct file_info file_info;
                        file_info.file_content = FILE_CONTENT_CPUINFO;
                        dir->dirent.size       = get_file_content(&file_info, data, FILE_DATA_SIZE);
                        dir->dirent.name       = FILE_CPUINFO_STR;
                        dir->dirent.filetype   = FILE_TYPE_REGULAR;
                        _sys_free(static_cast(void**, data));
                        result = ESUCC;
                }
                break;
        }
        }

        return result;
}

//==============================================================================
/**
 * @brief Read directory
 *
 * @param[in ]          *fs_handle              file system allocated memory
 * @param[in ]          *dir                    directory object
 * @param[out]          **dirent                directory entry
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
static int procfs_readdir_taskname(void *fs_handle, DIR *dir, dirent_t **dirent)
{
        UNUSED_ARG(fs_handle);

//        struct _sysmoni_taskstat taskdata; // TODO _sys function needed
//        int result = _sysm_get_ntask_stat(dir->f_seek, &taskdata);
//        if (result == ESUCC) {
//                dir->dirent.filetype = FILE_TYPE_REGULAR;
//                dir->dirent.name     = taskdata.task_name;
//                dir->dirent.size     = 0;
//                dir->f_seek++;
//                *dirent = &dir->dirent;
//        } else {
//                result = ENOENT;
//        }
//
//        return result;
}

//==============================================================================
/**
 * @brief Read directory
 *
 * @param[in ]          *fs_handle              file system allocated memory
 * @param[in ]          *dir                    directory object
 * @param[out]          **dirent                directory entry
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
static int procfs_readdir_bin(void *fs_handle, DIR *dir, dirent_t **dirent)
{
        UNUSED_ARG(fs_handle);

        if (dir->f_seek < (size_t)_sys_get_programs_table_size()) {
                dir->dirent.filetype = FILE_TYPE_PROGRAM;
                dir->dirent.name     = _sys_get_programs_table()[dir->f_seek].program_name;
                dir->dirent.size     = 0;

                dir->f_seek++;

                *dirent = &dir->dirent;

                return ESUCC;
        } else {
                return ENOENT;
        }
}

//==============================================================================
/**
 * @brief Read directory
 *
 * @param[in ]          *fs_handle              file system allocated memory
 * @param[in ]          *dir                    directory object
 * @param[out]          **dirent                directory entry
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
static int procfs_readdir_taskid(void *fs_handle, DIR *dir, dirent_t **dirent)
{
        UNUSED_ARG(fs_handle);

        int result = ENOENT;
        // TODO _sys function needed
//        if (dir->f_dd && dir->f_seek < (size_t)_sysm_get_number_of_monitored_tasks()) {
//                struct _sysmoni_taskstat taskdata;
//                result = _sysm_get_ntask_stat(dir->f_seek, &taskdata);
//                if (result == ESUCC) {
//                        _sys_snprintf(dir->f_dd, TASK_ID_STR_LEN, "%x", (int)taskdata.task_handle);
//
//                        dir->dirent.filetype = FILE_TYPE_DIR;
//                        dir->dirent.name     = dir->f_dd;
//                        dir->dirent.size     = 0;
//
//                        dir->f_seek++;
//
//                        *dirent = &dir->dirent;
//                }
//        }

        return result;
}

//==============================================================================
/**
 * @brief Read directory
 *
 * @param[in ]          *fs_handle              file system allocated memory
 * @param[in ]          *dir                    directory object
 * @param[out]          **dirent                directory entry
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
static int procfs_readdir_taskid_n(void *fs_handle, DIR *dir, dirent_t **dirent)
{
        UNUSED_ARG(fs_handle);

        if (dir->f_seek >= FILE_CONTENT_COUNT) {
                return ENOENT;
        }

        dir->dirent.filetype = FILE_TYPE_REGULAR;

        switch (dir->f_seek) {
        case FILE_CONTENT_TASK_NAME     : dir->dirent.name = FILE_TASK_NAME_STR; break;
        case FILE_CONTENT_TASK_PRIO     : dir->dirent.name = FILE_TASK_PRIO_STR; break;
        case FILE_CONTENT_TASK_FREESTACK: dir->dirent.name = FILE_TASK_FREESTACK_STR; break;
        case FILE_CONTENT_TASK_USEDMEM  : dir->dirent.name = FILE_TASK_USEDMEM_STR; break;
        case FILE_CONTENT_TASK_OPENFILES: dir->dirent.name = FILE_TASK_OPENFILES_STR; break;
        }

        char *data;
        int result = _sys_zalloc(FILE_DATA_SIZE, static_cast(void**, &data));
        if (result == ESUCC) {
                struct file_info file_info;
                file_info.file_content = dir->f_seek;
                file_info.taskhdl      = dir->f_dd;

                dir->dirent.size = get_file_content(&file_info, data, FILE_DATA_SIZE);
                dir->f_seek++;

                *dirent = &dir->dirent;

                _sys_free(static_cast(void**, &data));
        }

        return result;
}

//==============================================================================
/**
 * @brief Force lock mutex
 *
 * @param mtx           mutex
 */
//==============================================================================
static inline void mutex_force_lock(mutex_t *mtx)
{
        while (_sys_mutex_lock(mtx, MTX_BLOCK_TIME) != ESUCC);
}

//==============================================================================
/**
 * @brief Add file info to list
 *
 * @param procmem               FS context
 * @param taskhdl               task handle to write in file info
 * @param file_content          file content to write in file info
 * @param fd                    file descriptor (result)
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
static int add_file_info_to_list(struct procfs *procmem, task_t *taskhdl, enum file_content file_content, fd_t *fd)
{
        struct file_info *file_info;
        int result = _sys_zalloc(sizeof(struct file_info), static_cast(void**, &file_info));
        if (result == ESUCC) {
                file_info->taskhdl      = taskhdl;
                file_info->file_content = file_content;

                mutex_force_lock(procmem->resource_mtx);

                if (_sys_llist_push_back(procmem->file_list, file_info)) {
                        *fd = (fd_t)file_info;

                } else {
                        _sys_free(static_cast(void**, &file_info));
                        result = ENOMEM;
                }

                _sys_mutex_unlock(procmem->resource_mtx);
        }

        return result;
}

//==============================================================================
/**
 * @brief Function return file content and size
 *
 * @param file_info     file information
 * @param buff          buffer
 * @param size          buffer size
 *
 * @return number of bytes written to buffer
 */
//==============================================================================
static uint get_file_content(struct file_info *file_info, char *buff, uint size)
{
//        struct _sysmoni_taskstat task_info; // TODO _sys function needed
//        if (file_info->file_content < FILE_CONTENT_CPUINFO) {
//                if (_sysm_get_task_stat(file_info->taskhdl, &task_info) != ESUCC) {
//                        return 0;
//                }
//        }

//        switch (file_info->file_content) { // TODO _sys function needed
//        case FILE_CONTENT_TASK_FREESTACK:
//                return _sys_snprintf(buff, size, "%u\n", task_info.free_stack);
//
//        case FILE_CONTENT_TASK_NAME:
//                return _sys_snprintf(buff, size, "%s\n", task_info.task_name);
//
//        case FILE_CONTENT_TASK_OPENFILES:
//                return _sys_snprintf(buff, size, "%u\n", task_info.opened_files);
//
//        case FILE_CONTENT_TASK_PRIO:
//                return _sys_snprintf(buff, size, "%d\n", task_info.priority);
//
//        case FILE_CONTENT_TASK_USEDMEM:
//                return _sys_snprintf(buff, size, "%u\n", task_info.memory_usage);

//        case FILE_CONTENT_CPUINFO: {
//                #if defined(ARCH_stm32f1)
//                RCC_ClocksTypeDef freq;
//                RCC_GetClocksFreq(&freq);
//                #endif
//
//                return _sys_snprintf(buff, size,
//                                    "CPU name  : %s\n"
//                                    "CPU vendor: %s\n"
//                            #if defined(ARCH_stm32f1)
//                                    "CPU     Hz: %d\n"
//                                    "SYSCLK  Hz: %d\n"
//                                    "PCLK1   Hz: %d\n"
//                                    "PCLK1T  Hz: %d\n"
//                                    "PCLK2   Hz: %d\n"
//                                    "PCLK2T  Hz: %d\n"
//                                    "ADCCLK  Hz: %d\n"
//                            #endif
//                                    ,_CPUCTL_PLATFORM_NAME
//                                    ,_CPUCTL_VENDOR_NAME
//                            #if defined(ARCH_stm32f1)
//                                    ,freq.HCLK_Frequency
//                                    ,freq.SYSCLK_Frequency
//                                    ,freq.PCLK1_Frequency
//                                    ,(RCC->CFGR & RCC_CFGR_PPRE1_2) ? (freq.PCLK1_Frequency / 2) : freq.PCLK1_Frequency
//                                    ,freq.PCLK2_Frequency
//                                    ,(RCC->CFGR & RCC_CFGR_PPRE2_2) ? (freq.PCLK2_Frequency / 2) : freq.PCLK2_Frequency
//                                    ,freq.ADCCLK_Frequency
//                            #endif
//                );
//        }
//
//        default:
//                return 0;
//        }
}

/*==============================================================================
  End of file
==============================================================================*/
