/*=========================================================================*//**
@file    devfs.c

@author  Daniel Zorychta

@brief   This file support device file system.

@note    Copyright (C) 2013 Daniel Zorychta <daniel.zorychta@gmail.com>

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
  Local macros
==============================================================================*/
#define CHAIN_NUMBER_OF_NODES           8
#define TIMEOUT_MS                      MAX_DELAY_MS

/*==============================================================================
  Local object types
==============================================================================*/
struct devfs_chain {
        struct devnode {
                union {
                        dev_t             drv;
                        pipe_t           *pipe;
                        void             *generic;
                } IF;
                char                     *path;
                tfile_t                   type;
                gid_t                     gid;
                uid_t                     uid;
                mode_t                    mode;
                int                       opended;
        } devnode[CHAIN_NUMBER_OF_NODES];

        struct devfs_chain *next;
};

struct devfs {
        struct devfs_chain *root_chain;
        mutex_t            *mutex;
        int                 number_of_opened_files;
        int                 number_of_chains;
        int                 number_of_used_nodes;
};

/*==============================================================================
  Local function prototypes
==============================================================================*/
static int  chain_create                 (struct devfs_chain **chain);
static void chain_destroy                (struct devfs_chain *chain);
static int  chain_get_node_by_path       (struct devfs_chain *chain, const char *path, struct devnode **node);
static int  chain_get_empty_node         (struct devfs_chain *chain, struct devnode **node);
static int  chain_get_n_node             (struct devfs_chain *chain, int n, struct devnode **node);
static int  create_new_chain_if_necessary(struct devfs *devfs);

/*==============================================================================
  Local objects
==============================================================================*/

/*==============================================================================
  Exported objects
==============================================================================*/

/*==============================================================================
  External objects
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
 * @return One of errno value (errno.h).
 */
