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
#include "system.h"
#include <string.h>


/*==================================================================================================
                                  Local symbolic constants/macros
==================================================================================================*/
/* delete definition of illegal macros in this file */
#undef ioctl
#undef opendir
#undef remove
#undef rename

/* wait time for operation on VFS */
#define MTX_BLOCK_TIME        100


/*==================================================================================================
                                   Local types, enums definitions
==================================================================================================*/
typedef enum {
      NODE_TYPE_UNKNOWN,
      NODE_TYPE_DIR,
      NODE_TYPE_FILE,
      NODE_TYPE_DRV,
} nodeType_t;

struct node {
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
};

typedef struct node node_t;

struct fshdl_s {
      node_t  root;
      mutex_t mtx;
      list_t  *openFile;
};


/*==================================================================================================
                                      Local function prototypes
==================================================================================================*/
static ch_t     *GetWordFromPath(ch_t *str, ch_t **word, size_t *length);
static node_t   *GetNode(const ch_t *path, node_t *startnode, i32_t deep, i32_t *item);
static i32_t     GetPathDeep(const ch_t *path);
static dirent_t  lfs_readdir(DIR_t *dir);

/*==================================================================================================
                                      Local object definitions
==================================================================================================*/
static struct fshdl_s *fs;


/*==================================================================================================
                                        Function definitions
==================================================================================================*/

//================================================================================================//
/**
 * @brief Initialize VFS module
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//================================================================================================//
stdRet_t lfs_init(u32_t dev)
{
      (void)dev;

      stdRet_t ret = STD_RET_OK;

      if (fs == NULL) {
            fs = calloc(1, sizeof(struct fshdl_s));

            if (fs) {
                  CreateMutex(fs->mtx);
                  fs->root.data = ListCreate();
                  fs->openFile  = ListCreate();

                  if (!fs->mtx || !fs->root.data || !fs->openFile) {
                        if (fs->mtx)
                              DeleteMutex(fs->mtx);

                        if (fs->root.data)
                              ListDestroy(fs->root.data);

                        if (fs->openFile)
                              ListDestroy(fs->openFile);

                        free(fs);

                        fs = NULL;
                  } else {
                        fs->root.name = "/";
                        fs->root.size = sizeof(node_t);
                        fs->root.type = NODE_TYPE_DIR;
                  }
            }

            if (fs == NULL)
                  ret = STD_RET_ERROR;
      }

      return ret;
}


//================================================================================================//
/**
 * @brief Function create node for driver file
 *
 * @param *path               path when driver-file shall be created
 * @param *drvcfg             pointer to description of driver
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//================================================================================================//
stdRet_t lfs_mknod(u32_t dev, const ch_t *path, struct vfs_drvcfg *drvcfg)
{
      (void)dev;

      stdRet_t status = STD_RET_ERROR;

      if (TakeMutex(fs->mtx, MTX_BLOCK_TIME)) {
            if (path && drvcfg && fs) {
                  /* try parse folder name to create */
                  i32_t deep = GetPathDeep(path);

                  if (deep) {
                        /* go to target dir */
                        node_t *node   = GetNode(path, &fs->root, -1, NULL);
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
                                          struct vfs_drvcfg *dcfg    = calloc(1, sizeof(struct vfs_drvcfg));

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
                  }
            }

            GiveMutex(fs->mtx);
      }

      return status;
}


//================================================================================================//
/**
 * @brief Create directory
 *
 * @param *path   path to new directory
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//================================================================================================//
stdRet_t lfs_mkdir(u32_t dev, const ch_t *path)
{
      (void)dev;

      stdRet_t status = STD_RET_ERROR;

      if (TakeMutex(fs->mtx, MTX_BLOCK_TIME)) {
            if (path && fs) {
                  /* try parse folder name to create */
                  i32_t deep = GetPathDeep(path);

                  if (deep) {
                        /* go to target dir */
                        node_t *node   = GetNode(path, &fs->root, -1, NULL);
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
                  }
            }

            GiveMutex(fs->mtx);
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
stdRet_t lfs_opendir(u32_t dev, const ch_t *path, DIR_t *dir)
{
      (void)dev;

      stdRet_t status = STD_RET_ERROR;

      if (TakeMutex(fs->mtx, MTX_BLOCK_TIME)) {
            if (path && fs) {
                  /* go to target dir */
                  node_t *node = GetNode(path, &fs->root, 0, NULL);

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
            }

            GiveMutex(fs->mtx);
      }

      return status;
}


