/*==============================================================================
File    romfs.c

Author  Daniel Zorychta

Brief   ROM File System

        Copyright (C) 2018 Daniel Zorychta <daniel.zorychta@gmail.com>

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

==============================================================================*/

/*==============================================================================
  Include files
==============================================================================*/
#include "fs/fs.h"
#include "romfs_types.h"

/*==============================================================================
  Local macros
==============================================================================*/

/*==============================================================================
  Local object types
==============================================================================*/
typedef struct {
        sem_t *openfiles;
} romfs_t;

/*==============================================================================
  Local function prototypes
==============================================================================*/
static int get_entry(const char *path, const romfs_entry_t **entry);

/*==============================================================================
  Local objects
==============================================================================*/

/*==============================================================================
  Exported objects
==============================================================================*/
extern const romfs_dir_t romfsdir_root;
extern const size_t romfs_total_size;
extern const size_t romfs_files;

/*==============================================================================
  External objects
==============================================================================*/

/*==============================================================================
  Function definitions
==============================================================================*/

//==============================================================================
/**
 * @brief Initialize file system.
 *
 * @param[out]          **fs_handle             file system allocated memory
 * @param[in ]           *src_path              file source path
 * @param[in ]           *opts                  file system options (can be NULL)
 *
 * @return One of errno value (errno.h).
 */
//==============================================================================
API_FS_INIT(romfs, void **fs_handle, const char *src_path, const char *opts)
{
        UNUSED_ARG2(src_path, opts);

        int err = sys_zalloc(sizeof(romfs_t), fs_handle);
        if (!err) {
                romfs_t *hdl = *fs_handle;

                err = sys_semaphore_create(65536, 0, &hdl->openfiles);
                if (err) {
                        sys_free(fs_handle);
                }
        }

        return err;
}

//==============================================================================
/**
 * @brief Release file system.
 *
 * @param[in ]          *fs_handle              file system allocated memory
 *
 * @return One of errno value (errno.h).
 */
//==============================================================================
API_FS_RELEASE(romfs, void *fs_handle)
{
        romfs_t *hdl = fs_handle;

        size_t openfiles = 0;

        int err = sys_semaphore_get_value(hdl->openfiles, &openfiles);
        if (!err) {
                if (openfiles == 0) {
                        sys_semaphore_destroy(hdl->openfiles);
                        hdl->openfiles = NULL;
                        err = sys_free(&fs_handle);
                } else {
                        err = EBUSY;
                }
        }

        return err;
}

//==============================================================================
/**
 * @brief Open file.
 *
 * @param[in ]          *fs_handle              file system allocated memory
 * @param[out]          *fhdl                   file extra data
 * @param[out]          *fpos                   file position
 * @param[in]           *path                   file path
 * @param[in]            flags                  file open flags
 *
 * @return One of errno value (errno.h).
 */
//==============================================================================
API_FS_OPEN(romfs, void *fs_handle, void **fhdl, fpos_t *fpos, const char *path, u32_t flags)
{
        romfs_t *hdl = fs_handle;

        if (flags != O_RDONLY) {
                return EROFS;
        }

        *fpos = 0;

        const romfs_entry_t *entry = NULL;

        int err = get_entry(path, &entry);
        if (!err) {
                if (entry->type == ROMFS_FILE_TYPE__FILE) {
                        *fhdl = cast(void*, entry);
                        sys_semaphore_signal(hdl->openfiles);
                } else {
                       err = EISDIR;
                }
        }

        return err;
}

//==============================================================================
/**
 * @brief Close file.
 *
 * @param[in ]          *fs_handle              file system allocated memory
 * @param[in ]          *fhdl                   file handle
 * @param[in ]           force                  force close
 *
 * @return One of errno value (errno.h).
 */
//==============================================================================
API_FS_CLOSE(romfs, void *fs_handle, void *fhdl, bool force)
{
        UNUSED_ARG2(fhdl, force);

        romfs_t *hdl = fs_handle;

        return sys_semaphore_wait(hdl->openfiles, 10);
}

//==============================================================================
/**
 * @brief Write data to the file.
 *
 * @param[in ]          *fs_handle              file system allocated memory
 * @param[in ]          *fhdl                   file handle
 * @param[in ]          *src                    data source
 * @param[in ]           count                  number of bytes to write
 * @param[in ]          *fpos                   position in file
 * @param[out]          *wrcnt                  number of written bytes
 * @param[in ]           fattr                  file attributes
 *
 * @return One of errno value (errno.h).
 */