//==============================================================================
API_FS_INIT(devfs, void **fs_handle, const char *src_path, const char *opts)
{
        UNUSED_ARG2(src_path, opts);

        int err = sys_zalloc(sizeof(struct devfs), fs_handle);
        if (err == ESUCC) {
                struct devfs *devfs = *fs_handle;

                err = sys_mutex_create(MUTEX_TYPE_NORMAL, &devfs->mutex);
                if (err != ESUCC)
                        goto finish;

                err = chain_create(&devfs->root_chain);
                if (err != ESUCC)
                        goto finish;

                devfs->number_of_chains       = 1;
                devfs->number_of_opened_files = 0;
                devfs->number_of_used_nodes   = 0;

                finish:
                if (err != ESUCC) {
                        if (devfs->mutex)
                                sys_mutex_destroy(devfs->mutex);

                        if (devfs->root_chain)
                                chain_destroy(devfs->root_chain);

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
API_FS_RELEASE(devfs, void *fs_handle)
{
        struct devfs *devfs = fs_handle;

        int err = sys_mutex_lock(devfs->mutex, 100);
        if (!err) {
                if (devfs->number_of_opened_files != 0) {
                        sys_mutex_unlock(devfs->mutex);
                        err = EBUSY;

                } else {
                        chain_destroy(devfs->root_chain);

                        mutex_t *mtx = devfs->mutex;
                        memset(devfs, 0, sizeof(struct devfs));

                        sys_mutex_unlock(mtx);
                        sys_mutex_destroy(mtx);

                        sys_free(fs_handle);
                }
        }

        return err;
}

//==============================================================================
/**
 * @brief Open file
 *
 * @param[in ]          *fs_handle              file system allocated memory
 * @param[out]          *fhdl                   file extra data
 * @param[out]          *fpos                   file position
 * @param[in]           *path                   file path
 * @param[in]            flags                  file open flags
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
API_FS_OPEN(devfs, void *fs_handle, void **fhdl, fpos_t *fpos, const char *path, u32_t flags)
{
        struct devfs *devfs  = fs_handle;

        int err = sys_mutex_lock(devfs->mutex, TIMEOUT_MS);
        if (err == ESUCC) {

                struct devnode *node;
                err = chain_get_node_by_path(devfs->root_chain, path, &node);
                if (err == ESUCC) {
                        int open;
                        if (node->type == FILE_TYPE_DRV) {
                                open = sys_driver_open(node->IF.drv, flags);
                        } else if (node->type == FILE_TYPE_PIPE) {
                                open = ESUCC;
                        } else {
                                open = ENOENT;
                        }

                        if (open == ESUCC) {
                                *fhdl = node;
                                *fpos = 0;
                                devfs->number_of_opened_files++;
                                node->opended++;
                        }
                }

                sys_mutex_unlock(devfs->mutex);
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
API_FS_CLOSE(devfs, void *fs_handle, void *fhdl, bool force)
{
        struct devfs   *devfs  = fs_handle;
        struct devnode *node   = fhdl;

        int err;
        if (node->type == FILE_TYPE_DRV) {
                err = sys_driver_close(node->IF.drv, force);
        } else if (node->type == FILE_TYPE_PIPE) {
                err = sys_pipe_close(node->IF.pipe);
        } else {
                err = ENOENT;
        }

        if (err == ESUCC) {

                err = sys_mutex_lock(devfs->mutex, TIMEOUT_MS);
                if (err == ESUCC) {
                        devfs->number_of_opened_files--;
                        node->opended--;
                        sys_mutex_unlock(devfs->mutex);
                }
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
API_FS_WRITE(devfs,
             void            *fs_handle,
             void            *fhdl,
             fd_t             fd,
             const u8_t      *src,
             size_t           count,
             fpos_t          *fpos,
             size_t          *wrcnt,
             struct vfs_fattr fattr)
{
        UNUSED_ARG1(fs_handle);
        UNUSED_ARG1(fd);

        struct devnode *node = fhdl;

        if (node->type == FILE_TYPE_DRV) {
                return sys_driver_write(node->IF.drv, src, count, fpos, wrcnt, fattr);
        } else if (node->type == FILE_TYPE_PIPE) {
                return sys_pipe_write(node->IF.pipe, src, count, wrcnt, fattr.non_blocking_wr);
        } else {
                return ENXIO;
        }
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
API_FS_READ(devfs,
            void            *fs_handle,
            void            *fhdl,
            u8_t            *dst,
            size_t           count,
            fpos_t          *fpos,
            size_t          *rdcnt,
            struct vfs_fattr fattr)
{
        UNUSED_ARG1(fs_handle);

        struct devnode *node = fhdl;

        if (node->type == FILE_TYPE_DRV) {
                return sys_driver_read(node->IF.drv, dst, count, fpos, rdcnt, fattr);
        } else if (node->type == FILE_TYPE_PIPE) {
                return sys_pipe_read(node->IF.pipe, dst, count, rdcnt, fattr.non_blocking_rd);
        } else {
                return ENXIO;
        }
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
API_FS_IOCTL(devfs, void *fs_handle, void *fhdl, int request, void *arg)
{
        UNUSED_ARG1(fs_handle);

        struct devnode *node = fhdl;

        if (node->type == FILE_TYPE_DRV) {
                return sys_driver_ioctl(node->IF.drv, request, arg);
        } else if (node->type == FILE_TYPE_PIPE && request == IOCTL_PIPE__CLOSE) {
                return sys_pipe_close(node->IF.pipe) ? ESUCC : EINVAL;
        } else if (node->type == FILE_TYPE_PIPE && request == IOCTL_PIPE__CLEAR) {
                return sys_pipe_clear(node->IF.pipe) ? ESUCC : EINVAL;
        } else {
                return EBADRQC;
        }
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
API_FS_FLUSH(devfs, void *fs_handle, void *fhdl)
{
        UNUSED_ARG1(fs_handle);

        struct devnode *node = fhdl;

        if (node->type == FILE_TYPE_DRV) {
                return sys_driver_flush(node->IF.drv);
        } else {
                return EINVAL;
        }
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
API_FS_FSTAT(devfs, void *fs_handle, void *fhdl, struct stat *stat)
{
        UNUSED_ARG1(fs_handle);

        struct devnode *node = fhdl;

        int err = EINVAL;

        time_t t = 0;
        sys_get_time(&t);

        stat->st_ctime = t;
        stat->st_mtime = t;
        stat->st_gid   = node->gid;
        stat->st_uid   = node->uid;
        stat->st_mode  = node->mode;

        if (node->type == FILE_TYPE_DRV) {
                struct vfs_dev_stat devstat;
                err = sys_driver_stat(node->IF.drv, &devstat);
                if (err == ESUCC) {
                        stat->st_dev  = node->IF.drv;
                        stat->st_size = devstat.st_size;
                        stat->st_type = FILE_TYPE_DRV;
                }
        } else if (node->type == FILE_TYPE_PIPE) {
                size_t pipelen;
                err = sys_pipe_get_length(node->IF.pipe, &pipelen);
                if (err == ESUCC) {
                        stat->st_dev  = 0;
                        stat->st_size = pipelen;
                        stat->st_type = FILE_TYPE_PIPE;
                }
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
API_FS_MKDIR(devfs, void *fs_handle, const char *path, mode_t mode)
{
        UNUSED_ARG1(fs_handle);
        UNUSED_ARG1(path);
        UNUSED_ARG1(mode);

        return ENOTSUP;
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
API_FS_MKFIFO(devfs, void *fs_handle, const char *path, mode_t mode)
{
        struct devfs *devfs  = fs_handle;

        int err = sys_mutex_lock(devfs->mutex, TIMEOUT_MS);
        if (err == ESUCC) {

                err = create_new_chain_if_necessary(devfs);
                if (err == ESUCC) {

                        struct devnode *node;
                        err = chain_get_empty_node(devfs->root_chain, &node);
                        if (err == ESUCC) {

                                err = sys_pipe_create(&node->IF.pipe);
                                if (err == ESUCC) {

                                        err = sys_malloc(strlen(path + 1) + 1,
                                                             cast(void**, &node->path));
                                        if (err == ESUCC) {

                                                strcpy(node->path, path + 1);
                                                node->gid  = 0;
                                                node->uid  = 0;
                                                node->mode = mode;
                                                node->type = FILE_TYPE_PIPE;

                                                devfs->number_of_used_nodes++;
                                        } else {
                                                sys_pipe_destroy(node->IF.pipe);
                                        }
                                }
                        }
                }

                sys_mutex_unlock(devfs->mutex);
        }

        return err;
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
API_FS_MKNOD(devfs, void *fs_handle, const char *path, const dev_t dev)
{
        struct devfs *devfs  = fs_handle;

        int err = sys_mutex_lock(devfs->mutex, TIMEOUT_MS);
        if (err == ESUCC) {

                err = create_new_chain_if_necessary(devfs);
                if (err == ESUCC) {

                        struct devnode *node;
                        err = chain_get_empty_node(devfs->root_chain, &node);
                        if (err == ESUCC) {

                                err = sys_malloc(strlen(path + 1) + 1, cast(void**, &node->path));
                                if (err == ESUCC) {

                                        strcpy(node->path, path + 1);
                                        node->IF.drv = dev;
                                        node->gid    = 0;
                                        node->uid    = 0;
                                        node->mode   = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH;
                                        node->type   = FILE_TYPE_DRV;

                                        devfs->number_of_used_nodes++;
                                }
                        }
                }

                sys_mutex_unlock(devfs->mutex);
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
API_FS_OPENDIR(devfs, void *fs_handle, const char *path, DIR *dir)
{
        struct devfs *devfs = fs_handle;

        if (strcmp(path, "/") == 0) {
                dir->d_hdl   = NULL;
                dir->d_items = devfs->number_of_used_nodes;
                dir->d_seek  = 0;

                return ESUCC;
        } else {
                return ENOENT;
        }
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
API_FS_CLOSEDIR(devfs, void *fs_handle, DIR *dir)
{
        UNUSED_ARG2(fs_handle, dir);
        return ESUCC;
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
API_FS_READDIR(devfs, void *fs_handle, DIR *dir)
{
        struct devfs *devfs  = fs_handle;

        int err = sys_mutex_lock(devfs->mutex, TIMEOUT_MS);
        if (err == ESUCC) {

                struct devnode *node;
                err = chain_get_n_node(devfs->root_chain, dir->d_seek, &node);
                if (err == ESUCC) {

                        if (node->type == FILE_TYPE_DRV) {
                                struct vfs_dev_stat devstat;
                                err = sys_driver_stat(node->IF.drv, &devstat);
                                if (err == ESUCC) {
                                        dir->dirent.size = devstat.st_size;
                                } else {
                                        dir->dirent.size = 0;
                                }
                                dir->dirent.dev      = node->IF.drv;
                                dir->dirent.filetype = FILE_TYPE_DRV;

                        } else if (node->type == FILE_TYPE_PIPE) {
                                size_t n;
                                err = sys_pipe_get_length(node->IF.pipe, &n);
                                if (err == ESUCC) {
                                        dir->dirent.size = n;
                                } else {
                                        dir->dirent.size = 0;
                                }

                                dir->dirent.filetype = FILE_TYPE_PIPE;
                        } else {
                                err = EINVAL;
                        }

                        dir->dirent.name = node->path;
                        dir->d_seek++;
                }

                sys_mutex_unlock(devfs->mutex);
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
API_FS_REMOVE(devfs, void *fs_handle, const char *path)
{
        struct devfs *devfs  = fs_handle;

        int err = sys_mutex_lock(devfs->mutex, TIMEOUT_MS);
        if (err == ESUCC) {

                struct devnode *node;
                err = chain_get_node_by_path(devfs->root_chain, path, &node);
                if (err == ESUCC) {

                        if (node->opended == 0) {
                                if (node->type == FILE_TYPE_PIPE) {
                                        sys_pipe_destroy(node->IF.pipe);
                                }
                                node->IF.generic = NULL;

                                sys_free(cast(void**, &node->path));
                                node->path  = NULL;
                                node->gid   = 0;
                                node->uid   = 0;
                                node->mode  = 0;

                                devfs->number_of_used_nodes--;
                        } else {
                                err = EBUSY;
                        }
                }

                sys_mutex_unlock(devfs->mutex);
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
API_FS_RENAME(devfs, void *fs_handle, const char *old_name, const char *new_name)
{
        struct devfs *devfs  = fs_handle;

        int err = sys_mutex_lock(devfs->mutex, TIMEOUT_MS);
        if (err == ESUCC) {

                struct devnode *node;
                err = chain_get_node_by_path(devfs->root_chain, old_name, &node);
                if (err == ESUCC) {

                        char *name;
                        err = sys_malloc(strlen(new_name) + 1, cast(void**, &name));
                        if (err == ESUCC) {
                                strcpy(name, new_name);
                                sys_free(cast(void**, &node->path));
                                node->path = name;
                        } else {
                                err = ENOSPC;
                        }
                }

                sys_mutex_unlock(devfs->mutex);
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
API_FS_CHMOD(devfs, void *fs_handle, const char *path, mode_t mode)
{
        struct devfs *devfs  = fs_handle;

        int err = sys_mutex_lock(devfs->mutex, TIMEOUT_MS);
        if (err == ESUCC) {

                struct devnode *node;
                err = chain_get_node_by_path(devfs->root_chain, path, &node);
                if (err == ESUCC) {
                        node->mode = mode;
                }

                sys_mutex_unlock(devfs->mutex);
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
API_FS_CHOWN(devfs, void *fs_handle, const char *path, uid_t owner, gid_t group)
{
        struct devfs *devfs  = fs_handle;

        int err = sys_mutex_lock(devfs->mutex, TIMEOUT_MS);
        if (err == ESUCC) {

                struct devnode *node;
                err = chain_get_node_by_path(devfs->root_chain, path, &node);
                if (err == ESUCC) {
                        node->uid = owner;
                        node->gid = group;
                }

                sys_mutex_unlock(devfs->mutex);
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
API_FS_STAT(devfs, void *fs_handle, const char *path, struct stat *stat)
{
        struct devfs *devfs  = fs_handle;

        int err = sys_mutex_lock(devfs->mutex, TIMEOUT_MS);
        if (err == ESUCC) {

                struct devnode *node;
                err = chain_get_node_by_path(devfs->root_chain, path, &node);
                if (err == ESUCC) {
                        err = _devfs_fstat(devfs, node, stat);
                }

                sys_mutex_unlock(devfs->mutex);
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
API_FS_STATFS(devfs, void *fs_handle, struct statfs *statfs)
{
        struct devfs *devfs = fs_handle;

        statfs->f_bsize  = sizeof(struct devnode);
        statfs->f_blocks = devfs->number_of_chains * CHAIN_NUMBER_OF_NODES;
        statfs->f_bfree  = statfs->f_blocks - devfs->number_of_used_nodes;
        statfs->f_ffree  = statfs->f_blocks - devfs->number_of_used_nodes;
        statfs->f_files  = devfs->number_of_used_nodes;
        statfs->f_type   = SYS_FS_TYPE__DEV;
        statfs->f_fsname = "devfs";

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
API_FS_SYNC(devfs, void *fs_handle)
{
        UNUSED_ARG1(fs_handle);
        return ESUCC;
}

//==============================================================================
/**
 * @brief Return node pointer
 *
 * @param[in]  chain            chain
 * @param[in]  path             node's path
 * @param[out] node             found node
 *
 * @retval ESUCC
 * @retval ENOENT
 */
//==============================================================================
static int chain_get_node_by_path(struct devfs_chain *chain, const char *path, struct devnode **node)
{
        for (struct devfs_chain *nchain = chain; nchain != NULL; nchain = nchain->next) {
                for (int i = 0; i < CHAIN_NUMBER_OF_NODES; i++) {
                        if (nchain->devnode[i].path == NULL) {
                                continue;
                        }

                        if (strcmp(nchain->devnode[i].path, path + 1) == 0) {
                                *node = &nchain->devnode[i];
                                return ESUCC;
                        }
                }
        }

        return ENOENT;
}

//==============================================================================
/**
 * @brief Return node pointer
 *
 * @param[in]  chain            chain
 * @param[out] node             found node
 *
 * @retval ESUCC
 * @retval ENOENT
 */
//==============================================================================
static int chain_get_empty_node(struct devfs_chain *chain, struct devnode **node)
{
        for (struct devfs_chain *nchain = chain; nchain != NULL; nchain = nchain->next) {
                for (int i = 0; i < CHAIN_NUMBER_OF_NODES; i++) {
                        if (nchain->devnode[i].path == NULL) {
                                *node = &nchain->devnode[i];
                                return ESUCC;
                        }
                }
        }

        return ENOENT;
}

//==============================================================================
/**
 * @brief Return node pointer
 *
 * @param[in]  devfs            file system memory
 * @param[in]  n                node number
 * @param[out] node             found node
 *
 * @retval ESUCC
 * @retval ENOENT
 */
//==============================================================================
static int chain_get_n_node(struct devfs_chain *chain, int n, struct devnode **node)
{
        int n_node = 0;

        for (struct devfs_chain *nchain = chain; nchain != NULL; nchain = nchain->next) {
                for (int i = 0; i < CHAIN_NUMBER_OF_NODES; i++) {
                        if (nchain->devnode[i].path != NULL) {
                                if (n_node++ == n) {
                                        *node = &nchain->devnode[i];
                                        return ESUCC;
                                }
                        }
                }
        }

        return ENOENT;
}

//==============================================================================
/**
 * @brief Create new chain for nodes
 *
 * @param[out] chain    pointer to chain object pointer
 *
 * @return One of errno value
 */
//==============================================================================
static int chain_create(struct devfs_chain **chain)
{
        return sys_zalloc(sizeof(struct devfs_chain), cast(void**, chain));
}

//==============================================================================
/**
 * @brief Delete chain
 *
 * @param[in] *chain            chain to delete
 */
//==============================================================================
static void chain_destroy(struct devfs_chain *chain)
{
        if (chain->next) {
                chain_destroy(chain->next);
        }

        for (int i = 0; i < CHAIN_NUMBER_OF_NODES; i++) {
                if (chain->devnode[i].IF.generic) {
                        sys_free(&chain->devnode[i].IF.generic);
                }

                if (chain->devnode[i].path) {
                        sys_free(cast(void**, &chain->devnode[i].path));
                }
        }

        sys_free(cast(void**, &chain));
}

//==============================================================================
/**
 * @brief Function create new chain if no empty nodes exist
 *
 * @param devfs         file system object
 *
 * @retval ESUCC        new chain created or exist
 * @retval ENOSPC       no space on disc (no memory)
 */
//==============================================================================
static int create_new_chain_if_necessary(struct devfs *devfs)
{
        if (devfs->number_of_chains * CHAIN_NUMBER_OF_NODES == devfs->number_of_used_nodes) {
                struct devfs_chain *chain = devfs->root_chain;
                while (chain->next != NULL) {
                        chain = chain->next;
                }

                if (chain_create(&chain->next) == ESUCC) {
                        devfs->number_of_chains++;
                } else {
                        return ENOSPC;
                }
        }

        return ESUCC;
}

/*==============================================================================
  End of file
==============================================================================*/
