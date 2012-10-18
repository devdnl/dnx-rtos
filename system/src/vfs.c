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
#include "regdrv.h"
#include <string.h>


/*==================================================================================================
                                  Local symbolic constants/macros
==================================================================================================*/


/*==================================================================================================
                                   Local types, enums definitions
==================================================================================================*/


/*==================================================================================================
                                      Local function prototypes
==================================================================================================*/


/*==================================================================================================
                                      Local object definitions
==================================================================================================*/


/*==================================================================================================
                                        Function definitions
==================================================================================================*/

//================================================================================================//
/**
 * @brief
 */
//================================================================================================//
FILE_t *fopen(const ch_t *name, const ch_t *mode)
{
      (void) mode;

      FILE_t *file = NULL;

      if (name)
      {
            file = (FILE_t*)Calloc(1, sizeof(FILE_t));

            if (file != NULL)
            {
                  ch_t *slash = strchr(name + 1, '/');

                  if (strncmp("/dev", name, slash - name) == 0)
                  {
                        regDrvData_t drvdata = GetDrvData(slash + 1);

                        if (drvdata.open)
                        {
                              if (drvdata.open(drvdata.device) == STD_RET_OK)
                              {
                                    file->close = drvdata.close;
                                    file->fd    = drvdata.device;
                                    file->ioctl = drvdata.ioctl;
                                    file->read  = drvdata.read;
                                    file->write = drvdata.write;
                                    goto fopen_end;
                              }
                        }
                  }

                  Free(file);
                  file = NULL;
            }
      }

      fopen_end:
      return file;
}


//================================================================================================//
/**
 * @brief
 */
//================================================================================================//
stdRet_t fclose(FILE_t *file)
{
      stdRet_t status = STD_RET_ERROR;

      if (file)
      {
            if (file->close(file->fd) == STD_RET_OK)
            {
                  Free(file);
                  status = STD_RET_OK;
            }
      }

      return status;
}


//================================================================================================//
/**
 * @brief
 */
//================================================================================================//
size_t fwrite(void *ptr, size_t size, size_t nitems, FILE_t *file)
{
      if (ptr && size && nitems && file)
      {
            if (file->write)
            {
                  return file->write(file->fd, ptr, size * nitems, file->seek);
            }
      }

      return STD_RET_ERROR;
}


//================================================================================================//
/**
 * @brief
 */
//================================================================================================//
size_t fread(void *ptr, size_t size, size_t nitems, FILE_t *file)
{
      if (ptr && size && nitems && file)
      {
            if (file->read)
            {
                  return file->read(file->fd, (void*)ptr, size * nitems, file->seek);
            }
      }

      return STD_RET_ERROR;
}


//================================================================================================//
/**
 * @brief
 */
//================================================================================================//
stdRet_t fseek(FILE_t *file, i32_t offset, i32_t mode)
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
 * @brief
 */
//================================================================================================//
stdRet_t ioctl(FILE_t *file, IORq_t rq, void *data)
{
      stdRet_t status = STD_RET_ERROR;

      if (file && file->ioctl)
      {
            return file->ioctl(file->fd, rq, data);
      }

      return status;
}



#ifdef __cplusplus
}
#endif

/*==================================================================================================
                                            End of file
==================================================================================================*/
