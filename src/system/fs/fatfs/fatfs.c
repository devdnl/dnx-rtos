/*=========================================================================*//**
@file    fatfs.c

@author  Daniel Zorychta

@brief   FAT File system support.

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
#include "system/dnxfs.h"
#include "libfat/libfat.h"

/*==============================================================================
  Local symbolic constants/macros
==============================================================================*/

/*==============================================================================
  Local types, enums definitions
==============================================================================*/
struct fatdir {
        FATDIR dir;
#if _LIBFAT_USE_LFN == 0
        char name[14];
#else
        char name[(_LIBFAT_MAX_LFN + 1) * sizeof(TCHAR)];
#endif
};

struct fatfs {
        FILE  *fsfile;
        FATFS  fatfs;
        int    opened_files;
        int    opened_dirs;
};

/*==============================================================================
  Local function prototypes
==============================================================================*/
static stdret_t fatfs_closedir  (void *fs_handle, DIR *dir);
static dirent_t fatfs_readdir   (void *fs_handle, DIR *dir);
static int      handle_error    (FRESULT fresult);

/*==============================================================================
  Local object definitions
==============================================================================*/

/*==============================================================================
  Exported object definitions
==============================================================================*/

/*==============================================================================
  Function definitions
==============================================================================*/

//==============================================================================
/**
 * @brief Initialize file system
 *
 * @param[out]          **fs_handle             file system allocated memory
 * @param[in ]           *src_path              file source path
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
API_FS_INIT(fatfs, void **fs_handle, const char *src_path)
{
        STOP_IF(!fs_handle);
        STOP_IF(!src_path);

        struct fatfs *hdl = calloc(1, sizeof(struct fatfs));

        if (hdl) {
                *fs_handle = hdl;

                if ((hdl->fsfile = vfs_fopen(src_path, "r+"))) {
                        if (handle_error(libfat_mount(hdl->fsfile, &hdl->fatfs)) == 0) {
                                return STD_RET_OK;
                        }
                }

                /* error */
                if (hdl->fsfile) {
                        vfs_fclose(hdl->fsfile);
                }

                free(hdl);
        }

        return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief Release file system
 *
 * @param[in ]          *fs_handle              file system allocated memory
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
API_FS_RELEASE(fatfs, void *fs_handle)
{
        STOP_IF(!fs_handle);

        struct fatfs *hdl = fs_handle;

        if (hdl->opened_dirs == 0 && hdl->opened_files == 0) {
                if (handle_error(libfat_umount(&hdl->fatfs)) == 0) {
                        vfs_fclose(hdl->fsfile);
                        free(hdl);
                        return STD_RET_OK;
                }
        } else {
                errno = EBUSY;
        }

        return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief Open file
 *
 * @param[in ]          *fs_handle              file system allocated memory
 * @param[out]          *extra                  file extra data
 * @param[out]          *fd                     file descriptor
 * @param[out]          *fpos                   file position
 * @param[in]           *path                   file path
 * @param[in]            flags                  file open flags (see vfs.h)
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
API_FS_OPEN(fatfs, void *fs_handle, void **extra, fd_t *fd, u64_t *fpos, const char *path, int flags)
{
        UNUSED_ARG(fd);

        STOP_IF(!fs_handle);
        STOP_IF(!extra);
        STOP_IF(!fpos);
        STOP_IF(!path);

        struct fatfs *hdl = fs_handle;

        FATFILE *fat_file = calloc(1, sizeof(FATFILE));
        if (!fat_file)
                return STD_RET_ERROR;

        *extra = fat_file;

        u8_t fat_mode = 0;
        if (flags == O_RDONLY) {
                fat_mode = LIBFAT_FA_READ | LIBFAT_FA_OPEN_EXISTING;
        } else if (flags == O_RDWR) {
                fat_mode = LIBFAT_FA_READ | LIBFAT_FA_WRITE | LIBFAT_FA_OPEN_EXISTING;
        } else if (flags == (O_WRONLY | O_CREAT)) {
                fat_mode = LIBFAT_FA_WRITE | LIBFAT_FA_CREATE_ALWAYS;
        } else if (flags == (O_RDWR | O_CREAT)) {
                fat_mode = LIBFAT_FA_WRITE | LIBFAT_FA_READ | LIBFAT_FA_CREATE_ALWAYS;
        } else if (flags == (O_WRONLY | O_APPEND | O_CREAT)) {
                fat_mode = LIBFAT_FA_WRITE | LIBFAT_FA_OPEN_ALWAYS;
        } else if (flags == (O_RDWR | O_APPEND | O_CREAT)) {
                fat_mode = LIBFAT_FA_WRITE | LIBFAT_FA_READ | LIBFAT_FA_OPEN_ALWAYS;
        } else {
                free(fat_file);
                errno = EINVAL;
                return STD_RET_ERROR;
        }

        if (handle_error(libfat_open(&hdl->fatfs, fat_file, path, fat_mode)) != 0) {
                free(fat_file);
                return STD_RET_ERROR;
        }

        if (handle_error(libfat_open(&hdl->fatfs, fat_file, path, fat_mode)) != 0) {
                free(fat_file);
                return STD_RET_ERROR;
        }

        if (flags & O_APPEND) {
                if (handle_error(libfat_lseek(fat_file, libfat_size(fat_file))) != 0) {
                        free(fat_file);
                        return STD_RET_ERROR;
                }
                *fpos = libfat_size(fat_file);
        } else {
                *fpos = 0;
        }

        hdl->opened_files++;

        return STD_RET_OK;
}

//==============================================================================
/**
 * @brief Close file
 *
 * @param[in ]          *fs_handle              file system allocated memory
 * @param[in ]          *extra                  file extra data
 * @param[in ]           fd                     file descriptor
 * @param[in ]           force                  force close
 * @param[in ]          *file_owner             task which opened file (valid if force is true)
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
API_FS_CLOSE(fatfs, void *fs_handle, void *extra, fd_t fd, bool force, task_t *file_owner)
{
        UNUSED_ARG(fd);
        UNUSED_ARG(force);
        UNUSED_ARG(file_owner);

        STOP_IF(!fs_handle);
        STOP_IF(!extra);

        struct fatfs *hdl = fs_handle;

        FATFILE *fat_file = extra;
        if (handle_error(libfat_close(fat_file)) == 0) {
                free(fat_file);
                hdl->opened_files--;
                return STD_RET_OK;
        }

        return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief Write data to the file
 *
 * @param[in ]          *fs_handle              file system allocated memory
 * @param[in ]          *extra                  file extra data
 * @param[in ]           fd                     file descriptor
 * @param[in ]          *src                    data source
 * @param[in ]           count                  number of bytes to write
 * @param[in ]          *fpos                   position in file

 * @return number of written bytes, -1 if error
 */
//==============================================================================
API_FS_WRITE(fatfs, void *fs_handle, void *extra, fd_t fd, const u8_t *src, size_t count, u64_t *fpos)
{
        UNUSED_ARG(fs_handle);
        UNUSED_ARG(fd);

        STOP_IF(!extra);
        STOP_IF(!src);
        STOP_IF(!count);
        STOP_IF(!fpos);

        FATFILE *fat_file = extra;
        uint     n        = 0;

        if (libfat_tell(fat_file) != (u32_t)*fpos) {
                if (handle_error(libfat_lseek(fat_file, (u32_t)*fpos)) != 0) {
                        return -1;
                }
        }

        if (handle_error(libfat_write(fat_file, src, count, &n)) != 0) {
                return -1;
        } else {
                return n;
        }
}

//==============================================================================
/**
 * @brief Read data from file
 *
 * @param[in ]          *fs_handle              file system allocated memory
 * @param[in ]          *extra                  file extra data
 * @param[in ]           fd                     file descriptor
 * @param[out]          *dst                    data destination
 * @param[in ]           count                  number of bytes to read
 * @param[in ]          *fpos                   position in file

 * @return number of read bytes, -1 if error
 */
//==============================================================================
API_FS_READ(fatfs, void *fs_handle, void *extra, fd_t fd, void *dst, size_t count, u64_t *fpos)
{
        UNUSED_ARG(fs_handle);
        UNUSED_ARG(fd);

        STOP_IF(!extra);
        STOP_IF(!dst);
        STOP_IF(!count);
        STOP_IF(!fpos);

        FATFILE *fat_file = extra;
        uint     n        = 0;

        if (libfat_tell(fat_file) != (u32_t)*fpos) {
                if (handle_error(libfat_lseek(fat_file, (u32_t)*fpos)) != 0) {
                        return -1;
                }
        }

        if (handle_error(libfat_read(fat_file, dst, count, &n)) != 0) {
                return -1;
        } else {
                return n;
        }
}

//==============================================================================
/**
 * @brief IO operations on files
 *
 * @param[in ]          *fs_handle              file system allocated memory
 * @param[in ]          *extra                  file extra data
 * @param[in ]           fd                     file descriptor
 * @param[in ]           request                request
 * @param[in ][out]     *arg                    request's argument
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
API_FS_IOCTL(fatfs, void *fs_handle, void *extra, fd_t fd, int request, void *arg)
{
        UNUSED_ARG(fs_handle);
        UNUSED_ARG(extra);
        UNUSED_ARG(fd);
        UNUSED_ARG(request);
        UNUSED_ARG(arg);

        /* not supported by this file system */
        errno = EBADRQC;

        return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief Flush file data
 *
 * @param[in ]          *fs_handle              file system allocated memory
 * @param[in ]          *extra                  file extra data
 * @param[in ]           fd                     file descriptor
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
API_FS_FLUSH(fatfs, void *fs_handle, void *extra, fd_t fd)
{
        UNUSED_ARG(fs_handle);
        UNUSED_ARG(fd);

        STOP_IF(!extra);

        FATFILE *fat_file = extra;
        if (handle_error(libfat_sync(fat_file)) == 0)
                return STD_RET_OK;
        else
                return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief Return file status
 *
 * @param[in ]          *fs_handle              file system allocated memory
 * @param[in ]          *extra                  file extra data
 * @param[in ]           fd                     file descriptor
 * @param[out]          *stat                   file status
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
API_FS_FSTAT(fatfs, void *fs_handle, void *extra, fd_t fd, struct stat *stat)
{
        UNUSED_ARG(fs_handle);
        UNUSED_ARG(fd);

        STOP_IF(!extra);
        STOP_IF(!stat);

        FATFILE *fat_file  = extra;
        stat->st_dev   = 0;
        stat->st_gid   = 0;
        stat->st_mode  = 0777;
        stat->st_mtime = 0;
        stat->st_size  = fat_file->fsize;
        stat->st_uid   = 0;
        stat->st_type  = FILE_TYPE_REGULAR;

        return STD_RET_OK;
}

//==============================================================================
/**
 * @brief Create directory
 *
 * @param[in ]          *fs_handle              file system allocated memory
 * @param[in ]          *path                   name of created directory
 * @param[in ]           mode                   dir mode
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
API_FS_MKDIR(fatfs, void *fs_handle, const char *path, mode_t mode)
{
        struct fatfs *hdl = fs_handle;

        STOP_IF(!path);

        if (handle_error(libfat_mkdir(&hdl->fatfs, path)) == 0) {
                uint8_t dosmode = mode & S_IWUSR ? 0 : LIBFAT_AM_RDO;
                if (handle_error(libfat_chmod(&hdl->fatfs, path, dosmode, LIBFAT_AM_RDO)) == 0) {
                        return STD_RET_OK;
                }
        }

        return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief Create pipe
 *
 * @param[in ]          *fs_handle              file system allocated memory
 * @param[in ]          *path                   name of created pipe
 * @param[in ]           mode                   pipe mode
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
API_FS_MKFIFO(fatfs, void *fs_handle, const char *path, mode_t mode)
{
        STOP_IF(!fs_handle);
        STOP_IF(!path);
        UNUSED_ARG(mode);

        /* not supported by this file system */
        errno = EPERM;

        return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief Create node for driver file
 *
 * @param[in ]          *fs_handle              file system allocated memory
 * @param[in ]          *path                   name of created node
 * @param[in ]          *drv_if                 driver interface
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
API_FS_MKNOD(fatfs, void *fs_handle, const char *path, const struct vfs_drv_interface *drv_if)
{
        UNUSED_ARG(fs_handle);
        UNUSED_ARG(path);
        UNUSED_ARG(drv_if);

        /* not supported by this file system */
        errno = EPERM;

        return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief Open directory
 *
 * @param[in ]          *fs_handle              file system allocated memory
 * @param[in ]          *path                   name of opened directory
 * @param[in ]          *dir                    directory object
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
API_FS_OPENDIR(fatfs, void *fs_handle, const char *path, DIR *dir)
{
        STOP_IF(!fs_handle);
        STOP_IF(!path);
        STOP_IF(!dir);

        struct fatfs *hdl = fs_handle;

        char *dospath = calloc(strlen(path) + 1, 1);
        if (!dospath) {
                return STD_RET_ERROR;
        }

        dir->f_dd = malloc(sizeof(struct fatdir));
        if (!dir->f_dd) {
                free(dospath);
                return STD_RET_ERROR;
        }

        if (strlen(path) == 1) {
                strcpy(dospath, path);
        } else {
                strncpy(dospath, path, strlen(path) - 1);
        }

        dir->f_handle   = hdl;
        dir->f_closedir = fatfs_closedir;
        dir->f_readdir  = fatfs_readdir;
        dir->f_seek     = 0;
        dir->f_items    = 0;

        struct fatdir *fatdir = dir->f_dd;
        if (handle_error(libfat_opendir(&hdl->fatfs, &fatdir->dir, dospath)) == 0) {
                free(dospath);
                hdl->opened_dirs++;
                return STD_RET_OK;
        }

        free(dospath);
        free(dir->f_dd);

        return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief Function close dir
 *
 * @param[in] *fs_handle        FS handle
 * @param[in] *dir              directory info
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
static stdret_t fatfs_closedir(void *fs_handle, DIR *dir)
{
        STOP_IF(!fs_handle);
        STOP_IF(!dir);

        struct fatfs *hdl = fs_handle;

        if (dir->f_dd) {
                free(dir->f_dd);
                hdl->opened_dirs--;
        }

        return STD_RET_OK;
}

//==============================================================================
/**
 * @brief Function read current directory
 *
 * @param[in] *fs_handle        FS handle
 * @param[in] *dir              directory info
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
static dirent_t fatfs_readdir(void *fs_handle, DIR *dir)
{
        UNUSED_ARG(fs_handle);

        STOP_IF(!dir);

        struct fatdir *fatdir = dir->f_dd;

        dirent_t dirent;
        dirent.name = NULL;
        dirent.size = 0;

        FILEINFO fat_file_info;
#if _LIBFAT_USE_LFN != 0
        fat_file_info.lfname = &fatdir->name[0];
        fat_file_info.lfsize = _LIBFAT_MAX_LFN;
#endif
        if (handle_error(libfat_readdir(&fatdir->dir, &fat_file_info)) == 0) {
                if (fat_file_info.fname[0] != 0) {
#if _LIBFAT_USE_LFN != 0
                        if (fat_file_info.lfname[0] == 0) {
                                memcpy(fatdir->name, fat_file_info.fname, 13);
                        }
#else
                        memcpy(fatdir->name, fat_file_info.fname, 13);
#endif
                        dirent.name     = &fatdir->name[0];
                        dirent.filetype = fat_file_info.fattrib & LIBFAT_AM_DIR ? FILE_TYPE_DIR : FILE_TYPE_REGULAR;
                        dirent.size     = fat_file_info.fsize;
                }
        }

        return dirent;
}

//==============================================================================
/**
 * @brief Remove file/directory
 *
 * @param[in ]          *fs_handle              file system allocated memory
 * @param[in ]          *path                   name of removed file/directory
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
API_FS_REMOVE(fatfs, void *fs_handle, const char *path)
{
        STOP_IF(!fs_handle);
        STOP_IF(!path);

        struct fatfs *hdl = fs_handle;

        if (handle_error(libfat_unlink(&hdl->fatfs, path)) == 0)
                return STD_RET_OK;
        else
                return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief Rename file/directory
 *
 * @param[in ]          *fs_handle              file system allocated memory
 * @param[in ]          *old_name               old object name
 * @param[in ]          *new_name               new object name
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
API_FS_RENAME(fatfs, void *fs_handle, const char *old_name, const char *new_name)
{
        STOP_IF(!fs_handle);
        STOP_IF(!old_name);
        STOP_IF(!new_name);

        struct fatfs *hdl = fs_handle;

        if (handle_error(libfat_rename(&hdl->fatfs, old_name, new_name)) == 0)
                return STD_RET_OK;
        else
                return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief Change file's mode
 *
 * @param[in ]          *fs_handle              file system allocated memory
 * @param[in ]          *path                   file path
 * @param[in ]           mode                   new file mode
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
API_FS_CHMOD(fatfs, void *fs_handle, const char *path, int mode)
{
        STOP_IF(!fs_handle);
        STOP_IF(!path);

        struct fatfs *hdl = fs_handle;

        uint8_t dosmode = mode & S_IWUSR ? 0 : LIBFAT_AM_RDO;
        if (handle_error(libfat_chmod(&hdl->fatfs, path, dosmode, LIBFAT_AM_RDO)) == 0) {
                return STD_RET_OK;
        }

        return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief Change file's owner and group
 *
 * @param[in ]          *fs_handle              file system allocated memory
 * @param[in ]          *path                   file path
 * @param[in ]           owner                  new file owner
 * @param[in ]           group                  new file group
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
API_FS_CHOWN(fatfs, void *fs_handle, const char *path, int owner, int group)
{
        UNUSED_ARG(fs_handle);
        UNUSED_ARG(path);
        UNUSED_ARG(owner);
        UNUSED_ARG(group);

        /* not supported by this file system */
        errno = EPERM;

        return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief Return file/dir status
 *
 * @param[in ]          *fs_handle              file system allocated memory
 * @param[in ]          *path                   file path
 * @param[out]          *stat                   file status
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
API_FS_STAT(fatfs, void *fs_handle, const char *path, struct stat *stat)
{
        STOP_IF(!fs_handle);
        STOP_IF(!path);
        STOP_IF(!stat);

        struct fatfs *hdl = fs_handle;

        FILEINFO file_info;
        if (handle_error(libfat_stat(&hdl->fatfs, path, &file_info)) == 0) {
                stat->st_dev   = 0;
                stat->st_gid   = 0;
                stat->st_mode  = 0777;
                stat->st_mtime = file_info.ftime;
                stat->st_size  = file_info.fsize;
                stat->st_uid   = 0;
                stat->st_type  = FILE_TYPE_REGULAR;

                return STD_RET_OK;
        }

        return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief Return file system status
 *
 * @param[in ]          *fs_handle              file system allocated memory
 * @param[out]          *statfs                 file system status
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
API_FS_STATFS(fatfs, void *fs_handle, struct vfs_statfs *statfs)
{
        STOP_IF(!fs_handle);
        STOP_IF(!statfs);

        struct fatfs *hdl    = fs_handle;
        u32_t  free_clusters = 0;

        struct stat fstat;
        fstat.st_size = 0;
        if (vfs_fstat(hdl->fsfile, &fstat) != 0)
                return STD_RET_ERROR;

        if (handle_error(libfat_getfree(&hdl->fatfs, &free_clusters)) == 0) {
                statfs->f_bsize  = _LIBFAT_MAX_SS;
                statfs->f_bfree  = free_clusters * hdl->fatfs.csize;
                statfs->f_blocks = fstat.st_size / _LIBFAT_MAX_SS;
                statfs->f_ffree  = 0;
                statfs->f_files  = 0;
                statfs->f_type   = hdl->fatfs.fs_type;

                if (hdl->fatfs.fs_type == LIBFAT_FS_FAT12)
                        statfs->f_fsname = "fatfs (FAT12)";
                else if (hdl->fatfs.fs_type == LIBFAT_FS_FAT16)
                        statfs->f_fsname = "fatfs (FAT16)";
                else if (hdl->fatfs.fs_type == LIBFAT_FS_FAT32)
                        statfs->f_fsname = "fatfs (FAT32)";
                else
                        statfs->f_fsname = "fatfs (FAT\?\?)";

                return STD_RET_OK;
        }

        return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief Function handle libfat errors and translate to errno
 *
 * @param fresult       libfat result
 *
 * @return 0 if no error, 1 if error
 */
//==============================================================================
static int handle_error(FRESULT fresult)
{
        int errn = 0;

        switch (fresult) {
        case FR_OK                 : errn = 0;           break;
        case FR_DISK_ERR           : errn = EIO;         break;
        case FR_INT_ERR            : errn = EAGAIN;      break;
        case FR_NOT_READY          : errn = EBUSY;       break;
        case FR_NO_FILE            : errn = ENOENT;      break;
        case FR_NO_PATH            : errn = ENOENT;      break;
        case FR_INVALID_NAME       : errn = ENOENT;      break;
        case FR_DENIED             : errn = EACCES;      break;
        case FR_EXIST              : errn = EEXIST;      break;
        case FR_INVALID_OBJECT     : errn = ENOENT;      break;
        case FR_WRITE_PROTECTED    : errn = EROFS;       break;
        case FR_INVALID_DRIVE      : errn = EMEDIUMTYPE; break;
        case FR_NOT_ENABLED        : errn = ENOSPC;      break;
        case FR_NO_FILESYSTEM      : errn = EMEDIUMTYPE; break;
        case FR_MKFS_ABORTED       : errn = ECANCELED;   break;
        case FR_TIMEOUT            : errn = ETIME;       break;
        case FR_LOCKED             : errn = ECANCELED;   break;
        case FR_NOT_ENOUGH_CORE    : errn = ENOMEM;      break;
        case FR_TOO_MANY_OPEN_FILES: errn = EMFILE;      break;
        case FR_INVALID_PARAMETER  : errn = EINVAL;      break;
        }

        if (errn) {
                errno = errn;
                return 1;
        } else {
                return 0;
        }
}

#ifdef __cplusplus
}
#endif

/*==============================================================================
  End of file
==============================================================================*/
