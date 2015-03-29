/*=========================================================================*//**
@file    lfs.c

@author  Daniel Zorychta

@brief   This file support list file system

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
#include "core/fs.h"

/*==============================================================================
  Local symbolic constants/macros
==============================================================================*/
#define MTX_BLOCK_TIME                  10
#define PIPE_LENGTH                     CONFIG_STREAM_BUFFER_LENGTH
#define PIPE_WRITE_TIMEOUT              1
#define PIPE_READ_TIMEOUT               MAX_DELAY

/*==============================================================================
  Local types, enums definitions
==============================================================================*/
/** file types */
enum node_type {
        NODE_TYPE_DIR  = FILE_TYPE_DIR,
        NODE_TYPE_FILE = FILE_TYPE_REGULAR,
        NODE_TYPE_DRV  = FILE_TYPE_DRV,
        NODE_TYPE_LINK = FILE_TYPE_LINK,
        NODE_TYPE_PIPE = FILE_TYPE_PIPE
};

/** node structure */
typedef struct node {
        char            *name;                  /* file name                 */
        enum node_type   type;                  /* file type                 */
        fd_t             fd;                    /* file descriptor           */
        mode_t           mode;                  /* protection                */
        uid_t            uid;                   /* user ID of owner          */
        gid_t            gid;                   /* group ID of owner         */
        fpos_t           size;                  /* file size                 */
        u32_t            mtime;                 /* time of last modification */
        void            *data;                  /* file type specified data  */
} node_t;

/** info of opened file */
struct opened_file_info {
        node_t          *child;                 /* opened node                */
        node_t          *parent;                /* base of opened node        */
        bool             remove_at_close;       /* file to remove after close */
};

/** main memory structure */
struct LFS_data {
        node_t           root_dir;              /* root dir '/'           */
        mutex_t         *resource_mtx;          /* lock mutex             */
        llist_t         *opended_files;         /* list with opened files */
};

