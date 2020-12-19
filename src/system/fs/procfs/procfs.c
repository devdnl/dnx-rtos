/*=========================================================================*//**
@file    procfs.c

@author  Daniel Zorychta

@brief   This file support process file system (procfs)

@note    Copyright (C) 2012-2015 Daniel Zorychta <daniel.zorychta@gmail.com>

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
#include "fs/fs.h"

/*==============================================================================
  Local symbolic constants/macros
==============================================================================*/
#define CLK_FILE_PATH                   "/dev/clk"

#define PATH_ROOT                       "/"
#define PATH_ROOT_BIN                   "/bin"
#define PATH_ROOT_PID                   "/pid"
#define PATH_ROOT_CPUINFO               "/cpuinfo"

#define FILE_BUFFER                     384
#define PID_STR_LEN                     12

/*==============================================================================
  Local types, enums definitions
==============================================================================*/
struct procfs {
        llist_t *file_list;
        mutex_t *resource_mtx;
        char    *offset_path;
};

enum path_content {
        FILE_CONTENT_ROOT,
        FILE_CONTENT_BIN,
        FILE_CONTENT_PID,
        FILE_CONTENT_CPUINFO,
        _FILE_CONTENT_COUNT
};

struct file_info {
        enum path_content content;
        int16_t           arg;
};

struct dir_info {
        const char *dir_name;
        char        name[32];
};

