/*=============================================================================================*//**
@file    proc.c

@author  Daniel Zorychta

@brief   This file support /proc files

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
#include "proc.h"
#include "system.h"
#include <string.h>


/*==================================================================================================
                                  Local symbolic constants/macros
==================================================================================================*/
#define BLOCK_TIME                  1000


/*==================================================================================================
                                   Local types, enums definitions
==================================================================================================*/
struct filenode
{
      procfd_t fd;
      ch_t     *name;
      ch_t     *data;
      size_t   size;
      struct filenode *next;
};

struct filehdl
{
      struct  filenode *file;
      size_t  fdcnt;
      mutex_t mtx;
};


/*==================================================================================================
                                      Local function prototypes
==================================================================================================*/


/*==================================================================================================
                                      Local object definitions
==================================================================================================*/
static struct filehdl *procmem;


/*==================================================================================================
                                        Function definitions
==================================================================================================*/

//================================================================================================//
/**
 * @brief Remove existing file
 *
 * @param *name   file name
 *
 * @retval STD_RET_OK         remove success
 * @retval STD_RET_ERROR      remove error
 */
//================================================================================================//
stdRet_t PROC_remove(u32_t fd)
{
      stdRet_t status = STD_RET_ERROR;

      if (fd)
      {
            /* if memory exist */
            if (procmem)
            {
                  if (TakeMutex(procmem->mtx, BLOCK_TIME) == OS_OK)
                  {
                        /* try find file */
                        struct filenode *fileptr    = procmem->file;
                        struct filenode *fileptrprv = NULL;

                        while (fileptr)
                        {
                              if (fileptr->fd == fd)
                              {
                                    if (fileptrprv)
                                          fileptrprv->next = fileptr->next;

                                    if (fileptr->name)
                                          free(fileptr->name);

                                    if (fileptr->data)
                                          free(fileptr->data);

                                    free(fileptr);

                                    procmem->file->size--;

                                    status = STD_RET_OK;

                                    break;
                              }
                              else
                              {
                                    /* next file */
                                    fileptrprv = fileptr;
                                    fileptr    = fileptr->next;
                              }
                        }

                        GiveMutex(procmem->mtx);
                  }
            }
      }

      return status;
}


//================================================================================================//
/**
 * @brief Returns file size
 *
 * @param fd      file descriptor
 *
 * @return file size; if file does not exist return 0
 */
//================================================================================================//
size_t PROC_GetFileSize(procfd_t fd)
{
      size_t size = 0;

      if (fd)
      {
            if (procmem)
            {
                  if (TakeMutex(procmem->mtx, BLOCK_TIME) == OS_OK)
                  {
                        /* try find file */
                        struct filenode *fileptr = procmem->file;

                        while (fileptr)
                        {
                              if (fileptr->fd == fd)
                              {
                                    size = fileptr->size;
                                    break;
                              }
                              else
                              {
                                    fileptr = fileptr->next;
                              }
                        }

                        GiveMutex(procmem->mtx);
                  }
            }
      }

      return size;
}


//================================================================================================//
/**
 * @brief Function opens file
 *
 * @param *name   file name
 * @param *mode   file mode
 *
 * @return file description, if file does not exist return 0
 */
//================================================================================================//
procfd_t PROC_open(const ch_t *name, ch_t *mode)
{
      procfd_t fd = 0;

      if (name && mode)
      {
            /* allocate memory when not created yet */
            if (procmem == NULL)
            {
                  procmem = calloc(1, sizeof(struct filehdl));

                  if (procmem)
                  {
                        procmem->file = calloc(1, sizeof(struct filenode));
                        ch_t *name    = calloc(2, sizeof(ch_t));
                        CreateMutex(procmem->mtx);

                        if (!procmem->file || !name || !procmem->mtx)
                        {
                              if (procmem->file)
                                    free(procmem->file);

                              if (name)
                                    free(name);

                              if (procmem->mtx)
                                    DeleteMutex(procmem->mtx);
                        }
                        else
                        {
                              name = ".";
                              procmem->file->data = NULL;
                              procmem->file->fd   = ++procmem->fdcnt;
                              procmem->file->name = name;
                              procmem->file->next = NULL;
                              procmem->file->size = 0;
                        }
                  }
            }

            /* if memory exist */
            if (procmem)
            {
                  if (TakeMutex(procmem->mtx, BLOCK_TIME) == OS_OK)
                  {
                        /* try find file */
                        struct filenode *fileptr    = procmem->file;
                        struct filenode *fileptrprv = procmem->file;

                        while (fileptr)
                        {
                              if (strcmp(fileptr->name, name) == 0)
                              {
                                    /* gets file descriptor */
                                    fd = fileptr->fd;

                                    /* new data will be write */
                                    if (  (strncmp("w",  mode, 2) == 0)
                                       || (strncmp("w+", mode, 2) == 0) )
                                    {
                                          if (fileptr->data)
                                          {
                                                free(fileptr->data);
                                                fileptr->data = NULL;
                                                fileptr->size = 0;
                                          }
                                    }

                                    goto open_exit;
                              }
                              else
                              {
                                    /* next file */
                                    fileptrprv = fileptr;
                                    fileptr    = fileptr->next;
                              }
                        }

                        /* create file when mode is correct */
                        if (  (strncmp("w", mode, 2) == 0) || (strncmp("w+", mode, 2) == 0)
                           || (strncmp("a", mode, 2) == 0) || (strncmp("a+", mode, 2) == 0) )
                        {
                              /* file does not found - create new file */
                              struct filenode *newfile = calloc(1, sizeof(struct filenode));
                              ch_t *fname = malloc(strlen(name));

                              if (!newfile || !fname)
                              {
                                    if (newfile)
                                          free(newfile);

                                    if (fname)
                                          free(fname);
                              }
                              else
                              {
                                    strcpy(fname, name);

                                    fileptrprv->next = newfile;

                                    newfile->data = NULL;
                                    newfile->fd   = ++(procmem->fdcnt);
                                    newfile->name = fname;
                                    newfile->next = NULL;
                                    newfile->size = 0;

                                    procmem->file->size++;

                                    fd = newfile->fd;
                              }
                        }

                        open_exit:
                        GiveMutex(procmem->mtx);
                  }
            }
      }

      return fd;
}


