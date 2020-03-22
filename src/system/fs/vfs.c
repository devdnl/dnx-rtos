/*=========================================================================*//**
@file    vfs.c

@author  Daniel Zorychta

@brief   This file support virtual file system

@note    Copyright (C) 2012-2015 Daniel Zorychta <daniel.zorychta@gmail.com>

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
#include <dnx/misc.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include "fs/vfs.h"
#include "lib/llist.h"
#include "kernel/kwrapper.h"
#include "kernel/process.h"
#include "kernel/sysfunc.h"

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

/*==============================================================================
  Local function prototypes
==============================================================================*/
static bool is_first_fs      (const char *mount_point);
static int  new_FS_entry     (FS_entry_t *parent_FS, const char *fs_mount_point, const char *fs_src_file, const vfs_FS_itf_t *fs_interface, const char *opts, FS_entry_t **fs_entry);
static int  delete_FS_entry  (FS_entry_t *this);
static bool is_file_valid    (FILE *file);
static bool is_dir_valid     (DIR *dir);
static int  parse_flags      (const char *str, u32_t *flags);
static int  get_path_FS      (const char *path, size_t len, int *position, FS_entry_t **fs_entry);
static int  get_path_base_FS (const char *path, const char **extPath, FS_entry_t **fs_entry);
static int  new_absolute_path(const struct vfs_path *path, enum path_correction corr, char **new_path);

/*==============================================================================
  Local object definitions
==============================================================================*/
static struct {
        llist_t *mnt_list;
        mutex_t *resource_mtx;
} VFS;

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
int _vfs_init(void)
{
        int err = _llist_create_krn(_MM_KRN, NULL, NULL, &VFS.mnt_list);
        if (!err) {
                err = _mutex_create(MUTEX_TYPE_RECURSIVE, &VFS.resource_mtx);
        }

        return err;
}

//==============================================================================
/**
 * @brief Function mount file system in VFS.
 *
 * @param[in] *src_path         path to source file when file system load data
 * @param[in] *mount_point      path when dir shall be mounted
 * @param[in] *fsif             pointer to file system interface
 * @param[in] *opts             options
 *
 * @return One of errno values.
 */
