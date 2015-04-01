/*=========================================================================*//**
@file    vfs.c

@author  Daniel Zorychta

@brief   This file support virtual file system

@note    Copyright (C) 2012-2015 Daniel Zorychta <daniel.zorychta@gmail.com>

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

/*==============================================================================
  Include files
==============================================================================*/
#include <dnx/misc.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include "fs/vfs.h"
#include "lib/llist.h"
#include "kernel/kwrapper.h"

/*==============================================================================
  Local symbolic constants/macros
==============================================================================*/
#undef errno
#define PATH_MAX_LEN             256

/*==============================================================================
  Local types, enums definitions
==============================================================================*/
typedef struct FS_entry {
        const char         *mount_point;
        struct FS_entry    *parent;
        void               *handle;
        const vfs_FS_itf_t *interface;
        u8_t                children_cnt;
} FS_entry_t;

enum path_correction {
        ADD_SLASH,
        SUB_SLASH,
        NO_SLASH_ACTION,
};

/*==============================================================================
  Local function prototypes
==============================================================================*/
static bool         is_first_fs             (const char *mount_point);
static int          new_FS_entry            (FS_entry_t *parent_FS, const char *fs_mount_point, const char *fs_src_file, const vfs_FS_itf_t *fs_interface, FS_entry_t **fs_entry);
static int          delete_FS_entry         (FS_entry_t *this);
static bool         is_file_valid           (FILE *file);
static bool         is_dir_valid            (DIR *dir);
static int          increase_task_priority  (void);
static inline void  restore_priority        (int priority);
static bool         parse_flags             (const char *str, u32_t *flags);
static int          get_path_FS             (const char *path, size_t len, int *position, FS_entry_t **fs_entry);
static int          get_path_base_FS        (const char *path, const char **extPath, FS_entry_t **fs_entry);
static int          new_CWD_path            (const char *path, enum path_correction corr, char **new_path);

/*==============================================================================
  Local object definitions
==============================================================================*/
static llist_t  *vfs_mnt_list;
static mutex_t  *vfs_resource_mtx;

/*==============================================================================
  Function definitions
==============================================================================*/

//==============================================================================
/**
 * @brief Initialize VFS module
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
bool _vfs_init(void)
{
        vfs_mnt_list     = _llist_new(_sysmalloc, _sysfree, NULL, NULL);
        vfs_resource_mtx = _mutex_new(MUTEX_TYPE_RECURSIVE);

        return vfs_mnt_list && vfs_resource_mtx;
}

//==============================================================================
/**
 * @brief Function mount file system in VFS
 *
 * @param[in] *src_path         path to source file when file system load data
 * @param[in] *mount_point      path when dir shall be mounted
 * @param[in] *fsif             pointer to file system interface
 *
 * @return One of errno values
 */
//==============================================================================
int _vfs_mount(const char *src_path, const char *mount_point, struct vfs_FS_itf *fsif)
{
        if (  !mount_point
           || !src_path
           || !fsif->fs_init
           || !fsif->fs_release
           || !fsif->fs_open
           || !fsif->fs_close
           || !fsif->fs_write
           || !fsif->fs_read
           || !fsif->fs_ioctl
           || !fsif->fs_fstat
           || !fsif->fs_flush
           || !fsif->fs_mkdir
           || !fsif->fs_mkfifo
           || !fsif->fs_mknod
           || !fsif->fs_opendir
           || !fsif->fs_remove
           || !fsif->fs_rename
           || !fsif->fs_chmod
           || !fsif->fs_chown
           || !fsif->fs_stat
           || !fsif->fs_statfs
           || !fsif->fs_sync) {

                return EINVAL;
        }

        // create new paths that are corrected by CWD
        char *cwd_mount_point;
        int result = new_CWD_path(mount_point, ADD_SLASH, &cwd_mount_point);
        if (result != ESUCC) {
                return result;
        }

        char *cwd_src_path;
        result = new_CWD_path(src_path, SUB_SLASH, &cwd_src_path);
        if (result != ESUCC) {
                return result;
        }

        // create new entry
        if (_mutex_lock(vfs_resource_mtx, MAX_DELAY_MS)) {

                FS_entry_t *new_fs;

                /*
                 * create new FS in existing DIR and FS, otherwise create new FS if
                 * first mount
                 */
                if (is_first_fs(cwd_mount_point)) {
                        result = new_FS_entry(NULL, cwd_mount_point, cwd_src_path, fsif, &new_fs);
                        if (result != ESUCC) {
                                goto finish;
                        }
                } else {
                        const char *ext_path;
                        FS_entry_t *base_fs;
                        FS_entry_t *mounted_fs;

                        result = get_path_base_FS(cwd_mount_point, &ext_path, &base_fs);
                        if (result != ESUCC) {
                                goto finish;
                        }

                        result = get_path_FS(cwd_mount_point, PATH_MAX_LEN, NULL, &mounted_fs);
                        if (result == ENOENT) {
                                DIR dir;
                                result = base_fs->interface->fs_opendir(base_fs->handle, ext_path, &dir);
                                if (result == ESUCC) {
                                        base_fs->children_cnt++;

                                        result = dir.f_closedir(dir.f_handle, &dir);
                                        if (result != ESUCC) {
                                                goto finish;
                                        }

                                        result = new_FS_entry(NULL, cwd_mount_point, cwd_src_path, fsif, &new_fs);
                                        if (result != ESUCC) {
                                                goto finish;
                                        }
                                }
                        } else {
                                result = EADDRINUSE;
                        }
                }

                /*
                 * mount FS if created
                 */
                if (result == ESUCC) {
                        if (!_llist_push_back(vfs_mnt_list, new_fs)) {
                                delete_FS_entry(new_fs);
                                result = ENOMEM;
                        }
                }

                finish:
                _mutex_unlock(vfs_resource_mtx);
        }

        if (result != ESUCC && cwd_mount_point)
                _sysfree(cwd_mount_point);

        if (cwd_src_path)
                _sysfree(cwd_src_path);

        return result;
}

