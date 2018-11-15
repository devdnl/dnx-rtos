/*=========================================================================*//**
@file    ramfs.c

@author  Daniel Zorychta

@brief   This file support RAM file system (grows dynamically in RAM)

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
#define MTX_TIMEOUT                     60000
#define PIPE_LENGTH                     __OS_STREAM_BUFFER_LENGTH__
#define PIPE_WRITE_TIMEOUT              1
#define PIPE_READ_TIMEOUT               MAX_DELAY
#define DATA_CHAIN_SIZE                 __RAMFS_FILE_CHAIN_SIZE__

/*==============================================================================
  Local types, enums definitions
==============================================================================*/
/** regular file data chain */
typedef struct data_chain {
        struct data_chain *next;
        u8_t buf[DATA_CHAIN_SIZE];
} data_chain_t;

/** node structure */
typedef struct node {
        char            *name;                  //!< file name
        mode_t           mode;                  //!< protection
        uid_t            uid;                   //!< user ID of owner
        gid_t            gid;                   //!< group ID of owner
        size_t           size;                  //!< file size
        time_t           mtime;                 //!< time of last modification
        time_t           ctime;                 //!< time of creation

        union {
                pipe_t       *pipe_t;
                llist_t      *llist_t;
                data_chain_t *data_chain_t;
                dev_t         dev_t;
        } data;
} node_t;

/** info of opened file */
struct opened_file_info {
        node_t          *child;                 //!< opened node
        node_t          *parent;                //!< base of opened node
        bool             remove_at_close;       //!< file to remove after close
};

/** main memory structure */
struct RAMFS {
        node_t           root_dir;              //!< root dir '/'
        mutex_t         *resource_mtx;          //!< lock mutex
        llist_t         *opended_files;         //!< list with opened files
        size_t           file_count;            //!< number of files
};

/*==============================================================================
  Local function prototypes
==============================================================================*/
static int  new_node                    (struct RAMFS *hdl, node_t *parent, char *filename, mode_t mode, i32_t *item, node_t **child);
static int  delete_node                 (struct RAMFS *hdl, node_t *base, node_t *target, u32_t position);
static int  get_node                    (const char *path, node_t *startnode, i32_t deep, i32_t *item, node_t **node);
static uint get_path_deep               (const char *path);
static int  add_node_to_open_files_list (struct RAMFS *hdl, node_t *parent, node_t *child);
static void clear_regular_file          (node_t *node);
static int  write_regular_file          (node_t *node, const u8_t *src, size_t count, fpos_t fpos, size_t *wrcnt);
static int  read_regular_file           (node_t *node, u8_t *dst, size_t count, fpos_t fpos, size_t *rdcnt);

/*==============================================================================
  Local object definitions
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
API_FS_INIT(ramfs, void **fs_handle, const char *src_path, const char *opts)
{
        UNUSED_ARG2(src_path, opts);

        int err = sys_zalloc(sizeof(struct RAMFS), fs_handle);
        if (!err) {
                struct RAMFS *hdl = *fs_handle;

                err = sys_mutex_create(MUTEX_TYPE_RECURSIVE, &hdl->resource_mtx);
                if (err)
                        goto finish;

                err = sys_llist_create(NULL, NULL, cast(llist_t**, &hdl->root_dir.data.llist_t));
                if (err)
                        goto finish;

                err = sys_llist_create(sys_llist_functor_cmp_pointers, NULL, &hdl->opended_files);
                if (err)
                        goto finish;

                hdl->root_dir.name = "/";
                hdl->root_dir.size = sizeof(node_t);
                hdl->root_dir.mode = (S_IRWXU | S_IRWXG | S_IRWXO) | S_IFDIR;

                finish:
                if (err) {
                        if (hdl->resource_mtx)
                                sys_mutex_destroy(hdl->resource_mtx);

                        if (hdl->root_dir.data.llist_t)
                                sys_llist_destroy(hdl->root_dir.data.llist_t);

                        if (hdl->opended_files)
                                sys_llist_destroy(hdl->opended_files);

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
API_FS_RELEASE(ramfs, void *fs_handle)
{
        UNUSED_ARG1(fs_handle);

        /*
         * Here the LFS should delete all lists and free all allocated buffers.
         * If will be necessary should be implemented in the future.
         */

        return ENOTSUP;
}