/*==============================================================================
  Local function prototypes
==============================================================================*/
static int    procfs_readdir_root(struct procfs *hdl, DIR *dir);
static int    procfs_readdir_pid (struct procfs *hdl, DIR *dir);
static int    procfs_readdir_bin (struct procfs *hdl, DIR *dir);
static int    add_file_to_list   (struct procfs *hdl, int16_t arg, enum path_content content, void **object);
static size_t get_file_content   (struct file_info *file, u8_t *buff, size_t size, i32_t seek);
static void   buf_snprintf(u8_t *buf, size_t *size, size_t *clen, i32_t *seek, const char *fmt, ...);
static size_t get_file_size(struct file_info *file);

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
 * @param[in ]           *opts                  file system options (can be NULL)
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
API_FS_INIT(procfs, void **fs_handle, const char *src_path, const char *opts)
{
        UNUSED_ARG1(src_path);

        int err = sys_zalloc(sizeof(struct procfs), fs_handle);
        if (err == ESUCC) {
                struct procfs *procfs = *fs_handle;

                err = sys_llist_create(sys_llist_functor_cmp_pointers, NULL, &procfs->file_list);
                if (err != ESUCC)
                        goto finish;

                err = sys_mutex_create(MUTEX_TYPE_NORMAL, &procfs->resource_mtx);
                if (err != ESUCC)
                        goto finish;

                if (!isstrempty(opts)) {

                        if (!(  isstreq(opts, PATH_ROOT)
                             || isstreq(opts, PATH_ROOT_BIN)
                             || isstreq(opts, PATH_ROOT_PID)) ) {

                                err = ENOENT;
                                goto finish;
                        }

                        err = sys_zalloc(strsize(opts), cast(void*, &procfs->offset_path));
                        if (err) {
                                goto finish;
                        } else {
                                strcpy(procfs->offset_path, opts);

                                if (LAST_CHARACTER(procfs->offset_path) == '/') {
                                        LAST_CHARACTER(procfs->offset_path) = '\0';
                                }
                        }
                }

                finish:
                if (err != ESUCC) {
                        if (procfs->file_list)
                                sys_llist_destroy(procfs->file_list);

                        if (procfs->resource_mtx)
                                sys_mutex_destroy(procfs->resource_mtx);

                        if (procfs->offset_path)
                                sys_free(cast(void*, &procfs->offset_path));

                        sys_free(fs_handle);
                }
        }

        return err;
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

        int err = sys_mutex_lock(procfs->resource_mtx, 100);
        if (!err) {
                if (sys_llist_size(procfs->file_list) != 0) {
                        sys_mutex_unlock(procfs->resource_mtx);
                        err = EBUSY;
                } else {
                        sys_llist_destroy(procfs->file_list);
                        mutex_t *mtx = procfs->resource_mtx;
                        sys_free(cast(void*, &procfs->offset_path));
                        memset(procfs, 0, sizeof(struct procfs));
                        sys_mutex_unlock(mtx);
                        sys_mutex_destroy(mtx);
                        sys_free(&fs_handle);
                }
        }

        return err;
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
        struct procfs *hdl = fs_handle;

        if (flags != O_RDONLY) {
                return EROFS;
        }

        int err = ENOENT;

        *fpos = 0;

        // create path with offset
        char *opath = const_cast(char*, path);

        if (hdl->offset_path) {
                err = sys_malloc(strlen(hdl->offset_path) + strsize(path),
                                 cast(void*, &opath));

                if (!err) {
                        strcpy(opath, hdl->offset_path);
                        strcat(opath, path);
                } else {
                        return err;
                }
        }

        // pointer for modification
        char *mpath = opath;

        err = ENOENT;

        // "/pid" path
        if (isstreq(mpath, PATH_ROOT_PID)) {
                err = add_file_to_list(hdl, -1, FILE_CONTENT_PID, fhdl);

        // "/pid/<pid>" path
        } else if (isstreqn(mpath, PATH_ROOT_PID"/", strlen(PATH_ROOT_PID) + 1)) {
                mpath += strlen(PATH_ROOT_PID) + 1;

                i32_t pid = 0;
                sys_strtoi(mpath, 10, &pid);

                process_stat_t stat;
                if (sys_process_get_stat_pid(pid, &stat) == ESUCC) {
                        err = add_file_to_list(hdl, pid, FILE_CONTENT_PID, fhdl);
                } else {
                        err = ENOENT;
                }

        // "/bin" path
        } else if (isstreq(mpath, PATH_ROOT_BIN)) {
                err = add_file_to_list(hdl, -1, FILE_CONTENT_BIN, fhdl);

        // "/bin/<prog>" path
        } else if (isstreqn(mpath, PATH_ROOT_BIN"/", strlen(PATH_ROOT_BIN) + 1)) {
                mpath += strlen(PATH_ROOT_BIN) + 1;

                int n = sys_get_programs_table_size();
                for (int i = 0; i < n; i++) {
                        if (isstreq(mpath, sys_get_programs_table()[i].name)) {
                                err = add_file_to_list(hdl, i, FILE_CONTENT_BIN, fhdl);
                                break;
                        }
                }

        // "/cpuinfo" path
        } else if (isstreq(mpath, PATH_ROOT_CPUINFO)) {
                err = add_file_to_list(hdl, 0, FILE_CONTENT_CPUINFO, fhdl);

        } else {
                err = ENOENT;
        }

        if (opath != path) {
                sys_free(cast(void*, &opath));
        }

        return err;
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

        int err = sys_mutex_lock(fsctx->resource_mtx, MAX_DELAY_MS);
        if (!err) {
                int pos = sys_llist_find_begin(fsctx->file_list, fhdl);
                err = sys_llist_erase(fsctx->file_list, pos) ? ESUCC : ENOENT;

                sys_mutex_unlock(fsctx->resource_mtx);
        }

        return err;
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

        struct file_info *file = fhdl;
        int               err  = ENOENT;

        if (file && file->content < _FILE_CONTENT_COUNT) {
                *rdcnt = get_file_content(file, dst, count, *fpos);
                err = 0;
        }

        return err;
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

        struct file_info *file = fhdl;

        stat->st_dev   = 0;
        stat->st_mode  = S_IRUSR | S_IRGRP | S_IROTH;
        stat->st_mtime = COMPILE_EPOCH_TIME;
        stat->st_ctime = COMPILE_EPOCH_TIME;
        stat->st_size  = 0;
        stat->st_gid   = 0;
        stat->st_uid   = 0;

        int err = EINVAL;

        if (file->content < _FILE_CONTENT_COUNT) {

                if (file->arg >= 0) {
                        stat->st_size  = get_file_size(file);
                        stat->st_mode |= S_IFREG;

                        if (  (file->content == FILE_CONTENT_PID)
                           || (file->content == FILE_CONTENT_CPUINFO) ) {

                                time_t t = 0;
                                sys_gettime(&t);

                                stat->st_mtime = t;
                                stat->st_ctime = t;
                        }

                        if (file->content == FILE_CONTENT_BIN) {
                                stat->st_mode |= S_IFPROG;
                                stat->st_mode |= S_IXUSR;
                        }
                } else {
                        stat->st_mode |= S_IFDIR;
                }

                err = ESUCC;
        }

        return err;
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
        struct procfs *hdl = fs_handle;
        int err;

        // create path with offset
        char *opath = const_cast(char*, path);

        if (hdl->offset_path) {
                err = sys_malloc(strlen(hdl->offset_path) + strsize(path),
                                 cast(void*, &opath));

                if (!err) {
                        strcpy(opath, hdl->offset_path);
                        strcat(opath, path);
                } else {
                        return err;
                }
        }

        dir->d_seek = 0;

        err = sys_zalloc(sizeof(struct dir_info), &dir->d_hdl);
        if (!err) {
                struct dir_info *dirinfo = dir->d_hdl;

                if (isstreq(opath, PATH_ROOT)) {
                        dirinfo->dir_name = PATH_ROOT;
                        dir->d_items      = 3;

                } else if (isstreq(opath, PATH_ROOT_PID"/")) {
                        dirinfo->dir_name = PATH_ROOT_PID;
                        dir->d_items      = sys_process_get_count();

                } else if (isstreq(opath, PATH_ROOT_BIN"/")) {
                        dirinfo->dir_name = PATH_ROOT_BIN;
                        dir->d_items      = sys_get_programs_table_size();

                } else {
                        sys_free(&dir->d_hdl);
                        err = ENOENT;
                }
        }

        if (opath != path) {
                sys_free(cast(void*, &opath));
        }

        return err;
}