//==============================================================================
/**
 * @brief Function unmount file system
 *
 * @param[in] *path             dir path
 *
 * @return One of errno values
 */
//==============================================================================
int _vfs_umount(const char *path)
{
        if (!path) {
                return EINVAL;
        }

        char *cwd_path;
        int result = new_CWD_path(path, ADD_SLASH, &cwd_path);
        if (result == ESUCC) {
                if (_mutex_lock(vfs_resource_mtx, MAX_DELAY_MS)) {

                        int position; FS_entry_t *mount_fs;
                        result = get_path_FS(cwd_path, PATH_MAX_LEN, &position, &mount_fs);

                        _sysfree(cwd_path);

                        if (result == ESUCC) {
                                if (mount_fs->children_cnt == 0) {
                                        result = delete_FS_entry(mount_fs);
                                        if (result == ESUCC) {
                                                _llist_take(vfs_mnt_list, position);
                                        }
                                } else {
                                        result = EBUSY;
                                }
                        }

                        _mutex_unlock(vfs_resource_mtx);
                }
        }

        return result;
}

//==============================================================================
/**
 * @brief Function return file system describe object
 *
 * @param [in]  seek          item to read
 * @param [out] mntent        pointer to mntent object
 *
 * @return One of errno values
 */
//==============================================================================
int _vfs_getmntentry(int seek, struct mntent *mntent)
{
        if (!mntent) {
                return EINVAL;
        }

        int result = ENOENT;

        FS_entry_t *fs = NULL;
        if (_mutex_lock(vfs_resource_mtx, MAX_DELAY_MS)) {
                fs = _llist_at(vfs_mnt_list, seek);
                _mutex_unlock(vfs_resource_mtx);
        }

        if (fs) {
                int priority = increase_task_priority();

                struct statfs stat_fs;
                result = fs->interface->fs_statfs(fs->handle, &stat_fs);
                if (result == ESUCC) {
                        mntent->mnt_fsname = stat_fs.f_fsname;
                        mntent->mnt_dir    = fs->mount_point;
                        mntent->free       = (u64_t)stat_fs.f_bfree  * stat_fs.f_bsize;
                        mntent->total      = (u64_t)stat_fs.f_blocks * stat_fs.f_bsize;
                }

                restore_priority(priority);
        }

        return result;
}

//==============================================================================
/**
 * @brief Function create node for driver file
 *
 * @param[in] path              path when driver-file shall be created
 * @param[in] dev               device number
 *
 * @return One of errno values
 */
//==============================================================================
int _vfs_mknod(const char *path, dev_t dev)
{
        if (!path || dev < 0) {
                return EINVAL;
        }

        char *cwd_path;
        int result = new_CWD_path(path, NO_SLASH_ACTION, &cwd_path);
        if (result == ESUCC) {

                const char *external_path; FS_entry_t *fs;
                result = get_path_base_FS(cwd_path, &external_path, &fs);
                if (result == ESUCC) {

                        int priority = increase_task_priority();
                        result = fs->interface->fs_mknod(fs->handle, external_path, dev);
                        restore_priority(priority);
                }

                _sysfree(cwd_path);
        }

        return result;
}

//==============================================================================
/**
 * @brief Create directory
 *
 * @param[in] path              path to new directory
 * @param[in] mode              directory mode
 *
 * @return One of errno values
 */
//==============================================================================
int _vfs_mkdir(const char *path, mode_t mode)
{
        if (!path) {
                return EINVAL;
        }

        char *cwd_path;
        int result = new_CWD_path(path, SUB_SLASH, &cwd_path);
        if (result == ESUCC) {

                const char *external_path; FS_entry_t *fs;
                result = get_path_base_FS(cwd_path, &external_path, &fs);
                if (result == ESUCC) {

                        int priority = increase_task_priority();
                        result = fs->interface->fs_mkdir(fs->handle, external_path, mode);
                        restore_priority(priority);
                }

                _sysfree(cwd_path);
        }

        return result;
}

