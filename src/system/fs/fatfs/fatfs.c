/*=========================================================================*//**
@file    fatfs.c

@author  Daniel Zorychta

@brief   FAT File system support.

@note    Copyright (C) 2013 Daniel Zorychta <daniel.zorychta@gmail.com>

         This program is free software; you can redistribute it and/or modify
         it under the terms of the GNU General Public License as published by
         the Free Software Foundation and modified by the dnx RTOS exception.

         NOTE: The modification  to the GPL is  included to allow you to
               distribute a combined work that includes dnx RTOS without
               being obliged to provide the source  code for proprietary
               components outside of the dnx RTOS.

         The dnx RTOS  is  distributed  in the hope  that  it will be useful,
         but WITHOUT  ANY  WARRANTY;  without  even  the implied  warranty of
         MERCHANTABILITY  or  FITNESS  FOR  A  PARTICULAR  PURPOSE.  See  the
         GNU General Public License for more details.

         Full license text is available on the following file: doc/license.txt.


*//*==========================================================================*/

/*==============================================================================
  Include files
==============================================================================*/
#include "fs/fs.h"
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
static int    faterr_2_errno(FRESULT fresult);
static time_t time_fat2unix(uint32_t fattime);

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
 * @param[in ]           *opts                  file system options (can be NULL)
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
API_FS_INIT(fatfs, void **fs_handle, const char *src_path, const char *opts)
{
        UNUSED_ARG1(opts);

        int err = sys_zalloc(sizeof(struct fatfs), fs_handle);
        if (err == ESUCC) {
                struct fatfs *hdl = *fs_handle;

                err = sys_fopen(src_path, "r+", &hdl->fsfile);
                if (err == ESUCC) {
                        err = faterr_2_errno(libfat_mount(hdl->fsfile, &hdl->fatfs));
                }

                if (err != ESUCC) {
                        if (hdl->fsfile)
                                sys_fclose(hdl->fsfile);

                        sys_free(fs_handle);
                }
        }

        return err;
}

