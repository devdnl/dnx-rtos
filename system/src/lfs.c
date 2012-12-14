/*=============================================================================================*//**
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


*//*==============================================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*==================================================================================================
                                            Include files
==================================================================================================*/
#include "lfs.h"
#include "dlist.h"
#include <string.h>
#include "oswrap.h"


/*==================================================================================================
                                  Local symbolic constants/macros
==================================================================================================*/
/* wait time for operation on FS */
#define MTX_BLOCK_TIME                    10

#define calloc(nmemb, msize)              LFS_CALLOC(nmemb, msize)
#define malloc(size)                      LFS_MALLOC(size)
#define free(mem)                         LFS_FREE(mem)


/*==================================================================================================
                                   Local types, enums definitions
==================================================================================================*/
typedef enum {
      NODE_TYPE_DIR  = FILE_TYPE_DIR,
      NODE_TYPE_FILE = FILE_TYPE_REGULAR,
      NODE_TYPE_DRV  = FILE_TYPE_DRV,
      NODE_TYPE_LINK = FILE_TYPE_LINK
} nodeType_t;

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

typedef struct openInfo {
      node_t *node;           /* opened node */
      node_t *nodebase;       /* base of opened node */
      //ch_t   *taskName;       /* task name (debug only) */
      bool_t  doRM;           /* file to remove after close */
      u32_t   itemID;         /* item ID in base directory list */
} fopenInfo_t;

struct fshdl_s {
      node_t   root;          /* root dir '/' */
      mutex_t  mtx;           /* lock mutex */
      list_t  *openFile;      /* list with opened files */
};


/*==================================================================================================
                                      Local function prototypes
==================================================================================================*/
static stdRet_t  rmNode(node_t *base, node_t *target, u32_t baseitemid);
static node_t   *GetNode(const ch_t *path, node_t *startnode, i32_t deep, i32_t *item);
static i32_t     GetPathDeep(const ch_t *path);
static dirent_t  lfs_readdir(DIR_t *dir);

/*==================================================================================================
                                      Local object definitions
==================================================================================================*/
static struct fshdl_s *lfs;


/*==================================================================================================
                                        Function definitions
==================================================================================================*/

//================================================================================================//
/**
 * @brief Initialize VFS module
 *
 * @param  dev          device number
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//================================================================================================//
stdRet_t lfs_init(devx_t dev)
{
      (void)dev;

      stdRet_t ret = STD_RET_OK;

      if (lfs == NULL) {
            lfs = calloc(1, sizeof(struct fshdl_s));

            if (lfs) {
                  CreateMutex(lfs->mtx);
                  lfs->root.data = ListCreate();
                  lfs->openFile  = ListCreate();

                  if (!lfs->mtx || !lfs->root.data || !lfs->openFile) {
                        if (lfs->mtx)
                              DeleteMutex(lfs->mtx);

                        if (lfs->root.data)
                              ListDestroy(lfs->root.data);

                        if (lfs->openFile)
                              ListDestroy(lfs->openFile);

                        free(lfs);

                        lfs = NULL;
                  } else {
                        lfs->root.name = "/";
                        lfs->root.size = sizeof(node_t);
                        lfs->root.type = NODE_TYPE_DIR;
                  }
            }

            if (lfs == NULL)
                  ret = STD_RET_ERROR;
      }

      return ret;
}


//================================================================================================//
/**
 * @brief Function create node for driver file
 *
 * @param  dev                device number
 * @param *path               path when driver-file shall be created
 * @param *drvcfg             pointer to description of driver
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//================================================================================================//
stdRet_t lfs_mknod(devx_t dev, const ch_t *path, struct vfs_drvcfg *drvcfg)
{
      (void)dev;

      stdRet_t status = STD_RET_ERROR;

      if (path && drvcfg && lfs) {
            if (path[0] == '/') {
                  while (TakeMutex(lfs->mtx, MTX_BLOCK_TIME) != OS_OK);

                  node_t *node   = GetNode(path, &lfs->root, -1, NULL);
                  node_t *ifnode = GetNode(strrchr(path, '/'), node, 0, NULL);

                  /* check if target node is OK */
                  if (node && !ifnode) {
                        if (node->type == NODE_TYPE_DIR) {
                              ch_t  *drvname    = strrchr(path, '/') + 1;
                              u32_t  drvnamelen = strlen(drvname);
                              ch_t  *filename   = calloc(drvnamelen + 1, sizeof(ch_t));

                              if (filename) {
                                    strcpy(filename, drvname);

                                    node_t         *dirfile = calloc(1, sizeof(node_t));
                                    struct vfs_drvcfg *dcfg = calloc(1, sizeof(struct vfs_drvcfg));

                                    if (dirfile && dcfg) {
                                          memcpy(dcfg, drvcfg, sizeof(struct vfs_drvcfg));

                                          dirfile->name  = filename;
                                          dirfile->size  = sizeof(node_t) + strlen(filename)
                                                         + sizeof(struct vfs_drvcfg);
                                          dirfile->type  = NODE_TYPE_DRV;
                                          dirfile->data  = dcfg;
                                          dirfile->dev   = dcfg->dev;
                                          dirfile->part  = dcfg->part;

                                          /* add new drv to this folder */
                                          if (ListAddItem(node->data, dirfile) >= 0) {
                                                status = STD_RET_OK;
                                          }
                                    }

                                    /* free memory when error */
                                    if (status == STD_RET_ERROR) {
                                          if (dirfile)
                                                free(dirfile);

                                          if (dcfg)
                                                free(dcfg);

                                          free(filename);
                                    }
                              }
                        }
                  }

                  GiveMutex(lfs->mtx);
            }
      }

      return status;
}