//================================================================================================//
/**
 * @brief Remove file
 *
 * @param *patch        localization of file/directory
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//================================================================================================//
stdRet_t lfs_remove(u32_t dev, const ch_t *path)
{
      (void)dev;

      stdRet_t status = STD_RET_ERROR;

      if (TakeMutex(fs->mtx, MTX_BLOCK_TIME)) {
            if (path) {
                  /* go to dir */
                  i32_t   item;
                  node_t *nodebase = GetNode(path, &fs->root, -1, NULL);
                  node_t *nodeobj  = GetNode(strrchr(path, '/'), nodebase, 0, &item);

                  /* check if target nodes ares OK */
                  if (nodebase) {

                        if (nodeobj && nodeobj != &fs->root) {

                              /* node must be local FILE or DIR */
                              if (  nodeobj->type == NODE_TYPE_DIR
                                 || nodeobj->type == NODE_TYPE_FILE
                                 || nodeobj->type == NODE_TYPE_DRV) {

                                    /* if DIR check if is empty */
                                    if (nodeobj->type == NODE_TYPE_DIR) {

                                          if (ListGetItemCount(nodeobj->data) > 0) {
                                                goto lfs_remove_end;
                                          } else {
                                                ListDestroy(nodeobj->data);
                                                nodeobj->data = NULL;
                                          }
                                    }

                                    if (nodeobj->name)
                                          free(nodeobj->name);

                                    if (nodeobj->data)
                                          free(nodeobj->data);

                                    u32_t itemid = ListGetItemID(nodebase->data, item);

                                    if (ListRmItemByID(nodebase->data, itemid) == 0)
                                          status = STD_RET_OK;
                              }
                        }
                  }
            }

            GiveMutex(fs->mtx);
      }

      lfs_remove_end:
      return status;
}


//================================================================================================//
/**
 * @brief Rename file name
 * The implementation of rename can move files only if external FS provide functionality. Local
 * VFS can not do this.
 *
 * @param *oldName            old file name
 * @param *newName            new file name
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//================================================================================================//
stdRet_t lfs_rename(u32_t dev, const ch_t *oldName, const ch_t *newName)
{
      (void)dev;

      stdRet_t status = STD_RET_ERROR;

      if (TakeMutex(fs->mtx, MTX_BLOCK_TIME)) {
            if (oldName && newName) {
                  node_t *oldNodeBase = GetNode(oldName, &fs->root, -1, NULL);
                  node_t *newNodeBase = GetNode(newName, &fs->root, -1, NULL);

                  if (oldNodeBase && newNodeBase && oldName[0] == '/' && newName[0] == '/') {
                        /* the same dir -- rename operation */
                        if (oldNodeBase == newNodeBase) {

                              ch_t   *name = calloc(1, strlen(strrchr(newName, '/') + 1));
                              node_t *node = GetNode(strrchr(oldName, '/'), newNodeBase, 0, NULL);

                              if (name && node) {
                                    strcpy(name, strrchr(newName, '/') + 1);

                                    if (node->name)
                                          free(node->name);

                                    node->name = name;

                                    status = STD_RET_OK;

                              } else {
                                    if (name)
                                          free(name);
                              }
                        /* different dir -- move operation */
                        } else {

                        }
                  }
            }

            GiveMutex(fs->mtx);
      }

      return status;
}


