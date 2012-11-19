/*=============================================================================================*//**
@file    vfs.c

@author  Daniel Zorychta

@brief   This file support virtual file system

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
#include "vfs.h"
#include "system.h"
#include <string.h>


/*==================================================================================================
                                  Local symbolic constants/macros
==================================================================================================*/


/*==================================================================================================
                                   Local types, enums definitions
==================================================================================================*/
typedef enum {
      NODE_TYPE_UNKNOWN,
      NODE_TYPE_DIR,
      NODE_TYPE_FILE,
      NODE_TYPE_FS,
      NODE_TYPE_DRV,
} nodeType_t;

struct node {
      ch_t       *name;
      nodeType_t  type;
      u32_t       size;
      void       *data;
};

typedef struct node node_t;

struct fshdl_s {
      node_t  root;
      mutex_t mtx;
};


/*==================================================================================================
                                      Local function prototypes
==================================================================================================*/
static ch_t     *GetWordFromPath(ch_t *str, ch_t **word, size_t *length);
static node_t   *GetNode(const ch_t *path, node_t *startnode, const ch_t **extPath, i32_t deep, i32_t *nitem);
static i32_t     GetPathDeep(const ch_t *path);
static dirent_t  readdir(DIR_t *dir);


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
stdRet_t vfs_init(void)
{
      stdRet_t ret = STD_RET_OK;

      if (fs == NULL) {
            fs = calloc(1, sizeof(struct fshdl_s));

            if (fs) {
                  CreateMutex(fs->mtx);
                  fs->root.data = ListCreate();

                  if (!fs->mtx || !fs->root.data) {
                        if (fs->mtx)
                              DeleteMutex(fs->mtx);

                        if (fs->root.data)
                              ListDestroy(fs->root.data);

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
 * @brief Function mount file system in VFS
 *
 * @param *path               path when dir shall be mounted
 * @param *mountcfg           pointer to description of mount FS
 *
 * @retval STD_RET_OK         mount success
 * @retval STD_RET_ERROR      mount error
 */
//================================================================================================//
stdRet_t vfs_mount(const ch_t *path, vfsmcfg_t *mountcfg)
{
      stdRet_t status = STD_RET_ERROR;

      if (path && mountcfg && fs) {
            /* try parse folder name to create */
            i32_t deep = GetPathDeep(path);

            if (deep) {
                  /* go to target dir */
                  node_t *node = GetNode(path, &fs->root, NULL, -1, NULL);

                  /* check if target node is OK */
                  if (node) {
                        if (node->type == NODE_TYPE_DIR) {
                              ch_t  *dirname    = strrchr(path, '/') + 1;
                              u32_t  dirnamelen = strlen(dirname);
                              ch_t  *name       = calloc(dirnamelen + 1, sizeof(ch_t));

                              if (name) {
                                    strcpy(name, dirname);

                                    node_t    *fsdir = calloc(1, sizeof(node_t));
                                    vfsmcfg_t *mcfg  = calloc(1, sizeof(vfsmcfg_t));

                                    if (fsdir && mcfg) {
                                          memcpy(mcfg, mountcfg, sizeof(vfsmcfg_t));

                                          fsdir->name = name;
                                          fsdir->size = sizeof(node_t) + strlen(name) + sizeof(vfsmcfg_t);
                                          fsdir->type = NODE_TYPE_FS;
                                          fsdir->data = mcfg;

                                          /* add new fsdir to this folder */
                                          if (ListAddItem(node->data, fsdir) >= 0) {
                                                status = STD_RET_OK;
                                          }
                                    }

                                    /* free memory when error */
                                    if (status == STD_RET_ERROR) {
                                          if (fsdir)
                                                free(fsdir);

                                          if (mcfg)
                                                free(mcfg);

                                          free(name);
                                    }
                              }
                        }
                  }
            }
      }

      return status;
}


//================================================================================================//
/**
 * @brief Function umount dir from file system
 *
 * @param *path               dir path
 *
 * @retval STD_RET_OK         mount success
 * @retval STD_RET_ERROR      mount error
 */
//================================================================================================//
stdRet_t vfs_umount(const ch_t *path)
{
      stdRet_t status = STD_RET_ERROR;

      if (path && fs) {
            /* try parse folder name to create */
            i32_t deep = GetPathDeep(path);

            if (deep) {
                  /* go to FS dir */
                  i32_t  item;
                  node_t *nodebase = GetNode(path, &fs->root, NULL, -1, NULL);
                  node_t *nodefs   = GetNode(strrchr(path, '/'), nodebase, NULL, 0, &item);

                  /* check if target node is OK */
                  if (nodebase && nodefs) {
                        if (nodefs->type == NODE_TYPE_FS) {
                              if (nodefs->data)
                                    free(nodefs->data);

                              if (nodefs->name)
                                    free(nodefs->name);

                              /* remove from file system and free node */
                              ListRmItem(nodebase->data, item);

                              status = STD_RET_OK;
                        }
                  }
            }
      }

      return status;
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
stdRet_t vfs_mknod(const ch_t *path, vfsdcfg_t *drvcfg)
{
      stdRet_t status = STD_RET_ERROR;

      if (path && drvcfg && fs) {
            /* try parse folder name to create */
            i32_t deep = GetPathDeep(path);

            if (deep) {
                  /* go to target dir */
                  node_t *node = GetNode(path, &fs->root, NULL, -1, NULL);

                  /* check if target node is OK */
                  if (node) {
                        if (node->type == NODE_TYPE_DIR) {
                              ch_t  *drvname    = strrchr(path, '/') + 1;
                              u32_t  drvnamelen = strlen(drvname);
                              ch_t  *filename   = calloc(drvnamelen + 1, sizeof(ch_t));

                              if (filename) {
                                    strcpy(filename, drvname);

                                    node_t    *dirfile = calloc(1, sizeof(node_t));
                                    vfsdcfg_t *dcfg    = calloc(1, sizeof(vfsdcfg_t));

                                    if (dirfile && dcfg) {
                                          memcpy(dcfg, drvcfg, sizeof(vfsdcfg_t));

                                          dirfile->name = filename;
                                          dirfile->size = sizeof(node_t) + strlen(filename) + sizeof(vfsdcfg_t);
                                          dirfile->type = NODE_TYPE_DRV;
                                          dirfile->data = dcfg;

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
stdRet_t vfs_mkdir(const ch_t *path) /* DNLFIXME: mkdir must check if dir exist */
{
      stdRet_t status = STD_RET_ERROR;

      if (path && fs) {
            /* try parse folder name to create */
            i32_t deep = GetPathDeep(path);

            if (deep) {
                  /* go to target dir */
                  const ch_t *extPath = NULL;
                  node_t     *node    = GetNode(path, &fs->root, &extPath, -1, NULL);

                  /* check if target node is OK */
                  if (node) {
                        switch (node->type) {
                        /* internal FS */
                        case NODE_TYPE_DIR:
                        {
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
                              break;
                        }

                        /* external FS */
                        case NODE_TYPE_FS:
                        {
                              if (node->data) {
                                    vfsmcfg_t *extfs = node->data;

                                    if (node->data) {
                                          if (extfs->mkdir(extPath) == 0)
                                                status = STD_RET_OK;
                                    }
                              }
                              break;
                        }

                        default:
                              break;
                        }
                  }
            }
      }

      return status;
}


//================================================================================================//
/**
 * @brief Function open directory
 *
 * @param *path         directory path
 *
 * @return directory object
 */
//================================================================================================//
DIR_t *vfs_opendir(const ch_t *path)
{
      DIR_t *dir = NULL;

      if (path && fs) {
            dir = calloc(1, sizeof(DIR_t));

            if (dir) {
                  /* go to target dir */
                  const ch_t *extPath = NULL;
                  node_t     *node    = GetNode(path, &fs->root, &extPath, 0, NULL);

                  if (node) {
                        switch (node->type) {
                        case NODE_TYPE_DIR:
                              dir->items = ListGetItemCount(node->data);
                              dir->rddir = readdir;
                              dir->seek  = 0;
                              dir->dd    = node;
                              break;

                        case NODE_TYPE_FS:
                              if (node->data) {
                                    /*
                                     * freeing DIR object because external FS create
                                     * own object internally
                                     */
                                    free(dir);
                                    dir = NULL;

                                    if (extPath == NULL) {
                                          extPath = "/";
                                    }

                                    /* open external DIR */
                                    vfsmcfg_t *extfs = node->data;

                                    if (extfs->opendir)
                                          dir = extfs->opendir(extPath);
                              }
                              break;

                        /* Probably FILE */
                        default:
                              free(dir);
                              dir = NULL;
                              break;
                        }
                  } else {
                        free(dir);
                        dir = NULL;
                  }
            }
      }

      return dir;
}


//================================================================================================//
/**
 * @brief Function close opened directory
 *
 * @param *dir          directory object
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//================================================================================================//
stdRet_t vfs_closedir(DIR_t *dir)
{
      stdRet_t status = STD_RET_ERROR;

      if (dir)
      {
            free(dir);
            status = STD_RET_OK;
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
dirent_t vfs_readdir(DIR_t *dir)
{
      dirent_t direntry;
      direntry.name = NULL;
      direntry.size = 0;

      if (dir->rddir) {
            direntry = dir->rddir(dir);
      }

      return direntry;
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
stdRet_t vfs_remove(const ch_t *path)
{
      stdRet_t status = STD_RET_ERROR;

      if (path) {
            /* go to dir */
            i32_t       item;
            const ch_t *extPath  = NULL;
            node_t     *nodebase = GetNode(path, &fs->root, &extPath, -1, NULL);
            node_t     *nodeobj  = GetNode(strrchr(path, '/'), nodebase, NULL, 0, &item);

            /* check if target nodes ares OK */
            if (nodebase) {

                  if (nodeobj) {

                        /* node must be local FILE or DIR */
                        if (  nodeobj->type == NODE_TYPE_DIR
                           || nodeobj->type == NODE_TYPE_FILE
                           || nodeobj->type == NODE_TYPE_DRV) {

                              /* if DIR check if is empty */
                              if (nodeobj->type == NODE_TYPE_DIR) {

                                    if (ListGetItemCount(nodeobj->data) != 0) {
                                          goto vfs_remove_end;
                                    } else {
                                          ListDestroy(nodeobj->data);
                                          nodeobj->data = NULL;
                                    }
                              }

                              if (nodeobj->name)
                                    free(nodeobj->name);

                              if (nodeobj->data)
                                    free(nodeobj->data);

                              if (ListRmItem(nodebase->data, item) == 0)
                                    status = STD_RET_OK;
                        }
                  } else if (nodebase->type == NODE_TYPE_FS) {

                        vfsmcfg_t *ext = nodebase->data;

                        if (ext->remove) {
                              if (ext->remove(extPath) == 0) {
                                    status = STD_RET_OK;
                              }
                        }
                  }
            }
      }

      vfs_remove_end:
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
stdRet_t vfs_rename(const ch_t *oldName, const ch_t *newName)
{
      stdRet_t status = STD_RET_ERROR;

      if (oldName && newName) {
            const ch_t *oldExtPath;
            const ch_t *newExtPath;
            node_t     *oldNodeBase = GetNode(oldName, &fs->root, &oldExtPath, -1, NULL);
            node_t     *newNodeBase = GetNode(newName, &fs->root, &newExtPath, -1, NULL);

            if (oldNodeBase && newNodeBase) {
                  /* rename in the same directory */
                  if (oldNodeBase == newNodeBase) {
                        /* rename in VFS structure */
                        if (oldNodeBase->type == NODE_TYPE_DIR) {
                              ch_t   *name = calloc(1, strlen(strrchr(newName, '/') + 1));
                              node_t *node = GetNode(strrchr(oldName, '/'), newNodeBase, NULL, 0, NULL);

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
                        /* rename in external FS */
                        } else if (oldNodeBase->type == NODE_TYPE_FS) {
                              vfsmcfg_t *ext = oldNodeBase->data;

                              if (ext) {
                                    if (ext->rename) {
                                          status = ext->rename(oldExtPath, newExtPath);
                                    }
                              }
                        }
                  /* rename in different directory -- move operation */
                  } else if (oldNodeBase->type == NODE_TYPE_DIR && newNodeBase->type == NODE_TYPE_DIR) {

                  }
            }
      }

      return status;
}











//================================================================================================//
/**
 * @brief Function open selected file
 *
 * @param *name         file path
 * @param *mode         file mode
 *
 * @retval NULL if file can't be created
 */
//================================================================================================//
FILE_t *vfs_fopen(const ch_t *name, const ch_t *mode)
{
      FILE_t *file = NULL;

      if (name && mode) {
            file = calloc(1, sizeof(FILE_t));

            if (file) {
                  /* root filter */
                  if (name[0] == '/') {
                        struct node *node   = NULL;
//                        list_t *root = fs->root;
                        ch_t   *path = (ch_t *)name;
                        ch_t   *word = NULL;
                        size_t  len;

                        /* go to file/dir/fs specified in path */
                        while (path) {
                              path = GetWordFromPath(path, &word, &len);

//                              root = GetNodeFromList(root, word, len, &node);

                              if (node == NULL) {
                                    path = 0;
                              }
                        }

                        /* check file type */
                        if (path == NULL) {
                              switch (node->type) {
                              case NODE_TYPE_DIR:
                                    /* DNLTODO */
                                    break;

                              case NODE_TYPE_FILE:
                                    /* DNLTODO */
                                    break;

                              case NODE_TYPE_FS:
                                    /* DNLTODO */
                                    break;

                              default:
                                    break;
                              }
                        }
                  }




//                  /* check if path is device */
//                  stdRet_t stat = STD_RET_ERROR;
//
//                  ch_t *filename = strchr(name + 1, '/');
//
//                  if (filename)
//                  {
//                        filename++;
//
//                        if (strncmp("/bin/", name, filename - name) == 0)
//                        {
//                                    /* nothing to do */
//                        }
//                        else if (strncmp("/dev/", name, filename - name) == 0)
//                        {
//                              regDrvData_t drvdata;
//
//                              if (GetDrvData(filename, &drvdata) == STD_RET_OK)
//                              {
//                                    if (drvdata.open(drvdata.device) == STD_RET_OK)
//                                    {
//                                          file->fdclose = drvdata.close;
//                                          file->fd      = drvdata.device;
//                                          file->fdioctl = drvdata.ioctl;
//                                          file->fdread  = drvdata.read;
//                                          file->fdwrite = drvdata.write;
//
//                                          stat = STD_RET_OK;
//                                    }
//                              }
//                        }
////                        else if (strncmp("/proc/", name, filename - name) == 0)
////                        {
////                              if ((file->fd = PROC_open(filename, (ch_t*)mode)) != 0)
////                              {
////                                    file->fdclose = PROC_close;
////                                    file->fdioctl = NULL;
////                                    file->fdread  = PROC_read;
////                                    file->fdwrite = PROC_write;
////
////                                    stat = STD_RET_OK;
////                              }
////                        }
//
//                        /* file does not exist */
//                        if (stat != STD_RET_OK)
//                        {
//                              free(file);
//                              file = NULL;
//                        }
//                        else
//                        {
//                              file->mode = (ch_t*)mode;
//
//                              /* open for reading */
//                              if (strncmp("r", mode, 2) == 0)
//                              {
//                                    file->fdwrite = NULL;
//                              }
//                              /* open for writing (file need not exist) */
//                              else if (strncmp("w", mode, 2) == 0)
//                              {
//                                    file->fdread = NULL;
//                              }
//                              /* open for appending (file need not exist) */
//                              else if (strncmp("a", mode, 2) == 0)
//                              {
//                                    file->fdread = NULL;
//                              }
//                              /* open for reading and writing, start at beginning */
//                              else if (strncmp("r+", mode, 2) == 0)
//                              {
//                                    /* nothing to change */
//                              }
//                              /* open for reading and writing (overwrite file) */
//                              else if (strncmp("w+", mode, 2) == 0)
//                              {
//                                    /* nothing to change */
//                              }
//                              /* open for reading and writing (append if file exists) */
//                              else if (strncmp("a+", mode, 2) == 0)
//                              {
//                                    /* nothing to change */
//                              }
//                              /* invalid mode */
//                              else
//                              {
//                                    fclose(file);
//                                    file = NULL;
//                              }
//                        }
//                  }
//                  else
//                  {
//                        free(file);
//                        file = NULL;
//                  }
            }
      }

      return file;
}


//================================================================================================//
/**
 * @brief Function close opened file
 *
 * @param *file               pinter to file
 *
 * @retval STD_RET_OK         file closed successfully
 * @retval STD_RET_ERROR      file not closed
 */
//================================================================================================//
stdRet_t vfs_fclose(FILE_t *file)
{
      stdRet_t status = STD_RET_ERROR;

      if (file)
      {
            if (file->fdclose(file->fd) == STD_RET_OK)
            {
                  free(file);
                  status = STD_RET_OK;
            }
      }

      return status;
}


//================================================================================================//
/**
 * @brief Function write data to file
 *
 * @param *ptr                address to data (src)
 * @param size                item size
 * @param nitems              number of items
 * @param *file               pointer to file object
 *
 * @return STD_RET_OK or 0 if write finished successfully, otherwise > 0
 */
//================================================================================================//
size_t vfs_fwrite(void *ptr, size_t size, size_t nitems, FILE_t *file)
{
      size_t n = 0;

      if (ptr && size && nitems && file)
      {
            if (file->fdwrite)
            {
                  n = file->fdwrite(file->fd, ptr, size, nitems, file->seek);
                  file->seek += n * size;
            }
      }

      return n;
}


//================================================================================================//
/**
 * @brief Function read data from file
 *
 * @param *ptr                address to data (dst)
 * @param size                item size
 * @param nitems              number of items
 * @param *file               pointer to file object
 *
 * @return number of read items
 */
//================================================================================================//
size_t vfs_fread(void *ptr, size_t size, size_t nitems, FILE_t *file)
{
      size_t n = 0;

      if (ptr && size && nitems && file)
      {
            if (file->fdread)
            {
                  n = file->fdread(file->fd, ptr, size, nitems, file->seek);
                  file->seek += n * size;
            }
      }

      return n;
}


//================================================================================================//
/**
 * @brief Function set seek value
 *
 * @param *file               file object
 * @param offset              seek value
 * @param mode                seek mode DNLFIXME implement: seek mode
 *
 * @retval STD_RET_OK         seek moved successfully
 * @retval STD_RET_ERROR      error occured
 */
//================================================================================================//
stdRet_t vfs_fseek(FILE_t *file, i32_t offset, i32_t mode)
{
      (void)mode;

      stdRet_t status = STD_RET_ERROR;

      if (file)
      {
            file->seek = offset;
            status     = STD_RET_OK;
      }

      return status;
}


//================================================================================================//
/**
 * @brief Function support not standard operations
 *
 * @param *file               file
 * @param rq                  request
 * @param *data               pointer to datas
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//================================================================================================//
stdRet_t vfs_ioctl(FILE_t *file, IORq_t rq, void *data)
{
      stdRet_t status = STD_RET_ERROR;

      if (file && file->fdioctl)
      {
            status = file->fdioctl(file->fd, rq, data);
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
 * @param[out] *nitem         node is n-item of list which was found
 *
 * @return node
 */
//================================================================================================//
static node_t *GetNode(const ch_t *path, node_t *startnode, const ch_t **extPath, i32_t deep, i32_t *nitem)
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
                              node_t *node = ListGetItemData(curnode->data, i++);

                              if (node) {
                                    if (strncmp(node->name, word, len) == 0) {
                                          curnode = node;

                                          if (nitem)
                                                *nitem = i - 1;
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

                        /* if external system, exit */
                        if (curnode->type == NODE_TYPE_FS) {
                              if (extPath)
                                    *extPath = path;

                              break;
                        }

                        dirdeep--;
                  }
            }
      }

      return curnode;
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
static dirent_t readdir(DIR_t *dir)
{
      dirent_t dirent;
      dirent.isfile = TRUE;
      dirent.name   = NULL;
      dirent.size   = 0;

      if (dir) {
            node_t *from = dir->dd;
            node_t *node = ListGetItemData(from->data, dir->seek++);

            if (node) {
                  dirent.isfile = (node->type == NODE_TYPE_FILE || node->type == NODE_TYPE_DRV);
                  dirent.name   = node->name;
                  dirent.size   = node->size;
            }
      }

      return dirent;
}

#ifdef __cplusplus
}
#endif

/*==================================================================================================
                                            End of file
==================================================================================================*/