//================================================================================================//
/**
 * @brief Create directory
 *
 * @param  dev          device number
 * @param *path         path to new directory
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//================================================================================================//
stdRet_t lfs_mkdir(devx_t dev, const ch_t *path)
{
      (void)dev;

      stdRet_t status = STD_RET_ERROR;

      if (path && lfs) {
            if (path[0] == '/') {
                  while (TakeMutex(lfs->mtx, MTX_BLOCK_TIME) != OS_OK);

                  node_t *node   = GetNode(path, &lfs->root, -1, NULL);
                  node_t *ifnode = GetNode(strrchr(path, '/'), node, 0, NULL);

                  /* check if target node is OK and the same name doesn't exist */
                  if (node && !ifnode) {
                        /* internal FS */
                        if (node->type ==  NODE_TYPE_DIR) {
                              ch_t  *dirname    = strrchr(path, '/') + 1;
                              u32_t  dirnamelen = strlen(dirname);
                              ch_t  *name       = calloc(dirnamelen + 1, sizeof(ch_t));

                              /* check if name buffer is created */
                              if (name) {
                                    strcpy(name, dirname);

                                    node_t *dir = calloc(1, sizeof(node_t));

                                    if (dir) {
                                          dir->data = ListCreate();

                                          if (dir->data) {
                                                dir->name = (ch_t*)name;
                                                dir->size = sizeof(node_t) + strlen(name);
                                                dir->type = NODE_TYPE_DIR;

                                                /* add new folder to this folder */
                                                if (ListAddItem(node->data, dir) >= 0) {
                                                      status = STD_RET_OK;
                                                }
                                          }
                                    }

                                    /* free memory when error */
                                    if (status == STD_RET_ERROR) {
                                          if (dir) {
                                                if (dir->data)
                                                      ListDestroy(dir->data);

                                                free(dir);
                                          }

                                          free(name);
                                    }
                              }
                        }
                  }

                  GiveMutex(lfs->mtx);
            }
      }

      return status;
}


//================================================================================================//
/**
 * @brief Function open directory
 *
 * @param  dev          device number
 * @param *path         directory path
 * @param *dir          directory info
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//================================================================================================//
stdRet_t lfs_opendir(devx_t dev, const ch_t *path, DIR_t *dir)
{
      (void)dev;

      stdRet_t status = STD_RET_ERROR;

      if (path && lfs) {
            while (TakeMutex(lfs->mtx, MTX_BLOCK_TIME) != OS_OK);

            /* go to target dir */
            node_t *node = GetNode(path, &lfs->root, 0, NULL);

            if (node) {
                  if (node->type == NODE_TYPE_DIR) {
                        if (dir) {
                              dir->items = ListGetItemCount(node->data);
                              dir->rddir = lfs_readdir;
                              dir->seek  = 0;
                              dir->dd    = node;
                        }

                        status = STD_RET_OK;
                  }
            }

            GiveMutex(lfs->mtx);
      }

      return status;
}


