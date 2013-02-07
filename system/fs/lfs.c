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

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Include files
==============================================================================*/
#include "lfs.h"
#include "dlist.h"
#include <string.h>

/*==============================================================================
  Local symbolic constants/macros
==============================================================================*/
/* wait time for operation on FS */
#define MTX_BLOCK_TIME                    10

/*==============================================================================
  Local types, enums definitions
==============================================================================*/
/** file types */
typedef enum {
        NODE_TYPE_DIR  = FILE_TYPE_DIR,
        NODE_TYPE_FILE = FILE_TYPE_REGULAR,
        NODE_TYPE_DRV  = FILE_TYPE_DRV,
        NODE_TYPE_LINK = FILE_TYPE_LINK
} nodeType_t;

/** node structure */
typedef struct node {
        ch_t       *name;       /* file name */
        nodeType_t  type;       /* file type */
        u32_t       dev;        /* major device number */
        u32_t       part;       /* minor device number */
        u32_t       mode;       /* protection */
        u32_t       uid;        /* user ID of owner */
        u32_t       gid;        /* group ID of owner */
        size_t      size;       /* file size */
        u32_t       mtime;      /* time of last modification */
        void       *data;       /* file type specified data */
} node_t;

/** open file info */
typedef struct openInfo {
        node_t *node;           /* opened node */
        node_t *nodebase;       /* base of opened node */
        bool_t  removeAtClose;  /* file to remove after close */
        u32_t   itemID;         /* item ID in base directory list */
} fopenInfo_t;

/** main memory structure */
struct fshdl_s {
        node_t   root;          /* root dir '/' */
        mutex_t  mtx;           /* lock mutex */
        list_t  *openFile;      /* list with opened files */
        u32_t    idcnt;         /* list ID counter */
};

/*==============================================================================
  Local function prototypes
==============================================================================*/
static stdRet_t  rmNode(node_t *base, node_t *target, u32_t baseitemid);
static node_t   *GetNode(const ch_t *path, node_t *startnode, i32_t deep, i32_t *item);
static uint_t    GetPathDeep(const ch_t *path);
static dirent_t  lfs_readdir(fsd_t fsd, DIR_t *dir);
static stdRet_t  lfs_closedir(fsd_t fsd, DIR_t *dir);
static node_t   *CreateNode(node_t *nodebase, ch_t *filename, i32_t *item);
static stdRet_t  AddNodeToListOfOpenFiles(node_t *nodebase, node_t *node, i32_t *item);

/*==============================================================================
  Local object definitions
==============================================================================*/
static struct fshdl_s *lfs;

/*==============================================================================
  Function definitions
==============================================================================*/