//==============================================================================
int _vfs_mount(const struct vfs_path   *src_path,
               const struct vfs_path   *mount_point,
               const struct vfs_FS_itf *fsif,
               const char              *opts)
{
        if (  !mount_point
           || !mount_point->PATH
           || !src_path
           || !src_path->PATH
           || !fsif->fs_init
           || !fsif->fs_release
           || !fsif->fs_open
           || !fsif->fs_close
           || !fsif->fs_write
           || !fsif->fs_read
           || !fsif->fs_ioctl
           || !fsif->fs_fstat
           || !fsif->fs_flush
           || !fsif->fs_mknod
           || !fsif->fs_sync
           || !fsif->fs_opendir
           || !fsif->fs_closedir
           || !fsif->fs_readdir
    #if __OS_ENABLE_FSTAT__ == _YES_
           || !fsif->fs_stat
    #endif
    #if __OS_ENABLE_MKDIR__ == _YES_
           || !fsif->fs_mkdir
    #endif
    #if __OS_ENABLE_MKFIFO__ == _YES_
           || !fsif->fs_mkfifo
    #endif
    #if __OS_ENABLE_REMOVE__ == _YES_
           || !fsif->fs_remove
    #endif
    #if __OS_ENABLE_RENAME__ == _YES_
           || !fsif->fs_rename
    #endif
    #if __OS_ENABLE_CHMOD__ == _YES_
           || !fsif->fs_chmod
    #endif
    #if __OS_ENABLE_CHOWN__ == _YES_
           || !fsif->fs_chown
    #endif
    #if __OS_ENABLE_STATFS__ == _YES_
           || !fsif->fs_statfs
    #endif
           || fsif->fs_magic != _VFS_FILE_SYSTEM_MAGIC_NO) {

                return EINVAL;
        }

        // create new paths that are corrected by CWD
        char *cwd_mount_point = NULL;
        int err = new_absolute_path(mount_point, ADD_SLASH, &cwd_mount_point);
        if (err) {
                return err;
        }

        char *cwd_src_path = NULL;
        err = new_absolute_path(src_path, SUB_SLASH, &cwd_src_path);
        if (err) {
                goto finish;
        }

        // create new entry
        err = _mutex_lock(VFS.resource_mtx, MAX_DELAY_MS);
        if (not err) {

                FS_entry_t *new_fs;

                /*
                 * create new FS in existing DIR and FS, otherwise create new FS if
                 * first mount
                 */
                if (is_first_fs(cwd_mount_point)) {
                        err = new_FS_entry(NULL, cwd_mount_point, cwd_src_path,
                                           fsif, opts, &new_fs);
                } else {
                        const char *ext_path;
                        FS_entry_t *base_fs;
                        FS_entry_t *mounted_fs;

                        err = get_path_base_FS(cwd_mount_point, &ext_path, &base_fs);
                        if (!err) {
                                err = get_path_FS(cwd_mount_point, PATH_MAX_LEN,
                                                  NULL, &mounted_fs);
                        }

                        if (err == ENOENT) {
                                DIR dir;
                                err = base_fs->interface->fs_opendir(base_fs->handle,
                                                                     ext_path, &dir);
                                if (!err) {
                                        base_fs->children_cnt++;

                                        err = base_fs->interface->fs_closedir(base_fs->handle, &dir);
                                        if (!err) {
                                                err = new_FS_entry(base_fs, cwd_mount_point,
                                                                   cwd_src_path, fsif,
                                                                   opts, &new_fs);
                                        }
                                }
                        } else {
                                err = EADDRINUSE;
                        }
                }

                /*
                 * mount FS if created
                 */
                if (!err) {
                        if (!_llist_push_back(VFS.mnt_list, new_fs)) {
                                delete_FS_entry(new_fs);
                                err = ENOMEM;
                        }
                }

                _mutex_unlock(VFS.resource_mtx);
        }

        finish:
        if (err && cwd_mount_point) {
                _kfree(_MM_KRN, cast(void**, &cwd_mount_point));
        }

        if (cwd_src_path) {
                _kfree(_MM_KRN, cast(void**, &cwd_src_path));
        }

        return err;
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
int _vfs_umount(const struct vfs_path *path)
{
        if (!path || !path->PATH) {
                return EINVAL;
        }

        char *cwd_path;
        int err = new_absolute_path(path, ADD_SLASH, &cwd_path);
        if (not err) {
                err = _mutex_lock(VFS.resource_mtx, MAX_DELAY_MS);
                if (not err) {

                        int         position;
                        FS_entry_t *mount_fs;
                        err = get_path_FS(cwd_path, PATH_MAX_LEN, &position, &mount_fs);

                        if (not err) {
                                if (mount_fs->children_cnt == 0) {
                                        err = delete_FS_entry(mount_fs);
                                        if (not err) {
                                                _llist_take(VFS.mnt_list, position);
                                        }
                                } else {
                                        err = EBUSY;
                                }
                        }

                        _mutex_unlock(VFS.resource_mtx);
                }

                _kfree(_MM_KRN, cast(void**, &cwd_path));
        }

        return err;
}

#if __OS_ENABLE_STATFS__ == _YES_
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

        FS_entry_t *fs = NULL;
        int err = _mutex_lock(VFS.resource_mtx, MAX_DELAY_MS);
        if (!err) {
                fs  = _llist_at(VFS.mnt_list, seek);
                err = fs ? ESUCC : ENOENT;
                _mutex_unlock(VFS.resource_mtx);
        }

        if (!err) {
                mntent->mnt_dir   = fs->mount_point;
                mntent->mnt_free  = 0;
                mntent->mnt_total = 0;

                struct statfs stat_fs;
                memset(&stat_fs, 0, sizeof(stat_fs));

                err = fs->interface->fs_statfs(fs->handle, &stat_fs);
                if (!err) {
                        mntent->mnt_fsname = stat_fs.f_fsname;
                        mntent->mnt_free   = (u64_t)stat_fs.f_bfree  * stat_fs.f_bsize;
                        mntent->mnt_total  = (u64_t)stat_fs.f_blocks * stat_fs.f_bsize;
                } else {
                        mntent->mnt_fsname = stat_fs.f_fsname ? stat_fs.f_fsname : "unknown";
                }
        }

        return err;
}
#endif

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
int _vfs_mknod(const struct vfs_path *path, dev_t dev)
{
        if (!path || !path->PATH) {
                return EINVAL;
        }

        char *cwd_path;
        int err = new_absolute_path(path, NO_SLASH_ACTION, &cwd_path);
        if (!err) {

                const char *external_path;
                FS_entry_t *fs;
                err = get_path_base_FS(cwd_path, &external_path, &fs);
                if (!err) {
                        err = fs->interface->fs_mknod(fs->handle, external_path, dev);
                }

                _kfree(_MM_KRN, cast(void**, &cwd_path));
        }

        return err;
}