//================================================================================================//
/**
 * @brief Function read next item of opened directory
 *
 * @param *dir          directory object
 *
 * @return element attributes
 */
//================================================================================================//
static dirent_t lfs_readdir(DIR_t *dir)
{
      dirent_t dirent;
      dirent.name   = NULL;
      dirent.size   = 0;

      if (dir && lfs) {
            while (TakeMutex(lfs->mtx, MTX_BLOCK_TIME) != OS_OK);

            node_t *from = dir->dd;
            node_t *node = ListGetItemDataByNo(from->data, dir->seek++);

            if (node) {
                  dirent.filetype = node->type;
                  dirent.name     = node->name;
                  dirent.size     = node->size;
            }

            GiveMutex(lfs->mtx);
      }

      return dirent;
}


//================================================================================================//
/**
 * @brief Remove file
 *
 * @param  dev          device number
 * @param *patch        localization of file/directory
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//================================================================================================//
stdRet_t lfs_remove(devx_t dev, const ch_t *path)
{
      (void)dev;

      stdRet_t status = STD_RET_ERROR;

      if (path && lfs) {
            while (TakeMutex(lfs->mtx, MTX_BLOCK_TIME) != OS_OK);

            i32_t   item;
            bool_t  dorm     = TRUE;
            node_t *nodebase = GetNode(path, &lfs->root, -1, NULL);
            node_t *nodeobj  = GetNode(path, &lfs->root, 0, &item);

            /* check if target nodes are OK */
            if (nodebase && nodeobj && nodeobj != &lfs->root) {

                  /* if path is ending on slash, the object must be DIR */
                  if (path[strlen(path) - 1] == '/') {
                        if (nodeobj->type != NODE_TYPE_DIR) {
                              goto lfs_remove_end;
                        }
                  }

                  /* check if file is opened */
                  if (nodeobj->type != NODE_TYPE_DIR) {
                        i16_t n = ListGetItemCount(lfs->openFile);

                        for (i16_t i = 0; i < n; i++) {
                              fopenInfo_t *olfoi = ListGetItemDataByNo(lfs->openFile, i);

                              if (olfoi->node == nodeobj) {
                                    olfoi->doRM = TRUE;
                                    dorm        = FALSE;
                              }
                        }
                  }

                  /* remove node if possible */
                  if (dorm == TRUE) {
                        u32_t itemid;

                        if (ListGetItemID(nodebase->data, item, &itemid) == 0)
                              status = rmNode(nodebase, nodeobj, itemid);
                  } else {
                        status = STD_RET_OK;
                  }
            }

            lfs_remove_end:
            GiveMutex(lfs->mtx);
      }

      return status;
}


