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
static int      closedir                     (void *fs_handle, DIR *dir);
static int      readdir                      (void *fs_handle, DIR *dir, dirent_t **dirent);
static int      chain_create                 (struct devfs_chain **chain);
static void     chain_destroy                (struct devfs_chain *chain);
static int      chain_get_node_by_path       (struct devfs_chain *chain, const char *path, struct devnode **node);
static int      chain_get_empty_node         (struct devfs_chain *chain, struct devnode **node);
static int      chain_get_n_node             (struct devfs_chain *chain, int n, struct devnode **node);
static int      create_new_chain_if_necessary(struct devfs *devfs);

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
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
API_FS_INIT(devfs, void **fs_handle, const char *src_path)
{
        UNUSED_ARG1(src_path);

        int result = sys_zalloc(sizeof(struct devfs), fs_handle);
        if (result == ESUCC) {
                struct devfs *devfs = *fs_handle;

                result = sys_mutex_create(MUTEX_TYPE_NORMAL, &devfs->mutex);
                if (result != ESUCC)
                        goto finish;

                result = chain_create(&devfs->root_chain);
                if (result != ESUCC)
                        goto finish;

                devfs->number_of_chains       = 1;
                devfs->number_of_opened_files = 0;
                devfs->number_of_used_nodes   = 0;

                finish:
                if (result != ESUCC) {
                        if (devfs->mutex)
                                sys_mutex_destroy(devfs->mutex);

                        if (devfs->root_chain)
                                chain_destroy(devfs->root_chain);

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
API_FS_RELEASE(devfs, void *fs_handle)
{
        struct devfs *devfs = fs_handle;

        if (sys_mutex_lock(devfs->mutex, 100) == ESUCC) {
                if (devfs->number_of_opened_files != 0) {
                        sys_mutex_unlock(devfs->mutex);
                        return EBUSY;
                }

                sys_critical_section_begin();
                {
                        sys_mutex_unlock(devfs->mutex);
                        chain_destroy(devfs->root_chain);
                        sys_mutex_destroy(devfs->mutex);
                        sys_free(fs_handle);
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
 * @param[out]          *extra                  file extra data
 * @param[out]          *fd                     file descriptor
 * @param[out]          *fpos                   file position
 * @param[in]           *path                   file path
 * @param[in]            flags                  file open flags
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
API_FS_OPEN(devfs, void *fs_handle, void **extra, fd_t *fd, fpos_t *fpos, const char *path, u32_t flags)
{
        UNUSED_ARG1(fd);

        struct devfs *devfs  = fs_handle;

        int status = sys_mutex_lock(devfs->mutex, TIMEOUT_MS);
        if (status == ESUCC) {

                struct devnode *node;
                status = chain_get_node_by_path(devfs->root_chain, path, &node);
                if (status == ESUCC) {
                        int open;
                        if (node->type == FILE_TYPE_DRV) {
                                open = sys_driver_open(node->IF.drv, flags);
                        } else if (node->type == FILE_TYPE_PIPE) {
                                open = ESUCC;
                        } else {
                                open = ENOENT;
                        }

                        if (open == ESUCC) {
                                *extra = node;
                                *fpos  = 0;
                                devfs->number_of_opened_files++;
                                node->opended++;
                        }
                }

                sys_mutex_unlock(devfs->mutex);
        }

        return status;
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
API_FS_CLOSE(devfs, void *fs_handle, void *extra, fd_t fd, bool force)
{
        UNUSED_ARG1(fd);

        struct devfs   *devfs  = fs_handle;
        struct devnode *node   = extra;

        int status;
        if (node->type == FILE_TYPE_DRV) {
                status = sys_driver_close(node->IF.drv, force);
        } else if (node->type == FILE_TYPE_PIPE) {
                status = ESUCC;
        } else {
                status = ENOENT;
        }

        if (status == ESUCC) {

                status = sys_mutex_lock(devfs->mutex, TIMEOUT_MS);
                if (status == ESUCC) {
                        devfs->number_of_opened_files--;
                        node->opended--;
                        sys_mutex_unlock(devfs->mutex);
                }
        }

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
 * @param[out]          *wrcnt                  number of written bytes
 * @param[in ]           fattr                  file attributes
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
API_FS_WRITE(devfs,
             void            *fs_handle,
             void            *extra,
             fd_t             fd,
             const u8_t      *src,
             size_t           count,
             fpos_t          *fpos,
             size_t          *wrcnt,
             struct vfs_fattr fattr)
{
        UNUSED_ARG1(fs_handle);
        UNUSED_ARG1(fd);

        struct devnode *node = extra;

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
API_FS_READ(devfs,
            void            *fs_handle,
            void            *extra,
            fd_t             fd,
            u8_t            *dst,
            size_t           count,
            fpos_t          *fpos,
            size_t          *rdcnt,
            struct vfs_fattr fattr)
{
        UNUSED_ARG1(fs_handle);
        UNUSED_ARG1(fd);

        struct devnode *node = extra;

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
 * @param[in ]          *extra                  file extra data
 * @param[in ]           fd                     file descriptor
 * @param[in ]           request                request
 * @param[in ][out]     *arg                    request's argument
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
API_FS_IOCTL(devfs, void *fs_handle, void *extra, fd_t fd, int request, void *arg)
{
        UNUSED_ARG1(fs_handle);
        UNUSED_ARG1(fd);

        struct devnode *node = extra;

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
 * @param[in ]          *extra                  file extra data
 * @param[in ]           fd                     file descriptor
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
API_FS_FLUSH(devfs, void *fs_handle, void *extra, fd_t fd)
{
        UNUSED_ARG1(fs_handle);
        UNUSED_ARG1(fd);

        struct devnode *node = extra;

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
 * @param[in ]          *extra                  file extra data
 * @param[in ]           fd                     file descriptor
 * @param[out]          *stat                   file status
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
API_FS_FSTAT(devfs, void *fs_handle, void *extra, fd_t fd, struct stat *stat)
{
        UNUSED_ARG2(fs_handle, fd);

        struct devnode *node = extra;

        int result = EINVAL;

        stat->st_atime = 0;
        stat->st_mtime = 0;
        stat->st_gid   = node->gid;
        stat->st_uid   = node->uid;
        stat->st_mode  = node->mode;

        if (node->type == FILE_TYPE_DRV) {
                struct vfs_dev_stat devstat;
                result = sys_driver_stat(node->IF.drv, &devstat);
                if (result == ESUCC) {
                        stat->st_dev  = devstat.st_major << 8 | devstat.st_minor;
                        stat->st_size = devstat.st_size;
                        stat->st_type = FILE_TYPE_DRV;
                }
        } else if (node->type == FILE_TYPE_PIPE) {
                size_t pipelen;
                result = sys_pipe_get_length(node->IF.pipe, &pipelen);
                if (result == ESUCC) {
                        stat->st_size = pipelen;
                        stat->st_type = FILE_TYPE_PIPE;
                        stat->st_dev  = 0;
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

        int result = sys_mutex_lock(devfs->mutex, TIMEOUT_MS);
        if (result == ESUCC) {

                result = create_new_chain_if_necessary(devfs);
                if (result == ESUCC) {

                        struct devnode *node;
                        result = chain_get_empty_node(devfs->root_chain, &node);
                        if (result == ESUCC) {

                                result = sys_pipe_create(&node->IF.pipe);
                                if (result == ESUCC) {

                                        result = sys_malloc(strlen(path + 1) + 1,
                                                             reinterpret_cast(void**, &node->path));
                                        if (result == ESUCC) {

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

        return result;
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

        int result = sys_mutex_lock(devfs->mutex, TIMEOUT_MS);
        if (result == ESUCC) {

                result = create_new_chain_if_necessary(devfs);
                if (result == ESUCC) {

                        struct devnode *node;
                        result = chain_get_empty_node(devfs->root_chain, &node);
                        if (result == ESUCC) {

                                result = sys_malloc(strlen(path + 1) + 1, reinterpret_cast(void**, &node->path));
                                if (result == ESUCC) {

                                        strcpy(node->path, path + 1);
                                        node->IF.drv = dev;
                                        node->gid    = 0;
                                        node->uid    = 0;
                                        node->mode   = S_IRUSR | S_IWUSR | S_IRGRO | S_IWGRO | S_IROTH | S_IWOTH;
                                        node->type   = FILE_TYPE_DRV;

                                        devfs->number_of_used_nodes++;
                                }
                        }
                }

                sys_mutex_unlock(devfs->mutex);
        }

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
API_FS_OPENDIR(devfs, void *fs_handle, const char *path, DIR *dir)
{
        struct devfs *devfs = fs_handle;

        if (strcmp(path, "/") == 0) {
                dir->f_closedir = closedir;
                dir->f_readdir  = readdir;
                dir->f_dd       = NULL;
                dir->f_handle   = fs_handle;
                dir->f_items    = devfs->number_of_used_nodes;
                dir->f_seek     = 0;

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
static int closedir(void *fs_handle, DIR *dir)
{
        UNUSED_ARG1(fs_handle);
        UNUSED_ARG1(dir);

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
static int readdir(void *fs_handle, DIR *dir, dirent_t **dirent)
{
        struct devfs *devfs  = fs_handle;

        int result = sys_mutex_lock(devfs->mutex, TIMEOUT_MS);
        if (result == ESUCC) {

                struct devnode *node;
                result = chain_get_n_node(devfs->root_chain, dir->f_seek, &node);
                if (result == ESUCC) {

                        if (node->type == FILE_TYPE_DRV) {
                                struct vfs_dev_stat devstat;
                                result = sys_driver_stat(node->IF.drv, &devstat);
                                if (result == ESUCC) {
                                        dir->dirent.size = devstat.st_size;
                                } else {
                                        dir->dirent.size = 0;
                                }
                                dir->dirent.dev      = node->IF.drv;
                                dir->dirent.filetype = FILE_TYPE_DRV;

                        } else if (node->type == FILE_TYPE_PIPE) {
                                size_t n;
                                result = sys_pipe_get_length(node->IF.pipe, &n);
                                if (result == ESUCC) {
                                        dir->dirent.size = n;
                                } else {
                                        dir->dirent.size = 0;
                                }

                                dir->dirent.filetype = FILE_TYPE_PIPE;
                        } else {
                                result = EINVAL;
                        }

                        dir->dirent.name = node->path;
                        dir->f_seek++;

                        *dirent = &dir->dirent;
                }

                sys_mutex_unlock(devfs->mutex);
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
API_FS_REMOVE(devfs, void *fs_handle, const char *path)
{
        struct devfs *devfs  = fs_handle;

        int status = sys_mutex_lock(devfs->mutex, TIMEOUT_MS);
        if (status == ESUCC) {

                struct devnode *node;
                status = chain_get_node_by_path(devfs->root_chain, path, &node);
                if (status == ESUCC) {

                        if (node->opended == 0) {
                                if (node->type == FILE_TYPE_PIPE) {
                                        sys_pipe_destroy(node->IF.pipe);
                                }
                                node->IF.generic = NULL;

                                sys_free(reinterpret_cast(void**, &node->path));
                                node->path  = NULL;
                                node->gid   = 0;
                                node->uid   = 0;
                                node->mode  = 0;

                                devfs->number_of_used_nodes--;
                        } else {
                                status = EBUSY;
                        }
                }

                sys_mutex_unlock(devfs->mutex);
        }

        return status;
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

        int result = sys_mutex_lock(devfs->mutex, TIMEOUT_MS);
        if (result == ESUCC) {

                struct devnode *node;
                result = chain_get_node_by_path(devfs->root_chain, old_name, &node);
                if (result == ESUCC) {

                        char *name;
                        result = sys_malloc(strlen(new_name) + 1, reinterpret_cast(void**, &name));
                        if (result == ESUCC) {
                                strcpy(name, new_name);
                                sys_free(reinterpret_cast(void**, &node->path));
                                node->path = name;
                        } else {
                                result = ENOSPC;
                        }
                }

                sys_mutex_unlock(devfs->mutex);
        }

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
API_FS_CHMOD(devfs, void *fs_handle, const char *path, mode_t mode)
{
        struct devfs *devfs  = fs_handle;

        int status = sys_mutex_lock(devfs->mutex, TIMEOUT_MS);
        if (status == ESUCC) {

                struct devnode *node;
                status = chain_get_node_by_path(devfs->root_chain, path, &node);
                if (status == ESUCC) {
                        node->mode = mode;
                }

                sys_mutex_unlock(devfs->mutex);
        }

        return status;
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

        int status = sys_mutex_lock(devfs->mutex, TIMEOUT_MS);
        if (status == ESUCC) {

                struct devnode *node;
                status = chain_get_node_by_path(devfs->root_chain, path, &node);
                if (status == ESUCC) {
                        node->uid = owner;
                        node->gid = group;
                }

                sys_mutex_unlock(devfs->mutex);
        }

        return status;
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

        int status = sys_mutex_lock(devfs->mutex, TIMEOUT_MS);
        if (status == ESUCC) {

                struct devnode *node;
                status = chain_get_node_by_path(devfs->root_chain, path, &node);
                if (status == ESUCC) {
                        status = _devfs_fstat(devfs, node, 0, stat);
                }

                sys_mutex_unlock(devfs->mutex);
        }

        return status;
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
        statfs->f_type   = 1;
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
        return sys_zalloc(sizeof(struct devfs_chain), reinterpret_cast(void**, chain));
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
                        sys_free(reinterpret_cast(void**, &chain->devnode[i].path));
                }
        }

        sys_free(reinterpret_cast(void**, &chain));
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