//==============================================================================
/**
 * @brief Release file system
 *
 * @param[in ]          *fs_handle              file system allocated memory
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
API_FS_RELEASE(fatfs, void *fs_handle)
{
        struct fatfs *hdl = fs_handle;
        int           err = EBUSY;

        if (hdl->opened_dirs == 0 && hdl->opened_files == 0) {
                err = faterr_2_errno(libfat_umount(&hdl->fatfs));
                if (!err) {
                        sys_fclose(hdl->fsfile);
                        sys_free(&fs_handle);
                }
        }

        return err;
}

//==============================================================================
/**
 * @brief Open file
 *
 * @param[in ]          *fs_handle              file system allocated memory
 * @param[out]          *fhdl                   file handler (user defined)
 * @param[out]          *fpos                   file position
 * @param[in]           *path                   file path
 * @param[in]            flags                  file open flags
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
API_FS_OPEN(fatfs, void *fs_handle, void **fhdl, fpos_t *fpos, const char *path, u32_t flags)
{
        struct fatfs *hdl = fs_handle;

        int err = sys_zalloc(sizeof(FATFILE), fhdl);
        if (err)
                return err;

        FATFILE *fat_file = *fhdl;

        u8_t fat_mode = 0;
        if (flags == O_RDONLY) {
                fat_mode = LIBFAT_FA_READ | LIBFAT_FA_OPEN_EXISTING;
        } else if (flags == O_RDWR) {
                fat_mode = LIBFAT_FA_READ | LIBFAT_FA_WRITE | LIBFAT_FA_OPEN_EXISTING;
        } else if (flags == (O_WRONLY | O_CREAT | O_TRUNC)) {
                fat_mode = LIBFAT_FA_WRITE | LIBFAT_FA_CREATE_ALWAYS;
        } else if (flags == (O_RDWR | O_CREAT)) {
                fat_mode = LIBFAT_FA_WRITE | LIBFAT_FA_READ | LIBFAT_FA_CREATE_ALWAYS;
        } else if (flags == (O_WRONLY | O_APPEND | O_CREAT)) {
                fat_mode = LIBFAT_FA_WRITE | LIBFAT_FA_OPEN_ALWAYS;
        } else if (flags == (O_RDWR | O_APPEND | O_CREAT)) {
                fat_mode = LIBFAT_FA_WRITE | LIBFAT_FA_READ | LIBFAT_FA_OPEN_ALWAYS;
        } else {
                sys_free(fhdl);
                return EINVAL;
        }

        err = faterr_2_errno(libfat_open(&hdl->fatfs, fat_file, path, fat_mode));
        if (err) {
                sys_free(fhdl);
                return err;
        }

        if (flags & O_APPEND) {
                err = faterr_2_errno(libfat_lseek(fat_file, libfat_size(fat_file)));
                if (err) {
                        sys_free(fhdl);
                        return err;
                }
                *fpos = libfat_size(fat_file);
        } else {
                *fpos = 0;
        }

        hdl->opened_files++;

        return err;
}

//==============================================================================
/**
 * @brief Close file
 *
 * @param[in ]          *fs_handle              file system allocated memory
 * @param[in ]          *fhdl                  file handle
 * @param[in ]           force                  force close
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
API_FS_CLOSE(fatfs, void *fs_handle, void *fhdl, bool force)
{
        UNUSED_ARG1(force);

        struct fatfs *hdl = fs_handle;

        FATFILE *fatfile = fhdl;
        int err = faterr_2_errno(libfat_close(fatfile));
        if (err == ESUCC) {
                sys_free(&fhdl);
                hdl->opened_files--;
        }

        return err;
}

//==============================================================================
/**
 * @brief Write data to the file
 *
 * @param[in ]          *fs_handle              file system allocated memory
 * @param[in ]          *fhdl                  file handle
 * @param[in ]          *src                    data source
 * @param[in ]           count                  number of bytes to write
 * @param[in ]          *fpos                   position in file
 * @param[out]          *wrcnt                  number of written bytes
 * @param[in ]           fattr                  file attributes
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
API_FS_WRITE(fatfs,
             void            *fs_handle,
             void            *fhdl,
             const u8_t      *src,
             size_t           count,
             fpos_t          *fpos,
             size_t          *wrcnt,
             struct vfs_fattr fattr)
{
        UNUSED_ARG2(fs_handle, fattr);

        FATFILE *fat_file = fhdl;
        int      err      = ESUCC;

        if (libfat_tell(fat_file) != (u32_t)*fpos) {
                err = faterr_2_errno(libfat_lseek(fat_file, (u32_t)*fpos));
        }

        if (err == ESUCC) {
                uint n = 0;
                err = faterr_2_errno(libfat_write(fat_file, src, count, &n));
                if (err == ESUCC) {
                        *wrcnt = n;
                }
        }

        return err;
}

//==============================================================================
/**
 * @brief Read data from file
 *
 * @param[in ]          *fs_handle              file system allocated memory
 * @param[in ]          *fhdl                  file handle
 * @param[out]          *dst                    data destination
 * @param[in ]           count                  number of bytes to read
 * @param[in ]          *fpos                   position in file
 * @param[out]          *rdcnt                  number of read bytes
 * @param[in ]           fattr                  file attributes
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
API_FS_READ(fatfs,
            void            *fs_handle,
            void            *fhdl,
            u8_t            *dst,
            size_t           count,
            fpos_t          *fpos,
            size_t          *rdcnt,
            struct vfs_fattr fattr)
{
        UNUSED_ARG2(fs_handle, fattr);

        FATFILE *fat_file = fhdl;
        int      err      = ESUCC;

        if (libfat_tell(fat_file) != (u32_t)*fpos) {
                err = faterr_2_errno(libfat_lseek(fat_file, (u32_t)*fpos));
        }

        if (err == ESUCC) {
                uint n = 0;
                err = faterr_2_errno(libfat_read(fat_file, dst, count, &n));
                if (err == ESUCC) {
                        *rdcnt = n;
                }
        }

        return err;
}

//==============================================================================
/**
 * @brief IO operations on files
 *
 * @param[in ]          *fs_handle              file system allocated memory
 * @param[in ]          *fhdl                  file handle
 * @param[in ]           request                request
 * @param[in ][out]     *arg                    request's argument
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
API_FS_IOCTL(fatfs, void *fs_handle, void *fhdl, int request, void *arg)
{
        UNUSED_ARG1(fs_handle);
        UNUSED_ARG1(fhdl);
        UNUSED_ARG1(request);
        UNUSED_ARG1(arg);

        return ENOTSUP;
}

//==============================================================================
/**
 * @brief Flush file data
 *
 * @param[in ]          *fs_handle              file system allocated memory
 * @param[in ]          *fhdl                  file handle
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
API_FS_FLUSH(fatfs, void *fs_handle, void *fhdl)
{
        UNUSED_ARG1(fs_handle);

        FATFILE *fat_file = fhdl;
        return faterr_2_errno(libfat_flush(fat_file));
}

//==============================================================================
/**
 * @brief Return file status
 *
 * @param[in ]          *fs_handle              file system allocated memory
 * @param[in ]          *fhdl                  file handle
 * @param[out]          *stat                   file status
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
API_FS_FSTAT(fatfs, void *fs_handle, void *fhdl, struct stat *stat)
{
        UNUSED_ARG1(fs_handle);

        FATFILE *fat_file = fhdl;

        stat->st_dev   = 0;
        stat->st_gid   = 0;
        stat->st_mode  = S_IFREG | S_IRWXU | S_IRWXG | S_IRWXO;
        stat->st_mtime = time_fat2unix((fat_file->fdate << 16) | fat_file->ftime);
        stat->st_size  = fat_file->fsize;
        stat->st_uid   = 0;

        return ESUCC;
}

//==============================================================================
/**
 * @brief Create directory
 *
 * @param[in ]          *fs_handle              file system allocated memory
 * @param[in ]          *path                   name of created directory
 * @param[in ]           mode                   dir mode
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
API_FS_MKDIR(fatfs, void *fs_handle, const char *path, mode_t mode)
{
        struct fatfs *hdl = fs_handle;

        int err = faterr_2_errno(libfat_mkdir(&hdl->fatfs, path));
        if (err == ESUCC) {
                uint8_t dosmode = mode & S_IWUSR ? 0 : LIBFAT_AM_RDO;
                err = faterr_2_errno(libfat_chmod(&hdl->fatfs, path, dosmode, LIBFAT_AM_RDO));
        }

        return err;
}

//==============================================================================
/**
 * @brief Create pipe
 *
 * @param[in ]          *fs_handle              file system allocated memory
 * @param[in ]          *path                   name of created pipe
 * @param[in ]           mode                   pipe mode
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
API_FS_MKFIFO(fatfs, void *fs_handle, const char *path, mode_t mode)
{
        UNUSED_ARG1(fs_handle);
        UNUSED_ARG1(path);
        UNUSED_ARG1(mode);

        return ENOTSUP;
}

//==============================================================================
/**
 * @brief Create node for driver file
 *
 * @param[in ]          *fs_handle              file system allocated memory
 * @param[in ]          *path                   name of created node
 * @param[in ]           dev                    driver number
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
API_FS_MKNOD(fatfs, void *fs_handle, const char *path, const dev_t dev)
{
        UNUSED_ARG1(fs_handle);
        UNUSED_ARG1(path);
        UNUSED_ARG1(dev);

        return ENOTSUP;
}

//==============================================================================
/**
 * @brief Open directory
 *
 * @param[in ]          *fs_handle              file system allocated memory
 * @param[in ]          *path                   name of opened directory
 * @param[in ]          *dir                    directory object
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
API_FS_OPENDIR(fatfs, void *fs_handle, const char *path, DIR *dir)
{
        struct fatfs *hdl = fs_handle;

        size_t pathlen = strlen(path);
        char  *dospath = NULL;
        int err = sys_zalloc(pathlen + 1, cast(void*, &dospath));
        if (err == ESUCC) {

                err = sys_malloc(sizeof(struct fatdir), &dir->d_hdl);
                if (err == ESUCC) {

                        strcpy(dospath, path);
                        if (pathlen > 1) {
                                dospath[pathlen - 1] = '\0';
                        }

                        dir->d_seek     = 0;
                        dir->d_items    = 0;

                        struct fatdir *fatdir = dir->d_hdl;
                        err = faterr_2_errno(libfat_opendir(&hdl->fatfs,
                                                            &fatdir->dir,
                                                            dospath));
                        if (err == ESUCC) {
                                hdl->opened_dirs++;
                        } else {
                                sys_free(&dir->d_hdl);
                        }
                }

                sys_free(cast(void*, &dospath));
        }

        return err;
}

//==============================================================================
/**
 * @brief Function close dir
 *
 * @param[in] *fs_handle        FS handle
 * @param[in] *dir              directory info
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
API_FS_CLOSEDIR(fatfs, void *fs_handle, DIR *dir)
{
        struct fatfs *hdl = fs_handle;

        if (dir->d_hdl) {
                sys_free(&dir->d_hdl);
                hdl->opened_dirs--;
        }

        return ESUCC;
}

//==============================================================================
/**
 * @brief Read directory
 *
 * @param[in ]          *fs_handle              file system allocated memory
 * @param[in,out]       *dir                    directory object
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
API_FS_READDIR(fatfs, void *fs_handle, DIR *dir)
{
        UNUSED_ARG1(fs_handle);

        struct fatdir *fatdir = dir->d_hdl;
        fatdir->dir.index     = dir->d_seek;

        FILEINFO fat_file_info;
#if _LIBFAT_USE_LFN != 0
        fat_file_info.lfname = &fatdir->name[0];
        fat_file_info.lfsize = _LIBFAT_MAX_LFN;
#endif

        int err = faterr_2_errno(libfat_readdir(&fatdir->dir, &fat_file_info));
        if (err == ESUCC) {
                if (fat_file_info.fname[0] != 0) {
#if _LIBFAT_USE_LFN != 0
                        if (fat_file_info.lfname[0] == 0) {
                                memcpy(fatdir->name, fat_file_info.fname, 13);
                        }
#else
                        memcpy(fatdir->name, fat_file_info.fname, 13);
#endif
                        dir->dirent.d_name   = &fatdir->name[0];
                        dir->dirent.size     = fat_file_info.fsize;
                        dir->dirent.mode     = (S_IRWXU | S_IRWXG | S_IRWXO)
                                             | (fat_file_info.fattrib & LIBFAT_AM_DIR
                                               ? S_IFDIR : S_IFREG);
                        dir->d_seek          = fatdir->dir.index;
                } else {
                        err = ENOENT;
                }
        }

        return err;
}

//==============================================================================
/**
 * @brief Remove file/directory
 *
 * @param[in ]          *fs_handle              file system allocated memory
 * @param[in ]          *path                   name of removed file/directory
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
API_FS_REMOVE(fatfs, void *fs_handle, const char *path)
{
        struct fatfs *hdl = fs_handle;
        return faterr_2_errno(libfat_unlink(&hdl->fatfs, path));
}

//==============================================================================
/**
 * @brief Rename file/directory
 *
 * @param[in ]          *fs_handle              file system allocated memory
 * @param[in ]          *old_name               old object name
 * @param[in ]          *new_name               new object name
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
API_FS_RENAME(fatfs, void *fs_handle, const char *old_name, const char *new_name)
{
        struct fatfs *hdl = fs_handle;
        return faterr_2_errno(libfat_rename(&hdl->fatfs, old_name, new_name));
}

//==============================================================================
/**
 * @brief Change file's mode
 *
 * @param[in ]          *fs_handle              file system allocated memory
 * @param[in ]          *path                   file path
 * @param[in ]           mode                   new file mode
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
API_FS_CHMOD(fatfs, void *fs_handle, const char *path, mode_t mode)
{
        struct fatfs *hdl     = fs_handle;
        uint8_t       dosmode = mode & S_IWUSR ? 0 : LIBFAT_AM_RDO;
        return faterr_2_errno(libfat_chmod(&hdl->fatfs, path, dosmode, LIBFAT_AM_RDO));
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
 * @return One of errno value (errno.h)
 */