//==============================================================================
API_FS_WRITE(romfs,
             void            *fs_handle,
             void            *fhdl,
             const u8_t      *src,
             size_t           count,
             fpos_t          *fpos,
             size_t          *wrcnt,
             struct vfs_fattr fattr)
{
        UNUSED_ARG7(fs_handle, fhdl, src, count, fpos, wrcnt, fattr);
        return EROFS;
}

//==============================================================================
/**
 * @brief Read data from file.
 *
 * @param[in ]          *fs_handle              file system allocated memory
 * @param[in ]          *fhdl                   file handle
 * @param[out]          *dst                    data destination
 * @param[in ]           count                  number of bytes to read
 * @param[in ]          *fpos                   position in file
 * @param[out]          *rdcnt                  number of read bytes
 * @param[in ]           fattr                  file attributes
 *
 * @return One of errno value (errno.h).
 */
//==============================================================================
API_FS_READ(romfs,
            void            *fs_handle,
            void            *fhdl,
            u8_t            *dst,
            size_t           count,
            fpos_t          *fpos,
            size_t          *rdcnt,
            struct vfs_fattr fattr)
{
        UNUSED_ARG2(fs_handle, fattr);

        int err = EFAULT;

        romfs_entry_t *entry = fhdl;
        if (entry) {
                i32_t len = ((entry->size) ? *entry->size : 0) - *fpos;
                      len = min((i32_t)count, len);

                if (len > 0) {
                        memcpy(dst, entry->data + *fpos, len);
                        *rdcnt = len;
                } else {
                        *rdcnt = 0;
                }

                err = ESUCC;
        }

        return err;
}

//==============================================================================
/**
 * @brief IO operations on files.
 *
 * @param[in ]          *fs_handle              file system allocated memory
 * @param[in ]          *fhdl                   file handle
 * @param[in ]           request                request
 * @param[in ][out]     *arg                    request's argument
 *
 * @return One of errno value (errno.h).
 */
//==============================================================================
API_FS_IOCTL(romfs, void *fs_handle, void *fhdl, int request, void *arg)
{
        UNUSED_ARG4(fs_handle, fhdl, request, arg);
        return ENOTSUP;
}

//==============================================================================
/**
 * @brief Flush file data.
 *
 * @param[in ]          *fs_handle              file system allocated memory
 * @param[in ]          *fhdl                   file handle
 *
 * @return One of errno value (errno.h).
 */
//==============================================================================
API_FS_FLUSH(romfs, void *fs_handle, void *fhdl)
{
        UNUSED_ARG2(fs_handle, fhdl);
        return ESUCC;
}

//==============================================================================
/**
 * @brief Return file status.
 *
 * @param[in ]          *fs_handle              file system allocated memory
 * @param[in ]          *fhdl                   file handle
 * @param[out]          *stat                   file status
 *
 * @return One of errno value (errno.h).
 */
//==============================================================================
API_FS_FSTAT(romfs, void *fs_handle, void *fhdl, struct stat *stat)
{
        UNUSED_ARG1(fs_handle);

        int err = EIO;

        romfs_entry_t *entry = fhdl;
        if (entry) {
                stat->st_ctime = COMPILE_EPOCH_TIME;
                stat->st_mtime = COMPILE_EPOCH_TIME;
                stat->st_dev   = 0;
                stat->st_size  = entry->size ? *entry->size : 0;
                stat->st_gid   = 0;
                stat->st_uid   = 0;
                stat->st_mode  = S_IRUSR | S_IRGRP | S_IROTH
                               | ( entry->type == ROMFS_FILE_TYPE__FILE
                                 ? (S_IXUSR * __ROMFS_CFG_EXEC_FILES__) : 0 )
                               | (entry->type == ROMFS_FILE_TYPE__DIR
                                 ? S_IFDIR : S_IFREG);
                err = ESUCC;
        }

        return err;
}

//==============================================================================
/**
 * @brief Return file/dir status.
 *
 * @param[in ]          *fs_handle              file system allocated memory
 * @param[in ]          *path                   file path
 * @param[out]          *stat                   file status
 *
 * @return One of errno value (errno.h).
 */
//==============================================================================
API_FS_STAT(romfs, void *fs_handle, const char *path, struct stat *stat)
{
        const romfs_entry_t *entry = NULL;

        int err = get_entry(path, &entry);
        if (!err) {
                err = _romfs_fstat(fs_handle, const_cast(void*, entry), stat);
        }

        return err;
}

