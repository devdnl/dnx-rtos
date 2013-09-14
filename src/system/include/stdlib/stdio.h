#ifndef _STDIO_H_
#define _STDIO_H_
/*=========================================================================*//**
@file    dnxio.h

@author  Daniel Zorychta

@brief

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
#include "config.h"
#include "core/sysmoni.h"

/*==============================================================================
  Exported macros
==============================================================================*/
/** stream values */
#define EOF                     (-1)
#define ETX                     0x03
#define EOT                     0x04

/** stdio buffer size */
#define BUFSIZ                  CONFIG_FSCANF_STREAM_BUFFER_SIZE

/** function-like macros */
#define ioctl(file, ...)        vfs_ioctl(file, __VA_ARGS__)

/*==============================================================================
  Exported object types
==============================================================================*/

/*==============================================================================
  Exported objects
==============================================================================*/

/*==============================================================================
  Exported functions
==============================================================================*/

/*==============================================================================
  Exported inline functions
==============================================================================*/

//==============================================================================
/**
 * @brief Function open selected file
 *
 * @param[in] *name             file path
 * @param[in] *mode             file mode
 *
 * @retval NULL if file can't be created
 */
//==============================================================================
static inline FILE *fopen(const char *path, const char *mode)
{
        return sysm_fopen(path, mode);
}

//==============================================================================
/**
 * @brief Function close old stream and open new
 *
 * @param[in] *name             file path
 * @param[in] *mode             file mode
 * @param[in] *file             old stream
 *
 * @retval NULL if file can't be created
 */
//==============================================================================
static inline FILE *freopen(const char *path, const char *mode, FILE *file)
{
        return sysm_freopen(path, mode, file);
}

//==============================================================================
/**
 * @brief Function close opened file
 *
 * @param[in] *file             pinter to file
 *
 * @return 0 on success. On error, -1 is returned
 */
//==============================================================================
static inline int fclose(FILE *file)
{
        return sysm_fclose(file);
}

//==============================================================================
/**
 * @brief Function write data to file
 *
 * @param[in] *ptr              address to data (src)
 * @param[in]  size             item size
 * @param[in]  nitems           number of items
 * @param[in] *file             pointer to file object
 *
 * @return the number of items successfully written. If an error occurs, or the
 *         end-of-file is reached, the return value is a short item count (or 0).
 */
//==============================================================================
static inline size_t fwrite(void *ptr, size_t size, size_t nitems, FILE *file)
{
        return vfs_fwrite(ptr, size, nitems, file);
}

//==============================================================================
/**
 * @brief Function read data from file
 *
 * @param[out] *ptr             address to data (dst)
 * @param[in]   size            item size
 * @param[in]   nitems          number of items
 * @param[in]  *file            pointer to file object
 *
 * @return the number of items successfully read. If an error occurs, or the
 *         end-of-file is reached, the return value is a short item count (or 0).
 */
//==============================================================================
static inline size_t fread(void *ptr, size_t size, size_t nitems, FILE *file)
{
        return vfs_fread(ptr, size, nitems, file);
}

//==============================================================================
/**
 * @brief Function set seek value
 *
 * @param[in] *file             file object
 * @param[in]  offset           seek value
 * @param[in]  mode             seek mode
 *
 * @return 0 on success. On error, -1 is returned
 */
//==============================================================================
static inline int fseek(FILE *file, i64_t offset, int mode)
{
        return vfs_fseek(file, offset, mode);
}

//==============================================================================
/**
 * @brief Function returns seek value
 *
 * @param[in] *file             file object
 *
 * @return -1 if error, otherwise correct value
 */
//==============================================================================
static inline i64_t ftell(FILE *file)
{
        return vfs_ftell(file);
}

//==============================================================================
/**
 * @brief Function returns file/dir status
 *
 * @param[in]  *path            file/dir path
 * @param[out] *stat            pointer to stat structure
 *
 * @return 0 on success. On error, -1 is returned
 */
//==============================================================================
static inline int fstat(FILE *file, struct vfs_stat *stat)
{
        return vfs_fstat(file, stat);
}

//==============================================================================
/**
 * @brief Function flush file data
 *
 * @param[in] *file     file to flush
 *
 * @return 0 on success. On error, -1 is returned
 */
//==============================================================================
static inline int fflush(FILE *file)
{
        return vfs_fflush(file);
}

//==============================================================================
/**
 * @brief Function check end of file
 *
 * @param[in] *file     file
 *
 * @return 0 if there is not a file end, otherwise greather than 0
 */
//==============================================================================
static inline int feof(FILE *file)
{
        return vfs_feof(file);
}

//==============================================================================
/**
 * @brief Function rewind file
 *
 * @param[in] *file     file
 *
 * @return 0 on success. On error, -1 is returned
 */
