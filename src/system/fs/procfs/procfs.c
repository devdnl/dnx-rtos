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

/*==============================================================================
  Local symbolic constants/macros
==============================================================================*/
#define CLK_FILE_PATH                   "/dev/pll"

#define DIR_ROOT                        "/"
#define DIR_PID_NAME                    "pid"
#define DIR_BIN_NAME                    "bin"
#define FILE_CPUINFO_NAME               "cpuinfo"

#define PATH_ROOT                       DIR_ROOT
#define PATH_ROOT_PID                   (DIR_ROOT DIR_PID_NAME "/")
#define PATH_ROOT_BIN                   (DIR_ROOT DIR_BIN_NAME "/")
#define PATH_ROOT_CPUINFO               (DIR_ROOT FILE_CPUINFO_NAME)

#define FILE_BUFFER                     384
#define PID_STR_LEN                     12

/*==============================================================================
  Local types, enums definitions
==============================================================================*/
struct procfs {
      llist_t *file_list;
      mutex_t *resource_mtx;
};

struct file_info {
      pid_t pid;

      enum file_content {
              FILE_CONTENT_PID,
              FILE_CONTENT_CPUINFO,
              FILE_CONTENT_BIN,
              FILE_CONTENT_COUNT
      } content;
};

/*==============================================================================
  Local function prototypes
==============================================================================*/
static int    procfs_closedir_generic(void *fs_handle, DIR *dir);
static int    procfs_readdir_root    (void *fs_handle, DIR *dir, dirent_t **dirent);
static int    procfs_readdir_pid     (void *fs_handle, DIR *dir, dirent_t **dirent);
static int    procfs_readdir_bin     (void *fs_handle, DIR *dir, dirent_t **dirent);
static int    add_file_to_list       (struct procfs *fsctx, pid_t pid, enum file_content content, void **object);
static size_t get_file_content       (struct file_info *file_info, char *buff, size_t size);

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
        UNUSED_ARG1(src_path);

        int result = sys_zalloc(sizeof(struct procfs), fs_handle);
        if (result == ESUCC) {
                struct procfs *procfs = *fs_handle;

                result = sys_llist_create(sys_llist_functor_cmp_pointers, NULL, &procfs->file_list);
                if (result != ESUCC)
                        goto finish;

                result = sys_mutex_create(MUTEX_TYPE_NORMAL, &procfs->resource_mtx);
                if (result != ESUCC)
                        goto finish;

                finish:
                if (result != ESUCC) {
                        if (procfs->file_list)
                                sys_llist_destroy(procfs->file_list);

                        if (procfs->resource_mtx)
                                sys_mutex_destroy(procfs->resource_mtx);

                        sys_free(fs_handle);
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

        if (sys_mutex_lock(procfs->resource_mtx, 100) == ESUCC) {
                if (sys_llist_size(procfs->file_list) != 0) {
                        sys_mutex_unlock(procfs->resource_mtx);
                        return EBUSY;
                }

                sys_critical_section_begin();
                {
                        sys_mutex_unlock(procfs->resource_mtx);
                        sys_mutex_destroy(procfs->resource_mtx);
                        sys_llist_destroy(procfs->file_list);
                        sys_free(&fs_handle);
                }
                sys_critical_section_end();

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
 * @param[out]          *fhdl                   file handle
 * @param[out]          *fpos                   file position
 * @param[in]           *path                   file path
 * @param[in]            flags                  file open flags
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
API_FS_OPEN(procfs, void *fs_handle, void **fhdl, fpos_t *fpos, const char *path, u32_t flags)
{
        struct procfs *fsctx = fs_handle;

        int result = EROFS;

        if (flags == O_RDONLY) {
                *fpos = 0;

                if (strncmp(path, PATH_ROOT_PID, strlen(PATH_ROOT_PID)) == 0) {
                        if (LAST_CHARACTER(path) != '/') {
                                path += strlen(PATH_ROOT_PID);

                                i32_t pid = 0;
                                sys_strtoi(path, 10, &pid);

                                process_stat_t stat;
                                if (sys_process_get_stat_pid(pid, &stat) == ESUCC) {
                                        return add_file_to_list(fsctx, pid, FILE_CONTENT_PID, fhdl);
                                } else {
                                        result = ENOENT;
                                }
                        } else {
                                result = ENOENT;
                        }

                } else if (strncmp(path, PATH_ROOT_BIN, strlen(PATH_ROOT_BIN)) == 0) {
                        path += strlen(PATH_ROOT_BIN);

                        for (int i = 0; i < sys_get_programs_table_size(); i++) {
                                if (strcmp(path, sys_get_programs_table()[i].name) == 0) {
                                        return add_file_to_list(fsctx, 0, FILE_CONTENT_BIN, fhdl);
                                }
                        }

                } else if (strcmp(path, PATH_ROOT_CPUINFO) == 0) {
                        return add_file_to_list(fsctx, 0, FILE_CONTENT_CPUINFO, fhdl);

                } else {
                        result = ENOENT;
                }
        }

        return result;
}

//==============================================================================
/**
 * @brief Close file
 *
 * @param[in ]          *fs_handle              file system allocated memory
 * @param[in ]          *fhdl                   file fhdl data
 * @param[in ]           force                  force close
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
API_FS_CLOSE(procfs, void *fs_handle, void *fhdl, bool force)
{
        UNUSED_ARG1(force);

        struct procfs *fsctx = fs_handle;

        int result = sys_mutex_lock(fsctx->resource_mtx, 1000);
        if (result == ESUCC) {
                int pos = sys_llist_find_begin(fsctx->file_list, fhdl);
                result  = sys_llist_erase(fsctx->file_list, pos) ? ESUCC : ENOENT;

                sys_mutex_unlock(fsctx->resource_mtx);
        }

        return result;
}

//==============================================================================
/**
 * @brief Write data to the file
 *
 * @param[in ]          *fs_handle              file system allocated memory
 * @param[in ]          *fhdl                   file handle
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
             void            *fhdl,
             const u8_t      *src,
             size_t           count,
             fpos_t          *fpos,
             size_t          *wrcnt,
             struct vfs_fattr fattr)
{
        UNUSED_ARG7(fs_handle, fhdl, src, count, wrcnt, fpos, fattr);

        return EROFS;
}

//==============================================================================
/**
 * @brief Read data from file
 *
 * @param[in ]          *fs_handle              file system allocated memory
 * @param[in ]          *fhdl                   file fhdl data
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
            void            *fhdl,
            u8_t            *dst,
            size_t           count,
            fpos_t          *fpos,
            size_t          *rdcnt,
            struct vfs_fattr fattr)
{
        UNUSED_ARG2(fs_handle, fattr);

        struct file_info *file   = fhdl;
        int               result = ENOENT;

        if (file && file->content < FILE_CONTENT_COUNT) {

                char *content;
                result = sys_zalloc(FILE_BUFFER, cast(void**, &content));
                if (result == ESUCC) {
                        size_t data_size = get_file_content(file, content, FILE_BUFFER);
                        size_t seek      = min(*fpos, SIZE_MAX);
                        if (seek > data_size) {
                                *rdcnt = 0;
                        } else {
                                size_t n = (data_size - seek <= count) ? data_size - seek : count;
                                strncpy((char *)dst, content + seek, n);
                                *rdcnt = n;
                        }

                        sys_free(cast(void**, &content));
                }
        }

        return result;
}

//==============================================================================
/**
 * @brief IO operations on files
 *
 * @param[in ]          *fs_handle              file system allocated memory
 * @param[in ]          *fhdl                   file handle
 * @param[in ]           request                request
 * @param[in ][out]     *arg                    request's argument
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
API_FS_IOCTL(procfs, void *fs_handle, void *fhdl, int request, void *arg)
{
        UNUSED_ARG4(fs_handle, fhdl, request, arg);

        return ENOTSUP;
}

//==============================================================================
/**
 * @brief Flush file data
 *
 * @param[in ]          *fs_handle              file system allocated memory
 * @param[in ]          *fhdl                   file handle
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
API_FS_FLUSH(procfs, void *fs_handle, void *fhdl)
{
        UNUSED_ARG2(fs_handle, fhdl);

        return ESUCC;
}

//==============================================================================
/**
 * @brief Return file status
 *
 * @param[in ]          *fs_handle              file system allocated memory
 * @param[in ]          *fhdl                   file handle
 * @param[out]          *stat                   file status
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
API_FS_FSTAT(procfs, void *fs_handle, void *fhdl, struct stat *stat)
{
        UNUSED_ARG1(fs_handle);

        struct file_info *file   = fhdl;
        int               result = ENOENT;

        if (file && file->content < FILE_CONTENT_COUNT) {
                stat->st_dev   = 0;
                stat->st_mode  = S_IRUSR | S_IRGRO | S_IROTH;
                stat->st_mtime = 0;
                stat->st_size  = 0;
                stat->st_gid   = 0;
                stat->st_uid   = 0;
                stat->st_type  = FILE_TYPE_REGULAR;

                char *content;
                result = sys_zalloc(FILE_BUFFER, cast(void**, &content));
                if (result == ESUCC) {
                        stat->st_size = get_file_content(file, content, FILE_BUFFER);
                        sys_free(cast(void**, &content));
                }
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
        UNUSED_ARG3(fs_handle, path, mode);

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
        UNUSED_ARG3(fs_handle, path, mode);

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
        UNUSED_ARG3(fs_handle, path, dev);

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
        UNUSED_ARG1(fs_handle);

        dir->d_seek = 0;
        dir->d_dd   = NULL;

        int result = ENOENT;

        if (strcmp(path, PATH_ROOT) == 0) {
                dir->d_dd       = NULL;
                dir->d_items    = 3;
                dir->d_readdir  = procfs_readdir_root;
                dir->d_closedir = procfs_closedir_generic;
                result          = ESUCC;

        } else if (strcmp(path, PATH_ROOT_PID) == 0) {
                dir->d_dd       = NULL;
                dir->d_items    = sys_process_get_count();
                dir->d_readdir  = procfs_readdir_pid;
                dir->d_closedir = procfs_closedir_generic;
                result          = ESUCC;

        } else if (strcmp(path, PATH_ROOT_BIN) == 0) {
                dir->d_dd       = NULL;
                dir->d_items    = sys_get_programs_table_size();
                dir->d_readdir  = procfs_readdir_bin;
                dir->d_closedir = procfs_closedir_generic;
                result          = ESUCC;
        }

        return result;
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
        UNUSED_ARG2(fs_handle, path);

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
        UNUSED_ARG3(fs_handle, old_name, new_name);

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
        UNUSED_ARG3(fs_handle, path, mode);

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
        UNUSED_ARG4(fs_handle, path, owner, group);

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
        void  *fhdl = NULL;
        fpos_t fpos = 0;

        int err = _procfs_open(fs_handle, &fhdl, &fpos, path, O_RDONLY);
        if (!err) {
                err = _procfs_fstat(fs_handle, fhdl, stat);
                _procfs_close(fs_handle, fhdl, true);
        }

        return err;
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
        UNUSED_ARG1(fs_handle);

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
        UNUSED_ARG1(fs_handle);

        return ESUCC;
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
        UNUSED_ARG1(fs_handle);

        if (dir->d_dd) {
                return sys_free(&dir->d_dd);
        } else {
                return ESUCC;
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
static int procfs_readdir_root(void *fs_handle, DIR *dir, dirent_t **dirent)
{
        UNUSED_ARG1(fs_handle);

        int result;

        *dirent         = &dir->dirent;
        dir->dirent.dev = 0;

        switch (dir->d_seek++) {
        case 0:
                dir->dirent.name     = DIR_BIN_NAME;
                dir->dirent.filetype = FILE_TYPE_DIR;
                dir->dirent.size     = 0;
                result               = ESUCC;
                break;

        case 1:
                dir->dirent.name     = DIR_PID_NAME;
                dir->dirent.filetype = FILE_TYPE_DIR;
                dir->dirent.size     = 0;
                result               = ESUCC;
                break;

        case 2: {
                char *content;
                result = sys_zalloc(FILE_BUFFER, cast(void**, &content));
                if (result == ESUCC) {
                        struct file_info file = {.content = FILE_CONTENT_CPUINFO};
                        dir->dirent.name      = FILE_CPUINFO_NAME;
                        dir->dirent.filetype  = FILE_TYPE_REGULAR;
                        dir->dirent.size      = get_file_content(&file, content, FILE_BUFFER);
                        result                = sys_free(cast(void**, &content));
                }
                break;
        }

        default:
                *dirent = NULL;
                result  = ENOENT;
                break;
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
static int procfs_readdir_pid(void *fs_handle, DIR *dir, dirent_t **dirent)
{
        UNUSED_ARG1(fs_handle);

        process_stat_t stat;
        int result = sys_process_get_stat_seek(dir->d_seek++, &stat);
        if (result == ESUCC) {
                char *content;
                result  = sys_zalloc(FILE_BUFFER, cast(void**, &content));
                if (result == ESUCC) {

                        if (dir->d_dd) {
                                sys_free(&dir->d_dd);
                        }

                        result = sys_zalloc(PID_STR_LEN, &dir->d_dd);
                        if (result == ESUCC) {
                                /*
                                 * Note: freed in next cycle or dir close
                                 */
                                sys_snprintf(dir->d_dd, PID_STR_LEN, "%u", stat.pid);

                                dir->dirent.name      = dir->d_dd;
                                dir->dirent.filetype  = FILE_TYPE_REGULAR;
                                dir->dirent.dev       = 0;

                                struct file_info file = {.pid = stat.pid, .content = FILE_CONTENT_PID};
                                dir->dirent.size      = get_file_content(&file, content, FILE_BUFFER);

                                *dirent               = &dir->dirent;
                        }

                        result = sys_free(cast(void**, &content));
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
static int procfs_readdir_bin(void *fs_handle, DIR *dir, dirent_t **dirent)
{
        UNUSED_ARG1(fs_handle);

        if (dir->d_seek < (size_t)sys_get_programs_table_size()) {
                dir->dirent.filetype = FILE_TYPE_PROGRAM;
                dir->dirent.name     = const_cast(char*, sys_get_programs_table()[dir->d_seek].name);
                dir->dirent.size     = 0;
                *dirent              = &dir->dirent;
                dir->d_seek++;

                return ESUCC;
        } else {
                return ENOENT;
        }
}

//==============================================================================
/**
 * @brief Add file info to list
 *
 * @param fsctx                 FS context
 * @param pid                   pid number
 * @param content               file content to write in file info
 * @param object                file object (result)
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
static int add_file_to_list(struct procfs *fsctx, pid_t pid, enum file_content content, void **object)
{
        struct file_info *file;
        int result = sys_zalloc(sizeof(struct file_info), cast(void**, &file));
        if (result == ESUCC) {
                file->pid     = pid;
                file->content = content;

                result = sys_mutex_lock(fsctx->resource_mtx, 1000);
                if (result == ESUCC) {
                        if (sys_llist_push_back(fsctx->file_list, file)) {
                                *object = file;
                                result  = ESUCC;
                        } else {
                                sys_free(cast(void**, &file));
                                result = ENOMEM;
                        }

                        sys_mutex_unlock(fsctx->resource_mtx);
                }
        }

        return result;
}

//==============================================================================
/**
 * @brief Function return file content and size
 *
 * @param file          file information
 * @param buff          buffer
 * @param size          buffer size
 *
 * @return number of bytes written to buffer
 */
//==============================================================================
static size_t get_file_content(struct file_info *file, char *buff, size_t size)
{
        size_t         len = 0;
        process_stat_t stat;

        switch (file->content) {
        case FILE_CONTENT_BIN:
                /* none */
                break;

        case FILE_CONTENT_PID:
                if (sys_process_get_stat_pid(file->pid, &stat) == ESUCC) {
                        len = sys_snprintf(buff, size,
                                            "Name: %s\n"
                                            "PID: %d\n"
                                            "Memory usage: %d bytes\n"
                                            "Memory Block Count: %d\n"
                                            "Open Files: %d\n"
                                            "Open Dirs: %d\n"
                                            "Open Mutexes: %d\n"
                                            "Open Semaphores: %d\n"
                                            "Open Queues: %d\n"
                                            "Open Sockets: %d\n"
                                            "Threads: %d\n"
                                            "CPU Load: %d.%d%%\n"
                                            "Stack Size: %d\n"
                                            "Stack Usage: %d\n"
                                            "Priority: %d\n",
                                            stat.name,
                                            stat.pid,
                                            stat.memory_usage,
                                            stat.memory_block_count,
                                            stat.files_count,
                                            stat.dir_count,
                                            stat.mutexes_count,
                                            stat.semaphores_count,
                                            stat.queue_count,
                                            stat.socket_count,
                                            stat.threads_count,
                                            stat.CPU_load / 10, stat.CPU_load % 10,
                                            stat.stack_size,
                                            stat.stack_max_usage,
                                            stat.priority);
                }
                break;

        case FILE_CONTENT_CPUINFO:
                len = sys_snprintf(buff, size,
                                    "CPU name  : %s\n"
                                    "CPU vendor: %s\n",
                                    _CPUCTL_PLATFORM_NAME,
                                    _CPUCTL_VENDOR_NAME);

                FILE *pll;
                int result = sys_fopen(CLK_FILE_PATH, "r+", &pll);
                if (result == ESUCC) {
                        PLL_clk_info_t clkinf;
                        clkinf.iterator = 0;

                        while (  sys_ioctl(pll, IOCTL_PLL__GET_CLK_INFO, &clkinf) == ESUCC
                              && clkinf.clock_name) {

                                len += sys_snprintf(buff + len,
                                                     size - len,
                                                     "%16s: %d Hz\n",
                                                     clkinf.clock_name,
                                                     cast(int, clkinf.clock_Hz));
                        }

                        sys_fclose(pll);
                } else {
                        len += sys_snprintf(buff + len, size - len,
                                            "Warning: no '"CLK_FILE_PATH"' file to read clocks\n");
                }
                break;

        default:
                break;
        }

        return len;
}

/*==============================================================================
  End of file
==============================================================================*/