//================================================================================================//
/**
 * @brief Rename file name
 * The implementation of rename can move files only if external FS provide functionality. Local
 * VFS can not do this.
 *
 * @param  dev                device number
 * @param *oldName            old file name
 * @param *newName            new file name
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//================================================================================================//
stdRet_t lfs_rename(devx_t dev, const ch_t *oldName, const ch_t *newName)
{
      (void)dev;

      stdRet_t status = STD_RET_ERROR;

      if (oldName && newName && lfs) {
            while (TakeMutex(lfs->mtx, MTX_BLOCK_TIME) != OS_OK);

            node_t *oldNodeBase = GetNode(oldName, &lfs->root, -1, NULL);
            node_t *newNodeBase = GetNode(newName, &lfs->root, -1, NULL);

            if (  oldNodeBase && newNodeBase && oldName[0] == '/' && newName[0] == '/'
               && oldName[strlen(oldName) - 1] != '/' && newName[strlen(newName) - 1] != '/') {

                  if (oldNodeBase == newNodeBase) {
                        ch_t   *name = calloc(1, strlen(strrchr(newName, '/') + 1));
                        node_t *node = GetNode(oldName, &lfs->root, 0, NULL);

                        if (name && node) {
                              strcpy(name, strrchr(newName, '/') + 1);

                              if (node->name)
                                    free(node->name);

                              node->name = name;

                              if (node->type == NODE_TYPE_DIR)
                                    node->size = sizeof(node_t) + strlen(name);
                              else if (node->type == NODE_TYPE_DRV)
                                    node->size = sizeof(node_t) + strlen(name) + sizeof(struct vfs_drvcfg);

                              status = STD_RET_OK;
                        } else {
                              if (name)
                                    free(name);

                              if (node)
                                    free(node);
                        }
                  }
            }

            GiveMutex(lfs->mtx);
      }

      return status;
}


//================================================================================================//
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
//================================================================================================//
stdRet_t lfs_chmod(devx_t dev, const ch_t *path, u32_t mode)
{
      (void)dev;

      stdRet_t status = STD_RET_ERROR;

      if (path && lfs) {
            while (TakeMutex(lfs->mtx, MTX_BLOCK_TIME) != OS_OK);

            node_t *node = GetNode(path, &lfs->root, 0, NULL);

            if (node) {
                  node->mode = mode;

                  status = STD_RET_OK;
            }

            GiveMutex(lfs->mtx);
      }

      return status;
}


//================================================================================================//
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
//================================================================================================//
stdRet_t lfs_chown(devx_t dev, const ch_t *path, u16_t owner, u16_t group)
{
      (void)dev;
      stdRet_t status = STD_RET_ERROR;

      if (path && lfs) {
            while (TakeMutex(lfs->mtx, MTX_BLOCK_TIME) != OS_OK);

            node_t *node = GetNode(path, &lfs->root, 0, NULL);

            if (node) {
                  node->uid = owner;
                  node->gid = group;

                  status = STD_RET_OK;
            }

            GiveMutex(lfs->mtx);
      }

      return status;
}


//================================================================================================//
/**
 * @brief Function returns file/dir status
 *
 * @param  dev          device number
 * @param *path         file/dir path
 * @param *stat         pointer to stat structure
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//================================================================================================//
stdRet_t lfs_stat(devx_t dev, const ch_t *path, struct vfs_stat *stat)
{
      (void)dev;

      stdRet_t status = STD_RET_ERROR;

      if (path && stat && lfs) {
            while (TakeMutex(lfs->mtx, MTX_BLOCK_TIME) != OS_OK);

            node_t *node = GetNode(path, &lfs->root, 0, NULL);

            if (node) {
                  if (  (path[strlen(path) - 1] == '/' && node->type == NODE_TYPE_DIR)
                     ||  path[strlen(path) - 1] != '/') {
                        stat->st_dev   = node->dev;
                        stat->st_rdev  = node->part;
                        stat->st_gid   = node->gid;
                        stat->st_mode  = node->mode;
                        stat->st_mtime = node->mtime;
                        stat->st_size  = node->size;
                        stat->st_uid   = node->uid;

                        status = STD_RET_OK;
                  }
            }

            GiveMutex(lfs->mtx);
      }

      return status;
}


//================================================================================================//
/**
 * @brief Function returns file status
 *
 * @param  dev          device number
 * @param  fd           file descriptor
 * @param *stat         pointer to status structure
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//================================================================================================//
stdRet_t lfs_fstat(devx_t dev, fd_t fd, struct vfs_stat *stat)
{
      (void)dev;

      stdRet_t status = STD_RET_ERROR;

      if (stat && lfs) {
            node_t *node = NULL;

            while (TakeMutex(lfs->mtx, MTX_BLOCK_TIME) != OS_OK);

            fopenInfo_t *foi = ListGetItemDataByID(lfs->openFile, fd);

            if (foi)
                  node = foi->node;

            if (node) {
                  stat->st_dev   = node->dev;
                  stat->st_rdev  = node->part;
                  stat->st_gid   = node->gid;
                  stat->st_mode  = node->mode;
                  stat->st_mtime = node->mtime;
                  stat->st_size  = node->size;
                  stat->st_uid   = node->uid;
            }

            GiveMutex(lfs->mtx);
      }

      return status;
}


//================================================================================================//
/**
 * @brief Function returns FS status
 *
 * @param dev           fs device
 * @param *statfs       pointer to status structure
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//================================================================================================//
stdRet_t lfs_statfs(devx_t dev, struct vfs_statfs *statfs)
{
      (void)dev;

      stdRet_t status = STD_RET_ERROR;

      if (statfs) {
            statfs->f_bfree  = mm_GetFreeHeapSize();
            statfs->f_blocks = mm_GetHeapSize();
            statfs->f_ffree  = 0;
            statfs->f_files  = 0;
            statfs->f_type   = 0x01;
            statfs->fsname   = "LFS";

            status = STD_RET_OK;
      }

      return status;
}


//================================================================================================//
/**
 * @brief Function release file system
 *
 * @param dev           fs device
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//================================================================================================//
stdRet_t lfs_release(devx_t dev)
{
      (void)dev;

      return STD_RET_OK;
}


//================================================================================================//
/**
 * @brief Function open selected file
 *
 * @param  dev          device number
 * @param *name         file path
 * @param *mode         file mode
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//================================================================================================//
stdRet_t lfs_open(devx_t dev, fd_t *fd, size_t *seek, const ch_t *path, const ch_t *mode)
{
      (void)dev;

      stdRet_t status = STD_RET_ERROR;

      if (fd && path && mode && lfs) {
            while (TakeMutex(lfs->mtx, MTX_BLOCK_TIME) != OS_OK);

            i32_t   item;
            node_t *node     = GetNode(path, &lfs->root, 0, &item);
            node_t *nodebase = GetNode(path, &lfs->root, -1, NULL);

            /* create file when mode is correct and when file does not exist -------------------- */
            if (  (  (strncmp("w", mode, 2) == 0) || (strncmp("w+", mode, 2) == 0)
                  || (strncmp("a", mode, 2) == 0) || (strncmp("a+", mode, 2) == 0) )
               && nodebase && node == NULL                                           ) {

                  if (nodebase->type == NODE_TYPE_DIR) {
                        ch_t   *filename = calloc(1, strlen(strrchr(path, '/')));
                        node_t *fnode    = calloc(1, sizeof(node_t));

                        if (filename && fnode) {
                              strcpy(filename, strrchr(path, '/') + 1);

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

                              node = fnode;

                              if ((item = ListAddItem(nodebase->data, fnode)) < 0) {
                                    free(filename);
                                    free(fnode);
                                    goto lfs_open_end;
                              }
                        } else {
                              if (filename)
                                    free(filename);

                              if (fnode)
                                    free(fnode);

                              goto lfs_open_end;
                        }
                  }
            }

            /* file exist ----------------------------------------------------------------------- */
            if (node && nodebase && item >= 0) {

                  if (node->type != NODE_TYPE_DIR) {

                        /* add file to opened files list */
                        fopenInfo_t *foi = calloc(1, sizeof(fopenInfo_t));

                        if (foi) {
                              foi->doRM     = FALSE;
                              foi->node     = node;
                              foi->nodebase = nodebase;
                              //foi->taskName = GET_TASK_NAME();

                              if (ListGetItemID(nodebase->data, item, &foi->itemID) == 0) {

                                    /* find if file shall be removed */
                                    fopenInfo_t *olfoi;
                                    i16_t n = ListGetItemCount(lfs->openFile);

                                    for (i16_t i = 0; i < n; i++) {
                                          olfoi = ListGetItemDataByNo(lfs->openFile, i);

                                          if (olfoi->node == node) {
                                                if (olfoi->doRM == TRUE) {
                                                   foi->doRM = TRUE;
                                                   break;
                                                }
                                          }
                                    }

                                    /* add open file info to list */
                                    if ((item = ListAddItem(lfs->openFile, foi)) < 0) {
                                          free(foi);
                                          goto lfs_open_end;
                                    }
                              }

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

                              status = STD_RET_OK;

                        } else if (node->type == NODE_TYPE_DRV) {
                              struct vfs_drvcfg *drv = node->data;

                              if (drv->f_open) {
                                    if (drv->f_open(drv->dev, drv->part) == STD_RET_OK) {
                                          *seek  = 0;
                                          status = STD_RET_OK;
                                    } else {
                                          ListRmItemByNo(lfs->openFile, item);
                                          goto lfs_open_end;
                                    }
                              }
                        }

                        /* everything success - load FD */
                        ListGetItemID(lfs->openFile, item, fd);
                  }
            }

            lfs_open_end:
            GiveMutex(lfs->mtx);
      }

      return status;
}