//==============================================================================
API_FS_CHOWN(fatfs, void *fs_handle, const char *path, uid_t owner, gid_t group)
{
        UNUSED_ARG1(fs_handle);
        UNUSED_ARG1(path);
        UNUSED_ARG1(owner);
        UNUSED_ARG1(group);

        return ENOTSUP;
}

//==============================================================================
/**
 * @brief Return file/dir status
 *
 * @param[in ]          *fs_handle              file system allocated memory
 * @param[in ]          *path                   file path
 * @param[out]          *stat                   file status
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
API_FS_STAT(fatfs, void *fs_handle, const char *path, struct stat *stat)
{
        struct fatfs *hdl = fs_handle;

        FILEINFO file_info;
        file_info.lfname = NULL;
        file_info.lfsize = 0;

        int err = faterr_2_errno(libfat_stat(&hdl->fatfs, path, &file_info));
        if (err == ESUCC) {
                stat->st_dev   = 0;
                stat->st_gid   = 0;
                stat->st_mtime = time_fat2unix((file_info.fdate << 16) | file_info.ftime);
                stat->st_size  = file_info.fsize;
                stat->st_uid   = 0;
                stat->st_mode  = (S_IRWXU | S_IRWXG | S_IRWXO)
                               | (file_info.fattrib & LIBFAT_AM_DIR ?
                                 S_IFDIR : S_IFREG);
        }

        return err;
}

//==============================================================================
/**
 * @brief Return file system status
 *
 * @param[in ]          *fs_handle              file system allocated memory
 * @param[out]          *statfs                 file system status
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
API_FS_STATFS(fatfs, void *fs_handle, struct statfs *statfs)
{
        struct fatfs *hdl    = fs_handle;
        u32_t  free_clusters = 0;

        memset(statfs, 0, sizeof(struct statfs));
        statfs->f_fsname = "fatfs (FAT\?\?)";
        statfs->f_type   = hdl->fatfs.fs_type;

        struct stat fstat;
        fstat.st_size = 0;

        int err = sys_fstat(hdl->fsfile, &fstat);
        if (err == ESUCC) {

                err = faterr_2_errno(libfat_getfree(&hdl->fatfs, &free_clusters));
                if (err == ESUCC) {
                        statfs->f_bsize  = _LIBFAT_MAX_SS;
                        statfs->f_bfree  = free_clusters * hdl->fatfs.csize;
                        statfs->f_blocks = fstat.st_size / _LIBFAT_MAX_SS;
                        statfs->f_ffree  = 0;
                        statfs->f_files  = 0;
                        statfs->f_type   = SYS_FS_TYPE__SOLID;

                        if (hdl->fatfs.fs_type == LIBFAT_FS_FAT12)
                                statfs->f_fsname = "fatfs (FAT12)";
                        else if (hdl->fatfs.fs_type == LIBFAT_FS_FAT16)
                                statfs->f_fsname = "fatfs (FAT16)";
                        else if (hdl->fatfs.fs_type == LIBFAT_FS_FAT32)
                                statfs->f_fsname = "fatfs (FAT32)";
                        else
                                statfs->f_fsname = "fatfs (FAT\?\?)";
                }
        }

        return err;
}

//==============================================================================
/**
 * @brief Synchronize all buffers to a medium
 *
 * @param[in ]          *fs_handle              file system allocated memory
 *
 * @return None
 */
