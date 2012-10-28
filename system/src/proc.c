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
#include "memman.h"
#include "oswrap.h"
#include <string.h>


/*==================================================================================================
                                  Local symbolic constants/macros
==================================================================================================*/
#define FILE_START_SIZE                   10


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
stdRet_t PROC_remove(const ch_t *name)
{
      stdRet_t status = STD_RET_ERROR;

      if (name)
      {
            /* if memory exist */
            if (procmem && (strcmp(name, ".") != 0))
            {
                  /* try find file */
                  struct filenode *fileptr    = procmem->file;
                  struct filenode *fileptrprv = NULL;

                  while (fileptr)
                  {
                        if (strcmp(fileptr->name, name) == 0)
                        {
                              if (fileptrprv)
                                    fileptrprv->next = fileptr->next;

                              if (fileptr->name)
                                    Free(fileptr->name);

                              if (fileptr->data)
                                    Free(fileptr->data);

                              Free(fileptr);

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
            }
      }

      return status;
}


size_t PROC_GetFileSize(procfd_t fd)
{
      size_t size = 0;

      if (fd)
      {
            if (procmem)
            {
                  /* try find file */
                  struct filenode *fileptr = procmem->file;

                  while (fileptr)
                  {
                        if (fileptr->fd == fd)
                        {
                              size = fileptr->size;
                        }
                        else
                        {
                              /* next file */
                              fileptr = fileptr->next;
                        }
                  }
            }
      }

      return size;
}


procfd_t PROC_open(const ch_t *name)
{
      procfd_t fd = 0;

      if (name)
      {
            /* allocate memory when not created yet */
            if (procmem == NULL)
            {
                  procmem = Calloc(1, sizeof(struct filehdl));

                  if (procmem)
                  {
                        procmem->file = Calloc(1, sizeof(struct filenode));
                        ch_t *name    = Calloc(2, sizeof(ch_t));
                        procmem->mtx  = CreateMutex();

                        if (!procmem->file || !name || !procmem->mtx)
                        {
                              if (procmem->file)
                                    Free(procmem->file);

                              if (name)
                                    Free(name);

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
                  /* try find file */
                  struct filenode *fileptr    = procmem->file;
                  struct filenode *fileptrprv = procmem->file;

                  while (fileptr)
                  {
                        if (strcmp(fileptr->name, name) == 0)
                        {
                              /* gets file descriptor */
                              fd = fileptr->fd;
                              goto open_exit;
                        }
                        else
                        {
                              /* next file */
                              fileptrprv = fileptr;
                              fileptr    = fileptr->next;
                        }
                  }

                  /* file does not found - create new file */
                  struct filenode *newfile = Calloc(1, sizeof(struct filenode));
                  ch_t *fname = Malloc(strlen(name));

                  if (!newfile || !fname)
                  {
                        if (newfile)
                              Free(newfile);

                        if (fname)
                              Free(fname);
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
      }

      open_exit:
      return fd;
}



stdRet_t PROC_close(procfd_t fd)
{
      return STD_RET_OK;
}



size_t PROC_write(nod_t fd, void *src, size_t size, size_t nitems, size_t seek)
{
      return 0;
}


size_t PROC_read(nod_t fd, void *dst, size_t size, size_t nitems, size_t seek)
{
      return 0;
}


void PROC_opendir(DIR_t *dir)
{
      dir->readdir = PROC_readdir;
      dir->items   = procmem->file->size;
      dir->seek    = 1;
}


dirent_t PROC_readdir(size_t seek)
{
      dirent_t direntry;
      direntry.name = NULL;
      direntry.size = 0;

      if (seek <= procmem->file->size)
      {
            struct filenode *fileptr = procmem->file->next;

            while (fileptr && --seek)
            {
                  fileptr = fileptr->next;
            }

            direntry.name = fileptr->name;
            direntry.size = fileptr->size;
      }

      return direntry;
}


#ifdef __cplusplus
}
#endif

/*==================================================================================================
                                            End of file
==================================================================================================*/