#if __OS_ENABLE_MKDIR__ == _YES_
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
int _vfs_mkdir(const struct vfs_path *path, mode_t mode)
{
        if (!path || !path->PATH) {
                return EINVAL;
        }

        char *cwd_path;
        int err = new_absolute_path(path, SUB_SLASH, &cwd_path);
        if (!err) {

                const char *external_path;
                FS_entry_t *fs;
                err = get_path_base_FS(cwd_path, &external_path, &fs);
                if (!err) {
                        err = fs->interface->fs_mkdir(fs->handle, external_path, S_IPMT(mode));
                }

                _kfree(_MM_KRN, cast(void**, &cwd_path));
        }

        return err;
}
#endif

#if __OS_ENABLE_MKFIFO__ == _YES_
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
int _vfs_mkfifo(const struct vfs_path *path, mode_t mode)
{
        if (!path || !path->PATH) {
                return EINVAL;
        }

        char *cwd_path;
        int err = new_absolute_path(path, NO_SLASH_ACTION, &cwd_path);
        if (!err) {

                const char *external_path;
                FS_entry_t *fs;
                err = get_path_base_FS(cwd_path, &external_path, &fs);
                if (!err) {
                        err = fs->interface->fs_mkfifo(fs->handle, external_path, S_IPMT(mode));
                }

                _kfree(_MM_KRN, cast(void**, &cwd_path));
        }

        return err;
}
#endif

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
int _vfs_opendir(const struct vfs_path *path, DIR **dir)
{
        if (!path || !path->PATH || !dir) {
                return EINVAL;
        }

        int err = _kmalloc(_MM_KRN, sizeof(DIR), cast(void**, dir));
        if (!err) {
                char *cwd_path;
                err = new_absolute_path(path, ADD_SLASH, &cwd_path);
                if (!err) {

                        const char *external_path;
                        FS_entry_t *fs;
                        err = get_path_base_FS(cwd_path, &external_path, &fs);

                        if (!err) {
                                (*dir)->FS_hdl = fs->handle;
                                (*dir)->FS_if  = fs->interface;

                                err = fs->interface->fs_opendir(fs->handle, external_path, *dir);
                        }

                        _kfree(_MM_KRN, cast(void**, &cwd_path));
                }

                if (!err) {
                        (*dir)->header.type = RES_TYPE_DIR;
                } else {
                        _kfree(_MM_KRN, cast(void**, dir));
                }
        }

        return err;
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
        int err = EINVAL;

        if (is_dir_valid(dir)) {
                err = dir->FS_if->fs_closedir(dir->FS_hdl, dir);
                if (!err) {
                        dir->header.type = RES_TYPE_UNKNOWN;
                        _kfree(_MM_KRN, cast(void**, &dir));
                }
        }

        return err;
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
        int err = EINVAL;

        if (is_dir_valid(dir) && dirent) {
                err = dir->FS_if->fs_readdir(dir->FS_hdl, dir);
                if (!err) {
                        *dirent = &dir->dirent;
                } else {
                        *dirent = NULL;
                }
        }

        return err;
}

//==============================================================================
/**
 * @brief Function set position of read index.
 *
 * @param dir           directory object
 * @param seek          file position
 *
 * @return One of errno values.
 */
//==============================================================================
int _vfs_seekdir(DIR *dir, u32_t seek)
{
        if (is_dir_valid(dir)) {
                dir->d_seek = seek;
                return ESUCC;
        } else {
                return EINVAL;
        }
}

//==============================================================================
/**
 * @brief Function get position of read index.
 *
 * @param dir           directory object
 * @param seek          file position pointer
 *
 * @return One of errno values.
 */