//==============================================================================
API_FS_SYNC(fatfs, void *fs_handle)
{
        struct fatfs *hdl = fs_handle;
        return faterr_2_errno(libfat_sync(&hdl->fatfs));
}

//==============================================================================
/**
 * @brief Function handle libfat errors and translate to errno
 *
 * @param fresult       libfat result
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
static int faterr_2_errno(FRESULT fresult)
{
        static const uint8_t fat2errno[] = {
                [FR_OK                 ] = ESUCC,
                [FR_DISK_ERR           ] = EIO,
                [FR_INT_ERR            ] = EAGAIN,
                [FR_NOT_READY          ] = EBUSY,
                [FR_NO_FILE            ] = ENOENT,
                [FR_NO_PATH            ] = ENOENT,
                [FR_INVALID_NAME       ] = ENOENT,
                [FR_DENIED             ] = EACCES,
                [FR_EXIST              ] = EEXIST,
                [FR_INVALID_OBJECT     ] = EINVAL,
                [FR_WRITE_PROTECTED    ] = EROFS,
                [FR_INVALID_DRIVE      ] = EMEDIUMTYPE,
                [FR_NOT_ENABLED        ] = ENOSPC,
                [FR_NO_FILESYSTEM      ] = EMEDIUMTYPE,
                [FR_MKFS_ABORTED       ] = ECANCELED,
                [FR_TIMEOUT            ] = ETIME,
                [FR_LOCKED             ] = ECANCELED,
                [FR_NOT_ENOUGH_CORE    ] = ENOMEM,
                [FR_TOO_MANY_OPEN_FILES] = EMFILE,
                [FR_INVALID_PARAMETER  ] = EINVAL,
        };

        return (fresult >= ARRAY_SIZE(fat2errno) ? ENOTSUP : fat2errno[fresult]);
}

//==============================================================================
/**
 * @brief  ?
 * @param  ?
 * @return ?
 */
//==============================================================================
static time_t time_fat2unix(uint32_t fattime)
{
        struct tm tm;
        tm.tm_sec  = (fattime & 0x1F) << 1;
        tm.tm_min  = (fattime >> 5) & 0x3F;
        tm.tm_hour = (fattime >> 11) & 0x1F;
        tm.tm_mday = (fattime >> 16) & 0x1F;
        tm.tm_mon  = ((fattime >> 21) & 0x0F) - 1;
        tm.tm_year = ((fattime >> 25) & 0x7F) + 80;

        return sys_mktime(&tm);
}

/*==============================================================================
  End of file
==============================================================================*/