//==============================================================================
/**
 * @brief Create pipe
 *
 * @param[in] path              path to pipe
 * @param[in] mode              directory mode
 *
 * @return One of errno values
 */
//==============================================================================
int _vfs_mkfifo(const char *path, mode_t mode)
{
        if (!path) {
                return EINVAL;
        }

        char *cwd_path;
        int result = new_CWD_path(path, NO_SLASH_ACTION, &cwd_path);
        if (result == ESUCC) {

                const char *external_path; FS_entry_t *fs;
                result = get_path_base_FS(cwd_path, &external_path, &fs);
                if (result == ESUCC) {

                        int priority = increase_task_priority();
                        result = fs->interface->fs_mkfifo(fs->handle, external_path, mode);
                        restore_priority(priority);
                }

                _sysfree(cwd_path);
        }

        return result;
}

//==============================================================================
/**
 * @brief Function open directory
 *
 * @param[in]  path                 directory path
 * @param[out] dir                  pointer to dir pointer
 *
 * @return One of errno values
 */
//==============================================================================
int _vfs_opendir(const char *path, DIR **dir)
{
        if (!path || !dir) {
                return EINVAL;
        }

        int result = ENOMEM;

        *dir = _sysmalloc(sizeof(DIR));
        if (*dir) {
                char *cwd_path;
                result = new_CWD_path(path, ADD_SLASH, &cwd_path);
                if (result == ESUCC) {

                        const char *external_path; FS_entry_t *fs;
                        result = get_path_base_FS(cwd_path, &external_path, &fs);
                        _sysfree(cwd_path);

                        if (result == ESUCC) {
                                (*dir)->f_handle = fs->handle;

                                int priority = increase_task_priority();
                                result = fs->interface->fs_opendir(fs->handle, external_path, *dir);
                                restore_priority(priority);
                        }
                }

                if (result == ESUCC) {
                        (*dir)->self = *dir;
                } else {
                        _sysfree(dir);
                        *dir = NULL;
                }
        }

        return result;
}

//==============================================================================
/**
 * @brief Function close opened directory
 *
 * @param[in] *dir                  directory object
 *
 * @return One of errno values
 */
//==============================================================================
int _vfs_closedir(DIR *dir)
{
        int result = EINVAL;

        if (is_dir_valid(dir) && dir->f_closedir) {
                result = dir->f_closedir(dir->f_handle, dir);
                if (result == ESUCC) {
                        dir->self = NULL;
                        _sysfree(dir);
                }
        }

        return result;
}

//==============================================================================
/**
 * @brief Function read next item of opened directory
 *
 * @param[in]  *dir                  directory object
 * @param[out] **dirent              pointer to direntry pointer
 *
 * @return One of errno values
 */
//==============================================================================
int _vfs_readdir(DIR *dir, dirent_t **dirent)
{
        int result = EINVAL;

        if (is_dir_valid(dir) && dirent && dir->f_readdir) {
                int priority = increase_task_priority();
                result = dir->f_readdir(dir->f_handle, dir, dirent);
                restore_priority(priority);
        }

        return result;
}

//==============================================================================
/**
 * @brief Remove file
 * Removes file or directory. Removes directory if is not a mount point.
 *
 * @param[in] *path                localization of file/directory
 *
 * @return One of errno values
 */