//==============================================================================
int _vfs_telldir(DIR *dir, u32_t *seek)
{
        if (is_dir_valid(dir) && seek) {
                *seek = dir->d_seek;
                return ESUCC;
        } else {
                return EINVAL;
        }
}

#if __OS_ENABLE_REMOVE__ == _YES_
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
int _vfs_remove(const struct vfs_path *path)
{
        if (!path || !path->PATH) {
                return EINVAL;
        }

        char *cwd_path;
        int err = new_absolute_path(path, ADD_SLASH, &cwd_path);
        if (!err) {

                const char *external_path;
                FS_entry_t *mount_fs;
                FS_entry_t *base_fs;

                err = _mutex_lock(VFS.resource_mtx, MAX_DELAY_MS);
                if (!err) {

                        err = get_path_FS(cwd_path, PATH_MAX_LEN, NULL, &mount_fs);
                        if (err == ENOENT) {
                                // remove slash at the end
                                LAST_CHARACTER(cwd_path) = '\0';

                                // get parent FS
                                err = get_path_base_FS(cwd_path, &external_path, &base_fs);
                        } else {
                                err = EBUSY;
                        }

                        _mutex_unlock(VFS.resource_mtx);
                }

                if (!err) {
                        err = base_fs->interface->fs_remove(base_fs->handle, external_path);
                }

                _kfree(_MM_KRN, cast(void**, &cwd_path));
        }

        return err;
}
#endif

#if __OS_ENABLE_RENAME__ == _YES_
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
int _vfs_rename(const struct vfs_path *old_name, const struct vfs_path *new_name)
{
        if (!old_name || !old_name->PATH || !new_name || !new_name->PATH) {
                return EINVAL;
        }

        char *cwd_old_name = NULL;
        char *cwd_new_name = NULL;

        int err = new_absolute_path(old_name, NO_SLASH_ACTION, &cwd_old_name);
        if (!err) {
                err = new_absolute_path(new_name, NO_SLASH_ACTION, &cwd_new_name);
        }

        if (!err) {
                FS_entry_t *old_fs;
                FS_entry_t *new_fs;
                const char *old_extern_path;
                const char *new_extern_path;

                err = _mutex_lock(VFS.resource_mtx, MAX_DELAY_MS);
                if (!err) {
                        err = get_path_base_FS(cwd_old_name, &old_extern_path, &old_fs);
                        if (!err) {
                                err = get_path_base_FS(cwd_new_name, &new_extern_path, &new_fs);
                        }
                        _mutex_unlock(VFS.resource_mtx);
                }

                if (!err) {

                        if (old_fs == new_fs) {
                                err = old_fs->interface->fs_rename(old_fs->handle,
                                                                   old_extern_path,
                                                                   new_extern_path);
                        } else {
                                err = ENOTSUP;
                        }
                }
        }

        if (cwd_old_name) {
                _kfree(_MM_KRN, cast(void**, &cwd_old_name));
        }

        if (cwd_new_name) {
                _kfree(_MM_KRN, cast(void**, &cwd_new_name));
        }

        return err;
}
#endif

#if __OS_ENABLE_CHMOD__ == _YES_
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
int _vfs_chmod(const struct vfs_path *path, mode_t mode)
{
        if (!path || !path->PATH) {
                return EINVAL;
        }

        char *cwd_path;
        int err = new_absolute_path(path, NO_SLASH_ACTION, &cwd_path);
        if (!err) {

                const char *external_path;
                FS_entry_t *fs;
                err = get_path_base_FS(cwd_path, &external_path, &fs);
                if (!err) {
                        err = fs->interface->fs_chmod(fs->handle, external_path, S_IPMT(mode));
                }

                _kfree(_MM_KRN, cast(void**, &cwd_path));
        }

        return err;
}
#endif

#if __OS_ENABLE_CHOWN__ == _YES_
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
int _vfs_chown(const struct vfs_path *path, uid_t owner, gid_t group)
{
        if (!path || !path->PATH) {
                return EINVAL;
        }

        char *cwd_path;
        int err = new_absolute_path(path, NO_SLASH_ACTION, &cwd_path);
        if (!err) {

                const char *external_path;
                FS_entry_t *fs;
                err = get_path_base_FS(cwd_path, &external_path, &fs);
                if (!err) {
                        err = fs->interface->fs_chown(fs->handle, external_path, owner, group);
                }

                _kfree(_MM_KRN, cast(void**, &cwd_path));
        }

        return err;
}
#endif