//================================================================================================//
/**
 * @brief Close opened file
 */
//================================================================================================//
stdRet_t PROC_close(procfd_t fd)
{
      (void)fd;

      return STD_RET_OK;
}


//================================================================================================//
/**
 * @brief Write to file
 *
 * @param fd            file descriptor
 * @param *src          data source
 * @param size          item size
 * @param nitems        number of items
 * @param seek          position in file
 *
 * @return number of written items
 */
//================================================================================================//
size_t PROC_write(nod_t fd, void *src, size_t size, size_t nitems, size_t seek)
{
      size_t n = 0;

      if (fd && src && size && nitems)
      {
            if (TakeMutex(procmem->mtx, BLOCK_TIME) == OS_OK)
            {
                  struct filenode *fileptr = procmem->file;

                  while (fileptr)
                  {
                        if (fileptr->fd == fd)
                        {
                              size_t wrsize  = size * nitems;
                              size_t filelen = fileptr->size;

                              if (seek > filelen)
                                    seek = filelen;

                              if ((seek + wrsize) > filelen || fileptr->data == NULL)
                              {
                                    ch_t *newdata = malloc(filelen + wrsize);

                                    if (newdata)
                                    {
                                          if (fileptr->data)
                                          {
                                                memcpy(newdata, fileptr->data, filelen);
                                                free(fileptr->data);
                                          }

                                          memcpy(newdata + seek, src, wrsize);
                                          n = nitems;

                                          fileptr->data  = newdata;
                                          fileptr->size += wrsize - (filelen - seek);
                                    }
                              }
                              else
                              {
                                    memcpy(fileptr->data + seek, src, wrsize);
                                    n = nitems;
                              }

                              break;
                        }
                        else
                        {
                              fileptr = fileptr->next;
                        }
                  }

                  GiveMutex(procmem->mtx);
            }
      }

      return n;
}


//================================================================================================//
/**
 * @brief Read from file
 *
 * @param fd            file descriptor
 * @param *dst          data destination
 * @param size          item size
 * @param nitems        number of items
 * @param seek          position in file
 *
 * @return number of read items
 */
//================================================================================================//
size_t PROC_read(nod_t fd, void *dst, size_t size, size_t nitems, size_t seek)
{
      size_t n = 0;

      if (fd && dst && size && nitems)
      {
            if (TakeMutex(procmem->mtx, BLOCK_TIME) == OS_OK)
            {
                  struct filenode *fileptr = procmem->file;

                  while (fileptr)
                  {
                        if (fileptr->fd == fd)
                        {
                              size_t filelen = fileptr->size;
                              size_t items2rd;

                              /* check if seek is not bigger than file length */
                              if (seek > filelen)
                              {
                                    seek = filelen;
                              }

                              /* check how many items to read is on current file position */
                              if (((filelen - seek) / size) >= nitems)
                              {
                                    items2rd = nitems;
                              }
                              else
                              {
                                    items2rd = (filelen - seek) / size;
                              }

                              /* copy if file buffer exist */
                              if (fileptr->data)
                              {
                                    if (items2rd > 0)
                                    {
                                          memcpy(dst, fileptr->data + seek, items2rd * size);
                                          n = items2rd;
                                    }
                              }

                              break;
                        }
                        else
                        {
                              fileptr = fileptr->next;
                        }
                  }

                  GiveMutex(procmem->mtx);
            }
      }

      return n;
}


//================================================================================================//
/**
 * @brief Function opens /proc directory
 *
 * @param [out] *dir          directory object
 */
//================================================================================================//
void PROC_opendir(DIR_t *dir)
{
      dir->rddir = PROC_readdir;
      dir->seek  = 1;

      if (procmem)
            dir->items = procmem->file->size;
      else
            dir->items = 0;
}


//================================================================================================//
/**
 * @brief Function read next /proc directory elements
 *
 * @param seek    item number
 *
 * @return file attributes
 */
//================================================================================================//
dirent_t PROC_readdir(size_t seek)
{
      dirent_t direntry;
      direntry.rm   = PROC_remove;
      direntry.name = NULL;
      direntry.size = 0;

      if (procmem)
      {
            if (TakeMutex(procmem->mtx, BLOCK_TIME) == OS_OK)
            {
                  if (seek <= procmem->file->size)
                  {
                        struct filenode *fileptr = procmem->file->next;

                        while (fileptr && --seek)
                        {
                              fileptr = fileptr->next;
                        }

                        direntry.name   = fileptr->name;
                        direntry.size   = fileptr->size;
                        direntry.isfile = TRUE;
                        direntry.fd     = fileptr->fd;
                  }

                  GiveMutex(procmem->mtx);
            }
      }

      return direntry;
}


#ifdef __cplusplus
}
#endif

/*==================================================================================================
                                            End of file
==================================================================================================*/
