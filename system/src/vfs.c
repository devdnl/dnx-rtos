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
#include "memman.h"
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
 * @brief Function open selected file
 *
 * @param *name         file path
 * @param *mode         file mode
 *
 * @retval NULL if file can't be created
 */
//================================================================================================//
FILE_t *fopen(const ch_t *name, const ch_t *mode)
{
      (void) mode;

      FILE_t *file = NULL;

      if (name && mode)
      {
            file = Calloc(1, sizeof(FILE_t));

            if (file != NULL)
            {
                  ch_t *slash = strchr(name + 1, '/');

                  /* check if path is device */
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
                  /* file does not exist */
                  else
                  {
                        Free(file);
                        file = NULL;
                  }

                  /* check mode */
                  if (file)
                  {
                        /* open for reading */
                        if (strncmp("r", mode, 2) == 0)
                        {
                              file->write = NULL;
                        }
                        /* open for writing (file need not exist) */
                        else if (strncmp("w", mode, 2) == 0)
                        {
                              file->read = NULL;
                        }
                        /* open for appending (file need not exist) */
                        else if (strncmp("a", mode, 2) == 0)
                        {
                              file->read = NULL;
                        }
                        /* open for reading and writing, start at beginning */
                        else if (strncmp("r+", mode, 2) == 0)
                        {
                              /* nothing to change */
                        }
                        /* open for reading and writing (overwrite file) */
                        else if (strncmp("w+", mode, 2) == 0)
                        {
                              /* nothing to change */
                        }
                        /* open for reading and writing (append if file exists) */
                        else if (strncmp("a+", mode, 2) == 0)
                        {
                              /* nothing to change */
                        }
                        /* invalid mode */
                        else
                        {
                              Free(file);
                              file = NULL;
                        }
                  }
            }
      }

      fopen_end:
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
size_t fwrite(void *ptr, size_t size, size_t nitems, FILE_t *file)
{
      size_t n = 0;

      if (ptr && size && nitems && file)
      {
            if (file->write)
            {
                  n = file->write(file->fd, ptr, size, nitems, file->seek);
                  file->seek += n;
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
size_t fread(void *ptr, size_t size, size_t nitems, FILE_t *file)
{
      size_t n = 0;

      if (ptr && size && nitems && file)
      {
            if (file->read)
            {
                  n = file->read(file->fd, ptr, size, nitems, file->seek);
                  file->seek += n;
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
