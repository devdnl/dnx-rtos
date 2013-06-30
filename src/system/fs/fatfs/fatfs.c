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
#include "fs/fatfs.h"
#include "user/regprg.h"
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
static stdret_t fatfs_closedir(void *fshdl, dir_t *dir);
static dirent_t fatfs_readdir(void *fshdl, dir_t *dir);

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
 * @param[out] **fshdl          pointer to allocated memory by file system
 * @param[in]  *src_path        file source path
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
stdret_t fatfs_init(void **fshdl, const char *src_path)
{
        _stop_if(!fshdl);
        _stop_if(!src_path);

        struct fatfs *hdl = calloc(1, sizeof(struct fatfs));

        if (hdl) {
                *fshdl = hdl;

                if ((hdl->fsfile = fopen(src_path, "r+"))) {
                        if (libfat_mount(hdl->fsfile, &hdl->fatfs) == FR_OK) {
                                return STD_RET_OK;
                        }
                }

                /* error */
                if (hdl->fsfile) {
                        fclose(hdl->fsfile);
                }

                free(hdl);
        }

        return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief Function release file system
 *
 * @param[in] *fshdl            FS handle
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
stdret_t fatfs_release(void *fshdl)
{
        _stop_if(!fshdl);

        struct fatfs *hdl = fshdl;

        if (hdl->opened_dirs == 0 && hdl->opened_files == 0) {
                libfat_umount(&hdl->fatfs);
                fclose(hdl->fsfile);
                free(hdl);
                return STD_RET_OK;
        }

        return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief Function open selected file
 *
 * @param[in]  *fshdl           FS handle
 * @param[out] *extra           file extra data (useful in FS wrappers)
 * @param[out] *fd              file descriptor
 * @param[out] *lseek           file position
 * @param[in]  *path            file path
 * @param[in]  *mode            file mode
 *
 * @retval STD_RET_OK           file opened/created
 * @retval STD_RET_ERROR        file not opened/created
 */
//==============================================================================
stdret_t fatfs_open(void *fshdl, void **extra, fd_t *fd, u64_t *lseek, const char *path, const char *mode)
{
        (void)fd;

        _stop_if(!fshdl);
        _stop_if(!extra);
        _stop_if(!lseek);
        _stop_if(!path);
        _stop_if(!mode);

        struct fatfs *hdl = fshdl;

        FATFILE *fat_file = calloc(1, sizeof(FATFILE));
        if (!fat_file)
                return STD_RET_ERROR;

        *extra = fat_file;

        u8_t fat_mode = 0;
        if (strncmp("r",  mode, 2) == 0) {
                fat_mode = LIBFAT_FA_READ | LIBFAT_FA_OPEN_EXISTING;
        } else if (strncmp("r+", mode, 2) == 0) {
                fat_mode = LIBFAT_FA_READ | LIBFAT_FA_WRITE | LIBFAT_FA_OPEN_EXISTING;
        } else if (strncmp("w",  mode, 2) == 0) {
                fat_mode = LIBFAT_FA_WRITE | LIBFAT_FA_CREATE_ALWAYS;
        } else if (strncmp("w+", mode, 2) == 0) {
                fat_mode = LIBFAT_FA_WRITE | LIBFAT_FA_READ | LIBFAT_FA_CREATE_ALWAYS;
        } else if (strncmp("a",  mode, 2) == 0) {
                fat_mode = LIBFAT_FA_WRITE | LIBFAT_FA_OPEN_ALWAYS;
        } else if (strncmp("a+", mode, 2) == 0) {
                fat_mode = LIBFAT_FA_WRITE | LIBFAT_FA_READ | LIBFAT_FA_OPEN_ALWAYS;
        }

        if (libfat_open(&hdl->fatfs, fat_file, path, fat_mode) != FR_OK) {
                free(fat_file);
                return STD_RET_ERROR;
        }

        if (strncmp("a", mode, 2) == 0 || strncmp("a+", mode, 2) == 0) {
                libfat_lseek(fat_file, libfat_size(fat_file));
                *lseek = libfat_size(fat_file);
        } else {
                *lseek = 0;
        }

        hdl->opened_files++;

        return STD_RET_OK;
}

//==============================================================================
/**
 * @brief Function close file in LFS
 *
 * @param[in] *fshdl            FS handle
 * @param[in] *extra            file extra data (useful in FS wrappers)
 * @param[in]  fd               file descriptor
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
stdret_t fatfs_close(void *fshdl, void *extra, fd_t fd)
{
        (void)fd;

        _stop_if(!fshdl);
        _stop_if(!extra);

        struct fatfs *hdl = fshdl;

        FATFILE *fat_file = extra;
        if (libfat_close(fat_file) == FR_OK) {
                free(fat_file);
                hdl->opened_files--;
                return STD_RET_OK;
        }

        return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief Function write data to the file
 *
 * @param[in] *fshdl            FS handle
 * @param[in] *extra            file extra data (useful in FS wrappers)
 * @param[in]  fd               file descriptor
 * @param[in] *src              data source
 * @param[in]  size             item size
 * @param[in]  nitems           number of items
 * @param[in]  lseek            position in file
 *
 * @return number of written items
 */
//==============================================================================
size_t fatfs_write(void *fshdl, void *extra, fd_t fd, const void *src, size_t size, size_t nitems, u64_t lseek)
{
        (void)fshdl;
        (void)fd;

        _stop_if(!extra);
        _stop_if(!src);
        _stop_if(!size);
        _stop_if(!nitems);

        FATFILE *fat_file = extra;
        uint     n        = 0;

        if (libfat_tell(fat_file) != (u32_t)lseek) {
                libfat_lseek(fat_file, (u32_t)lseek);
        }

        libfat_write(fat_file, src, size * nitems, &n);
        return n / size;
}

//==============================================================================
/**
 * @brief Function read from file data
 *
 * @param[in]  *fshdl           FS handle
 * @param[in]  *extra           file extra data (useful in FS wrappers)
 * @param[in]   fd              file descriptor
 * @param[out] *dst             data destination
 * @param[in]   size            item size
 * @param[in]   nitems          number of items
 * @param[in]   lseek           position in file
 *
 * @return number of read items
 */
//==============================================================================
size_t fatfs_read(void *fshdl, void *extra, fd_t fd, void *dst, size_t size, size_t nitems, u64_t lseek)
{
        (void)fshdl;
        (void)fd;

        _stop_if(!extra);
        _stop_if(!dst);
        _stop_if(!size);
        _stop_if(!nitems);

        FATFILE *fat_file = extra;
        uint     n        = 0;

        if (libfat_tell(fat_file) != (u32_t)lseek) {
                libfat_lseek(fat_file, (u32_t)lseek);
        }

        libfat_read(fat_file, dst, size * nitems, &n);
        return n / size;
}

//==============================================================================
/**
 * @brief IO operations on files
 *
 * @param[in]     *fshdl        FS handle
 * @param[in]     *extra        file extra data (useful in FS wrappers)
 * @param[in]      fd           file descriptor
 * @param[in]      iorq         request
 * @param[in,out]  args         additional arguments
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
stdret_t fatfs_ioctl(void *fshdl, void *extra, fd_t fd, int iorq, va_list args)
{
        (void)fshdl;
        (void)extra;
        (void)fd;
        (void)iorq;
        (void)args;

        /* not supported by this file system */

        return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief Function flush file data
 *
 * @param[in]     *fshdl        FS handle
 * @param[in]     *extra        file extra data (useful in FS wrappers)
 * @param[in]      fd           file descriptor
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
stdret_t fatfs_flush(void *fshdl, void *extra, fd_t fd)
{
        (void)fshdl;
        (void)fd;

        _stop_if(!extra);

        FATFILE *fat_file = extra;
        if (libfat_sync(fat_file) == FR_OK)
                return STD_RET_OK;
        else
                return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief Function returns file status
 *
 * @param[in]  *fshdl                FS handle
 * @param[in]  *extra                file extra data (useful in FS wrappers)
 * @param[in]   fd                   file descriptor
 * @param[out] *stat                 pointer to status structure
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
stdret_t fatfs_fstat(void *fshdl, void *extra, fd_t fd, struct vfs_stat *stat)
{
        (void)fshdl;
        (void)fd;

        _stop_if(!extra);
        _stop_if(!stat);

        FATFILE *fat_file  = extra;
        stat->st_dev   = 0;
        stat->st_gid   = 0;
        stat->st_mode  = 0777;
        stat->st_mtime = 0;
        stat->st_size  = fat_file->fsize;
        stat->st_uid   = 0;

        return STD_RET_OK;
}

//==============================================================================
/**
 * @brief Create directory
 *
 * @param[in] *fshdl            FS handle
 * @param[in] *path             path to new directory
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
stdret_t fatfs_mkdir(void *fshdl, const char *path)
{
        struct fatfs *hdl = fshdl;

        _stop_if(!path);

        if (libfat_mkdir(&hdl->fatfs, path) == FR_OK)
                return STD_RET_OK;
        else
                return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief Function create node for driver file
 *
 * @param[in] *fshdl            FS handle
 * @param[in] *path             path when driver-file shall be created
 * @param[in] *drv_if           pointer to driver interface
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
stdret_t fatfs_mknod(void *fshdl, const char *path, struct vfs_drv_interface *drv_if)
{
        (void)fshdl;
        (void)path;
        (void)drv_if;

        /* not supported by this file system */

        return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief Function open directory
 *
 * @param[in]  *fshdl           FS handle
 * @param[in]  *path            directory path
 * @param[out] *dir             directory info
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
stdret_t fatfs_opendir(void *fshdl, const char *path, dir_t *dir)
{
        _stop_if(!fshdl);
        _stop_if(!path);
        _stop_if(!dir);

        struct fatfs *hdl = fshdl;

        char *dospath = calloc(strlen(path) + 1, 1);
        if (!dospath) {
                return STD_RET_ERROR;
        }

        dir->dd = malloc(sizeof(struct fatdir));
        if (!dir->dd) {
                free(dospath);
                return STD_RET_ERROR;
        }

        if (strlen(path) == 1) {
                strcpy(dospath, path);
        } else {
                strncpy(dospath, path, strlen(path) - 1);
        }

        dir->handle = hdl;
        dir->cldir  = fatfs_closedir;
        dir->rddir  = fatfs_readdir;
        dir->seek   = 0;
        dir->items  = 0;

        struct fatdir *fatdir = dir->dd;
        if (libfat_opendir(&hdl->fatfs, &fatdir->dir, dospath) == FR_OK) {
                free(dospath);
                hdl->opened_dirs++;
                return STD_RET_OK;
        }

        free(dospath);
        free(dir->dd);

        return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief Function close dir
 *
 * @param[in] *fshdl            FS handle
 * @param[in] *dir              directory info
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
static stdret_t fatfs_closedir(void *fshdl, dir_t *dir)
{
        _stop_if(!fshdl);
        _stop_if(!dir);

        struct fatfs *hdl = fshdl;

        if (dir->dd) {
                free(dir->dd);
                hdl->opened_dirs--;
        }

        return STD_RET_OK;
}

//==============================================================================
/**
 * @brief Function read current directory
 *
 * @param[in] *fshdl            FS handle
 * @param[in] *dir              directory info
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
static dirent_t fatfs_readdir(void *fshdl, dir_t *dir)
{
        (void) fshdl;

        _stop_if(!dir);

        struct fatdir *fatdir = dir->dd;

        dirent_t dirent;
        dirent.name = NULL;
        dirent.size = 0;

        FILEINFO fat_file_info;
#if _LIBFAT_USE_LFN != 0
        fat_file_info.lfname = &fatdir->name[0];
        fat_file_info.lfsize = _LIBFAT_MAX_LFN;
#endif
        if (libfat_readdir(&fatdir->dir, &fat_file_info) == FR_OK) {
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
 * @brief Remove file
 *
 * @param[in] *fshdl            FS handle
 * @param[in] *patch            localization of file/directory
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
stdret_t fatfs_remove(void *fshdl, const char *path)
{
        _stop_if(!fshdl);
        _stop_if(!path);

        struct fatfs *hdl = fshdl;

        if (libfat_unlink(&hdl->fatfs, path) == FR_OK)
                return STD_RET_OK;
        else
                return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief Rename file name
 *
 * @param[in] *fshdl                FS handle
 * @param[in] *oldName              old file name
 * @param[in] *newName              new file name
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
stdret_t fatfs_rename(void *fshdl, const char *old_name, const char *new_name)
{
        _stop_if(!fshdl);
        _stop_if(!old_name);
        _stop_if(!new_name);

        struct fatfs *hdl = fshdl;

        if (libfat_rename(&hdl->fatfs, old_name, new_name) == FR_OK)
                return STD_RET_OK;
        else
                return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief Function change file mode
 *
 * @param[in] *fshdl                FS handle
 * @param[in] *path                 path
 * @param[in]  mode                 file mode
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
stdret_t fatfs_chmod(void *fshdl, const char *path, int mode)
{
        _stop_if(!fshdl);
        _stop_if(!path);

        struct fatfs *hdl = fshdl;

        uint8_t dosmode = mode & OWNER_MODE(MODE_W) ? 0 : LIBFAT_AM_RDO;
        if (libfat_chmod(&hdl->fatfs, path, dosmode, LIBFAT_AM_RDO) == FR_OK) {
                return STD_RET_OK;
        }

        return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief Function change file owner and group
 *
 * @param[in] *fshdl                FS handle
 * @param[in] *path                 path
 * @param[in]  owner                file owner
 * @param[in]  group                file group
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
stdret_t fatfs_chown(void *fshdl, const char *path, int owner, int group)
{
        (void)fshdl;
        (void)path;
        (void)owner;
        (void)group;

        /* not supported by this file system */

        return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief Function returns file/dir status
 *
 * @param[in]  *fshdl                FS handle
 * @param[in]  *path                 file/dir path
 * @param[out] *stat                 pointer to stat structure
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
stdret_t fatfs_stat(void *fshdl, const char *path, struct vfs_stat *stat)
{
        _stop_if(!fshdl);
        _stop_if(!path);
        _stop_if(!stat);

        struct fatfs *hdl = fshdl;

        FILEINFO file_info;
        if (libfat_stat(&hdl->fatfs, path, &file_info) == FR_OK) {
                stat->st_dev   = 0;
                stat->st_gid   = 0;
                stat->st_mode  = 0777;
                stat->st_mtime = file_info.ftime;
                stat->st_size  = file_info.fsize;
                stat->st_uid   = 0;

                return STD_RET_OK;
        }

        return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief Function returns FS status
 *
 * @param[in]  *fshdl               FS handle
 * @param[out] *statfs              pointer to status structure
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
stdret_t fatfs_statfs(void *fshdl, struct vfs_statfs *statfs)
{
        _stop_if(!fshdl);
        _stop_if(!statfs);

        struct fatfs *hdl = fshdl;
        u32_t  free_clusters = 0;

        struct vfs_stat fstat;
        fstat.st_size = 0;
        fstat(hdl->fsfile, &fstat);

        if (libfat_getfree(&hdl->fatfs, &free_clusters) == FR_OK) {
                statfs->f_bsize  = _LIBFAT_MAX_SS;
                statfs->f_bfree  = free_clusters * hdl->fatfs.csize;
                statfs->f_blocks = fstat.st_size / _LIBFAT_MAX_SS;
                statfs->f_ffree  = 0;
                statfs->f_files  = 0;
                statfs->f_type   = hdl->fatfs.fs_type;

                if (hdl->fatfs.fs_type == LIBFAT_FS_FAT12)
                        statfs->fsname = "fatfs (FAT12)";
                else if (hdl->fatfs.fs_type == LIBFAT_FS_FAT16)
                        statfs->fsname = "fatfs (FAT16)";
                else if (hdl->fatfs.fs_type == LIBFAT_FS_FAT32)
                        statfs->fsname = "fatfs (FAT32)";
                else
                        statfs->fsname = "fatfs (FAT\?\?)";

                return STD_RET_OK;
        }

        return STD_RET_ERROR;
}

#ifdef __cplusplus
}
#endif

/*==============================================================================
  End of file
==============================================================================*/