//================================================================================================//
/**
 * @brief Function close file in LFS
 * Function return always STD_RET_OK, to close file in LFS no operation is needed.
 *
 * @param dev     device number
 * @param fd      file descriptor
 *
 * @retval STD_RET_OK
 */
//================================================================================================//
stdRet_t lfs_close(devx_t dev, fd_t fd)
{
      (void)dev;

      stdRet_t     status = STD_RET_ERROR;
      node_t      *node;
      fopenInfo_t *foi;

      if (lfs) {
            while (TakeMutex(lfs->mtx, MTX_BLOCK_TIME) != OS_OK);

            foi  = ListGetItemDataByID(lfs->openFile, fd);

            if (foi) {
                  node = foi->node;

                  if (node) {
                        /* close device if file is driver type */
                        if (node->type == NODE_TYPE_DRV) {
                              if (node->data) {
                                    struct vfs_drvcfg *drv = node->data;

                                    if (drv->f_close) {
                                          status = drv->f_close(drv->dev, drv->part);
                                    }
                              }

                              if (status != STD_RET_OK)
                                    goto lfs_close_end;
                        }

                        /* delete file from open list */
                        fopenInfo_t finf = *foi;

                        if (ListRmItemByID(lfs->openFile, fd) != 0) {
                              /* critical error! */
                              goto lfs_close_end;
                        }

                        /* file to remove, check if other app does not opens this file */
                        status = STD_RET_OK;

                        if (finf.doRM == TRUE) {
                              i16_t n = ListGetItemCount(lfs->openFile);

                              for (i16_t i = 0; i < n; i++) {
                                    foi = ListGetItemDataByNo(lfs->openFile, i);

                                    if (foi->node == node) {
                                          goto lfs_close_end;
                                    }
                              }

                              /* file can be removed */
                              status = rmNode(finf.nodebase, finf.node, finf.itemID);
                        }
                  }
            }

            lfs_close_end:
            GiveMutex(lfs->mtx);
      }

      return status;
}