//==============================================================================
/**
 * @brief Return file system status.
 *
 * @param[in ]          *fs_handle              file system allocated memory
 * @param[out]          *statfs                 file system status
 *
 * @return One of errno value (errno.h).
 */
//==============================================================================
API_FS_STATFS(romfs, void *fs_handle, struct statfs *statfs)
{
        UNUSED_ARG1(fs_handle);

        statfs->f_bsize  = 1;
        statfs->f_blocks = romfs_total_size;
        statfs->f_bfree  = 0;
        statfs->f_ffree  = 0;
        statfs->f_files  = romfs_files;
        statfs->f_type   = SYS_FS_TYPE__SOLID;
        statfs->f_fsname = "romfs";

        return ESUCC;
}

//==============================================================================
/**
 * @brief Create directory.
 *
 * @param[in ]          *fs_handle              file system allocated memory
 * @param[in ]          *path                   name of created directory
 * @param[in ]           mode                   dir mode
 *
 * @return One of errno value (errno.h).
 */
//==============================================================================
API_FS_MKDIR(romfs, void *fs_handle, const char *path, mode_t mode)
{
        UNUSED_ARG3(fs_handle, path, mode);
        return EROFS;
}

//==============================================================================
/**
 * @brief Create pipe.
 *
 * @param[in ]          *fs_handle              file system allocated memory
 * @param[in ]          *path                   name of created pipe
 * @param[in ]           mode                   pipe mode
 *
 * @return One of errno value (errno.h).
 */
//==============================================================================
API_FS_MKFIFO(romfs, void *fs_handle, const char *path, mode_t mode)
{
        UNUSED_ARG3(fs_handle, path, mode);
        return EROFS;
}

//==============================================================================
/**
 * @brief Create node for driver file.
 *
 * @param[in ]          *fs_handle              file system allocated memory
 * @param[in ]          *path                   name of created node
 * @param[in ]           dev                    driver number
 *
 * @return One of errno value (errno.h).
 */
//==============================================================================
API_FS_MKNOD(romfs, void *fs_handle, const char *path, const dev_t dev)
{
        UNUSED_ARG3(fs_handle, path, dev);
        return EROFS;
}

//==============================================================================
/**
 * @brief Open directory.
 *
 * @param[in ]          *fs_handle              file system allocated memory
 * @param[in ]          *path                   name of opened directory
 * @param[in ]          *dir                    directory object
 *
 * @return One of errno value (errno.h).
 */
//==============================================================================
API_FS_OPENDIR(romfs, void *fs_handle, const char *path, DIR *dir)
{
        romfs_t *hdl = fs_handle;

        const romfs_entry_t *entry = NULL;

        int err = get_entry(path, &entry);
        if (!err) {
                if (entry->type == ROMFS_FILE_TYPE__DIR) {
                        dir->d_hdl   = const_cast(void*, entry->data);
                        dir->d_items = cast(romfs_dir_t*, entry->data)->items;
                        dir->d_seek  = 0;

                        sys_semaphore_signal(hdl->openfiles);
                } else {
                        err = ENOTDIR;
                }
        }

        return err;
}

//==============================================================================
/**
 * @brief Close directory.
 *
 * @param[in ]          *fs_handle              file system allocated memory
 * @param[in ]          *dir                    directory object
 *
 * @return One of errno value (errno.h).
 */
//==============================================================================
API_FS_CLOSEDIR(romfs, void *fs_handle, DIR *dir)
{
        UNUSED_ARG1(dir);

        romfs_t *hdl = fs_handle;

        return sys_semaphore_wait(hdl->openfiles, 10);
}

//==============================================================================
/**
 * @brief Read directory.
 *
 * @param[in ]          *fs_handle              file system allocated memory
 * @param[in,out]       *dir                    directory object
 *
 * @return One of errno value (errno.h).
 */
//==============================================================================
API_FS_READDIR(romfs, void *fs_handle, DIR *dir)
{
        UNUSED_ARG1(fs_handle);

        int err = EFAULT;

        const romfs_dir_t *d = dir->d_hdl;

        if (d && (dir->d_seek < d->items)) {
                dir->dirent.dev    = 0;
                dir->dirent.d_name = d->entry[dir->d_seek].name;
                dir->dirent.size   = d->entry[dir->d_seek].size
                                   ? *d->entry[dir->d_seek].size : 0;
                dir->dirent.mode   = S_IRUSR | S_IRGRP | S_IROTH
                                 | (d->entry[dir->d_seek].type == ROMFS_FILE_TYPE__DIR
                                   ? S_IFDIR : S_IFREG);
                dir->d_seek++;

                err = ESUCC;

        } else {
                err = ENOENT;
        }

        return err;
}

