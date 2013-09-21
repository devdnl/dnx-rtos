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
        } devnode[CHAIN_NUMBER_OF_NODES];

        struct devfs_chain       *next_chain;
};

struct devfs {
        struct devfs_chain *root_chain;
        int                 number_of_opened_devices;
        int                 number_of_chains;
        int                 number_of_used_nodes;
};

/*==============================================================================
  Local function prototypes
==============================================================================*/
static stdret_t closedir(void *fs_handle, DIR *dir);
static dirent_t readdir (void *fs_handle, DIR *dir);

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

        struct devfs *devfs = calloc(1, sizeof(struct devfs));
        if (devfs) {
                devfs->root_chain = calloc(1, sizeof(struct devfs_chain));
                if (devfs->root_chain) {
                        devfs->number_of_chains = 1;
                        *fs_handle = devfs;
                        return STD_RET_OK;
                }

                free(devfs);
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

        if (devfs->number_of_opened_devices != 0) {
                return STD_RET_ERROR;
        } else {

                /*TODO devfs release */
                return STD_RET_OK;
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

        struct devfs *devfs = fs_handle;

        for (struct devfs_chain *chain = devfs->root_chain; chain != NULL; chain = chain->next_chain) {
                for (int i = 0; i < CHAIN_NUMBER_OF_NODES; i++) {
                        if (chain->devnode[i].drvif == NULL)
                                continue;

                        if (strcmp(chain->devnode[i].path, path) != 0)
                                continue;

                        const struct vfs_drv_interface *drvif = chain->devnode[i].drvif;

                        if (drvif->drv_open(drvif->handle, O_DEV_FLAGS(flags)) == STD_RET_OK) {
                                *extra = &chain->devnode[i];
                                *fpos  = 0;
                                devfs->number_of_opened_devices++;
                                return STD_RET_OK;
                        } else {
                                return STD_RET_ERROR;
                        }
                }
        }

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

        struct devfs   *devfs = fs_handle;
        struct devnode *node  = extra;

        if (node->drvif->drv_close(node->drvif->handle, force, file_owner) == STD_RET_OK) {
                devfs->number_of_opened_devices--;
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

        struct devfs *devfs = fs_handle;

        if (devfs->number_of_chains * CHAIN_NUMBER_OF_NODES == devfs->number_of_used_nodes) {
                struct devfs_chain *chain = devfs->root_chain;
                for (; chain != NULL; chain = chain->next_chain);

                STOP_IF(chain == NULL); /* TEST */

                chain->next_chain = calloc(1, sizeof(struct devfs_chain));
                if (!chain->next_chain)
                        return STD_RET_ERROR;

                devfs->number_of_chains++;
        }

        for (struct devfs_chain *chain = devfs->root_chain; chain != NULL; chain = chain->next_chain) {
                for (int i = 0; i < CHAIN_NUMBER_OF_NODES; i++) {
                        if (chain->devnode[i].drvif != NULL)
                                continue;

                        chain->devnode[i].drvif = malloc(sizeof(struct vfs_drv_interface));
                        if (!chain->devnode[i].drvif)
                                return STD_RET_ERROR;
                        *chain->devnode[i].drvif = *drv_if;

                        chain->devnode[i].path  = calloc(strlen(path), sizeof(char));
                        if (!chain->devnode[i].path)
                                return STD_RET_ERROR; /* FIXME memory leakage */
                        strcpy(chain->devnode[i].path, path);

                        chain->devnode[i].gid   = 0;
                        chain->devnode[i].uid   = 0;
                        chain->devnode[i].mode  = OWNER_MODE(MODE_R | MODE_W)
                                                | GROUP_MODE(MODE_R | MODE_W)
                                                | OTHER_MODE(MODE_R | MODE_W);

                        devfs->number_of_used_nodes++;

                        return STD_RET_OK;
                }
        }

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

        size_t n_node = 0;

        for (struct devfs_chain *chain = devfs->root_chain; chain != NULL; chain = chain->next_chain) {
                for (int i = 0; i < CHAIN_NUMBER_OF_NODES; i++, n_node++) {
                        if (n_node == dir->f_seek) {
                                dirent.name = (char *)chain->devnode[i].path;

                                struct vfs_dev_stat devstat;
                                devstat.st_size = 0;

                                const struct vfs_drv_interface *drvif = chain->devnode[i].drvif;
                                drvif->drv_stat(drvif->handle, &devstat);

                                dirent.size = devstat.st_size;

                                return dirent;
                        }
                }
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

        struct devfs *devfs = fs_handle;

        for (struct devfs_chain *chain = devfs->root_chain; chain != NULL; chain = chain->next_chain) {
                for (int i = 0; i < CHAIN_NUMBER_OF_NODES; i++) {
                        if (strcmp(chain->devnode[i].path, path) == 0) {
                                free(chain->devnode[i].drvif);
                                free(chain->devnode[i].path);

                                chain->devnode[i].drvif = NULL;
                                chain->devnode[i].path  = NULL;
                                chain->devnode[i].gid   = 0;
                                chain->devnode[i].uid   = 0;
                                chain->devnode[i].mode  = 0;

                                devfs->number_of_used_nodes--;
                                return STD_RET_OK;
                        }
                }
        }

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
API_FS_RENAME(devfs, void *fs_handle, const char *old_name, const char *new_name)
{
        STOP_IF(!fs_handle);
        STOP_IF(!old_name);
        STOP_IF(!new_name);

        struct devfs *devfs = fs_handle;

        for (struct devfs_chain *chain = devfs->root_chain; chain != NULL; chain = chain->next_chain) {
                for (int i = 0; i < CHAIN_NUMBER_OF_NODES; i++) {
                        if (strcmp(chain->devnode[i].path, old_name) == 0) {
                                char *name = calloc(strlen(new_name), 1);
                                if (!name)
                                        return STD_RET_ERROR;
                                strcpy(name, new_name);

                                free(chain->devnode[i].path);
                                chain->devnode[i].path = name;
                                return STD_RET_OK;
                        }
                }
        }

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
API_FS_CHMOD(devfs, void *fs_handle, const char *path, int mode)
{
        STOP_IF(!fs_handle);
        STOP_IF(!path);

        struct devfs *devfs = fs_handle;

        for (struct devfs_chain *chain = devfs->root_chain; chain != NULL; chain = chain->next_chain) {
                for (int i = 0; i < CHAIN_NUMBER_OF_NODES; i++) {
                        if (strcmp(chain->devnode[i].path, path) == 0) {
                                chain->devnode[i].mode = mode;
                                return STD_RET_OK;
                        }
                }
        }

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
API_FS_CHOWN(devfs, void *fs_handle, const char *path, int owner, int group)
{
        STOP_IF(!fs_handle);
        STOP_IF(!path);

        struct devfs *devfs = fs_handle;

        for (struct devfs_chain *chain = devfs->root_chain; chain != NULL; chain = chain->next_chain) {
                for (int i = 0; i < CHAIN_NUMBER_OF_NODES; i++) {
                        if (strcmp(chain->devnode[i].path, path) == 0) {
                                chain->devnode[i].uid = owner;
                                chain->devnode[i].gid = group;
                                return STD_RET_OK;
                        }
                }
        }

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
API_FS_STAT(devfs, void *fs_handle, const char *path, struct vfs_stat *stat)
{
        STOP_IF(!fs_handle);
        STOP_IF(!path);
        STOP_IF(!stat);

        struct devfs *devfs = fs_handle;

        for (struct devfs_chain *chain = devfs->root_chain; chain != NULL; chain = chain->next_chain) {
                for (int i = 0; i < CHAIN_NUMBER_OF_NODES; i++) {
                        if (strcmp(chain->devnode[i].path, path) == 0) {
                                struct vfs_dev_stat devstat;
                                devstat.st_size  = 0;
                                devstat.st_major = 0;
                                devstat.st_minor = 0;

                                const struct vfs_drv_interface *drvif = chain->devnode[i].drvif;
                                drvif->drv_stat(drvif->handle, &devstat);

                                stat->st_atime = 0;
                                stat->st_mtime = 0;
                                stat->st_dev   = devstat.st_major << 8 | devstat.st_minor;
                                stat->st_gid   = chain->devnode[i].gid;
                                stat->st_uid   = chain->devnode[i].uid;
                                stat->st_mode  = chain->devnode[i].mode;
                                stat->st_size  = devstat.st_size;

                                return STD_RET_OK;
                        }
                }
        }

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
API_FS_STATFS(devfs, void *fs_handle, struct vfs_statfs *statfs)
{
        STOP_IF(!fs_handle);
        STOP_IF(!statfs);

        struct devfs *devfs = fs_handle;

        statfs->f_blocks = devfs->number_of_chains * CHAIN_NUMBER_OF_NODES;
        statfs->f_bfree  = statfs->f_blocks - devfs->number_of_used_nodes;
        statfs->f_ffree  = statfs->f_bfree;
        statfs->f_files  = devfs->number_of_used_nodes;
        statfs->f_type   = 1;
        statfs->fsname   = "devfs";

        return STD_RET_OK;
}

#ifdef __cplusplus
}
#endif

/*==============================================================================
  End of file
==============================================================================*/