#if __OS_ENABLE_FSTAT__ == _YES_
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
int _vfs_stat(const struct vfs_path *path, struct stat *stat)
{
        if (!path || !path->PATH|| !stat) {
                return EINVAL;
        }

        char *cwd_path;
        int err = new_absolute_path(path, NO_SLASH_ACTION, &cwd_path);
        if (!err) {

                const char *external_path;
                FS_entry_t *fs;
                err = get_path_base_FS(cwd_path, &external_path, &fs);
                if (!err) {
                        err = fs->interface->fs_stat(fs->handle, external_path, stat);
                }

                _kfree(_MM_KRN, cast(void**, &cwd_path));
        }

        return err;
}
#endif

#if __OS_ENABLE_STATFS__ == _YES_
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
int _vfs_statfs(const struct vfs_path *path, struct statfs *statfs)
{
        if (!path || !path->PATH || !statfs) {
                return EINVAL;
        }

        char *cwd_path;
        int err = new_absolute_path(path, ADD_SLASH, &cwd_path);
        if (!err) {

                FS_entry_t *fs;
                err = get_path_base_FS(cwd_path, NULL, &fs);
                if (!err) {
                        err = fs->interface->fs_statfs(fs->handle, statfs);
                }

                _kfree(_MM_KRN, cast(void**, &cwd_path));
        }

        return err;
}
#endif

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
int _vfs_fopen(const struct vfs_path *path, const char *mode, FILE **file)
{
        if (!path || !path->PATH || !mode || !file) {
                return EINVAL;
        }

        if (LAST_CHARACTER(path->PATH) == '/') { /* path is a directory */
                return EISDIR;
        }

        u32_t            o_flags;
        vfs_file_flags_t f_flags;

        int err = parse_flags(mode, &o_flags);
        if (err) {
                return err;
        }

        memset(&f_flags, 0, sizeof(vfs_file_flags_t));
        f_flags.rd     = ((o_flags == O_RDONLY) || (o_flags & O_RDWR));
        f_flags.wr     = (o_flags & (O_RDWR | O_WRONLY));
        f_flags.append = (o_flags & (O_APPEND));

        char *cwd_path = NULL;
        err = new_absolute_path(path, NO_SLASH_ACTION, &cwd_path);
        if (err) {
                return err;
        }

        FILE *file_obj = NULL;
        err = _kzalloc(_MM_KRN, sizeof(FILE), cast(void**, &file_obj));
        if (!err && file_obj) {

                const char *external_path;
                FS_entry_t *fs;
                err = get_path_base_FS(cwd_path, &external_path, &fs);
                if (!err) {
                        err = fs->interface->fs_open(fs->handle,
                                                     &file_obj->f_hdl,
                                                     &file_obj->f_lseek,
                                                     external_path,
                                                     o_flags);

                        struct stat stat;
                        if (!err) {
                                err = fs->interface->fs_fstat(fs->handle,
                                                              file_obj->f_hdl,
                                                              &stat);
                        }

                        if (!err) {
                                if ((stat.st_mode & S_IRUSR) == 0) {
                                        f_flags.rd = false;
                                }

                                if ((stat.st_mode & S_IWUSR) == 0) {
                                        f_flags.wr = false;
                                }

                                if (f_flags.append) {
                                        file_obj->f_lseek = stat.st_size;
                                }

                                file_obj->FS_hdl      = fs->handle;
                                file_obj->FS_if       = fs->interface;
                                file_obj->f_flag      = f_flags;
                                file_obj->header.type = RES_TYPE_FILE;
                        }
                }
        }

        if (cwd_path) {
                _kfree(_MM_KRN, cast(void**, &cwd_path));
        }

        if (file_obj) {
                if (file_obj->header.type == RES_TYPE_FILE) {
                        *file = file_obj;
                } else {
                        _kfree(_MM_KRN, cast(void**, &file_obj));
                }
        }

        return err;
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
        int err = EINVAL;

        if (is_file_valid(file) && file->FS_if->fs_close) {
                err = file->FS_if->fs_close(file->FS_hdl, file->f_hdl, force);
                if (!err) {
                        file->header.type = RES_TYPE_UNKNOWN;
                        file->FS_hdl      = NULL;
                        file->FS_hdl      = NULL;
                        _kfree(_MM_KRN, cast(void**, &file));
                }
        }

        return err;
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
        int err = EINVAL;

        if (ptr && size && wrcnt && is_file_valid(file)) {
                *wrcnt = 0;

                if (file->f_flag.wr) {
                        // move seek to the end of file if "a+" (wr+rd+app) mode is using
                        if (file->f_flag.append && file->f_flag.rd && file->f_flag.seekmod) {
                                _vfs_fseek(file, 0, VFS_SEEK_END);
                                file->f_flag.seekmod = false;
                        }

                        err = file->FS_if->fs_write(file->FS_hdl,
                                                    file->f_hdl,
                                                    ptr,
                                                    size,
                                                    &file->f_lseek,
                                                    wrcnt,
                                                    file->f_flag.fattr);

                        if (!err) {
                                if ((*wrcnt < size) && !file->f_flag.fattr.non_blocking_wr) {
                                        file->f_flag.eof = true;
                                }

                                if (cast(ssize_t, *wrcnt) >= 0) {
                                        file->f_lseek += cast(u64_t, *wrcnt);
                                }
                        } else {
                                file->f_flag.error = true;
                        }
                } else {
                        file->f_flag.error = true;
                        err = EPERM;
                }
        }

        return err;
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
        int err = EINVAL;

        if (ptr && size && rdcnt && is_file_valid(file)) {
                *rdcnt = 0;

                if (file->f_flag.rd) {
                        err = file->FS_if->fs_read(file->FS_hdl,
                                                   file->f_hdl,
                                                   ptr,
                                                   size,
                                                   &file->f_lseek,
                                                   rdcnt,
                                                   file->f_flag.fattr);

                        if (!err) {
                                if ((*rdcnt < size) && !file->f_flag.fattr.non_blocking_rd) {
                                        file->f_flag.eof = true;
                                }

                                if (cast(ssize_t, *rdcnt) >= 0) {
                                        file->f_lseek += cast(u64_t, *rdcnt);
                                }
                        } else {
                                file->f_flag.error = true;
                        }
                } else {
                        file->f_flag.error = true;
                        err = EPERM;
                }
        }

        return err;
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
                        int err = _vfs_fstat(file, &stat);
                        if (err) {
                                return err;
                        }
                }

                switch (mode) {
                case VFS_SEEK_SET: file->f_lseek  = offset; break;
                case VFS_SEEK_CUR: file->f_lseek += offset; break;
                case VFS_SEEK_END: file->f_lseek  = stat.st_size + offset; break;
                default: return EINVAL;
                }

                file->f_flag.eof     = false;
                file->f_flag.seekmod = true;

                if ((mode == VFS_SEEK_SET) && (offset == 0)) {
                        file->f_flag.error = false;
                }

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
                case IOCTL_VFS__NON_BLOCKING_RD_MODE:
                        file->f_flag.fattr.non_blocking_rd = true;
                        return ESUCC;

                case IOCTL_VFS__NON_BLOCKING_WR_MODE:
                        file->f_flag.fattr.non_blocking_wr = true;
                        return ESUCC;

                case IOCTL_VFS__DEFAULT_RD_MODE:
                        file->f_flag.fattr.non_blocking_rd = false;
                        return ESUCC;

                case IOCTL_VFS__DEFAULT_WR_MODE:
                        file->f_flag.fattr.non_blocking_wr = false;
                        return ESUCC;

                case IOCTL_VFS__IS_NON_BLOCKING_RD_MODE:
                        *va_arg(arg, bool*) = file->f_flag.fattr.non_blocking_rd;
                        return ESUCC;

                case IOCTL_VFS__IS_NON_BLOCKING_WR_MODE:
                        *va_arg(arg, bool*) = file->f_flag.fattr.non_blocking_wr;
                        return ESUCC;
                }

                return file->FS_if->fs_ioctl(file->FS_hdl,
                                             file->f_hdl,
                                             rq, va_arg(arg, void*));
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
        int err = EINVAL;

        if (is_file_valid(file) && stat) {
                err = file->FS_if->fs_fstat(file->FS_hdl, file->f_hdl, stat);
        }

        return err;
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
        int err = EINVAL;

        if (is_file_valid(file)) {
                err = file->FS_if->fs_flush(file->FS_hdl, file->f_hdl);
        }

        return err;
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
 * @param[in]  file     file
 * @param[out] error    error indicator (1 for error, 0 no error)
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
 * @brief Synchronize internal buffers of mounted file systems
 *
 * @param None
 *
 * @return None
 */