//================================================================================================//
/**
 * @brief Function write to file data
 *
 * @param  dev          device number
 * @param  fd           file descriptor
 * @param *src          data source
 * @param  size         item size
 * @param  nitems       number of items
 * @param  seek         position in file
 *
 * @return number of written items
 */
//================================================================================================//
size_t lfs_write(devx_t dev, fd_t fd, void *src, size_t size, size_t nitems, size_t seek)
{
      (void)dev;

      size_t n = 0;

      if (src && size && nitems && lfs) {
            node_t *node = NULL;

            while (TakeMutex(lfs->mtx, MTX_BLOCK_TIME) != OS_OK);

            fopenInfo_t *foi = ListGetItemDataByID(lfs->openFile, fd);

            if (foi)
                  node = foi->node;

            if (node) {
                  if (node->type == NODE_TYPE_DRV && node->data) {
                        struct vfs_drvcfg *drv = node->data;

                        if (drv->f_write) {
                              GiveMutex(lfs->mtx);
                              n = drv->f_write(drv->dev, drv->part, src, size, nitems, seek);
                              goto lfs_write_end;
                        }
                  } else if (node->type == NODE_TYPE_FILE) {
                        size_t wrsize  = size * nitems;
                        size_t filelen = node->size;

                        if (seek > filelen)
                              seek = filelen;

                        if ((seek + wrsize) > filelen || node->data == NULL) {
                              ch_t *newdata = malloc(filelen + wrsize);

                              if (newdata) {
                                    if (node->data) {
                                          memcpy(newdata, node->data, filelen);
                                          free(node->data);
                                    }

                                    node->data  = newdata;
                                    node->size += wrsize - (filelen - seek);

                                    memcpy(newdata + seek, src, wrsize);
                                    n = nitems;
                              }
                        } else {
                              memcpy(node->data + seek, src, wrsize);
                              n = nitems;
                        }
                  }
            }

            GiveMutex(lfs->mtx);
      }

      lfs_write_end:
      return n;
}


//================================================================================================//
/**
 * @brief Function read from file data
 *
 * @param  dev          device number
 * @param  fd           file descriptor
 * @param *dst          data destination
 * @param  size         item size
 * @param  nitems       number of items
 * @param  seek         position in file
 *
 * @return number of read items
 */
