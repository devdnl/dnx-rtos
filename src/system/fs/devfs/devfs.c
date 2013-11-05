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

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Include files
==============================================================================*/
#include "system/dnxfs.h"

/*==============================================================================
  Local macros
==============================================================================*/
#define CHAIN_NUMBER_OF_NODES           8
#define TIMEOUT_MS                      MAX_DELAY

/*==============================================================================
  Local object types
==============================================================================*/
struct devfs_chain {
        struct devnode {
                struct vfs_drv_interface *drvif;
                char                     *path;
                int                       gid;
                int                       uid;
                int                       mode;
                int                       opended;
        } devnode[CHAIN_NUMBER_OF_NODES];

        struct devfs_chain       *next_chain;
};

struct devfs {
        struct devfs_chain *root_chain;
        mutex_t            *mutex;
        int                 number_of_opened_devices;
        int                 number_of_chains;
        int                 number_of_used_nodes;
};

/*==============================================================================
  Local function prototypes
==============================================================================*/
static stdret_t            closedir             (void *fs_handle, DIR *dir);
static dirent_t            readdir              (void *fs_handle, DIR *dir);
static struct devnode     *get_node_by_path     (struct devfs *devfs, const char *path);
static struct devnode     *get_empty_node       (struct devfs *devfs);
static struct devnode     *get_n_node           (struct devfs *devfs, int n);
static struct devfs_chain *new_chain            (void);
static void                delete_chain         (struct devfs_chain *chain);

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
        STOP_IF(!fs_handle);
        STOP_IF(!src_path);

        struct devfs       *devfs = malloc(sizeof(struct devfs));
        mutex_t            *mtx   = mutex_new();
        struct devfs_chain *chain = new_chain();

        if (devfs && mtx && chain) {
                devfs->root_chain               = chain;
                devfs->mutex                    = mtx;
                devfs->number_of_chains         = 1;
                devfs->number_of_opened_devices = 0;
                devfs->number_of_used_nodes     = 0;

                *fs_handle = devfs;
                return STD_RET_OK;
        }

        if (devfs) {
                free(devfs);
        }

        if (mtx) {
                mutex_delete(mtx);
        }

        if (chain) {
                delete_chain(chain);
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
        STOP_IF(!fs_handle);

        struct devfs *devfs = fs_handle;

        if (mutex_lock(devfs->mutex, 100) == MUTEX_LOCKED) {
                if (devfs->number_of_opened_devices != 0) {
                        mutex_unlock(devfs->mutex);
                        return STD_RET_ERROR;
                }

                critical_section_begin();
                mutex_unlock(devfs->mutex);

                delete_chain(devfs->root_chain);
                mutex_delete(devfs->mutex);
                free(devfs);

                critical_section_end();
                return STD_RET_OK;
        }

        return STD_RET_ERROR;
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
API_FS_OPEN(devfs, void *fs_handle, void **extra, fd_t *fd, u64_t *fpos, const char *path, int flags)
{
        STOP_IF(!fs_handle);
        STOP_IF(!extra);
        STOP_IF(!fd);
        STOP_IF(!fpos);
        STOP_IF(!path);

        struct devfs *devfs  = fs_handle;
        stdret_t      status = STD_RET_ERROR;

        if (mutex_lock(devfs->mutex, TIMEOUT_MS) == MUTEX_LOCKED) {

                struct devnode *node = get_node_by_path(devfs, path);
                if (node) {
                        const struct vfs_drv_interface *drvif = node->drvif;

                        if (drvif->drv_open(drvif->handle, O_DEV_FLAGS(flags)) == STD_RET_OK) {
                                *extra = node;
                                *fpos  = 0;
                                devfs->number_of_opened_devices++;
                                node->opended++;
                                status = STD_RET_OK;
                        }
                }

                mutex_unlock(devfs->mutex);
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
 * @param[in ]          *file_owner             task which opened file (valid if force is true)
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
API_FS_CLOSE(devfs, void *fs_handle, void *extra, fd_t fd, bool force, const task_t *file_owner)
{
        STOP_IF(!fs_handle);
        STOP_IF(!extra);
        STOP_IF(!file_owner);

        UNUSED_ARG(fd);

        struct devfs   *devfs  = fs_handle;
        struct devnode *node   = extra;

        if (node->drvif->drv_close(node->drvif->handle, force, file_owner) == STD_RET_OK) {

                if (mutex_lock(devfs->mutex, TIMEOUT_MS) == MUTEX_LOCKED) {
                        devfs->number_of_opened_devices--;
                        node->opended--;
                        mutex_unlock(devfs->mutex);
                }

                return STD_RET_OK;
        }

        return STD_RET_ERROR;
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

 * @return number of written bytes
 */
//==============================================================================
API_FS_WRITE(devfs, void *fs_handle,void *extra, fd_t fd, const u8_t *src, size_t count, u64_t *fpos)
{
        STOP_IF(!fs_handle);
        STOP_IF(!extra);
        STOP_IF(!src);
        STOP_IF(!fpos);

        UNUSED_ARG(fd);

        struct devnode *node = extra;

        return node->drvif->drv_write(node->drvif->handle, src, count, fpos);
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

 * @return number of read bytes
 */
//==============================================================================
API_FS_READ(devfs, void *fs_handle, void *extra, fd_t fd, u8_t *dst, size_t count, u64_t *fpos)
{
        STOP_IF(!fs_handle);
        STOP_IF(!extra);
        STOP_IF(!dst);
        STOP_IF(!fpos);

        UNUSED_ARG(fd);

        struct devnode *node = extra;

        return node->drvif->drv_read(node->drvif->handle, dst, count, fpos);
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
 * @retval ...
 */
//==============================================================================
API_FS_IOCTL(devfs, void *fs_handle, void *extra, fd_t fd, int request, void *arg)
{
        STOP_IF(!fs_handle);
        STOP_IF(!extra);

        UNUSED_ARG(fd);

        struct devnode *node = extra;

        return node->drvif->drv_ioctl(node->drvif->handle, request, arg);
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
        STOP_IF(!fs_handle);
        STOP_IF(!extra);

        UNUSED_ARG(fd);

        struct devnode *node = extra;

        return node->drvif->drv_flush(node->drvif->handle);
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
API_FS_FSTAT(devfs, void *fs_handle, void *extra, fd_t fd, struct vfs_stat *stat)
{
        STOP_IF(!fs_handle);
        STOP_IF(!extra);
        STOP_IF(!stat);

        UNUSED_ARG(fd);

        struct devnode *node = extra;

        struct vfs_dev_stat devstat;
        if (node->drvif->drv_stat(node->drvif->handle, &devstat) == STD_RET_OK) {
                stat->st_atime = 0;
                stat->st_mtime = 0;
                stat->st_dev   = devstat.st_major << 8 | devstat.st_minor;
                stat->st_gid   = node->gid;
                stat->st_uid   = node->uid;
                stat->st_mode  = node->mode;
                stat->st_size  = devstat.st_size;

                return STD_RET_OK;
        }

        return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief Create directory
 *
 * @param[in ]          *fs_handle              file system allocated memory
 * @param[in ]          *path                   name of created directory
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
API_FS_MKDIR(devfs, void *fs_handle, const char *path)
{
        STOP_IF(!fs_handle);
        STOP_IF(!path);

        return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief Create node for driver file
 *
 * @param[in ]          *fs_handle              file system allocated memory
 * @param[in ]          *path                   name of created node
 * @param[in ]          *drv_if                 driver interface
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
API_FS_MKNOD(devfs, void *fs_handle, const char *path, const struct vfs_drv_interface *drv_if)
{
        STOP_IF(!fs_handle);
        STOP_IF(!path);
        STOP_IF(!drv_if);

        struct devfs *devfs  = fs_handle;
        stdret_t      status = STD_RET_ERROR;

        if (mutex_lock(devfs->mutex, TIMEOUT_MS) == MUTEX_LOCKED) {

                if (devfs->number_of_chains * CHAIN_NUMBER_OF_NODES == devfs->number_of_used_nodes) {
                        struct devfs_chain *chain = devfs->root_chain;
                        while (chain->next_chain != NULL) {
                                chain = chain->next_chain;
                        }

                        chain->next_chain = new_chain();
                        if (!chain->next_chain)
                                goto exit;

                        devfs->number_of_chains++;
                }

                struct devnode *node = get_empty_node(devfs);
                if (node) {
                        node->drvif = malloc(sizeof(struct vfs_drv_interface));
                        node->path  = malloc(strlen(path + 1) + 1);

                        if (node->drvif && node->path) {
                                *node->drvif = *drv_if;
                                strcpy(node->path, path + 1);
                                node->gid  = 0;
                                node->uid  = 0;
                                node->mode = S_IRUSR | S_IWUSR | S_IRGRO | S_IWGRO | S_IROTH | S_IWOTH;

                                devfs->number_of_used_nodes++;

                                status = STD_RET_OK;
                                goto exit;
                        }

                        if (node->drvif) {
                                free(node->drvif);
                                node->drvif = NULL;
                        }

                        if (node->path) {
                                free(node->path);
                                node->path = NULL;
                        }
                }

exit:
                mutex_unlock(devfs->mutex);
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
        STOP_IF(!fs_handle);
        STOP_IF(!path);
        STOP_IF(!dir);

        struct devfs *devfs = fs_handle;

        if (strcmp(path, "/") == 0) {
                dir->f_closedir = closedir;
                dir->f_readdir  = readdir;
                dir->f_dd       = NULL;
                dir->f_handle   = fs_handle;
                dir->f_items    = devfs->number_of_used_nodes;
                dir->f_seek     = 0;

                return STD_RET_OK;
        }

        return STD_RET_ERROR;
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
        STOP_IF(!fs_handle);
        STOP_IF(!dir);

        return STD_RET_OK;
}

//==============================================================================
/**
 * @brief Read directory
 *
 * @param[in ]          *fs_handle              file system allocated memory
 * @param[in ]          *dir                    directory object
 *
 * @return directory entry description object
 */
//==============================================================================
static dirent_t readdir(void *fs_handle, DIR *dir)
{
        STOP_IF(!fs_handle);
        STOP_IF(!dir);

        struct devfs *devfs = fs_handle;

        dirent_t dirent;
        dirent.filetype = FILE_TYPE_DRV;
        dirent.name     = NULL;
        dirent.size     = 0;

        if (mutex_lock(devfs->mutex, TIMEOUT_MS) == MUTEX_LOCKED) {

                struct devnode *node = get_n_node(devfs, dir->f_seek);
                if (node) {
                        struct vfs_dev_stat devstat;
                        devstat.st_size = 0;
                        node->drvif->drv_stat(node->drvif->handle, &devstat);

                        dirent.name = node->path;
                        dirent.size = devstat.st_size;

                        dir->f_seek++;
                }

                mutex_unlock(devfs->mutex);
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
        STOP_IF(!fs_handle);
        STOP_IF(!path);

        struct devfs *devfs  = fs_handle;
        stdret_t      status = STD_RET_ERROR;

        if (mutex_lock(devfs->mutex, TIMEOUT_MS) == MUTEX_LOCKED) {

                struct devnode *node = get_node_by_path(devfs, path);
                if (node) {
                        if (node->opended == 0) {
                                free(node->drvif);
                                free(node->path);

                                node->drvif = NULL;
                                node->path  = NULL;
                                node->gid   = 0;
                                node->uid   = 0;
                                node->mode  = 0;
                                node->opended--;

                                devfs->number_of_used_nodes--;
                                status = STD_RET_OK;
                        }
                }

                mutex_unlock(devfs->mutex);
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
        STOP_IF(!fs_handle);
        STOP_IF(!old_name);
        STOP_IF(!new_name);

        struct devfs *devfs  = fs_handle;
        stdret_t      status = STD_RET_ERROR;

        if (mutex_lock(devfs->mutex, TIMEOUT_MS) == MUTEX_LOCKED) {

                struct devnode *node = get_node_by_path(devfs, old_name);
                if (node) {
                        char *name = malloc(strlen(new_name) + 1);
                        if (name) {
                                strcpy(name, new_name);
                                free(node->path);
                                node->path = name;

                                status = STD_RET_OK;
                        }
                }

                mutex_unlock(devfs->mutex);
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
API_FS_CHMOD(devfs, void *fs_handle, const char *path, int mode)
{
        STOP_IF(!fs_handle);
        STOP_IF(!path);

        struct devfs *devfs  = fs_handle;
        stdret_t      status = STD_RET_ERROR;

        if (mutex_lock(devfs->mutex, TIMEOUT_MS) == MUTEX_LOCKED) {

                struct devnode *node = get_node_by_path(devfs, path);
                if (node) {
                        node->mode = mode;
                        status = STD_RET_OK;
                }

                mutex_unlock(devfs->mutex);
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
API_FS_CHOWN(devfs, void *fs_handle, const char *path, int owner, int group)
{
        STOP_IF(!fs_handle);
        STOP_IF(!path);

        struct devfs *devfs  = fs_handle;
        stdret_t      status = STD_RET_ERROR;

        if (mutex_lock(devfs->mutex, TIMEOUT_MS) == MUTEX_LOCKED) {

                struct devnode *node = get_node_by_path(devfs, path);
                if (node) {
                        node->uid = owner;
                        node->gid = group;

                        status = STD_RET_OK;
                }

                mutex_unlock(devfs->mutex);
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
API_FS_STAT(devfs, void *fs_handle, const char *path, struct vfs_stat *stat)
{
        STOP_IF(!fs_handle);
        STOP_IF(!path);
        STOP_IF(!stat);

        struct devfs *devfs  = fs_handle;
        stdret_t      status = STD_RET_ERROR;

        if (mutex_lock(devfs->mutex, TIMEOUT_MS) == MUTEX_LOCKED) {

                struct devnode *node = get_node_by_path(devfs, path);
                if (node) {
                        struct vfs_dev_stat devstat;
                        devstat.st_size  = 0;
                        devstat.st_major = 0;
                        devstat.st_minor = 0;
                        node->drvif->drv_stat(node->drvif->handle, &devstat);

                        stat->st_atime = 0;
                        stat->st_mtime = 0;
                        stat->st_dev   = devstat.st_major << 8 | devstat.st_minor;
                        stat->st_gid   = node->gid;
                        stat->st_uid   = node->uid;
                        stat->st_mode  = node->mode;
                        stat->st_size  = devstat.st_size;

                        status = STD_RET_OK;
                }

                mutex_unlock(devfs->mutex);
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
API_FS_STATFS(devfs, void *fs_handle, struct vfs_statfs *statfs)
{
        STOP_IF(!fs_handle);
        STOP_IF(!statfs);

        struct devfs *devfs = fs_handle;

        statfs->f_blocks = 0;
        statfs->f_bfree  = 0;
        statfs->f_ffree  = 0;
        statfs->f_files  = devfs->number_of_used_nodes;
        statfs->f_type   = 1;
        statfs->f_fsname = "devfs";

        return STD_RET_OK;
}

//==============================================================================
/**
 * @brief Return node pointer
 *
 * @param[in] *devfs            file system memory
 * @param[in] *path             node's path
 *
 * @return node pointer
 */
//==============================================================================
static struct devnode *get_node_by_path(struct devfs *devfs, const char *path)
{
        for (struct devfs_chain *chain = devfs->root_chain; chain != NULL; chain = chain->next_chain) {
                for (int i = 0; i < CHAIN_NUMBER_OF_NODES; i++) {
                        if (chain->devnode[i].drvif == NULL)
                                continue;

                        if (strcmp(chain->devnode[i].path, path + 1) == 0)
                                return &chain->devnode[i];
                }
        }

        return NULL;
}

//==============================================================================
/**
 * @brief Return node pointer
 *
 * @param[in] *devfs            file system memory
 *
 * @return node pointer
 */
//==============================================================================
static struct devnode *get_empty_node(struct devfs *devfs)
{
        for (struct devfs_chain *chain = devfs->root_chain; chain != NULL; chain = chain->next_chain) {
                for (int i = 0; i < CHAIN_NUMBER_OF_NODES; i++) {
                        if (chain->devnode[i].drvif == NULL)
                                return &chain->devnode[i];
                }
        }

        return NULL;
}

//==============================================================================
/**
 * @brief Return node pointer
 *
 * @param[in] *devfs            file system memory
 * @param[in]  n                node number
 *
 * @return node pointer
 */
//==============================================================================
static struct devnode *get_n_node(struct devfs *devfs, int n)
{
        int n_node = 0;

        for (struct devfs_chain *chain = devfs->root_chain; chain != NULL; chain = chain->next_chain) {
                for (int i = 0; i < CHAIN_NUMBER_OF_NODES; i++) {
                        if (chain->devnode[i].drvif != NULL) {
                                if (n_node++ == n)
                                        return &chain->devnode[i];
                        }
                }
        }

        return NULL;
}

//==============================================================================
/**
 * @brief Create new chain for nodes
 *
 * @return pointer to new object or NULL if error
 */
//==============================================================================
static struct devfs_chain *new_chain(void)
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
static void delete_chain(struct devfs_chain *chain)
{
        if (chain->next_chain) {
                delete_chain(chain->next_chain);
        }

        for (int i = 0; i < CHAIN_NUMBER_OF_NODES; i++) {
                if (chain->devnode[i].drvif) {
                        free(chain->devnode[i].drvif);
                }

                if (chain->devnode[i].path) {
                        free(chain->devnode[i].path);
                }
        }

        free(chain);
}



#ifdef __cplusplus
}
#endif

/*==============================================================================
  End of file
==============================================================================*/