//==============================================================================
/**
 * @brief Close directory
 *
 * @param[in ]          *fs_handle              file system allocated memory
 * @param[in ]          *dir                    directory object
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
API_FS_CLOSEDIR(procfs, void *fs_handle, DIR *dir)
{
        UNUSED_ARG1(fs_handle);

        if (dir->d_hdl) {
                return sys_free(&dir->d_hdl);
        } else {
                return ESUCC;
        }
}

//==============================================================================
/**
 * @brief Read directory
 *
 * @param[in ]          *fs_handle              file system allocated memory
 * @param[in,out]       *dir                    directory object
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
API_FS_READDIR(procfs, void *fs_handle, DIR *dir)
{
        int err = EFAULT;

        struct dir_info *dirinfo = dir->d_hdl;
        if (dirinfo) {
                if (isstreq(dirinfo->dir_name, PATH_ROOT)) {
                        err = procfs_readdir_root(fs_handle, dir);

                } else if (isstreq(dirinfo->dir_name, PATH_ROOT_PID)) {
                        err = procfs_readdir_pid(fs_handle, dir);

                } else if (isstreq(dirinfo->dir_name, PATH_ROOT_BIN)) {
                        err = procfs_readdir_bin(fs_handle, dir);
                }
        }

        return err;
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
        statfs->f_type   = SYS_FS_TYPE__SYS;
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
 * @brief Read directory
 *
 * @param[in ]          *hdl                    file system allocated memory
 * @param[in,out]       *dir                    directory object
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
static int procfs_readdir_root(struct procfs *hdl, DIR *dir)
{
        UNUSED_ARG1(hdl);

        int err = ESUCC;

        dir->dirent.dev  = 0;
        dir->dirent.size = 0;

        switch (dir->d_seek++) {
        case 0:
                dir->dirent.d_name = "bin";
                dir->dirent.mode   = S_IRUSR | S_IRGRP | S_IROTH | S_IFDIR;
                break;

        case 1:
                dir->dirent.d_name = "pid";
                dir->dirent.mode   = S_IRUSR | S_IRGRP | S_IROTH | S_IFDIR;
                break;

        case 2: {
                struct file_info file = {.content = FILE_CONTENT_CPUINFO, .arg = 0};
                dir->dirent.d_name = "cpuinfo";
                dir->dirent.mode   = S_IRUSR | S_IRGRP | S_IROTH | S_IFREG;
                dir->dirent.size   = get_file_size(&file);
                break;
        }

        default:
                err = ENOENT;
                break;
        }

        return err;
}

//==============================================================================
/**
 * @brief Read directory
 *
 * @param[in ]          *hdl                    file system allocated memory
 * @param[in,out]       *dir                    directory object
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
static int procfs_readdir_pid(struct procfs *hdl, DIR *dir)
{
        UNUSED_ARG1(hdl);

        process_stat_t stat;
        int err = sys_process_get_stat_seek(dir->d_seek++, &stat);
        if (!err) {

                struct dir_info *dirinfo = dir->d_hdl;

                sys_snprintf(dirinfo->name, sizeof(dirinfo->name),
                             "%u", stat.pid);

                dir->dirent.d_name = dirinfo->name;
                dir->dirent.mode   = S_IRUSR | S_IRGRP | S_IROTH | S_IFREG;
                dir->dirent.dev    = 0;

                struct file_info file = {.arg = stat.pid, .content = FILE_CONTENT_PID};
                dir->dirent.size      = get_file_size(&file);
        }

        return err;
}

//==============================================================================
/**
 * @brief Read directory
 *
 * @param[in ]          *hdl                    file system allocated memory
 * @param[in,out]       *dir                    directory object
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
static int procfs_readdir_bin(struct procfs *hdl, DIR *dir)
{
        UNUSED_ARG1(hdl);

        int err = ENOENT;

        if (dir->d_seek < (size_t)sys_get_programs_table_size()) {

                dir->dirent.d_name = sys_get_programs_table()[dir->d_seek].name;
                dir->dirent.mode   = S_IRUSR | S_IRGRP | S_IROTH | S_IXUSR | S_IFPROG;

                struct file_info file = {.arg = dir->d_seek, .content = FILE_CONTENT_BIN};
                dir->dirent.size      = get_file_size(&file);

                dir->d_seek++;

                err = ESUCC;
        }

        return err;
}

//==============================================================================
/**
 * @brief Add file info to list
 *
 * @param hdl                   FS context
 * @param arg                   additional argument
 * @param content               file content to write in file info
 * @param object                file object (result)
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
static int add_file_to_list(struct procfs *hdl, int16_t arg,
                            enum path_content content, void **object)
{
        struct file_info *file;
        int err = sys_zalloc(sizeof(struct file_info), cast(void**, &file));
        if (!err) {
                file->content = content;
                file->arg     = arg;

                err = sys_mutex_lock(hdl->resource_mtx, MAX_DELAY_MS);
                if (!err) {
                        if (sys_llist_push_back(hdl->file_list, file)) {
                                *object = file;
                        } else {
                                sys_free(cast(void**, &file));
                                err = ENOMEM;
                        }

                        sys_mutex_unlock(hdl->resource_mtx);
                }
        }

        return err;
}

//==============================================================================
/**
 * @brief  Return file size
 *
 * @param  ?
 *
 * @return ?
 */