/*==============================================================================
  Local function prototypes
==============================================================================*/
static inline char  get_last_char               (const char *str);
static void         mutex_force_lock            (mutex_t *mtx);
static int          new_node                    (node_t *parent, char *filename, enum node_type type, i32_t *item, node_t **child);
static int          delete_node                 (node_t *base, node_t *target, u32_t position);
static int          get_node                    (const char *path, node_t *startnode, i32_t deep, i32_t *item, node_t **node);
static uint         get_path_deep               (const char *path);
static int          lfs_readdir                 (void *fs_handle, DIR *dir, dirent_t **dirent);
static int          lfs_closedir                (void *fs_handle, DIR *dir);
static int          add_node_to_open_files_list (struct LFS_data *lfs, node_t *parent, node_t *child);

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
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
API_FS_INIT(lfs, void **fs_handle, const char *src_path)
{
        UNUSED_ARG(src_path);

        struct LFS_data *lfs = calloc(1, sizeof(struct LFS_data));
        if (!lfs) {
                return ENOMEM;
        }

        lfs->resource_mtx  = _sys_mutex_new(MUTEX_RECURSIVE);
        lfs->root_dir.data = _sys_llist_new(NULL, NULL);
        lfs->opended_files = _sys_llist_new(_sys_llist_functor_cmp_pointers, NULL);

        if (!lfs->resource_mtx || !lfs->root_dir.data || !lfs->opended_files) {
                if (lfs->resource_mtx) {
                        _sys_mutex_delete(lfs->resource_mtx);
                }

                if (lfs->root_dir.data) {
                        _sys_llist_delete(lfs->root_dir.data);
                }

                if (lfs->opended_files) {
                        _sys_llist_delete(lfs->opended_files);
                }

                free(lfs);
                return ENOMEM;
        } else {
                lfs->root_dir.name = "/";
                lfs->root_dir.size = sizeof(node_t);
                lfs->root_dir.type = NODE_TYPE_DIR;

                *fs_handle = lfs;
                return ESUCC;
        }
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
API_FS_RELEASE(lfs, void *fs_handle)
{
        UNUSED_ARG(fs_handle);

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
API_FS_MKNOD(lfs, void *fs_handle, const char *path, const dev_t dev)
{
        struct LFS_data *lfs = fs_handle;

        mutex_force_lock(lfs->resource_mtx);

        // parent node must exist
        node_t *parent;
        int result = get_node(path, &lfs->root_dir, -1, NULL, &parent);
        if (result == ESUCC) {
                // create new node
                result           = ENOMEM;
                char *basename   = strrchr(path, '/') + 1;
                char *child_name = calloc(strlen(basename) + 1, sizeof(char));
                if (child_name) {
                        strcpy(child_name, basename);

                        node_t *child;
                        result = new_node(parent, child_name, NODE_TYPE_DRV, NULL, &child);
                        if (result == ESUCC) {
                                child->data = reinterpret_cast(void*, dev);
                        } else {
                                free(child_name);
                        }
                }
        }

        _sys_mutex_unlock(lfs->resource_mtx);

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
API_FS_MKDIR(lfs, void *fs_handle, const char *path, mode_t mode)
{
        struct LFS_data *lfs = fs_handle;

        mutex_force_lock(lfs->resource_mtx);

        // parent node must exist
        node_t *parent;
        int result = get_node(path, &lfs->root_dir, -1, NULL, &parent);
        if (result == ESUCC) {
                // create new node
                result           = ENOMEM;
                char *basename   = strrchr(path, '/') + 1;
                char *child_name = calloc(strlen(basename) + 1, sizeof(char));
                if (child_name) {
                        strcpy(child_name, basename);

                        node_t *child;
                        result = new_node(parent, child_name, NODE_TYPE_DIR, NULL, &child);
                        if (result == ESUCC) {
                                child->mode = mode;
                        } else {
                                free(child_name);
                        }
                }
        }

        _sys_mutex_unlock(lfs->resource_mtx);

        return result;
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
API_FS_MKFIFO(lfs, void *fs_handle, const char *path, mode_t mode)
{
        struct LFS_data *lfs = fs_handle;

        mutex_force_lock(lfs->resource_mtx);

        // parent node must exist
        node_t *parent;
        int result = get_node(path, &lfs->root_dir, -1, NULL, &parent);
        if (result == ESUCC) {
                // create new node
                result           = ENOMEM;
                char *basename   = strrchr(path, '/') + 1;
                char *child_name = calloc(strlen(basename) + 1, sizeof(char));
                if (child_name) {
                        strcpy(child_name, basename);

                        node_t *child;
                        result = new_node(parent, child_name, NODE_TYPE_PIPE, NULL, &child);
                        if (result == ESUCC) {
                                child->mode = mode;
                        } else {
                                free(child_name);
                        }
                }
        }

        _sys_mutex_unlock(lfs->resource_mtx);

        return result;
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
API_FS_OPENDIR(lfs, void *fs_handle, const char *path, DIR *dir)
{
        struct LFS_data *lfs = fs_handle;

        mutex_force_lock(lfs->resource_mtx);

        node_t *parent;
        int result = get_node(path, &lfs->root_dir, 0, NULL, &parent);
        if (result == ESUCC) {
                if (parent->type == NODE_TYPE_DIR) {
                        dir->f_items    = _sys_llist_size(parent->data);
                        dir->f_readdir  = lfs_readdir;
                        dir->f_closedir = lfs_closedir;
                        dir->f_seek     = 0;
                        dir->f_dd       = parent;
                } else {
                        result = ENOTDIR;
                }
        }

        _sys_mutex_unlock(lfs->resource_mtx);

        return result;
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
static int lfs_closedir(void *fs_handle, DIR *dir)
{
        UNUSED_ARG(fs_handle);
        UNUSED_ARG(dir);

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
static int lfs_readdir(void *fs_handle, DIR *dir, dirent_t **dirent)
{
        struct LFS_data *lfs    = fs_handle;
        int              result = ENOENT;

        mutex_force_lock(lfs->resource_mtx);

        node_t *parent = dir->f_dd;
        node_t *child  = _sys_llist_at(parent->data, dir->f_seek++);

        if (child) {
                if (child->type == NODE_TYPE_DRV) {
                        struct vfs_dev_stat dev_stat;
                        result = _sys_driver_stat((dev_t)child->data, &dev_stat);
                        if (result == ESUCC) {
                                child->size     = dev_stat.st_size;
                                dir->dirent.dev = (dev_t)child->data;
                        }
                } else {
                        result = ESUCC;
                }

                dir->dirent.filetype = child->type;
                dir->dirent.name     = child->name;
                dir->dirent.size     = child->size;
                *dirent              = &dir->dirent;
        }

        _sys_mutex_unlock(lfs->resource_mtx);

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
API_FS_REMOVE(lfs, void *fs_handle, const char *path)
{
        struct LFS_data *lfs = fs_handle;

        mutex_force_lock(lfs->resource_mtx);

        node_t *parent;
        int result = get_node(path, &lfs->root_dir, -1, NULL, &parent);
        if (result != ESUCC){
                goto finish;
        }

        i32_t item; node_t *child;
        result = get_node(path, &lfs->root_dir, 0, &item, &child);
        if (result != ESUCC) {
                goto finish;
        }

        if (child == &lfs->root_dir) {
                result = EPERM;
                goto finish;
        }

        /* if path is ending on slash, the object must be DIR */
        if (get_last_char(path) == '/' && child->type != NODE_TYPE_DIR) {
                result = ENOTDIR;
                goto finish;
        }

        bool remove_file = true;

        /* check if file is opened */
        if (child->type != NODE_TYPE_DIR) {
                _sys_llist_foreach(struct opened_file_info*, opened_file, lfs->opended_files) {
                        if (opened_file->child == child) {
                                opened_file->remove_at_close = true;
                                remove_file = false;
                        }
                }
        }

        /* remove node if possible */
        if (remove_file == true) {
                result = delete_node(parent, child, item);
        } else {
                result = ESUCC;
        }

        finish:
        _sys_mutex_unlock(lfs->resource_mtx);
        return result;
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
API_FS_RENAME(lfs, void *fs_handle, const char *old_name, const char *new_name)
{
        struct LFS_data *lfs = fs_handle;

        if (get_last_char(old_name) == '/' || get_last_char(new_name) == '/') {
                return EISDIR;
        }

        mutex_force_lock(lfs->resource_mtx);

        node_t *target;
        int result = get_node(old_name, &lfs->root_dir, 0, NULL, &target);
        if (result == ESUCC) {
                char *basename = strrchr(new_name, '/') + 1;
                char *new_name = calloc(1, strlen(basename) + 1);
                if (new_name) {
                        if (target->name) {
                                free(target->name);
                        }

                        target->name = new_name;
                } else {
                        result = ENOMEM;
                }
        }

        _sys_mutex_unlock(lfs->resource_mtx);
        return result;
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
API_FS_CHMOD(lfs, void *fs_handle, const char *path, mode_t mode)
{
        struct LFS_data *lfs = fs_handle;

        mutex_force_lock(lfs->resource_mtx);

        node_t *target;
        int result = get_node(path, &lfs->root_dir, 0, NULL, &target);
        if (result == ESUCC) {
                target->mode = mode;
        }

        _sys_mutex_unlock(lfs->resource_mtx);

        return result;
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
API_FS_CHOWN(lfs, void *fs_handle, const char *path, uid_t owner, gid_t group)
{
        struct LFS_data *lfs = fs_handle;

        mutex_force_lock(lfs->resource_mtx);

        node_t *target;
        int result = get_node(path, &lfs->root_dir, 0, NULL, &target);
        if (result == ESUCC) {
                target->uid = owner;
                target->gid = group;
        }

        _sys_mutex_unlock(lfs->resource_mtx);

        return result;
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
API_FS_STAT(lfs, void *fs_handle, const char *path, struct stat *stat)
{
        struct LFS_data *lfs = fs_handle;

        mutex_force_lock(lfs->resource_mtx);

        node_t *target;
        int result = get_node(path, &lfs->root_dir, 0, NULL, &target);
        if (result == ESUCC) {
                if ( (get_last_char(path) == '/' && target->type == NODE_TYPE_DIR)
                   || get_last_char(path) != '/') {

                        if (target->type == NODE_TYPE_DRV) {
                                struct vfs_dev_stat dev_stat;
                                result = _sys_driver_stat((dev_t)target->data, &dev_stat);
                                if (result == ESUCC) {
                                        target->size = dev_stat.st_size;
                                        stat->st_dev = dev_stat.st_major;
                                }
                        } else {
                                stat->st_dev = target->fd;
                        }

                        stat->st_gid   = target->gid;
                        stat->st_mode  = target->mode;
                        stat->st_mtime = target->mtime;
                        stat->st_size  = target->size;
                        stat->st_uid   = target->uid;
                        stat->st_type  = target->type;
                } else {
                        result = EINVAL;
                }
        }

        _sys_mutex_unlock(lfs->resource_mtx);

        return result;
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
API_FS_FSTAT(lfs, void *fs_handle, void *extra, fd_t fd, struct stat *stat)
{
        UNUSED_ARG(fd);

        struct LFS_data *lfs = fs_handle;

        mutex_force_lock(lfs->resource_mtx);

        int result;

        struct opened_file_info *opened_file = extra;
        if (opened_file && opened_file->child) {
                if (opened_file->child->type == NODE_TYPE_DRV) {
                        struct vfs_dev_stat dev_stat;
                        result = _sys_driver_stat((dev_t)opened_file->child->data, &dev_stat);
                        if (result == ESUCC) {
                                opened_file->child->size = dev_stat.st_size;
                                stat->st_dev = dev_stat.st_major;
                        }
                } else {
                        stat->st_dev = opened_file->child->fd;
                        result       = ESUCC;
                }

                stat->st_gid   = opened_file->child->gid;
                stat->st_mode  = opened_file->child->mode;
                stat->st_mtime = opened_file->child->mtime;
                stat->st_size  = opened_file->child->size;
                stat->st_uid   = opened_file->child->uid;
                stat->st_type  = opened_file->child->type;
        } else {
                result = ENOENT;
        }

        _sys_mutex_unlock(lfs->resource_mtx);

        return result;
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
API_FS_STATFS(lfs, void *fs_handle, struct statfs *statfs)
{
        UNUSED_ARG(fs_handle);

        statfs->f_bfree  = 0;
        statfs->f_blocks = 0;
        statfs->f_ffree  = 0;
        statfs->f_files  = 0;
        statfs->f_type   = 0x01;
        statfs->f_fsname = "lfs";

        return ESUCC;
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
API_FS_OPEN(lfs, void *fs_handle, void **extra, fd_t *fd, fpos_t *fpos, const char *path, u32_t flags)
{
        UNUSED_ARG(fd);

        struct LFS_data *lfs = fs_handle;

        mutex_force_lock(lfs->resource_mtx);

        // open file parent
        node_t *parent;
        int result = get_node(path, &lfs->root_dir, -1, NULL, &parent);
        if (result != ESUCC) {
                goto finish;
        }

        // try to open selected file, if not exist then try create if O_CREAT flag is set
        i32_t item; node_t *child;
        result = get_node(path, &lfs->root_dir, 0, &item, &child);
        if (result == ENOENT) {
                // check that file should be created
                if (!(flags & O_CREAT)) {
                        goto finish;
                }

                char *basename  = strrchr(path, '/') + 1;
                char *file_name = calloc(1, strlen(basename) + 1);
                if (file_name == NULL) {
                        result = ENOMEM;
                        goto finish;
                }

                strcpy(file_name, basename);

                result = new_node(parent, file_name, NODE_TYPE_FILE, &item, &child);
                if (result != ESUCC) {
                        free(file_name);
                        goto finish;
                }
        } else {
                if (child->type == NODE_TYPE_DIR) {
                        result = EISDIR;
                        goto finish;
                }
        }

        /* add file to list of open files */
        result = add_node_to_open_files_list(lfs, parent, child);
        if (result != ESUCC) {
                goto finish;
        }

        /* set file parameters */
        if (child->type == NODE_TYPE_FILE) {
                // truncate file if requested
                if (flags & O_TRUNC) {
                        if (child->data) {
                                free(child->data);
                                child->data = NULL;
                        }

                        child->size = 0;
                }

                if (flags & O_APPEND) {
                        *fpos = child->size;
                } else {
                        *fpos = 0;
                }

        } else if (child->type == NODE_TYPE_DRV) {
                result = _sys_driver_open((dev_t)child->data, flags);
                if (result == ESUCC) {
                        *fpos = 0;
                } else {
                        _sys_llist_pop_back(lfs->opended_files);
                        goto finish;
                }
        }

        // load pointer to file descriptor
        *extra = _sys_llist_back(lfs->opended_files);

        finish:
        _sys_mutex_unlock(lfs->resource_mtx);

        return result;
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
API_FS_CLOSE(lfs, void *fs_handle, void *extra, fd_t fd, bool force)
{
        UNUSED_ARG(fd);

        struct LFS_data *lfs    = fs_handle;
        int              result = ENOENT;

        mutex_force_lock(lfs->resource_mtx);

        struct opened_file_info *opened_file = extra;
        int pos = _sys_llist_find_begin(lfs->opended_files, opened_file);

        if (opened_file && opened_file->child && pos >= 0) {
                node_t *target = opened_file->child;

                /* close device if file is driver type */
                if (target->type == NODE_TYPE_DRV) {
                        result = _sys_driver_close((dev_t)target->data, force);

                } else {
                        /* file to remove, check if other task does not opens this file */
                        if (opened_file->remove_at_close == true) {
                                bool remove = true;

                                _sys_llist_foreach(struct opened_file_info*, file, lfs->opended_files) {
                                        if (file->child == target) {
                                                remove = false;
                                        }
                                }

                                if (remove) {
                                        result = delete_node(opened_file->parent, opened_file->child, pos);
                                }
                        } else {
                                result = ESUCC;
                        }
                }

                /* remove file from list */
                if (result == ESUCC) {
                        _sys_llist_erase(lfs->opended_files, pos);
                }
        }

        _sys_mutex_unlock(lfs->resource_mtx);

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
API_FS_WRITE(lfs,
             void            *fs_handle,
             void            *extra,
             fd_t             fd,
             const u8_t      *src,
             size_t           count,
             fpos_t          *fpos,
             size_t          *wrcnt,
             struct vfs_fattr fattr)
{
        UNUSED_ARG(fd);

        struct LFS_data *lfs    = fs_handle;
        int              result = ENOENT;

        mutex_force_lock(lfs->resource_mtx);

        struct opened_file_info *opened_file = extra;
        if (opened_file && opened_file->child) {
                node_t *target = opened_file->child;

                if (target->type == NODE_TYPE_DRV) {
                        _sys_mutex_unlock(lfs->resource_mtx);
                        return _sys_driver_write((dev_t)target->data, src, count, fpos, fattr);

                } else if (target->type == NODE_TYPE_PIPE) {
                       _sys_mutex_unlock(lfs->resource_mtx);

                       result = _sys_pipe_write(target->data, src, count, wrcnt, fattr.non_blocking_wr);
                       if (result == ESUCC) {
                               if (*wrcnt > 0) {
                                       target->size = _sys_pipe_get_length(target->data);
                               }
                       }

                       return result;

               } else if (target->type == NODE_TYPE_FILE) {
                       size_t write_size  = count;
                       size_t file_length = target->size;
                       size_t seek        = *fpos > SIZE_MAX ? SIZE_MAX : *fpos;

                       if (seek > file_length) {
                               seek = file_length;
                       }

                       if ((seek + write_size) > file_length || target->data == NULL) {
                               char *new_data = malloc(file_length + write_size);
                               if (new_data) {
                                       if (target->data) {
                                               memcpy(new_data, target->data, file_length);
                                               free(target->data);
                                       }

                                       memcpy(new_data + seek, src, write_size);

                                       target->data  = new_data;
                                       target->size += write_size - (file_length - seek);

                                       *wrcnt = count;
                                       result = ESUCC;
                               } else {
                                       result = ENOSPC;
                               }

                       } else {
                               memcpy(static_cast(u8_t*, target->data) + seek, src, write_size);
                               *wrcnt = count;
                               result = ESUCC;
                       }
                }
        }

        _sys_mutex_unlock(lfs->resource_mtx);

        return result;
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
API_FS_READ(lfs,
            void            *fs_handle,
            void            *extra,
            fd_t             fd,
            u8_t            *dst,
            size_t           count,
            fpos_t          *fpos,
            size_t          *rdcnt,
            struct vfs_fattr fattr)
{
        UNUSED_ARG(fd);

        struct LFS_data *lfs    = fs_handle;
        int              result = ENOENT;

        mutex_force_lock(lfs->resource_mtx);

        struct opened_file_info *opened_file = extra;
        if (opened_file && opened_file->child) {
                node_t *target = opened_file->child;

                if (target->type == NODE_TYPE_DRV) {
                        _sys_mutex_unlock(lfs->resource_mtx);
                        return _sys_driver_read((dev_t)target->data, dst, count, fpos, fattr);

                } else if (target->type == NODE_TYPE_PIPE) {
                        _sys_mutex_unlock(lfs->resource_mtx);

                        result = _sys_pipe_read(target->data, dst, count, rdcnt, fattr.non_blocking_rd);
                        if (result == ESUCC) {
                                if (*rdcnt > 0) {
                                        target->size = _sys_pipe_get_length(target->data);
                                }
                        }

                        return result;

                } else if (target->type == NODE_TYPE_FILE) {
                        size_t file_length = target->size;
                        size_t seek        = *fpos > SIZE_MAX ? SIZE_MAX : *fpos;

                        if (seek > file_length) {
                                seek = file_length;
                        }

                        /* check how many items to read is on current file position */
                        size_t items_to_read = ((file_length - seek) >= count) ? count : (file_length - seek);

                        /* copy if file buffer exist */
                        if (target->data) {
                                if (items_to_read > 0) {
                                        memcpy(dst, static_cast(u8_t*, target->data) + seek, items_to_read);
                                        *rdcnt = items_to_read;
                                } else {
                                        *rdcnt = 0;
                                }

                                result = ESUCC;
                        } else {
                                result = EIO;
                        }
                }
        }

        _sys_mutex_unlock(lfs->resource_mtx);

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
API_FS_IOCTL(lfs, void *fs_handle, void *extra, fd_t fd, int request, void *arg)
{
        UNUSED_ARG(fd);

        struct LFS_data *lfs    = fs_handle;
        int              result = ENOENT;

        mutex_force_lock(lfs->resource_mtx);

        struct opened_file_info *opened_file = extra;
        if (opened_file && opened_file->child) {
                if (opened_file->child->type == NODE_TYPE_DRV) {
                        _sys_mutex_unlock(lfs->resource_mtx);
                        return _sys_driver_ioctl((dev_t)opened_file->child->data, request, arg);

                } else if (opened_file->child->type == NODE_TYPE_PIPE) {

                        switch (request) {
                        case IOCTL_PIPE__CLOSE:
                                _sys_mutex_unlock(lfs->resource_mtx);
                                return _sys_pipe_close(opened_file->child->data) ? ESUCC : EIO;

                        case IOCTL_PIPE__CLEAR:
                                _sys_mutex_unlock(lfs->resource_mtx);
                                return _sys_pipe_clear(opened_file->child->data) ? ESUCC : EIO;

                        default:
                                result = EBADRQC;
                                break;
                        }
                }
        }

        _sys_mutex_unlock(lfs->resource_mtx);

        return result;
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
API_FS_FLUSH(lfs, void *fs_handle, void *extra, fd_t fd)
{
        UNUSED_ARG(fd);

        struct LFS_data *lfs    = fs_handle;
        int              result = ENOENT;

        mutex_force_lock(lfs->resource_mtx);

        struct opened_file_info *opened_file = extra;
        if (opened_file && opened_file->child) {
                if (opened_file->child->type == NODE_TYPE_DRV) {
                        _sys_mutex_unlock(lfs->resource_mtx);
                        return _sys_driver_flush((dev_t)opened_file->child->data);
                } else {
                        result = ESUCC;
                }
        }

        _sys_mutex_unlock(lfs->resource_mtx);

        return result;
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
API_FS_SYNC(lfs, void *fs_handle)
{
        UNUSED_ARG(fs_handle);
        return ESUCC;
}

//==============================================================================
/**
 * @brief Return last character of selected string
 */
//==============================================================================
static inline char get_last_char(const char *str)
{
        return LAST_CHARACTER(str);
}

//==============================================================================
/**
 * @brief Function force lock mutex
 *
 * @param mtx           mutex
 */
//==============================================================================
static void mutex_force_lock(mutex_t *mtx)
{
        while (_sys_mutex_lock(mtx, MTX_BLOCK_TIME) != true);
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
static int delete_node(node_t *base, node_t *target, u32_t position)
{
        if (target->type == NODE_TYPE_DIR) {
                if (_sys_llist_size(target->data) > 0) {
                        return ENOTEMPTY;
                } else {
                        _sys_llist_delete(target->data);
                        target->data = NULL;
                }

        } else if (target->type == NODE_TYPE_PIPE) {

                if (target->data) {
                        _sys_pipe_delete(target->data);
                        target->data = NULL;
                }
        }

        if (target->name) {
                free(target->name);
        }

        if (target->data) {
                free(target->data);
        }

        _sys_llist_erase(base->data, position);

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

        if (path[0] == '/') {
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

        if (startnode->type != NODE_TYPE_DIR) {
                return ENOTDIR;
        }

        node_t *current_node = startnode;
        int     dir_deep     = get_path_deep(path);
        int     result       = ENOENT;

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
                int list_size = _sys_llist_size(current_node->data);

                /* find that object exist ------------------------------------*/
                int i = 0;
                while (list_size > 0) {
                        node_t *next_node = _sys_llist_at(current_node->data, i++);

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
                result = ESUCC;
        }

        return result;
}

//==============================================================================
/**
 * @brief Function create new file in selected node
 * Function allocate new node. If node is created successfully then filename
 * cannot be freed!
 *
 * @param[in]  parent           parent node
 * @param[in]  filename         filename (must be earlier allocated)
 * @param[in]  type             node type
 * @param[out] item             new node number in base node (can be NULL)
 * @param[out] child            new node
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
static int new_node(node_t *parent, char *filename, enum node_type type, i32_t *item, node_t **child)
{
        if (!parent || !filename) {
                return EINVAL;
        }

        if (parent->type != NODE_TYPE_DIR) {
                return ENOTDIR;
        }

        _sys_llist_foreach(node_t*, node, parent->data) {
                if (strcpy(node->name, filename) == 0) {
                        return EEXIST;
                }
        }

        int     result = ENOMEM;
        node_t *node   = calloc(1, sizeof(node_t));
        if (node) {
                node->name  = filename;
                node->data  = NULL;
                node->gid   = 0;
                node->mode  = 0;
                node->mtime = 0;
                node->size  = 0;
                node->type  = type;
                node->uid   = 0;

                if (type == NODE_TYPE_DIR) {
                        node->data = _sys_llist_new(NULL, NULL);
                        if (node->data == NULL) {
                                free(node);
                                return ENOMEM;
                        }
                } else if (type == FILE_TYPE_PIPE) {
                        node->data = _sys_pipe_new();
                        if (node->data == NULL) {
                                free(node);
                                return ENOMEM;
                        }
                }

                if (_sys_llist_push_back(parent->data, node)) {
                        if (item)
                                *item  = _sys_llist_size(parent->data) - 1;

                        *child = node;
                        result = ESUCC;
                } else {
                        free(node);
                }
        }

        return result;
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
static int add_node_to_open_files_list(struct LFS_data *lfs,
                                       node_t          *parent,
                                       node_t          *child)
{
        struct opened_file_info *opened_file_info = calloc(1, sizeof(struct opened_file_info));
        if (opened_file_info) {
                opened_file_info->remove_at_close = false;
                opened_file_info->child           = child;
                opened_file_info->parent          = parent;

                /* find if file shall be removed */
                _sys_llist_foreach(struct opened_file_info*, opened_file, lfs->opended_files) {
                        if (opened_file->child == child && opened_file->remove_at_close) {
                                opened_file_info->remove_at_close = true;
                                break;
                        }
                }

                /* add open file info to list */
                if (_sys_llist_push_back(lfs->opended_files, opened_file_info)) {
                        return ESUCC;
                } else {
                        free(opened_file_info);
                }
        }

        return ENOMEM;
}

/*==============================================================================
  End of file
==============================================================================*/
