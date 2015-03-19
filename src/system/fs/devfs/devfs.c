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
#include "core/fs.h"

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
static stdret_t            closedir                     (void *fs_handle, DIR *dir);
static dirent_t           *readdir                      (void *fs_handle, DIR *dir);
static struct devfs_chain *chain_new                    (void);
static void                chain_delete                 (struct devfs_chain *chain);
static struct devnode     *chain_get_node_by_path       (struct devfs_chain *chain, const char *path);
static struct devnode     *chain_get_empty_node         (struct devfs_chain *chain);
static struct devnode     *chain_get_n_node             (struct devfs_chain *chain, int n);
static int                 create_new_chain_if_necessary(struct devfs *devfs);

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
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
API_FS_INIT(devfs, void **fs_handle, const char *src_path)
{
        UNUSED_ARG(src_path);

        struct devfs       *devfs = malloc(sizeof(struct devfs));
        mutex_t            *mtx   = _sys_mutex_new(MUTEX_NORMAL);
        struct devfs_chain *chain = chain_new();

        if (devfs && mtx && chain) {
                devfs->root_chain               = chain;
                devfs->mutex                    = mtx;
                devfs->number_of_chains         = 1;
                devfs->number_of_opened_files   = 0;
                devfs->number_of_used_nodes     = 0;

                *fs_handle = devfs;
                return STD_RET_OK;
        } else {
                if (devfs) {
                        free(devfs);
                }

                if (mtx) {
                        _sys_mutex_delete(mtx);
                }

                if (chain) {
                        chain_delete(chain);
                }
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
API_FS_RELEASE(devfs, void *fs_handle)
{
        struct devfs *devfs = fs_handle;

        if (_sys_mutex_lock(devfs->mutex, 100)) {
                if (devfs->number_of_opened_files != 0) {
                        _sys_mutex_unlock(devfs->mutex);
                        errno = EBUSY;
                        return STD_RET_ERROR;
                }

                _sys_critical_section_begin();
                _sys_mutex_unlock(devfs->mutex);

                chain_delete(devfs->root_chain);
                _sys_mutex_delete(devfs->mutex);
                free(devfs);

                _sys_critical_section_end();
                return STD_RET_OK;
        } else {
                errno = EBUSY;

                return STD_RET_ERROR;
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
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
API_FS_OPEN(devfs, void *fs_handle, void **extra, fd_t *fd, fpos_t *fpos, const char *path, vfs_open_flags_t flags)
{
        UNUSED_ARG(fd);

        struct devfs *devfs  = fs_handle;
        stdret_t      status = STD_RET_ERROR;

        if (_sys_mutex_lock(devfs->mutex, TIMEOUT_MS)) {

                struct devnode *node = chain_get_node_by_path(devfs->root_chain, path);
                if (node) {
                        stdret_t open = STD_RET_ERROR;
                        if (node->type == FILE_TYPE_DRV) {
                                open = _sys_driver_open(node->IF.drv, flags);
                        } else if (node->type == FILE_TYPE_PIPE) {
                                open = STD_RET_OK;
                        }

                        if (open == STD_RET_OK) {
                                *extra = node;
                                *fpos  = 0;
                                devfs->number_of_opened_files++;
                                node->opended++;
                                status = STD_RET_OK;
                        }
                }

                _sys_mutex_unlock(devfs->mutex);
        } else {
                errno = EBUSY;
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
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
API_FS_CLOSE(devfs, void *fs_handle, void *extra, fd_t fd, bool force)
{
        UNUSED_ARG(fd);

        struct devfs   *devfs  = fs_handle;
        struct devnode *node   = extra;

        stdret_t close = STD_RET_ERROR;
        if (node->type == FILE_TYPE_DRV) {
                close = _sys_driver_close(node->IF.drv, force);
        } else if (node->type == FILE_TYPE_PIPE) {
                close = STD_RET_OK;
        }

        if (close == STD_RET_OK) {

                if (_sys_mutex_lock(devfs->mutex, TIMEOUT_MS)) {
                        devfs->number_of_opened_files--;
                        node->opended--;
                        _sys_mutex_unlock(devfs->mutex);
                }

                return STD_RET_OK;
        } else {
                return STD_RET_ERROR;
        }
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
API_FS_WRITE(devfs, void *fs_handle,void *extra, fd_t fd, const u8_t *src, size_t count, fpos_t *fpos, struct vfs_fattr fattr)
{
        UNUSED_ARG(fs_handle);
        UNUSED_ARG(fd);

        struct devnode *node = extra;

        if (node->type == FILE_TYPE_DRV) {
                return _sys_driver_write(node->IF.drv, src, count, fpos, fattr);
        } else if (node->type == FILE_TYPE_PIPE) {
                return _sys_pipe_write(node->IF.pipe, src, count, fattr.non_blocking_wr);
        } else {
                errno = ENXIO;
                return -1;
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
 * @param[in ]           fattr                  file attributes
 *
 * @return number of read bytes, -1 if error
 */
//==============================================================================
API_FS_READ(devfs, void *fs_handle, void *extra, fd_t fd, u8_t *dst, size_t count, fpos_t *fpos, struct vfs_fattr fattr)
{
        UNUSED_ARG(fs_handle);
        UNUSED_ARG(fd);

        struct devnode *node = extra;

        if (node->type == FILE_TYPE_DRV) {
                return _sys_driver_read(node->IF.drv, dst, count, fpos, fattr);
        } else if (node->type == FILE_TYPE_PIPE) {
                return _sys_pipe_read(node->IF.pipe, dst, count, fattr.non_blocking_rd);
        } else {
                errno = ENXIO;
                return -1;
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
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
API_FS_IOCTL(devfs, void *fs_handle, void *extra, fd_t fd, int request, void *arg)
{
        UNUSED_ARG(fs_handle);
        UNUSED_ARG(fd);

        struct devnode *node = extra;

        if (node->type == FILE_TYPE_DRV) {
                return _sys_driver_ioctl(node->IF.drv, request, arg);
        } else if (node->type == FILE_TYPE_PIPE && request == IOCTL_PIPE__CLOSE) {
                return _sys_pipe_close(node->IF.pipe) ? STD_RET_OK : STD_RET_ERROR;
        } else if (node->type == FILE_TYPE_PIPE && request == IOCTL_PIPE__CLEAR) {
                return _sys_pipe_clear(node->IF.pipe) ? STD_RET_OK : STD_RET_ERROR;
        } else {
                errno = EBADRQC;
                return STD_RET_ERROR;
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
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
API_FS_FLUSH(devfs, void *fs_handle, void *extra, fd_t fd)
{
        UNUSED_ARG(fs_handle);
        UNUSED_ARG(fd);

        struct devnode *node = extra;

        if (node->type == FILE_TYPE_DRV) {
                return _sys_driver_flush(node->IF.drv);
        } else {
                return STD_RET_ERROR;
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
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
API_FS_FSTAT(devfs, void *fs_handle, void *extra, fd_t fd, struct stat *stat)
{
        UNUSED_ARG(fs_handle);
        UNUSED_ARG(fd);

        struct devnode *node = extra;

        struct vfs_dev_stat devstat;
        int                 pipelen;
        stdret_t            getfstat = STD_RET_ERROR;

        if (node->type == FILE_TYPE_DRV) {
                getfstat = _sys_driver_stat(node->IF.drv, &devstat);
                if (getfstat == STD_RET_OK) {
                        stat->st_dev  = devstat.st_major << 8 | devstat.st_minor;
                        stat->st_size = devstat.st_size;
                        stat->st_type = FILE_TYPE_DRV;
                }
        } else if (node->type == FILE_TYPE_PIPE) {
                pipelen = _sys_pipe_get_length(node->IF.pipe);
                if (pipelen >= 0) {
                        stat->st_size = pipelen;
                        stat->st_type = FILE_TYPE_PIPE;
                        stat->st_dev  = 0;

                        getfstat = STD_RET_OK;
                }
        }

        if (getfstat == STD_RET_OK) {
                stat->st_atime = 0;
                stat->st_mtime = 0;
                stat->st_gid   = node->gid;
                stat->st_uid   = node->uid;
                stat->st_mode  = node->mode;

                return STD_RET_OK;
        } else {
                return STD_RET_ERROR;
        }
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
API_FS_MKDIR(devfs, void *fs_handle, const char *path, mode_t mode)
{
        UNUSED_ARG(fs_handle);
        UNUSED_ARG(path);
        UNUSED_ARG(mode);

        errno = EPERM;

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
API_FS_MKFIFO(devfs, void *fs_handle, const char *path, mode_t mode)
{
        struct devfs *devfs  = fs_handle;
        stdret_t      status = STD_RET_ERROR;

        if (_sys_mutex_lock(devfs->mutex, TIMEOUT_MS)) {

                if (create_new_chain_if_necessary(devfs) >= 0) {

                        struct devnode *node = chain_get_empty_node(devfs->root_chain);
                        if (node) {
                                node->IF.pipe = _sys_pipe_new();
                                node->path    = malloc(strlen(path + 1) + 1);

                                if (node->IF.pipe && node->path) {
                                        strcpy(node->path, path + 1);
                                        node->gid  = 0;
                                        node->uid  = 0;
                                        node->mode = mode;
                                        node->type = FILE_TYPE_PIPE;

                                        devfs->number_of_used_nodes++;

                                        status = STD_RET_OK;
                                } else {
                                        if (node->IF.pipe) {
                                                _sys_pipe_delete(node->IF.pipe);
                                                node->IF.pipe = NULL;
                                        }

                                        if (node->path) {
                                                free(node->path);
                                                node->path = NULL;
                                        }

                                        errno = ENOSPC;
                                }
                        }
                }

                _sys_mutex_unlock(devfs->mutex);
        }

        return status;
}

//==============================================================================
/**
 * @brief Create node for driver file
 *
 * @param[in ]          *fs_handle              file system allocated memory
 * @param[in ]          *path                   name of created node
 * @param[in ]           dev                    driver number
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
API_FS_MKNOD(devfs, void *fs_handle, const char *path, const dev_t dev)
{
        struct devfs *devfs  = fs_handle;
        stdret_t      status = STD_RET_ERROR;

        if (_sys_mutex_lock(devfs->mutex, TIMEOUT_MS)) {

                if (create_new_chain_if_necessary(devfs) >= 0) {

                        struct devnode *node = chain_get_empty_node(devfs->root_chain);
                        if (node) {
                                node->path = malloc(strlen(path + 1) + 1);
                                if (node->path) {
                                        strcpy(node->path, path + 1);
                                        node->IF.drv = dev;
                                        node->gid    = 0;
                                        node->uid    = 0;
                                        node->mode   = S_IRUSR | S_IWUSR | S_IRGRO | S_IWGRO | S_IROTH | S_IWOTH;
                                        node->type   = FILE_TYPE_DRV;

                                        devfs->number_of_used_nodes++;

                                        status = STD_RET_OK;
                                } else {
                                        errno = ENOSPC;
                                }
                        }
                }

                _sys_mutex_unlock(devfs->mutex);
        }

        return status;
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

                return STD_RET_OK;
        } else {
                errno = ENOENT;
                return STD_RET_ERROR;
        }
}

//==============================================================================
/**
 * @brief Close directory
 *
 * @param[in ]          *fs_handle              file system allocated memory
 * @param[in ]          *dir                    directory object
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
static stdret_t closedir(void *fs_handle, DIR *dir)
{
        UNUSED_ARG(fs_handle);
        UNUSED_ARG(dir);

        return STD_RET_OK;
}

//==============================================================================
/**
 * @brief Read directory
 *
 * @param[in ]          *fs_handle              file system allocated memory
 * @param[in ]          *dir                    directory object
 *
 * @return Pointer to directory entry description object
 */
//==============================================================================
static dirent_t *readdir(void *fs_handle, DIR *dir)
{
        struct devfs *devfs = fs_handle;

        dirent_t *dirent = NULL;

        if (_sys_mutex_lock(devfs->mutex, TIMEOUT_MS)) {

                struct devnode *node = chain_get_n_node(devfs->root_chain, dir->f_seek);
                if (node) {
                        if (node->type == FILE_TYPE_DRV) {
                                struct vfs_dev_stat devstat;
                                if (_sys_driver_stat(node->IF.drv, &devstat) == STD_RET_OK) {
                                        dir->dirent.size = devstat.st_size;
                                } else {
                                        dir->dirent.size = 0;
                                }
                                dir->dirent.dev      = node->IF.drv;
                                dir->dirent.filetype = FILE_TYPE_DRV;

                        } else if (node->type == FILE_TYPE_PIPE) {
                                int n = _sys_pipe_get_length(node->IF.pipe);
                                if (n >= 0) {
                                        dir->dirent.size = n;
                                } else {
                                        dir->dirent.size = 0;
                                }

                                dir->dirent.filetype = FILE_TYPE_PIPE;
                        } else {
                                dir->dirent.filetype = FILE_TYPE_REGULAR;
                        }

                        dir->dirent.name = node->path;
                        dir->f_seek++;

                        dirent = &dir->dirent;
                } else {
                        errno = 0;
                }

                _sys_mutex_unlock(devfs->mutex);
        }

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
API_FS_REMOVE(devfs, void *fs_handle, const char *path)
{
        struct devfs *devfs  = fs_handle;
        stdret_t      status = STD_RET_ERROR;

        if (_sys_mutex_lock(devfs->mutex, TIMEOUT_MS)) {

                struct devnode *node = chain_get_node_by_path(devfs->root_chain, path);
                if (node) {
                        if (node->opended == 0) {
                                if (node->type == FILE_TYPE_PIPE) {
                                        _sys_pipe_delete(node->IF.pipe);
                                }
                                node->IF.generic = NULL;

                                free(node->path);
                                node->path  = NULL;
                                node->gid   = 0;
                                node->uid   = 0;
                                node->mode  = 0;

                                devfs->number_of_used_nodes--;
                                status = STD_RET_OK;
                        } else {
                                errno = EBUSY;
                        }
                }

                _sys_mutex_unlock(devfs->mutex);
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
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
API_FS_RENAME(devfs, void *fs_handle, const char *old_name, const char *new_name)
{
        struct devfs *devfs  = fs_handle;
        stdret_t      status = STD_RET_ERROR;

        if (_sys_mutex_lock(devfs->mutex, TIMEOUT_MS)) {

                struct devnode *node = chain_get_node_by_path(devfs->root_chain, old_name);
                if (node) {
                        char *name = malloc(strlen(new_name) + 1);
                        if (name) {
                                strcpy(name, new_name);
                                free(node->path);
                                node->path = name;

                                status = STD_RET_OK;
                        } else {
                                errno = ENOMEM;
                        }
                }

                _sys_mutex_unlock(devfs->mutex);
        }

        return status;
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
API_FS_CHMOD(devfs, void *fs_handle, const char *path, mode_t mode)
{
        struct devfs *devfs  = fs_handle;
        stdret_t      status = STD_RET_ERROR;

        if (_sys_mutex_lock(devfs->mutex, TIMEOUT_MS)) {

                struct devnode *node = chain_get_node_by_path(devfs->root_chain, path);
                if (node) {
                        node->mode = mode;
                        status = STD_RET_OK;
                }

                _sys_mutex_unlock(devfs->mutex);
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
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
API_FS_CHOWN(devfs, void *fs_handle, const char *path, uid_t owner, gid_t group)
{
        struct devfs *devfs  = fs_handle;
        stdret_t      status = STD_RET_ERROR;

        if (_sys_mutex_lock(devfs->mutex, TIMEOUT_MS)) {

                struct devnode *node = chain_get_node_by_path(devfs->root_chain, path);
                if (node) {
                        node->uid = owner;
                        node->gid = group;

                        status = STD_RET_OK;
                }

                _sys_mutex_unlock(devfs->mutex);
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
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
API_FS_STAT(devfs, void *fs_handle, const char *path, struct stat *stat)
{
        struct devfs *devfs  = fs_handle;
        stdret_t      status = STD_RET_ERROR;

        if (_sys_mutex_lock(devfs->mutex, TIMEOUT_MS)) {

                struct devnode *node = chain_get_node_by_path(devfs->root_chain, path);
                if (node) {
                        status = _devfs_fstat(devfs, node, 0, stat);
                }

                _sys_mutex_unlock(devfs->mutex);
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
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
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

        return STD_RET_OK;
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
        UNUSED_ARG(fs_handle);
}

//==============================================================================
/**
 * @brief Return node pointer
 *        Errno: ENOENT
 *
 * @param[in] chain             chain
 * @param[in] path              node's path
 *
 * @return node pointer
 */
//==============================================================================
static struct devnode *chain_get_node_by_path(struct devfs_chain *chain, const char *path)
{
        for (struct devfs_chain *nchain = chain; nchain != NULL; nchain = nchain->next) {
                for (int i = 0; i < CHAIN_NUMBER_OF_NODES; i++) {
                        if (nchain->devnode[i].path == NULL)
                                continue;

                        if (strcmp(nchain->devnode[i].path, path + 1) == 0)
                                return &nchain->devnode[i];
                }
        }

        errno = ENOENT;

        return NULL;
}

//==============================================================================
/**
 * @brief Return node pointer
 *        Errno: ENOENT
 *
 * @param[in] chain             chain
 *
 * @return node pointer
 */
//==============================================================================
static struct devnode *chain_get_empty_node(struct devfs_chain *chain)
{
        for (struct devfs_chain *nchain = chain; nchain != NULL; nchain = nchain->next) {
                for (int i = 0; i < CHAIN_NUMBER_OF_NODES; i++) {
                        if (nchain->devnode[i].path == NULL)
                                return &nchain->devnode[i];
                }
        }

        errno = ENOENT;

        return NULL;
}

//==============================================================================
/**
 * @brief Return node pointer
 *        Errno: ENOENT
 *
 * @param[in] *devfs            file system memory
 * @param[in]  n                node number
 *
 * @return node pointer
 */
//==============================================================================
static struct devnode *chain_get_n_node(struct devfs_chain *chain, int n)
{
        int n_node = 0;

        for (struct devfs_chain *nchain = chain; nchain != NULL; nchain = nchain->next) {
                for (int i = 0; i < CHAIN_NUMBER_OF_NODES; i++) {
                        if (nchain->devnode[i].path != NULL) {
                                if (n_node++ == n)
                                        return &nchain->devnode[i];
                        }
                }
        }

        errno = ENOENT;

        return NULL;
}

//==============================================================================
/**
 * @brief Create new chain for nodes
 *
 * @return pointer to new object or NULL if error
 */
//==============================================================================
static struct devfs_chain *chain_new(void)
{
        return calloc(1, sizeof(struct devfs_chain));
}

//==============================================================================
/**
 * @brief Delete chain
 *
 * @param[in] *chain            chain to delete
 */
//==============================================================================
static void chain_delete(struct devfs_chain *chain)
{
        if (chain->next) {
                chain_delete(chain->next);
        }

        for (int i = 0; i < CHAIN_NUMBER_OF_NODES; i++) {
                if (chain->devnode[i].IF.generic) {
                        free(chain->devnode[i].IF.generic);
                }

                if (chain->devnode[i].path) {
                        free(chain->devnode[i].path);
                }
        }

        free(chain);
}

//==============================================================================
/**
 * @brief Function create new chain if no empty nodes exist
 * ERRNO: ENOSPC
 *
 * @param devfs         file system object
 *
 * @retval 1            new chain created
 * @retval 0            number of nodes is enough
 * @retval -1           error occurred
 */
//==============================================================================
static int create_new_chain_if_necessary(struct devfs *devfs)
{
        if (devfs->number_of_chains * CHAIN_NUMBER_OF_NODES == devfs->number_of_used_nodes) {
                struct devfs_chain *chain = devfs->root_chain;
                while (chain->next != NULL) {
                        chain = chain->next;
                }

                chain->next = chain_new();
                if (!chain->next) {
                        errno = ENOSPC;
                        return -1;
                } else {
                        devfs->number_of_chains++;
                        return 1;
                }
        }

        return 0;
}

/*==============================================================================
  End of file
==============================================================================*/