//==============================================================================
static size_t get_file_size(struct file_info *file)
{
        size_t size = UINT16_MAX;
        return get_file_content(file, NULL, size, 0);
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
static size_t get_file_content(struct file_info *file, u8_t *buff, size_t size, i32_t seek)
{
        size_t clen = 0;

        switch (file->content) {
        case FILE_CONTENT_PID: {
                process_stat_t stat;
                if (sys_process_get_stat_pid(file->arg, &stat) == ESUCC) {

                        if (size) buf_snprintf(buff, &size, &clen, &seek, "Name: %s\n", stat.name);
                        if (size) buf_snprintf(buff, &size, &clen, &seek, "PID: %d\n", stat.pid);
                        if (size) buf_snprintf(buff, &size, &clen, &seek, "Memory usage: %d bytes\n", stat.memory_usage);
                        if (size) buf_snprintf(buff, &size, &clen, &seek, "Memory Block Count: %d\n", stat.memory_block_count);
                        if (size) buf_snprintf(buff, &size, &clen, &seek, "Open Files: %d\n", stat.files_count);
                        if (size) buf_snprintf(buff, &size, &clen, &seek, "Open Dirs: %d\n", stat.dir_count);
                        if (size) buf_snprintf(buff, &size, &clen, &seek, "Open Mutexes: %d\n", stat.mutexes_count);
                        if (size) buf_snprintf(buff, &size, &clen, &seek, "Open Semaphores: %d\n", stat.semaphores_count);
                        if (size) buf_snprintf(buff, &size, &clen, &seek, "Open Queues: %d\n", stat.queue_count);
                        if (size) buf_snprintf(buff, &size, &clen, &seek, "Open Sockets: %d\n", stat.socket_count);
                        if (size) buf_snprintf(buff, &size, &clen, &seek, "Threads: %d\n", stat.threads_count);
                        if (size) buf_snprintf(buff, &size, &clen, &seek, "CPU Load: %d.%d%%\n", stat.CPU_load / 10, stat.CPU_load % 10);
                        if (size) buf_snprintf(buff, &size, &clen, &seek, "Stack Size: %d\n", stat.stack_size);
                        if (size) buf_snprintf(buff, &size, &clen, &seek, "Stack Usage: %d\n", stat.stack_max_usage);
                        if (size) buf_snprintf(buff, &size, &clen, &seek, "Priority: %d\n", stat.priority);
                        if (size) buf_snprintf(buff, &size, &clen, &seek, "Syscalls/s: %u\n", stat.syscalls);
                }
                break;
        }

        case FILE_CONTENT_CPUINFO:
                buf_snprintf(buff, &size,&clen, &seek,
                             "CPU name  : %s\n"
                             "CPU vendor: %s\n",
                             _CPUCTL_PLATFORM_NAME,
                             _CPUCTL_VENDOR_NAME);

                FILE *pll;
                if (sys_fopen(CLK_FILE_PATH, "r+", &pll) == ESUCC) {

                        CLK_info_t clkinf;
                        clkinf.iterator = 0;

                        while ( size > 0
                              && sys_ioctl(pll, IOCTL_CLK__GET_CLK_INFO, &clkinf) == ESUCC
                              && clkinf.name) {

                                buf_snprintf(buff, &size, &clen, &seek,
                                             "%16s: %d Hz\n",
                                             clkinf.name,
                                             cast(int, clkinf.freq_Hz));
                        }

                        sys_fclose(pll);
                } else {
                        buf_snprintf(buff, &size, &clen, &seek,
                                     "Warning: no '"CLK_FILE_PATH"' file to read clocks\n");
                }
                break;

#if __OS_SYSTEM_SHEBANG_ENABLE__ > 0
        case FILE_CONTENT_BIN: {
                const struct _prog_data *pdata = sys_get_programs_table();
                if (file->arg < sys_get_programs_table_size()) {

                        buf_snprintf(buff, &size, &clen, &seek,
                                     "#!%s\n", pdata[file->arg].name);
                }
                break;
        }
#endif

        default:
                break;
        }

        return clen;
}

//==============================================================================
/**
 * @brief  Function fill buffer with text according to file position and buffer size.
 *
 * @param  buf          destination buffer (can be null)
 * @param  size         destination buffer size
 * @param  clen         content length (output)
 * @param  seek         file position
 * @param  fmt          printf() format
 * @param  ...          printf() arguments
 */
//==============================================================================
static void buf_snprintf(u8_t *buf, size_t *size, size_t *clen, i32_t *seek, const char *fmt, ...)
{
        char line[128];

        va_list arg;
        va_start(arg, fmt);
        i32_t len = sys_vsnprintf(line, sizeof(line), fmt, arg);
        va_end(arg);

        if (len - *seek > 0) {
                len -= *seek;
                len = min(len, (i32_t)*size);

                if (buf) {
                        memcpy(buf + *clen, line + *seek, len);
                }

                *clen += len;
                *size -= len;
                *seek  = 0;
        } else {
                *seek -= min(*seek, len);
        }
}

/*==============================================================================
  End of file
==============================================================================*/
