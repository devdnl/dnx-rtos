/*=========================================================================*//**
@file    lfs.c

@author  Daniel Zorychta

@brief   This file support list file system

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

/*==============================================================================
  Include files
==============================================================================*/
#include "core/fs.h"
#include <string.h>
#include <dnx/thread.h>
#include <dnx/misc.h>
#include <sys/ioctl.h>
#include "core/list.h"
#include "core/pipe.h"

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
        u32_t            uid;                   /* user ID of owner          */
        u32_t            gid;                   /* group ID of owner         */
        u64_t            size;                  /* file size                 */
        u32_t            mtime;                 /* time of last modification */
        void            *data;                  /* file type specified data  */
} node_t;

/** info of opened file */
struct opened_file_info {
        node_t          *node;                  /* opened node                    */
        node_t          *base_node;             /* base of opened node            */
        bool             remove_at_close;       /* file to remove after close     */
        u32_t            item_ID;               /* item ID in base directory list */
};

/** main memory structure */
struct LFS_data {
        node_t           root_dir;              /* root dir '/'           */
        mutex_t         *resource_mtx;          /* lock mutex             */
        list_t          *list_of_opended_files; /* list with opened files */
        u32_t            id_counter;            /* list ID counter        */
};

/*==============================================================================
  Local function prototypes
==============================================================================*/
static inline char      get_first_char                  (const char *str);
static inline char      get_last_char                   (const char *str);
static void             mutex_force_lock                (mutex_t *mtx);
static node_t          *new_node                        (struct LFS_data *lfs, node_t *nodebase, char *filename, i32_t *item);
static stdret_t         delete_node                     (node_t *base, node_t *target, u32_t baseitemid);
static node_t          *get_node                        (const char *path, node_t *startnode, i32_t deep, i32_t *item);
static uint             get_path_deep                   (const char *path);
static dirent_t         lfs_readdir                     (void *fs_handle, DIR *dir);
static stdret_t         lfs_closedir                    (void *fs_handle, DIR *dir);
static stdret_t         add_node_to_list_of_open_files  (struct LFS_data *lfs, node_t *base_node, node_t *node, i32_t *item);

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
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
API_FS_INIT(lfs, void **fs_handle, const char *src_path)
{
        UNUSED_ARG(src_path);

        STOP_IF(!fs_handle);

        struct LFS_data *lfs;
        if (!(lfs = calloc(1, sizeof(struct LFS_data)))) {
                return STD_RET_ERROR;
        }

        lfs->resource_mtx  = mutex_new(MUTEX_RECURSIVE);
        lfs->root_dir.data = list_new();
        lfs->list_of_opended_files = list_new();

        if (!lfs->resource_mtx || !lfs->root_dir.data || !lfs->list_of_opended_files) {
                if (lfs->resource_mtx) {
                        mutex_delete(lfs->resource_mtx);
                }

                if (lfs->root_dir.data) {
                        list_delete(lfs->root_dir.data);
                }

                if (lfs->list_of_opended_files) {
                        list_delete(lfs->list_of_opended_files);
                }

                free(lfs);
                return STD_RET_ERROR;
        } else {
                lfs->root_dir.name = "/";
                lfs->root_dir.size = sizeof(node_t);
                lfs->root_dir.type = NODE_TYPE_DIR;

                *fs_handle = lfs;
                return STD_RET_OK;
        }
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
API_FS_RELEASE(lfs, void *fs_handle)
{
        UNUSED_ARG(fs_handle);

        /*
         * Here the LFS should delete all lists and free all allocated buffers.
         * If will be necessary should be implemented in the future.
         */

        errno = EPERM;

        return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief Create node for driver file
 *
 * @param[in ]          *fs_handle              file system allocated memory
 * @param[in ]          *path                   name of created node
 * @param[in ]           dev                    driver id
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
API_FS_MKNOD(lfs, void *fs_handle, const char *path, const dev_t dev)
{
        STOP_IF(!fs_handle);
        STOP_IF(!path);

        struct LFS_data *lfs = fs_handle;

        if (path[0] != '/') {
                return STD_RET_ERROR;
        }

        mutex_force_lock(lfs->resource_mtx);
        node_t *node     = get_node(path, &lfs->root_dir, -1, NULL);
        node_t *drv_node = get_node(strrchr(path, '/'), node, 0, NULL);

        /* directory must exist and driver's file not */
        if (!node || drv_node) {
                goto error;
        }

        if (node->type != NODE_TYPE_DIR) {
                goto error;
        }

        char *drv_name     = strrchr(path, '/') + 1;
        uint  drv_name_len = strlen(drv_name);

        char *drv_file_name;
        if ((drv_file_name = calloc(drv_name_len + 1, sizeof(char)))) {
                strcpy(drv_file_name, drv_name);

                node_t *drv_file = calloc(1, sizeof(node_t));
                if (drv_file) {
                        drv_file->name = drv_file_name;
                        drv_file->size = 0;
                        drv_file->type = NODE_TYPE_DRV;
                        drv_file->data = (void *)dev;
                        drv_file->fd   = 0;

                        /* add new driver to this folder */
                        if (list_add_item(node->data, lfs->id_counter++, drv_file) >= 0) {
                                mutex_unlock(lfs->resource_mtx);
                                return STD_RET_OK;
                        }
                }

                /* free memory when error */
                if (drv_file) {
                        free(drv_file);
                }

                free(drv_file_name);
        }

error:
        mutex_unlock(lfs->resource_mtx);
        return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief Create directory
 *
 * @param[in ]          *fs_handle              file system allocated memory
 * @param[in ]          *path                   name of created directory
 * @param[in ]           mode                   dir mode
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
API_FS_MKDIR(lfs, void *fs_handle, const char *path, mode_t mode)
{
        STOP_IF(!fs_handle);
        STOP_IF(!path);

        struct LFS_data *lfs = fs_handle;
        char *new_dir_name   = NULL;

        if (path[0] != '/') {
                return STD_RET_ERROR;
        }

        mutex_force_lock(lfs->resource_mtx);
        node_t *base_node = get_node(path, &lfs->root_dir, -1, NULL);
        node_t *file_node = get_node(strrchr(path, '/'), base_node, 0, NULL);

        /* base node must exist and created node not */
        if (base_node == NULL) {
                goto error;
        }

        if (file_node != NULL) {
                errno = EEXIST;
                goto error;
        }

        if (base_node->type != NODE_TYPE_DIR) {
                goto error;
        }

        char *dir_name_ptr = strrchr(path, '/') + 1;
        uint  dir_name_len = strlen(dir_name_ptr);

        if ((new_dir_name = calloc(dir_name_len + 1, sizeof(char)))) {
                strcpy(new_dir_name, dir_name_ptr);

                node_t *new_dir;
                if (!(new_dir = calloc(1, sizeof(node_t)))) {
                        goto error;
                }

                if ((new_dir->data = list_new())) {
                        new_dir->name = new_dir_name;
                        new_dir->size = sizeof(node_t);
                        new_dir->type = NODE_TYPE_DIR;
                        new_dir->mode = mode;

                        /* add new folder to this folder */
                        if (list_add_item(base_node->data, lfs->id_counter++, new_dir) >= 0) {
                                mutex_unlock(lfs->resource_mtx);
                                return STD_RET_OK;
                        } else {
                                list_delete(new_dir->data);
                        }
                } else {
                        free(new_dir);
                }
        }

error:
        if (new_dir_name) {
                free(new_dir_name);
        }

        mutex_unlock(lfs->resource_mtx);
        return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief Create pipe
 *
 * @param[in ]          *fs_handle              file system allocated memory
 * @param[in ]          *path                   name of created pipe
 * @param[in ]           mode                   pipe mode
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
API_FS_MKFIFO(lfs, void *fs_handle, const char *path, mode_t mode)
{
        STOP_IF(!fs_handle);
        STOP_IF(!path);

        struct LFS_data *lfs = fs_handle;

        if (FIRST_CHARACTER(path) != '/') {
                errno = ENOENT;
                return STD_RET_ERROR;
        }

        mutex_force_lock(lfs->resource_mtx);
        node_t *dir_node  = get_node(path, &lfs->root_dir, -1, NULL);
        node_t *fifo_node = get_node(strrchr(path, '/'), dir_node, 0, NULL);

        /* directory must exist and driver's file not */
        if (!dir_node) {
                goto error;
        }

        if (fifo_node) {
                errno = EEXIST;
                goto error;
        }

        if (dir_node->type != NODE_TYPE_DIR) {
                goto error;
        }

        char *fifo_name     = strrchr(path, '/') + 1;
        uint  fifo_name_len = strlen(fifo_name);

        char *fifo_file_name = calloc(fifo_name_len + 1, sizeof(char));
        if (fifo_file_name) {
                strcpy(fifo_file_name, fifo_name);

                node_t *fifo_file = calloc(1, sizeof(node_t));
                pipe_t *pipe      = pipe_new();

                if (fifo_file && pipe) {

                        fifo_file->name = fifo_file_name;
                        fifo_file->size = 0;
                        fifo_file->type = NODE_TYPE_PIPE;
                        fifo_file->data = pipe;
                        fifo_file->fd   = 0;
                        fifo_file->mode = mode;

                        /* add pipe to folder */
                        if (list_add_item(dir_node->data, lfs->id_counter++, fifo_file) >= 0) {
                                mutex_unlock(lfs->resource_mtx);
                                return STD_RET_OK;
                        }
                }

                /* free memory when error */
                if (fifo_file) {
                        free(fifo_file);
                }

                if (pipe) {
                        pipe_delete(pipe);
                }

                free(fifo_file_name);
        }

error:
        mutex_unlock(lfs->resource_mtx);
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
API_FS_OPENDIR(lfs, void *fs_handle, const char *path, DIR *dir)
{
        STOP_IF(!fs_handle);
        STOP_IF(!path);
        STOP_IF(!dir);

        struct LFS_data *lfs = fs_handle;
        mutex_force_lock(lfs->resource_mtx);

        /* go to target dir */
        node_t *node;
        if ((node = get_node(path, &lfs->root_dir, 0, NULL))) {
                if (node->type == NODE_TYPE_DIR) {
                        dir->f_items    = list_get_item_count(node->data);
                        dir->f_readdir  = lfs_readdir;
                        dir->f_closedir = lfs_closedir;
                        dir->f_seek     = 0;
                        dir->f_dd       = node;

                        mutex_unlock(lfs->resource_mtx);
                        return STD_RET_OK;
                } else {
                        errno = ENOTDIR;
                }
        }

        mutex_unlock(lfs->resource_mtx);
        return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief Function close dir
 *
 * @param[in] *fs_handle        FS handle
 * @param[in] *dir              directory info
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
static stdret_t lfs_closedir(void *fs_handle, DIR *dir)
{
        UNUSED_ARG(fs_handle);
        UNUSED_ARG(dir);

        return STD_RET_OK;
}

//==============================================================================
/**
 * @brief Function read next item of opened directory
 *
 * @param[in] *fs_handle        FS handle
 * @param[in] *dir              directory object
 *
 * @return element attributes
 */
//==============================================================================
static dirent_t lfs_readdir(void *fs_handle, DIR *dir)
{
        STOP_IF(!fs_handle);
        STOP_IF(!dir);

        struct LFS_data *lfs = fs_handle;

        dirent_t dirent;
        dirent.name = NULL;
        dirent.size = 0;

        mutex_force_lock(lfs->resource_mtx);

        node_t *from = dir->f_dd;
        node_t *node = list_get_nitem_data(from->data, dir->f_seek++);

        if (node) {
                if (node->type == NODE_TYPE_DRV) {
                        struct vfs_dev_stat dev_stat;
                        dev_stat.st_size = 0;
                        driver_stat((dev_t)node->data, &dev_stat);
                        node->size = dev_stat.st_size;
                        dirent.dev = (dev_t)node->data;
                }

                dirent.filetype = node->type;
                dirent.name     = node->name;
                dirent.size     = node->size;
        }

        mutex_unlock(lfs->resource_mtx);

        return dirent;
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
API_FS_REMOVE(lfs, void *fs_handle, const char *path)
{
        STOP_IF(!fs_handle);
        STOP_IF(!path);

        struct LFS_data *lfs = fs_handle;

        mutex_force_lock(lfs->resource_mtx);
        i32_t   item;
        bool    remove_file = true;
        node_t *base_node   = get_node(path, &lfs->root_dir, -1, NULL);
        node_t *obj_node    = get_node(path, &lfs->root_dir, 0, &item);

        if (!base_node || !obj_node || obj_node == &lfs->root_dir) {
                goto error;
        }

        /* if path is ending on slash, the object must be DIR */
        if (get_last_char(path) == '/') {
                if (obj_node->type != NODE_TYPE_DIR) {
                        errno = ENOTDIR;
                        goto error;
                }
        }

        /* check if file is opened */
        if (obj_node->type != NODE_TYPE_DIR) {
                i32_t n = list_get_item_count(lfs->list_of_opended_files);

                for (int i = 0; i < n; i++) {
                        struct opened_file_info *opened_file = list_get_nitem_data(lfs->list_of_opended_files, i);
                        if (opened_file->node == obj_node) {
                                opened_file->remove_at_close = true;
                                remove_file = false;
                        }
                }
        }

        /* remove node if possible */
        if (remove_file == true) {
                u32_t item_ID;
                if (list_get_nitem_ID(base_node->data, item, &item_ID) == STD_RET_OK) {
                        mutex_unlock(lfs->resource_mtx);
                        return delete_node(base_node, obj_node, item_ID);
                } else {
                        errno = ENOENT;
                }
        } else {
                mutex_unlock(lfs->resource_mtx);
                return STD_RET_OK;
        }

error:
        mutex_unlock(lfs->resource_mtx);
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
API_FS_RENAME(lfs, void *fs_handle, const char *old_name, const char *new_name)
{
        STOP_IF(!fs_handle);
        STOP_IF(!old_name);
        STOP_IF(!new_name);

        struct LFS_data *lfs = fs_handle;
        char *new_node_name  = NULL;

        mutex_force_lock(lfs->resource_mtx);
        node_t *old_node_base = get_node(old_name, &lfs->root_dir, -1, NULL);
        node_t *new_node_base = get_node(new_name, &lfs->root_dir, -1, NULL);

        if (!old_node_base || !new_node_base) {
                goto error;
        }

        if (old_node_base != new_node_base) {
                goto error;
        }

        if (get_first_char(old_name) != '/' || get_first_char(new_name) != '/') {
                goto error;
        }

        if (get_last_char(old_name) == '/' || get_last_char(new_name) == '/') {
                goto error;
        }

        new_node_name = calloc(1, strlen(strrchr(new_name, '/') + 1));
        node_t *node  = get_node(old_name, &lfs->root_dir, 0, NULL);

        if (new_node_name && node) {
                strcpy(new_node_name, strrchr(new_name, '/') + 1);

                if (node->name) {
                        free(node->name);
                }

                node->name = new_node_name;

                if (node->type == NODE_TYPE_DIR) {
                        node->size = sizeof(node_t);
                }

                mutex_unlock(lfs->resource_mtx);
                return STD_RET_OK;
        }

error:
        if (new_node_name) {
                free(new_node_name);
        }

        mutex_unlock(lfs->resource_mtx);
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
API_FS_CHMOD(lfs, void *fs_handle, const char *path, int mode)
{
        STOP_IF(!fs_handle);
        STOP_IF(!path);

        struct LFS_data *lfs = fs_handle;

        mutex_force_lock(lfs->resource_mtx);

        node_t *node = get_node(path, &lfs->root_dir, 0, NULL);
        if (node) {
                node->mode = mode;
                mutex_unlock(lfs->resource_mtx);
                return STD_RET_OK;
        }

        mutex_unlock(lfs->resource_mtx);

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
API_FS_CHOWN(lfs, void *fs_handle, const char *path, int owner, int group)
{
        STOP_IF(!fs_handle);
        STOP_IF(!path);

        struct LFS_data *lfs = fs_handle;

        mutex_force_lock(lfs->resource_mtx);

        node_t *node = get_node(path, &lfs->root_dir, 0, NULL);
        if (node) {
                node->uid = owner;
                node->gid = group;

                mutex_unlock(lfs->resource_mtx);
                return STD_RET_OK;
        }

        mutex_unlock(lfs->resource_mtx);

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
API_FS_STAT(lfs, void *fs_handle, const char *path, struct stat *stat)
{
        STOP_IF(!fs_handle);
        STOP_IF(!path);
        STOP_IF(!stat);

        struct LFS_data *lfs = fs_handle;

        mutex_force_lock(lfs->resource_mtx);

        node_t *node = get_node(path, &lfs->root_dir, 0, NULL);
        if (node) {
                if ( (get_last_char(path) == '/' && node->type == NODE_TYPE_DIR)
                   || get_last_char(path) != '/') {

                        if (node->type == NODE_TYPE_DRV) {
                                struct vfs_dev_stat dev_stat;
                                dev_stat.st_size = 0;
                                driver_stat((dev_t)node->data, &dev_stat);
                                node->size   = dev_stat.st_size;
                                stat->st_dev = dev_stat.st_major;
                        } else {
                                stat->st_dev = node->fd;
                        }

                        stat->st_gid   = node->gid;
                        stat->st_mode  = node->mode;
                        stat->st_mtime = node->mtime;
                        stat->st_size  = node->size;
                        stat->st_uid   = node->uid;
                        stat->st_type  = node->type;

                        mutex_unlock(lfs->resource_mtx);
                        return STD_RET_OK;
                }
        }

        mutex_unlock(lfs->resource_mtx);

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
API_FS_FSTAT(lfs, void *fs_handle, void *extra, fd_t fd, struct stat *stat)
{
        UNUSED_ARG(extra);

        STOP_IF(!fs_handle);
        STOP_IF(!stat);

        struct LFS_data *lfs = fs_handle;

        mutex_force_lock(lfs->resource_mtx);

        struct opened_file_info *opened_file = list_get_iditem_data(lfs->list_of_opended_files, fd);
        if (opened_file) {
                if (opened_file->node) {
                        if (opened_file->node->type == NODE_TYPE_DRV) {
                                struct vfs_dev_stat dev_stat;
                                dev_stat.st_size = 0;
                                driver_stat((dev_t)opened_file->node->data, &dev_stat);
                                opened_file->node->size = dev_stat.st_size;
                                stat->st_dev = dev_stat.st_major;
                        } else {
                                stat->st_dev = opened_file->node->fd;
                        }

                        stat->st_gid   = opened_file->node->gid;
                        stat->st_mode  = opened_file->node->mode;
                        stat->st_mtime = opened_file->node->mtime;
                        stat->st_size  = opened_file->node->size;
                        stat->st_uid   = opened_file->node->uid;
                        stat->st_type  = opened_file->node->type;

                        mutex_unlock(lfs->resource_mtx);
                        return STD_RET_OK;
                }
        }

        errno = ENOENT;

        mutex_unlock(lfs->resource_mtx);

        return STD_RET_ERROR;
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
API_FS_STATFS(lfs, void *fs_handle, struct vfs_statfs *statfs)
{
        UNUSED_ARG(fs_handle);

        STOP_IF(!statfs);

        statfs->f_bfree  = 0;
        statfs->f_blocks = 0;
        statfs->f_ffree  = 0;
        statfs->f_files  = 0;
        statfs->f_type   = 0x01;
        statfs->f_fsname = "lfs";

        return STD_RET_OK;
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
API_FS_OPEN(lfs, void *fs_handle, void **extra, fd_t *fd, u64_t *fpos, const char *path, int flags)
{
        UNUSED_ARG(extra);

        STOP_IF(!fs_handle);
        STOP_IF(!fd);
        STOP_IF(!fpos);
        STOP_IF(!path);

        struct LFS_data *lfs = fs_handle;

        mutex_force_lock(lfs->resource_mtx);

        i32_t   item;
        node_t *node      = get_node(path, &lfs->root_dir, 0, &item);
        node_t *base_node = get_node(path, &lfs->root_dir, -1, NULL);

        /* create new file when necessary */
        if (base_node && node == NULL) {
                if (!(flags & O_CREAT)) {
                        goto error;
                }

                char *file_name = calloc(1, strlen(strrchr(path, '/')));
                if (file_name == NULL) {
                        goto error;
                }

                strcpy(file_name, strrchr(path, '/') + 1);
                node = new_node(lfs, base_node, file_name, &item);

                if (node == NULL) {
                        free(file_name);
                        goto error;
                }
        }

        /* file shall exist */
        if (!node || !base_node || item < 0) {
                goto error;
        }

        /* node must be a file */
        if (node->type == NODE_TYPE_DIR) {
                errno = EISDIR;
                goto exit;
        }

        /* add file to list of open files */
        if (add_node_to_list_of_open_files(lfs, base_node, node, &item) == STD_RET_ERROR) {
                goto error;
        }

        /* set file parameters */
        if (node->type == NODE_TYPE_FILE) {
                if ((flags & O_CREAT) && !(flags & O_APPEND)) {
                        if (node->data) {
                                free(node->data);
                                node->data = NULL;
                        }

                        node->size = 0;
                }

                if (!(flags & O_APPEND)) {
                        *fpos = 0;
                } else {
                        *fpos = node->size;
                }
        } else if (node->type == NODE_TYPE_DRV) {
                if (driver_open((dev_t)node->data, O_DEV_FLAGS(flags)) == STD_RET_OK) {
                        *fpos = 0;
                } else {
                        list_rm_nitem(lfs->list_of_opended_files, item);
                        goto error;
                }
        }

        /* everything success - load FD */
        u32_t cfd;
        list_get_nitem_ID(lfs->list_of_opended_files, item, &cfd);
        *fd = (fd_t)cfd;
        mutex_unlock(lfs->resource_mtx);
        return STD_RET_OK;

error:
        errno = ENOENT;
exit:
        mutex_unlock(lfs->resource_mtx);
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
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
API_FS_CLOSE(lfs, void *fs_handle, void *extra, fd_t fd, bool force)
{
        UNUSED_ARG(extra);

        STOP_IF(!fs_handle);

        struct LFS_data *lfs = fs_handle;
        stdret_t status      = STD_RET_ERROR;

        mutex_force_lock(lfs->resource_mtx);

        struct opened_file_info *opened_file = list_get_iditem_data(lfs->list_of_opended_files, fd);
        if (opened_file == NULL) {
                goto exit;
        }

        node_t *node = opened_file->node;
        if (node == NULL) {
                goto exit;
        }

        /* close device if file is driver type */
        if (node->type == NODE_TYPE_DRV) {
                if ((status = driver_close((dev_t)node->data, force)) != STD_RET_OK) {
                        goto exit;
                }
        }

        /* delete file from open list */
        struct opened_file_info opened_file_data = *opened_file;

        if (list_rm_iditem(lfs->list_of_opended_files, fd) != STD_RET_OK) {
                /* critical error! */
                goto exit;
        }

        /* file to remove, check if other task does not opens this file */
        status = STD_RET_OK;

        if (opened_file_data.remove_at_close == true) {
                i32_t n = list_get_item_count(lfs->list_of_opended_files);

                for (int i = 0; i < n; i++) {
                        opened_file = list_get_nitem_data(lfs->list_of_opended_files, i);

                        if (opened_file->node == node) {
                                goto exit;
                        }
                }

                /* file can be removed */
                status = delete_node(opened_file_data.base_node,
                                     opened_file_data.node,
                                     opened_file_data.item_ID);
        }

exit:
        errno = ENOENT;

        mutex_unlock(lfs->resource_mtx);
        return status;
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
 * @param[in ]           fattr                  file attributes
 *
 * @return number of written bytes, -1 if error
 */
//==============================================================================
API_FS_WRITE(lfs, void *fs_handle, void *extra, fd_t fd, const u8_t *src, size_t count, u64_t *fpos, struct vfs_fattr fattr)
{
        UNUSED_ARG(extra);

        STOP_IF(!fs_handle);
        STOP_IF(!fpos);
        STOP_IF(!src);
        STOP_IF(!count);

        struct LFS_data *lfs = fs_handle;
        ssize_t          n   = -1;

        mutex_force_lock(lfs->resource_mtx);

        struct opened_file_info *opened_file = list_get_iditem_data(lfs->list_of_opended_files, fd);
        if (!opened_file) {
                errno = ENOENT;
                goto exit;
        }

        node_t *node = opened_file->node;
        if (!node) {
                errno = ENOENT;
                goto exit;
        }

        if (node->type == NODE_TYPE_DRV) {
                mutex_unlock(lfs->resource_mtx);
                return driver_write((dev_t)node->data, src, count, fpos, fattr);
        } else if (node->type == NODE_TYPE_FILE) {
                size_t write_size  = count;
                size_t file_length = node->size;
                size_t seek        = *fpos > SIZE_MAX ? SIZE_MAX : *fpos;

                if (seek > file_length) {
                        seek = file_length;
                }

                if ((seek + write_size) > file_length || node->data == NULL) {
                        char *new_data = malloc(file_length + write_size);
                        if (new_data == NULL) {
                                errno = ENOSPC;
                                goto exit;
                        }

                        if (node->data) {
                                memcpy(new_data, node->data, file_length);
                                free(node->data);
                        }

                        memcpy(new_data + seek, src, write_size);

                        node->data  = new_data;
                        node->size += write_size - (file_length - seek);

                        n = count;
                } else {
                        memcpy(node->data + seek, src, write_size);
                        n = count;
                }
        } else if (node->type == NODE_TYPE_PIPE) {
                mutex_unlock(lfs->resource_mtx);

                n = pipe_write(node->data, src, count, fattr.non_blocking_wr);

                if (n > 0) {
                        node->size = pipe_get_length(node->data);
                }

        } else {
                errno = ENOENT;
        }

exit:
        mutex_unlock(lfs->resource_mtx);
        return n;
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
 * @param[in ]           fattr                  file attributes
 *
 * @return number of read bytes, -1 if error
 */
//==============================================================================
API_FS_READ(lfs, void *fs_handle, void *extra, fd_t fd, u8_t *dst, size_t count, u64_t *fpos, struct vfs_fattr fattr)
{
        UNUSED_ARG(extra);

        STOP_IF(!fs_handle);
        STOP_IF(!fpos);
        STOP_IF(!dst);
        STOP_IF(!count);

        struct LFS_data *lfs = fs_handle;
        ssize_t          n   = -1;

        mutex_force_lock(lfs->resource_mtx);

        struct opened_file_info *opened_file = list_get_iditem_data(lfs->list_of_opended_files, fd);
        if (opened_file == NULL) {
                goto exit;
        }

        node_t *node = opened_file->node;
        if (node == NULL) {
                goto exit;
        }

        if (node->type == NODE_TYPE_DRV) {
                mutex_unlock(lfs->resource_mtx);
                return driver_read((dev_t)node->data, dst, count, fpos, fattr);
        } else if (node->type == NODE_TYPE_FILE) {
                size_t file_length = node->size;
                size_t seek        = *fpos > SIZE_MAX ? SIZE_MAX : *fpos;

                /* check if seek is not bigger than file length */
                if (seek > file_length) {
                        seek = file_length;
                }

                /* check how many items to read is on current file position */
                size_t items_to_read;
                if ((file_length - seek) >= count) {
                        items_to_read = count;
                } else {
                        items_to_read = file_length - seek;
                }

                /* copy if file buffer exist */
                if (node->data) {
                        if (items_to_read > 0) {
                                memcpy(dst, node->data + seek, items_to_read);
                                n = items_to_read;
                        } else {
                                n = 0;
                        }
                } else {
                        errno = EIO;
                }
        } else if (node->type == NODE_TYPE_PIPE) {
                mutex_unlock(lfs->resource_mtx);

                n = pipe_read(node->data, dst, count, fattr.non_blocking_rd);

                if (n > 0) {
                        node->size = pipe_get_length(node->data);
                }
        } else {
                errno = EIO;
        }

exit:
        mutex_unlock(lfs->resource_mtx);
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
API_FS_IOCTL(lfs, void *fs_handle, void *extra, fd_t fd, int request, void *arg)
{
        UNUSED_ARG(extra);

        STOP_IF(!fs_handle);

        struct LFS_data *lfs = fs_handle;

        mutex_force_lock(lfs->resource_mtx);

        struct opened_file_info *opened_file = list_get_iditem_data(lfs->list_of_opended_files, fd);
        if (opened_file == NULL) {
                goto error;
        }

        if (opened_file->node == NULL) {
                goto error;
        }

        if (opened_file->node->type == NODE_TYPE_DRV) {
                mutex_unlock(lfs->resource_mtx);
                return driver_ioctl((dev_t)opened_file->node->data, request, arg);
        } else if (opened_file->node->type == NODE_TYPE_PIPE) {

                if (request != PIPE_CLOSE) {
                        errno = EBADRQC;
                        goto exit;
                }

                mutex_unlock(lfs->resource_mtx);
                return pipe_close(opened_file->node->data) ? STD_RET_OK : STD_RET_ERROR;
        }

error:
        errno = ENOENT;

exit:
        mutex_unlock(lfs->resource_mtx);
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
API_FS_FLUSH(lfs, void *fs_handle, void *extra, fd_t fd)
{
        UNUSED_ARG(extra);

        STOP_IF(!fs_handle);

        struct LFS_data *lfs = fs_handle;

        mutex_force_lock(lfs->resource_mtx);

        struct opened_file_info *opened_file = list_get_iditem_data(lfs->list_of_opended_files, fd);
        if (opened_file == NULL) {
                goto error;
        }

        if (opened_file->node == NULL) {
                goto error;
        }

        if (opened_file->node->type == NODE_TYPE_DRV) {
                mutex_unlock(lfs->resource_mtx);
                return driver_flush((dev_t)opened_file->node->data);
        }

error:
        errno = ENOENT;
        mutex_unlock(lfs->resource_mtx);
        return STD_RET_ERROR;
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
 * @brief Return first character of selected string
 */
//==============================================================================
static inline char get_first_char(const char *str)
{
        return FIRST_CHARACTER(str);
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
        while (mutex_lock(mtx, MTX_BLOCK_TIME) != true);
}

//==============================================================================
/**
 * @brief Remove selected node
 *
 * @param[in] *base             base node
 * @param[in] *target           target node
 * @param[in]  baseitemid       item in base node that point to target
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
static stdret_t delete_node(node_t *base, node_t *target, u32_t baseitemid)
{
        if (target->type == NODE_TYPE_DIR) {

                if (list_get_item_count(target->data) > 0) {
                        return STD_RET_ERROR;
                } else {
                        list_delete(target->data);
                        target->data = NULL;
                }
        } else if (target->type == NODE_TYPE_PIPE) {

                if (target->data) {
                        pipe_delete(target->data);
                        target->data = NULL;
                }
        }

        if (target->name) {
                free(target->name);
        }

        if (target->data) {
                free(target->data);
        }

        if (list_rm_iditem(base->data, baseitemid) == STD_RET_OK) {
                return STD_RET_OK;
        }

        return STD_RET_ERROR;
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
 *        ERRNO: ENOENT
 *
 * @param[in]  path             path
 * @param[in]  startnode        start node
 * @param[out] extPath          external path begin (pointer from path)
 * @param[in]  deep             deep control
 * @param[out] item             node is n-item of list which was found
 *
 * @return node
 */
//==============================================================================
static node_t *get_node(const char *path, node_t *startnode, i32_t deep, i32_t *item)
{
        if (!path || !startnode) {
                errno = ENOENT;
                return NULL;
        }

        if (startnode->type != NODE_TYPE_DIR) {
                errno = ENOENT;
                return NULL;
        }

        node_t *current_node = startnode;
        int     dir_deep     = get_path_deep(path);

        /* go to selected node -----------------------------------------------*/
        while (dir_deep + deep > 0) {
                /* get element from path */
                if ((path = strchr(path, '/')) == NULL) {
                        break;
                } else {
                        path++;
                }

                uint  path_length;
                char *path_end;
                if ((path_end = strchr(path, '/')) == NULL) {
                        path_length = strlen(path);
                } else {
                        path_length = path_end - path;
                }

                /* get number of list items */
                int list_size = list_get_item_count(current_node->data);

                /* find that object exist ------------------------------------*/
                int i = 0;
                while (list_size > 0) {
                        node_t *next_node = list_get_nitem_data(current_node->data, i++);

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
                        errno        = ENOENT;
                        current_node = NULL;
                        break;
                }

                dir_deep--;
        }

        return current_node;
}

//==============================================================================
/**
 * @brief Function create new file in selected node
 * Function allocate new node. If node is created successfully then filename
 * cannot be freed!
 * ERRNO: ENOENT, ENOTDIR, ENOMEM
 *
 * @param [in] *lfs             pointer to current LFS instance
 * @param [in] *nodebase        node base
 * @param [in] *filename        filename (must be earlier allocated)
 * @param[out] *item            new node number in base node
 *
 * @return NULL if error, otherwise new node address
 */
//==============================================================================
static node_t *new_node(struct LFS_data *lfs, node_t *nodebase, char *filename, i32_t *item)
{
        if (!nodebase || !filename) {
                errno = ENOENT;
                return NULL;
        }

        if (nodebase->type != NODE_TYPE_DIR) {
                errno = ENOTDIR;
                return NULL;
        }

        node_t *node;
        if ((node = calloc(1, sizeof(node_t))) == NULL) {
                return NULL;
        }

        node->name  = filename;
        node->data  = NULL;
        node->gid   = 0;
        node->mode  = 0;
        node->mtime = 0;
        node->size  = 0;
        node->type  = NODE_TYPE_FILE;
        node->uid   = 0;

        i32_t   node_number;
        if ((node_number = list_add_item(nodebase->data, lfs->id_counter++, node)) < 0) {
                free(node);
                errno = ENOENT;
                return NULL;
        }

        *item = node_number;
        return node;
}

//==============================================================================
/**
 * @brief Function add node to list of open files
 *        ERRNO: ENOMEM
 *
 * @param [in] *lfs             pointer to current LFS instance
 * @param [in] *base_node       base node
 * @param [in] *node            node data added to list of open files
 * @param [io] *item            in:  node number in base node
 *                              out: open file's number in list of open files
 *
 * @retval STD_RET_OK           file registered in list of open files
 * @retval STD_RET_ERROR        file not registered
 */
//==============================================================================
static stdret_t add_node_to_list_of_open_files(struct LFS_data *lfs, node_t *base_node,
                                               node_t *node, i32_t *item)
{
        struct opened_file_info *opened_file_info;
        if (!(opened_file_info = calloc(1, sizeof(struct opened_file_info)))) {
                return STD_RET_ERROR;
        }

        opened_file_info->remove_at_close = false;
        opened_file_info->node            = node;
        opened_file_info->base_node       = base_node;

        if (list_get_nitem_ID(base_node->data, *item, &opened_file_info->item_ID) != STD_RET_OK) {
                goto error;
        }

        /* find if file shall be removed */
        i32_t open_file_count = list_get_item_count(lfs->list_of_opended_files);

        for (i32_t i = 0; i < open_file_count; i++) {
                struct opened_file_info *opened_file = list_get_nitem_data(lfs->list_of_opended_files, i);

                if (opened_file->node != node) {
                        continue;
                }

                if (opened_file->remove_at_close == true) {
                        opened_file_info->remove_at_close = true;
                        break;
                }
        }

        /* add open file info to list */
        i32_t open_file_list_position = list_add_item(lfs->list_of_opended_files,
                                                      lfs->id_counter++,
                                                      opened_file_info);

        if (open_file_list_position >= 0) {
                *item = open_file_list_position;
                return STD_RET_OK;
        }

error:
        errno = ENOMEM;
        free(opened_file_info);
        return STD_RET_ERROR;
}

/*==============================================================================
  End of file
==============================================================================*/