//==============================================================================
static inline int rewind(FILE *file)
{
        return vfs_rewind(file);
}

//==============================================================================
/**
 * @brief Checks if the error indicator associated with stream is set
 *
 * @param[in] *file             pinter to file
 *
 * @return 0 on success (no error). On error, 1 is returned
 */
//==============================================================================
static inline int ferror(FILE *file)
{
        return file != NULL ? 0 : (int)file;
}

//==============================================================================
/**
 * @brief Function gets mount point for n item
 *
 * @param[in]   item            mount point number
 * @param[out] *mntent          mount entry data
 *
 * @retval STD_RET_OK           mount success
 * @retval STD_RET_ERROR        mount error
 */
//==============================================================================
static inline stdret_t getmntentry(size_t item, struct vfs_mntent *mntent)
{
        return vfs_getmntentry(item, mntent);
}

//==============================================================================
/**
 * @brief Function create node for driver file
 *
 * @param[in] *path                 path when driver-file shall be created
 * @param[in] *drvcfg               pointer to description of driver
 *
 * @return zero on success. On error, -1 is returned
 */
//==============================================================================
static inline int mknod(const char *path, struct vfs_drv_interface *drvif)
{
        return vfs_mknod(path, drvif);
}

//==============================================================================
/**
 * @brief Create directory
 *
 * @param[in] *path                 path to new directory
 *
 * @return 0 on success. On error, -1 is returned
 */
//==============================================================================
static inline int mkdir(const char *path)
{
        return vfs_mkdir(path);
}

//==============================================================================
/**
 * @brief Function open directory
 *
 * @param[in] *path                 directory path
 *
 * @return directory object
 */
//==============================================================================
static inline DIR *opendir(const char *path)
{
        return sysm_opendir(path);
}

//==============================================================================
/**
 * @brief Function close opened directory
 *
 * @param[in] *dir                  directory object
 *
 * @return 0 on success. On error, -1 is returned
 */
//==============================================================================
static inline int closedir(DIR *dir)
{
        return sysm_closedir(dir);
}

//==============================================================================
/**
 * @brief Function read next item of opened directory
 *
 * @param[in] *dir                  directory object
 *
 * @return element attributes
 */
//==============================================================================
static inline dirent_t readdir(DIR *dir)
{
        return vfs_readdir(dir);
}

//==============================================================================
/**
 * @brief Remove file
 * Removes file or directory. Removes directory if is not a mount point.
 *
 * @param[in] *patch                localization of file/directory
 *
 * @return 0 on success. On error, -1 is returned
 */
//==============================================================================
static inline int remove(const char *path)
{
        return vfs_remove(path);
}

//==============================================================================
/**
 * @brief Rename file name
 * The implementation of rename can move files only if external FS provide
 * functionality. Local VFS cannot do this. Cross FS move is also not possible.
 *
 * @param[in] *old_name                  old file name
 * @param[in] *new_name                  new file name
 *
 * @return 0 on success. On error, -1 is returned
 */
//==============================================================================
static inline int rename(const char *old_name, const char *new_name)
{
        return vfs_rename(old_name, new_name);
}

//==============================================================================
/**
 * @brief Function change file mode
 *
 * @param[in] *path         file path
 * @param[in]  mode         file mode
 *
 * @return 0 on success. On error, -1 is returned
 */
//==============================================================================
static inline int chmod(const char *path, int mode)
{
        return vfs_chmod(path, mode);
}

//==============================================================================
/**
 * @brief Function change file owner and group
 *
 * @param[in] *path         file path
 * @param[in]  owner        file owner
 * @param[in]  group        file group
 *
 * @return 0 on success. On error, -1 is returned
 */
//==============================================================================
static inline int chown(const char *path, int owner, int group)
{
        return vfs_chown(path, owner, group);
}

//==============================================================================
/**
 * @brief Function returns file/dir status
 *
 * @param[in]  *path            file/dir path
 * @param[out] *stat            pointer to structure
 *
 * @return 0 on success. On error, -1 is returned
 */
//==============================================================================
static inline int stat(const char *path, struct vfs_stat *stat)
{
        return vfs_stat(path, stat);
}

//==============================================================================
/**
 * @brief Function returns file system status
 *
 * @param[in]  *path            fs path
 * @param[out] *statfs          pointer to FS status structure
 *
 * @return 0 on success. On error, -1 is returned
 */
//==============================================================================
static inline int statfs(const char *path, struct vfs_statfs *statfs)
{
        return vfs_statfs(path, statfs);
}

#ifdef __cplusplus
}
#endif

#endif /* _STDIO_H_ */
/*==============================================================================
  End of file
==============================================================================*/