//==============================================================================
void _vfs_sync(void)
{
        if (_mutex_lock(VFS.resource_mtx, MAX_DELAY_MS) == ESUCC) {

                _llist_foreach(FS_entry_t*, fs, VFS.mnt_list) {
                        fs->interface->fs_sync(fs->handle);
                }

                _mutex_unlock(VFS.resource_mtx);
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
        return _llist_size(VFS.mnt_list) == 0 && strcmp(mount_point, "/") == 0;
}

//==============================================================================
/**
 * @brief  Create a new file system entry. Function initialize selected file system.
 *
 * @param[in ] parent_FS        parent file system (can be NULL if none)
 * @param[in ] fs_mount_point   file system mount point. This object must be permanent
 * @param[in ] fs_src_file      file system source file
 * @param[in ] fs_interface     file system interface (is copied to the object)
 * @param[in ] opts             file system options
 * @param[out] entry            entry
 *
 * @return One of errno value (errno.h).
 */
//==============================================================================
static int new_FS_entry(FS_entry_t         *parent_FS,
                        const char         *fs_mount_point,
                        const char         *fs_src_file,
                        const vfs_FS_itf_t *fs_interface,
                        const char         *opts,
                        FS_entry_t         **fs_entry)
{
        FS_entry_t *new_FS = NULL;
        int err = _kmalloc(_MM_KRN, sizeof(FS_entry_t), cast(void**, &new_FS));
        if (!err) {
                err = fs_interface->fs_init(&new_FS->handle, fs_src_file, opts);
                if (!err) {
                        new_FS->interface     = fs_interface;
                        new_FS->mount_point   = fs_mount_point;
                        new_FS->parent        = parent_FS;
                        new_FS->children_cnt  = 0;
                        *fs_entry             = new_FS;
                } else {
                        _kfree(_MM_KRN, cast(void**, &new_FS));
                }
        }

        return err;
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
        int err = EINVAL;

        if (this) {
                err = this->interface->fs_sync(this->handle);
                if (err) {
                        printk("VFS: unable to sync '%s' (%d)", this->mount_point, err);
                }

                err = this->interface->fs_release(this->handle);
                if (!err) {
                        if (this->parent && this->parent->children_cnt) {
                                this->parent->children_cnt--;
                        }

                        if (this->mount_point) {
                                _kfree(_MM_KRN, cast(void**, &this->mount_point));
                        }

                        _kfree(_MM_KRN, cast(void**, &this));
                }
        }

        return err;
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
        return (  _mm_is_object_in_heap(file)
               && file->FS_hdl          != NULL
               && file->FS_if           != NULL
               && file->header.type     == RES_TYPE_FILE
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
        return (_mm_is_object_in_heap(dir) && dir->header.type == RES_TYPE_DIR);
}

//==============================================================================
/**
 * @brief Function convert file open mode string to flags
 *
 * @param[in]  str      file open mode string
 * @param[out] flags    file flags (for internal file use)
 *
 * @return One of errno value.
 */
//==============================================================================
static int parse_flags(const char *str, u32_t *flags)
{
        if (strcmp("r", str) == 0 || strcmp("rb", str) == 0) {
                *flags = O_RDONLY;
                return ESUCC;
        }

        if (strcmp("r+", str) == 0 || strcmp("rb+", str) == 0 || strcmp("r+b", str) == 0) {
                *flags = O_RDWR;
                return ESUCC;
        }

        if (strcmp("w", str) == 0 || strcmp("wb", str) == 0) {
                *flags = O_WRONLY | O_CREAT | O_TRUNC;
                return ESUCC;
        }

        if (strcmp("w+", str) == 0 || strcmp("wb+", str) == 0 || strcmp("w+b", str) == 0) {
                *flags = O_RDWR | O_CREAT | O_TRUNC;
                return ESUCC;
        }

        if (strcmp("a", str) == 0 || strcmp("ab", str) == 0) {
                *flags = O_WRONLY | O_CREAT | O_APPEND;
                return ESUCC;
        }

        if (strcmp("a+", str) == 0 || strcmp("ab+", str) == 0 || strcmp("a+b", str) == 0) {
                *flags = O_RDWR | O_CREAT | O_APPEND;
                return ESUCC;
        }

        return EINVAL;
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

        _llist_foreach(FS_entry_t*, entry, VFS.mnt_list) {
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

        int err = _mutex_lock(VFS.resource_mtx, MAX_DELAY_MS);
        if (!err) {

                while (path_tail >= path) {
                        err = get_path_FS(path, path_tail - path + 1, NULL, fs_entry);
                        if (!err) {
                                break;
                        } else {
                                while (*(--path_tail) != '/' && path_tail >= path);
                        }
                }

                _mutex_unlock(VFS.resource_mtx);
        }

        if (!err && ext_path) {
                *ext_path = path_tail;
        }

        return err;
}

//==============================================================================
/**
 * @brief Function create new path with slash and CWD correction.
 *        Function set errno: ENOMEM. Path should be destroyed by free() family
 *        functions.
 *
 * @param[in]  path             path to correct
 * @param[in]  corr             path correction kind
 * @param[out] new_path         new path
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
static int new_absolute_path(const struct vfs_path *path, enum path_correction corr, char **new_path)
{
        size_t abslen = 3;      // additional 2 slashes (if needed) and nul

        if (path->PATH[0] == '/') {
                abslen += strlen(path->PATH);
        } else {
                abslen += strlen(path->CWD) + strlen(path->PATH);
        }

        // memory allocation
        char *abspath;
        int err = _kzalloc(_MM_KRN, abslen, cast(void*, &abspath));
        if (!err) {
                if (path->PATH[0] == '/') {
                        strcpy(abspath, path->PATH);
                } else {
                        strcpy(abspath, path->CWD);

                        if (LAST_CHARACTER(path->CWD) != '/') {
                                strcat(abspath, "/");
                        }

                        strcat(abspath, path->PATH);
                }

                _vfs_realpath(abspath, corr);

                size_t plen = strsize(abspath);

                if (_mm_align(plen) < _mm_align(abslen)) {
                        if (_kzalloc(_MM_KRN, plen, cast(void**, new_path)) == 0) {
                                strcpy(*new_path, abspath);
                                _kfree(_MM_KRN, cast(void*, &abspath));
                                abspath = *new_path;
                        }
                }

                *new_path = abspath;
        }

        return err;
}

//==============================================================================
/**
 * @brief Function reduce relative path elements from absolute path
 *
 * @param[in, out] path        reduce absolute path buffer (./, ../)
 * @param[in]      corr        path correction kind
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
int _vfs_realpath(char *path, enum path_correction corr)
{
        int err = EINVAL;

        if (path && path[0] == '/') {

                char *rdptr = path;
                char *wrptr = path;
                char  last  = ' ';

                while (*rdptr != '\0') {
                        if (  *(rdptr + 0) == '.'
                           && *(rdptr + 1) == '/' ) {

                                rdptr += 2;

                        } else if (  *(rdptr + 0) == '.'
                                  && *(rdptr + 1) == '.'
                                  && *(rdptr + 2) == '/' ) {

                                if (wrptr > path) {
                                        wrptr--;
                                        while ((wrptr > path) && *(--wrptr) != '/');
                                        wrptr += 1;
                                        rdptr += 3;
                                }

                        } else {
                                if (last == '/' && *rdptr == '/') {
                                        rdptr++;
                                } else {
                                        last     = *rdptr;
                                        *wrptr++ = *rdptr++;
                                }
                        }
                }

                switch (corr) {
                case ADD_SLASH: if (last != '/') *wrptr++ = '/'; break;
                case SUB_SLASH: if ((last == '/') && (wrptr - path > 1))  wrptr--; break;
                default: break;
                }

                *wrptr = '\0';

                err = ESUCC;
        }

        return err;
}

/*==============================================================================
  End of file
==============================================================================*/