//==============================================================================
/**
 * @brief Remove file/directory.
 *
 * @param[in ]          *fs_handle              file system allocated memory
 * @param[in ]          *path                   name of removed file/directory
 *
 * @return One of errno value (errno.h).
 */
//==============================================================================
API_FS_REMOVE(romfs, void *fs_handle, const char *path)
{
        UNUSED_ARG2(fs_handle, path);
        return EROFS;
}

//==============================================================================
/**
 * @brief Rename file/directory.
 *
 * @param[in ]          *fs_handle              file system allocated memory
 * @param[in ]          *old_name               old object name
 * @param[in ]          *new_name               new object name
 *
 * @return One of errno value (errno.h).
 */
//==============================================================================
API_FS_RENAME(romfs, void *fs_handle, const char *old_name, const char *new_name)
{
        UNUSED_ARG3(fs_handle, old_name, new_name);
        return EROFS;
}

//==============================================================================
/**
 * @brief Change file's mode.
 *
 * @param[in ]          *fs_handle              file system allocated memory
 * @param[in ]          *path                   file path
 * @param[in ]           mode                   new file mode
 *
 * @return One of errno value (errno.h).
 */
//==============================================================================
API_FS_CHMOD(romfs, void *fs_handle, const char *path, mode_t mode)
{
        UNUSED_ARG3(fs_handle, path, mode);
        return EROFS;
}

//==============================================================================
/**
 * @brief Change file's owner and group.
 *
 * @param[in ]          *fs_handle              file system allocated memory
 * @param[in ]          *path                   file path
 * @param[in ]           owner                  new file owner
 * @param[in ]           group                  new file group
 *
 * @return One of errno value (errno.h).
 */
//==============================================================================
API_FS_CHOWN(romfs, void *fs_handle, const char *path, uid_t owner, gid_t group)
{
        UNUSED_ARG4(fs_handle, path, owner, group);
        return EROFS;
}

//==============================================================================
/**
 * @brief Synchronize all buffers to a medium.
 *
 * @param[in ]          *fs_handle              file system allocated memory
 *
 * @return One of errno value (errno.h).
 */
//==============================================================================
API_FS_SYNC(romfs, void *fs_handle)
{
        UNUSED_ARG1(fs_handle);
        return ESUCC;
}

//==============================================================================
/**
 * @brief Search entry by path.
 *
 * @param  path         path
 * @param  entry        path related entry
 *
 * @return One of errno value.
 */
//==============================================================================
static int get_entry(const char *path, const romfs_entry_t **entry)
{
        static const romfs_entry_t root = {
                .type = ROMFS_FILE_TYPE__DIR,
                .size = NULL,
                .data = &romfsdir_root,
                .name = "/"
        };

        int err = ENOENT;

        if (strcmp(path, "/") == 0) {
                *entry = &root;
                err    = ESUCC;

        } else {
                path++;

                const romfs_dir_t   *dir = &romfsdir_root;
                const romfs_entry_t *ent = NULL;
                bool               found = false;

                do {
                        const char *end = strchr(path, '/');
                        size_t nlen = (end == NULL)
                                    ? strlen(path)
                                    : ((uintptr_t)end - (uintptr_t)path);

                        if (nlen == 0) break;

                        found = false;

                        for (size_t i = 0; i < dir->items; i++) {

                                ent = &dir->entry[i];

                                if (  (strlen(ent->name) == nlen)
                                   && (strncmp(ent->name, path, nlen) == 0) ) {

                                        found = true;
                                        path += nlen;

                                        if ((*path != '\0') && strcmp(path, "/")) {
                                                path++;

                                                if (ent->type == ROMFS_FILE_TYPE__DIR) {
                                                        dir = ent->data;

                                                } else {
                                                        err = ENOTDIR;
                                                        goto finish;
                                                }
                                        } else {
                                                err = ESUCC;
                                                goto finish;
                                        }

                                        break;
                                }
                        }

                } while (*path && dir && found);

                finish:
                if (!err) {
                        *entry = ent;
                }
        }

        return err;
}

/*==============================================================================
  End of file
==============================================================================*/