//==============================================================================
int _vfs_remove(const char *path)
{
        if (!path) {
                return EINVAL;
        }

        char *cwd_path;
        int result = new_CWD_path(path, ADD_SLASH, &cwd_path);
        if (result == ESUCC) {

                const char *external_path;
                FS_entry_t *mount_fs;
                FS_entry_t *base_fs;
                if (_mutex_lock(vfs_resource_mtx, MAX_DELAY_MS)) {

                        result = get_path_FS(cwd_path, PATH_MAX_LEN, NULL, &mount_fs);
                        if (result == ENOENT) {
                                // remove slash at the end
                                LAST_CHARACTER(cwd_path) = '\0';

                                // get parent FS
                                result = get_path_base_FS(cwd_path, &external_path, &base_fs);
                        } else {
                                result = EBUSY;
                        }

                        _mutex_unlock(vfs_resource_mtx);
                }

                if (result == ESUCC) {
                        result = base_fs->interface->fs_remove(base_fs->handle, external_path);
                }

                _sysfree(cwd_path);
        }

        return result;
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
 * @return One of errno values
 */
//==============================================================================
int _vfs_rename(const char *old_name, const char *new_name)
{
        if (!old_name || !new_name) {
                return EINVAL;
        }

        char *cwd_old_name;
        int result = new_CWD_path(old_name, NO_SLASH_ACTION, &cwd_old_name);
        if (result != ESUCC) {
                return result;
        }

        char *cwd_new_name;
        result = new_CWD_path(new_name, NO_SLASH_ACTION, &cwd_new_name);
        if (result != ESUCC) {
                return result;
        }


        FS_entry_t *old_fs;
        FS_entry_t *new_fs;
        const char *old_extern_path;
        const char *new_extern_path;

        if (_mutex_lock(vfs_resource_mtx, MAX_DELAY_MS)) {
                result = get_path_base_FS(cwd_old_name, &old_extern_path, &old_fs);
                if (result == ESUCC) {
                        result = get_path_base_FS(cwd_new_name, &new_extern_path, &new_fs);
                }
                _mutex_unlock(vfs_resource_mtx);
        }

        if (result == ESUCC) {

                if (old_fs == new_fs) {
                        int priority = increase_task_priority();

                        result = old_fs->interface->fs_rename(old_fs->handle,
                                                              old_extern_path,
                                                              new_extern_path);
                        restore_priority(priority);
                } else {
                        result = ENOTSUP;
                }
        }

        if (cwd_old_name) {
                _sysfree(cwd_old_name);
        }

        if (cwd_new_name) {
                _sysfree(cwd_new_name);
        }

        return result;
}

//==============================================================================
/**
 * @brief Function change file mode
 *
 * @param[in] *path         file path
 * @param[in]  mode         file mode
 *
 * @return One of errno values
 */
//==============================================================================
int _vfs_chmod(const char *path, mode_t mode)
{
        if (!path) {
                return EINVAL;
        }

        char *cwd_path;
        int result = new_CWD_path(path, NO_SLASH_ACTION, &cwd_path);
        if (result == ESUCC) {

                const char *external_path; FS_entry_t *fs;
                result = get_path_base_FS(cwd_path, &external_path, &fs);
                if (result == ESUCC) {

                        int priority = increase_task_priority();
                        result = fs->interface->fs_chmod(fs->handle, external_path, mode);
                        restore_priority(priority);
                }

                _sysfree(cwd_path);
        }

        return result;
}

//==============================================================================
/**
 * @brief Function change file owner and group
 *
 * @param[in] *path         file path
 * @param[in]  owner        file owner
 * @param[in]  group        file group
 *
 * @return One of errno values
 */
//==============================================================================
int _vfs_chown(const char *path, uid_t owner, gid_t group)
{
        if (!path) {
                return EINVAL;
        }

        char *cwd_path;
        int result = new_CWD_path(path, NO_SLASH_ACTION, &cwd_path);
        if (result == ESUCC) {

                const char *external_path; FS_entry_t *fs;
                result = get_path_base_FS(cwd_path, &external_path, &fs);
                if (result == ESUCC) {

                        int priority = increase_task_priority();
                        result = fs->interface->fs_chown(fs->handle, external_path, owner, group);
                        restore_priority(priority);
                }

                _sysfree(cwd_path);
        }

        return result;
}

//==============================================================================
/**
 * @brief Function returns file/dir status
 *
 * @param[in]  *path            file/dir path
 * @param[out] *stat            pointer to structure
 *
 * @return One of errno values
 */
//==============================================================================
int _vfs_stat(const char *path, struct stat *stat)
{
        if (!path || !stat) {
                return EINVAL;
        }

        char *cwd_path;
        int result = new_CWD_path(path, NO_SLASH_ACTION, &cwd_path);
        if (result == ESUCC) {

                const char *external_path; FS_entry_t *fs;
                result = get_path_base_FS(cwd_path, &external_path, &fs);
                if (result == ESUCC) {

                        int priority = increase_task_priority();
                        result = fs->interface->fs_stat(fs->handle, external_path, stat);
                        restore_priority(priority);
                }

                _sysfree(cwd_path);
        }

        return result;
}

//==============================================================================
/**
 * @brief Function returns file system status
 *
 * @param[in]  *path            fs path
 * @param[out] *statfs          pointer to FS status structure
 *
 * @return One of errno values
 */
//==============================================================================
int _vfs_statfs(const char *path, struct statfs *statfs)
{
        if (!path || !statfs) {
                return EINVAL;
        }

        char *cwd_path;
        int result = new_CWD_path(path, ADD_SLASH, &cwd_path);
        if (result == ESUCC) {

                const char *external_path; FS_entry_t *fs;
                result = get_path_base_FS(cwd_path, &external_path, &fs);
                if (result == ESUCC) {

                        int priority = increase_task_priority();
                        result = fs->interface->fs_statfs(fs->handle, statfs);
                        restore_priority(priority);
                }
        }

        return result;
}

//==============================================================================
/**
 * @brief Function open selected file
 *
 * @param[in]  *name             file path
 * @param[in]  *mode             file mode
 * @param[out] **file            pointer to file pointer
 *
 * @return One of errno values
 */
//==============================================================================
int _vfs_fopen(const char *path, const char *mode, FILE **file)
{
        if (!path || !mode || !file) {
                return EINVAL;
        }

        if (LAST_CHARACTER(path) == '/') { /* path is a directory */
                return EISDIR;
        }

        u32_t flags;
        if (parse_flags(mode, &flags) == false) {
                return EINVAL;
        }

        vfs_file_flags_t fflags;
        memset(&flags, 0, sizeof(vfs_file_flags_t));
        fflags.rd     = ((flags == O_RDONLY) || (flags & O_RDWR));
        fflags.wr     = (flags & (O_RDWR | O_WRONLY));
        fflags.append = (flags & (O_APPEND));

        char *cwd_path = NULL;
        int result = new_CWD_path(path, NO_SLASH_ACTION, &cwd_path);
        if (result == ESUCC) {
                return result;
        }

        FILE *file_obj = _syscalloc(1, sizeof(FILE));

        if (result == ESUCC && file_obj) {
                const char *external_path; FS_entry_t *fs;
                result = get_path_base_FS(cwd_path, &external_path, &fs);
                if (result == ESUCC) {

                        int priority = increase_task_priority();

                        result = fs->interface->fs_open(fs->handle,
                                                        &file_obj->f_extra_data,
                                                        &file_obj->fd,
                                                        &file_obj->f_lseek,
                                                        external_path,
                                                        flags);
                        if (result == ESUCC) {
                                file_obj->FS_hdl = fs->handle;
                                file_obj->FS_if  = fs->interface;
                                file_obj->f_flag = fflags;
                                file_obj->self   = file_obj;
                        }

                        restore_priority(priority);
                }
        }

        if (cwd_path) {
                _sysfree(cwd_path);
        }

        if (file_obj) {
                if (file_obj->self) {
                        *file = file_obj;
                } else {
                        _sysfree(file_obj);
                }
        }

        return result;
}

//==============================================================================
/**
 * @brief Function close opened file
 *
 * @param[in] file              pinter to file
 * @param[in] force             force close
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
int _vfs_fclose(FILE *file, bool force)
{
        int result = EINVAL;

        if (is_file_valid(file) && file->FS_if->fs_close) {
                result = file->FS_if->fs_close(file->FS_hdl,
                                               file->f_extra_data,
                                               file->fd, force);
                if (result == ESUCC) {
                        file->self   = NULL;
                        file->FS_hdl = NULL;
                        file->FS_hdl = NULL;
                        _sysfree(file);
                }
        }

        return result;
}

//==============================================================================
/**
 * @brief Function write data to file
 *
 * @param[in]  ptr              address to data (src)
 * @param[in]  count            number of items
 * @param[out] wrcnt            number of written elements
 * @param[in]  file             pointer to file object
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
int _vfs_fwrite(const void *ptr, size_t size, size_t *wrcnt, FILE *file)
{
        int result = EINVAL;

        if (ptr && size && wrcnt && is_file_valid(file)) {
                if (file->f_flag.wr) {
                        // move seek to the end of file if "a+" (wr+rd+app) mode is using
                        if (file->f_flag.append && file->f_flag.rd && file->f_flag.seekmod) {
                                _vfs_fseek(file, 0, VFS_SEEK_END);
                                file->f_flag.seekmod = false;
                        }

                        result = file->FS_if->fs_write(file->FS_hdl,
                                                       file->f_extra_data,
                                                       file->fd,
                                                       ptr,
                                                       size,
                                                       &file->f_lseek,
                                                       wrcnt,
                                                       file->f_flag.fattr);

                        if (result == ESUCC) {
                                if (static_cast(ssize_t, *wrcnt) < 0) {
                                        file->f_flag.error = true;
                                        *wrcnt = 0;

                                } else if ((*wrcnt < size) && !file->f_flag.fattr.non_blocking_wr) {
                                        file->f_flag.eof = true;
                                }

                                if (static_cast(ssize_t, *wrcnt) >= 0) {
                                        file->f_lseek += static_cast(u64_t, *wrcnt);
                                }
                        }
                }
        }

        return result;
}

//==============================================================================
/**
 * @brief Function read data from file
 *
 * @param[out] ptr              address to data (dst)
 * @param[in]  size             item size
 * @param[out] rdcnt            number of read bytes
 * @param[in]  file             pointer to file object
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
int _vfs_fread(void *ptr, size_t size, size_t *rdcnt, FILE *file)
{
        int result = EINVAL;

        if (ptr && size && rdcnt && is_file_valid(file)) {
                if (file->f_flag.rd) {
                        result = file->FS_if->fs_read(file->FS_hdl,
                                                      file->f_extra_data,
                                                      file->fd,
                                                      ptr,
                                                      size,
                                                      &file->f_lseek,
                                                      rdcnt,
                                                      file->f_flag.fattr);

                        if (result == ESUCC) {
                                if (static_cast(ssize_t, *rdcnt) < 0) {
                                        file->f_flag.error = true;
                                        *rdcnt = 0;
                                } else if ((*rdcnt < size) && !file->f_flag.fattr.non_blocking_rd) {
                                        file->f_flag.eof = true;
                                }

                                if (static_cast(ssize_t, *rdcnt) >= 0) {
                                        file->f_lseek += static_cast(u64_t, *rdcnt);
                                }
                        }
                }
        }

        return result;
}

//==============================================================================
/**
 * @brief Function set seek value
 *
 * @param[in] *file             file object
 * @param[in]  offset           seek value
 * @param[in]  mode             seek mode
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
int _vfs_fseek(FILE *file, i64_t offset, int mode)
{
        struct stat stat;

        if (is_file_valid(file) && mode <= VFS_SEEK_END) {
                if (file->f_flag.append && file->f_flag.wr && !file->f_flag.rd) {
                        return ESUCC;
                }

                if (mode == VFS_SEEK_END) {
                        stat.st_size = 0;
                        int result = _vfs_fstat(file, &stat);
                        if (result != ESUCC) {
                                return result;
                        }
                }

                switch (mode) {
                case VFS_SEEK_SET: file->f_lseek  = offset; break;
                case VFS_SEEK_CUR: file->f_lseek += offset; break;
                case VFS_SEEK_END: file->f_lseek  = stat.st_size + offset; break;
                default: return EINVAL;
                }

                file->f_flag.eof     = false;
                file->f_flag.error   = false;
                file->f_flag.seekmod = true;

                return ESUCC;
        }

        return EINVAL;
}

//==============================================================================
/**
 * @brief Function returns seek value
 *
 * @param[in]  file             file object
 * @param[out] lseek            file seek
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
int _vfs_ftell(FILE *file, i64_t *lseek)
{
        if (is_file_valid(file) && lseek) {
                *lseek = file->f_lseek;
                return ESUCC;
        } else {
                return EINVAL;
        }
}

//==============================================================================
/**
 * @brief Function support not standard operations on devices
 *
 * @param[in]     *file         file
 * @param[in]      rq           request
 * @param[in,out]  arg          argument
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
int _vfs_vfioctl(FILE *file, int rq, va_list arg)
{
        if (is_file_valid(file)) {
                switch (rq) {
                case IOCTL_VFS__NON_BLOCKING_RD_MODE: file->f_flag.fattr.non_blocking_rd = true;  return ESUCC;
                case IOCTL_VFS__NON_BLOCKING_WR_MODE: file->f_flag.fattr.non_blocking_wr = true;  return ESUCC;
                case IOCTL_VFS__DEFAULT_RD_MODE     : file->f_flag.fattr.non_blocking_rd = false; return ESUCC;
                case IOCTL_VFS__DEFAULT_WR_MODE     : file->f_flag.fattr.non_blocking_wr = false; return ESUCC;
                }

                return file->FS_if->fs_ioctl(file->FS_hdl, file->f_extra_data, file->fd, rq, va_arg(arg, void*));
        } else {
                return EINVAL;
        }
}

//==============================================================================
/**
 * @brief Function returns file/dir status
 *
 * @param[in]  *file            file object
 * @param[out] *stat            pointer to stat structure
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
int _vfs_fstat(FILE *file, struct stat *stat)
{
        int result = EINVAL;

        if (is_file_valid(file) && stat) {
                int priority = increase_task_priority();

                result = file->FS_if->fs_fstat(file->FS_hdl,
                                               file->f_extra_data,
                                               file->fd,
                                               stat);

                restore_priority(priority);
        }

        return result;
}

//==============================================================================
/**
 * @brief Function flush file data
 *
 * @param[in] *file     file to flush
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
int _vfs_fflush(FILE *file)
{
        int result = EINVAL;

        if (is_file_valid(file)) {
                int priority = increase_task_priority();

                result = file->FS_if->fs_flush(file->FS_hdl,
                                               file->f_extra_data,
                                               file->fd);

                restore_priority(priority);
        }

        return result;
}

//==============================================================================
/**
 * @brief Function check end of file
 *
 * @param[in]  *file    file
 * @param[out] *eof     EOF indicator
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
int _vfs_feof(FILE *file, int *eof)
{
        if (is_file_valid(file) && eof) {
                *eof = file->f_flag.eof ? EOF : 0;
                return ESUCC;
        } else {
                return EINVAL;
        }
}

//==============================================================================
/**
 * @brief Function clear file's error
 *
 * @param[in] *file     file
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
int _vfs_clearerr(FILE *file)
{
        if (is_file_valid(file)) {
                file->f_flag.eof   = false;
                file->f_flag.error = false;
                return ESUCC;
        } else {
                return EINVAL;
        }
}

//==============================================================================
/**
 * @brief Function check that file has no errors
 *
 * @param[in] *file     file
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
int _vfs_ferror(FILE *file, int *error)
{
        if (is_file_valid(file) && error) {
                *error = file->f_flag.error ? 1 : 0;
                return ESUCC;
        } else {
                return EINVAL;
        }
}

//==============================================================================
/**
 * @brief Function rewind file
 *
 * @param[in] *file     file
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
int _vfs_rewind(FILE *file)
{
        return _vfs_fseek(file, 0, VFS_SEEK_SET);
}

//==============================================================================
/**
 * @brief Synchronize internal buffers of mounted file systems
 *
 * @param None
 *
 * @return None
 */
//==============================================================================
void _vfs_sync(void)
{
        if (_mutex_lock(vfs_resource_mtx, MAX_DELAY_MS)) {

                _llist_foreach(FS_entry_t*, fs, vfs_mnt_list) {
                        fs->interface->fs_sync(fs->handle);
                }

                _mutex_unlock(vfs_resource_mtx);
        }
}

//==============================================================================
/**
 * @brief  Function check if selected mount point and current mount path
 *         allow to mount this file system as root.
 *
 * @param  mount_point          mount point path
 *
 * @return If file system can be mount as root file system then true is returned,
 *         otherwise false.
 */
//==============================================================================
static bool is_first_fs(const char *mount_point)
{
        return _llist_size(vfs_mnt_list) == 0 && strcmp(mount_point, "/") == 0;
}

//==============================================================================
/**
 * @brief  Create a new file system entry. Function initialize selected file system.
 *
 * @param[in ] parent_FS        parent file system (can be NULL if none)
 * @param[in ] fs_mount_point   file system mount point. This object must be permanent
 * @param[in ] fs_src_file      file system source file
 * @param[in ] fs_interface     file system interface (is copied to the object)
 * @param[out] entry            entry
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
static int new_FS_entry(FS_entry_t               *parent_FS,
                        const char               *fs_mount_point,
                        const char               *fs_src_file,
                        const vfs_FS_itf_t *fs_interface,
                        FS_entry_t               **fs_entry)
{
        int result = ENOMEM;

        FS_entry_t *new_FS = _sysmalloc(sizeof(FS_entry_t));
        if (new_FS) {
                result = fs_interface->fs_init(&new_FS->handle, fs_src_file);
                if (result == ESUCC) {
                        new_FS->interface     = fs_interface;
                        new_FS->mount_point   = fs_mount_point;
                        new_FS->parent = parent_FS;
                        new_FS->children_cnt  = 0;
                        *fs_entry             = new_FS;
                } else {
                        _sysfree(new_FS);
                }
        }

        return result;
}

//==============================================================================
/**
 * @brief  Delete created file system entry. Function try release mounted file system.
 *
 * @param  this         file system entry object
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
static int delete_FS_entry(FS_entry_t *this)
{
        int result = EINVAL;

        if (this) {
                result = this->interface->fs_release(this->handle);
                if (result == ESUCC) {
                        if (this->parent && this->parent->children_cnt) {
                                this->parent->children_cnt--;
                        }

                        if (this->mount_point) {
                                _sysfree(const_cast(char*, this->mount_point));
                        }

                        _sysfree(this);
                }
        }

        return result;
}

//==============================================================================
/**
 * @brief Check if file object is valid
 *
 * @param file  file object to examine
 *
 * @return true if object is valid, otherwise false
 */
//==============================================================================
static bool is_file_valid(FILE *file)
{
        return (  file
               && file->self == file
               && file->FS_hdl
               && file->FS_if
               && file->FS_if->fs_magic == _VFS_FILE_SYSTEM_MAGIC_NO);
}

//==============================================================================
/**
 * @brief Check if dir object is valid
 *
 * @param file  file object to examine
 *
 * @return true if object is valid, otherwise false
 */
//==============================================================================
static bool is_dir_valid(DIR *dir)
{
        return (dir && dir->self == dir);
}

//==============================================================================
/**
 * @brief Function increase task priority and return original priority value
 *
 * @return original task priority
 */
//==============================================================================
static int increase_task_priority(void)
{
        int priority = _task_get_priority();

        if (priority < PRIORITY_HIGHEST) {
                _task_set_priority(priority + 1);
        }

        return priority;
}

//==============================================================================
/**
 * @brief Function restore original priority
 *
 * @param priority
 */
//==============================================================================
static inline void restore_priority(int priority)
{
        _task_set_priority(priority);
}

//==============================================================================
/**
 * @brief Function convert file open mode string to flags
 *
 * @param[in]  str      file open mode string
 * @param[out] flags    file flags (for internal file use)
 *
 * @return On success true is returned, otherwise false.
 */
//==============================================================================
static bool parse_flags(const char *str, u32_t *flags)
{
        if (strcmp("r", str) == 0 || strcmp("rb", str) == 0) {
                *flags = O_RDONLY;
                return true;
        }

        if (strcmp("r+", str) == 0 || strcmp("rb+", str) == 0 || strcmp("r+b", str) == 0) {
                *flags = O_RDWR;
                return true;
        }

        if (strcmp("w", str) == 0 || strcmp("wb", str) == 0) {
                *flags = O_WRONLY | O_CREAT | O_TRUNC;
                return true;
        }

        if (strcmp("w+", str) == 0 || strcmp("wb+", str) == 0 || strcmp("w+b", str) == 0) {
                *flags = O_RDWR | O_CREAT | O_TRUNC;
                return true;
        }

        if (strcmp("a", str) == 0 || strcmp("ab", str) == 0) {
                *flags = O_WRONLY | O_CREAT | O_APPEND;
                return true;
        }

        if (strcmp("a+", str) == 0 || strcmp("ab+", str) == 0 || strcmp("a+b", str) == 0) {
                *flags = O_RDWR | O_CREAT | O_APPEND;
                return true;
        }

        return false;
}

//==============================================================================
/**
 * @brief Function return file system entry of selected path
 *
 * @param[in]  path             path to FS
 * @param[in]  len              path length
 * @param[out] position         object position in list (can be NULL)
 * @param[out] fs_entry         found entry
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
static int get_path_FS(const char *path, size_t len, int *position, FS_entry_t **fs_entry)
{
        int pos = 0;

        _llist_foreach(FS_entry_t*, entry, vfs_mnt_list) {
                if (strncmp(path, entry->mount_point, len) == 0) {
                        if (position) {
                                *position = pos;
                        }

                        *fs_entry = entry;
                        return ESUCC;
                }

                pos++;
        }

        return ENOENT;
}

//==============================================================================
/**
 * @brief Function returned the base file system of selected path. The external
 *        path is passed by pointer ext_path.
 *        Function is thread safe.
 *
 * @param[in]  path           path to FS
 * @param[out] ext_path       pointer to external part of path (can be NULL)
 * @param[out] fs_entry       file system entry
 *
 * @return On success base file system entry is returned, otherwise NULL.
 */
//==============================================================================
static int get_path_base_FS(const char *path, const char **ext_path, FS_entry_t **fs_entry)
{
        const char *path_tail = path + strlen(path);

        if (*(path_tail - 1) == '/') {
                path_tail--;
        }

        int result = ENOENT;

        if (_mutex_lock(vfs_resource_mtx, MAX_DELAY_MS)) {

                while (path_tail >= path) {
                        result = get_path_FS(path, path_tail - path + 1, NULL, fs_entry);
                        if (result == ESUCC) {
                                break;
                        } else {
                                while (*(--path_tail) != '/' && path_tail >= path);
                        }
                }

                _mutex_unlock(vfs_resource_mtx);
        }

        if (result == ESUCC && ext_path) {
                *ext_path = path_tail;
        }

        return result;
}

//==============================================================================
/**
 * @brief Function create new path with slash and cwd correction
 *        Function set errno: ENOMEM
 *
 * @param[in]  path             path to correct
 * @param[in]  corr             path correction kind
 * @param[out] new_path         new path
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
static int new_CWD_path(const char *path, enum path_correction corr, char **new_path)
{
        uint        new_path_len = strlen(path);
        const char *cwd;
        uint        cwd_len = 0;
        int         result  = ENOMEM;

        /* correct ending slash */
        if (corr == SUB_SLASH && LAST_CHARACTER(path) == '/') {
                new_path_len--;
        } else if (corr == ADD_SLASH && LAST_CHARACTER(path) != '/') {
                new_path_len++;
        }

        /* correct cwd */
        if (FIRST_CHARACTER(path) != '/') {
                cwd = _task_get_data()->f_cwd;
                if (cwd) {
                        cwd_len       = strlen(cwd);
                        new_path_len += cwd_len;

                        if (LAST_CHARACTER(cwd) != '/' && cwd_len) {
                                new_path_len++;
                                cwd_len++;
                        }
                }
        }

        *new_path = _syscalloc(new_path_len + 1, sizeof(char));
        if (*new_path) {
                if (cwd_len && cwd) {
                        strcpy(*new_path, cwd);

                        if (LAST_CHARACTER(cwd) != '/') {
                                strcat(*new_path, "/");
                        }
                }

                if (corr == SUB_SLASH) {
                        strncat(*new_path, path, new_path_len - cwd_len);

                } else if (corr == ADD_SLASH) {
                        strcat(*new_path, path);

                        if (LAST_CHARACTER(*new_path) != '/') {
                                strcat(*new_path, "/");
                        }

                } else {
                        strcat(*new_path, path);
                }

                result = ESUCC;
        }

        return result;
}

/*==============================================================================
  End of file
==============================================================================*/