//==============================================================================
/**
 * @brief Initialize VFS module
 *
 * @param *srcPath      source path
 * @param *fsd          file system descriptor
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
stdRet_t lfs_init(const ch_t *srcPath, fsd_t *fsd)
{
        (void) fsd;
        (void) srcPath;

        if (lfs != NULL) {
                return STD_RET_OK;
        }

        lfs = calloc(1, sizeof(struct fshdl_s));
        if (lfs) {
                lfs->mtx = CreateMutex();
                lfs->root.data = ListCreate();
                lfs->openFile = ListCreate();

                if (!lfs->mtx || !lfs->root.data || !lfs->openFile) {
                        if (lfs->mtx) {
                                DeleteMutex(lfs->mtx);
                        }

                        if (lfs->root.data) {
                                ListDelete(lfs->root.data);
                        }

                        if (lfs->openFile) {
                                ListDelete(lfs->openFile);
                        }

                        free(lfs);
                        lfs = NULL;

                        return STD_RET_ERROR;
                } else {
                        lfs->root.name = "/";
                        lfs->root.size = sizeof(node_t);
                        lfs->root.type = NODE_TYPE_DIR;

                        return STD_RET_OK;
                }
        }

        return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief Function create node for driver file
 *
 * @param  fsd                file system descriptor
 * @param *path               path when driver-file shall be created
 * @param *drvcfg             pointer to description of driver
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
stdRet_t lfs_mknod(fsd_t fsd, const ch_t *path, struct vfs_drvcfg *drvcfg)
{
        (void) fsd;

        node_t *node;
        node_t *ifnode;
        node_t *dirfile;
        ch_t   *drvname;
        ch_t   *filename;
        uint_t  drvnamelen;
        struct vfs_drvcfg *dcfg;

        if (!path || !drvcfg || !lfs) {
                return STD_RET_ERROR;
        }

        if (path[0] != '/') {
                return STD_RET_ERROR;
        }

        while (TakeMutex(lfs->mtx, MTX_BLOCK_TIME) != OS_OK);
        node   = GetNode(path, &lfs->root, -1, NULL);
        ifnode = GetNode(strrchr(path, '/'), node, 0, NULL);

        /* directory must exist and created node not */
        if (node == NULL || ifnode != NULL) {
                goto lfs_mknod_error;
        }

        if (node->type != NODE_TYPE_DIR) {
                goto lfs_mknod_error;
        }

        drvname    = strrchr(path, '/') + 1;
        drvnamelen = strlen(drvname);

        filename = calloc(drvnamelen + 1, sizeof(ch_t));
        if (filename) {
                strcpy(filename, drvname);

                dirfile = calloc(1, sizeof(node_t));
                dcfg    = calloc(1, sizeof(struct vfs_drvcfg));

                if (dirfile && dcfg) {
                        memcpy(dcfg, drvcfg, sizeof(struct vfs_drvcfg));

                        dirfile->name = filename;
                        dirfile->size = 0;
                        dirfile->type = NODE_TYPE_DRV;
                        dirfile->data = dcfg;
                        dirfile->dev  = dcfg->dev;
                        dirfile->part = dcfg->part;

                        /* add new driver to this folder */
                        if (ListAddItem(node->data, lfs->idcnt++, dirfile) >= 0) {
                                GiveMutex(lfs->mtx);
                                return STD_RET_OK;
                        }
                }

                /* free memory when error */
                if (dirfile) {
                        free(dirfile);
                }

                if (dcfg) {
                        free(dcfg);
                }

                free(filename);
        }

        lfs_mknod_error:
        GiveMutex(lfs->mtx);
        return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief Create directory
 *
 * @param  fsd          file system descriptor
 * @param *path         path to new directory
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
stdRet_t lfs_mkdir(fsd_t fsd, const ch_t *path)
{
        (void) fsd;

        node_t *node;
        node_t *ifnode;
        node_t *dir;
        ch_t   *dirname;
        ch_t   *name;
        uint_t  dirnamelen;

        if (!path || !lfs) {
                return STD_RET_ERROR;
        }

        if (path[0] != '/') {
                return STD_RET_ERROR;
        }

        while (TakeMutex(lfs->mtx, MTX_BLOCK_TIME) != OS_OK);
        node   = GetNode(path, &lfs->root, -1, NULL);
        ifnode = GetNode(strrchr(path, '/'), node, 0, NULL);

        /* check if target node not exist or the same name exist */
        if (node == NULL || ifnode != NULL) {
                return STD_RET_ERROR;
        }

        if (node->type != NODE_TYPE_DIR) {
                return STD_RET_ERROR;
        }

        dirname    = strrchr(path, '/') + 1;
        dirnamelen = strlen(dirname);

        name = calloc(dirnamelen + 1, sizeof(ch_t));
        if (name) {
                strcpy(name, dirname);

                dir = calloc(1, sizeof(node_t));
                if (dir == NULL) {
                        goto lfs_mkdir_error;
                }

                dir->data = ListCreate();
                if (dir->data) {
                        dir->name = (ch_t*)name;
                        dir->size = sizeof(node_t) + strlen(name);
                        dir->type = NODE_TYPE_DIR;

                        /* add new folder to this folder */
                        if (ListAddItem(node->data, lfs->idcnt++, dir) >= 0) {
                                GiveMutex(lfs->mtx);
                                return STD_RET_OK;
                        }

                        ListDelete(dir->data);
                }

                /* free memory when error */
                free(dir);
                free(name);
        }

        lfs_mkdir_error:
        GiveMutex(lfs->mtx);
        return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief Function open directory
 *
 * @param  fsd          file system descriptor
 * @param *path         directory path
 * @param *dir          directory info
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
stdRet_t lfs_opendir(fsd_t fsd, const ch_t *path, DIR_t *dir)
{
        (void) fsd;

        if (path && lfs) {
                while (TakeMutex(lfs->mtx, MTX_BLOCK_TIME) != OS_OK);

                /* go to target dir */
                node_t *node = GetNode(path, &lfs->root, 0, NULL);

                if (node) {
                        if (node->type == NODE_TYPE_DIR) {
                                if (dir) {
                                        dir->items = ListGetItemCount(node->data);
                                        dir->rddir = lfs_readdir;
                                        dir->cldir = lfs_closedir;
                                        dir->seek  = 0;
                                        dir->dd    = node;
                                }

                                GiveMutex(lfs->mtx);
                                return STD_RET_OK;
                        }
                }

                GiveMutex(lfs->mtx);
        }

        return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief Function close dir
 *
 * @param  fsd          file system descriptor
 * @param *dir          directory info
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
static stdRet_t lfs_closedir(fsd_t fsd, DIR_t *dir)
{
        (void) fsd;
        (void) dir;

        return STD_RET_OK;
}

//==============================================================================
/**
 * @brief Function read next item of opened directory
 *
 * @param *dir          directory object
 *
 * @return element attributes
 */
//==============================================================================
static dirent_t lfs_readdir(fsd_t fsd, DIR_t *dir)
{
        (void) fsd;

        dirent_t dirent;
        dirent.name = NULL;
        dirent.size = 0;

        if (dir && lfs) {
                while (TakeMutex(lfs->mtx, MTX_BLOCK_TIME) != OS_OK);

                node_t *from = dir->dd;
                node_t *node = ListGetItemDataByNo(from->data, dir->seek++);

                if (node) {
                        dirent.filetype = node->type;
                        dirent.name = node->name;
                        dirent.size = node->size;
                }

                GiveMutex(lfs->mtx);
        }

        return dirent;
}

//==============================================================================
/**
 * @brief Remove file
 *
 * @param  fsd          file system descriptor
 * @param *patch        localization of file/directory
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
stdRet_t lfs_remove(fsd_t fsd, const ch_t *path)
{
        (void) fsd;

        i32_t   item;
        bool_t  dorm;
        u32_t   itemid;
        node_t *nodebase;
        node_t *nodeobj;

        if (!path || !lfs) {
                return STD_RET_ERROR;
        }

        while (TakeMutex(lfs->mtx, MTX_BLOCK_TIME) != OS_OK);

        dorm     = TRUE;
        nodebase = GetNode(path, &lfs->root, -1, NULL);
        nodeobj  = GetNode(path, &lfs->root, 0, &item);

        if (!nodebase || !nodeobj || nodeobj == &lfs->root) {
                goto lfs_remove_error;
        }

        /* if path is ending on slash, the object must be DIR */
        if (path[strlen(path) - 1] == '/') {
                if (nodeobj->type != NODE_TYPE_DIR) {
                        goto lfs_remove_error;
                }
        }

        /* check if file is opened */
        if (nodeobj->type != NODE_TYPE_DIR) {
                i32_t n = ListGetItemCount(lfs->openFile);

                for (int_t i = 0; i < n; i++) {
                        fopenInfo_t *olfoi = ListGetItemDataByNo(lfs->openFile, i);

                        if (olfoi->node == nodeobj) {
                                olfoi->removeAtClose = TRUE;
                                dorm = FALSE;
                        }
                }
        }

        /* remove node if possible */
        if (dorm == TRUE) {
                if (ListGetItemID(nodebase->data, item, &itemid) == 0) {
                        GiveMutex(lfs->mtx);
                        return rmNode(nodebase, nodeobj, itemid);
                }
        } else {
                GiveMutex(lfs->mtx);
                return STD_RET_OK;
        }

        lfs_remove_error:
        GiveMutex(lfs->mtx);
        return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief Rename file name
 * The implementation of rename can move files only if external FS provide
 * functionality. Local VFS cannot do this.
 *
 * @param  fsd                file system descriptor
 * @param *oldName            old file name
 * @param *newName            new file name
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
stdRet_t lfs_rename(fsd_t fsd, const ch_t *oldName, const ch_t *newName)
{
        (void) fsd;

        node_t *oldNodeBase;
        node_t *newNodeBase;
        node_t *node;
        ch_t   *name;

        if (!oldName || !newName || !lfs) {
                return STD_RET_ERROR;
        }

        while (TakeMutex(lfs->mtx, MTX_BLOCK_TIME) != OS_OK);
        oldNodeBase = GetNode(oldName, &lfs->root, -1, NULL);
        newNodeBase = GetNode(newName, &lfs->root, -1, NULL);

        if (!oldNodeBase || !newNodeBase) {
                goto lfs_rename_error;
        }

        if (oldNodeBase != oldNodeBase) {
                goto lfs_rename_error;
        }

        if (oldName[0] != '/' || newName[0] != '/') {
                goto lfs_rename_error;
        }

        if (  oldName[strlen(oldName) - 1] == '/'
           || newName[strlen(newName) - 1] == '/') {
                goto lfs_rename_error;
        }

        name = calloc(1, strlen(strrchr(newName, '/') + 1));
        node = GetNode(oldName, &lfs->root, 0, NULL);

        if (name && node) {
                strcpy(name, strrchr(newName, '/') + 1);

                if (node->name) {
                        free(node->name);
                }

                node->name = name;

                if (node->type == NODE_TYPE_DIR) {
                        node->size = sizeof(node_t) + strlen(name);
                } else if (node->type == NODE_TYPE_DRV) {
                        node->size = 0;
                }

                GiveMutex(lfs->mtx);
                return STD_RET_OK;
        }

        /* error - free allocated memory */
        if (name) {
                free(name);
        }

        if (node) {
                free(node);
        }

        lfs_rename_error:
        GiveMutex(lfs->mtx);
        return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief Function change file mode
 *
 * @param dev     fs device
 * @param *path   path
 * @param mode    file mode
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
stdRet_t lfs_chmod(fsd_t fsd, const ch_t *path, u32_t mode)
{
        (void) fsd;

        if (path && lfs) {
                while (TakeMutex(lfs->mtx, MTX_BLOCK_TIME) != OS_OK);

                node_t *node = GetNode(path, &lfs->root, 0, NULL);

                if (node) {
                        node->mode = mode;
                        GiveMutex(lfs->mtx);
                        return STD_RET_OK;
                }

                GiveMutex(lfs->mtx);
        }

        return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief Function change file owner and group
 *
 * @param dev     fs device
 * @param *path   path
 * @param owner   file owner
 * @param group   file group
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
stdRet_t lfs_chown(fsd_t fsd, const ch_t *path, u16_t owner, u16_t group)
{
        (void) fsd;

        if (path && lfs) {
                while (TakeMutex(lfs->mtx, MTX_BLOCK_TIME) != OS_OK);

                node_t *node = GetNode(path, &lfs->root, 0, NULL);

                if (node) {
                        node->uid = owner;
                        node->gid = group;

                        GiveMutex(lfs->mtx);
                        return STD_RET_OK;
                }

                GiveMutex(lfs->mtx);
        }

        return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief Function returns file/dir status
 *
 * @param  fsd          file system descriptor
 * @param *path         file/dir path
 * @param *stat         pointer to stat structure
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
stdRet_t lfs_stat(fsd_t fsd, const ch_t *path, struct vfs_stat *stat)
{
        (void) fsd;

        if (!path || !stat || !lfs) {
                return STD_RET_ERROR;
        }

        while (TakeMutex(lfs->mtx, MTX_BLOCK_TIME) != OS_OK);

        node_t *node = GetNode(path, &lfs->root, 0, NULL);
        if (node) {
                if ( (path[strlen(path) - 1] == '/' && node->type == NODE_TYPE_DIR)
                   || path[strlen(path) - 1] != '/') {

                        stat->st_dev   = node->dev;
                        stat->st_rdev  = node->part;
                        stat->st_gid   = node->gid;
                        stat->st_mode  = node->mode;
                        stat->st_mtime = node->mtime;
                        stat->st_size  = node->size;
                        stat->st_uid   = node->uid;

                        GiveMutex(lfs->mtx);
                        return STD_RET_OK;
                }
        }

        GiveMutex(lfs->mtx);

        return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief Function returns file status
 *
 * @param  fsd          file system descriptor
 * @param  fd           file descriptor
 * @param *stat         pointer to status structure
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
stdRet_t lfs_fstat(fsd_t fsd, fd_t fd, struct vfs_stat *stat)
{
        (void) fsd;

        if (stat && lfs) {
                while (TakeMutex(lfs->mtx, MTX_BLOCK_TIME) != OS_OK);

                fopenInfo_t *foi = ListGetItemDataByID(lfs->openFile, fd);

                if (foi) {
                        if (foi->node) {
                                stat->st_dev   = foi->node->dev;
                                stat->st_rdev  = foi->node->part;
                                stat->st_gid   = foi->node->gid;
                                stat->st_mode  = foi->node->mode;
                                stat->st_mtime = foi->node->mtime;
                                stat->st_size  = foi->node->size;
                                stat->st_uid   = foi->node->uid;

                                GiveMutex(lfs->mtx);
                                return STD_RET_OK;
                        }
                }

                GiveMutex(lfs->mtx);
        }

        return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief Function returns FS status
 *
 * @param dev           fs device
 * @param *statfs       pointer to status structure
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
stdRet_t lfs_statfs(fsd_t fsd, struct vfs_statfs *statfs)
{
        (void) fsd;

        if (statfs) {
                statfs->f_bfree  = memman_GetFreeHeapSize();
                statfs->f_blocks = memman_GetHeapSize();
                statfs->f_ffree  = 0;
                statfs->f_files  = 0;
                statfs->f_type   = 0x01;
                statfs->fsname   = "lfs";

                return STD_RET_OK;
        }

        return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief Function release file system
 *
 * @param dev           fs device
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
stdRet_t lfs_release(fsd_t fsd)
{
        (void) fsd;

        return STD_RET_OK;
}

//==============================================================================
/**
 * @brief Function open selected file
 *
 * @param [in]  fsd             file system descriptor
 * @param[out] *fd              file descriptor
 * @param[out] *seek            file position
 * @param [in] *path            file path
 * @param [in] *mode            file mode
 *
 * @retval STD_RET_OK           file opened/created
 * @retval STD_RET_ERROR        file not opened/created
 */
//==============================================================================
stdRet_t lfs_open(fsd_t fsd, fd_t *fd, size_t *seek, const ch_t *path, const ch_t *mode)
{
        (void) fsd;

        node_t *node;
        node_t *nodebase;
        ch_t   *filename;
        i32_t   item;
        u32_t   cfd;


        if (!fd || !path || !mode || !lfs) {
                return STD_RET_ERROR;
        }

        while (TakeMutex(lfs->mtx, MTX_BLOCK_TIME) != OS_OK);

        node     = GetNode(path, &lfs->root, 0, &item);
        nodebase = GetNode(path, &lfs->root, -1, NULL);

        /* create new file when necessary */
        if (nodebase && node == NULL) {
                if (   (strncmp("w",  mode, 2) != 0)
                    && (strncmp("w+", mode, 2) != 0)
                    && (strncmp("a",  mode, 2) != 0)
                    && (strncmp("a+", mode, 2) != 0) ) {
                        goto lfs_open_error;
                }

                filename = calloc(1, strlen(strrchr(path, '/')));
                if (filename == NULL) {
                        goto lfs_open_error;
                }

                strcpy(filename, strrchr(path, '/') + 1);
                node = CreateNode(nodebase, filename, &item);

                if (node == NULL) {
                        free(filename);
                        goto lfs_open_error;
                }
        }

        /* file shall exist */
        if (!node || !nodebase || item < 0) {
                goto lfs_open_error;
        }

        /* node must be a file */
        if (node->type == NODE_TYPE_DIR) {
                goto lfs_open_error;
        }

        /* add file to list of open files */
        if (AddNodeToListOfOpenFiles(nodebase, node,
                                     &item) == STD_RET_ERROR) {
                goto lfs_open_error;
        }

        /* set file parameters */
        if (node->type == NODE_TYPE_FILE) {
                /* set seek at begin if selected */
                if (  strncmp("r",  mode, 2) == 0
                   || strncmp("r+", mode, 2) == 0
                   || strncmp("w",  mode, 2) == 0
                   || strncmp("w+", mode, 2) == 0 ) {
                        *seek = 0;
                }

                /* set file size */
                if (  strncmp("w",  mode, 2) == 0
                   || strncmp("w+", mode, 2) == 0 ) {

                        if (node->data) {
                                free(node->data);
                                node->data = NULL;
                        }

                        node->size = 0;
                }

                /* set seek at file end */
                if (  strncmp("a",  mode, 2) == 0
                   || strncmp("a+", mode, 2) == 0 ) {
                        *seek = node->size;
                }
        } else if (node->type == NODE_TYPE_DRV) {
                struct vfs_drvcfg *drv = node->data;

                if (drv->f_open == NULL) {
                        goto lfs_open_error;
                }

                if (drv->f_open(drv->dev, drv->part) == STD_RET_OK) {
                        *seek = 0;
                } else {
                        ListRmItemByNo(lfs->openFile, item);
                        goto lfs_open_error;
                }
        }

        /* everything success - load FD */
        ListGetItemID(lfs->openFile, item, &cfd);
        *fd = (fd_t)cfd;
        GiveMutex(lfs->mtx);
        return STD_RET_OK;

        lfs_open_error:
        GiveMutex(lfs->mtx);
        return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief Function close file in LFS
 * Function return always STD_RET_OK, to close file in LFS no operation is
 * needed.
 *
 * @param dev     device number
 * @param fd      file descriptor
 *
 * @retval STD_RET_OK
 */
//==============================================================================
stdRet_t lfs_close(fsd_t fsd, fd_t fd)
{
        (void) fsd;

        stdRet_t           status = STD_RET_ERROR;
        node_t            *node;
        fopenInfo_t       *foi;
        struct vfs_drvcfg *drv;
        fopenInfo_t        finf;

        if (lfs == NULL) {
                return STD_RET_ERROR;
        }

        while (TakeMutex(lfs->mtx, MTX_BLOCK_TIME) != OS_OK);

        foi = ListGetItemDataByID(lfs->openFile, fd);
        if (foi == NULL) {
                goto lfs_close_end;
        }

        node = foi->node;
        if (node == NULL) {
                goto lfs_close_end;
        }

        /* close device if file is driver type */
        if (node->type == NODE_TYPE_DRV) {
                if (node->data == NULL) {
                        goto lfs_close_end;
                }

                drv = node->data;
                if (drv->f_close == NULL) {
                        goto lfs_close_end;
                }

                if ((status = drv->f_close(drv->dev, drv->part)) != STD_RET_OK) {
                        goto lfs_close_end;
                }
        }

        /* delete file from open list */
        finf = *foi;

        if (ListRmItemByID(lfs->openFile, fd) != 0) {
                /* critical error! */
                goto lfs_close_end;
        }

        /* file to remove, check if other app does not opens this file */
        status = STD_RET_OK;

        if (finf.removeAtClose == TRUE) {
                i32_t n = ListGetItemCount(lfs->openFile);

                for (int_t i = 0; i < n; i++) {
                        foi = ListGetItemDataByNo(lfs->openFile, i);

                        if (foi->node == node) {
                                goto lfs_close_end;
                        }
                }

                /* file can be removed */
                status = rmNode(finf.nodebase, finf.node, finf.itemID);
        }

        lfs_close_end:
        GiveMutex(lfs->mtx);
        return status;
}

//==============================================================================
/**
 * @brief Function write to file data
 *
 * @param  fsd          file system descriptor
 * @param  fd           file descriptor
 * @param *src          data source
 * @param  size         item size
 * @param  nitems       number of items
 * @param  seek         position in file
 *
 * @return number of written items
 */
//==============================================================================
size_t lfs_write(fsd_t fsd, fd_t fd, void *src, size_t size, size_t nitems, size_t seek)
{
        (void) fsd;

        node_t            *node;
        fopenInfo_t       *foi;
        struct vfs_drvcfg *drv;
        ch_t              *newdata;
        size_t             wrsize;
        size_t             filelen;
        size_t             n = 0;


        if (!src || !size || !nitems || !lfs) {
                return 0;
        }

        while (TakeMutex(lfs->mtx, MTX_BLOCK_TIME) != OS_OK);
        foi = ListGetItemDataByID(lfs->openFile, fd);

        if (foi == NULL) {
                goto lfs_write_end;
        }

        node = foi->node;
        if (node == NULL) {
                goto lfs_write_end;
        }

        if (node->type == NODE_TYPE_DRV && node->data) {
                drv = node->data;

                if (drv->f_write) {
                        GiveMutex(lfs->mtx);

                        return drv->f_write(drv->dev, drv->part, src, size,
                                            nitems, seek);
                }
        } else if (node->type == NODE_TYPE_FILE) {
                wrsize  = size * nitems;
                filelen = node->size;

                if (seek > filelen) {
                        seek = filelen;
                }

                if ((seek + wrsize) > filelen || node->data == NULL) {
                        newdata = malloc(filelen + wrsize);
                        if (newdata == NULL) {
                                goto lfs_write_end;
                        }

                        if (node->data) {
                                memcpy(newdata, node->data, filelen);
                                free(node->data);
                        }

                        memcpy(newdata + seek, src, wrsize);

                        node->data  = newdata;
                        node->size += wrsize - (filelen - seek);

                        n = nitems;
                } else {
                        memcpy(node->data + seek, src, wrsize);
                        n = nitems;
                }
        }

        lfs_write_end:
        GiveMutex(lfs->mtx);
        return n;
}

//==============================================================================
/**
 * @brief Function read from file data
 *
 * @param  fsd          file system descriptor
 * @param  fd           file descriptor
 * @param *dst          data destination
 * @param  size         item size
 * @param  nitems       number of items
 * @param  seek         position in file
 *
 * @return number of read items
 */
//==============================================================================
size_t lfs_read(fsd_t fsd, fd_t fd, void *dst, size_t size, size_t nitems, size_t seek)
{
        (void)fsd;

        node_t            *node;
        fopenInfo_t       *foi;
        struct vfs_drvcfg *drv;
        size_t             filelen;
        size_t             items2rd;
        size_t             n = 0;


        if (!dst || !size || !nitems || !lfs) {
                return 0;
        }

        while (TakeMutex(lfs->mtx, MTX_BLOCK_TIME) != OS_OK);

        foi = ListGetItemDataByID(lfs->openFile, fd);
        if (foi == NULL) {
                goto lfs_read_end;
        }

        node = foi->node;
        if (node == NULL) {
                goto lfs_read_end;
        }

        if (node->type == NODE_TYPE_DRV && node->data) {
                drv = node->data;

                if (drv->f_read) {
                        GiveMutex(lfs->mtx);
                        return drv->f_read(drv->dev, drv->part, dst, size,
                                           nitems, seek);
                }
        } else if (node->type == NODE_TYPE_FILE) {
                filelen = node->size;

                /* check if seek is not bigger than file length */
                if (seek > filelen) {
                        seek = filelen;
                }

                /* check how many items to read is on current file position */
                if (((filelen - seek) / size) >= nitems) {
                        items2rd = nitems;
                } else {
                        items2rd = (filelen - seek) / size;
                }

                /* copy if file buffer exist */
                if (node->data) {
                        if (items2rd > 0) {
                                memcpy(dst, node->data + seek, items2rd * size);
                                n = items2rd;
                        }
                }
        }

        lfs_read_end:
        GiveMutex(lfs->mtx);
        return n;
}

//==============================================================================
/**
 * @brief IO operations on files
 *
 * @param  fsd    file system descriptor
 * @param  fd     file descriptor
 * @param  iorq   request
 * @param *data   data pointer
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
stdRet_t lfs_ioctl(fsd_t fsd, fd_t fd, IORq_t iorq, void *data)
{
        (void) fsd;

        fopenInfo_t       *foi;
        struct vfs_drvcfg *drv;


        if (lfs == NULL) {
                return STD_RET_ERROR;
        }

        while (TakeMutex(lfs->mtx, MTX_BLOCK_TIME) != OS_OK);

        foi = ListGetItemDataByID(lfs->openFile, fd);
        if (foi == NULL) {
                goto lfs_ioctl_end;
        }

        if (foi->node == NULL) {
                goto lfs_ioctl_end;
        }

        if (foi->node->type == NODE_TYPE_DRV && foi->node->data) {
                drv = foi->node->data;

                if (drv->f_ioctl) {
                        GiveMutex(lfs->mtx);
                        return drv->f_ioctl(foi->node->dev, foi->node->part,
                                            iorq, data);
                }
        }

        lfs_ioctl_end:
        GiveMutex(lfs->mtx);
        return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief Remove selected node
 *
 * @param *base            base node
 * @param *target          target node
 * @param  baseitemid      item in base node that point to target
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
static stdRet_t rmNode(node_t *base, node_t *target, u32_t baseitemid)
{
        /* if DIR check if is empty */
        if (target->type == NODE_TYPE_DIR) {

                if (ListGetItemCount(target->data) > 0) {
                        return STD_RET_ERROR;
                } else {
                        ListDelete(target->data);
                        target->data = NULL;
                }
        }

        if (target->name) {
                free(target->name);
        }

        if (target->data) {
                free(target->data);
        }

        if (ListRmItemByID(base->data, baseitemid) == 0) {
                return STD_RET_OK;
        }

        return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief Check path deep
 *
 * @param *path         path
 *
 * @return path deep
 */
//==============================================================================
static uint_t GetPathDeep(const ch_t *path)
{
        uint_t      deep     = 0;
        const ch_t *lastpath = NULL;

        if (path[0] == '/') {
                lastpath = path++;

                while ((path = strchr(path, '/'))) {
                        lastpath = path;
                        path++;
                        deep++;
                }

                if (lastpath[1] != '\0') {
                        deep++;
                }
        }

        return deep;
}

//==============================================================================
/**
 * @brief Function find node by path
 *
 * @param[in]  *path          path
 * @param[in]  *startnode     start node
 * @param[out] **extPath      external path begin (pointer from path)
 * @param[in]   deep          deep control
 * @param[out] *item          node is n-item of list which was found
 *
 * @return node
 */
//==============================================================================
static node_t *GetNode(const ch_t *path, node_t *startnode, i32_t deep, i32_t *item)
{
        node_t *curnode;
        node_t *node;
        ch_t   *pathend;
        int_t   dirdeep;
        uint_t  pathlen;
        int_t   listsize;


        if (!path || !startnode) {
                return NULL;
        }

        if (startnode->type != NODE_TYPE_DIR) {
                return NULL;
        }

        curnode = startnode;
        dirdeep = GetPathDeep(path);

        /* go to selected node -----------------------------------------------*/
        while (dirdeep + deep > 0) {
                /* get element from path */
                if ((path = strchr(path, '/')) == NULL) {
                        break;
                } else {
                        path++;
                }

                if ((pathend = strchr(path, '/')) == NULL) {
                        pathlen = strlen(path);
                } else {
                        pathlen = pathend - path;
                }

                /* get number of list items */
                listsize = ListGetItemCount(curnode->data);

                /* find that object exist ------------------------------------*/
                int_t i = 0;
                while (listsize > 0) {
                        node = ListGetItemDataByNo(curnode->data, i++);

                        if (node == NULL) {
                                dirdeep = 1 - deep;
                                break;
                        }

                        if (  strlen(node->name) == pathlen
                           && strncmp(node->name, path, pathlen) == 0 ) {

                                curnode = node;

                                if (item) {
                                        *item = i - 1;
                                }

                                break;
                        }

                        listsize--;
                }

                /* directory does not found or error */
                if (listsize == 0 || curnode == NULL) {
                        curnode = NULL;
                        break;
                }

                dirdeep--;
        }

        return curnode;
}

//==============================================================================
/**
 * @brief Function create new file in selected node
 * Function allocate new node. If node is created successfully then filename
 * cannot be freed!
 *
 * @param [in] *nodebase        node base
 * @param [in] *filename        filename (must be earlier allocated)
 * @param[out] *item            new node number in base node
 *
 * @return NULL if error, otherwise new node address
 */
//==============================================================================
static node_t *CreateNode(node_t *nodebase, ch_t *filename, i32_t *item)
{
        node_t *fnode;
        i32_t   nitem;

        if (!nodebase || !filename) {
                return NULL;
        }

        if (nodebase->type != NODE_TYPE_DIR) {
                return NULL;
        }

        if ((fnode = calloc(1, sizeof(node_t))) == NULL) {
                return NULL;
        }

        fnode->name  = filename;
        fnode->data  = NULL;
        fnode->dev   = 0;
        fnode->gid   = 0;
        fnode->mode  = 0;
        fnode->mtime = 0;
        fnode->part  = 0;
        fnode->size  = 0;
        fnode->type  = NODE_TYPE_FILE;
        fnode->uid   = 0;

        if ((nitem = ListAddItem(nodebase->data, lfs->idcnt++, fnode)) < 0) {
                free(fnode);
                return NULL;
        }

        *item = nitem;
        return fnode;
}

//==============================================================================
/**
 * @brief Function add node to list of open files
 *
 * @param [in] *nodebase        base node
 * @param [in] *node            node data added to list of open files
 * @param [io] *item            in:  node number in base node
 *                              out: open file's number in list of open files
 *
 * @retval STD_RET_OK           file registered in list of open files
 * @retval STD_RET_ERROR        file not registered
 */
//==============================================================================
static stdRet_t AddNodeToListOfOpenFiles(node_t *nodebase, node_t *node, i32_t *item)
{
        fopenInfo_t *openFileInfo;
        fopenInfo_t *openedFile;
        i32_t        openFileCount;
        i32_t        openFileListPos;

        if ((openFileInfo = calloc(1, sizeof(fopenInfo_t))) == NULL) {
                return STD_RET_ERROR;
        }

        openFileInfo->removeAtClose = FALSE;
        openFileInfo->node          = node;
        openFileInfo->nodebase      = nodebase;

        if (ListGetItemID(nodebase->data, *item, &openFileInfo->itemID) != 0) {
                goto AddFileToListOfOpenFiles_Error;
        }

        /* find if file shall be removed */
        openFileCount = ListGetItemCount(lfs->openFile);

        for (i32_t i = 0; i < openFileCount; i++) {
                openedFile = ListGetItemDataByNo(lfs->openFile, i);

                if (openedFile->node != node) {
                        continue;
                }

                if (openedFile->removeAtClose == TRUE) {
                        openFileInfo->removeAtClose = TRUE;
                        break;
                }
        }

        /* add open file info to list */
        openFileListPos = ListAddItem(lfs->openFile,
                                      lfs->idcnt++,
                                      openFileInfo);

        if (openFileListPos >= 0) {
                *item = openFileListPos;
                return STD_RET_OK;
        }

        AddFileToListOfOpenFiles_Error:
        free(openFileInfo);
        return STD_RET_ERROR;
}

#ifdef __cplusplus
}
#endif

/*==============================================================================
  End of file
==============================================================================*/
