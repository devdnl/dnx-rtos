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
} nodeType_t;

struct node {
      ch_t       *name;
      nodeType_t  type;
      u32_t       size;
      void       *data;
};

typedef struct node node_t;

struct fshdl_s {
      list_t  *root;
      mutex_t mtx;
};


/*==================================================================================================
                                      Local function prototypes
==================================================================================================*/
static ch_t   *GetWordFromPath(ch_t *str, ch_t **word, size_t *length);
static list_t *GetNodeFromList(list_t *list, ch_t *dirname, size_t len, struct node **node);
static list_t *GotoDir(ch_t *path);
static i32_t   GetPathDeep(const ch_t *path);
static i32_t   CreateDirNode(list_t *list, const ch_t *name);


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
                  fs->root = ListCreate();

                  if (!fs->mtx || !fs->root) {
                        if (fs->mtx)
                              DeleteMutex(fs->mtx);

                        if (fs->root)
                              ListFree(fs->root);

                        free(fs);

                        fs = NULL;
                  }
            }

            if (fs == NULL)
                  ret = STD_RET_ERROR;
      }

      return ret;
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
stdRet_t vfs_mkdir(const ch_t *path)
{
      stdRet_t status = STD_RET_ERROR;

      if (path && fs) {
            list_t *root = fs->root;
            i32_t   deep = GetPathDeep(path);
            ch_t   *word;
            node_t *node;
            size_t  len;

            while (deep > 1) {
                  path = GetWordFromPath((ch_t*)path, &word, &len);
                  root = GetNodeFromList(root, word, len, &node);

                  if (node->type == NODE_TYPE_FS) {
                        break;
                  }
                  deep--;
            }

            if (deep == 1 && root) {
                  switch (node->type) {
                  case NODE_TYPE_DIR:
                        GetWordFromPath((ch_t*)path, &word, &len);

                        ch_t *name = calloc(1, len + 1);

                        if (name) {
                              strncpy(name, word, len);

                              if (CreateDirNode(root, name) != 0){
                                    free(name);
                              } else {
                                    status = STD_RET_OK;
                              }
                        }
                        break;

                  case NODE_TYPE_FS:
                        if (node->data) {
                              vfsnode_t *extfs = node->data;

                              status = extfs->mkdir(path);
                        }
                        break;

                  default:
                        break;
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
                        list_t *root = fs->root;
                        ch_t   *path = (ch_t *)name;
                        ch_t   *word = NULL;
                        size_t  len;

                        /* go to file/dir/fs specified in path */
                        while (path) {
                              path = GetWordFromPath(path, &word, &len);

                              root = GetNodeFromList(root, word, len, &node);

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
 * @brief Function mount file system in VFS
 *
 * @param node                registered node
 * @param *dir                folder name
 *
 * @retval STD_RET_OK         mount success
 * @retval STD_RET_ERROR      mount error
 */
//================================================================================================//
stdRet_t vfs_mount(struct vfsnode node, const ch_t *dir)
{

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
 * @retval STD_RET_OK         success
 * @retval STD_RET_XX         error
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
 * @brief Function open root directory
 *
 * @param *dir          directory
 *
 * @return number of items
 */
//================================================================================================//
static void ROOT_opendir(DIR_t *dir) /* DNLFIXME apply better solution (table) */
{
//      dir->rddir = ROOT_readdir;
//      dir->seek  = 0;
//      dir->items = 3;
}


//================================================================================================//
/**
 * @brief Function read selected item
 *
 * @param seek          nitem
 * @return file attributes
 */
//================================================================================================//
static dirent_t ROOT_readdir(size_t seek)
{
      dirent_t direntry;
      direntry.rm     = NULL;
      direntry.name   = NULL;
      direntry.size   = 0;
      direntry.isfile = FALSE;
      direntry.fd     = 0;

      if (seek < 3)
      {
            switch (seek) /* DNLFIXME apply better solution (table) */
            {
                  case 0: direntry.name = "bin"; break;
                  case 1: direntry.name = "dev"; break;
                  case 2: direntry.name = "proc"; break;
                  default: break;
            }

            direntry.size = 0;
      }

      return direntry;
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

      if (path)
      {
            dir = calloc(1 , sizeof(DIR_t));

            if (dir)
            {
                  /* check if path is device */
                  stdRet_t stat = STD_RET_OK;

                  if (strcmp("/", path) == 0)
                  {
//                        ROOT_opendir(dir);
                  }
                  else if (strcmp("/bin", path) == 0)
                  {
//                        REGAPP_opendir(dir);
                  }
                  else if (strcmp("/dev", path) == 0)
                  {
//                        REGDRV_opendir(dir);
                  }
//                  else if (strcmp("/proc", path) == 0)
//                  {
//                        PROC_opendir(dir);
//                  }
                  else
                  {
                        stat = STD_RET_ERROR;
                  }

                  /* file does not exist */
                  if (stat != STD_RET_OK)
                  {
                        free(dir);
                        dir = NULL;
                  }
            }
      }

      return dir;
}


//================================================================================================//
/**
 * @brief Function read next item of opened directory
 *
 * @param *dir          directory object
 * @return element attributes
 */
//================================================================================================//
dirent_t vfs_readdir(DIR_t *dir)
{
      dirent_t direntry;
      direntry.name = NULL;
      direntry.size = 0;
      direntry.fd   = 0;

      if (dir->rddir)
      {
            direntry = dir->rddir(dir->seek);
            dir->seek++;
      }

      return direntry;
}


//================================================================================================//
/**
 * @brief Function close opened directory
 *
 * @param *dir          directory object
 *
 * @retval STD_RET_OK         close success
 * @retval STD_RET_ERROR      close error
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
 * @brief Remove file
 *
 * @param *patch        localization of file
 *
 * @return STD_RET_OK if success, otherwise STD_RET_ERROR
 */
//================================================================================================//
size_t vfs_remove(const ch_t *path)
{
      stdRet_t status = STD_RET_ERROR;

      /* DNLTODO implement vfs_remove */
//      if (direntry->rm)
//      {
//            status = direntry->rm(direntry->fd);
//      }

      return status;
}


//================================================================================================//
/**
 * @brief Rename file name
 *
 * @param *oldName            old file name
 * @param *newName            new file name
 *
 * @return 0 if success, otherwise != 0
 */
//================================================================================================//
size_t vfs_rename(const ch_t *oldName, const ch_t *newName)
{

}


//================================================================================================//
/**
 * @brief Create directory node
 */
//================================================================================================//
static i32_t CreateDirNode(list_t *list, const ch_t *name)
{
      i32_t n = 1;

      node_t *dir = calloc(1, sizeof(node_t));

      if (dir && list) {
            dir->data = ListCreate();

            if (dir->data) {
                  dir->name = name;
                  dir->size = sizeof(node_t) + strlen(name);
                  dir->type = NODE_TYPE_DIR;

                  if (ListAddItem(list, dir) < 0) {
                        ListFree(dir->data);
                        free(dir);
                        dir = NULL;
                  } else {
                        n = 0;
                  }
            }
      }

      return n;
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
 * @brief Function find node
 *
 * @param[in]  *list          input dir list
 * @param[in]  *dirname       finding folder name
 * @param[in]  len            dir name length
 * @param[out] **node         found node
 */
//================================================================================================//
static list_t *GetNodeFromList(list_t *list, ch_t *dirname, size_t len, struct node **node)
{
      i32_t   listsize = 0;
      list_t *nextlist = NULL;
      node_t *dir      = NULL;

      if (list && dirname && node) {
            listsize = ListGetItemCount(list);
            i32_t i  = 0;

            while (listsize-- > 0) {
                  dir = ListGetItemData(list, i++);

                  if (strncmp(dir->name, dirname, len) == 0) {
                        if (dir->type == NODE_TYPE_DIR)
                              nextlist = dir->data;

                        break;
                  }
            }

            *node = dir;
      }

      return nextlist;
}


#ifdef __cplusplus
}
#endif

/*==================================================================================================
                                            End of file
==================================================================================================*/