//==============================================================================
/**
 * @brief Create node for driver file
 *
 * @param[in ]          *fs_handle              file system allocated memory
 * @param[in ]          *path                   name of created node
 * @param[in ]           dev                    driver id
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
API_FS_MKNOD(ramfs, void *fs_handle, const char *path, const dev_t dev)
{
        struct RAMFS *hdl = fs_handle;

        int err = sys_mutex_lock(hdl->resource_mtx, MTX_TIMEOUT);
        if (!err) {

                // parent node must exist
                node_t *parent;
                err = get_node(path, &hdl->root_dir, -1, NULL, &parent);
                if (!err) {
                        // create new node
                        char *basename = strrchr(path, '/') + 1;

                        char *child_name;
                        err = sys_zalloc(strsize(basename), cast(void**, &child_name));
                        if (!err) {
                                strcpy(child_name, basename);

                                node_t *child;
                                err = new_node(hdl, parent, child_name,
                                               S_IRWXU | S_IRGRP | S_IROTH | S_IFDEV,
                                               NULL, &child);
                                if (!err) {
                                        child->data.dev_t = dev;
                                } else {
                                        sys_free(cast(void**, &child_name));
                                }
                        }
                }

                sys_mutex_unlock(hdl->resource_mtx);
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
API_FS_MKDIR(ramfs, void *fs_handle, const char *path, mode_t mode)
{
        struct RAMFS *hdl = fs_handle;

        int err = sys_mutex_lock(hdl->resource_mtx, MTX_TIMEOUT);
        if (!err) {

                // parent node must exist
                node_t *parent;
                err = get_node(path, &hdl->root_dir, -1, NULL, &parent);
                if (!err) {
                        // create new node
                        char *basename = strrchr(path, '/') + 1;

                        char *child_name;
                        err = sys_zalloc(strsize(basename), cast(void**, &child_name));
                        if (!err) {
                                strcpy(child_name, basename);

                                node_t *child;
                                err = new_node(hdl, parent, child_name,
                                               S_IPMT(mode) | S_IFDIR,
                                               NULL, &child);
                                if (err) {
                                        sys_free(cast(void**, &child_name));
                                }
                        }
                }

                sys_mutex_unlock(hdl->resource_mtx);
        }

        return err;
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
API_FS_MKFIFO(ramfs, void *fs_handle, const char *path, mode_t mode)
{
        struct RAMFS *hdl = fs_handle;

        int err = sys_mutex_lock(hdl->resource_mtx, MTX_TIMEOUT);
        if (!err) {

                // parent node must exist
                node_t *parent;
                err = get_node(path, &hdl->root_dir, -1, NULL, &parent);
                if (!err) {
                        // create new node
                        char *basename = strrchr(path, '/') + 1;

                        char *child_name;
                        err = sys_zalloc(strsize(basename), cast(void**, &child_name));
                        if (!err) {
                                strcpy(child_name, basename);

                                node_t *child;
                                err = new_node(hdl, parent, child_name,
                                               S_IPMT(mode) | S_IFIFO,
                                               NULL, &child);
                                if (err) {
                                        sys_free(cast(void**, &child_name));
                                }
                        }
                }

                sys_mutex_unlock(hdl->resource_mtx);
        }

        return err;
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
API_FS_OPENDIR(ramfs, void *fs_handle, const char *path, DIR *dir)
{
        struct RAMFS *hdl = fs_handle;

        int err = sys_mutex_lock(hdl->resource_mtx, MTX_TIMEOUT);
        if (!err) {

                node_t *parent;
                err = get_node(path, &hdl->root_dir, 0, NULL, &parent);
                if (!err) {
                        if (S_ISDIR(parent->mode)) {
                                dir->d_items    = sys_llist_size(parent->data.llist_t);
                                dir->d_seek     = 0;
                                dir->d_hdl      = parent;
                        } else {
                                err = ENOTDIR;
                        }
                }

                sys_mutex_unlock(hdl->resource_mtx);
        }

        return err;
}

//==============================================================================
/**
 * @brief Function close dir
 *
 * @param[in] *fs_handle        FS handle
 * @param[in] *dir              directory info
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
API_FS_CLOSEDIR(ramfs, void *fs_handle, DIR *dir)
{
        UNUSED_ARG2(fs_handle, dir);
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
API_FS_READDIR(ramfs, void *fs_handle, DIR *dir)
{
        struct RAMFS *hdl = fs_handle;

        int err = sys_mutex_lock(hdl->resource_mtx, MTX_TIMEOUT);
        if (!err) {

                node_t *parent = dir->d_hdl;
                node_t *child  = sys_llist_at(parent->data.llist_t, dir->d_seek++);

                if (child) {
                        dir->dirent.d_name = child->name;
                        dir->dirent.mode   = child->mode;
                        dir->dirent.size   = child->size;

                        if (S_ISDEV(child->mode)) {
                                sys_mutex_unlock(hdl->resource_mtx);

                                struct vfs_dev_stat dev_stat;
                                err = sys_driver_stat(child->data.dev_t, &dev_stat);
                                if (!err) {
                                        dir->dirent.size = dev_stat.st_size;
                                } else if (err == ENODEV) {
                                        dir->dirent.size = 0;
                                        err = ESUCC;
                                }

                                dir->dirent.dev = child->data.dev_t;

                                return err;
                        } else {
                                err = ESUCC;
                        }
                } else {
                        err = ENOENT;
                }

                sys_mutex_unlock(hdl->resource_mtx);
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
API_FS_REMOVE(ramfs, void *fs_handle, const char *path)
{
        struct RAMFS *hdl = fs_handle;

        int err = sys_mutex_lock(hdl->resource_mtx, MTX_TIMEOUT);
        if (!err) {

                node_t *parent;
                err = get_node(path, &hdl->root_dir, -1, NULL, &parent);
                if (err){
                        goto finish;
                }

                i32_t item; node_t *child;
                err = get_node(path, &hdl->root_dir, 0, &item, &child);
                if (err) {
                        goto finish;
                }

                if (child == &hdl->root_dir) {
                        err = EPERM;
                        goto finish;
                }

                /* if path is ending on slash, the object must be DIR */
                if (strlch(path) == '/' && !S_ISDIR(child->mode)) {
                        err = ENOTDIR;
                        goto finish;
                }

                bool remove_file = true;

                /* check if file is opened */
                if (!S_ISDIR(child->mode)) {
                        sys_llist_foreach(struct opened_file_info*, opened_file, hdl->opended_files) {
                                if (opened_file->child == child) {
                                        opened_file->remove_at_close = true;
                                        remove_file = false;
                                }
                        }
                }

                /* remove node if possible */
                if (remove_file == true) {
                        err = delete_node(hdl, parent, child, item);
                } else {
                        err = ESUCC;
                }

                finish:
                sys_mutex_unlock(hdl->resource_mtx);
        }

        return err;
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
API_FS_RENAME(ramfs, void *fs_handle, const char *old_name, const char *new_name)
{
        struct RAMFS *hdl = fs_handle;

        if (strlch(old_name) == '/' || strlch(new_name) == '/') {
                return EISDIR;
        }

        int err = sys_mutex_lock(hdl->resource_mtx, MTX_TIMEOUT);
        if (!err) {

                node_t *target;
                err = get_node(old_name, &hdl->root_dir, 0, NULL, &target);
                if (!err) {
                        char *basename = strrchr(new_name, '/') + 1;

                        char *newname;
                        err = sys_zalloc(strsize(basename), cast(void**, &newname));
                        if (!err) {
                                strcpy(newname, basename);

                                if (target->name) {
                                        sys_free(cast(void**, target->name));
                                }

                                target->name = newname;
                        }
                }

                sys_mutex_unlock(hdl->resource_mtx);
        }

        return err;
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
API_FS_CHMOD(ramfs, void *fs_handle, const char *path, mode_t mode)
{
        struct RAMFS *hdl = fs_handle;

        int err = sys_mutex_lock(hdl->resource_mtx, MTX_TIMEOUT);
        if (!err) {

                node_t *target;
                err = get_node(path, &hdl->root_dir, 0, NULL, &target);
                if (!err) {
                        target->mode = S_IFMT(target->mode) | S_IPMT(mode);
                }

                sys_mutex_unlock(hdl->resource_mtx);
        }

        return err;
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
API_FS_CHOWN(ramfs, void *fs_handle, const char *path, uid_t owner, gid_t group)
{
        struct RAMFS *hdl = fs_handle;

        int err = sys_mutex_lock(hdl->resource_mtx, MTX_TIMEOUT);
        if (!err) {

                node_t *target;
                err = get_node(path, &hdl->root_dir, 0, NULL, &target);
                if (!err) {
                        target->uid = owner;
                        target->gid = group;
                }

                sys_mutex_unlock(hdl->resource_mtx);
        }

        return err;
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
API_FS_FSTAT(ramfs, void *fs_handle, void *extra, struct stat *stat)
{
        UNUSED_ARG1(fs_handle);

        int err = ENOENT;

        struct opened_file_info *opened_file = extra;
        if (opened_file && opened_file->child) {

                stat->st_gid   = opened_file->child->gid;
                stat->st_mode  = opened_file->child->mode;
                stat->st_mtime = opened_file->child->mtime;
                stat->st_ctime = opened_file->child->ctime;
                stat->st_size  = opened_file->child->size;
                stat->st_uid   = opened_file->child->uid;

                if (S_ISDEV(opened_file->child->mode)) {

                        stat->st_dev = opened_file->child->data.dev_t;

                        struct vfs_dev_stat dev_stat;
                        err = sys_driver_stat(opened_file->child->data.dev_t,
                                              &dev_stat);
                        if (!err) {
                                stat->st_size = dev_stat.st_size;

                        } else if (err == ENODEV) {
                                stat->st_size = 0;
                                err = ESUCC;
                        }

                } else {
                        stat->st_dev = 0;
                        err          = ESUCC;
                }
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
API_FS_STAT(ramfs, void *fs_handle, const char *path, struct stat *stat)
{
        struct RAMFS *hdl = fs_handle;

        int err = sys_mutex_lock(hdl->resource_mtx, MTX_TIMEOUT);
        if (!err) {

                node_t *target;
                err = get_node(path, &hdl->root_dir, 0, NULL, &target);
                if (!err) {
                        if ( (strlch(path) == '/' && S_ISDIR(target->mode))
                           || strlch(path) != '/') {

                                struct opened_file_info file;
                                file.child           = target;
                                file.parent          = NULL;
                                file.remove_at_close = false;

                                err = _ramfs_fstat(fs_handle, &file, stat);
                        } else {
                                err = EINVAL;
                        }
                }

                sys_mutex_unlock(hdl->resource_mtx);
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
API_FS_STATFS(ramfs, void *fs_handle, struct statfs *statfs)
{
        struct RAMFS *hdl = fs_handle;

        statfs->f_bfree  = cast(u32_t, sys_get_free_mem());
        statfs->f_blocks = cast(u32_t, sys_get_mem_size());
        statfs->f_bsize  = 1;
        statfs->f_ffree  = cast(u32_t, sys_get_free_mem() / sizeof(node_t));
        statfs->f_files  = hdl->file_count;
        statfs->f_type   = SYS_FS_TYPE__RAM;
        statfs->f_fsname = "ramfs";

        return ESUCC;
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
API_FS_OPEN(ramfs, void *fs_handle, void **fhdl, fpos_t *fpos, const char *path, u32_t flags)
{
        struct RAMFS *hdl = fs_handle;

        int err = sys_mutex_lock(hdl->resource_mtx, MTX_TIMEOUT);
        if (!err) {

                // open file parent
                node_t *parent;
                err = get_node(path, &hdl->root_dir, -1, NULL, &parent);
                if (err) {
                        goto finish;
                }

                // try to open selected file, if not exist then try create if O_CREAT flag is set
                i32_t item; node_t *child;
                err = get_node(path, &hdl->root_dir, 0, &item, &child);
                if (err == ENOENT) {
                        // check that file should be created
                        if (!(flags & O_CREAT)) {
                                goto finish;
                        }

                        char *basename = strrchr(path, '/') + 1;

                        char *file_name;
                        err = sys_zalloc(strsize(basename), cast(void**, &file_name));
                        if (err) {
                                goto finish;
                        }

                        strcpy(file_name, basename);

                        err = new_node(hdl, parent, file_name, 0666 | S_IFREG,
                                       &item, &child);
                        if (err) {
                                sys_free(cast(void**, &file_name));
                                goto finish;
                        }
                } else {
                        if (S_ISDIR(child->mode)) {
                                err = EISDIR;
                                goto finish;
                        }
                }

                /* add file to list of open files */
                err = add_node_to_open_files_list(hdl, parent, child);
                if (err) {
                        goto finish;
                }

                // load pointer to file descriptor
                *fhdl = sys_llist_back(hdl->opended_files);

                /* set file parameters */
                if (S_ISREG(child->mode)) {
                        // truncate file if requested
                        if (flags & O_TRUNC) {
                                clear_regular_file(child);
                        }

                        if (flags & O_APPEND) {
                                *fpos = child->size;
                        } else {
                                *fpos = 0;
                        }

                } else if (S_ISDEV(child->mode)) {
                        err = sys_driver_open(child->data.dev_t, flags);
                        if (!err) {
                                *fpos = 0;
                        } else {
                                int pos = sys_llist_find_end(hdl->opended_files, *fhdl);
                                sys_llist_erase(hdl->opended_files, pos);
                        }
                }

                finish:
                sys_mutex_unlock(hdl->resource_mtx);
        }

        return err;
}

//==============================================================================
/**
 * @brief Close file
 *
 * @param[in ]          *fs_handle              file system allocated memory
 * @param[in ]          *fhdl                   file handle
 * @param[in ]           force                  force close
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
API_FS_CLOSE(ramfs, void *fs_handle, void *fhdl, bool force)
{
        struct RAMFS *hdl = fs_handle;

        int err = sys_mutex_lock(hdl->resource_mtx, MTX_TIMEOUT);
        if (!err) {

                struct opened_file_info *opened_file = fhdl;
                int pos = sys_llist_find_begin(hdl->opended_files, opened_file);

                if (opened_file && opened_file->child && pos >= 0) {
                        node_t *target = opened_file->child;

                        /* close device if file is driver type */
                        if (S_ISDEV(target->mode)) {
                                err = sys_driver_close(target->data.dev_t, force);

                        } else if (S_ISFIFO(target->mode)) {
                                err = sys_pipe_close(target->data.pipe_t);

                        } else {
                                /* file to remove, check if other task does not opens this file */
                                if (opened_file->remove_at_close) {
                                        bool remove = true;

                                        sys_llist_foreach(struct opened_file_info*, file, hdl->opended_files) {
                                                if (file->child == target) {
                                                        remove = false;
                                                        break;
                                                }
                                        }

                                        if (remove) {
                                                err = delete_node(hdl,
                                                                  opened_file->parent,
                                                                  opened_file->child,
                                                                  pos);
                                        }
                                } else {
                                        err = ESUCC;
                                }
                        }

                        /* remove file from list */
                        if (!err) {
                                sys_llist_erase(hdl->opended_files, pos);
                        }
                } else {
                        err = ENOENT;
                }

                sys_mutex_unlock(hdl->resource_mtx);
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
API_FS_WRITE(ramfs,
             void            *fs_handle,
             void            *fhdl,
             const u8_t      *src,
             size_t           count,
             fpos_t          *fpos,
             size_t          *wrcnt,
             struct vfs_fattr fattr)
{
        struct RAMFS *hdl = fs_handle;

        int err = sys_mutex_lock(hdl->resource_mtx, MTX_TIMEOUT);
        if (!err) {

                err = ENOENT;

                struct opened_file_info *opened_file = fhdl;
                if (opened_file && opened_file->child) {
                        node_t *node = opened_file->child;

                        sys_gettime(&node->mtime);

                        if (S_ISDEV(node->mode)) {
                                sys_mutex_unlock(hdl->resource_mtx);
                                return sys_driver_write(node->data.dev_t, src,
                                                        count, fpos, wrcnt, fattr);

                        } else if (S_ISFIFO(node->mode)) {
                               sys_mutex_unlock(hdl->resource_mtx);

                               err = sys_pipe_write(node->data.pipe_t, src,
                                                    count, wrcnt, fattr.non_blocking_wr);
                               if (!err) {
                                       if (*wrcnt > 0) {
                                               size_t size = 0;
                                               err = sys_pipe_get_length(node->data.pipe_t,
                                                                         &size);
                                               node->size = size;
                                       }
                               }

                               return err;

                       } else if (S_ISREG(node->mode)) {
                               err = write_regular_file(node, src, count, *fpos, wrcnt);
                        }
                }

                sys_mutex_unlock(hdl->resource_mtx);
        }

        return err;
}

//==============================================================================
/**
 * @brief Read data from file
 *
 * @param[in ]          *fs_handle              file system allocated memory
 * @param[in ]          *fhdl                   file handle
 * @param[out]          *dst                    data destination
 * @param[in ]           count                  number of bytes to read
 * @param[in ]          *fpos                   position in file
 * @param[out]          *rdcnt                  number of read bytes
 * @param[in ]           fattr                  file attributes
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
API_FS_READ(ramfs,
            void            *fs_handle,
            void            *fhdl,
            u8_t            *dst,
            size_t           count,
            fpos_t          *fpos,
            size_t          *rdcnt,
            struct vfs_fattr fattr)
{
        struct RAMFS *hdl = fs_handle;

        int err = sys_mutex_lock(hdl->resource_mtx, MTX_TIMEOUT);
        if (!err) {

                err = ENOENT;

                struct opened_file_info *opened_file = fhdl;
                if (opened_file && opened_file->child) {
                        node_t *node = opened_file->child;

                        if (S_ISDEV(node->mode)) {
                                sys_mutex_unlock(hdl->resource_mtx);
                                return sys_driver_read(node->data.dev_t, dst,
                                                       count, fpos, rdcnt, fattr);

                        } else if (S_ISFIFO(node->mode)) {
                                sys_mutex_unlock(hdl->resource_mtx);

                                err = sys_pipe_read(node->data.pipe_t, dst,
                                                    count, rdcnt, fattr.non_blocking_rd);
                                if (!err) {
                                        if (*rdcnt > 0) {
                                                size_t size;
                                                err = sys_pipe_get_length(node->data.pipe_t,
                                                                          &size);
                                                node->size = size;
                                        }
                                }

                                return err;

                        } else if (S_ISREG(node->mode)) {
                                err = read_regular_file(node, dst, count, *fpos, rdcnt);
                        }
                }

                sys_mutex_unlock(hdl->resource_mtx);
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
API_FS_IOCTL(ramfs, void *fs_handle, void *fhdl, int request, void *arg)
{
        struct RAMFS *hdl = fs_handle;

        int err = sys_mutex_lock(hdl->resource_mtx, MTX_TIMEOUT);
        if (!err) {

                err = ENOENT;

                struct opened_file_info *opened_file = fhdl;
                if (opened_file && opened_file->child) {
                        if (S_ISDEV(opened_file->child->mode)) {
                                sys_mutex_unlock(hdl->resource_mtx);
                                return sys_driver_ioctl(opened_file->child->data.dev_t,
                                                        request, arg);

                        } else if (S_ISFIFO(opened_file->child->mode)) {

                                switch (request) {
                                case IOCTL_PIPE__CLOSE:
                                        sys_mutex_unlock(hdl->resource_mtx);
                                        return sys_pipe_close(opened_file->child->data.pipe_t);

                                case IOCTL_PIPE__CLEAR:
                                        sys_mutex_unlock(hdl->resource_mtx);
                                        return sys_pipe_clear(opened_file->child->data.pipe_t);

                                case IOCTL_PIPE__PERMANENT:
                                        sys_mutex_unlock(hdl->resource_mtx);
                                        return sys_pipe_permanent(opened_file->child->data.pipe_t);

                                default:
                                        err = EBADRQC;
                                        break;
                                }
                        }
                }

                sys_mutex_unlock(hdl->resource_mtx);
        }

        return err;
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
API_FS_FLUSH(ramfs, void *fs_handle, void *fhdl)
{
        struct RAMFS *hdl = fs_handle;

        int err = sys_mutex_lock(hdl->resource_mtx, MTX_TIMEOUT);
        if (!err) {

                struct opened_file_info *opened_file = fhdl;
                if (opened_file && opened_file->child) {
                        if (S_ISDEV(opened_file->child->mode)) {
                                sys_mutex_unlock(hdl->resource_mtx);
                                return sys_driver_flush(opened_file->child->data.dev_t);
                        } else {
                                err = ESUCC;
                        }
                } else {
                        err = ENOENT;
                }

                sys_mutex_unlock(hdl->resource_mtx);
        }

        return err;
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
API_FS_SYNC(ramfs, void *fs_handle)
{
        UNUSED_ARG1(fs_handle);
        return ESUCC;
}

//==============================================================================
/**
 * @brief Remove selected node
 *
 * @param[in] *base             base node
 * @param[in] *target           target node
 * @param[in]  position         item in base node that point to target
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
static int delete_node(struct RAMFS *hdl, node_t *base, node_t *target, u32_t position)
{
        if (S_ISDIR(target->mode)) {
                if (sys_llist_size(target->data.llist_t) > 0) {
                        return ENOTEMPTY;
                } else {
                        sys_llist_destroy(target->data.llist_t);
                        target->data.llist_t = NULL;
                }

        } else if (S_ISFIFO(target->mode)) {
                if (target->data.pipe_t) {
                        sys_pipe_destroy(target->data.pipe_t);
                        target->data.pipe_t = NULL;
                }

        } else if (S_ISREG(target->mode)) {
                clear_regular_file(target);
        }

        if (target->name) {
                sys_free(cast(void**, &target->name));
        }

        sys_llist_erase(base->data.llist_t, position);

        hdl->file_count--;

        return ESUCC;
}

//==============================================================================
/**
 * @brief Check path deep
 *
 * @param[in] *path             path
 *
 * @return path deep
 */
//==============================================================================
static uint get_path_deep(const char *path)
{
        uint deep = 0;

        if (strfch(path) == '/') {
                const char *last_path = path++;

                while ((path = strchr(path, '/'))) {
                        last_path = path;
                        path++;
                        deep++;
                }

                if (last_path[1] != '\0') {
                        deep++;
                }
        }

        return deep;
}

//==============================================================================
/**
 * @brief Function find node by path
 *
 * @param[in]  path             path
 * @param[in]  startnode        start node
 * @param[out] extPath          external path begin (pointer from path)
 * @param[in]  deep             deep control
 * @param[out] item             node is n-item of list which was found
 * @param[out] node             found node
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
static int get_node(const char *path, node_t *startnode, i32_t deep, i32_t *item, node_t **node)
{
        if (!path || !startnode) {
                return ENOENT;
        }

        if (!S_ISDIR(startnode->mode)) {
                return ENOTDIR;
        }

        node_t *current_node = startnode;
        int     dir_deep     = get_path_deep(path);
        int     err          = ENOENT;

        /* go to selected node -----------------------------------------------*/
        while (dir_deep + deep > 0) {
                /* get element from path */
                if ((path = strchr(path, '/')) == NULL) {
                        break;
                } else {
                        path++;
                }

                char *path_end    = strchr(path, '/');
                uint  path_length = !path_end ? strlen(path) : (size_t)path_end - (size_t)path;

                /* get number of list items */
                int list_size = sys_llist_size(current_node->data.llist_t);

                /* find that object exist ------------------------------------*/
                int i = 0;
                while (list_size > 0) {
                        node_t *next_node = sys_llist_at(current_node->data.llist_t, i++);

                        if (next_node == NULL) {
                                dir_deep = 1 - deep;
                                break;
                        }

                        if (  strlen(next_node->name) == path_length
                           && strncmp(next_node->name, path, path_length) == 0 ) {

                                current_node = next_node;

                                if (item) {
                                        *item = i - 1;
                                }

                                break;
                        }

                        list_size--;
                }

                /* directory does not found or error */
                if (list_size == 0 || current_node == NULL) {
                        current_node = NULL;
                        break;
                }

                dir_deep--;
        }

        if (current_node) {
                *node  = current_node;
                err = ESUCC;
        }

        return err;
}

//==============================================================================
/**
 * @brief Function create new file in selected node
 * Function allocate new node. If node is created successfully then filename
 * cannot be freed!
 *
 * @param[in]  parent           parent node
 * @param[in]  filename         filename (must be earlier allocated)
 * @param[in]  mode             mode (permissions, file type)
 * @param[out] item             new node number in base node (can be NULL)
 * @param[out] child            new node
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
static int new_node(struct RAMFS *hdl,
                    node_t       *parent,
                    char         *filename,
                    mode_t        mode,
                    i32_t        *item,
                    node_t      **child)
{
        if (!parent || !filename) {
                return EINVAL;
        }

        if (!S_ISDIR(parent->mode)) {
                return ENOTDIR;
        }

        sys_llist_foreach(node_t*, node, parent->data.llist_t) {
                if (strncmp(node->name, filename, 255) == 0) {
                        return EEXIST;
                }
        }

        node_t *node;
        int err = sys_zalloc(sizeof(node_t), cast(void**, &node));
        if (!err) {

                time_t tm = 0;
                sys_gettime(&tm);

                node->name         = filename;
                node->data.llist_t = NULL;
                node->gid          = 0;
                node->uid          = 0;
                node->mode         = mode;
                node->mtime        = tm;
                node->ctime        = tm;
                node->size         = 0;

                if (S_ISDIR(mode)) {
                        err = sys_llist_create(NULL, NULL, cast(llist_t**, &node->data));

                } else if (S_ISFIFO(mode)) {
                        err = sys_pipe_create(cast(pipe_t**, &node->data));
                }

                if (!err) {
                        if (sys_llist_push_back(parent->data.llist_t, node)) {
                                if (item) {
                                        *item = sys_llist_size(parent->data.llist_t) - 1;
                                }

                                *child = node;

                                hdl->file_count++;
                        } else {
                                err = ENOMEM;
                        }
                }

                if (err) {
                        sys_free(cast(void**, &node));
                }
        }

        return err;
}

//==============================================================================
/**
 * @brief Function add node to list of open files
 *
 * @param [in] lfs              pointer to current LFS instance
 * @param [in] parent           parent node
 * @param [in] child            child added to list of open files
 *
 * @retval One of errno value (errno.h)
 */
//==============================================================================
static int add_node_to_open_files_list(struct RAMFS *hdl,
                                       node_t       *parent,
                                       node_t       *child)
{
        struct opened_file_info *opened_file_info;
        int err = sys_zalloc(sizeof(struct opened_file_info),
                                 cast(void**, &opened_file_info));
        if (!err) {
                opened_file_info->remove_at_close = false;
                opened_file_info->child           = child;
                opened_file_info->parent          = parent;

                /* find if file shall be removed */
                sys_llist_foreach(struct opened_file_info*, opened_file, hdl->opended_files) {
                        if (opened_file->child == child && opened_file->remove_at_close) {
                                opened_file_info->remove_at_close = true;
                                break;
                        }
                }

                /* add open file info to list */
                if (!sys_llist_push_back(hdl->opended_files, opened_file_info)) {
                        sys_free(cast(void**, &opened_file_info));
                        err = ENOMEM;
                }
        }

        return err;
}

//==============================================================================
/**
 * @brief Function clear data of regular file.
 *
 * @param node                  node to write
 */
//==============================================================================
static void clear_regular_file(node_t *node)
{
        data_chain_t *chain = node->data.data_chain_t;

        while (chain) {
                data_chain_t *next = chain->next;
                sys_free(cast(void**, &chain));
                chain = next;
        }

        node->size = 0;
        node->data.data_chain_t = NULL;
}

//==============================================================================
/**
 * @brief Function write data to regular file.
 *
 * @param node                  node to write
 * @param src                   source buffer
 * @param count                 number of bytes to write
 * @param fpos                  file seek
 * @param wrcnt                 write counter
 *
 * @retval One of errno value (errno.h)
 */
//==============================================================================
static int write_regular_file(node_t *node, const u8_t *src,
                              size_t count, fpos_t fpos, size_t *wrcnt)
{
        int           err   = ESUCC;
        data_chain_t *chain = node->data.data_chain_t;
        size_t        depth = fpos / DATA_CHAIN_SIZE;
        size_t        seek  = fpos - (depth * DATA_CHAIN_SIZE);

        if (chain == NULL) {
                err = sys_zalloc(sizeof(data_chain_t), cast(void**, &chain));
                if (!err) {
                        node->data.data_chain_t = chain;
                }
        }

        // travel to selected chain
        while (!err && depth) {
                if (chain->next == NULL) {
                        err = sys_zalloc(sizeof(data_chain_t),
                                         cast(void**, &chain->next));
                }

                chain = chain->next;
                depth--;
        }

        // at position
        while (!err && count) {
                size_t tocpy = min(DATA_CHAIN_SIZE - seek, count);
                memcpy(&chain->buf[seek], src, tocpy);
                src    += tocpy;
                fpos   += tocpy;
                *wrcnt += tocpy;
                count  -= tocpy;
                seek    = 0;

                if (count) {
                        if (chain->next == NULL) {
                                err = sys_zalloc(sizeof(data_chain_t),
                                                 cast(void**, &chain->next));
                        }

                        chain = chain->next;
                }
        }

        // calculate file size
        if (depth == 0) {
                node->size = max(node->size, fpos);
        }

        return err;
}

//==============================================================================
/**
 * @brief Function read data from regular file.
 *
 * @param node                  node to read
 * @param dst                   destination buffer
 * @param count                 number of bytes to read
 * @param fpos                  file seek
 * @param rdcnt                 read counter
 *
 * @retval One of errno value (errno.h)
 */
//==============================================================================
static int read_regular_file(node_t *node, u8_t *dst,
                             size_t count, fpos_t fpos, size_t *rdcnt)
{
        int err = ESUCC;

        data_chain_t *chain = node->data.data_chain_t;
        size_t        depth = fpos / DATA_CHAIN_SIZE;
        size_t        seek  = fpos - (depth * DATA_CHAIN_SIZE);

        for (; chain && depth; chain = chain->next, depth--);

        while (chain && count && fpos < node->size) {
                size_t tocpy = min(DATA_CHAIN_SIZE - seek, count);
                       tocpy = min(tocpy, node->size - fpos);

                memcpy(dst, &chain->buf[seek], tocpy);
                dst    += tocpy;
                fpos   += tocpy;
                *rdcnt += tocpy;
                count  -= tocpy;
                seek    = 0;

                chain = chain->next;
        }

        return err;
}

/*==============================================================================
  End of file
==============================================================================*/