//================================================================================================//
/**
 * @brief Function change file mode
 *
 * @param dev     fs device
 * @param mode    file mode
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//================================================================================================//
stdRet_t lfs_chmod(u32_t dev, u32_t mode)
{
      (void)dev;
      (void)mode;

      return STD_RET_ERROR;
}


//================================================================================================//
/**
 * @brief Function change file owner and group
 *
 * @param dev     fs device
 * @param owner   file owner
 * @param group   file group
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//================================================================================================//
stdRet_t lfs_chown(u32_t dev, u16_t owner, u16_t group)
{
      (void)dev;
      (void)owner;
      (void)group;

      return STD_RET_ERROR;
}


//================================================================================================//
/**
 * @brief Function returns file/dir status
 *
 * @param *path         file/dir path
 * @param *stat         pointer to stat structure
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//================================================================================================//
stdRet_t lfs_stat(u32_t dev, const ch_t *path, struct vfs_stat *stat)
{
      (void)dev;

      stdRet_t status = STD_RET_ERROR;

      if (TakeMutex(fs->mtx, MTX_BLOCK_TIME)) {
            if (path && stat) {
                  node_t *node = GetNode(path, &fs->root, 0, NULL);

                  if (node) {
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

            GiveMutex(fs->mtx);
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
stdRet_t lfs_statfs(u32_t dev, struct vfs_statfs *statfs)
{
      (void)dev;
      (void)statfs;

      return STD_RET_ERROR;
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
stdRet_t lfs_release(u32_t dev)
{
      (void)dev;

      return STD_RET_ERROR;
}


//================================================================================================//
/**
 * @brief Function open selected file
 *
 * @param *name         file path
 * @param *mode         file mode
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//================================================================================================//
stdRet_t lfs_open(u32_t dev, fd_t *fd, const ch_t *path, const ch_t *mode)
{
      (void)dev;

      stdRet_t status = STD_RET_ERROR;

      if (fd && path && mode) {
            if (TakeMutex(fs->mtx, MTX_BLOCK_TIME)) {
                  node_t *node = GetNode(path, &fs->root, 0, NULL);

                  /* file does not exist */
                  if (node == NULL) {
                        /* create file when mode is correct */
                        if (  (strncmp("w", mode, 2) == 0) || (strncmp("w+", mode, 2) == 0)
                           || (strncmp("a", mode, 2) == 0) || (strncmp("a+", mode, 2) == 0) ) {

                              /* file does not exist, try to create new file */
                              node_t *nodebase = GetNode(path, &fs->root, -1, NULL);

                              if (nodebase) {
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

                                                if (ListAddItem(nodebase->data, fnode) < 0) {
                                                      free(fnode);
                                                      free(filename);
                                                } else {
                                                      status = STD_RET_OK;
                                                }
                                          }
                                    }
                              }
                        }
                  }

                  if (status == STD_RET_OK) {
                        /* set default status */
                        status = STD_RET_ERROR;

                        /* file shall exist */
                        node = GetNode(path, &fs->root, 0, NULL);

                        if (node) {
                              struct vfs_drvcfg *drv = node->data;
                              i32_t item;

                              /* open file according to file type */
                              if (node->type == NODE_TYPE_FILE) {
                                    /* declare file as open in open list */
                                    item = ListAddItem(fs->openFile, node);

                                    if (item >= 0) {
                                          /* clears file when condition is fulfilled */
                                          if ( node->data && (  (strncmp("w",  mode, 2) == 0)
                                                             || (strncmp("w+", mode, 2) == 0) ) ) {
                                                free(node->data);
                                                node->data = NULL;
                                                node->size = 0;
                                          }

                                          *fd = ListGetItemID(fs->openFile, item);

                                          status = STD_RET_OK;
                                    }
                              } else if (node->type ==  NODE_TYPE_DRV) {
                                    if (drv->open) {
                                          if (drv->open(drv->dev, drv->part) == STD_RET_OK) {
                                                *fd    = drv->part;
                                                status = STD_RET_OK;
                                          }
                                    }
                              }

                              /* check file mode */
                              if (status == STD_RET_OK) {
                                    /* here shall be checked permissions */
                              }
                        }
                  }
            }

            GiveMutex(fs->mtx);
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
      dirent.isfile = TRUE;
      dirent.name   = NULL;
      dirent.size   = 0;

      if (dir) {
            node_t *from = dir->dd;
            node_t *node = ListGetItemDataByNo(from->data, dir->seek++);

            if (node) {
                  dirent.isfile = (node->type == NODE_TYPE_FILE || node->type == NODE_TYPE_DRV);
                  dirent.name   = node->name;
                  dirent.size   = node->size;
            }
      }

      return dirent;
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
stdRet_t lfs_close(u32_t dev, fd_t fd)
{
      (void)dev;

      stdRet_t status = STD_RET_ERROR;

      if (ListUnlinkItemDataByID(fs->openFile, fd) == 0) {
            if (ListRmItemByID(fs->openFile, fd) == 0) {
                  status = STD_RET_OK;
            }
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
size_t lfs_write(u32_t dev, fd_t fd, void *src, size_t size, size_t nitems, size_t seek)
{
      (void)dev;

      size_t n = 0;

      if (TakeMutex(fs->mtx, MTX_BLOCK_TIME)) {
            if (src && size && nitems) {
                  node_t *node = ListGetItemDataByID(fs->openFile, fd);

                  if (node) {
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

                                    memcpy(newdata + seek, src, wrsize);
                                    n = nitems;

                                    node->data  = newdata;
                                    node->size += wrsize - (filelen - seek);
                              }
                        } else {
                              memcpy(node->data + seek, src, wrsize);
                              n = nitems;
                        }
                  }
            }
      }

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
size_t lfs_read(u32_t dev, u32_t fd, void *dst, size_t size, size_t nitems, size_t seek)
{
      (void)dev;

      size_t n = 0;

      if (TakeMutex(fs->mtx, MTX_BLOCK_TIME)) {
            if (dst && size && nitems) {
                  node_t *node = ListGetItemDataByID(fs->openFile, fd);

                  if (node) {
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
      }

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
stdRet_t lfs_ioctl(u32_t dev, fd_t fd, IORq_t iroq, void *data)
{
      stdRet_t status = STD_RET_ERROR;

      node_t *node = ListGetItemDataByID(fs->openFile, fd);

      if (node) {
            if (node->type == NODE_TYPE_DRV && node->data) {
                  struct vfs_drvcfg *drv = node->data;

                  if (drv->ioctl) {
                        status = drv->ioctl(dev, fd, iroq, data);
                  }
            }
      }

      return status;
}


//================================================================================================//
/**
 * @brief Function return pointer to word
 *
 * @param[in]  *str          string
 * @param[out] **word        pointer to word beginning
 * @param[out] *length       pointer to word length
 *
 * @return pointer to next word, otherwise NULL
 */
//================================================================================================//
static ch_t *GetWordFromPath(ch_t *str, ch_t **word, size_t *length)
{
      ch_t *bwd = NULL;
      ch_t *ewd = NULL;
      ch_t *nwd = NULL;

      if (str) {
            bwd = strchr(str, '/');

            if (bwd) {
                  ewd = strchr(bwd + 1, '/');

                  if (ewd == NULL) {
                        ewd = strchr(bwd + 1, '\0');
                        nwd = NULL;
                  } else {
                        nwd = ewd;
                  }

                  bwd++;
            }
      }

      if (word)
            *word   = bwd;

      if (length)
            *length = ewd - bwd;

      return nwd;
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
                  curnode          = startnode;
                  i32_t   dirdeep  = GetPathDeep(path);
                  ch_t   *word;
                  size_t  len;
                  i32_t   listsize;

                  while (dirdeep + deep > 0) {
                        /* get word from path */
                        path = GetWordFromPath((ch_t*)path, &word, &len);

                        /* get number of list items */
                        listsize = ListGetItemCount(curnode->data);

                        /* find that object exist */
                        i32_t i = 0;
                        while (listsize > 0) {
                              node_t *node = ListGetItemDataByNo(curnode->data, i++);

                              if (node) {
                                    if (strncmp(node->name, word, len) == 0) {
                                          curnode = node;

                                          if (item)
                                                *item = i - 1;//ListGetItemID(curnode->data, i - 1);
                                          break;
                                    }
                              } else {
                                    dirdeep = 1 - deep;
                                    break;
                              }

                              listsize--;
                        }

                        /* dir does not found or error */
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