//================================================================================================//
size_t lfs_read(devx_t dev, u32_t fd, void *dst, size_t size, size_t nitems, size_t seek)
{
      (void)dev;

      size_t n = 0;

      if (dst && size && nitems && lfs) {
            node_t *node = NULL;

            while (TakeMutex(lfs->mtx, MTX_BLOCK_TIME) != OS_OK);

            fopenInfo_t *foi = ListGetItemDataByID(lfs->openFile, fd);

            if (foi)
                  node = foi->node;

            if (node) {
                  if (node->type == NODE_TYPE_DRV && node->data) {
                        struct vfs_drvcfg *drv = node->data;

                        if (drv->f_read) {
                              GiveMutex(lfs->mtx);
                              n = drv->f_read(drv->dev, drv->part, dst, size, nitems, seek);
                              goto lfs_read_end;
                        }
                  } else if (node->type == NODE_TYPE_FILE) {
                        size_t filelen = node->size;
                        size_t items2rd;

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
            }

            GiveMutex(lfs->mtx);
      }

      lfs_read_end:
      return n;
}


//================================================================================================//
/**
 * @brief IO operations on files
 *
 * @param  dev    fs device number
 * @param  fd     file descriptor
 * @param  iorq   request
 * @param *data   data pointer
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//================================================================================================//
stdRet_t lfs_ioctl(devx_t dev, fd_t fd, IORq_t iorq, void *data)
{
      (void)dev;

      stdRet_t status = STD_RET_ERROR;
      node_t  *node   = NULL;

      if (lfs) {
            while (TakeMutex(lfs->mtx, MTX_BLOCK_TIME) != OS_OK);

            fopenInfo_t *foi = ListGetItemDataByID(lfs->openFile, fd);

            if (foi)
                  node = foi->node;

            if (node) {
                  if (node->type == NODE_TYPE_DRV && node->data) {
                        struct vfs_drvcfg *drv = node->data;

                        if (drv->f_ioctl) {
                              GiveMutex(lfs->mtx);
                              status = drv->f_ioctl(node->dev, node->part, iorq, data);
                              goto lfs_ioctl_end;
                        }
                  }
            }

            GiveMutex(lfs->mtx);
      }

      lfs_ioctl_end:
      return status;
}


//================================================================================================//
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
//================================================================================================//
static stdRet_t rmNode(node_t *base, node_t *target, u32_t baseitemid)
{
      stdRet_t status = STD_RET_ERROR;

      /* if DIR check if is empty */
      if (target->type == NODE_TYPE_DIR) {

            if (ListGetItemCount(target->data) > 0) {
                  goto rmNode_end;
            } else {
                  ListDestroy(target->data);
                  target->data = NULL;
            }
      }

      if (target->name)
            free(target->name);

      if (target->data)
            free(target->data);

      if (ListRmItemByID(base->data, baseitemid) == 0)
            status = STD_RET_OK;

      rmNode_end:
      return status;
}


//================================================================================================//
/**
 * @brief Check path deep
 *
 * @param *path         path
 *
 * @return path deep
 */
//================================================================================================//
static i32_t GetPathDeep(const ch_t *path)
{
      u32_t deep = 0;
      const ch_t *lastpath = NULL;

      if (path[0] == '/') {
            lastpath = path++;

            while ((path = strchr(path, '/'))) {
                  lastpath = path;
                  path++;
                  deep++;
            }

            if (lastpath[1] != '\0')
                  deep++;
      }

      return deep;
}


//================================================================================================//
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
//================================================================================================//
static node_t *GetNode(const ch_t *path, node_t *startnode, i32_t deep, i32_t *item)
{
      node_t *curnode = NULL;

      if (path && startnode) {
            if (startnode->type == NODE_TYPE_DIR) {
                  curnode       = startnode;
                  i32_t dirdeep = GetPathDeep(path);
                  u16_t pathlen;
                  ch_t *pathend;

                  /* go to selected node -------------------------------------------------------- */
                  while (dirdeep + deep > 0) {
                        /* get element from path */
                        if ((path = strchr(path, '/')) == NULL)
                              break;
                        else
                              path++;

                        if ((pathend = strchr(path, '/')) == NULL)
                              pathlen = strlen(path);
                        else
                              pathlen = pathend - path;

                        /* get number of list items */
                        i32_t listsize = ListGetItemCount(curnode->data);

                        /* find that object exist ----------------------------------------------- */
                        i32_t i = 0;
                        while (listsize > 0) {
                              node_t *node = ListGetItemDataByNo(curnode->data, i++);

                              if (node) {
                                    if (  strlen(node->name) == pathlen
                                       && strncmp(node->name, path, pathlen) == 0) {

                                          curnode = node;

                                          if (item)
                                                *item = i - 1;

                                          break;
                                    }
                              } else {
                                    dirdeep = 1 - deep;
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
            }
      }

      return curnode;
}


#ifdef __cplusplus
}
#endif

/*==================================================================================================
                                            End of file
==================================================================================================*/
